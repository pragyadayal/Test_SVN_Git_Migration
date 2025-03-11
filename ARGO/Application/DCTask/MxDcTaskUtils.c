///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Telecom Pvt. Ltd., Baroda, India.
//   Project      : ACS (Access Control System)
//   Created By   : Mukesh Savaliya
//   File         : MxDcTaskUtils.c
//   Description  : This file contains DC task related utility functions.
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxDcTask.h"
#include "MxEventLog.h"
#include "MxSlaveMsg.h"
#include "MxSysTimer.h"
#include "MxDst.h"
#include "MxAcmsTask.h"
#include "MxLogs.h"
#include "MxAlarms.h"
#include "MxGpio.h"
#include "MxMenu.h"
#include "MxEnrollment.h"
#include "MxEthernetPort.h"
#include "MxWifiPort.h"
#include "MxMobileBroadband.h"
#include "MxBackupUpdate.h"
#include "MxBlockDevice.h"
#include "MxSysCommonDef.h"
#include "MxSysUtils.h"
#include "MxConfigFileIo.h"
#include "MxOEM75Drv.h"
#include "MxDvrIntegration.h"
#include "MxIOLink.h"
#include "MxDeviceSpecific.h"
#include "MxFaceRecognitionConfig.h"
#include "MxCafeteria.h"
#include "MxUserConfig.h"
#include "HttpReqFSM.h"
#include "MxAutoInterfaceManager.h"
#include "MxReaderCommonUtils.h"
//******** Extern Variables **********
extern DOOR_ACTIVITY_INFO_t 	DcInfo;
extern SYS_BASIC_PARA_t			SysBasicPara;
extern DOOR_MONITOR_t			DcMonitorControl;
extern struct tm 				CurrTimeDate;
extern SYS_ADVANCE_PARA_t		SysAdvPara;
extern SMART_CARD_KEY_UPDATE_t	SmartCardKeyUpdateDetail;
extern MMC_INFO_t 				MmcInfo;
extern DOOR_MODE_SCH_CONFIG_t	DoorModeSchCfg[MAX_DOOR_MODE_SCHEDULE];
extern BIO_IDENTIFY_SERVER_INFO_t BioIdentifyServer;
extern	SPECIAL_FUNCTION_t		SpecialFunctions[MAX_SPECIAL_FUNC];
extern CONNECT_TO_PANEL_CONFIG_t ConnectToPanelPara;
extern FR_SERVER_CONFIG_t				FRServerConfig;
extern CAFE_MENU_PROC_INFO_t	cafeMenuProcInfo;
extern SERVER_PARA_t		  	ServerParameters;
//******** Defines and Data Types ****

//******** Function Prototypes *******
//static void AccessWaitTimeout(UINT32 dummy);
static void ShowCardForKeyChng(UINT32 data);

//******** Global Variables **********
READER_MODE_e	DoorEntryExitMode = MAX_READER_MODE;
BOOL			DispDoorModeF = FALSE;

//******** Static Variables **********

/***********************************************************************************************************************************/
//	bit    15	  14	  13	  12	  11	  10	   9	    8		7		6		5		4		3	2       1	   0
/***********************************************************************************************************************************/
//																											-----
//			RFU   QR      CARD2  CARD1    BLE     FACE	 API	   Group	Palm	Finger	Card	Pin		 RFU	  Camera   Entry
//																																Exit
/*************************************************************************************************************************************/
const EV_FIELD3 CredEvFieldVal[MAX_CREDENTIAL_TYPE] = {0x001, 0x002, 0x004, 0x020, 0x000, 0x040,0x080, 0x400};

//const EV_FIELD3 CredEvFieldVal[MAX_CREDENTIAL_TYPE] = {0x10, 0x20, 0x40, 0x320, 0x00, 0x640, 0x1280};

//******** Function Definations ******

//*****************************************************************************
//	LoadInterDigitTimer()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         NONE
//	Description:
//		This function starts or reloads the inter digit timer.
//*****************************************************************************
void LoadInterDigitTimer(void)
{
	if(DcInfo.interDigitTmrHandle[INTERNAL_PORT] == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t interDigitTime;

		interDigitTime.count = INTER_DIGIT_TIMER_CNT;
		interDigitTime.funcPtr = &InterDigitTimeout;
		interDigitTime.data = 0;

		if(StartTimer(interDigitTime, &(DcInfo.interDigitTmrHandle[INTERNAL_PORT]), __FUNCTION__) == FAIL)
		{
			EPRINT(DC_LOG,"Unable to start Interdigit Timer");
		}
	}
	else
	{
		ReloadTimer(DcInfo.interDigitTmrHandle[INTERNAL_PORT], INTER_DIGIT_TIMER_CNT);
	}
}

//*****************************************************************************
//	InterDigitTimeout()
//	Param:
//		IN : dummy
//		OUT: None
//	Returns:
//	         NONE
//	Description:
//		This is callback function for inter-digit timer.
//		This function sends command for audio visual indication for timeout.
//*****************************************************************************
void InterDigitTimeout(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&(DcInfo.interDigitTmrHandle[INTERNAL_PORT]));
	DcInfo.authReason = AUTH_TIMEOUT;
	DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
}

//*****************************************************************************
//	StoreRcvdCred()
//	Param:
//		IN : CredType
//		OUT: NONE
//	Returns: NONE
//	Description:
//			If Stores given credential type and increments credential count.
//*****************************************************************************
void StoreRcvdCred(CREDENTIAL_TYPE_e credType, UINT32 credSubType) 
{
	// If received credential type is invalid then set credInvalidF = TRUE
	// last stored crential is sent to cosec monitor event
	if(credType == INVALID_CREDENTIAL)
	{
		DcInfo.credInvalidF = TRUE;
	}
	else
	{
		DcInfo.credInvalidF = FALSE;
		DcInfo.credType[DcInfo.credCnt] = credType;
		DcInfo.credSubType[DcInfo.credCnt++] = credSubType;
	}
}

//*****************************************************************************
//	IsDoorBusy()
//	Param:
//		IN : None
//		OUT: None
//	Returns:
//	         NO/YES
//	Description:
//		This will check idle condition for firmware upload.
//*****************************************************************************
BOOL IsDoorBusy(void)
{
	BOOL	status = NO;

	if(GetDoorApplType() == PANEL_DOOR_APPL)
	{
		return status;
	}

	if( ( DcInfo.fpBeingProcessed == TRUE ) ||
		( DcInfo.interDigitTmrHandle[INTERNAL_PORT] != INVALID_TIMER_HANDLE) ||
		( DcInfo.interDigitTmrHandle[EXTERNAL_PORT] != INVALID_TIMER_HANDLE) ||
		( DcInfo.splFnGetUserInputTmrHandle != INVALID_TIMER_HANDLE) ||
		( ( DcInfo.curState != DC_IDLE_STATE) && (DcInfo.curState != DC_CAFETERIA_STATE) ) ||
		(DcInfo.dcTaskIdleF == FALSE) )

	{
		status = YES;
	}

	return status;
}

//*****************************************************************************
//	IsDoorBusyForBTRequest()
//	Param:
//		IN : None
//		OUT: None
//	Returns:
//	         NO/YES
//	Description:
//		This will check idle condition for firmware upload.
//*****************************************************************************
BOOL IsDoorBusyForBTRequest(void)
{
	BOOL	status = NO;

	if(GetDoorApplType() == PANEL_DOOR_APPL)
	{
		return status;
	}

	if( ( DcInfo.fpBeingProcessed == TRUE ) ||
		( DcInfo.interDigitTmrHandle[INTERNAL_PORT] != INVALID_TIMER_HANDLE) ||
		( DcInfo.interDigitTmrHandle[EXTERNAL_PORT] != INVALID_TIMER_HANDLE) ||
		( DcInfo.splFnGetUserInputTmrHandle != INVALID_TIMER_HANDLE) ||
		( ( DcInfo.curState != DC_IDLE_STATE) &&
		(DcInfo.curState != DC_WAIT_FOR_2ND_PERSON_STATE) &&
		(DcInfo.curState != DC_WAIT_OCCUPANCY_NEXT_USER_STATE)&&
		(DcInfo.curState != DC_CAFETERIA_STATE)&&
		(DcInfo.curState != DC_WAIT_ADDL_CRED_STATE)))
	{
		status = YES;
	}

	return status;
}

//*****************************************************************************
//	StoreUserInfoAsFirstUser()
//	Param:
//		IN : none
//		OUT: none
//	Returns:
//			 none
//	Description:
//			 Stores current user's information as first user.
//*****************************************************************************
void StoreUserInfoAsFirstUser(void)
{
	UINT8					idx;

	DcInfo.firstUserIdx = DcInfo.userIdx;
	for(idx = 0; idx < DcInfo.credCnt; idx++)
	{
		DcInfo.firstUserCredType[idx] = DcInfo.credType[idx];
		DcInfo.firstUserSubCredType[idx] = DcInfo.credSubType[idx];
	}

	DcInfo.firstUserCredCnt = DcInfo.credCnt;
}

//*****************************************************************************
//	GetUserCredEvFieldInfo()
//	Param:
//		IN : userCnt	- first user/second user
//		OUT: none
//	Returns:
//			 constructed event field
//	Description:
//			 It gets user cred info for event field 3.
//*****************************************************************************
EV_FIELD3 GetUserCredEvFieldInfo(USER_CNT_e userCnt)
{
	DOOR_ACTIVITY_INFO_t * 	dcPtr;
	UINT8					idx;
	EV_FIELD3				evField3 = 0;

	dcPtr = &(DcInfo);

	if(userCnt == FIRST_USER)
	{
		for(idx = 0; idx < DcInfo.firstUserCredCnt; idx++)
		{
			evField3  |= CredEvFieldVal[DcInfo.firstUserCredType[idx]];
			evField3  |= dcPtr -> firstUserSubCredType[idx];
		}
	}
	else if(userCnt == FIRST_OCCUPANT)
	{
		for(idx = 0; idx < DcInfo.firstOccupantCredCnt; idx++)
		{
			evField3 |= CredEvFieldVal[DcInfo.firstOccupatcredType[idx]];
			evField3 |= dcPtr -> firstOccupatSubcredType[idx];
		}
	}
	else
	{
		for(idx = 0; idx < DcInfo.credCnt; idx++)
		{
			evField3 |= CredEvFieldVal[DcInfo.credType[idx]];
			evField3 |= dcPtr -> credSubType[idx];
		}
	}
	return (evField3 * 10);      //Multiply by 10 for add 0 at last for entry/exit octant
}

//*****************************************************************************
//	GetEvMode()
//	Param:
//		IN : readerMode
//	Returns:
//			 constructed event field
//	Description:
//			 It gets user cred info for event field 3.
//*****************************************************************************
EV_FIELD3 GetEvMode(UINT8 readerMode)
{
	EV_FIELD3 evField3;

	if(readerMode == ENTRY_READER)
	{
		evField3 = USER_ENTRY;
	}
	else
	{
		evField3 = USER_EXIT;
	}
	return evField3;
}

//*****************************************************************************
//	DeleteFsmTimers()
//	Param:
//		IN : none
//		OUT: none
//	Returns:
//			 None
//	Description:
//			 It will delete the timer used in door FSM state.
//*****************************************************************************
void DeleteFsmTimers(void)
{
	switch(DcInfo.curState)
	{
		default:
			break;

		case DC_IDLE_STATE:
			DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
			break;

		case DC_WAIT_FOR_2ND_PERSON_STATE:
			DeleteTimer(&DcInfo.twoPersonTmrHandle);
			break;

		case DC_WAIT_ADDL_CRED_STATE:
			DeleteTimer(&DcInfo.multiAccessTmrHandle);
			break;

		case DC_PIN_CHANGE_STATE:
			DeleteTimer(&DcInfo.multiInputWaitTmrHandle);
			break;

		case DC_WAIT_OCCUPANCY_NEXT_USER_STATE:
			DeleteTimer(&DcInfo.occupancyNextUsrWaitTimer);
			break;
	}
}

//*****************************************************************************
//	GetEventIdFromAuthReason()
//	Param:
//		IN : authReason - Auth Reason
//		OUT: none
//	Returns:
//			eventId
//	Description:
//			 It will fing event Id from auth reason
//*****************************************************************************
EVENT_ID_e GetEventIdFromAuthReason(AUTH_REASON_e authReason)
{
	EVENT_ID_e eventId;

	switch(authReason)
	{
	// Auth Reason For user Allowed
	case AUTH_VISITOR_ALLOWED_SURRENDER_PASS:
	case AUTH_ALLOWED:			// user allowed (no specific reason)
		eventId = USER_ALLOW;
		break;

	case AUTH_ALLOWED_DURESS:		// user allowed (duress)
		eventId = USER_ALLOW_WITH_DURESS;
		break;
		
	case AUTH_PASSBACK_ENTRY_SOFT_VIOLATION:	// user allowed (soft violation of anti pass back rule)
	case AUTH_PASSBACK_EXIT_SOFT_VIOLATION:	// user allowed (soft violation of anti pass back rule)
		eventId = USER_ALLOW_ANTIPASS_SOFT;
		break;

	case AUTH_ALLOW_SSA_USER:
		eventId = USER_ALLOW_SSA;
		break;

	case AUTH_ALLOW_USER_SOFT_ROUTE_ACCESS:
		eventId = USER_ALLOW_SOFT_ROUTE_ACCESS;
		break;

	case AUTH_ALLOW_2PERSON_PRIMARY:
		eventId = USER_ALLOW_2PERSON_PRIMARY;
		break;

	case AUTH_ALLOW_2PERSON_SECONDARY:
		eventId = USER_ALLOW_2PERSON_SECONDARY;
		break;

	// Auth Reason For user Denied
	case AUTH_USER_DISABLED:	// user denied (disabled)
	case AUTH_CAFE_TXN_REASON_USER_IS_NOT_ACTIVE:
		eventId = USER_DENIED_DISABLE;
		break;

	case AUTH_USER_BLOCKED:		// user denied (duress code)
		eventId = USER_DENIED_BLOCKED;
		break;

	case AUTH_PASSBACK_ENTRY_HARD_VIOLATION:		// user denied (Hard antipass back)
	case AUTH_PASSBACK_EXIT_HARD_VIOLATION:		// user denied (Hard antipass back)
		eventId = USER_DENIED_ANTI_PASSBACK;
		break;

	case AUTH_INVALID_PIN:
	case AUTH_INVALID_CARD:
	case AUTH_INVALID_FINGER:
	case AUTH_FP_MATCHING_TIMEOUT:
	case AUTH_CAFE_TXN_REASON_INVALID_CREDENTIAL:
	case AUTH_INVALID_FACE:
	case AUTH_FACE_MATCHING_TIMEOUT:
	case AUTH_INVALID_FINGER_TEMP_USER: 
		if(DcInfo.zamUserIdx == INVALID_USER)
		{
			eventId = USER_DENIED_INVALID;
		}
		else
		{
			eventId = USER_DENIED_CONTROL_ZONE;
		}
		break;

	case AUTH_NO_ACCESS:	// user denied (user don't have access to the zone)
	case AUTH_CARD_READ_FAILED:
	case AUTH_INVALID_INPUT:
	case AUTH_CAFE_TXN_REASON_CARD_WRITE_FAIL:
	case AUTH_CAFE_TXN_REASON_CARD_READ_FAIL:
	case AUTH_CAFE_TXN_REASON_USER_NOT_ALLOWED:
	case AUTH_ROUTE_ACCESS_CARD_WRITE_FAILED:
	case AUTH_USER_NOT_PRESENT_ON_PANEL:
		eventId = USER_DENIED_CONTROL_ZONE;
		break;

	case AUTH_TWO_PERSON:
		eventId = USER_DENIED_2PERSON;
		break;

	case AUTH_OCCUPANCY_VIOLATION:
		eventId = USER_DENIED_OCCUPANCY_CONTROL;
		break;

	case AUTH_DOOR_LOCK:
		eventId =  USER_DENIED_DOOR_LOCK;
		break;

	case AUTH_FIRST_IN:
		eventId = USER_DENIED_FIRST_IN;
		break;

	case AUTH_CAFE_TXN_REASON_SERVER_TIMEOUT:
	case AUTH_TIMEOUT:
		eventId = USER_DENIED_TIME_OUT;
		break;

	case AUTH_VALIDITY_EXPIRED:
	case AUTH_CAFE_TXN_REASON_USER_NOT_VALID:
		eventId = USER_DENIED_VALIDITY_EXPIRED;
		break;

	case AUTH_INVALID_ROUTE_ACCESS:
	case AUTH_INVALID_ROUTE_INPUT:
	case AUTH_DOOR_NOT_IN_ROUTE:
		eventId = USER_DENIED_INVALID_ROUTE_ACCESS;
		break;

	case AUTH_VAM_ALLOWED:
		eventId = VAM_ALLOWED;
		break;

	case AUTH_VAM_ALLOWED_SOFT_ROUTE:
		eventId = VAM_ALLOWED_SMART_CARD_ROUTE_SOFT;
		break;

	case AUTH_VAM_INVALID_CARD:
		eventId = VAM_DENIED_INVALID_CARD;
		break;

	case AUTH_VAM_DOOR_LOCK:
		eventId = VAM_DENIED_DOOR_LOCKED;
		break;

	case AUTH_VAM_VALIDITY_EXPIRED:
		eventId = VAM_DENIED_PASS_EXPIRED;
		break;

	case AUTH_VAM_NO_ACCESS:
	case AUTH_VAM_ROUTE_ACCESS_CARD_WRITE_FAILED:
		eventId = VAM_DENIED_CONTROL_ZONE;
		break;

	case AUTH_VAM_CRED_INVALID_SEQUENCE:
		eventId = VAM_DENIED_INVALID_SEQUENCE;
		break;

	case AUTH_VAM_TIMEOUT:
		eventId = VAM_DENIED_TIME_OUT;
		break;

	case AUTH_VAM_INVALID_ROUTE_ACCESS:
	case AUTH_VAM_DOOR_NOT_IN_ROUTE:
	case AUTH_VAM_INVALID_CRED_FOR_ROUTE_ACCESS:
		eventId = VAM_DENIED_INVALID_ROUTE_ACCESS;
		break;

	case AUTH_MIN_OCCUPANCY_IS_1:
	case AUTH_MIN_OCCUPANCY_IS_2:
		eventId = USER_DENIED_MINIMUM_OCCUPANCY;
		break;

	case AUTH_ZERO_OCCUPANCY_NOT_ALLOWED_1:
	case AUTH_ZERO_OCCUPANCY_NOT_ALLOWED_2:
		eventId = USER_DENIED_ZERO_OCCUPANCY_NOT_ALLOWED;
		break;

	case AUTH_CAFE_TXN_REASON_INSUFFICIENT_BALANCE:
		eventId = INSUFFICIENT_BALANCE;
		break;

	case AUTH_CAFE_TXN_REASON_INVALID_TRANSACTION:
		eventId = CAFE_USER_DENIED;
		break;

	case AUTH_FR_DISABLED:
		eventId = USER_DENIED_FR_DISABLED;
		break;

	case AUTH_TEMP_HARD_THRESHOLD_EXCEED:
		eventId = USER_DENIED_TEMP_HARD_THRESHOLD_EXCEED;
		break;

	case AUTH_FACE_MASK_HARD_VIOLATION:
		eventId = USER_DENIED_FACE_MASK_HARD_VIOLATION;
		break;

	default:
		EPRINT(DC_LOG, "Request to log event for auth reason[%d] is failed", authReason);
		eventId = USER_DENIED_INVALID;
		break;
	}
	return eventId;
}

//*****************************************************************************
//	GetUserRespFromAuthReason()
//	Param:
//		IN : authReason - Auth Reason
//		OUT: none
//	Returns:
//			eventId
//	Description:
//			 It will find and return user response for given authentication reason.
//*****************************************************************************
USER_RESP_e GetUserRespFromAuthReason(AUTH_REASON_e authReason)
{
	USER_RESP_e userResp = MAX_USER_RESP;

	switch(authReason)
	{
		case AUTH_USER_DISABLED:
		case AUTH_CAFE_TXN_REASON_USER_IS_NOT_ACTIVE:
		case AUTH_CAFE_TXN_REASON_INSUFFICIENT_BALANCE:
			userResp = USER_RESP_ACCESS_AUTH_USER_DISABLED;
			break;

		case AUTH_USER_BLOCKED:
			userResp = USER_RESP_ACCESS_AUTH_USER_BLOCKED;
			break;

		case AUTH_PASSBACK_ENTRY_HARD_VIOLATION:
		case AUTH_PASSBACK_EXIT_HARD_VIOLATION:
			userResp = USER_RESP_ACCESS_AUTH_PASSBACK_HARD_VIOLATION;
			break;

		case AUTH_INVALID_PIN:
			userResp = USER_RESP_ACCESS_AUTH_INVALID_PIN;
			break;

		case AUTH_INVALID_CARD:
			userResp = USER_RESP_ACCESS_AUTH_INVALID_CARD;
			break;

		case AUTH_INVALID_FINGER:
		case AUTH_INVALID_FINGER_TEMP_USER:
			userResp = USER_RESP_ACCESS_AUTH_INVALID_FINGER;
			break;

		case AUTH_INVALID_FACE:
			userResp = USER_RESP_ACCESS_AUTH_INVALID_FACE;
			break;

		case AUTH_FP_MATCHING_TIMEOUT:
			userResp = USER_RESP_ACCESS_AUTH_FP_MATCHING_TIMEOUT;
			break;

		case AUTH_NO_ACCESS:
		case AUTH_CAFE_TXN_REASON_USER_NOT_ALLOWED:
			userResp = USER_RESP_ACCESS_AUTH_NO_ACCESS;
			break;

		case AUTH_TWO_PERSON:
			userResp = USER_RESP_ACCESS_AUTH_TWO_PERSON;
			break;

		case AUTH_OCCUPANCY_VIOLATION:
			userResp = USER_RESP_ACCESS_AUTH_OCCUPANCY_VIOLATION;
			break;

		case AUTH_DOOR_LOCK:
			userResp = USER_RESP_ACCESS_AUTH_DOOR_LOCK;
			break;

		case AUTH_FIRST_IN:
			userResp = USER_RESP_ACCESS_AUTH_FIRST_IN;
			break;

		case AUTH_CAFE_TXN_REASON_SERVER_TIMEOUT:
		case AUTH_TIMEOUT:
			userResp = USER_RESP_ACCESS_AUTH_TIMEOUT;
			break;

		case AUTH_CARD_READ_FAILED:
		case AUTH_CAFE_TXN_REASON_CARD_READ_FAIL:
			userResp = USER_RESP_ACCESS_AUTH_CARD_READ_FAILED;
			break;

		case AUTH_VALIDITY_EXPIRED:
		case AUTH_CAFE_TXN_REASON_USER_NOT_VALID:
			userResp = USER_RESP_ACCESS_AUTH_VALIDITY_EXPIRED;
			break;

		case AUTH_INVALID_INPUT:
		case AUTH_API_INVALID_INPUT:
		case AUTH_CAFE_TXN_REASON_INVALID_CREDENTIAL:
			userResp = USER_RESP_ACCESS_AUTH_INVALID_INPUT;
			break;

		case AUTH_CAFE_TXN_REASON_CARD_WRITE_FAIL:
			userResp = USER_RESP_CAFE_CARD_WRITE_FAILED;
			break;

		case AUTH_INVALID_ROUTE_INPUT:
			userResp = USER_RESP_AUTH_INVALID_ROUTE_INPUT;
			break;

		case AUTH_INVALID_ROUTE_ACCESS:
			userResp = USER_RESP_AUTH_INVALID_ROUTE_ACCESS;
			break;

		case AUTH_DOOR_NOT_IN_ROUTE:
			userResp = USER_RESP_AUTH_DOOR_NOT_IN_ROUTE;
			break;

		case AUTH_ROUTE_ACCESS_CARD_WRITE_FAILED:
			userResp = USER_RESP_ROUTE_ACCESS_CARD_WRITE_FAILED;
			break;

		case AUTH_VAM_ALLOWED:
		case AUTH_VAM_ALLOWED_SOFT_ROUTE:
			userResp = USER_RESP_VAM_ACCESS_ALLOWED;
			break;

		case AUTH_VAM_DOOR_LOCK:
			userResp = USER_RESP_VAM_DOOR_LOCK;
			break;

		case AUTH_VAM_VALIDITY_EXPIRED:
			userResp = USER_RESP_VAM_VALIDITY_EXPIRED;
			break;

		case AUTH_VAM_CRED_INVALID_SEQUENCE:
			userResp = USER_RESP_VAM_CRED_INVALID_SEQ;
			break;

		case AUTH_VAM_INVALID_CARD:
			userResp = USER_RESP_VAM_INVALID_CARD;
			break;

		case AUTH_VAM_INVALID_ROUTE_ACCESS:
		case AUTH_VAM_DOOR_NOT_IN_ROUTE:
			userResp = USER_RESP_VAM_INVALID_SAMRT_ROUTE_ACCESS;
			break;

		case AUTH_VAM_INVALID_CRED_FOR_ROUTE_ACCESS:
			userResp = USER_RESP_VAM_INVALID_SMART_ROUTE_INPUT;
			break;

		case AUTH_VAM_ROUTE_ACCESS_CARD_WRITE_FAILED:
			userResp = USER_RESP_VAM_SMART_ROUTE_CARD_WRITE_FAILED;
			break;

		case AUTH_VAM_NO_ACCESS:
			userResp = USER_RESP_VAM_NO_ACCESS;
			break;

		case AUTH_VAM_TIMEOUT:
			userResp = USER_RESP_TIMEOUT;
			break;

		case AUTH_MIN_OCCUPANCY_IS_1:
		case AUTH_ZERO_OCCUPANCY_NOT_ALLOWED_1:
			userResp = USER_RESP_MIN_OCCUPANCY_IS_1;
			break;

		case AUTH_MIN_OCCUPANCY_IS_2:
		case AUTH_ZERO_OCCUPANCY_NOT_ALLOWED_2:
			userResp = USER_RESP_MIN_OCCUPANCY_IS_2;
			break;

		case AUTH_CAFE_TXN_REASON_INVALID_TRANSACTION:
			userResp = USER_RESP_CAFE_DENIED;
			break;

		case AUTH_CAFE_POLICY_DENIED_WEEK_OFF:
		case AUTH_CAFE_POLICY_DENIED_NON_WORKING_DAY:
		case AUTH_CAFE_POLICY_DENIED_INVALID_SHIFT:
		case AUTH_CAFE_POLICY_DENIED_DAILY_TRANSACTION:
		case AUTH_CAFE_POLICY_DENIED_DAILY_MENU_LIMIT:
		case AUTH_CAFE_POLICY_DENIED_DAILY_ITEM_LIMIT:
		case AUTH_CAFE_POLICY_DENIED_MONTHLY_TRANSACTION:
		case AUTH_CAFE_POLICY_DENIED_MONTHLY_MENU_LIMIT:
		case AUTH_CAFE_POLICY_DENIED_MONTHLY_ITEM_LIMIT:
		case AUTH_CAFE_POLICY_DENIED_PREORDER_LIMIT:
		case AUTH_CAFE_POLICY_DENIED_WEEKLY_LIMIT:
		case AUTH_CAFE_POLICY_DENIED_OFFLINE_TRANSACTION_NOT_ALLOWED:
		case AUTH_CAFE_POLICY_DENIED_OFFLINE_ITEM_PER_MENU_LIMIT:
			userResp = USER_RESP_CAFE_POLICY_USER_DENIED;
			break;

		case AUTH_CAFE_USER_ID_MISMATCH:
			userResp = USER_RESP_CAFE_CAFE_USERID_MISMATCH;
			break;

		case AUTH_USER_NOT_PRESENT_ON_PANEL:
			userResp = USER_RESP_NOT_PRESENT_ON_PANEL;
			break;

		case AUTH_FACE_MATCHING_TIMEOUT:
			userResp = USER_RESP_ACCESS_AUTH_TIMEOUT;
			break;

		case AUTH_FR_DISABLED:
			userResp = USER_RESP_DENIED_FR_DISABLED;
			break;

		case AUTH_TEMP_HARD_THRESHOLD_EXCEED:
			userResp = USER_RESP_TEMP_HARD_THRESHOLD_EXCEEDED;
			break;

#if defined(FR_ADDITIONAL_FEATURE_SUPPORTED)
		case AUTH_FR_DISABLED_ON_DEVICE:
			userResp = USER_RESP_DENIED_FR_DISABLED_ON_DEVICE;
			break;
#endif

#if defined(FACE_MASK_COMPULSION_ENABLE)
		case AUTH_FACE_MASK_HARD_VIOLATION:
			userResp = USER_RESP_DENIED_FACE_MASK_NOT_DETECTED;
			break;
#endif

		default:
			break;
	}

	return userResp;
}

//*****************************************************************************
//	SetUserAsVisitor()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function mark user as visitor and stores visitor name
//				to user config data.
//*****************************************************************************
void SetUserAsVisitor(void)
{
	// Set user type as visitor
    DcInfo.userType = VISITOR_USER;
}

//*****************************************************************************
//	SetFpMatchTimer()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function starts/reloads 'finger match abort timer'. Upon
//				timeout, it will abort finger matching in case of reader has not
//				intimated.
//*****************************************************************************
void SetFpMatchTimer(UINT8	timerValue)
{
	TIMER_INFO_t timeInfo;

	if(DcInfo.fpMatchTmrHandle == INVALID_TIMER_HANDLE)
	{
		timeInfo.funcPtr = &FpMatchingTimeout;
		timeInfo.count = (timerValue * 10);
		timeInfo.data = 0;

		StartTimer(timeInfo, &DcInfo.fpMatchTmrHandle, __FUNCTION__);
	}
	else
	{
		ReloadTimer(DcInfo.fpMatchTmrHandle, (timerValue * 10) );
	}
}

//*****************************************************************************
//	FpMatchingTimeout()
//	Param:
//		IN :	data -- unused
//		OUT:	None
//	Returns:	None
//	Description:
//				This function is callback function for 'finger matching abort timer'
//				It simply resets timer handle as invalid.
//*****************************************************************************
void FpMatchingTimeout(UNUSED_PARA UINT32 data)
{
	DeleteTimer(&DcInfo.fpMatchTmrHandle);
	DcInfo.authReason = AUTH_FP_MATCHING_TIMEOUT;
	DcInfo.fpBeingProcessed = FALSE;
	DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
}

//*****************************************************************************
// GetEventModeFromSplCode()
// Param:       In    - reader mode
//					  - special code
//              Out	  - none
// Returns:	    reader Mode
// Description: It gives event mode as per special code being accessed
//				irrespective of configured reader mode.
//*****************************************************************************
USER_EVENT_INFO_e GetEventModeFromSplCode(READER_MODE_e readerMode, EV_USER_SPECIAL_CODE_FIELD_e splCode)
{
	USER_EVENT_INFO_e evMode;

	switch(splCode)
	{
		case OFFICIAL_WORK_IN:
		case SHORT_LEAVE_IN:
		case CLOCK_IN:
		case POST_BREAK_IN:
		case OVER_TIME_IN:
		case LATE_IN_ALLOWED:
			evMode = USER_ENTRY;
			break;

		case OFFICIAL_WORK_OUT:
		case SHORT_LEAVE_OUT:
		case CLOCK_OUT:
		case PRE_BREAK_OUT:
		case OVER_TIME_OUT:
		case EARLY_OUT_ALLOWED:
			evMode = USER_EXIT;
			break;

		default:
		{
			if(DcInfo.doorEvtMode == MAX_USER_EVENT_INFO)
			{
				evMode = GetEvMode(readerMode);
			}
			else
			{
				evMode = DcInfo.doorEvtMode;
			}
			break;
		}
	}
	return evMode;
}

//*****************************************************************************
// CreateDirInMmc()
// Param:       In    - None
// Returns:	    None
// Description: It checks and If Event Pics Folder is not available
//				then creates it
//*****************************************************************************
void CreateDirInMmc(void)
{
	CHAR	buff[100];
	struct stat		temp;

	// Create Event Pics Folder
	sprintf(buff,"%s/%s", MmcInfo.devPath, EVENT_PICS_FOLDER);
	if(stat(buff, &temp) != 0)
	{
		EPRINT(DC_LOG, "Creating Directory:%s/%s", MmcInfo.devPath, EVENT_PICS_FOLDER);
		sprintf(buff, "mkdir -p %s/%s", MmcInfo.devPath, EVENT_PICS_FOLDER);
		system(buff);
	}

	// Create User Pics Folder
	sprintf(buff,"%s/%s", MmcInfo.devPath, USER_PICS_FOLDER);
	if(stat(buff, &temp) != 0)
	{
		EPRINT(DC_LOG, "Creating Directory:%s/%s", MmcInfo.devPath, USER_PICS_FOLDER);
		sprintf(buff, "mkdir -p %s/%s", MmcInfo.devPath, USER_PICS_FOLDER);
		system(buff);
	}
}

//*****************************************************************************
//	GetLateInEarlOutStatus()
//	Param:
//		IN:	NONE
//		OUT:NONE
//	Returns:
//		status-status of MultiuserSpecialFunc
//	Description:
//			It returns communication status of ACMS module (Login or Logout).
//
//*****************************************************************************
MULTI_USER_SPL_STATUS_e GetLateInEarlOutStatus(void)
{
	if(DcInfo.multiUserSplCode == LATE_IN_ALLOWED)
	{
		return LATE_IN_ACTIVE;
	}
	else if (DcInfo.multiUserSplCode == EARLY_OUT_ALLOWED)
	{
		return EARLY_OUT_ACTIVE;
	}
	else
	{
		return FUNC_INACTIVE;
	}
}

//*****************************************************************************
//	GetEventField2FromAuthReason()
//	Param:
//		IN:	authReason
//		OUT:NONE
//	Returns:
//		Event Field 2nd value
//	Description:
//			It returns value of event second field
//*****************************************************************************
EV_FIELD2 GetEventField2FromAuthReason(AUTH_REASON_e authReason)
{
	EV_FIELD2 evField2 = UNUSED_FIELD;

	switch(authReason)
	{
		case AUTH_INVALID_ROUTE_INPUT:
		case AUTH_VAM_INVALID_CRED_FOR_ROUTE_ACCESS:
		{
			evField2 = CRED_INVALID_SMART_CARD_BASED_ROUTE;
			break;
		}

		case AUTH_INVALID_ROUTE_ACCESS:
		case AUTH_VAM_INVALID_ROUTE_ACCESS:
		{
			evField2 = DOOR_NOT_IN_SEQ_SMART_ROUTE_ACCESS;
			break;
		}

		case AUTH_DOOR_NOT_IN_ROUTE:
		case AUTH_VAM_DOOR_NOT_IN_ROUTE:
		{
			evField2 = DOOR_NOT_IN_SMART_CARD_BASED_ROUTE;
			break;
		}

		default:
			break;
	}
	return evField2;
}

//*****************************************************************************
//	PinEntryPossible()
//	Param:
//		IN:	NONE
//		OUT:NONE
//	Returns:
//		YES/NO
//	Description:
//			It checks if PIN entry is possible considering input is being
//			taken on entry/exit.
//*****************************************************************************
BOOL PinEntryPossible(RESPONSE_PORT_e pinRespPort)
{
	if(DcInfo.fpBeingProcessed == TRUE)
	{
		return NO;
	}

	if( (DcInfo.splFnGetUserInputTmrHandle != INVALID_TIMER_HANDLE) ||
		(DcInfo.curState != DC_IDLE_STATE) )
	{
		if(DcInfo.respPort != pinRespPort)
		{
			return NO;
		}
	}

	if( ( (pinRespPort == INTERNAL_PORT) && (DcInfo.interDigitTmrHandle[EXTERNAL_PORT] != INVALID_TIMER_HANDLE) ) ||
		( (pinRespPort == EXTERNAL_PORT) && (DcInfo.interDigitTmrHandle[INTERNAL_PORT] != INVALID_TIMER_HANDLE) ) )
	{
		return NO;
	}

	return YES;
}

//*****************************************************************************
//	DeleteAllFpFromDoor()
//	Param:
//		IN : none
//		OUT: none
//	Returns:
//			 none
//	Description:
//			 It sends command to DC to delete all finger print templates from
//			 its reader.
//
//*****************************************************************************
UINT8 DeleteAllFpFromDoor(void)
{
	READER_ID_e					readerId;
	DELETE_USER_FP_CMD_DATA_t 	deleteUserFp;

	if(DcInfo.curState == DC_ENROLLMENT_STATE)
	{
		return WAIT;
	}

	// If none of the FPR is configured in a system, we return SUCCESS
	// If command can not sent to any active FPR, we return FAIL
	for(readerId = 0; readerId < MAX_READERS; readerId++)
	{
		if(SysBasicPara.reader[readerId].type == SUP_FP_3030_READER)
		{
			//Send message to read - delete all templates
			deleteUserFp.readerId = readerId;
			deleteUserFp.deleteMethod = DELETE_ALL_TEMPLATES;
			if(ProcessReaderCommand(MSG_DELETE_FP_TEMPLATES_CMD, &deleteUserFp, sizeof(DELETE_USER_FP_CMD_DATA_t)) == FAIL)
			{
				EPRINT(DC_LOG,"Failed to send 'DELETE ALL USER FINGERS' command to reader[%d]", (readerId+1));
				return FAIL;
			}
			else
			{
				DPRINT(DC_LOG, "Sent 'DELETE ALL USER FINGERS' command to reader[%d]", (readerId+1));
			}
		}
	}

	DeleteAllUsersFpFiles();
	return SUCCESS;
}

//*****************************************************************************
//	LoadShowCardForKeyChngTimer()
//	Param:
//		IN :	timeValue -- delay in second
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function starts/reloads lcdIdleTmrHandle to show 'show card'
//		audio visual response.
//*****************************************************************************
BOOL LoadShowCardForKeyChngTimer(UINT16	timerValue)
{
	TIMER_INFO_t	   waitDelayTime;

	if(DcInfo.SmrtCardKeyChngTmrHandle == INVALID_TIMER_HANDLE)
	{
		waitDelayTime.funcPtr = &ShowCardForKeyChng;
		waitDelayTime.count = (timerValue*10*TIMER_100MS);
		waitDelayTime.data = 0;

		return (StartTimer(waitDelayTime, &DcInfo.SmrtCardKeyChngTmrHandle, __FUNCTION__) );
	}
	else
	{
		return (ReloadTimer(DcInfo.SmrtCardKeyChngTmrHandle, timerValue) );
	}
}

//*****************************************************************************
//	ShowCardForKeyChng()
//	Param:
//		IN :	data -- unused
//		OUT:	None
//	Returns:	None
//	Description:
//*****************************************************************************
static void ShowCardForKeyChng(UNUSED_PARA UINT32 data)
{
	BOOL showEscBtnF = TRUE;

	DeleteTimer(&DcInfo.SmrtCardKeyChngTmrHandle);
	SetAudioVisual(USER_RESP_KEY_CHANGE_SHOW_CARD, INTERNAL_PORT, (VOIDPTR)&showEscBtnF);
}

//*****************************************************************************
//	SendMsgChangeHIDRdrKey
//	Param:
//		IN :	readerId	- reader index
//				rdrKeyLoc 	- card type
//		OUT:	none
//	Returns:	none
//	Description:
//			Sends message to DC to change key of hid reader at specified location.
//*****************************************************************************
void SendMsgChangeHIDRdrKey(UINT8 readerId, UINT8 rdrKeyLoc, UINT8PTR keyPtr)
{
	CHANGE_HID_RDR_KEY_CMD_DATA_t cmdData;
	cmdData.readerId = readerId;
	//here we have keep cardId because of its necessary to process command from queue.
	cmdData.cardType = HID_2K2;
	cmdData.rdrKeyLoc = rdrKeyLoc;
	memcpy( (VOIDPTR)cmdData.key, (VOIDPTR)keyPtr, HID_ICLASS_KEY_SIZE);
	ProcessReaderCommand(MSG_CHANGE_HID_RDR_KEY_CMD, &cmdData, sizeof(CHANGE_HID_RDR_KEY_CMD_DATA_t));
}

//*****************************************************************************
//	SmartCardKeyChngActn
//	Param:
//		IN :	NONE
//				NONE
//		OUT:	none
//	Returns:	none
//	Description:
//			This function will take action acoording to tha smart kard key detail.
//*****************************************************************************
void SmartCardKeyChngActn(void)
{
	if(SysBasicPara.reader[READER1].type == HID_ICLASS_U_READER)
	{
		SendMsgChangeHIDRdrKey(READER1, READER_KEY_15, SmartCardKeyUpdateDetail.oldKey);
	}
	SwitchDcToIdleState();
	if(SmartCardKeyUpdateDetail.smartCardKeyChangeF == FALSE)
	{
		DeleteTimer(&DcInfo.SmrtCardKeyChngTmrHandle);
		SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
	}
}

//*****************************************************************************
//	VisitorDenyEvtAction()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         converted String
//	Description:
//		This function does events and actions related to visitor deny
//*****************************************************************************
void VisitorDenyEvtAction(BOOL showAvResp)
{
	if( (showAvResp == TRUE) && (IsDoorBusy() == NO) )
	{
		SetAudioVisual(USER_RESP_VISITOR_ACCESS_DENY, INTERNAL_PORT, NULL);
	}

	WriteEventLog(VISITOR_DENIED_FROM_WEB_API, (UINT32)DcInfo.visitorExtraIfo1, (UINT16)DcInfo.visitorExtraIfo2, ANY_READER_MODE, UNUSED_FIELD, UNUSED_FIELD);//TODO: Entry/Exit

	if(SysAdvPara.dvrDenyEventUserType != USERS_ONLY)
	{
		WriteDvrEvt(DVR_CONFIG_EVENT_USER_DENIED, DVR_SEND_EVENT_VISITOR_DENIED, UNKNOWN_USER_IDX, ANY_READER_MODE);//TODO: Entry/Exit
		UpdateIoLinkInPortStatus(USER_DENIED_TRIGGER, ACTIVE);
	}
}

//*****************************************************************************
//	IntercomPanicEvtAction()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         converted String
//	Description:
//		This function does events and actions related to visitor deny
//*****************************************************************************
void IntercomPanicEvtAction(void)
{
	DeclareAlarm(INTERCOM_PANIC_ALARM, CRITICAL_ALARM);

	//active intercom panic input port
	UpdateIoLinkInPortStatus(INTERCOM_PANIC_PORT, ACTIVE);

	WriteDvrEvt(DVR_CONFIG_EVENT_INTERCOM_PANIC, DVR_SEND_EVENT_INTERCOM_PANIC, UNKNOWN_USER_IDX, ANY_READER_MODE);

}

//*****************************************************************************
//	VisitorAllowEvtAction()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         converted String
//	Description:
//		This function does events and actions related to visitor allow
//*****************************************************************************
void VisitorAllowEvtAction(BOOL showAvResp)
{
	DcInfo.userIdDoorOpenCloseEvent = INVALID_USER_ID;
	memset(DcInfo.alphanumIddoorOpenCloseEvent, 0, sizeof(DcInfo.alphanumIddoorOpenCloseEvent));
	OpenDoor();
	if( (showAvResp == TRUE) && (IsDoorBusy() == NO) )
	{
		SetAudioVisual(USER_RESP_OPEN_DOOR_WEB_API_COMMAND, INTERNAL_PORT, NULL);
	}

	WriteEventLog(VISITOR_ALLOWED_FORM_WEB_API, (UINT32)DcInfo.visitorExtraIfo1, (UINT16)DcInfo.visitorExtraIfo2, ANY_READER_MODE, UNUSED_FIELD, UNUSED_FIELD);//TODO: entry/exit

	if(SysAdvPara.dvrAllowEventuserType != USERS_ONLY)
	{
		WriteDvrEvt(DVR_CONFIG_EVENT_USER_ALLOWED, DVR_SEND_EVENT_VISITOR_ALLOWED, UNKNOWN_USER_IDX, ANY_READER_MODE);//TODO: Entry/Exit
		UpdateIoLinkInPortStatus(USER_ALLOWED_TRIGGER, ACTIVE);
	}
}

//*****************************************************************************
//	GetReaderEntryExitMode()
//	Param:
//		IN : readerId
//		OUT: NONE
//	Returns:
//	         reader mode of the given readerId
//	Description:
//		This function returns the reader's entry exit mode considering door
//		mode schedule
//*****************************************************************************
READER_MODE_e GetReaderEntryExitMode(UINT8 readerId)
{
	//If reader is external reader or Door Mode schedule is disable
	if( (readerId >= READER3) || (SysBasicPara.doorModeSchdlEnblF == DISABLED) || (getDeviceInactiveStatus() == TRUE) )
	{
		return SysBasicPara.reader[readerId].mode;
	}

	return DoorEntryExitMode;
}

//*****************************************************************************
//	GetDcActivityInfoReaderMode()
//	Param:
//		IN : readerId
//		OUT: NONE
//	Returns:
//	         reader mode of the given readerId
//	Description:
//		This function returns the dc info reader's entry exit mode considering door
//		mode schedule
//*****************************************************************************
void UpdateDcInfoReaderMode(UINT8 readerId)
{
	//If it is first user and first credential than only update dc info reader mode
	if( (DcInfo.firstUserIdx == INVALID_USER) && (DcInfo.firstOccupantIdx == INVALID_USER) && (DcInfo.credCnt == FIRST_CREDENTIAL) )
	{
		DcInfo.readerMode = GetReaderEntryExitMode(readerId);
		DcInfo.showDoorModeOnAllowF = DispDoorModeF;
	}
}

//*****************************************************************************
//	UpdateDoorEntryExitMode()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         NONE
//	Description:
//		This function will update door's entry/exit mode based on schedule
//*****************************************************************************
void UpdateDoorEntryExitMode(void)
{
	UINT8	idx;

	if(SysBasicPara.doorModeSchdlEnblF == ENABLED)
	{
		for(idx = 0; idx < MAX_DOOR_MODE_SCHEDULE; idx++)
		{

			if(getDeviceInactiveStatus() == TRUE) //Bypass the ReaderModeSchedule if device is inactive
			    continue;
			
			//Check if current day is enabled in schedule
			if(DoorModeSchCfg[idx].days[CurrTimeDate.tm_wday] == DISABLED)
				continue;

			//Ckeck if current time is between schedule start and end time
			if(IsCurrTimeInGivenWindow(DoorModeSchCfg[idx].startTime, DoorModeSchCfg[idx].endTime) == YES)
			{
				DoorEntryExitMode = DoorModeSchCfg[idx].doorRdrMode;
				DispDoorModeF = TRUE;
				break;
			}
		}

		//If current day and time is not matched with any schedule use the assigned mode in system basic para
		if(idx == MAX_DOOR_MODE_SCHEDULE)
		{
			DoorEntryExitMode = SysBasicPara.reader[READER1].mode;
			DispDoorModeF = FALSE;
		}
	}
}

//*****************************************************************************
//	UpdateDoorIdleDisplay()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         NONE
//	Description:
//		This function updates door's idle display to show or hide IN/OUT mode
//		if door is in idle state and LCD idle timer is not running
//*****************************************************************************
void UpdateDoorIdleDisplay(void)
{
	SendMessageToQt(" ", GUI_MSG_SEND_CONFIG_INFO);
//	if( (DcInfo.curState == DC_IDLE_STATE) && (DcInfo.lcdIdleTmrHandle == INVALID_TIMER_HANDLE) )
//	{
//		DisplayIdleScreen(0);
//	}
}

//*****************************************************************************
//	DoorModeScheduleUpdate()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         NONE
//	Description:
//		This function is called every minute and update door mode variable and
//		idle screen based on door mode schedule
//*****************************************************************************
void DoorModeScheduleUpdate(void)
{
	READER_MODE_e	tmpDoorEntryExitMode = DoorEntryExitMode;
	BOOL			tmpDispDoorModeF = DispDoorModeF;

	if(SysBasicPara.doorModeSchdlEnblF == ENABLED)
	{
		UpdateDoorEntryExitMode();
		if( (tmpDoorEntryExitMode != DoorEntryExitMode) || (tmpDispDoorModeF != DispDoorModeF) )
		{
			UpdateDoorIdleDisplay();
		}
	}
	else if(DispDoorModeF == TRUE)
	{
		DoorEntryExitMode = MAX_READER_MODE;
		DispDoorModeF = FALSE;
		UpdateDoorIdleDisplay();
	}
}

//*****************************************************************************
//	SetQrCodeAuthReasonInDcInfo()
//	Param:	IN:
//			OUT:
//	Description:
//				This function check authentication reason & current door state
//				and return suitable reason & door mode for QR code
//*****************************************************************************
void SetQrCodeAuthReasonInDcInfo(void)
{
	switch(DcInfo.authReason)
	{
		case AUTH_ALLOWED:
		case AUTH_ALLOW_2PERSON_PRIMARY:
		case AUTH_ALLOW_2PERSON_SECONDARY:
		case AUTH_PASSBACK_ENTRY_SOFT_VIOLATION:// user is allowed (soft violation of anti pass back rule)
		case AUTH_PASSBACK_EXIT_SOFT_VIOLATION:
		case AUTH_ALLOW_SSA_USER:
		case AUTH_ALLOW_USER_SOFT_ROUTE_ACCESS:
			DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			break;
			
		case AUTH_VISITOR_ALLOWED_SURRENDER_PASS:
			DcInfo.qrAuthReason = QR_AUTH_VISITOR_PASS_SURRENDER;
			break;

		case AUTH_TWO_PERSON:
			DcInfo.qrAuthReason = QR_AUTH_TWO_PERSON;
			break;

		case AUTH_PASSBACK_EXIT_HARD_VIOLATION:
			DcInfo.qrAuthReason = QR_AUTH_ENTRY_NOT_RECORDED;
			break;

		case AUTH_PASSBACK_ENTRY_HARD_VIOLATION:
			DcInfo.qrAuthReason = 	QR_AUTH_EXIT_NOT_RECORDED;
			break;

		case AUTH_USER_BLOCKED:
			DcInfo.qrAuthReason = QR_AUTH_USER_BLOCKED;
			break;

		case AUTH_DOOR_LOCK:
			DcInfo.qrAuthReason = QR_AUTH_DOOR_LOCKED;
			break;

		case AUTH_FIRST_IN:
			DcInfo.qrAuthReason = QR_AUTH_NO_1ST_USER_ENTRY;
			break;

		case AUTH_ZERO_OCCUPANCY_NOT_ALLOWED_1:
		case AUTH_ZERO_OCCUPANCY_NOT_ALLOWED_2:
		case AUTH_MIN_OCCUPANCY_IS_1:
		case AUTH_MIN_OCCUPANCY_IS_2:
			DcInfo.qrAuthReason = QR_AUTH_MIN_OCCUPANCY_NOT_SATISFIED;
			break;

		case AUTH_OCCUPANCY_VIOLATION:
			DcInfo.qrAuthReason = QR_AUTH_TOO_MANY_OCCUPANCY;
			break;

		case AUTH_TIMEOUT:
			DcInfo.qrAuthReason = QR_AUTH_TIMEOUT;
			break;

		case AUTH_VALIDITY_EXPIRED:
			DcInfo.qrAuthReason = QR_AUTH_VALIDITY_EXPIRED;
			break;

		case AUTH_API_INVALID_INPUT:
			DcInfo.qrAuthReason = QR_AUTH_USER_NOT_IDENTIFIED;
			break;

		case AUTH_NO_ACCESS:
			DcInfo.qrAuthReason = QR_AUTH_ENTRY_RESTRICTED;
			break;

		case AUTH_USER_DISABLED:
			DcInfo.qrAuthReason = QR_AUTH_USER_NOT_ACTIVE;
			break;

		case AUTH_INVALID_INPUT:
			DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			break;

		case AUTH_EVENT_LOG_FAIL:
			DcInfo.qrAuthReason = QR_AUTH_SYS_ERROR;
			break;

		case AUTH_EVENT_REGISTERED:
			DcInfo.qrAuthReason = QR_AUTH_EVENT_REGISTERED;
			break;


		case AUTH_USER_NOT_PRESENT_ON_PANEL:
			DcInfo.qrAuthReason = QR_AUTH_USER_NOT_IDENTIFIED;
			break;
			

		case AUTH_TEMP_HARD_THRESHOLD_EXCEED:
			if(DcInfo.apiUser == SET)
			{
				DcInfo.qrAuthReason = QR_AUTH_THRESHOLD_TEMPERATURE_EXCEDED_HARD;
			}
			break;

#if defined(FACE_MASK_COMPULSION_ENABLE)
		case AUTH_FACE_MASK_HARD_VIOLATION:
			if(DcInfo.apiUser == SET)
			{
				DcInfo.qrAuthReason = QR_AUTH_FACE_MASK_NOT_DETECTED;
			}
			break;
#endif

			case AUTH_FR_DISABLED:
			DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			break;

		default:
			DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			break;
	}
}

//*****************************************************************************
//	LoadInterDigitTimerForExternal()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         NONE
//	Description:
//		This function starts or reloads the inter digit timer.
//*****************************************************************************
void LoadInterDigitTimerForExternal(void)
{
	if(DcInfo.interDigitTmrHandle[EXTERNAL_PORT] == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t interDigitTime;

		interDigitTime.count = INTER_DIGIT_TIMER_CNT;
		interDigitTime.funcPtr = &InterDigitTimeoutOnExternal;
		interDigitTime.data = 0;

		if(StartTimer(interDigitTime, &(DcInfo.interDigitTmrHandle[EXTERNAL_PORT]), __FUNCTION__) == FAIL)
		{
			EPRINT(DC_LOG,"Unable to start Interdigit Timer FOR EXTERNAL PIN READER");
		}
	}
	else
	{
		ReloadTimer(DcInfo.interDigitTmrHandle[EXTERNAL_PORT], INTER_DIGIT_TIMER_CNT);
	}
}

//*****************************************************************************
//	InterDigitTimeoutOnExternal()
//	Param:
//		IN : dummy
//		OUT: None
//	Returns:
//	         NONE
//	Description:
//		This is callback function for inter-digit timer.
//		This function sends command for audio visual indication for timeout.
//*****************************************************************************
void InterDigitTimeoutOnExternal(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&(DcInfo.interDigitTmrHandle[EXTERNAL_PORT]));
	DcInfo.keyCnt = 0;

	if(DcInfo.pinChangeModeState == PIN_CHANGE_IDLE_STATE)
	{
		DcInfo.authReason = AUTH_TIMEOUT;
		DenyUser(DcInfo.userIdx, GetReaderEntryExitMode(READER3), EXTERNAL_PORT, DC_STATE_SWITCH_TO_IDLE);
	}
	else
	{
		DcInfo.pinChangeModeState = PIN_CHANGE_IDLE_STATE;
		DeleteTimer(&(DcInfo.multiInputWaitTmrHandle));
		SetAudioVisual(USER_RESP_TIMEOUT, DcInfo.respPort, NULL);
		SwitchDcToIdleState();
	}
}

//*****************************************************************************
//	SetTimerMultiInputWaitTimer()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         NONE
//	Description:
//		This function starts or reloads the multiple input wait timer for external pin reader.
//*****************************************************************************
BOOL SetTimerMultiInputWaitTimer(UINT32 timer)
{
	if(DcInfo.multiInputWaitTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t interDigitTime;

		interDigitTime.count = timer * 10;
		interDigitTime.funcPtr = &MultiInputWaitTimeoutOnExternal;
		interDigitTime.data = 0;
		if(StartTimer(interDigitTime, &(DcInfo.multiInputWaitTmrHandle), __FUNCTION__) == FAIL)
		{
			EPRINT(DC_LOG,"Unable to start Multi Input wait Timer FOR EXTERNAL PIN READER");
			return(FAIL);
		}
		return(SUCCESS);
	}
	else
	{
		return(ReloadTimer(DcInfo.multiInputWaitTmrHandle, (timer * 10) ) );
	}
}

//*****************************************************************************
//	MultiInputWaitTimeoutOnExternal()
//	Param:
//		IN : dummy
//		OUT: None
//	Returns:
//	         NONE
//	Description:
//		This is callback function for inter-digit timer.
//		This function sends command for audio visual indication for timeout.
//*****************************************************************************
void MultiInputWaitTimeoutOnExternal(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&(DcInfo.multiInputWaitTmrHandle));

	DcInfo.pinChangeModeState = PIN_CHANGE_IDLE_STATE;
	SetAudioVisual(USER_RESP_TIMEOUT, DcInfo.respPort, NULL);
	SwitchDcToIdleState();
}

//*****************************************************************************
//	SendVerifyHostTemplateByScanCmd()
//	Param:
//		IN : VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t verifyHostTempleteByScan
//		OUT: None
//	Returns:
//	         NONE
//	Description:
//		This function sends verify host template by scan comman to finger
//		reader.
//*****************************************************************************
void SendVerifyHostTemplateByScanCmd(VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t verifyHostTempleteByScan)
{
	DISABLE_FREE_SCAN_CMD_DATA_t	disableFreeScan;
	ENABLE_FREE_SCAN_CMD_DATA_t		enableFreeScan;

	disableFreeScan.readerId = verifyHostTempleteByScan.readerId;
	enableFreeScan.readerId = verifyHostTempleteByScan.readerId;

	if(BioIdentifyServer.enblBioServer == DISABLED)
	{
		ProcessReaderCommand(MSG_DISABLE_FREE_SCAN_CMD, &disableFreeScan, sizeof(DISABLE_FREE_SCAN_CMD_DATA_t));
	}
	ProcessReaderCommand(MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD, &verifyHostTempleteByScan, sizeof(VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t));
	if(BioIdentifyServer.enblBioServer == DISABLED)
	{
		ProcessReaderCommand(MSG_ENABLE_FREE_SCAN_CMD, &enableFreeScan, sizeof(ENABLE_FREE_SCAN_CMD_DATA_t));
	}
}
//*****************************************************************************
//	GetGoingIN_OUTSpecialFunctionInfo()
//	Param:
//		IN :
//		OUT:
//	Returns:
//
//	Description:
//
//*****************************************************************************
UINT8 GetGoingIN_OUTSpecialFunctionInfo(SLAVE_MSG_ID_e msgId,  UINT8PTR specialFuncIndexPtr)
{
    UINT8 idx, activeSpecialFunctionIdx=0;

    if((getDeviceInactiveStatus() == TRUE) && (TRUE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER1)))
    {
        return FALSE;
    }
    else if(msgId == GUI_BUTTON_GOING_IN)	//Going-In
    {
        for(idx = 0 ; idx < MAX_SPECIAL_FUNC ; idx++)
        {
            switch(idx)
            {
				case MARK_CLOCK_IN :
				case CLK_IN_ENTRY_OFFICIAL_WORK :
				case CLK_IN_MARKING_BREAK :
				case OVER_TIME_MARKING_IN :
				case SHORT_LEAVE_MARKING_IN :
				{
					if( SpecialFunctions[idx].enable == TRUE )
					{
						*specialFuncIndexPtr = idx;
						activeSpecialFunctionIdx++;
					}

				}break;

			default:
				break;
			}
		}
	}
	else if(msgId == GUI_BUTTON_GOING_OUT)	//Going-Out
	{
		for(idx = 0 ; idx < MAX_SPECIAL_FUNC ; idx++)
		{
			switch(idx)
			{
				case MARK_CLOCK_OUT :
				case CLK_OUT_EXIT_OFFICIAL_WORK :
				case CLK_OUT_MARKING_BREAK :
				case OVER_TIME_MARKING_OUT :
				case SHORT_LEAVE_MARKING_OUT :
				{
					if( SpecialFunctions[idx].enable == TRUE )
					{
						*specialFuncIndexPtr = idx;
						activeSpecialFunctionIdx++;
					}

				}break;

			default:
				break;
			}
		}
	}
	return activeSpecialFunctionIdx;
}

//*****************************************************************************
//	IsConnectToPanelEnabled()
//	Param:
//		IN : None
//		OUT: None
//	Returns:
//	         YES/NO
//	Description:
//		If connect to panel flag is enabled return yes alse return No
//*****************************************************************************
BOOL IsConnectToPanelEnabled(void)
{
	if(ConnectToPanelPara.connectToPanel == ENABLED)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

//*****************************************************************************
//	IsWaitingForAcsBleCredential()
//	Param:
//		IN : None
//		OUT: None
//	Returns:
//	         YES/NO
//	Description:
//		This Function will check whether waiting for ACS BLE Credential or Not?
//*****************************************************************************
BOOL IsWaitingForAcsBleCredential(VOID)
{
	CREDENTIAL_TYPE_e		curCredType;
	API_ACCESS_MODE_e 		apiaccessMode;

	curCredType = DcInfo.credType[DcInfo.credCnt - 1];

	if(DcInfo.readerMode == ENTRY_READER)
	{
		apiaccessMode = SysBasicPara.apiEntryAccessMode;
	}
	else
	{
		apiaccessMode = SysBasicPara.apiExitAccessMode;
	}

	if((DcInfo.apiUser == SET) && (curCredType == CB_READER_BLE_CREDENTIAL))
	{
		if((apiaccessMode == ACCESS_MODE_MOBILE_THEN_PIN) || (apiaccessMode == ACCESS_MODE_MOBILE_THEN_FACE))
		{
			return YES;
		}
	}
	return NO;
}

//*****************************************************************************
//	IsFREnable()
//	Param:
//		IN : None
//		OUT: None
//	Returns:
//	         YES/NO
//	Description:
//		This Function will check whether FR enable in device as well as in user.
//*****************************************************************************
BOOL IsFREnable(VOID)
{
	if((FRServerConfig.enableFrServer == TRUE) && (IsFaceRecoEnbl(DcInfo.userIdx) == TRUE))
	{
		return YES;
	}
	return NO;
}

//*****************************************************************************
//	HandleUnExpectedCommCloseReq()
//	Param:
//		IN : None
//		OUT: None
//	Returns:
//	         YES/NO
//	Description:
//		This Function will Handle Unexpected Mobile Application close request.
//*****************************************************************************
VOID HandleUnExpectedCommCloseReq(VOID)
{
	UINT8 dummyData;

	if(GetDoorApplType() == PANEL_DOOR_APPL)
	{
		ProcMsg(MSG_BLE_DISCONNECTED_UNEXPECTEDLY,&dummyData,sizeof(dummyData));
	}
	else
	{
		if(DcInfo.curState == DC_CAFETERIA_STATE)
		{
			if(cafeMenuProcInfo.cafeProcState == CAFE_PROC_STATE_POLICY_CHECK_CMD)
			{
				DPRINT(DC_LOG,"Option selection must be done within Ble session");
				SwitchCafeMenuToIdle(CAFE_MENU_IDLE_INSTANT);
			}
		}
		else if(IsWaitingForAcsPin() == YES)
		{
			DPRINT(DC_LOG,"Ble Connection Closed by Remote Device Unexpectedly");
			SwitchDcToIdleState();
		}
	}
}

//*****************************************************************************
//	IsWaitingForAcsPin()
//	Param:
//		IN : None
//		OUT: None
//	Returns:
//	         YES/NO
//	Description:
//		This Function will check whether ACS Request made for Mobile then Pin.
//*****************************************************************************
BOOL IsWaitingForAcsPin(VOID)
{
	CREDENTIAL_TYPE_e		curCredType;
	API_ACCESS_MODE_e 		apiaccessMode;

	curCredType = DcInfo.credType[DcInfo.credCnt - 1];

	if(DcInfo.readerMode == ENTRY_READER)
	{
		apiaccessMode = SysBasicPara.apiEntryAccessMode;
	}
	else
	{
		apiaccessMode = SysBasicPara.apiExitAccessMode;
	}

	if((DcInfo.apiUser == SET) && (curCredType == CB_READER_BLE_CREDENTIAL))
	{
		if(apiaccessMode == ACCESS_MODE_MOBILE_THEN_PIN)
		{
			return YES;
		}
	}
	return NO;
}

//*****************************************************************************
//	CheckHttpServerConfig()
//	Param:
//		IN : None
//		OUT: None
//	Returns:
//	         YES/NO
//	Description:
//
//*****************************************************************************
VOID CheckHttpServerConfig()
{
	if((strlen(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName) == 0) && (GetServerMode() == COSEC_CENTRA))
	{
		snprintf(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName, MAX_SERVER_ADDR_254, "%s", ServerParameters.ethServerHostName);
		snprintf(SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerHostName, MAX_SERVER_ADDR_254, "%s", ServerParameters.ethServerHostName);

		UpdateHttpServerHostNameService();

		if(TRUE == IsHttpReqFSMActive())
		{
			InsertHttpReqEvent(UF_RestartEvent, NULL);
		}

		if(SysBasicPara.httpWebServerConfig.httpSyncConfig.interfaceSelctionF == AUTO_SELECTION)
		{
			SetAutoInterfaceRestartF(AUTO_HTTP_SERVER, TRUE);
		}
	}
}
