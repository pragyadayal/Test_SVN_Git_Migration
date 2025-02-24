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
//   Created By   : Ronak Gohel
//   File         : MxAvResp.c
//   Description  : This file contains audio and visual related functions.
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "MxTypedef.h"
#include "MxAvResp.h"
#include "MxConfig.h"
#include "MxDcTask.h"
#include "MxDcTaskFeatures.h"
#include "MxDcTaskUtils.h"
#include "MxEventLog.h"
#include "MxSlaveMsg.h"
#include "MxSysTimer.h"
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
#include "MxAcmsCommand.h"
#include "MxCafeteria.h"
#include "MxGuiHandler.h"
#include "MxSysCommonDef.h"
#include "MxSysTest.h"
#include "MxDisplayMessage.h"
#include "MxGpioDrv.h"
#include "MxAvRespTest.h"
#include "MxEnrollment.h"
#include "MxEnrollment.h"
#include "MxUserConfig.h"

//******** Extern Variables **********
extern DOOR_ACTIVITY_INFO_t 	DcInfo;
extern SYS_BASIC_PARA_t			SysBasicPara;
extern SYS_ADVANCE_PARA_t		SysAdvPara;
extern CAFE_MENU_NAVIGATION_t 	cafeMenuNavigation;
extern CAFE_CMD_DETAIL_t   		CafeSetCmd;
extern CAFE_MENU_PROC_INFO_t	cafeMenuProcInfo;
extern VAM_CONFIG_t				VamConfig[MAX_VAM_CONFIG];
extern JCM_CONFIG_t				JcmConfig[MAX_JCM_CONFIG+3]; //+3 for 'None', 'Default' and 'Continue' Job
extern READER_MODE_e			DoorEntryExitMode;
extern BOOL						DispDoorModeF;

extern CAFE_CMD_DETAIL_t		CafeSetCmd;


extern const MSG_GEN_INFO_QT_t defltQtGenInfo;


void SetAudioVisualTest(void)
{

	USER_RESP_e 			userResp;

	void 					*dataPtr;
	MSG_GEN_INFO_QT_t	 	qtGenInfo 		= defltQtGenInfo;
	SYS_TEST_INPUT_DATA_t	inputData;
	USER_RESP_BU_STATUS_t	statusTmpVar;
	USB_STATUS_EV_DATA_t	evtDataTmp;
	SYS_TEST_OUTPUT_DATA_t	outPutData;
	SYSTEM_DEFAULT_STATUS_e defaultStatus;
	UINT8					tempUINT8Var;

	inputData.detectedInput = AUX_INPUT_INACTIVE;
	inputData.dispTimeOut = AUDIO_VISUAL_TEST_SLEEP_TIME;
	inputData.expectedInput = AUX_INPUT_INACTIVE;
	inputData.testState = SYSTEM_TEST_DOOR_TMPR;

	SetUserName(0, "user name");
	SetAlphaNumId(0, "user1");
	DcInfo.userIdx = 0;
	SysAdvPara.twoPersonTimer = AUDIO_VISUAL_TEST_SLEEP_TIME;
	SysAdvPara.multiAccessTimer = AUDIO_VISUAL_TEST_SLEEP_TIME;
	SysAdvPara.interDigitTimer = AUDIO_VISUAL_TEST_SLEEP_TIME;

for(userResp = USER_RESP_IDLE; userResp < MAX_USER_RESP; userResp++)
	{
		DPRINT(SYS_LOG,"userResp = %d",userResp);
		switch(userResp)
		{
			default:
				continue;

				case USER_RESP_IDLE:
				case USER_RESP_DISPLAY_IDLE_SCREEN:
				case USER_RESP_DISPLAY_GOING_IN_SCREEN:
				case USER_RESP_DISPLAY_GOING_OUT_SCREEN:
				case USER_RESP_DISPLAY_MENU_SCREEN:
				case USER_RESP_NEXT_PERSON:
				case USER_RESP_PLACE_FINGER:
				case USER_RESP_NEXT_FINGER:
				case USER_RESP_CARD_ENROLLED:
				case USER_RESP_ENROLL_FAIL:
				case USER_RESP_ENROLL_TIMEOUT:
				case USER_RESP_WRITING_CARD:
				case USER_RESP_USER_NOT_FOUND:
				case USER_RESP_USER_CRED_DELETED:
				case USER_RESP_ENROLLMENT_DISABLED:
				case USER_RESP_FUNCTION_DISABLED:
				case USER_RESP_VERIFY_SELECTION:
				case USER_RESP_VERIFY_READER:
				case USER_RESP_VERIFY_CARD_TYPE:
				case USER_RESP_CRED_LIMIT_EXCEEDS:
				case USER_RESP_DUPL_USER_CARD:
				case USER_RESP_DUPL_USER_FINGER:
				case USER_RESP_ENROLL_USER_NOT_FOUND:
				case USER_RESP_FP_MEM_FULL:
				case USER_RESP_ENROLL_TRY_AGAIN_FINGER:
				case USER_RESP_READING_FROM_CARD:
				case USER_RESP_ENTER_PIN:
				case USER_RESP_MMC_DEVICE_STATUS:
				case USER_RESP_CAFE_CMD_TIMEOUT:
				case USER_RESP_TIMEOUT:
				case USER_RESP_SELF_ENROLL_REQ_INPUT:
				case USER_RESP_PROCESSING_QUERY:
				case USER_RESP_TOUCH_SCREEN_TEST_START:
				case USER_RESP_LCD_TEST_START:
				case USER_RESP_TEST_FAIL:
				case USER_RESP_TEST_SUCCESS:
				case USER_RESP_KEY_CHANGE_SHOW_CARD:
				case USER_RESP_KEY_CHANGE_SUCCESS:
				case USER_RESP_CHANGING_CARD_KEY:
				case USER_RESP_KEY_CHANGE_FAIL:
				case USER_RESP_CAFE_SHOW_CARD:
				case USER_RESP_PRINT_CMD_SENT:
				case USER_RESP_MENU_FAILED:
				case USER_RESP_PSWD_MISMATCH:
				case USER_RESP_PSWD_WRONG:
				case USER_RESP_PSWD_BLANK:
				case USER_RESP_PSWD_SUCCESS:
				case USER_RESP_PASSWORD_DEFAULTED:
				case USER_RESP_MENU_LOGIN_FAILED:
				case USER_RESP_SEARCHING_WIFI:
				case USER_RESP_WIFI_AP_NOT_FOUND:
				case USER_RESP_CAFE_NOT_SERVING_NOW:
				case USER_RESP_WPS_PROCESSING:
				case USER_RESP_WIFI_CONNECTED:
				case USER_RESP_PD_USER_ALLOWED:
				case USER_RESP_PD_USER_DENIED:
				case USER_RESP_PD_READING_CARD:
				case USER_RESP_PD_PLACE_YOUR_FINGER:
				case USER_RESP_PD_INVALID_CARD:
				case USER_RESP_PD_CARD_READ_FAILED:
				case USER_RESP_PD_INVALID_PASSWORD:
				case USER_RESP_PD_MENU_TIMEOUT:
				case USER_RESP_PD_SYSTEM_DEFAULTING:
				case USER_RESP_PD_SYSTEM_DEFAULTED:
				case USER_RESP_PD_FIRMWARE_UPLOAD_SUCCESS:
				case USER_RESP_PD_FIRMWARE_UPLOAD_FAIL:
				case USER_RESP_PD_COMMUNICATION_ERROR:
				case USER_RESP_PD_FIRMWARE_XFER_SUCCESS:
				case USER_RESP_PD_FIRMWARE_XFER_FAIL:
				case USER_RESP_PD_SEARCHING_WIFI:
				case USER_RESP_PD_WPS_PROCESSING:
				case USER_RESP_PD_WIFI_CONNECTED:
				{
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}
				case USER_RESP_FP_ENROLLED:
				{
					dataPtr = &tempUINT8Var;

					tempUINT8Var = 20;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);

					tempUINT8Var = 50;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);

					tempUINT8Var = 70;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);

					tempUINT8Var = 90;
					break;
				}

				case USER_RESP_CAFE_ALLOW:
				{
					cafeMenuProcInfo.userBalance = 100000000;
					cafeMenuNavigation.totalTxnAmount = 50000;
					cafeMenuNavigation.totalTxnDiscount = 2000;
					DcInfo.authReason = AUTH_CAFE_TXN_REASON_PREPAID_DEVICE;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);

					DcInfo.authReason = AUTH_CAFE_TXN_REASON_PREPAID_DEVICE_DISCNT;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);

					DcInfo.authReason = AUTH_CAFE_TXN_REASON_PREPAID_SERVER;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);

					DcInfo.authReason = AUTH_CAFE_TXN_REASON_PREPAID_SERVER_DISCNT;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					break;
				}
				case USER_RESP_BUZ_AUDIO_IDLE:
				{
					break;
				}
				case USER_RESP_SHOW_2ND_CREDENTIAL:
				case USER_RESP_SHOW_3RD_CREDENTIAL:
				{

					DcInfo.credCnt = THIRD_CREDENTIAL;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.credCnt = SECOND_CREDENTIAL;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}
				case USER_RESP_CAFE_SHOW_YOUR_ID:
				{
					qtGenInfo.ifEscBtn = FALSE;
					dataPtr= &qtGenInfo;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}
				case USER_RESP_SYSTEM_DEFAULT:
				{
					dataPtr = &defaultStatus;
					defaultStatus = SYSTEM_DEFAULT_START;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					defaultStatus = SYSTEM_DEFAULT_COMPLETE;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);

					break;
				}
				case USER_RESP_TWO_STATE_OUPUT:
				{
					outPutData.dispTimeOut = AUDIO_VISUAL_TEST_SLEEP_TIME;
					for(outPutData.testState = SYSTEM_TEST_DOOR_RELAY; outPutData.testState <= SYSTEM_TEST_AUX_RELAY; outPutData.testState++)
					{
						for(outPutData.output = IO_INACTIVE; outPutData.output <=  IO_ACTIVE; outPutData.output++)
						{
							SetAudioVisual(userResp, INTERNAL_PORT, (SYS_TEST_OUTPUT_DATA_t *)(&outPutData) );
							sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
						}
					}
					continue;
				}
				case USER_RESP_APPLY_TWO_STATE_INPUT:
				{
					inputData.testState = SYSTEM_TEST_DOOR_TMPR;
					for(inputData.expectedInput = IO_INACTIVE; inputData.expectedInput <=  IO_ACTIVE; inputData.expectedInput++)
					{
						SetAudioVisual(userResp, INTERNAL_PORT, &inputData);
						sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					}
					inputData.expectedInput = IO_INACTIVE;
					for(inputData.testState = SYSTEM_TEST_EXT_TMPR; inputData.testState <= SYSTEM_TEST_EXT_SWITCH; inputData.testState++)
					{
						SetAudioVisual(userResp, INTERNAL_PORT, &inputData);
						sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					}
					continue;
				}
				case USER_RESP_APPLY_FOUR_STATE_INPUT:
				{
					inputData.testState = SYSTEM_TEST_DOOR_SENSE;
					for(inputData.expectedInput = IO_INACTIVE; inputData.expectedInput <  MAX_IO_STATUS; inputData.expectedInput++)
					{
						SetAudioVisual(userResp, INTERNAL_PORT, &inputData);
						sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					}
					inputData.expectedInput = IO_INACTIVE;
					for(inputData.testState = SYSTEM_TEST_AUX_IN; inputData.testState <= SYSTEM_TEST_AUX_IN; inputData.testState++)
					{
						SetAudioVisual(userResp, INTERNAL_PORT, &inputData);
						sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					}
					continue;
				}

				case USER_RESP_WRONG_FOUR_STATE_INPUT:
				{
					inputData.testState = SYSTEM_TEST_DOOR_SENSE;
					for(inputData.expectedInput = IO_INACTIVE; inputData.expectedInput <  MAX_IO_STATUS; inputData.expectedInput++)
					{
						inputData.detectedInput = (MAX_IO_STATUS - inputData.expectedInput - 1);
						SetAudioVisual(userResp, INTERNAL_PORT, &inputData);
						sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					}
					continue;
				}
				case USER_RESP_ACCESS_ALLOWED:
				{

					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.showDoorModeOnAllowF = ENABLED;
					DcInfo.readerMode = ENTRY_READER;
					DcInfo.authReason = AUTH_PASSBACK_ENTRY_SOFT_VIOLATION;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_PASSBACK_EXIT_SOFT_VIOLATION;
					DcInfo.readerMode = EXIT_READER;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_ALLOW_USER_SOFT_ROUTE_ACCESS;
					SetRestrictAccess(DcInfo.userIdx, TRUE);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);

					break;
				}
				case USER_RESP_ACCESS_DENIED:
				{
					DcInfo.authReason = AUTH_USER_BLOCKED;
					SysBasicPara.application = TA;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					SysBasicPara.application = CAFETERIA;
					SetBlockForCafe(DcInfo.userIdx, BLOCKED_MONTHLY_CONSUPTION);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					SetBlockForCafe(DcInfo.userIdx, BLOCKED_DAILY_LIMIT);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					SetBlockForCafe(DcInfo.userIdx, BLOCKED_INSUFFICIENT_BALANCE);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_FIRST_IN;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_USER_DISABLED;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_INVALID_INPUT;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_INVALID_ROUTE_INPUT;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_INVALID_ROUTE_ACCESS;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_CAFE_TXN_REASON_INVALID_CREDENTIAL;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_CARD_READ_FAILED;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_ROUTE_ACCESS_CARD_WRITE_FAILED;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_CAFE_TXN_REASON_CARD_READ_FAIL;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_INVALID_PIN;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_INVALID_FINGER;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_INVALID_CARD;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_FP_MATCHING_TIMEOUT;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_NO_ACCESS;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_PASSBACK_ENTRY_HARD_VIOLATION;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_PASSBACK_EXIT_HARD_VIOLATION;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_TWO_PERSON;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_OCCUPANCY_VIOLATION;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_DOOR_LOCK;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_VALIDITY_EXPIRED;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_EVENT_LOG_FAIL;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_CAFE_TXN_REASON_INSUFFICIENT_BALANCE;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_CAFE_TXN_REASON_CARD_WRITE_FAIL;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_CAFE_TXN_REASON_NO_ITEM_SELECTED;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_ZERO_OCCUPANCY_NOT_ALLOWED_1;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_ZERO_OCCUPANCY_NOT_ALLOWED_2;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					SysBasicPara.application = TA;
					DcInfo.authReason = AUTH_NO_ACCESS;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}


				case USER_RESP_SHOW_CARD:
				{
					setEnrollInputPera(ENROLL_CATEGORY_USER);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					setEnrollInputPera(ENROLL_CATEGORY_SP_FN);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}


				case USER_RESP_NEXT_CARD:
				{
					setEnrollInputPera(ENROLL_CATEGORY_USER);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					setEnrollInputPera(ENROLL_CATEGORY_SP_FN);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}
				case USER_RESP_REMOVE_FINGER:
				{
					CleanUpTempUserFP();
					setEnrollActivityPera();
					DcInfo.curState = DC_IDLE_STATE;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.curState = DC_ENROLLMENT_STATE;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}


				case USER_RESP_GET_USER_INPUT:
				{
					DcInfo.splFnId = ENROL_MODE_DELETE_DATA;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.splFnId = ENROL_USER;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.splFnId = ENROL_SPECIAL_CARDS;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					break;
				}
				case USER_RESP_GET_USER_CREDENTIAL:
				{
					DcInfo.splFnId = CLK_IN_ENTRY_OFFICIAL_WORK;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}


				case USER_RESP_SPL_FN_ACTIVATED:
				{
					DcInfo.splFnId = LOCK_DOOR;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.splFnId = UNLOCK_DOOR;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.splFnId = NORMAL_DOOR;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.splFnId = CLEAR_SYS_ALARM;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.splFnId = LATE_START_MARKING_ACT;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.splFnId = LATE_START_MARKING_DEACT;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.splFnId = EARLY_CLOSING_MARKING_ACT;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.splFnId = EARLY_CLOSING_MARKING_DEACT;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);

					break;
				}
				case USER_RESP_SPL_FN_IS_ACTIVE:
				{
					DcInfo.multiUserSplCode = LATE_IN_ALLOWED;
					SetAudioVisual(USER_RESP_SPL_FN_IS_ACTIVE,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.multiUserSplCode = EARLY_OUT_ALLOWED;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					break;
				}

				case USER_RESP_PLACE_FINGER_AGAIN:
				{

					setEnrollActivityPera();
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}


				case USER_RESP_BACKUP_UPDATE_PROGRESS:
				{
					dataPtr = &statusTmpVar;
					statusTmpVar.buType = BU_TYPE_FIRMWARE;
					statusTmpVar.prog = 50;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					continue;
				}
				case USER_RESP_BACKUP_UPDATE_NO_EVENT:
				case USER_RESP_BACKUP_UPDATE_XFER_FAILED:
				case USER_RESP_BACKUP_UPDATE_XFER_SUCCESS:
				case USER_RESP_BACKUP_UPDATE_NO_MENORY:
				case USER_RESP_BACKUP_UPDATE_INVALID_DEVICE:
				case USER_RESP_BACKUP_UPDATE_FILE_NOT_FOUND:
				case USER_RESP_BACKUP_UPDATE_FW_FILE_NOT_FOUND:
				case USER_RESP_BACKUP_UPDATE_USB_NOT_FOUND:
				case USER_RESP_FIRMWARE_UPLOAD_STATUS_SUCCESS:
				case USER_RESP_FIRMWARE_UPLOAD_STATUS_FAIL:
				case USER_RESP_BACKUP_UPDATE_CHECKSUM_FAIL:
				case USER_RESP_BACKUP_UPDATE_DOWNGRAGE_CNTRL:
				{
					dataPtr = &statusTmpVar;
					statusTmpVar.buType = BU_TYPE_FIRMWARE;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}

				case USER_RESP_USB_DISCONNECTED:
				{
					evtDataTmp.status = USB_DISCONNECTED;
					dataPtr = &(evtDataTmp.status);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}
				case USER_RESP_USB_CONNECTED:
				{
					evtDataTmp.status = USB_CONNECTED;
					dataPtr = &(evtDataTmp.status);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}
				case USER_RESP_USB_INVALID_FS:
				{
					evtDataTmp.status = USB_INVALID_FS;
					dataPtr = &(evtDataTmp.status);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}



				case USER_RESP_CAFE_CMD_SUCCESS:
				{
					CafeSetCmd.usrBalance = 100000000;
					CafeSetCmd.amount = 999999;
					CafeSetCmd.cmd = CAFE_RECHARGE_TRANSACTION;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					CafeSetCmd.cmd = CAFE_RESET_TRANSACTION;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					CafeSetCmd.cmd = CAFE_READ_BAL_TRANSACTION;
					SetUserPaymentModeForCafe(DcInfo.userIdx, POSTPAID);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					SetUserPaymentModeForCafe(DcInfo.userIdx, PREPAID_SERVER_SIDE);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					SetUserPaymentModeForCafe(DcInfo.userIdx, PREPAID_DEVICE_SIDE);
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}
				case USER_RESP_CAFE_CMD_FAIL:
				{
					CafeSetCmd.cmd = CAFE_RECHARGE_TRANSACTION;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					CafeSetCmd.cmd = CAFE_RESET_TRANSACTION;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					CafeSetCmd.cmd = CAFE_READ_BAL_TRANSACTION;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}


				case USER_RESP_OPEN_DOOR_WEB_API_COMMAND:
				{
					DispDoorModeF = FALSE;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DispDoorModeF = ENABLED;
					DoorEntryExitMode = ENTRY_READER;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DoorEntryExitMode = EXIT_READER;
					SetAudioVisual(userResp,  INTERNAL_PORT, dataPtr);
					break;
				}


				case USER_RESP_VAM_ACCESS_ALLOWED:
				{
					DcInfo.vamConfigIndex = 0;
					VamConfig[DcInfo.vamConfigIndex].passId = 123;
					DcInfo.showDoorModeOnAllowF = ENABLED;
					DcInfo.readerMode = ENTRY_READER;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.readerMode = EXIT_READER;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.showDoorModeOnAllowF = DISABLED;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					DcInfo.authReason = AUTH_VAM_ALLOWED_SOFT_ROUTE;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					break;
				}


				case USER_RESP_VAM_SHOW_2ND_CREDENTIAL:
				case USER_RESP_VAM_SHOW_3RD_CREDENTIAL:
				case USER_RESP_VAM_DOOR_LOCK:
				case USER_RESP_VAM_VALIDITY_EXPIRED:
				case USER_RESP_VAM_CRED_INVALID_SEQ:
				case USER_RESP_VAM_INVALID_CARD:
				case USER_RESP_VAM_INVALID_SAMRT_ROUTE_ACCESS:
				case USER_RESP_VAM_INVALID_SMART_ROUTE_INPUT:
				case USER_RESP_VAM_SMART_ROUTE_CARD_WRITE_FAILED:
				case USER_RESP_VAM_NO_ACCESS:
				case USER_RESP_VISITOR_ACCESS_DENY:
				{
					DcInfo.vamConfigIndex = 0;
					VamConfig[DcInfo.vamConfigIndex].passId = 123;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					break;
				}


				case USER_RESP_OCCUPANCY_WAIT_FOR_NEXT_USER:
				{
					DcInfo.userIdx = 0;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					break;

				}

				case USER_RESP_ACCESS_ALLOWED_WITH_JOBCODE:
				{
					DcInfo.userIdx = 0;
					DcInfo.selectedJobIdx = 0;
					JcmConfig[DcInfo.selectedJobIdx].refId = NONE_JOB_REF_ID;
					SetAudioVisual(userResp, INTERNAL_PORT, dataPtr);
					sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
					JcmConfig[DcInfo.selectedJobIdx].refId = 0;
					strcpy(JcmConfig[DcInfo.selectedJobIdx].jobCode, "abcd");
					break;
				}


		}sleep(AUDIO_VISUAL_TEST_SLEEP_TIME);
	}
SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, dataPtr);

}
