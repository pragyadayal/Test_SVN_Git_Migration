#if !defined MXDEVICESPECIFIC_H
#define MXDEVICESPECIFIC_H
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
//   File         : MxDeviceSpecific.h
//   Description  : This file contains device specific configuration 
//                  related defines.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****

//******** Defines and Data Types ****

#define SYSTEM_ARCH_32_BIT
#define POINTER_SIZE_32_BIT_ARCH 				4

#define DEVICE_IS_ARGO_DOOR

#define ACCESS_CONTROL_DEVICE_CODE				20	//same device code for GATE and VEGA NEW (with Bluetooth)

//NOTE:For samay door device code is 16
#define ACCESS_CONTROL_SAMAY_DEVICE_CODE		16	//SAMAY Door controller type

#define DEVICE_TYPE_FOR_THIRD_PARTY_SERVER	7

#define MODEM_FEATURE_SUPPORTED
#define BLOCK_DEVICE_FEATURE_SUPPORTED
#define MULTI_LANGUAGE_SUPPORTED
#define OEM_SUPPORTED


#define COSEC_DEMO_DEVICE_CODE_OFFSET			0x80
#define WIEGAND_OUTPUT_SUPPORT
#define PANEL_DOOR_SUPPORT
#define GUI_SUPPORTED
#define BLE_SUPPORTED
#define MULTI_LANGUAGE_SUPPORTED
#define MULTI_LANG_FILE_APPLICABLE_COLUMN		3
#define LICENSE_DONGLE_SUPPORT
// Reader related Defines
#define READER1_UART_DEV_FILE			"/dev/ttyO1"
#define READER2_UART_DEV_FILE			"/dev/ttyACM0"
#define READER3_UART_DEV_FILE			"/dev/ttyO3"

#define MAX_AUTO_DETECT_READER			(5)
#define READER_RXTX_WAIT_DELAY			5000
#define UPNP_SUPPORTED

#define SYS_REBOOT_TMR_CNT				30			// Resolution is 100 msec

//#define SFPR_CAPACITIVE_SENSOR
//#define SFPR_PARA_AUTO_RESPONSE_TO_BOTH
//#define MIFARE_READER_INIT_PRIORITY_PN532
#define CAFETERIA_FEATURE_SUPPORTED
//#define CAFETERIA_ITEM_IMAGE_SUPPORTED
//#define DURESS_FEATURE_SUPPORTED
#define CER_READER_SUPPORTED
#define FP_READER_SUPPORTED
//#define THREAD_BASED_RXTX
#define BIO_IDT_SRVR_SUPPORT
#define HTTP_CLIENT_SUPPORT
#define FACE_RECOGNITION_SUPPORT
#define CB_READER_BLE_SUPPORTED
#define MIFARE_READER_INIT_PRIORITY_CLR663
// #define LICENSE_DONGLE_SUPPORT
#define ATOM_READER_SUPPORTED		// Generic defines for all atom readers

#define PUSH_API_FEATURE_SUPPORTED
#define PUSH_API_SUPPORTED
#define BLOCK_DEVICE_FEATURE_SUPPORTED
#define MODEM_FEATURE_SUPPORTED

/* Face Mask Compulsion Featured is applicable for Both Gate as well as vega */
#define FACE_MASK_COMPULSION_ENABLE

/* FR liveness featured is applicable for all FR devices Gate, Vega, ARGO as well as FMX */
#define FR_LIVENESS_FEATURE_ENABLE

#define FR_EXCEPTIONAL_FACE_FEATURE_ENABLE
#if defined(FACE_MASK_COMPULSION_ENABLE) || defined(FR_LIVENESS_FEATURE_ENABLE) || defined(FR_EXCEPTIONAL_FACE_FEATURE_ENABLE)
	/* Define FR Additional Features Supported */
	#define FR_ADDITIONAL_FEATURE_SUPPORTED
#endif

// Event related Defines
#define	MAX_EVENT_COUNT					500000									// Maximum Event Store In event log file
#define EVENT_FILE						"/matrix/applData/event/Event"			// Event Log File initial name
#define EVENT_COUNT_FILE				"/matrix/applData/event/EventCnt.log"
#define	OFFLINE_EVENT_FILE				"/matrix/applData/event/OfflineEvent"			// Event Log File initial name
#define THIRDPARTY_EVENT_FILE			"/matrix/applData/event/ThirdPartyUpdateEventCnt.txt"

#define LOG_FILE_PATH					"/matrix/applData/log/"
#define CONFIG_DEFAULT_LOG_FILE         "/matrix/applData/log/ConfigDefaultLog.txt"
#define CONFIG_FAIL_LOG_FILE            "/matrix/applData/log/ConfigFailLog.txt"

#define WEB_SRV_ROUTE_FILE				"/matrix/web/bin/route.txt"
#define WEB_SRV_HTTP_ROUTE_FILE			"/matrix/web/bin/http_route.txt"
#define WEB_SRV_HTTPS_ROUTE_FILE		"/matrix/web/bin/https_route.txt"

#define SYS_CONFIG_PATH					"/matrix/applData/config/"
#define SYS_ATTENDACE_DETAIL_PATH		SYS_CONFIG_PATH"attendanceDetail/"
#define SYS_TEST_CONFIG_FILE			SYS_CONFIG_PATH"MxSysTest.cfg"
#define SYS_REBOOT_TEST_CONFIG_FILE		SYS_CONFIG_PATH"MxSysRebootTest.cfg"
#define SYS_REBOOT_FILE					"/reboot_signature"
#define ABSENTEE_RECORD					SYS_CONFIG_PATH"MxAbsenteeRule"
#define SMART_CARD_CONFIGURABLE_FEATURE
#define USER_ATTENDANCE_DETAIL_FEATURE

#define DEVICE_IP_SIGN_FILE				"/matrix/deviceIpSignature.txt"
#define SERVER_IP_SIGN_FILE				"/matrix/serverIpSignature.txt"

#define AUTO_SYS_TEST_CONFIG_FILE		SYS_CONFIG_PATH"MxAutoSysTest.cfg"

#define FIRMWARE_ENCRYPTION_KEY         "j31h6743y13959m266u9"
#define SUPPORT_HTTP_IMAGE_SYNC
#define SUPPORT_HTTP_IMAGE_SYNC

#define TEMPORARY_USER_FEATURE_SUPPORTED

#define WORK_ITEM_SUPPORTED
#define FILE_WORK_ITEM_SUPPORTED
#endif  // MXDEVICESPECIFIC_H
