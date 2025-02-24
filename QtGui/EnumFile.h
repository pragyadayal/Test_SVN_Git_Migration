#ifndef ENUMFILE_H
#define ENUMFILE_H
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
//   Owner        : Kaival Shah
//   File         : EnumFile.h
//   Description  : This file contains the enums required by most of the
//                  classes and qml.
/////////////////////////////////////////////////////////////////////////////
#include <QObject>
#include <QMetaType>

/////////////////////////////////////////////////////////////////////////////
//Class:  EnumClass
//Description:
//      This class contains the enums required by most of the classes and qml.
/////////////////////////////////////////////////////////////////////////////
class EnumClass : public QObject
{
    Q_OBJECT

public:
    //Registers all enum types below to the meta-object system

	Q_ENUMS (SLAVE_MSG_ID_e)
	Q_ENUMS (DYNAMIC_VALIDATION_RESP_e)
	Q_ENUMS (MENU_ICON_e)
	Q_ENUMS (FOOTER_TYPE_e)
	Q_ENUMS (KEYPAD_TYPE_e)
	Q_ENUMS (DOOR_STATUS_e)
    Q_ENUMS (DEVICE_TYPE_e)
    Q_ENUMS (FACE_MASK_DECISION_e)
    Q_ENUMS (INTERNAL_FINGER_READER_e)
    Q_ENUMS (WIFI_SIGNAL_QUALITY_e)


    typedef enum
    {
		//################## MESSAGES FOR QT COMMUNICATION ##################
		MSG_SEND_TO_QT_CMD = 0x6000,				// To Qt
		MSG_FROM_QT_RESP,						// From Qt
	
		//################## EVENTS FROM QT ##################
		GUI_ALNUM_KEEP_ALIVE = 0x7000,			// When user touches at any screen,
		GUI_BUTTON_FIELD_CLEAR,	//01			// Will Be Commonly Used to Clear Any Editable Field
		GUI_BUTTON_KEEP_ALIVE,	//02
		GUI_BUTTON_HOME,		//03
		GUI_BUTTON_BACK,		//04
		
		//PIN Entering screen
		GUI_BUTTON_GOING_IN,	//05
		GUI_BUTTON_GOING_OUT,	//06
		GUI_BUTTON_MENU,		//07
		GUI_BUTTON_PIN_ENTER_SCREEN,//08			// For Entering the PIN : new design
		GUI_BUTTON_ADMIN_MENU_SCREEN,//09   		// For Admin Menu Screen : new design
		GUI_BUTTON_HRD_MENU_SCREEN,    //0A			// For HRD Menu Screen : new design
		
		//Idle screen with 10 IN-OUT Buttons and Main Admin menu button
		GUI_BUTTON_REGULAR_IN,			//0B		// Button for CLOCK IN
		GUI_BUTTON_REGULAR_OUT,			//0C		// Button for CLOCK OUT
		GUI_BUTTON_BREAK_IN,			//0D		// Button for BreakIn
		GUI_BUTTON_BREAK_OUT,			//0E		// Button for breakout
		GUI_BUTTON_SHORT_LEAVE_IN,		//0F		// Button for shortleave in
		GUI_BUTTON_SHORT_LEAVE_OUT,		//10		// Button for shortleave out
		GUI_BUTTON_OFFICIAL_IN,			//11		// Button for offical in
		GUI_BUTTON_OFFICIAL_OUT,		//12		// Button for offical out
		GUI_BUTTON_OVERTIME_IN,  		//13		// Button for overtime in
		GUI_BUTTON_OVERTIME_OUT,		//14		// Button for overtime out
		GUI_BUTTON_PINPAGE_MENU,		//15
		GUI_BUTTON_ADMIN_MENU,			//16	// Button for Admin menu page
		GUI_BUTTON_HRD_MENU,            //17    // Button for Hrd menu page
		
		// Admin main Menu page with Special,Enroll,Settings, and System option
		GUI_BUTTON_SPECIAL_MENU,		//18		// Main Button for HRD
		GUI_BUTTON_LATE_IN_START,		//19		// Button for starting late in
		GUI_BUTTON_LATE_IN_STOP,		//1A		// Button for stopping late in
		GUI_BUTTON_EARLY_OUT_START,		//1B		// Button for starting early out
		GUI_BUTTON_EARLY_OUT_STOP,		//1C		// Button for stopping early out
		
		GUI_BUTTON_ENROLL_MENU,			//1D		// Button for Enrolling process
		GUI_BUTTON_ENROLL_USER,			//1E		// Button for enrolling user
		GUI_BUTTON_ENROLL_SPL_FN,		//1F		// Button for enrolling special cards
		GUI_BUTTON_DEL_CRED,			//20		// Button for deleting credential
		GUI_BUTTON_VIEW_USER_PROFILE,   //21        // Button for  viewing using profile
	
		GUI_BUTTON_NW_MENU,				//22		// Main Button for various settings
		GUI_BUTTON_SET_ETHERNET_CFG,	//23		// Button for LAN settings
		GUI_BUTTON_SET_WIFI_CFG,		//24		// Button for WI-FI settings
		GUI_BUTTON_SEARCH_WIFINW, 	    //25       	// Button For Searching WiFI Networks
		GUI_BUTTON_SHOW_WIFI_ON_SELECT, //26        // Button Event, When Any 1 Network is Selected
		GUI_BUTTON_SET_SERVER_CFG,		//27
		GUI_BUTTON_BROADBAND_CFG,		//28
		
		GUI_BUTTON_SYSTEM_MENU,			//29
		GUI_BUTTON_DOOR_CONTROL_MENU,	//2A
		GUI_BUTTON_DOOR_NORMAL,			//2B
		GUI_BUTTON_DOOR_LOCK,			//2C
		GUI_BUTTON_DOOR_UNLOCK,			//2D
		GUI_BUTTON_CLEAR_ALARM,			//2E
		GUI_BUTTON_SET_DATE_TIME,		//2F
		GUI_BUTTON_CHANGE_PW,			//30		// Button for changing admin Password
	
		//Advanced Configuration Buttons
		GUI_BUTTON_BACKGROUND_CFG,      //31        //Button For Backgrounds Images
		GUI_BUTTON_SYSTEM_STATUS,		//32		//Button For Status Page
		GUI_BUTTON_READ_CFG,			//33		//Button For Read Configuration
		GUI_BUTTON_WRITE_EVENTS,		//34		//Write Event Button
		GUI_BUTTON_FW_UPGRADE,			//35		//Firmware Upgrare Button
		GUI_BUTTON_START_ACTIVITY,		//36		//This Button Will Give Start Command
		GUI_BUTTON_ADMIN_MENU_SETTING,	//37
		GUI_BUTTON_ADMIN_MENU_DOOR_CONTROL,//38
		GUI_BUTTON_ADMIN_MENU_BACKUP_AND_UPDATE,//39
	
		MSG_PIN_VALIDATION_EVENT,			//3A	// For sending PIN data to Application for verification
		MSG_ETHERNET_CFG_VALIDATION_EVENT,	//3B	// For sending LAN Entered Parameter to Application
		MSG_SERVER_CFG_VALIDATION_EVENT,	//3C	// For sending Server Parameter to Application
		MSG_MOBILE_CFG_VALIDATION_EVENT,    //3D    // For sending Broadband Parameter to Application
		MSG_SEND_MODEM_PROFILE_DATA,		//3E	// Used for selected profile view
		MSG_WIFI_CFG_VALIDATION_EVENT,		//3F	// For sending Wireless N/W Parameter to Application
		MSG_ENROL_USER_VALIDATION_EVENT,	//40	// For sending Enrolled User data to Application
		MSG_ENROL_SEARCH_USER_LIST_EVENT,   //41    // For Sending User Name For Search to Appllication
		MSG_ENROL_SEARCH_USER_NAME_FROM_ID,	//42	// For Sending User Name from User Id Entered.
		MSG_ENROL_SPL_FN_VALIDATION_EVENT,	//43	// For sending Enrolled Spl Functin Id to Application
		MSG_DEL_CRED_VALIDATION_EVENT,		//44	// For sending Deleted Credentials data to Application
		MSG_DEL_CRED_SEARCH_USER_LIST_EVENT,//45	// For Sending User Name For Search to Appllication
		MSG_DEL_CRED_SEARCH_USER_NAME_FROM_ID,//46	// For Sending User Name from User Id Entered.
		MSG_DATE_TIME_VALIDATION_EVENT,		//47		// For sending Date Time  data to Application
		MSG_ADMIN_PW_VALIDATION_EVENT,		//48	// For sending Change Admin Password Data to Application
		MSG_HRD_PW_VALIDATION_EVENT,		// 49	// For sending Change HRD Password Data to Application
		MSG_LOGIN_MENU_VALIDATION_EVENT,	//4a	// For Sending Admin/Hrd Login Password Data to Application
		MSG_BACKGROUND_SET_EVENT,			//4b	// For Sending Backgroung Image Index To THe Application
		GUI_SEND_STATRT_UP_INFO,			//4c
		GUI_BUTTON_CAFE_BILL_PAY,			//4d
		GUI_BUTTON_CAFE_SOLDOUT_ITEM, 		//4e
		GUI_BUTTON_CAFE_AVAILABLE_ITEM, 	//4f
		GUI_BUTTON_CLEAR_CAFE_MENU,			//50
		GUI_BUTTON_E_CANTEEN_MENU,        	//51
		GUI_BUTTON_VIEW_BALANCE_MENU,       //52
		GUI_BUTTON_PRINT_LAST_BILL_MENU,    //53
		MSG_CAFE_SOLDOUT_VALIDATION_EVENT,  //54
		MSG_CAFE_AVAILABLE_VALIDATION_EVENT,//55
		GUI_BUTTON_DISPALY_CFG,				//56
		GUI_BUTTON_CLOCK_CFG,				// 57
		MSG_CLOCK_SET_EVENT,				// 58
		GUI_BUTTON_MANAGE_CANTEEN_MENU,		// 59
		MSG_TOUCH_SCREEN_TEST_SUCCESS,		// 5a
		GUI_BUTTON_KEYPAD_CFG,				// 5b
		MSG_KEYPAD_SET_EVENT,				// 5c
		MSG_WIFI_DFLT_CFG_EVENT,            // 5d
		GUI_BUTTON_ABOUT_DEVICE_CONNECTIVITY,		// 5E
		GUI_BUTTON_ABOUT_DEVICE_READERS_INTERFACES,	//5F
		GUI_BUTTON_ABOUT_DEVICE_ALARMS,	//60
		GUI_BUTTON_ZONE_SETTINGS,		//61
		GUI_BUTTON_ALARM_OPTIONS,		//62
		GUI_BUTTON_ACT_DND,				//63
		GUI_BUTTON_DEACT_DND,			//64
		GUI_BUTTON_ACT_DMZ,				//65
		GUI_BUTTON_DEACT_DMZ,			//66
		GUI_BUTTON_ZONE_CONTROL,		//67
		GUI_BUTTON_LOCK_ZONE,			//68
		GUI_BUTTON_UNLOCK_ZONE,			//69
		GUI_BUTTON_NORMALIZE_ZONE,		//6A
		GUI_BUTTON_SET_PANIC_ALARM,		//6B
		GUI_BUTTON_MUTE_BUZZER,			//6C
		GUI_BUTTON_CLEAR_DOOR_AUX_OP,	//6D
		GUI_BUTTON_MUTE_MASTER_BUZZER,	//6E
		GUI_BUTTON_CLEAR_MASTER_OP,		//6F
		GUI_BUTTON_CANCEL,				//70	//PIN Entry Screen
		GUI_BUTTON_WPS_PUSH_BUTTON,		//71
		GUI_BUTTON_WPS_PIN_ENTRY,		//72
		GUI_BUTTON_PASSWORD_MENU,		//73
		GUI_BUTTON_DEFAULT_HRD_PASSWORD,//74
		GUI_BUTTON_DOOR_MODE_CHANGE,	//75
	    GUI_BUTTON_ARM_ZONE,            //76
	    GUI_BUTTON_DISARM_ZONE,         //77
	    GUI_BUTTON_ARM_DOOR,            //78
	    GUI_BUTTON_DISARM_DOOR,         //79
		MSG_WPS_WIFI_CONNECTED,			//7A
		MSG_WAIT_FOR_WPS_PIN,			//7B
		GUI_BUTTON_SELF_ENROLLMENT_START,	//7C
		GUI_BUTTON_SELF_ENROLLMENT_CANCEL,	//7D

        //Job code list event & buttons
        MSG_SEARCH_JOB_CODE_LIST_EVENT,		//7E
        GUI_BUTTON_NEXT_JOB_CODE_LIST,      //7F
        GUI_BUTTON_PREVIOUS_JOB_CODE_LIST,  //80
		MSB_JOB_CODE_SELECTED_EVENT,    	//81

        GUI_BUTTON_DOOR_MODE_IN,            //82
        GUI_BUTTON_DOOR_MODE_OUT,           //83
        MSG_CAFE_ITEM_SELECTION_EVENT,      //84
        GUI_BUTTON_NEXT_CAFE_MENU_LIST,		//85
        GUI_BUTTON_PREVIOUS_CAFE_MENU_LIST,	//86
        GUI_BUTTON_NEXT_MAIN_CAFE_MENU_SCREEN_LIST,     //87
        GUI_BUTTON_PREVIOUS_MAIN_CAFE_MENU_SCREEN_LIST, //88
        GUI_BUTTON_NEXT_CAFE_AVAIL_MENU_LIST,           //89
        GUI_BUTTON_PREVIOUS_CAFE_AVAIL_MENU_LIST,       //8A
        GUI_BUTTON_FTP_FMW_UPGRADE_CANCEL,              //8B
        GUI_BUTTON_FTP_FMW_UPGRADE_OK,                  //8C
        GUI_BUTTON_ADMIN_SYNC_FP_TEMPLATE,              //8D
        GUI_BUTTON_SERVER_MODE_CHANGE,         		   	//8E
        GUI_MSG_ASSIGN_USER_ON_DEVICE_START,            //8F
        GUI_MSG_ASSIGN_USER_ON_DEVICE_CANCEL,           //90
        GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE_START,        //91
        GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE_CANCEL,       //92
        MSG_ENTER_USER_NAME_VALIDATION_EVENT,           //93
        GUI_BUTTON_REFRESH_ATTNDNS_DTL,                 //94
        GUI_MSG_TRANSACTION_WITHOUT_DISCOUNT_START,     //95
        GUI_MSG_TRANSACTION_WITHOUT_DISCOUNT_CANCEL,    //96
        GUI_BUTTON_RELOAD_ATTNDNS_DETAIL_TIMER,         //97
        GUI_BUTTON_DELETE_IDLE_SCREEN_TIMER,            //98
        GUI_BUTTON_APPLY_LEAVE,                         //99

        //Installation Screen related buttons and events
        GUI_BUTTON_CANCEL_INSTALLATION_PROCESS,         //9A
        GUI_BUTTON_SKIP_DYNAMIC_IP_PROCESS,             //9B
        GUI_BUTTON_OK_ASSIGN_STATIC_DEVICE_IP,          //9C
        GUI_BUTTON_OK_ASSIGN_SERVER_URL,                //9D
        GUI_BUTTON_SWITCH_TO_VYOM,                      //9E
        GUI_BUTTON_SWITCH_TO_CENTRA,                    //9F
        GUI_BUTTON_OK_ASSIGN_TENANT_ID,                 //A0

        //Auto System Test related buttons and events
        GUI_BUTTON_AUTO_SYSTEM_TEST_CANCEL,             //A1
        GUI_BUTTON_AUTO_SYSTEM_TEST_START,              //A2

        GUI_BUTTON_TAP_FOR_FR,                          //A3
        GUI_BUTTON_FR_FOOTER_FOR_SPL_FUN,               //A4
        GUI_BUTTON_BRIGHTNESS_CFG,                      //A5
        GUI_BUTTON_ADAPTIVE_FR_RESP,                    //A6
        GUI_SHARED_MEM_STS,                             //A7
        GUI_BUTTON_FACE_ENRL_STOP,                      //A8
        GUI_BUTTON_FACE_ENRL_CONTINUE,                  //A9
        GUI_BUTTON_FACE_ENRL_CANCEL,                    //AA
        GUI_BUTTON_FACE_ENRL_TERMINATE,                 //AB
        GUI_BUTTON_POPUP_SCREEN_TIMEOUT,                //AC
        GUI_BUTTON_FACE_ENRL_CONTINUE_ABORT,            //AD
        GUI_BUTTON_TEMP_USR_ACKNOWLEDGEMENT_RESP,       //AE
        GUI_BUTTON_TEMP_USR_NAME_RESP,                  //AF
        GUI_SCREEN_TOUCH_EVENT,                         //B0
        GUI_STATUSBAR_EXPAND_STATUS,                    //B1
        MSG_AUTH_STATUSBAR_MENU_ACCESS_EVENT,           //B2
        GUI_QR_SCAN_CODE_DETECTED,						//B3


        //################## Message/Command to QT ##################
		GUI_MSG_NOTICE_TEXT = 0x8000,				// Any single interactive message text e.g TIMEOUT
		GUI_MSG_SPL_FUNC_STATUS,		//01		// Screen to be shown with id,name,photo and submit when enrolling new user
		GUI_MSG_SPL_FUNC_DYNAMIC_SCREEN,//02		// Screen to be shown with id,name,photo and submit when enrolling new user
		GUI_MSG_USER_DENIED,			//03		// name,id,photo,+text received+Reason for denial
		GUI_MSG_USER_ALLOWED,			//04		// name,id,photo,+text received+personalized message
		
		GUI_MSG_HRD_VIEW_USERPROFILE_MENU,	//05	// view user profile  menu  screen
		GUI_MSG_PAGE_VIEW_USERPROFILE,		//06	// page showing id-name-designation-branch-dept-bloodg-contact
	
		//Will be Used when Application sends Information for the Menu Pages
		//Below Messages Include Info For the Editable Menu Pages
		GUI_MSG_DISPLAY_IDLE_SCREEN,	//07		// Message to dispaly Idle screen
		GUI_MSG_EHTERNET_CFG_SCREEN,	//08		// Message for setting Ehternet parameters
		GUI_MSG_ACMS_SERVER_CFG_SCREEN,	//09  		// Message for setting Server Port Parameters
		GUI_MSG_WIFI_CFG_SCREEN, 		//0A		// Message for setting wifi parameters
		GUI_MSG_MODEM_CFG_SCREEN,		//0B		// Message for setting Mobile Broadband parameters
		GUI_MSG_SEARCH_WIFI_SCREEN,		//0C
		GUI_MSG_DATE_TIME_SCREEN,	    //0D   		// Change DateTime
		GUI_MSG_CHANGE_PW_SCREEN, 		//0E    	// Change admin pw settings meu screen
		GUI_MSG_ENROLLMENT_SCREEN,		//0F
		GUI_MSG_ENROLL_USER_SCREEN,     //10    	// Enrollment Screen when Enroll card is shown
		GUI_MSG_ENROLL_SPL_FN_SCREEN,   //11   	 	// Special Function Enroll screen when card shown
		GUI_MSG_DEL_CRED_SCREEN, 		//12    	// Delete Credential Screen when card is shown
		GUI_MSG_GOING_IN_SCREEN,		//13
		GUI_MSG_GOING_OUT_SCREEN,		//14
		GUI_MSG_MENU_SCREEN,			//15
		GUI_MSG_PIN_ENTER_SCREEN,       //16	    // PIN Entering screen When Direct Message Comes
		GUI_MSG_MENU_LOGIN_SCREEN,		//17		// Admin/Hrd login Screen When Direct Message Comes
		GUI_MSG_ADMIN_MENU_SCREEN,      //18        // Admin Menu Screen When Direct Message Comes
		GUI_MSG_HRD_MENU_SCREEN,        //19        // Hrd Menu Screen When Direct Message Comes
		GUI_MSG_ADMIN_SETTING_SCREEN,	//1A
		GUI_MSG_ADMIN_DOOR_CONTROL_SCREEN,//1B
		GUI_MSG_ADMIN_BACKUP_AND_UPDATE_SCREEN,//1C
	
		GUI_MSG_STATUSBAR_SCREEN,        	//1D       	// Status Bar Mesg For Notification
		GUI_MSG_BACKGROUND_SCREEN,       	//1E		// Setting Background Screen
		GUI_MSG_MODEM_PROFILE_DATA,		 	//1F		// selected Modem Profile Data
		GUI_MSG_SYS_STATUS_SCREEN,		 	//20		// System Status Page
		GUI_MSG_BACKUP_UPDATE_PROG,		 	//21		// This Message Will Send work done :1 to 100
		GUI_MSG_ENROLL_GET_NAME_FROM_ID, 	//22		// This Message sends Name for Enroll screen from Id
		GUI_MSG_DEL_CRED_GET_NAME_FROM_ID,	//23		// This Message sends Name for Del Cred screen from Id
		GUI_MSG_SEARCHED_ENROLL_USER_LIST,	//24		//TO Send Search User List For Enroll Screen
		GUI_MSG_SEARCHED_DEL_CRED_USER_LIST,//25		//TO Send Search User List For Delete Cred Screen
		GUI_MSG_STARTUP_INFO,	          	//26    	// Start up information
		GUI_MSG_SET_MMC_DEV_PATH,         	//27      	// sending Audio, Video And User Pics file path
		GUI_MSG_FIRMWARE_UPLOAD_PROG,	  	//28		// Display Progress bar,When upgrading from ACMS/Jeeves
		GUI_MSG_DYNAMIC_POP_UP_SCREEN,		//29
		GUI_MSG_MODEM_PROFILE_SCREEN,		// 2a
		GUI_MSG_CAFE_ITEM_LIST_SCREEN,		// 2b
		GUI_MSG_CAFE_NOT_SERVING_SCREEN, 	//2c
		GUI_MSG_CAFE_SOLDOUT_ITEM_SCREEN,   //2d
		GUI_MSG_CAFE_AVAILABLE_ITEM_SCREEN, //2e
		GUI_MSG_CAFE_USER_ALLOWED,			// 2f
		GUI_MSG_E_CANTEEN_MENU_SCREEN,      //30
		GUI_MSG_PRINT_LAST_BILL_SCREEN,		//31
		GUI_MSG_SEND_CONFIG_INFO,			// 32
		GUI_MSG_VALIDATION_RESPONSE,		//33
		GUI_MSG_DISPLAY_MENU_SCREEN,		// 34
		GUI_MSG_BACKGROUND_SELECTION_SCREEN,// 35
		GUI_MSG_CLOCK_SELECTION_SCREEN,		// 36
		GUI_MSG_MANAGE_CANTEEN_SCREEN,		// 37
		
		// System Test Related Screens
		GUI_MSG_TOUCH_PANEL_TEST_SCREEN,	// 38
		GUI_MSG_LCD_TEST_SCREEN,			// 39
		GUI_MSG_SYSTEM_TEST_POPUP_SCREEN,	// 3a
		GUI_MSG_START_GUI_APPLICATION,		// 3b
		GUI_MSG_KEYPAD_SELECTION_SCREEN,	// 3c
		GUI_MSG_CAFE_USER_DENIED,           // 3d
		GUI_MSG_VIEW_USER_PROFILE_SCREEN,	//3e
		GUI_MSG_ZONE_CFG_SCREEN,			//3f
		GUI_MSG_ZONE_CONTROL_SCREEN,		//40
		GUI_MSG_ALARM_OPTIONS_SCREEN,		//41
		GUI_MSG_ABOUT_DEVICE_CONNECTIVITY_SCREEN,		//42
		GUI_MSG_ABOUT_DEVICE_READERS_INTERFACES_SCREEN,	//43
		GUI_MSG_ABOUT_DEVICE_ALARMS_SCREEN,				//44
		GUI_MSG_WPS_PROGRESS_SCREEN,					//45
		GUI_MSG_REFRESH_WALLPAPER,						//46
		GUI_MSG_MENU_PASSWORD_SCREEN,					//47
		
		//Wifi setting Menu related screen
		GUI_MSG_SEARCHING_WIFI_SCREEN,					//48
		GUI_MSG_WPS_PROCESSING_SCREEN,					//49

        //Self enrollment screen
        GUI_MSG_SELF_ENROLLMENT_SCREEN,					//4A

        //Job code list screen & message
        GUI_MSG_JOB_CODE_DISPLAY_SCREEN,                //4B
        GUI_MSG_JOB_CODE_LIST,                          //4C

        GUI_MSG_NET_CONN_NOT_AVAILABLE_SCREEN,          //4D
        GUI_MSG_DOOR_MODE_SELECTION_SCREEN,             //4E
        GUI_MSG_CAFE_ITEM_SELECTION,        			//4F
        GUI_MSG_CAFE_LIST_SCREEN_UPDATE,                //50
        GUI_MSG_MILTILANG_FONT_LOAD,                    //51
        GUI_MSG_MULTILANG_FLAG,                         //52
        GUI_MSG_INOUT_SPCL_FUNCTION_SELECTION,          //53
        GUI_MSG_DATABASE_CORRUPTION,                    //54
        GUI_MSG_INIT_FIRMWARE_UPGRADE,                  //55
        GUI_MSG_ASSIGN_USER_ON_DEVICE,					//56
        GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE,				//57
        GUI_MSG_ENTER_USER_NAME,                        //58
        GUI_MSG_WIFI_ENABLE_DISABLE_FLAG,               //59
        GUI_MSG_SHOW_USER_ATTENDANCE_DETAIL,			//5A
        GUI_MSG_TRANSACTION_WITHOUT_DISCOUNT,           //5B
        GUI_MSG_CAFE_POLICY_USER_DENIED,                //5C
		GUI_MSG_APPLY_LEAVE_RESPONSE,                   //5D

        //Device Installation related screen
        GUI_MSG_INSTALLATION_SCREEN_DEVICE_IP,			//5E
        GUI_MSG_INSTALLATION_SCREEN_SERVER_IP,			//5F
        GUI_MSG_INSTALLATION_SCREEN_VYOM_TENANT_ID,		//60

        //Auto System Test related screen
        GUI_MSG_START_AUTO_SYSTEM_TEST_SCREEN,			//61
        GUI_MSG_AUTO_SYSTEM_TEST_RUNNING_SCREEN,		//62

		GUI_MSG_RELOAD_IMAGE,							//63
        GUI_MSG_FR_SERV_ENABLE,                         //64
        GUI_MSG_FR_STREAM_STATUS,                       //65
        GUI_MSG_FR_CAMERA_CONNECTION_FAILED,            //66
        GUI_MSG_DISPLAY_IDLE_SCREEN_FOR_FR_STREAMING,   //67
        GUI_MSG_BRIGHTNESS_SELECTION_SCREEN,            //68
        GUI_MSG_BRIGHTNESS_VALIDATION_EVENT,			//69
        GUI_MSG_BRIGHTNEES_SET_FROM_SPIN_BOX,           //6A
        GUI_MSG_FR_CAMERA_CONNECTION_FAILED_INFINITE,   //6B

        GUI_MSG_USER_ALLOWED_COMMON_SCREEN,				//6C common msg for allow and denied as per Req FP15
        GUI_MSG_USER_DENIED_COMMON_SCREEN,				//6D common msg for allow and denied as per Req FP15
        GUI_MSG_CAFE_USER_ALLOWED_COMMON_SCREEN,		//6E common msg for cafe allow and denied as per Req FP15
        GUI_MSG_CAFE_USER_DENIED_COMMON_SCREEN,			//6F common msg for cafe allow and denied as per Req FP15
        GUI_MSG_FR_BUSY_SIGNAL,                         //70
        GUI_MSG_ADAPTIVE_FR_SCREEN,                     //71
        GUI_MSG_APPL_MODE_INFO,							//72
        GUI_MSG_DYNAMIC_TEMPERATURE_SCREEN,             //73
        GUI_BUTTON_TEMPERATURE,							//74
        GUI_MSG_TEMPERATURE_MENU_SCREEN,				//75
        GUI_MSG_TEMPERATURE_REFRESH,                    //76
        GUI_MSG_AMBIENT_TEMP_DETECTING_SCREEN,          //77
        GUI_MSG_AMBIENT_COUNTDOWN_COMPLETED,            //78
        GUI_MSG_DYNAMIC_USER_INFO_SCREEN,				//79
        GUI_MSG_FRAME_RATE,                             //7A
        GUI_MSG_INIT_RGB_SHARED_MEMORY,                 //7B
        GUI_MSG_INIT_IR_SHARED_MEMORY,                  //7C
        GUI_MSG_DESTROY_RGB_SHARED_MEMORY,              //7D
        GUI_MSG_DESTROY_IR_SHARED_MEMORY,               //7E
        GUI_MSG_LOG_INFO,								//7F
        GUI_MSG_FACE_DETECTING_WAIT_SCREEN,				//80
        GUI_MSG_FACE_DETECTING_COUNTDOWN_COMPLETED,		//81
        GUI_MSG_FACE_ENROLL_PROCESSING_SCREEN,			//82
        GUI_MSG_FACE_ENROLL_USER_POPUP_SCREEN,			//83
        GUI_MSG_SET_OVERLAY_TEXT_ON_STREAM,				//84
        GUI_MSG_FR_STREAM_FOR_SPECIAL_FUNCTOIN,			//85
        GUI_MSG_FOR_MULTI_ACCESS,  				//86
        GUI_MSG_TEMP_USER_CREATE_CONFIRM_SCREEN,		//87
        GUI_MSG_TEMP_USER_ENTER_NAME_SCREEN,			//88
        GUI_MSG_TEMP_USER_POPUP_SCREEN,                         //89
        GUI_MSG_TEMP_USER_REGISTERED_SUCCESSFULLY,              //8A
        GUI_MSG_WIFI_SIGNAL_STRENGTH,                           //8B
        GUI_MSG_DEVICE_INACTIVE,                                //8C
        GUI_MSG_MAX_SCREEN,

		NO_MSG = 0xFFFF							// No message to send- used between comm task and other applications
	}SLAVE_MSG_ID_e;

    typedef enum
    {
        VALIDATION_RESP_NONE = 0,					//0
        VALIDATION_RESP_INVALID_PASWORD,            //1
        VALIDATION_RESP_INVALID_IP,                 //2
        VALIDATION_RESP_INVALID_SUBNET,             //3
        VALIDATION_RESP_INVALID_GATEWAY,            //4
        VALIDATION_RESP_INVALID_PDNS,               //5
        VALIDATION_RESP_INVALID_ADNS,               //6
        VALIDATION_RESP_INVALID_PORT,               //7
        VALIDATION_RESP_INVALID_DNS,                //8
        VALIDATION_RESP_WRONG_CURR_PSWD,            //9
        VALIDATION_RESP_PSWD_MISSMATCH,             //10
        VALIDATION_RESP_PSWD_BLANK,                 //11
        VALIDATION_RESP_INVALID_SERVER_IP_ETH,      //12
        VALIDATION_RESP_INVALID_SERVER_IP_WIFI,     //13
        VALIDATION_RESP_INVALID_SERVER_IP_MODEM,    //14
        VALIDATION_RESP_INVALID_SERVER_PORT_ETH,	//15
        VALIDATION_RESP_INVALID_SERVER_PORT_WIFI,	//16
        VALIDATION_RESP_INVALID_SERVER_PORT_MODEM,	//17
		VALIDATION_RESP_CHARS_NOT_SUPPORTED,		//18
        VALIDATION_RESP_BLANK_SERVER_URL,			//19
        VALIDATION_RESP_BLANK_SERVER_PORT,			//20
        VALIDATION_RESP_INVALID_SERVER_PORT,		//21
        VALIDATION_RESP_BLANK_SERVER_TENANT_ID,		//22
        VALIDATION_RESP_INVALID_SERVER_TENANT_ID,	//23
        VALIDATION_RESP_BLANK_SLEEP_TIMER_VALUE,    //24
        VALIDATION_RESP_INVALID_SLEEP_TIMER_VALUE,	//25
        MAX_VALIDATION_RESP
    }DYNAMIC_VALIDATION_RESP_e;

    typedef enum
    {
        ICON_NONE = 0,                              //0
        ICON_ADMIN,                                 //1
        ICON_HRD,                                   //2
        ICON_ABOUT_DEVICE,                          //3
        ICON_E_CANTEEN,                             //4
        ICON_SETTINGS,                              //5
        ICON_DOOR_CONTROL,                          //6
        ICON_ENROLLMENT,                            //7
        ICON_LATE_IN_START,                         //8
        ICON_EARLY_OUT_START,                       //9
        ICON_LATE_IN_STOP,                          //10
        ICON_EARLY_OUT_STOP,                        //11
        ICON_BACKUP_AND_UPDATE,                     //12
        ICON_CLEAR_ALARM,                           //13
        ICON_DATE_AND_TIME,                         //14
        ICON_DISPLAY,                               //15
        ICON_SERVER,                                //16
        ICON_LAN,                                   //17
        ICON_WI_FI,                                 //18
        ICON_MODEM,                                 //19
        ICON_PASSWORD,                              //20
        ICON_SET_WALL_PAPER,                        //21
        ICON_SELECT_CLOCK,                          //22
        ICON_SELECT_KEYPAD,                         //23
        ICON_LOCK_DOOR,                             //24
        ICON_UNLOCK_DOOR,                           //25
        ICON_NORMALIZE_DOOR,                        //26
        ICON_ENROLL_USER,                           //27
        ICON_ENROLL_SPECIAL_CARDS,                  //28
        ICON_DELETE_CREDENTIALS,                    //29
        ICON_RESTORE_CONFIGURATIONS,                //30
        ICON_WRITE_EVENTS,                          //31
        ICON_FIRMWARE_UPGRADE,                      //32
        ICON_MANAGE_E_CANTEEN,                      //33
        ICON_VIEW_BALANCE,                          //34
        ICON_PRINT_LAST_BILL,                       //35
        ICON_ITEM_SOLD_OUT,                         //36
        ICON_ITEM_AVAILABLE,                        //37
        ICON_OFFICIAL_WORK_IN,                      //38
        ICON_OFFICIAL_WORK_OUT,                     //39
        ICON_SHORT_LEAVE_IN,                        //40
        ICON_SHORT_LEAVE_OUT,                       //41
        ICON_REGULAR_IN,                            //42
        ICON_REGULAR_OUT,                           //43
        ICON_BREAK_IN,                              //44
        ICON_BREAK_OUT,                             //45
        ICON_OVERTIME_IN,                           //46
        ICON_OVERTIME_OUT,                          //47
        ICON_DOOR_LOCK,                             //48
        ICON_DOOR_UNLOCK,                           //49
        ICON_DOOR_NORMAL,                           //50
        ICON_GOING_IN_REGULAR,                      //51
        ICON_GOING_IN_OFFICIAL,                     //52
        ICON_GOING_IN_POST_BREAK,                   //53
        ICON_GOING_IN_OVER_TIME,                    //54
        ICON_GOING_IN_SHORT_LEAVE,                  //55
        ICON_GOING_OUT_REGULAR,                     //56
        ICON_GOING_OUT_OFFICIAL,                    //57
        ICON_GOING_OUT_POST_BREAK,                  //58
        ICON_GOING_OUT_OVER_TIME,                   //59
        ICON_GOING_OUT_SHORT_LEAVE,                 //60
        ICON_ENROLL_READ_ONLY_CARD,                 //61
        ICON_ENROLL_SMART_CARD,                     //62
        ICON_ENROLL_FINGER_ONLY,                    //63
        ICON_ENROLL_FP_THEN_CARD,                   //64
        ICON_USER_PROFILES,                         //65
        ICON_ZONE_SETTINGS,                         //66
        ICON_ALARM_OPTIONS,                         //67
        ICON_ACT_DND,                         //68
        ICON_DEACT_DND,                       //69
        ICON_ACT_DMZ,                         //70
        ICON_DEACT_DMZ,                       //71
        ICON_ZONE_CONTROL,                    //72
        ICON_ZONE_LOCK,                       //73
        ICON_ZONE_UNLOCK,                     //74
        ICON_ZONE_NORMAL,                      //75
        ICON_SET_PANIC_ALARM,                  //76
        ICON_MUTE_DOOR_BUZZER,                 //77
        ICON_CLEAR_DOOR_AUX_OP,                 //78
        ICON_MUTE_MASTER_BUZZEER,               //79
        ICON_CLEAR_MASTER_OP,                //80
        ICON_LOCK_ZONE,                       //81
        ICON_UNLOCK_ZONE,                     //82
        ICON_NORMALIZE_ZONE,                  //83
        ICON_CHANGE_PASSWORD,                 //84
        ICON_DEFAULT_HRD_PASSWORD,            //85
        ICON_DOOR_MODE_CHANGE,                 //86
        ICON_ZONE_ARM,                         //87
        ICON_ZONE_DISARM,                      //88
        ICON_DOOR_ARM,                         //89
        ICON_DOOR_DISARM,                      //90
        ICON_DOOR_MODE_IN,                     //91
        ICON_DOOR_MODE_OUT,                     //92
        ICON_GOING_IN_OFFICIAL_SPLFUNC,         //93
        ICON_GOING_OUT_OFFICIAL_SPLFUNC,        //94
        ICON_GOING_IN_SHORT_LEAVE_SPLFUNC,      //95
        ICON_GOING_OUT_SHORT_LEAVE_SPLFUNC ,    //96
        ICON_GOING_IN_REGULAR_SPLFUNC,          //97
        ICON_GOING_OUT_REGULAR_SPLFUNC,         //98

        ICON_GOING_IN_POST_BREAK_SPLFUNC,       //99
        ICON_GOING_OUT_POST_BREAK_SPLFUNC,      //100
        ICON_GOING_IN_OVER_TIME_SPLFUNC,        //101
        ICON_GOING_OUT_OVER_TIME_SPLFUNC,       //102
        ICON_BRIGHTNESS_SETTINGS,               //103

        ICON_TEMPERATURE,                        //104
        ICON_ENROLL_FACE,						//105
        ICON_ENROLL_DURESS_FP,					//106
    }MENU_ICON_e;

    typedef enum
    {
        FOOTER_NONE = 0,                            //0
        FOOTER_IDLE,                                //1
        FOOTER_HOME,                                //2
        FOOTER_PIN,                                 //3
        FOOTER_BACK_HOME,                           //4
        FOOTER_BACK_PIN_HOME,                       //5
        FOOTER_BACK_PIN_HOME_FACE,                  //6
        FOOTER_PIN_FACE,                            //7
        FOOTER_PIN_CENTER,                          //8
        FOOTER_ABORT_STOP_DISABLE,                  //9
        FOOTER_ABORT_STOP_ENABLE,                   //10
    }FOOTER_TYPE_e;

    typedef enum
    {
        KEYPAD_NONE = 0,                            //0
        KEYPAD_ALPHANUMERIC,                        //1
        KEYPAD_NUMERIC,                             //2
        KEYPAD_DIALPAD                              //3
    }KEYPAD_TYPE_e;

    typedef enum
    {
        DOOR_ONLINE = 0,
        DOOR_NWFAULT,
        DOOR_DEGRADEMODE
    }DOOR_STATUS_e;

    typedef enum
    {
        DEVICE_TYPE_ARGO = 0,
        DEVICE_TYPE_VEGA,
        DEVICE_TYPE_FMX
    }DEVICE_TYPE_e;

    typedef enum
    {
        FACE_MASK_DECISION_ACCESS_ALLOWED = 0,
        FACE_MASK_DECISION_ACCESS_ALLOWED_SOFT,
        FACE_MASK_DECISION_ACCESS_DENIED_NOT_DETECTED,
        FACE_MASK_DECISION_MAX,
    }FACE_MASK_DECISION_e;

    typedef enum
    {
        INTERNAL_FP_NOT_SUPPORTED = 0,
        INTERNAL_FP_SUPPORTED,
        MAX_INTERNAL_FP,
    }INTERNAL_FINGER_READER_e;

    typedef enum
    {
        NO_WIFI_SIGNAL,
        WEAK_WIFI_SIGNAL,
        MODERATE_WIFI_SIGNAL,
        STRONG_WIFI_SIGNAL,
        STRONGEST_WIFI_SIGNAL,
        MAX_WIFI_SIGNAL_QUALITY
    }WIFI_SIGNAL_QUALITY_e;

    //constructor
    EnumClass(){}

    ~EnumClass(){}          //destructor
};
#endif // ENUMFILE_H
