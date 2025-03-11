#if !defined MXMENU_H
#define MXMENU_H
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
//   Project      : NGT( Next Generation Terminal)
//   Created By   : Mukesh Savaliya
//   File         : MxMenu.h
//   Description  : This file contains all the defines and data types related
//					to MENU. 
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxSlaveCommonMsgList.h"

//******** Defines and Data Types ****
#define MENU_USER_RESP_TIMER	2	// timer in seconds
#define MAX_DISP_PWD_ATTEMPT_COUNT 		5
#define MAX_DISP_INVALID_ATTEMPT_WAIT_TIMER 	5  // 5 min

typedef enum
{
	MENU_SCREEN_NONE = 0,
	MENU_MAIN_MENU_SCREEN,
	MENU_CURRENT_SCREEN,
	MENU_HRD_ADMIN_E_CANTEEN_SCREEN,
	MENU_LOGIN_SCREEN,
	MENU_HRD_SCREEN,
	MENU_E_CANTEEN_SCREEN,
	MENU_MANAGE_CANTEEN_SCREEN,
	MENU_ADMIN_SCREEN,
	MENU_DATE_TIME_SETTINGS_SCREEN,
	MENU_CHANGE_PSWD_SCREEN,
	MENU_LAN_SETTINGS_SCREEN,
	MENU_SERVER_SETTINGS_SCREEN,
	MENU_WIFI_SETTINGS_SCREEN,
	MENU_WIFI_SEARCH_RESULT_SCREEN,
	MENU_BROADBAND_PROFILE_SCREEN,
	MENU_BROADBAND_SETTINGS_SCREEN,
	MENU_DISPLAY_STATUS_SCREEN,
	MENU_ADMIN_SETTING_SCREEN,
	MENU_ADMIN_DOOR_CONTROL_SCREEN,
	MENU_ADMIN_BACKUP_AND_UPDATE_SCREEN,
	MENU_CAFE_SOLDOUT_ITEM_SCREEN,
	MENU_CAFE_AVAILBALE_ITEM_SCREEN,
	MENU_ENROLLMENT_SCREEN,
	MENU_DISPLAY_OPTION_SCREEN,
	MENU_BACKGROUND_SELECTION_SCREEN,
	MENU_CLOCK_SELECTION_SCREEN,
	MENU_KEYPAD_SELECTION_SCREEN,
	MENU_ABOUT_DEVICE_CONNECTIVITY,
	MENU_ABOUT_DEVICE_READERS_INTERFACES,
	MENU_ABOUT_DEVICE_ALARMS,
	MENU_WIFI_WPS_WAIT_SCREEN,
	MENU_PASSWORD_SCREEN,
	MENU_BRIGHTNESS_SETTING_SCREEN,
	MENU_TEMPERATURE_SCREEN,
}MENU_SCREEN_e;

typedef enum
{
	CAFE_MENU_FIRST_PAGE,
	CAFE_MENU_NEXT_PAGE,
	CAFE_MENU_PREV_PAGE,
	MAX_CAFE_MENU_NAVIGATION,
}CAFE_MENU_CODE_NAVIGATION_e;

//******** Function Prototypes *******
void InitMenuActivity(void);
void SwitchDcToMenuState(void);
void DcMenuStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
void DisplayMenuScreen(MENU_SCREEN_e menuScreen);
BOOL LoadMenuWaitTimer(UINT8 timerValue, VOIDPTR funcPtr, MENU_SCREEN_e menuScreen);
void HandleWpsConnectedEv(void);
void MenuTimeout(UINT32 dummy);
void SearchWirelessNw(void);
void StartWaitForWpsPinThread(void);
void StopMenuActivity(void);
void ChangeMenuInfoStateToAdmin(void);
BOOL LoadMenuWaitTimer(UINT8 timerValue, VOIDPTR funcPtr, MENU_SCREEN_e menuScreen);
void SetDispPwdAttemptCount(CONFIG_USER_TYPE_e user, UINT8 AttemptCount);
UINT8 GetDispPwdAttemptCount(CONFIG_USER_TYPE_e user);
BOOL SetDispInvalidAttemptWaitTimer(CONFIG_USER_TYPE_e userType);
UINT32 GetRemainAttemptWaitTime(CONFIG_USER_TYPE_e userType);
void ResetDispPwdAttemptData(UINT32 userType);
void DispInvalidAttemptWaitTimeout(UINT32 userType);

#endif  // MXMENU_H
