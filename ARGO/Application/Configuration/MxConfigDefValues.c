///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Comsec Pvt. Ltd., Baroda, India.
//   Project      : Access Control System
//   Created By   : Kuldip Gor
//   File         : MxConfigDefValues.c
//   Description  : This file declares default values of all configuration
//					parameters.
//
/////////////////////////////////////////////////////////////////////////////

//******** Include files *******
#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxConfigOld.h"
#include "MxSysCommonDef.h"
#include "MxMenu.h"

//******** Extern Variables **********


//******** Defines and Data Types ****
#define DEFAULT_PASSWORD_USER_ADMIN					""
#define DEFAULT_PASSWORD_USER_HRD					""
#define DEFAULT_PASSWORD_USER_ADMIN_VER_1			"1234"
#define	DEFAULT_PASSWORD_USER_HRD_VER_1				"1111"
//******** Function Prototypes *******


//******** Global Variables **********

// Network Time Protocol Address
const CHARPTR NtpAddress[MAX_NTP_ADDR] =
{
	"ntp1.cs.wisc.edu",
	"time.windows.com",
	"time.nist.gov"
};

const SYS_VAR_t	DefltSysVar =
{
	.backgroundImage 	= 0,
	.currentClock 		= 0,
	.dstState 			= DST_STATE_NORMAL,
	.currentKeypad 		= KEYPAD_DIALPAD,
	.doorLockState		= DOOR_LOCK_NORMAL,
};

// SYSTEM PASSWORDS
const PASSWORD_t DfltPassword =
{
	.displayPswd =
	{
		DEFAULT_PASSWORD_USER_ADMIN,	// Display Admin password
		DEFAULT_PASSWORD_USER_HRD,	// Display HRD password
	},
	.webPswd = 
	{
		DEFAULT_PASSWORD_USER_ADMIN,	// Web Admin password
		DEFAULT_PASSWORD_USER_HRD,	// Web HRD password
	},
	.PswdResetDaysCount = {0,0},		// Password expiry days
    .DispPwdAttemptCount = {MAX_DISP_PWD_ATTEMPT_COUNT, MAX_DISP_PWD_ATTEMPT_COUNT},
    .DispPwdLoginRestrictTimer = {INVALID_TIMER_HANDLE, INVALID_TIMER_HANDLE},
};

// SYSTEM PASSWORDS
const PASSWORD_t DfltPasswordVer1 =
{
	.displayPswd =
	{
		DEFAULT_PASSWORD_USER_ADMIN_VER_1,	// Display Old Admin password
		DEFAULT_PASSWORD_USER_HRD_VER_1,	// Display Old HRD password
	},
	.webPswd =
	{
		DEFAULT_PASSWORD_USER_ADMIN_VER_1,	// Web Admin password
		DEFAULT_PASSWORD_USER_HRD_VER_1,	// Web HRD password
	},
	.PswdResetDaysCount = {0,0},			// Password expiry days
};

// USER CONFIGURATION
const  USER_CONFIG_t DfltUserConfig =
{
	.enable				= DISABLED,				// Working status
	.userId				= INVALID_USER_ID,		// ID (8 digits)
	.name				= "",					// Name
	.alphaNumId			= "",					// Alphanumeric User ID
	.vip				= NO,					// VIP status
	.bypassFinger		= NO,					// Bypass finger print status
	.cardId				= {0,0},				// Maximum cards associated
	.pinCode			= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},		// PIN code
	.visitorIdx			= INVALID_VISITOR_IDX,	// Index of visitor in visitor table
	.terminationEnable	= DISABLED,				// Termination date status
	.terminationDate 	= {1,0,9},				// Termination date(DD:MM:YY)
	.holidaySchId		= INVALID_SCHEDULE_ID,	// Holiday schedule ID
	.absentee			= {DISABLED,60},		// Absentee Rule Information
	.restrictAccess		= FALSE,				// Restrict access enable/disable
	.allowedForCafe 	= DISABLED,					// Allowed For Cafeteria
	.discountLevelCafe	= CAFE_DISCNT_LEVEL_NONE,	// DiscountLevel For Cafeteria
    .paymentModeForCafe = PREPAID_DEVICE_SIDE,		// Payment Mode For Cafeteria
    .blockForCafe		= NOT_BLOCKED,					// Blocked For Cafeteria
    .smartRouteId		= USER_SMART_ROUTE_NONE,	// Smart Route ID
    .smartRouteLevel 	= MAX_SMART_ROUTED_LEVEL,	// User Access Level
    .selfEnrlEnbl		= DISABLED,
    .selfEnrlSkipedCnt	= 0,
    .jobType			= JCM_NONE,
	.checkBalanceBeforeTxnF = FALSE,
	.userGrpId			= INVALID_USER_GRP,
	.cafeteriaPolicyCheckF = FALSE,
	.showAttndcDtlF		= ENABLED,
	.birthdayMsgEnable	= FALSE,
	.dateOfBirth		= {1,0,9},
	.enableFaceRecoF	= DISABLED,
	.bypassFaceF		= DISABLED,
	.offlineCafeTransactionType = CAFE_OFFLINE_TRANSACTION_NOT_ALLOWED,			// Allowed offline cafeteria transaction
	.consumedItemQuantity = {[0 ... (MAX_CAFE_ITEM_CODE-1)] = 0},
	.DuressFp = 
	{
	        DURESS_FP_IDX_NONE,
	        DURESS_FP_IDX_NONE,
	        DURESS_FP_IDX_NONE,
	        DURESS_FP_IDX_NONE,
	},
	.panelDoorAssignF 	= DISABLED,
	.profilePhotoSyncPendingF = DISABLED,
    	.faceCredData = {
                            .faceCountInServer =    0,
                            .getFacePokeCount = 0,
                            .setFacePokeCount = 0,
                            .setFaceIndex = {[0 ... (SET_MAX_USER_FACE_INDEX-1)] = 0},
                            .lastFaceEnrollTime = 0,
                            .lastFaceReceivetime = 0,
                    	},
};

// USER CONFIGURATION
const  USER_CONFIG_t DfltTestUserConfig =
{
	.enable				= ENABLED,				// Working status
	.userId				= 1,					// ID (8 digits)
	.name				= "Matrix",					// Name
	.alphaNumId			= "1",					// Alphanumeric User ID
	.vip				= NO,					// VIP status
	.bypassFinger		= NO,					// Bypass finger print status
	.cardId				= {0,0},				// Maximum cards associated
	.pinCode			= "1",					// PIN code
	.visitorIdx			= INVALID_VISITOR_IDX,	// Index of visitor in visitor table
	.terminationEnable	= DISABLED,				// Termination date status
	.terminationDate 	= {1,0,9},				// Termination date(DD:MM:YY)
	.holidaySchId		= INVALID_SCHEDULE_ID,	// Holiday schedule ID
	.absentee			= {DISABLED,60},		// Absentee Rule Information
	.restrictAccess		= FALSE,				// Restrict access enable/disable
	.allowedForCafe 	= DISABLED,					// Allowed For Cafeteria
	.discountLevelCafe	= CAFE_DISCNT_LEVEL_NONE,	// DiscountLevel For Cafeteria
    .paymentModeForCafe = PREPAID_DEVICE_SIDE,		// Payment Mode For Cafeteria
    .blockForCafe		= NOT_BLOCKED,				// Blocked For Cafeteria
    .smartRouteId		= USER_SMART_ROUTE_NONE,	// Smart Route ID
    .smartRouteLevel 	= MAX_SMART_ROUTED_LEVEL,	// User Access Level
	.selfEnrlEnbl		= DISABLED,
    .selfEnrlSkipedCnt	= 0,
    .jobType			= JCM_NONE,
	.checkBalanceBeforeTxnF = FALSE,
	.userGrpId			= INVALID_USER_GRP,
	.cafeteriaPolicyCheckF = FALSE,
	.showAttndcDtlF		= ENABLED,
	.offlineCafeTransactionType = CAFE_OFFLINE_TRANSACTION_NOT_ALLOWED,			// Allowed offline cafeteria transaction
	.consumedItemQuantity = {[0 ... (MAX_CAFE_ITEM_CODE-1)] = 0},
	.panelDoorAssignF 	= DISABLED,
	.profilePhotoSyncPendingF = DISABLED,
    	.faceCredData = {
                            .faceCountInServer =    0,
                            .getFacePokeCount = 0,
                            .setFacePokeCount = 0,
                            .setFaceIndex = {[0 ... (SET_MAX_USER_FACE_INDEX-1)] = 0},
                            .lastFaceEnrollTime = 0,
                            .lastFaceReceivetime = 0,
                    	},
};

// BASIC SYSTEM PARAMETERS
const SYS_BASIC_PARA_t  DfltBasicSysPara =
{
	.devId				= 0,					// Device ID
	.name				= "",					// Name of the door
	.timeFormat			= TIME_24_HOUR,			// Time format
	.doorAccessMode		= ANY_CREDENTIAL,		// Access mode
	.application		= ACCESS_CTRL,			// Application (AC + TA or TA only)
	.exitOnLock			= ENABLED,				// Allow to exit when door locked
	.tamperAlarm		= DISABLED,				// Tamper Alarm status
	.doorSense			= 	{
								DISABLED,
								DISABLED,
								NORMAL_CLOSE
							},
	.doorPulseTime		= 5,					// Pulse time (Sec)
	.autoRelock			= DISABLED,				// Auto Relock status
	.userDefNtp			= FALSE,				// User Defined NTP Address enable
	.preDefNtpAddr		= "ntp1.cs.wisc.edu",	// Predefined NTP Address
	.userDefNtpAddr		= "",					// User Defined NTP Address
	.utz				= 48,					// Universal time zone(INDIA=48)
	.timeUpdateMode		= MANUAL_TIME,			// Time Mode
	.workingHrStart		= {0,0},				// Start Time
	.workingHrStop		= {23,59},				// End Time
	.workingDays		= {YES, YES, YES, YES, YES, YES, YES},	// Active Working Day
	.holidayWorking		= YES,					// Active Holiday
	.reader				= 	{
								{SM_PROX_READER,ENTRY_READER},		// Internal or External Reader 1
								{SUP_FP_3030_READER,ENTRY_READER},	// Internal or External Reader 2
								{NO_READER_PRESENT,EXIT_READER},		// Internal or External Reader 1
								{NO_READER_PRESENT,EXIT_READER},	// Internal or External Reader 2
								{NO_READER_PRESENT,EXIT_READER},	// READER5
							},
	.requestExitSw		= ENABLED,				// Request to Exit Switch status
	.buzMuteF           = DISABLED,             // Buzzer Mute Option Flag
	.tagRedetectDelay  	= DEFLT_UHF_TAG_REDETECT_TIME,
	.cdcExitReaderF		= FALSE,								// Exit reader type (Old COSEC type, Path type)
	.cerPresentF		= FALSE,
	.extRdrAccessMode   = ANY_CREDENTIAL,		// Access mode
	.degradeModeEnable	= DISABLED,
	.degradeWaitTimer	= 5,					//time to start degrade mode
	.doorModeSchdlEnblF	= DISABLED,
	.entryModeWallpaperF= DISABLED,
	.exitModeWallpaperF	= DISABLED,
	.fprTemplateFormat	= SUPREMA_NATIVE_FORMAT,
	.extWiegandRdrMode	= WIGND_RDR_MODE_INPUT,
	.wiegandOutFormat 	= WIGND_OUT_FRMT_26_BIT,
	.wiegandOutDir		= WIGND_OUT_DIR_MSB_FIRST,
	.waitForAccessFeedback = FALSE,
	.accessFeedbackWaitTime = 2,
	.doorModeSelectEnblF = FALSE,
	.waitForUsrVerification = FALSE,
	.wgndOutCstmFrmtNo		= {0, 0, 0, 0, 0, 0},
	.wgndOutEventSpclCode	= {0, 0, 0, 0, 0, 0},
	.autoRelockTime			= 3,				// Auto Relock Time
	.IoLinkNwInterface		= ETHERNET_PORT,
	.apiAccessAllowEnable	= DISABLED,
	.apiEntryAccessMode = ACCESS_MODE_QR_CODE,
	.apiExitAccessMode 	= ACCESS_MODE_QR_CODE,
	.apiSecurityKey		= "",
	.accessAllowCadDuration	= CADENCE_LONG,
	.visitorDoorAccessMode   = ANY_CREDENTIAL,		// Access mode
	.visitorExtRdrAccessMode = ANY_CREDENTIAL,		// Access mode
	.pinChangeCode			 = "",
	.promptSpFnF			= FALSE,
	.generateEventF 		= TRUE,
	.cbrPresentF			= FALSE,
	.accessAllowScreenDuration = 3000,
	.accessDenyCadDuration = CADENCE_LONG,
	.accessDenyScreenDuration = 3000,
	.abnormalAlarm = DISABLED,
	.forceOpenAlarm = DISABLED,
	.faultAlarm = DISABLED,
	.panicAlarm = DISABLED,
	.cafeteriaFrAccessMode = CAFE_FR_ACCESS_MODE_NONE,
	.tempThresholdAlarm		= INACTIVE,
	.faceMaskNotDetectedAlarm	= INACTIVE,
	.atomRD100PresentF			= FALSE,
	.atomRD200PresentF			= FALSE,
	.atomRD300PresentF			= FALSE,
	.degradeModeType			= BASIC_DEGRADE_MODE_TYPE,
	.doorPulseTimeFloat			= 5.0,
	.accessScheduleID			= 0, // Device access schedule ID
	.accessScheduleEnableF			= FALSE, //Status of Assigned Schedule
	.hideMenuBarForAccessScheduleF		= FALSE,

    .httpWebServerConfig.httpSyncConfig.httpEncryptionModeF = DISABLED,		// 0=inactive, 1=active
    .httpWebServerConfig.httpSyncConfig.interfaceSelctionF=DISABLED,		// 0=Auto, 1=Manual
    .httpWebServerConfig.httpSyncConfig.httpInterface=ETHERNET_PORT,
    .httpWebServerConfig.httpSyncConfig.httpServerIpAddress= "",		// Server ip(16 digits)
    .httpWebServerConfig.httpSyncConfig.httpServerPort = DFLT_HTTP_SERVER_PORT,
    .httpWebServerConfig.httpSyncConfig.httpServerDirectory = "cosec",
    .httpWebServerConfig.httpSyncConfig.httpServerHostName = "",
    .httpWebServerConfig.httpSyncConfig.httpServerUserId	= "MatrixAdmin",
    .httpWebServerConfig.httpSyncConfig.httpServerPassword	= "admin",

    .httpWebServerConfig.profilePicHttpSyncConfig.httpEncryptionModeF = DISABLED,		// 0=inactive, 1=active
    .httpWebServerConfig.profilePicHttpSyncConfig.interfaceSelctionF=DISABLED,		// 0=Auto, 1=Manual
    .httpWebServerConfig.profilePicHttpSyncConfig.httpInterface=ETHERNET_PORT,
    .httpWebServerConfig.profilePicHttpSyncConfig.httpServerIpAddress= "",		// Server ip(16 digits)
    .httpWebServerConfig.profilePicHttpSyncConfig.httpServerPort = DFLT_HTTP_SERVER_PORT,
    .httpWebServerConfig.profilePicHttpSyncConfig.httpServerDirectory = "cosec",
    .httpWebServerConfig.profilePicHttpSyncConfig.httpServerHostName = "",
    .httpWebServerConfig.profilePicHttpSyncConfig.httpServerUserId	= "MatrixAdmin",
    .httpWebServerConfig.profilePicHttpSyncConfig.httpServerPassword	= "admin",

    .httpWebServerConfig.httpSyncRequestRetryTimer	= DFLT_HTTP_SYNC_REQUEST_RETRY_TIMER,

};

// ADVANCE SYSTEM PARAMETERS
const SYS_ADVANCE_PARA_t  DfltSysAdvPara =
{
	.absenteeRuleEnable			= 	DISABLED,							// Absentee rule enable/disable
	.addlSecurity				= 	{DISABLED,0},						// Additional Security Code
	.useCount					= 	{DISABLED,5},						// Use Count
	.firstIn					= 	{DISABLED,0},						// First In User Rule
	.twoPerson					= 	{DISABLED,1,TWO_PERSON_GRP_NONE},	// 2 Person Rule
	.oldOccupancyCtrl			= 	{DISABLED,9},   					// Occupancy Control rule
	.oldAntiPassBack			= 	{DISABLED,DISABLED,NO,YES},		// Anti Pass Back Rule
	.auxInput					= 	{DISABLED,NO,NORMAL_OPEN,3},		// Alarm Input
	.auxOutput					= 	{DISABLED},						// Aux output
	.enrollmentEnable			= 	DISABLED,							// Enrollment Enable
	.enrollmentType				= 	ENROLL_BIOMETRIC_ONLY,			// Enrollment type
	.fpCount					= 	2,								// No of Finger enroll through Special Function
	.cardCount					= 	CARD2,							// No Card enroll through Special Function
	.maxFingerPerUser			= 	2,								// Maximum Finger per user
	.templatesPerFinger			= 	ONE_TEMPLATE,						// Template per Finger
	.twoPersonTimer				= 	5,								// 2 Person Wait Timer
	.interDigitTimer			= 	5,								// Inter Digit Wait Timer
	.multiAccessTimer			= 	5,								// Multi Access Wait Timer
	.lateEarlyTimer				= 	60,								// Special Function Wait Timer
	.genExitSwEvent				= 	DISABLED,							// Generate Exit switch event
	.genInvalidUserEvent		= 	DISABLED,							// Generate Invalid user event
	.SSAConfig					= 	{DISABLED, CARD_ONLY, 8},			// Default SSA Config
	.dvrIntigrationEnbl			= 	DISABLED,
	.dvrIpAddr					= 	"192.168.1.123",
	.dvrPort					= 	8000,
	.dvrInterface				= 	ETHERNET_PORT,
	.smartRouteAccessEnbl		= 	DISABLED,
	.enblGreetingsF				= 	DISABLED,
	.greetingConfig				=
								{
									{
										{0,0},
										{11,59},
										"Good Morning"
									},
									{
										{12,0},
										{15,59},
										"Good Afternoon"
									},
									{
										{16,0},
										{20,59},
										"Good Evening"
									},
									{
										{21,0},
										{23,59},
										"Good Night"
									}
								},
	.twoPersonMode				= 	PRIMARY_MUST,						// Two person mode
	.autoAlarmAck				= 	DISABLED,							// Auto Amlarm Acknowldge Enable
	.autoAlarmAckTimer			= 	10,								// Timer to Acknowldge automatically
	.mifareCustomKeyEnbl		= 	DISABLED,							// mifare custom key disabled
	.mifareCustomKey			= 	{0,0,0,0,0,0},					// default mifare custom key
	.hidIclasCustomKeyEnbl		= 	DISABLED,							// hid icalss custom key disabled
	.hidIclasCustomKey			= 	{0,0,0,0,0,0,0,0},				// default hid iclass custom key
	.customKeyAutoUpdateF		= 	FALSE,							// custom key auto update disabled
	.smartRouteOrder			= 	LEVEL_1_IS_LOWEST,				// Smart card based route access type is level 1 as lowest level
	.verifyAccessPolicyOnExitF 	= 	DISABLED,						// Access Policy Check On Exit
	.facilityCode				= 	1,								// Facilty Code// Type of door lock
	.lockType					= 	EM,								// lock type
	.acceptExtIOLinkF			= 	DISABLED,							// Accept External IO Link request
	.outputWaitTime				= 	0,								// Output wait time
	.webEncryptionModeF			= 	DISABLED,							// Encryption mode
	.selfEnrlEnbl				= 	DISABLED,
	.selfEnrlRetryCnt			= 	5,
	.vamEnable					= 	DISABLED,
	.occupancyCtrl				= 	{DISABLED,9,1,ENABLED},
	.dvrAllowEventuserType		= 	ALL_USERS,
	.dvrDenyEventUserType		= 	ALL_USERS,
	.jcmEnblF					= 	DISABLED,
	.visitorPassSurrenderEnblF 	= 	DISABLED,
	.cafeInterTransactionDelay 	= 	0,
	.firstInResetAfter			=	FIRST_IN_RESET_AFTER_WORKING_HOURS_EXPIRE,
	.firstInTimer				=	3,
	.enrollIdType				=	ALPHANUM_ID,
	.twoCredMustModeF			=	FALSE,
	.sequentialInOut			=	DISABLED,
	.connectToPanel				=	DISABLED,
	.panelIpAddr				= 	"",
	.panelPort					=	11000,
	.showPin					= 	DISABLED,
	.desfireCustomKeyEnbl		= 	DISABLED,							// desfire custom key disabled
	.desfireCustomKey			= 	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	// default desfire custom key
	.showAttndcDtlF				=	DISABLED,
	.dvrInterfaceSelctionF		= 	DISABLED,							// DISABLED=Auto,  ENABLED=Manual
	.antiPassBack				= 	{DISABLED,DISABLED,NO,YES,RESET_PER_DAY,1},
	.autoHideMenuBarF			=	DISABLED,
	.birthdayMessage			=	"Happy Birthday",
	.MJPEGStreamAPI				= 	"",
	.streamAPIUsername			= 	"",
	.streamAPIPassword			= 	"",
	.retainJobSelectionF		= 	DISABLED,
	.saveFpImageF			= 	ENABLED,
	.temperatureLogCnfg			=	{	INACTIVE,
										UNIT_FAHRENHEIT,
										SENSOR_FEVO,
										INTERFACE_USB,
										0.95,
										USER_RESTRICTION_SOFT,
										DISABLED,
										99.5,
										0.0,
										3.0,
										10,
										1,
										0.5,
										95.0
									},
	//.maxFacePerUser				=	FACE_ENROLL_CNT_DFLT,
	.maxFacePerUser				=	FACE_ENROLL_CNT_DFLT,
	.faceMaskComp_Enable				= INACTIVE,
	.faceMaskComp_IdtFaceMaskDetectionThreshold	= DFLT_IDT_MASK_DETECTION_THRESHOLD,
	.faceMaskComp_VisibleFaceForEnrl	= DFLT_VISIBLE_FACE_FOR_ENRL,
	.faceMaskComp_ApproachCameraWaitTimer		= DFLT_APPROACH_CAMERA_WAIT_TIMER,
	.faceMaskComp_MaskDetectionTimeOut		= DFLT_MASK_DETECTION_TIMEOUT,
	.faceMaskComp_RestrictionType			= FACE_MASK_RESTRICTION_SOFT,
	.httpPort					= DFLT_HTTP_PORT,
	.httpsPort					= DFLT_HTTPS_PORT,
//	.displayDistinctiveFeedbackF = DISABLED,
//	.InPunchColor = IN_PUNCH_COLOR_GREEN,
//	.OutPunchColor = OUT_PUNCH_COLOR_YELLOW,
	.alwTLSVer				=	WEB_TLS_VERSION_1_0,
	.passwordPolicyF			=	DISABLED,
	.itemQuantityRestrictionF		=	DISABLED,
	.allowedOfflineItemQuantity		=	1,
	.cafeOfflineItemRestrict = { 	.activeMenuCode = INVALID_CAFE_MENU_CODE,
					.start = {0,0},
					.end = {0,0},
					.workingDay = MAX_WDAY,
					.startDate = {1,0,9},
				},
	.duressDetection				= 	DISABLED,
	.facilityCodeCheckEnableF			= 	DISABLED,
	.temporaryUserGenerateInfo = {	.autoAddTempUser   = NO_USER,
					.confirmReqF	    = DISABLED,
					.addNameViaDeviceF  = DISABLED,
				},
    .authenticationType = DIGEST_AUTHENTICATION,
    .authenticationAlgorithm = MD5_ALGORITM,
};

// DAY LIGHT SAVING TIME
const DST_CONFIG_t DfltDstConfig =
{
	.mode		= DST_MODE_DISABLE,							// Dst Mode
	.forward	= 	{
						{WEEKORDINAL_FIRST, SUNDAY, JANUARY},	// Forward Day Wise
						{1,0},									// Forward Date Wise
						{0,0}
					},											// Forward Time
	.backward	= 	{
						{WEEKORDINAL_FIRST, SUNDAY, JANUARY},	// backward Day Wise
						{1,0},									// Backward Date Wise
						{0,0}
					},											// Backward Time
	.timePeriod	=	{0,0}										// Dst Apply Time Information
};

//ETHERNET PARAMETERS
const ETHERNET_PARA_t  DfltEthPara =
{
	.ipAddress	= DFLT_ETH_IP_ADDR,			// Ipaddress(16 digits)
	.subnet		= DFLT_SM_ADDR,				// Subnet IP(16 digits)
	.gateway	= "",						// Gateway IP(16 digits)
	.macAddress	= DFLT_MAC_ADDR,			// Mac address(18 digits)
	.dns1		= "",						// DNS 1 address(16 digits)
	.dns2		= "",						// DNS 2 address(16 digits)
	.nwType		= LAN_IP_DYNAMIC,
};

//V11R3 SERVER PARAMETERS
const V11R3_SERVER_PARA_t  DfltV11R3ServerPara =
{
	.interface				= ETHERNET_PORT,			// Default server port
	.secondInterface		= INVALID_PORT,				// second priority interface
	.thirdInterface			= INVALID_PORT,				// third priority interface
	.ethServerIpAddress		= DFLT_SERVER_IP_ADDR,		// Server ip(16 digits)
	.ethServerHostName		= "",						// Default server host name
	.ethServerConnection	= SERVER_CONN_IP_ADDR,		// Default server communication
	.ethServerPort			= DFLT_ACMS_SERVER_PORT,	// Server port to listen ACMS message
	.wifiServerIpAddress	= DFLT_SERVER_IP_ADDR,		// Server ip(16 digits)
	.wifiServerHostName		= "",						// Default server host name
	.wifiServerConnection	= SERVER_CONN_IP_ADDR,		// Default server communication
	.wifiServerPort			= DFLT_ACMS_SERVER_PORT,	// Server port to listen ACMS message
	.modemServerIpAddress	= DFLT_SERVER_IP_ADDR,		// Server ip(16 digits)
	.modemServerHostName	= "",						// Default server host name
	.modemServerConnection	= SERVER_CONN_IP_ADDR,		// Default server communication
	.modemServerPort		= DFLT_ACMS_SERVER_PORT,	// Server port to listen ACMS message
	.httpInterface			= ETHERNET_PORT,
	.httpServerConnection	= SERVER_CONN_IP_ADDR,
	.httpServerIpAddress	= DFLT_SERVER_IP_ADDR,		// Server ip(16 digits)
	.httpServerHostName		= "",						// Default server host name
	.httpServerPort			= DFLT_HTTP_SERVER_PORT,	// Server port to listen ACMS message
};

//SERVER PARAMETERS
const SERVER_PARA_t  DfltServerParaCentra =
{
	.interface				= ETHERNET_PORT,			// Default server port
	.secondInterface		= WIFI_PORT,				// second priority interface
	.thirdInterface			= PPP_PORT,					// third priority interface
	.ethServerIpAddress		= "",						// Server ip(16 digits)
	.ethServerHostName		= DFLT_SERVER_IP_ADDR,		// Default server host name
	.ethServerConnection	= SERVER_CONN_HOST_NAME,	// Default server communication
	.ethServerPort			= DFLT_ACMS_SERVER_PORT,	// Server port to listen ACMS message
	.wifiServerIpAddress	= "",						// Server ip(16 digits)
	.wifiServerHostName		= DFLT_SERVER_IP_ADDR,		// Default server host name
	.wifiServerConnection	= SERVER_CONN_HOST_NAME,	// Default server communication
	.wifiServerPort			= DFLT_ACMS_SERVER_PORT,	// Server port to listen ACMS message
	.modemServerIpAddress	= "",						// Server ip(16 digits)
	.modemServerHostName	= DFLT_SERVER_IP_ADDR,		// Default server host name
	.modemServerConnection	= SERVER_CONN_HOST_NAME,	// Default server communication
	.modemServerPort		= DFLT_ACMS_SERVER_PORT,	// Server port to listen ACMS message
	.httpServerConnection	= SERVER_CONN_HOST_NAME,
	.type 					= 0,						// type of server config
	.tcpEncryptionModeF		= DISABLED,					//encryption mode for communication between server and device
	.TenantIdVyom			= DFLT_TENANT_ID_VYOM,		//tenant id of device
};

//SERVER PARAMETERS
const SERVER_PARA_t  DfltServerParaVyom =
{
	.interface				= ETHERNET_PORT,			// Default server port
	.secondInterface		= WIFI_PORT,				// second priority interface
	.thirdInterface			= PPP_PORT,					// third priority interface

	.ethServerIpAddress		= "",						// Server ip(16 digits)
	.ethServerHostName		= DFLT_HOST_NAME_VYOM,		// Default server host name
	.ethServerConnection	= SERVER_CONN_HOST_NAME,	// Default server communication
	.ethServerPort			= DFLT_PORT_NO_VYOM,		// Server port to listen ACMS message

	.wifiServerIpAddress	= "",						// Server ip(16 digits)
	.wifiServerHostName		= DFLT_HOST_NAME_VYOM,		// Default server host name
	.wifiServerConnection	= SERVER_CONN_HOST_NAME,	// Default server communication
	.wifiServerPort			= DFLT_PORT_NO_VYOM,		// Server port to listen ACMS message

	.modemServerIpAddress	= "",						// Server ip(16 digits)
	.modemServerHostName	= DFLT_HOST_NAME_VYOM,		// Default server host name
	.modemServerConnection	= SERVER_CONN_HOST_NAME,	// Default server communication
	.modemServerPort		= DFLT_PORT_NO_VYOM,		// Server port to listen ACMS message

	.httpServerConnection	= SERVER_CONN_HOST_NAME,
	.type 					= 0,						// type of server config
	.tcpEncryptionModeF		= ENABLED,					//encryption mode for communication between server and device
	.TenantIdVyom			= DFLT_TENANT_ID_VYOM,		//tenant id of device
};

//WIRELESS PARAMETERS
const WIFI_PARA_t	DfltWifiPara =
{
	.ipAddress	= DFLT_WIFI_IP_ADDR,				// Ipaddress(16 digits)
	.subnet		= DFLT_SM_ADDR,						// Subnet IP(16 digits)
	.gateway	= "",								// Gateway IP(16 digits)
	.macAddress	= DFLT_MAC_ADDR,					// Mac address(18 digits)
	.dns1		= "",								// DNS address(16 digits)
	.dns2		= "",								// Domain Name
	.nwType		= IP_STATIC,						// Slave assign mode Static
	.ssid		= "",								// SSID Name
	.secType	= NO_SEC_TYPE,						// No security
	.encType	= NO_ENCRYPTION,					// No Encryption
	.passphresh	= "",								// Passphresh
	.keymgmt	= WPA_PSK_KEY_MGMT,
	.wpsTrigerTyp = WPS_TRIGER_NONE,
	.wpsPin		= "",

	.advAuthType		= AUTH_TYPE_AUTO,
	.userName			= "",
	.anonymousIdentity	= "",
	.peapVer			= PEAP_VER_AUTO,
	.innerAuth			= INNER_AUTH_AUTO,
	.enableWifi			= FALSE,
	.bandFreqType		= WIFI_BAND_FREQ_2GHz,
};

//Modem Parameters
const MODEM_CONFIG_t	DfltModemProfile[MAX_MODEM_PROFILE] =
{
	{
		.enable			= ENABLED,			//Selection status
		.profileName	= "Airtel",			//Profile Name
		.dialNo			= "*99#",			//Dial Num
		.userName		= "",				//User Name
		.userPswd		= "",				//User Pswd
		.modemType		= MODEM_GSM,		//Modem Type:GSM/CDMA
		.apn			= "airtelgprs.com",	//APN Name
		.preferedPort	= 0,				//disabled
	},

	{
		.enable			= DISABLED,
		.profileName	= "Bsnl",
		.dialNo			= "*99***1#",
		.userName		= "",
		.userPswd		= "",
		.modemType		= MODEM_GSM,
		.apn			= "bsnlnet",
		.preferedPort	= 0,
	},

	{
		.enable			= DISABLED,
		.profileName	= "Vodafone",
		.dialNo			= "*99#",
		.userName		= "",
		.userPswd		= "",
		.modemType		= MODEM_GSM,
		.apn			= "www",
		.preferedPort	= 0,
	},

	{
		.enable			= DISABLED,
		.profileName	= "TATA Photon+",
		.dialNo			= "#777",
		.userName		= "internet",
		.userPswd		= "internet",
		.modemType		= MODEM_CDMA,
		.apn			= "",
		.preferedPort	= 0,
	},

	{
		.enable			= DISABLED,
		.profileName	= "Reliance-Jio",
		.dialNo			= "*99#",
		.userName		= "",
		.userPswd		= "",
		.modemType		= MODEM_GSM,
		.apn			= "jionet",
		.preferedPort	= 0,
	}
};

// Command and control centre communication
const COMMAND_CTRL_CONFIG_t DfltCmdCtrlConfig =
{
	.enable				= DISABLED,						// enable command control notification
	.ipAddress			= DFLT_COMMAND_CTRL_IP_ADDR,	// IP address
	.port				= DFLT_COMMAND_CTRL_PORT,		// port to listen command and control message
	.cccInterface		= ETHERNET_PORT,
	.interfaceSelctionF	= DISABLED,						// 0=Auto, 1=Manual
};

// FIRST IN USERS
const FIRST_IN_LIST_t  DfltFirstInUser =
{
	.members = {0}
};

const FIRST_IN_INFO_t DfltFristInInfo =
{
	.lockF			= TRUE,				//bydefault first in lock is enabled
	.startTimeInSec = 0,
	.endTimeInSec	= 0,
};

// TWO PERSON GROUP
const TWO_PERSON_GROUP_t DfltTwoPerson =
{
	.name	= "",						//Name of the group
	.userId	= {0,0,0,0,0,				//User ID
				0,0,0,0,0,
				0,0,0,0,0,
				0,0,0,0,0}
};

// BLOCKED USER CONFIGURATION
const BLOCKED_USER_GROUP_t DfltBlockedUser =
{
	.userIdx	= INVALID_USER,			//User Index(4 Digit)
	.userName	= "",					//User Name
	.date		= 0,					//Date on which user is blocked
	.reason		= REASON_UNKNOWN,		//Reason for blocking
};

// HOLIDAY SCHEDULE
const HOLIDAY_t DfltHoliday =
{
	.name		= "",					// Name of Holiday
	.startDate	= {1,0,9},				// Start date of Holiday
	.endDate	= {1,0,9},				// End date of Holiday
};

// VISITOR MANAGEMENT
const VISITOR_MANAGEMENT_t DfltVisitor =
{
	.visitorName	= "",					// Visitor name
	.startDate		= {1,0,9},				// Visitor start date
	.endDate		= {1,0,9},				// Visitor end date
	.startTime		= {0,0},				// Visitor start time
	.endTime		= {0,0},				// Visitor end time
	.repeatMode		= DO_NOT_REPEAT,		// Visitor repeat mode
	.repeatDays		= {
						NO_REPEAT_DAY,
						NO_REPEAT_DAY,
						NO_REPEAT_DAY,
						NO_REPEAT_DAY,
						NO_REPEAT_DAY,
						NO_REPEAT_DAY,
						NO_REPEAT_DAY
					  },					// Visitor repeat days
};

// SPECIAL FUNCTIONS
const SPECIAL_FUNCTION_t DfltSpecialFunc =
{
	.enable		= ENABLED,				// Special card enable/disable
	.userGrpId	= 0,					// Functional group ID
	.cardId		= {0,0,0,0},				// Card number
	.promptJobCodeF	= TRUE,
};

// TIME TRIGGERED FUNCTION
const TIME_TRIG_FUNC_t	DfltTimeTrigFunc  =
{
	.enable			= DISABLED,				// Working Status
	.outputPort		= 0,					// Output Port
	.name			= "",					// Function Name
	.startTime		= {0,0},				// startTime
	.duration		= 10,					// duration
	.workingDays	= {NO, NO, NO, NO, NO, NO, NO},
											// Active working days
	.holidayWorking	= NO					// Working holiday
};

// FPR configuartion
const FPR_CONFIG_t	DfltFprConfig =
{
	.lightCondition		= INDOOR,					// Light Condition
	.sensitivityLevel	= SENSITIVITY_LEVEL_8,		// Sensitivity Level
	.imageQuality		= IMAGE_QUALITY_MODERATE,		// Image quality
	.secLevel			= SEC_LEVEL_NORMAL,		// Security Level
	.fastMode			= FAST_MODE_AUTO,
};

// Default RADF Value Configuration
const READERS_PARAMETER_t DfltReadersParaConfig =
{
	.sm125Radf	= {	DEFLT_SM125_RADF_VALUE_ENTRY_READER,
					DEFLT_SM125_RADF_VALUE_ENTRY_READER,
					DEFLT_SM125_RADF_VALUE_EXIT_READER,
					DEFLT_SM125_RADF_VALUE_EXIT_READER},
					
	.ambientPara = {DEFLT_AMB_RETRYWAITTIMER,
					DEFLT_AMB_TEMPCOUNT,
					DEFLT_AMB_TEMPDEVIATION,
					DEFLT_AMB_AMBCYCLE,
					DEFLT_AMB_AMBDEVIATION,
					DEFLT_AMB_SHOWAMBONSTATUS,
					DEFLT_AMB_CHECKAMBREADING,
					DEFLT_AMB_AMBUSRDEVIATION},
};

const IO_LINK_CONFIG_t	DefltIoLinkConfig =
{
	.enable		= DISABLED,
	.name		= "",
	.inputPort	= USER_ALLOWED_TRIGGER,
	.outputPort	= AUX_OUT_PORT,
	.outputType	= OP_PULSE,
	.pulseTime	= 10,
	.resetLinkF = DISABLED,
	.resetLinkTime = {0,0},
};

const IO_LINK_EXT_DEV_CONFIG_t	DefltIoLinkExtDevConfig =
{
	.devIpAddr	= "",
};

const IO_LINK_EXT_DEV_MAPPING_t	DefltIoLinkExtDevMapping =
{
	.devEnable =	// All Devices DISABLED
	{
		0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	// 0-31
		0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	// 32-63
		0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	// 64-95
		0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	// 96-127

		0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	// 128-31
		0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	// 0-31
		0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	// 0-31
		0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,		// 0-255
	}
};

const CAFE_ITEM_CONFIG_t	DefltCafeItemConfig =
{
	.name			= "",
	.price			= 0.0,
	.disCntEnabled 	= DISABLED,
	.disCnt			= {0.0, 0.0, 0.0, 0.0}
};

const CAFE_MENU_CONFIG_t	DefltCafeMenuConfig =
{
	.active			= DISABLED,
	.dfltItemEnbl	= DISABLED,
	.itemCode		= {	[0 ... (MAX_CAFE_MENU_CODE-1)] = INVALID_CAFE_ITEM_CODE}
};

const CAFE_MENU_SCHEDULE_t	DefltCafeMenuSchConfig =
{
	.menuCode		= INVALID_CAFE_MENU_CODE,
	.dayTimeInfo	= { .start			= {0,0},
						.end			= {0,0},
						.workingDays	= {
											INACTIVE,	// SUNDAY
											ACTIVE,		// MONDAY
											ACTIVE,		// TUESDAY
											ACTIVE,		// WEDNESDAY
											ACTIVE,		// THURSDAY
											ACTIVE,		// FRIDAY
											INACTIVE,	// SATURDAY
										  }
					},
};

const PRINTER_CONFIG_t		DefltPrinterConfig =
{
	.printerType		= PRINTER_NONE,
	.printerCommType	= RS_232,
	.companyName		= "",
	.companyAddr		= {"", "", "", ""},
	.punchLine			= "Thank You!",
	.baudRate			= READER_UART_BAUD_115200,
	.printReceiptCnt	= 1,
	.excludePriceFrmCpn = FALSE,
	.printUserBalanceOnCouponF = FALSE,
};

const DVR_EVENT_ACTION_CONFIG_t	DfltDvrEvtActionConfig =
{
	.active				= FALSE,
	.evtType			= DVR_CONFIG_EVENT_USER_ALLOWED,
	.actionType			= DVR_ACTION_EMAIL_NOTIFICATION,
	.startTime			= {0, 0},
	.endTime			= {0, 0},
	.mode				= ANY_READER_MODE,
	.selectCamera		= 0,								//bydefault all camera is disabled.
	.activeDaysF		= {INACTIVE, INACTIVE, INACTIVE, INACTIVE, INACTIVE, INACTIVE, INACTIVE},
	.holiDayOverrideF	= INACTIVE,
	.recDuration		= 5,
	.popupDuration		= 5,
	.ptzPreset			= 1,
	.emailAddr			= "",
};

//Route Based Configuration
const	SMART_ROUTE_ACCESS_CONFIG_t DfltSmartRouteAccessConfig =
{
	.active  				= INACTIVE,					// Route is Inactive bydefault
	.doorLevel 				= DEFAULT_DOOR_LEVEL,		// Door Level
	.routeType				= SMART_ROUTE_ACCESS_SOFT,	// Route access type
	.resetOnStartLevel		= INACTIVE,
	.minLevel 				= DEFAULT_MIN_ROUTE_LEVEL,
	.maxLevel 				= DEFAULT_MAX_ROUTE_LEVEL,
	.sequencedF				= ENABLED,
};

//File Log Parameters Configuration
const	FILE_LOG_PARA_CONFIG_t DfltFileLogPara =
{
	.sysLog 				= TRUE,
	.dcLog					= FALSE,
	.webJvsLog				= FALSE,
	.acmsLog				= FALSE,
	.eventLog				= FALSE,
	.configLog				= FALSE,
	.alarmLog				= FALSE,
	.ioLink					= FALSE,
	.generalLog				= FALSE,
	.guiLog					= FALSE,
	.buLog					= FALSE,
	.readerTaskLog			= FALSE,
	.readerRxTxLog			= FALSE,
	.acmsRxTxLog			= FALSE,
	.webApiLog				= FALSE,
	.commLog				= FALSE,
	.bleLog					= FALSE,
	.readerDebugLog			= FALSE,
	.httpSyncLog			= FALSE,
};

// Web API Events Configuration
const WEB_API_EVENTS_CONFIG_t DfltWebAPIEventsConfig =
{
	.enable				= DISABLED,						// enable Web API events
	.ipAddress			= DFLT_WEB_API_EVENTS_IP_ADDR,	// IP address
	.port				= DFLT_WEB_API_EVENTS_PORT,		// port to listen Web API Events
	.respTime			= 10,
	.interface			= ETHERNET_PORT,
	.keepLiveEvent		= FALSE,
	.trigger			= FALSE,
};

const CARD_FORMAT_CONFIG_t	DfltCardFormatConf =
{
	.maxCardBit = {64, 64},
	.cardStructBit = {0, 0},
	.cardStruct = { {[0 ... (MAX_CARD_ID_BITS-1)] = 0},
					{[0 ... (MAX_CARD_ID_BITS-1)] = 0}
				  },
	.cardIdOrder = {CARD_ID_ORDER_FORWARD, CARD_ID_ORDER_FORWARD},
	.FCIncardIdF = {TRUE, TRUE},
	.operationSeq = {READING_ORDER_THAN_CONFIGURE,READING_ORDER_THAN_CONFIGURE},
};

const CARD_FORMAT_CONFIG_t	BlankCardFormatConf =
{
	.maxCardBit = {0, 0},
	.cardStructBit = {0, 0},
	.cardStruct = { {[0 ... (MAX_CARD_ID_BITS-1)] = 0},
					{[0 ... (MAX_CARD_ID_BITS-1)] = 0}
				  },
	.cardIdOrder = {0, 0},
	.FCIncardIdF = {0, 0},
	.operationSeq = {0,0},
};

const VAM_CONFIG_t	DfltVamConfig =
{
	.passId = 0,
	.vamCardId = {0, 0, 0},
	.startDate = {1,0,9},
	.startTime = {0,0},
	.endDate = {1,0,9},
	.endTime = {0,0},
	.smartRouteId = VAM_SMART_ROUTE_ID_NONE,
	.accessLevel = VAME_ACCESS_LEVEL_NONE,
};

const JCM_CONFIG_t	DfltJcmConfig =
{
	.refId	 = INVALID_JOB_REF_ID,
	.jobCode = "",
	.jobName = "",
	.startDate = {1,0,9},
	.endDate = {1,0,9},
};

const JCM_CONFIG_t	NoneJobConfig =				// None Job with reference ID FFFFFFFF
{
	.refId	 = NONE_JOB_REF_ID,
	.jobCode = "None",
	.jobName = "None",
	.startDate = {1,0,9},
	.endDate = {1,0,37},
};

const JCM_CONFIG_t	DefaultJobConfig =				// None Job with reference ID FFFFFFFE
{
	.refId	 = DEFAULT_JOB_REF_ID,
	.jobCode = "Default",
	.jobName = "Default",
	.startDate = {1,0,9},
	.endDate = {1,0,37},
};

const JCM_CONFIG_t	ContinueJobConfig =				// None Job with reference ID FFFFFFFD
{
	.refId	 = CONTINUE_JOB_REF_ID,
	.jobCode = "Continue",
	.jobName = "Continue Job",
	.startDate = {1,0,9},
	.endDate = {1,0,37},
};

const DOOR_MODE_SCH_CONFIG_t DfltDoorModeSchedul=
{
	.startTime		= {0,0},
	.endTime		= {0,0},
	.doorRdrMode    = ENTRY_READER,
	.days	= {
					INACTIVE,		// SUNDAY
					INACTIVE,		// MONDAY
					INACTIVE,		// TUESDAY
					INACTIVE,		// WEDNESDAY
					INACTIVE,		// THURSDAY
					INACTIVE,		// FRIDAY
					INACTIVE,		// SATURDAY
			  }
};

// Server Mode Configuration
const SERVER_MODE_CONFIG_t DfltServerMode=
{
	.serverMode		= COSEC_CENTRA,
};

// Vyom Server Configuration
const VYOM_SERVER_CONFIG_t DfltVyomServerPara =
{
		.interfaceVyom 			= ETHERNET_PORT,
		.secondInterfaceVyom	= INVALID_PORT,
		.thirdInterfaceVyom		= INVALID_PORT,
		.ServerHostNameVyom		= DFLT_HOST_NAME_VYOM,
		.ServerPortVyom			= DFLT_PORT_NO_VYOM,
		.TenantIdVyom			= DFLT_TENANT_ID_VYOM,
		.serverIpAddressVyom	= DFLT_SERVER_IP_ADDR,
		.serverConnectionVyom	= SERVER_CONN_HOST_NAME,
		.httpInterfaceVyom		= ETHERNET_PORT,
		.httpServerHostNameVyom	= "",
		.httpServerPortVyom		= DFLT_HTTP_SERVER_PORT,
};

// For VYOM mode
const BACKUP_AND_UPDATE_t DfltBackUpAndUpdateParaForVyomMode =
{
		.interface 				= ETHERNET_PORT,
		.ftpServerUrl			= "ftp://matrixtelecomsolutions.com",
		.ftpServerUseName 		= "cosecforread",
		.ftpServerUserPassword 	= "Cosec@341",
		.autoUpdate 			= ENABLED,
		.upgradeBasedOn			= FIRMWARE_UPGRADE_BASED_ON_VERSION,
		.interfaceSelctionF		= DISABLED,		// 0=Auto, 1=Manual
		.ftpServerResolveIpAddress	= "",
};

// For CENTRA mode
const BACKUP_AND_UPDATE_t DfltBackUpAndUpdateParaForCentraMode  =
{
		.interface 				= ETHERNET_PORT,
		.ftpServerUrl			= "",
		.ftpServerUseName 		= "",
		.ftpServerUserPassword 	= "",
		.autoUpdate 			= DISABLED,
		.upgradeBasedOn			= FIRMWARE_UPGRADE_BASED_ON_VERSION,
		.interfaceSelctionF		= DISABLED,		// 0=Auto, 1=Manual
		.ftpServerResolveIpAddress	= "",
};

const CONNECT_TO_PANEL_CONFIG_t DfltConnectToPanelPara =
{
		.connectToPanel			= DISABLED,
		.panelNwInterface		= ETHERNET_PORT,
		.panelIpAddr			= "",
		.panelPort				= 11000,
		.interfaceSelctionF		= DISABLED,		// 0=Auto, 1=Manual
		.panelEncryptionF		= DISABLED,
};


const LICENSE_DONGLE_CONFIG_t DfltLicenseDonglePara =
{
	.licenseServerUrl		= DFLT_SERVER_IP_ADDR,
	.licenseServerPort		= DFLT_PORT_NO_VYOM,
	.licenseServerIpAddress = "",
};

const IP_CAMERA_CONFIG_t DfltIpCameraConfig =
{
		.interfaceSelectionF		= FALSE,
		.captureSnapshotUrl			= "",
		.captureSnapshotUserName	= "",
		.captureSnapshotPassword	= "",
		.captureSnapshotSchedule	= {[0 ... (MAX_CAPTURE_SNAPSHOT_SCH - 1)] = { "", INACTIVE, CAPTURE_EVENT_USER_ALLOWED,
										CAPTURE_EVENT_MODE_BOTH, {0, 0}, {23, 59},
										{ACTIVE, ACTIVE, ACTIVE, ACTIVE, ACTIVE, ACTIVE, ACTIVE, ACTIVE}}},
};

const BRIGHTNESS_DETAIL_t DfltDisplayBrightness =
{
		.sleepTimer = 100,
		.setBrightness = 7,
};

//Access Schedule Default Configuration
const ACCESS_SCHEDULE_t  DfltAccessScheduleConfig =
{	
      .timeFrameEnableF = 	DISABLED,
      .timeFrameStartTime =	{0,0},
      .timeFrameEndTime =	{23,59},
      .timeFrameActiveDaysF = {INACTIVE, INACTIVE, INACTIVE, INACTIVE, INACTIVE, INACTIVE, INACTIVE },
      .timeFrameHolidayF	= INACTIVE,
};
//******** Static Variables **********

//******** Function Definations ******

