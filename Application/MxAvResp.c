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
//   Created By   : Mukesh Savaliya
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
#include <math.h>

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
#include "MxPDDcTask.h"
#include "MxCommTask.h"
#include "MxCafeScreenMirror.h"
#include "MxDcSpecialFunc.h"
#include "MxPanelCommTask.h"
#include "MxBluetooth.h"
#include "MxAttendanceDetail.h"
#include "MxSplFnScheduleConfig.h"
#ifdef PUSH_API_SUPPORTED
#include "MxThirdPartyServerTask.h"
#endif

#include "MxFaceRecognitionConfig.h"
#include "MxTemperatureSensor.h"
#include "MxDcTaskCommon.h"
#include "MxFrLinkedLists.h"
#include "MxReaderGuiMessage.h"
#include "MxUserConfig.h"
#include "MxCafeteria.h"
#include "HttpReqFSM.h"
#include "MxRegistryFile.h"
#include "MxTempUserTask.h"

//******** Extern Variables **********
extern uint16_t					curProcFaceUserIdx;
extern uint32_t 				lastIdleTimeTick;
extern DOOR_ACTIVITY_INFO_t 	DcInfo;
extern SYS_BASIC_PARA_t			SysBasicPara;
extern SYS_ADVANCE_PARA_t		SysAdvPara;
extern VISITOR_MANAGEMENT_t		Visitor[];
extern DOOR_MONITOR_t			DcMonitorControl;
extern MMC_INFO_t 				MmcInfo;
extern USB_INFO_t				UsbInfo;
extern SERVER_PARA_t			ServerParameters;
extern CAFE_MENU_NAVIGATION_t 	cafeMenuNavigation;
extern CAFE_CMD_DETAIL_t   		CafeSetCmd;
extern CAFE_MENU_PROC_INFO_t	cafeMenuProcInfo;
extern CAFE_RELOAD_e			reloadCafeMenuF;
extern ENROLLMENT_TEMP_INPUTS_t EnrollmentTempInputs;
extern ENROLLMENT_SCREEN_e 		currentEnrollScreen;
extern BOOL						isCurrentEnrllmentSourceQT;
extern CHARPTR					IoStatus[];
extern COMM_STATE_e				CommState;
extern STATUS_BAR_INFO_t		CurrStatusbarData;
extern DEGRADE_STATE_e			DegradeState;
extern VAM_CONFIG_t				VamConfig[MAX_VAM_CONFIG];
extern JCM_CONFIG_t				JcmConfig[MAX_JCM_CONFIG+3]; //+3 for 'None', 'Default' and 'Continue' Job
extern READER_MODE_e			DoorEntryExitMode;
extern BOOL						DispDoorModeF;
extern SMART_CARD_KEY_UPDATE_t	SmartCardKeyUpdateDetail;
extern SERVER_MODE_CONFIG_t		ServerModeCfg;
extern COMM_PORT_e				acmsCurActiveInterface;
extern WIFI_PARA_t				WifiParameters;
extern CAFE_POLICY_CHECK_CMD_MSG_INFO_t cafePolicyCheckResp;
extern CAFE_ITEM_CONFIG_t		CafeItemConfig[MAX_CAFE_ITEM_CODE];
extern CONNECT_TO_PANEL_CONFIG_t	ConnectToPanelPara;
extern UINT32						DetectedUserFaceMatchScore;
extern BRIGHTNESS_DETAIL_t			DisplayBrightness;
extern struct tm 					CurrTimeDate;
extern CHAR 						BenchmarkFWzip[BENCHMARK_FW_STR_LEN];

extern FR_SERVER_CONFIG_t			FRServerConfig;

extern TEMPERATURE_SENSOR_INFO_t 	tempSensorInfo;
extern READERS_PARAMETER_t			ReadersParaConfig;
extern const DYNAMIC_TEMP_SCREEN_INFO_QT_t deflQtDynamicTempScreenInfo;
extern BOOL							FRStreamDisplayF;
extern CAFE_OFFLINE_ITEM_RESTRICT_INFO_t cafeOfflineItemRestrictInfo;
extern UINT16				AcmsPollDuration;
extern const TEMP_USER_COMMON_QT_t 			defltQtTempUsrCommon;
extern PASSWORD_t			  	Password;

//******** Defines and Data Types ****
typedef enum
{
	CAFE_POLICY_ITEM_RESPCODE_ALLOWED = 0,				//0
	CAFE_POLICY_ITEM_RESPCODE_DENIED_DAILY_LIMIT,		//1
	CAFE_POLICY_ITEM_RESPCODE_DENIED_MONTHLY_LIMIT,		//2
	CAFE_POLICY_ITEM_RESPCODE_ALLOWED_WITHOUT_DISCOUNT,	//3
	CAFE_POLICY_ITEM_RESPCODE_DENIED_PREORDER_LIMIT,	//4

	MAX_CAFE_POLICY_ITEM_RESPCODE
}CAFE_POLICY_ITEM_RESPCODE_e;
//******** Function Prototypes *******
static CHARPTR GetInvalidInputMsg(void);

//******** Global Variables **********
TIMER_HANDLE 		BrightnessTmrHandle;
TIMER_HANDLE 		temperatureTmrHandle;
TIMER_HANDLE        statusBarTmrHandle = INVALID_TIMER_HANDLE;
//******** Static Variables **********

//TODO: final review
#define DISP_PROGRESS_FIELD_LEN		QT_MSG_SIZE
#define MAX_LCD_IDLE_TIMER			255

// String to be displayed in status field
static MSG_MULTI_LANGUAGE_STRING_e fpQualityStr[MAX_FP_QUALITY_INFO] =
{
	MSG_WEAK_656,
	MSG_MODERATE_358,
	MSG_STRONG_589,
	MSG_STRONGEST_590,
};

static MSG_MULTI_LANGUAGE_STRING_e buProgStrPtr[MAX_BU_TYPE] =
{
	MSG__1, /*DISP_CLEAR_STRING*/
	MSG_UPGRADING_FIRMWARE_747,
	MSG_RESTORING_CONFIGURATION_822,
	MSG_WRITING_EVENTS_668,
};

static MSG_MULTI_LANGUAGE_STRING_e buTypeLabelStrPtr[MAX_BU_TYPE] =
{
	MSG__1,/*DISP_CLEAR_STRING*/
	MSG_FIRMWARE_UPGRADE_245,
	MSG_RESTORE_CONFIGURATION_497,
	MSG_WRITE_EVENTS_667,
};

static MSG_MULTI_LANGUAGE_STRING_e invalidInputStr[MAX_CREDENTIAL_TYPE] =
{
	CRED_TYPE(GEN_ARG2_FROM_2)
};

MSG_MULTI_LANGUAGE_STRING_e ActivateSplFnStr[MAX_SPECIAL_FUNC] =
{
	MSG_OFFICIAL_WORK_IN_411,
	MSG_OFFICIAL_WORK_OUT_412,
	MSG_SHORT_LEAVE_IN_550,
	MSG_SHORT_LEAVE_OUT_551,
	MSG_REGULAR_IN_492,
	MSG_REGULAR_OUT_493,
	MSG_BREAK_END_739,
	MSG_BREAK_START_392,
	MSG_OVER_TIME_IN_416,
	MSG_OVER_TIME_OUT_417,
	MSG_ENROLL_USER_195,
	MSG_ENROLL_SPECIAL_CARDS_192,
	MSG_DELETE_CREDENTIALS_129,
	MSG_LATE_IN_START_316,
	MSG_LATE_IN_STOP_317,
	MSG_EARLY_OUT_START_182,
	MSG_EARLY_OUT_STOP_183,
	MSG_DOOR_LOCK_166,
	MSG_DOOR_UNLOCK_176,
	MSG_DOOR_NORMAL_169,
	MSG_DOOR_CLEAR_ALARM_158,
	MSG_ITEM_SOLD_OUT_305,
	MSG_ITEM_AVAILABLE_304,
};

const MSG_GEN_INFO_QT_t defltQtGenInfo =
{
	.line1 = "",
	.line2 = "",
	.line3 = "",
	.line4 = "",
	.line5 = "",
	.imageIdx = 0,
	.ifPinEntryBtn = FALSE,
	.ifEscBtn = FALSE,
};

const USER_COMMON_QT_t defltQtUsrCommon =
{
	.ifUserInfo = TRUE,
	.cafeSepF = FALSE,
	.user	 	= {"", INVALID_USER_ID, "", FALSE, ""},	//alphaNum Id, Id, name, photo, designation
	.genInfo	= {"", "", "", "", "", 0, FALSE , FALSE},	// line1, line2, line3,line4,line5,imageIndex,pinEntry Btn
	.accessTime = {""},
	.greetingInfo	= {FALSE, "", ""},
	.matchingScore	=	{""},
	.temperature = {""},
	.temperatureUnit = UNIT_CELCIUS,
	.temperatureViolation = FALSE,
	.faceMaskDecision = QT_FACE_MASK_DECISION_MAX,
	// .isJobCodeAllowScreen = FALSE,
};

const USER_DENIED_QT_t defltQtUsrDenied =
{
	.ifUserInfo = TRUE,
	.user	 	= {"", INVALID_USER_ID, "", FALSE, ""},	//alphaNum Id, Id, name, photo, designation
	.genInfo	= {"", "", "", "", "", 0, FALSE , FALSE},	// line1, line2, line3,line4,line5,imageIndex,pinEntry Btn
};

const CAFE_POLICY_USER_DENIED_QT_t defltQtCafeUsrDenied =
{
	.ifUserInfo = TRUE,
	.user	 	= {"", INVALID_USER_ID, "", FALSE, ""},	//alphaNum Id, Id, name, photo, designation
	.genInfo	= {"", "", "", "", "", 0, FALSE , FALSE},	// line1, line2, line3,line4,line5,imageIndex,pinEntry Btn
};

const USER_ALLOWED_QT_t defltQtUsrAllowed =
{
	.user	 	= {"", INVALID_USER_ID, "", FALSE, ""},	//alphaNum Id, Id, name, photo, designation
	.genInfo		= {"", "", "", "", "", 0, FALSE , FALSE},	// line1, line2, line3, line4, line5,imageIndex,pinEntry Btn
	.jobCodeInfo	= {"", "", ""},						// Job code info
	.allowTime		= {""},
};

static const CAFE_USER_ALLOWED_QT_t defltQtCafeAllowed =
{
	.user	 	= {"", INVALID_USER_ID, "", FALSE, ""},	//alphaNum Id, Id, name, photo, designation
	.genInfo		= {"", "", "", "", "", CAFE_ALLOW_SCREEN_IMAGE_USER_ALLOWED, FALSE , FALSE},
	.cafeSepF		= FALSE,
};

const FUNC_STATUS_QT_t deflQtFuncStatusInfo =
{
	.splFunc = "",
	.ifUserInfo = FALSE,
	.user	 	= {"", INVALID_USER_ID, "", FALSE, ""},	//alphaNum Id, Id, name, photo, designation
	.genInfo	= {"", "", "", "", "", 0, FALSE, FALSE},	// line1, line2, line3,line4,line5,imageIndex,pinEntry Btn
	.temperature = {""},
	.temperatureUnit = UNIT_CELCIUS,
	.temperatureViolation = FALSE,
	.faceMaskDecision = QT_FACE_MASK_DECISION_MAX,
};

const DYNAMIC_POPUP_INFO_QT_t	deflQtDynamicPopUpInfo =
{
	.titleText = "",
	.imageIdx = 0,
	.message1 = "",
	.message2 = "",
	.footerHideF = TRUE,
};

const TEMP_USER_POPUP_INFO_QT_t	defltQtTempUserPopUpInfo =
{
	.titleText = "",
	.imageIdx = 0,
	.message1 = "",
	.message2 = "",
	.footerHideF = TRUE,
};

const DYNAMIC_SPL_FUNC_INFO_t   deflQtDynamicSplInfo =
{
	.splFunc = "",
	.splFuncImageIdx = 0,
	.noticeLine = "",
	.ifEscBtn = FALSE,
	.ifPinEntryBtn = FALSE,
};

static const DYNAMIC_VALIDATION_RESPONSE_t deflQtDynamicValidationRespInfo =
{
	.respMsgIdx = 0,
};

const SYSTEM_TEST_INFO_t	deflQtSystemTestInfo =
{
	.titleText = "",
	.message1  = "",
	.message2  = "",
	.message3  = "",
};

#if defined(FR_ADDITIONAL_FEATURE_SUPPORTED)
const DYNAMIC_USER_INFO_SCREEN_QT_t	dfltQtDynamiUserInfo = 
{
	.user	= {"", INVALID_USER_ID, "", FALSE, ""},
	.imageIdx = IMAGE_NONE,
};
#endif

const DYNAMIC_IDLE_SCREEN_INFO_QT_t dfltQtIdleScreenInfo =
{
	.showOverlay = FALSE,
	.showOverlayText = FALSE,
	.overlayText = "",
	.showUserInfo = FALSE,
	.userId = INVALID_USER_ID,
	.name = "",
	.footerIdx = FOOTER_IDLE,
};

// System test Enums
MSG_MULTI_LANGUAGE_STRING_e sysTestStateStr[MAX_SYSTEM_TEST_STATES] =
{
	MSG_IDLE_711,
	MSG_KEYPAD_TEST_710,
	MSG_DOOR_RELAY_709,
	MSG_AUXILIARY_RELAY_781,
	MSG_DOOR_SENSE_171,
	MSG_AUXILIARY_INPUT_780,
	MSG_DOOR_TAMPER_708,
	MSG_EXIT_READER_TAMPER_707,
	MSG_LOCK_TAMPER_706,
	MSG_EXIT_SWITCH_227,
};

static const UINT16 cadResp[MAX_USER_RESP] =
{
	USER_RESPONSE_CMD(GEN_USER_RESP_CMD_NAME)
};

static BOOL UserAllowEntryExitF = TRUE;
//******** Function Definations ******

//*****************************************************************************
//	SetBuzLedResp()
//	Param:
//		IN : ledBuzzResp- led-buzzer response type
//			 respPort	- INTERNAL/EXTERNAL port where AV response to be played
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//		This function updates given AV response on given led-buzzer port.
//
//*****************************************************************************
void SetBuzLedResp(USER_RESP_e userResp, RESPONSE_PORT_e respPort)
{
	if(userResp < MAX_USER_RESP)
	{
		
		if((USER_RESP_DEVICE_INACTIVE == userResp) && (TRUE == getDeviceInactiveStatus()))
		{
			SetLed(cadResp[userResp], respPort);
		}
		else if(cadResp[userResp] < ACCESS_CONTROL_RESP_MAX)
		{
			SetBuzzer(cadResp[userResp], respPort);
			SetLed(cadResp[userResp], respPort);
		}
	}
}

//*****************************************************************************
//	SetAudioVisual()
//	Param:
//		IN : USER_RESPONSE_e	-	Message Id For respective String
//			 RESPONSE_PORT_e	-	INTERNAL/EXTERNAL port
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//		This function prepares and updates audio-visual indications on
//		LED/Buzzer port based on given user response.
//
//*****************************************************************************
void SetAudioVisual(USER_RESP_e userResp, RESPONSE_PORT_e respPort, VOIDPTR dataPtr)
{
	UINT16 						userIdx;
	MSG_GEN_INFO_QT_t	 		qtGenInfo 		= defltQtGenInfo;
	FUNC_STATUS_QT_t			qtFuncStatusInfo = deflQtFuncStatusInfo;
	DYNAMIC_POPUP_INFO_QT_t		qtDynamicInfo = deflQtDynamicPopUpInfo;
	TEMP_USER_POPUP_INFO_QT_t	tempUserPopUpInfo = defltQtTempUserPopUpInfo;
	DYNAMIC_SPL_FUNC_INFO_t		qtDynamicSplFuncInfo = deflQtDynamicSplInfo;
	SYSTEM_TEST_INFO_t			qtSystemTestInfo = deflQtSystemTestInfo;
	INT32 						userBalanceDecimal, userBalanceFraction;
	UINT32 						totalTxnDiscountDecimal, totalTxnDiscountFraction;
	UINT32 						totalTxnAmountDecimal, totalTxnAmountFraction;
	CHAR						dataStr[RX_BUF_MAX];	//buffer to send data with TRANSACTION_COMPLETE to mirror display device
	UINT16						dataLen;
	UINT8						screenType = 2;

	qtFuncStatusInfo.genInfo = defltQtGenInfo;

	SetBuzLedResp(userResp,respPort);

	switch(respPort)
	{
		default:
			break;

		case EXTERNAL_PORT:
		{
			SendGuiMessageToReader(userResp, dataPtr, GUI_MSG_MAX_SCREEN);
		}
		break;

		case INTERNAL_PORT:
		{
			switch(userResp)
			{
				default:
					break;
                case USER_RESP_DEVICE_INACTIVE:
                {
                    SendMessageToQt("", GUI_MSG_DEVICE_INACTIVE);
                    break;
                }

				case USER_RESP_ENTRY_RESTRICTED_DEVICE_INACTIVE:
				{
					USER_COMMON_QT_t deny = defltQtUsrCommon;

					deny.ifUserInfo = FALSE;
					strncpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENTRY_RESTRICTED_214), (3*QT_MSG_SIZE));
					strncpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_RESTRICTED_ACCESS_HOURS_1097), (3*QT_MSG_SIZE));
					deny.genInfo.imageIdx = DENY_SCREEN_CAFE_FAILED;
					SendMessageToQt(&deny, GUI_MSG_USER_DENIED_COMMON_SCREEN);

					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_TOUCH_SCREEN_TEST_START:
				{
					SendMessageToQt("", GUI_MSG_TOUCH_PANEL_TEST_SCREEN);
					SetLcdIdleTimer(SYS_TEST_PROG_TIMEOUT);
					break;
				}

				case USER_RESP_LCD_TEST_START:
				{
					SendMessageToQt("", GUI_MSG_LCD_TEST_SCREEN);
					SetLcdIdleTimer(SYS_LCD_TEST_TIMER);
					break;
				}

				case USER_RESP_ADD_TEMP_USER_CONFIRMATION:
				{
					SendMessageToQt("", GUI_MSG_TEMP_USER_CREATE_CONFIRM_SCREEN);
                    LoadAddTempUserTimer(ADD_TEMP_USER_TIMER);
					break;
				}
				case USER_RESP_ADD_TEMP_USER_NAME:
				{
					SendMessageToQt("", GUI_MSG_TEMP_USER_ENTER_NAME_SCREEN);
                    LoadAddTempUserTimer(MENU_TIMER);
					break;
				}
				case USER_RESP_TEMP_USER_REGISTERED_SUCCESSFULLY:
				{
					TEMP_USER_COMMON_QT_t	*tempUserData;

					tempUserData = (TEMP_USER_COMMON_QT_t *)dataPtr;
					
					SendMessageToQt(tempUserData, GUI_MSG_TEMP_USER_REGISTERED_SUCCESSFULLY);
					TempUserPopUpTimer(TEMP_USER_POPUP_SCREEN_TIMER);
					break;
				}
				case USER_RESP_TEMP_USER_PROCESSING:
				{
					tempUserPopUpInfo.imageIdx = TEMP_USER_POPUP_PROCESSING;
					strcpy(tempUserPopUpInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PROCESSING_467));
					SendMessageToQt(&tempUserPopUpInfo, GUI_MSG_TEMP_USER_POPUP_SCREEN);
					TempUserProcessingTimer(TEMP_USER_PROCESSING_TIMER);
					break;
				}
				case USER_RESP_TEMP_USER_FAILED_TO_CREATE:
				{
					strcpy(tempUserPopUpInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FAILED_TO_CREATE_TEMPORARY_USER_1086));
					tempUserPopUpInfo.imageIdx = TEMP_USER_POPUP_FAILED;
					SendMessageToQt(&tempUserPopUpInfo, GUI_MSG_TEMP_USER_POPUP_SCREEN);
					TempUserPopUpTimer(TEMP_USER_POPUP_SCREEN_TIMER);
					break;
				}
				case USER_RESP_TEMP_USER_ABORTED:
				{
					strcpy(tempUserPopUpInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TEMPORARY_USER_WORKER_CREATION_PROCESS_ABORTED_1091));
					tempUserPopUpInfo.imageIdx = TEMP_USER_POPUP_FAILED;
					SendMessageToQt(&tempUserPopUpInfo, GUI_MSG_TEMP_USER_POPUP_SCREEN);
					TempUserPopUpTimer(TEMP_USER_POPUP_SCREEN_TIMER);
					break;
				}
				case USER_RESP_TEMP_USER_TIMEOUT:
				{
					strcpy(tempUserPopUpInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TIME_OUT_605));
					strcpy(tempUserPopUpInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FAILED_TO_SYNC_USER_CREDENTIALS_892));
					strcpy(tempUserPopUpInfo.message2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRY_AGAIN_LATER_841));
					tempUserPopUpInfo.imageIdx = TEMP_USER_POPUP_TIMEOUT;
					SendMessageToQt(&tempUserPopUpInfo, GUI_MSG_TEMP_USER_POPUP_SCREEN);
					TempUserPopUpTimer(TEMP_USER_POPUP_SCREEN_TIMER);
					break;
				}
				case USER_RESP_TWO_STATE_OUPUT:
				{
					SYS_TEST_OUTPUT_DATA_t	*outPutData;

					outPutData = (SYS_TEST_OUTPUT_DATA_t *)dataPtr;

					strcpy(qtSystemTestInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,sysTestStateStr[outPutData -> testState]));
					sprintf(qtSystemTestInfo.message2, "%s %s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_OUTPUT_712), IoStatus[outPutData -> output] );

					SendMessageToQt(&qtSystemTestInfo, GUI_MSG_SYSTEM_TEST_POPUP_SCREEN);
					SetLcdIdleTimer(outPutData -> dispTimeOut);
					break;
				}

				case USER_RESP_APPLY_TWO_STATE_INPUT:
				{
					SYS_TEST_INPUT_DATA_t	*twoStateData;

					twoStateData = (SYS_TEST_INPUT_DATA_t *)dataPtr;

					strcpy(qtSystemTestInfo.titleText,GetDisplayMessage(FALSE,NULL_CHAR,sysTestStateStr[twoStateData -> testState]));
					sprintf(qtSystemTestInfo.message2, "%s%s", GetDisplayMessage(FALSE,NULL_CHAR, MSG_APPLY_INPUT_824), IoStatus[twoStateData -> expectedInput] );

					SendMessageToQt(&qtSystemTestInfo, GUI_MSG_SYSTEM_TEST_POPUP_SCREEN);
					SetLcdIdleTimer(twoStateData -> dispTimeOut);
					break;
				}

				case USER_RESP_APPLY_FOUR_STATE_INPUT:
				{
					SYS_TEST_INPUT_DATA_t	*fourStateData;

					fourStateData = (SYS_TEST_INPUT_DATA_t *)dataPtr;

					strcpy(qtSystemTestInfo.titleText,GetDisplayMessage(FALSE,NULL_CHAR,sysTestStateStr[fourStateData -> testState]));
					sprintf(qtSystemTestInfo.message2, "%s%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_APPLY_INPUT_824), IoStatus[fourStateData -> expectedInput]);

					SendMessageToQt(&qtSystemTestInfo, GUI_MSG_SYSTEM_TEST_POPUP_SCREEN);
					SetLcdIdleTimer(fourStateData -> dispTimeOut);
					break;
				}

				case USER_RESP_WRONG_FOUR_STATE_INPUT:
				{
					SYS_TEST_INPUT_DATA_t	*fourStateData;

					fourStateData = (SYS_TEST_INPUT_DATA_t *)dataPtr;

					strcpy(qtSystemTestInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,sysTestStateStr[fourStateData -> testState]));
					strcpy(qtSystemTestInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_WRONG_INPUT_716));
					sprintf(qtSystemTestInfo.message2, "%s%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_DETECTED_INPUT_825), IoStatus[fourStateData -> detectedInput]);
					sprintf(qtSystemTestInfo.message3, "%s%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_APPLY_INPUT_824), IoStatus[fourStateData -> expectedInput]);

					SendMessageToQt(&qtSystemTestInfo, GUI_MSG_SYSTEM_TEST_POPUP_SCREEN);
					SetLcdIdleTimer(fourStateData -> dispTimeOut);
					break;
				}

				case USER_RESP_TEST_FAIL:
				{
					strcpy(qtSystemTestInfo.message2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TEST_FAIL_713));

					SendMessageToQt(&qtSystemTestInfo, GUI_MSG_SYSTEM_TEST_POPUP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_TEST_SUCCESS:
				{
					strcpy(qtSystemTestInfo.message2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TEST_SUCCESS_714));

					SendMessageToQt(&qtSystemTestInfo, GUI_MSG_SYSTEM_TEST_POPUP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_SYSTEM_DEFAULT:
				{
					SYSTEM_DEFAULT_STATUS_e *defaultStatus;
					defaultStatus = (SYSTEM_DEFAULT_STATUS_e *)dataPtr;

					switch(*defaultStatus)
					{
						case SYSTEM_DEFAULT_START:
						{
							strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PLEASE_WAIT_444));
							strcpy(qtDynamicInfo.message2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DEFAULTING_126));
							qtDynamicInfo.imageIdx = POPUP_SYSTEM_DEFAULTING;
							SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
							break;
						}

						case SYSTEM_DEFAULT_COMPLETE:
						{
							qtDynamicInfo.imageIdx = POPUP_SYSTEM_DEFAULTED;
							strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SYSTEM_DEFAULTED_596));
							SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
							SetLcdIdleTimer(1);
							break;
						}

						default:
							break;
					}
					break;
				}

				case USER_RESP_SHOW_CARD_API:
				case USER_RESP_KEY_CHANGE_SHOW_CARD:
				{
					BOOL showEscBtnF = *(BOOL *)dataPtr;

					if(userResp == USER_RESP_KEY_CHANGE_SHOW_CARD)
					{
						strcpy(qtGenInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_KEY_CHANGE_854));
					}
					strcpy(qtGenInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_THE_CARD_556));
					qtGenInfo.imageIdx = NOTICE_SHOW_YOUR_CARD;
					qtGenInfo.ifEscBtn = showEscBtnF;
					SendMessageToQt(&qtGenInfo, GUI_MSG_NOTICE_TEXT);

					SetLcdIdleTimer(0);
					break;
				}

				case USER_RESP_KEY_CHANGE_SUCCESS:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_KEY_CHANGE_854));
					qtDynamicInfo.imageIdx = POPUP_SUCCESS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_KEY_CHANGE_SUCCESS_755));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_CHANGING_CARD_KEY:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_KEY_CHANGE_854));
					qtDynamicInfo.imageIdx = POPUP_WRITING_SMART_CARD;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_WRITING_SMART_CARD_669));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

					SetLcdIdleTimer(0);
					break;
				}

				case USER_RESP_KEY_CHANGE_FAIL:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_KEY_CHANGE_854));
					qtDynamicInfo.imageIdx = POPUP_FAILED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_KEY_CHANGE_FAILED_754));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_ADD_USER_FAIL:
				{
					strcpy(qtDynamicInfo.titleText, "");
					qtDynamicInfo.imageIdx = POPUP_FAILED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE, NULL_CHAR, MSG_FAILED_TO_ADD_USER_887));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				// This Response is used in Cafeteria Application
				case USER_RESP_CAFE_SHOW_YOUR_ID:
				{
					qtGenInfo.ifEscBtn = *(BOOL *)dataPtr;

					if( (FRServerConfig.enableFrServer == TRUE) && (SysBasicPara.cafeteriaFrAccessMode == CAFE_FR_ACCESS_MODE_ITEM_SELECTION)
							&& (FALSE == qtGenInfo.ifEscBtn) && (DcInfo.credType[0] != CARD_CREDENTIAL))
					{
						strcpy(qtGenInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CAMERA_CONNECTIVITY_LOST_1019));
						strcpy(qtGenInfo.line3, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_YOUR_ID_561));
					}
					else
					{
						strcpy(qtGenInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_YOUR_ID_561));
					}

					qtGenInfo.imageIdx = NOTICE_SHOW_YOUR_ID;

					SendMessageToQt(&qtGenInfo, GUI_MSG_NOTICE_TEXT);

					SetLcdIdleTimer(CAFE_MENU_WAIT_TIME);
					break;
				}

				// This Response is used in Cafeteria Application
				case USER_RESP_CAFE_SHOW_CARD:
				{
					strcpy(qtGenInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_THE_CARD_556));
					qtGenInfo.imageIdx = NOTICE_SHOW_YOUR_CARD;
					qtGenInfo.ifEscBtn = TRUE;
					SendMessageToQt(&qtGenInfo, GUI_MSG_NOTICE_TEXT);

					SetLcdIdleTimer(CAFE_MENU_WAIT_TIME);
					break;
				}

				case USER_RESP_PRINT_CMD_SENT:
				{
					qtDynamicInfo.imageIdx = POPUP_SUCCESS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PRINT_COMMAND_SENT_465));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_REMOVE_FINGER:
				{
					if(DcInfo.curState == DC_ENROLLMENT_STATE)
					{
						strcpy(qtFuncStatusInfo.splFunc, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
						qtFuncStatusInfo.ifUserInfo = TRUE;
						userIdx = GetUserIdxBeingEnrolled();
						sprintf(qtFuncStatusInfo.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
						qtFuncStatusInfo.user.userId = GetUserIdFromIdx(userIdx);
						strcpy(qtFuncStatusInfo.user.name, GetUserName(userIdx) );
						qtFuncStatusInfo.user.photo = TRUE;
						strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_REMOVE_YOUR_FINGER_494));
						qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_REMOVE_YOUR_FINGER;

						// Update Current Enrollment Screen
						currentEnrollScreen = GET_USER_CREDENTIAL_SCREEN;
						SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);
						SetLcdIdleTimer(ENROLLMENT_TIMER);
					}
					else
					{
					    strcpy(qtGenInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_REMOVE_YOUR_FINGER_494));
						qtGenInfo.imageIdx = NOTICE_REMOVE_YOUR_FINGER_IMAGE;
						SendMessageToQt(&qtGenInfo, GUI_MSG_NOTICE_TEXT);
						SetLcdIdleTimer(MAX_LCD_IDLE_TIMER);
					}

					break;
				}

				case USER_RESP_FP_IDENTIFICATION_GLOBALLY:
				{
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_IDENTIFYING_GLOBALLY_311));
					qtDynamicInfo.imageIdx = POPUP_PROCESSING;
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(MAX_LCD_IDLE_TIMER);
					break;
				}

				// NOTE: Do not load display idle timer as MENU state will take care of
				// what to display upon timeout
				case USER_RESP_MENU_FAILED:
				{
					qtDynamicInfo.imageIdx = POPUP_FAILED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FAIL_745));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				// NOTE: Do not load display idle timer as MENU state will take care of
				// what to display upon timeout
				case USER_RESP_PSWD_MISMATCH:
				{
					qtDynamicInfo.imageIdx = POPUP_FAILED;
					strcpy(qtDynamicInfo.message1 , GetDisplayMessage(FALSE,NULL_CHAR,MSG_PASSWORD_MISMATCH_429));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				case USER_RESP_CAFE_CAFE_USERID_MISMATCH:
				{
					qtDynamicInfo.imageIdx = POPUP_FAILED;
					strcpy(qtDynamicInfo.message1 , GetDisplayMessage(FALSE,NULL_CHAR,MSG_USER_ID_MISMATCH_906));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration); 
					break;
				}

				// NOTE: Do not load display idle timer as MENU state will take care of
				// what to display upon timeout
				case USER_RESP_PSWD_WRONG:
				{
					qtDynamicInfo.imageIdx = POPUP_FAILED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_PASSWORD_288));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				case USER_RESP_PSWD_BLANK:
				{
					qtDynamicInfo.imageIdx = POPUP_FAILED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PASSWORD_CANNOT_BE_BLANK_815));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				// NOTE: Do not load display idle timer as MENU state will take care of
				// what to display upon timeout
				case USER_RESP_PSWD_SUCCESS:
				{
					qtDynamicInfo.imageIdx = POPUP_SUCCESS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PASSWORD_CHANGED_SUCCESSFULLY_814));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				case USER_RESP_PASSWORD_DEFAULTED:
				{
					qtDynamicInfo.imageIdx = POPUP_SUCCESS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PASSWORD_DEFAULTED_428));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				// NOTE: Do not load display idle timer as MENU state will take care of
				// what to display upon timeout
				case USER_RESP_MENU_LOGIN_FAILED:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_LOGIN_FAILED_328));
					qtDynamicInfo.imageIdx = POPUP_INAVALID_PASSWORD;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_PASSWORD_288));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

            case USER_RESP_MENU_LOGIN_RESTRICTED:
            {
                UINT8 msgLen = 0;
                CONFIG_USER_TYPE_e userType = *(CONFIG_USER_TYPE_e *)dataPtr;
                strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_LOGIN_FAILED_328));
                qtDynamicInfo.imageIdx = POPUP_INAVALID_PASSWORD;

                if(Password.DispPwdLoginRestrictTimer[userType] == INVALID_TIMER_HANDLE)
                {
                    strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_PASSWORD_288));

                    msgLen += snprintf(qtDynamicInfo.message2, QT_MSG_SIZE, "%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_YOU_HAVE_1098));
                    snprintf(qtDynamicInfo.message2+msgLen, QT_MSG_SIZE, " %d %s", GetDispPwdAttemptCount(userType), GetDisplayMessage(FALSE,NULL_CHAR,MSG_MORE_LOGIN_ATTEMPT_S_1099));
                }
                else
                {
                    msgLen += snprintf(qtDynamicInfo.message1, QT_MSG_SIZE, "%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_ACCOUNT_LOCKED_TRY_AFTER_1100));
                    snprintf(qtDynamicInfo.message1+msgLen, QT_MSG_SIZE, " %d %s", GetDispPwdAttemptCount(userType), GetDisplayMessage(FALSE,NULL_CHAR,MSG_MINUTE_S_1101));
                }
                SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
                break;
            }

				// NOTE: Do not load display idle timer as MENU state will take care of
				// what to display upon timeout
				case USER_RESP_SEARCHING_WIFI:
				{
					strcpy(qtDynamicInfo.titleText, "");
					qtDynamicInfo.imageIdx = POPUP_SEARCHING_FOR_WIFI;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SEARCHING_FOR_WIFI_ACCESS_POINTS_813));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				// NOTE: Do not load display idle timer as MENU state will take care of
				// what to display upon timeout
				case USER_RESP_WIFI_AP_NOT_FOUND:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_NO_ACCESS_POINTS_FOUND_387));
					if( CommState == SLAVE_DEGRADE )
					{
						qtDynamicInfo.imageIdx = PD_POPUP_NO_ACESSPOINTS_FOUND;
					}
					else
					{
						qtDynamicInfo.imageIdx = POPUP_NO_ACESSPOINTS_FOUND;
					}
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				case USER_RESP_SHOW_2ND_CREDENTIAL:
				case USER_RESP_SHOW_3RD_CREDENTIAL:
				{
					userIdx = DcInfo.userIdx;
					qtFuncStatusInfo.ifUserInfo = TRUE;
					sprintf(qtFuncStatusInfo.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
					qtFuncStatusInfo.user.userId = GetUserIdFromIdx(userIdx);
					strcpy(qtFuncStatusInfo.user.name, GetUserName(userIdx) );
					qtFuncStatusInfo.user.photo = TRUE;

					// If waiting for second credential, display "second credential"
					if(DcInfo.credCnt == SECOND_CREDENTIAL)
					{
						qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_SHOW_SECOND_CREDENTIAL;
						strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_2ND_CREDENTIAL_552));
					}
					// If waiting for third credential, display "third credential"
					else //It's THIRD_CREDENTIAL
					{
						qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_SHOW_THIRD_CREDENTIAL;
						strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_3RD_CREDENTIAL_553));
					}

					qtFuncStatusInfo.genInfo.ifPinEntryBtn = TRUE;
					SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);

					SetLcdIdleTimer(SysAdvPara.multiAccessTimer);

					break;
				}

				case USER_RESP_DISPLAY_IDLE_SCREEN:
				{
					DisplayIdleScreen(0);
					break;
				}

				case USER_RESP_IDLE:
 				{
					DisplayIdleScreen(0);
  					break;
 				}

				case USER_RESP_DISPLAY_GOING_IN_SCREEN:
 				{
					SendMessageToQt("", GUI_MSG_GOING_IN_SCREEN);
					SetLcdIdleTimer(MENU_TIMER);
  					break;
 				}

				case USER_RESP_DISPLAY_GOING_OUT_SCREEN:
 				{
					SendMessageToQt("", GUI_MSG_GOING_OUT_SCREEN);
					SetLcdIdleTimer(MENU_TIMER);
 					break;
 				}

				case USER_RESP_DISPLAY_MENU_SCREEN:
 				{
					SendMessageToQt("", GUI_MSG_MENU_SCREEN);
					SetLcdIdleTimer(MENU_TIMER);
 					break;
 				}

 				case USER_RESP_BUZ_AUDIO_IDLE:
				{
					break;
				}

				case USER_RESP_PROCESS:
				{
 					break;
				}

				case USER_RESP_TIMEOUT:
				{
					strcpy(qtDynamicInfo.titleText, "");
					qtDynamicInfo.imageIdx = POPUP_TIMEOUT;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TIME_OUT_605));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

					if ((DcInfo.curState == DC_MENU_STATE)
							&& (tempSensorInfo.isAmbientTempRequired == TRUE)
							&& (tempSensorInfo.isTempQueryInBackground == FALSE))
					{
						SetTemperatureIdleTimer(LCD_IDLE_TIMER);
					}
					else
					{
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					}
					break;
				}

				case USER_RESP_ACCESS_ALLOWED:
				case USER_RESP_ACCESS_EVENT_REGISTERED:
				{
					USER_COMMON_QT_t allow = defltQtUsrCommon;
					CHARPTR		greetingMsgPtr;

					greetingMsgPtr = GetGreetingMsg(DcInfo.userIdx);

					if(greetingMsgPtr != NULL)
					{
						sprintf(allow.genInfo.line1,"%s",greetingMsgPtr);
					}

					sprintf(allow.user.alphaNumId, "%s", GetAlphaNumId(DcInfo.userIdx) );
					allow.user.userId = GetUserIdFromIdx(DcInfo.userIdx);
					strcpy(allow.user.name, GetUserName(DcInfo.userIdx) );
					allow.user.photo = TRUE;
					snprintf(allow.user.designation, DESIGNATION_SIZE, "%s", DcInfo.designation);

					// Temperature
					snprintf(allow.temperature, MAX_TEMPERATURE_LENGTH, "%.1f", DcInfo.usrTemperature);
					allow.temperatureUnit = SysAdvPara.temperatureLogCnfg.tempUnit;

					/* Set user exceeds temperature threshold */
					if (DcInfo.usrExceedTempThreshold == TRUE)
					{
						allow.temperatureViolation = TRUE;
					}
#if defined(FACE_MASK_COMPULSION_ENABLE)
                    if((DcInfo.duressF == NO) && ((IsGroupFrApplicable() == NO) &&
                            (SysAdvPara.faceMaskComp_Enable == TRUE)))
                    {
                        allow.faceMaskDecision = DcInfo.isFaceMaskViolated ? QT_FACE_MASK_DECISION_ACCESS_ALLOWED_SOFT :
                            QT_FACE_MASK_DECISION_ACCESS_ALLOWED;
                    }
#endif

					SetUserAllowEntryExitModeFlag(TRUE);

					if(IsUserAccessRestricted(DcInfo.userIdx))
					{
						strcpy(allow.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_EVENT_REGISTERED_222));
					}
					else
					{
						if( (SysAdvPara.sequentialInOut == ENABLED) &&
								(DcInfo.userIdx < SSA_USER_IDX) &&
								(IsDoorModeSelectionRequired() == NO) &&
								(GetCurrSplCode() == USER_SPECIAL_CODE_NONE) &&
								(GetActiveScheduleSplFnCode(&CurrTimeDate) == USER_SPECIAL_CODE_NONE) &&
								(SysBasicPara.doorModeSchdlEnblF == DISABLED))
						{
							if(DcInfo.doorEvtMode == USER_ENTRY)
							{
								strcpy(allow.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENTRY_ALLOWED_758));
							}
							else
							{
								SetUserAllowEntryExitModeFlag(FALSE);
								strcpy(allow.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_EXIT_ALLOWED_759));
							}
						}
						else if(DcInfo.showDoorModeOnAllowF == ENABLED)
						{
							if(DcInfo.readerMode == ENTRY_READER)
							{
								strcpy(allow.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENTRY_ALLOWED_758));
							}
							else if(DcInfo.readerMode == EXIT_READER)
							{
								SetUserAllowEntryExitModeFlag(FALSE);
								strcpy(allow.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_EXIT_ALLOWED_759));
							}
						}
					}

					switch(DcInfo.authReason)
					{
					case AUTH_PASSBACK_ENTRY_SOFT_VIOLATION:
						strcpy(allow.genInfo.line3, GetDisplayMessage(FALSE,NULL_CHAR,MSG_EXIT_NOT_REGISTERED_226));
						break;

					case AUTH_PASSBACK_EXIT_SOFT_VIOLATION:
						strcpy(allow.genInfo.line3, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENTRY_NOT_REGISTERED_213));
						break;

					case AUTH_ALLOW_USER_SOFT_ROUTE_ACCESS:
						strcpy(allow.genInfo.line3, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ACCESS_ROUTE_VIOLATED_23));
						break;

					case AUTH_VISITOR_ALLOWED_SURRENDER_PASS:
						strcpy(allow.genInfo.line3, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PASS_SURRENDERED_867));
						break;

					default:
						break;
					}

					if(userResp == USER_RESP_ACCESS_ALLOWED)
					{
						allow.genInfo.imageIdx = ALLOW_SCREEN_ACCESS_ALLOWED_IMAGE;	//Image Index
					}
					else
					{
						allow.genInfo.imageIdx = ALLOW_SCREEN_EVENT_REGISTERED_IMAGE;
					}

					if(DcInfo.showAttendanceDetailTimer != INVALID_TIMER_HANDLE)
					{
						// Show only attendance detail screen.
						ShowAtdncDtlTimerTimeout(DcInfo.userIdx);
					}
					else
					{
						SendMessageToQt(&allow, GUI_MSG_USER_ALLOWED_COMMON_SCREEN);
						SetLcdIdleTimerMilliSec(SysBasicPara.accessAllowScreenDuration);
					}
					break;
				}

				case USER_RESP_CAFE_NOT_SERVING_NOW:
				{
					SendMessageToQt("", GUI_MSG_CAFE_NOT_SERVING_SCREEN);
					break;
				}

				case USER_RESP_CAFE_ALLOW:
				{
					USER_COMMON_QT_t cafeAllow = defltQtUsrCommon;

					if(DcInfo.authReason == AUTH_CAFE_TXN_REASON_PREPAID_SERVER || DcInfo.authReason == AUTH_CAFE_TXN_REASON_PREPAID_SERVER_DISCNT)
					{
						FLOAT64 tempAmount = (FLOAT64)(((INT64)cafeMenuNavigation.userCafeBalance - (INT64)cafeMenuNavigation.totalTxnAmount))/100;
						if(tempAmount < (MAX_CAFE_USER_BALANCE_NEGATIVE) || tempAmount > (MAX_CAFE_USER_BALANCE_POSITIVE))
						{
							EPRINT(DC_LOG,"After Purchase Value Is Going Out Of Range");
							cafeMenuNavigation.userCafeBalance = 0;
							cafeMenuNavigation.userCafeBalanceReceivedF = FALSE;
						}
					}
					else if(DcInfo.authReason == AUTH_CAFE_TXN_REASON_POSTPAID || DcInfo.authReason == AUTH_CAFE_TXN_REASON_POSTPAID_DISCNT)
					{
						FLOAT64 tempAmount = (FLOAT64)(((INT64)cafeMenuNavigation.userCafeBalance + (INT64)cafeMenuNavigation.totalTxnAmount))/100;
						if(tempAmount < (MAX_CAFE_USER_BALANCE_NEGATIVE) || tempAmount > (MAX_CAFE_USER_BALANCE_POSITIVE))
						{
							EPRINT(DC_LOG,"After Purchase Value Is Going Out Of Range");
							cafeMenuNavigation.userCafeBalance = 0;
							cafeMenuNavigation.userCafeBalanceReceivedF = FALSE;
						}
					}

					INT32 remanBalanceWithoutDiscountNumber = (cafeMenuNavigation.userCafeBalance - cafeMenuNavigation.totalTxnAmount);
					INT32 remanBalanceWithoutDiscountDecimal, remanBalanceWithoutDiscountFraction;
					UINT32 totalTxnNumber = ( cafeMenuNavigation.totalTxnAmount - cafeMenuNavigation.totalTxnDiscount);
					UINT32 totalTxnDecimal, totalTxnFraction;
					INT32 remainingBalanceNumber=(cafeMenuNavigation.userCafeBalance - totalTxnNumber);
					INT32 remainingBalanceDecimal, remainingBalanceFraction;
					INT32 remainingWeeklyBalanceDecimal, remainingWeeklyBalanceFraction;

					UINT32 weeklyRemainLimitWithoutDiscount = (cafeMenuNavigation.weeklyPendingLimit - cafeMenuNavigation.totalTxnAmount);
					UINT32 weeklyRemainLimitWithoutDiscountDecimal, weeklyRemainLimitWithoutDiscountFraction;

					UINT32 weeklyRemainLimitWithDiscount = (cafeMenuNavigation.weeklyPendingLimit - totalTxnNumber);
					UINT32 weeklyRemainLimitWithDiscountDecimal, weeklyRemainLimitWithDiscountFraction;

					totalTxnDecimal = (UINT32)(totalTxnNumber / DOUBLE_PRECISION_FACTOR);
					totalTxnFraction = (UINT32)(totalTxnNumber % DOUBLE_PRECISION_FACTOR);

					remanBalanceWithoutDiscountDecimal = (remanBalanceWithoutDiscountNumber / DOUBLE_PRECISION_FACTOR);
					remanBalanceWithoutDiscountFraction = (remanBalanceWithoutDiscountNumber % DOUBLE_PRECISION_FACTOR);
					if(remanBalanceWithoutDiscountFraction < 0)
					{
						remanBalanceWithoutDiscountFraction *= (-1);
					}
					userBalanceDecimal = (INT32)(cafeMenuNavigation.userCafeBalance / DOUBLE_PRECISION_FACTOR);
					userBalanceFraction = (INT32)(cafeMenuNavigation.userCafeBalance % DOUBLE_PRECISION_FACTOR);
					if(userBalanceFraction < 0)
					{
						userBalanceFraction = userBalanceFraction * (-1);
					}
					totalTxnAmountDecimal = (UINT32)(cafeMenuNavigation.totalTxnAmount / DOUBLE_PRECISION_FACTOR);
					totalTxnAmountFraction = (UINT32)(cafeMenuNavigation.totalTxnAmount % DOUBLE_PRECISION_FACTOR);

					totalTxnDiscountDecimal = (UINT32)(cafeMenuNavigation.totalTxnDiscount / DOUBLE_PRECISION_FACTOR);
					totalTxnDiscountFraction = (UINT32)(cafeMenuNavigation.totalTxnDiscount % DOUBLE_PRECISION_FACTOR);

					remainingBalanceDecimal = (remainingBalanceNumber / DOUBLE_PRECISION_FACTOR);
					remainingBalanceFraction = (remainingBalanceNumber % DOUBLE_PRECISION_FACTOR);

					weeklyRemainLimitWithoutDiscountDecimal = (UINT32)(weeklyRemainLimitWithoutDiscount / DOUBLE_PRECISION_FACTOR);
					weeklyRemainLimitWithoutDiscountFraction = (UINT32)(weeklyRemainLimitWithoutDiscount % DOUBLE_PRECISION_FACTOR);

					weeklyRemainLimitWithDiscountDecimal = (UINT32)(weeklyRemainLimitWithDiscount / DOUBLE_PRECISION_FACTOR);
					weeklyRemainLimitWithDiscountFraction = (UINT32)(weeklyRemainLimitWithDiscount % DOUBLE_PRECISION_FACTOR);

					sprintf(cafeAllow.user.alphaNumId, "%s", GetAlphaNumId(DcInfo.userIdx) );
					cafeAllow.user.userId = GetUserIdFromIdx(DcInfo.userIdx);
					strcpy(cafeAllow.user.name,GetUserName(DcInfo.userIdx) );
					cafeAllow.user.photo = TRUE;
					cafeAllow.cafeSepF = TRUE;

					dataLen = snprintf(dataStr,RX_BUF_MAX, "%s%c%s%c", cafeAllow.user.alphaNumId, CAFE_SCR_FS,
								cafeAllow.user.name, CAFE_SCR_FS);

					switch(DcInfo.authReason)
					{
						default:
							return;

						case AUTH_CAFE_TXN_REASON_PREPAID_DEVICE:
						{
							INT32 remainBalance;
							INT32  amountDecimal;
							INT32 amountFraction;

							amountDecimal = (INT32)(cafeMenuProcInfo.userBalance / DOUBLE_PRECISION_FACTOR);
							amountFraction = (INT32)(cafeMenuProcInfo.userBalance % DOUBLE_PRECISION_FACTOR);
							if(amountFraction < 0)
							{
								amountFraction *= (-1);
							}
							snprintf(cafeAllow.genInfo.line1,sizeof(cafeAllow.genInfo.line1), "%-19s%c%4d.%02d", GetDisplayMessage(FALSE,NULL_CHAR,MSG_AVAILABLE_BALANCE_57),VALUE_FSP,amountDecimal, amountFraction);
							snprintf(cafeAllow.genInfo.line2,sizeof(cafeAllow.genInfo.line2), "%-19s%c%4u.%02u", GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRANSACTION_AMOUNT_610), VALUE_FSP, totalTxnAmountDecimal, totalTxnAmountFraction);

							dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02d%c%4u.%02u%c", amountDecimal, amountFraction, CAFE_SCR_FS,
											totalTxnAmountDecimal, totalTxnAmountFraction, CAFE_SCR_FS);

							remainBalance = (cafeMenuProcInfo.userBalance - cafeMenuNavigation.totalTxnAmount);
							amountDecimal = (UINT32)(remainBalance / DOUBLE_PRECISION_FACTOR);
							amountFraction = (UINT32)(remainBalance % DOUBLE_PRECISION_FACTOR);
							if(amountFraction < 0)
							{
								amountFraction *= (-1);
							}
							// Weekly Remaining Limit
							if( (TRUE == IsCafeteriaPolicyCheckRequired(DcInfo.userIdx)) && (TRUE == cafeMenuNavigation.weeklyPendingLimitReceivedF) )
							{
								snprintf(cafeAllow.genInfo.line3,sizeof(cafeAllow.genInfo.line3), "%-19s%c%4u.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_WEEKLY_REMAINING_LIMIT_1070),VALUE_FSP, weeklyRemainLimitWithoutDiscountDecimal, weeklyRemainLimitWithoutDiscountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4u.%02u%c", weeklyRemainLimitWithoutDiscountDecimal, weeklyRemainLimitWithoutDiscountFraction, CAFE_SCR_FS);

								// Net Balance
								snprintf(cafeAllow.genInfo.line4,sizeof(cafeAllow.genInfo.line4), "%-19s%c%4d.%02d", GetDisplayMessage(TRUE,CHAR_COLON,MSG_NET_BALANCE_375),VALUE_FSP, amountDecimal, amountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02d%c", amountDecimal, amountFraction, CAFE_SCR_FS);
							}
							else
							{
								// Net Balance
								snprintf(cafeAllow.genInfo.line3,sizeof(cafeAllow.genInfo.line3), "%-19s%c%4d.%02d", GetDisplayMessage(TRUE,CHAR_COLON,MSG_NET_BALANCE_375),VALUE_FSP, amountDecimal, amountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02d%c", amountDecimal, amountFraction, CAFE_SCR_FS);
							}
							break;
						}

						case AUTH_CAFE_TXN_REASON_PREPAID_DEVICE_DISCNT:
						{
							INT32 remainBalance;
							INT32 amountDecimal;
							INT32 amountFraction;

							amountDecimal = (INT32)(cafeMenuProcInfo.userBalance / DOUBLE_PRECISION_FACTOR);
							amountFraction = (INT32)(cafeMenuProcInfo.userBalance % DOUBLE_PRECISION_FACTOR);
							if(amountFraction < 0)
							{
								amountFraction *= (-1);
							}
							snprintf(cafeAllow.genInfo.line1,sizeof(cafeAllow.genInfo.line1), "%-19s%c%4d.%02d", GetDisplayMessage(FALSE,NULL_CHAR,MSG_AVAILABLE_BALANCE_57), VALUE_FSP,amountDecimal, amountFraction);
							snprintf(cafeAllow.genInfo.line2,sizeof(cafeAllow.genInfo.line2), "%-19s%c%4u.%02u", GetDisplayMessage(FALSE,NULL_CHAR,MSG_TOTAL_AMOUNT_823), VALUE_FSP, totalTxnAmountDecimal, totalTxnAmountFraction);
							snprintf(cafeAllow.genInfo.line3,sizeof(cafeAllow.genInfo.line3), "%-19s%c%4u.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_DISCOUNT_141),VALUE_FSP,totalTxnDiscountDecimal, totalTxnDiscountFraction);
							snprintf(cafeAllow.genInfo.line4,sizeof(cafeAllow.genInfo.line4), "%-19s%c%4u.%02u", GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRANSACTION_AMOUNT_610), VALUE_FSP, totalTxnDecimal, totalTxnFraction);

							dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02d%c%4u.%02u%c%4u.%02u%c%4u.%02u%c",
									amountDecimal, amountFraction, CAFE_SCR_FS, totalTxnDecimal,
									totalTxnFraction, CAFE_SCR_FS, totalTxnDiscountDecimal,
									totalTxnDiscountFraction, CAFE_SCR_FS, totalTxnAmountDecimal, totalTxnAmountFraction,
									CAFE_SCR_FS);

							remainBalance = (cafeMenuProcInfo.userBalance - (cafeMenuNavigation.totalTxnAmount - cafeMenuNavigation.totalTxnDiscount) );
							amountDecimal = (UINT32)(remainBalance / DOUBLE_PRECISION_FACTOR);
							amountFraction = (UINT32)(remainBalance % DOUBLE_PRECISION_FACTOR);
							if(amountFraction < 0)
							{
								amountFraction *= (-1);
							}
							// Weekly Remaining Limit
							if( (TRUE == IsCafeteriaPolicyCheckRequired(DcInfo.userIdx)) && (TRUE == cafeMenuNavigation.weeklyPendingLimitReceivedF) )
							{
								snprintf(cafeAllow.genInfo.line5,sizeof(cafeAllow.genInfo.line5), "%-19s%c%4u.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_WEEKLY_REMAINING_LIMIT_1070),VALUE_FSP, weeklyRemainLimitWithDiscountDecimal, weeklyRemainLimitWithDiscountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4u.%02u%c", weeklyRemainLimitWithDiscountDecimal, weeklyRemainLimitWithDiscountFraction, CAFE_SCR_FS);

								// Net Balance
								snprintf(cafeAllow.genInfo.line6,sizeof(cafeAllow.genInfo.line6), "%-19s%c%4d.%02d", GetDisplayMessage(TRUE,CHAR_COLON,MSG_NET_BALANCE_375),VALUE_FSP, amountDecimal, amountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02d%c", amountDecimal, amountFraction, CAFE_SCR_FS);
							}
							else
							{
								// Net Balance
								snprintf(cafeAllow.genInfo.line5,sizeof(cafeAllow.genInfo.line5), "%-19s%c%4d.%02d", GetDisplayMessage(TRUE,CHAR_COLON,MSG_NET_BALANCE_375),VALUE_FSP, amountDecimal, amountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02d%c", amountDecimal, amountFraction, CAFE_SCR_FS);
							}
							break;
						}

						case AUTH_CAFE_TXN_REASON_PREPAID_SERVER:
						{
							snprintf(cafeAllow.genInfo.line1,sizeof(cafeAllow.genInfo.line1), "%-19s%c%4u.%02u", GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRANSACTION_AMOUNT_610), VALUE_FSP, totalTxnAmountDecimal, totalTxnAmountFraction);
							dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4u.%02u%c", totalTxnAmountDecimal, totalTxnAmountFraction, CAFE_SCR_FS);

							// Weekly Remaining Limit
							if( (TRUE == IsCafeteriaPolicyCheckRequired(DcInfo.userIdx)) && (TRUE == cafeMenuNavigation.weeklyPendingLimitReceivedF) )
							{
								snprintf(cafeAllow.genInfo.line2,sizeof(cafeAllow.genInfo.line2), "%-19s%c%4u.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_WEEKLY_REMAINING_LIMIT_1070),VALUE_FSP, weeklyRemainLimitWithoutDiscountDecimal, weeklyRemainLimitWithoutDiscountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4u.%02u%c", weeklyRemainLimitWithoutDiscountDecimal, weeklyRemainLimitWithoutDiscountFraction, CAFE_SCR_FS);

								// Net Balance
								if(TRUE == cafeMenuNavigation.userCafeBalanceReceivedF)
								{
									snprintf(cafeAllow.genInfo.line3,sizeof(cafeAllow.genInfo.line3), "%-19s%c%4d.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_NET_BALANCE_375),VALUE_FSP, remanBalanceWithoutDiscountDecimal, remanBalanceWithoutDiscountFraction);
									dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02u%c", remanBalanceWithoutDiscountDecimal, remanBalanceWithoutDiscountFraction, CAFE_SCR_FS);
								}
							}
							else if(TRUE == cafeMenuNavigation.userCafeBalanceReceivedF) // Net Balance
							{
								snprintf(cafeAllow.genInfo.line2,sizeof(cafeAllow.genInfo.line2), "%-19s%c%4d.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_NET_BALANCE_375),VALUE_FSP, remanBalanceWithoutDiscountDecimal, remanBalanceWithoutDiscountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02u%c", remanBalanceWithoutDiscountDecimal, remanBalanceWithoutDiscountFraction, CAFE_SCR_FS);
							}
							break;
						}

						case AUTH_CAFE_TXN_REASON_PREPAID_SERVER_DISCNT:
						{
							snprintf(cafeAllow.genInfo.line1,sizeof(cafeAllow.genInfo.line1), "%-19s%c%4u.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_DISCOUNT_141), VALUE_FSP, totalTxnDiscountDecimal, totalTxnDiscountFraction);
							snprintf(cafeAllow.genInfo.line2,sizeof(cafeAllow.genInfo.line2), "%-19s%c%4u.%02u", GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRANSACTION_AMOUNT_610), VALUE_FSP, totalTxnDecimal, totalTxnFraction);
							dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4u.%02u%c%4u.%02u%c", totalTxnDecimal, totalTxnFraction, CAFE_SCR_FS,
											   totalTxnDiscountDecimal, totalTxnDiscountFraction, CAFE_SCR_FS);

							// Weekly Remaining Limit
							if( (TRUE == IsCafeteriaPolicyCheckRequired(DcInfo.userIdx)) && (TRUE == cafeMenuNavigation.weeklyPendingLimitReceivedF) )
							{
								snprintf(cafeAllow.genInfo.line3,sizeof(cafeAllow.genInfo.line3), "%-19s%c%4u.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_WEEKLY_REMAINING_LIMIT_1070),VALUE_FSP, weeklyRemainLimitWithDiscountDecimal, weeklyRemainLimitWithDiscountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4u.%02u%c", weeklyRemainLimitWithDiscountDecimal, weeklyRemainLimitWithDiscountFraction, CAFE_SCR_FS);

								// Net Balance
								if(TRUE == cafeMenuNavigation.userCafeBalanceReceivedF)
								{
									snprintf(cafeAllow.genInfo.line4,sizeof(cafeAllow.genInfo.line4), "%-19s%c%4d.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_NET_BALANCE_375),VALUE_FSP, remainingBalanceDecimal, remainingBalanceFraction);
									dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02u%c", remainingBalanceDecimal, remainingBalanceFraction, CAFE_SCR_FS);
								}
							}
							else if(TRUE == cafeMenuNavigation.userCafeBalanceReceivedF) // Net Balance
							{
								snprintf(cafeAllow.genInfo.line3,sizeof(cafeAllow.genInfo.line3), "%-19s%c%4d.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_NET_BALANCE_375),VALUE_FSP, remainingBalanceDecimal, remainingBalanceFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02u%c", remainingBalanceDecimal, remainingBalanceFraction, CAFE_SCR_FS);
							}
							break;
						}

						case AUTH_CAFE_TXN_REASON_POSTPAID:
						{
							INT32 currentMonthUsageWithoutDiscount;
							INT32 currentMonthUsageWithoutDiscountDecimal, currentMonthUsageWithoutDiscountFraction;

							currentMonthUsageWithoutDiscount = (cafeMenuNavigation.userCafeBalance + cafeMenuNavigation.totalTxnAmount);

							currentMonthUsageWithoutDiscountDecimal = (INT32)(currentMonthUsageWithoutDiscount / DOUBLE_PRECISION_FACTOR);
							currentMonthUsageWithoutDiscountFraction = (INT32)(currentMonthUsageWithoutDiscount % DOUBLE_PRECISION_FACTOR);
							if(currentMonthUsageWithoutDiscountFraction < 0)
							{
								currentMonthUsageWithoutDiscountFraction *= (-1);
							}
							snprintf(cafeAllow.genInfo.line1,sizeof(cafeAllow.genInfo.line1), "%-19s%c%4u.%02u", GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRANSACTION_AMOUNT_610), VALUE_FSP, totalTxnAmountDecimal, totalTxnAmountFraction);
							dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4u.%02u%c", totalTxnAmountDecimal, totalTxnAmountFraction, CAFE_SCR_FS);

							// Weekly Remaining Limit
							if( (TRUE == IsCafeteriaPolicyCheckRequired(DcInfo.userIdx)) && (TRUE == cafeMenuNavigation.weeklyPendingLimitReceivedF) )
							{
								snprintf(cafeAllow.genInfo.line2,sizeof(cafeAllow.genInfo.line2), "%-19s%c%4u.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_WEEKLY_REMAINING_LIMIT_1070),VALUE_FSP, weeklyRemainLimitWithoutDiscountDecimal, weeklyRemainLimitWithoutDiscountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4u.%02u%c", weeklyRemainLimitWithoutDiscountDecimal, weeklyRemainLimitWithoutDiscountFraction, CAFE_SCR_FS);

								// Current Month Usage
								if(TRUE == cafeMenuNavigation.userCafeBalanceReceivedF)
								{
									snprintf(cafeAllow.genInfo.line3,sizeof(cafeAllow.genInfo.line3), "%-19s%c%4d.%02d", GetDisplayMessage(TRUE,CHAR_COLON,MSG_CURRENT_MONTH_USAGE_787),VALUE_FSP, currentMonthUsageWithoutDiscountDecimal, currentMonthUsageWithoutDiscountFraction);
									dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02d%c", currentMonthUsageWithoutDiscountDecimal, currentMonthUsageWithoutDiscountFraction, CAFE_SCR_FS);
								}
							}
							else if(TRUE == cafeMenuNavigation.userCafeBalanceReceivedF) // Current Month Usage
							{
								snprintf(cafeAllow.genInfo.line2,sizeof(cafeAllow.genInfo.line2), "%-19s%c%4d.%02d", GetDisplayMessage(TRUE,CHAR_COLON,MSG_CURRENT_MONTH_USAGE_787),VALUE_FSP, currentMonthUsageWithoutDiscountDecimal, currentMonthUsageWithoutDiscountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02d%c", currentMonthUsageWithoutDiscountDecimal, currentMonthUsageWithoutDiscountFraction, CAFE_SCR_FS);
							}
							break;
						}

						case AUTH_CAFE_TXN_REASON_POSTPAID_DISCNT:
						{
							INT32 currentMonthUsageWithDiscount;
							INT32 currentMonthUsageWithDiscountDecimal, currentMonthUsageWithDiscountFraction;

							currentMonthUsageWithDiscount = (cafeMenuNavigation.userCafeBalance + (cafeMenuNavigation.totalTxnAmount - cafeMenuNavigation.totalTxnDiscount) );

							currentMonthUsageWithDiscountDecimal = (INT32)(currentMonthUsageWithDiscount / DOUBLE_PRECISION_FACTOR);
							currentMonthUsageWithDiscountFraction = (INT32)(currentMonthUsageWithDiscount % DOUBLE_PRECISION_FACTOR);

							snprintf(cafeAllow.genInfo.line1,sizeof(cafeAllow.genInfo.line1), "%-19s%c%4u.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_DISCOUNT_141), VALUE_FSP, totalTxnDiscountDecimal, totalTxnDiscountFraction);
							snprintf(cafeAllow.genInfo.line2,sizeof(cafeAllow.genInfo.line2), "%-19s%c%4u.%02u", GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRANSACTION_AMOUNT_610), VALUE_FSP, totalTxnDecimal, totalTxnFraction);
							dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4u.%02u%c%4u.%02u%c", totalTxnDecimal, totalTxnFraction,CAFE_SCR_FS,
											   totalTxnDiscountDecimal, totalTxnDiscountFraction, CAFE_SCR_FS);

							// Weekly Remaining Limit
							if( (TRUE == IsCafeteriaPolicyCheckRequired(DcInfo.userIdx)) && (TRUE == cafeMenuNavigation.weeklyPendingLimitReceivedF) )
							{
								snprintf(cafeAllow.genInfo.line3,sizeof(cafeAllow.genInfo.line3), "%-19s%c%4u.%02u", GetDisplayMessage(TRUE,CHAR_COLON,MSG_WEEKLY_REMAINING_LIMIT_1070),VALUE_FSP, weeklyRemainLimitWithDiscountDecimal, weeklyRemainLimitWithDiscountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4u.%02u%c", weeklyRemainLimitWithDiscountDecimal, weeklyRemainLimitWithDiscountFraction, CAFE_SCR_FS);

								// Current Month Usage
								if(TRUE == cafeMenuNavigation.userCafeBalanceReceivedF)
								{
									snprintf(cafeAllow.genInfo.line4,sizeof(cafeAllow.genInfo.line4), "%-19s%c%4d.%02d", GetDisplayMessage(TRUE,CHAR_COLON,MSG_CURRENT_MONTH_USAGE_787),VALUE_FSP, currentMonthUsageWithDiscountDecimal, currentMonthUsageWithDiscountFraction);
									dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02d%c", currentMonthUsageWithDiscountDecimal, currentMonthUsageWithDiscountFraction, CAFE_SCR_FS);
								}
							}
							else if(TRUE == cafeMenuNavigation.userCafeBalanceReceivedF) // Current Month Usage
							{
								snprintf(cafeAllow.genInfo.line3,sizeof(cafeAllow.genInfo.line3), "%-19s%c%4d.%02d", GetDisplayMessage(TRUE,CHAR_COLON,MSG_CURRENT_MONTH_USAGE_787),VALUE_FSP, currentMonthUsageWithDiscountDecimal, currentMonthUsageWithDiscountFraction);
								dataLen += snprintf(dataStr+dataLen, (RX_BUF_MAX-dataLen), "%4d.%02d%c", currentMonthUsageWithDiscountDecimal, currentMonthUsageWithDiscountFraction, CAFE_SCR_FS);
							}
							break;
						}
					}

					//send TRANSACTION_COMPLETE to mirror display device
					SendDataToAllMirrorDisplayDevices(TRANSACTION_COMPLETE, dataStr, dataLen);
					SendMessageToQt(&cafeAllow, GUI_MSG_CAFE_USER_ALLOWED_COMMON_SCREEN);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessAllowScreenDuration);
					break;
				}
				case USER_RESP_CAFE_POLICY_USER_DENIED:
				{
					USER_COMMON_QT_t cafePolicyDeny = defltQtUsrCommon;

					userIdx = DcInfo.userIdx;

					// Handle timeout response separately as it requires to show symbol along with text.
					if(DcInfo.authReason == AUTH_TIMEOUT)
					{
						strcpy(qtDynamicInfo.titleText, "");
						qtDynamicInfo.imageIdx = POPUP_TIMEOUT;
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TIME_OUT_605));
						SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

						SetLcdIdleTimer(LCD_IDLE_TIMER);
 					}
					else
					{
						cafePolicyDeny.genInfo.imageIdx = DENY_SCREEN_CAFE_FAILED;

						switch(DcInfo.authReason)
						{
							case AUTH_CAFE_POLICY_DENIED_WEEK_OFF:
							{
								strcpy(cafePolicyDeny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_WEEK_OFF_PUBLIC_HOLIDAY_895));
								break;
							}
							case AUTH_CAFE_POLICY_DENIED_NON_WORKING_DAY:
							{
								strcpy(cafePolicyDeny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_NOT_A_WORKING_DAY_896));
								break;
							}
							case AUTH_CAFE_POLICY_DENIED_INVALID_SHIFT:
							{
								strcpy(cafePolicyDeny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_NOT_IN_ACTIVE_SHIFT_897));
								break;
							}
							case AUTH_CAFE_POLICY_DENIED_DAILY_TRANSACTION:
							{
								strcpy(cafePolicyDeny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DAILY_TRANSACTION_LIMIT_EXCEEDED_898));
								break;
							}
							case AUTH_CAFE_POLICY_DENIED_DAILY_MENU_LIMIT:
							{
								strcpy(cafePolicyDeny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DAILY_MENU_LIMIT_EXCEEDED_899));
								break;
							}
							case AUTH_CAFE_POLICY_DENIED_DAILY_ITEM_LIMIT:
							{
								UINT8 	itemIdx, cnt=0;
								UINT16 	itemID, itemDataLen = 0;
								CHAR	tempData[QT_RX_BUF_SIZE*2];

								memset(tempData, 0, sizeof(tempData));
								strcpy(cafePolicyDeny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DAILY_ITEM_LIMIT_EXCEEDED_900));
	
								for(itemIdx = 0; itemIdx < cafePolicyCheckResp.totalItems; itemIdx++)
								{
									if(cafePolicyCheckResp.cafeItemRespCode[itemIdx] == CAFE_POLICY_ITEM_RESPCODE_DENIED_DAILY_LIMIT)
									{
										itemID = cafePolicyCheckResp.cafeItemIdx[itemIdx]-1;
										if(cnt != 0)
										{
											itemDataLen += sprintf(tempData+itemDataLen, ", %s",CafeItemConfig[itemID].name);
										}
										else
										{
											itemDataLen += sprintf(tempData+itemDataLen, "%s",CafeItemConfig[itemID].name);
										}
										cnt++;
									}
								}

								snprintf(cafePolicyDeny.genInfo.line2, sizeof(cafePolicyDeny.genInfo.line2),"%s",tempData);
								break;
							}
							case AUTH_CAFE_POLICY_DENIED_MONTHLY_TRANSACTION:
							{
								strcpy(cafePolicyDeny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_MONTHLY_TRANSACTION_LIMIT_EXCEEDED_901));
								break;
							}
							case AUTH_CAFE_POLICY_DENIED_MONTHLY_MENU_LIMIT:
							{
								strcpy(cafePolicyDeny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_MONTHLY_MENU_LIMIT_EXCEEDED_902));
								break;
							}
							case AUTH_CAFE_POLICY_DENIED_MONTHLY_ITEM_LIMIT:
							{
								UINT8 	itemIdx, cnt=0;
								UINT16 	itemID, itemDataLen = 0;
								CHAR	tempData[QT_RX_BUF_SIZE*2];

								memset(tempData, 0, sizeof(tempData));
								strcpy(cafePolicyDeny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_MONTHLY_ITEM_LIMIT_EXCEEDED_903));
								for(itemIdx = 0; itemIdx < cafePolicyCheckResp.totalItems; itemIdx++)
								{
									if(cafePolicyCheckResp.cafeItemRespCode[itemIdx] == CAFE_POLICY_ITEM_RESPCODE_DENIED_MONTHLY_LIMIT)
									{
										itemID = cafePolicyCheckResp.cafeItemIdx[itemIdx]-1;
										if(cnt != 0)
										{
											itemDataLen += sprintf(tempData+itemDataLen, ", %s",CafeItemConfig[itemID].name);
										}
										else
										{
											itemDataLen += sprintf(tempData+itemDataLen, "%s",CafeItemConfig[itemID].name);
										}
										cnt++;
									}
								}

								snprintf(cafePolicyDeny.genInfo.line2, sizeof(cafePolicyDeny.genInfo.line2),"%s",tempData);
								break;
							}

							case AUTH_CAFE_POLICY_DENIED_WEEKLY_LIMIT:
							{
								snprintf(cafePolicyDeny.genInfo.line1, sizeof(cafePolicyDeny.genInfo.line1),"%s",GetDisplayMessage(FALSE,NULL_CHAR,MSG_WEEKLY_TRANSACTION_LIMIT_EXCEEDED_1071));
								break;
							}

							case AUTH_CAFE_POLICY_DENIED_OFFLINE_ITEM_PER_MENU_LIMIT:
							{
								UINT8 	itemIdx = 0, cnt = 0;
								UINT16 	itemID = 0, itemDataLen = 0;
								CHAR	tempData[QT_RX_BUF_SIZE*2];

								memset(tempData, 0, sizeof(tempData));

								snprintf(cafePolicyDeny.genInfo.line1, sizeof(cafePolicyDeny.genInfo.line1),"%s",GetDisplayMessage(FALSE,NULL_CHAR,MSG_ITEM_QUANTITY_PER_MENU_EXCEEDED_1077));

								for(itemIdx = 0; itemIdx < cafeOfflineItemRestrictInfo.totalItems; itemIdx++)
								{
									itemID = cafeOfflineItemRestrictInfo.cafeItemIdx[itemIdx];
									if(cnt != 0)
									{
										itemDataLen += snprintf(tempData+itemDataLen,((QT_RX_BUF_SIZE*2)-itemDataLen), ", %s",CafeItemConfig[itemID].name);
									}
									else
									{
										itemDataLen += snprintf(tempData+itemDataLen,((QT_RX_BUF_SIZE*2)-itemDataLen), "%s",CafeItemConfig[itemID].name);
									}
									cnt++;
								}
								snprintf(cafePolicyDeny.genInfo.line2, sizeof(cafePolicyDeny.genInfo.line2),"%s",tempData);
								break;
							}

							case AUTH_CAFE_POLICY_DENIED_PREORDER_LIMIT:
							{
								UINT8 	itemIdx, cnt=0;
								UINT16 	itemID, itemDataLen = 0;
								CHAR	tempData[QT_RX_BUF_SIZE*2];

								memset(tempData, 0, sizeof(tempData));
								strcpy(cafePolicyDeny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PRE_ORDERED_QUANTITY_EXCEEDED_968));

								for(itemIdx = 0; itemIdx < cafePolicyCheckResp.totalItems; itemIdx++)
								{
									if(cafePolicyCheckResp.cafeItemRespCode[itemIdx] == CAFE_POLICY_ITEM_RESPCODE_DENIED_PREORDER_LIMIT)
									{
										itemID = cafePolicyCheckResp.cafeItemIdx[itemIdx]-1;
										if(cnt != 0)
										{
											itemDataLen += sprintf(tempData+itemDataLen, ", %s",CafeItemConfig[itemID].name);
										}
										else
										{
											itemDataLen += sprintf(tempData+itemDataLen, "%s",CafeItemConfig[itemID].name);
										}
										cnt++;
									}
								}

								snprintf(cafePolicyDeny.genInfo.line2, sizeof(cafePolicyDeny.genInfo.line2),"%s",tempData);
								break;
							}

							case AUTH_CAFE_POLICY_DENIED_OFFLINE_TRANSACTION_NOT_ALLOWED:
							{
								snprintf(cafePolicyDeny.genInfo.line1, sizeof(cafePolicyDeny.genInfo.line1),"%s",GetDisplayMessage(FALSE,NULL_CHAR,MSG_OFFLINE_TRANSACTION_NOT_ALLOWED_1043));
								break;
							}

							default:
								break;
						}

						if( (cafePolicyDeny.ifUserInfo == TRUE) && (userIdx != UNKNOWN_USER_IDX) )
						{
							sprintf(cafePolicyDeny.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
							cafePolicyDeny.user.userId = GetUserIdFromIdx(userIdx);
							strcpy(cafePolicyDeny.user.name, GetUserName(userIdx) );
							cafePolicyDeny.user.photo = TRUE;
						}

						dataLen = sprintf(dataStr, "%s%c%s%c", cafePolicyDeny.user.alphaNumId, CAFE_SCR_FS, cafePolicyDeny.user.name, CAFE_SCR_FS);

						//send TRANSACTION_COMPLETE to mirror display device
						SendDataToAllMirrorDisplayDevices(TRANSACTION_COMPLETE, dataStr, dataLen);

						SendMessageToQt(&cafePolicyDeny, GUI_MSG_CAFE_USER_DENIED_COMMON_SCREEN);
						SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					}

					break;
				}

				case USER_RESP_ACCESS_DENIED:
				case USER_RESP_CAFE_DENIED:
				case USER_RESP_CAFE_CARD_WRITE_FAILED:
				case USER_RESP_ACCESS_AUTH_TIMEOUT:
				case USER_RESP_ACCESS_AUTH_USER_BLOCKED:
				case USER_RESP_ACCESS_AUTH_FIRST_IN:
				case USER_RESP_ACCESS_AUTH_USER_DISABLED:
				case USER_RESP_ACCESS_AUTH_INVALID_INPUT:
				case USER_RESP_ACCESS_AUTH_CARD_READ_FAILED:
				case USER_RESP_ROUTE_ACCESS_CARD_WRITE_FAILED:
				case USER_RESP_ACCESS_AUTH_INVALID_PIN:
				case USER_RESP_ACCESS_AUTH_INVALID_FINGER:
				case USER_RESP_ACCESS_AUTH_INVALID_FACE:
				case USER_RESP_ACCESS_AUTH_INVALID_CARD:
				case USER_RESP_AUTH_INVALID_ROUTE_INPUT:
				case USER_RESP_AUTH_INVALID_ROUTE_ACCESS:
				case USER_RESP_AUTH_DOOR_NOT_IN_ROUTE:
				case USER_RESP_ACCESS_AUTH_FP_MATCHING_TIMEOUT:
				case USER_RESP_ACCESS_AUTH_NO_ACCESS:
				case USER_RESP_ACCESS_AUTH_PASSBACK_HARD_VIOLATION:
				case USER_RESP_ACCESS_AUTH_TWO_PERSON:
				case USER_RESP_ACCESS_AUTH_OCCUPANCY_VIOLATION:
				case USER_RESP_ACCESS_AUTH_DOOR_LOCK:
				case USER_RESP_ACCESS_AUTH_VALIDITY_EXPIRED:
				case USER_RESP_MIN_OCCUPANCY_IS_1:
				case USER_RESP_MIN_OCCUPANCY_IS_2:
				case USER_RESP_NOT_PRESENT_ON_PANEL:
				case USER_RESP_DENIED_FR_DISABLED:
				case USER_RESP_TEMP_HARD_THRESHOLD_EXCEEDED:
				case USER_RESP_AMBIENT_TEMPERATURE_CONFLICT:
#if defined(FACE_MASK_COMPULSION_ENABLE)
				case USER_RESP_DENIED_FACE_MASK_NOT_DETECTED:
#endif
				{
					USER_COMMON_QT_t deny = defltQtUsrCommon;
					userIdx = DcInfo.userIdx;

					// Handle timeout response separately as it requires to show symbol along with text.
					if(DcInfo.authReason == AUTH_TIMEOUT)
					{
						strcpy(qtDynamicInfo.titleText, "");
						qtDynamicInfo.imageIdx = POPUP_TIMEOUT;
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TIME_OUT_605));
						SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

						SetLcdIdleTimer(LCD_IDLE_TIMER);
						
						if(IsGroupFrApplicable() == YES && IsFreeScanTimerReload() == YES)
						{
							if(StartFRStreamingOnDeviceIdleScreen() == SUCCESS)
								LoadDisplayFRStreamToIdleScreenTimer(LCD_IDLE_TIMER + FRServerConfig.faceScanTimeOut,FALSE);
						}
 					}
					else
					{
						if(SysBasicPara.application == CAFETERIA)
						{
							deny.genInfo.imageIdx = DENY_SCREEN_CAFE_FAILED;
						}
						else
						{
							deny.genInfo.imageIdx = DENY_SCREEN_ACCESS_DENIED_IMAGE;	//Image Index
						}

						switch(DcInfo.authReason)
						{
							case AUTH_USER_BLOCKED:
							{
								if(SysBasicPara.application == CAFETERIA)
								{
									strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_USER_IS_BLOCKED_627));

									switch(GetBlockForCafe(DcInfo.userIdx))
									{
										case BLOCKED_MONTHLY_CONSUPTION:
											strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_MONTHLY_CONSUMPTION_LIMIT_EXCEEDED_817));
											break;

										case BLOCKED_DAILY_LIMIT:
											strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DAILY_CONSUMPTION_LIMIT_EXCEEDED_818));
											break;

										case BLOCKED_INSUFFICIENT_BALANCE:
											strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INSUFFICIENT_BALANCE_275));
											break;

										default:
											break;
									}
								}
								else
								{
									strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_USER_IS_BLOCKED_627));
								}
								break;
							}
							case AUTH_ADAPTIVE_ERL_TIMEOUT:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ADAPTIVE_ENROLL_TIME_OUT_1014));
							}break;
							case AUTH_ADAPTIVE_ERL_REJECT:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ADAPTIVE_ENROLL_FAILED_1015));
							}break;
							case AUTH_FIRST_IN:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_NO_FIRST_USER_ENTRY_391));
								break;
							}

							case AUTH_USER_DISABLED:
							case AUTH_CAFE_TXN_REASON_USER_IS_NOT_ACTIVE:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_USER_IS_NOT_ACTIVE_629));
								break;
							}

							case AUTH_INVALID_INPUT:
							{
								ACCESS_MODE_e accessMode;
								accessMode = GetAccessModeFromRespPort(DcInfo.userType, respPort);

								if( (accessMode == ACCESS_MODE_CARD_THEN_BIOMETRIC) && (DcInfo.credCnt == 1) && (DcInfo.credType[FIRST_CREDENTIAL] == FP_CREDENTIAL))
								{
									snprintf(deny.genInfo.line1, sizeof(deny.genInfo.line1), "%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_CARD_FIRST_1020));
								}
								else
								{
									strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,invalidInputStr[INVALID_CREDENTIAL]));
								}
								break;
							}

							case AUTH_API_INVALID_INPUT:
							{
								if (DcInfo.zamUserIdx == UNKNOWN_USER_IDX)
								{
									deny.ifUserInfo = FALSE;
								}
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,invalidInputStr[BLE_QR_CREDENTIAL]));
								break;
							}

							case AUTH_INVALID_ROUTE_INPUT:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_ROUTE_INPUT_290));
								break;
							}

							case AUTH_INVALID_ROUTE_ACCESS:
							case AUTH_DOOR_NOT_IN_ROUTE:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_ROUTE_ACCESS_289));
								break;
							}

							case AUTH_CAFE_TXN_REASON_INVALID_CREDENTIAL:
							{
								deny.ifUserInfo = TRUE;
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PREPAID_PAYMENT_458));
								strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_CREDENTIAL_785));
								break;
							}

							case AUTH_CARD_READ_FAILED:
							{
								if(userIdx == UNKNOWN_USER_IDX)
								{
									deny.ifUserInfo = FALSE;
								}
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CARD_READ_FAILED_77));
								break;
							}

							case AUTH_ROUTE_ACCESS_CARD_WRITE_FAILED:
							{
								if(userIdx == UNKNOWN_USER_IDX)
								{
									deny.ifUserInfo = FALSE;
								}
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CARD_WRITE_FAILED_80));
								break;
							}

							case AUTH_CAFE_TXN_REASON_CARD_READ_FAIL:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRANSACTION_FAILED_611));
								strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CARD_READ_FAILED_77));
								break;
							}

							case AUTH_INVALID_PIN:
							{
								if(DcInfo.zamUserIdx == UNKNOWN_USER_IDX)
								{
									deny.ifUserInfo = FALSE;
								}
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SORRY_INVALID_PIN_570));
								deny.genInfo.imageIdx = DENY_SCREEN_PIN_NOT_IDENTIFY;
								break;

							}

							case AUTH_INVALID_FINGER:
							{
								if(DcInfo.zamUserIdx == UNKNOWN_USER_IDX)
								{
									deny.ifUserInfo = FALSE;
								}
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SORRY_FINGER_NOT_IDENTIFIED_571));
								deny.genInfo.imageIdx = DENY_SCREEN_FINGER_NOT_IDENTIFY;
								break;
							}

							case AUTH_INVALID_FINGER_TEMP_USER:
							{
								if(DcInfo.zamUserIdx == UNKNOWN_USER_IDX)
								{
									deny.ifUserInfo = FALSE;
								}
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SORRY_FINGER_NOT_IDENTIFIED_571));
                                strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PLACE_SAME_FINGER_FOR_TEMP_USER_1090));
								deny.genInfo.imageIdx = DENY_SCREEN_FINGER_NOT_IDENTIFY;
								break;
							}

							case AUTH_INVALID_FACE:
							{
								if(DcInfo.zamUserIdx == UNKNOWN_USER_IDX)
								{
									deny.ifUserInfo = FALSE;
								}
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SORRY_FACE_NOT_IDENTIFIED_941));
								deny.genInfo.imageIdx = DENY_SCREEN_FACE_NOT_IDENTIFY;
								break;
							}

							case AUTH_INVALID_CARD:
							{
								if(DcInfo.zamUserIdx == UNKNOWN_USER_IDX)
								{
									deny.ifUserInfo = FALSE;
								}

								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SORRY_INVALID_CARD_569));
								deny.genInfo.imageIdx = DENY_SCREEN_CARD_NOT_IDENTIFY; //OverRide Image Index
								break;
							}

							case AUTH_FP_MATCHING_TIMEOUT:
							{
								deny.ifUserInfo = FALSE;
								strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FP_MATCHING_TIME_OUT_757));
								break;
							}

							case AUTH_NO_ACCESS:
							case AUTH_CAFE_TXN_REASON_USER_NOT_ALLOWED:
							{
								if(SysBasicPara.application ==  CAFETERIA)
								{
									strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_USER_NOT_ALLOWED_628));
								}
								else
								{
									strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENTRY_RESTRICTED_214));

								}
								break;
							}

							case AUTH_PASSBACK_ENTRY_HARD_VIOLATION:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_EXIT_NOT_RECORDED_225));
								break;
							}

							case AUTH_PASSBACK_EXIT_HARD_VIOLATION:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENTRY_NOT_RECORDED_212));
								break;
							}

							case AUTH_TWO_PERSON:
							{
								if( (DcInfo.userIdx != UNKNOWN_USER_IDX) || (DcInfo.zamUserIdx != UNKNOWN_USER_IDX) )
								{
									// If user is denied in ZAM mode, Deny first user identified
									if(DcInfo.zamUserIdx != UNKNOWN_USER_IDX)
									{
										userIdx = DcInfo.zamUserIdx;
									}
								}
								else
								{
									deny.ifUserInfo = FALSE;
								}
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_2_PERSON_MUST_ZONE_18));
								break;
							}

							case AUTH_OCCUPANCY_VIOLATION:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TOO_MANY_OCCUPANTS_608));
								break;
							}

							case AUTH_DOOR_LOCK:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DOOR_IS_LOCKED_165));
								break;
							}

							case AUTH_VALIDITY_EXPIRED:
							case AUTH_CAFE_TXN_REASON_USER_NOT_VALID:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VALIDITY_EXPIRED_634));
								break;
							}

							case AUTH_EVENT_LOG_FAIL:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SYSTEM_ERROR_597));
								break;
							}

							case AUTH_CAFE_TXN_REASON_INSUFFICIENT_BALANCE:
							{
								userBalanceDecimal = (INT32)(cafeMenuProcInfo.userBalance / DOUBLE_PRECISION_FACTOR);
								userBalanceFraction = (INT32)(cafeMenuProcInfo.userBalance % DOUBLE_PRECISION_FACTOR);
								if(userBalanceFraction < 0)
								{
									userBalanceFraction *= (-1);
								}
								totalTxnAmountDecimal = (UINT32)(cafeMenuNavigation.totalTxnAmount / DOUBLE_PRECISION_FACTOR);
								totalTxnAmountFraction = (UINT32)(cafeMenuNavigation.totalTxnAmount % DOUBLE_PRECISION_FACTOR);

								sprintf(deny.genInfo.line1,"%s",GetDisplayMessage(FALSE,NULL_CHAR,MSG_INSUFFICIENT_BALANCE_275));
								sprintf(deny.genInfo.line2, "%s : %u.%02u", GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRANSACTION_AMOUNT_610), totalTxnAmountDecimal, totalTxnAmountFraction);
								if(GetUserPaymentModeForCafe(DcInfo.userIdx) != PREPAID_SERVER_SIDE)
								{
									sprintf(deny.genInfo.line3, "%s : %d.%02d",GetDisplayMessage(TRUE, CHAR_COLON, MSG_AVAILABLE_BALANCE_57), userBalanceDecimal, userBalanceFraction);
								}
								break;
							}

							case AUTH_CAFE_TXN_REASON_CARD_WRITE_FAIL:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRANSACTION_FAILED_611));
								strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CARD_WRITE_FAILED_80));
								break;
							}

							case AUTH_CAFE_TXN_REASON_NO_ITEM_SELECTED:
							{
								deny.ifUserInfo = FALSE;
								strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_NO_ITEM_SELECTED_3));
								break;
							}

							case AUTH_ZERO_OCCUPANCY_NOT_ALLOWED_1:
							case AUTH_MIN_OCCUPANCY_IS_1:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_MINIMUM_1_USER_228));
								break;
							}

							case AUTH_ZERO_OCCUPANCY_NOT_ALLOWED_2:
							case AUTH_MIN_OCCUPANCY_IS_2:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_MINIMUM_2_USERS_242));
								break;
							}

							case AUTH_CAFE_TXN_REASON_SERVER_TIMEOUT:
							{
								deny.ifUserInfo = FALSE;
								strcpy( deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR, MSG_SERVER_NOT_RESPONDING_866) );
								break;
							}

							case AUTH_CAFE_TXN_REASON_INVALID_TRANSACTION:
							{
								deny.ifUserInfo = FALSE;
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_TRANSACTION_877));
								break;
							}

							case AUTH_USER_NOT_PRESENT_ON_PANEL:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_USER_NOT_FOUND_575));
								break;
							}

							case AUTH_FR_DISABLED:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FACE_RECOGNITION_IS_DISABLED_962));
								break;
							}

							case AUTH_TEMP_HARD_THRESHOLD_EXCEED:
							{
								deny.ifUserInfo = TRUE;
								snprintf(deny.temperature, MAX_TEMPERATURE_LENGTH, "%.1f", DcInfo.usrTemperature);
								deny.temperatureUnit = SysAdvPara.temperatureLogCnfg.tempUnit;
								/* Set user exceeds temperature threshold */
								if (DcInfo.usrExceedTempThreshold == TRUE)
								{
									deny.temperatureViolation = TRUE;
								}
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TEMPERATURE_EXCEEDED_980));
								break;
							}

							case AUTH_AMBIENT_TEMPERATURE_CONFLICT:
							{
								deny.ifUserInfo = TRUE;
								snprintf(deny.temperature, MAX_TEMPERATURE_LENGTH, "%.1f", DcInfo.usrTemperature);
								deny.temperatureUnit = SysAdvPara.temperatureLogCnfg.tempUnit;
								/* Set user temperature violation */
								deny.temperatureViolation = TRUE;
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_AMBIENT_TEMP_CONFLICT_1008));
								break;
							}

#if defined(FACE_MASK_COMPULSION_ENABLE)
							case AUTH_FACE_MASK_HARD_VIOLATION:
							{
								deny.ifUserInfo = TRUE;
								deny.faceMaskDecision = QT_FACE_MASK_DECISION_ACCESS_DENIED_NOT_DETECTED;
								snprintf(deny.genInfo.line1, sizeof(deny.genInfo.line1), "%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_FACE_MASK_NOT_DETECTED_1041));
								break;
							}
#endif

							default:
								break;	//Default for denied response
						}

						if( (deny.ifUserInfo == TRUE) && (userIdx != UNKNOWN_USER_IDX) )
						{
							sprintf(deny.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
							deny.user.userId = GetUserIdFromIdx(userIdx);
							strcpy(deny.user.name, GetUserName(userIdx) );
							snprintf(deny.user.designation, DESIGNATION_SIZE, "%s", DcInfo.designation);
							deny.user.photo = TRUE;
						}

						if(SysBasicPara.application == CAFETERIA)
						{
							dataLen = sprintf(dataStr, "%s%c%s%c", deny.user.alphaNumId, CAFE_SCR_FS, deny.user.name, CAFE_SCR_FS);

							if(DcInfo.authReason == AUTH_CAFE_TXN_REASON_INSUFFICIENT_BALANCE)
							{
								dataLen += sprintf(dataStr+dataLen, "%d.%02d%c%u.%02u%c", userBalanceDecimal, userBalanceFraction,
														CAFE_SCR_FS, totalTxnAmountDecimal, totalTxnAmountFraction, CAFE_SCR_FS);
							}
							//send TRANSACTION_COMPLETE to mirror display device
							SendDataToAllMirrorDisplayDevices(TRANSACTION_COMPLETE, dataStr, dataLen);

							SendMessageToQt(&deny, GUI_MSG_CAFE_USER_DENIED_COMMON_SCREEN);
						}
						else
						{
							SendMessageToQt(&deny, GUI_MSG_USER_DENIED_COMMON_SCREEN);
						}
						SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration); //TODO_Shubham: this timer should be in else?
					}
  					break;
				}

				case USER_RESP_NEXT_PERSON:
				{
					strcpy(qtFuncStatusInfo.splFunc, "");
					qtFuncStatusInfo.ifUserInfo = TRUE;
					sprintf(qtFuncStatusInfo.user.alphaNumId, "%s",GetAlphaNumId(DcInfo.userIdx) );
					qtFuncStatusInfo.user.userId = GetUserIdFromIdx(DcInfo.userIdx);
					strcpy(qtFuncStatusInfo.user.name, GetUserName(DcInfo.userIdx) );
					qtFuncStatusInfo.user.photo = TRUE;
					qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_WAIT_FOR_SECOND_PERSON;
					strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_WAITING_FOR_2ND_USER_649));
					strcpy(qtFuncStatusInfo.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_2_PERSON_MUST_ZONE_18));
					qtFuncStatusInfo.genInfo.ifPinEntryBtn = TRUE;
					// Temperature
					snprintf(qtFuncStatusInfo.temperature, MAX_TEMPERATURE_LENGTH, "%.1f", DcInfo.usrTemperature);
					qtFuncStatusInfo.temperatureUnit = SysAdvPara.temperatureLogCnfg.tempUnit;
					/* Set user exceeds temperature threshold */
					if (DcInfo.usrExceedTempThreshold == TRUE)
					{
						qtFuncStatusInfo.temperatureViolation = TRUE;
					}
#if defined(FACE_MASK_COMPULSION_ENABLE)
                    if((IsGroupFrApplicable() == NO) &&
                            (SysAdvPara.faceMaskComp_Enable == TRUE))
                    {
						qtFuncStatusInfo.faceMaskDecision = DcInfo.isFaceMaskViolated ? QT_FACE_MASK_DECISION_ACCESS_ALLOWED_SOFT :
																						QT_FACE_MASK_DECISION_ACCESS_ALLOWED;
                    }
#endif
					SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);
					SetLcdIdleTimer(SysAdvPara.twoPersonTimer);

					break;
				}

				case USER_RESP_SHOW_CARD:
				{
					switch(GetEnrollmentCategory() )
					{
						default:
							return;

						case ENROLL_CATEGORY_USER:
						{
							strcpy(qtFuncStatusInfo.splFunc, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
							qtFuncStatusInfo.ifUserInfo = TRUE;
							userIdx = GetUserIdxBeingEnrolled();
							sprintf(qtFuncStatusInfo.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
							qtFuncStatusInfo.user.userId = GetUserIdFromIdx(userIdx);
							strcpy(qtFuncStatusInfo.user.name, GetUserName(userIdx) );
							qtFuncStatusInfo.user.photo = TRUE;
							strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_THE_CARD_556));
							qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_SHOW_CARD;
							qtFuncStatusInfo.genInfo.ifEscBtn	= TRUE;

							// Update Current Enrollment Screen
							currentEnrollScreen = GET_USER_CREDENTIAL_SCREEN;
							SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);
							break;
						}

						case ENROLL_CATEGORY_SP_FN:
						{
							SPECIAL_FUNCTIONS_e	splFnIdx;

							splFnIdx = GetSplFnIdxBeingEnrolled();
							strcpy(qtDynamicSplFuncInfo.splFunc, GetDisplayMessage(FALSE,NULL_CHAR,ActivateSplFnStr[splFnIdx]));
							qtDynamicSplFuncInfo.splFuncImageIdx = splFnIdx;
							strcpy(qtDynamicSplFuncInfo.noticeLine, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_THE_CARD_556));
							qtDynamicSplFuncInfo.ifEscBtn = TRUE;

							currentEnrollScreen = GET_SPL_FN_CARD_SCREEN;
							SendMessageToQt(&qtDynamicSplFuncInfo, GUI_MSG_SPL_FUNC_DYNAMIC_SCREEN);
							break;
						}
					}
					SetLcdIdleTimer(ENROLLMENT_TIMER);
					break;
				}

				case USER_RESP_PLACE_FINGER:
				{
					strcpy(qtFuncStatusInfo.splFunc, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLL_USER_195));
					qtFuncStatusInfo.ifUserInfo = TRUE;
					userIdx = GetUserIdxBeingEnrolled();
					sprintf(qtFuncStatusInfo.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
					qtFuncStatusInfo.user.userId = GetUserIdFromIdx(userIdx);
					strcpy(qtFuncStatusInfo.user.name, GetUserName(userIdx) );
					qtFuncStatusInfo.user.photo = TRUE;
					strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PLACE_YOUR_FINGER_440));
					qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_PLACE_FINGER;
					qtFuncStatusInfo.genInfo.ifEscBtn	= TRUE;

					// Update Current Enrollment Screen
					currentEnrollScreen = GET_USER_CREDENTIAL_SCREEN;
					SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);
					SetLcdIdleTimer(ENROLLMENT_TIMER);
					break;
				}

				case USER_RESP_NEXT_CARD:
				{
					if(GetEnrollmentCategory() == ENROLL_CATEGORY_USER)
					{
						strcpy(qtFuncStatusInfo.splFunc, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
						qtFuncStatusInfo.ifUserInfo = TRUE;
						userIdx = GetUserIdxBeingEnrolled();
						sprintf(qtFuncStatusInfo.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
						qtFuncStatusInfo.user.userId = GetUserIdFromIdx(userIdx);
						strcpy(qtFuncStatusInfo.user.name, GetUserName(userIdx) );
						qtFuncStatusInfo.user.photo = TRUE;
						qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_SHOW_NEXT_CARD;
						strcpy(qtFuncStatusInfo.genInfo.line2,GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_NEXT_CARD_698));
						qtFuncStatusInfo.genInfo.ifEscBtn = TRUE;
						SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);
					}
					else
					{
						SPECIAL_FUNCTIONS_e	splFnIdx;

						splFnIdx = GetSplFnIdxBeingEnrolled();
						strcpy(qtDynamicSplFuncInfo.splFunc, GetDisplayMessage(FALSE,NULL_CHAR,ActivateSplFnStr[splFnIdx]));
						qtDynamicSplFuncInfo.splFuncImageIdx = splFnIdx;
						strcpy(qtDynamicSplFuncInfo.noticeLine, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_NEXT_CARD_698));
						qtDynamicSplFuncInfo.ifEscBtn = TRUE;

						currentEnrollScreen = GET_SPL_FN_CARD_SCREEN;
						SendMessageToQt(&qtDynamicSplFuncInfo, GUI_MSG_SPL_FUNC_DYNAMIC_SCREEN);
					}
					SetLcdIdleTimer(ENROLLMENT_TIMER);
					break;
				}

				case USER_RESP_NEXT_FINGER:
				{
					strcpy(qtFuncStatusInfo.splFunc, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
					qtFuncStatusInfo.ifUserInfo = TRUE;
					userIdx = GetUserIdxBeingEnrolled();
					sprintf(qtFuncStatusInfo.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
					qtFuncStatusInfo.user.userId = GetUserIdFromIdx(userIdx);
					strcpy(qtFuncStatusInfo.user.name, GetUserName(userIdx) );
					qtFuncStatusInfo.user.photo = TRUE;
					strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PLACE_NEXT_FINGER_804));
					qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_PLACE_FINGER;
					qtFuncStatusInfo.genInfo.ifEscBtn = TRUE;
					SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);

					SetLcdIdleTimer(ENROLLMENT_TIMER);

					break;
				}

				case USER_RESP_CARD_ENROLLED:
				{
					strcpy(qtDynamicInfo.titleText, "");
					qtDynamicInfo.imageIdx = POPUP_ENROLLMENT_SUCCESS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_SUCCESS_199));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
					break;
				}

				case USER_RESP_ACS_ENROLLED:
					{
						strcpy(qtDynamicInfo.titleText, "");
						qtDynamicInfo.imageIdx = POPUP_ENROLLMENT_SUCCESS;
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_SUCCESS_199));

						SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
						SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
						break;
					}

				case USER_RESP_FP_ENROLLED:
				{
					UINT8 	imageQualityScore;
					CHAR	tmpString[25];

					imageQualityScore = *((UINT8PTR)dataPtr);
					// construct 'Quality=xxx% Week/Moderate/Strong/Strongest' message
					strcpy(tmpString, GetDisplayMessage(TRUE, CHAR_COLON, MSG_QUALITY_SCORE_475));
					sprintf(&tmpString[strlen(GetDisplayMessage(TRUE, CHAR_COLON,MSG_QUALITY_SCORE_475))], "%d", imageQualityScore);
					strcat(tmpString, "%");
					if(imageQualityScore < 40)
					{
						strcat(tmpString, GetDisplayMessage(FALSE,NULL_CHAR,fpQualityStr[FP_QUALITY_WEAK]));
					}
					else if(imageQualityScore < 60)
					{
						strcat(tmpString, GetDisplayMessage(FALSE,NULL_CHAR,fpQualityStr[FP_QUALITY_MODERATE]));
					}
					else if(imageQualityScore < 80)
					{
						strcat(tmpString, GetDisplayMessage(FALSE,NULL_CHAR,fpQualityStr[FP_QUALITY_STRONG]));
					}
					else
					{
						strcat(tmpString, GetDisplayMessage(FALSE,NULL_CHAR,fpQualityStr[FP_QUALITY_STRONGEST]));
					}
					qtDynamicInfo.imageIdx = POPUP_ENROLLMENT_SUCCESS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_SUCCESS_199));
					strcpy(qtDynamicInfo.message2, tmpString);

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
 					break;
				}

				case USER_RESP_ENROLL_FAIL:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
					qtDynamicInfo.imageIdx = POPUP_ENROLLMENT_FAILED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_FAILED_197));
					if(dataPtr != NULL)
					{
						strcpy(qtDynamicInfo.message2, (CHARPTR)dataPtr);
					}
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
 					break;
				}

				case USER_RESP_ENROLL_TIMEOUT:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TIME_OUT_605));
					qtDynamicInfo.imageIdx = POPUP_TIMEOUT;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_FAILED_197));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);

 					break;
				}

				case USER_RESP_ADD_USER_WAIT_TIME_OUT:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TIME_OUT_605));
					qtDynamicInfo.imageIdx = POPUP_TIMEOUT;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FAILED_TO_SYNC_USER_CREDENTIALS_892));
					strcpy(qtDynamicInfo.message2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRY_AGAIN_LATER_841));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				case USER_RESP_NETWORK_ERROR:
				{
					SendMessageToQt("", GUI_MSG_NET_CONN_NOT_AVAILABLE_SCREEN);
					break;
				}

				case USER_RESP_WRITE_CARD_API:
				case USER_RESP_WRITING_CARD:
				{
					if(userResp == USER_RESP_WRITING_CARD)
					{
						strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
					}
					qtDynamicInfo.imageIdx = POPUP_WRITING_SMART_CARD;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_WRITING_SMART_CARD_669));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

					SetLcdIdleTimer(ENROLLMENT_TIMER);

					break;
				}

				case USER_RESP_FACE_ENROLL_WAITING:
				{
					EPRINT(DC_LOG, "USER_RESP_FACE_ENROLL_WAITING SENT");
					currentEnrollScreen = ENROLL_USER_FACE_SCREEN;
					SendMessageToQt("", GUI_MSG_FACE_DETECTING_WAIT_SCREEN);
					break;
					//fallthroug for QT Face enrollment input process
				}
				case USER_RESP_GET_USER_INPUT:
				{
					switch(DcInfo.splFnId)
					{
						case ENROL_MODE_DELETE_DATA:
						{
							DELETE_USER_CREDEN_t deleteInput;

							deleteInput.credType = SysAdvPara.enrollmentType;

							SendMessageToQt(&deleteInput, GUI_MSG_DEL_CRED_SCREEN);
							SetLcdIdleTimer(MENU_TIMER);
							break;
						}

						case ENROL_USER:
						{
							ENROLL_USER_t enrollInput;

							if(EnrollmentTempInputs.enrollRepeatF == FALSE)
							{
								// For the first time Enrollment
                                  EnrollmentTempInputs.faceCnt=1;
								if(SysAdvPara.enrollmentType == ENROLL_FACE_ONLY)
									enrollInput.credType  = 4;
								else if((SysAdvPara.enrollmentType == ENROLL_DURESS_FP))
									enrollInput.credType  = 5;
								else
									enrollInput.credType  = SysAdvPara.enrollmentType;
								if(SysAdvPara.cardCount > MAX_USER_CARD)
								{
									enrollInput.cardCount = MAX_USER_CARD;
								}
								else
								{
									enrollInput.cardCount = SysAdvPara.cardCount;
								}
								
								    if(SysAdvPara.fpCount > SysAdvPara.maxFingerPerUser)
								    {
								            if(EnrollmentTempInputs.enrollmentType == ENROLL_DURESS_FP)
								            {
									enrollInput.fpCount = MAX_USER_DURESS_FP;
								            }
								              else{
									    enrollInput.fpCount = SysAdvPara.maxFingerPerUser;
								            }
								    }
								    else
								    {
									    enrollInput.fpCount = SysAdvPara.fpCount;
								    }
								
								enrollInput.maxFaces = SysAdvPara.maxFacePerUser;
								
								snprintf(enrollInput.alphaNumId, ALPHANUM_ID_SIZE, "%s", "");
								snprintf(enrollInput.name, NAME_SIZE_UTF8, "%s", "");
								EnrollmentTempInputs.enrollRepeatF   = TRUE;
							}
							else
							{
								// For Enrollment repeat process send temp.
								// stored of selected data
								if(EnrollmentTempInputs.enrollmentType == ENROLL_FACE_ONLY)
									enrollInput.credType  = 4;
								else if((EnrollmentTempInputs.enrollmentType == ENROLL_DURESS_FP))
									enrollInput.credType  = 5;
								else
									enrollInput.credType  = EnrollmentTempInputs.enrollmentType;
								enrollInput.cardCount = EnrollmentTempInputs.cardCnt;
								enrollInput.fpCount   = EnrollmentTempInputs.fpCnt;
								enrollInput.maxFaces	= EnrollmentTempInputs.faceCnt;

								snprintf(enrollInput.alphaNumId, ALPHANUM_ID_SIZE, "%s", EnrollmentTempInputs.alphaNumId);
								snprintf(enrollInput.name, NAME_SIZE_UTF8, "%s", EnrollmentTempInputs.name);
							}

							// Update Current Enrollment Screen
							currentEnrollScreen = ENROLL_USER_SCREEN;
							SendMessageToQt(&enrollInput, GUI_MSG_ENROLL_USER_SCREEN);
							SetLcdIdleTimer(MENU_TIMER);
							break;
						}

						case ENROL_SPECIAL_CARDS:
						{
							// Update Current Enrollment Screen
							currentEnrollScreen = ENROLL_SPL_FN_CARD_SCREEN;
							SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_ENROLL_SPL_FN_SCREEN);
							SetLcdIdleTimer(MENU_TIMER);
							break;
						}
						default:
							break;
					}
					break;
				}

				case USER_RESP_GET_USER_CREDENTIAL:
				{
					switch(DcInfo.splFnId)
					{
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
						{
							strcpy(qtDynamicSplFuncInfo.splFunc, GetDisplayMessage(FALSE,NULL_CHAR,ActivateSplFnStr[DcInfo.splFnId]));
							qtDynamicSplFuncInfo.splFuncImageIdx = DcInfo.splFnId;
							strcpy(qtDynamicSplFuncInfo.noticeLine, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_YOUR_ID_561));
							qtDynamicSplFuncInfo.ifPinEntryBtn = TRUE;
							qtDynamicSplFuncInfo.ifEscBtn = TRUE;

							SendMessageToQt(&qtDynamicSplFuncInfo, GUI_MSG_SPL_FUNC_DYNAMIC_SCREEN);
							SetLcdIdleTimer(SysAdvPara.multiAccessTimer);
							break;
						}

						default:
							break;
					}
 					break;
				}

				case USER_RESP_USER_NOT_FOUND:
				{
					strcpy(qtDynamicInfo.titleText, "");
					qtDynamicInfo.imageIdx = POPUP_INVALID_USER_ID;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_USER_ID_293));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
 					break;
				}

				case USER_RESP_USER_CRED_DELETED:
				{
					strcpy(qtDynamicInfo.titleText, "");
					qtDynamicInfo.imageIdx = POPUP_CREDENTIAL_DELETE_SUCCESS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CREDENTIALS_DELETED_803));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
 					break;
				}

				case USER_RESP_SPL_FN_ACTIVATED:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,ActivateSplFnStr[DcInfo.splFnId]));
					switch(DcInfo.splFnId)
					{
						case LOCK_DOOR:
						{
							qtDynamicInfo.imageIdx = POPUP_DOOR_LOCK_SUCCESS;
							strcpy(qtDynamicInfo.message1,GetDisplayMessage(FALSE,NULL_CHAR,MSG_SUCCESS_594));
							break;
						}
						case UNLOCK_DOOR:
						{
							qtDynamicInfo.imageIdx = POPUP_DOOR_UNLOCK_SUCCEESS;
							strcpy(qtDynamicInfo.message1,GetDisplayMessage(FALSE,NULL_CHAR,MSG_SUCCESS_594));
							break;
						}
						case NORMAL_DOOR:
						{
							qtDynamicInfo.imageIdx = POPUP_DOOR_NORMAL_SUCCESS;
							strcpy(qtDynamicInfo.message1,GetDisplayMessage(FALSE,NULL_CHAR,MSG_SUCCESS_594));
							break;
						}
						case CLEAR_SYS_ALARM:
						{
							qtDynamicInfo.imageIdx = POPUP_CLEAR_ALARM;
							strcpy(qtDynamicInfo.message1,GetDisplayMessage(FALSE,NULL_CHAR,MSG_SUCCESS_594));
							break;
						}
						case LATE_START_MARKING_ACT:
						{
							qtDynamicInfo.imageIdx = POPUP_LATEIN_START;
							strcpy(qtDynamicInfo.message1,GetDisplayMessage(FALSE,NULL_CHAR,MSG_STARTED_582));
							break;
						}
						case LATE_START_MARKING_DEACT:
						{
							qtDynamicInfo.imageIdx = POPUP_LATEIN_STOP;
							strcpy(qtDynamicInfo.message1,GetDisplayMessage(FALSE,NULL_CHAR,MSG_STOPPED_588));
							break;
						}
						case EARLY_CLOSING_MARKING_ACT:
						{
							qtDynamicInfo.imageIdx = POPUP_EARLYOUT_START;
							strcpy(qtDynamicInfo.message1,GetDisplayMessage(FALSE,NULL_CHAR,MSG_STARTED_582));
							break;
						}
						case EARLY_CLOSING_MARKING_DEACT:
						{
							qtDynamicInfo.imageIdx = POPUP_EARLYOUT_STOP;
							strcpy(qtDynamicInfo.message1,GetDisplayMessage(FALSE,NULL_CHAR,MSG_STOPPED_588));
							break;
						}

						default:
							break;
					}
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
 					break;
				}

				case USER_RESP_SPL_FN_IS_ACTIVE:
				{
					if(DcInfo.multiUserSplCode == LATE_IN_ALLOWED)
					{
						strcpy(qtDynamicInfo.titleText, "");
						qtDynamicInfo.imageIdx = POPUP_LATEIN_ACTIVE;
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_LATE_IN_IS_ACTIVE_315));
					}
					else if(DcInfo.multiUserSplCode == EARLY_OUT_ALLOWED)
					{
						strcpy(qtDynamicInfo.titleText, "");
						qtDynamicInfo.imageIdx = POPUP_EARLYOUT_ACTIVE;
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_EARLY_OUT_IS_ACTIVE_181));
					}
					else
					{
						return;
					}
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
 					break;
				}

				case USER_RESP_ENROLLMENT_DISABLED:
				{
					qtDynamicInfo.imageIdx = POPUP_ENROLLMENT_INACTIVE;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_INACTIVE_198));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
 					break;
				}

				case USER_RESP_FUNCTION_DISABLED:
				{
					qtDynamicInfo.imageIdx = POPUP_FAILED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FUNCTION_NOT_ACTIVE_752));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_VERIFY_SELECTION:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
					qtDynamicInfo.imageIdx = POPUP_WRONG_SELECTION;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_WRONG_SELECTION_671));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
 					break;
				}

				case USER_RESP_VERIFY_READER:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
					qtDynamicInfo.imageIdx = POPUP_CONFIGURE_READER;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_NO_SUCH_READER_394));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
 					break;
				}

				case USER_RESP_VERIFY_CARD_TYPE:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
					qtDynamicInfo.imageIdx = POPUP_INVALID_CARD_TYPE;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_WRONG_CARD_TYPE_670));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
 					break;
				}

				case USER_RESP_CRED_LIMIT_EXCEEDS:
				{
					qtDynamicInfo.imageIdx = POPUP_CREDENTIAL_EXISTS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CREDENTIAL_EXISTS_106));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
 					break;
				}

				case USER_RESP_DUPL_USER_CARD:
				case USER_RESP_DUPL_USER_SPL_FN_CARD:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
					qtDynamicInfo.imageIdx = POPUP_CARDID_EXSIST;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CARD_ID_EXISTS_75));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
 					break;
				}

				case USER_RESP_DUPL_USER_FINGER:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
					qtDynamicInfo.imageIdx = POPUP_FINGER_EXSIST;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FINGER_PRINT_EXIST_240));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
 					break;
				}

				case USER_RESP_ENROLL_USER_NOT_FOUND:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
					qtDynamicInfo.imageIdx = POPUP_INVALID_USER_ID;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_USER_ID_293));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
 					break;
				}

				case USER_RESP_FP_MEM_FULL:
				{
					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
					qtDynamicInfo.imageIdx = POPUP_FP_MEMORY_FULL;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FP_MEMORY_FULL_248));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
 					break;
				}

				case USER_RESP_PLACE_FINGER_AGAIN:
				{
					strcpy(qtFuncStatusInfo.splFunc, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
					qtFuncStatusInfo.ifUserInfo = TRUE;
					userIdx = GetUserIdxBeingEnrolled();
					sprintf(qtFuncStatusInfo.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
					qtFuncStatusInfo.user.userId = GetUserIdFromIdx(userIdx);
					strcpy(qtFuncStatusInfo.user.name, GetUserName(userIdx) );
					qtFuncStatusInfo.user.photo = TRUE;
 					strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FIRST_SCAN_SUCCESSFUL_246));
					strcpy(qtFuncStatusInfo.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PLACE_THE_SAME_FINGER_439));
					qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_PLACE_FINGER;

					// Update Current Enrollment Screen
					currentEnrollScreen = GET_USER_CREDENTIAL_SCREEN;
					SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);
					SetLcdIdleTimer(ENROLLMENT_TIMER);
 					break;
				}

				case USER_RESP_ENROLL_TRY_AGAIN_FINGER:
				{
					strcpy(qtFuncStatusInfo.splFunc, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_196));
					qtFuncStatusInfo.ifUserInfo = TRUE;
					userIdx = GetUserIdxBeingEnrolled();
					sprintf(qtFuncStatusInfo.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
					qtFuncStatusInfo.user.userId = GetUserIdFromIdx(userIdx);
					strcpy(qtFuncStatusInfo.user.name, GetUserName(userIdx) );
					qtFuncStatusInfo.user.photo = TRUE;
					strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_FAILED_197));
 					strcpy(qtFuncStatusInfo.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_POOR_IMAGE_QUALITY_447));
					qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_PLACE_FINGER;

					// Update Current Enrollment Screen
					currentEnrollScreen = GET_USER_CREDENTIAL_SCREEN;
					SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);
					SetLcdIdleTimer(ENROLLMENT_TIMER);
					break;
				}

				case USER_RESP_READ_CARD_API:
				case USER_RESP_READING_FROM_CARD:
				{
					qtDynamicInfo.imageIdx = POPUP_READING_CARD;
					strcpy(qtDynamicInfo.message1,GetDisplayMessage(FALSE,NULL_CHAR,MSG_READING_DATA_488));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					if(userResp == USER_RESP_READING_FROM_CARD)
					{
						SetLcdIdleTimer(SysAdvPara.multiAccessTimer);
					}
					else
					{
						SetLcdIdleTimer(ENROLLMENT_TIMER);
					}
					break;
				}

				case USER_RESP_ENTER_PIN:
				{
					SendMessageToQt(" ", GUI_MSG_PIN_ENTER_SCREEN);
					SetLcdIdleTimer(SysAdvPara.interDigitTimer);
					break;
				}

				case USER_RESP_BACKUP_UPDATE_PROGRESS:
				{
					USER_RESP_BU_STATUS_t * statusPtr = dataPtr;
					BACKUP_UPDATE_PROGRESS_QT_t buProg;

					strcpy(qtFuncStatusInfo.splFunc, GetDisplayMessage(FALSE,NULL_CHAR,buProgStrPtr[statusPtr -> buType]));
					strcpy(buProg.splFunc, qtFuncStatusInfo.splFunc);
					sprintf(buProg.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PLEASE_WAIT_444));
					buProg.prog = statusPtr -> prog;
					SendMessageToQt(&buProg, GUI_MSG_BACKUP_UPDATE_PROG);

					SetLcdIdleTimer(FW_PROG_TIMER);
					break;
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
				case USER_RESP_BENCHMARK_FW_FOUND:
				{
					BACKUP_UPDATE_TYPE_e * buType = dataPtr;

					strcpy(qtDynamicInfo.titleText, (VOIDPTR)GetDisplayMessage(FALSE,NULL_CHAR,buTypeLabelStrPtr[*buType]));

					switch(userResp)
					{
						default:
							return;

					case USER_RESP_BACKUP_UPDATE_NO_EVENT:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_EVENTS_NOT_FOUND_223));
						qtDynamicInfo.imageIdx = POPUP_EVENTS_FOUND;
						break;

					case USER_RESP_BACKUP_UPDATE_XFER_FAILED:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRANSFER_FAIL_740));
						qtDynamicInfo.imageIdx = POPUP_FAILED;
						break;

					case USER_RESP_BACKUP_UPDATE_XFER_SUCCESS:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TRANSFER_SUCCESSFUL_612));
						if(*buType == BU_TYPE_FIRMWARE)
						{
							strcpy(qtDynamicInfo.message2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DEVICE_RESTARTING_135));
						}

						qtDynamicInfo.imageIdx = POPUP_FIRMWARE_TRANSFER_SUCCESS;
						
						break;

					case USER_RESP_BACKUP_UPDATE_NO_MENORY:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INSUFFICIENT_MEMORY_276));
						qtDynamicInfo.imageIdx = POPUP_INSUFFISIENT_MEMORY;
						break;

					case USER_RESP_BACKUP_UPDATE_INVALID_DEVICE:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_DEVICE_280));
						qtDynamicInfo.imageIdx = POPUP_USB_INVALID_FILESYSTEM;
						break;

					case USER_RESP_BACKUP_UPDATE_FILE_NOT_FOUND:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FILE_NOT_FOUND_738));
						qtDynamicInfo.imageIdx = POP_UP_CONFIG_FILE_NOT_FOUND;
						break;

					case USER_RESP_BACKUP_UPDATE_FW_FILE_NOT_FOUND:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FIRMWARE_NOT_FOUND_244));
						qtDynamicInfo.imageIdx = POP_UP_FIRMWARE_NOT_FOUND;
						break;

					case USER_RESP_BACKUP_UPDATE_USB_NOT_FOUND:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_NO_USB_DEVICE_FOUND_395));
						qtDynamicInfo.imageIdx = POPUP_NO_USB_FOUND;
						break;

					case USER_RESP_FIRMWARE_UPLOAD_STATUS_SUCCESS:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SUCCESS_594));
					
						qtDynamicInfo.imageIdx = POPUP_SUCCESS;
						break;

					case USER_RESP_FIRMWARE_UPLOAD_STATUS_FAIL:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FAIL_745));
							qtDynamicInfo.imageIdx = POPUP_FAILED;
						break;

					case USER_RESP_BACKUP_UPDATE_CHECKSUM_FAIL:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CHECKSUM_FAIL_744));
							qtDynamicInfo.imageIdx = POPUP_FAILED;
						break;

					case USER_RESP_BACKUP_UPDATE_DOWNGRAGE_CNTRL:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DOWNGRADE_NOT_ALLOWED_743));
							qtDynamicInfo.imageIdx = POPUP_FAILED;
						break;

					case USER_RESP_BENCHMARK_FW_FOUND:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FAIL_745));
							qtDynamicInfo.imageIdx = POPUP_FAILED;
						break;
					}

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(MENU_USER_RESP_TIMER);
					break;
				}

				case USER_RESP_USB_DISCONNECTED:
				case USER_RESP_USB_CONNECTED:
				case USER_RESP_USB_INVALID_FS:
				{
					USB_STATUS_EV_DATA_t * evData = dataPtr;

					switch(evData -> status)
					{
					default:
						break;

					case USB_DISCONNECTED:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_USB_DEVICE_REMOVED_624));
						qtDynamicInfo.imageIdx = POPUP_USB_DISCONNECTED;
						break;

					case USB_CONNECTED:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_USB_DEVICE_DETECTED_623));
						qtDynamicInfo.imageIdx = POPUP_USB_CONNECTED;
						break;

					case USB_INVALID_FS:
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_USB_FS_NOT_SUPPORTED_737));
						qtDynamicInfo.imageIdx = POPUP_USB_INVALID_FILESYSTEM;
						break;
					}

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(MENU_USER_RESP_TIMER);
					break;
				}

				case USER_RESP_MMC_DEVICE_STATUS:
				{
					SendMessageToQt("", GUI_MSG_SET_MMC_DEV_PATH);
					break;
				}

				case USER_RESP_ENTER_PASSWD:
				{
					break;
				}

				case USER_RESP_CAFE_CMD_SUCCESS:
				{
					USER_COMMON_QT_t cafeAllow = defltQtUsrCommon;
					INT32 usrBalanceDecimal, usrBalanceFraction;
					INT32 newBalanceDecimal, newBalanceFraction;
					INT32 cmdAmountDecimal, cmdAmountFraction;

					usrBalanceDecimal = (INT32)(CafeSetCmd.usrBalance / DOUBLE_PRECISION_FACTOR);
					usrBalanceFraction = (INT32)(CafeSetCmd.usrBalance % DOUBLE_PRECISION_FACTOR);
					if(usrBalanceFraction < 0)
					{
						usrBalanceFraction *= (-1);
					}
					newBalanceDecimal = (INT32)(( CafeSetCmd.usrBalance + CafeSetCmd.amount) / DOUBLE_PRECISION_FACTOR);
					newBalanceFraction = (INT32)(( CafeSetCmd.usrBalance + CafeSetCmd.amount) % DOUBLE_PRECISION_FACTOR);
					if(newBalanceFraction < 0)
					{
						newBalanceFraction *= (-1);
					}
					cmdAmountDecimal = (INT32)(CafeSetCmd.amount / DOUBLE_PRECISION_FACTOR);
					cmdAmountFraction = (INT32)(CafeSetCmd.amount % DOUBLE_PRECISION_FACTOR);
					if(cmdAmountFraction < 0)
					{
						cmdAmountFraction *= (-1);
					}
					userIdx = DcInfo.userIdx;

					sprintf(cafeAllow.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
					cafeAllow.user.userId = GetUserIdFromIdx(userIdx);
					strcpy(cafeAllow.user.name, GetUserName(userIdx) );
					cafeAllow.user.photo = TRUE;
					cafeAllow.cafeSepF = FALSE;

					if(CafeSetCmd.cmd == CAFE_RECHARGE_TRANSACTION)
					{
						sprintf(cafeAllow.genInfo.line1, "%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_RECHARGE_SUCCESSFUL_490));
						sprintf(cafeAllow.genInfo.line3, "%s : %0d.%02d", GetDisplayMessage(TRUE,CHAR_COLON,MSG_NEW_BALANCE_382), newBalanceDecimal, newBalanceFraction);
					}
					else if(CafeSetCmd.cmd == CAFE_RESET_TRANSACTION)
					{
						sprintf(cafeAllow.genInfo.line1, "%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_RESET_SUCCESSFUL_496));
						sprintf(cafeAllow.genInfo.line3, "%s : %0d.%02d", GetDisplayMessage(TRUE,CHAR_COLON,MSG_NEW_BALANCE_382), cmdAmountDecimal, cmdAmountFraction);
					}
					else if(CafeSetCmd.cmd == CAFE_READ_BAL_TRANSACTION)
					{
						if(GetUserPaymentModeForCafe(DcInfo.userIdx) == POSTPAID)
						{
							screenType = 3;
							sprintf(cafeAllow.genInfo.line3, "%s : %s", GetDisplayMessage(TRUE,CHAR_COLON,MSG_CURRENT_MONTH_USAGE_787), CafeSetCmd.usrBalanceStr);
						}
						else if(GetUserPaymentModeForCafe(DcInfo.userIdx) == PREPAID_SERVER_SIDE)
						{
							sprintf(cafeAllow.genInfo.line3, "%s : %s", GetDisplayMessage(TRUE,CHAR_COLON,MSG_BALANCE_61), CafeSetCmd.usrBalanceStr);
						}
						else
						{
							sprintf(cafeAllow.genInfo.line3, "%s : %d.%02d", GetDisplayMessage(TRUE,CHAR_COLON,MSG_BALANCE_61), usrBalanceDecimal, usrBalanceFraction);
							sprintf(CafeSetCmd.usrBalanceStr, "%d.%02d", usrBalanceDecimal, usrBalanceFraction);
							dataLen = sprintf(dataStr, "%u%c%u%c%s%c%s%c%s%c", USER_BALANCE_STATE, CAFE_SCR_FS, screenType, CAFE_SCR_FS,
											GetAlphaNumId(userIdx), CAFE_SCR_FS, GetUserName(DcInfo.userIdx), CAFE_SCR_FS, CafeSetCmd.usrBalanceStr, CAFE_SCR_FS);
							SendDataToAllMirrorDisplayDevices(RSP_GET_STATE, dataStr, dataLen);
						}
						cafeAllow.genInfo.imageIdx = CAFE_ALLOW_SCREEN_IMAGE_VIEW_BALANCE_SUCCESS;
					}
					SendMessageToQt(&cafeAllow, GUI_MSG_CAFE_USER_ALLOWED_COMMON_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_CAFE_CMD_FAIL:
				{
					USER_COMMON_QT_t deny = defltQtUsrCommon;

					userIdx = DcInfo.userIdx;

					sprintf(deny.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
					deny.user.userId = GetUserIdFromIdx(userIdx);
					strcpy(deny.user.name, GetUserName(userIdx) );
					deny.user.photo = TRUE;

					deny.genInfo.imageIdx = DENY_SCREEN_CAFE_FAILED;	//Image Index

					if(CafeSetCmd.cmd == CAFE_RECHARGE_TRANSACTION)
					{
						strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_RECHARGE_FAILED_489));
					}
					else if(CafeSetCmd.cmd == CAFE_RESET_TRANSACTION)
					{
						strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_RESET_FAILED_495));
					}
					else if(CafeSetCmd.cmd == CAFE_READ_BAL_TRANSACTION)
					{
						strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CARD_READ_FAILED_77));
						screenType = 1;
						dataLen = sprintf(dataStr, "%u%c%u%c", USER_BALANCE_STATE, CAFE_SCR_FS, screenType, CAFE_SCR_FS);
						SendDataToAllMirrorDisplayDevices(RSP_GET_STATE, dataStr, dataLen);
					}

					SendMessageToQt(&deny, GUI_MSG_CAFE_USER_DENIED_COMMON_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_TRANSACTION_WITHOUT_DISCOUNT:
				{
					USER_QT_t	userInfo;
					userIdx = DcInfo.userIdx;

					sprintf(userInfo.alphaNumId, "%s", GetAlphaNumId(userIdx) );
					userInfo.userId = GetUserIdFromIdx(userIdx);
					strcpy(userInfo.name, GetUserName(userIdx) );
					userInfo.photo = TRUE;

					SendMessageToQt(&userInfo, GUI_MSG_TRANSACTION_WITHOUT_DISCOUNT);
					SetLcdIdleTimer(MENU_TIMER);
					break;
				}

				case USER_RESP_CAFE_CMD_TIMEOUT:
				{
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_NET_CONN_NOT_AVAILABLE_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_WPS_PROCESSING:
				{
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PLEASE_WAIT_444));
					qtDynamicInfo.imageIdx = POPUP_PROCESSING;

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(MENU_TIMER);
					break;
				}

				case USER_RESP_WIFI_CONNECTED:
				{
					qtDynamicInfo.imageIdx = POPUP_NETWORK_ESTABLISHED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_NETWORK_ESTABLISHED_376));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				// PD Responses
				case USER_RESP_PD_USER_ALLOWED:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_USER_ALLOWED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ACCESS_ALLOWED_21));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessAllowScreenDuration);
					break;
				}

				case USER_RESP_PD_USER_DENIED:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_USER_DENIED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ACCESS_DENIED_0));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}

				case USER_RESP_PD_READING_CARD:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_READING_CARD;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_READING_DATA_756));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(READER_RESP_TIMER);
					break;
				}

				case USER_RESP_PD_PLACE_YOUR_FINGER:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_PLACE_YOUR_FINGER;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PLACE_YOUR_FINGER_440));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(READER_RESP_TIMER);
					break;
				}

				case USER_RESP_PD_INVALID_CARD:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_INVALID_CARD;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SORRY_INVALID_CARD_569));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}

				case USER_RESP_PD_CARD_READ_FAILED:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_CARD_READ_FAILED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CARD_READ_FAILED_77));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}

				case USER_RESP_PD_INVALID_PASSWORD:
				{
                UINT8 msgLen = 0;
                CONFIG_USER_TYPE_e userType = *(CONFIG_USER_TYPE_e *)dataPtr;
                strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_LOGIN_FAILED_328));
                qtDynamicInfo.imageIdx = POPUP_INAVALID_PASSWORD;

                if(Password.DispPwdLoginRestrictTimer[userType] == INVALID_TIMER_HANDLE)
                {
                    strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_PASSWORD_288));

                    msgLen += snprintf(qtDynamicInfo.message2, QT_MSG_SIZE, "%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_YOU_HAVE_1098));
                    snprintf(qtDynamicInfo.message2+msgLen, QT_MSG_SIZE, " %d %s", GetDispPwdAttemptCount(userType), GetDisplayMessage(FALSE,NULL_CHAR,MSG_MORE_LOGIN_ATTEMPT_S_1099));
                }
                else
                {
                    msgLen += snprintf(qtDynamicInfo.message1, QT_MSG_SIZE, "%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_ACCOUNT_LOCKED_TRY_AFTER_1100));
                    snprintf(qtDynamicInfo.message1+msgLen, QT_MSG_SIZE, " %d %s", GetDispPwdAttemptCount(userType), GetDisplayMessage(FALSE,NULL_CHAR,MSG_MINUTE_S_1101));
                }
                SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
                SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_PD_MENU_TIMEOUT:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_MENU_TIMEOUT;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_TIME_OUT_605));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_PD_SYSTEM_DEFAULTING:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_SYSTEM_DEFAULTING;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DEFAULTING_126));

					strcpy(qtDynamicInfo.message2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PLEASE_WAIT_444));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				case USER_RESP_PD_SYSTEM_DEFAULTED:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_SYSTEM_DEFAULTED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SYSTEM_DEFAULTED_596));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_PD_FIRMWARE_UPLOAD_SUCCESS:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_FIRMWARE_UPLOAD_SUCCESS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SUCCESS_594));

					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FIRMWARE_UPGRADE_245));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_PD_FIRMWARE_UPLOAD_FAIL:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_FIRMWARE_UPLOAD_FAIL;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FAILED_234));

					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FIRMWARE_UPGRADE_245));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_PD_COMMUNICATION_ERROR:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_COMMUNICATION_ERROR;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_RESTRICTED_MENU_OPERATIONS_498));

					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_COMMUNICATION_ERROR_92));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				// TODO: to remove this case??
				case USER_RESP_PD_FIRMWARE_XFER_SUCCESS:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_FIRMWARE_XFER_SUCCESS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SUCCESS_594));

					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FIRMWARE_UPGRADE_245));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				// TODO: to remove this case??
				case USER_RESP_PD_FIRMWARE_XFER_FAIL:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_USER_ALLOWED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FAILED_234));

					strcpy(qtDynamicInfo.titleText, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FIRMWARE_UPGRADE_245));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}
				// NOTE: Do not load display idle timer as MENU state will take care of
				// what to display upon timeout
				case USER_RESP_PD_SEARCHING_WIFI:
				{
					strcpy(qtDynamicInfo.titleText, "");
					qtDynamicInfo.imageIdx = PD_POPUP_SEARCHING_FOR_WIFI;
					strcpy(qtDynamicInfo.message1,  GetDisplayMessage(FALSE,NULL_CHAR,MSG_SEARCHING_FOR_WIFI_ACCESS_POINTS_813));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}
				case USER_RESP_PD_WPS_PROCESSING:
				{
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PLEASE_WAIT_444));
					qtDynamicInfo.imageIdx = PD_POPUP_PROCESSING;

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(MENU_TIMER);
					break;
				}

				case USER_RESP_PD_WIFI_CONNECTED:
				{
					qtDynamicInfo.imageIdx = PD_POPUP_NETWORK_ESTABLISHED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_NETWORK_ESTABLISHED_376));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					break;
				}

				case USER_RESP_OPEN_DOOR_WEB_API_COMMAND:
				{
					USER_ALLOWED_QT_t allow = defltQtUsrAllowed;

					sprintf(allow.user.alphaNumId, "%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_VISITOR_644));
					strcpy(allow.user.name, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VISITOR_644));
					allow.user.photo = TRUE;

					if(DispDoorModeF == ENABLED)
					{
						if(DoorEntryExitMode == ENTRY_READER)
						{
							strcpy(allow.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENTRY_ALLOWED_758));
						}
						else if(DoorEntryExitMode == EXIT_READER)
						{
							strcpy(allow.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_EXIT_ALLOWED_759));
						}
					}
					else
					{
						strcpy(allow.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ACCESS_ALLOWED_21));
					}

					GetCurrentTimeForUserAllowScreen(allow.allowTime);
					allow.genInfo.imageIdx = ALLOW_SCREEN_ACCESS_ALLOWED_IMAGE;	//Image Index

					SendMessageToQt(&allow, GUI_MSG_USER_ALLOWED);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessAllowScreenDuration);
					break;
				}

				case USER_RESP_SELF_ENROLL_REQ_INPUT:
				{
					SendMessageToQt("", GUI_MSG_SELF_ENROLLMENT_SCREEN);
					SetLcdIdleTimer(MENU_TIMER);
					break;
				}

				case USER_RESP_DO_YOU_ASSIGN_USER_ON_DEVICE:
				{
					ENROLL_USER_t enrollInput;
					snprintf(enrollInput.alphaNumId, ALPHANUM_ID_SIZE, "%s", EnrollmentTempInputs.alphaNumId);
					snprintf(enrollInput.name, NAME_SIZE_UTF8, "%s", EnrollmentTempInputs.name);

					SendMessageToQt(&enrollInput, GUI_MSG_ASSIGN_USER_ON_DEVICE);
					SetLcdIdleTimer(MENU_TIMER);
					break;
				}

				case USER_RESP_DO_YOU_ADD_ASSIGN_USER_ON_DEVICE:
				{
					ENROLL_USER_t enrollInput;
					snprintf(enrollInput.alphaNumId, ALPHANUM_ID_SIZE, "%s", EnrollmentTempInputs.alphaNumId);

					SendMessageToQt(&enrollInput, GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE);
					SetLcdIdleTimer(MENU_TIMER);
					break;
				}

				case USER_RESP_ACS_ENROLLMENT_START_ON_DEVICE:
				{
					strcpy(qtFuncStatusInfo.splFunc, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLL_USER_195));
					qtFuncStatusInfo.ifUserInfo = TRUE;
					userIdx = GetUserIdxBeingEnrolled();
					sprintf(qtFuncStatusInfo.user.alphaNumId, "%s", GetAlphaNumId(userIdx) );
					qtFuncStatusInfo.user.userId = GetUserIdFromIdx(userIdx);
					strcpy(qtFuncStatusInfo.user.name, GetUserName(userIdx) );
					qtFuncStatusInfo.user.photo = TRUE;
					strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_WAITING_FOR_MOBILE_DEVICE_976));
					qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_IMAGE_NONE;
					qtFuncStatusInfo.genInfo.ifEscBtn	= TRUE;

					// Update Current Enrollment Screen
					currentEnrollScreen = GET_USER_CREDENTIAL_SCREEN;
					SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);
					SetLcdIdleTimer(BLE_ENROLLMENT_TIMER);
					break;
				}

				case USER_RESP_VAM_ACCESS_ALLOWED:
				{
					USER_ALLOWED_QT_t		allow = defltQtUsrAllowed;

					sprintf(allow.user.alphaNumId, "%u", VamConfig[DcInfo.vamConfigIndex].passId);

					if(DcInfo.showDoorModeOnAllowF == ENABLED)
					{
						if(DcInfo.readerMode == ENTRY_READER)
						{
							strcpy(allow.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VEHICLE_ACCESS_ALLOWED_819));
						}
						else if(DcInfo.readerMode == EXIT_READER)
						{
							strcpy(allow.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VEHICLE_EXIT_ALLOWED_820));
						}
					}
					else
					{
						strcpy(allow.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VEHICLE_ACCESS_ALLOWED_819));
					}

					allow.user.photo = TRUE;
					if(DcInfo.authReason == AUTH_VAM_ALLOWED_SOFT_ROUTE)
					{
						strcpy(allow.genInfo.line3, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ACCESS_ROUTE_VIOLATED_23));
					}

					GetCurrentTimeForUserAllowScreen(allow.allowTime);

					allow.genInfo.imageIdx = ALLOW_SCREEN_ACCESS_ALLOWED_IMAGE;	//Image Index

					SendMessageToQt(&allow, GUI_MSG_USER_ALLOWED);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessAllowScreenDuration);
					break;
				}

				case USER_RESP_VAM_SHOW_2ND_CREDENTIAL:
				case USER_RESP_VAM_SHOW_3RD_CREDENTIAL:
				{
					userIdx = DcInfo.userIdx;
					qtFuncStatusInfo.ifUserInfo = TRUE;
					sprintf(qtFuncStatusInfo.user.alphaNumId, "%u", VamConfig[DcInfo.vamConfigIndex].passId);
					qtFuncStatusInfo.user.photo = TRUE;
					strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_NEXT_CARD_698));
					if(userResp == USER_RESP_VAM_SHOW_2ND_CREDENTIAL)
					{
						qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_SHOW_SECOND_CREDENTIAL;
						strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_2ND_CREDENTIAL_552));
					}
					else
					{
						qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_SHOW_THIRD_CREDENTIAL;
						strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SHOW_3RD_CREDENTIAL_553));
					}
					qtFuncStatusInfo.genInfo.ifPinEntryBtn = FALSE;
					SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);

					SetLcdIdleTimer(SysAdvPara.twoPersonTimer);
					break;
				}

				case USER_RESP_VAM_DOOR_LOCK:
				{
					USER_DENIED_QT_t deny = defltQtUsrDenied;

					deny.genInfo.imageIdx = DENY_SCREEN_ACCESS_DENIED_IMAGE;
					sprintf(deny.user.alphaNumId, "%u", VamConfig[DcInfo.vamConfigIndex].passId);
					deny.user.photo = TRUE;
					strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VEHICLE_ACCESS_DENIED_636));
					strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DOOR_IS_LOCKED_165));
					SendMessageToQt(&deny, GUI_MSG_USER_DENIED);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}

				case USER_RESP_VAM_VALIDITY_EXPIRED:
				{
					USER_DENIED_QT_t deny = defltQtUsrDenied;

					deny.genInfo.imageIdx = DENY_SCREEN_ACCESS_DENIED_IMAGE;
					sprintf(deny.user.alphaNumId, "%u", VamConfig[DcInfo.vamConfigIndex].passId);
					deny.user.photo = TRUE;
					strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VEHICLE_ACCESS_DENIED_636));
					strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_PASS_699));
					SendMessageToQt(&deny, GUI_MSG_USER_DENIED);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}

				case USER_RESP_VAM_CRED_INVALID_SEQ:
				{
					USER_DENIED_QT_t deny = defltQtUsrDenied;

					deny.genInfo.imageIdx = DENY_SCREEN_ACCESS_DENIED_IMAGE;
					sprintf(deny.user.alphaNumId, "%u", VamConfig[DcInfo.vamConfigIndex].passId);
					deny.user.photo = TRUE;
					strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VEHICLE_ACCESS_DENIED_636));
					strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_CARD_SEQUENCE_278));
					SendMessageToQt(&deny, GUI_MSG_USER_DENIED);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}

				case USER_RESP_VAM_INVALID_CARD:
				{
					USER_DENIED_QT_t deny = defltQtUsrDenied;

					deny.genInfo.imageIdx = DENY_SCREEN_ACCESS_DENIED_IMAGE;
					sprintf(deny.user.alphaNumId, "%u", VamConfig[DcInfo.vamConfigIndex].passId);
					deny.user.photo = TRUE;
					strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SORRY_INVALID_CARD_569));
					SendMessageToQt(&deny, GUI_MSG_USER_DENIED);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}

				case USER_RESP_VAM_INVALID_SAMRT_ROUTE_ACCESS:
				{
					USER_DENIED_QT_t deny = defltQtUsrDenied;

					deny.genInfo.imageIdx = DENY_SCREEN_ACCESS_DENIED_IMAGE;
					sprintf(deny.user.alphaNumId, "%u", VamConfig[DcInfo.vamConfigIndex].passId);
					deny.user.photo = TRUE;
					strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VEHICLE_ACCESS_DENIED_636));
					strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_ROUTE_ACCESS_289));
					SendMessageToQt(&deny, GUI_MSG_USER_DENIED);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}

				case USER_RESP_VAM_INVALID_SMART_ROUTE_INPUT:
				{
					USER_DENIED_QT_t deny = defltQtUsrDenied;

					deny.genInfo.imageIdx = DENY_SCREEN_ACCESS_DENIED_IMAGE;
					sprintf(deny.user.alphaNumId, "%u", VamConfig[DcInfo.vamConfigIndex].passId);
					deny.user.photo = TRUE;
					strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VEHICLE_ACCESS_DENIED_636));
					strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INVALID_ROUTE_INPUT_290));
					SendMessageToQt(&deny, GUI_MSG_USER_DENIED);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}

				case USER_RESP_VAM_SMART_ROUTE_CARD_WRITE_FAILED:
				{
					USER_DENIED_QT_t deny = defltQtUsrDenied;

					deny.genInfo.imageIdx = DENY_SCREEN_ACCESS_DENIED_IMAGE;
					sprintf(deny.user.alphaNumId, "%u", VamConfig[DcInfo.vamConfigIndex].passId);
					deny.user.photo = TRUE;
					strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VEHICLE_ACCESS_DENIED_636));
					strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CARD_WRITE_FAILED_80));
					SendMessageToQt(&deny, GUI_MSG_USER_DENIED);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}

				case USER_RESP_VAM_NO_ACCESS:
				{
					USER_DENIED_QT_t deny = defltQtUsrDenied;

					deny.genInfo.imageIdx = DENY_SCREEN_ACCESS_DENIED_IMAGE;
					sprintf(deny.user.alphaNumId, "%u", VamConfig[DcInfo.vamConfigIndex].passId);
					deny.user.photo = TRUE;
					strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VEHICLE_ACCESS_DENIED_636));
					strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENTRY_RESTRICTED_214));
					SendMessageToQt(&deny, GUI_MSG_USER_DENIED);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}

				case USER_RESP_VISITOR_ACCESS_DENY:
				{
					USER_DENIED_QT_t deny = defltQtUsrDenied;

					deny.genInfo.imageIdx = DENY_SCREEN_ACCESS_DENIED_IMAGE; //Image Index

					sprintf(deny.user.alphaNumId, "%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_VISITOR_644));
					strcpy(deny.user.name, GetDisplayMessage(FALSE,NULL_CHAR,MSG_VISITOR_644));
					deny.user.photo = TRUE;
					strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ACCESS_DENIED_0));
					strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENTRY_RESTRICTED_214));
					SendMessageToQt(&deny, GUI_MSG_USER_DENIED);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}

				case USER_RESP_OCCUPANCY_WAIT_FOR_NEXT_USER:
				{
					strcpy(qtFuncStatusInfo.splFunc, "");
					qtFuncStatusInfo.ifUserInfo = TRUE;
					sprintf(qtFuncStatusInfo.user.alphaNumId, "%s",GetAlphaNumId(DcInfo.userIdx) );
					qtFuncStatusInfo.user.userId = GetUserIdFromIdx(DcInfo.userIdx);
					strcpy(qtFuncStatusInfo.user.name, GetUserName(DcInfo.userIdx) );
					qtFuncStatusInfo.user.photo = TRUE;
					qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_WAIT_FOR_SECOND_PERSON;
					strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_WAITING_FOR_2ND_USER_649));
					qtFuncStatusInfo.genInfo.ifPinEntryBtn = TRUE;

					// Temperature
					snprintf(qtFuncStatusInfo.temperature, MAX_TEMPERATURE_LENGTH, "%.1f", DcInfo.usrTemperature);
					qtFuncStatusInfo.temperatureUnit = SysAdvPara.temperatureLogCnfg.tempUnit;
					/* Set user exceeds temperature threshold */
					if (DcInfo.usrExceedTempThreshold == TRUE)
					{
						qtFuncStatusInfo.temperatureViolation = TRUE;
					}
#if defined(FACE_MASK_COMPULSION_ENABLE)
                    if((IsGroupFrApplicable() == NO) &&
                            (SysAdvPara.faceMaskComp_Enable == TRUE))
                    {
						qtFuncStatusInfo.faceMaskDecision = DcInfo.isFaceMaskViolated ? QT_FACE_MASK_DECISION_ACCESS_ALLOWED_SOFT :
																						QT_FACE_MASK_DECISION_ACCESS_ALLOWED;
                    }

#endif
					SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);
					SetLcdIdleTimer(SysAdvPara.twoPersonTimer);
					break;
				}

				case USER_RESP_ACCESS_ALLOWED_WITH_JOBCODE:
				{
					USER_ALLOWED_QT_t allow = defltQtUsrAllowed;

					sprintf(allow.user.alphaNumId, "%s", GetAlphaNumId(DcInfo.userIdx) );
					allow.user.userId = GetUserIdFromIdx(DcInfo.userIdx);
					strcpy(allow.user.name, GetUserName(DcInfo.userIdx) );
					allow.user.photo = TRUE;

					if(	JcmConfig[DcInfo.selectedJobIdx].refId == NONE_JOB_REF_ID)
					{
						strcpy(allow.jobCodeInfo.jobActionStr, GetDisplayMessage(FALSE,NULL_CHAR,MSG_JOB_ENDED_686));
					}
					else if( JcmConfig[DcInfo.selectedJobIdx].refId == DEFAULT_JOB_REF_ID)
					{
						strcpy(allow.jobCodeInfo.jobActionStr, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DEFAULT_JOB_INITIATED_502));
					}
					else if( JcmConfig[DcInfo.selectedJobIdx].refId == CONTINUE_JOB_REF_ID)
					{
						strcpy(allow.jobCodeInfo.jobActionStr, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ONGOING_JOB_CONTINUED_509));
					}
					else
					{
						strcpy(allow.jobCodeInfo.jobCodeId, JcmConfig[DcInfo.selectedJobIdx].jobCode);
						strcpy(allow.jobCodeInfo.jobCodeName, JcmConfig[DcInfo.selectedJobIdx].jobName);
						strcpy(allow.jobCodeInfo.jobActionStr, GetDisplayMessage(FALSE,NULL_CHAR,MSG_INITIATED_685));
					}

 					SendMessageToQt(&allow, GUI_MSG_USER_ALLOWED);
 					SetLcdIdleTimerMilliSec(SysBasicPara.accessAllowScreenDuration);
					break;
				}

				case USER_RESP_PROCESSING_QUERY:
				{
					UINT8 lcdProgressTime = DISP_PROCESSING_TIMER;
					
					lcdProgressTime += AcmsPollDuration;
					qtDynamicInfo.imageIdx = POPUP_PROCESSING;
					qtDynamicInfo.footerHideF = FALSE;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PROCESSING_467));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(lcdProgressTime);
					break;
				}

				case USER_RESP_WAIT_FOR_SIGNAL:
				{
					UINT8 lcdProgressTime = DISP_PROCESSING_TIMER;

					if(dataPtr != NULL)
						lcdProgressTime = (UINT8)(*( (UINT8PTR)dataPtr) );

						qtDynamicInfo.imageIdx = POPUP_PROCESSING;
					qtDynamicInfo.footerHideF = TRUE;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PROCESSING_467));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(lcdProgressTime);
					break;
				}

				case USER_RESP_PLACE_FINGER_BIO_ONLY_MODE:
				{
					userIdx = DcInfo.userIdx;
					qtFuncStatusInfo.ifUserInfo = TRUE;
					sprintf(qtFuncStatusInfo.user.alphaNumId, "%s", GetAlphaNumId(userIdx));
					qtFuncStatusInfo.user.userId = GetUserIdFromIdx(userIdx);
					strcpy(qtFuncStatusInfo.user.name, GetUserName(userIdx));
					qtFuncStatusInfo.user.photo = TRUE;
					qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_PLACE_FINGER;
					strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE, NULL_CHAR, MSG_PLACE_YOUR_FINGER_440) );
					SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);

					SetLcdIdleTimer(SysAdvPara.multiAccessTimer);
					break;
				}

				case USER_RESP_SUCCESS_SCREEN:
				{
					qtDynamicInfo.imageIdx = POPUP_SUCCESS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_SUCCESS_594));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_FAIL_SCREEN:
				{
					MSG_MULTI_LANGUAGE_STRING_e	*displayMsgIdxPtr = (MSG_MULTI_LANGUAGE_STRING_e *)dataPtr;
					MSG_MULTI_LANGUAGE_STRING_e	displayMsgIdx = MSG_FAIL_745;

					if( (displayMsgIdxPtr != NULL) && ( (*displayMsgIdxPtr) < MSG__0) )
					{
						displayMsgIdx = (*displayMsgIdxPtr);
					}

					qtDynamicInfo.imageIdx = POPUP_FAILED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,displayMsgIdx));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case MODE_NOT_SUPPORTED_SCREEN:
				{
					qtDynamicInfo.imageIdx = POPUP_FAILED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_MODE_NOT_SUPPORTED_12));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_PIN_CHANGED:
				{
					qtDynamicInfo.imageIdx = POPUP_SUCCESS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PIN_CHANGED_869));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_PIN_EXISTS:
				{
					qtDynamicInfo.imageIdx = POPUP_FAILED;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PIN_ALREADY_EXISTS_868));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case FTP_FIRMWARE_INITILIZE:
				{
					SendMessageToQt("", GUI_MSG_INIT_FIRMWARE_UPGRADE);
					SetLcdIdleTimer(MENU_TIMER);
					break;
				}

				case USER_RESP_ATTENDANCE_DETAIL:
				{
					SendMessageToQt(dataPtr, GUI_MSG_SHOW_USER_ATTENDANCE_DETAIL);
					if(DcInfo.isLcdTimerRunning == FALSE)
					{
						SetLcdIdleTimer(LCD_IDLE_TIMER_ATNDC_SUMMARY_2SEC);
					}
					break;
				}

				case APPLY_LEAVE_RESPONSE:
				{
					SendMessageToQt(dataPtr, GUI_MSG_APPLY_LEAVE_RESPONSE);
					SetLcdIdleTimer(5);

					break;
				}

				case USER_RESP_FAILED_TO_CONNECT_CAMERA:
				{
					if(DcInfo.curState != DC_CAFETERIA_STATE)
					{
						SendMessageToQt("", GUI_MSG_FR_CAMERA_CONNECTION_FAILED);
						SetLcdIdleTimer(LCD_IDLE_TIMER);
						SwitchDcToIdleState();
					}
					break;
				}

				case USER_RESP_TEMPERATURE_SENSOR_DISCONNECTED:
				{
					DYNAMIC_TEMP_SCREEN_INFO_QT_t		qtDynamicTempScreenInfo = deflQtDynamicTempScreenInfo;
					qtDynamicTempScreenInfo.footerHideF = TRUE;
					qtDynamicTempScreenInfo.ImageHideF = FALSE;
					qtDynamicTempScreenInfo.imageIdx = POPUP_IMAGE_SENSOR_DISCONNECTED;

					SendMessageToQt(&qtDynamicTempScreenInfo, GUI_MSG_DYNAMIC_TEMPERATURE_SCREEN);

					/* Redirect to menu screen in case device is in menu state */
					if ((DcInfo.curState == DC_MENU_STATE)
							&& (tempSensorInfo.isTempQueryInBackground == FALSE))
					{
						SetTemperatureIdleTimer(LCD_IDLE_TIMER);
					}
					else
					{
						SetLcdIdleTimer(LCD_IDLE_TIMER);
					}
					break;
				}

				case USER_RESP_DETECTING_TEMPERATURE_SCREEN:
				{
					DYNAMIC_TEMP_SCREEN_INFO_QT_t		qtDynamicTempScreenInfo = deflQtDynamicTempScreenInfo;
					qtDynamicTempScreenInfo.footerHideF = FALSE;
					qtDynamicTempScreenInfo.ImageHideF = FALSE;
					userIdx = DcInfo.userIdx;
					snprintf(qtDynamicTempScreenInfo.user.alphaNumId,sizeof(qtDynamicTempScreenInfo.user.alphaNumId),"%s", GetAlphaNumId(userIdx));
					qtDynamicTempScreenInfo.user.userId = GetUserIdFromIdx(userIdx);
					snprintf(qtDynamicTempScreenInfo.user.name,sizeof(qtDynamicTempScreenInfo.user.name),"%s",GetUserName(userIdx));
					qtDynamicTempScreenInfo.imageIdx = POPUP_IMAGE_DETECTING_TEMPERATURE;
					SendMessageToQt(&qtDynamicTempScreenInfo, GUI_MSG_DYNAMIC_TEMPERATURE_SCREEN);
					break;
				}

				case USER_RESP_MINIMUM_TEMP_RCVD:
				{
					DYNAMIC_TEMP_SCREEN_INFO_QT_t		qtDynamicTempScreenInfo = deflQtDynamicTempScreenInfo;
					qtDynamicTempScreenInfo.footerHideF = TRUE;
					qtDynamicTempScreenInfo.ImageHideF = TRUE;
					qtDynamicTempScreenInfo.imageIdx = POPUP_TEMP_IMAGE_MAX;
					snprintf(qtDynamicTempScreenInfo.titleText, sizeof(qtDynamicTempScreenInfo.titleText), GetDisplayMessage(FALSE,NULL_CHAR,MSG_DETECTED_TEMPERATURE_IS_BELOW_MINIMUM_993));
					SendMessageToQt(&qtDynamicTempScreenInfo, GUI_MSG_DYNAMIC_TEMPERATURE_SCREEN);

					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_SHOW_PARTIAL_APPROACH_SCREEN:
				{
					DYNAMIC_TEMP_SCREEN_INFO_QT_t		qtDynamicTempScreenInfo = deflQtDynamicTempScreenInfo;
					qtDynamicTempScreenInfo.footerHideF = TRUE;
					qtDynamicTempScreenInfo.ImageHideF = FALSE;
					userIdx = DcInfo.userIdx;
					snprintf(qtDynamicTempScreenInfo.user.alphaNumId,sizeof(qtDynamicTempScreenInfo.user.alphaNumId),"%s", GetAlphaNumId(userIdx));
					qtDynamicTempScreenInfo.user.userId = GetUserIdFromIdx(userIdx);
					snprintf(qtDynamicTempScreenInfo.user.name,sizeof(qtDynamicTempScreenInfo.user.name),"%s",GetUserName(userIdx));
					qtDynamicTempScreenInfo.imageIdx = POPUP_IMAGE_PARTIAL_APPROACH;
					SendMessageToQt(&qtDynamicTempScreenInfo, GUI_MSG_DYNAMIC_TEMPERATURE_SCREEN);
					break;
				}

				case USER_RESP_SHOW_FULL_APPROACH_SCREEN:
				{
					DYNAMIC_TEMP_SCREEN_INFO_QT_t		qtDynamicTempScreenInfo = deflQtDynamicTempScreenInfo;
					qtDynamicTempScreenInfo.footerHideF = TRUE;
					qtDynamicTempScreenInfo.ImageHideF = FALSE;
					userIdx = DcInfo.userIdx;
					snprintf(qtDynamicTempScreenInfo.user.alphaNumId,sizeof(qtDynamicTempScreenInfo.user.alphaNumId),"%s", GetAlphaNumId(userIdx));
					qtDynamicTempScreenInfo.user.userId = GetUserIdFromIdx(userIdx);
					snprintf(qtDynamicTempScreenInfo.user.name,sizeof(qtDynamicTempScreenInfo.user.name),"%s",GetUserName(userIdx));
					qtDynamicTempScreenInfo.imageIdx = POPUP_IMAGE_FULL_APPROACH;
					SendMessageToQt(&qtDynamicTempScreenInfo, GUI_MSG_DYNAMIC_TEMPERATURE_SCREEN);
					break;
				}

				case USER_RESP_FACE_DELETE_FAILED:
					strcpy(qtDynamicInfo.titleText, "");
					qtDynamicInfo.imageIdx = POPUP_CREDENTIAL_DELETE_FAIL;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_FAILED_TO_DELETE_1050));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;

				case USER_RESP_FACE_DELETE_PROCESSING:
				{
					strcpy(qtDynamicInfo.titleText, "");
					qtDynamicInfo.imageIdx = POPUP_PROCESSING;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DELETING_CREDENTIALS_1051));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
				}break;
				case USER_RESP_FACE_ENROLL_PROCESSING:
				{
					strcpy(qtDynamicInfo.titleText, "");
					qtDynamicInfo.imageIdx = POPUP_PROCESSING;
					qtDynamicInfo.footerHideF = TRUE;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PROCESSING_467));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
				}
				break;
				
				case USER_RESP_DEVICE_BUSY:
				{
					qtDynamicInfo.imageIdx = POPUP_DEVICE_BUSY;
					qtDynamicInfo.footerHideF = TRUE;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_DEVICE_IS_BUSY_977));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(LCD_IDLE_TIMER);
				}
				break;

#if defined(FR_ADDITIONAL_FEATURE_SUPPORTED)
				case USER_RESP_DENIED_FR_DISABLED_ON_DEVICE:
				{
					qtDynamicInfo.imageIdx = POPUP_FAILED;
					snprintf(qtDynamicInfo.message1, sizeof(qtDynamicInfo.message1), "%s", GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENABLE_FACE_RECOGNITION_FOR_MASK_DETECTION_1039));
					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

					SetLcdIdleTimer(LCD_IDLE_TIMER);
					break;
				}

				case USER_RESP_APPROACH_CAMERA:
				{
					DYNAMIC_USER_INFO_SCREEN_QT_t	qtDynamicUserInfo = dfltQtDynamiUserInfo;
					UINT8 *imageIdxPtr = (UINT8 *)dataPtr;
					snprintf(qtDynamicUserInfo.user.alphaNumId,sizeof(qtDynamicUserInfo.user.alphaNumId),"%s", GetAlphaNumId(DcInfo.userIdx));
					qtDynamicUserInfo.user.userId = GetUserIdFromIdx(DcInfo.userIdx);
					snprintf(qtDynamicUserInfo.user.name,sizeof(qtDynamicUserInfo.user.name),"%s",GetUserName(DcInfo.userIdx));
					if(imageIdxPtr != NULL)
					{
						switch(*imageIdxPtr)
						{
							case PARTIAL_APPROACH:
								qtDynamicUserInfo.imageIdx = IMAGE_PARTIAL_APPROACH;							
								break;
							
							case TOTAL_APPROACH:
								qtDynamicUserInfo.imageIdx = IMAGE_TOTAL_APPROACH;
								break;

							default:
								qtDynamicUserInfo.imageIdx = IMAGE_NONE;
						}
						SendMessageToQt(&qtDynamicUserInfo, GUI_MSG_DYNAMIC_USER_INFO_SCREEN);
					}
					else
					{
						EPRINT(GUI_LOG, "No Data to Send, userResponse: [%d]", userResp);
					}
					break;
				}
#endif

				case USER_RESP_DISPLAY_IDLE_SCREEN_FOR_FR_STREAMING_IDLE_CAD:
				case USER_RESP_DISPLAY_IDLE_SCREEN_FOR_FR_STREAMING_WAIT_CAD:
				{
					DYNAMIC_IDLE_SCREEN_INFO_QT_t	qtDynamicIdleScreenInfo = dfltQtIdleScreenInfo;
					FR_OVERLAY_STREAM_INFO_t* frOverlayStreamInfo = (FR_OVERLAY_STREAM_INFO_t *)dataPtr;

					qtDynamicIdleScreenInfo.showOverlay = frOverlayStreamInfo->showOverlay;
					qtDynamicIdleScreenInfo.showOverlayText = frOverlayStreamInfo->showOverlayText;
					snprintf(qtDynamicIdleScreenInfo.overlayText, sizeof(qtDynamicIdleScreenInfo.overlayText),
							GetDisplayMessage(FALSE,NULL_CHAR, frOverlayStreamInfo->overlayMultilangTextIdx));
					qtDynamicIdleScreenInfo.showUserInfo = frOverlayStreamInfo->showUserInfo;

					/* Fill user info if required */
					if (qtDynamicIdleScreenInfo.showUserInfo == TRUE)
					{
						if(frOverlayStreamInfo->specialFuncActiveF == TRUE)
						{
							snprintf(qtDynamicIdleScreenInfo.name,sizeof(qtDynamicIdleScreenInfo.name),"%s",GetDisplayMessage(FALSE,NULL_CHAR,ActivateSplFnStr[(DcInfo.singleUserSplCode-1)]));
						}
						else
						{
							qtDynamicIdleScreenInfo.userId = GetUserIdFromIdx(DcInfo.userIdx);
							snprintf(qtDynamicIdleScreenInfo.name,sizeof(qtDynamicIdleScreenInfo.name),"%s",GetUserName(DcInfo.userIdx));
						}
					}
					qtDynamicIdleScreenInfo.footerIdx = frOverlayStreamInfo->footerIdx;
					qtDynamicIdleScreenInfo.doNotApplyAutoHideF = frOverlayStreamInfo->doNotApplyAutoHide;
					SendMessageToQt(&qtDynamicIdleScreenInfo, GUI_MSG_DISPLAY_IDLE_SCREEN_FOR_FR_STREAMING);
					break;
				}

				case USER_RESP_FACE_ENROLLED:
				{
					strcpy(qtDynamicInfo.titleText, "");
					qtDynamicInfo.imageIdx = POPUP_ENROLLMENT_SUCCESS;
					strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_ENROLLMENT_SUCCESS_199));

					SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					SetLcdIdleTimer(ENROLLMENT_USER_RESP_TIMER);
					break;
				}
				case USER_RESP_PD_DA_USER_ALLOWED:
				{
					USER_COMMON_QT_t allow = defltQtUsrCommon;
					snprintf(allow.user.alphaNumId, ALPHANUM_ID_SIZE, "%s", GetAlphaNumId(DcInfo.userIdx) );
					

					allow.user.designation[0] = 0;
					allow.user.userId = GetUserIdFromIdx(DcInfo.userIdx);


					strcpy(allow.user.name, GetUserName(DcInfo.userIdx) );
					allow.user.photo = TRUE;


					allow.genInfo.imageIdx = ALLOW_SCREEN_ACCESS_ALLOWED_IMAGE;
					
					SendMessageToQt(&allow, GUI_MSG_USER_ALLOWED_COMMON_SCREEN);
					SetLcdIdleTimerMilliSec(SysBasicPara.accessAllowScreenDuration);
					break;
					 
				}
				case USER_RESP_PD_DA_USER_DENIED:
				{
					USER_COMMON_QT_t deny = defltQtUsrCommon;
					
					if(DcInfo.authReason == AUTH_TIMEOUT)
					{
						qtDynamicInfo.imageIdx = POPUP_TIMEOUT;
						strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE, NULL_CHAR, MSG_TIME_OUT_605));
						SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
					}
					else
					{
						deny.genInfo.imageIdx = DENY_SCREEN_ACCESS_DENIED_IMAGE;	//Image Index
						
						switch(DcInfo.authReason)
						{

							case AUTH_USER_BLOCKED:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE, NULL_CHAR, MSG_USER_IS_BLOCKED_627));
								break;
							}
						
							case AUTH_USER_DISABLED:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE, NULL_CHAR, MSG_USER_IS_NOT_ACTIVE_629));
								break;
							}

							case AUTH_CARD_READ_FAILED:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE, NULL_CHAR, MSG_CARD_READ_FAILED_77));
								break;
							}

							case AUTH_INVALID_ZAM_INPUT:
							case AUTH_INVALID_INPUT:
							{
								strcpy(deny.genInfo.line1, GetInvalidInputMsg());

								if(DcInfo.zamUserIdx == INVALID_USER)
								{
									deny.ifUserInfo = FALSE;
								}
								break;
							}
							
							case AUTH_API_INVALID_INPUT:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE, NULL_CHAR, MSG_ACCESS_DENIED_0));

								if(DcInfo.zamUserIdx  == INVALID_USER)
								{
									deny.ifUserInfo = FALSE;
								}
								break;
							}

							case AUTH_FP_MATCHING_TIMEOUT:
							{
								deny.ifUserInfo = FALSE;
								strcpy(deny.genInfo.line2, GetDisplayMessage(FALSE, NULL_CHAR, MSG_FP_MATCHING_TIME_OUT_757));
								break;
							}

							case AUTH_NO_ACCESS:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE, NULL_CHAR, MSG_ENTRY_RESTRICTED_214));
								break;
							}
							
							case AUTH_VALIDITY_EXPIRED:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE, NULL_CHAR, MSG_VALIDITY_EXPIRED_634));
								break;
							}
							
							case AUTH_EVENT_LOG_FAIL:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE, NULL_CHAR, MSG_SYSTEM_ERROR_597));
								break;
							}
							
 							case AUTH_DENIED_DOOR_NOT_ASSIGNED:
							{
								strcpy(deny.genInfo.line1, GetDisplayMessage(FALSE, NULL_CHAR, MSG_DOOR_NOT_ASSIGNED_938));
 								break;
							}
							
							default :
								break;	//Default for denied response
						}

						if(deny.ifUserInfo == TRUE)
						{
							snprintf(deny.user.alphaNumId, ALPHANUM_ID_SIZE, "%s", GetAlphaNumId(DcInfo.userIdx) );

							deny.user.designation[0] = 0;
							deny.user.userId = GetUserIdFromIdx(DcInfo.userIdx);
							
							strcpy(deny.user.name, GetUserName(DcInfo.userIdx) );
							deny.user.photo = TRUE;
						}
						
						SendMessageToQt(&deny, GUI_MSG_USER_DENIED_COMMON_SCREEN);
					}
					SetLcdIdleTimerMilliSec(SysBasicPara.accessDenyScreenDuration);
					break;
				}			  

				case USER_RESP_PD_DA_PLACE_YOUR_FINGER:
				{
				  	userIdx = DcInfo.userIdx;
					qtFuncStatusInfo.ifUserInfo = TRUE;
					sprintf(qtFuncStatusInfo.user.alphaNumId, "%s", GetAlphaNumId(userIdx));

					qtFuncStatusInfo.user.userId = GetUserIdFromIdx(userIdx);
					
					strcpy(qtFuncStatusInfo.user.name, GetUserName(userIdx));
					qtFuncStatusInfo.user.photo = TRUE;
					qtFuncStatusInfo.genInfo.imageIdx = GEN_INFO_PLACE_FINGER;
					strcpy(qtFuncStatusInfo.genInfo.line1, GetDisplayMessage(FALSE, NULL_CHAR, MSG_PLACE_YOUR_FINGER_440) );
					SendMessageToQt(&qtFuncStatusInfo, GUI_MSG_SPL_FUNC_STATUS);

					SetLcdIdleTimer(SysAdvPara.multiAccessTimer);
					break;
				}
							  
			}// END OF SWITCH- user response
		}// End of case- internal port
		break;
	}
}

//*****************************************************************************
//	SetLcdIdleTimer()
//	Param:
//		IN :	timerValue in seconds
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function loads timer to put lcd into idle state after
//				given time.
//				If display is updated before timeout then timer will be
//				reloaded with new value.
//*****************************************************************************
BOOL SetLcdIdleTimer(UINT16 timerValue)
{
	TIMER_INFO_t tInfo;

	if(timerValue > 0)
	{
		// NOTE: we are adding 100 msec extra so that display idle timeout won't
		// happen earlier than application timers.
		if(DcInfo.lcdIdleTmrHandle == INVALID_TIMER_HANDLE)
		{
			tInfo.count = ((timerValue * 10) + 1);
			tInfo.data  = 0;
			tInfo.funcPtr = &DisplayIdleScreen;

			return(StartTimer(tInfo, &DcInfo.lcdIdleTmrHandle, __FUNCTION__) );
		}
		else
		{
			return (ReloadTimer(DcInfo.lcdIdleTmrHandle, ((timerValue * 10) + 1) ) );
		}
	}
	else
	{
		DeleteTimer(&DcInfo.lcdIdleTmrHandle);
		return SUCCESS;
	}
}

//*****************************************************************************
//	SetLcdIdleTimerMilliSec()
//	Param:
//		IN :	timerValue in Milliseconds
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function loads timer to put lcd into idle state after
//				given time.
//				If display is updated before timeout then timer will be
//				reloaded with new value.
//*****************************************************************************
BOOL SetLcdIdleTimerMilliSec(UINT16 timerValue)
{
	TIMER_INFO_t tInfo;

	if(timerValue > 0)
	{
		// NOTE: we are adding 100 msec extra so that display idle timeout won't
		// happen earlier than application timers.
		if(DcInfo.lcdIdleTmrHandle == INVALID_TIMER_HANDLE)
		{
			tInfo.count = ((timerValue/TIMER_UNIT) + 1);
			tInfo.data  = 0;
			tInfo.funcPtr = &DisplayIdleScreen;

			return(StartTimer(tInfo, &DcInfo.lcdIdleTmrHandle, __FUNCTION__) );
		}
		else
		{
			return (ReloadTimer(DcInfo.lcdIdleTmrHandle, ((timerValue/TIMER_UNIT ) + 1) ) );
		}
	}
	else
	{
		DeleteTimer(&DcInfo.lcdIdleTmrHandle);
		return SUCCESS;
	}
}
//*****************************************************************************
//	DisplayIdleScreen()
//	Param:
//		IN :	dummy - unused
//		OUT:	None
//	Returns:	None
//	Description:
//				This function is callback function for 'SetLcdIdleTimer'
//				It simply resets timer handle as invalid.
//
//*****************************************************************************
void DisplayIdleScreen(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&DcInfo.lcdIdleTmrHandle);

	if(GetDoorApplType() == DIRECT_DOOR_APPL)
	{
		if(DcInfo.delayInActive == TRUE)
		{
		   DcInfo.delayInActive = FALSE;
		   BOOL status = getDeviceInactiveStatus();
		   setDeviceInactiveStatus(status, TRUE);
		}
	}

	if(SysBasicPara.application == CAFETERIA)
	{
		SwitchToCafeteriaState(TRUE);
	}
	else
	{
		SendMessageToQt(&FRStreamDisplayF, GUI_MSG_FR_STREAM_STATUS);
		SendMessageToQt(" ", GUI_MSG_DISPLAY_IDLE_SCREEN);
		// To Display "Waiting For Responses" overlay text message while GroupFR
		if(IsGroupFrApplicable() == YES)
		{
			FR_OVERLAY_STREAM_INFO_t streamOverlayTextInfo = {
															.showOverlay = FALSE,				// True If overlay is required
															.showOverlayText = FALSE,			// True If overlay text is required
															.overlayMultilangTextIdx = MSG_MAX_LANG_STRING, 	// overlay text
															.showUserInfo = FALSE,			// True If user info rectangle is required
															.footerIdx = FOOTER_IDLE,
															.specialFuncActiveF = FALSE,
															};
			streamOverlayTextInfo.showOverlay = GetDisplayOverlayTextFlag();
			streamOverlayTextInfo.overlayMultilangTextIdx = MSG_WAITING_FOR_RESPONSES_1049;
			streamOverlayTextInfo.showOverlayText = streamOverlayTextInfo.showOverlay;

			SendMessageToQt((VOIDPTR)&streamOverlayTextInfo, GUI_MSG_SET_OVERLAY_TEXT_ON_STREAM);
		}
	}

    if(DcInfo.curState == DC_TEMP_ADD_USER_STATE)
    {
        TerminateTempUserEnrollmentActivity(DC_STATE_SWITCH_TO_IDLE);
    }

	// TODO: to replace with function
	if(NO == IsWaitingForAcsBleCredentialInDegrade())
		DegradeState = DGRD_IDLE_STATE;
	DeleteAllDoneIdtNodesForUser(curProcFaceUserIdx);
	curProcFaceUserIdx = UNKNOWN_USER_IDX;
	lastIdleTimeTick = GetSysTick();
	DPRINT(SYS_LOG, "Screen gets idle on [%u]", lastIdleTimeTick);
}
//*****************************************************************************
//	SetLCDDimTimer()
//	Param:
//		IN :	timerValue in seconds
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function loads timer to put lcd into idle state after
//				given time.
//				If display is updated before timeout then timer will be
//				reloaded with new value.
//*****************************************************************************
BOOL SetLCDDimTimer(void)
{
	TIMER_INFO_t tInfo;

    if(DisplayBrightness.sleepTimer > 0)
	{
		if(BrightnessTmrHandle == INVALID_TIMER_HANDLE)
		{
            tInfo.count = ((DisplayBrightness.sleepTimer * 10));
			tInfo.data  = 0;
            tInfo.funcPtr = &SetLCDDimTimerTimeOut;
			SetPWMStatus(DisplayBrightness.setBrightness);
			return(StartTimer(tInfo, &BrightnessTmrHandle, __FUNCTION__) );
		}
		else
		{
			SetPWMStatus(DisplayBrightness.setBrightness);
            return (ReloadTimer(BrightnessTmrHandle, DisplayBrightness.sleepTimer * 10));
		}
	}
	else
	{
        SetLCDDimTimerTimeOut(0);
		return SUCCESS;
	}
}
//*****************************************************************************
//	SetLCDDimTimerTimeOut()
//	Param:
//		IN :	dummy - unused
//		OUT:	None
//	Returns:	None
//	Description:
//				This function is callback function for 'SetLcdIdleTimer'
//				It simply resets timer handle as invalid.
//
//*****************************************************************************
void SetLCDDimTimerTimeOut(UNUSED_PARA UINT32 dummy)
{
    int16_t    LcdBrightnessDimLevel;

    GetRegistryValueInt(MX_LCD_BRIGHTNESS_DIM_LEVEL, PWM_DUTY_LOW, PWM_DUTY_MAX, PWM_DUTY_30, &LcdBrightnessDimLevel);
	DeleteTimer(&BrightnessTmrHandle);
    SetPWMStatus(LcdBrightnessDimLevel);
}
//*****************************************************************************
//	UpdateDisplayStatusField()
//	Param:
//		IN :	dummy - unused
//		OUT:	None
//	Returns:	None
//	Description:
//				This function updates status field to be displayed.
//
//*****************************************************************************
void UpdateDisplayStatusField(void)
{
	STATUS_BAR_INFO_t 	statusField;

	if(GetDoorApplType() == PANEL_DOOR_APPL)
	{
		statusField.wifiStatus = (WifiParameters.enableWifi) ? GetWiFiStatus() : WifiParameters.enableWifi;
		statusField.degradeModeType = BASIC_DEGRADE_MODE_TYPE;
		if(CommState == SLAVE_DEGRADE)
		{
			statusField.degradeModeF = TRUE;
			statusField.nwFaultF = FALSE;
			statusField.degradeModeType = SysBasicPara.degradeModeType;
		}
		else if(CommState == SLAVE_OFFLINE)
		{
			statusField.degradeModeF = FALSE;
			statusField.nwFaultF = TRUE;
		}
		else
		{
			statusField.degradeModeF = FALSE;
			statusField.nwFaultF = FALSE;
		}

		statusField.ethStatus = GetEthernetStatus();
		statusField.bluetoothStatus = GetBluetoothStatus();
		if(CommState == SLAVE_ONLINE)
		{
			statusField.doorLockStatus = CurrStatusbarData.doorLockStatus;
			statusField.alarmStatus = CurrStatusbarData.alarmStatus;
			statusField.lateInEarlyOutStatus = CurrStatusbarData.lateInEarlyOutStatus;
			statusField.dndF = CurrStatusbarData.dndF;
			statusField.deadManZoneF = CurrStatusbarData.deadManZoneF;
			statusField.downloadingF = CurrStatusbarData.downloadingF;
		}
		else
		{
			statusField.doorLockStatus = 0;
			statusField.alarmStatus = FALSE;
			statusField.lateInEarlyOutStatus = 0;
			statusField.dndF = FALSE;
			statusField.deadManZoneF = FALSE;
			statusField.downloadingF = FALSE;
		}
	}
	else
	{
		statusField.alarmStatus = IsAnyAlarmPresent();
		statusField.doorLockStatus = DcMonitorControl.doorLock;
		statusField.wifiStatus = (WifiParameters.enableWifi) ? GetWiFiStatus() : WifiParameters.enableWifi;
		statusField.ethStatus = GetEthernetStatus();
		statusField.modemStatus = GetModemStatus();
		statusField.usbStatus = UsbInfo.status;
		statusField.bluetoothStatus = GetBluetoothStatus();


		if(IsConnectToPanelEnabled() == YES)
		{
			statusField.serverStatus = GetPanelStatus();
		}
#ifdef	PUSH_API_SUPPORTED
		else if(ServerModeCfg.serverMode == COSEC_THIRD_PARTY)
		{
			statusField.serverStatus = GetThirdParyServerStatus();
		}
#endif
		else
		{
			statusField.serverStatus = GetAcmsStatus();
		}
		statusField.lateInEarlyOutStatus = GetLateInEarlOutStatus();
		statusField.downloadingF = CurrStatusbarData.downloadingF;

		UINT8 activeSpecialFuncIndex = 0, activeSpecialFunCnt = 0;
		activeSpecialFunCnt = GetGoingIN_OUTSpecialFunctionInfo(GUI_BUTTON_GOING_IN,  &activeSpecialFuncIndex);
		if(activeSpecialFunCnt >= 1)
		{
			statusField.goingInEnableF = TRUE;
		}
		else
		{
			statusField.goingInEnableF = FALSE;
		}

		activeSpecialFuncIndex = 0, activeSpecialFunCnt = 0;
		activeSpecialFunCnt = GetGoingIN_OUTSpecialFunctionInfo(GUI_BUTTON_GOING_OUT,  &activeSpecialFuncIndex);
		if(activeSpecialFunCnt >= 1)
		{
			statusField.goingOutEnableF = TRUE;
		}
		else
		{
			statusField.goingOutEnableF = FALSE;
		}
	}
	statusField.temperatureSensorStatusF = tempSensorInfo.tempereatureSensorStatus;

	if (ReadersParaConfig.ambientPara.showAmbonStatus == TRUE)
	{
		statusField.temperature = tempSensorInfo.ambientTemperature;
		statusField.tempUnit = SysAdvPara.temperatureLogCnfg.tempUnit;
	}
	else
	{
		statusField.temperature = 0.0;
		statusField.tempUnit = 0;
	}
	if(CurrStatusbarData.FsmSyncStatus >= FSM_IdleState && CurrStatusbarData.FsmSyncStatus <= FSM_MaxState)
		statusField.FsmSyncStatus = CurrStatusbarData.FsmSyncStatus;
	else
		statusField.FsmSyncStatus = FSM_IdleState;
	SendMessageToQt(&statusField, GUI_MSG_STATUSBAR_SCREEN);
}

//*****************************************************************************
//	SetNwStatusCad()
//	Param:
//		IN :	COMM_PORT_e		communication interface(ethernet, wifi, mobile broadband
//		OUT:	UINT8			ABSENT, PRESENT or INVALID_LED_STATUS status
//	Returns:	NONE
//	Description:
//				This function updates network status cadance on LED.
//
//*****************************************************************************
void SetNwStatusCad(COMM_PORT_e interface, UINT8 status)
{
	static UINT8	interfaceStatus[MAX_PORT] = {ABSENT, ABSENT, ABSENT};

	if(IsConnectToPanelEnabled() == NO)
	{
		if(status != INVALID_LED_STATUS)
		{
			interfaceStatus[interface] = status;
		}
		if(((ServerModeCfg.serverMode == COSEC_CENTRA) && (acmsCurActiveInterface == interface)) ||
				((ServerModeCfg.serverMode == COSEC_VYOM) && (acmsCurActiveInterface == interface)))
		{
			if(interfaceStatus[interface] == ABSENT)
			{
				SetLed(ACCESS_OFFLINE_RESP, INT_EXT_PORT);
			}
			// Interface is available now, set status as ONLINE
			// if server connection timeout is not yet observered.
			else if(GetAcmsStatus() == ACMS_ONLINE)
			{
				SetLed(ACCESS_ONLINE_RESP, INT_EXT_PORT);
				if(TRUE == getDeviceInactiveStatus())
				{
				      if( TRUE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER1) )
				      {
					      SetLed(ACCESS_INACTIVE_DEVICE_RESP, INTERNAL_PORT);
				      }

				      if( TRUE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER3) )
				      {
					      SetAudioVisual(USER_RESP_DEVICE_INACTIVE, EXTERNAL_PORT, (VOIDPTR)" ");
				      }

				}
			}
			else
			{
				SetLed(ACCESS_DEGRADE_RESP, INT_EXT_PORT);
			}
		}
#ifdef 	PUSH_API_SUPPORTED
		else if((ServerModeCfg.serverMode == COSEC_THIRD_PARTY) && (acmsCurActiveInterface == interface))
		{
			if(interfaceStatus[interface] == ABSENT)
			{
				SetLed(ACCESS_OFFLINE_RESP, INT_EXT_PORT);
			}
			else if(GetThirdParyServerStatus() == ACMS_ONLINE)
			{
				SetLed(ACCESS_ONLINE_RESP, INT_EXT_PORT);
			}
			else
			{
				SetLed(ACCESS_DEGRADE_RESP, INT_EXT_PORT);
			}
		}
#endif
	}
	else
	{
		if(status != INVALID_LED_STATUS)
		{
			interfaceStatus[interface] = status;
		}
		if(((ServerModeCfg.serverMode == COSEC_CENTRA) && (ConnectToPanelPara.panelNwInterface == interface)) ||
				((ServerModeCfg.serverMode == COSEC_VYOM) && (ConnectToPanelPara.panelNwInterface == interface)))
		{
			if(interfaceStatus[interface] == ABSENT)
			{
				SetLed(ACCESS_OFFLINE_RESP, INT_EXT_PORT);
			}
			// Interface is available now, set status as ONLINE
			// if server connection timeout is not yet observered.
			else if(GetPanelStatus() == PANEL_COMM_ONLINE)
			{
				SetLed(ACCESS_ONLINE_RESP, INT_EXT_PORT);
				if(TRUE == getDeviceInactiveStatus())
				{
				      if( TRUE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER1) )
				      {
					      SetLed(ACCESS_INACTIVE_DEVICE_RESP, INTERNAL_PORT);
				      }

				      if( TRUE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER3) )
				      {
					      SetAudioVisual(USER_RESP_DEVICE_INACTIVE, EXTERNAL_PORT, (VOIDPTR)" ");
				      }

				}
			}
			else
			{
				SetLed(ACCESS_DEGRADE_RESP, INT_EXT_PORT);
			}
		}
	}
}

//*****************************************************************************
//	SendBuProgAvResp()
//	Param:
//		IN : buType - backup & update type
//			 prog - progress (1 to 100)
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL
//	Description:
//		It generates user response for showing progress of the current activity.
//*****************************************************************************
void SendBuProgAvResp(BACKUP_UPDATE_TYPE_e buType, UINT8 percentage)
{
	USER_RESP_BU_STATUS_t	userResp;

	userResp.buType = buType;
	userResp.prog = percentage;
	SetAudioVisual(USER_RESP_BACKUP_UPDATE_PROGRESS, INTERNAL_PORT, &userResp);
}

//*****************************************************************************
//	SetTemperatureIdleTimer()
//	Param:
//		IN :	timerValue in seconds
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function loads timer to put lcd into temperature menu state after
//				given time.
//*****************************************************************************
BOOL SetTemperatureIdleTimer(UINT16 timerValue)
{
	TIMER_INFO_t tInfo;

	if(timerValue > 0)
	{
		if(temperatureTmrHandle == INVALID_TIMER_HANDLE)
		{
			tInfo.count = ((timerValue * 10));
			tInfo.data  = 0;
			tInfo.funcPtr = &SetTemperatureTimerTimeOut;
			return(StartTimer(tInfo, &temperatureTmrHandle, __FUNCTION__) );
		}
		else
		{
			return (ReloadTimer(temperatureTmrHandle, timerValue * 10));
		}
	}
	else
	{
		DeleteTimer(&temperatureTmrHandle);
		return SUCCESS;
	}
}
//*****************************************************************************
//	SetTemperatureTimerTimeOut()
//	Param:
//		IN :	dummy - unused
//		OUT:	None
//	Returns:	None
//	Description:
//				This function is callback function for 'SetTemperatureIdleTimer'
//				It simply resets timer handle as invalid and displays temperature menu screen.
//
//*****************************************************************************
void SetTemperatureTimerTimeOut(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&temperatureTmrHandle);

	if (tempSensorInfo.tempereatureSensorStatus == PRESENT)
	{
		DisplayIdleScreen(0);
		SwitchDcToIdleState();
	}
	/* Come back to menu temperature screen if sensor is disconnected */
	else
	{
	/* Display Menu temperature screen */
	DisplayMenuScreen(MENU_TEMPERATURE_SCREEN);
	}
}

//*****************************************************************************
//	SetUserAllowEntryExitModeFlag()
//	Param:
//		IN :	dummy - unused
//		OUT:	None
//	Returns:	None
//	Description:
//
//*****************************************************************************
VOID SetUserAllowEntryExitModeFlag(BOOL entryExitF)
{
	UserAllowEntryExitF = entryExitF;
}


//*****************************************************************************
//	GetUserAllowEntryExitModeFlag()
//	Param:
//		IN :	dummy - unused
//		OUT:	None
//	Returns:	None
//	Description:
//
//*****************************************************************************
BOOL GetUserAllowEntryExitModeFlag(VOID)
{
	return UserAllowEntryExitF;
}
//*****************************************************************************
//	GetInvalidInputMsg()
//	Param:
//		IN : NONE
//		OUT: NONE.
//	Returns:
//			 pointer to the message string.
//	Description:
//			 This function wiil return invalid input string based on credential
// 			 related information set.
//*****************************************************************************
static CHARPTR GetInvalidInputMsg(void)
{
	if(DcInfo.credInvalidF == TRUE)
	{
		return (GetDisplayMessage(FALSE,NULL_CHAR,invalidInputStr[INVALID_CREDENTIAL]));
	}
	else
	{
		return (GetDisplayMessage(FALSE,NULL_CHAR,invalidInputStr[DcInfo.credType[DcInfo.credCnt-1]]));
	}
}

//*****************************************************************************
//	StatusBarExpanded()
//	Param:
//		IN : Status
//		OUT: NONE
//	Returns:
//	         NONE
//	Description:
//		This function starts or deletes the statusBar timer.
//*****************************************************************************
BOOL StatusBarExpanded(BOOL Status)
{
    CurrStatusbarData.statusBarExpandedF = Status;

    if( GetDoorApplType() == DIRECT_DOOR_APPL )
    {
        if((CurrStatusbarData.statusBarExpandedF == TRUE) && (statusBarTmrHandle == INVALID_TIMER_HANDLE))
        {
            TIMER_INFO_t statusBarTime;

            statusBarTime.count = MENU_TIMER * 10;
            statusBarTime.funcPtr = &StatusBarExpandedTimeout;
            statusBarTime.data = 0;

            if(StartTimer(statusBarTime, &statusBarTmrHandle, __FUNCTION__) == FAIL)
            {
                EPRINT(DC_LOG,"Unable to start Interdigit Timer");
            }
        }
        else if(statusBarTmrHandle != INVALID_TIMER_HANDLE)
        {
            DeleteTimer(&statusBarTmrHandle);
        }
    }
}

//*****************************************************************************
//	StatusBarExpandedTimeout()
//	Param:
//		IN : dummy
//		OUT: None
//	Returns:
//	         NONE
//	Description:
//		This is callback function for StatusBarExpanded.
//		This function sends command for audio visual indication for timeout.
//*****************************************************************************
void StatusBarExpandedTimeout(UNUSED_PARA UINT32 dummy)
{
    DeleteTimer(&statusBarTmrHandle);
    SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
    return;
}

void ShowEntryRestrictedForDeviceInactive(void)
{
    SwitchDcToIdleState();
    SetAudioVisual(USER_RESP_ENTRY_RESTRICTED_DEVICE_INACTIVE,DcInfo.respPort, NULL);
}
