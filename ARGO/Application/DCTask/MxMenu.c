/////////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Telecom Pvt. Ltd., Baroda, India.
//   Project      : Access control System
//   Created By   : Mukesh Savaliya
//   File         : MxMenu.c
//   Description  : This file handles all activity related to device menu.
//					It displays menu options as per category and performs
//					navigation, validation and actions as per selected menu
//					options.
/////////////////////////////////////////////////////////////////////////////

//*** Place all include files here ***
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "MxTypedef.h"
#include "MxMenu.h"
#include "MxConfig.h"
#include "MxConfigFileIo.h"
#include "MxDcSpecialFunc.h"
#include "MxAvResp.h"
#include "MxDcTask.h"
#include "MxSysTimer.h"
#include "MxGuiHandler.h"
#include "MxWifiPort.h"
#include "MxMobileBroadband.h"
#include "MxEthernetPort.h"
#include "MxSysUtils.h"
#include "MxBackupUpdate.h"
#include "MxAcmsTask.h"
#include "MxCafeteria.h"
#include "MxPrinter.h"
#include "MxCafeteria.h"
#include "MxSysTime.h"
#include "MxNetworkUtils.h"
#include "MxPDDcTask.h"
#include "MxCommTask.h"
#include "MxLogs.h"
#include "MxSysCommonUtils.h"
#include "MxWebJvsWifiSetting.h"
#include "MxPushApiConfig.h"
#include "MxThirdPartyServerTask.h"
#include "MxTemperatureSensor.h"
#include "MxCommonConfig.h"
#include "MxTempUserTask.h"

//******** Extern Variables **********
extern DOOR_ACTIVITY_INFO_t		DcInfo;
extern SYS_ADVANCE_PARA_t		SysAdvPara;
extern SYS_BASIC_PARA_t			SysBasicPara;
extern ETHERNET_PARA_t			EthernetParameters;
extern SERVER_PARA_t			ServerParameters;
extern MODEM_CONFIG_t			ModemProfile[];
extern WIFI_PARA_t				WifiParameters;
extern SYS_VAR_t				SysVar;
extern const CHARPTR			wifiNwSecureStr[2];
extern BOOL						WifiApParaChngF;
extern BOOL						WifiNwParaChngF;
extern BOOL						WifiWpsTrigF;
extern UINT8					SelectedModemProfile;
extern THREAD_STATUS_e			WifiConfFileWriteStatus;
extern PASSWORD_t				DfltPassword;
extern COMM_STATE_e				CommState;
extern MENU_SCREEN_e			PDCurrentMenuScreen;
extern PASSWORD_t			  	Password;
extern BOOL 					SamayModeEnable ;
extern SERVER_MODE_CONFIG_t		ServerModeCfg;
extern pthread_mutex_t 			MutexLockAutoSwitchWiFi;
extern BOOL						ManualWifiConnectF;
extern BRIGHTNESS_DETAIL_t		DisplayBrightness;
extern PUSH_API_t				PushAPIConfig;
extern TEMPERATURE_SENSOR_INFO_t tempSensorInfo;

//****** Defines and Data Types ******
#define MAX_MENU_WIFI_NW		30

typedef enum
{
	MENU_HRD_STATE = 0,
	MENU_ADMIN_STATE,
	MENU_E_CANTEEN_STATE,
	MENU_ABOUT_DEVICE_STATE,
}MENU_STATE_e;

// Stores current menu information
typedef struct
{
	TIMER_HANDLE		menuTmrHandle;		// How Long to stay in menu state
	MENU_STATE_e		state;				// Menu states: Admin, Hrd
	pthread_t 			wifiThread;			// thread (running to search wifi servers) data
	BOOL				wifiThreadStatus;	// thread (running to search wifi servers) status
	MENU_SCREEN_e		currMenuScreen;		// current menu screen
	BOOL 				displayBackBtn;		//Required for Back Navigation
}MENU_INFO_t;

//******** Function Prototypes *******
static void ExitMenuState(void);
static VOIDPTR InitWifiScan(VOIDPTR arg);
static BOOL ValidateMenuLogin(LOGIN_PASSWORD_t * pswd);
static void BackMenuScreen(void);
static VOIDPTR WaitForWpsPin(VOIDPTR arg);

//******** Global Variables **********
pthread_t 				wpsPinWaitThread;
BOOL					wpsPinWaitThreadStatus = INACTIVE;

//******** Static Variables **********
static MENU_INFO_t		menuInfo;
static WIFI_NW_INFO_t	wifiInfo[MAX_WIFI_SEARCH_DEVICES];

//******** Function Definations ******

//*****************************************************************************
//	InitMenuActivity()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function Initialises Menu state Related Parametes.
//*****************************************************************************
void InitMenuActivity(void)
{
	menuInfo.wifiThreadStatus = INACTIVE;
	menuInfo.menuTmrHandle = INVALID_TIMER_HANDLE;
}

//*****************************************************************************
//	SwitchDcToMenuState()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function switches DCTask FSM to Menu state and
//				initialises its parameter.
//*****************************************************************************
void SwitchDcToMenuState(void)
{
	// Start Menu Wait Timer
	if(LoadMenuWaitTimer(MENU_TIMER, &MenuTimeout, MENU_SCREEN_NONE) == SUCCESS)
	{
		CleanUpTempUserFP();
		DcInfo.curState = DC_MENU_STATE;
		// Set reader mode and AV response port
		DcInfo.readerMode = GetReaderEntryExitMode(READER1);
		DcInfo.respPort = INTERNAL_PORT;
	}
	else
	{
		EPRINT(DC_LOG,"Unable to start Menu Wait Timer");
	}
}

//*****************************************************************************
//	LoadMenuWaitTimer()
//	Param:
//		IN :	timerValue	- in seconds
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//	Description:
//				It loads menu timer.
//*****************************************************************************
BOOL LoadMenuWaitTimer(UINT8 timerValue, VOIDPTR funcPtr, MENU_SCREEN_e menuScreen)
{
	TIMER_INFO_t tInfo;

	// Reload timer only if given menu screen is current and menu timer already
	// running
	if( (menuScreen == MENU_CURRENT_SCREEN) &&
					(menuInfo.menuTmrHandle != INVALID_TIMER_HANDLE) )
	{
		return (ReloadTimer(menuInfo.menuTmrHandle, (timerValue * 10) ) );
	}
	else
	{
        DPRINT(DC_LOG,"Starting Timer to Switch DC state to Menu");
		// NOTE: Must delete timer first because call back function may be different
		DeleteTimer(&menuInfo.menuTmrHandle);
        DeleteAllPendingIdtNodes();
        DeleteAllDoneIdtNodes();

		tInfo.count = (timerValue * 10);	// Timer is @100mSec.
		tInfo.data  = menuScreen;
		tInfo.funcPtr = funcPtr;
		return (StartTimer(tInfo, &menuInfo.menuTmrHandle, __FUNCTION__));
	}
}

//*****************************************************************************
//	MenuTimeout()
//	Param:
//		IN :	dummy
//		OUT:	None
//	Returns:	None
//	Description:
//				It terminates menu activity and switches dc task fsm into IDLE
//				state.
//*****************************************************************************
void MenuTimeout(UNUSED_PARA UINT32 dummy)
{
	//SetAudioVisual(USER_RESP_TIMEOUT, INTERNAL_PORT, NULL);
	SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
	ExitMenuState();
}

//*****************************************************************************
//	SearchWirelessNw()
//		IN :	None
//	Returns:	None
//	Description:
//				This function will search wireless networks in a thread, and will
//				send the availble network info to QT.
//*****************************************************************************
void StartWaitForWpsPinThread(void)
{
	if( (wpsPinWaitThreadStatus == THREAD_INACTIVE) || (wpsPinWaitThreadStatus == THREAD_DONE) )
	{
		if(pthread_create(&wpsPinWaitThread, NULL, WaitForWpsPin,
													NULL) == 0)
		{
			// In case thread is completed just before call reaches here (which is observed),
			// do not change thread status to active
			if(wpsPinWaitThreadStatus == THREAD_INACTIVE)
			{
				wpsPinWaitThreadStatus = THREAD_ACTIVE;
			}
			pthread_detach(wpsPinWaitThread);
		}
		else
		{
			// TODO: what to do now??
			EPRINT(DC_LOG, "Error: Can't create thread for waiting wps pin");
		}
	}
}

//*****************************************************************************
//	WaitForWpsPin()
//		IN :	arg	-	not used
//	Returns:	None
//	Description:
//				This function will wait till the wps pin is generated.
//*****************************************************************************
static VOIDPTR WaitForWpsPin(UNUSED_PARA VOIDPTR arg)
{
	WPS_PROGRESS_t	wpsProgData;

	while(1)
	{
		if(WifiConfFileWriteStatus == THREAD_DONE)
		{
			WifiConfFileWriteStatus = THREAD_INACTIVE;
			break;
		}
		DelayMiliSecs(100);
	}

	wpsProgData.wpsTrigerTyp = WifiParameters.wpsTrigerTyp;
	sprintf(wpsProgData.wpsPin, "%s", WifiParameters.wpsPin);
	SendMessageToQt(&wpsProgData, GUI_MSG_WPS_PROGRESS_SCREEN);
	//Update Current Menu screen
	if( (GetDoorApplType() == PANEL_DOOR_APPL) && (CommState == SLAVE_ONLINE) )
	{
		printf("MSG_WAIT_FOR_WPS_PIN to Panel\n");
		SendMsg(MSG_WAIT_FOR_WPS_PIN, NULL, 0);
	}
	else
	{
		if(GetDoorApplType() == PANEL_DOOR_APPL)
			PDCurrentMenuScreen = MENU_WIFI_WPS_WAIT_SCREEN;
		else
			menuInfo.currMenuScreen = MENU_WIFI_WPS_WAIT_SCREEN;
		LoadMenuWaitTimer(WIFI_WPS_TRIGGER_TIMER, &MenuTimeout, MENU_SCREEN_NONE);
		SetLcdIdleTimer(WIFI_WPS_TRIGGER_TIMER);
	}

	wpsPinWaitThreadStatus = THREAD_DONE;
	pthread_exit(&wpsPinWaitThread);
}
//*****************************************************************************
//	SearchWirelessNw()
//		IN :	None
//	Returns:	None
//	Description:
//				This function will search wireless networks in a thread, and will
//				send the availble network info to QT.
//*****************************************************************************
void SearchWirelessNw(void)
{
	char message[] = "Command For WIFI Search";

	if( (menuInfo.wifiThreadStatus == THREAD_INACTIVE) || (menuInfo.wifiThreadStatus == THREAD_DONE) )
	{
		if(pthread_create(&menuInfo.wifiThread, NULL, InitWifiScan, (VOIDPTR)&message) == 0)
		{
			// In case thread is completed just before call reaches here (which is observed),
			// do not change thread status to active
			if(menuInfo.wifiThreadStatus == THREAD_INACTIVE)
			{
				menuInfo.wifiThreadStatus = THREAD_ACTIVE;
			}
			pthread_detach(menuInfo.wifiThread);
		}
		else
		{
			// TODO: what to do now??
			EPRINT(DC_LOG, "Error: Can't create thread to search wifi networks");
		}
	}
}

//*****************************************************************************
//	InitWifiScan()
//		IN :	arg	-	not used
//	Returns:	None
//	Description:
//				This function will search Wireless Networks . All the Available
//				Networks Information Will be Appended with the Formatted Message.
//				And Final Message Will be sent to the Qt Application.
//*****************************************************************************
static VOIDPTR InitWifiScan(UNUSED_PARA VOIDPTR arg)
{
	UINT8 			totalWiFiNw = 0, wifiIdx, idx, currIdx;
	BOOL           	sentIdx[MAX_MENU_WIFI_NW];
	CHAR 			msgBuf[2048];
	UINT16			length = 0;

	memset( (VOIDPTR)sentIdx, 0, MAX_MENU_WIFI_NW);
	totalWiFiNw = SearchWifiDevices(wifiInfo);
	if(totalWiFiNw > MAX_MENU_WIFI_NW)
	{
		totalWiFiNw = MAX_MENU_WIFI_NW;
	}
	// If any network found
	if(totalWiFiNw > 0)
	{
		// Append no. of available networks as first byte
		sprintf(msgBuf , "%d%c", totalWiFiNw, UI_MSG_FSP);
		length = strlen(msgBuf);

		// Append wifi network parameters for each wifi n/w found
		for(wifiIdx = 0; wifiIdx <= totalWiFiNw; wifiIdx++)
		{
			currIdx = MAX_MENU_WIFI_NW;
			for(idx = 0; idx < totalWiFiNw; idx++)
			{
				if(wifiIdx == 0)
				{
					//In First search finds for connected AccessPoints
					if(wifiInfo[idx].connStatF == TRUE)
					{
						currIdx = idx;
					}
				}
				else
				{
					//If it is not sent
					if(sentIdx[idx] == NO)
					{
						//For the first time currIdx is idx
						if(currIdx == MAX_MENU_WIFI_NW)
						{
							currIdx = idx;
						}
						else
						{
							//Search for highest strength in remaining network profiles
							if(wifiInfo[idx].sigQuality >= wifiInfo[currIdx].sigQuality)
							{
								currIdx = idx;
							}
						}
					}
				}
			}
			// appends data in msgbuff
			if(currIdx != MAX_MENU_WIFI_NW )
			{
                CHAR	tmpStr[SYSTEM_CMD_BUF_SIZE];
                CHAR    cmd1[SYSTEM_CMD_BUF_SIZE] = "";
                CHAR    tmpwifirespBuffStrssid[SYSTEM_CMD_BUF_SIZE];


                PrepStringData (wifiInfo[currIdx].ssid ,tmpStr);
                if(snprintf(cmd1,SYSTEM_CMD_BUF_SIZE,"echo -en \"%s\"",tmpStr) < 0)
                {
                    EPRINT(SYS_LOG, "snprintf failed or buffer overflow occured");
                 }
                ExeCmdWithStrOutputResp(cmd1, tmpwifirespBuffStrssid, SYSTEM_CMD_BUF_SIZE);
//				%s %c %d %c %d %c %d %c %d %c %d %c %d %c %s  %c %c
//				(32 +1 +1 +1	+1 +1 +1 +1 +1 +1 +1 +1 +1 +1 +17 +1 +1 = 64)*(MAX_MENU_WIFI_NW = 30)=1920
				sprintf(&msgBuf[length],"%s%c%d%c%d%c%d%c%d%c%d%c%d%c%s%c%d%c%c",
                            tmpwifirespBuffStrssid, DESH_FSP,
							wifiInfo[currIdx].nwSecureF, DESH_FSP,
							wifiInfo[currIdx].sigQuality, DESH_FSP,
							wifiInfo[currIdx].connStatF, DESH_FSP,
							wifiInfo[currIdx].secType, DESH_FSP,
							wifiInfo[currIdx].keyMgmt, DESH_FSP,
							wifiInfo[currIdx].wpsEnabledF, DESH_FSP,
							wifiInfo[currIdx].bssid, DESH_FSP,
							wifiInfo[currIdx].bandFreqType,DESH_FSP,UI_MSG_FSP);
				length = strlen(msgBuf);
				sentIdx[currIdx] = YES;
			}
		}

		SendMessageToQt(&msgBuf, GUI_MSG_SEARCH_WIFI_SCREEN);
		if(GetDoorApplType() == PANEL_DOOR_APPL && CommState == SLAVE_ONLINE)
		{
			// If panel door and door is ONLINE then timer will be loaded in panel
			SendMsg(MENU_WIFI_AP_SEARCH_RESP, (VOIDPTR)&totalWiFiNw, sizeof(totalWiFiNw));
		}
		else
		{
			LoadMenuWaitTimer(MENU_TIMER, &MenuTimeout, MENU_SCREEN_NONE);
		}
	}
	else
	{
		SetAudioVisual(USER_RESP_WIFI_AP_NOT_FOUND, INTERNAL_PORT, NULL);
		if(GetDoorApplType() == PANEL_DOOR_APPL && CommState == SLAVE_ONLINE)
		{
			// If panel door and door is ONLINE then timer will be loaded in panel
			SendMsg(MENU_WIFI_AP_SEARCH_RESP, (VOIDPTR)&totalWiFiNw, sizeof(totalWiFiNw));
		}
		else
		{
			LoadMenuWaitTimer(MENU_USER_RESP_TIMER, &DisplayMenuScreen,
													MENU_WIFI_SETTINGS_SCREEN);
		}
	}

	menuInfo.wifiThreadStatus = THREAD_DONE;
	pthread_exit(&menuInfo.wifiThread);
}

//*****************************************************************************
//	DcMenuStateFn()
//		IN :	msgId 	- msgId to be processed
//				dataPtr	- points to message information
//				length 	- length of message
//	Returns:	None
//	Description:
//				This function processes DC related events in DC MENU state.
//*****************************************************************************
void DcMenuStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	UINT8 					errorIdx = 0;
	BOOL 					validationF = SUCCESS;
	BOOL					serverParaChangeF = FALSE;
	SPECIAL_FUNCTIONS_e		splFunc = MAX_SPECIAL_FUNC;
	DYNAMIC_VALIDATION_RESPONSE_t 	qtDynamicValidationRespInfo;

	switch(msgId)
	{
	default:
		DcDefaultMsgHandler(msgId, dataPtr, length);
		break;

	case GUI_BUTTON_CANCEL:
	case GUI_BUTTON_HOME:
		// User response must be set idle to overcome incomplete process' response
		SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
		ExitMenuState();
		break;
		
	case GUI_BUTTON_BACK:
	{
		/* Reset temperature requested flag */
		tempSensorInfo.temperatureReqested = FALSE;

		BackMenuScreen();
	}
	break;

	case GUI_ALNUM_KEEP_ALIVE:
	case GUI_BUTTON_KEEP_ALIVE:
	case GUI_BUTTON_FIELD_CLEAR:
		// Just reload menu timer for current screen being displayed
		SetLcdIdleTimer(MENU_TIMER);
		LoadMenuWaitTimer(MENU_TIMER, &MenuTimeout, MENU_CURRENT_SCREEN);
		break;

	case GUI_BUTTON_DISPALY_CFG:
		DisplayMenuScreen(MENU_DISPLAY_OPTION_SCREEN);
		break;

	case GUI_BUTTON_BACKGROUND_CFG:
		DisplayMenuScreen(MENU_BACKGROUND_SELECTION_SCREEN);
		break;

	case GUI_BUTTON_CLOCK_CFG:
		DisplayMenuScreen(MENU_CLOCK_SELECTION_SCREEN);
		break;

	case GUI_BUTTON_KEYPAD_CFG:
		DisplayMenuScreen(MENU_KEYPAD_SELECTION_SCREEN);
		break;

	case GUI_BUTTON_BRIGHTNESS_CFG:
		DisplayMenuScreen(MENU_BRIGHTNESS_SETTING_SCREEN);
		break;

	case GUI_BUTTON_SYSTEM_STATUS:
		menuInfo.state = MENU_ABOUT_DEVICE_STATE;
		if(strlen(Password.displayPswd[USER_ADMIN]) > 0)
		{
			DisplayMenuScreen(MENU_LOGIN_SCREEN);
		}
		else
		{
			DisplayMenuScreen(MENU_CHANGE_PSWD_SCREEN);
		}
		break;

	case GUI_BUTTON_ABOUT_DEVICE_CONNECTIVITY:
		DisplayMenuScreen(MENU_ABOUT_DEVICE_CONNECTIVITY);
		break;

	case GUI_BUTTON_ABOUT_DEVICE_READERS_INTERFACES:
		DisplayMenuScreen(MENU_ABOUT_DEVICE_READERS_INTERFACES);
		break;

	case GUI_BUTTON_ABOUT_DEVICE_ALARMS:
		DisplayMenuScreen(MENU_ABOUT_DEVICE_ALARMS);
		break;

	case GUI_BUTTON_READ_CFG:
		DeleteTimer(&menuInfo.menuTmrHandle);
		SetBackupUpdateState(UPDATE_CFG_STATE);
		return;

	case GUI_BUTTON_WRITE_EVENTS:
		DeleteTimer(&menuInfo.menuTmrHandle);
		SetBackupUpdateState(BACKUP_EV_STATE);
		break;

	case GUI_BUTTON_FW_UPGRADE:
		DeleteTimer(&menuInfo.menuTmrHandle);
		SetBackupUpdateState(UPDATE_FW_STATE);
		break;

	case GUI_BUTTON_PASSWORD_MENU:
		DisplayMenuScreen(MENU_PASSWORD_SCREEN);
		break;

	case GUI_BUTTON_CHANGE_PW:
		DisplayMenuScreen(MENU_CHANGE_PSWD_SCREEN);
		break;

	case GUI_BUTTON_SET_DATE_TIME:
		DisplayMenuScreen(MENU_DATE_TIME_SETTINGS_SCREEN);
		break;

	case GUI_BUTTON_ADMIN_MENU:
		SetAudioVisual(USER_RESP_ENTER_PASSWD, INTERNAL_PORT, NULL);
		menuInfo.state = MENU_ADMIN_STATE;
		if(strlen(Password.displayPswd[USER_ADMIN]) > 0)
		{
			DisplayMenuScreen(MENU_LOGIN_SCREEN);
		}
		else
		{
			DisplayMenuScreen(MENU_CHANGE_PSWD_SCREEN);
		}
		break;

	case GUI_BUTTON_ADMIN_MENU_SETTING:
		DisplayMenuScreen(MENU_ADMIN_SETTING_SCREEN);
		break;

	case GUI_BUTTON_ADMIN_MENU_DOOR_CONTROL:
		DisplayMenuScreen(MENU_ADMIN_DOOR_CONTROL_SCREEN);
		break;

	case GUI_BUTTON_ADMIN_MENU_BACKUP_AND_UPDATE:
		DisplayMenuScreen(MENU_ADMIN_BACKUP_AND_UPDATE_SCREEN);
		break;

	case GUI_BUTTON_ENROLL_MENU:
		DisplayMenuScreen(MENU_ENROLLMENT_SCREEN);
		break;

	case GUI_BUTTON_HRD_MENU:
		SetAudioVisual(USER_RESP_ENTER_PASSWD, INTERNAL_PORT, NULL);
		menuInfo.displayBackBtn = FALSE; //if direct hrd navigation occurs
		menuInfo.state = MENU_HRD_STATE;
		if(strlen(Password.displayPswd[USER_HRD]) > 0)
		{
			DisplayMenuScreen(MENU_LOGIN_SCREEN);
		}
		else
		{
			DisplayMenuScreen(MENU_CHANGE_PSWD_SCREEN);
		}
		break;

	case GUI_BUTTON_E_CANTEEN_MENU:
		DisplayMenuScreen(MENU_E_CANTEEN_SCREEN);
		break;

	case GUI_BUTTON_MANAGE_CANTEEN_MENU:
		SetAudioVisual(USER_RESP_ENTER_PASSWD, INTERNAL_PORT, NULL);
		menuInfo.state = MENU_E_CANTEEN_STATE;
		DisplayMenuScreen(MENU_LOGIN_SCREEN);
		break;

	case GUI_BUTTON_VIEW_BALANCE_MENU:
		ReadCafeBalance();
		break;

	case GUI_BUTTON_PRINT_LAST_BILL_MENU:
		SetAudioVisual(USER_RESP_PRINT_CMD_SENT, INTERNAL_PORT, NULL);
		PrintReceipt(PRINT_DUPLICATE_BILL);
		break;

	case GUI_BUTTON_SEARCH_WIFINW:
		SetAudioVisual(USER_RESP_SEARCHING_WIFI, INTERNAL_PORT, NULL);
		LoadMenuWaitTimer(MENU_TIMER, &MenuTimeout, MENU_SCREEN_NONE);
		SearchWirelessNw();
		break;

	case GUI_BUTTON_WPS_PUSH_BUTTON:
	{
		WPS_PROGRESS_t			wpsProgData;

		pthread_mutex_lock(&MutexLockAutoSwitchWiFi);
		ManualWifiConnectF = TRUE;

		WifiParameters.wpsTrigerTyp = WPS_TRIGER_PUSH_BUTTON;

		if(WriteWifiParaConfig() == SUCCESS)
		{
			WifiWpsTrigF = TRUE;
			ChangeNetworkInterfacePara(WIFI_PARA);
			DPRINT(WEBJVS_LOG,"Wifi Parameter Config set successfully");
		}

		pthread_mutex_unlock(&MutexLockAutoSwitchWiFi);

		wpsProgData.wpsTrigerTyp = WifiParameters.wpsTrigerTyp;
		SendMessageToQt(&wpsProgData, GUI_MSG_WPS_PROGRESS_SCREEN);
		//Update Current Menu screen
		menuInfo.currMenuScreen = MENU_WIFI_WPS_WAIT_SCREEN;
		LoadMenuWaitTimer(WIFI_WPS_TRIGGER_TIMER, &MenuTimeout, MENU_SCREEN_NONE);
		SetLcdIdleTimer(WIFI_WPS_TRIGGER_TIMER);
		break;
	}
	case GUI_BUTTON_WPS_PIN_ENTRY:
	{
		pthread_mutex_lock(&MutexLockAutoSwitchWiFi);
		ManualWifiConnectF = TRUE;

		strcpy(WifiParameters.wpsPin, "");
		WifiParameters.wpsTrigerTyp = WPS_TRIGER_PIN_FROM_DEVICE;

		if(WriteWifiParaConfig() == SUCCESS)
		{
			WifiWpsTrigF = TRUE;
			ChangeNetworkInterfacePara(WIFI_PARA);
			DPRINT(WEBJVS_LOG,"Wifi Parameter Config set successfully");
		}

		pthread_mutex_unlock(&MutexLockAutoSwitchWiFi);

		SetAudioVisual(USER_RESP_WPS_PROCESSING, INTERNAL_PORT, NULL);
		LoadMenuWaitTimer(MENU_TIMER, &MenuTimeout, MENU_SCREEN_NONE);
		StartWaitForWpsPinThread();
		break;
	}
	case GUI_BUTTON_SHOW_WIFI_ON_SELECT:
		menuInfo.currMenuScreen = MENU_WIFI_SETTINGS_SCREEN;
		LoadMenuWaitTimer(MENU_TIMER, &MenuTimeout, MENU_SCREEN_NONE);
		break;

	case GUI_BUTTON_REGULAR_IN:
		splFunc = MARK_CLOCK_IN;
		break;

	case GUI_BUTTON_REGULAR_OUT:
		splFunc = MARK_CLOCK_OUT;
		break;

	case GUI_BUTTON_BREAK_IN:
		splFunc = CLK_IN_MARKING_BREAK;
		break;

	case GUI_BUTTON_BREAK_OUT:
		splFunc = CLK_OUT_MARKING_BREAK;
		break;

	case GUI_BUTTON_SHORT_LEAVE_IN:
		splFunc = SHORT_LEAVE_MARKING_IN;
		break;

	case GUI_BUTTON_SHORT_LEAVE_OUT:
		splFunc = SHORT_LEAVE_MARKING_OUT;
		break;

	case GUI_BUTTON_OFFICIAL_IN:
		splFunc = CLK_IN_ENTRY_OFFICIAL_WORK;
		break;

	case GUI_BUTTON_OFFICIAL_OUT:
		splFunc = CLK_OUT_EXIT_OFFICIAL_WORK;
		break;

	case GUI_BUTTON_OVERTIME_IN:
		splFunc = OVER_TIME_MARKING_IN;
		break;

	case GUI_BUTTON_OVERTIME_OUT:
		splFunc = OVER_TIME_MARKING_OUT;
		break;

	case GUI_BUTTON_LATE_IN_START:
		splFunc = LATE_START_MARKING_ACT;
		break;

	case GUI_BUTTON_LATE_IN_STOP:
		splFunc = LATE_START_MARKING_DEACT;
		break;

	case GUI_BUTTON_EARLY_OUT_START:
		splFunc = EARLY_CLOSING_MARKING_ACT;
		break;

	case GUI_BUTTON_EARLY_OUT_STOP:
		splFunc = EARLY_CLOSING_MARKING_DEACT;
		break;

	case GUI_BUTTON_ENROLL_USER:
		splFunc = ENROL_USER;
		break;

	case GUI_BUTTON_ENROLL_SPL_FN:
		splFunc = ENROL_SPECIAL_CARDS;
		break;

	case GUI_BUTTON_DEL_CRED:
		splFunc = ENROL_MODE_DELETE_DATA;
		break;

	case GUI_BUTTON_DOOR_LOCK:
		splFunc = LOCK_DOOR;
		break;

	case GUI_BUTTON_DOOR_UNLOCK:
		splFunc = UNLOCK_DOOR;
		break;

	case GUI_BUTTON_DOOR_NORMAL:
		splFunc = NORMAL_DOOR;
		break;

	case GUI_BUTTON_CLEAR_ALARM:
		splFunc = CLEAR_SYS_ALARM;
		break;

	case GUI_BUTTON_CAFE_SOLDOUT_ITEM:
		splFunc = ITEM_SOLD_OUT;
		break;

	case GUI_BUTTON_CAFE_AVAILABLE_ITEM:
		splFunc = ITEM_AVAILABLE;
		break;

	/**********************************Qt Data To be Handled For Verification*******************************/
	case MSG_ETHERNET_CFG_VALIDATION_EVENT:
	case MSG_WIFI_CFG_VALIDATION_EVENT:
	case MSG_WIFI_DFLT_CFG_EVENT:
	{
		DisplayMenuScreen(MENU_ADMIN_SETTING_SCREEN);
		break;
	}

	case MSG_SERVER_CFG_VALIDATION_EVENT:
	{
		CHAR			   cmdBuf[128];
		SERVER_ADDRESS_t * serverPara = dataPtr;
		validationF 				  = FAIL;

		if( ((serverPara -> serverMode) != COSEC_THIRD_PARTY) && (serverPara -> serverPortNumber < MIN_SERVER_PORT_VAL) )
		{
			errorIdx = VALIDATION_RESP_INVALID_SERVER_PORT;
		}
		else if( ((serverPara -> serverMode) == COSEC_THIRD_PARTY) && (serverPara -> serverPortNumber < 1) )
		{
			errorIdx = VALIDATION_RESP_INVALID_SERVER_PORT;
		}
		else if( ( (IsServerModeVyom() == YES) || ((serverPara -> serverMode) == COSEC_VYOM) )
				&& ( atoll(serverPara ->serverTenantId) == 0) )
		{
			errorIdx = VALIDATION_RESP_INVALID_SERVER_TENANT_ID;
		}
		else
		{
			// SERVER MODE
			if(ServerModeCfg.serverMode != (SERVER_MODE_e)(serverPara -> serverMode) )
			{
				serverParaChangeF = TRUE;
				if((SERVER_MODE_e)(serverPara -> serverMode) == COSEC_THIRD_PARTY)
				{
					ServerModeCfg.serverMode = COSEC_THIRD_PARTY;
				}
				else
				{
					ServerModeCfg.serverMode = (SERVER_MODE_e)(serverPara -> serverMode);
				}
			}

			if(ServerModeCfg.serverMode == COSEC_THIRD_PARTY)
			{
				// SERVER URL
				if(strcmp(PushAPIConfig.pushApiUrl , serverPara -> serverURL) != 0)
				{
					serverParaChangeF = TRUE;
					sprintf(PushAPIConfig.pushApiUrl, "%s", serverPara -> serverURL);
				}

				// SERVER PORT
				if(PushAPIConfig.pushApiPort != (UINT16)serverPara -> serverPortNumber)
				{
					serverParaChangeF = TRUE;
					PushAPIConfig.pushApiPort = (UINT16)serverPara -> serverPortNumber;
				}

				if(serverParaChangeF == TRUE)
				{
					RestartThirdPartyServer();
				}

				if(WriteServerModeConfig() == SUCCESS)
				{
					DPRINT(WEBJVS_LOG, "Server Mode Config set successfully");
				}
				validationF = WritePushAPIConfig();
			}
			else
			{
				// SERVER URL
				if(strcmp(ServerParameters.ethServerHostName, serverPara -> serverURL) != 0)
				{
					serverParaChangeF = TRUE;
					sprintf(ServerParameters.ethServerHostName, "%s", serverPara -> serverURL);
				}
				if(strcmp(ServerParameters.wifiServerHostName, serverPara -> serverURL) != 0)
				{
					serverParaChangeF = TRUE;
					sprintf(ServerParameters.wifiServerHostName, "%s", serverPara -> serverURL);
				}
				if(strcmp(ServerParameters.modemServerHostName, serverPara -> serverURL) != 0)
				{
					serverParaChangeF = TRUE;
					sprintf(ServerParameters.modemServerHostName, "%s", serverPara -> serverURL);
				}

				// SERVER PORT
				if(ServerParameters.ethServerPort != (UINT16)serverPara -> serverPortNumber)
				{
					serverParaChangeF = TRUE;
					ServerParameters.ethServerPort = (UINT16)serverPara -> serverPortNumber;
				}

				if(ServerParameters.wifiServerPort != (UINT16)serverPara -> serverPortNumber)
				{
					serverParaChangeF = TRUE;
					ServerParameters.wifiServerPort = (UINT16)serverPara -> serverPortNumber;
				}

				if(ServerParameters.modemServerPort != (UINT16)serverPara -> serverPortNumber)
				{
					serverParaChangeF = TRUE;
					ServerParameters.modemServerPort = (UINT16)serverPara -> serverPortNumber;
				}

				// SERVER TENANT ID
				if(IsServerModeVyom() == YES)
				{
					if(strcmp(ServerParameters.TenantIdVyom, serverPara -> serverTenantId) != 0)
					{
						serverParaChangeF = TRUE;
						sprintf(ServerParameters.TenantIdVyom, "%s", serverPara -> serverTenantId);
					}
				}

				if(WriteServerModeConfig() == SUCCESS)
				{
					DPRINT(WEBJVS_LOG, "Server Mode Config set successfully");
				}
				CheckHttpServerConfig();
				validationF = WriteServerParaConfig();
			}
		}

		if(validationF == SUCCESS)
		{
			//Delete serverIpSignature.txt on change of Server Settings
			memset(cmdBuf,0,sizeof(cmdBuf));
			sprintf(cmdBuf,"rm %s",SERVER_IP_SIGN_FILE);
			system(cmdBuf);

			if(serverParaChangeF == TRUE)
			{
				ChangeNetworkInterfacePara(SERVER_PARA);
			}

			DisplayMenuScreen(MENU_ADMIN_SETTING_SCREEN);
		}
		else
		{
			qtDynamicValidationRespInfo.respMsgIdx = errorIdx;
			SendMessageToQt(&qtDynamicValidationRespInfo, GUI_MSG_VALIDATION_RESPONSE);
		}
		break;
	}

	case MSG_MOBILE_CFG_VALIDATION_EVENT:
	{
		BOOL 			modemParaChangeF = FALSE;
		UINT8 			profileIdx;
		MODEM_PROFILE_t	* profileConfig = dataPtr;

		if( ( ModemProfile[profileConfig -> profileNum].enable == DISABLED) && (profileConfig -> isSelected == ENABLED) )
		{
			modemParaChangeF = ENABLED;
			for(profileIdx = 0; profileIdx < MAX_MODEM_PROFILE ; profileIdx++)
			{
				ModemProfile[profileIdx].enable = FALSE;
			}
		}
		ModemProfile[profileConfig -> profileNum].enable = profileConfig -> isSelected;
		if(strcmp(ModemProfile[profileConfig -> profileNum].profileName, profileConfig->name) != 0)
		{
			modemParaChangeF = TRUE;
			strcpy(ModemProfile[profileConfig -> profileNum].profileName, profileConfig->name);

		}
		if(strcmp(ModemProfile[profileConfig -> profileNum].dialNo, profileConfig->dialNo) != 0)
		{
			modemParaChangeF = TRUE;
			strcpy(ModemProfile[profileConfig -> profileNum].dialNo, profileConfig->dialNo);
		}
		if( strcmp(ModemProfile[profileConfig -> profileNum].userName, profileConfig->usrName) != 0)
		{
			modemParaChangeF = TRUE;
			strcpy(ModemProfile[profileConfig -> profileNum].userName, profileConfig->usrName);
		}
		if(strcmp(ModemProfile[profileConfig -> profileNum].userPswd, profileConfig->passwd) != 0)
		{
			modemParaChangeF = TRUE;
			strcpy(ModemProfile[profileConfig -> profileNum].userPswd, profileConfig->passwd);
		}
		if(strcmp(ModemProfile[profileConfig -> profileNum].apn, profileConfig->apn) != 0)
		{
			modemParaChangeF = TRUE;
			strcpy(ModemProfile[profileConfig -> profileNum].apn, profileConfig->apn);
		}
		if(ModemProfile[profileConfig -> profileNum].modemType != profileConfig->serviceType)
		{
			modemParaChangeF = TRUE;
			ModemProfile[profileConfig -> profileNum].modemType = profileConfig->serviceType;
		}
		if( modemParaChangeF == TRUE )
		{
			if(WriteModemParaConfig() == SUCCESS)
			{
#ifndef DEVICE_IS_PATH_V2
				if(GetCurPPPPortType() != USB_MODEM_PORT)
#endif
				{
					if(profileConfig -> isSelected == ENABLED)
					{
						SelectedModemProfile = profileConfig -> profileNum;
						ChangeNetworkInterfacePara(MODEM_PARA);
					}
					else
					{
						ChangeNetworkInterfacePara(INTERFACE_PARA);
					}
				}
			}
		}
		DisplayMenuScreen(MENU_ADMIN_SETTING_SCREEN);
		break;
	}

	case MSG_DATE_TIME_VALIDATION_EVENT:
		{
			DATE_DDMMYYYY_t date;
			TIME_t			time;

			CHARPTR		msgStr;
			CHARPTR		fieldPtr;

			// Get pointer to TIME field
			msgStr = ((DATE_TIME_t *)dataPtr) -> time;

			// Get pointer to HOUR field
			fieldPtr = GetStrBeforeFsp(&msgStr, TIME_FSP);
			if(fieldPtr)
			{
				time.hour = atoi(fieldPtr);
			}

			// Get pointer to MINUTE field
			fieldPtr = GetStrBeforeFsp(&msgStr, TIME_FSP);
			if(fieldPtr)
			{
				time.min = atoi(fieldPtr);
			}

			// Get pointer to SECOND field
			fieldPtr = GetStrBeforeFsp(&msgStr, TIME_FSP);
			if(fieldPtr)
			{
				time.sec = atoi(fieldPtr);
			}

			// Get pointer to DATE field
			msgStr = ((DATE_TIME_t *)dataPtr) -> date;

			// Get pointer to DAY field
			fieldPtr = GetStrBeforeFsp(&msgStr, DATE_FSP);
			if(fieldPtr)
			{
				date.day = atoi(fieldPtr);
			}

			// Get pointer to MONTH field
			fieldPtr = GetStrBeforeFsp(&msgStr, DATE_FSP);
			if(fieldPtr)
			{
				date.month = atoi(fieldPtr);
			}

			// Get pointer to YEAR field
			fieldPtr = GetStrBeforeFsp(&msgStr, DATE_FSP);
			if(fieldPtr)
			{
				date.year = atoi(fieldPtr);
			}

			if(ChangeDateTime(date, time, LOCAL_TIME) == SUCCESS)
			{
				DisplayMenuScreen(MENU_DATE_TIME_SETTINGS_SCREEN);
			}
			else
			{
				SetAudioVisual(USER_RESP_MENU_FAILED, INTERNAL_PORT, NULL);
				LoadMenuWaitTimer(MENU_USER_RESP_TIMER, &DisplayMenuScreen, MENU_HRD_ADMIN_E_CANTEEN_SCREEN);
			}
			break;
		}

	case MSG_ADMIN_PW_VALIDATION_EVENT:
	{
		CHANGE_PASSWORD_t	* pwConfig = dataPtr;
		ChangeDisplayPswd(USER_ADMIN, pwConfig->newPw);
		WritePswdConfig();
        DcInfo.isUserAuthenticatedF=TRUE;
		if(menuInfo.state == MENU_ADMIN_STATE)
		{
			DisplayMenuScreen(MENU_ADMIN_SCREEN);
		}
		else if(menuInfo.state == MENU_ABOUT_DEVICE_STATE)
		{
			DisplayMenuScreen(MENU_HRD_ADMIN_E_CANTEEN_SCREEN);
		}
		else
		{
			DisplayMenuScreen(MENU_ADMIN_SETTING_SCREEN);
		}
		break;
	}

	case MSG_HRD_PW_VALIDATION_EVENT:
	{
		CHANGE_PASSWORD_t	* pwConfig = dataPtr;
		ChangeDisplayPswd(USER_HRD, pwConfig->newPw);
		WritePswdConfig();
		DisplayMenuScreen(MENU_HRD_ADMIN_E_CANTEEN_SCREEN);
		break;
	}

	case GUI_BUTTON_DEFAULT_HRD_PASSWORD:
	{
		ChangeDisplayPswd(USER_HRD, (CHARPTR)DfltPassword.displayPswd[USER_HRD]);
		WritePswdConfig();
		InvalidAttemptWaitTimeout(USER_HRD);
		SetAudioVisual(USER_RESP_PASSWORD_DEFAULTED, INTERNAL_PORT, NULL);
		LoadMenuWaitTimer(MENU_USER_RESP_TIMER, &DisplayMenuScreen, MENU_PASSWORD_SCREEN);
		break;
	}

	case MSG_LOGIN_MENU_VALIDATION_EVENT:
	{
        if(ValidateMenuLogin((LOGIN_PASSWORD_t *) dataPtr) == SUCCESS)
		{
			DisplayMenuScreen(MENU_HRD_ADMIN_E_CANTEEN_SCREEN);
		}
		break;
	}

	case MSG_BACKGROUND_SET_EVENT:
		SysVar.backgroundImage = ( (BACKGROUND_SET_t *)dataPtr) -> imageIdx;
		WriteSysVars();
		DisplayMenuScreen(MENU_DISPLAY_OPTION_SCREEN);
		break;

	case MSG_CLOCK_SET_EVENT:
		SysVar.currentClock = ( (CLOCK_SET_t *)dataPtr) -> clockIdx;
		WriteSysVars();
		DisplayMenuScreen(MENU_DISPLAY_OPTION_SCREEN);
		break;

	case MSG_KEYPAD_SET_EVENT:
		SysVar.currentKeypad = (KEYPAD_e )( ( (KEYPAD_SET_t *)dataPtr) -> keypadIdx);
		WriteSysVars();
		DisplayMenuScreen(MENU_DISPLAY_OPTION_SCREEN);
		break;

	case GUI_MSG_BRIGHTNESS_VALIDATION_EVENT:
		DisplayBrightness.sleepTimer = (UINT16 )( ( (BRIGHTNESS_DETAIL_t *)dataPtr) -> sleepTimer);
		DisplayBrightness.setBrightness = (PWM_DUTY_e )( ( (BRIGHTNESS_DETAIL_t *)dataPtr) -> setBrightness);
		if(WriteDisplayBrightnessConfig() == FAIL)
		{
			DPRINT(SYS_LOG,"Display Brightness Configuration Write Fail\n");
		}
		DisplayMenuScreen(MENU_DISPLAY_OPTION_SCREEN);
		break;

	case MSG_SEND_MODEM_PROFILE_DATA:
		DisplayMenuScreen(MENU_BROADBAND_SETTINGS_SCREEN);
		break;

	case GUI_BUTTON_DOOR_MODE_CHANGE:
		if(SamayModeEnable == TRUE)
		{
			SetAudioVisual(MODE_NOT_SUPPORTED_SCREEN, INTERNAL_PORT, NULL);
		}
		else
		{
			SetDoorApplType(PANEL_DOOR_APPL);
			break;
		}
		break;

		case GUI_BUTTON_SERVER_MODE_CHANGE:
		{
			CHARPTR		msgStr;
			CHARPTR		fieldPtr;

			msgStr = dataPtr;

			// Get pointer to YEAR field
			fieldPtr = GetStrBeforeFsp(&msgStr, UI_MSG_FSP);
			if(fieldPtr)
			{
				ServerModeCfg.serverMode = (SERVER_MODE_e)atoi(fieldPtr);

				if(WriteServerModeConfig() == SUCCESS)
				{
					DPRINT(WEBJVS_LOG, "Server Mode Config set successfully");
				}

				LoadDfltVyomServerConfig();
				LoadDfltServerParaConfig();
				#ifdef PUSH_API_SUPPORTED
				LoadDfltPushApiConfig();
				#endif

				if(WriteDfltSysFlag(SYS_DFLT_BASIC) == SUCCESS)
				{
					SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
				}
			}
			break;
		}

		case GUI_MSG_WIFI_ENABLE_DISABLE_FLAG:
		{
			BOOL wifiEnableF;

			wifiEnableF = *((CHARPTR)dataPtr);

			if(wifiEnableF != WifiParameters.enableWifi )
			{
				pthread_mutex_lock(&MutexLockAutoSwitchWiFi);
				WifiParameters.enableWifi = wifiEnableF;

				if(WifiParameters.enableWifi == ENABLED)
				{
					ManualWifiConnectF = TRUE;
				}

				StartWifiEnableDisableThreadFunc(WifiParameters.enableWifi);
				SaveWifiParaToSavedWifiList();

				if(WriteWifiParaConfig() == SUCCESS)
				{
					DPRINT(GUI_LOG,"Wifi Parameter Config set successfully");
				}
				pthread_mutex_unlock(&MutexLockAutoSwitchWiFi);
			}
		}
		break;

		case GUI_BUTTON_TEMPERATURE:
		{
			/* Do current temperature query */
			DoTemperatureQuery(TEMP_QUERY_NORMAL);

			tempSensorInfo.isTempQueryInBackground = FALSE;
			DisplayMenuScreen(MENU_TEMPERATURE_SCREEN);
			break;
		}

		case GUI_MSG_TEMPERATURE_REFRESH:
		{
			/* Reset temperature requested flag */
			tempSensorInfo.temperatureReqested = FALSE;
			tempSensorInfo.isTempQueryInBackground = FALSE;

            if ((IsTempratureConfigApplicable() == TRUE) && (tempSensorInfo.tempereatureSensorStatus == ACTIVE))
			{
				SendMessageToQt("", GUI_MSG_AMBIENT_TEMP_DETECTING_SCREEN);
			}
			else
			{
				/* Display sensor disconnected screen */
				SetAudioVisual(USER_RESP_TEMPERATURE_SENSOR_DISCONNECTED, INTERNAL_PORT, NULL);
			}
			break;
		}

		case GUI_MSG_AMBIENT_COUNTDOWN_COMPLETED:
		{
			/* Check sensor connectivity */
            if ((IsTempratureConfigApplicable() == TRUE) && (tempSensorInfo.tempereatureSensorStatus == ACTIVE))
			{
				/* Display temperature detecting screen */
				SetAudioVisual(USER_RESP_DETECTING_TEMPERATURE_SCREEN, INTERNAL_PORT, NULL);

				/* Do ambient temperature query */
				DoTemperatureQuery(TEMP_QUERY_AMBIENT);
			}
			else if (tempSensorInfo.tempereatureSensorStatus == INACTIVE)
			{
				/* Display sensor disconnected screen */
				SetAudioVisual(USER_RESP_TEMPERATURE_SENSOR_DISCONNECTED, INTERNAL_PORT, NULL);
			}
			break;
		}

		/* To detect ambient temperature in background*/
		case MSG_TEMPERATURE_DETECTED_EVENT:
		{
			FLOAT32 detectedTemperature = 0.00;

			/* Get user's temperature detected by temperature sensor */
			detectedTemperature = *((FLOAT32*)dataPtr);

			/* Check if ambient temperature requested or current temperature */
			if ((tempSensorInfo.isAmbientTempRequired == TRUE) && (tempSensorInfo.isTempQueryInBackground == FALSE))
			{
				/* Do ambient temperature detection Process and check if finished */
				if (TRUE == ProcAmbientTemperatureDetection(detectedTemperature))
				{
                    if ((IsTempratureConfigApplicable() == TRUE) && (tempSensorInfo.tempereatureSensorStatus == ACTIVE))
					{
						/* Update ambient temperature on display */
						DisplayMenuScreen(MENU_TEMPERATURE_SCREEN);

						/*  Play success cadence */
						SetBuzLedResp(USER_RESP_SUCCESS_SCREEN, INTERNAL_PORT);

						/* Do current temperature query */
						DoTemperatureQuery(TEMP_QUERY_NORMAL);
					}
					else
					{
						/* Display sensor disconnected screen */
						SetAudioVisual(USER_RESP_TEMPERATURE_SENSOR_DISCONNECTED, INTERNAL_PORT, NULL);
					}
				}
			}
			else if (tempSensorInfo.isTempQueryInBackground == FALSE)
			{
				/* Check if calibration required */
				if (SysAdvPara.temperatureLogCnfg.calibrationPara != 0)
				{
					/* Calibrate temperature detected */
					detectedTemperature = detectedTemperature + SysAdvPara.temperatureLogCnfg.calibrationPara;
				}

				DPRINT(SYS_LOG, "Calibrated detected temperature[%f]", detectedTemperature);

				/* Update current temperature */
				tempSensorInfo.currentTemperature = detectedTemperature;

				/* Start ambient temperature detection only when temperature sensing is enabled and sensor is active */
                if ((IsTempratureConfigApplicable() == TRUE) && (tempSensorInfo.tempereatureSensorStatus == ACTIVE))
				{
					/* Update current temperature on display */
					SendMessageToQt("", GUI_MSG_TEMPERATURE_MENU_SCREEN);

					/* Do current temperature query */
					DoTemperatureQuery(TEMP_QUERY_NORMAL);
				}
			}
			break;
		}
	}

	// Check if special function is to be pocessed
	if(splFunc != MAX_SPECIAL_FUNC)
	{
		StopMenuActivity();
		DcInfo.splFnId = splFunc;
		if(ProcessSplFn() == FALSE)
		{
			if(DcInfo.splFnThroughCard == TRUE)
			{
				SwitchDcToIdleState();
			}
			else
			{
				LoadMenuWaitTimer(LCD_IDLE_TIMER, &DisplayMenuScreen, menuInfo.currMenuScreen);
			}
		}
	}
}

//*****************************************************************************
//	ValidateMenuLogin()
//	Param:
//		IN :	pswdInfo - login password
//		OUT:	None
//	Returns:	SUCCESS	- If password matches with HRD/ADMIN
//				FAIL	- If password does not match with HRD/ADMIN
//	Description:
//				This function validates if given password matches with ADMIN or
//				HRD password.
//*****************************************************************************
static BOOL ValidateMenuLogin(LOGIN_PASSWORD_t * pswdInfo)
{
	BOOL status = FAIL;
    CONFIG_USER_TYPE_e userType = MAX_USER_TYPE;

	if( (menuInfo.state == MENU_HRD_STATE) || (menuInfo.state == MENU_E_CANTEEN_STATE) )
	{
        userType  = USER_HRD;
		// If given password is matched with HRD password
        if(CompareDisplayPswd(USER_HRD, pswdInfo -> loginPw) == TRUE && Password.DispPwdLoginRestrictTimer[USER_HRD] == INVALID_TIMER_HANDLE)
		{
            ResetDispPwdAttemptData(USER_HRD);
			status = SUCCESS;
		}
	}
	else if( (menuInfo.state == MENU_ADMIN_STATE) || (menuInfo.state == MENU_ABOUT_DEVICE_STATE) )
	{
        userType = USER_ADMIN;
		// If given password is matched with ADMIN password
        if(CompareDisplayPswd(USER_ADMIN, pswdInfo -> loginPw) == TRUE && Password.DispPwdLoginRestrictTimer[USER_ADMIN] == INVALID_TIMER_HANDLE)
		{
            DcInfo.isUserAuthenticatedF = TRUE;
            ResetDispPwdAttemptData(USER_ADMIN);
			status = SUCCESS;
		}
    }

    if(status == FAIL)
    {
        SetDispPwdAttemptCount(userType, Password.DispPwdAttemptCount[userType] - 1);
        SetAudioVisual(USER_RESP_MENU_LOGIN_RESTRICTED, INTERNAL_PORT, (VOIDPTR)&userType);
        if(Password.DispPwdLoginRestrictTimer[userType] == INVALID_TIMER_HANDLE)
        {
            LoadMenuWaitTimer(MENU_USER_RESP_TIMER, &DisplayMenuScreen, MENU_LOGIN_SCREEN);
        }
        else
        {
            LoadMenuWaitTimer(MENU_USER_RESP_TIMER, &DisplayMenuScreen, MENU_MAIN_MENU_SCREEN);
        }
    }
	return status;
}

//*****************************************************************************
//	ExitMenuState()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function cleans up MENU activity to abnormally exit MENU
//				state.
//*****************************************************************************
static void ExitMenuState(void)
{
	StopMenuActivity();
	SwitchDcToIdleState();
}

//*****************************************************************************
//	DisplayMenuScreen()
//	Param:
//		IN :	menuScreen -  screen to display
//		OUT:	None
//	Returns:	None
//	Description:
//				This function loads menu screen as per input and loads menu
//				timer.
//*****************************************************************************
void DisplayMenuScreen(MENU_SCREEN_e menuScreen)
{
	CHAR msgBuf[1024];

	LoadMenuWaitTimer(MENU_TIMER, &MenuTimeout, MENU_SCREEN_NONE);

	switch(menuScreen)
	{
	default:
		break;

	case MENU_MAIN_MENU_SCREEN:
		SendMessageToQt("", GUI_MSG_MENU_SCREEN);
		break;

	case MENU_LOGIN_SCREEN:
		SendMessageToQt(" ", GUI_MSG_MENU_LOGIN_SCREEN);
		break;

	case MENU_HRD_ADMIN_E_CANTEEN_SCREEN:
		if(menuInfo.state == MENU_ADMIN_STATE)
		{
			menuScreen = MENU_ADMIN_SCREEN;
			SendMessageToQt(" ",GUI_MSG_ADMIN_MENU_SCREEN);
		}
		else if(menuInfo.state == MENU_HRD_STATE)
		{
			menuScreen = MENU_HRD_SCREEN;
			SendMessageToQt(" ", GUI_MSG_HRD_MENU_SCREEN);
		}
		else if(menuInfo.state == MENU_E_CANTEEN_STATE)
		{
			menuScreen = MENU_MANAGE_CANTEEN_SCREEN;
			SendMessageToQt(" ", GUI_MSG_MANAGE_CANTEEN_SCREEN);
		}
		else if(menuInfo.state == MENU_ABOUT_DEVICE_STATE)// About Device option from device display
		{
			menuScreen = MENU_DISPLAY_STATUS_SCREEN;
			SendMessageToQt(" ", GUI_MSG_SYS_STATUS_SCREEN);
		}
		break;

	case MENU_MANAGE_CANTEEN_SCREEN:
		//menuScreen = MENU_MANAGE_CANTEEN_SCREEN;
		SendMessageToQt(" ", GUI_MSG_MANAGE_CANTEEN_SCREEN);
		break;

	case MENU_E_CANTEEN_SCREEN:
		//menuScreen = MENU_E_CANTEEN_SCREEN;
		SendMessageToQt(" ", GUI_MSG_E_CANTEEN_MENU_SCREEN);
		break;

	case MENU_HRD_SCREEN:
		//menuScreen = MENU_HRD_SCREEN;
		SendMessageToQt(" ", GUI_MSG_HRD_MENU_SCREEN);
		break;

	case MENU_ADMIN_SCREEN:
		//menuScreen = MENU_ADMIN_SCREEN;
		SendMessageToQt(" ", GUI_MSG_ADMIN_MENU_SCREEN);
		break;

	case MENU_PASSWORD_SCREEN:
		SendMessageToQt(" ", GUI_MSG_MENU_PASSWORD_SCREEN);
		break;

	case MENU_CHANGE_PSWD_SCREEN:
		// NOTE: this screen is right now directly displayed
		SendMessageToQt(" ", GUI_MSG_CHANGE_PW_SCREEN);
		break;

	case MENU_DATE_TIME_SETTINGS_SCREEN:
		// Pass screen message id only
		SendMessageToQt(" ", GUI_MSG_DATE_TIME_SCREEN);
		break;

	case MENU_LAN_SETTINGS_SCREEN:
		// Pass screen message id only
		SendMessageToQt(" ", GUI_MSG_EHTERNET_CFG_SCREEN);
		break;

	case MENU_SERVER_SETTINGS_SCREEN:
		// Pass screen message id only
		SendMessageToQt(" ", GUI_MSG_ACMS_SERVER_CFG_SCREEN);
		break;

	case MENU_WIFI_SETTINGS_SCREEN:
		// Pass screen message id only
		SendMessageToQt(" ", GUI_MSG_WIFI_CFG_SCREEN);
		break;

	case MENU_BROADBAND_PROFILE_SCREEN:
		// Pass screen message id only
		SendMessageToQt(" ", GUI_MSG_MODEM_PROFILE_SCREEN);
		break;

	case MENU_BROADBAND_SETTINGS_SCREEN:
		// Pass screen message id only
		SendMessageToQt( "" , GUI_MSG_MODEM_PROFILE_DATA);
		break;

	case MENU_BACKGROUND_SELECTION_SCREEN:
		// Pass screen message id only
		SendMessageToQt(" ", GUI_MSG_BACKGROUND_SELECTION_SCREEN);
		break;

	case MENU_CLOCK_SELECTION_SCREEN:
		// Pass screen message id only
		SendMessageToQt(" ", GUI_MSG_CLOCK_SELECTION_SCREEN);
		break;

	case MENU_KEYPAD_SELECTION_SCREEN:
		// Pass screen message id only
		SendMessageToQt(" ", GUI_MSG_KEYPAD_SELECTION_SCREEN);
		break;

	case MENU_BRIGHTNESS_SETTING_SCREEN:
		// Pass screen message id only
		SendMessageToQt(" ", GUI_MSG_BRIGHTNESS_SELECTION_SCREEN);
		break;

	case MENU_DISPLAY_STATUS_SCREEN:
		// Pass screen message id only
		if(strlen(Password.displayPswd[USER_ADMIN]) > 0)
		{
			DisplayMenuScreen(MENU_LOGIN_SCREEN);
		}
		else
		{
			DisplayMenuScreen(MENU_CHANGE_PSWD_SCREEN);
		}
		break;

	case MENU_ABOUT_DEVICE_CONNECTIVITY:
		// Pass screen message id only
		menuScreen = MENU_ABOUT_DEVICE_CONNECTIVITY;
		SendMessageToQt(" ", GUI_MSG_ABOUT_DEVICE_CONNECTIVITY_SCREEN);
		break;

	case MENU_ABOUT_DEVICE_READERS_INTERFACES:
		// Pass screen message id only
		menuScreen = MENU_ABOUT_DEVICE_READERS_INTERFACES;
		SendMessageToQt(" ", GUI_MSG_ABOUT_DEVICE_READERS_INTERFACES_SCREEN);
		break;

	case MENU_ABOUT_DEVICE_ALARMS:
		// Pass screen message id only
		menuScreen = MENU_ABOUT_DEVICE_ALARMS;
		SendMessageToQt(" ", GUI_MSG_ABOUT_DEVICE_ALARMS_SCREEN);
		break;

	case MENU_ADMIN_SETTING_SCREEN:
		sprintf(msgBuf,"%d%c%d%c",GetWiFiStatus(), UI_MSG_FSP, SamayModeEnable, UI_MSG_FSP );
		SendMessageToQt(msgBuf, GUI_MSG_ADMIN_SETTING_SCREEN);
		break;

	case MENU_ADMIN_DOOR_CONTROL_SCREEN:
		SendMessageToQt(" ", GUI_MSG_ADMIN_DOOR_CONTROL_SCREEN);
		break;

	case MENU_ADMIN_BACKUP_AND_UPDATE_SCREEN:
		SendMessageToQt(" ", GUI_MSG_ADMIN_BACKUP_AND_UPDATE_SCREEN);
		break;

	case MENU_ENROLLMENT_SCREEN:
		SendMessageToQt(" ", GUI_MSG_ENROLLMENT_SCREEN);
		break;

	case MENU_DISPLAY_OPTION_SCREEN:
		SendMessageToQt(" ", GUI_MSG_DISPLAY_MENU_SCREEN);
		break;

	case MENU_TEMPERATURE_SCREEN:
		SendMessageToQt("", GUI_MSG_TEMPERATURE_MENU_SCREEN);
		break;
	}

	// Update current menu screen
	menuInfo.currMenuScreen = menuScreen;

	SetLcdIdleTimer(MENU_TIMER);
}

//*****************************************************************************
//	BackMenuScreen()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function loads previous menu screen.
//*****************************************************************************
static void BackMenuScreen(void)
{
	switch(menuInfo.currMenuScreen)
	{
	// Ignore and return for these cases
	default:
	case MENU_LOGIN_SCREEN:
	case MENU_HRD_ADMIN_E_CANTEEN_SCREEN:
	case MENU_ADMIN_SCREEN:
		return;

	case MENU_DISPLAY_STATUS_SCREEN:
		DisplayMenuScreen(MENU_MAIN_MENU_SCREEN);
		break;

	case MENU_ABOUT_DEVICE_ALARMS:
	case MENU_ABOUT_DEVICE_CONNECTIVITY:
	case MENU_ABOUT_DEVICE_READERS_INTERFACES:
		menuInfo.currMenuScreen = MENU_DISPLAY_STATUS_SCREEN;
		SendMessageToQt(" ", GUI_MSG_SYS_STATUS_SCREEN);
		break;

	case MENU_ENROLLMENT_SCREEN:
		DisplayMenuScreen(MENU_HRD_ADMIN_E_CANTEEN_SCREEN);
		break;

	case MENU_CAFE_SOLDOUT_ITEM_SCREEN:
	case MENU_CAFE_AVAILBALE_ITEM_SCREEN:
		DisplayMenuScreen(MENU_MANAGE_CANTEEN_SCREEN);
		break;

	case MENU_HRD_SCREEN:
		if(menuInfo.state == MENU_ADMIN_STATE)
		{
			DisplayMenuScreen(MENU_ADMIN_SCREEN);
		}
		break;

	case MENU_ADMIN_SETTING_SCREEN:
	case MENU_ADMIN_BACKUP_AND_UPDATE_SCREEN:
	case MENU_ADMIN_DOOR_CONTROL_SCREEN:
	case MENU_TEMPERATURE_SCREEN:
		DisplayMenuScreen(MENU_HRD_ADMIN_E_CANTEEN_SCREEN);
		break;

	case MENU_CHANGE_PSWD_SCREEN:
	{
        if(DcInfo.isUserAuthenticatedF == TRUE)
        {
            if(menuInfo.state == MENU_ADMIN_STATE)
            {
                DisplayMenuScreen(MENU_PASSWORD_SCREEN);
            }
            else
            {
                DisplayMenuScreen(MENU_HRD_ADMIN_E_CANTEEN_SCREEN);
            }
        }
        else
        {
            /* goto idle Screen if user is Not Authenticated */
            SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
            ExitMenuState();
        }
		break;
	}
	case MENU_BROADBAND_PROFILE_SCREEN:
	case MENU_BROADBAND_SETTINGS_SCREEN:
	case MENU_WIFI_SETTINGS_SCREEN:
	case MENU_DATE_TIME_SETTINGS_SCREEN:
	case MENU_LAN_SETTINGS_SCREEN:
	case MENU_SERVER_SETTINGS_SCREEN:
	case MENU_DISPLAY_OPTION_SCREEN:
	case MENU_WIFI_WPS_WAIT_SCREEN:
	case MENU_PASSWORD_SCREEN:
		DisplayMenuScreen(MENU_ADMIN_SETTING_SCREEN);
		break;

	case MENU_WIFI_SEARCH_RESULT_SCREEN:
		DisplayMenuScreen(MENU_WIFI_SETTINGS_SCREEN);
		break;

	case MENU_BACKGROUND_SELECTION_SCREEN:
	case MENU_CLOCK_SELECTION_SCREEN:
	case MENU_KEYPAD_SELECTION_SCREEN:
	case MENU_BRIGHTNESS_SETTING_SCREEN:
		DisplayMenuScreen(MENU_DISPLAY_OPTION_SCREEN);
		break;
	}
}

//*****************************************************************************
//	StopMenuActivity()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//			This function stops going on menu activivty.
//*****************************************************************************
void StopMenuActivity(void)
{
	menuInfo.wifiThreadStatus = INACTIVE;
	DeleteTimer(&menuInfo.menuTmrHandle);
}

//*****************************************************************************
//	HandleWpsConnectedEv()
//	Param:
//		IN : NONE
//		OUT:
//	Returns:
//			 NONE
//	Description:
//			This function unloads wps wait popup, exits from menu state and
//			switches DC to idle when Wifi connection establishes through wps.
//*****************************************************************************
void HandleWpsConnectedEv(void)
{
	//If connection is established switch to idle state
	if(DcInfo.curState == DC_MENU_STATE && menuInfo.currMenuScreen == MENU_WIFI_WPS_WAIT_SCREEN)
	{
		SetAudioVisual(USER_RESP_WIFI_CONNECTED, INTERNAL_PORT, NULL);
		LoadMenuWaitTimer(MENU_USER_RESP_TIMER, &DisplayMenuScreen, MENU_ADMIN_SETTING_SCREEN);
	}
	else if(CommState != SLAVE_ONLINE && PDCurrentMenuScreen == MENU_WIFI_WPS_WAIT_SCREEN)
	{
		SetAudioVisual(USER_RESP_PD_WIFI_CONNECTED, INTERNAL_PORT, NULL);
		LoadMenuWaitTimer(MENU_USER_RESP_TIMER, &PDDisplayMenuScreen, MENU_ADMIN_SETTING_SCREEN);
	}
	else if( GetDoorApplType() == PANEL_DOOR_APPL && CommState == SLAVE_ONLINE)
	{
		// If panel door and door is ONLINE then timer will be loaded in panel
		// In Panel this event will be handled only in the menu state.
		SendMsg(MSG_WPS_WIFI_CONNECTED, NULL, 0);
	}
}

//*****************************************************************************
//	ChangeMenuInfoStateToAdmin
//	Param:
//		IN :none
//		OUT: none
//	Returns:
//	Description:
//          Changes the menuinfo State to MENU_ADMIN_STATE
//*****************************************************************************
void ChangeMenuInfoStateToAdmin(void)
{
    if(menuInfo.state != MENU_ADMIN_STATE)
    {
        menuInfo.state = MENU_ADMIN_STATE;
    }
    return;
}

//*****************************************************************************
//	SetDispInvalidAttemptWaitTimer()
//	Param:
//		IN :    userType
//		OUT: 	NONE
//	Returns:    UINT8
//	Description: returns Display Password Attempt Count For Specific user
//
//*****************************************************************************
UINT8 GetDispPwdAttemptCount(CONFIG_USER_TYPE_e userType)
{
    return Password.DispPwdAttemptCount[userType];
}

//*****************************************************************************
//	SetDispInvalidAttemptWaitTimer()
//	Param:
//		IN :    userType
//         :    AttemptCount
//		OUT: 	NONE
//	Returns: NONE
//	Description: sets Display Password Attempt Count For Specific user
//
//*****************************************************************************
void SetDispPwdAttemptCount(CONFIG_USER_TYPE_e userType, UINT8 AttemptCount)
{
    if(userType < MAX_USER_TYPE && AttemptCount <= MAX_DISP_PWD_ATTEMPT_COUNT &&
            Password.DispPwdAttemptCount[userType] != AttemptCount && Password.DispPwdLoginRestrictTimer[userType] == INVALID_TIMER_HANDLE)
    {
        Password.DispPwdAttemptCount[userType] = AttemptCount;

        if(Password.DispPwdAttemptCount[userType] == 0)
        {
            Password.DispPwdAttemptCount[userType] = MAX_DISP_PWD_ATTEMPT_COUNT;
            SetDispInvalidAttemptWaitTimer(userType);
        }
        WritePswdConfig();
    }
    return;
}

//*****************************************************************************
//	SetDispInvalidAttemptWaitTimer()
//	Param:
//		IN :    userType
//		OUT: 	NONE
//	Returns: SUCCESS/FAIL
//	Description: Start The Timer For Specific User,To Block Login On The Display Menu
//
//*****************************************************************************
BOOL SetDispInvalidAttemptWaitTimer(CONFIG_USER_TYPE_e userType)
{
    if(Password.DispPwdLoginRestrictTimer[userType] == INVALID_TIMER_HANDLE)
    {
        TIMER_INFO_t tInfo;
        tInfo.count = (60 * 10);
        tInfo.data = userType;
        tInfo.funcPtr = &DispInvalidAttemptWaitTimeout;
        if(StartTimer(tInfo, &Password.DispPwdLoginRestrictTimer[userType], __FUNCTION__) == FAIL)
        {
            return FAIL;
        }
    }
    return SUCCESS;
}

//*****************************************************************************
//	ResetDispPwdAttemptData
//	Param:
//		IN :	userType
//		OUT:	none
//	Returns:	none
//	Description:Reset the Display Password Attempt Related Data for particular user.
//
//*****************************************************************************
void ResetDispPwdAttemptData(UINT32 userType)
{
    if(userType == MAX_USER_TYPE)
    {
        for(userType = 0; userType < MAX_USER_TYPE; userType++)
        {
            DeleteTimer(&Password.DispPwdLoginRestrictTimer[userType]);
            Password.DispPwdAttemptCount[userType] = MAX_DISP_PWD_ATTEMPT_COUNT;
            Password.DispPwdLoginRestrictTimer[userType] = INVALID_TIMER_HANDLE;
        }
    }
    else
    {
        DeleteTimer(&Password.DispPwdLoginRestrictTimer[userType]);
        Password.DispPwdAttemptCount[userType] = MAX_DISP_PWD_ATTEMPT_COUNT;
        Password.DispPwdLoginRestrictTimer[userType] = INVALID_TIMER_HANDLE;
    }
    WritePswdConfig();
    return;
}

//*****************************************************************************
//	DispInvalidAttemptWaitTimeout
//	Param:
//		IN :	userType
//		OUT:	none
//	Returns:	none
//	Description:delete the timer and set variable 0 for particular user.
//
//*****************************************************************************
void DispInvalidAttemptWaitTimeout(UINT32 userType)
{
    if(Password.DispPwdLoginRestrictTimer[userType] == INVALID_TIMER_HANDLE)
    {
        ResetDispPwdAttemptData(userType);
        return;
    }

    DeleteTimer(&Password.DispPwdLoginRestrictTimer[userType]);
    Password.DispPwdLoginRestrictTimer[userType] = INVALID_TIMER_HANDLE;
    if(Password.DispPwdAttemptCount[userType] != 0)
    {
        SetDispInvalidAttemptWaitTimer(userType);
        --Password.DispPwdAttemptCount[userType];
        WritePswdConfig();
    }

    if(Password.DispPwdAttemptCount[userType] == 0)
    {
        ResetDispPwdAttemptData(userType);
    }
    return;
}

