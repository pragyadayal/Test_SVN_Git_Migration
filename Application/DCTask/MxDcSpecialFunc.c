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
//   Project      : ACS ( Access Control System)
//   Created By   : Ronak Tanna
//   File         : MxDcSpecialFunc.c
//   Description  : This file implements DC task's Special Func state
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

#include "MxTypedef.h"
#include "MxDeviceSpecific.h"
#include "MxSysTimer.h"
#include "MxConfig.h"
#include "MxDcTask.h"
#include "MxSlaveMsg.h"
#include "MxEventLog.h"
#include "MxAvResp.h"
#include "MxDcSpecialFunc.h"
#include "MxEnrollment.h"
#include "MxLogs.h"
#include "MxAlarms.h"
#include "MxAcmsMsgHandler.h"
#include "MxCafeteria.h"
#include "MxSlaveCommonMsgList.h"
#include "MxMenu.h"
#include "MxFaceRecognition.h"
#include "MxUserConfig.h"
#include "MxFrLinkedLists.h"
#include "MxFaceRecognitionConfig.h"

//******** Extern Variables **********
extern SYS_BASIC_PARA_t			SysBasicPara;
extern SYS_ADVANCE_PARA_t		SysAdvPara;
extern SPECIAL_FUNCTION_t		SpecialFunctions[MAX_SPECIAL_FUNC];
extern DOOR_MONITOR_t			DcMonitorControl;
extern DOOR_ACTIVITY_INFO_t 	DcInfo;
extern FP_TRANS_PARA_t			FpTransPara;
extern UINT16					lastDispCafeMenuIdx;
extern BOOL						FRStreamDisplayF;
extern FR_SERVER_CONFIG_t				FRServerConfig;

//******** Defines and Data Types ****


//******** Function Prototypes *******
static BOOL SetMultiUserSplCodeTimer(void);
static void MultiUserSplCodeTimeout(UINT32 dummy);
static BOOL SetSplFnGetUserInputTimer(UINT8 timer);
static BOOL SetSplFnNextInputTimer(void);
static void SplFnNextInputTimeout(UINT32 dummy);
static void SwitchDcToSplFnState(void);
static void ExitSplFnState(void);
static BOOL IsSplFnProcPsbl(void);
static void SpFnDisplayWaitTimeout(UINT32 dummy);
static void DisplayInOutSpFnList(void);
static BOOL SetUsrDltFaceTimer(void);
static void UsrDltFaceTimeout(UNUSED_PARA UINT32 dummy);

//******** Global Variables **********
TIMER_HANDLE UsrDltFaceTmrHandle = INVALID_TIMER_HANDLE;

//******** Static Variables **********


//******** Function Definations ******

//*****************************************************************************
//	DcSplFnStateFn()
//	Param: 
//		IN: msgId 	- Message which needs to be processed
//			dataPtr - Pointer to data of the message
//			length 	- Length of the message 
//		OUT: None
//	Returns:
//		NONE 
//	Description:
//		This function processes special function that requires some inputs from 
//		user.
//		
//*****************************************************************************
void DcSplFnStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	CHAR					alphaNumId[ALPHANUM_ID_SIZE];
	UINT32					userId;
	UINT8					evField2;
	BOOL					waitForResponse = FALSE;

	// While wait timer is running, we only need to handle events
	// that are common to all states.
	if(DcInfo.splFnNextInputTmrHandle != INVALID_TIMER_HANDLE) 
	{
		DcDefaultMsgHandler(msgId, dataPtr, length);		
		return;
	}
	
	// Based on special function id, process further
	switch(DcInfo.splFnId)
	{
		default:
		{
			SwitchDcToIdleState();
			break;
		}
		
		case ENROL_MODE_DELETE_DATA:
		{
			switch(msgId)
			{
				default:
				{
					DcDefaultMsgHandler(msgId, dataPtr, length);
					break;
				}

				case MSG_DEL_CRED_SEARCH_USER_NAME_FROM_ID:
				{
					if(SysAdvPara.enrollIdType == REF_USER_ID)
					{
						userId = (UINT32)atol( ( (USER_ALPHANUMID_t *)dataPtr) -> userAlphanumId);
						SendUserNameFromRefId( userId, GUI_MSG_DEL_CRED_GET_NAME_FROM_ID);
					}
					else
					{
						SendUserNameFromId( ((USER_ALPHANUMID_t *)dataPtr) -> userAlphanumId, GUI_MSG_DEL_CRED_GET_NAME_FROM_ID);
					}
					break;
				}
				
				case MSG_DEL_CRED_SEARCH_USER_LIST_EVENT:
				{
					SendMsgSearchedUserList(((USER_NAME_SEARCH_t *)dataPtr) -> userName, GUI_MSG_SEARCHED_DEL_CRED_USER_LIST);
					break;
				}
				
				case MSG_DEL_CRED_VALIDATION_EVENT:
				{
					BOOL	validUserIdF = FALSE;

					DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
				
					sprintf(alphaNumId, "%s",  ((DELETE_USER_CREDEN_t *)dataPtr) -> alphaNumId);
					// Check if user id is present in user configuration database
					if(SysAdvPara.enrollIdType == ALPHANUM_ID)
					{
						if(IsAlphaNumIdValid(alphaNumId , &(DcInfo.userIdx)) == YES)	// user not found in database
						{
							validUserIdF = TRUE;
						}
					}
					else
					{
						userId = (UINT32)atol(alphaNumId);
						if(IsUserValid(userId, &(DcInfo.userIdx)) == YES)
						{
							validUserIdF = TRUE;
						}
					}

					if(validUserIdF == FALSE)
					{
						SetAudioVisual(USER_RESP_USER_NOT_FOUND, DcInfo.respPort, NULL);
						// Set timer for how long to display user message before asking for next user input
						if(SetSplFnNextInputTimer() != SUCCESS)
						{	
							EPRINT(DC_LOG, "Unable to set timer to wait for next input");
						}
						return;
					}
					
					userId = GetUserId(DcInfo.userIdx);
					// Delete credentials for given user based on default enrollment mode
                    //ChkUpdateDeleteTypeForFace(dataPtr, ENROLLMENT_SOURCE_QT_GUI);
					if( (((DELETE_USER_CREDEN_t *)dataPtr) -> credType == ENROLL_READ_ONLY_CARD)
						||(((DELETE_USER_CREDEN_t *)dataPtr) -> credType == ENROLL_SMART_CARD) )
					{
						DeleteUserCards(DcInfo.userIdx);
						// Log event for credential- card is deleted
						WriteEventLog(USER_CREDENTIAL_DELETED, userId, USER_CARD, SOURCE_SPL_FN_EV, UNUSED_FIELD, UNUSED_FIELD);
					}
					else if(((DELETE_USER_CREDEN_t *)dataPtr) -> credType == ENROLL_BIOMETRIC_ONLY)
					{
						DeleteUserFp(DcInfo.userIdx);
						// Log event for credential- FP is deleted
						if(SysBasicPara.fprTemplateFormat == ISO_19794_FORMAT)
						{
							evField2 = USER_FP_ISO_FORMAT;
						}
						else
						{
							evField2 = USER_FP;
						}
						WriteEventLog(USER_CREDENTIAL_DELETED, userId, evField2, SOURCE_SPL_FN_EV, UNUSED_FIELD, UNUSED_FIELD);
					}
					else if(((DELETE_USER_CREDEN_t *)dataPtr) -> credType == ENROLL_BIOMETRIC_THEN_CARD)
					{
						DeleteUserCards(DcInfo.userIdx);
						// Log event for credential- card is deleted
						WriteEventLog(USER_CREDENTIAL_DELETED, userId, USER_CARD, SOURCE_SPL_FN_EV, UNUSED_FIELD, UNUSED_FIELD);

						DeleteUserFp(DcInfo.userIdx);
						// Log event for credential- FP is deleted
						if(SysBasicPara.fprTemplateFormat == ISO_19794_FORMAT)
						{
							evField2 = USER_FP_ISO_FORMAT;
						}
						else
						{
							evField2 = USER_FP;
						}
						WriteEventLog(USER_CREDENTIAL_DELETED, userId, evField2, SOURCE_SPL_FN_EV, UNUSED_FIELD, UNUSED_FIELD);
					}
					else if(((DELETE_USER_CREDEN_t *)dataPtr) -> credType == ENROLL_FACE_ONLY)
					{
						if(IsUserValid(userId, &(DcInfo.userIdx)) == NO)
						{
							EPRINT(DC_LOG, "Not valid User");
							SetAudioVisual(USER_RESP_USER_NOT_FOUND, DcInfo.respPort, NULL);
							// Set timer for how long to display user message before asking for next user input
							if(SetSplFnNextInputTimer() != SUCCESS)
							{	
								EPRINT(DC_LOG, "Unable to set timer to wait for next input");
							}
							return;
						}

						if(FRServerConfig.enableFrServer != TRUE || FRServerConfig.frMode != FR_MODE_LOCAL)
						{
							EPRINT(DC_LOG, "Fr server not Enabled or Fr server Mode is not Local");
							SetAudioVisual(USER_RESP_FACE_DELETE_FAILED, DcInfo.respPort, NULL);
							// Set timer for how long to display user message before asking for next user input
							if(SetSplFnNextInputTimer() != SUCCESS)
							{	
								EPRINT(DC_LOG, "Unable to set timer to wait for next input");
							}	
							return;
						}

                        if(ProcessDltFaceRequest(userId, INVALID_FACE_TMPLT_IDX,NON_IDT_SRC_LOCAL) == SUCCESS)
						{
							SetAudioVisual(USER_RESP_FACE_DELETE_PROCESSING, DcInfo.respPort, NULL);
							waitForResponse = TRUE;
							SetUsrDltFaceTimer();
						}
						else
						{
							SetAudioVisual(USER_RESP_FACE_DELETE_FAILED, DcInfo.respPort, NULL);
							// Set timer for how long to display user message before asking for next user input
							if(SetSplFnNextInputTimer() != SUCCESS)
							{	
								EPRINT(DC_LOG, "Unable to set timer to wait for next input");
							}
							return;
						}
					}

					if(waitForResponse == FALSE)
					{
						SetAudioVisual(USER_RESP_USER_CRED_DELETED, DcInfo.respPort, NULL);
						// Set timer for how long to display user message before asking for next user input
						if(SetSplFnNextInputTimer() != SUCCESS)
						{
							EPRINT(DC_LOG, "Unable to set timer to wait for next input");
						}
					}
					break;
				}
					
				case GUI_ALNUM_KEEP_ALIVE:
				case GUI_BUTTON_KEEP_ALIVE:
				case GUI_BUTTON_FIELD_CLEAR:
			 	{
					// Reload display idle timer
					SetLcdIdleTimer(MENU_TIMER);
					
					// Load related special function timer
					if(SetSplFnGetUserInputTimer(MENU_TIMER) != SUCCESS)
					{	
						EPRINT(DC_LOG, "Unable to set timer to get user id for spl fn: delete credentials");
					}
					break;
				}

				case GUI_BUTTON_BACK:
				{
					if(DcInfo.splFnThroughCard == FALSE)
					{
						TerminateSplFnActivity();
						SwitchDcToMenuState();
						DisplayMenuScreen(MENU_ENROLLMENT_SCREEN);
					}
					else
					{
						SetAudioVisual(USER_RESP_IDLE, DcInfo.respPort, NULL);
						ExitSplFnState();
					}
					break;
				}

				case GUI_BUTTON_CANCEL:
				case GUI_BUTTON_HOME:
				{
					// User response must be set idle to overcome incomplete process' response
					SetAudioVisual(USER_RESP_IDLE, DcInfo.respPort, NULL);
					ExitSplFnState();
					break;
				}
			}
		break;
		}

		case ITEM_SOLD_OUT:
		case ITEM_AVAILABLE:
		{
			switch(msgId)
			{
				default:
				{
					DcDefaultMsgHandler(msgId, dataPtr, length);
					break;
				}

				case GUI_BUTTON_CANCEL:
				case GUI_BUTTON_HOME:
				{
					// User response must be set idle to overcome incomplete process' response
					SetAudioVisual(USER_RESP_IDLE, DcInfo.respPort, NULL);
					ExitSplFnState();
					break;
				}

				case MSG_CAFE_SOLDOUT_VALIDATION_EVENT:
				{
					DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
					CafeMarkItemSoldOutFlag(dataPtr, TRUE);
					break;
				}

				case MSG_CAFE_AVAILABLE_VALIDATION_EVENT:
				{
					DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
					CafeMarkItemSoldOutFlag(dataPtr, FALSE);
					break;
				}

				case GUI_ALNUM_KEEP_ALIVE:
				case GUI_BUTTON_KEEP_ALIVE:
				{
					// Reload display idle timer
					SetLcdIdleTimer(MENU_TIMER);

					// Load related special function timer
					if(SetSplFnGetUserInputTimer(MENU_TIMER) != SUCCESS)
					{
						EPRINT(DC_LOG, "Unable to set timer to get user id for spl fn: delete credentials");
					}
					break;
				}

				case GUI_BUTTON_BACK:
				{
					//Same handling for Sold Out and Available Items
					if(DcInfo.splFnThroughCard == FALSE)
					{
						TerminateSplFnActivity();
						SwitchDcToMenuState();
						DisplayMenuScreen(MENU_MANAGE_CANTEEN_SCREEN);
					}
					else
					{
						SetAudioVisual(USER_RESP_IDLE, DcInfo.respPort, NULL);
						ExitSplFnState();
					}
					break;
				}
			}
			break;
		}
	}
}

//*****************************************************************************
//	ProcessSplFn
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It processes special function on given door.
//			This function should be called only if spl fn validation is success.
//
//*****************************************************************************
BOOL ProcessSplFn(void)
{
	BOOL	splFnSingleUserTnAActivatedF = CLEAR;	// if set indicates T&A spl fn for single user is activated
	BOOL	splFnMultiUserTnAActivatedF = CLEAR;	// if set indicates T&A spl fn for multiple user is activated
	BOOL	splFnTriggeredF = CLEAR;				// if set indicates spl fn is triggred 
													// (either activated/deactivated/set/reset/etc..)
	if(IsSplFnProcPsbl() == NO)
	{
		return FALSE;
	}

	switch(DcInfo.splFnId)
	{	
		case CLK_IN_ENTRY_OFFICIAL_WORK:
		{
			DcInfo.singleUserSplCode = OFFICIAL_WORK_IN;
			splFnSingleUserTnAActivatedF = SET;
			break;
		}

		case CLK_OUT_EXIT_OFFICIAL_WORK:
		{
			DcInfo.singleUserSplCode = OFFICIAL_WORK_OUT;
			splFnSingleUserTnAActivatedF = SET;
			break;
		}

		case SHORT_LEAVE_MARKING_IN:
		{
			DcInfo.singleUserSplCode = SHORT_LEAVE_IN;
			splFnSingleUserTnAActivatedF = SET;
			break;
		}

		case SHORT_LEAVE_MARKING_OUT:
		{
			DcInfo.singleUserSplCode = SHORT_LEAVE_OUT;
			splFnSingleUserTnAActivatedF = SET;
			break;
		}

		case MARK_CLOCK_IN:
		{
			DcInfo.singleUserSplCode = CLOCK_IN;
			splFnSingleUserTnAActivatedF = SET;
			break;
		}

		case MARK_CLOCK_OUT:
		{
			DcInfo.singleUserSplCode = CLOCK_OUT;
			splFnSingleUserTnAActivatedF = SET;
			break;
		}

		case CLK_IN_MARKING_BREAK:
		{
			DcInfo.singleUserSplCode = POST_BREAK_IN;
			splFnSingleUserTnAActivatedF = SET;
			break;
		}

		case CLK_OUT_MARKING_BREAK:
		{
			DcInfo.singleUserSplCode = PRE_BREAK_OUT;
			splFnSingleUserTnAActivatedF = SET;
			break;
		}

		case OVER_TIME_MARKING_IN:
		{
			DcInfo.singleUserSplCode = OVER_TIME_IN;
			splFnSingleUserTnAActivatedF = SET;
			break;
		}

		case OVER_TIME_MARKING_OUT:
		{
			DcInfo.singleUserSplCode = OVER_TIME_OUT;
			splFnSingleUserTnAActivatedF = SET;
			break;
		}

		case ENROL_USER:
		case ENROL_SPECIAL_CARDS:
		{
			ENROLLMENT_INPUTS_t enrollInputs;
	
			enrollInputs.source = SOURCE_SPECIAL_FUNC;
			enrollInputs.enrollmentType = SysAdvPara.enrollmentType;
			// Assign card count from default settings
			// Do not pass max than allowed count
			enrollInputs.cardCnt = SysAdvPara.cardCount;
			if(DcInfo.splFnId == ENROL_USER)
			{
				enrollInputs.category = ENROLL_CATEGORY_USER;
				if(SysAdvPara.cardCount > MAX_USER_CARD)
				{
					enrollInputs.cardCnt = MAX_USER_CARD;
				}
			}
			else
			{
				enrollInputs.category = ENROLL_CATEGORY_SP_FN;
			}	
			
			// Assign finger count from default settings
			// Do not pass max than allowed count
			enrollInputs.fpCnt = SysAdvPara.fpCount;
			if(enrollInputs.fpCnt > SysAdvPara.maxFingerPerUser)
			{
				enrollInputs.fpCnt = SysAdvPara.maxFingerPerUser;
			}
			
			enrollInputs.faceCnt  = SysAdvPara.maxFacePerUser;  //NOTE:- it will not take processed because SysAdvPara.enrollmentType is not FACE type as per Requirement 

			// Clear flags as we don't write any info into card if enrollment is
			// triggered through menu or special function card
			memset(&enrollInputs.cardWriteFields, CLEAR, sizeof(CARD_WRITE_FIELDS_t));

			if(RequestEnrollment(&enrollInputs) != ENROLLMENT_STARTED)
			{
				return FALSE;
			}
			return TRUE;
		}

		case ENROL_MODE_DELETE_DATA:
		{
			// Load related special function timer
			if(SetSplFnGetUserInputTimer(MENU_TIMER) != SUCCESS)
			{	
				EPRINT(DC_LOG, "Unable to set timer to get user id for spl fn: delete credentials");
				return FALSE;
			}

			// some inputs are required to complete the process of this special function 
			// So, change state to special function state to get the input
			SwitchDcToSplFnState();
			// Send message to display "Enter user id"
			SetAudioVisual(USER_RESP_GET_USER_INPUT, DcInfo.respPort, NULL);
			return TRUE;
		}

		case ITEM_SOLD_OUT:
		case ITEM_AVAILABLE:
		{
			// Load related special function timer
			if(SetSplFnGetUserInputTimer(MENU_TIMER) != SUCCESS)
			{
				EPRINT(DC_LOG, "Unable to set timer to get user id for spl fn: ItemSold/Available");
				return FALSE;
			}

			// some inputs are required to complete the process of this special function
			// So, change state to special function state to get the input
			SwitchDcToSplFnState();

			lastDispCafeMenuIdx = 0;
			if(DcInfo.splFnId == ITEM_SOLD_OUT)
			{
				SendCafeMenuMsgToQt(GUI_MSG_CAFE_SOLDOUT_ITEM_SCREEN);
			}
			else
			{
				SendCafeMenuMsgToQt(GUI_MSG_CAFE_AVAILABLE_ITEM_SCREEN);
			}
			SetLcdIdleTimer(MENU_TIMER);
			return TRUE;
		}

		case LATE_START_MARKING_ACT:
		{
			DcInfo.multiUserSplCode = LATE_IN_ALLOWED;
			splFnMultiUserTnAActivatedF = SET;
			UpdateDisplayStatusField();
			break;
		}

		case LATE_START_MARKING_DEACT:
		{
			// if already active then only deactivate it
			if(DcInfo.multiUserSplCode == LATE_IN_ALLOWED)
			{
				DcInfo.multiUserSplCode = USER_SPECIAL_CODE_NONE;
				DeleteTimer(&(DcInfo.multiUserSplCodeTmrHandle) );
			}
			UpdateDisplayStatusField();
			splFnTriggeredF = SET;
			break;
		}

		case EARLY_CLOSING_MARKING_ACT:
		{
			DcInfo.multiUserSplCode = EARLY_OUT_ALLOWED;
			splFnMultiUserTnAActivatedF = SET;
			UpdateDisplayStatusField();
			break;
		}

		case EARLY_CLOSING_MARKING_DEACT:
		{
			// if already active then only deactivate it
			if(DcInfo.multiUserSplCode == EARLY_OUT_ALLOWED)
			{
				DcInfo.multiUserSplCode = USER_SPECIAL_CODE_NONE;			
				DeleteTimer(&(DcInfo.multiUserSplCodeTmrHandle) );
			}
			splFnTriggeredF = SET;
			UpdateDisplayStatusField();
			break;
		}

		case LOCK_DOOR:
		{
			SetDoorLockState(DOOR_LOCKED);
			splFnTriggeredF = SET;
			break;
		}

		case UNLOCK_DOOR:
		{
			SetDoorLockState(DOOR_UNLOCKED);
			splFnTriggeredF = SET;
			break;
		}

		case NORMAL_DOOR:
		{
			SetDoorLockState(DOOR_LOCK_NORMAL);
			splFnTriggeredF = SET;
			break;
		}
		
		case CLEAR_SYS_ALARM:
		{
			splFnTriggeredF = SET;
			ClearAllAlarms();
			break;
		}	

		default:
		{
			return FALSE;
		}
	}

	// Actions for spl fn is triggred (either activated/deactivated/set/reset/etc..)
	// display spl fn activated, update status field, switch DC task to idle state
	if(splFnTriggeredF == SET)
	{
		SetAudioVisual(USER_RESP_SPL_FN_ACTIVATED, DcInfo.respPort, NULL);
	}
	// Actions for T&A spl fn for single user:
	// Ask for user input, update status field, start timer to get user input, switch DC task to idle state
	else if(splFnSingleUserTnAActivatedF == SET)
	{
		ACCESS_MODE_e accessMode;
		accessMode = GetAccessModeFromRespPort(DcInfo.userType, DcInfo.respPort);
		if( accessMode == ACCESS_MODE_FACE ||
		    accessMode == ACCESS_MODE_FACE_CARD ||
		    accessMode == ACCESS_MODE_FACE_PIN ||
		    accessMode == ACCESS_MODE_FACE_FINGER_PRINT )
		{
			StreamPayload_t         streamInfo;
			streamInfo.reason = START_STREAM_SPECIAL_FUNCTION;
			streamInfo.captureMode = STREAM_CAPTURE_FACE;
			if(StartFRStreamingOnDeviceIdleScreen() == SUCCESS)
			{
				StartWaitForFrStreamTimer(streamInfo);
				
				if(SetSplFnGetUserInputTimer(SysAdvPara.multiAccessTimer) == FAIL)
				{	
					EPRINT(DC_LOG, "Unable to start SplFunc Get UserID Timer");
				}
			}
			else
			{
				StartFRStreamFailureResp(streamInfo);
			}
		}
		else
		{
			SetAudioVisual(USER_RESP_GET_USER_CREDENTIAL, DcInfo.respPort, NULL);
			if(SetSplFnGetUserInputTimer(SysAdvPara.multiAccessTimer) == FAIL)
			{	
				EPRINT(DC_LOG, "Unable to start SplFunc Get UserID Timer");
			}
		}
	}
	// Actions for T&A spl fn for multiple user:
	// Ask for user input, update status field, start timer to get user input, switch DC task to idle state
	else if(splFnMultiUserTnAActivatedF == SET)
	{
		SetAudioVisual(USER_RESP_SPL_FN_ACTIVATED, DcInfo.respPort, NULL);
		if(SetMultiUserSplCodeTimer() == FAIL)
		{	
			EPRINT(DC_LOG, "Unable to start SpecialFunc Timer");
		}
	}
	
	if( ( (splFnTriggeredF == SET) || (splFnMultiUserTnAActivatedF == SET) ) && ( DcInfo.splFnThroughCard == FALSE ) )
	{
		LoadMenuWaitTimer(LCD_IDLE_TIMER, &DisplayMenuScreen, MENU_HRD_ADMIN_E_CANTEEN_SCREEN);
	}
	else
	{
		if( (splFnSingleUserTnAActivatedF == SET) && (DcInfo.splFnThroughCard == TRUE) )
		{
			SwitchDcToIdleState();
			DcInfo.splFnThroughCard = TRUE;
		}
		else
		{
			SwitchDcToIdleState();
		}
	}
	return TRUE;
}

//*****************************************************************************
//	IsSplFnProcPsbl
//	Param: 
//		IN :	splFnId		- index of spl fn that is to be processed
//				respPort	- request is from internal or from external port
//		OUT:	none
//	Returns:	YES		- If further processing of given spl fn is possible
//				NO		- If further processing of given spl fn is not possible
//	Description:
//			It validates all conditions required to process given spl fn further.
//			Specific reason will be displayed if validation fails in most cases.
//
//*****************************************************************************
static BOOL IsSplFnProcPsbl(void)
{
	// if special function is disabled do not process it
	if(SpecialFunctions[DcInfo.splFnId].enable == DISABLED)
	{
		SetAudioVisual(USER_RESP_FUNCTION_DISABLED, DcInfo.respPort, NULL);
		return NO;
	}

	if( (SysBasicPara.application == ACCESS_CTRL) || (SysBasicPara.application == TA) )
	{
		switch(DcInfo.splFnId)
		{	
			// No check required for following spl fn 
			case CLK_IN_ENTRY_OFFICIAL_WORK:
			case CLK_OUT_EXIT_OFFICIAL_WORK:
			case SHORT_LEAVE_MARKING_IN:
			case SHORT_LEAVE_MARKING_OUT:
			case MARK_CLOCK_IN:
			case MARK_CLOCK_OUT:
			case CLK_IN_MARKING_BREAK:
			case CLK_OUT_MARKING_BREAK:
			case OVER_TIME_MARKING_IN:
			case OVER_TIME_MARKING_OUT:
			case LOCK_DOOR:
			case UNLOCK_DOOR:
			case NORMAL_DOOR:
			case CLEAR_SYS_ALARM:
			{
				return YES;
			}
	
			case ENROL_USER:
			case ENROL_SPECIAL_CARDS:
			{
				// This function is not supported from exit mode, ignore it
				if(DcInfo.respPort == EXTERNAL_PORT)
				{
					return NO;
				}

				// if default global enrollment mode is disabled, function can not be triggered
				if(SysAdvPara.enrollmentEnable == DISABLED)
				{
					SetAudioVisual(USER_RESP_ENROLLMENT_DISABLED, DcInfo.respPort, NULL);
					return NO;
				}

				return YES;
			}

			case ENROL_MODE_DELETE_DATA:
			{
				// This function is not supported from exit mode, ignore it
				if(DcInfo.respPort == EXTERNAL_PORT)
				{
					return NO;
				}

				return YES;
			}
			case LATE_START_MARKING_DEACT:
			{
				if(DcInfo.multiUserSplCode == EARLY_OUT_ALLOWED)
				{
					SetAudioVisual(USER_RESP_SPL_FN_IS_ACTIVE, DcInfo.respPort, &DcInfo.splFnId);
					return NO;
				}
				return YES;
			}

			case EARLY_CLOSING_MARKING_DEACT:
			{
				if(DcInfo.multiUserSplCode == LATE_IN_ALLOWED)
				{
					SetAudioVisual(USER_RESP_SPL_FN_IS_ACTIVE, DcInfo.respPort, &DcInfo.splFnId);
					return NO;
				}
				return YES;
			}
			case LATE_START_MARKING_ACT:
			case EARLY_CLOSING_MARKING_ACT:
			{
				// if T&A special function for multi user is already active, ignore new activation
				if(DcInfo.multiUserSplCodeTmrHandle != INVALID_TIMER_HANDLE)
				{	
					SetAudioVisual(USER_RESP_SPL_FN_IS_ACTIVE, DcInfo.respPort,
														&DcInfo.splFnId);
					return NO;
				}
	
				return YES;
			}
	
			default:
			{
				return NO;
			}
		}
	}
	else
	{
		switch(DcInfo.splFnId)
		{
			case ENROL_USER:
			case ENROL_SPECIAL_CARDS:
			{
				// This function is not supported from exit mode, ignore it
				if(DcInfo.respPort == EXTERNAL_PORT)
				{
					return NO;
				}

				// if default global enrollment mode is disabled, function can not be triggered
				if(SysAdvPara.enrollmentEnable == DISABLED)
				{
					SetAudioVisual(USER_RESP_ENROLLMENT_DISABLED, DcInfo.respPort, NULL);
					return NO;
				}
				
				return YES;
			}	

			case ENROL_MODE_DELETE_DATA:
			case ITEM_SOLD_OUT:
			case ITEM_AVAILABLE:
			case CLEAR_SYS_ALARM:
			case LOCK_DOOR:
			case UNLOCK_DOOR:
			case NORMAL_DOOR:
			{
				// This function is not supported from exit mode, ignore it
				if(DcInfo.respPort == EXTERNAL_PORT)
				{
					return NO;
				}
				return YES;
			}

			default:
			{
				return NO;
			}	
		}
	}
}

//*****************************************************************************
//	SetMultiUserSplCodeTimer
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//				starts or reload timer for special functions which requires to 
//				process multiple users.
//				
//*****************************************************************************
static BOOL SetMultiUserSplCodeTimer(void)
{
	if(DcInfo.multiUserSplCodeTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;
		
		// convert time  which is into minutes into 100 msec resolutions
		tInfo.count = (SysAdvPara.lateEarlyTimer * 60 * 10);
		tInfo.data = 0;
		tInfo.funcPtr = &MultiUserSplCodeTimeout;
		return (StartTimer(tInfo, &DcInfo.multiUserSplCodeTmrHandle, __FUNCTION__));
	}
	else //reload timer
	{
		return (ReloadTimer(DcInfo.multiUserSplCodeTmrHandle, 
									(SysAdvPara.lateEarlyTimer * 60 * 10)) );
	}
}

//*****************************************************************************
//	MultiUserSplCodeTimeout
//	Param: 
//		IN :	dummy
//		OUT:	none
//	Returns:	none
//	Description:
//				On timeout of Multi-user special func, this func clears
//				code from door activity structure.
//
//*****************************************************************************
static void MultiUserSplCodeTimeout(UNUSED_PARA UINT32 dummy)
{
	DcInfo.multiUserSplCode = USER_SPECIAL_CODE_NONE;
	UpdateDisplayStatusField();
	DeleteTimer(&DcInfo.multiUserSplCodeTmrHandle);	
}

//*****************************************************************************
//	SetSplFnGetUserInputTimer
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//				starts or reload timer for special functions to get user input.
//
//*****************************************************************************
static BOOL SetSplFnGetUserInputTimer(UINT8 timer)
{
	if(DcInfo.splFnGetUserInputTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;
		
		tInfo.count = (timer * 10);
		tInfo.data = 0;
		tInfo.funcPtr = &SplFnGetUserInputTimeout;
		return (StartTimer(tInfo, &(DcInfo.splFnGetUserInputTmrHandle), __FUNCTION__) );
	}
	else //reload timer
	{
		return (ReloadTimer(DcInfo.splFnGetUserInputTmrHandle, (timer * 10)) );
	}
}

//*****************************************************************************
//	SplFnGetUserInputTimeout
//	Param: 
//		IN :	dummy
//		OUT:	none
//	Returns:	none
//	Description:
//				It switches DC to idle state and sends audio visual response to 
//				DC for timeout. If special code for single user is active, 
//				it deactivates special code.
//
//*****************************************************************************
void SplFnGetUserInputTimeout(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);

	StopFRStreamingOnDeviceIdleScreen(TRUE);

	// Deactivate special code
	DcInfo.singleUserSplCode = USER_SPECIAL_CODE_NONE;
	SetAudioVisual(USER_RESP_TIMEOUT, DcInfo.respPort, NULL);
	SwitchDcToIdleState();
}

//*****************************************************************************
//	SetSplFnNextInputTimer
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//				starts or reload timer for special functions to get user input.
//
//*****************************************************************************
static BOOL SetSplFnNextInputTimer(void)
{
	if(DcInfo.splFnNextInputTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;
		
		tInfo.count = (ENROLLMENT_USER_RESP_TIMER *10);
		tInfo.data = 0;
		tInfo.funcPtr = &SplFnNextInputTimeout;
		return (StartTimer(tInfo, &(DcInfo.splFnNextInputTmrHandle), __FUNCTION__) );
	}
	else //reload timer
	{
		return (ReloadTimer(DcInfo.splFnNextInputTmrHandle, (ENROLLMENT_USER_RESP_TIMER*10)) );
	}
}

//*****************************************************************************
//	SetUsrDltFaceTimer
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//				starts or reload timer for local delete face request's response timeout handling.
//
//*****************************************************************************
static BOOL SetUsrDltFaceTimer(void)
{
	if(UsrDltFaceTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;
		
		tInfo.count = (DELETE_USER_RESP_TIMER *10);
		tInfo.data = 0;
		tInfo.funcPtr = &UsrDltFaceTimeout;
		return (StartTimer(tInfo, &(UsrDltFaceTmrHandle), __FUNCTION__) );
	}
	else //reload timer
	{
		return (ReloadTimer(UsrDltFaceTmrHandle, (DELETE_USER_RESP_TIMER*10)) );
	}
}

static void UsrDltFaceTimeout(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&UsrDltFaceTmrHandle);
	SetAudioVisual(USER_RESP_FACE_DELETE_FAILED, DcInfo.respPort, NULL);
	// Set timer for how long to display user message before asking for next user input
	if(SetSplFnNextInputTimer() != SUCCESS)
	{	
		EPRINT(DC_LOG, "Unable to set timer to wait for next input");
	}
}
//*****************************************************************************
//	SplFnNextInputTimeout
//	Param: 
//		IN :	dummy
//		OUT:	none
//	Returns:	none
//	Description:
//				It switches DC to idle state and sends audio visual response to 
//				DC for timeout. If special code for single user is active, 
//				it deactivates special code.
//
//*****************************************************************************
static void SplFnNextInputTimeout(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&DcInfo.splFnNextInputTmrHandle);
	// Send message to display "Enter user id"
	SetAudioVisual(USER_RESP_GET_USER_INPUT, DcInfo.respPort, NULL);
	// Load related special function timer
	if(SetSplFnGetUserInputTimer(MENU_TIMER) != SUCCESS)
	{	
		EPRINT(DC_LOG, "Unable to set timer to get user id for spl fn: delete credentials");
	}
}

//*****************************************************************************
//	GetCurrSplCode
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	EV_USER_SPECIAL_CODE_FIELD_e
//	Description:
//				Returns current special code for user. If special code for 
//				processing single user is active give that as current special
//				code over multi-user special function.
//
//*****************************************************************************
UINT8 GetCurrSplCode(void)
{
	if(DcInfo.singleUserSplCode != USER_SPECIAL_CODE_NONE)
	{
		return DcInfo.singleUserSplCode;
	}
	else
	{
		return (DcInfo.multiUserSplCode);
	}
}

//*****************************************************************************
//	ExitSplFnState()
//	Param: 
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function cleans up spl fn activity to abnormally exit 
//				special function state. 
//	NOTE:		It won't clean up EARLY OUT or LATE IN spl fn activity.
//
//*****************************************************************************
static void ExitSplFnState(void)
{
	TerminateSplFnActivity();
	SwitchDcToIdleState();
}

//*****************************************************************************
//	SwitchDcToSplFnState()
//	Param: 
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function switches DCTask FSM to special function state and 
//				initializes required parameters.
//				
//*****************************************************************************
static void SwitchDcToSplFnState(void)
{
	CleanUpTempUserFP();
	// Change DC slave FSM state to Special function state
	DcInfo.curState = DC_SPL_FUNC_STATE;
}

//*****************************************************************************
//	SendUserNameFromRefId
//	Param:
//		IN :	userIdx - User Id
//				msgId	 - Message Id to Whom List Needs to be sent
//		OUT:	none
//	Returns:	none
//	Description:
//			This function searches user name into user databse from User Id and
//			sends searched list to qt.
//*****************************************************************************
void SendUserNameFromRefId(UINT32 userRefId, SLAVE_MSG_ID_e msgId)
{
	CHAR 	msgBuf[50];
	UINT16 	userIdx;

	if(IsUserValid(userRefId, &userIdx) == YES)
	{
		sprintf(msgBuf, "%s%c%d%c", GetUserName(userIdx), UI_MSG_FSP , GetUserId(userIdx) , UI_MSG_FSP);
		SendMessageToQt(&msgBuf, msgId);
	}
	else
	{
		SendMessageToQt("", msgId);
	}
	return;
}

//*****************************************************************************
//	SendUserNameFromId
//	Param: 
//		IN :	userIdx - User Id
//				msgId	 - Message Id to Whom List Needs to be sent
//		OUT:	none
//	Returns:	none
//	Description:
//			This function searches user name into user databse from User Id and
//			sends searched list to qt.
//*****************************************************************************
void SendUserNameFromId(CHARPTR alphaNumId, SLAVE_MSG_ID_e msgId)
{
	CHAR 	msgBuf[50];
	UINT16 	userIdx;

	if(IsAlphaNumIdValid(alphaNumId, &userIdx) == YES)
	{
		sprintf(msgBuf, "%s%c%d%c", GetUserName(userIdx), UI_MSG_FSP , GetUserId(userIdx) , UI_MSG_FSP);
		SendMessageToQt(&msgBuf, msgId);
	}
	else
	{
		SendMessageToQt("", msgId);
	}
	return;
}

//*****************************************************************************
//	SendMsgSearchedUserList
//	Param: 
//		IN :	userName - User Name To Search
//				msgId	 - Message Id to Whom List Needs to be sent
//		OUT:	none
//	Returns:	none
//	Description:
//			This function searches given user name into user databse and sends
//			searched list to qt.
//*****************************************************************************
void SendMsgSearchedUserList(CHARPTR userName, SLAVE_MSG_ID_e msgId)
{
	UINT16 		idx, len = 0;
	UINT16		userList[10];	// to store index of searched user 
	UINT8		searchedCnt = 0;
	CHAR 		msgBuf[500];
	
	// Search user first
	for(idx = 0; ( (idx < MAX_USERS) && (searchedCnt < 10) ); idx++)
	{
		if(strncasecmp(GetUserName(idx), userName, strlen(userName) ) == 0)
		{
			userList[searchedCnt++] = idx;
		}
	}

	// Now construct message
	if(searchedCnt)
	{
		sprintf(msgBuf, "%d%c", searchedCnt, UI_MSG_FSP);
		len = strlen(msgBuf);
		
		for(idx = 0; idx < searchedCnt; idx++)
		{
			if(SysAdvPara.enrollIdType == REF_USER_ID)
			{
				sprintf(&msgBuf[len], "%u%c%s%c%d%c",
								GetUserId(userList[idx]), DESH_FSP,
								GetUserName(userList[idx]), DESH_FSP ,
								GetUserId(userList[idx]) , UI_MSG_FSP);
			}
			else
			{
				sprintf(&msgBuf[len], "%s%c%s%c%d%c",
								GetAlphaNumId(userList[idx]), DESH_FSP,
								GetUserName(userList[idx]), DESH_FSP ,
								GetUserId(userList[idx]) , UI_MSG_FSP);
			}
			len = strlen(msgBuf);
		}
		SendMessageToQt(&msgBuf, msgId);
	}
	else
	{
		SendMessageToQt("", msgId);
	}
}

void GetMultiUserSplFnStatus(MUL_USR_SPL_FUN_QT_t * mulUsrSplFnStatus)
{

	mulUsrSplFnStatus ->  lateInStartStop	= TRUE;
	mulUsrSplFnStatus ->  earlyOutStartStop	= TRUE;

	if(DcInfo.multiUserSplCode == LATE_IN_ALLOWED)
	{
		mulUsrSplFnStatus ->  lateInStartStop	= FALSE;
		mulUsrSplFnStatus->	lateInEnable = SpecialFunctions[LATE_START_MARKING_DEACT].enable;
		mulUsrSplFnStatus->	earlyOutEnable = SpecialFunctions[EARLY_CLOSING_MARKING_ACT].enable;
	}
	else if(DcInfo.multiUserSplCode == EARLY_OUT_ALLOWED)
	{
		mulUsrSplFnStatus ->  earlyOutStartStop	= FALSE;
		mulUsrSplFnStatus->	lateInEnable = SpecialFunctions[LATE_START_MARKING_ACT].enable;
		mulUsrSplFnStatus->	earlyOutEnable = SpecialFunctions[EARLY_CLOSING_MARKING_DEACT].enable;
	}
	else
	{
		mulUsrSplFnStatus->	lateInEnable = SpecialFunctions[LATE_START_MARKING_ACT].enable;
		mulUsrSplFnStatus->	earlyOutEnable = SpecialFunctions[EARLY_CLOSING_MARKING_ACT].enable;
	}
}

//*****************************************************************************
//	TerminateSplFnActivity()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function terminates special function related activity
//
//*****************************************************************************
void TerminateSplFnActivity(VOID)
{
	// Terminate all timers which may be running in this state (except for LATE IN and EARLY OUT spl fn)
	DeleteTimer(&(DcInfo.splFnGetUserInputTmrHandle));
	DeleteTimer(&DcInfo.splFnNextInputTmrHandle);
	// deactivate special code which might be active for single user T&A spl fn
	DcInfo.singleUserSplCode = USER_SPECIAL_CODE_NONE;
}

//*****************************************************************************
//	SwitchDcToSpFnDisplayState()
//	Param:
//		IN :	doorId
//		OUT:	None
//	Returns:	None
//	Description:
//				This function switches DCTask FSM to DC_JOB_CODE_DISPLAY_STATE
//				and initializes required parameters.
//*****************************************************************************
void SwitchDcToSpFnDisplayState(void)
{
	UINT8	tmpIdx, searchedInOutSpFnCnt = 0;

	for(tmpIdx = 0 ; tmpIdx < MAX_INOUT_SPL_FUNC_LIST;  tmpIdx ++)
	{
		if( SpecialFunctions[tmpIdx].enable == TRUE)
		{
			searchedInOutSpFnCnt++;
		}
	}

	if(searchedInOutSpFnCnt == 0)
	{
		DcInfo.singleUserSplCode = USER_SPECIAL_CODE_NONE;
		//return back to check remaining policies
		CheckFinalPolicyAndAllow();
		return;
	}

	DisplayInOutSpFnList();

	// start timer for user input

	if(SetLcdIdleTimer(SP_FN_INPUT_WAIT_TIMER) == FAIL)
	{
		EPRINT(DC_LOG,"Unable to start LCD IDLE Timer for sp fn input for Door");
	}

	if(SetSpFnDisplayWaitTimer(SP_FN_INPUT_WAIT_TIMER) == FAIL)
	{
		EPRINT(DC_LOG,"Unable to start sp fn Wait Timer for Door");
	}

	CleanUpTempUserFP();
	DcInfo.curState = DC_SELECT_SP_FN_STATE;
}

//*****************************************************************************
//	SetSpFnDisplayWaitTimer()
//	Param:
//		IN :	timer value
//		OUT: 	NONE
//	Returns: SUCCESS/FAIL
//	Description: start/reload special function display wait timer
//
//*****************************************************************************
BOOL SetSpFnDisplayWaitTimer(UINT8 timer)
{
	if(DcInfo.spFnInputWaitTimer == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = (timer * 10);
		tInfo.data = 0;
		tInfo.funcPtr = &SpFnDisplayWaitTimeout;
		return (StartTimer(tInfo, &DcInfo.spFnInputWaitTimer, __FUNCTION__) );
	}
	else //reload timer
	{
		return (ReloadTimer(DcInfo.spFnInputWaitTimer, (timer * 10)) );
	}
}

//*****************************************************************************
//	SpFnDisplayWaitTimeout
//	Param:
//		IN :	dummy
//		OUT:	none
//	Returns:	none
//	Description:
//				It switches DC to idle state and sends audio visual response to
//				DC for timeout. If special code for single user is active,
//				it deactivates special code.
//
//*****************************************************************************
static void SpFnDisplayWaitTimeout(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&DcInfo.spFnInputWaitTimer);
	DcInfo.authReason = AUTH_TIMEOUT;
	DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
}

//*****************************************************************************
//	DcSelectSpFnState()
//	Param:
//		IN : msgId - ID of the received message
//			 dataPtr - Pointer to the data
//			 length - length of the data
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//			This Function will be called by the dc task FSM. It passes events
//		to appropriate job costing related function and handles
//		in default.
//
//*****************************************************************************
void DcSelectSpFnState(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	BOOL	startSpFnTimerF = FALSE;

	switch(msgId)
	{
		case GUI_BUTTON_REGULAR_IN:
		{
			DcInfo.singleUserSplCode = CLOCK_IN;
			DeleteTimer(&DcInfo.spFnInputWaitTimer);
			CheckFinalPolicyAndAllow();
			break;
		}
		case GUI_BUTTON_REGULAR_OUT:
		{
			DcInfo.singleUserSplCode = CLOCK_OUT;
			DeleteTimer(&DcInfo.spFnInputWaitTimer);
			CheckFinalPolicyAndAllow();
			break;
		}
		case GUI_BUTTON_BREAK_IN:
		{
			DcInfo.singleUserSplCode = POST_BREAK_IN;
			DeleteTimer(&DcInfo.spFnInputWaitTimer);
			CheckFinalPolicyAndAllow();
			break;
		}
		case GUI_BUTTON_BREAK_OUT:
		{
			DcInfo.singleUserSplCode = PRE_BREAK_OUT;
			DeleteTimer(&DcInfo.spFnInputWaitTimer);
			CheckFinalPolicyAndAllow();
			break;
		}
		case GUI_BUTTON_SHORT_LEAVE_IN:
		{
			DcInfo.singleUserSplCode = SHORT_LEAVE_IN;
			DeleteTimer(&DcInfo.spFnInputWaitTimer);
			CheckFinalPolicyAndAllow();
			break;
		}
		case GUI_BUTTON_SHORT_LEAVE_OUT:
		{
			DcInfo.singleUserSplCode = SHORT_LEAVE_OUT;
			DeleteTimer(&DcInfo.spFnInputWaitTimer);
			CheckFinalPolicyAndAllow();
			break;
		}
		case GUI_BUTTON_OFFICIAL_IN:
		{
			DcInfo.singleUserSplCode = OFFICIAL_WORK_IN;
			DeleteTimer(&DcInfo.spFnInputWaitTimer);
			CheckFinalPolicyAndAllow();
			break;
		}
		case GUI_BUTTON_OFFICIAL_OUT:
		{
			DcInfo.singleUserSplCode = OFFICIAL_WORK_OUT;
			DeleteTimer(&DcInfo.spFnInputWaitTimer);
			CheckFinalPolicyAndAllow();
			break;
		}
		case GUI_BUTTON_OVERTIME_IN:
		{
			DcInfo.singleUserSplCode = OVER_TIME_IN;
			DeleteTimer(&DcInfo.spFnInputWaitTimer);
			CheckFinalPolicyAndAllow();
			break;
		}

		case GUI_BUTTON_OVERTIME_OUT:
		{
			DcInfo.singleUserSplCode = OVER_TIME_OUT;
			DeleteTimer(&DcInfo.spFnInputWaitTimer);
			CheckFinalPolicyAndAllow();
			break;
		}

		case GUI_BUTTON_KEEP_ALIVE:
		{
			startSpFnTimerF = TRUE;
			break;
		}

		case GUI_BUTTON_HOME:
		{
			SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
			DeleteTimer(&DcInfo.spFnInputWaitTimer);
			SwitchDcToIdleState();
			break;
		}

		default:
		{
			DcDefaultMsgHandler(msgId, dataPtr, length);
			break;
		}
	}
	if(startSpFnTimerF == TRUE)
	{
		if(SetLcdIdleTimer(SP_FN_INPUT_WAIT_TIMER) == FAIL)
		{
			EPRINT(DC_LOG,"Unable to start LCD IDLE Timer for sp fn input for Door");
		}

		if(SetSpFnDisplayWaitTimer(SP_FN_INPUT_WAIT_TIMER) == FAIL)
		{
			EPRINT(DC_LOG,"Unable to start sp fn Wait Timer for Door");
		}
	}
}

//*****************************************************************************
//	DisplayJobStringsFn
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description: displays job list on LCD display
//
//*****************************************************************************
static void DisplayInOutSpFnList(void)
{
	UINT8 					idx=0;
	INT32					tmpIdx;
	IN_OUT_SP_FN_MSG_QT_t	inOutSpMsg;

	for(tmpIdx = 0 ; tmpIdx < MAX_INOUT_SPL_FUNC_LIST;  tmpIdx ++)
	{
		if( SpecialFunctions[tmpIdx].enable == TRUE)
		{
			inOutSpMsg.enableDisabledSpecionFunctionList[idx] = tmpIdx;
			idx++;
		}
	}
	inOutSpMsg.spFnCnt = idx;
	SendMessageToQt((VOIDPTR)&inOutSpMsg, GUI_MSG_INOUT_SPCL_FUNCTION_SELECTION);
}

/**
 * @brief ReloadSpecialFunctionInputTimer
 * 
 * @return VOID 
 */
VOID ReloadSpecialFunctionInputTimer(void)
{
	if(SetSplFnNextInputTimer() != SUCCESS)
	{
		EPRINT(DC_LOG, "Unable to set timer to wait for next input");
	}
}

void ChkUpdateDeleteTypeForFace(DELETE_USER_CREDEN_t * inputDataPtr,ENROLLMENT_SOURCE_e SourceType)
{
	if(NULL == inputDataPtr)
		return;
	
	switch(SourceType)
	{
		case ENROLLMENT_SOURCE_QT_GUI:
			if(inputDataPtr->credType == 4)
			{
				inputDataPtr->credType = ENROLL_FACE_ONLY;
			}
			break;
		case ENROLLMENT_SOURCE_ACMS_GUI:
		case ENROLLMENT_SOURCE_API:
			if(inputDataPtr->credType == 5)
			{
				inputDataPtr->credType = ENROLL_FACE_ONLY;
			}
			break;
		case ENROLLMENT_SOURCE_DEVICE_GUI: //NOTE: In Device side handle it its GUI code
		default:
			break;
	}
}
