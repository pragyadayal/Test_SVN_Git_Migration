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
//   Project      : ACS ( Access Control System)
//   Created By   : Bharat Gohil
//   File         : MxACMSCommand.c
//   Description  : This ACMS command file.ACMS send command,according to
//					command	function is execute and set parameter to ACSMaster.
/////////////////////////////////////////////////////////////////////////////

//system Includes
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

//matrix Includes.
#include "MxTypedef.h"
#include "MxEventLog.h"
#include "MxConfig.h"
#include "MxAcmsTask.h"
#include "MxAcmsConfig.h"
#include "MxAcmsCommand.h"
#include "MxAcmsCommon.h"
#include "MxLogs.h"
#include "MxAlarms.h"
#include "MxSysTimer.h"
#include "MxDcTask.h"
#include "MxGpio.h"
#include "MxIOLink.h"
#include "MxSysTime.h"
#include "MxTimeTrigFunc.h"
#include "MxSysUtils.h"
#include "MxEnrollment.h"
#include "MxSysCommonDef.h"
#include "MxCafeteria.h"
#include "MxAcmsMsgHandler.h"
#include "MxConfigFileIo.h"
#include "MxSysCommonUtils.h"
#include "MxFaceRecognition.h"
#include "MxFrLinkedLists.h"
#include "MxUserConfig.h"
#include "MxFaceRecognitionConfig.h"
#include "HttpReqFSM.h"
 #include "MxCommonConfig.h"

//******** Extern Variables **********
extern SYS_BASIC_PARA_t					SysBasicPara;
extern FP_TRANS_PARA_t 					FpTransPara;
extern MMC_INFO_t 						MmcInfo;
extern SYS_ADVANCE_PARA_t				SysAdvPara;
extern DOOR_ACTIVITY_INFO_t				DcInfo;
extern DOOR_MONITOR_t					DcMonitorControl;
extern BOOL 							FtpFrmwrUpgrdFrmServerF;
extern FR_SERVER_CONFIG_t				FRServerConfig;
extern READER_STATUS_e 					readerStatus[MAX_READERS];
//******** Defines and Data Types ****
#define FILENAME_SIZE			25


//******** Function Prototypes *******
static void ActAuxOutTimeout(UINT32 dummy);

//******** Global Variables **********
CAFE_CMD_DETAIL_t		CafeSetCmd;
//******** Static Variables **********

//******** Function Definations ******

//*****************************************************************************
//	AcmsProcDateTimeCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function sets time and date.
//*****************************************************************************
BOOL AcmsProcDateTimeCmd(CHARPTR * rxData)
{
	BOOL				retVal = FAIL;
	CHAR 				tempCmdBuff[ACMS_DATE_TIME_STR_LEN + 1];
	DATE_DDMMYYYY_t		date;
	TIME_t				time;
	TIME_ZONE_INFO_e	gmtTimeF = LOCAL_TIME;

	if( (SysBasicPara.timeUpdateMode == MANUAL_TIME) && (ParseString(rxData, tempCmdBuff, sizeof(tempCmdBuff) ) == SUCCESS) )
	{
		if(ParseString(rxData, tempCmdBuff, sizeof(tempCmdBuff) ) == SUCCESS)
		{
			gmtTimeF = GMT_TIME;
		}
		// We require date-time in specific format (DDMMYYYYHHMMSS)
		if(strlen(tempCmdBuff) == ACMS_DATE_TIME_STR_LEN)
		{
			// Extract DAY, MONTH and YEAR
			date.day = (UINT8)GetUINT16FromStringAndLen ( (tempCmdBuff + ACMS_DATE_TIME_DAY_STR_OFFSET), ACMS_DATE_DAY_STR_LEN);
			date.month = (UINT8)GetUINT16FromStringAndLen ( (tempCmdBuff + ACMS_DATE_TIME_MONTH_STR_OFFSET), ACMS_DATE_MONTH_STR_LEN);
			date.year = GetUINT16FromStringAndLen( (tempCmdBuff + ACMS_DATE_TIME_YEAR_STR_OFFSET), ACMS_DATE_YEAR_STR_LEN);

			// Extract HOUR, MINUTE and SECOND
			time.hour = (UINT8)GetUINT16FromStringAndLen ( (tempCmdBuff + ACMS_DATE_TIME_HOUR_STR_OFFSET), ACMS_TIME_HOUR_STR_LEN);
			time.min = (UINT8)GetUINT16FromStringAndLen ( (tempCmdBuff + ACMS_DATE_TIME_MINUTE_STR_OFFSET), ACMS_TIME_MINUTE_STR_LEN);
			time.sec = (UINT8)GetUINT16FromStringAndLen ( (tempCmdBuff + ACMS_DATE_TIME_SECOND_STR_OFFSET), ACMS_TIME_SECOND_STR_LEN);

			retVal = ChangeDateTime(date, time, gmtTimeF);
		}
	}
	return retVal;
}

//*****************************************************************************
//	AcmsProcAckAlarmCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function acknowledge given alarm.
//*****************************************************************************
UINT8 AcmsProcAckAlarmCmd(CHARPTR * rxData)
{
	UINT8			retVal = FAIL;
	UINT64			field;

	if(ParseField(rxData, &field) == SUCCESS)
	{
		AckAlarm(SOURCE_ACMS_EV, (ALARM_TYPE_e)field);
		retVal = SUCCESS;
	}
	return retVal;
}

//*****************************************************************************
//	AcmsProcClrAlarmCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function clears given alarm.
//*****************************************************************************
UINT8 AcmsProcClrAlarmCmd(CHARPTR * rxData)
{
	UINT8			retVal = FAIL;
	UINT64			field;

	if(ParseField(rxData, &field) == SUCCESS)
	{
		ClearAlarm(SOURCE_ACMS_EV, (ALARM_TYPE_e)field);
		retVal = SUCCESS;
	}
	return retVal;
}

//*****************************************************************************
//	AcmsProcDoorLockCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function locks door.
//*****************************************************************************
UINT8 AcmsProcDoorLockCmd(UNUSED_PARA CHARPTR * rxData)
{
	SetDoorLockState(DOOR_LOCKED);
	return SUCCESS;
}

//*****************************************************************************
//	AcmsProcDoorUnLockCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function unlocks door.
//*****************************************************************************
UINT8 AcmsProcDoorUnLockCmd(UNUSED_PARA CHARPTR * rxData)
{
	SetDoorLockState(DOOR_UNLOCKED);
	return SUCCESS;
}

//*****************************************************************************
//	AcmsProcDoorNormalCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function normals door.
//*****************************************************************************
UINT8 AcmsProcDoorNormalCmd(UNUSED_PARA CHARPTR * rxData)
{
	SetDoorLockState(DOOR_LOCK_NORMAL);
	return SUCCESS;
}

//*****************************************************************************
//	AcmsProcDoorAuxInDisableCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function disable aux input (soft override).
//*****************************************************************************
UINT8 AcmsProcDoorAuxInDisableCmd(UNUSED_PARA CHARPTR * rxData)
{
	return ChangeDoorSoftStatus(CHANGE_DC_AUX_IN, SOFT_DISABLED);
}

//*****************************************************************************
//	AcmsProcDoorAuxInNormalCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function put aux input to normal state.
//*****************************************************************************
UINT8 AcmsProcDoorAuxInNormalCmd(UNUSED_PARA CHARPTR * rxData)
{
	return ChangeDoorSoftStatus(CHANGE_DC_AUX_IN, DOOR_STATUS_NORMAL);
}

//*****************************************************************************
//	AcmsProcDoorAuxOutDisableCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function disables aux output (soft override).
//*****************************************************************************
UINT8 AcmsProcDoorAuxOutDisableCmd(UNUSED_PARA CHARPTR * rxData)
{
	return ChangeDoorSoftStatus(CHANGE_DC_AUX_OUT, SOFT_DISABLED);
}

//*****************************************************************************
//	AcmsProcDoorAuxOutNormalCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function puts aux output to normal state.
//*****************************************************************************
UINT8 AcmsProcDoorAuxOutNormalCmd(UNUSED_PARA CHARPTR * rxData)
{
	return ChangeDoorSoftStatus(CHANGE_DC_AUX_OUT, DOOR_STATUS_NORMAL);
}

//*****************************************************************************
//	AcmsProcDoorSenseDisableCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function disables door sense (soft override).
//*****************************************************************************
UINT8 AcmsProcDoorSenseDisableCmd(UNUSED_PARA CHARPTR * rxData)
{
	return ChangeDoorSoftStatus(CHANGE_DC_SENSE, SOFT_DISABLED);
}

//*****************************************************************************
//	AcmsProcDoorSenseNormalCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function puts door sense to normal state.
//*****************************************************************************
UINT8 AcmsProcDoorSenseNormalCmd(UNUSED_PARA CHARPTR * rxData)
{
	return ChangeDoorSoftStatus(CHANGE_DC_SENSE, DOOR_STATUS_NORMAL);
}

//*****************************************************************************
//	AcmsProcIOLinkRstCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function resets output associated with given IO link.
//*****************************************************************************
UINT8 AcmsProcIOLinkRstCmd(CHARPTR * rxData)
{
	UINT8	retVal = FAIL;
	UINT8 	ioLinkIdx;
	UINT64	field;

	if( (ParseField(rxData, &field) ) == SUCCESS)
	{
		ioLinkIdx = (UINT8)field - ACMS_INDEX_OFFSET;
		if(ioLinkIdx < MAX_IO_LINK)
		{
			ResetIoLink(ioLinkIdx);
			retVal = SUCCESS;
		}
	}
	return retVal;
}

//*****************************************************************************
//	AcmsProcDoorAuxOutLatchRstCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function resets aux output.
//*****************************************************************************
UINT8 AcmsProcDoorAuxOutLatchRstCmd(UNUSED_PARA CHARPTR * rxData)
{
	return ChangeDoorSoftStatus(RESET_AUX_OUT, DOOR_STATUS_NORMAL);
}

//*****************************************************************************
//	AcmsProcLogoutCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function logouts ACS from ACMS if login code matches.
//*****************************************************************************
UINT8 AcmsProcLogoutCmd(CHARPTR * rxData)
{
	UINT8	retVal = FAIL;
	UINT64	field;

	// Parse Login code
	// If fail, don't process further
	if(ParseField(rxData, &field) == SUCCESS)
	{
		//Logout only if login code match
		if( (UINT32)field == LOGIN_CODE)
		{
			LogoutAcms(ACMS_COMM_LOGOUT, CLOSE_ALL_SERVER_SOCKETS);
			retVal = SUCCESS;
		}
	}
	return retVal;
}

//*****************************************************************************
//	AcmsProcRestoreUserCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function restores user from block user list.
//*****************************************************************************
UINT8 AcmsProcRestoreUserCmd(CHARPTR * rxData)
{
	UINT8	retVal = FAIL;
	UINT16 	userIdx;
	UINT32	userId;
	UINT64	field;

	if(ParseField(rxData, &field) == SUCCESS)
	{
		userId = (UINT32)field;

		if(IsUserValid(userId, &userIdx) == YES)
		{
			RestoreBlockedUser(userIdx);
        	retVal = SUCCESS;
 		}
	}
	return retVal;
}

//*****************************************************************************
//	AcmsProcClrAllFpCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function deletes all finger print templates from door.
//*****************************************************************************
UINT8 AcmsProcClrAllFpCmd(UNUSED_PARA CHARPTR * rxData)
{
	return ChangeDoorSoftStatus(CLEAR_ALL_FP, DOOR_STATUS_NORMAL);
}

//*****************************************************************************
//	AcmsProcDeleteFpCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function deletes finger print of a given user from system.
//*****************************************************************************
UINT8 AcmsProcDeleteFpCmd(CHARPTR * rxData)
{
	UINT8	retVal = FAIL,evField2;
	UINT16 	userIdx;
	UINT64	field;

	if(ParseField(rxData,&field) == SUCCESS)
	{
		FpTransPara.usrId = (UINT32)field;
		if(IsUserValid(FpTransPara.usrId, &userIdx) == YES)
		{	
			if((MAX_READERS != GetSupremaReaderId()) && (readerStatus[GetSupremaReaderId()] == READER_IS_ACTIVE))
			{
				FpTransPara.fpTransStatus = FP_TRANS_STATUS_IN_PROCESS;
			}
			else
			{
				FpTransPara.fpTransStatus = FP_TRANS_STATUS_IDLE; 
			}
			retVal = DeleteUserFp(userIdx);
			// log event for credential- FP are deleted
			if(retVal == SUCCESS)
			{
				if(SysBasicPara.fprTemplateFormat == ISO_19794_FORMAT)
				{
					evField2 = USER_FP_ISO_FORMAT;
				}
				else
				{
					evField2 = USER_FP;
				}
				WriteEventLog(USER_CREDENTIAL_DELETED, GetUserId(userIdx), evField2, SOURCE_ACMS_EV, UNUSED_FIELD, UNUSED_FIELD);
			}
		}
	}
	return retVal;
}

//*****************************************************************************
//	AcmsProcDeleteCardCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function deletes given user's cards from system.
//*****************************************************************************
UINT8 AcmsProcDeleteCardCmd(CHARPTR * rxData)
{
	UINT16	userIdx;
	UINT32	userId;
	UINT64	field;

	if( (ParseField(rxData, &field) ) == SUCCESS)
	{
		userId = (UINT32)field;

		if(IsUserValid(userId, &userIdx) == YES)
		{
			DeleteUserCards(userIdx);
			// log event for credential- cards are deleted
			WriteEventLog(USER_CREDENTIAL_DELETED, GetUserId(userIdx), USER_CARD, SOURCE_ACMS_EV, UNUSED_FIELD, UNUSED_FIELD);
			return SUCCESS;
		}
	}

	return FAIL;
}

//*****************************************************************************
//	AcmsProcSoftRebootCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function reboots system.
//*****************************************************************************
UINT8 AcmsProcSoftRebootCmd(UNUSED_PARA CHARPTR * rxData)
{
	SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
	return SUCCESS;
}

//*****************************************************************************
//	AcmsProcSystemDefaultCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function defaults configuration of system.
// NOTE: this function doesn't default network and password configuration.
//*****************************************************************************
UINT8 AcmsProcSystemDefaultCmd(UNUSED_PARA CHARPTR * rxData)
{
	// Set flag = TRUE to default system.
	// If set successfully then reboot the system.
	// System should default configuartion after power ON by looking this flag
	if(WriteDfltSysFlag(SYS_DFLT_BASIC) == SUCCESS)
	{
		SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	AcmsProcUpdateFirmwareCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function updates firmware put in FTP.
//		This function is not supported now.
//*****************************************************************************
UINT8 AcmsProcUpdateFirmwareCmd(UNUSED_PARA CHARPTR * rxData)
{
	return FAIL;
}

//*****************************************************************************
//	AcmsProcTimeTriggerResetCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function resets Time Trigger function.
//*****************************************************************************
UINT8 AcmsProcTimeTriggerResetCmd(CHARPTR * rxData)
{
	UINT8	retVal = FAIL;
	UINT8	funcIdx;
	UINT64	field;

	if( (ParseField(rxData, &field) ) == SUCCESS)
	{
		funcIdx = ((UINT8)field) - ACMS_INDEX_OFFSET;
		if(funcIdx < MAX_TIME_TRIG_FUNC)
		{
			ResetTimeTrigFunc((UINT32)funcIdx);
			SetTimeTrigFuncManuallyResetStatus(funcIdx);
			retVal = SUCCESS;
		}
	}
	return retVal;
}

//*****************************************************************************
//	AcmsProcDoorOpenCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function opens door.
//*****************************************************************************
UINT8 AcmsProcDoorOpenCmd(UNUSED_PARA CHARPTR * rxData)
{
	DcInfo.userIdDoorOpenCloseEvent = INVALID_USER_ID;
	memset(DcInfo.alphanumIddoorOpenCloseEvent, 0, sizeof(DcInfo.alphanumIddoorOpenCloseEvent));
	return OpenDoor();
}

//*****************************************************************************
//	AcmsDoorOpenCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function opens door.
//*****************************************************************************
UINT8 AcmsDoorOpenCmd(UNUSED_PARA CHARPTR * rxData)
{
	if( ((DcInfo.curState != DC_CAFETERIA_STATE) && (DcInfo.curState != DC_IDLE_STATE))  || (DcMonitorControl.doorLock == DOOR_LOCKED) || (DcMonitorControl.doorLock == DOOR_UNLOCKED))
	{
		return FAIL;
	}

	DcInfo.userIdDoorOpenCloseEvent = INVALID_USER_ID;
	memset(DcInfo.alphanumIddoorOpenCloseEvent, 0, sizeof(DcInfo.alphanumIddoorOpenCloseEvent));
	return OpenDoor();
}
//*****************************************************************************
//	AcmsProcChangePasswordCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function changes the password.
//*****************************************************************************
UINT8 AcmsProcChangePasswordCmd(CHARPTR * rxData)
{
	UINT8				retVal = FAIL;
	CHAR 				tempCmdBuff[MAX_SYS_CMD_BUF_SZ];
	CONFIG_USER_TYPE_e	userType = MAX_USER_TYPE;

	if(ParseString(rxData, tempCmdBuff, MAX_SYS_CMD_BUF_SZ) == SUCCESS)
	{
		if(strcasecmp(tempCmdBuff, "HRD") == 0)
		{
			userType = USER_HRD;
		}
		else if(strcasecmp(tempCmdBuff, "ADMIN") == 0)
		{
			userType = USER_ADMIN;
		}
		else
		{
			return FAIL;
		}

		if(ParseString(rxData, tempCmdBuff, MAX_SYS_CMD_BUF_SZ) == SUCCESS)
		{
			if(strlen(tempCmdBuff) < MAX_PASSWORD_SIZE)
			{
				ChangeDisplayPswd(userType, tempCmdBuff);
				WritePswdConfig();
				retVal = SUCCESS;
			}
		}
	}

	return retVal;
}

//*****************************************************************************
//	AcmsProcRechargeCafeBalance()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    	This function assign received data to cafeteria command structure and
//		initiate procedure to recharge user card for cafeteria.
//
//*****************************************************************************
UINT8 AcmsProcRechargeCafeBalance(CHARPTR * rxData)
{
	UINT64	field;
	CHAR	amount[MAX_CAFE_BALANCE_SIZE+1];
	UINT8	retVal = FAIL;

	if(ParseField(rxData, &field) == SUCCESS)
	{
		CafeSetCmd.cmd = CAFE_RECHARGE_TRANSACTION;
		CafeSetCmd.userId = (UINT32)field;
		if(ParseString(rxData, amount, MAX_CAFE_BALANCE_SIZE + 3) == SUCCESS)
		{
			FLOAT64 tempBalance = atof(amount);
			if(tempBalance >= (MAX_CAFE_USER_BALANCE_NEGATIVE) && tempBalance <= (MAX_CAFE_USER_BALANCE_POSITIVE))
			{
				EPRINT(DC_LOG,"amount = [%s]",amount);
				CafeSetCmd.amount = FloatStr32ToInt32(amount);
				retVal = ProcCafeCmd();
			}
			else
			{
				EPRINT(ACMS_LOG,"Received recharge amount is Out Of Range");
				return FAIL;
			}
		}
	}

	return retVal;
}

//*****************************************************************************
//	AcmsProcResetCafeBalance()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    	This function assign received data to cafeteria command structure and
//		initiate procedure to recharge user card for cafeteria.
//*****************************************************************************
UINT8 AcmsProcResetCafeBalance(CHARPTR * rxData)
{
	UINT64	field;
	CHAR	amount[MAX_CAFE_BALANCE_SIZE+3];
	UINT8	retVal = FAIL;

	if(ParseField(rxData, &field) == SUCCESS)
	{
		CafeSetCmd.cmd = CAFE_RESET_TRANSACTION;
		CafeSetCmd.userId = (UINT32)field;
		if(ParseString(rxData, amount, MAX_CAFE_BALANCE_SIZE+3) == SUCCESS)
		{
			FLOAT64 tempBalance = atof(amount);
			if(tempBalance >= (MAX_CAFE_USER_BALANCE_NEGATIVE) && tempBalance <= (MAX_CAFE_USER_BALANCE_POSITIVE))
			{
				CafeSetCmd.amount = FloatStr32ToInt32(amount);
				retVal = ProcCafeCmd();
			}
			else
			{
				EPRINT(ACMS_LOG,"Received Reset amount is Out Of Range");
				return FAIL;
			}
		}
		else
		{
			EPRINT(DC_LOG,"Reset value parsing failed");
		}
	}

	return retVal;
}

//*****************************************************************************
//	AcmsProcDeleteCafeItemImage()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function delete the item image stored into the external memory.
//*****************************************************************************
UINT8 AcmsProcDeleteCafeItemImage(UNUSED_PARA CHARPTR * rxData)
{
#ifdef CAFETERIA_ITEM_IMAGE_SUPPORTED
	UINT64	field;

	if( (ParseField(rxData, &field) ) == SUCCESS)
	{
		ProcessDeleteCafeItemImageCmd((UINT8)field);
		return SUCCESS;
	}
#endif
	return FAIL;
}

//*****************************************************************************
//	AcmsProcDeleteUserPhotoCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function delete the user photo stored into the external memory.
//*****************************************************************************
UINT8 AcmsProcDeleteUserPhotoCmd(CHARPTR * rxData)
{
	UINT32	userId;
	UINT64	field;
	UINT16 	userIdx;

	if( (ParseField(rxData, &field) ) == SUCCESS)
	{
		userId = (UINT32)field;
		if(userId != INVALID_USER_ID) 
		{
			if(IsUserValid(userId, &userIdx) == YES)
			{
				HandleDeleteUserPhotoCmd(userIdx);
				return SUCCESS;
			}
		}
	}
	return FAIL;
}


//*****************************************************************************
//	ActAuxOutTimeout()
//	Param:
//		IN:	NONE
//		OUT: NONE
//	Returns:
//		NONE
//	Description:
//	    This function Deactivates Aux output when at timeout
//*****************************************************************************
static void ActAuxOutTimeout(UNUSED_PARA UINT32 dummy)
{
	DeactivateAuxRelay();
	DeleteTimer(&DcInfo.auxOutActTmrHandle);
}

//*****************************************************************************
//	AcmsProcActAuxOutCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function activates Aux output for time defined in command
//*****************************************************************************
UINT8 AcmsProcActAuxOutCmd (CHARPTR * rxData)
{
	UINT8				retVal = FAIL;
	UINT64				field;
	TIMER_INFO_t        tempTimerInfo;

	if(ParseField(rxData, &field) == SUCCESS)
	{
		if( (DcMonitorControl.auxOutDsbl == DOOR_STATUS_NORMAL) && ((SysAdvPara.auxOutput.enable == TRUE) && (DcInfo.curState == DC_IDLE_STATE)))
		{
			if(DcInfo.auxOutActTmrHandle == INVALID_TIMER_HANDLE)
			{
				tempTimerInfo.count = ( (UINT32)(field) * 10);
				tempTimerInfo.funcPtr = &ActAuxOutTimeout;
				tempTimerInfo.data = 0;
				retVal = StartTimer(tempTimerInfo, &DcInfo.auxOutActTmrHandle,__FUNCTION__);
				ActivateAuxRelay((UINT16)(field));
			}
		}
	}
	return retVal;
}

//**************************************************************************************
//	AcmsProcFtpFirmwareUpgradeCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function Force upgrade FTP firmware when recieve firmware upgrade command
//***************************************************************************************
UINT8 AcmsProcFtpFirmwareUpgradeCmd (UNUSED_PARA CHARPTR * rxData)
{
	FtpFrmwrUpgrdFrmServerF = TRUE;	// This flag is monitor from main.c file

	return SUCCESS;
}


//**************************************************************************************
//	AcmsProcAPBReset()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function Force upgrade FTP firmware when recieve firmware upgrade command
//***************************************************************************************
UINT8 AcmsProcAPBResetCmd (CHARPTR * rxData)
{
	UINT32	userId;
	UINT64	field;
	UINT16	userIdx;
	if( (ParseField(rxData, &field) ) == SUCCESS)
	{
		userId = (UINT32)field;
		if(IsUserValid(userId, &userIdx) == YES)
		{
			ResetAntiPassBackForUser(userIdx);
			return SUCCESS;
		}
	}
	return FAIL;
}

//**************************************************************************************
//	AcmsProcDeleteOneFaceTmplOfUserCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function will delete all Face Templates of one user
//***************************************************************************************
UINT8 AcmsProcDeleteOneFaceTmplOfUserCmd (UNUSED_PARA CHARPTR * rxData)
{
	UINT32	userId;
	UINT64	field;
	CHAR 	dest[5]; //Array of 5 has been taken because face Idx will be from 0 to 29 only e.g 2 digits only
	NonIDTRequest_t tmpNonIDTRequestData;
	GetFRNonIDTRequestData(&tmpNonIDTRequestData,NON_IDT_TYPE_FTC);

	if(tmpNonIDTRequestData.isLocallyProcessing == TRUE 
		|| tmpNonIDTRequestData.frProcessRequestStatus == INQUEUE_STATUS_SET)
	{
		EPRINT(ACMS_LOG, "FR NonIDT operation is going on..FR is busy");
		return FAIL;
	}

	if((ParseField(rxData, &field) ) == SUCCESS)
	{
		userId = (UINT32)field;

		if((userId != INVALID_USER_ID) && (ParseString(rxData, dest, 8) == SUCCESS) )
		{
			GetFRNonIDTRequestData(&tmpNonIDTRequestData, NON_IDT_TYPE_CMD);
			if((strlen(dest) > 0) )
			{
				field = atoll(dest);
				tmpNonIDTRequestData.faceIdx = field;
			}
			else
			{
				tmpNonIDTRequestData.faceIdx =  INVALID_FACE_TMPLT_IDX;
			}

			// Fill the Structure as we are deleting all face templates of one user
			tmpNonIDTRequestData.userRefId = userId;
			tmpNonIDTRequestData.frProcessRequestId = CMD_USR_DELFACE;
			SetFRNonIDTRequestData(tmpNonIDTRequestData, NON_IDT_TYPE_CMD);

			if(ProcessNonIDTRequest(NULL, NULL) == SUCCESS)
			{
				return SUCCESS;
			}
			else
			{
				EPRINT(SYS_LOG,"Failed to start thread to delete face templates for ONE user");
				return FAIL;
			}
		}
		else
		{
			EPRINT(SYS_LOG,"Failed to parse face index or User Id invalid");
			return FAIL;
		}
	}
	else
	{
		EPRINT(SYS_LOG,"Failed to parse User Id");
	}
	return FAIL;
}

//**************************************************************************************
//	AcmsProcDeleteAllFaceTmplOfUserCmd()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	   This function will delete all Face Templates of All users
//***************************************************************************************
UINT8 AcmsProcDeleteAllFaceTmplOfUserCmd (UNUSED_PARA CHARPTR * rxData)
{
	NonIDTRequest_t tmpNonIDTRequestData;

	// Fill the Structure as we are deleting all face templates of all users
	GetFRNonIDTRequestData(&tmpNonIDTRequestData,NON_IDT_TYPE_FTC);

	if(tmpNonIDTRequestData.isLocallyProcessing == TRUE 
		|| tmpNonIDTRequestData.frProcessRequestStatus == INQUEUE_STATUS_SET)
	{
		EPRINT(ACMS_LOG, "FR NonIDT operation is going on..FR is busy");
		return FAIL;
	}
	tmpNonIDTRequestData.userRefId = INVALID_USER;
	tmpNonIDTRequestData.faceIdx = INVALID_FACE_TMPLT_IDX;
	tmpNonIDTRequestData.frProcessRequestId = CMD_USR_CLEAR_ALL_FACE;
	SetFRNonIDTRequestData(tmpNonIDTRequestData, NON_IDT_TYPE_CMD);

	if(ProcessNonIDTRequest(NULL, NULL) == SUCCESS)
	{
		return SUCCESS;
	}
	else
	{
		EPRINT(SYS_LOG,"Failed to start thread to delete ALL face-templates for ALL users ");
		return FAIL;
	}
}


//**************************************************************************************
//	AcmsProcUpdateFace()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function handle UpdateFace command from ACMS.
//***************************************************************************************
UINT8 AcmsProcUpdateFace(CHARPTR * rxData)
{
	UINT8 retSts = false;
    UINT64	field = 0;
	UINT32 userId = INVALID_USER_ID;
    UINT16 userIdx = UNKNOWN_USER_IDX;
	UF_TYPE_e updateFaceType = UF_TYPE_MAX;

	if( (FALSE == FRServerConfig.enableFrServer) || (FRServerConfig.frMode != FR_MODE_LOCAL) )
	{
		EPRINT(ACMS_LOG,"Ignore UPDATEFACE command, FR is disabled or FR Mode other than Local");
		return retSts;
	}

	if((ParseField(rxData, &field) ) == SUCCESS)
	{
		updateFaceType = (UF_TYPE_e)field;
		switch(updateFaceType)
		{
			case UF_TYPE_UPDATEDFACE:
			case UF_TYPE_RESYNC:
			case UF_TYPE_DELFACE:
				field = 0;
				if(FAIL == ParseField(rxData, &field))
				{
					EPRINT(ACMS_LOG,"Failed to Parse User ID for UPDATEFACE command Type:[%d]",updateFaceType);
					return retSts;
				}
				userId = (UINT32)field;
				if((userId == INVALID_USER_ID) || (NO == IsUserValid(userId, &userIdx)))
				{
                    EPRINT(ACMS_LOG,"UPDATEFACE failed since Invalid user found user:[%u]",userId);
					return retSts;
				}

				if(userIdx == UNKNOWN_USER_IDX)
				{
					EPRINT(ACMS_LOG,"UPDATEFACE failed since user index is unknown");
					return retSts;
				}
				break;
			case UF_TYPE_CLEARALL:
				break;
			case UF_TYPE_MAX:
				EPRINT(ACMS_LOG,"Invalid updateFaceType:[%d] for UPDATEFACE command",updateFaceType);
				return retSts;
		}

		retSts = HandleUpdateFaceCmd(updateFaceType, userIdx);
	}
	return retSts;
}

//**************************************************************************************
//	AcmsProcPhotoSync()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	   This function will Parse PhotSync Command And Handle it.
//***************************************************************************************
UINT8 AcmsProcPhotoSync(CHARPTR * rxData)
{
	UINT8 retSts = false;
    UINT64	field = 0;
	UINT32 userId = INVALID_USER_ID;
    UINT16 userIdx = UNKNOWN_USER_IDX;
	PHOTO_SYNC_TYPE_e photoSyncType;
	
	if((ParseField(rxData, &field) ) == SUCCESS)
	{
		photoSyncType = (PHOTO_SYNC_TYPE_e)field;
		switch(photoSyncType)
		{
			default:
			{
				return retSts;
			}break;
			
			case PROFILE_PHOTO_TYPE:
			{
				if(FAIL == ParseField(rxData, &field))
				{
					EPRINT(ACMS_LOG,"Failed to Parse User ID for GETPHOTO command Type:[%d]",photoSyncType);
					return retSts;
				}
				userId = (UINT32)field;
				EPRINT(ACMS_LOG,"Received userId = %d",userId);
				if((userId == INVALID_USER_ID) || (NO == IsUserValid(userId, &userIdx)))
				{
					EPRINT(ACMS_LOG,"GETPHOTO failed since Invalid user found user:[%u]",userId);
					return retSts;
				}

				if(userIdx == UNKNOWN_USER_IDX)
				{
					EPRINT(ACMS_LOG,"GETPHOTO failed since user index is unknown");
					return retSts;
				}
			}break;
		}
		retSts = HandlePhotoSyncCmd(photoSyncType, userIdx);
	}
	return retSts;
}
