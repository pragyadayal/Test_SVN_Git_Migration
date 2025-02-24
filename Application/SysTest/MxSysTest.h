#if !defined MXSYSTEST_H
#define MXSYSTEST_H
///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix ComSec Pvt. Ltd., Baroda, India.
//   Project      : Access Control System
//   Created By   : Kaival Shah
//   File         : MxSysTest.h
//   Description  : This file declares system test related defines and prototypes
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"

//******** Defines and Data Types ****
typedef enum
{
	SYSTEM_TEST_IDLE,
	SYSTEM_TEST_TOUCH_PANEL,
	SYSTEM_TEST_DOOR_RELAY,
	SYSTEM_TEST_AUX_RELAY,
	SYSTEM_TEST_DOOR_SENSE,
	SYSTEM_TEST_AUX_IN,
	SYSTEM_TEST_DOOR_TMPR,
	SYSTEM_TEST_EXT_TMPR,
	SYSTEM_TEST_LOCK_TMPR,
	SYSTEM_TEST_EXT_SWITCH,
	SYSTEM_TEST_LCD,
	MAX_SYSTEM_TEST_STATES,
}SYSTEM_TEST_STATES_e;

typedef enum
{
	SYSTEM_TEST_STATUS_IDLE,
	SYSTEM_TEST_STATUS_PROGRESS,
	SYSTEM_TEST_STATUS_COMPLETE,
	MAX_SYSTEM_TEST_STATUS
}SYSTEM_TEST_STATUS_e;

typedef enum
{
	VOLTAGE_1_1V,
	VOLTAGE_1_26V,
	VOLTAGE_1_5V,
	VOLTAGE_1_8V,
	VOLTAGE_3_3V,
	VOLTAGE_5_0V,
	VOLTAGE_12_0V,
	MAX_VOLTAGE_LIST,
}PCB_VOLTAGE_LIST_e;

typedef enum
{
	GENERIC_BUZ_LED,
	GENERIC_TOUCH_PANEL,
	GENERIC_AUX_RELAY,
	GENERIC_DOOR_RELAY,
	GENERIC_AUX_IN,
	GENERIC_DOOR_SENSE,
	GENERIC_DOOR_TMPR,
	GENERIC_EXT_TMPR,
	GENERIC_LOCK_TMPR,
	GENERIC_EXT_SWITCH,
	GENERIC_USB,
	GENERIC_MEMORY_CARD,
	GENERIC_RTC_TEST,
	GENERIC_LCD_TEST,
	MAX_GENERIC_IO_LIST
}GENERIC_IO_LIST_e;

typedef enum
{
	SYS_TEST_INT_EM_PROX,
	SYS_TEST_INT_HID_PROX,
	SYS_TEST_INT_MIFARE,
	SYS_TEST_INT_HID_ICLASS,
	SYS_TEST_EXT_HID_ICLASS_U,
	SYS_TEST_EXT_HID_ICLASS_W,
	SYS_TEST_MAX_CARD_READER,
}SYS_TEST_CARD_RDR_LIST_e;

#define HWD_DESIN_NO_SIZE		16		//15+NULL
#define DEVICE_SR_NO_SIZE		16		//15+NULL
#define DATE_STR_SIZE			11		//10+NULL
#define EMP_NAME_SIZE			31		//30+NULL
#define EMP_ID_SIZE				16		//15+NULL
#define FTP_USR_NAME_SIZE		16		//15+NULL
#define FTP_PWD_SIZE			16		//15+NULL
#define	 SYS_TEST_REPORT_FILE		"/matrix/web/html_pages/MxTestReport.txt"
#define UPGRD_FILE_DETAIL_SIZE	40

#define SYS_TEST_PROG_TIMEOUT	10		//in second
#define SYS_LCD_TEST_TIMER		30

typedef struct
{
	UINT16		sysRebootCnt;
}SYSTEM_REBOOT_TEST_PARA_t;

#define SYS_TEST_PARA_VER		1
typedef struct
{
	char			empName[EMP_NAME_SIZE];
	char			empId[EMP_ID_SIZE];
	char			devSrNo[DEVICE_SR_NO_SIZE];
	char			mainPcbDesign[HWD_DESIN_NO_SIZE];
	char			connPcbDesign[HWD_DESIN_NO_SIZE];
	char			testDate[DATE_STR_SIZE];
	char			ftpIpaddr[MAX_IP_ADDRESS];
	char			ftpUsrName[FTP_USR_NAME_SIZE];
	char			ftpPwd[FTP_PWD_SIZE];
	BOOL			macAddrResult;
	BOOL			voltResult[MAX_VOLTAGE_LIST];
	BOOL			rtcFreqResult;
	char			cardRdrRange[SYS_TEST_MAX_CARD_READER][3];
	BOOL			cardRdrResult[SYS_TEST_MAX_CARD_READER];
	BOOL			intFingerRdrResult;
	BOOL			extFingerRdrResult;
	BOOL			genericIoResult[MAX_GENERIC_IO_LIST];
	BOOL			usrEnrlResult;
	BOOL			sysDfltResult;
	BOOL			wifiResult;
	char			asmblerName[EMP_NAME_SIZE];
	char			asmblerId[EMP_ID_SIZE];
	UINT64			detectedCardId[SYS_TEST_MAX_CARD_READER];
	BOOL			sysUpgrdFileInMmcRes;
	CHAR			upgrdFileDetail[UPGRD_FILE_DETAIL_SIZE];
	BOOL			bluetoothResult;
}SYSTEM_TEST_PARA_t;

typedef struct
{
	UINT8					detectedInput;
	UINT8					expectedInput;
	UINT8					dispTimeOut;
	SYSTEM_TEST_STATES_e	testState;
}SYS_TEST_INPUT_DATA_t;

typedef struct
{
	UINT8					output;
	UINT8					dispTimeOut;
	SYSTEM_TEST_STATES_e	testState;
}SYS_TEST_OUTPUT_DATA_t;

//******** Function Prototypes *******
void DcSystemTestStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
void SwitchDcToSysTestState(SYSTEM_TEST_STATES_e state);
void EnrollTestUser(UINT16 userIdx);
void InitSysTest(void);
BOOL WriteSysTestPara(void);
void GenerateTestReport(void);
void SendTestReport(void);
void SetCardIdInTestConfig(READER_ID_e rdrId, UINT64 cardId);
void InitSystemRebootTest(void);
void StartRebootTest(void);
UINT16 GenerateDesPattern(CHARPTR charPtr);
UINT16 PrintSubHeaderInFile(CHARPTR charPtr, CHARPTR strPtr);
#endif  // MXSYSTEST_H
