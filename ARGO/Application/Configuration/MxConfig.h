#if !defined MXCONFIG_H
#define MXCONFIG_H
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
//   Created By   : Priti Purohit
//   File         : MxConfig.h
//   Description  : This file consists of configuration related defines and
// 					structures
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include <time.h>
#include <openssl/aes.h>
#include <stdio.h>

#include "MxDeviceSpecific.h"
#include "MxTypedef.h"
#include "MxSysCommonDef.h"
#include "MxReaderRxTx.h"
#include "MxConfigDef.h"
#include "MxMultiLangConfig.h"
#include "MxDcTaskCommonDef.h"
#include "MxSysParaConfig.h"
#include "MxIdentificationServerConfig.h"
#include "MxEthernetPort.h"
#include "MxGpioDrv.h"
#include "MxAcmsCommon.h"
#include "MxSlaveCommon.h"
#include "MxSysTimer.h"

//******** Defines and Data Types ****

#define HW_VER						1

#ifndef SW_MINOR_REV
#define SW_MINOR_REV 				0
#endif

/* Used In FR communication */
#define SW_PATCH					SW_MINOR_REV
#define DOOR_COMM_SW_VER			14
#define DOOR_BUILD_NO				1
#define SYS_TYPE					"PD"
#define DOOR_APPL_TYPE_SIGNATURE_FILE	"/panelDoorApp"

// Finger Print Reader Configuration
#define	FPR_CFG_VER				1			// Finger Print Reader configuration version
typedef struct
{
	LIGHT_CONDITION_e	lightCondition;		// Light condition
	SENSITIVITY_LEVEL_e	sensitivityLevel;	// Sensitivity Level
	IMAGE_QUALITY_e		imageQuality;		// Image Quality
	SECURITY_LEVEL_e	secLevel;			// Security Level
	FAST_MODE_e			fastMode;			// Fast Mode
}FPR_CONFIG_t;

// RADF Value Configuration
#define	READER_PARA_CFG_VER		2			// Finger Print Reader configuration version
typedef struct
{
	UINT16 				retryWaitTimer;		//Default 120 sec after this ambient value will be read range 10 - 300 sec
	UINT8 				tempCount;			//default 30 this much count need to store of +/- tempdeviation range 1 - 50
	FLOAT32 			tempDeviation;		//default 0.5 this much count deviation is allowed between  two tempcount range 0.1 to 9.9
	UINT8 				ambCycle;			//default 5 this much ambient cycle need to wait and then avaerage will be store range 1 - 50
	FLOAT32 			ambDeviation;		//default 0.5 this much count deviation is allowed between  two ambcycle range 0.1 to 9.9
	BOOL				showAmbonStatus;   //default 1 this flag deside to show ambient temp[ on screen or not range 0 or 1
	BOOL				checkAmbReading;   //default 0 if this flag is set and ambient is in range with user temp =/- 0.2 fixed then user will not allowed range 0 or 1
	FLOAT32 			ambUserDeviation;	 //default 0.2 this much deviation is allowed between user and amb.
}AMBIENT_PARA_t;

typedef struct
{
	UINT8				sm125Radf[MAX_READERS];
	AMBIENT_PARA_t		ambientPara;
}READERS_PARAMETER_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
//						2 person Rule group configuration
///////////////////////////////////////////////////////////////////////////////////////////////////
#define TWO_PERSON_CFG_VER			1			// 2 - person configuration version
typedef struct
{
	CHAR 	name[MX_USER_NAME_SIZE];					// Group Name
	UINT32 	userId[MAX_TWO_PERSON_MEMBERS];		// Group Member's User IDs
}TWO_PERSON_GROUP_t;

// 2 person Rule configuration
typedef struct
{
	BOOL 	enable;								// Enable
	UINT8  	grpId1;								// Group id 1
	UINT8  	grpId2;								// Group id 2
}TWO_PERSON_t;

/**************************************************************************************************
//						Absentee Rule
**************************************************************************************************/
typedef struct
{
	BOOL	enable;								// Enable
	UINT16	dayCount;							// Maximum days count
}ABSENTEE_t;

/**************************************************************************************************
						Door sense Configuration
**************************************************************************************************/
typedef struct
{
	BOOL			enable;						// enable
	BOOL			supervise;					// Supervised Enable
	SENSE_TYPE_e	type;						// Door Sense type (NO/NC)
}DOOR_SENSE_t;

/**************************************************************************************************
						Aux Input Configuration
**************************************************************************************************/
typedef struct
{
	BOOL			enable;						// Enable
	BOOL			supervise;					// Supervised Enable
	SENSE_TYPE_e	sense;						// Door Sense (NO/NC)
	UINT8			debounceTime;				// Debounce Time
}AUX_INPUT_t;

/**************************************************************************************************
						Aux Output Configuration
**************************************************************************************************/
typedef struct
{
	BOOL			enable;						// Enable
}AUX_OUTPUT_t;

/**************************************************************************************************
						Door Schedule index Configuration
**************************************************************************************************/
#define READER_MODE_SCHEDULE_VER	1
typedef struct
{
	TIME_HHMM_t 			startTime;
	TIME_HHMM_t 			endTime;
	BOOL					days[MAX_WDAY];
	READER_MODE_e			doorRdrMode;
}DOOR_MODE_SCH_CONFIG_t;

/**************************************************************************************************
						Additional Security Code Configuration
**************************************************************************************************/
#define ADDITIONAL_SECURITY_CODE_CFG_VER	1	// Additional Security code confiuration version
typedef struct
{
	BOOL	enable;								// Enable
	UINT16	code;								// Code
}ASC_t;

/**************************************************************************************************
						Anti Pass Back Rule Configuration
**************************************************************************************************/
#define ANTI_PASS_BACK_CFG_VER			1		// Anti-Pass Back configuration version
typedef struct
{
	BOOL 	enableEntry;						// Enable Antipassback in Entry
	BOOL 	enableExit;							// Enable Antipassback in Exit
	BOOL   	hard;								// Hard Enable
	BOOL   	forgiveness;						// Forgiveness Enable
}OLD_ANTI_PASS_BACK_t;

typedef struct
{
	BOOL 				enableEntry;						// Enable Antipassback in Entry
	BOOL 				enableExit;							// Enable Antipassback in Exit
	BOOL   				hard;								// Hard Enable
	BOOL   				forgiveness;						// Forgiveness Enable
	APB_RESET_TYPE_e	resetAfter;							// Reset After (PER DAY / TIMER BASED)
	UINT16				forgivenessTimer; 					// 1 to 999 minutes
}ANTI_PASS_BACK_t;

/**************************************************************************************************
						BLocked User Configuration
**************************************************************************************************/
#define BLOCKED_USER_CFG_VER			1

// Blocked user configuration
typedef struct
{
	UINT16			userIdx;					// User index
	CHAR			userName[NAME_SIZE_UTF8];		// User Name
	time_t			date;						// Time stamp
	BLOCK_REASON_e  reason;  					// Reason for block
}BLOCKED_USER_GROUP_t;

/**************************************************************************************************
						Day Light Saving Configuration
**************************************************************************************************/
#define	DST_CFG_VER				1				// Day Light Saving configuration version

// DST related dat to be save
typedef struct
{
	DAY_WWDDMM_t	dayWise;
	DATE_DDMM_t		dateWise;
	TIME_HHMM_t		time;			// Current time
}DST_INFO_t;

// Day Light Saving Configuration
typedef struct
{
	DST_MODE_e    	mode;			// Mode of time change
	DST_INFO_t  	forward;		// Forward current time
	DST_INFO_t  	backward;		// Backward current time
	TIME_HHMM_t		timePeriod;		// Time Perioad
}DST_CONFIG_t;

/**************************************************************************************************
						First In User Rule Configuration
**************************************************************************************************/
#define	FIRST_IN_CFG_VER			3			// First In User Configuration version

// First In User List Configuration
typedef struct
{
	UINT32  members[OLD_MAX_FIRST_IN_MEMBERS];		// Member's User ID in list
}FIRST_IN_LIST_VER1_t;

typedef struct
{
    UINT32  members[MAX_FIRST_IN_MEMBERS_99];		// Member's User ID in list
}FIRST_IN_LIST_VER2_t;

typedef struct
{
    UINT32  members[MAX_FIRST_IN_MEMBERS];		// Member's User ID in list
}FIRST_IN_LIST_t;

// First In user Rule Configuration
typedef struct
{
	BOOL 	enable;								// Enable
	UINT8  	listId;								// List id
}FIRST_IN_RULE_t;

// First in info related to first in lock-unlock state.
#define FIRST_IN_INFO	1
typedef struct
{
	BOOL 		lockF;				// locked due to first in
	time_t		startTimeInSec;		// (In seconds with ref. to Epoch)Start time of valid first in user
	time_t		endTimeInSec;		// (In seconds with ref. to Epoch)End time of valid first in user
}FIRST_IN_INFO_t;

#define	HOLIDAY_SCH_VER				1			// Holiday Schedule configuration version

// Holiday schedule configuration
typedef struct
{
	CHAR  			name[HOLIDAY_NAME_SIZE];	// Name of holiday
	DATE_DDMMYY_t  	startDate;					// Start date of holiday
	DATE_DDMMYY_t  	endDate;					// End date of holiday
}HOLIDAY_t;

// Holiday Schedule list configuation
typedef struct
{
	UINT8		scheduleId;						// holiday schedule Id
	HOLIDAY_t 	holidays[MAX_HOLIDAYS];  		// List of holidays
}HOLIDAY_SCHEDULE_t;

/**************************************************************************************************
						ETHERNET PARAMETERS
**************************************************************************************************/

#define	 ETHERNET_PARA_VER	1	// Intensionally kept ver=1 to avoid defaulting parameters on downgrade
typedef struct
{
	CHAR			ipAddress[MAX_IP_ADDRESS];				// IP address
	CHAR			subnet[MAX_SUBNET_MASK];				// Subnet Mask
	CHAR			gateway[MAX_GATEWAY];					// Gateway IP address
	CHAR			macAddress[MAX_MAC_ADDRESS];			// MAC address of Master
	CHAR			dns1[MAX_DNS_ADDRESS];					// DNS1 address
	CHAR			dns2[MAX_DNS_ADDRESS];					// DNS2 address
	LAN_IP_MODE_e	nwType;
}ETHERNET_PARA_t;

/**************************************************************************************************
						SERVER PARAMETERS
**************************************************************************************************/

#define	V11R3_SERVER_PARA_VER	 1
typedef struct
{
	COMM_PORT_e 	interface;								// Default server communication port
	COMM_PORT_e		secondInterface;
	COMM_PORT_e		thirdInterface;

	CHAR			ethServerIpAddress[MAX_IP_ADDRESS];		// IP address
	CHAR        	ethServerHostName[MAX_SERVER_ADDR];		// Ethernet Server Hostname
	SERVER_CONN_e 	ethServerConnection;					// Ethernet Server Connection Mode
	UINT16			ethServerPort;							// Ethernet Server port to listen ACMS message

	CHAR			wifiServerIpAddress[MAX_IP_ADDRESS];	// Wifi Server IP address
	CHAR        	wifiServerHostName[MAX_SERVER_ADDR];	// Wifi Server Hostname
	SERVER_CONN_e 	wifiServerConnection;					// Wifi Server Connection Mode
	UINT16			wifiServerPort;							// Wifi Server port to listen ACMS message

	CHAR			modemServerIpAddress[MAX_IP_ADDRESS];	// Modem Server IP address
	CHAR        	modemServerHostName[MAX_SERVER_ADDR];	// Modem Server Hostname
	SERVER_CONN_e 	modemServerConnection;					// Modem Server Connection Mode
	UINT16			modemServerPort;						// Modem Server port to listen ACMS message

	COMM_PORT_e 	httpInterface;
	CHAR			httpServerIpAddress[MAX_IP_ADDRESS];	// Http Server IP address
	CHAR        	httpServerHostName[MAX_SERVER_ADDR];	// Http Server Hostname
	SERVER_CONN_e 	httpServerConnection;					// Http Server Connection Mode
	UINT16			httpServerPort;							// Http Server port to listen ACMS message
}V11R3_SERVER_PARA_t;

#define	SERVER_PARA_VER		 	3							//Move Http config into Basic Sys Para (CON-4627)
#define	SERVER_PARA_SUB_VER	 	1							//v11R4
typedef struct
{
	COMM_PORT_e 	interface;								// Default server communication port
	COMM_PORT_e		secondInterface;
	COMM_PORT_e		thirdInterface;

	CHAR			ethServerIpAddress[MAX_IP_ADDRESS];		// IP address
	CHAR        	ethServerHostName[MAX_SERVER_ADDR_254];		// Ethernet Server Hostname
	SERVER_CONN_e 	ethServerConnection;					// Ethernet Server Connection Mode
	UINT16			ethServerPort;							// Ethernet Server port to listen ACMS message

	CHAR			wifiServerIpAddress[MAX_IP_ADDRESS];	// Wifi Server IP address
	CHAR        	wifiServerHostName[MAX_SERVER_ADDR_254];	// Wifi Server Hostname
	SERVER_CONN_e 	wifiServerConnection;					// Wifi Server Connection Mode
	UINT16			wifiServerPort;							// Wifi Server port to listen ACMS message

	CHAR			modemServerIpAddress[MAX_IP_ADDRESS];	// Modem Server IP address
	CHAR        	modemServerHostName[MAX_SERVER_ADDR_254];	// Modem Server Hostname
	SERVER_CONN_e 	modemServerConnection;					// Modem Server Connection Mode
	UINT16			modemServerPort;						// Modem Server port to listen ACMS message

	SERVER_CONN_e 	httpServerConnection;					// Http Server Connection Mode

	UINT8			type;
	BOOL			tcpEncryptionModeF;
	CHAR			TenantIdVyom[MAX_TENANT_ID];
	UINT8			unusedBytes[512];

}SERVER_PARA_t;

#define	WIFI_PARA_VER		3
// WiFi Parameter configuation
typedef struct
{
	CHAR				ipAddress[MAX_IP_ADDRESS];			// IP address
	CHAR				subnet[MAX_SUBNET_MASK];			// Subnet Mask
	CHAR				gateway[MAX_GATEWAY];				// Gateway IP address
	CHAR				macAddress[MAX_MAC_ADDRESS];		// MAC address
	CHAR				dns1[MAX_DNS_ADDRESS];				// DNS1 address
	CHAR				dns2[MAX_DNS_ADDRESS];				// DNS2 address
	IP_MODE_e			nwType;								// Type of network
	CHAR				ssid[MAX_SSID_NAME];				// SSID name
	SEC_TYPE_e			secType;							// Security Type
	ENCRYPTION_e		encType;							// Encryption type
	CHAR				passphresh[MAX_PASSPHRESH_SIZE];	// Pass phresh
	WIFI_KEY_MGMT_e		keymgmt;
	WPS_TRIGER_TYP_e	wpsTrigerTyp;
	CHAR				wpsPin[MAX_WPS_PIN_SIZE];

	WIFI_AUTH_TYPE_e	advAuthType;
	CHAR				userName[MAX_WIFI_IDENTITY_SIZE];
	CHAR				anonymousIdentity[MAX_WIFI_IDENTITY_SIZE];
	PEAP_VER_e			peapVer;
	INNER_AUTH_e		innerAuth;
	BOOL				enableWifi;
	WIFI_BAND_FREQ_TYPE_e	bandFreqType;
}WIFI_PARA_t;

#define	MODEM_PARA_VER	1

// Mobile broadband Parameter configuration
typedef struct
{
	BOOL			enable;									// Selected/Not Sel
	CHAR			profileName[MODEM_PROVIDER_NAME_SIZE];	// Profile Name
	CHAR			dialNo[MODEM_DIAL_NUM_SIZE];			// Dialled Number
	CHAR			userName[MODEM_USER_NAME_SIZE];			// User Name
	CHAR			userPswd[MODEM_PASSWORD_SIZE];			// Password
	MODEM_TYPE_e	modemType;								// GSM/CDMA
	CHAR			apn[MODEM_APN_NAME_SIZE];				// APN Name
	UINT8			preferedPort;							// Preferred Port
}MODEM_CONFIG_t;

#define COMMAND_CTRL_PARA_VER	1
typedef struct
{
	BOOL			enable;						// enable command control notification
	CHAR			ipAddress[MAX_IP_ADDRESS];	// IP address
	UINT16			port;						// port to listen command and control message
	COMM_PORT_e		cccInterface;
	BOOL			interfaceSelctionF;			// 0=Auto, 1=Manual
}COMMAND_CTRL_CONFIG_t;

/**************************************************************************************************
						Occupancy Control Configuration
**************************************************************************************************/
#define OCCUPANCY_CONTROL_CFG_VER		1	// Configuration version
typedef struct
{
	BOOL    enable;							// Enable
	UINT16  count;							// Available count
}OLD_OCCUPANCY_CNTRL_t;

typedef struct
{
	BOOL    enable;							// Enable
	UINT16  count;							// max count allowed
	UINT8	minUsers;						// min count allowed
	BOOL	zeroOccupancy;					// zero occupancy enable
}OCCUPANCY_CNTRL_t;

/**************************************************************************************************
						Password enumeration and configuration
**************************************************************************************************/
#define	PASSWORD_VER				3

// Password configuration
typedef struct
{
	CHAR  displayPswd[MAX_USER_TYPE][MAX_PASSWORD_SIZE];	// Display Screen Password Per User NOTE we have not changed user size because previously it was common for both (Web & Display)
	CHAR  webPswd[MAX_USER_TYPE][MAX_WEB_PASSWORD_SIZE];	// Web Password Per User
	UINT16 PswdResetDaysCount[MAX_USER_TYPE];		// Password expiry days count
    UINT8  DispPwdAttemptCount[MAX_USER_TYPE];
    TIMER_HANDLE DispPwdLoginRestrictTimer[MAX_USER_TYPE];
}PASSWORD_t;

#define	SPECIAL_FUNCTION_VER	1

// Special Function Configuration
typedef struct
{
	BOOL		enable;								// Enable
	UINT8		userGrpId;							// User group ID
	UINT64		cardId[MAX_SP_FUNC_CARD];			// Maximum cards associated
	BOOL		promptJobCodeF;
}SPECIAL_FUNCTION_t;

// Time Stamping Configuration
#define TIME_STAMP_CFG_VER						1
typedef struct
{
	BOOL					enable;					// Enable
	CHAR 					name[MX_USER_NAME_SIZE];		// Name
	TIME_HHMM_t				startTime;				// Start time
	TIME_HHMM_t				endTime;				// end time
	TIME_STAMP_TRIGGER_e	activeTrigger;		// Active triger on action
	BOOL					activeDays[MAX_WDAY];	// Days of week Enable
	BOOL					activeHoliday;			// Holiday Enable
}TIME_STAMP_CONFIG_t;

// Time Triggered Function Configuration
#define	TIME_TRIG_FUNC_CFG_VER			1			// Time Trigger Function configuration version
typedef struct
{
	BOOL			enable;							// Enable
	OUTPUT_PORT_e 	outputPort;						// Output Port
	CHAR			name[MX_USER_NAME_SIZE];    			// Name
	TIME_HHMM_t		startTime;						// Start time
	UINT16 			duration;						// Duration
	BOOL			workingDays[MAX_WDAY];			// Days of week Enable
	BOOL			holidayWorking;					// Working Holidays Enable
}TIME_TRIG_FUNC_t;

// Use Count Control Configuration
typedef struct
{
	BOOL	enable;									// Enable
	UINT8	count;									// Use Count/minute
}USE_COUNT_CNTRL_t;

#define	VISITOR_MANAGEMENT_VER			2	// V14R4

// Visitor Management Configuration
typedef struct
{
	CHAR				visitorName[NAME_SIZE_UTF8];	// Visitor Name
	DATE_DDMMYY_t		startDate;						// Visitor start date
	DATE_DDMMYY_t		endDate;						// Visitor end date
	TIME_HHMM_t			startTime;						// Visitor start time
	TIME_HHMM_t			endTime;						// Visitor end time
	REPEAT_MODE_VMS_e	repeatMode;						// Visitor repeat mode
	BOOL				repeatDays[MAX_REPEAT_DAYS];	// Visitor repeat days
}VISITOR_MANAGEMENT_t;

//Cafeteria Restrict Item in Offline Mode
typedef struct
{
	UINT8		activeMenuCode;			// Active Menu code
	TIME_HHMM_t	start;				// Start time HH:MM
	TIME_HHMM_t	end;				// End time HH:MM
	WEEKDAYS_e	workingDay;			// Active days for access
	DATE_DDMMYY_t	startDate;
}CAFE_OFFLINE_ITEM_RESTRICT_CONFIG_t;


typedef struct
{
 uint8_t faceCountInServer; //Total face in cosec server-received during http req-response
 uint8_t getFacePokeCount; //Notification count we received from Cosec to update the user's face
 uint8_t setFacePokeCount; //Notification count we received from Cosec to upload the user's face
 uint8_t setFaceIndex[SET_MAX_USER_FACE_INDEX]; //
 time_t lastFaceEnrollTime; //(server) time on which the template was enrolled on server.
 time_t lastFaceReceivetime; //(device) time on which template is received to device.
}FaceCredData_t;

#define	USER_CFG_VER				9		//Sync From Device functionality

// User Configuration
typedef struct
{
	//	User General Information
	BOOL					enable;					// Enable
	UINT32					userId;					// User ID (8 digits)
	CHAR					name[NAME_SIZE_UTF8];		// Name
	CHAR					alphaNumId[ALPHANUM_ID_SIZE];//Alphanumeric UserID

	//	User Access informaton
	BOOL					vip;					// VIP Enable
	BOOL					bypassFinger;			// Bypass finger print status
	UINT64					cardId[MAX_USER_CARD];	// Maximum cards associated
	CHAR					pinCode[MAX_PIN_SIZE];	// PIN code
	UINT16					visitorIdx;				// Index of visitor in visitor table
	BOOL 					terminationEnable;		// Termination date status
	DATE_DDMMYY_t			terminationDate;		// Termination date(DD:MM:YY)
	UINT8					holidaySchId;			// Holiday schedule ID
	ABSENTEE_t				absentee;				// Absentee Rule
	BOOL 					restrictAccess;			// restrict access enable/disable
	BOOL					allowedForCafe;			// allowed for cafeteria enable/disable
	CAFE_DISCNT_LEVEL_e		discountLevelCafe;		// Discount level for cafeteria
	USER_PAYMENT_MODE_e		paymentModeForCafe;		// PaymentMode For Cafe
	UINT8					blockForCafe;			// User Block for Cafe Flag
	UINT8					smartRouteId;			// Smart Route ID
	UINT8					smartRouteLevel;		// Smart User Access Level
	BOOL					selfEnrlEnbl;
	UINT8					selfEnrlSkipedCnt;		// no of count that user has skipped self enrolment

	USER_JOB_TYPE_e			jobType;				// job type fixed for user or dynamic (to be selected at runtime)
	BOOL					checkBalanceBeforeTxnF; // applicable in case of prepaid server payment mode only
	UINT16					userGrpId;
	BOOL					cafeteriaPolicyCheckF;	// Cafeteria Policy Check before transaction
	BOOL					showAttndcDtlF;
	BOOL					birthdayMsgEnable;
	DATE_DDMMYY_t			dateOfBirth;
	BOOL					enableFaceRecoF;	// Enable User For Face Verification
	BOOL					bypassFaceF;		// Bypass the face verification for user	
	UINT8					offlineCafeTransactionType;	// offline cafeteria transaction type - Not Allowed, Allowed with discount or Allowed without discount.

	UINT8					consumedItemQuantity[MAX_CAFE_ITEM_CODE];	// Consumed Item Quantity in Offline mode 
	UINT8					DuressFp[MAX_DURESS_DUAL_FP_TEMPLATE];
	BOOL					panelDoorAssignF;
	BOOL					profilePhotoSyncPendingF;
	FaceCredData_t          		faceCredData;
}USER_CONFIG_t;

#define BASIC_SYS_PARA_VER	14		//Door Access Schedule

// Door Controller Basic Configuration
typedef struct
{
	//	General
	UINT16					devId;										// Device ID
	CHAR					name[DEVICE_NAME_SIZE_UTF8];							// Name for the door
	TIME_FORMAT_e			timeFormat;									// Time format
	ACCESS_MODE_e			doorAccessMode;								// Access mode for Door
	ZONE_APPL_e				application;								// Application (AC + TA or TA only)
	BOOL					exitOnLock;									// Allow to exit when door locked
	BOOL 					tamperAlarm;								// Tamper Alarm status
	DOOR_SENSE_t			doorSense;									// Door sense enabled, supervized, type
	UINT16					doorPulseTime;								// Pulse time (Sec)
	BOOL 					autoRelock;									// Auto Relock status
	//	Time Zone
	BOOL					userDefNtp;									// User-defined NTP address Enable
	CHAR					preDefNtpAddr[MAX_NTP_ADDRESS_UTF8];				// Predefined Network Time Protocol address
	CHAR					userDefNtpAddr[MAX_USER_DEF_NTP_ADDRESS_UTF8];	// User-defined NTP addresses
	UINT8 					utz;										// Universal time zone
	NTP_MODE_e				timeUpdateMode;								// Network Time Protocol Mode

	TIME_HHMM_t				workingHrStart;								// Working Hour Start
	TIME_HHMM_t				workingHrStop;								// Working Hour Stop
	BOOL					workingDays[MAX_WDAY];						// working days (sun-sat)
	BOOL					holidayWorking;								// Working Holidays
	//	Reader
	READER_t				reader[MAX_READERS];						// Internal and external reader
	BOOL 					requestExitSw;								// Request to Exit Switch Enable
	BOOL					buzMuteF;									// Buzzer Mute Option Flag
	UINT16					tagRedetectDelay;							// Time to avoid consequence detection of UHF tag
	BOOL					cdcExitReaderF;								// Exit reader type (Old COSEC type, Path type)
	BOOL					cerPresentF;
	ACCESS_MODE_e			extRdrAccessMode;							// Access mode for reader on external port
	BOOL 					degradeModeEnable;							// Enable/Disable Degraded Mode
	UINT8					degradeWaitTimer;							// Wait time for Degrade mode
	BOOL   					doorModeSchdlEnblF;							//Door mode schedule flag
	BOOL					entryModeWallpaperF;					// Show wallpaper flag when door mode is entry
	BOOL					exitModeWallpaperF;						// Show wallpaper flag when door mode is exit
	FPR_TEMPLATE_FORMAT_e	fprTemplateFormat;
	WIGND_RDR_MODE_e		extWiegandRdrMode;							// normal door mode or act as reader
	WIGND_OUT_FRMT_e		wiegandOutFormat;							// wiegand output format
	WIGND_OUT_DIR_e			wiegandOutDir;								// wiegand output direction(MSB first or LSB first)
	BOOL					waitForAccessFeedback;						// wait for feedback or not
	UINT8					accessFeedbackWaitTime;						// feedback wait time in second
	BOOL					doorModeSelectEnblF;
	BOOL					waitForUsrVerification;						// Flag to enable wait for user verification by 3rd party panel
	UINT8					wgndOutCstmFrmtNo[MAX_WIEGAND_EVENT_FORMAT];	// Wiegand out format selection for different event
	UINT32					wgndOutEventSpclCode[MAX_WIEGAND_EVENT_FORMAT];	// Special code for event
	COMM_PORT_e				IoLinkNwInterface;
	UINT16					autoRelockTime;								// Door Auto Relock Time
	BOOL					apiAccessAllowEnable;
	API_ACCESS_MODE_e 		apiEntryAccessMode;
	API_ACCESS_MODE_e 		apiExitAccessMode;
	CHAR					apiSecurityKey[SECURITY_KEY_SIZE+1];
	CAD_DURATION_e			accessAllowCadDuration;								// cadence duration- long, medium or short
	ACCESS_MODE_e			visitorDoorAccessMode;								// Access mode of visitor for Door
	ACCESS_MODE_e			visitorExtRdrAccessMode;							// Access mode of visitor for reader on external port
	CHAR					pinChangeCode[MAX_PIN_CHANGE_CODE_SIZE];
	BOOL					promptSpFnF;
	BOOL					generateEventF;
	BOOL					cbrPresentF;
	UINT16					accessAllowScreenDuration;				//Duration for access allow screen
	CAD_DURATION_e			accessDenyCadDuration;
	UINT16					accessDenyScreenDuration;              // Duration for access Denied screen
	BOOL 					abnormalAlarm;
	BOOL 					forceOpenAlarm;
	BOOL 					faultAlarm;
	BOOL 					panicAlarm;
	BOOL					tempThresholdAlarm;
	CAFE_FR_ACCESS_MODE_e	cafeteriaFrAccessMode;
	BOOL					faceMaskNotDetectedAlarm;	// Access Denied - Face Mask Not Detected
	BOOL					atomRD100PresentF;
	BOOL					atomRD200PresentF;
	BOOL					atomRD300PresentF;
	UINT8					degradeModeType;
	FLOAT32					doorPulseTimeFloat;
	UINT8					accessScheduleID;
	BOOL 					accessScheduleEnableF;
	BOOL 					hideMenuBarForAccessScheduleF;
	HTTP_WEB_SERVER_CONFIG_t 		httpWebServerConfig;
}SYS_BASIC_PARA_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
//						Door Controller Advance Configuration
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	BOOL			enable;				//SSA enable or disabled
	ACCESS_MODE_e	accessMode;			//SSA access mode
	UINT8			accessLevel;		//door SSA access level
}SSA_CONFIG_t;

typedef struct
{
	TIME_HHMM_t		startTime;
	TIME_HHMM_t		endTime;
	CHAR			message[GREETING_MSG_SIZE];
}GREETING_CONFIG_t;

typedef struct
{
	TIME_HHMM_t		startTime;
	TIME_HHMM_t		endTime;
	CHAR			message[GREETING_MSG_SIZE_UTF8];
}GREETING_CONFIG_NEW_t;

typedef struct
{
	BOOL 						isEnable;
	TEMPERATURE_UNIT_e			tempUnit; //0-Fahrenheit, 1 Celcius
	TEMPERATURE_SENSOR_TYPE_e	sensorType; //0-AST , 1-Web Based
	SENSOR_INTERFACE_e			sensorInterface; //0-RS232, 1-USB, 2-HTTPS
	FLOAT32						emissivity;	// Range 0.95 to 0.98
	USER_RESTRICTION_e			restrictionType; //0-Soft, 1-Hard
	BOOL						bypassOnDisconnect;
	FLOAT32						thresholdTemp;
	FLOAT32						calibrationPara; //Range -5 to 5
	FLOAT32						approachWaitTmr;	// 0.0 - 15.0
	UINT8						tempDetectionTimer; // 5-15
	UINT8						validSampleCnt;	// 3 - 20
	FLOAT32						thresholdDeviation;	// 0.0-1.0
	FLOAT32						minimumThreshold;	// 999.9
}TEMPERATURE_LOG_CONFIG_t;

#define ADVANCE_SYS_PARA_VER			13	//Temporary User Config
typedef struct
{
	// Features enable/disable
	BOOL						absenteeRuleEnable;		// Absentee rule enable/disable informaton
	ASC_t						addlSecurity;			// Additional Security Code Information
	USE_COUNT_CNTRL_t			useCount;				// User Count Control Information
	FIRST_IN_RULE_t				firstIn;				// First In User Rule Information
	TWO_PERSON_t				twoPerson;				// 2 Person Rule Information
	OLD_OCCUPANCY_CNTRL_t		oldOccupancyCtrl;		// Occupancy Control Information
	OLD_ANTI_PASS_BACK_t		oldAntiPassBack;		// Anti Pass Back Information
	AUX_INPUT_t					auxInput;				// Aux Input Information
	AUX_OUTPUT_t				auxOutput;				// Aux Output Information

	//	Enrollment
	BOOL						enrollmentEnable;		// Enrollment status
	ENROLLMENT_TYPE_e			enrollmentType;			// Enrollment type
	UINT8						fpCount;				// No of Finger prints to enroll per user
	UINT8						cardCount;				// No of Cards to enroll per user
	UINT8						maxFingerPerUser;		// Max no. of fingers per user to be enrolled
	TEMPLATES_PER_FINGER_e		templatesPerFinger;		// Templates per finger Information
	//	System Timers
	UINT8						twoPersonTimer;			// 2 Person Wait Timer
	UINT8						interDigitTimer;		// Inter Digit Wait Timer
	UINT8						multiAccessTimer;		// Multi User Access Timer
	UINT8						lateEarlyTimer;			// Late-IN/Late-OUT Timer

	BOOL						genExitSwEvent;			// Generate Exit switch event
	BOOL						genInvalidUserEvent;	// Generate Invalid user event
	SSA_CONFIG_t				SSAConfig;				// smart secure access configuation

	//dvr-nvr intigration related
	BOOL						dvrIntigrationEnbl;
	CHAR						dvrIpAddr[MAX_IP_ADDRESS];
	UINT16						dvrPort;
	COMM_PORT_e					dvrInterface;

	// Smart route access related
	BOOL						smartRouteAccessEnbl;

	//greeting related
	BOOL						enblGreetingsF;
	GREETING_CONFIG_NEW_t		greetingConfig[MAX_GREETING_CONFIG];

	TWO_PERSON_MODE_e			twoPersonMode;

	BOOL						autoAlarmAck;
	UINT16						autoAlarmAckTimer;
	BOOL						mifareCustomKeyEnbl;
	UINT8						mifareCustomKey[MIFARE_KEY_SIZE];
	BOOL						hidIclasCustomKeyEnbl;
	UINT8						hidIclasCustomKey[HID_ICLASS_KEY_SIZE];
	BOOL						customKeyAutoUpdateF;
	SMART_ROUTE_ORDER_TYPE_e 	smartRouteOrder;
	BOOL						verifyAccessPolicyOnExitF;
	UINT16						facilityCode;					// Facilty Code// Type of door lock
	DOOR_LOCK_TYPE_e 			lockType;
	BOOL						acceptExtIOLinkF;				// Accept External IO Link request
	UINT8						outputWaitTime;					//output wait time
	BOOL						webEncryptionModeF;
	BOOL						selfEnrlEnbl;
	UINT8						selfEnrlRetryCnt;
	BOOL						vamEnable;
	OCCUPANCY_CNTRL_t			occupancyCtrl;
	DVR_EVENT_USER_TYPE_e		dvrAllowEventuserType;
	DVR_EVENT_USER_TYPE_e		dvrDenyEventUserType;
	BOOL						jcmEnblF;
	BOOL						visitorPassSurrenderEnblF;
	UINT16						cafeInterTransactionDelay;
	FIRST_IN_RESET_AFTER_TYPE_e firstInResetAfter;
	UINT16						firstInTimer;
	ENROLL_ID_TYPE_e			enrollIdType;
	BOOL						twoCredMustModeF;
	BOOL						sequentialInOut;
	BOOL						connectToPanel;
	CHAR						panelIpAddr[MAX_IP_ADDRESS];
	UINT16						panelPort;
	BOOL						showPin;
	BOOL						desfireCustomKeyEnbl;
	UINT8						desfireCustomKey[DESFIRE_CARD_KEY_SIZE];
	BOOL						showAttndcDtlF;
	BOOL						dvrInterfaceSelctionF;			// 0=Auto, 1=Manual
	ANTI_PASS_BACK_t			antiPassBack;			// Anti Pass Back Information
	BOOL						autoHideMenuBarF;
	CHAR						birthdayMessage[GREETING_MSG_SIZE_UTF8];
	CHAR						MJPEGStreamAPI[FR_STREAM_API_LENGTH];
	CHAR						streamAPIUsername[FR_STREAM_API_USERNAME_LENGTH];
//	streamAPIPassword is un-used, 128 character password support is given in frCameraPassword(FR_CAMERA_CONFIG_t)
	CHAR						streamAPIPassword[FR_STREAM_API_PASSWORD_LENGTH];
	BOOL						retainJobSelectionF;
	BOOL						saveFpImageF;
	TEMPERATURE_LOG_CONFIG_t	temperatureLogCnfg; // Temperature logging configuration
	BOOL						faceMaskComp_Enable;				// Face Mask Compulsion - Enable
	FLOAT32						faceMaskComp_IdtFaceMaskDetectionThreshold;	// Face Mask Detection Threshold identification
	FLOAT32						faceMaskComp_VisibleFaceForEnrl;	// Face Mask Detection Threshold enrollment
	FLOAT32						faceMaskComp_ApproachCameraWaitTimer;		// Approach to Camera Wait-Timer
	UINT8						faceMaskComp_MaskDetectionTimeOut;		// Mask Detection Time-Out
	FACE_MASK_RESTRICTION_e		faceMaskComp_RestrictionType;			// Restriction Type (0=Soft,1=Hard)
	UINT16						httpPort;
	UINT16						httpsPort;
	UINT8						maxFacePerUser;		// Max no. of faces per user to be enrolled
//	BOOL						displayDistinctiveFeedbackF;
//	IN_PUNCH_COLOR_e			InPunchColor;
//	OUT_PUNCH_COLOR_e			OutPunchColor;
	UINT8						alwTLSVer;
	BOOL						passwordPolicyF;	// This flag is currently applicable to only Direct Doors
	BOOL						itemQuantityRestrictionF;
	UINT8						allowedOfflineItemQuantity;
	CAFE_OFFLINE_ITEM_RESTRICT_CONFIG_t		cafeOfflineItemRestrict;
	BOOL 						duressDetection;
	BOOL						facilityCodeCheckEnableF;
	TEMPORARY_USER_GENERATE_t			temporaryUserGenerateInfo;
    AUTHENTICATION_TYPE_e       authenticationType;
    AUTHENTICATION_ALGORITM_e   authenticationAlgorithm;
}SYS_ADVANCE_PARA_t;

#define IO_LINKING_VER		1
typedef struct
{
	BOOL			enable;							// Enable/disable
	CHAR			name[MX_USER_NAME_SIZE];				// IO Linking Name
	INPUT_PORT_e	inputPort;						// Input trigger condition
	OUTPUT_PORT_e	outputPort;						// Output port
	OUTPUT_TYPE_e	outputType;						// Type of output (pulse, interlock, latch)
	UINT8			pulseTime;						// pulse time when output type is pulse
	BOOL			resetLinkF;						// reset LinkEnable Flag
	TIME_HHMM_t		resetLinkTime;
}IO_LINK_CONFIG_t;

#define IO_LINK_EXT_DEV_VER		1
typedef struct
{
	CHAR			devIpAddr[MAX_IP_ADDRESS];
}IO_LINK_EXT_DEV_CONFIG_t;

#define IO_LINK_EXT_DEV_MAPPING_VER		1
typedef struct
{
	BOOL			devEnable[MAX_IO_LINK_EXT_DEVICE];
}IO_LINK_EXT_DEV_MAPPING_t;

typedef struct
{
	UINT32 					userId;
	CHAR					alphaNumId[ALPHANUM_ID_SIZE];
	CHAR					name[MX_USER_NAME_SIZE];
	INT8					accessLevel;
	DATE_DDMMYY_t			validity;
	BOOL					vipFlag;
	BOOL					bypassFinger;
	INT8					pin[MAX_PIN_SIZE];
}SSA_USER_CONFIG_t;


#define DVR_EVT_ACTN_CONFIG_VER		1
typedef struct
{
	BOOL					active;
	DVR_CONFIG_EVENT_TYPE_e	evtType;
	DVR_ACTION_TYPE_e		actionType;
	TIME_HHMM_t				startTime;
	TIME_HHMM_t				endTime;
	READER_MODE_e			mode;
	UINT32					selectCamera;					//each bit represent one camera.
	BOOL					activeDaysF[MAX_WDAY];
	BOOL					holiDayOverrideF;
	UINT8					recDuration;
	UINT8					popupDuration;
	UINT8					ptzPreset;
	CHAR					emailAddr[MAX_EMAIL_ADDR_SIZE_UTF8];
}DVR_EVENT_ACTION_CONFIG_t;

// time zone information
typedef struct
{
	TIME_HHMM_t		start;					// Start time HH:MM
	TIME_HHMM_t		end;					// End time HH:MM
	BOOL			workingDays[MAX_WDAY];	// Active days for access
}TIME_ZONE_t;

// e-Canteen Item Configuration
#define	CAFE_ITEM_CFG_VER				1
typedef struct
{
	CHAR		name[SIZE_CAFE_ITEM_NAME_UTF8];
	FLOAT32		price;
	BOOL		disCntEnabled;
	FLOAT32		disCnt[MAX_CAFE_ITEM_DISCOUNT_LEVEL];
}CAFE_ITEM_CONFIG_t;

// e-Canteen Menu Configuration
#define	CAFE_MENU_CFG_VER				1
typedef struct
{
	BOOL		active;
	BOOL		dfltItemEnbl;
	UINT8		itemCode[MAX_CAFE_MENU_ITEMS];
}CAFE_MENU_CONFIG_t;

// e-Canteen Menu Schedule Configuration
#define	CAFE_MENU_SCHEDULE_CFG_VER		1
typedef struct
{
	UINT8					menuCode;
	TIME_ZONE_t				dayTimeInfo;
}CAFE_MENU_SCHEDULE_t;

// e-Canteen Printer Configuration
#define	CAFE_PRINTER_CFG_VER			2
typedef struct
{
	PRINTER_TYPE_e			printerType;
	PRINTER_COMM_TYPE_e  	printerCommType;
	CHAR					companyName[MAX_PRINTEABLE_SIZE_UTF8];
	CHAR					companyAddr[MAX_ADDR_LINE_PRINT][MAX_PRINTEABLE_SIZE_UTF8];
	CHAR					punchLine[MAX_PRINTEABLE_SIZE_UTF8];
	READER_UART_BAUD_e		baudRate;
	UINT8					printReceiptCnt;
	BOOL					excludePriceFrmCpn;
	BOOL					printUserBalanceOnCouponF;	// User Balance on Coupon
}PRINTER_CONFIG_t;

#define SMART_ROUTE_ACCESS_CONFIG_VER	1
typedef struct
{
	BOOL						active;
	UINT8						doorLevel;
	SMART_ROUTE_ACCESS_TYPE_e	routeType;
	BOOL						resetOnStartLevel;
	UINT8						minLevel;
	UINT8						maxLevel;
	BOOL						sequencedF;
}SMART_ROUTE_ACCESS_CONFIG_t;

#define SYS_VAR_VER		1
typedef struct
{
	UINT8				backgroundImage;
	UINT8				currentClock;
	DST_STATE_e			dstState;
	KEYPAD_e			currentKeypad;
	DOOR_LOCK_STATE_e	doorLockState;
}SYS_VAR_t;

#define FILE_LOG_PARA_CONFIG_VER	4
typedef struct
{
	BOOL		sysLog;
	BOOL 		dcLog;
	BOOL 		webJvsLog;
	BOOL		acmsLog;
	BOOL		eventLog;
	BOOL		configLog;
	BOOL		alarmLog;
	BOOL		ioLink;
	BOOL		generalLog;
	BOOL		guiLog;
	BOOL		buLog;
	BOOL		readerTaskLog;
	BOOL		readerRxTxLog;
	BOOL		acmsRxTxLog;
	BOOL		webApiLog;
	BOOL		commLog;
	BOOL		bleLog;
	BOOL		readerDebugLog;
	BOOL		httpSyncLog;
}FILE_LOG_PARA_CONFIG_t;

/*
typedef enum
{
	TCP_EVENTS_INTERFACE_AUTO,
	TCP_EVENTS_INTERFACE_ETHERNET,
	TCP_EVENTS_INTERFACE_WIFI,
	TCP_EVENTS_INTERFACE_MODEM,
	MAX_TCP_EVENTS_INTERFACE
}TCP_EVENTS_INTERFACE_e;
*/

#define WEB_API_EVENTS_PARA_VER			1
typedef struct
{
	BOOL					enable;						// enable web api events
	CHAR					ipAddress[MAX_IP_ADDRESS];	// IP address
	UINT16					port;						// port to listen web api events
	UINT16					respTime;
	COMM_PORT_e				interface;
	BOOL					trigger;
	BOOL					keepLiveEvent;
}WEB_API_EVENTS_CONFIG_t;

typedef struct __attribute__((packed))
{
	UINT16					rollOverCnt;
	UINT32					seqNo;
}WEB_API_EVENTS_ACTIVITY_t;

#define CARD_FORMAT_CONFIG_VER	1
typedef struct
{
	UINT8				maxCardBit[INT_EXT_PORT];
	UINT8				cardStructBit[INT_EXT_PORT];
	UINT8				cardStruct[INT_EXT_PORT][MAX_CARD_ID_BITS];
	CARD_ID_ORDER_e		cardIdOrder[INT_EXT_PORT];
	BOOL				FCIncardIdF[INT_EXT_PORT];
	OPERATION_SEQUENCE_e	operationSeq[INT_EXT_PORT];
}CARD_FORMAT_CONFIG_t;

//Vehicle access configuration
#define VAM_CONFIG_VER	1
typedef struct
{
	UINT32				passId;
	UINT64				vamCardId[MAX_VAM_CARD_TYPE];
	DATE_DDMMYY_t		startDate;
	TIME_HHMM_t			startTime;
	DATE_DDMMYY_t		endDate;
	TIME_HHMM_t			endTime;
	UINT8				smartRouteId;
	UINT8				accessLevel;
}VAM_CONFIG_t;

//Job Costing Config
#define JCM_CONFIG_VER	2
typedef struct
{
	UINT32				refId;
	CHAR 				jobCode[JOB_CODE_SIZE];
	CHAR				jobName[JOB_NAME_SIZE_UTF8];
	DATE_DDMMYY_t		startDate;
	DATE_DDMMYY_t		endDate;
}JCM_CONFIG_t;

//This config only used to pass data
#define LED_AND_BUZZER_CADANCE_CONFIG_VER 1
typedef struct
{
	BOOL		led;
	BOOL		buzzer;
	UINT16		cadBits;
	UINT8		cycles;
}BUZZ_LED_CAD_CONFIG_t;

#define SERVER_MODE_CONFIG_VER 1
typedef struct
{
	SERVER_MODE_e		serverMode;
}SERVER_MODE_CONFIG_t;

#define VYOM_SERVER_CONFIG_VER 1
typedef struct
{
	COMM_PORT_e 	interfaceVyom;									// Default server communication port
	COMM_PORT_e		secondInterfaceVyom;
	COMM_PORT_e		thirdInterfaceVyom;
	CHAR        	ServerHostNameVyom[MAX_VYOM_SERVER_ADDR];		// Server Hostname
	UINT16			ServerPortVyom;
	CHAR			TenantIdVyom[MAX_TENANT_ID];
	CHAR			serverIpAddressVyom[MAX_IP_ADDRESS];			// Vyom Server IP address
	SERVER_CONN_e 	serverConnectionVyom;							// Ethernet Server Connection Mode
	COMM_PORT_e 	httpInterfaceVyom;
	CHAR        	httpServerHostNameVyom[MAX_VYOM_SERVER_ADDR];	// Http Server Hostname
	UINT16			httpServerPortVyom;								// Http Server port to listen ACMS message
}VYOM_SERVER_CONFIG_t;

#define BACKUP_AND_UPDATE_CONFIG_VER 		2	//FP5
typedef struct
{
	COMM_PORT_e 	interface;
	CHAR			ftpServerUrl[MAX_FTP_SERVER_URL];
	CHAR			ftpServerUseName[MAX_FTP_USER_NAME];
	CHAR			ftpServerUserPassword[MAX_FTP_USER_PASSWORD];
	BOOL			autoUpdate;
	FIRMWARE_UPGRADE_BASED_ON_e	 upgradeBasedOn;
	BOOL			interfaceSelctionF;
	CHAR			ftpServerResolveIpAddress[MAX_IP_ADDRESS];	// FTP Server URL
}BACKUP_AND_UPDATE_t;

#define CONNECT_TO_PANEL_CONFIG_VER  1
typedef struct
{
	BOOL			connectToPanel;
	COMM_PORT_e		panelNwInterface;
	CHAR			panelIpAddr[MAX_IP_ADDRESS];
	UINT16			panelPort;
	BOOL			interfaceSelctionF;			// 0=Auto, 1=Manual
	BOOL			panelEncryptionF;
}CONNECT_TO_PANEL_CONFIG_t;

#define LICENSE_DONGLE_VER	  1
typedef struct
{
	CHAR			licenseServerUrl[MAX_SERVER_ADDR_254];
	UINT16			licenseServerPort;
	CHAR			licenseServerIpAddress[MAX_IP_ADDRESS];		// IP address
}LICENSE_DONGLE_CONFIG_t;


#define CAPTURE_SNAPSHOT_SCH_VER	1
typedef struct
{
	CHAR						scheduleName[MAX_IP_CAMERA_SCH_NAME];
	BOOL						eventActive;
	CAPTURE_IMG_EVENT_e			captureEvent;
	CAPTURE_IMG_EVENT_MODE_e	captureEventMode;
	TIME_HHMM_t					startTime;
	TIME_HHMM_t					endTime;
	BOOL						activeDay[MAX_WDAY+1]; //MAX_WDAY = HOLIDAY
}CAPTURE_SNAPSHOT_SCH_t;

#define IP_CAMERA_CONFIG_VER		2	//FP5
typedef struct
{
	BOOL						interfaceSelectionF;	//0auto 1manual
	CHAR						captureSnapshotUrl[CAPTURE_SNAPSHOT_URL_LENGTH];
	CHAR						captureSnapshotUserName[CAPTURE_SNAPSHOT_USERNAME_LENGTH];
	CHAR						captureSnapshotPassword[CAPTURE_SNAPSHOT_PASSWORD_LENGTH];
	CAPTURE_SNAPSHOT_SCH_t		captureSnapshotSchedule[MAX_CAPTURE_SNAPSHOT_SCH];
}IP_CAMERA_CONFIG_t;

#define BRIGHTNESS_CONFIG_VER		1
typedef struct
{
	UINT16 		sleepTimer;
	PWM_DUTY_e 	setBrightness;
}BRIGHTNESS_DETAIL_t;

typedef struct
{
	UINT32			evtCnt;
	UINT16			evtRollCnt;
}EVENT_PICS_EVENT_INFO_t;

typedef struct
{
	UINT16						picsIdx;
	EVENT_PICS_EVENT_INFO_t		eventInfo[MAX_EVENT_PICS_COUNT];
}EVENT_PICS_INFO_t;

enum JVS_ENCRYPT_PSWD_ID_e
{
	JVS_ENCRYPT_LOGIN_PSWD_ID	= 0,
	JVS_ENCRYPT_CHNG_PSWD_ID,
	JVS_ENCRYPT_HRD_PSWD_ID,
};

typedef struct
{
	char EncryptPswd[AES_BLOCK_SIZE+2];
	char EncryptInitVector[AES_BLOCK_SIZE+2];
}WebEncryptPswdInfo;

WebEncryptPswdInfo gWebEncryptPswdInfo[JVS_ENCRYPT_HRD_PSWD_ID];

#define ACCESS_SCHEDULE_CONFIG_VER		1
typedef struct
{
    BOOL 	 timeFrameEnableF;
    TIME_HHMM_t  timeFrameStartTime;
    TIME_HHMM_t  timeFrameEndTime;
    BOOL 	 timeFrameActiveDaysF[MAX_WDAY];
    BOOL         timeFrameHolidayF;
}ACCESS_SCHEDULE_t;

//******** Function Prototypes *******
BOOL IsAlphaNumIdValid(CHARPTR alphaNumId, UINT16PTR userIdxPtr);
BOOL IsCardValid(UINT64 cardId, UINT16PTR userIdxPtr, UINT32 *credSubType);
BOOL IsSplFnCard(UINT64 cardId, SPECIAL_FUNCTIONS_e * splFnIdxPtr, CARD_INDEX_e * cardIdxPtr);
BOOL IsPinValid(CHARPTR userPin, UINT16PTR userIdxPtr);
BOOL IsUserBlocked(UINT16 userIdx, UINT8PTR blockIdxPtr);
BOOL IsVisitorValid(UINT16 visitorIdx);
BOOL IsUserValidityExpired(DATE_DDMMYY_t terminationDate);
BOOL IsUserIn2PersonGrp(UINT16 userIdx, UINT8PTR twoPersonGrpDtl);
BOOL IsFirstInUser(UINT32 userId, UINT8PTR firstInIdxPtr);
BOOL IsReaderPresent(READER_CATEGORY_e category, RESPONSE_PORT_e port, READER_MODE_e mode, READER_ID_e * readerIdPtr);

BOOL AddBlockedUser(UINT16 userIdx, BLOCK_REASON_e reason);
void RestoreBlockedUser(UINT16 userIdx);

RESPONSE_PORT_e GetReaderPort(READER_ID_e readerId);

UINT8 GetAvailUserFp(UINT16 userIdx);
UINT8 GetCurrentUserFp(UINT16 userIdx);
UINT8 GetAvailUserCard(UINT16 userIdx);
UINT8 GetAvailSplFnCard(UINT8 splFnIdx);

CHARPTR GetGreetingMsg(UINT16 userIdx);

BOOL IsUserCardPresent(UINT16 userIdx, UINT8 cardIdx);
BOOL IsSplFnCardPresent(UINT8 splFnIdx, UINT8 cardIdx);
void DeleteUserCards(UINT16 userIdx);
void RemoveUserFromSystem(UINT16 userIdx);
void CreateTestUser(UINT16 index);
void GetSM125DefltRadfValue(void);
DOOR_APPL_TYPE_e GetDoorApplType(void);
void ReadDoorApplType(void);
void SetDoorApplType(DOOR_APPL_TYPE_e doorApplType);
BOOL GetFreeUserFpIdx(UINT16 userIdx, UINT8PTR fpIdxPtr);
void GetSysBasicConfigWorkingCopy(SYS_BASIC_PARA_t * tempSysBasicPara);

UINT8 GetCosecDeviceCode(void);

BOOL GetVamConfigIndex(UINT32 passId, UINT16PTR configIdx);
BOOL IsVamCardValid(UINT64 cardId);

void StringReverse(CHARPTR string);

// Configuration utility functions
READER_TYPE_e GetReaderTypeCfg(READER_ID_e readerId);
void SetReaderTypeCfg(READER_ID_e readerId, READER_TYPE_e readerType);
BOOL GetBuzMuteCfg(void);
BOOL GetSmartRouteAccessEnblCfg(void);
BOOL GetAddlSecCodeEnblCfg(void);
BOOL GetFacilityCodeEnblCfg(void);
UINT16PTR GetAddlSecCodeCfgPtr(void);
UINT16PTR GetFacilityCodeCfgPtr(void);
BOOL GetHidIclassCustomKeyEnblCfg(void);
UINT8PTR GetHidIclassCustomKeyCfgPtr(void);
BOOL GetMifareCustomKeyEnblCfg(void);
UINT8PTR GetMifareCustomKeyCfgPtr(void);
BOOL GetDesfireCustomKeyEnblCfg(void);
UINT8PTR GetDesfireCustomKeyCfgPtr(void);
UINT16 GetCustomKeyAutoUpdateEnblCfg(void);
UINT16 GetUHFTagRedetectDelayCfg(void);
BOOL GetJcmConfigIdxFromRefId(UINT32 refId, UINT16PTR configIdxPtr);
BOOL SetCadenceDuration(CAD_DURATION_e accessAllowCadDuration, CAD_DURATION_e accessaDenyCadDuration);
CAD_DURATION_e GetAccesAllowCadDuration(void);
CAD_DURATION_e GetAccesDenyCadDuration(void);
RESPONSE_PORT_e	GetReaderRespPortQr(READER_MODE_e readerMode);
BOOL GetAlphaNumIdForSIUser(CHARPTR alphaNumId);
BOOL IsServerModeVyom(void);
UINT8 GetServerMode(void);
BOOL IsServerModeThirdParty(void);
USER_IN_OUT_STATUS_e GetUserInOutStatus(UINT32 userIdx);
BOOL SetUserInOutStatus(UINT32 userIdx, USER_IN_OUT_STATUS_e status);
BOOL ChngReqConfigForVariantWoIntFPReader(VOID);
BOOL GetComboRdrPresentCfg(const EXT_COMBO_READER_e readerId);
BOOL SetComboRdrCfgType(const READER_ID_e readerId, const SYS_BASIC_PARA_t * const srcSysBasicPara ,SYS_BASIC_PARA_t * destSysBasicPara);
BOOL DecryptPswdFromWebGUI(char *pInStr, char *pOutStr, unsigned long encslength, int EncryptPswdId);

#endif  // MXCONFIG_H
