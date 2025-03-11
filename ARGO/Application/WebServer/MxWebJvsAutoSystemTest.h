#ifndef MXWEBJVSAUTOSYSTEMTEST_H_
#define MXWEBJVSAUTOSYSTEMTEST_H_
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
//   Project      : ACS ( Access Control System)
//   Created By   : Bhavin Panara
//   File         : MxWebJvsAutoSystemTest.h
//   Description  : 
//
/////////////////////////////////////////////////////////////////////////////
//system includes
#include <stdlib.h>
//Matrix include files
#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxSlaveCommonMsgList.h"
#include "MxSysTest.h"

//******** Defines and Data Types ****
typedef enum
{
    ARGO_VARIANT_FOE212=0,
	ARGO_VARIANT_FOM212,
	ARGO_VARIANT_FOI212,
	ARGO_VARIANT_CAE200,
	ARGO_VARIANT_CAM200,
	ARGO_VARIANT_CAI200,
	MAX_ARGO_VARIANT_TYPE,
}ARGO_VARIANT_TYPE_e;

enum
{
    EXTERNAL_U_INIT=0,
    EXTERNAL_U_DATA_SEND,
    EXTERNAL_U_DATA_RECV,
    EXTERNAL_W_INIT,
    EXTERNAL_W_DATA_SEND,
    EXTERNAL_W_DATA_RECV,
    EXTERNAL_TEST_COMPLETE
};
typedef enum
{
	AUTO_TEST_USB_NOT_CONNECTED,
	AUTO_TEST_LOW_SPEED_USB,
	AUTO_TEST_HIGH_SPEED_USB,
}AUTO_TEST_USB_CONNECTION_STATUS_e;

typedef enum
{
	AUTO_TEST_FRONT_USB_PORT,
	AUTO_TEST_BACK_USB_PORT,
	AUTO_TEST_MAX_USB_PORT,
}AUTO_TEST_USB_PORT_e;

typedef enum
{
	INTERNAL_CARD_LF,
	INTERNAL_CARD_HF,
	EXTERNAL_CARD_U,
	EXTERNAL_CARD_W,
	SYSTEM_TAMPER,
	CREATE_USER_AND_ENROLL_FP,
	BUZZ_LED,
	LCD_TEST,
	MAX_FUNCTIONAL_TEST
}FUNCTIONAL_TEST_e;


typedef enum
{
	//AUTO_TEST_AUX_RELAY,
	//AUTO_TEST_AUX_INPUT,
	//AUTO_TEST_LOCK_TAMPER,
	AUTO_TEST_DOOR_RELAY,
	AUTO_TEST_DOOR_SENCE,
	AUTO_TEST_EXIT_SWITCH,
//	AUTO_TEST_EXIT_READER_TAMPER,
	AUTO_TEST_USB,
	AUTO_TEST_MEMORY_CARD,
	AUTO_TEST_RTC,
	AUTO_TEST_WIFI,
	AUTO_TEST_BLUETOOTH,
	MAX_AUTO_TEST
}AUTO_TEST_e;

typedef enum
{
	TEST_STATUS_PENDING,
	TEST_STATUS_RUNNING,
	TEST_STATUS_FAIL,
	TEST_STATUS_SUCCESS,
	MAX_TEST_STATUS
}TEST_STATUS_e;

typedef enum
{
	AUTO_TEST_COMPLETED,
	AUTO_TEST_RUNNING,
	AUTO_TEST_CANCELLED_OR_ESC,
	MAX_AUTO_TEST_STATUS
}TEST_WAIT_STATUS_e;

#define AUTO_SYS_TEST_PARA_VER		1
typedef struct
{
	CHAR			empId[EMP_ID_SIZE];
	CHAR			devSrNo[DEVICE_SR_NO_SIZE];
	UINT8 			devType;		//COSEC DOOR - FOE212 = 1
									//COSEC DOOR - FOM212 = 2
									//COSEC DOOR - FOI212 = 3
									//COSEC DOOR - CAE = 4
									//COSEC DOOR - CAM = 5
									//COSEC DOOR - CAI = 6
	CHAR			testDate[DATE_STR_SIZE];
	CHAR			ftpIpaddr[MAX_IP_ADDRESS];
	CHAR			ftpUsrName[FTP_USR_NAME_SIZE];
	CHAR			ftpPwd[FTP_PWD_SIZE];
	TEST_STATUS_e 	autoTestStatus[MAX_AUTO_TEST];
	TEST_STATUS_e 	functionalTestStatus[MAX_FUNCTIONAL_TEST];

}AUTO_SYSTEM_TEST_PARA_t;

//******** Extern Variables **********
extern BOOL						IsESCButtonPressedF;

//******** Function Prototypes *******
UINT8 SetAutoTestInfo(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 GetThreadAutoTestInfo(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 GetAutoTestInfo(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 FillAutoTestInfo(FILE* pTXDataFile);
UINT8 SetSystemDefault(FILE *pRXDataFile, FILE *pTXDataFile);
VOID  DcAutoSystemTestStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
VOID  DcAutoSystemTestWaitStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
VOID  UpdateResultStatus(void);
AUTO_TEST_USB_CONNECTION_STATUS_e AutoTestUsbConnectionStatus(void);
void InitAutoSysTest(void);
BOOL ReadAutoSysTestPara(void);
BOOL LoadDfltAutoSysTestPara(void);
BOOL WriteAutoSysTestPara(void);
void UpdateAutoSysTestResultForConfig(void);
void GenerateAutoTestReport(void);
void SendAutoTestReport(void);
UINT8 SysAutoTestSendReportToFtp(FILE *pRXDataFile, FILE *pTXDataFile);
void DisplayScreenToStartSystemTest(UINT32 dummy);
UINT8 GetAutoTestParaDevType();
#endif /* MXWEBJVSAUTOSYSTEMTEST_H_ */
