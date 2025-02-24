//////////////////////////////////////////////////////////////////////////
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
//   Created By   : Ankit Sohaliya
//   File         : MxSSA.c
//   Description  : This file contains Smart secure access related functionality.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "MxTypedef.h"
#include "MxSysTimer.h"
#include "MxConfig.h"
#include "MxDcTask.h"
#include "MxSlaveMsg.h"
#include "MxEventLog.h"
#include "MxAvResp.h"
#include "MxIOLink.h"
#include "MxLogs.h"
#include "MxSSA.h"
#include "MxDcSpecialFunc.h"
#include "MxDvrIntegration.h"
#include "MxSysUtils.h"
#include "MxWiegandOutput.h"
#include "MxSmartCardFormatConfig.h"
#include "MxReaderGuiMessage.h"
#include "MxUserConfig.h"

//******** Extern Variables **********
extern SYS_ADVANCE_PARA_t	SysAdvPara;
extern SYS_BASIC_PARA_t		SysBasicPara;
extern const CHARPTR		ReaderRespStatusStr[];
extern const USER_CONFIG_t  DfltUserConfig;
extern DOOR_ACTIVITY_INFO_t	DcInfo;

//******** Defines and Data Types ****

//******** Global Variables **********
READ_SSA_INFO_RESP_DATA_t * SSADataPtr;

//******** Function Prototypes *******
static void ProcessUserForSSA(void);
static BOOL StoreSSAData(void);

//******** Function Definations ******
//*****************************************************************************
//	DcSSAStateFn()
//	Param:
//		IN : doorId - Index of Door controller
//		OUT: None
//	Returns:
//		NONE
//	Description:
//		This function processes DC related events in DC SSA state.
//
//*****************************************************************************
void DcSSAStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	switch(msgId)
	{
		default:
		{
			// Events that are not handled in this state are passed to default message handler .
			DcDefaultMsgHandler(msgId, dataPtr, length);
			break;
		}

		case MSG_READ_SSA_INFO_RESP:
		{
			SSADataPtr = dataPtr;

			DPRINT(DC_LOG,"MSG_READ_SSA_INFO_RESP: status: %s", ReaderRespStatusStr[SSADataPtr->status]);
			if(SSADataPtr->status == READER_RESP_STATUS_SUCCESS)
			{
				if(StoreSSAData() == SUCCESS)
				{
					READ_PERS_INFO_CMD_DATA_t	readPersInfo;
					readPersInfo.cardType = DcInfo.cardType;
					readPersInfo.readerId = DcInfo.readerId;
					readPersInfo.userNameF = SET;
					readPersInfo.bloodGrpF = SET;
					readPersInfo.branchF = SET;
					readPersInfo.departmentF = SET;
					readPersInfo.emrgncyContactF = SET;
					readPersInfo.designationF = SET;
					readPersInfo.medicalHistoryF = SET;
					LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);
					ProcessReaderCommand(MSG_READ_PERS_INFO_CMD, &readPersInfo,	sizeof(READ_PERS_INFO_CMD_DATA_t));
					SetAudioVisual(USER_RESP_READING_FROM_CARD, DcInfo.respPort, NULL);
				}
				else
				{
					DcInfo.authReason = AUTH_NO_ACCESS;
					DenyUser(UNKNOWN_USER_IDX ,DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				}
			}
			else
			{
				DcInfo.authReason = AUTH_CARD_READ_FAILED;
				DenyUser(UNKNOWN_USER_IDX ,DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}
		case MSG_READ_PERS_INFO_RESP:
		{
			READ_PERS_INFO_RESP_DATA_t * respDataPtr = (READ_PERS_INFO_RESP_DATA_t *)dataPtr;

			DPRINT(DC_LOG, "MSG_READ_PERS_INFO_RESP: status: %s", ReaderRespStatusStr[respDataPtr -> status]);
			if(respDataPtr -> status == READER_RESP_STATUS_SUCCESS)
			{
				if(respDataPtr->designationF == TRUE)
				{
					snprintf(DcInfo.designation, DESIGNATION_SIZE, "%s", respDataPtr->designation);
				}
			}
			ProcessUserForSSA();
			break;
		}
		case GUI_ALNUM_KEEP_ALIVE:
		case GUI_BUTTON_FIELD_CLEAR:
		{
			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;

			// Is user entering PIN then only reload timer
			if(DcInfo.interDigitTmrHandle[pinRespPort] != INVALID_TIMER_HANDLE)
			{
				if(pinRespPort == INTERNAL_PORT)
				{
					LoadInterDigitTimer();
					// Reload display idle timer
					SetLcdIdleTimer(SysAdvPara.interDigitTimer);
				}
				else if(pinRespPort == EXTERNAL_PORT)
				{
					LoadInterDigitTimerForExternal();
					SetReaderLcdIdleTimer(SysAdvPara.interDigitTimer);
				}
			}
			break;
		}

		case GUI_BUTTON_CANCEL:
		case GUI_BUTTON_HOME:
		{
			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;
			DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
			if((pinRespPort == INTERNAL_PORT) || ((pinRespPort == EXTERNAL_PORT) && (NO == CheckComboRdrPresent(COMBO_READER_ATOM_RD100) ) ))
			{
			    SetAudioVisual(USER_RESP_IDLE, pinRespPort, NULL);
			}
			SwitchDcToIdleState();
			break;

		}
		
		case GUI_BUTTON_PIN_ENTER_SCREEN:
		{
			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;

			if(PinEntryPossible(pinRespPort) == YES)
			{
				// Set Response Port and Reader Mode
				DcInfo.respPort = pinRespPort;

				if(DcInfo.respPort == INTERNAL_PORT)
				{
					UpdateDcInfoReaderMode(READER1);
					LoadInterDigitTimer();
					// Reload display idle timer
					SetLcdIdleTimer(SysAdvPara.interDigitTimer);
				}
				else
				{
					UpdateDcInfoReaderMode(READER3);
					LoadInterDigitTimerForExternal();
					SetReaderLcdIdleTimer(SysAdvPara.interDigitTimer);
				}
				// Delete timer used to get user input for special function to process single user
				DeleteTimer(&(DcInfo.splFnGetUserInputTmrHandle));
				SetAudioVisual(USER_RESP_ENTER_PIN, pinRespPort, NULL);
			}
			break;
		}

		case MSG_PIN_VALIDATION_EVENT :
		{
			PIN_RESULT_e	pinResp;

			DeleteTimer(&(DcInfo.interDigitTmrHandle[INTERNAL_PORT]));
			DeleteTimer(&(DcInfo.multiAccessTmrHandle));

			// Process key pressed to check for user's PIN
			// NOTE: If enter is pressed as first key, user will be denied
			pinResp = ProcessPinDetect(((PIN_DATA_t *)dataPtr));

			if(pinResp == PIN_MATCH)
			{
 				//Valid user
				SetAudioVisual(USER_RESP_PROCESS, DcInfo.respPort, NULL);
				// Process user for ZAM
				ProcessSSAAddlCred();
			}
			else if(pinResp == PIN_INVALID) // PIN does not match any user
			{
 				DcInfo.authReason = AUTH_INVALID_PIN;
 				DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case MSG_SCAN_TEMPLATE_RESP:
		case MSG_FP_DETECTED_EVENT:
		{
			FP_RESULT_e	fpResp;

			// Process Finger Print detected to find a match
			if(msgId == MSG_SCAN_TEMPLATE_RESP)
			{
				fpResp = ProcessFpScanTemplateResp(dataPtr);
			}
			else
			{
				fpResp = ProcessFpDetect((FP_DETECTED_EV_DATA_t *)dataPtr);
			}
			if(fpResp == FP_MATCH)
			{
				//Valid user
				SetAudioVisual(USER_RESP_PROCESS, DcInfo.respPort, NULL);
				// Process user for ZAM
				ProcessSSAAddlCred();
			}
			// No match found or Timeout for identification
			else if( (fpResp == FP_INVALID) || (fpResp == FP_MATCHING_TIMEOUT) )
			{
				if(fpResp == FP_MATCHING_TIMEOUT)
				{
					DcInfo.authReason = AUTH_FP_MATCHING_TIMEOUT;
				}
				else
				{
					//In SSA, if finger is not configured as one of the credential
					//Hence it is considered as INVALID_INPUT
					DcInfo.authReason = AUTH_INVALID_INPUT;
					// Store current credential as invalid
					StoreRcvdCred(INVALID_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
				}
				DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case MSG_CARD_DETECTED_EVENT:
		{
			CARD_RESULT_e	cardResp;
			CREDENTIAL_TYPE_e credType = CARD_CREDENTIAL;

			// Process card detected to find a match
			cardResp = ProcessCardDetect((CARD_DETECTED_EV_DATA_t *)dataPtr,credType);
			if(cardResp == USER_CARD_MATCH)
			{
				//Valid user
				SetAudioVisual(USER_RESP_PROCESS, DcInfo.respPort, NULL);
				// Process user for ZAM
				ProcessSSAAddlCred();
			}
			else if( (cardResp == CARD_INVALID) || (cardResp == SP_FUNC_CARD_MATCH) )
			{
				//NOTE: In SSA, Card is not expected as 2nd or 3rd credential.
				//Hence it is considered as INVALID_INPUT
				DcInfo.authReason = AUTH_INVALID_INPUT;
				// Store current credential as invalid
				StoreRcvdCred(INVALID_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
				DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		// for FP 1:1 verification (provided template v/s live scanned finger)
		case MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_RESP:
		{
			VERIFY_HOST_TEMPLATE_BY_SCAN_RESP_DATA_t * verifyByScanResp;

			// Get entire message from the Queue
			verifyByScanResp = (VOIDPTR)dataPtr;
			DcInfo.verifyingFpByScanF = CLEAR;	// fp scanning is done

			DPRINT(DC_LOG, "rcvd MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_RESP, status = %s",
									ReaderRespStatusStr[verifyByScanResp -> status]);

			if(verifyByScanResp -> status == READER_RESP_STATUS_SUCCESS)
			{
				// Set the credential type and count
				StoreRcvdCred(FP_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
				//Valid user
				SetAudioVisual(USER_RESP_PROCESS, DcInfo.respPort, NULL);
				// Process user for ZAM
				ProcessSSAAddlCred();
			}
			// if finger scan is succesful tell user to remove his finger
			else if(verifyByScanResp -> status == READER_RESP_STATUS_SCAN_SUCCESS)
			{
				SetAudioVisual(USER_RESP_REMOVE_FINGER, DcInfo.respPort, NULL);
				// extend time to wait for getting verification response completely
				LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);
			}
			// in case of 1:1 matching time out will only occur if user wont place finger
			// ignore timeout response of reader
			else if(verifyByScanResp -> status != READER_RESP_STATUS_TIME_OUT)
			{
				DcInfo.authReason = AUTH_INVALID_FINGER;
				// Set the credential type and count
				StoreRcvdCred(FP_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
				DenyUser(DcInfo.userIdx,DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		// response for read finger print template from card
		case MSG_READ_FP_RESP:
		{
			READ_FP_RESP_DATA_t	* readFpFromCard;

			// Get entire message from the Queue
			readFpFromCard = (VOIDPTR)dataPtr;
			DPRINT(DC_LOG, "rcvd MSG_READ_FP_RESP, status = %s",
								ReaderRespStatusStr[readFpFromCard -> status]);

			// If template is read successfully
			if(readFpFromCard -> status == READER_RESP_STATUS_SUCCESS)
			{
				if( (readFpFromCard -> template1F == SET) || (readFpFromCard -> template2F == SET) )
				{
					READER_ID_e fpReaderId;

					if(IsReaderPresent(FP_READER, DcInfo.respPort, ANY_READER_MODE, &fpReaderId) == YES)
					{
						DPRINT(DC_LOG, "Read fp from card successfully, verify host template by scan, 1:1 matching");
						// SEND command VERIFY_BY_SCAN to finger print reader
						VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t	verifyByScan;
						verifyByScan.readerId = fpReaderId;
						// set no. of templates
						verifyByScan.noOfTemplates = 0;
						if(readFpFromCard -> template1F == SET)
							verifyByScan.noOfTemplates++;
						if(readFpFromCard -> template2F == SET)
							verifyByScan.noOfTemplates++;

						verifyByScan.responseTime = 0;
						// copy template data
						if( (readFpFromCard -> template1F == SET) && (readFpFromCard -> template2F == SET) )
							memcpy(verifyByScan.template , readFpFromCard -> template, MAX_SIZE_DOUBLE_SFM_TEMPLATES);
						else
							memcpy(verifyByScan.template, readFpFromCard -> template, MAX_SIZE_SINGLE_SFM_TEMPLATE);

						SendVerifyHostTemplateByScanCmd(verifyByScan);
							// set flag- system is waiting for user finger to be scanned
						DcInfo.verifyingFpByScanF = SET;
					}
				}

				// Update audio/visual to prompt for more credentials
				if(DcInfo.credCnt == SECOND_CREDENTIAL)
				{
					// Update audio/visual to prompt for more credentials
					SetAudioVisual(USER_RESP_SHOW_2ND_CREDENTIAL, DcInfo.respPort, NULL);
				}
				else
				{
					// Update audio/visual to prompt for more credentials
					SetAudioVisual(USER_RESP_SHOW_3RD_CREDENTIAL, DcInfo.respPort, NULL);
				}
				LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);
			}
			else
			{
				DcInfo.authReason = AUTH_CARD_READ_FAILED;
				DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		//Pin Detected from External Reader
		case MSG_PIN_DETECTED_ON_EXTERNAL_READER_EVENT:
		{
			PIN_RESULT_e pinResp;

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetectOnExternalReader((READER_PIN_DATA_t *)dataPtr);
			if(pinResp == PIN_MATCH)
			{
				//Valid user
				SetAudioVisual(USER_RESP_PROCESS, DcInfo.respPort, NULL);
				// Process user for ZAM
				ProcessSSAAddlCred();
			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}
	}
}

//*****************************************************************************
//	ProcessSSAAddlCred()
//	Param:
//		IN : doorId - Index of Door controller
//		OUT: None
//	Returns:
//		NONE
//	Description:
//		This function processes DC related events in DC SSA state.
//
//*****************************************************************************
void ProcessSSAAddlCred(void)
{
	DeleteTimer(&(DcInfo.multiAccessTmrHandle) );

	//If credential is not valid for ZAM, deny user
	if(IsCredentialValid() == NO)
	{
 		DPRINT(DC_LOG, "User [%s]: credentials not as per access mode", GetAlphaNumId(DcInfo.userIdx));
		DcInfo.authReason = AUTH_INVALID_INPUT;
		DcInfo.zamUserIdx = DcInfo.userIdx;
		// Store current credential as invalid
		StoreRcvdCred(INVALID_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
		DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}

	//Valid credential found

	//Check if more credentials are required for the user
	if(GetCredentialCnt() > DcInfo.credCnt)
	{
		if(CheckForWgndWaitforVerify() == TRUE)
		{
			DPRINT(DC_LOG, "Switch To WIEGAND User Verify Wait Stat");
			SwitchToUserVerifyWaitStat();
		}
		else
		{
			//More credentials are required. Prompt user for additional credential
			CheckSSANextCredToProcess ();
			LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);
		}
		return;
	}

	// All necessary credentials have been verified
	// Process user access request further for door policies
	// delete ZAM timer as it is no logner needed
	DcInfo.authReason = AUTH_ALLOW_SSA_USER;
	CheckFinalSSAPolicyAndAllow();
}

//*****************************************************************************
//	IsSSAValid()
//	Param:
//		IN : doorId - Index of Door controller
//		OUT: None
//	Returns:
//		NONE
//	Description:
//		This function decide, it is suitable to apply smart secure access policy
//*****************************************************************************
BOOL IsSSAValid(void)
{
	if(IsDemoSoftware() == YES)
	{
		return FALSE;
	}

	if(IsSamaySoftware() == ENABLED)
	{
		return FALSE;
	}

	if( (SysAdvPara.SSAConfig.enable == TRUE) &&
		(SysBasicPara.application != CAFETERIA) )
	{
		if(IsCustomSmartCardConfigEnabled(DcInfo.cardType) == YES)
		{
			return TRUE;
		}

		if( (DcInfo.cardType == MIFARE_4K) ||
			(DcInfo.cardType == MIFARE_1K) ||
			(DcInfo.cardType == HID_16K2)  ||
			(DcInfo.cardType == HID_16K16) ||
			(DcInfo.cardType == MIFARE_DESFIRE_2K) ||
			(DcInfo.cardType == MIFARE_DESFIRE_4K) ||
			(DcInfo.cardType == MIFARE_DESFIRE_8K) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

//*****************************************************************************
//	ProcessUserForSSA()
//	Param:
//		IN : doorId
//		OUT: NONE
//	Returns:
//			 None
//	Description:
//			 This function will process user on entry reader.
//
//	Prerequisite:
//			User varification based on smart secure access policy
//
//*****************************************************************************
static void ProcessUserForSSA(void)
{
	// Default auth reason is allowed
	DcInfo.authReason = AUTH_ALLOWED;

	if(SysAdvPara.occupancyCtrl.enable == ENABLED)
	{
		DcInfo.authReason = AUTH_NO_ACCESS;
		DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}

	if(GetAddlSecCodeEnblCfg() == ENABLED)
	{
		if( (DcInfo.addlSecCode != SysAdvPara.addlSecurity.code) || (DcInfo.addlSecCode == 0) )
		{
			DPRINT(DC_LOG,"User [%s] is with invalid ASC card", GetAlphaNumId(DcInfo.userIdx));
			DcInfo.authReason = AUTH_NO_ACCESS;
			DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			return;
		}
	}

	if((IsCustomSmartCardConfigEnabled(DcInfo.cardType) == NO) || (IsGivenFieldForSmartCardConfigured(DcInfo.cardType, CARD_FIELD_VALIDITY) == YES))
	{
		DATE_DDMMYY_t tTerminationDate = GetTerminationDate(DcInfo.userIdx);
		//linux base devices month value starts from 0 in for day count conversion
		//so month value need set for date read from card
		if(tTerminationDate.month != 0)
		{
			tTerminationDate.month--;
			SetTerminationDate(DcInfo.userIdx, tTerminationDate);
		}

		if(IsUserValidityExpired(tTerminationDate) == YES)
		{
			DPRINT(DC_LOG,"User [%s] : Validity Expired", GetAlphaNumId(DcInfo.userIdx));
			DcInfo.authReason = AUTH_VALIDITY_EXPIRED;
			DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			return;
		}
	}

	if(SysBasicPara.application == TA)
	{
		DPRINT(DC_LOG,"User [%s] T_A applicaton", GetAlphaNumId(DcInfo.userIdx));
		DcInfo.authReason = AUTH_ALLOW_SSA_USER;
		CheckFinalSSAPolicyAndAllow();
		return;
	}

	if(IsUserVIP(DcInfo.userIdx) == TRUE)
	{
		// All access policies is checked and user is allowed
		DPRINT(DC_LOG,"User [%s] is VIP user", GetAlphaNumId(DcInfo.userIdx));
		DcInfo.authReason = AUTH_ALLOW_SSA_USER;
		CheckFinalSSAPolicyAndAllow();
		return;
	}

	//If current time is within user's working hour
	if((IsCurrTimeInGivenWindow(SysBasicPara.workingHrStart, SysBasicPara.workingHrStop)) == NO)
	{
		DPRINT(DC_LOG,"User [%s] is not in working hours", GetAlphaNumId(DcInfo.userIdx));
		DcInfo.authReason = AUTH_NO_ACCESS;
		DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}
	else
	{
		DPRINT(DC_LOG,"User [%s] is in working hours", GetAlphaNumId(DcInfo.userIdx));
	}

	if((IsCustomSmartCardConfigEnabled(DcInfo.cardType) == NO) || (IsGivenFieldForSmartCardConfigured(DcInfo.cardType, CARD_FIELD_ACCESS_LEVEL) == YES))
	{
		if(SysAdvPara.SSAConfig.accessLevel > DcInfo.accessLevel)
		{
			DPRINT(DC_LOG,"User [%s] : access level not matched", GetAlphaNumId(DcInfo.userIdx));
			DcInfo.authReason = AUTH_NO_ACCESS;
			DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			return;
		}
	}

	if(CheckSmartRouteFeature() == PROCESS_DONE)
	{
		return;
	}

	if( (DcInfo.readerMode == EXIT_READER) || (SysAdvPara.SSAConfig.accessMode == CARD_ONLY) )
	{
		DcInfo.authReason = AUTH_ALLOW_SSA_USER;
		CheckFinalSSAPolicyAndAllow();
	}
	else
	{
		ProcessSSAAddlCred();
	}
}

//*****************************************************************************
//	CheckSSANextCredToProcess
//	Param:
//		IN :	doorId	- door on which user presented credential
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
void CheckSSANextCredToProcess(void)
{
	READER_ID_e				fpReaderId;
	UINT8 					idx;
	BOOL					readFpFromCardF = FALSE;
	BOOL					fpCredRcvdF;

	// check if fp credential is already received
	fpCredRcvdF = FALSE;
	for(idx = 0; idx < (MAX_CREDENTIAL_CNT - 1); idx++)
	{
		if(DcInfo.credType[idx] == FP_CREDENTIAL)
			fpCredRcvdF = TRUE;
	}

	// Check if FP verification is required
	if( (IsFingerBypass(DcInfo.userIdx) == DISABLED)	// FP is not bypassed for this user
		&& (fpCredRcvdF == FALSE)								// FP credential hasn't received yet
		&& (IsReaderPresent(FP_READER, DcInfo.respPort, ANY_READER_MODE, &fpReaderId) == YES) )
																// FPR is present
	{
		// based on ZAM mode, determine if fp verification is required
		switch(SysAdvPara.SSAConfig.accessMode)
		{
			case FINGER_PRINT_PIN:		// Fingerprint+ PIN
			{
				readFpFromCardF = TRUE;
				break;
			}

			case CARD_FINGER_PRINT:		// Card + Fingerprint
			{
				readFpFromCardF = TRUE; // 1:1 Verification: user template v/s live scan
				break;
			}

			case CARD_FINGER_PRINT_PIN:	// Card + Fingerprint + PIN
			{
				readFpFromCardF = TRUE;
				break;
			}

			default:
				break;
		}
	}

	// read finger template from card and verify it with live scan
	if( (readFpFromCardF == TRUE) && (DcInfo.verifyingFpByScanF != SET) )
	{
		// SEND command READ_FP from card
		READ_FP_CMD_DATA_t	readFpFromCard;

		DPRINT(DC_LOG, "Required 1:1 FP verification, provided template v/s scan");

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
		// Set audio visual for next credential
		if(DcInfo.credCnt == SECOND_CREDENTIAL)
		{
			// Update audio/visual to prompt for more credentials
			SetAudioVisual(USER_RESP_SHOW_2ND_CREDENTIAL, DcInfo.respPort, NULL);
		}
		else
		{
			// Update audio/visual to prompt for more credentials
			SetAudioVisual(USER_RESP_SHOW_3RD_CREDENTIAL, DcInfo.respPort, NULL);
		}
	}
}

//*****************************************************************************
//	StoreSSAData()
//	Param:
//		IN :	readerId - Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length - Length of the message
//		OUT:	None
//	Returns:	FAIL/SUCCESS
//	Description:
//				This function handles events in smart secure access
//				state
//*****************************************************************************
static BOOL StoreSSAData(void)
{
	if(IsCustomSmartCardConfigEnabled(SSADataPtr->cardType) == NO)
	{
		//If this two flags  are clear then return fail otherwise store data
		if( (SSADataPtr -> validityF != SET) || (SSADataPtr -> accessLevelF != SET) )
		{
			DPRINT(DC_LOG, "SSA Card: Validity or Access level not set in card");
			return FAIL;
		}
	}
	else
	{
		if((IsGivenFieldForSmartCardConfigured(SSADataPtr->cardType, CARD_FIELD_VALIDITY) == YES) && (SSADataPtr -> validityF != SET))
		{
			DPRINT(DC_LOG, "SSA Card: Validity not set in card custom mode");
			return FAIL;
		}

		if((IsGivenFieldForSmartCardConfigured(SSADataPtr->cardType, CARD_FIELD_ACCESS_LEVEL) == YES) && (SSADataPtr -> accessLevelF != SET))
		{
			DPRINT(DC_LOG, "SSA Card: Access level not set in card custom mode");
			return FAIL;
		}
	}

	USER_CONFIG_t tUserConfig;
	// Store Default Configuration
	memcpy(&tUserConfig, &DfltUserConfig, sizeof(USER_CONFIG_t));
	DcInfo.userIdx = SSA_USER_IDX;
	tUserConfig.cardId[0] = DcInfo.lastDetectedCard.cardId;
	//Copy SSA data in DcInfo User Config
	if(SSADataPtr -> accessLevelF == SET)
		DcInfo.accessLevel = SSADataPtr -> accessLevel;
	// Store User Id&&
	if(SSADataPtr -> userIdF == SET)
		tUserConfig.userId = SSADataPtr ->userId;

	// Store User Name
	if(SSADataPtr -> userNameF == SET)
		memcpy(tUserConfig.name, SSADataPtr -> userName, CARD_FIELD_LEN_USER_NAME);

	//Store AlphaNum Id
	if(SSADataPtr -> alphaNumIdF == SET)
		memcpy(tUserConfig.alphaNumId, SSADataPtr -> alphaNumId, ALPHANUM_ID_SIZE);

	//Store Bypas Finger Flag
	if(SSADataPtr -> bypassFingerF == SET)
		tUserConfig.bypassFinger = SSADataPtr ->bypassFinger;

	//Store Validity
	if(SSADataPtr -> validityF == SET)
		memcpy(&tUserConfig.terminationDate, SSADataPtr ->validity, sizeof(DATE_DDMMYY_t) );

	if(SSADataPtr -> vipF == SET)
		tUserConfig.vip = SSADataPtr -> vipFlag;

	if(SSADataPtr -> pinF == SET)
		memcpy(tUserConfig.pinCode, SSADataPtr -> pin, MAX_PIN_SIZE);

	//if read successfully then store route id else USER_SMART_ROUTE_NONE
	if( (SSADataPtr -> accessRouteF == SET) && (SSADataPtr -> accessRoute <= MAX_SMART_ROUTE_ACCESS) )
	{
		SSADataPtr -> accessRoute = SSADataPtr -> accessRoute - 1;//offset(0 - 1 = USER_SMART_ROUTE_NONE)
		tUserConfig.smartRouteId = SSADataPtr -> accessRoute;
	}
	else
	{
		tUserConfig.smartRouteId = USER_SMART_ROUTE_NONE;
	}

	//if read successfully then store smartRouteLevel else MAX_SMART_ROUTED_LEVEL
	if( (SSADataPtr -> smartAccessRouteLevelF == SET) && (SSADataPtr -> smartAccessRouteLevel < MAX_SMART_ROUTED_LEVEL) )
	{
		tUserConfig.smartRouteLevel = SSADataPtr -> smartAccessRouteLevel;
	}
	else
	{
		tUserConfig.smartRouteLevel = MAX_SMART_ROUTED_LEVEL;
	}
	SetUserConfig(SSA_USER_IDX, tUserConfig);

	return SUCCESS;
}

//*****************************************************************************
//	ProcessUserForBioSI()
//	Param:
//		IN : doorId
//		OUT: NONE
//	Returns:
//			 None
//	Description:
//			 This function will process user on entry reader.
//
//	Prerequisite:
//			User varification based on smart secure access policy
//
//*****************************************************************************
void ProcessUserForBioSI(void)
{
	// Default auth reason is allowed
	DcInfo.authReason = AUTH_ALLOWED;

	if(SysBasicPara.application == TA)
	{
		DPRINT(DC_LOG,"User [%s] T_A applicaton", GetAlphaNumId(DcInfo.userIdx));
		DcInfo.authReason = AUTH_ALLOW_SSA_USER;
		CheckFinalSSAPolicyAndAllow();
		return;
	}

	//If current time is within user's working hour
	if((IsCurrTimeInGivenWindow(SysBasicPara.workingHrStart, SysBasicPara.workingHrStop)) == NO)
	{
		DPRINT(DC_LOG,"User [%s] is not in working hours", GetAlphaNumId(DcInfo.userIdx));
		DcInfo.authReason = AUTH_NO_ACCESS;
		DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}
	else
	{
		DPRINT(DC_LOG,"User [%s] is in working hours", GetAlphaNumId(DcInfo.userIdx));
		DcInfo.authReason = AUTH_ALLOW_SSA_USER;
	 	CheckFinalSSAPolicyAndAllow();
	}
}

