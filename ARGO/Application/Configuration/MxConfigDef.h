#if !defined MXCONFIGDEF_H
#define MXCONFIGDEF_H
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
//   Created By   : Neeraj Usadadiya
//   File         : MxConfigDef.h
//   Description  : This file consists of configuration related defines and
//					enums.
//
/////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include "../MxSysCommonDef.h"
#include "MxSlaveCommon.h"

//******** Defines and Data Types ****
#define CONFIG_TABLE_SINGLE_ROW_PER_PAGE		1				// Single Row Page Index
#define DOOR_PULSE_TIMER			SysBasicPara.doorPulseTimeFloat

#define MAX_SSA_USERS				1
#define MAX_USERS					(50000 + MAX_SSA_USERS)		// Available user in system + 1 SSA user
#define SSA_USER_IDX				(MAX_USERS - 1)
#define MAX_USER_DEMO				20
#define MAX_USER_SAMAY				100

#define ALPHANUM_ID_SIZE_10_BYTE	(10 + NULL_CHAR_SIZE)	// Alphanumeric UserId Size till V11R2
#define HOLIDAY_NAME_SIZE			(20 + NULL_CHAR_SIZE)	// Holiday Name Size

#define MAX_SSID_NAME_OLD			(32 + NULL_CHAR_SIZE + 1)
#define MAX_SSID_NAME				(64 + NULL_CHAR_SIZE + 1)	// SSID name for the WiFi
#define MAX_WPS_PIN_SIZE			(9 + NULL_CHAR_SIZE)
#define	MAX_PASSPHRESH_SIZE			(64 + NULL_CHAR_SIZE)	// Passphrase for the WiFi
#define MAX_WIFI_IDENTITY_SIZE		(50 + NULL_CHAR_SIZE)	// Identity Size
#define MAX_SERVER_ADDR_254			(253 + NULL_CHAR_SIZE)	//server address size
#define MAX_SERVER_ADDR				(40 + NULL_CHAR_SIZE)	//server address size
#define MAX_SERVER_PORT_DIGITS		5
#define MAX_VYOM_SERVER_ADDR		(253 + NULL_CHAR_SIZE)	//Vyom server address size
#define MAX_TENANT_ID				(10 + NULL_CHAR_SIZE)	//Tenant Id Size

#define MAX_PROXY_NAME_SIZE_VIDYAWAAN		(100 + NULL_CHAR_SIZE) 		// For VIDYAWAAN Server
#define MAX_USER_ID_SIZE_VIDYAWAAN			(100 + NULL_CHAR_SIZE) 		// For VIDYAWAAN Server
#define MAX_PASSWORD_SIZE_VIDYAWAAN			(40 + NULL_CHAR_SIZE)
#define CUSTOM_URL_SIZE_VIDYAWAAN			(200 + NULL_CHAR_SIZE)
#define MAX_LONGITUDE_LATITUDE				(8 + NULL_CHAR_SIZE)
#define MIFARE_CUSTOM_KEY_VIDYAWAAN			{0xAB,0xAB,0xAB,0xAB,0xAB,0xAB}

#define DEFLT_UHF_TAG_REDETECT_TIME	600

// Range
#define MIN_SERVER_PORT_VAL			1024
#define	MAX_SERVER_PORT_VAL			65535

#define	ETH_INTERFACE				"eth0"
#define	WIFI_INTERFACE				"wlan0"
#define	PPP_INTERFACE				"ppp0"

// TCP IP Server port related
#define	DFLT_ETH_IP_ADDR			"192.168.50.1"
#define	DFLT_SM_ADDR				"255.255.255.0"
#define	DFLT_WIFI_IP_ADDR			"192.168.10.1"
#define	DFLT_SERVER_IP_ADDR			"192.168.50.100"
#define DFLT_COMMAND_CTRL_IP_ADDR	"192.168.50.200"
#define DFLT_WEB_API_EVENTS_IP_ADDR	"192.168.1.124"
#define DFLT_ACMS_SERVER_PORT		11000
#define DFLT_COMMAND_CTRL_PORT		11000
#define DFLT_WEB_API_EVENTS_PORT	11000
#define DFLT_HTTP_SERVER_PORT		80
#define	DFLT_MAC_ADDR				"0:0:0:0:0:0"

// Vyom server DFLT Configuration
#define DFLT_HOST_NAME_VYOM_VER1	"matrixvyom.com"
#define DFLT_HOST_NAME_VYOM			"device.matrixvyom.com"
#define DFLT_PORT_NO_VYOM			15025
#define DFLT_TENANT_ID_VYOM			"0"

#define MAX_QUERY_SIZE				50

// Modem Profile Configuration
#define MODEM_PROVIDER_NAME_SIZE	(15 + NULL_CHAR_SIZE)
#define MODEM_DIAL_NUM_SIZE			(16 + NULL_CHAR_SIZE)
#define MODEM_USER_NAME_SIZE		(40 + NULL_CHAR_SIZE)
#define MODEM_PASSWORD_SIZE			(40 + NULL_CHAR_SIZE)
#define MODEM_APN_NAME_SIZE			(40 + NULL_CHAR_SIZE)
#define MAX_MODEM_PROFILE			5

// Invalid Identity Defines
#define INVALID_USER_ID				0				// Invalid User ID
#define INVALID_CARD_ID				0				// Invalid Card ID
#define ACMS_INVALID_VISITOR_IDX	0

#define UNKNOWN_USER_IDX			0xffff			// Unknown User Index
#define	INVALID_USER				0xffff			// Invalid User
#define	INVALID_VISITOR_IDX			0xffff			// Invalid Visitor
#define	INVALID_SPL_FN				0xff			// Invalid Special Function
#define	INVALID_READER	 			0xff			// Invalid Reader
#define	INVALID_GREETING_IDX		0xff			// Invalid greeting index
#define INVALID_FACE_TMPLT_IDX		0xff			// Invalid Face Template Index

#define INVALID_SCHEDULE_IDX		0xff
#define INVALID_SCHEDULE_ID			0xff

#define	NOT_IN_BLOCKED_LIST			0xff			// Not in Blocked list
#define	NOT_IN_TWO_PRSN_GRP			0xff			// Not in 2 Person Group
#define	NOT_IN_FIRST_IN_LIST		0xff			// Not in First In Group

#define ALL_USER_FP					0xff			// All User Finger Print
#define DURESS_FP_IDX_NONE				0xFF	//Default User Duress Fp Index 
#define INVALID_FP_IDX				0xFF	//Invalid Fp Index 
#define TWO_PERSON_GRP_NONE			0xff			// two person group id is none

// Feature Related Defines
#define MAX_TWO_PERSON_MEMBERS		20				// Member available in each group of 2 - person
#define MAX_TWO_PERSON_MEMBERS_VER1	6				// Member available in each group of 2 - person

#define MAX_TWO_PERSON_GROUP		16				// Maximum group available in 2 - person

#define OLD_MAX_FIRST_IN_MEMBERS	25
#define MAX_FIRST_IN_MEMBERS_99		99
#define MAX_FIRST_IN_MEMBERS		999             // First In User Maximum avaible members
#define MAX_FIRST_IN_LISTS			4				// First In User Maximum available lists

#define MAX_HOLIDAYS				32				// Holiday Schedule Maximum availble days

#define MAX_BLOCKED_USER			99				// Blocked User Maximum user avaible

#define MAX_NTP_ADDRESS				(25 + NULL_CHAR_SIZE)	// Maximum NTP Address Size
#define MAX_USER_DEF_NTP_ADDRESS	(40 + NULL_CHAR_SIZE)	// Maximum User Default NTP Address Size
#define MAX_NTP_ADDRESS_UTF8		(MAX_NTP_ADDRESS - NULL_CHAR_SIZE)*4 +	NULL_CHAR_SIZE
#define MAX_USER_DEF_NTP_ADDRESS_UTF8 (MAX_USER_DEF_NTP_ADDRESS-1)*4 + NULL_CHAR_SIZE

#define MAX_UTZ						75				// Maximum UTZ
#define	MAX_VISITORS				50000			// Maximum Visitor
#define MAX_TIME_TRIG_FUNC			99				// Maximum Time Triger Function

#define	MAX_PIN_DIGITS				15				// Maximum Pin Digits
#define	MAX_PIN_SIZE				(MAX_PIN_DIGITS + NULL_CHAR_SIZE)	// Maximum Pin Size

#define MAX_PIN_CHANGE_CODE_DIGITS	6
#define MAX_PIN_CHANGE_CODE_SIZE	(MAX_PIN_CHANGE_CODE_DIGITS + NULL_CHAR_SIZE)

#define MAX_SP_FUNC_CARD 			4				// maximum cards associated with sepcial function
#define	MAX_USER_CARD 				2				// Maximum cards associated with user

#define MAX_IO_LINK					10
#define MAX_IO_LINK_EXT_DEVICE		255

#define MAX_USER_ID_DIGITS			8
#define MAX_SPL_FN_ID_DIGITS		2

#define OFFSET_SCHEDULE_ID			1

#define DEFLT_SM125_RADF_VALUE_ENTRY_READER		3	// Equivalent to (RADF = 96, 123.71 KHz)
#define DEFLT_SM125_RADF_VALUE_EXIT_READER		3	// Equivalent to (RADF = 96, 123.71 KHz)

#define DEFLT_AMB_RETRYWAITTIMER	120
#define DEFLT_AMB_TEMPCOUNT	30
#define DEFLT_AMB_TEMPDEVIATION	0.5
#define DEFLT_AMB_AMBCYCLE	5
#define DEFLT_AMB_AMBDEVIATION	0.5
#define DEFLT_AMB_SHOWAMBONSTATUS	FALSE
#define DEFLT_AMB_CHECKAMBREADING	TRUE
#define DEFLT_AMB_AMBUSRDEVIATION	0.2
// RADF value for EM prox in CER
#define DEFLT_SM125_RADF_VALUE_CER_READER		6 // Equivalent to (RADF = 93, 127.66 KHz)
#define MAX_ACMS_RADF_VALUE						8

#define MAX_GREETING_CONFIG						4
#define GREETING_MSG_SIZE						22		//21+NULL
#define GREETING_MSG_SIZE_UTF8					(GREETING_MSG_SIZE -1)*4 +1

// e-Canteen related Defines
#define INVALID_CAFE_MENU_CODE			0xFF
#define INVALID_CAFE_ITEM_CODE			0xFF
#define SIZE_CAFE_ITEM_NAME				10		// 9 + null
#define SIZE_CAFE_ITEM_NAME_UTF8		(SIZE_CAFE_ITEM_NAME -1)*4 + 1
#define SIZE_CAFE_MENU_NAME				16		// 15 + null
#define MAX_PRINTEABLE_SIZE				33		//32+NULL
#define MAX_PRINTEABLE_SIZE_UTF8		(MAX_PRINTEABLE_SIZE -1)*4 +1
#define MAX_ADDR_LINE_PRINT				4
#define MAX_CAFE_ITEM_CODE				255
#define MAX_CAFE_ITEM_DISCOUNT_LEVEL	4
#define MAX_CAFE_MENU_CODE				99
#define MAX_CAFE_MENU_ITEMS				99
#define MAX_CAFE_MENU_ITEM_VER2	    	30
#define MAX_CAFE_MENU_SCHEDULE			48
#define MAX_CAFE_BALANCE_SIZE			10
#define DOUBLE_PRECISION_FACTOR			100
#define LOCATON_PRECISION_FACTOR		10000

// Smart Card Based Route Access Related Defines
#define MAX_SMART_ROUTE_ACCESS			99
#define MAX_SMART_ROUTED_LEVEL			75
#define USER_SMART_ROUTE_NONE			0xFF
#define DEFAULT_DOOR_LEVEL				0xFF
#define DEFAULT_MIN_ROUTE_LEVEL			1
#define DEFAULT_MAX_ROUTE_LEVEL			75

//Smart Card Related Defines
#define DESFIRE_CARD_KEY_SIZE			16

// DVR-NVR intigration related
#define MAX_CAMERA					24
#define MAX_EMAIL_ADDR_SIZE			61		//60+null char
#define MAX_EMAIL_ADDR_SIZE_UTF8	((MAX_EMAIL_ADDR_SIZE-1)*4)+1
#define MAX_DVR_EVT_ACTN_CONFIG		20		//maxumum 20 event and action can be configured
#define MAX_CAMERA_TILL_V6R3		16

#define WEB_API_MATRIX_USER_ID				"matrix"
#define WEB_API_MAIRIX_PASSWORD				"Matrix@m394"

//Vehicle Access Related
#define MAX_VAM_CONFIG				12000
#define INVALID_VAM_CONFIG_INDEX	0xFFFF
#define VAM_SMART_ROUTE_ID_NONE		0xFF
#define VAME_ACCESS_LEVEL_NONE		0xFF

#define MAX_JCM_CONFIG				1000
#define INVALID_JOB_REF_ID			0
#define NONE_JOB_REF_ID				0xFFFFFFFF
#define DEFAULT_JOB_REF_ID			0xFFFFFFFE
#define CONTINUE_JOB_REF_ID			0xFFFFFFFD
#define INVALID_JCM_CONFIG_INDEX	0xFFFF
#define DEFAULT_RETAIN_JOB_CODE		0xFFFFFFFF
#define NONE_JOB_CODE_CONFIG_INDEX	1000
#define DEFAULT_JOB_CODE_CONFIG_INDEX	1001
#define CONTINUE_JOB_CODE_CONFIG_INDEX	1002

#define EXTENTION_NO_PART1_LEN		4
#define EXTENTION_NO_PART2_LEN		2
#define EXTENTION_NO_LEN			(EXTENTION_NO_PART1_LEN+EXTENTION_NO_PART2_LEN+1)		//include null char
#define MAX_DOOR_MODE_SCHEDULE		15

#define MAX_USER_BALANCE_STR_LENGTH	13

//cadence related values
#define SUCCESS_CAD_ON_LONG		6
#define SUCCESS_CAD_ON_MEDIUM	4
#define SUCCESS_CAD_ON_SHORT	2
#define SUCCESS_CAD_ON_SHORTEST	1

#define SUCCESS_CAD_CYCLES				1
#define FAIL_CAD_DURATION				2
#define FAIL_CAD_DURATION_SHORTEST		1

#define	FAIL_CAD_CYCLES_LONG	3
#define	FAIL_CAD_CYCLES_MEDIUM	2
#define	FAIL_CAD_CYCLES_SHORT	1

#define MAX_USER_GRP_DIGITS		3
#define INVALID_USER_GRP		0
#define MAX_LENGTH_TEN_ID_STR	14

#define FACE_ENROLL_CNT_MIN			0
#define FACE_ENROLL_CNT_MAX			29
#define FACE_ENROLL_CNT_DFLT		10 //0-9
//FTP Firmware Upgrade
#define MAX_FTP_SERVER_URL		(253 + NULL_CHAR_SIZE)
#define MAX_FTP_USER_NAME		(40 + NULL_CHAR_SIZE)
#define MAX_FTP_USER_PASSWORD	(128 + NULL_CHAR_SIZE)
#define BENCHMARK_FW_STR_LEN	(50)

// Bluetooth Support
#define BT_UUID_SIZE			16						// 128 bits

// http server config para
#define MAX_HTTP_SERVER_USER_ID_LEN			(20 + NULL_CHAR_SIZE)
#define MAX_HTTP_SERVER_PASSWORD_LEN		(128 + NULL_CHAR_SIZE)
#define MAX_HTTP_SERVER_DIRECTORY_LEN		(150 + NULL_CHAR_SIZE)

#define DFLT_HTTP_PORT						80
#define DFLT_HTTPS_PORT						443

#define MAX_IP_CAMERA_SCH_NAME				(120+NULL_CHAR_SIZE)
#define MAX_CAPTURE_SNAPSHOT_SCH			20
#define CAPTURE_SNAPSHOT_URL_LENGTH			(253+NULL_CHAR_SIZE)
#define CAPTURE_SNAPSHOT_USERNAME_LENGTH	(40+NULL_CHAR_SIZE)
#define CAPTURE_SNAPSHOT_PASSWORD_LENGTH	(128+NULL_CHAR_SIZE)

#define FR_STREAM_API_LENGTH			(253+NULL_CHAR_SIZE)
#define FR_STREAM_API_USERNAME_LENGTH	(40+NULL_CHAR_SIZE)
#define FR_STREAM_API_PASSWORD_LENGTH	(40+NULL_CHAR_SIZE)

#define MAX_EVENT_PICS_COUNT		1000

// Repeat Days - VMS
#define MAX_REPEAT_DAYS			7
#define NO_REPEAT_DAY			0

#define CB_BLE_NAME_SIZE			(20 + NULL_CHAR_SIZE)	// BLE Name Size
#define ATOM_BLE_NAME_SIZE			(20 + NULL_CHAR_SIZE)	// BLE Name Size

#define HTTP_SYNC_DATE_LENGTH_MAX (14+1) //DDMMYYYYHHMMSS + 1
#define DFLT_HTTP_SYNC_REQUEST_RETRY_TIMER (2)

//door application type direct door or panel door
typedef enum
{
	DIRECT_DOOR_APPL,
	PANEL_DOOR_APPL,

	MAX_DOOR_APPL_TYPE,
}DOOR_APPL_TYPE_e;

// QR MODE

typedef enum
{
	ACCESS_MODE_QR_CODE,				// QR code
	ACCESS_MODE_QR_CODE_THEN_BIO,		// QR code then Bio
	ACCESS_MODE_QR_CODE_THEN_CARD,		// QR code then Card
	ACCESS_MODE_MOBILE_THEN_PIN,		// Mobile then Pin
	ACCESS_MODE_MOBILE_THEN_FACE,		// Mobile then Face

	MAX_ACCESS_MODE_QR_CODE
}API_ACCESS_MODE_e;

// Special Function Card index
typedef enum
{
	CARD1 = 0,						// Card Index 1
	CARD2,							// Card Index 2
	CARD3,							// Card Index 3
	CARD4,							// Card Index 4

	CARD_NONE						// Card Index None
}CARD_INDEX_e;

#define GENERATE_ENROLLMENT_TYPE(GEN_TYPE) \
		GEN_TYPE(ENROLL_READ_ONLY_CARD) \
		GEN_TYPE(ENROLL_SMART_CARD) \
		GEN_TYPE(ENROLL_BIOMETRIC_ONLY) \
		GEN_TYPE(ENROLL_BIOMETRIC_THEN_CARD) \
        GEN_TYPE(ENROLL_DUMMY1) \
        GEN_TYPE(ENROLL_DUMMY2) \
        GEN_TYPE(ENROLL_MOBILE_DEVICE) \
        GEN_TYPE(ENROLL_FACE_ONLY) \
        GEN_TYPE(ENROLL_DURESS_FP) \

// Enrollment Type
typedef enum
{
	GENERATE_ENROLLMENT_TYPE(GENERATE_ENUM)
	MAX_ENROLLMENT_TYPE,
}ENROLLMENT_TYPE_e;

#define GENERATE_ACCESS_MODE(GEN_TYPE) \
		GEN_TYPE(CARD_ONLY) \
		GEN_TYPE(FINGER_PRINT_ONLY) \
		GEN_TYPE(CARD_PIN) \
		GEN_TYPE(FINGER_PRINT_PIN) \
		GEN_TYPE(CARD_FINGER_PRINT) \
		GEN_TYPE(CARD_FINGER_PRINT_PIN) \
		GEN_TYPE(ANY_CREDENTIAL) \
		GEN_TYPE(ACCESS_MODE_PALM) \
		GEN_TYPE(ACCESS_MODE_PALM_PIN) \
		GEN_TYPE(ACCESS_MODE_CARD_PALM) \
		GEN_TYPE(ACCESS_MODE_CARD_PIN_PALM) \
		GEN_TYPE(ACCESS_MODE_PALM_GRP_OPT) \
		GEN_TYPE(FINGER_PRINT_THEN_CARD) \
		GEN_TYPE(ACCESS_MODE_PALM_THEN_CARD) \
		GEN_TYPE(ACCESS_MODE_NONE) \
		GEN_TYPE(ACCESS_MODE_FACE) \
		GEN_TYPE(ACCESS_MODE_FACE_CARD) \
		GEN_TYPE(ACCESS_MODE_FACE_PIN) \
		GEN_TYPE(ACCESS_MODE_FACE_FINGER_PRINT) \
		GEN_TYPE(ACCESS_MODE_BLE) \
		GEN_TYPE(ACCESS_MODE_CARD_THEN_BIOMETRIC) \

// Credential Access Mode
typedef enum
{
	GENERATE_ACCESS_MODE(GENERATE_ENUM)
	MAX_ACCESS_MODE,
}ACCESS_MODE_e;

#define GENERATE_OUTPUT_PORT(GEN_TYPE) \
		GEN_TYPE(AUX_OUT_PORT) \
		GEN_TYPE(DOOR_LOCK_RELAY_PORT) \

// Output Ports
typedef enum
{
	GENERATE_OUTPUT_PORT(GENERATE_ENUM)
	MAX_OUTPUT_PORT,
}OUTPUT_PORT_e;

#define GENERATE_INPUT_PORT(GEN_TYPE) \
		GEN_TYPE(USER_ALLOWED_TRIGGER) \
		GEN_TYPE(USER_DENIED_TRIGGER) \
		GEN_TYPE(AUX_IN_PORT) \
		GEN_TYPE(USER_ALLOWED_WITH_DURESS) \
		GEN_TYPE(INTERCOM_PANIC_PORT) \
		GEN_TYPE(ZONE_EMPTY_PORT) \

// Input Ports (logical and physical)
typedef enum
{
	GENERATE_INPUT_PORT(GENERATE_ENUM)
	MAX_INPUT_PORT,
}INPUT_PORT_e;

#define GENERATE_OUTPUT_TYPE(GEN_TYPE) \
		GEN_TYPE(OP_PULSE) \
		GEN_TYPE(OP_INTERLOCK) \
		GEN_TYPE(OP_LATCH) \
		GEN_TYPE(OP_TOGGLE) \

// Output type
typedef enum
{
	GENERATE_OUTPUT_TYPE(GENERATE_ENUM)
	MAX_OUTPUT_TYPE,
}OUTPUT_TYPE_e;

#define GENERATE_NTP_MODE(GEN_TYPE) \
		GEN_TYPE(AUTO_TIME) \
		GEN_TYPE(MANUAL_TIME) \

// Network Time Protocol Mode
typedef enum
{
	GENERATE_NTP_MODE(GENERATE_ENUM)
	MAX_NTP_MODE							// Time Protocol Mode Invalid
}NTP_MODE_e;

// Network Time Protocol Address
typedef enum
{
	NTP_ADDR_NTP1_CS_WISC_EDU,				// ntp1.cs.wisc.edu
	NTP_ADDR_TIME_WINDOWS_COM,				// time.windows.com
	NTP_ADDR_TIME_NIST_GOV,					// time.nist.gov
	MAX_NTP_ADDR							// Maximum Address Available
}NTP_ADDR_e;

#define GENERATE_TWO_PERSON_MODE(GEN_TYPE) \
		GEN_TYPE(PRIMARY_MUST) \
		GEN_TYPE(PRIMARY_AND_SECONDARY_MUST) \

// 2 person- user group options
typedef enum
{
	GENERATE_TWO_PERSON_MODE(GENERATE_ENUM)
	MAX_TWO_PERSON_MODE,
}TWO_PERSON_MODE_e;

#define GENERATE_BLOCK_REASON(GEN_TYPE) \
		GEN_TYPE(REASON_UNKNOWN) \
		GEN_TYPE(REASON_ABSENTEE) \
		GEN_TYPE(REASON_ATTEMPTS) \
		GEN_TYPE(REASON_COUNT_CONTROL) \
		GEN_TYPE(REASON_INVALID_PIN) \

// Block user reasons
typedef enum
{
	GENERATE_BLOCK_REASON(GENERATE_ENUM)
	MAX_BLOCK_REASON,
}BLOCK_REASON_e;

#define GENERATE_DST_MODE(GEN_TYPE) \
		GEN_TYPE(DST_MODE_DISABLE) \
		GEN_TYPE(DST_MODE_DAY_WISE) \
		GEN_TYPE(DST_MODE_DATE_WISE) \

// Day Light Saving Mode
typedef enum
{
	GENERATE_DST_MODE(GENERATE_ENUM)
	MAX_DST_MODE,
}DST_MODE_e;

// Holiday schedule index based on toltal supported schedules
typedef enum
{
	HOLIDAY_SCHEDULE_1 = 0,
	HOLIDAY_SCHEDULE_2,
	HOLIDAY_SCHEDULE_3,
	HOLIDAY_SCHEDULE_4,

	MAX_HOLIDAY_SCHEDULE,
}HOLIDAY_SCHEDULE_INDEX_e;

#define GENERATE_COMM_PORT(GEN_TYPE) \
		GEN_TYPE(ETHERNET_PORT) \
		GEN_TYPE(WIFI_PORT) \
		GEN_TYPE(PPP_PORT) \
		GEN_TYPE(INVALID_PORT) \

// Communication enumeration
typedef enum
{
	GENERATE_COMM_PORT(GENERATE_ENUM)
	MAX_PORT = 3,
}COMM_PORT_e;

#define GENERATE_IP_MODE(GEN_TYPE) \
		GEN_TYPE(IP_STATIC) \
		GEN_TYPE(IP_DYNAMIC) \

typedef enum
{
	GENERATE_IP_MODE(GENERATE_ENUM)
	MAX_IP_MODE,
}IP_MODE_e;

#define GENERATE_SEC_TYPE(GEN_TYPE) \
		GEN_TYPE(NO_SEC_TYPE) \
		GEN_TYPE(WEP_TYPE) \
		GEN_TYPE(WPA_TYPE) \
		GEN_TYPE(WPA2_TYPE) \
		GEN_TYPE(WPA_ENTERPRISE) \
		GEN_TYPE(WPA2_ENTERPRISE) \

typedef enum
{
	GENERATE_SEC_TYPE(GENERATE_ENUM)
	MAX_SEC_TYPE,
}SEC_TYPE_e;

#define GENERATE_WIFI_AUTH_TYPE(GEN_TYPE) \
		GEN_TYPE(AUTH_TYPE_AUTO) \
		GEN_TYPE(AUTH_TYPE_TLS) \
		GEN_TYPE(AUTH_TYPE_LEAP) \
		GEN_TYPE(AUTH_TYPE_FAST) \
		GEN_TYPE(AUTH_TYPE_TUNNELED_TLS) \
		GEN_TYPE(AUTH_TYPE_PEAP) \

typedef enum
{
	GENERATE_WIFI_AUTH_TYPE(GENERATE_ENUM)
	MAX_AUTH_TYPE,
}WIFI_AUTH_TYPE_e;

#define GENERATE_PEAP_VER(GEN_TYPE) \
		GEN_TYPE(PEAP_VER_AUTO) \
		GEN_TYPE(PEAP_VER_0) \
		GEN_TYPE(PEAP_VER_1) \
		GEN_TYPE(PEAP_LABEL_1) \

typedef enum
{
	GENERATE_PEAP_VER(GENERATE_ENUM)
	MAX_PEAP_VER,
}PEAP_VER_e;

#define GENERATE_INNER_AUTH(GEN_TYPE) \
		GEN_TYPE(INNER_AUTH_AUTO) \
		GEN_TYPE(INNER_AUTH_MSCHAPV2) \
		GEN_TYPE(INNER_AUTH_MD5) \
		GEN_TYPE(INNER_AUTH_GTC) \

typedef enum
{
	GENERATE_INNER_AUTH(GENERATE_ENUM)
	MAX_INNER_AUTH,
}INNER_AUTH_e;

#define GENERATE_ENCRYPTION(GEN_TYPE) \
		GEN_TYPE(NO_ENCRYPTION) \
		GEN_TYPE(WEP_64_BIT) \
		GEN_TYPE(WEP_128_BIT) \
		GEN_TYPE(WPA2_AEC) \
		GEN_TYPE(WPA2_TKIP) \

typedef enum
{
	GENERATE_ENCRYPTION(GENERATE_ENUM)
	MAX_ENCRYPTION,
}ENCRYPTION_e;

#define GENERATE_WIFI_KEY_MGMT(GEN_TYPE) \
		GEN_TYPE(WPA_PSK_KEY_MGMT) \
		GEN_TYPE(WPA_EAP_KEY_MGMT) \

typedef enum
{
	GENERATE_WIFI_KEY_MGMT(GENERATE_ENUM)
	MAX_KEY_MGMT,
}WIFI_KEY_MGMT_e;

#define GENERATE_WPS_TRIGER_TYP(GEN_TYPE) \
		GEN_TYPE(WPS_TRIGER_NONE) \
		GEN_TYPE(WPS_TRIGER_PUSH_BUTTON) \
		GEN_TYPE(WPS_TRIGER_PIN_FROM_DEVICE) \
		GEN_TYPE(WPS_TRIGER_PIN_FROM_ACCESS_POINT) \

typedef enum
{
	GENERATE_WPS_TRIGER_TYP(GENERATE_ENUM)
	MAX_WPS_TRIGER_TYP,
}WPS_TRIGER_TYP_e;

#define GENERATE_MODEM_TYPE(GEN_TYPE) \
		GEN_TYPE(MODEM_GSM) \
		GEN_TYPE(MODEM_CDMA) \

typedef enum
{
	GENERATE_MODEM_TYPE(GENERATE_ENUM)
	MAX_MODEM_TYPE,
}MODEM_TYPE_e;

#define GENERATE_SERVER_CONN(GEN_TYPE) \
		GEN_TYPE(SERVER_CONN_IP_ADDR) \
		GEN_TYPE(SERVER_CONN_HOST_NAME) \

typedef enum
{
	GENERATE_SERVER_CONN(GENERATE_ENUM)
	MAX_SERVER_CONN,
}SERVER_CONN_e;

// Special Function List
typedef enum
{
	CLK_IN_ENTRY_OFFICIAL_WORK,						// Clock-IN as Entry after Official work
	CLK_OUT_EXIT_OFFICIAL_WORK,						// Clock-OUT as exit for Official work
	SHORT_LEAVE_MARKING_IN,							// Short Leave IN
	SHORT_LEAVE_MARKING_OUT,						// Short Leave OUT(single user)
	MARK_CLOCK_IN,									// Clock-IN
	MARK_CLOCK_OUT,									// Clock-OUT
	CLK_IN_MARKING_BREAK,							// Clock-IN as Entry after Break
	CLK_OUT_MARKING_BREAK,							// Clock-OUT as exit for Break
	OVER_TIME_MARKING_IN,							// Clock-IN for over-time
	OVER_TIME_MARKING_OUT,							// Clock-OUT after over-time
	ENROL_USER,										// Enrolment mode activated for user
	ENROL_SPECIAL_CARDS,							// Enrolment mode activated for special cards
	ENROL_MODE_DELETE_DATA,							// Enrolment mode activated for delete data
	LATE_START_MARKING_ACT,							// Late start of working hours activated
	LATE_START_MARKING_DEACT,						// Late start of working hours deactivated
	EARLY_CLOSING_MARKING_ACT,						// Early closing of working hours activated
	EARLY_CLOSING_MARKING_DEACT,					// Early closing of working hours deactivated
	LOCK_DOOR,										// Function Lock the Door
	UNLOCK_DOOR,									// Unlock the Door
	NORMAL_DOOR,									// Normalise the door
	CLEAR_SYS_ALARM,								// Clear system alarm
	ITEM_SOLD_OUT,									// Item SoldOut
	ITEM_AVAILABLE,									// Item Available

	MAX_SPECIAL_FUNC								// Invalid
}SPECIAL_FUNCTIONS_e;

// Special Function List For Panel Door
typedef enum
{
	PD_CLK_IN_ENTRY_OFFICIAL_WORK,				// 0=	Clock-IN as Entry for Official work
	PD_CLK_OUT_EXIT_OFFICIAL_WORK,				// 1=	Clock-OUT as exit for Official work
	PD_SHORT_LEAVE_MARKING_IN,					// 2=	Short Leave IN
	PD_SHORT_LEAVE_MARKING_OUT,				// 3=	Short Leave OUT
	PD_MARK_CLOCK_IN,							// 4=	Clock-IN
	PD_MARK_CLOCK_OUT,							// 5=	Clock-OUT
	PD_CLK_IN_MARKING_BREAK,					// 6=	Clock-IN as Entry after Break
	PD_CLK_OUT_MARKING_BREAK,					// 7=	Clock-OUT as exit for Break
	PD_OVER_TIME_MARKING_IN,					// 8=	Clock-IN for over-time
	PD_OVER_TIME_MARKING_OUT,					// 9=	Clock-OUT after over-time
	PD_GUARD_TOUR_ACT,							// 10=	Guard Tour
	PD_PANIC_ALARM_DOOR,						// 11=	Panic alarm
	PD_ENROL_USER,								// 12=	User Enrolment
	PD_ENROL_SPECIAL_CARDS,					// 13=	special function card Enrolment
	PD_ENROL_MODE_DELETE_DATA,					// 14=	Delete credentials
	PD_LATE_START_MARKING_ACT,					// 15=	Activate Late start of working hours
	PD_LATE_START_MARKING_DEACT,				// 16=	Deactivate Late start of working hours
	PD_EARLY_CLOSING_MARKING_ACT,				// 17=	Activate Early closing of working hours
	PD_EARLY_CLOSING_MARKING_DEACT,			// 18=	Deactivate Early closing of working hours
	PD_CHECK_USER_PROFILE_FROM_CARD,			// 19=
	PD_DND_ACT,								// 20=	Activate DND
	PD_DND_DEACT,								// 21=	Deactivate DND
	PD_DEADMAN_ZONE_ACT,						// 22=	Activate Dead Man zone monitoring
	PD_DEADMAN_ZONE_DEACT,						// 23=	Deactivate Dead Man zone monitoring
	PD_LOCK_DOOR,								// 24=	Lock the Door
	PD_UNLOCK_DOOR,							// 25=	Unlock the Door
	PD_NORMAL_DOOR,							// 26=	Normalise the door
	PD_LOCK_ZONE,								// 27=
	PD_UNLOCK_ZONE,							// 28=
	PD_NORMAL_ZONE,							// 29=
	PD_MUTE_ALARM_DOOR,						// 30=
	PD_MUTE_ALARM_MASTER,						// 31=
	PD_RESET_DC_AUX_OUT,						// 32=
	PD_RESET_MC_ALARM_OUT,						// 33=
	PD_ARM_DOOR,								// 34=	Arm single Door
	PD_DISARM_DOOR,							// 35=	Disarm single Door
	PD_ARM_ZONE,								// 36=	Arm all doors in a zone
	PD_DISARM_ZONE,							// 37=	disarm all doors i a zone
	PD_MAX_SPECIAL_FUNC_VER1,
	PD_CLEAR_SYS_ALARM = PD_MAX_SPECIAL_FUNC_VER1,// 38=
	PD_MAX_SPECIAL_FUNC,

	PD_SHOW_YOUR_CRED							// NOTE: Show ur Cread is not a special function it's used								
											//       in DATA Center Application for Display message on a screen.
}PD_SPECIAL_FUNCTIONS_e;

// Time Stamping Configuration enumeration
typedef enum
{
	TIME_STAMP_ON_ALLOW = 0,
	TIME_STAMP_ON_DENY,
	TIME_STAMP_ON_ALLOW_DENY
}TIME_STAMP_TRIGGER_e;

#define GENERATE_CAFE_DISCNT_LEVEL(GEN_TYPE) \
		GEN_TYPE(CAFE_DISCNT_LEVEL_NONE) \
		GEN_TYPE(CAFE_DISCNT_LEVEL_1) \
		GEN_TYPE(CAFE_DISCNT_LEVEL_2) \
		GEN_TYPE(CAFE_DISCNT_LEVEL_3) \
		GEN_TYPE(CAFE_DISCNT_LEVEL_4) \

typedef enum
{
	GENERATE_CAFE_DISCNT_LEVEL(GENERATE_ENUM)
	CAFE_DISCNT_LEVEL_MAX,
}CAFE_DISCNT_LEVEL_e;

#define GENERATE_USER_BLOCK_FOR_CAFE(GEN_TYPE)\
		GEN_TYPE(NOT_BLOCKED)\
		GEN_TYPE(BLOCKED_MONTHLY_CONSUPTION)\
		GEN_TYPE(BLOCKED_DAILY_LIMIT)\
		GEN_TYPE(BLOCKED_INSUFFICIENT_BALANCE)\

typedef enum
{
	GENERATE_USER_BLOCK_FOR_CAFE(GENERATE_ENUM)
	MAX_BLOCK_FOR_CAFE,
}USER_BLOCK_FOR_CAFE_e;

#define GENERATE_USER_PAYMENT_MODE(GEN_TYPE) \
		GEN_TYPE(PREPAID_DEVICE_SIDE) \
		GEN_TYPE(POSTPAID) \
		GEN_TYPE(PREPAID_SERVER_SIDE)\

typedef enum
{
	GENERATE_USER_PAYMENT_MODE(GENERATE_ENUM)
	MAX_PAYMENT_MODE,
}USER_PAYMENT_MODE_e;

#define GENERATE_SMART_ROUTE_ORDER_TYPE(GEN_TYPE) \
		GEN_TYPE(LEVEL_1_IS_LOWEST) \
		GEN_TYPE(LEVEL_1_IS_HIGHEST) \

typedef enum
{
	GENERATE_SMART_ROUTE_ORDER_TYPE(GENERATE_ENUM)
	MAX_SMART_ROUTE_ORDER,
}SMART_ROUTE_ORDER_TYPE_e;

#define GENERATE_DVR_CONFIG_EVENT_TYPE(GEN_TYPE) \
		GEN_TYPE(DVR_CONFIG_EVENT_USER_ALLOWED) \
		GEN_TYPE(DVR_CONFIG_EVENT_USER_DENIED) \
		GEN_TYPE(DVR_CONFIG_EVENT_USER_NOT_IDENTIFY) \
		GEN_TYPE(DVR_CONFIG_EVENT_AUX_IN_CHANGE) \
		GEN_TYPE(DVR_CONFIG_EVENT_DURESS_DETECTION) \
		GEN_TYPE(DVR_CONFIG_EVENT_DEAD_MAN) \
		GEN_TYPE(DVR_CONFIG_EVENT_DOOR_PANIC_ALARM) \
		GEN_TYPE(DVR_CONFIG_EVENT_DOOR_ABNORMAL_ALARM) \
		GEN_TYPE(DVR_CONFIG_EVENT_DOOR_FORCE_OPEN) \
		GEN_TYPE(DVR_CONFIG_EVENT_TAMPER_ALARM) \
		GEN_TYPE(DVR_CONFIG_EVENT_INTERCOM_PANIC) \

// DVR-NVR intigration related
typedef enum
{
	GENERATE_DVR_CONFIG_EVENT_TYPE(GENERATE_ENUM)
	MAX_DVR_CONFIG_EVENT_TYPE,
}DVR_CONFIG_EVENT_TYPE_e;

typedef enum
{
	DVR_SEND_EVENT_USER_ALLOWED = 0,
	DVR_SEND_EVENT_USER_DENIED,
	DVR_SEND_EVENT_USER_NOT_IDENTIFY,
	DVR_SEND_EVENT_AUX_IN_CHANGE,
	DVR_SEND_EVENT_DURESS_DETECTION,
	DVR_SEND_EVENT_DEAD_MAN,					//this is not applicable for directdoor
	DVR_SEND_EVENT_DOOR_PANIC_ALARM,
	DVR_SEND_EVENT_DOOR_ABNORMAL_ALARM,
	DVR_SEND_EVENT_DOOR_FORCE_OPEN,
	DVR_SEND_EVENT_TAMPER_ALARM,
	DVR_SEND_EVENT_INTERCOM_PANIC,
	DVR_SEND_EVENT_VISITOR_ALLOWED,
	DVR_SEND_EVENT_VISITOR_DENIED,
	MAX_DVR_SEND_EVENT_TYPE,
}DVR_SEND_EVENT_TYPE_e;

#define GENERATE_DVR_ACTION_TYPE(GEN_TYPE) \
		GEN_TYPE(DVR_ACTION_RECORDING) \
		GEN_TYPE(DVR_ACTION_UPLOAD_IMAGE) \
		GEN_TYPE(DVR_ACTION_VIDEO_POP_UP) \
		GEN_TYPE(DVR_ACTION_PTZ_PRESET) \
		GEN_TYPE(DVR_ACTION_EMAIL_NOTIFICATION) \

typedef enum
{
	GENERATE_DVR_ACTION_TYPE(GENERATE_ENUM)
	MAX_DVR_ACTION_TYPE,
}DVR_ACTION_TYPE_e;

#define GENERATE_PRINTER_TYPE(GEN_TYPE) \
		GEN_TYPE(PRINTER_NONE) \
		GEN_TYPE(EPSON_TM_T88IV) \
		GEN_TYPE(EPSON_TM_T88V) \
		GEN_TYPE(EPSON_TM_T90) \
		GEN_TYPE(WEP_TM_400) \

// e-Canteen related enumeration and configuration
typedef enum
{
	GENERATE_PRINTER_TYPE(GENERATE_ENUM)
	MAX_PRINTER,
}PRINTER_TYPE_e;

#define GENERATE_PRINTER_COMM_TYPE(GEN_TYPE) \
		GEN_TYPE(RS_232) \
		GEN_TYPE(USB) \

typedef enum
{
	GENERATE_PRINTER_COMM_TYPE(GENERATE_ENUM)
	MAX_PRINTER_TYPE,
}PRINTER_COMM_TYPE_e;

typedef enum
{
	BAUD_RATE_9600 = 0,
	BAUD_RATE_19200,
	BAUD_RATE_38400,
	BAUD_RATE_57600,
	BAUD_RATE_115200,

	MAX_BAUD_RATE
}PRINTER_BAUD_RATE_e;

#define GENERATE_SMART_ROUTE_ACCESS_TYPE(GEN_TYPE) \
		GEN_TYPE(SMART_ROUTE_ACCESS_SOFT) \
		GEN_TYPE(SMART_ROUTE_ACCESS_HARD) \

// Smart Card Based Route Access type
typedef enum
{
	GENERATE_SMART_ROUTE_ACCESS_TYPE(GENERATE_ENUM)
	MAX_SMART_ROUTE_ACCESS_TYPE,
}SMART_ROUTE_ACCESS_TYPE_e;

#define GENERATE_KEYPAD(GEN_TYPE) \
		GEN_TYPE(KEYPAD_ALPHANUM) \
		GEN_TYPE(KEYPAD_DIALPAD) \

typedef enum
{
	GENERATE_KEYPAD(GENERATE_ENUM)
	MAX_KEYPAD,
}KEYPAD_e;

typedef enum
{
	SYS_DFLT_FALSE,
	SYS_DFLT_BASIC,
	SYS_DFLT_BASIC_WITH_DOOR_MODE_CHNG,
	SYS_DFLT_ALL,
	SYS_DFLT_MAX,
}SYS_DFLT_TYPE_e;

enum
{
	CALIB_FAIL,
	CALIB_SUCCESS,
	CALIB_UNSUPPORTED,
	CALIB_NOT_REQUIRE,
};
typedef UINT8	CALIB_STATUS_e;

#define MAX_CARD_ID_BITS_VER1			64
#define MAX_CARD_FORMAT_PER_DOOR		5
typedef enum
{
	CARD_FORMAT_FC = 1,
	CARD_FORMAT_ODD_PARITY,
	CARD_FORMAT_CSN,
	CARD_FORMAT_EVEN_PARITY,
	MAX_CARD_FORMAT,
}CARD_FORMAT_e;

#define GENERATE_WIGND_OUT_FRMT(GEN_TYPE) \
		GEN_TYPE(WIGND_OUT_FRMT_26_BIT) \
		GEN_TYPE(WIGND_OUT_FRMT_ACTUAL) \
		GEN_TYPE(WIGND_OUT_FRMT_37_BIT) \
		GEN_TYPE(WIGND_OUT_FRMT_CUSTOM) \

typedef enum
{
	GENERATE_WIGND_OUT_FRMT(GENERATE_ENUM)
	MAX_WIGND_OUT_FRMT,
}WIGND_OUT_FRMT_e;

#define GENERATE_WIGND_OUT_DIR(GEN_TYPE) \
		GEN_TYPE(WIGND_OUT_DIR_MSB_FIRST) \
		GEN_TYPE(WIGND_OUT_DIR_LSB_FIRST) \

typedef enum
{
	GENERATE_WIGND_OUT_DIR(GENERATE_ENUM)
	MAX_WIGND_OUT_DIR,
}WIGND_OUT_DIR_e;

#define GENERATE_VAM_CARD_TYPE(GEN_TYPE) \
		GEN_TYPE(VAM_VEHICLE_CARD) \
		GEN_TYPE(VAM_DRIVER_CARD) \
		GEN_TYPE(VAM_HELPER_CARD) \

typedef enum
{
	GENERATE_VAM_CARD_TYPE(GENERATE_ENUM)
	MAX_VAM_CARD_TYPE,
}VAM_CARD_TYPE_e;

//Vam Config Status
typedef enum
{
	VAM_CARD_NOT_SURRENDER = 0,
	VAM_CARD_SURRENDER,
	CARD_STATUS_MAX
}VAM_CONFIG_CARD_STATUS_e;

#define GENERATE_DVR_EVENT_USER_TYPE(GEN_TYPE) \
		GEN_TYPE(ALL_USERS ) \
		GEN_TYPE(USERS_ONLY) \
		GEN_TYPE(VISITORS_ONLY) \

typedef enum
{
	GENERATE_DVR_EVENT_USER_TYPE(GENERATE_ENUM)
	MAX_DVR_EVENT_USER,
}DVR_EVENT_USER_TYPE_e;

typedef enum
{
	JCM_NONE,
	JCM_STATIC,
	JCM_DYNAMIC,

	MAX_JCM_TYPE,
}USER_JOB_TYPE_e;

#define GENERATE_APB_RESET_TYPE(GEN_TYPE) \
		GEN_TYPE(RESET_PER_DAY) \
		GEN_TYPE(RESET_TIMER_BASED) \

typedef enum
{
	GENERATE_APB_RESET_TYPE(GENERATE_ENUM)
	MAX_APB_RESET_TYPE,
}APB_RESET_TYPE_e;

// Repeat Mode
typedef enum
{
	DO_NOT_REPEAT = 0,
	REPEAT_DAILY,
	REPEAT_WEEKLY,

	MAX_REPEAT_MODE
}REPEAT_MODE_VMS_e;

#define GEN_CONFIGURATION_TABLE_ENUM(				CONFIGURATION_TABLE_INDEX,	CONFIGURATION_TABLE_ENUM, CONFIGURATION_TABLE_SET_ACMS_FUNC, CONFIGURATION_TABLE_CONFIG_UPDATE_FUNC)	CONFIGURATION_TABLE_ENUM,
#define GEN_CONFIGURATION_TABLE_SET_ACMS_FUNC(		CONFIGURATION_TABLE_INDEX,	CONFIGURATION_TABLE_ENUM, CONFIGURATION_TABLE_SET_ACMS_FUNC, CONFIGURATION_TABLE_CONFIG_UPDATE_FUNC)	CONFIGURATION_TABLE_SET_ACMS_FUNC,
#define GEN_CONFIGURATION_TABLE_CONFIG_UPDATE_FUNC(	CONFIGURATION_TABLE_INDEX,	CONFIGURATION_TABLE_ENUM, CONFIGURATION_TABLE_SET_ACMS_FUNC, CONFIGURATION_TABLE_CONFIG_UPDATE_FUNC)	CONFIGURATION_TABLE_CONFIG_UPDATE_FUNC,
#define GEN_CONFIGURATION_TABLE_NAME(				CONFIGURATION_TABLE_INDEX,	CONFIGURATION_TABLE_ENUM, CONFIGURATION_TABLE_SET_ACMS_FUNC, CONFIGURATION_TABLE_CONFIG_UPDATE_FUNC)	#CONFIGURATION_TABLE_ENUM,

//THIS CONFIGURATION IS FOR ACMS
//          INDEX	ENUM										SET_ACMS_FUNCTION						COUNFIG_UPDATE_FUNCTION
#define GEN_ACMS_DEVICE_CONFIGURATION_TABLE(API)\
		API(1,		USER_CONFIG_TABLE_ID,						SetAcmsUserConfigFields,				UserConfigUpdateHandler)\
		API(2,		FUNC_GROUP_TABLE_ID,						SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(3,		TZ_TABLE_ID,								SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(4,		ACCESS_GROUP_TABLE_ID,						SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(5,		ACCESS_ZONE_TABLE_ID,						SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(6,		HOLIDAY_SCH1_TABLE_ID,						SetAcmsHolidaySchedule1ConfigFields,	HolidayScheduleConfigUpdateHandler1)\
		API(7,		HOLIDAY_SCH2_TABLE_ID,						SetAcmsHolidaySchedule2ConfigFields,	HolidayScheduleConfigUpdateHandler2)\
		API(8,		HOLIDAY_SCH3_TABLE_ID,						SetAcmsHolidaySchedule3ConfigFields,	HolidayScheduleConfigUpdateHandler3)\
		API(9,		HOLIDAY_SCH4_TABLE_ID,						SetAcmsHolidaySchedule4ConfigFields,	HolidayScheduleConfigUpdateHandler4)\
		API(10,		SHIFT_SCH_TABLE_ID,							SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(11,		TWO_PERSON_TABLE_ID,						SetAcmsTwoPersonConfigFields,			TwoPersonConfigUpdateHandler)\
		API(12,		INPUT_GROUP_TABLE_ID,						SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(13,		OUTPUT_GROUP_TABLE_ID,						SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(14,		IO_LINK_TABLE_ID,							SetAcmsIoLinkConfigFields,				IoLinkConfigUpdateHandler)\
		API(15,		FIRST_IN_TABLE_ID,							SetAcmsFirstInUserConfigFields,			FirstInUserConfigUpdateHandler)\
		API(16,		GUARD_TOUR_TABLE_ID,						SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(17,		DC_TABLE_ID,								SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(18,		GEN_SYS_BASIC_TABLE_ID,						SetAcmsGenSysParaBasicConfigFields,		GenSysParaBasicConfigUpdateHandler)\
		API(19,		GEN_SYS_ADV_TABLE_ID,						SetAcmsGenSysParaAdvConfigFields,		GenSysParaAdvConfigUpdateHandler)\
		API(20,		NETWORK_PARA_TABLE_ID,						SetAcmsNetworkParaConfigFields,			ConfigUpdateUnknownConfigFields)\
		API(21,		DST_PARA_TABLE_ID,							SetAcmsDstConfigFields,					DstConfigUpdateHandler)\
		API(22,		SPECIAL_FUNC_TABLE_ID,						SetAcmsSplFunctionConfigFields,			SplFunctionConfigUpdateHandler)\
		API(23,		VISITOR_TABLE_ID,							SetAcmsVisitorMgmtConfigFields,			VisitorMgmtConfigUpdateHandler)\
		API(24,		TIME_TRIG_TABLE_ID,							SetAcmsTimeTriggerConfigFields,			TimeTriggerConfigUpdateHandler)\
		API(25,		GREETING_MSG_TABLE_ID,						SetAcmsGreetingMsgConfigFields,			ConfigUpdateUnknownConfigFields)\
		API(26,		TIME_STAMP_TABLE_ID,						SetAcmsTimeStampConfigFields,			ConfigUpdateUnknownConfigFields)\
		API(27,		PANEL_ROUTE_TABLE_ID,						SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(28,		CAFE_ITEM_TABLE_ID,							SetAcmsCafeItemConfigFields,			CafeItemConfigUpdateHandler)\
		API(29,		CAFE_MENU_TABLE_ID,							SetAcmsCafeMenuConfigFields,			CafeMenuConfigUpdateHandler)\
		API(30,		CAFE_MENU_SCHEDULE_TABLE_ID,				SetAcmsCafeMenuSchConfigFields,			CafeMenuSchConfigUpdateHandler)\
		API(31,		PRINTER_PARA_TABLE_ID,						SetAcmsPrinterConfigFields,				PrinterConfigUpdateHandler)\
		API(32,		FPR_CONFIG_TABLE_ID,						SetAcmsFprConfigFields,					FprConfigUpdateHandler)\
		API(33,		DVR_EVT_ACTN_CONFIG_TABLE_ID,				SetAcmsDvrEvtActnConfigFields,			DvrEvtActnConfigUpdateHandler)\
		API(34,		PALM_PARA_CONFIG_TABLE_ID,					SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(35,		SMART_ROUTE_ACCESS_CONFIG_TABLE_ID,			SetAcmsSmartRouteAccessConfigFields,	SmartRouteAccessConfigUpdateHandler)\
		API(36,		SHIFT_MASTER_TABLE_ID,						SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(37,		SHIFT_EXCEPTION_TABLE_ID,					SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(38,		IO_LINK_EXT_DEVICE_CONFIG_TABLE_ID,			SetAcmsIoLinkExtDeviceConfigFields,		IoLinkExtDevConfigUpdateHandler)\
		API(39,		IO_LINK_EXT_DEVICE_MAPPING_TABLE_ID,		SetAcmsIoLinkExtDeviceMappingFields,	IoLinkExtDevMappingUpdateHandler)\
		API(40,		EXT_CARD_FORMAT_CONFIG_TABLE_ID,			SetAcmsExtCardFormatConfigFields,		ConfigUpdateUnknownConfigFields)\
		API(41,		INT_CARD_FORMAT_CONFIG_TABLE_ID,			SetAcmsIntCardFormatConfigFields,		ConfigUpdateUnknownConfigFields)\
		API(42,		ACCESS_CLUSTER_CONFIG_TABLE_ID,				SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(43,		VAM_CONFIG_TABLE_ID,						SetAcmsVamConfigFields,					VamConfigUpdateHandler)\
		API(44,		OCCUPANCY_CONFIG_TABLE_ID,					SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(45,		JCM_CONFIG_TABLE_ID,						SetAcmsJcmConfigFields,					JcmConfigUpdateHandler)\
		API(46,		MULTI_LANGUAGE_CONFIG_TABLE_ID,				SetAcmsMultiLanConfigFields,			MultiLanguageUpdateHandler)\
		API(47,     DOOR_MODE_SCH_CONFIG_TABLE_ID,              SetAcmsDoorModeSchConfigFields,			RmsConfigUpdateHandler)\
		API(48,		SMART_CARD_FORMAT_TABLE_ID,					SetAcmsSmartCardFormatFields,			SmartCardFormatUpdateHandler)\
		API(49,		CUSTOM_SMART_CARD_CONFIG_TABLE_ID,			SetAcmsCustomSmartCardConfig,			CustomSmartCardConfigUpdateHandler)\
		API(50,		WIEGAND_OUT_CUSTOM_FORMAT_CONFIG_UPDATE_ID,	SetAcmsWiegandOutCstmFrmtConfigFields,	WiegandOutCstmFrmtConfigConfigUpdateHandler)\
		API(51,		IDENTIFICATION_SERVER_CONFIG_TABLE_ID,		SetAcmsIdentificationServerConfig,		IdentificationServerConfigUpdateHandler)\
		API(52,		DOOR_GROUP_CONFIG_TABLE_ID,					SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(53,		FMX_SCHEDULE_CONFIG_TABLE_ID,				SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(54,		MIFARE_SCTR_WISE_KEY_CNFG_TBL_UPDATE_ID,	SetAcmsMifareSectorWiseKeyConfigFields,	MifareSectorWiseKeyCnfgConfigUpdateHandler)\
		API(55,		IO_LINK_AUX_IN_TABLE_ID,					SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields	)\
		API(56,		IO_LINK_AUX_OUT_TABLE_ID,					SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields	)\
		API(57,		BLE_CONFIG_ID,								SetAcmsBleConfig,						BleConfigUpdateHandler)\
		API(58,		IP_CAMERA_CONFIG_TABLE_ID,					SetAcmsIpCameraConfig,					IpCameraConfigUpdateHandler)\
		API(59,		IP_CAMERA_ADVANCE_CONFIG_TABLE_ID,			SetAcmsIpCameraAdvanceConfig,			IpCameraAdvanceConfigUpdateHandler)\
		API(60,		BACKUP_AND_UPDATE_CONFIG_TABLE_ID,			SetAcmsBackupUpdateConfigFields,		BackupUpdateConfigUpdateHandler)\
		API(61,		ELEVATOR_CONFIG_TABLE_ID,					SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(62,		ELEVATOR_FLOOR_GROUP_CONFIG_TABLE_ID,		SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(63,		TIME_ZONE_GROUP_CONFIG_TABLE_ID,			SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(64,		FACE_RECO_FR_SERVER_CONFIG_TABLE_ID,		SetAcmsFaceRecogntionConfigFields,		FaceRecFrServerParaConfigUpdateHandler)\
		API(65,		SPECIAL_FUNCTION_SCHEDULE_TABLE_ID,			SetAcmsSplFnScheduleConfig,				SplFnScheduleConfigUpdateHandler)\
		API(66,		UNKNOWN_CONFIG_TABLE_ID1,					SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(67,		UNKNOWN_CONFIG_TABLE_ID2,					SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(68,		READER_BLE_CONFIG_TABLE_ID,					SetAcmsReaderBleConfig,					ReaderBleConfigUpdateHandler)\
		API(69,		UNKNOWN_CONFIG_TABLE_ID3,			SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\
		API(70,		ACCESS_SCHEDULE_TABLE_ID,			SetAcmsAccessScheduleConfigFields,		AccessScheduleConfigUpdateHandler)\
		API(255,	MAX_ACMS_CONFIG_TABLE,						SetAcmsUnknownConfigFields,				ConfigUpdateUnknownConfigFields)\

//THIS CONFIGURATIONS ARE NOT IN ACMS
#define GEN_DEVICE_CONFIGURATION_TABLE(API)\
		API(1,		ETHERNET_PARA_TABLE_UPDATE_ID,				SetAcmsUnknownConfigFields,				EthernetParaConfigUpdateHandler)\
		API(2,		SERVER_PARA_TABLE_UPDATE_ID,				SetAcmsUnknownConfigFields,				ServerParaConfigUpdateHandler)\
		API(3,		CARD_FORMAT_CONFIG_TABLE_UPDATE_ID,			SetAcmsUnknownConfigFields,				CardFormatConfigUpdateHandler)\
		API(4,		WEB_API_EVENTS_CONFIG_TABLE_UPDATE_ID,		SetAcmsUnknownConfigFields,				WebAPIEventsConfigUpdateHandler)\
		API(5,		CCC_PARA_UPDATE_ID,							SetAcmsUnknownConfigFields,				CccParaConfigUpdateHandler)\
		API(6,		FACE_RECO_FR_CAMERA_UPDATE_ID,				SetAcmsUnknownConfigFields,				FaceRecCameraConfigUpdateHandler)\
		API(7,		READER_BLE_CONFIG_ID,						SetAcmsUnknownConfigFields,				ReaderBleConfigUpdateHandler)\

// Configuration table id
typedef enum
{
	GEN_ACMS_DEVICE_CONFIGURATION_TABLE(GEN_CONFIGURATION_TABLE_ENUM)
	GEN_DEVICE_CONFIGURATION_TABLE(GEN_CONFIGURATION_TABLE_ENUM)

	MAX_CONFIG_TABLE,
}CONFIGURATION_TABLE_e;

// index for device led and buzzer cadance configuration
typedef enum
{

	LED_AND_BUZ_CAD_IDLE_ONLINE,
	LED_AND_BUZ_CAD_IDLE_OFFLINE,
	LED_AND_BUZ_CAD_DEGRADE,
	LED_AND_BUZ_CAD_WAIT,
	LED_AND_BUZ_CAD_SUCCESS,
	LED_AND_BUZ_CAD_DENIED,
	LED_AND_BUZ_CAD_TIME_OUT,
	LED_AND_BUZ_CAD_MINOR_ALARM,
	LED_AND_BUZ_CAD_MAJOR_ALARM,
	LED_AND_BUZ_CAD_CRITICAL_ALARM,
//	LED_AND_BUZ_CAD_INVALID_INPUT,
	MAX_LED_BUZ_CADANCE,
}DEVICE_LED_BUZ_CADANCE_INDEX_e;

typedef enum
{
	STATE_INACTIVE,
	STATE_ACTIVE,
	STATE_DONE,
}PROCESS_STATUS_e;

#define GENERATE_ENROLL_ID_TYPE(GEN_TYPE) \
		GEN_TYPE(ALPHANUM_ID) \
		GEN_TYPE(REF_USER_ID) \

typedef enum
{
	GENERATE_ENROLL_ID_TYPE(GENERATE_ENUM)
	MAX_ENROLL_ID,
}ENROLL_ID_TYPE_e;

typedef enum
{
	FIRST_IN_RESET_AFTER_WORKING_HOURS_EXPIRE,
	FIRST_IN_RESET_AFTER_TIMER_EXPIRE,
	MAX_FIRST_IN_RESET_AFTER_TYPE,
}FIRST_IN_RESET_AFTER_TYPE_e;

//COSEC_AADHAR not used in few devices so DUMMY_AADHAR used in it's place because value of COSEC_THIRD_PARTY should be 3.
//In future this enum should be placed in MxSysCommonUtils.h
typedef enum
{
	COSEC_CENTRA = 0,
	COSEC_VYOM,
	DUMMY_AADHAR,
#ifdef PUSH_API_SUPPORTED
	COSEC_THIRD_PARTY,
#endif
}SERVER_MODE_e;

typedef enum
{
	VIDYAWAAN_SERVER_URL = 0,			// Common for Vidhyawaan, JNTUH, AEBAS
	CUSTOM_URL,
}SERVER_URL_e;

#define USER_IN_OUT_STATUS_SIZE (MAX_USERS/8)

typedef enum{
				USER_IS_IN	= 0,
				USER_IS_OUT,
				MAX_USER_IN_OUT_STATUS,
}USER_IN_OUT_STATUS_e;

typedef enum
{
	FIRMWARE_UPGRADE_BASED_ON_VERSION = 0 ,
	FIRMWARE_UPGRADE_BASED_ON_TIME,
	MAX_FIRMWARE_UPGRADE_BASED_ON_STATUS,
}FIRMWARE_UPGRADE_BASED_ON_e;

typedef enum
{
	CAPTURE_EVENT_USER_ALLOWED,
	CAPTURE_EVENT_USER_DENIED,
	CAPTURE_EVENT_USER_NOT_IDENTIFIED,
	CAPTURE_EVENT_AUX_IN_1_STATUS_CHANGE,
	CAPTURE_EVENT_DOOR_ABNORMAL,
	CAPTURE_EVENT_DOOR_FORCE_OPEN,
	CAPTURE_EVENT_TEMPER_ALARM,
	CAPTURE_EVENT_INTERCOM_PANIC,
	CAPTURE_EVENT_VISITOR_ALLOWED,
	CAPTURE_EVENT_VISITOR_DENIED,
	MAX_CAPTURE_EVENT,
}CAPTURE_IMG_EVENT_e;

typedef enum
{
	CAPTURE_EVENT_MODE_ENTRY,
	CAPTURE_EVENT_MODE_EXIT,
	CAPTURE_EVENT_MODE_BOTH,
	MAX_CAPTURE_EVENT_MODE,
}CAPTURE_IMG_EVENT_MODE_e;

typedef enum
{
	INTERFACE_RS232,
	INTERFACE_USB,
	INTERFACE_HTTPS,
	MAX_SENSOR_INTERFACE,
}SENSOR_INTERFACE_e;

typedef enum
{
	SENSOR_AST,
	SENSOR_WEB_BASED,
	SENSOR_TERABEE,
	SENSOR_FEVO,
	MAX_TEMPERATURE_SENSOR_TYPE,
}TEMPERATURE_SENSOR_TYPE_e;

typedef enum
{
	USER_RESTRICTION_SOFT,
	USER_RESTRICTION_HARD,
	MAX_USER_RESTRICTION,
}USER_RESTRICTION_e;

typedef enum
{
	FACE_MASK_RESTRICTION_SOFT,
	FACE_MASK_RESTRICTION_HARD,
	MAX_FACE_MASK_RESTRICTION,
}FACE_MASK_RESTRICTION_e;

typedef enum
{
	CAFE_OFFLINE_TRANSACTION_NOT_ALLOWED,
	CAFE_OFFLINE_TRANSACTION_ALLOWED_WITHOUT_DISCOUNT,
	CAFE_OFFLINE_TRANSACTION_ALLOWED_WITH_DISCOUNT,
}CAFE_OFFLINE_TRANSACTION_e;

typedef enum
{
	IN_PUNCH_COLOR_GREEN,
	IN_PUNCH_COLOR_SKY_BLUE
}IN_PUNCH_COLOR_e;

typedef enum
{
	OUT_PUNCH_COLOR_YELLOW,
	OUT_PUNCH_COLOR_ORANGE,
}OUT_PUNCH_COLOR_e;

typedef enum
{
	WEB_TLS_VERSION_1_0,
	WEB_TLS_VERSION_1_1,
	WEB_TLS_VERSION_1_2,
}WEB_TLS_VERSION_e;

//******** Function Prototypes *******

#endif // MXCONFIGDEF_H
