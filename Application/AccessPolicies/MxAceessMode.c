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
//   Project      : Access Control System
//   Created By   : Ronak Tanna
//   File         : MxDcTaskFeatures.c
//   Description  : This file contains Access mode featue related functionality.
//
/////////////////////////////////////////////////////////////////////////////

//******** Include files **********
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "MxTypedef.h"
#include "MxSlaveMsg.h"
#include "MxConfig.h"
#include "MxDcTask.h"
#include "MxSysTimer.h"
#include "MxConfigFileIo.h"
#include "MxEventLog.h"
#include "MxAvResp.h"
#include "MxLogs.h"
#include "MxSysUtils.h"
#include "MxSysTime.h"
#include "MxReaderUtils.h"
#include "MxReaderCommonUtils.h"
#include "MxFaceRecognition.h"
#include "MxCommonBLE.h"
#include "MxFaceRecognitionConfig.h"
#include "MxUserConfig.h"

//******** Extern Variables **********
extern DOOR_ACTIVITY_INFO_t 	DcInfo;
extern SYS_BASIC_PARA_t			SysBasicPara;
extern SYS_ADVANCE_PARA_t		SysAdvPara;
extern FR_SERVER_CONFIG_t		FRServerConfig;
extern UINT8 			cardId[];
extern COMM_STATE_e				CommState;

//****** Defines and Data Types ******

//******** Function Prototypes *******
static void MultiAccessTimeout(UINT32 dummy);

//******** Global Variables **********

//******** Static Variables **********
static BOOL isStartFrStreamFailed = FALSE;

//******** Function Definitions ******

//*****************************************************************************
//	MultiAccessTimeout()
//	Param: 
//		IN : NONE
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//		This function is callback function for 'multi access timer' (ZAM)
//		It resets timer handle as invalid, logs event for user denied because
//		of timeout and gives audio visual indication for denial.
//*****************************************************************************
static void MultiAccessTimeout(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&DcInfo.multiAccessTmrHandle);
	DcInfo.authReason = AUTH_TIMEOUT;
	DEG_EV_CRED_TYPE_e credType = CRED_PIN;

	UINT32 tempUserID = INVALID_USER_ID;
	// Check if ble session is running
	if(IsWaitingForAcsPin() == YES)
	{
		ACS_BLE_ACCESS_REQ_RESPONSE_t	respBtData;
		respBtData.readerId = DcInfo.readerId;
		respBtData.respReason = QR_AUTH_TIMEOUT;
		respBtData.respStatus = ACS_RESP_USER_DENIED;
		SendBLEEventTask(INT_BLE_PIN_ACCESS_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
	}

	if(PANEL_DOOR_APPL == GetDoorApplType())
	{
		tempUserID = GetUserId(DcInfo.zamUserIdx);
		switch(DcInfo.credType[FIRST_CREDENTIAL])
		{
			default:
			case PIN_CREDENTIAL:
				credType = CRED_PIN;
				PDUserDenied(USER_RESP_PD_DA_USER_DENIED, credType, DcInfo.readerMode,TRUE, (VOIDPTR)&tempUserID);
				break;
			case CARD_CREDENTIAL:
				credType = CRED_CARD;
				PDUserDenied(USER_RESP_PD_DA_USER_DENIED, credType, DcInfo.readerMode,TRUE, (VOIDPTR)&cardId);
				break;
			case FP_CREDENTIAL:
				credType = CRED_FP;
				PDUserDenied(USER_RESP_PD_DA_USER_DENIED, credType, DcInfo.readerMode,TRUE, (VOIDPTR)&tempUserID);
				break;
			case CB_READER_BLE_CREDENTIAL:
				credType = CRED_ACS_BLE;
				PDUserDenied(USER_RESP_PD_DA_USER_DENIED, credType, DcInfo.readerMode,TRUE, (VOIDPTR)&cardId);
				break;
			case ACCESS_CARD_QR_CREDENTIAL:
				credType = CRED_QR_AS_CARD;
				PDUserDenied(USER_RESP_PD_DA_USER_DENIED, credType, DcInfo.readerMode,TRUE, (VOIDPTR)&cardId);
				break;
		}
		DPRINT(DC_LOG, "[Degrade] User Idx[%d] Denied Due to MultiAccess Time-Out for credType[%d]",DcInfo.zamUserIdx,credType);
		return;
	}
	DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
}

//*****************************************************************************
//	LoadMultiAccessTimer()
//	Param: 
//		IN : timerValue	- timer value in seconds
//		OUT: NONE
//	Returns:
//			 SUCCESS/FAIL
//	Description:
//		This function will start multi access timer for ZAM.When timer times 
//		out, it will call MultiAccessTimeOut function.
//*****************************************************************************
BOOL LoadMultiAccessTimer(UINT16 timerValue)
{
 	if(DcInfo.multiAccessTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = (timerValue * 10);
		tInfo.funcPtr = &MultiAccessTimeout;
		tInfo.data = 0;

		return(StartTimer(tInfo, &(DcInfo.multiAccessTmrHandle), __FUNCTION__));
	}
	else
	{
		return(ReloadTimer(DcInfo.multiAccessTmrHandle, (timerValue * 10)));
	}
}

//*****************************************************************************
//	IsCredentialValid
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	YES	-	If credential is as per ZAM
//				NO	-	If not valid for that zone
//	Description:
//			This function will check whether the credential is as per zone ZAM policy.
//			If multiple credentials are required, order in which it is presented
//			is not fixed. 
//*****************************************************************************
BOOL IsCredentialValid(void)
{
	BOOL					retVal = NO;
	CREDENTIAL_CNT_e		credentialCnt;
	CREDENTIAL_TYPE_e		curCredType;
	UINT16					userIdx;
	ACCESS_MODE_e			accessMode;
	BOOL					twoCredMstF = FALSE,accessModeApplicable=NO;

	userIdx = DcInfo.userIdx;	

	credentialCnt = (DcInfo.credCnt - 1);
	curCredType = DcInfo.credType[credentialCnt];

	if(DcInfo.procSSAF == TRUE)
	{
		accessMode = SysAdvPara.SSAConfig.accessMode;
	}
	else if(DcInfo.apiUser == SET)
	{
		API_ACCESS_MODE_e apiaccessMode;

		if(DcInfo.readerMode == ENTRY_READER)
		{
			apiaccessMode = SysBasicPara.apiEntryAccessMode;
		}
		else
		{
			apiaccessMode = SysBasicPara.apiExitAccessMode;
		}

		DPRINT(DC_LOG, "apiaccessMode %u curCredType %u credentialCnt %u", apiaccessMode, curCredType, credentialCnt);
		switch(apiaccessMode)
		{
			default:
				break;
			case ACCESS_MODE_QR_CODE:
				if((curCredType == BLE_QR_CREDENTIAL) || (curCredType == CB_READER_BLE_CREDENTIAL))
				{
					retVal = YES;
				}
				break;
			case ACCESS_MODE_QR_CODE_THEN_BIO:
			{
				if(credentialCnt == FIRST_CREDENTIAL)
				{
					if((curCredType ==  BLE_QR_CREDENTIAL) || (curCredType == CB_READER_BLE_CREDENTIAL))
					{
						retVal = YES;
					}
				}
				// second credential,then cannot be same as first credential
				else if(credentialCnt == SECOND_CREDENTIAL)
				{
					//Both credential should not be same
					if(curCredType == FP_CREDENTIAL)
					{
						retVal = YES;
					}
				}
				break;
			}
			case ACCESS_MODE_QR_CODE_THEN_CARD:
			{
				if(credentialCnt == FIRST_CREDENTIAL)
				{
					if((curCredType ==  BLE_QR_CREDENTIAL) || (curCredType == CB_READER_BLE_CREDENTIAL))
					{
						retVal = YES;
					}
				}
				// second credential,then cannot be same as first credential
				else if(credentialCnt == SECOND_CREDENTIAL)
				{
					//Both credential should not be same
					if(curCredType == CARD_CREDENTIAL)
					{
						retVal = YES;
					}
				}
				break;
			}
			case ACCESS_MODE_MOBILE_THEN_PIN:
			{
				if(credentialCnt == FIRST_CREDENTIAL)
				{
					if((curCredType == CB_READER_BLE_CREDENTIAL) || (curCredType == BLE_QR_CREDENTIAL))
					{
						retVal = YES;
					}
				}
				// second credential,then cannot be same as first credential
				else if(credentialCnt == SECOND_CREDENTIAL)
				{
					//Both credential should not be same
					if(curCredType == PIN_CREDENTIAL)
					{
						retVal = YES;
					}
				}
				break;
			}
#if 0
			case ACCESS_MODE_MOBILE_THEN_FACE:
			{
				if(credentialCnt == FIRST_CREDENTIAL)
				{
					if(curCredType == CB_READER_BLE_CREDENTIAL)
					{
						retVal = YES;
					}
				}
				// second credential,then cannot be same as first credential
				else if(credentialCnt == SECOND_CREDENTIAL)
				{
					//Both credential should not be same
					if(curCredType == FACE_CREDENTIAL)
					{
						retVal = YES;
					}
				}
				break;
			}
#endif
		}
		return(retVal);
	}
	else
	{
		accessMode = GetAccessModeFromRespPort(DcInfo.userType, DcInfo.respPort);
		twoCredMstF = SysAdvPara.twoCredMustModeF;
	}

	if(YES == (CheckComboRdrPresent(COMBO_READER_CB_U)) && (DcInfo.respPort == EXTERNAL_PORT))
	{
		if(curCredType == CB_READER_BLE_CREDENTIAL)
		{
			if((accessMode == ACCESS_MODE_BLE)||(accessMode == ANY_CREDENTIAL))
			{
				retVal = YES;
			}
		}
		else if((curCredType == CARD_CREDENTIAL)&& ((accessMode == CARD_ONLY) || (accessMode == ANY_CREDENTIAL)))
		{
			retVal = YES;
		}
		return retVal;
	}
	if(YES == (CheckComboRdrPresent(COMBO_READER_ATOM_RD100)) && (DcInfo.respPort == EXTERNAL_PORT))
	{
		switch(accessMode)
		{
			case ACCESS_MODE_BLE:
			{
				if(curCredType == CB_READER_BLE_CREDENTIAL)
				{
					retVal = YES;
				}
			}break;

			case CARD_ONLY:
			{
				if(curCredType == CARD_CREDENTIAL)
				{
					retVal = YES;
				}
			}break;

			case CARD_PIN: // Card + Pin
			{
				if((curCredType == CARD_CREDENTIAL) || (curCredType == PIN_CREDENTIAL))
				{
					// first credential, then order of card/pin don't matter
					if(credentialCnt == FIRST_CREDENTIAL)
					{
						retVal = YES;
					}
					else if(credentialCnt == SECOND_CREDENTIAL)
					{
						//First and second credential should not be same
						if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
						{
							retVal = YES;
						}
					}
				}
			}break;

			case ANY_CREDENTIAL:
			{
				retVal = YES;
			}break;

			default:
				break;
		}
		return retVal;
	}

	if((EXTERNAL_PORT == DcInfo.respPort) && (YES == CheckComboRdrPresent(COMBO_READER_ATOM_RD200)))
	{
		accessModeApplicable = NO;

		switch(accessMode)
		{
			case ACCESS_MODE_BLE:
			{
				if(CB_READER_BLE_CREDENTIAL == curCredType)
				{
					return YES;
				}
			}break;

			case ANY_CREDENTIAL:
			case CARD_ONLY:
			case ACCESS_MODE_CARD_THEN_BIOMETRIC:
			case CARD_FINGER_PRINT:
			case FINGER_PRINT_ONLY:
			case FINGER_PRINT_THEN_CARD:
			{
				accessModeApplicable = YES;
			}break;

			default:
				break;
		}
		// Access mode is not applicable so return from here
		if(NO == accessModeApplicable)
		{
			return accessModeApplicable;
		}
	}

	if((EXTERNAL_PORT == DcInfo.respPort) && (YES == CheckComboRdrPresent(COMBO_READER_ATOM_RD300)))
	{
		accessModeApplicable = NO;

		switch(accessMode)
		{
			case ACCESS_MODE_BLE:
			{
				if(CB_READER_BLE_CREDENTIAL == curCredType)
				{
					return YES;
				}
			}break;

			case ANY_CREDENTIAL:
			case CARD_ONLY:
			case ACCESS_MODE_CARD_THEN_BIOMETRIC:
			case CARD_FINGER_PRINT:
			case FINGER_PRINT_ONLY:
			case FINGER_PRINT_THEN_CARD:
			case CARD_PIN:
			case CARD_FINGER_PRINT_PIN:
			case FINGER_PRINT_PIN:
			{
				accessModeApplicable = YES;
			}break;

			default:
				break;
		}
		// Access mode is not applicable so return from here
		if(NO == accessModeApplicable)
		{
			return accessModeApplicable;
		}
	}

	DPRINT(DC_LOG, "accessMode %u curCredType %u credentialCnt %u", accessMode, curCredType, credentialCnt);
	switch(accessMode)
	{
		case ANY_CREDENTIAL:	//Any credential is valid for the zone
		{
			retVal = YES;
		}
		break;

		case FINGER_PRINT_ONLY:
		{
			// if credential is first or bypass enabled then authenticate it
			if( (credentialCnt == FIRST_CREDENTIAL) || (IsFingerBypass(userIdx) == TRUE) || (DcInfo.bypassBioCred == TRUE) )
			{
				retVal = YES;
			}
			else if(credentialCnt == SECOND_CREDENTIAL)
			{
				// if first and second credential is different and second credential is finger
				// then validate it else reject the credential.
				if( (curCredType != DcInfo.credType[FIRST_CREDENTIAL]) &&
					(DcInfo.credType[SECOND_CREDENTIAL] == FP_CREDENTIAL) )
				{
					retVal = YES;
				}
			}
		}
		break;

		case CARD_ONLY:			//Only cards are allowed
		{
 			if(curCredType == CARD_CREDENTIAL)
			{
 				retVal = YES;
			}
		}
		break;

		case CARD_PIN:		//Card and PIN are required
		{
			if( (curCredType == CARD_CREDENTIAL) || (curCredType == PIN_CREDENTIAL) )
			{
				// first credential, then order of card/pin don't matter
				if(credentialCnt == FIRST_CREDENTIAL)
				{
					retVal = YES;
				}
				else if(credentialCnt == SECOND_CREDENTIAL)
				{
					//First and second credential should not be same
					if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
					{
						retVal = YES;
					}
				}
			}
		}
		break;

		case FINGER_PRINT_PIN:  // Finger print and PIN are required
		{
			if( (curCredType == FP_CREDENTIAL) || (curCredType == PIN_CREDENTIAL) )
			{
				// first credential, then order of FP/PIN don't matter
				if(credentialCnt == FIRST_CREDENTIAL)
				{
						retVal = YES;
				}
				else if(credentialCnt == SECOND_CREDENTIAL)
				{
					//Both credential should not be same
					if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
					{
						retVal = YES;
					}
				}
			}
			else if(credentialCnt == SECOND_CREDENTIAL)
			{
				if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
				{
					if( (twoCredMstF == TRUE) &&
							( (DcInfo.credType[FIRST_CREDENTIAL] == PIN_CREDENTIAL) &&
								( (IsFingerBypass(userIdx) == TRUE) || (DcInfo.bypassBioCred == TRUE) ) ) )
					{
						retVal = YES;
					}
				}
			}
		}
		break;

		case CARD_FINGER_PRINT:		// Card and FP are required
		{
			if( (curCredType == FP_CREDENTIAL) || (curCredType == CARD_CREDENTIAL) )
			{
				//first credential, then order of Card/FP don't matter
				if(credentialCnt == FIRST_CREDENTIAL)
				{
					retVal = YES;
				}
				else if(credentialCnt == SECOND_CREDENTIAL)
				{
					if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
					{
						retVal = YES;
					}
				}
			}
			else if(credentialCnt == SECOND_CREDENTIAL)
			{
				if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
				{
					if( (twoCredMstF == TRUE) &&
							( (DcInfo.credType[FIRST_CREDENTIAL] == CARD_CREDENTIAL) &&
								( (IsFingerBypass(userIdx) == TRUE) || (DcInfo.bypassBioCred == TRUE) ) ) )
					{
						retVal = YES;
					}
				}
			}
		}
		break;

		case FINGER_PRINT_THEN_CARD:		// Card and FP are required
		{
			if( (curCredType == CARD_CREDENTIAL) || (curCredType == FP_CREDENTIAL) )
			{
				if(credentialCnt == FIRST_CREDENTIAL)
				{
					if( (IsFingerBypass(userIdx) == TRUE) || (DcInfo.bypassBioCred == TRUE)
							|| (curCredType == FP_CREDENTIAL) )
					{
						retVal = YES;
					}
				}
				else if(credentialCnt == SECOND_CREDENTIAL)
				{
					if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
					{
						retVal = YES;
					}
				}
			}
			else if ( (credentialCnt == SECOND_CREDENTIAL) &&  (DcInfo.credType[FIRST_CREDENTIAL] == CARD_CREDENTIAL) )
			{
				if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
				{
					retVal = YES;
				}
			}
		}
		break;

		case CARD_FINGER_PRINT_PIN:		// Card + FP + PIN required
		{
			if( (curCredType == CARD_CREDENTIAL) || (curCredType == FP_CREDENTIAL) || (curCredType == PIN_CREDENTIAL) )
			{
	 			// Order in which credential is presented dont'matter
				// So any credential is valid as first
				if(credentialCnt == FIRST_CREDENTIAL)
				{
					retVal = YES;
				}
				else if(credentialCnt == SECOND_CREDENTIAL)
				{
					//first and second credential should not be same
					if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
					{
						retVal = YES;
					}
				}
				else if(credentialCnt == THIRD_CREDENTIAL)
				{
					//credential should not be the same as first and second
					if( (curCredType != DcInfo.credType[FIRST_CREDENTIAL]) &&
						(curCredType != DcInfo.credType[SECOND_CREDENTIAL]) )
					{
						retVal = YES;
					}
				}
			}
		}
		break;

		case ACCESS_MODE_FACE:
		{
			// if credential is first or bypass enabled then authenticate it
			if(credentialCnt == FIRST_CREDENTIAL)
			{
				retVal = YES;
			}
			else if(credentialCnt == SECOND_CREDENTIAL)
			{
				// if first and second credential is different and second credential is Face
				// then validate it else reject the credential.
				if( (curCredType != DcInfo.credType[FIRST_CREDENTIAL]) &&
						(DcInfo.credType[SECOND_CREDENTIAL] == FACE_CREDENTIAL) )
				{
					retVal = YES;
				}
			}
		}
		break;


		case ACCESS_MODE_FACE_CARD:
		{
			if( (curCredType == FACE_CREDENTIAL) || (curCredType == CARD_CREDENTIAL) )
			{
				//first credential, then order of Card/Face don't matter
				if(credentialCnt == FIRST_CREDENTIAL)
				{
					retVal = YES;
				}
				else if(credentialCnt == SECOND_CREDENTIAL)
				{
					if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
					{
						retVal = YES;
					}
				}
			}
		}
		break;

		case ACCESS_MODE_FACE_PIN:
		{
			if( (curCredType == FACE_CREDENTIAL) || (curCredType == PIN_CREDENTIAL) )
			{
				// first credential, then order of Face/PIN don't matter
				if(credentialCnt == FIRST_CREDENTIAL)
				{
						retVal = YES;
				}
				else if(credentialCnt == SECOND_CREDENTIAL)
				{
					//Both credential should not be same
					if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
					{
						retVal = YES;
					}
				}
			}
		}
		break;

		case ACCESS_MODE_FACE_FINGER_PRINT:
		{
			if( (curCredType == FACE_CREDENTIAL) || (curCredType == FP_CREDENTIAL) )
			{
				// first credential, then order of Face/FP don't matter
				if(credentialCnt == FIRST_CREDENTIAL)
				{
						retVal = YES;
				}
				else if(credentialCnt == SECOND_CREDENTIAL)
				{
					//Both credential should not be same
					if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
					{
						retVal = YES;
					}
				}
			}
			else if(credentialCnt == SECOND_CREDENTIAL)
			{
				if(curCredType != DcInfo.credType[FIRST_CREDENTIAL])
				{
					if( (twoCredMstF == TRUE) &&
							( (DcInfo.credType[FIRST_CREDENTIAL] == FACE_CREDENTIAL) &&
								( (IsFingerBypass(userIdx) == TRUE) || (DcInfo.bypassBioCred == TRUE) ) ) )
					{
						retVal = YES;
					}
				}
			}
		}
		break;

		case ACCESS_MODE_NONE:	// Any credential is not valid.
		{
			retVal = NO;
		}
		break;

		case ACCESS_MODE_CARD_THEN_BIOMETRIC:
		{
			if(credentialCnt == FIRST_CREDENTIAL)
			{
				if(curCredType == CARD_CREDENTIAL)
				{
					retVal = YES;
				}
			}
			else if(credentialCnt == SECOND_CREDENTIAL)
			{
				if( (IsFingerBypass(userIdx) == TRUE) || (DcInfo.bypassBioCred == TRUE)
						|| (curCredType == FP_CREDENTIAL) )
				{
					retVal = YES;
				}
			}
		}
		break;

		default:
		{
			break;
		}
	}

	// we come here only if credential is not valid as per ZAM
	return retVal;
}

//*****************************************************************************
//	GetCredentialCnt
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	count	-	Number of credentials required for ZAM
//	Description:
//			This function figures out number of User's credential required based
//			on ZAM policy for the doorId. If user presenting credential has
//			bypass FP enabled, credential count will be adjusted accordingly.
//				
//*****************************************************************************
UINT8 GetCredentialCnt(void)
{
	UINT16					userIdx;
	CREDENTIAL_TYPE_e		curCredType;
	ACCESS_MODE_e			accessMode;
	UINT8					retVal = 0;
	BOOL					twoCredMstF = FALSE;
	
	userIdx = DcInfo.userIdx;
	curCredType = DcInfo.credType[( (DcInfo.credCnt) - 1)];

	if(DcInfo.procSSAF == TRUE)
	{
		accessMode = SysAdvPara.SSAConfig.accessMode;
	}
	else if(DcInfo.apiUser == SET)
	{
		API_ACCESS_MODE_e apiAccessMode;

		if(DcInfo.readerMode == ENTRY_READER)
		{
			apiAccessMode = SysBasicPara.apiEntryAccessMode;
		}
		else
		{
			apiAccessMode = SysBasicPara.apiExitAccessMode;
		}

		switch(apiAccessMode)
		{
			default:
			case ACCESS_MODE_QR_CODE:
				retVal = 1;
				break;
				
			case ACCESS_MODE_QR_CODE_THEN_BIO:
			{	
				if(((GetDoorApplType() == DIRECT_DOOR_APPL) && (SysAdvPara.twoCredMustModeF == DISABLED) && (IsFingerBypass(userIdx) == TRUE)) 
							 || ((GetDoorApplType() == PANEL_DOOR_APPL) && (ADVANCE_DEGRADE_MODE_TYPE == SysBasicPara.degradeModeType) && (CommState == SLAVE_DEGRADE) && (IsFingerBypass(userIdx) == TRUE)))
				{
					retVal = 1;
					DPRINT(DC_LOG,"ACCESS_MODE_QR_CODE_THEN_BIO IF door App Type[%u] ByPassFinger=[%u]",GetDoorApplType(),IsFingerBypass(userIdx));
				}
				else
				{
					retVal = 2;
					DPRINT(DC_LOG,"ACCESS_MODE_QR_CODE_THEN_BIO  door App Type[%u] ByPassFinger=[%u] or Two cred flag[%u]",GetDoorApplType()
									 ,IsFingerBypass(userIdx),SysAdvPara.twoCredMustModeF);
				}
			}break;
			
			case ACCESS_MODE_QR_CODE_THEN_CARD:
			case ACCESS_MODE_MOBILE_THEN_FACE:
				retVal = 2;
				break;

			case ACCESS_MODE_MOBILE_THEN_PIN:
			{
				// If Pin already verified(by mode device) no need to verify again
				if((BLE_QR_CREDENTIAL == curCredType) && (SET == DcInfo.pinVerified))
				{
					retVal = 1;
				}
				else
				{
					retVal = 2;
				}

			}break;
		}
		return(retVal);
	}
	else
	{
		accessMode = GetAccessModeFromRespPort(DcInfo.userType, DcInfo.respPort);
		twoCredMstF = SysAdvPara.twoCredMustModeF;
	}

	// based on ZAM mode, determine number of user credential needed on the door
	switch(accessMode)
	{
		case ANY_CREDENTIAL:
		case CARD_ONLY:
		case ACCESS_MODE_BLE:
		{
			retVal = 1;
			break;
		}
		case FINGER_PRINT_ONLY:
		{
			// Even if bypass FP is enabled and user presents FP, next credential is expected.
			if( (curCredType == FP_CREDENTIAL) || (IsFingerBypass(userIdx) == TRUE) || (DcInfo.bypassBioCred == TRUE) )
			{
				retVal = 1;
			}
			else
			{
				retVal = 2;
			}

			DPRINT(SYS_LOG, "reqd. credCnt = [%u], credCnt[%u]",retVal, DcInfo.credCnt);
			break;
		}

		case CARD_PIN:
		{
			retVal = 2;
			break;
		}

		case FINGER_PRINT_PIN:
		case CARD_FINGER_PRINT:
		case FINGER_PRINT_THEN_CARD:
		case ACCESS_MODE_FACE_FINGER_PRINT:
		case ACCESS_MODE_CARD_THEN_BIOMETRIC:
		{
			DPRINT(DC_LOG, "UserConfig.bypassFinger = %u, twoCredMstF: %u, curCredType: %u", IsFingerBypass(userIdx), twoCredMstF, curCredType);

			if( (IsFingerBypass(userIdx) == ENABLED) || (DcInfo.bypassBioCred == TRUE) )
			{
				// Even if bypass FP is enabled and user presents FP, next credential is expected.
				if( (curCredType == FP_CREDENTIAL) || (twoCredMstF == TRUE) )
				{
					retVal = 2;
				}
				else
				{
					retVal = 1;
				}
			}
			else
			{
				retVal = 2;
			}
		}
		break;

		case ACCESS_MODE_FACE:
		{
			// Even if bypass FP is enabled and user presents FP, next credential is expected.
			if(curCredType == FACE_CREDENTIAL)
			{
				retVal = 1;
			}
			else
			{
				retVal = 2;
			}

			DPRINT(SYS_LOG, "reqd. credCnt = [%u], credCnt[%u]",retVal, DcInfo.credCnt);
		}
		break;

		case ACCESS_MODE_FACE_CARD:
		case ACCESS_MODE_FACE_PIN:
		{
			retVal = 2;
		}
		break;

		case CARD_FINGER_PRINT_PIN:
		{
			if( (IsFingerBypass(userIdx) == ENABLED) || (DcInfo.bypassBioCred == TRUE) )
			{
				UINT8 	idx;
				BOOL	fpRecvF = FALSE;
				
				// Even if bypass FP is enabled and user presents FP, all credentials are expected.
				for(idx = 0; idx < (MAX_CREDENTIAL_CNT - 1); idx++)
				{
					if(DcInfo.credType[idx] == FP_CREDENTIAL)
					{
						fpRecvF = TRUE;
						break;
					}
				}

				if(fpRecvF == TRUE)
				{
					retVal = 3;
				}
				// we came here because FP is not received yet, so credential required is
				// one less than actual.
				else
				{
					return 2;
				}
			}
			// FP is not bypassed for the user, all credentials are required 
			else
			{
				retVal = 3;
			}
			break;
		}

		default:
		{
			break;
		}
	}

	return retVal;
}

//*****************************************************************************
//	CheckNextCredToProcess
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It checks whether 1:1 finger print verification is required for the
//			user accessing the door. 
//			If  required and FP is present on master database then it instructs 
//			FPR to verify user's template present on FPR with live scan, otherwise
//			when card is detected it instructs card reader to read template from 
//			card and will instruct FPR to match read template from card with live
//			scan. 
//			It should be called after identifying user. 
//				
//*****************************************************************************
void CheckNextCredToProcess(void)
{
	READER_ID_e				bioReaderId;
	UINT8 					idx;
	BOOL					verifyBioF = FALSE;
	BOOL					readFpFromCardF = FALSE;
	BOOL					bioCredRcvdF;
	BOOL					faceReceivedF;
	BOOL					bioReaderConfiguredF = FALSE;
	ACCESS_MODE_e			accessMode;

	/* Reset FR Stream Start Failed Flag */
	isStartFrStreamFailed = FALSE;

	bioReaderConfiguredF = IsReaderPresent(FP_READER, DcInfo.respPort, ANY_READER_MODE, &bioReaderId);

	// check if fp credential is already received
	bioCredRcvdF = FALSE;
	faceReceivedF = FALSE;
	for(idx = 0; idx < (MAX_CREDENTIAL_CNT - 1); idx++)
	{
		if(DcInfo.credType[idx] == FP_CREDENTIAL)
		{
			bioCredRcvdF = TRUE;
			break;
		}
	}

	for(idx = 0; idx < (MAX_CREDENTIAL_CNT - 1); idx++)
	{
		if(DcInfo.credType[idx] == FACE_CREDENTIAL)
		{
			faceReceivedF = TRUE;
			break;
		}
	}

	if(DcInfo.apiUser == SET && bioCredRcvdF == FALSE)
	{
		API_ACCESS_MODE_e apiAccessMode;

		if(DcInfo.readerMode == ENTRY_READER)
		{
			apiAccessMode = SysBasicPara.apiEntryAccessMode;
		}
		else
		{
			apiAccessMode = SysBasicPara.apiExitAccessMode;
		}

		switch(apiAccessMode)
		{
			default:
				break;
			case ACCESS_MODE_QR_CODE_THEN_BIO:
				// If this is first credential, then only we send verify command
				if(PANEL_DOOR_APPL == GetDoorApplType())
				{
					DcInfo.userFpcnt = GetEnrolledUserFp(GetUserId(DcInfo.userIdx));
				}
				else
				{
					DcInfo.userFpcnt = GetEnrolledUserFp(DcInfo.userIdx);
				}
				if(DcInfo.userFpcnt)
				{
					verifyBioF = TRUE;
				}
				break;
		}
	}
	else
	{
		accessMode = GetAccessModeFromRespPort(DcInfo.userType, DcInfo.respPort);

		// Check if FP verification is required
		// 1. FP is not bypassed for this user
		// 2. FP credential hasn't received yet
		// 3. FPR is present

		if( (IsFingerBypass(DcInfo.userIdx) == DISABLED) &&
			(bioCredRcvdF == FALSE) &&
			(DcInfo.bypassBioCred == FALSE) &&
			(bioReaderConfiguredF == YES) )
		{
			// based on ZAM mode, determine if fp verification is required
			switch(accessMode)
			{
				case FINGER_PRINT_ONLY:
				{
					// if user finger print is already enrolled with system, verify user's finger that 
					// is on reader with live finger scan 
					if(PANEL_DOOR_APPL == GetDoorApplType())
					{
						DcInfo.userFpcnt = GetEnrolledUserFp(GetUserId(DcInfo.userIdx));
					}
					else
					{
						DcInfo.userFpcnt = GetEnrolledUserFp(DcInfo.userIdx);
					}

					if(DcInfo.userFpcnt)
					{
						verifyBioF = TRUE;
					}
					else
					{
						if(DcInfo.credType[DcInfo.credCnt - 1] == CARD_CREDENTIAL)
						{
							if(IsSmartCard(DcInfo.cardType) == YES) // read finger template from card and verify it with live scan
							{
								readFpFromCardF = TRUE;// 1:1 Verification: user template v/s live scan
							}
						}
					}
					break;
				}

				case FINGER_PRINT_PIN:		// Fingerprint+ PIN
				{
					// if user finger print is already enrolled with system, verify user's finger that
					// is on reader with live finger scan
					if(PANEL_DOOR_APPL == GetDoorApplType())
					{
						DcInfo.userFpcnt = GetEnrolledUserFp(GetUserId(DcInfo.userIdx));
					}
					else
					{
						DcInfo.userFpcnt = GetEnrolledUserFp(DcInfo.userIdx);
					}
					if(DcInfo.userFpcnt)
					{
						verifyBioF = TRUE;
					}
					break;
				}

				case CARD_FINGER_PRINT:		// Card + Fingerprint
				case ACCESS_MODE_CARD_THEN_BIOMETRIC:
				{
					// if user finger print is already enrolled with system, verify user's finger that is on reader with live scan finger
					if(PANEL_DOOR_APPL == GetDoorApplType())
					{
						DcInfo.userFpcnt = GetEnrolledUserFp(GetUserId(DcInfo.userIdx));
					}
					else
					{
						DcInfo.userFpcnt = GetEnrolledUserFp(DcInfo.userIdx);
					}
					if(DcInfo.userFpcnt)
					{
						verifyBioF = TRUE;
					}
					else // read finger template from card and verify it with live scan
					{
						readFpFromCardF = TRUE; // 1:1 Verification: user template v/s live scan
					}

					break;
				}

				case CARD_FINGER_PRINT_PIN:	// Card + Fingerprint + PIN
				{
					// if user finger print is already enrolled with system, verify user's finger that is on reader with live scan finger
					if(PANEL_DOOR_APPL == GetDoorApplType())
					{
						DcInfo.userFpcnt = GetEnrolledUserFp(GetUserId(DcInfo.userIdx));
					}
					else
					{
						DcInfo.userFpcnt = GetEnrolledUserFp(DcInfo.userIdx);
					}
					if(DcInfo.userFpcnt)
					{
						// Trigger user FP verification only once when first credential is received
						// user must give his inputs within ZAM time or scanning time whichever is less
						if( ((DcInfo.credCnt)-1) == FIRST_CREDENTIAL)
							verifyBioF = TRUE;
					}
					// If received credential is card then read finger template from it
					else if(DcInfo.credType[(DcInfo.credCnt)-1] == CARD_CREDENTIAL)
					{
						readFpFromCardF = TRUE; 
					}
					break;
				}

				case ACCESS_MODE_FACE_FINGER_PRINT:
				{
					DcInfo.userFpcnt = GetEnrolledUserFp(DcInfo.userIdx);
					if(DcInfo.userFpcnt)
					{
						verifyBioF = TRUE;
					}
					break;
				}
				default:
					break;
			}
		}

		if(faceReceivedF == FALSE)
		{
			switch(accessMode)
			{
				case ACCESS_MODE_FACE:
				case ACCESS_MODE_FACE_CARD:
				case ACCESS_MODE_FACE_PIN:
				case ACCESS_MODE_FACE_FINGER_PRINT:
				{
					DPRINT(SYS_LOG,"Next Face Required And Implement as per sad");
					if (StartFRStreamingOnDeviceIdleScreen() == SUCCESS)
					{
						StreamPayload_t         streamInfo;
						streamInfo.reason = START_STREAM_NEXT_FACE_REQ;
						streamInfo.captureMode = STREAM_CAPTURE_FACE;
						StartWaitForFrStreamTimer(streamInfo);
// 						LoadDisplayFRStreamToIdleScreenTimer(FRServerConfig.identificationTimeOut,TRUE);
// 
// 						/* Set the Overlay info to be sent to QT */
// 						SetFRStreamOverlayInfo(TRUE, MSG_SHOW_2ND_CREDENTIAL_552, TRUE, FOOTER_PIN_CENTER, TRUE);
// 						ShowFRStreamOnDeviceDisplayTimer(ACCESS_WAIT_RESP);
					}
					else
					{
						/* Start FR stream failed due to no camera connectivity */
						isStartFrStreamFailed = TRUE;
					}
					return;
				}
					break;
				default:
					break;
			}
		}
	}

	// Pass finger template from device database for given user to reader to match with scanned finger.
	// If system do not have finger enrolled for user then,
	// Pass user id to reader to match finger template from reader database for given user with scanned finger.
	if(verifyBioF == TRUE)
	{
		DPRINT(DC_LOG, "Required To Check FP Count of User [%s]", GetAlphaNumId(DcInfo.userIdx));

		// Get Count of user's FP which are on FP Module
		USER_TEMPLATE_CNT_CMD_DATA_t cmdData;

		//TODO:- ReaderId is Hardcoded remove it
		cmdData.readerId = bioReaderId;
		cmdData.fprUserId = GetUserId(DcInfo.userIdx);

		ProcessReaderCommand(MSG_USER_TEMPLATE_CNT_CMD, (UINT8PTR)&cmdData, sizeof(USER_TEMPLATE_CNT_CMD_DATA_t));
	}
	// read finger template from card and verify it with live scan
	else if(readFpFromCardF == TRUE)
	{
		// SEND command READ_FP from card
		READ_FP_CMD_DATA_t	readFpFromCard;

		DPRINT(DC_LOG, "Required to read FP Template from Card for User [%s] ",GetAlphaNumId(DcInfo.userIdx));

		readFpFromCard.readerId = DcInfo.readerId;
		readFpFromCard.cardType = DcInfo.cardType;
		readFpFromCard.template1F = SET;
		readFpFromCard.template2F = SET;
		ProcessReaderCommand(MSG_READ_FP_CMD, &readFpFromCard, sizeof(READ_FP_CMD_DATA_t));

		// Indicate user that we are processing his access request
		SetAudioVisual(USER_RESP_READING_FROM_CARD, DcInfo.respPort, NULL);
	}
	else // no verification is required, ask for next credential
	{
		if( (accessMode == FINGER_PRINT_ONLY ) && (DcInfo.apiUser == CLEAR) && (DcInfo.cbReaderBleAccess == CLEAR))
		{
			SetAudioVisual(USER_RESP_PLACE_FINGER_BIO_ONLY_MODE, DcInfo.respPort, NULL);
		}

		// Set audio visual for next credential
		else if(DcInfo.credCnt == SECOND_CREDENTIAL)
		{
			if(IsWaitingForAcsBleCredential() == NO)
			{
				// Update audio/visual to prompt for more credentials
				SetAudioVisual(USER_RESP_SHOW_2ND_CREDENTIAL, DcInfo.respPort, NULL);
			}
		}

		//For QR CODE API User 3rd Credential will not require
		else if((DcInfo.apiUser == CLEAR) && (DcInfo.cbReaderBleAccess == CLEAR))
		{
			// Update audio/visual to prompt for more credentials
			SetAudioVisual(USER_RESP_SHOW_3RD_CREDENTIAL, DcInfo.respPort, NULL);
		}
	}
}

//*****************************************************************************
//	GetAccessModeFromRespPort
//	Param:
//		IN :	userType, response port
//		OUT:	none
//	Returns:	access mode
//	Description:
//			It will return access mode as per response port
//			For Internal port - access mode is as per door access mode
//			For External port - access mode is as per exit reader acess mode
//*****************************************************************************
ACCESS_MODE_e GetAccessModeFromRespPort(USER_TYPE_e userType, RESPONSE_PORT_e respPort)
{
	ACCESS_MODE_e accessMode;

	if(userType == VISITOR_USER)
	{
		if(respPort == EXTERNAL_PORT)
		{
			accessMode = SysBasicPara.visitorExtRdrAccessMode;
		}
		else
		{
			accessMode = SysBasicPara.visitorDoorAccessMode;
		}
	}
	else
	{
		if(respPort == EXTERNAL_PORT)
		{
			accessMode = SysBasicPara.extRdrAccessMode;
		}
		else
		{
			accessMode = SysBasicPara.doorAccessMode;
		}
	}
	return accessMode;
}

/**************************************************************************************************
IsStartFrStreamForFaceCredFailed
**************************************************************************************************/
/**
@brief          This function will give FR stream status for Face Credential
@param[in]      None
@return         TRUE if stream start failed else False
**************************************************************************************************/
BOOL IsStartFrStreamForFaceCredFailed(VOID)
{
	return (isStartFrStreamFailed);
}
