#if !defined MXCONFIGOLD_H
#define MXCONFIGOLD_H
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
//   Created By   : Viral Chavda
//   File         : MxConfigOld.h
//   Description  : This file consists of old configuration related defines and
// 					structures
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include <time.h>

#include "MxTypedef.h"
#include "MxSysCommonDef.h"
#include "MxConfig.h"
#include "MxFaceRecognitionConfig.h"
//******** Defines and Data Types ****
#define MAX_CAFE_MENU_VER1			9
#define MAX_CAFE_MENU_ITEM_VER2	    30
#define	OLD_MAX_READERS_VER9		4

//***********************************************************************************
//		Modem Configuration
//***********************************************************************************
#define MODEM_USER_NAME_SIZE_OLD		(25+1)
#define MODEM_PASSWORD_SIZE_OLD 		(25+1)

//******** Function Prototypes *******

typedef struct
{
	CHAR			visitorName[NAME_SIZE_UTF8];	// Visitor Name
	DATE_DDMMYY_t	startDate;						// Visitor start date
	DATE_DDMMYY_t	endDate;						// Visitor end date
	TIME_HHMM_t		startTime;						// Visitor start time
	TIME_HHMM_t		endTime;						// Visitor end time
}VISITOR_MANAGEMENT_VER1_t;


#define MAX_PIN_SIZE_6_BYTES		(6 + NULL_CHAR_SIZE)
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
	CHAR					pinCode[MAX_PIN_SIZE_6_BYTES];	// PIN code
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
}USER_CONFIG_VER1_t;

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
}USER_CONFIG_VER2_t;


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
	BOOL					offlineCafeTransactionType;	// allowed offline cafeteria transaction enable/disable
}USER_CONFIG_VER3_t;

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

	CHAR					lastfaceupdatedtime[HTTP_SYNC_DATE_LENGTH_MAX]; //(server) time on which the template was enrolled on server.
	time_t					lastfaceReceivedtime; //(device) time on which template is received to device.
	UINT8					faceUpdateCount;
	UINT8					totalFaceInServer;
}USER_CONFIG_VER4_t;

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

	CHAR					lastfaceupdatedtime[HTTP_SYNC_DATE_LENGTH_MAX]; //(server) time on which the template was enrolled on server.
	time_t					lastfaceReceivedtime; //(device) time on which template is received to device.
	UINT8					faceUpdateCount;
	UINT8					totalFaceInServer;
	UINT8					consumedItemQuantity[MAX_CAFE_ITEM_CODE];	// Consumed Item Quantity in Offline mode 
}USER_CONFIG_VER5_t;

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

	CHAR					lastfaceupdatedtime[HTTP_SYNC_DATE_LENGTH_MAX]; //(server) time on which the template was enrolled on server.
	time_t					lastfaceReceivedtime; //(device) time on which template is received to device.
	UINT8					faceUpdateCount;
	UINT8					totalFaceInServer;
	UINT8					consumedItemQuantity[MAX_CAFE_ITEM_CODE];	// Consumed Item Quantity in Offline mode 
	UINT8					DuressFp[MAX_DURESS_DUAL_FP_TEMPLATE];
}USER_CONFIG_VER6_t;

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

	CHAR					lastfaceupdatedtime[HTTP_SYNC_DATE_LENGTH_MAX]; //(server) time on which the template was enrolled on server.
	time_t					lastfaceReceivedtime; //(device) time on which template is received to device.
	UINT8					faceUpdateCount;
	UINT8					totalFaceInServer;
	UINT8					consumedItemQuantity[MAX_CAFE_ITEM_CODE];	// Consumed Item Quantity in Offline mode 
	UINT8					DuressFp[MAX_DURESS_DUAL_FP_TEMPLATE];
	BOOL					panelDoorAssignF;
}USER_CONFIG_VER7_t;

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

    CHAR					lastfaceupdatedtime[HTTP_SYNC_DATE_LENGTH_MAX]; //(server) time on which the template was enrolled on server.
    time_t					lastfaceReceivedtime; //(device) time on which template is received to device.
    UINT8					faceUpdateCount;
    UINT8					totalFaceInServer;
    UINT8					consumedItemQuantity[MAX_CAFE_ITEM_CODE];	// Consumed Item Quantity in Offline mode
    UINT8					DuressFp[MAX_DURESS_DUAL_FP_TEMPLATE];
    BOOL					panelDoorAssignF;
    BOOL					profilePhotoSyncPendingF;
}USER_CONFIG_VER8_t;

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
	BOOL 					alarmIndication;							// Local Alarm status
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
	READER_t				reader[OLD_MAX_READERS_VER9];				// Internal and external reader
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
	CAD_DURATION_e			cadDuration;								// cadence duration- long, medium or short
	ACCESS_MODE_e			visitorDoorAccessMode;								// Access mode of visitor for Door
	ACCESS_MODE_e			visitorExtRdrAccessMode;							// Access mode of visitor for reader on external port
	CHAR					pinChangeCode[MAX_PIN_CHANGE_CODE_SIZE];
	BOOL					promptSpFnF;
	BOOL					generateEventF;
	BOOL					cbrPresentF;
}SYS_BASIC_PARA_VER1_t;

typedef struct
{
	//	General
	UINT16					devId;										// Device ID
	CHAR					name[DEVICE_NAME_SIZE_UTF8];							// Name for the door
	TIME_FORMAT_e			timeFormat;									// Time format
	ACCESS_MODE_e			doorAccessMode;								// Access mode for Door
	ZONE_APPL_e				application;								// Application (AC + TA or TA only)
	BOOL					exitOnLock;									// Allow to exit when door locked
	BOOL 					alarmIndication;							// Local Alarm status
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
	READER_t				reader[OLD_MAX_READERS_VER9];				// Internal and external reader
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
}SYS_BASIC_PARA_VER2_t;

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
	READER_t				reader[OLD_MAX_READERS_VER9];				// Internal and external reader
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
}SYS_BASIC_PARA_VER3_t;

typedef struct{
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
	READER_t				reader[OLD_MAX_READERS_VER9];				// Internal and external reader
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
}SYS_BASIC_PARA_VER4_t;

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
	READER_t				reader[OLD_MAX_READERS_VER9];				// Internal and external reader
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
}SYS_BASIC_PARA_VER5_t;


//Added new access mode in apiEntryAccessMode and apiExitAccessMode
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
	READER_t				reader[OLD_MAX_READERS_VER9];				// Internal and external reader
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
}SYS_BASIC_PARA_VER6_t;

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
	READER_t				reader[OLD_MAX_READERS_VER9];				// Internal and external reader
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
}SYS_BASIC_PARA_VER7_t;

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
	READER_t				reader[OLD_MAX_READERS_VER9];				// Internal and external reader
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
}SYS_BASIC_PARA_VER8_t;

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
	READER_t				reader[OLD_MAX_READERS_VER9];				// Internal and external reader
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
}SYS_BASIC_PARA_VER9_t;

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
}SYS_BASIC_PARA_VER10_t;

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
}SYS_BASIC_PARA_VER11_t;

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
}SYS_BASIC_PARA_VER12_t;

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
}SYS_BASIC_PARA_VER13_t;

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
	CHAR						streamAPIPassword[FR_STREAM_API_PASSWORD_LENGTH];
}SYS_ADVANCE_PARA_VER1_t;

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
	CHAR						streamAPIPassword[FR_STREAM_API_PASSWORD_LENGTH];
	BOOL						retainJobSelectionF;
}SYS_ADVANCE_PARA_VER2_t;

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
}SYS_ADVANCE_PARA_VER3_t;

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
}SYS_ADVANCE_PARA_VER4_t;

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
}SYS_ADVANCE_PARA_VER5_t;

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
}SYS_ADVANCE_PARA_VER6_t;

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
}SYS_ADVANCE_PARA_VER7_t;

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
}SYS_ADVANCE_PARA_VER8_t;

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
}SYS_ADVANCE_PARA_VER9_t;

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
}SYS_ADVANCE_PARA_VER10_t;

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
}SYS_ADVANCE_PARA_VER11_t;

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
}SYS_ADVANCE_PARA_VER12_t;

typedef struct
{
	BOOL 				enableFrServer;
	UINT16				port;
	FACE_CAPTURING_e 	faceCapturing;
	UINT16 				faceScanTimeOut;
	FR_MODE_e 			frMode;
	CHAR 				hostName[MAX_SERVER_ADDR_254];
	CHAR 				frServerIpAddress[MAX_IP_ADDRESS];
	UINT16 				identificationTimeOut;
	BOOL				frServerInterfaceSelectionF;
	COMM_PORT_e			frServerInterface;
}FR_SERVER_CONFIG_VER1_t;

typedef struct
{
	BOOL 				enableFrServer;
	UINT16				port;
	FACE_CAPTURING_e 	faceCapturing;
	UINT16 				faceScanTimeOut;
	FR_MODE_e 			frMode;
	CHAR 				hostName[MAX_SERVER_ADDR_254];
	CHAR 				frServerIpAddress[MAX_IP_ADDRESS];
	UINT16 				identificationTimeOut;
	BOOL				frServerInterfaceSelectionF;
	COMM_PORT_e			frServerInterface;
	BOOL				enableFreeScanTimeOutF;
}FR_SERVER_CONFIG_VER2_t;

typedef struct
{
	BOOL 				enableFrServer;
	UINT16				port;
	FACE_CAPTURING_e 	faceCapturing;
	UINT16 				faceScanTimeOut;
	FR_MODE_e 			frMode;
	CHAR 				hostName[MAX_SERVER_ADDR_254];
	CHAR 				frServerIpAddress[MAX_IP_ADDRESS];
	UINT16 				identificationTimeOut;
	BOOL				frServerInterfaceSelectionF;
	COMM_PORT_e			frServerInterface;
	BOOL				enableFreeScanTimeOutF;
	BOOL				encryptionSsl;
}FR_SERVER_CONFIG_VER3_t;

typedef struct
{
	BOOL 				enableFrServer;
	UINT16				port;
	FACE_CAPTURING_e 	faceCapturing;
	UINT16 				faceScanTimeOut;
	FR_MODE_e 			frMode;
	CHAR 				hostName[MAX_SERVER_ADDR_254];
	CHAR 				frServerIpAddress[MAX_IP_ADDRESS];
	UINT16 				identificationTimeOut;
	BOOL				frServerInterfaceSelectionF;
	COMM_PORT_e			frServerInterface;
	BOOL				enableFreeScanTimeOutF;
	BOOL				encryptionSsl;
	BOOL				groupFrSupportF;
	FR_SERVER_CONFIG_ADAPTIVE_FACE_ENROLL_t adaptiveFaceEnrollCnfg;
}FR_SERVER_CONFIG_VER4_t;

typedef struct
{
	BOOL 				enableFrServer;
	UINT16				port;
	FACE_CAPTURING_e 	faceCapturing;
	UINT16 				faceScanTimeOut;
	FR_MODE_e 			frMode;
	CHAR 				hostName[MAX_SERVER_ADDR_254];
	CHAR 				frServerIpAddress[MAX_IP_ADDRESS];
	UINT16 				identificationTimeOut;
	BOOL				frServerInterfaceSelectionF;
	COMM_PORT_e			frServerInterface;
	BOOL				enableFreeScanTimeOutF;
	BOOL				encryptionSsl;
	BOOL				groupFrSupportF;
	FR_SERVER_CONFIG_ADAPTIVE_FACE_ENROLL_t adaptiveFaceEnrollCnfg;
	#if !defined(DEVICE_IS_FMX)
	BOOL				frServerFaceAntiSpoofingF;		// Face Anti-Spoofing
	FR_FACE_ANTI_SPOOFING_MODE_e	frServerFaceAntiSpoofingMode;		// Face Anti-Spoofing Mode 0=Basic, 1=Moderate, 2=Advance
	FLOAT32				frServerFaceAntiSpoofingThreshold;	// Face Anti-Spoofing Threshold
	#endif // #if !defined(DEVICE_IS_FMX)

}FR_SERVER_CONFIG_VER5_t;

typedef struct
{
    BOOL 				enableFrServer;
    UINT16				port;
    FACE_CAPTURING_e 	faceCapturing;
    UINT16 				faceScanTimeOut;
    FR_MODE_e 			frMode;
    CHAR 				hostName[MAX_SERVER_ADDR_254];
    CHAR 				frServerIpAddress[MAX_IP_ADDRESS];
    UINT16 				identificationTimeOut;
    BOOL				frServerInterfaceSelectionF;
    COMM_PORT_e			frServerInterface;
    BOOL				enableFreeScanTimeOutF;
    BOOL				encryptionSsl;
    BOOL				groupFrSupportF;
    FR_SERVER_CONFIG_ADAPTIVE_FACE_ENROLL_t adaptiveFaceEnrollCnfg;
    #if !defined(DEVICE_IS_FMX)
    BOOL				frServerFaceAntiSpoofingF;		// Face Anti-Spoofing
    FR_FACE_ANTI_SPOOFING_MODE_e	frServerFaceAntiSpoofingMode;		// Face Anti-Spoofing Mode 0=Basic, 1=Moderate, 2=Advance
    FLOAT32				frServerFaceAntiSpoofingThreshold;	// Face Anti-Spoofing Threshold
    BOOL				conflictChkF;
    FLOAT32				conflictMatchThrshld;
    BOOL				frServerExceptionalFaceF;
    UINT16				AdeptiveMaxFaceCnt;
    #endif // #if !defined(DEVICE_IS_FMX)

}FR_SERVER_CONFIG_VER6_t;

typedef struct
{
	BOOL 				enableFrServer;
    UINT16				port;
    FACE_CAPTURING_e 	faceCapturing;
    UINT16 				faceScanTimeOut;
    FR_MODE_e 			frMode;
    CHAR 				hostName[MAX_SERVER_ADDR_254];
    CHAR 				frServerIpAddress[MAX_IP_ADDRESS];
    UINT16 				identificationTimeOut;
    BOOL				frServerInterfaceSelectionF;
    COMM_PORT_e			frServerInterface;
    BOOL				enableFreeScanTimeOutF;
    BOOL				encryptionSsl;
    BOOL				groupFrSupportF;
    FR_SERVER_CONFIG_ADAPTIVE_FACE_ENROLL_t adaptiveFaceEnrollCnfg;
    #if !defined(DEVICE_IS_FMX)
    BOOL				frServerFaceAntiSpoofingF;		// Face Anti-Spoofing
    FR_FACE_ANTI_SPOOFING_MODE_e	frServerFaceAntiSpoofingMode;		// Face Anti-Spoofing Mode 0=Basic, 1=Moderate, 2=Advance
    FLOAT32				frServerFaceAntiSpoofingThreshold;	// Face Anti-Spoofing Threshold
    BOOL				conflictChkF;
    FLOAT32				conflictMatchThrshld;
    BOOL				frServerExceptionalFaceF;
    UINT16				AdeptiveMaxFaceCnt;
	BOOL				showUnknownUserAck;
    #endif // #if !defined(DEVICE_IS_FMX)
}FR_SERVER_CONFIG_VER7_t;

#define MAX_HTTP_SERVER_PASSWORD_LEN_VER1   (16 + NULL_CHAR_SIZE)
typedef struct
{
	CHAR			httpServerUserId[MAX_HTTP_SERVER_USER_ID_LEN];
	CHAR			httpServerPassword[MAX_HTTP_SERVER_PASSWORD_LEN_VER1];
	CHAR			httpServerDirectory[MAX_HTTP_SERVER_DIRECTORY_LEN];
	BOOL			interfaceSelctionF;		// 0=Auto, 1=Manual
}HTTP_SERVER_CONFIG_VER1_t;

typedef struct
{
	CHAR  user[MAX_USER_TYPE][MAX_PASSWORD_SIZE];	// Password Per User
}PASSWORD_VER1_t;

typedef struct
{
        CHAR  displayPswd[MAX_USER_TYPE][MAX_PASSWORD_SIZE];	// Display Screen Password Per User NOTE we have not changed user size because previously it was common for both (Web & Display)
        CHAR  webPswd[MAX_USER_TYPE][MAX_WEB_PASSWORD_SIZE];	// Web Password Per User
        UINT16 PswdResetDaysCount[MAX_USER_TYPE];		// Password expiry days count
}PASSWORD_VER2_t;

//Job Costing Config
#define JOB_CODE_SIZE_VER1 (10 + NULL_CHAR_SIZE)
typedef struct
{
	UINT32				refId;
	CHAR 				jobCode[JOB_CODE_SIZE_VER1];
	CHAR				jobName[JOB_NAME_SIZE_UTF8];
	DATE_DDMMYY_t		startDate;
	DATE_DDMMYY_t		endDate;
}JCM_CONFIG_VER1_t;

#define MAX_FTP_USER_PASSWORD_40_BYTES	(40 + NULL_CHAR_SIZE)
typedef struct
{
	COMM_PORT_e 					interface;
	CHAR							ftpServerUrl[MAX_FTP_SERVER_URL];
	CHAR							ftpServerUseName[MAX_FTP_USER_NAME];
	CHAR							ftpServerUserPassword[MAX_FTP_USER_PASSWORD_40_BYTES];
	BOOL							autoUpdate;
	FIRMWARE_UPGRADE_BASED_ON_e	 	upgradeBasedOn;
	BOOL							interfaceSelctionF;
	CHAR							ftpServerResolveIpAddress[MAX_IP_ADDRESS];	// FTP Server URL
}BACKUP_AND_UPDATE_VER1_t;

#define CAPTURE_SNAPSHOT_PASSWORD_LENGTH_40_Bytes	(40+NULL_CHAR_SIZE)
typedef struct
{
	BOOL						interfaceSelectionF;	//0auto 1manual
	CHAR						captureSnapshotUrl[CAPTURE_SNAPSHOT_URL_LENGTH];
	CHAR						captureSnapshotUserName[CAPTURE_SNAPSHOT_USERNAME_LENGTH];
	CHAR						captureSnapshotPassword[CAPTURE_SNAPSHOT_PASSWORD_LENGTH_40_Bytes];
	CAPTURE_SNAPSHOT_SCH_t		captureSnapshotSchedule[MAX_CAPTURE_SNAPSHOT_SCH];
}IP_CAMERA_CONFIG_VER1_t;

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
}FILE_LOG_PARA_CONFIG_VER1_t;

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
}FILE_LOG_PARA_CONFIG_VER3_t;

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
}FILE_LOG_PARA_CONFIG_VER2_t;
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

	COMM_PORT_e 	httpInterface;
	CHAR			httpServerIpAddress[MAX_IP_ADDRESS];	// Http Server IP address
	CHAR        	httpServerHostName[MAX_SERVER_ADDR_254];	// Http Server Hostname
	SERVER_CONN_e 	httpServerConnection;					// Http Server Connection Mode
	UINT16			httpServerPort;							// Http Server port to listen ACMS message

	UINT8			type;
	BOOL			tcpEncryptionModeF;
	BOOL			httpEncryptionModeF;
	CHAR			TenantIdVyom[MAX_TENANT_ID];
	UINT8			unusedBytes[512];
}SERVER_PARA_VER1_t;

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

	COMM_PORT_e 	httpInterface;
	CHAR			httpServerIpAddress[MAX_IP_ADDRESS];	// Http Server IP address
	CHAR        	httpServerHostName[MAX_SERVER_ADDR_254];	// Http Server Hostname
	SERVER_CONN_e 	httpServerConnection;					// Http Server Connection Mode
	UINT16			httpServerPort;							// Http Server port to listen ACMS message

	UINT8			type;
	BOOL			tcpEncryptionModeF;
	BOOL			httpEncryptionModeF;
	CHAR			TenantIdVyom[MAX_TENANT_ID];
	UINT8			unusedBytes[512];

	UINT8			httpSyncRequestRetryTimer; //HTTP FTC sync retry timer.
}SERVER_PARA_VER2_t;

typedef struct
{
	PRINTER_TYPE_e			printerType;
	PRINTER_COMM_TYPE_e  		printerCommType;
	CHAR				companyName[MAX_PRINTEABLE_SIZE_UTF8];
	CHAR				companyAddr[MAX_ADDR_LINE_PRINT][MAX_PRINTEABLE_SIZE_UTF8];
	CHAR				punchLine[MAX_PRINTEABLE_SIZE_UTF8];
	READER_UART_BAUD_e		baudRate;
	UINT8				printReceiptCnt;
	BOOL				excludePriceFrmCpn;
}PRINTER_CONFIG_VER_1_t;

typedef struct
{
	CHAR				ipAddress[MAX_IP_ADDRESS];			// IP address
	CHAR				subnet[MAX_SUBNET_MASK];			// Subnet Mask
	CHAR				gateway[MAX_GATEWAY];				// Gateway IP address
	CHAR				macAddress[MAX_MAC_ADDRESS];		// MAC address
	CHAR				dns1[MAX_DNS_ADDRESS];				// DNS1 address
	CHAR				dns2[MAX_DNS_ADDRESS];				// DNS2 address
	IP_MODE_e			nwType;								// Type of network
    CHAR				ssid[MAX_SSID_NAME_OLD];			// SSID name
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
}WIFI_PARA_VER1_t;

typedef struct
{
    CHAR				ipAddress[MAX_IP_ADDRESS];			// IP address
    CHAR				subnet[MAX_SUBNET_MASK];			// Subnet Mask
    CHAR				gateway[MAX_GATEWAY];				// Gateway IP address
    CHAR				macAddress[MAX_MAC_ADDRESS];		// MAC address
    CHAR				dns1[MAX_DNS_ADDRESS];				// DNS1 address
    CHAR				dns2[MAX_DNS_ADDRESS];				// DNS2 address
    IP_MODE_e			nwType;								// Type of network
    CHAR				ssid[MAX_SSID_NAME_OLD];			// SSID name
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
}WIFI_PARA_VER2_t;

#endif // MXCONFIGOLD_H
