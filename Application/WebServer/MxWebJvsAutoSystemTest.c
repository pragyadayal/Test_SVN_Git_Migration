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
//   File         : MxWebJvsAutoSystemTest.c
//   Description  : This File Contain Function for AutoSystem test functionality.
//
/////////////////////////////////////////////////////////////////////////////
// macro; to make use of recursive mutex
#define _GNU_SOURCE
//********Place all include files ****
//system includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>


//Matrix include files
#include "MxTypedef.h"
#include "MxSlaveCommonMsgList.h"
#include "MxWebJeeves.h"
#include "MxSysTime.h"
#include "MxDcTask.h"
#include "MxSysTest.h"
#include "MxSysUtils.h"
#include "MxConfigUpdateHandler.h"
#include "MxNetworkUtils.h"
#include "MxReaderTask.h"
#include "MxWebJvsAutoSystemTest.h"
#include "MxSysTest.h"
#include "Text2Pdf.h"
#include "MxMenu.h"
#include "MxSysReboot.h"
#include "MxWebJvsBluetoothSetting.h"
#include "MxBluetooth.h"
#include "MxBluetoothConfig.h"
#include "MxWebJvsWifiSetting.h"
#include "MxWifiPort.h"
#ifdef CER_READER_SUPPORTED
#include "MxCerTask.h"
#endif

//******** Defines and Data Types ****
#define AUTO_TEST_USB_SPEED_FILE				"/sys/bus/usb/devices/2-1/speed"
#define GENERATE_AUTO_OUTPUT_CHANGE_TIME		0.5		//in second
#define FUNCTIONAL_TEST_TIME					5
#define MASKED_FTP_PASSWORD			"................"

#define GEN_AUTO_SYS_TEST_INFO_ENUM(AUTO_SYS_TEST_INFO_ENUM, AUTO_SYS_TEST_INFO_NAME) AUTO_SYS_TEST_INFO_ENUM,
#define GEN_AUTO_SYS_TEST_INFO_NAME(AUTO_SYS_TEST_INFO_ENUM, AUTO_SYS_TEST_INFO_NAME) AUTO_SYS_TEST_INFO_NAME,

#define FUNCTIONAL_TEST_DURATION                10   //10 sec
#define AUTOMATED_TEST_DURATION                 15   //15 sec

#define SYS_TEST_BASIC_INFO(API)\
			API(AUTO_SYS_TEST_DEVICE_TYPE,			"doorType")\
			API(AUTO_SYS_TEST_DEVICE_SERIAL_NO,		"deviceSerialNumber")\
			API(AUTO_SYS_TEST_EMP_ID,				"employeeID")\
			API(AUTO_SYS_TEST_FTP_IP_ADDR,			"ftpIPAddress")\
			API(AUTO_SYS_TEST_FTP_USER_NAME,		"ftpUserName")\
			API(AUTO_SYS_TEST_FTP_PASSWORD,			"ftpPassword")\
			API(AUTO_SYS_TEST_START_TEST,			"startTest")\
			API(AUTO_SYS_TEST_DOOR_RELAY,	"doorRelay")\
			API(AUTO_SYS_TEST_DOOR_SENCE,	"doorSense")\
			API(AUTO_SYS_TEST_EXIT_SWITCH,	"exitSwitch")\
			API(AUTO_SYS_TEST_USB,	"USB")\
			API(AUTO_SYS_TEST_MEMORY_CARD,	"memoryCard")\
			API(AUTO_SYS_TEST_RTC,			"RTC")\
			API(AUTO_SYS_TEST_WIFI,			"wifi")\
			API(AUTO_SYS_TEST_BLE,			"bluetooth")\
			API(AUTO_SYS_INTERNAL_CARD_LF,			"intCardLF")\
			API(AUTO_SYS_INTERNAL_CARD_HF,			"intCardHF")\
			API(AUTO_SYS_EXTERNAL_CARD_U,			"extUReader")\
			API(AUTO_SYS_EXTERNAL_CARD_W,			"extWReader")\
			API(AUTO_SYS_SYSTEM_TAMPER,				"sysTamp")\
			API(AUTO_SYS_CREATE_USER_AND_ENROLL_FP,	"userEnroll")\
			API(AUTO_SYS_BUZZER_LED,				"buzzerLED")\
			API(AUTO_SYS_GENERATE_REPORT,		"generateReport")\
			API(AUTO_SYS_WAIT_STATUS,			"waitStatus")\
			API(AUTO_SYS_FUNCTIONAL_TEST_TIME,	"functionalTestTime")\
			API(AUTO_SYS_AUTO_TEST_TIME,		"autoTestTime")\


typedef enum
{
	SYS_TEST_BASIC_INFO(GEN_AUTO_SYS_TEST_INFO_ENUM)
	MAX_TOTAL_BASIC_INFO,
}SYS_TEST_BASIC_INFO_e;

//******** Function Prototypes *******
static BOOL StartGenerateOutputTimer(UINT8 outputCnt);
static void StartGenerateOutputTimerTimeOut(UINT32 outputCnt);
static void StartAutoSystemTest(VOID);

//******** Extern Variables **********
extern SYS_BASIC_PARA_t			SysBasicPara;
extern DOOR_ACTIVITY_INFO_t		DcInfo;
extern COMM_PORT_e				acmsCurActiveInterface;
extern SYS_ADVANCE_PARA_t		SysAdvPara;
extern READER_STATUS_e 			readerStatus[MAX_READERS];
extern ETHERNET_PARA_t 			EthernetParameters;
extern UINT32					SfprSensorSerialNumber;
extern THREAD_STATUS_e			text2PdfConverterThreadStatus ;
extern UINT8					BtDiscoveredCount;
extern BT_DISCOVERY_INFO_t 		BtDiscoveryInfo[MAX_BT_DISCOVER_DEVICE_COUNT];
extern MMC_INFO_t 				MmcInfo;
extern struct tm 				CurrTimeDate;
extern READER_AUTO_DETECT_PROC_INFO_t	ReaderAutoDetectInfo;
//******** Static Variables **********
static CHARPTR autoSysTestInfo [MAX_TOTAL_BASIC_INFO]=
{
		SYS_TEST_BASIC_INFO(GEN_AUTO_SYS_TEST_INFO_NAME)
};

static const CHARPTR autoTestResultStr[4] =
{
	"PENDING",
	"RUNNING",
	"FAIL",
	"SUCCESS",
};

static const CHARPTR deviceType[MAX_ARGO_VARIANT_TYPE+1] =
{
	"COSEC Door-FOE212",
	"COSEC Door-FOM212",
	"COSEC Door-FOI212",
	"COSEC Door-CAE200",
	"COSEC Door-CAM200",
	"COSEC Door-CAI200",
    "DEVICE NOT SELECTED",
};

const CHARPTR DeviceVariantType[MAX_ARGO_VARIANT_TYPE+1] =
{
    "ARGO_FOE212",
    "ARGO_FOM212",
    "ARGO_FOI212",
    "ARGO_CAE200",
    "ARGO_CAM200",
    "ARGO_CAI200",
    "Select",
};

//******** Static Variables **********
static UINT8 autoTestForRelay = SYSTEM_TEST_AUX_RELAY;
static TIMER_HANDLE				generateOutTmrHandle = INVALID_TIMER_HANDLE;
static TIMER_HANDLE				functionalTestTmrHandle = INVALID_TIMER_HANDLE;
static THREAD_STATUS_e			automatedTestThreadStatus = THREAD_INACTIVE;
static pthread_t				automatedTestThreadId = 0;
static VOIDPTR AutomatedTestThread(VOIDPTR dummy);
static THREAD_STATUS_e			functionalTestThreadStatus = THREAD_INACTIVE;
static pthread_t				functionalTestThreadId = 0;
static VOIDPTR FunctionalTestThread(VOIDPTR dummy);
static UINT8 CurrentRunningFnctnlTest = MAX_FUNCTIONAL_TEST;
static UINT32 functnlTestTimerTick	= 0, functionalTestTime = 0;
static	UINT32 autoTestTimerTick 	= 0, autoTestTime = 0;
static UINT8  DoorSenceInputChgCnt 	= 0;
static UINT8  TamperInputChangeCnt 	= 0;
static UINT8  RexInputChangeCnt 	= 0;
static UINT8 currentstate = EXTERNAL_U_INIT;
static BOOL isLowFreqencyF =  TRUE;

static const AUTO_SYSTEM_TEST_PARA_t  dfltAutoSysTestPara =
{
	.empId = "",
    .devType = MAX_ARGO_VARIANT_TYPE,
	.devSrNo = "",
	.testDate = "",
	.ftpIpaddr = "192.168.50.100",
	.ftpUsrName="matrix",
	.ftpPwd="matrix",
	.autoTestStatus = {[0 ... (MAX_AUTO_TEST-1)] = TEST_STATUS_PENDING},
	.functionalTestStatus = {[0 ... (MAX_FUNCTIONAL_TEST-1)] = TEST_STATUS_PENDING},
};

//******** Global Variables **********

TEST_STATUS_e FunctionalTestStatus[MAX_FUNCTIONAL_TEST] =
					{[0 ... (MAX_FUNCTIONAL_TEST-1)] = TEST_STATUS_PENDING};

TEST_STATUS_e AutoTestStatus[MAX_AUTO_TEST] =
					{[0 ... (MAX_AUTO_TEST-1)] = TEST_STATUS_PENDING};

AUTO_SYSTEM_TEST_PARA_t		AutoSystemTestPara;
BOOL						IsESCButtonPressedF = FALSE;
UINT8 						WifiSearchCnt = 0;

//******** Function Definations ******

//*****************************************************************************
//	SetAutoTestInfo()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description: This function is usefull for set auto test info.
//*****************************************************************************
UINT8 SetAutoTestInfo(FILE *pRXDataFile, FILE *pTXDataFile)
{
	CHAR 	tempFieldName[MAX_WEB_JVS_FIELD_NAME_LENGTH], tempFieldValue[MAX_WEB_JVS_FIELD_VALUE_LENGTH];
	UINT8	parseStatus, getElementStatus, elementIdx;

	while( (parseStatus = ParseQueryField(pRXDataFile, &tempFieldName[0], MAX_WEB_JVS_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_WEB_JVS_FIELD_VALUE_LENGTH) ) != FAIL)
	{

		DPRINT(WEBJVS_LOG,"%s=%s", tempFieldName, tempFieldValue);
		if( (getElementStatus = GetWebPageElementIdx(&autoSysTestInfo[0], MAX_TOTAL_BASIC_INFO, tempFieldName, &elementIdx)) == SUCCESS)
		{
			switch(elementIdx)
			{
				default:
					break;

				case AUTO_SYS_TEST_DEVICE_TYPE:
                {
                    if( atoi(tempFieldValue) >= 0 && atoi(tempFieldValue) < MAX_ARGO_VARIANT_TYPE )
                        AutoSystemTestPara.devType = atoi(tempFieldValue);
                    else
                        AutoSystemTestPara.devType = MAX_ARGO_VARIANT_TYPE ;
				}break;

				case AUTO_SYS_TEST_DEVICE_SERIAL_NO:
				{
					sprintf(AutoSystemTestPara.devSrNo,"%s",tempFieldValue);
				}break;

				case AUTO_SYS_TEST_EMP_ID:
				{
					sprintf(AutoSystemTestPara.empId,"%s",tempFieldValue);
				}break;

				case AUTO_SYS_TEST_FTP_IP_ADDR:
				{
					sprintf(AutoSystemTestPara.ftpIpaddr,"%s",tempFieldValue);
				}break;

				case AUTO_SYS_TEST_FTP_USER_NAME:
				{
					sprintf(AutoSystemTestPara.ftpUsrName,"%s",tempFieldValue);
				}break;

				case AUTO_SYS_TEST_FTP_PASSWORD:
				{
					if(strncmp(tempFieldValue, MASKED_FTP_PASSWORD, FTP_PWD_SIZE) != 0)
						sprintf(AutoSystemTestPara.ftpPwd, "%s",tempFieldValue);
				}break;

				case AUTO_SYS_GENERATE_REPORT:
				{
					UINT32 generateReportTimerTick = GetSysTick();
					GenerateAutoTestReport();
					ConvertTxtFileToPdf();
					while(text2PdfConverterThreadStatus != THREAD_DONE)
					{
						usleep(100);
					}
					text2PdfConverterThreadStatus = THREAD_INACTIVE;
					DPRINT(WEBJVS_LOG, "Report Generated In time[%d ms]", ElapsedTick(generateReportTimerTick));
				}break;

				case AUTO_SYS_BUZZER_LED:
				{
					FunctionalTestStatus[BUZZ_LED] = TEST_STATUS_FAIL;
					UpdateAutoSysTestResultForConfig();
					WriteAutoSysTestPara();
				}break;

				case AUTO_SYS_TEST_START_TEST:
				{
					#ifdef OEM_SUPPORTED
					OEM_SetDefaultSysCodeInPlatform();
					#endif //#ifdef OEM_SUPPORTED
					CleanUpTempUserFP();
					DcInfo.curState = DC_AUTO_SYSTEM_TEST_WAIT_STATE;
					DisplayScreenToStartSystemTest(0);

				}break;
			}
		}
		if( (parseStatus == WEB_JVS_LAST_FIELD) || (parseStatus == WEB_JVS_LAST_FIELD_WITHOUT_DATA) )
		{
			WriteWebResponse(pTXDataFile, "0");
			WriteJSONStartTag(pTXDataFile);	//JSON Data Start
			WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_WAIT_STATUS], AUTO_TEST_RUNNING);
			FillAutoTestInfo(pTXDataFile);
			WriteJSONENDTag(pTXDataFile);	//JSON Data END
			return SUCCESS;
		}
	}

	return SUCCESS;
}

//*****************************************************************************
//	StartAutoSystemTest()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description: This Function will start System Test Threads.
//*****************************************************************************
static void StartAutoSystemTest(VOID)
{
	SYS_BASIC_PARA_t	tempSysBasicPara;

	functnlTestTimerTick = GetSysTick();
	autoTestTimerTick = GetSysTick();

	SysBasicPara.userDefNtp = TRUE;
	sprintf(SysBasicPara.userDefNtpAddr, "%s", "192.168.50.100");
	tempSysBasicPara	=	SysBasicPara;

	StartTimeSync();

	// Start Test request only when button enable from page.
	automatedTestThreadStatus 	= THREAD_INACTIVE;
	functionalTestThreadStatus	= THREAD_INACTIVE;
	//Set Count To Zero
	DoorSenceInputChgCnt 	= 0;
	TamperInputChangeCnt 	= 0;
	RexInputChangeCnt 		= 0;

	// Set Value For Aux Input test
	SysAdvPara.auxInput.debounceTime = 0;

	//Set All Status To PENDING
	memset(FunctionalTestStatus, TEST_STATUS_PENDING, sizeof(FunctionalTestStatus));
	memset(AutoTestStatus, TEST_STATUS_PENDING, sizeof(AutoTestStatus));

	//Change Status Of KeyPad test.
	FunctionalTestStatus[LCD_TEST] = TEST_STATUS_SUCCESS;

	if(pthread_create(&automatedTestThreadId, NULL, AutomatedTestThread, NULL) == 0)
	{
		automatedTestThreadStatus = THREAD_ACTIVE;
		pthread_detach(automatedTestThreadId);
	}
	else
	{
		automatedTestThreadStatus = THREAD_INACTIVE;
	}

	if(AutoSystemTestPara.devType == ARGO_VARIANT_FOE212 /*FOE-212*/ )
	{
		tempSysBasicPara.reader[READER1].type = SM_PROX_READER;
		tempSysBasicPara.reader[READER2].type = SUP_FP_3030_READER;
	}
	else if(AutoSystemTestPara.devType == ARGO_VARIANT_FOM212 /*FOM-212*/ )
	{
		tempSysBasicPara.reader[READER1].type = MIFARE_U_READER;
		tempSysBasicPara.reader[READER2].type = SUP_FP_3030_READER;
	}
	else if(AutoSystemTestPara.devType == ARGO_VARIANT_FOI212 /*FOI-212*/ )
	{
		tempSysBasicPara.reader[READER1].type = HID_ICLASS_U_READER;
		tempSysBasicPara.reader[READER2].type = SUP_FP_3030_READER;
	}
	else if(AutoSystemTestPara.devType == ARGO_VARIANT_CAE200 /*CAE*/ )
	{
		tempSysBasicPara.reader[READER1].type = SM_PROX_READER;
		tempSysBasicPara.reader[READER2].type = NO_READER_PRESENT;
	}
	else if(AutoSystemTestPara.devType == ARGO_VARIANT_CAM200 /*CAM*/ )
	{
		tempSysBasicPara.reader[READER1].type = MIFARE_U_READER;
		tempSysBasicPara.reader[READER2].type = NO_READER_PRESENT;
	}
	else if(AutoSystemTestPara.devType == ARGO_VARIANT_CAI200 /*CAI*/ )
	{
		tempSysBasicPara.reader[READER1].type = HID_ICLASS_U_READER;
		tempSysBasicPara.reader[READER2].type = NO_READER_PRESENT;
	}

	if(ATOM_RD100_READER == ReaderAutoDetectInfo.readerType[READER4])
	{
		tempSysBasicPara.reader[READER3].type = ATOM_RD100_READER;
	}
	else
	{
		tempSysBasicPara.reader[READER3].type = HID_ICLASS_U_READER;
	}

	ConfigUpdateHandler(GEN_SYS_BASIC_TABLE_ID, 0, (VOIDPTR)(&tempSysBasicPara) );

	if(pthread_create(&functionalTestThreadId, NULL, FunctionalTestThread, NULL) == 0)
	{
		functionalTestThreadStatus = THREAD_ACTIVE;
		pthread_detach(functionalTestThreadId);
	}
	else
	{
		functionalTestThreadStatus = THREAD_INACTIVE;
	}
}

//*****************************************************************************
//	GetAutoTestInfo()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description: This Function Use full for Get information from page.
//*****************************************************************************
UINT8 GetThreadAutoTestInfo(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	if ((automatedTestThreadStatus == THREAD_DONE) && (functionalTestThreadStatus == THREAD_DONE))
	{
		automatedTestThreadStatus = THREAD_INACTIVE;
		functionalTestThreadStatus = THREAD_INACTIVE;
		SwitchDcToIdleState();
		UpdateAutoSysTestResultForConfig();
		WriteAutoSysTestPara();
		WriteWebResponse(pTXDataFile, "0");
		WriteJSONStartTag(pTXDataFile);	//JSON Data Start
		WriteJSONNumericData(pTXDataFile, autoSysTestInfo[AUTO_SYS_WAIT_STATUS], AUTO_TEST_COMPLETED);
		FillAutoTestInfo(pTXDataFile);
		WriteJSONENDTag(pTXDataFile);	//JSON Data END
		return SUCCESS;
	}

	if((automatedTestThreadStatus == THREAD_ACTIVE) || (functionalTestThreadStatus == THREAD_ACTIVE))
	{
		WriteWebResponse(pTXDataFile, "0");
		WriteJSONStartTag(pTXDataFile);	//JSON Data Start
		WriteJSONNumericData(pTXDataFile, autoSysTestInfo[AUTO_SYS_WAIT_STATUS], AUTO_TEST_RUNNING);
		FillAutoTestInfo(pTXDataFile);
		WriteJSONENDTag(pTXDataFile);	//JSON Data END
		return SUCCESS;
	}

	WriteWebResponse(pTXDataFile, "0");
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	if(IsESCButtonPressedF == FALSE)
	{
		WriteJSONNumericData(pTXDataFile, autoSysTestInfo[AUTO_SYS_WAIT_STATUS], AUTO_TEST_RUNNING);
	}
	else
	{
		IsESCButtonPressedF = FALSE;
		WriteJSONNumericData(pTXDataFile, autoSysTestInfo[AUTO_SYS_WAIT_STATUS], AUTO_TEST_CANCELLED_OR_ESC);
	}
	FillAutoTestInfo(pTXDataFile);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END
	return SUCCESS;
}
//*****************************************************************************
//	GetAutoTestInfo()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description: This Function Use full for Get information from page.
//*****************************************************************************
UINT8 GetAutoTestInfo(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	WriteWebResponse(pTXDataFile, "0");
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	FillAutoTestInfo(pTXDataFile);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END
	return SUCCESS;
}
//*****************************************************************************
//	FillAutoTestInfo()
//	Param:
//		IN	:
//		OUT	:
//	Returns	:
//	Description:
//		This function Fill Auto Test Info
//*****************************************************************************
UINT8 FillAutoTestInfo(FILE* pTXDataFile)
{
	// General Data
    if(AutoSystemTestPara.devType < MAX_ARGO_VARIANT_TYPE)
        WriteJSONNumericData(pTXDataFile, autoSysTestInfo[AUTO_SYS_TEST_DEVICE_TYPE], AutoSystemTestPara.devType);
    else
        WriteJSONNumericData(pTXDataFile, autoSysTestInfo[AUTO_SYS_TEST_DEVICE_TYPE], MAX_ARGO_VARIANT_TYPE);   // assign for "Select" Display
	WriteJSONNumericData(pTXDataFile, autoSysTestInfo[AUTO_SYS_TEST_DEVICE_TYPE], AutoSystemTestPara.devType);
	WriteJSONStringData(pTXDataFile, autoSysTestInfo[AUTO_SYS_TEST_DEVICE_SERIAL_NO], AutoSystemTestPara.devSrNo);
	WriteJSONStringData(pTXDataFile, autoSysTestInfo[AUTO_SYS_TEST_EMP_ID], AutoSystemTestPara.empId);
	WriteJSONStringData(pTXDataFile, autoSysTestInfo[AUTO_SYS_TEST_FTP_IP_ADDR], AutoSystemTestPara.ftpIpaddr);
	WriteJSONStringData(pTXDataFile, autoSysTestInfo[AUTO_SYS_TEST_FTP_USER_NAME], AutoSystemTestPara.ftpUsrName);
	WriteJSONStringData(pTXDataFile, autoSysTestInfo[AUTO_SYS_TEST_FTP_PASSWORD], MASKED_FTP_PASSWORD);
	//Auto Test Status

	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_TEST_DOOR_RELAY], AutoTestStatus[AUTO_TEST_DOOR_RELAY]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_TEST_DOOR_SENCE], AutoTestStatus[AUTO_TEST_DOOR_SENCE]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_TEST_EXIT_SWITCH], AutoTestStatus[AUTO_TEST_EXIT_SWITCH]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_TEST_USB], AutoTestStatus[AUTO_TEST_USB]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_TEST_MEMORY_CARD], AutoTestStatus[AUTO_TEST_MEMORY_CARD]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_TEST_RTC], AutoTestStatus[AUTO_TEST_RTC]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_TEST_WIFI], AutoTestStatus[AUTO_TEST_WIFI]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_TEST_BLE], AutoTestStatus[AUTO_TEST_BLUETOOTH]);

	//Functional Test Status
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_INTERNAL_CARD_LF], FunctionalTestStatus[INTERNAL_CARD_LF]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_INTERNAL_CARD_HF], FunctionalTestStatus[INTERNAL_CARD_HF]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_EXTERNAL_CARD_U], FunctionalTestStatus[EXTERNAL_CARD_U]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_EXTERNAL_CARD_W], FunctionalTestStatus[EXTERNAL_CARD_W]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_SYSTEM_TAMPER], FunctionalTestStatus[SYSTEM_TAMPER]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_CREATE_USER_AND_ENROLL_FP], FunctionalTestStatus[CREATE_USER_AND_ENROLL_FP]);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_BUZZER_LED], FunctionalTestStatus[BUZZ_LED]);

	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_FUNCTIONAL_TEST_TIME], functionalTestTime);
	WriteJSONNumericData(pTXDataFile,autoSysTestInfo[AUTO_SYS_AUTO_TEST_TIME], autoTestTime);

	return 0;
}
//*****************************************************************************
//	AutomatedTestThread()
//	Param:
//		IN	:
//		OUT	:
//	Returns	:
//	Description:
//		This function perform automated test.
//*****************************************************************************
static VOIDPTR AutomatedTestThread(UNUSED_PARA VOIDPTR dummy)
{
	CHAR			tempBuf[64];
	BOOL			result = TEST_STATUS_PENDING;
	UINT8 			retryCnt = 0;
	WIFI_NW_INFO_t	wifiInfo[MAX_WIFI_SEARCH_DEVICES];
    BOOL firstTimeWiFiF = TRUE;
    UINT32 autoTestTick = GetSysTick();

	// Generate Out Put For DOOR RELAY
	AutoTestStatus[AUTO_TEST_DOOR_RELAY] = TEST_STATUS_RUNNING;
	AutoTestStatus[AUTO_TEST_EXIT_SWITCH] = TEST_STATUS_RUNNING;
	AutoTestStatus[AUTO_TEST_DOOR_SENCE] = TEST_STATUS_RUNNING;
	DeactivateDoorRelay();
	autoTestForRelay = SYSTEM_TEST_DOOR_RELAY;
	StartGenerateOutputTimer(0);

    // Memory Card
    AutoTestStatus[AUTO_TEST_MEMORY_CARD] = TEST_STATUS_RUNNING;
    if(MmcInfo.status == MMC_CONNECTED)
    {
        AutoTestStatus[AUTO_TEST_MEMORY_CARD] = TEST_STATUS_SUCCESS;
    }
    else
    {
        AutoTestStatus[AUTO_TEST_MEMORY_CARD] = TEST_STATUS_FAIL;
    }


	SysBasicPara.timeUpdateMode = AUTO_TIME;
	sprintf(tempBuf, "su - se -c \"sntp -4 -s -t 3 %s &\"", (GetServerIpAddressPtr(acmsCurActiveInterface) ) );
	system(tempBuf);

    //WIFI Test
    AutoTestStatus[AUTO_TEST_WIFI] = TEST_STATUS_RUNNING;
    StartWifiEnableDisableThreadFunc(ENABLED);

    //Bluetooth Test
    AutoTestStatus[AUTO_TEST_BLUETOOTH] = TEST_STATUS_RUNNING;
    BtDiscoveredCount = 0;
    memset((VOIDPTR)BtDiscoveryInfo, 0, sizeof(BtDiscoveryInfo));

    while(ElapsedTick(autoTestTick)<= AUTOMATED_TEST_DURATION*1000)
    {
        if( (AutoTestStatus[AUTO_TEST_USB]!= TEST_STATUS_RUNNING) && ( AutoTestStatus[AUTO_TEST_WIFI] != TEST_STATUS_RUNNING) && (AutoTestStatus[AUTO_TEST_BLUETOOTH] != TEST_STATUS_RUNNING))
        {
            break;
        }
        if(StartBtAutoSystemTest() != WAIT)
        {
            if(BtDiscoveredCount > 0)
            {
                AutoTestStatus[AUTO_TEST_BLUETOOTH] = TEST_STATUS_SUCCESS;
            }
        }
        if(AutoTestUsbConnectionStatus() == AUTO_TEST_HIGH_SPEED_USB)
        {
            AutoTestStatus[AUTO_TEST_USB] = TEST_STATUS_SUCCESS;
        }
        else
        {
            AutoTestStatus[AUTO_TEST_USB] = TEST_STATUS_FAIL;
        }
        if(GetWiFiStatus() != NO_WIFI)
        {
            if(firstTimeWiFiF == TRUE)
            {
                if(SearchWifiDevices(wifiInfo) > 0)
                {
                    AutoTestStatus[AUTO_TEST_WIFI] = TEST_STATUS_SUCCESS;
                }
                else
                {
                    AutoTestStatus[AUTO_TEST_WIFI] = TEST_STATUS_FAIL;
                }
                firstTimeWiFiF = FALSE;
            }
        }
        usleep(500 * 1000);   //500 msec sleep
    }
    if(AutoTestStatus[AUTO_TEST_WIFI] != TEST_STATUS_SUCCESS)
    {
        AutoTestStatus[AUTO_TEST_WIFI] = TEST_STATUS_FAIL;
    }
    if(BtDiscoveredCount > 0)
    {
        AutoTestStatus[AUTO_TEST_BLUETOOTH] = TEST_STATUS_SUCCESS;
    }
    else
    {
        AutoTestStatus[AUTO_TEST_BLUETOOTH] = TEST_STATUS_FAIL;
    }
    if(AutoTestStatus[AUTO_TEST_USB] != TEST_STATUS_SUCCESS)
        AutoTestStatus[AUTO_TEST_USB] = TEST_STATUS_FAIL;


	//Set Value Back To System Default.
	SysAdvPara.auxInput.debounceTime = 3;

	automatedTestThreadStatus = THREAD_DONE;
	autoTestTime = ElapsedTick(autoTestTimerTick)/1000;
	DPRINT(SYS_LOG, "Autometed Test Compleated In  time[%d sec]", autoTestTime);
	pthread_exit(NULL);
}

//*****************************************************************************
//	DoExternalReaderTest()
//	Param:
//		IN	: Elapsed time
//		OUT	:
//	Returns	: None
//	Description:
//		This function perform external reader Auto detection task. and perform
//      unit testing of UART and Wigend sequencially.
//*****************************************************************************
void DoExternalReaderTest(UINT32 time)
{
    SYS_BASIC_PARA_t	tempSysBasicPara =	SysBasicPara;
    BOOL msg = TRUE;

    //check for uart fail 5 sec
    if(time >= 5 && currentstate < EXTERNAL_W_INIT)
    {
        currentstate = EXTERNAL_W_INIT;
    }

    switch (currentstate)
    {
        case EXTERNAL_U_INIT :
        {
            if(GetReaderStatus(READER3) == READER_IS_ACTIVE)
            {
                currentstate++;
            }
        }break;
        case EXTERNAL_U_DATA_SEND :
        {
            //send data to uart with start uart mode
            #ifdef CER_READER_SUPPORTED
            InsertMsgToCERTxQ(CER_TX_DOOR_SYSTEM_TEST_RS232_START, sizeof(msg), (UINT8PTR)&msg);
            currentstate++;
            #endif
        }break;
        case EXTERNAL_U_DATA_RECV :
        {
            if(FunctionalTestStatus[EXTERNAL_CARD_U] != TEST_STATUS_RUNNING)
            {
                currentstate++;
            }
            else
            {
                currentstate--;
            }
        }break;
        case EXTERNAL_W_INIT :
        {
            if(FunctionalTestStatus[EXTERNAL_CARD_U] != TEST_STATUS_SUCCESS)
            {
                FunctionalTestStatus[EXTERNAL_CARD_U] = TEST_STATUS_FAIL;
            }
            FunctionalTestStatus[EXTERNAL_CARD_W] = TEST_STATUS_RUNNING;
            //send data to uart with start wigend mode
            #ifdef CER_READER_SUPPORTED
            InsertMsgToCERTxQ(CER_TX_DOOR_SYSTEM_TEST_WEIGAND_START, sizeof(msg), (UINT8PTR)&msg);
            usleep(200);
            #endif

            // Set External reader to HID-W-READER
            tempSysBasicPara.reader[READER3].type = CB_W_READER;
            ConfigUpdateHandler(GEN_SYS_BASIC_TABLE_ID, 0, (VOIDPTR)(&tempSysBasicPara) );
            currentstate++;
        }break;
        case EXTERNAL_W_DATA_SEND :
        {
            if(GetReaderStatus(READER3) == READER_STATUS_UNKNOWN)
            {
                currentstate++;
            }
        }break;
        case EXTERNAL_W_DATA_RECV :
        {
            if(FunctionalTestStatus[EXTERNAL_CARD_W] != TEST_STATUS_RUNNING)
            {
                currentstate++;
            }
        }break;
        default :
            break;
    }
}
//*****************************************************************************
//	FunctionalTestThread()
//	Param:
//		IN	:
//		OUT	:
//	Returns	:
//	Description:
//		This function perform functional test.
//*****************************************************************************
static VOIDPTR FunctionalTestThread(UNUSED_PARA VOIDPTR dummy)
{
	UINT8 retryCnt = 0;
    SYS_BASIC_PARA_t	tempSysBasicPara =	SysBasicPara;
    struct timeval	tv1, tv2;
    currentstate = EXTERNAL_U_INIT;
    isLowFreqencyF = TRUE;

    FunctionalTestStatus[INTERNAL_CARD_LF] = TEST_STATUS_RUNNING;
    FunctionalTestStatus[INTERNAL_CARD_HF] = TEST_STATUS_RUNNING;
    FunctionalTestStatus[EXTERNAL_CARD_U] = TEST_STATUS_RUNNING;
    FunctionalTestStatus[SYSTEM_TAMPER] = TEST_STATUS_RUNNING;
    FunctionalTestStatus[CREATE_USER_AND_ENROLL_FP] = TEST_STATUS_RUNNING;
    FunctionalTestStatus[BUZZ_LED] = TEST_STATUS_RUNNING;
    AutoTestStatus[AUTO_TEST_RTC] = TEST_STATUS_RUNNING;

    UINT32 testStartTime = GetSysTick();
    GetTimeFromRTC(&tv1);
    while(ElapsedTick(testStartTime)<=FUNCTIONAL_TEST_DURATION *1000)
    {
          DoExternalReaderTest(ElapsedTick(testStartTime)/1000);
          usleep(500 * 1000); //Sleep for 500ms
    }
    GetTimeFromRTC(&tv2);
    if( (tv2.tv_sec - tv1.tv_sec) < FUNCTIONAL_TEST_DURATION-1 || (tv2.tv_sec - tv1.tv_sec) > FUNCTIONAL_TEST_DURATION+1)
    {
        AutoTestStatus[AUTO_TEST_RTC] = TEST_STATUS_FAIL;
    }
    else
    {
        AutoTestStatus[AUTO_TEST_RTC] = TEST_STATUS_SUCCESS;
    }
     //Fail all the test which are not passed
     if((AutoSystemTestPara.devType == ARGO_VARIANT_CAE200) || (AutoSystemTestPara.devType == ARGO_VARIANT_FOE212) || (AutoSystemTestPara.devType == ARGO_VARIANT_FOI212) || (AutoSystemTestPara.devType == ARGO_VARIANT_CAI200))
     {
         if (FunctionalTestStatus[INTERNAL_CARD_LF] != TEST_STATUS_SUCCESS)
             FunctionalTestStatus[INTERNAL_CARD_LF] = TEST_STATUS_FAIL;
     }
     if((AutoSystemTestPara.devType == ARGO_VARIANT_FOM212) || (AutoSystemTestPara.devType == ARGO_VARIANT_CAM200)|| (AutoSystemTestPara.devType == ARGO_VARIANT_FOI212)|| (AutoSystemTestPara.devType == ARGO_VARIANT_CAI200))
     {
         if(FunctionalTestStatus[INTERNAL_CARD_HF] != TEST_STATUS_SUCCESS)
             FunctionalTestStatus[INTERNAL_CARD_HF] = TEST_STATUS_FAIL;
     }
     if(FunctionalTestStatus[EXTERNAL_CARD_U]  != TEST_STATUS_SUCCESS)
         FunctionalTestStatus[EXTERNAL_CARD_U] = TEST_STATUS_FAIL;
     if(FunctionalTestStatus[EXTERNAL_CARD_W]  != TEST_STATUS_SUCCESS)
         FunctionalTestStatus[EXTERNAL_CARD_W] = TEST_STATUS_FAIL;
     if(FunctionalTestStatus[SYSTEM_TAMPER] != TEST_STATUS_SUCCESS)
         FunctionalTestStatus[SYSTEM_TAMPER] = TEST_STATUS_FAIL;
     if(FunctionalTestStatus[CREATE_USER_AND_ENROLL_FP] != TEST_STATUS_SUCCESS)
         FunctionalTestStatus[CREATE_USER_AND_ENROLL_FP] = TEST_STATUS_FAIL;
     if(FunctionalTestStatus[BUZZ_LED] != TEST_STATUS_SUCCESS)
         FunctionalTestStatus[BUZZ_LED] = TEST_STATUS_FAIL;


	functionalTestThreadStatus = THREAD_DONE;
	functionalTestTime = ElapsedTick(functnlTestTimerTick)/1000;
	DPRINT(SYS_LOG, "Functional Test Compleated In  time[%d sec]", functionalTestTime);
	pthread_exit(NULL);
}

//**********************************************************************************
//	DcAutoSystemTestWaitStateFn()
//	Param:
//		IN	:
//		OUT	:
//	Returns	:
//	Description:
//		This function will check user response (either to start system test or not)
//**********************************************************************************
void DcAutoSystemTestWaitStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UNUSED_PARA UINT16 length)
{
	DPRINT(DC_LOG,"%x %x", msgId, dataPtr);

	switch (msgId)
	{
		default:
			break;

  		case GUI_BUTTON_AUTO_SYSTEM_TEST_CANCEL:
		{
			IsESCButtonPressedF = TRUE;
			DeleteTimer(&DcInfo.sysTestScreenWaitTimerhandle);
			SwitchDcToIdleState();
			SendMessageToQt("", GUI_MSG_DISPLAY_IDLE_SCREEN);
		}break;

		case GUI_BUTTON_AUTO_SYSTEM_TEST_START:
		{
			DeleteTimer(&DcInfo.sysTestScreenWaitTimerhandle);
			CleanUpTempUserFP();
			DcInfo.curState = DC_AUTO_SYSTEM_TEST_STATE;
			SendMessageToQt("", GUI_MSG_AUTO_SYSTEM_TEST_RUNNING_SCREEN);
			StartAutoSystemTest();
		}break;
	}
}
//*****************************************************************************
//	DcAutoSystemTestStateFn()
//	Param:
//		IN	:
//		OUT	:
//	Returns	:
//	Description:
//		This function recive all Dc task event for Auto test state.
//*****************************************************************************
void DcAutoSystemTestStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UNUSED_PARA UINT16 length)
{
	READER_ID_e readerId = ALL_FP_READERS;
	UINT64 cardId = 0;
	DPRINT(DC_LOG,"%x", msgId);

	switch (msgId)
	{
		default:break;

		case MSG_DOOR_SENSE_CHANGE_EVENT:
		{
			if(AutoTestStatus[AUTO_TEST_DOOR_SENCE] == TEST_STATUS_RUNNING)
			{
				DoorSenceInputChgCnt++;
			}
		}
		break;

		case MSG_REX_IP_CHANGE_EVENT:
		{
			if(AutoTestStatus[AUTO_TEST_EXIT_SWITCH] == TEST_STATUS_RUNNING)
			{
				RexInputChangeCnt++;
			}
		}
		break;

		case MSG_TAMPER_IP_CHANGE_EVENT:
		{
            TamperInputChangeCnt++;

            if(TamperInputChangeCnt < 2)
            {
                break;
            }

            if(TamperInputChangeCnt >=2)
            {
                FunctionalTestStatus[SYSTEM_TAMPER] = TEST_STATUS_SUCCESS;
            }
            else
            {
                FunctionalTestStatus[SYSTEM_TAMPER] = TEST_STATUS_FAIL;
            }
		}
		break;

		case MSG_CARD_DETECTED_EVENT:
		{
            readerId = ((CARD_DETECTED_EV_DATA_t *)dataPtr)->readerId;
            cardId = ((CARD_DETECTED_EV_DATA_t *)dataPtr)->cardId;

            if(readerId == READER1)
            {
                //for ARGO FOE & CAE
                if( AutoSystemTestPara.devType == ARGO_VARIANT_FOE212 || AutoSystemTestPara.devType == ARGO_VARIANT_CAE200 )
                {
                    if(cardId != 0)
                    {
                        FunctionalTestStatus[INTERNAL_CARD_LF] = TEST_STATUS_SUCCESS;
                        SetBuzzer(ACCESS_ALLOWED_RESP, INTERNAL_PORT);
                        FunctionalTestStatus[BUZZ_LED] = TEST_STATUS_SUCCESS;
                    }
                    else
                    {
                        FunctionalTestStatus[INTERNAL_CARD_LF] = TEST_STATUS_FAIL;
                    }
                }
                //for ARGO FOM & CAM
                else if( AutoSystemTestPara.devType == ARGO_VARIANT_FOM212 || AutoSystemTestPara.devType == ARGO_VARIANT_CAM200 )
                {
                    if(cardId != 0)
                    {
                        FunctionalTestStatus[INTERNAL_CARD_HF] = TEST_STATUS_SUCCESS;
                        SetBuzzer(ACCESS_ALLOWED_RESP, INTERNAL_PORT);
                        FunctionalTestStatus[BUZZ_LED] = TEST_STATUS_SUCCESS;
                    }
                    else
                    {
                        FunctionalTestStatus[INTERNAL_CARD_HF] = TEST_STATUS_FAIL;
                    }
                }
                //for ARGO FOI & CAI
                else if( AutoSystemTestPara.devType == ARGO_VARIANT_FOI212 || AutoSystemTestPara.devType == ARGO_VARIANT_CAI200 )
                {
                    //for LF
                    if(isLowFreqencyF == TRUE)
                    {
                        if(cardId != 0)
                        {
                            SetBuzzer(ACCESS_ALLOWED_RESP, INTERNAL_PORT);
                            FunctionalTestStatus[INTERNAL_CARD_LF] = TEST_STATUS_SUCCESS;
                            FunctionalTestStatus[BUZZ_LED] = TEST_STATUS_SUCCESS;
                        }
                        else
                        {
                            FunctionalTestStatus[INTERNAL_CARD_LF] = TEST_STATUS_FAIL;
                        }
                        isLowFreqencyF = FALSE;
                    }
                    //for HF
                    else // for isLowFreqencyF = FALSE
                    {
                        if(cardId != 0)
                        {
                            SetBuzzer(ACCESS_ALLOWED_RESP, INTERNAL_PORT);
                            FunctionalTestStatus[INTERNAL_CARD_HF] = TEST_STATUS_SUCCESS;
                            FunctionalTestStatus[BUZZ_LED] = TEST_STATUS_SUCCESS;
                        }
                        else
                        {
                            FunctionalTestStatus[INTERNAL_CARD_HF] = TEST_STATUS_FAIL;
                        }
                    }
                }
            }
            if(readerId == READER3)
            {
                if(cardId != 0)
                {
                    FunctionalTestStatus[EXTERNAL_CARD_W] = TEST_STATUS_SUCCESS;
                }
                else
                {
                    FunctionalTestStatus[EXTERNAL_CARD_W] = TEST_STATUS_FAIL;
                }
            }
		}break;

		case MSG_FP_DETECTED_EVENT:
        {
            FunctionalTestStatus[CREATE_USER_AND_ENROLL_FP] = TEST_STATUS_SUCCESS;
            SetBuzzer(ACCESS_ALLOWED_RESP, INTERNAL_PORT);
            FunctionalTestStatus[BUZZ_LED] = TEST_STATUS_SUCCESS;
		}break;

        case MSG_RDR_RS232_DATA_DOOR_SYSTEM_TEST:
        {
                FunctionalTestStatus[EXTERNAL_CARD_U] = TEST_STATUS_SUCCESS;
        }break;
	}
}

//*****************************************************************************
//	StartTwoStateOutputTimer()
//	Param:
//		IN	:	UINT8 outputCnt
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//		This function start output timer to toggle output state for testing the
//		output
//*****************************************************************************
static BOOL StartGenerateOutputTimer(UINT8 outputCnt)
{
	TIMER_INFO_t	   generateOutPutTimer;

	if(generateOutTmrHandle == INVALID_TIMER_HANDLE)
	{
		generateOutPutTimer.funcPtr = &StartGenerateOutputTimerTimeOut;
		generateOutPutTimer.count = (GENERATE_AUTO_OUTPUT_CHANGE_TIME*10);
		generateOutPutTimer.data = outputCnt;

		return (StartTimer(generateOutPutTimer, &generateOutTmrHandle, __FUNCTION__) );
	}
	else
	{
		return (ReloadTimer(generateOutTmrHandle, (GENERATE_AUTO_OUTPUT_CHANGE_TIME*10) ) );
	}
}
//*****************************************************************************
//	StartGenerateOutputTimerTimeOut()
//	Param:
//		IN	:	UINT32 outputCnt
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//		This function is timeout function of twoStateOutTimer.
//*****************************************************************************
static void StartGenerateOutputTimerTimeOut(UINT32 outputCnt)
{
	DeleteTimer(&generateOutTmrHandle);
	if(outputCnt < 4)
	{
		if( (outputCnt % 2) == 0)
		{
			ActivateDoorRelay(0);
		}
		else
		{
			DeactivateDoorRelay();
		}
		outputCnt++;
		StartGenerateOutputTimer( (UINT8)outputCnt);
	}
	else
	{
		generateOutTmrHandle = INVALID_TIMER_HANDLE;
		UpdateResultStatus();
	}

}
//*****************************************************************************
//	UpdateResultStatus()
//	Param:
//		IN	:	NONE
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//				This function switch into idle state.
//*****************************************************************************
void UpdateResultStatus(void)
{
	//Update Result

	if(RexInputChangeCnt >=4)
	{
		AutoTestStatus[AUTO_TEST_EXIT_SWITCH] = TEST_STATUS_SUCCESS;
	}
	else
	{
		AutoTestStatus[AUTO_TEST_EXIT_SWITCH] = TEST_STATUS_FAIL;
	}

	if(DoorSenceInputChgCnt >=4)
	{
		AutoTestStatus[AUTO_TEST_DOOR_SENCE] = TEST_STATUS_SUCCESS;
	}
	else
	{
		AutoTestStatus[AUTO_TEST_DOOR_SENCE] = TEST_STATUS_FAIL;
	}

	if(	(AutoTestStatus[AUTO_TEST_EXIT_SWITCH] == TEST_STATUS_SUCCESS) ||
		(AutoTestStatus[AUTO_TEST_DOOR_SENCE] == TEST_STATUS_SUCCESS) )
	{
		AutoTestStatus[AUTO_TEST_DOOR_RELAY] = TEST_STATUS_SUCCESS;
	}
	else
	{
		AutoTestStatus[AUTO_TEST_DOOR_RELAY] = TEST_STATUS_FAIL;
	}
}

//*****************************************************************************
//	AutoTestUsbConnectionStatus()
//	Param:
//		IN: NONE
//		OUT:NONE.
//	Returns:
//		NONE
//	Description:
//		This function return USB status
//*****************************************************************************
AUTO_TEST_USB_CONNECTION_STATUS_e AutoTestUsbConnectionStatus(void)
{
		FILE *						fp;
		INT32						speed = 0;
		AUTO_TEST_USB_CONNECTION_STATUS_e status = AUTO_TEST_USB_NOT_CONNECTED;

		fp = fopen(AUTO_TEST_USB_SPEED_FILE, "r");
		if(fp == NULL)
		{
			EPRINT(WEBJVS_LOG,"fail to open usb speed information file");
			return AUTO_TEST_USB_NOT_CONNECTED;
		}

		fscanf(fp,"%d", &speed);

		if(speed >= 480)
		{
			status = AUTO_TEST_HIGH_SPEED_USB;
		}
		else if(speed < 480 && speed > 0)
		{
			status = AUTO_TEST_LOW_SPEED_USB;
		}
		else if(speed == 0)
		{
			status = AUTO_TEST_USB_NOT_CONNECTED;
		}
		fclose(fp);
		return status;
}
//*****************************************************************************
//	InitAutoSysTest()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system test related parameter
//*****************************************************************************
void InitAutoSysTest(void)
{
	//check system reboot test is going on or not.

	if(SUCCESS != ReadAutoSysTestPara() )
	{
		if(SUCCESS != LoadDfltAutoSysTestPara() )
		{
			EPRINT(CONFIG_LOG,"Failed to intialise Auto system test para");
		}
	}
    AutoSystemTestPara.devType = MAX_ARGO_VARIANT_TYPE;
    UINT8 resultReader1 = GetAutoDetectedReaderType(READER1);
    UINT8 resultReader2 = GetAutoDetectedReaderType(READER2);

    if(resultReader2 == 5)  //SUP_FP_3030_READER
    {
        if(resultReader1 == 1)  //Emprox
            AutoSystemTestPara.devType = ARGO_VARIANT_FOE212;
        if(resultReader1 == 3)  //Mifare
            AutoSystemTestPara.devType = ARGO_VARIANT_FOM212;
        if(resultReader1 == 4)  //HID
            AutoSystemTestPara.devType = ARGO_VARIANT_FOI212;
    }
    else
    {
        if(resultReader1 == 1)  //Emprox
            AutoSystemTestPara.devType = ARGO_VARIANT_CAE200;
        if(resultReader1 == 3)  //Mifare
            AutoSystemTestPara.devType = ARGO_VARIANT_CAM200;
        if(resultReader1 == 4)  //HID
            AutoSystemTestPara.devType = ARGO_VARIANT_CAI200;
    }

}
//*****************************************************************************
//	ReadAutoSysTestPara()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads system test parameter from non volatile memory.
//*****************************************************************************
BOOL ReadAutoSysTestPara(void)
{
	INT32 		fd;
	UINT8		ver;
	BOOL		status = FAIL;
	UINT8 		tmpIdx = 0;

	//	Open System Advance Parameter File
	if( (fd = open(AUTO_SYS_TEST_CONFIG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Auto system test parameter");
		return FAIL;
	}

	// Read version number from the file
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Auto system test version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != AUTO_SYS_TEST_PARA_VER)
	{
		EPRINT(CONFIG_LOG,"Auto system test para version doesn't match");
		close(fd);
		return status;
	}

	if(sizeof(AutoSystemTestPara) != read(fd,&AutoSystemTestPara,sizeof(AutoSystemTestPara) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read  Auto system test para");
		close(fd);
		return FAIL;
	}
	close(fd);


	// Update Functional Test
	for (tmpIdx = INTERNAL_CARD_LF ; tmpIdx < MAX_FUNCTIONAL_TEST; tmpIdx++)
	{
		FunctionalTestStatus[tmpIdx] = AutoSystemTestPara.functionalTestStatus[tmpIdx];
	}

	// Update Auto Test
	for (tmpIdx = AUTO_TEST_DOOR_RELAY ; tmpIdx < MAX_AUTO_TEST; tmpIdx++)
	{
		AutoTestStatus[tmpIdx] = AutoSystemTestPara.autoTestStatus[tmpIdx];
	}

	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltSysTestPara()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default system test parameters.
//*****************************************************************************
BOOL LoadDfltAutoSysTestPara(void)
{
	AutoSystemTestPara = dfltAutoSysTestPara;
	return WriteAutoSysTestPara();
}

//******************************************************************************
//	WriteAutoSysTestPara()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//	Description:
//
//******************************************************************************
BOOL WriteAutoSysTestPara(void)
{
	INT32	fd;
	UINT8	ver;

	if( (fd = open(AUTO_SYS_TEST_CONFIG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Auto system test parameter");
		return FAIL;
	}

	// Write version number
	ver = AUTO_SYS_TEST_PARA_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Auto system test para version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(AutoSystemTestPara) != write(fd,&AutoSystemTestPara,sizeof(AutoSystemTestPara) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write Auto system test para");
		close(fd);
		return FAIL;
	}
	close(fd);
	EPRINT(SYS_LOG,"Auto system test parameter Write Successfully");
	return SUCCESS;
}
//******************************************************************************
//	UpdateAutoSysTestResultForConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//	Description:
//
//******************************************************************************
void UpdateAutoSysTestResultForConfig(void)
{
	UINT8 tmpIdx = 0;


	// Update Functional Test
	for (tmpIdx = INTERNAL_CARD_LF ; tmpIdx < MAX_FUNCTIONAL_TEST; tmpIdx++)
	{
		AutoSystemTestPara.functionalTestStatus[tmpIdx] = FunctionalTestStatus[tmpIdx];
	}

	// Update Auto Test
	for (tmpIdx = AUTO_TEST_DOOR_RELAY ; tmpIdx < MAX_AUTO_TEST; tmpIdx++)
	{
		AutoSystemTestPara.autoTestStatus[tmpIdx] = AutoTestStatus[tmpIdx];
	}
}
//******************************************************************************
//	GenerateAutoTestReport()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//	Description:
//
//******************************************************************************
void GenerateAutoTestReport(void)
{
	INT32		fd = -1;
	UINT16		tmpStrLen = 0;
	char		tmpBuff[2048];

	fd = open(SYS_TEST_REPORT_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd > 0)
	{
		tmpStrLen = sprintf(tmpBuff, "########################### System Test Report################################\r\n");

		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Device Type      : %s\r\n", deviceType[AutoSystemTestPara.devType]);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		tmpStrLen += PrintSubHeaderInFile(&tmpBuff[tmpStrLen], "Basic Detail");

		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Employee Id      : %s\r\n", AutoSystemTestPara.empId);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Device Serial No.: %s\r\n", AutoSystemTestPara.devSrNo);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += snprintf(&tmpBuff[tmpStrLen], (sizeof(tmpBuff) - tmpStrLen), "Software Version : V%dR%d.%d\r\n",SW_VER, SW_REV, SW_MINOR_REV);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Mac Address      : %s\r\n", EthernetParameters.macAddress );
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Date(dd/mm/yyyy) : %02d/%02d/%02d\r\n", CurrTimeDate.tm_mday,
					(CurrTimeDate.tm_mon)+MONTH_OFFSET,CurrTimeDate.tm_year+LINUX_YEAR_OFFSET_1900);

		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		write(fd, (VOIDPTR)tmpBuff, tmpStrLen);

		/******** Functional Test Report ********/
		tmpStrLen = PrintSubHeaderInFile(tmpBuff, "Functional Test");

		/******** Internal Reader Report ********/
		if( (AutoSystemTestPara.devType == ARGO_VARIANT_FOE212 /*FOE-212*/ ) 
			|| (AutoSystemTestPara.devType == ARGO_VARIANT_FOI212 /*FOI-212*/ )
			|| (AutoSystemTestPara.devType == ARGO_VARIANT_CAE200 /*CAE*/ )
			|| (AutoSystemTestPara.devType == ARGO_VARIANT_CAI200 /*CAI*/ )
		)
		{
			tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
			tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Internal LF Card Reader   => %s\r\n",
			autoTestResultStr[AutoSystemTestPara.functionalTestStatus[INTERNAL_CARD_LF]]);
		}

		if( (AutoSystemTestPara.devType == ARGO_VARIANT_FOM212 /*FOM-212*/ ) 
			|| (AutoSystemTestPara.devType == ARGO_VARIANT_FOI212 /*FOI-212*/ ) 
			|| (AutoSystemTestPara.devType == ARGO_VARIANT_CAM200 /*CAM*/ )
			|| (AutoSystemTestPara.devType == ARGO_VARIANT_CAI200 /*CAI*/ )
		)
		{
			tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
			tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Internal HF Card Reader   => %s\r\n",
			autoTestResultStr[AutoSystemTestPara.functionalTestStatus[INTERNAL_CARD_HF]]);
		}

		/******** System Report ********/
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "System Tamper             => %s\r\n",
		autoTestResultStr[AutoSystemTestPara.functionalTestStatus[SYSTEM_TAMPER]]);

		if( (AutoSystemTestPara.devType != ARGO_VARIANT_CAM200 /*CAM*/ )
			&& (AutoSystemTestPara.devType != ARGO_VARIANT_CAI200 /*CAI*/ )
			&& (AutoSystemTestPara.devType != ARGO_VARIANT_CAE200 /*CAE*/ )
		)
		{
			tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
			tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "FP Sensor Serial No.	  => 0x%08x - [%010u]\r\n",
			SfprSensorSerialNumber, SfprSensorSerialNumber);
		
			tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
			tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Finger Sensor             => %s\r\n",
			autoTestResultStr[AutoSystemTestPara.functionalTestStatus[CREATE_USER_AND_ENROLL_FP]]);
		}

		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Buzzer                    => %s\r\n",
		autoTestResultStr[AutoSystemTestPara.functionalTestStatus[BUZZ_LED]]);

		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "LCD                       => %s\r\n",
		autoTestResultStr[AutoSystemTestPara.functionalTestStatus[LCD_TEST]]);
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);

		write(fd, (VOIDPTR)tmpBuff, tmpStrLen);

		/******** Automated Test Report ********/
		tmpStrLen = PrintSubHeaderInFile(tmpBuff, "Automated Test");

		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Door Relay                => %s\r\n", autoTestResultStr[AutoSystemTestPara.autoTestStatus[AUTO_TEST_DOOR_RELAY]] );

		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Door Sense                => %s\r\n", autoTestResultStr[AutoSystemTestPara.autoTestStatus[AUTO_TEST_DOOR_SENCE]] );

		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Exit Switch               => %s\r\n", autoTestResultStr[AutoSystemTestPara.autoTestStatus[AUTO_TEST_EXIT_SWITCH]] );

        tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
        tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "USB                       => %s\r\n", autoTestResultStr[AutoSystemTestPara.autoTestStatus[AUTO_TEST_USB]] );

        tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Memory Card               => %s\r\n", autoTestResultStr[AutoSystemTestPara.autoTestStatus[AUTO_TEST_MEMORY_CARD]] );

		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "RTC Test                  => %s\r\n", autoTestResultStr[AutoSystemTestPara.autoTestStatus[AUTO_TEST_RTC]] );

		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Wi-Fi                  	  => %s\r\n", autoTestResultStr[AutoSystemTestPara.autoTestStatus[AUTO_TEST_WIFI]] );

		//Bluetooth Test
		tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
		tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "Bluetooth                 => %s\r\n", autoTestResultStr[AutoSystemTestPara.autoTestStatus[AUTO_TEST_BLUETOOTH]] );

        /******** External Reader Report ********/
        tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
        tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "External U Card Reader    => %s\r\n",
        autoTestResultStr[AutoSystemTestPara.functionalTestStatus[EXTERNAL_CARD_U]]);

        tmpStrLen += GenerateDesPattern(&tmpBuff[tmpStrLen]);
        tmpStrLen += sprintf(&tmpBuff[tmpStrLen], "External W Card Reader    => %s\r\n",
        autoTestResultStr[AutoSystemTestPara.functionalTestStatus[EXTERNAL_CARD_W]]);
		write(fd, (VOIDPTR)tmpBuff, tmpStrLen);
		close(fd);
	}
}
//*****************************************************************************
//	SetSystemDefault()
//	Param:
//		IN: NONE
//		OUT:NONE.
//	Returns:
//		NONE
//	Description:
//		This function will default the device.
//
//*****************************************************************************
UINT8 SetSystemDefault(UNUSED_PARA FILE *pRXDataFile, UNUSED_PARA FILE *pTXDataFile)
{
	CHAR cmdBuf[128];

	if(WriteDfltSysFlag(SYS_DFLT_ALL) == SUCCESS)
	{
		memset(cmdBuf,0,sizeof(cmdBuf));
		sprintf(cmdBuf,"touch %s",DEVICE_IP_SIGN_FILE);
		if(system(cmdBuf) != 0)
		{
			EPRINT(SYS_LOG,"Failed To Create %s",DEVICE_IP_SIGN_FILE);
		}

		memset(cmdBuf,0,sizeof(cmdBuf));
		sprintf(cmdBuf,"touch %s",SERVER_IP_SIGN_FILE);
		if(system(cmdBuf) != 0)
		{
			EPRINT(SYS_LOG,"Failed To Create %s",SERVER_IP_SIGN_FILE);
		}
		SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
	}
	return SUCCESS;
}
//*****************************************************************************
//	SendTestReport()
//	Param:
//		IN: NONE
//		OUT:NONE
//	Returns:
//		NONE
//	Description:
//		This function Generate test report.
//*****************************************************************************
void SendAutoTestReport(void)
{
	char		tmpBuff[200];

    sprintf(tmpBuff,"su - se -c \"curl -T %s ftp://%s:%s@%s/%s_%s.pdf\"",SYS_TEST_REPORT_FILE_PDF,AutoSystemTestPara.ftpUsrName,AutoSystemTestPara.ftpPwd,AutoSystemTestPara.ftpIpaddr,DeviceVariantType[AutoSystemTestPara.devType],AutoSystemTestPara.devSrNo);
	system(tmpBuff);
}
//*****************************************************************************
//	SysTestSendReportToFtp()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//*****************************************************************************
UINT8 SysAutoTestSendReportToFtp(UNUSED_PARA FILE *pRXDataFile, UNUSED_PARA FILE *pTXDataFile)
{
	SendAutoTestReport();
	return SUCCESS;
}

//*****************************************************************************
//	DisplayScreenToStartSystemTest()
//	Param:
//		IN : dummy
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//			This Function will display screen "Do you want to start system test?"
//
//*****************************************************************************
void DisplayScreenToStartSystemTest(UNUSED_PARA UINT32 dummy)
{
	SendMessageToQt("",GUI_MSG_START_AUTO_SYSTEM_TEST_SCREEN);
	StartSystemTestScreenWaitTimer(SYSTEST_DISPLAY_SCREEN_TIME_OUT);
}

UINT8 GetAutoTestParaDevType()
{
	return AutoSystemTestPara.devType;
}
