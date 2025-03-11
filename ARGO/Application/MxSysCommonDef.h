#if !defined MXSYSCOMMONDEF_H
#define MXSYSCOMMONDEF_H
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
//   Project      : ACS (Access Control System)
//   Created By   : Ronak Tanna
//   File         : MxSysCommonDef.h
//   Description  : This file contains Defines and APIs common to system.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
//For compiling driver it requires absolute path of "MxTypedef.h"
#include "MxTypedef.h"
#include "MxDeviceSpecific.h"
#include "MxSlaveCommon.h"

//******** Defines and Data Types ****
#define MAX_ACMS_PACKET_SZ				13*1024	// Maximum size of any acms message
#define MAX_PANEL_PACKET_SIZE			1024

#define MENU_TIMER						30
#define DISP_PROCESSING_TIMER			20
#define INSTALLATION_PROCESS_WAIT_TIMER	30

#define ORDINAL_OFFSET					1
#define MONTH_OFFSET 					1		//month start form 0
#define	LINUX_YEAR_OFFSET_1900			1900
#define YEAR_OFFSET_2000				2000
//Year saved in configuration is 00-99 for 2000-2099
//In linux tm structure uses 1900 as 0 hence we need to adjust
#define ADJUST_TM_YEAR					100

#define MxAsciiToUINT32(buffPtr)			atoi(buffPtr)

#define CONVERT_TIME_IN_MIN(hr, min)		((hr * 60) + min)	//HH:MM time in minutes
#define	CONVERT_SEC_TO_MSEC(sec)			((UINT32)sec * 1000)
#define CONVERT_TIME_IN_SEC(hr, min, sec)	((hr * 60 * 60) + (min * 60) + sec)
#define	CONVERT_MSEC_TO_MIN(msec)			( ((UINT32)msec / 1000) / 60)

#define TIME_DDMMHH_IN_MIN(day, hr, min)	( ( ( (day * 24) + hr) * 60) + min)	// in minutes
#define SECONDS_IN_A_DAY					(86400)

#define KILOBYTE(x)			(x * 1024)
#define MAX_PHOTO_SIZE 		(KILOBYTE(100))

//Zip file
#define FIRMWARE_FOLDER				"firmware"				// Firmware folder
#define	SDK_VER						1
#define ACMS_FIRMWARE_FILE_VER		0
#define DIRECT_DOORFIRMWARE_FILE	"COSEC_ARGO_DOOR"			// Firmware file name
#define ROOT_FS_FIRMWARE_FILE_NAME	"COSEC_ARGO"			//
#define DIRECT_DOOR_FTP_FOLDER		"ARGO"
#define COSEC_ARGO_SDK_FILE			"COSEC_ARGO_SDK"		// Firmware file name
#define ZIP_FILE_EXT				".zip"					// Firmware file extention
#define FIRMWARE_LOG_FILE			"log_firm.txt"			// Firmware log file name

#define	EVENT_BACKUP_FOLDER			"event"
#define EVENT_BACKUP_FILE			"event.txt"
#define	EVENT_BACKUP_LOG_FILE		"log_evt.txt"

#define	CONFIG_FOLDER				"config"
#define CONFIG_FILE					"config.txt"
#define	CONFIG_LOG_FILE				"log_cfg.txt"

#define FIRMWARE_SIGN_FILE					"Signature.txt"
#define CHANGE_LIB_SOFT_LINK_SIGN_FILE		"ChangeSoftLinksToOldLibVer.txt"

#define WEB_BIN						"webs"
#define PAGE_LOAD_BIN				"pageload"

#define SFPR3030_BIN_FILE			"SFM3030"
#define SFPR3050_BIN_FILE			"SFM3050"
#define SFPR_BIN_FILE_EXT			".bin"

#define COSEC_CER_FIRMWARE_FILE		"COSEC_CER"
#define COSEC_CER_FIRMWARE_FILE_EXT	".bin"

#define COSEC_CBR_FIRMWARE_FILE		"COSEC_READER_CB"
#define COSEC_CBR_FIRMWARE_FILE_EXT	".bin"

#define COSEC_ATOM_FIRMWARE_FILE				"COSEC_READER_ATOM"
#define COSEC_ATOM_FIRMWARE_FILE_EXT			".bin"
#define COSEC_ATOM_RD200_300_FIRMWARE_FILE_EXT	".zip"

#define COSEC_CER_FIRMWARE_PATH		"/matrix/zip/COSEC_CER.bin"
#define APPL_PATH					"/matrix/appl/"
#define WEB_PATH					"/matrix/web/html_pages/"
#define CGI_PATH					"/matrix/web/html_pages/cgi-bin/"
#define WEB_BIN_PATH				"/matrix/web/bin/download/"

#define USER_PICS_FOLDER				"User_Pics"
#define DLT_USER_PICS_FOLDER			"User_Pics.delete"
#define SYS_TMP_PHOTO_DIR 				"Usr_P_"
#define USER_PICS_PREFIX				"User_"
#define DEFAULT_PICS_FILE_EXT			".jpg"
#define PHOTO_DEL_SIGNATURE_FILE_PATH 	"/photoDelSign.txt"

#define CUSTOM_IMAGE_PATH			"/matrix/customImage/"
#define WALLPAPER_PREFIX			"Wallpapers/"
#define DOOR_MODE_WALLPAPER_PREFIX	"Wallpapers/DoorModeWallpapers/"

#define TEMP_DIR					"/tmp/"
#define ZIP_PATH					"/matrix/zip/"
#define DECRYPTED_ZIP_PATH			"/matrix/decryptedzip"
#define USR_LIB_PATH				"/usr/lib/"
#define APPL_DATA_PATH				"/matrix/applData/"
#define WEB_BIN_AUTH_PATH			"/matrix/web/bin/"
#define DEMO_STATUS_FILE			"DemoControl.txt"
#define SAMAY_STATUS_FILE			"SamayControl.txt"
#define AUTH_TXT_FILE				"auth.txt"

#define	MAC_FOLDER_NAME_SIZE		10
#define	MAX_MMC_PATH_SIZE			50
#define	MAX_PICS_PATH_SIZE			100		// NOTE: User/Item pics path depends on MMC path
#define	MAX_USB_PATH_SIZE			50

#define INVALID_LED_STATUS			0xFF

#define GetReaderRespPort(readerId)		( (readerId >= READER3) ? EXTERNAL_PORT: INTERNAL_PORT)

#define MAX_SIZE_DOUBLE_SFM_TEMPLATES	(2 * MAX_SIZE_SINGLE_SFM_TEMPLATE)

#define FP_MATCH_ABORT_TIMER				12	// in seconds
#define SYSTEST_DISPLAY_SCREEN_TIME_OUT		15	// in seconds

#define MAX_CARD_ID_LEN				16		// maximum length of card id

#define MATRIX_RTC_STATUS			_IOR('p',0x20, long unsigned int)

#define MAX_TMP_BUFF_SIZE			1024
#define	MAX_CNFG_FILE_NAME_SZ		100
#define	MAX_SYS_CMD_BUF_SZ			150

#define FIRST_WEEK					0x01
#define SECOND_WEEK					0x02
#define THIRD_WEEK					0x04
#define FOURTH_WEEK					0x08
#define FIFTH_WEEK					0x10
#define LAST_WEEK					0x20
#define NO_WEEK						0xFF

#define FIRST_HOLIDAY_SCHEDULE_ID	1

#define	RELOAD_WEBUSER_SIGNATURE	"/tmp/reloadWebuser"
#define	RELOAD_WEB_ROUTE_SIGNATURE	"/tmp/reloadWebRoute"

#define EVENT_PICS_FOLDER			"Event_Pics"
#define EVENT_PICS_FILE_EXT			".jpg"

#define SDK_VERSION 				1

#define GENERATE_WEEKORDINAL(GEN_TYPE) \
		GEN_TYPE(WEEKORDINAL_FIRST) \
		GEN_TYPE(WEEKORDINAL_SECOND) \
		GEN_TYPE(WEEKORDINAL_THIRD) \
		GEN_TYPE(WEEKORDINAL_FOURTH) \
		GEN_TYPE(WEEKORDINAL_LAST) \

//Week
typedef enum
{
	GENERATE_WEEKORDINAL(GENERATE_ENUM)
	MAX_WEEKORDINAL
}WEEKORDINAL_e;

#define GENERATE_MONTH(GEN_TYPE) \
		GEN_TYPE(JANUARY) \
		GEN_TYPE(FEBRUARY) \
		GEN_TYPE(MARCH) \
		GEN_TYPE(APRIL) \
		GEN_TYPE(MAY) \
		GEN_TYPE(JUNE) \
		GEN_TYPE(JULY) \
		GEN_TYPE(AUGUST) \
		GEN_TYPE(SEPTEMBER) \
		GEN_TYPE(OCTOBER) \
		GEN_TYPE(NOVEMBER) \
		GEN_TYPE(DECEMBER) \

//Month
typedef enum
{
	GENERATE_MONTH(GENERATE_ENUM)
	MAX_MONTH
}MONTH_e;

typedef struct
{
	UINT8  	hour;
	UINT8  	minute;
}TIME_HHMM_t;

typedef struct	__attribute__((packed))
{
	WEEKORDINAL_e	weekOrdinal;
	WEEKDAYS_e		weekDay;
	MONTH_e			month;
}DAY_WWDDMM_t;

typedef struct
{
	UINT8  	day;
	UINT8  	month;
}DATE_DDMM_t;

typedef struct
{
	UINT8  	day;
	UINT8  	month;
	UINT16	year;
}DATE_DDMMYYYY_t;

//READER CONVERSION TYPE
typedef enum
{
	ACMS_TO_SYS = 0,
	SYS_TO_ACMS,
	SYS_TO_WEB,
	MAX_READER_CONV_TYPE
}READER_CONV_TYPE_e;

typedef enum
{
	CHECK_IP_ADDR = 0,
	CHECK_GATEWAY_ADDR,
	CHECK_DNS_ADDR,
	CHECK_SERVER_IP,
}CHECK_NW_ADDR_e;

#define GENERATE_DST_STATE(GEN_TYPE) \
		GEN_TYPE(DST_STATE_NORMAL) \
		GEN_TYPE(DST_STATE_FORWARD) \
		GEN_TYPE(DST_STATE_BACKWARD) \

typedef enum
{
	GENERATE_DST_STATE(GENERATE_ENUM)
	MAX_DST_STATE
}DST_STATE_e;

typedef enum
{
	DST_TIME_TYPE_FORWARD,
	DST_TIME_TYPE_BACKWARD,

	MAX_DST_TIME_TYPE
}DST_TIME_TYPE_e;

// READER CATEGORY
typedef enum
{
	CARD_READER,			// read only or smart card reader
	SMARTCARD_READER,		// smart card reader
	FP_READER,				// finger print reader
	EXTERNAL_PIN_READER,	// External Pin Reader

	ANY_READER = 0xFF,		// any reader
}READER_CATEGORY_e;

typedef enum
{
	THREAD_INACTIVE,
	THREAD_ACTIVE,
	THREAD_DONE,
}THREAD_STATUS_e;

//Don not change the position of bellow enum because it may affect in compilation
//rtc driver. In linux other standard library file like stdio.h, time.h, etc, can not
// be included, so if that file contains inclusion of above file then it create problem
enum
{
	RTC_POWER_ON = 1,				//RTC REBOOT DURING DOOR POWER OFF
	RTC_BATTERY_FAULT,				//RTC BATTERY LOW VOLTAGE DETECTED
	RTC_NOT_PRESENT,				//UNABLE TO ESTABLISH COMMUNICATION WITH RTC
	RTC_NORMAL,

	MAX_RTC_STATUS
};
typedef UINT8	RTC_STATUS_e;

typedef enum
{
	BU_TYPE_NONE,
	BU_TYPE_FIRMWARE,
	BU_TYPE_CFG,
	BU_TYPE_EVENT,

	MAX_BU_TYPE,
}BACKUP_UPDATE_TYPE_e;

typedef enum
{
	DOOR_TYPE_DD = 0,
	DOOR_TYPE_PD,
	MAX_DOOR_TYPE,
}DOOR_FIRMWARE_TYPE_e;

typedef enum
{
	FPR_SENSOR_NONE,
	OPTICAL_SENSOR,
	CAPACITIVE_SENSOR,
	MAX_FPR_SENSOR_TYPE,
}FPR_SENSOR_TYPE_e;

typedef enum
{
	USER_REMOVED_FROM_SYS,
	USER_NOT_REMOVEDFROM_SYS,
}REMOVE_USER_FROM_ANTIPASS_e;

//******** Function Prototypes *******

#endif  // MXSYSCOMMONDEF_H
