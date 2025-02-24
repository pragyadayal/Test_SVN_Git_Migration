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
//   Created By   : Meetkumar Bariya
//   File         : MxWebJvsSysTest.c
//   Description  :
//
/////////////////////////////////////////////////////////////////////////////
// macro; to make use of recursive mutex
#define _GNU_SOURCE
//********Place all include files ****
//system includes
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

//Matrix include files
#include "MxWebJeeves.h"
#include "MxTypedef.h"
#include "MxWebJvsSysTest.h"
#include "MxGpio.h"
#include "MxSysCommonDef.h"
#include "MxConfig.h"
#include "MxLogs.h"
#include "MxSysUtils.h"
#include "MxWifiPort.h"
#include "MxMobileBroadband.h"
#include "MxConfigFileIo.h"
#include "MxDcTask.h"
#include "MxReaderTask.h"
#include "MxSysTest.h"
#include "MxEventLog.h"
#include "MxConfigDef.h"
#include "MxDst.h"
#include "MxNwUtils.h"
#include "MxConfigUpdateHandler.h"
#include "MxSysTime.h"
#include "MxNetworkUtils.h"
#ifdef UPNP_SUPPORTED
#include "MxUPNPSupport.h"
#endif //#ifdef UPNP_SUPPORTED

//******** Extern Variables **********
extern SYSTEM_TEST_PARA_t			SystemTestPara;
extern ETHERNET_PARA_t				EthernetParameters;
extern SYS_BASIC_PARA_t				SysBasicPara;
extern MMC_INFO_t 					MmcInfo;
extern DOOR_ACTIVITY_INFO_t			DcInfo;				//Door Controller FSM info
extern READER_STATUS_e 				readerStatus[MAX_READERS];
extern SYSTEM_TEST_STATUS_e			TestStatusF;
extern const CHARPTR				wifiSignalQualityStr[MAX_WIFI_SIGNAL_QUALITY];
extern const CHARPTR				wifiNwSecureStr[2];
extern MMC_INFO_t 					MmcInfo;
extern COMM_PORT_e					acmsCurActiveInterface;
extern SYSTEM_REBOOT_TEST_PARA_t	SystemRebootTestPata;
extern THREAD_STATUS_e				text2PdfConverterThreadStatus ;
extern UINT32						SfprSensorSerialNumber;

//******** Defines and Data Types ****
#define USB_SPEED_FILE				"/sys/bus/usb/devices/2-1/speed"
#define MASKED_FTP_PASSWORD		"..............."

#define GEN_SYS_TEST_BASIC_INFO_ENUM(SYS_TEST_BASIC_INFO_ENUM, SYS_TEST_BASIC_INFO_NAME) SYS_TEST_BASIC_INFO_ENUM,
#define GEN_SYS_TEST_BASIC_INFO_NAME(SYS_TEST_BASIC_INFO_ENUM, SYS_TEST_BASIC_INFO_NAME) SYS_TEST_BASIC_INFO_NAME,

#define GEN_SYS_TEST_HW_TEST_ENUM(SYS_TEST_HW_TEST_ENUM, SYS_TEST_HW_TEST_NAME) SYS_TEST_HW_TEST_ENUM,
#define GEN_SYS_TEST_HW_TEST_NAME(SYS_TEST_HW_TEST_ENUM, SYS_TEST_HW_TEST_NAME) SYS_TEST_HW_TEST_NAME,

#define GEN_SYS_TEST_READER_TEST_ENUM(SYS_TEST_READER_TEST_ENUM, SYS_TEST_READER_TEST_NAME) SYS_TEST_READER_TEST_ENUM,
#define GEN_SYS_TEST_READER_TEST_NAME(SYS_TEST_READER_TEST_ENUM, SYS_TEST_READER_TEST_NAME) SYS_TEST_READER_TEST_NAME,

#define GEN_SYS_TEST_GENERIC_IO_TEST_ENUM(SYS_TEST_GENERIC_IO_TEST_ENUM, SYS_TEST_GENERIC_IO_TEST_NAME) SYS_TEST_GENERIC_IO_TEST_ENUM,
#define GEN_SYS_TEST_GENERIC_IO_TEST_NAME(SYS_TEST_GENERIC_IO_TEST_ENUM, SYS_TEST_GENERIC_IO_TEST_NAME) SYS_TEST_GENERIC_IO_TEST_NAME,


#define GEN_SYS_TEST_OTHER_TEST_ENUM(SYS_TEST_OTHER_TEST_ENUM, SYS_TEST_OTHER_TEST_NAME) SYS_TEST_OTHER_TEST_ENUM,
#define GEN_SYS_TEST_OTHER_TEST_NAME(SYS_TEST_OTHER_TEST_ENUM, SYS_TEST_OTHER_TEST_NAME) SYS_TEST_OTHER_TEST_NAME,

#define GEN_SYS_TEST_REPORT_ENUM(SYS_TEST_REPORT_ENUM, SYS_TEST_REPORT_NAME) SYS_TEST_REPORT_ENUM,
#define GEN_SYS_TEST_REPORT_NAME(SYS_TEST_REPORT_ENUM, SYS_TEST_REPORT_NAME) SYS_TEST_REPORT_NAME,

#define SYS_TEST_BASIC_INFO(API)\
			API(SYS_TEST_BASIC_INFO_EMP_NAME,			"employeeName")\
			API(SYS_TEST_BASIC_INFO_EMP_ID,				"employeeID")\
			API(SYS_TEST_BASIC_INFO_DEVICE_SERIAL_NO,	"deviceSerialNo")\
			API(SYS_TEST_BASIC_INFO_MAIN_PCB_DESIGN,	"mainPCBHWDesine")\
			API(SYS_TEST_BASIC_INFO_CONN_HW_DESIGN,		"connectorHWDesign")\
			API(SYS_TEST_BASIC_INFO_MAC_ADDR,			"macAddress")\
			API(SYS_TEST_BASIC_INFO_DATE,				"date")\
			API(SYS_TEST_BASIC_INFO_FTP_IP_ADDR,		"ftpIPAddress")\
			API(SYS_TEST_BASIC_INFO_FTP_USER_NAME,		"ftpUserName")\
			API(SYS_TEST_BASIC_INFO_MAIN_PCB_SW_VER,	"mainPCBSWVersion")\
			API(SYS_TEST_BASIC_INFO_RESULT,				"result")\
			API(SYS_TEST_BASIC_INFO_FTP_PASSWORD,		"ftpPassword")\
			API(SYS_TEST_BASIC_VARIANT_WITH_INTER_FP, 	"isVariantWithInternalFpRdr")\

#define SYS_TEST_HW_TEST(API)\
			API(SYS_TEST_HW_TEST_ASSEMBLER_NAME,	"assemblerName")\
			API(SYS_TEST_HW_TEST_ASSEMBLER_ID,		"assemblerID")\
			API(SYS_TEST_HW_TEST_VEGA_VOLTAGE0,		"vegaVoltage0")\
			API(SYS_TEST_HW_TEST_VEGA_VOLTAGE1,		"vegaVoltage1")\
			API(SYS_TEST_HW_TEST_VEGA_VOLTAGE2,		"vegaVoltage2")\
			API(SYS_TEST_HW_TEST_VEGA_VOLTAGE3,		"vegaVoltage3")\
			API(SYS_TEST_HW_TEST_VEGA_VOLTAGE4,		"vegaVoltage4")\
			API(SYS_TEST_HW_TEST_VEGA_VOLTAGE5,		"vegaVoltage5")\
			API(SYS_TEST_HW_TEST_VEGA_VOLTAGE6,		"vegaVoltage6")\
			API(SYS_TEST_HW_TEST_FREQUENCY,			"frequency")\

#define SYS_TEST_READER_TEST(API)\
			API(SYS_TEST_READER_TEST_INT_CARD_READER,			"intCardReader")\
			API(SYS_TEST_READER_TEST_INT_FINGER_READER,			"intFingerReader")\
			API(SYS_TEST_READER_TEST_EXTERNEL_READER,			"extReader")\
			API(SYS_TEST_READER_TEST_INT_CARD_RDR_STATUS,		"intCardReaderStatus")\
			API(SYS_TEST_READER_TEST_INT_FNGR_RDR_STATUS,		"intFingerReaderStatus")\
			API(SYS_TEST_READER_TEST_INT_FNGR_RDR_SERIAL_NO,	"fpReaderSrNo")\
			API(SYS_TEST_READER_TEST_EXT_RDR_STATUS_1,			"extReaderStatus1")\
			API(SYS_TEST_READER_TEST_EXT_RDR_STATUS_2,			"extReaderStatus2")\
			API(SYS_TEST_READER_TEST_LAST_DETECTED_CARD_READER,	"lastDetectedCardReader")\
			API(SYS_TEST_READER_TEST_CARD_ID,					"cardID")\
			API(SYS_TEST_READER_TEST_INT_EM_PROX_RDR_RESULT,	"intEMProxRdrResult")\
			API(SYS_TEST_READER_TEST_INT_HID_PROX_RDR_RESULT,	"intHIDProxRdrResult")\
			API(SYS_TEST_READER_TEST_INT_MIFARE_RDR_RESULT,		"intMiFareRdrResult")\
			API(SYS_TEST_READER_TEST_HID_ICLASS_U_RDR_RESULT,	"intHIDiClassURdrResult")\
			API(SYS_TEST_READER_TEST_VEGA_EXT_U_RDR_RESULT,		"vegaExternelURdrResult")\
			API(SYS_TEST_READER_TEST_VEGA_EXT_W_RDR_RESULT,		"vegaExternelWRdrResult")\

#define SYS_TEST_GENERIC_IO_TEST(API)\
			API(TEST_TYPE_BUZ_LED,						"buzzerTest")\
			API(SYS_TEST_GENERIC_IO_TEST_LCD_TEST,		"lcdTest")\
			API(TEST_TYPE_AUX_RELAY,					"auxRelayTest")\
			API(TEST_TYPE_DOOR_RELAY,					"doorRelayTest")\
			API(TEST_TYPE_TOUCH_PANEL,					"touchPanelTest")\
			API(TEST_TYPE_AUX_IN,						"auxInTest")\
			API(TEST_TYPE_DOOR_SENSE,					"doorSenseTest")\
			API(TEST_TYPE_DOOR_TMPR,					"sysTampTest")\
			API(TEST_TYPE_EXT_TMPR,						"exitRdrTampTest")\
			API(TEST_TYPE_LOCK_TMPR,					"lookTampTest")\
			API(TEST_TYPE_EXT_SW,						"exitSWTest")\
			API(TEST_TYPE_USB,							"usb")\
			API(TEST_TYPE_MMC_CARD_TEST,				"mmcCardTest")\
			API(TEST_TYPE_RTC,							"RTCTest")\
			API(SYS_TEST_GENERIC_IO_BUZ_LED,			"buzzerTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_AUX_RELAY,		"auxRelayTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_DOOR_RELAY,	"doorRelayTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_AUX_IN,		"auxInTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_TOUCH_PANEL,	"touchPanelTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_DOOR_SENSE,	"doorSenseTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_SYS_TMPR,		"sysTampTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_EXT_TMPR,		"exitRdrTampTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_LOCK_TMPR,		"lookTampTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_EXT_SWITCH,	"exitSWTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_USB,			"usbTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_MMC_CARD_TEST,	"mmcCardTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_RTC_TEST,		"RTCTestRes")\
			API(SYS_TEST_GENERIC_IO_TEST_LCD,			"lcdTestRes")\

#define SYS_TEST_OTHER_TEST(API)\
			API(SYS_TEST_OTHER_TEST_USER_ENROLL_TEST,			"userEnrollTest")\
			API(SYS_TEST_OTHER_TEST_SYSTEM_DEFAULT_TEST,		"sysDefaultTest")\
			API(SYS_TEST_OTHER_TEST_SYSTEM_REBOOT_TEST,			"sysRebootTest")\
			API(SYS_TEST_OTHER_TEST_SYSTEM_REBOOT_COUNT,		"sysRebootCount")\
			API(SYS_TEST_OTHER_TEST_USER_ENROLL_RESULT,			"userEnrollTestRes")\
			API(SYS_TEST_OTHER_TEST_SYSTEM_DEFAULT_RESULT,		"sysDefaultTestRes")\
			API(SYS_TEST_OTHER_TEST_WIFI_TEST_RESULT,			"wifiTestRes")\
			API(SYS_TEST_OTHER_TEST_BLUETOOTH_TEST_RESULT,		"bluetoothTestRes")\

typedef enum
{
	SYS_TEST_BASIC_INFO(GEN_SYS_TEST_BASIC_INFO_ENUM)
	MAX_TOTAL_BASIC_INFO,
}SYS_TEST_BASIC_INFO_e;

typedef enum
{
	SYS_TEST_HW_TEST(GEN_SYS_TEST_HW_TEST_ENUM)
	MAX_TOTAL_HW_TEST,
}SYS_TEST_HW_TEST_e;

typedef enum
{
	SYS_TEST_READER_TEST(GEN_SYS_TEST_READER_TEST_ENUM)
	MAX_TOTAL_READER_TEST,
}SYS_TEST_READER_TEST_e;

typedef enum
{
	SYS_TEST_GENERIC_IO_TEST(GEN_SYS_TEST_GENERIC_IO_TEST_ENUM)
	MAX_TOTAL_GENERIC_IO_TEST,
}SYS_TEST_GENERIC_IO_e;

typedef enum
{
	SYS_TEST_OTHER_TEST(GEN_SYS_TEST_OTHER_TEST_ENUM)
	MAX_TOTAL_OTHER_TEST,
}SYS_TEST_OTHER_TEST_e;

typedef enum
{
	USB_NOT_CONNECTED,
	LOW_SPEED_USB,
	HIGH_SPEED_USB,
}USB_CONNECTION_STATUS_e;

typedef enum
{
	FRONT_USB_PORT,
	BACK_USB_PORT,
	MAX_USB_PORT,
}USB_PORT_e;
//******** Function Prototypes *******

//******** Global Variables **********

//******** Static Variables **********
static CHARPTR sysTestBasicInfo [MAX_TOTAL_BASIC_INFO]=
{
		SYS_TEST_BASIC_INFO(GEN_SYS_TEST_BASIC_INFO_NAME)
};

static CHARPTR sysTestHWTest [MAX_TOTAL_HW_TEST]=
{
		SYS_TEST_HW_TEST(GEN_SYS_TEST_HW_TEST_NAME)
};

static CHARPTR sysTestReaderTest [MAX_TOTAL_READER_TEST]=
{
		SYS_TEST_READER_TEST(GEN_SYS_TEST_READER_TEST_NAME)
};

static CHARPTR sysTestGenericIO [MAX_TOTAL_GENERIC_IO_TEST]=
{
		SYS_TEST_GENERIC_IO_TEST(GEN_SYS_TEST_GENERIC_IO_TEST_NAME)
};

static CHARPTR sysTestOtherTest [MAX_TOTAL_OTHER_TEST]=
{
		SYS_TEST_OTHER_TEST(GEN_SYS_TEST_OTHER_TEST_NAME)
};

static USB_CONNECTION_STATUS_e UsbConnectionStatus(void);
static VOIDPTR RtcTestThread(VOIDPTR dummy);
static THREAD_STATUS_e		rtcTestThreadStatus = THREAD_INACTIVE;
static pthread_t			rtcTestThreadData = 0;
static void SaveDataAndGenerateTxData(FILE	*pTXDataFile);
//******** Function Definitions ******

//////////////////////////////////////////////////////////////////////////////
/************			SYSTEM TEST -> BASIC INFORMATION      ***************/
//////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//	SetSysTestBasicInfo()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 SetSysTestBasicInfo(FILE *pRXDataFile, FILE *pTXDataFile)
{
	CHAR 	tempFieldName[MAX_WEB_JVS_FIELD_NAME_LENGTH], tempFieldValue[MAX_WEB_JVS_FIELD_VALUE_LENGTH];
	UINT8	parseStatus, getElementStatus, elementIdx;
	while( (parseStatus = ParseQueryField(pRXDataFile, &tempFieldName[0], MAX_WEB_JVS_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_WEB_JVS_FIELD_VALUE_LENGTH) ) != FAIL)
	{
		DPRINT(WEBJVS_LOG,"%s=%s", tempFieldName, tempFieldValue);
		if( (getElementStatus = GetWebPageElementIdx(&sysTestBasicInfo[0], MAX_TOTAL_BASIC_INFO, tempFieldName, &elementIdx)) == SUCCESS)
		{
			switch(elementIdx)
			{
				default:
					break;

				case SYS_TEST_BASIC_INFO_EMP_NAME:
				{
					snprintf(SystemTestPara.empName, EMP_NAME_SIZE, "%s",tempFieldValue);
				}break;

				case SYS_TEST_BASIC_INFO_EMP_ID:
				{
					snprintf(SystemTestPara.empId, EMP_ID_SIZE, "%s",tempFieldValue);
				}break;

				case SYS_TEST_BASIC_INFO_DEVICE_SERIAL_NO:
				{
					snprintf(SystemTestPara.devSrNo, DEVICE_SR_NO_SIZE, "%s",tempFieldValue);
				}break;

				case SYS_TEST_BASIC_INFO_MAIN_PCB_DESIGN:
				{
					snprintf(SystemTestPara.mainPcbDesign, HWD_DESIN_NO_SIZE, "%s",tempFieldValue);
				}break;

				case SYS_TEST_BASIC_INFO_CONN_HW_DESIGN:
				{
					snprintf(SystemTestPara.connPcbDesign, HWD_DESIN_NO_SIZE, "%s",tempFieldValue);
				}break;

				case SYS_TEST_BASIC_INFO_DATE:
				{
					snprintf(SystemTestPara.testDate, DATE_STR_SIZE, "%s",tempFieldValue);
				}break;

				case SYS_TEST_BASIC_INFO_FTP_IP_ADDR:
				{
					snprintf(SystemTestPara.ftpIpaddr, MAX_IP_ADDRESS, "%s",tempFieldValue);
				}break;

				case SYS_TEST_BASIC_INFO_FTP_USER_NAME:
				{
					snprintf(SystemTestPara.ftpUsrName, FTP_USR_NAME_SIZE, "%s",tempFieldValue);
				}break;

				case SYS_TEST_BASIC_INFO_FTP_PASSWORD:
				{
					if(strncmp(tempFieldValue, MASKED_FTP_PASSWORD, sizeof(MASKED_FTP_PASSWORD)) != 0)
						snprintf(SystemTestPara.ftpPwd, FTP_PWD_SIZE, "%s",tempFieldValue);
				}break;

				case SYS_TEST_BASIC_INFO_RESULT:
				{
					SystemTestPara.macAddrResult	=	(BOOL)atoi(tempFieldValue);
				}break;
			}
		}
		if( (parseStatus == WEB_JVS_LAST_FIELD) || (parseStatus == WEB_JVS_LAST_FIELD_WITHOUT_DATA) )
		{
			WriteSysTestPara();

			#ifdef UPNP_SUPPORTED
			UpnpMx_UpdateService();
			#endif //#ifdef UPNP_SUPPORTED
			

			DPRINT(WEBJVS_LOG," System_Test_Basic_Information Data set successfully");
			//Write response data
			WriteWebResponse(pTXDataFile, "0");
			WriteJSONStartTag(pTXDataFile);	//JSON Data Start
			FillSysTestBasicInfo(pTXDataFile);
			WriteJSONENDTag(pTXDataFile);	//JSON Data END
			break;
		}
	}
	return SUCCESS;
}

//*****************************************************************************
//	GetSysTestBasicInfo()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 GetSysTestBasicInfo(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	//Write response code
	WriteWebResponse(pTXDataFile, "0");
	//Write response data
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	FillSysTestBasicInfo(pTXDataFile);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END
	return SUCCESS;
}

//*****************************************************************************
//	FillSysTestBasicInfo()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 FillSysTestBasicInfo(FILE* pTXDataFile)
{
	CHAR valueStr[MAX_WEB_JVS_FIELD_VALUE_LENGTH];

	WriteJSONStringData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_INFO_EMP_NAME], SystemTestPara.empName);
	WriteJSONStringData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_INFO_EMP_ID], SystemTestPara.empId);
	WriteJSONStringData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_INFO_DEVICE_SERIAL_NO], SystemTestPara.devSrNo);
	WriteJSONStringData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_INFO_MAIN_PCB_DESIGN], SystemTestPara.mainPcbDesign);
	WriteJSONStringData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_INFO_CONN_HW_DESIGN], SystemTestPara.connPcbDesign);
	WriteJSONStringData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_INFO_MAC_ADDR], EthernetParameters.macAddress);
	WriteJSONStringData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_INFO_DATE], SystemTestPara.testDate);
	WriteJSONStringData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_INFO_FTP_IP_ADDR], SystemTestPara.ftpIpaddr);
	WriteJSONStringData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_INFO_FTP_USER_NAME], SystemTestPara.ftpUsrName);

	snprintf(valueStr, sizeof(valueStr),"V%dR%d.%d",SW_VER, SW_REV, SW_MINOR_REV);
	WriteJSONStringData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_INFO_MAIN_PCB_SW_VER], valueStr);
	WriteJSONNumericData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_INFO_RESULT], SystemTestPara.macAddrResult);
	if(strnlen(SystemTestPara.ftpPwd, sizeof(MASKED_FTP_PASSWORD)) > 0)
		snprintf(valueStr, sizeof(MASKED_FTP_PASSWORD), "%s", MASKED_FTP_PASSWORD);
	else
		valueStr[0] = '\0';
	WriteJSONStringData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_INFO_FTP_PASSWORD], valueStr);
	
	WriteJSONNumericData(pTXDataFile, sysTestBasicInfo[SYS_TEST_BASIC_VARIANT_WITH_INTER_FP], IsVariant_With_Internal_FP_Rdr());

	return SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
/************			SYSTEM TEST -> HARDWARE TEST          ***************/
//////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//	SetSysTestHWTest()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 SetSysTestHWTest(FILE *pRXDataFile, FILE *pTXDataFile)
{
	CHAR 	tempFieldName[MAX_WEB_JVS_FIELD_NAME_LENGTH], tempFieldValue[MAX_WEB_JVS_FIELD_VALUE_LENGTH];
	UINT8	parseStatus, getElementStatus, elementIdx;

	while( (parseStatus = ParseQueryField(pRXDataFile, &tempFieldName[0], MAX_WEB_JVS_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_WEB_JVS_FIELD_VALUE_LENGTH) ) != FAIL)
	{
		DPRINT(WEBJVS_LOG,"%s=%s", tempFieldName, tempFieldValue);
		DPRINT(WEBJVS_LOG,"total arg = %d",MAX_TOTAL_HW_TEST);
		if( (getElementStatus = GetWebPageElementIdx(&sysTestHWTest[0], MAX_TOTAL_HW_TEST, tempFieldName, &elementIdx)) == SUCCESS)
		{
			DPRINT(WEBJVS_LOG,"element index = %d",elementIdx);
			switch(elementIdx)
			{
				default:
					break;

				case SYS_TEST_HW_TEST_ASSEMBLER_NAME:
				{
					snprintf(SystemTestPara.asmblerName, EMP_NAME_SIZE, "%s",tempFieldValue);
				}break;

				case SYS_TEST_HW_TEST_ASSEMBLER_ID:
				{
					snprintf(SystemTestPara.asmblerId, EMP_ID_SIZE, "%s",tempFieldValue);
				}break;

				case SYS_TEST_HW_TEST_VEGA_VOLTAGE0:
				{
					SystemTestPara.voltResult[VOLTAGE_1_1V]	=	(BOOL)atoi(tempFieldValue);
				}break;

				case SYS_TEST_HW_TEST_VEGA_VOLTAGE1:
				{
					SystemTestPara.voltResult[VOLTAGE_1_26V]	=	(BOOL)atoi(tempFieldValue);
				}break;

				case SYS_TEST_HW_TEST_VEGA_VOLTAGE2:
				{
					SystemTestPara.voltResult[VOLTAGE_1_5V]	=	(BOOL)atoi(tempFieldValue);
				}break;

				case SYS_TEST_HW_TEST_VEGA_VOLTAGE3:
				{
					SystemTestPara.voltResult[VOLTAGE_1_8V]	=	(BOOL)atoi(tempFieldValue);
				}break;

				case SYS_TEST_HW_TEST_VEGA_VOLTAGE4:
				{
					SystemTestPara.voltResult[VOLTAGE_3_3V]	=	(BOOL)atoi(tempFieldValue);
				}break;

				case SYS_TEST_HW_TEST_VEGA_VOLTAGE5:
				{
					SystemTestPara.voltResult[VOLTAGE_5_0V]	=	(BOOL)atoi(tempFieldValue);
				}break;

				case SYS_TEST_HW_TEST_VEGA_VOLTAGE6:
				{
					SystemTestPara.voltResult[VOLTAGE_12_0V]	=	(BOOL)atoi(tempFieldValue);
				}break;

				case SYS_TEST_HW_TEST_FREQUENCY:
				{
					SystemTestPara.rtcFreqResult = (BOOL)atoi(tempFieldValue);
				}break;
			}
		}
		if( (parseStatus == WEB_JVS_LAST_FIELD) || (parseStatus == WEB_JVS_LAST_FIELD_WITHOUT_DATA) )
		{
			WriteSysTestPara();

			DPRINT(WEBJVS_LOG," System_Test_Hardware_Test_Information Data set successfully");
			//Write response data
			WriteWebResponse(pTXDataFile, "0");
			WriteJSONStartTag(pTXDataFile);	//JSON Data Start
			FillSysTestHWTest(pTXDataFile);
			WriteJSONENDTag(pTXDataFile);	//JSON Data END
			break;
		}
	}
	return SUCCESS;
}

//*****************************************************************************
//	GetSysTestHWTest()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 GetSysTestHWTest(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	//Write response code
	WriteWebResponse(pTXDataFile, "0");
	//Write response data
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	FillSysTestHWTest(pTXDataFile);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END
	return SUCCESS;
}

//*****************************************************************************
//	FillSysTestHWTest()
//	Param:
//		IN:ConvertRdrType(READER1,SysBasicPara.reader[READER1].type,SYS_TO_WE
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 FillSysTestHWTest(FILE* pTXDataFile)
{
	WriteJSONStringData(pTXDataFile, sysTestHWTest[SYS_TEST_HW_TEST_ASSEMBLER_NAME], SystemTestPara.asmblerName);
	WriteJSONStringData(pTXDataFile, sysTestHWTest[SYS_TEST_HW_TEST_ASSEMBLER_ID], SystemTestPara.asmblerId);

	WriteJSONNumericData(pTXDataFile, sysTestHWTest[SYS_TEST_HW_TEST_VEGA_VOLTAGE0], SystemTestPara.voltResult[VOLTAGE_1_1V]);
	WriteJSONNumericData(pTXDataFile, sysTestHWTest[SYS_TEST_HW_TEST_VEGA_VOLTAGE1], SystemTestPara.voltResult[VOLTAGE_1_26V]);
	WriteJSONNumericData(pTXDataFile, sysTestHWTest[SYS_TEST_HW_TEST_VEGA_VOLTAGE2], SystemTestPara.voltResult[VOLTAGE_1_5V]);
	WriteJSONNumericData(pTXDataFile, sysTestHWTest[SYS_TEST_HW_TEST_VEGA_VOLTAGE3], SystemTestPara.voltResult[VOLTAGE_1_8V]);
	WriteJSONNumericData(pTXDataFile, sysTestHWTest[SYS_TEST_HW_TEST_VEGA_VOLTAGE4], SystemTestPara.voltResult[VOLTAGE_3_3V]);
	WriteJSONNumericData(pTXDataFile, sysTestHWTest[SYS_TEST_HW_TEST_VEGA_VOLTAGE5], SystemTestPara.voltResult[VOLTAGE_5_0V]);
	WriteJSONNumericData(pTXDataFile, sysTestHWTest[SYS_TEST_HW_TEST_VEGA_VOLTAGE6], SystemTestPara.voltResult[VOLTAGE_12_0V]);

	WriteJSONNumericData(pTXDataFile, sysTestHWTest[SYS_TEST_HW_TEST_FREQUENCY], SystemTestPara.rtcFreqResult);
	return SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
/************			SYSTEM TEST -> READER TEST          ***************/
//////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//	SetSysTestReaderTest()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 SetSysTestReaderTest(FILE *pRXDataFile, FILE *pTXDataFile)
{
	CHAR 	tempFieldName[MAX_WEB_JVS_FIELD_NAME_LENGTH], tempFieldValue[MAX_WEB_JVS_FIELD_VALUE_LENGTH];
	UINT8	parseStatus, getElementStatus, elementIdx;
	SYSTEM_TEST_PARA_t	tempSysTestPara;
	SYS_BASIC_PARA_t	tempSysBasicPara;
	READER_TYPE_e		tempRdrType = NO_READER_PRESENT;

	tempSysBasicPara	=	SysBasicPara;
	tempSysTestPara		=	SystemTestPara;

	GetSysBasicConfigWorkingCopy(&tempSysBasicPara);

	while( (parseStatus = ParseQueryField(pRXDataFile, &tempFieldName[0], MAX_WEB_JVS_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_WEB_JVS_FIELD_VALUE_LENGTH) ) != FAIL)
	{
		DPRINT(WEBJVS_LOG,"%s=%s", tempFieldName, tempFieldValue);
		if( (getElementStatus = GetWebPageElementIdx(&sysTestReaderTest[0], MAX_TOTAL_READER_TEST, tempFieldName, &elementIdx)) == SUCCESS)
		{
			switch(elementIdx)
			{
				default:
					break;

				case SYS_TEST_READER_TEST_INT_CARD_READER:
				{
					tempSysBasicPara.reader[READER1].type = (READER_TYPE_e)atoi(tempFieldValue);
				}break;

				case SYS_TEST_READER_TEST_INT_FINGER_READER:
				{
					tempRdrType = (READER_TYPE_e)atoi(tempFieldValue);
					if((SUP_FP_3030_READER == tempRdrType) && (NO == IsVariant_With_Internal_FP_Rdr()))//Not applicable for ARGO CAM/CAI
					{
						DPRINT(WEBJVS_LOG,"Finger Reader not applicable for ARGO CAI/CAM Variant");
					}
					else
					{
						tempSysBasicPara.reader[READER2].type = tempRdrType;
					}
				}break;

				case SYS_TEST_READER_TEST_EXTERNEL_READER:
				{
					tempSysBasicPara.reader[READER3].type = (READER_TYPE_e)atoi(tempFieldValue);
				}break;

				case SYS_TEST_READER_TEST_INT_EM_PROX_RDR_RESULT:
				{
					tempSysTestPara.cardRdrResult[SYS_TEST_INT_EM_PROX] = (BOOL)atoi(tempFieldValue);
				}break;

				case SYS_TEST_READER_TEST_INT_HID_PROX_RDR_RESULT:
				{
					tempSysTestPara.cardRdrResult[SYS_TEST_INT_HID_PROX] = (BOOL)atoi(tempFieldValue);
				}break;

				case SYS_TEST_READER_TEST_INT_MIFARE_RDR_RESULT:
				{
					tempSysTestPara.cardRdrResult[SYS_TEST_INT_MIFARE] = (BOOL)atoi(tempFieldValue);
				}break;

				case SYS_TEST_READER_TEST_HID_ICLASS_U_RDR_RESULT:
				{
					tempSysTestPara.cardRdrResult[SYS_TEST_INT_HID_ICLASS] = (BOOL)atoi(tempFieldValue);
				}break;

				case SYS_TEST_READER_TEST_VEGA_EXT_U_RDR_RESULT:
				{
					tempSysTestPara.cardRdrResult[SYS_TEST_EXT_HID_ICLASS_U] = (BOOL)atoi(tempFieldValue);
				}break;

				case SYS_TEST_READER_TEST_VEGA_EXT_W_RDR_RESULT:
				{
					tempSysTestPara.cardRdrResult[SYS_TEST_EXT_HID_ICLASS_W] = (BOOL)atoi(tempFieldValue);
				}break;
			}
		}

		if( (parseStatus == WEB_JVS_LAST_FIELD) || (parseStatus == WEB_JVS_LAST_FIELD_WITHOUT_DATA) )
		{
			SystemTestPara		=	tempSysTestPara;
			WriteSysTestPara();
			ConfigUpdateHandler(GEN_SYS_BASIC_TABLE_ID, 0, (VOIDPTR)(&tempSysBasicPara) );

			DPRINT(WEBJVS_LOG," System_Test_Reader_Test Data set successfully");
			//Write response data
			WriteWebResponse(pTXDataFile, "0");
			WriteJSONStartTag(pTXDataFile);	//JSON Data Start
			FillSysTestReaderTest(pTXDataFile);
			WriteJSONENDTag(pTXDataFile);	//JSON Data END
			break;
		}
	}
	return SUCCESS;
}

//*****************************************************************************
//	GetSysTestReaderTest()
//	Param:READER
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 GetSysTestReaderTest(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	//Write response code
	WriteWebResponse(pTXDataFile, "0");
	//Write response data
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	FillSysTestReaderTest(pTXDataFile);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END
	return SUCCESS;
}

//*****************************************************************************
//	FillSysTestReaderTest()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 FillSysTestReaderTest(FILE* pTXDataFile)
{
	CHAR	valueStr[MAX_WEB_JVS_FIELD_VALUE_LENGTH];

	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_INT_CARD_READER], SysBasicPara.reader[READER1].type);
	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_INT_FINGER_READER], SysBasicPara.reader[READER2].type);

	///////   	Check If Combo Exit Reader is present or not   //////

	if(SysBasicPara.cerPresentF == TRUE)
	{
		WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_EXTERNEL_READER], (UINT8)ConvertRdrType(READER3, COMBO_EXIT_READER, SYS_TO_WEB));
	}
	else
	{
		WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_EXTERNEL_READER], SysBasicPara.reader[READER3].type);
	}
	///////   	Status of internal and external Readers  		//////

	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_INT_CARD_RDR_STATUS], GetReaderStatus(READER1));
	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_INT_FNGR_RDR_STATUS], GetReaderStatus(READER2));
	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_EXT_RDR_STATUS_1],	   GetReaderStatus(READER3));
	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_EXT_RDR_STATUS_2],    GetReaderStatus(READER4));

	sprintf(valueStr,"0x%08X",SfprSensorSerialNumber);				//FP reader serial number
	WriteJSONStringData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_INT_FNGR_RDR_SERIAL_NO],    valueStr);

	///////  		Last detected Card and Card ID            ///////

	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_LAST_DETECTED_CARD_READER],DcInfo.lastDetectedCard.readerId);
	sprintf(valueStr,"%llu",DcInfo.lastDetectedCard.cardId);
	WriteJSONStringData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_CARD_ID],    valueStr);

	SetCardIdInTestConfig(DcInfo.lastDetectedCard.readerId, DcInfo.lastDetectedCard.cardId);

	///////	      Reader Result Information 				 ///////

	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_INT_EM_PROX_RDR_RESULT], 	SystemTestPara.cardRdrResult[SYS_TEST_INT_EM_PROX]);
	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_INT_HID_PROX_RDR_RESULT], 	SystemTestPara.cardRdrResult[SYS_TEST_INT_HID_PROX]);
	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_INT_MIFARE_RDR_RESULT], 	SystemTestPara.cardRdrResult[SYS_TEST_INT_MIFARE]);
	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_HID_ICLASS_U_RDR_RESULT], 	SystemTestPara.cardRdrResult[SYS_TEST_INT_HID_ICLASS]);
	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_VEGA_EXT_U_RDR_RESULT], 	SystemTestPara.cardRdrResult[SYS_TEST_EXT_HID_ICLASS_U]);
	WriteJSONNumericData(pTXDataFile, sysTestReaderTest[SYS_TEST_READER_TEST_VEGA_EXT_W_RDR_RESULT], 	SystemTestPara.cardRdrResult[SYS_TEST_EXT_HID_ICLASS_W]);

	return SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
/************			SYSTEM TEST -> GENERIC IO TEST          *************/
//////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//	SetSysTestGenericIO()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************

UINT8 SetSysTestGenericIO(FILE *pRXDataFile, FILE *pTXDataFile)
{
	CHAR 	tempFieldName[MAX_WEB_JVS_FIELD_NAME_LENGTH], tempFieldValue[MAX_WEB_JVS_FIELD_VALUE_LENGTH];
	UINT8	parseStatus, getElementStatus, elementIdx, status = WAIT;
	SYSTEM_TEST_STATES_e	testState = MAX_SYSTEM_TEST_STATES;

	if( (TestStatusF == SYSTEM_TEST_STATUS_PROGRESS) || (rtcTestThreadStatus == THREAD_ACTIVE) )
	{
		return WAIT;
	}
	else if(TestStatusF == SYSTEM_TEST_STATUS_COMPLETE)
	{
		TestStatusF = SYSTEM_TEST_STATUS_IDLE;
		SaveDataAndGenerateTxData(pTXDataFile);
		return SUCCESS;
	}
	else if(rtcTestThreadStatus == THREAD_DONE)
	{
		rtcTestThreadStatus = THREAD_INACTIVE;
		SaveDataAndGenerateTxData(pTXDataFile);
		return SUCCESS;
	}

	while( (parseStatus = ParseQueryField(pRXDataFile, &tempFieldName[0], MAX_WEB_JVS_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_WEB_JVS_FIELD_VALUE_LENGTH) ) != FAIL)
	{
		DPRINT(WEBJVS_LOG,"%s=%s", tempFieldName, tempFieldValue);
		if( (getElementStatus = GetWebPageElementIdx(&sysTestGenericIO[0], MAX_TOTAL_GENERIC_IO_TEST, tempFieldName, &elementIdx)) == SUCCESS)
		{
			switch(elementIdx)
			{
				default:
					break;

				case SYS_TEST_GENERIC_IO_BUZ_LED:
				{
					SystemTestPara.genericIoResult[GENERIC_BUZ_LED] = (BOOL)atoi(tempFieldValue);
					status = SUCCESS;
				}break;

				case SYS_TEST_GENERIC_IO_TEST_AUX_RELAY:
				{
					SystemTestPara.genericIoResult[GENERIC_AUX_RELAY] = (BOOL)atoi(tempFieldValue);
					status = SUCCESS;
				}break;

				case SYS_TEST_GENERIC_IO_TEST_DOOR_RELAY:
				{
					SystemTestPara.genericIoResult[GENERIC_DOOR_RELAY] = (BOOL)atoi(tempFieldValue);
					status = SUCCESS;
				}break;

				case TEST_TYPE_BUZ_LED:
				{
					SetBuzzer(ACCESS_TEST_BUZLEDS_RESP, INT_EXT_PORT);
					SetLed(ACCESS_TEST_BUZLEDS_RESP, INT_EXT_PORT);
					status = SUCCESS;
				}break;

				case SYS_TEST_GENERIC_IO_TEST_LCD_TEST:
				{
					SetAudioVisual(USER_RESP_LCD_TEST_START, INTERNAL_PORT, NULL);
					status = SUCCESS;
				}break;

				case SYS_TEST_GENERIC_IO_TEST_LCD:
				{
					SystemTestPara.genericIoResult[GENERIC_LCD_TEST] = (BOOL)atoi(tempFieldValue);
					status = SUCCESS;
				}break;

				case TEST_TYPE_AUX_RELAY:
				{
					testState = SYSTEM_TEST_AUX_RELAY;
				}break;

				case TEST_TYPE_DOOR_RELAY:
				{
					testState = SYSTEM_TEST_DOOR_RELAY;
				}break;

				case TEST_TYPE_TOUCH_PANEL:
				{
					testState = SYSTEM_TEST_TOUCH_PANEL;
				}break;

				case TEST_TYPE_AUX_IN:
				{
					testState = SYSTEM_TEST_AUX_IN;
				}break;

				case TEST_TYPE_DOOR_SENSE:
				{
					testState = SYSTEM_TEST_DOOR_SENSE;
				}break;

				case TEST_TYPE_DOOR_TMPR:
				{
					testState = SYSTEM_TEST_DOOR_TMPR;
				}break;

				case TEST_TYPE_EXT_TMPR:
				{
					testState = SYSTEM_TEST_EXT_TMPR;
				}break;

				case TEST_TYPE_LOCK_TMPR:
				{
					testState = SYSTEM_TEST_LOCK_TMPR;
				}break;

				case TEST_TYPE_EXT_SW:
				{
					testState = SYSTEM_TEST_EXT_SWITCH;
				}break;

				case TEST_TYPE_USB:
				{
					if(UsbConnectionStatus() == HIGH_SPEED_USB)
					{
						SystemTestPara.genericIoResult[GENERIC_USB] = SUCCESS;
					}
					else
					{
						SystemTestPara.genericIoResult[GENERIC_USB] = FAIL;
					}
					status = SUCCESS;
				}break;

				case TEST_TYPE_MMC_CARD_TEST:
				{
					if(MmcInfo.status == MMC_CONNECTED)
					{
						SystemTestPara.genericIoResult[GENERIC_MEMORY_CARD] = SUCCESS;
					}
					else
					{
						SystemTestPara.genericIoResult[GENERIC_MEMORY_CARD] = FAIL;
					}
					status = SUCCESS;
				}break;

				case TEST_TYPE_RTC:
				{
					if(rtcTestThreadStatus == THREAD_INACTIVE)
					{
						if(pthread_create(&rtcTestThreadData, NULL, RtcTestThread, NULL) == 0)
						{
							// In case thread is completed just before call reaches here (which is observed),
							// do not change thread status to active
							if(rtcTestThreadStatus == THREAD_INACTIVE)
							{
								rtcTestThreadStatus = THREAD_ACTIVE;
							}
							// Thread created successfully now detach thread ??
							pthread_detach(rtcTestThreadData);
						}
					}
				}break;
			}
		}
		if( (parseStatus == WEB_JVS_LAST_FIELD) || (parseStatus == WEB_JVS_LAST_FIELD_WITHOUT_DATA) )
		{
			WriteSysTestPara();
			DPRINT(WEBJVS_LOG," System_Test_Generic_INPUT/OUTPUT_Test done successfully");
			if(testState != MAX_SYSTEM_TEST_STATES)
			{
				SwitchDcToSysTestState(testState);
			}
			else if(rtcTestThreadStatus == THREAD_INACTIVE)
			{
				WriteWebResponse(pTXDataFile, "0");
				WriteJSONStartTag(pTXDataFile);	//JSON Data Start
				FillSysTestGenericIO(pTXDataFile);
				WriteJSONENDTag(pTXDataFile);	//JSON Data END
			}
			break;
		}
	}
	return status;
}

//*****************************************************************************
//	GetSysTestGenericIO()
//	Param:READER
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 GetSysTestGenericIO(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	//Write response code
	WriteWebResponse(pTXDataFile, "0");
	//Write response data
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	FillSysTestGenericIO(pTXDataFile);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END
	return SUCCESS;
}

//*****************************************************************************
//	FillSysTestGenericIO()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 FillSysTestGenericIO(FILE* pTXDataFile)
{
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_BUZ_LED], 			SystemTestPara.genericIoResult[GENERIC_BUZ_LED]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_AUX_RELAY], 	SystemTestPara.genericIoResult[GENERIC_AUX_RELAY]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_DOOR_RELAY],	SystemTestPara.genericIoResult[GENERIC_DOOR_RELAY]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_TOUCH_PANEL],	SystemTestPara.genericIoResult[GENERIC_TOUCH_PANEL]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_AUX_IN], 		SystemTestPara.genericIoResult[GENERIC_AUX_IN]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_DOOR_SENSE],	SystemTestPara.genericIoResult[GENERIC_DOOR_SENSE]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_SYS_TMPR], 		SystemTestPara.genericIoResult[GENERIC_DOOR_TMPR]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_EXT_TMPR], 		SystemTestPara.genericIoResult[GENERIC_EXT_TMPR]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_LOCK_TMPR], 	SystemTestPara.genericIoResult[GENERIC_LOCK_TMPR]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_EXT_SWITCH],	SystemTestPara.genericIoResult[GENERIC_EXT_SWITCH]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_USB], 			SystemTestPara.genericIoResult[GENERIC_USB]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_MMC_CARD_TEST], SystemTestPara.genericIoResult[GENERIC_MEMORY_CARD]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_RTC_TEST], 		SystemTestPara.genericIoResult[GENERIC_RTC_TEST]);
	WriteJSONNumericData(pTXDataFile, sysTestGenericIO[SYS_TEST_GENERIC_IO_TEST_LCD], 			SystemTestPara.genericIoResult[GENERIC_LCD_TEST]);

	return SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
/************			SYSTEM TEST -> OTHER TEST          *************/
//////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//	SetSysTestOtherInfo()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 SetSysTestOtherInfo(FILE *pRXDataFile, FILE *pTXDataFile)
{
	CHAR 	tempFieldName[MAX_WEB_JVS_FIELD_NAME_LENGTH], tempFieldValue[MAX_WEB_JVS_FIELD_VALUE_LENGTH],cmdbuf[256];
	UINT8	parseStatus, getElementStatus, elementIdx, status=WAIT;

	while( (parseStatus = ParseQueryField(pRXDataFile, &tempFieldName[0], MAX_WEB_JVS_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_WEB_JVS_FIELD_VALUE_LENGTH) ) != FAIL)
	{
		DPRINT(WEBJVS_LOG,"%s=%s", tempFieldName, tempFieldValue);
		if( (getElementStatus = GetWebPageElementIdx(&sysTestOtherTest[0], MAX_TOTAL_OTHER_TEST, tempFieldName, &elementIdx)) == SUCCESS)
		{
			switch(elementIdx)
			{
				default:
					break;

				case SYS_TEST_OTHER_TEST_USER_ENROLL_TEST:
				{
					CreateTestUser(0);
					EnrollTestUser(0);
					status = SUCCESS;
				}break;

				case SYS_TEST_OTHER_TEST_SYSTEM_DEFAULT_TEST:
				{
					if(WriteDfltSysFlag(SYS_DFLT_ALL) == SUCCESS)
					{
						#ifdef OEM_SUPPORTED
						OEM_SetDefaultSysCodeInPlatform();
						#endif
						sprintf(cmdbuf,"touch %s",DEVICE_IP_SIGN_FILE);
						if(system(cmdbuf) != 0)
						{
							EPRINT(SYS_LOG,"Failed to create %s",DEVICE_IP_SIGN_FILE);
						}
						sprintf(cmdbuf,"touch %s",SERVER_IP_SIGN_FILE);
						if(system(cmdbuf) != 0)
						{
							EPRINT(SYS_LOG,"Failed to create %s",SERVER_IP_SIGN_FILE);
						}
						SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
					}
					status = SUCCESS;
				}break;

				case SYS_TEST_OTHER_TEST_SYSTEM_REBOOT_TEST:
				{
					SystemRebootTestPata.sysRebootCnt = (BOOL)atoi(tempFieldValue);
					StartRebootTest();
					status = SUCCESS;
				}break;

				case SYS_TEST_OTHER_TEST_USER_ENROLL_RESULT:
				{
					SystemTestPara.usrEnrlResult = (BOOL)atoi(tempFieldValue);
					status = SUCCESS;
				}break;

				case SYS_TEST_OTHER_TEST_SYSTEM_DEFAULT_RESULT:
				{
					SystemTestPara.sysDfltResult = (BOOL)atoi(tempFieldValue);
					status = SUCCESS;
				}break;

				case SYS_TEST_OTHER_TEST_WIFI_TEST_RESULT:
				{
					SystemTestPara.wifiResult = (BOOL)atoi(tempFieldValue);
					status = SUCCESS;
				}break;
			}
		}
		if( (parseStatus == WEB_JVS_LAST_FIELD) || (parseStatus == WEB_JVS_LAST_FIELD_WITHOUT_DATA) )
		{
			DPRINT(WEBJVS_LOG," System_Test_Other_Test done successfully");
			WriteSysTestPara();
			//Write response data
			WriteWebResponse(pTXDataFile, "0");
			WriteJSONStartTag(pTXDataFile);	//JSON Data Start
			FillSysTestOtherInfo(pTXDataFile);
			WriteJSONENDTag(pTXDataFile);	//JSON Data END
			break;
		}
	}
	return status;
}

//*****************************************************************************
//	GetSysTestOtherInfo()
//	Param:READER
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 GetSysTestOtherInfo(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	//Write response code
	WriteWebResponse(pTXDataFile, "0");
	//Write response data
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	FillSysTestOtherInfo(pTXDataFile);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END
	return SUCCESS;
}

//*****************************************************************************
//	FillSysTestOtherInfo()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 FillSysTestOtherInfo(FILE* pTXDataFile)
{
	WriteJSONNumericData(pTXDataFile, sysTestOtherTest[SYS_TEST_OTHER_TEST_USER_ENROLL_RESULT], 	SystemTestPara.usrEnrlResult);
	WriteJSONNumericData(pTXDataFile, sysTestOtherTest[SYS_TEST_OTHER_TEST_SYSTEM_DEFAULT_RESULT], 	SystemTestPara.sysDfltResult);
	WriteJSONNumericData(pTXDataFile, sysTestOtherTest[SYS_TEST_OTHER_TEST_SYSTEM_REBOOT_COUNT], 	SystemRebootTestPata.sysRebootCnt);
	WriteJSONNumericData(pTXDataFile, sysTestOtherTest[SYS_TEST_OTHER_TEST_WIFI_TEST_RESULT], 		SystemTestPara.wifiResult);
	WriteJSONNumericData(pTXDataFile, sysTestOtherTest[SYS_TEST_OTHER_TEST_BLUETOOTH_TEST_RESULT], 	SystemTestPara.bluetoothResult);
	return SUCCESS;
}

//*****************************************************************************
//	RtcTestThread()
//	Param:
//		IN: NONE
//		OUT:NONE.
//	Returns:
//		NONE
//	Description:
//		This function is a thread function. it test rtc by reading time from rtc
//		in the interval of 5 second and verify the difference in time is not other then
//		5+-1 seconds.
//*****************************************************************************
static VOIDPTR RtcTestThread(UNUSED_PARA VOIDPTR dummy)
{
	CHAR			tempBuf[64];
	BOOL			result = SUCCESS;
	struct timeval	tv1, tv2;

	GetTimeFromRTC(&tv1);
	sleep(5);
	GetTimeFromRTC(&tv2);
	if( (tv2.tv_sec - tv1.tv_sec) < 4 || (tv2.tv_sec - tv1.tv_sec) > 6)
	{
		result = FAIL;
	}

	SystemTestPara.genericIoResult[GENERIC_RTC_TEST] = result;

	SysBasicPara.timeUpdateMode = AUTO_TIME;
	snprintf(tempBuf, sizeof(tempBuf), "su - se -c \"sntp -4 -s -t 3 %s &\"", (GetServerIpAddressPtr(acmsCurActiveInterface) ) );
	system(tempBuf);

	rtcTestThreadStatus = THREAD_DONE;

	pthread_exit(NULL);
}

//*****************************************************************************
//	UsbConnectionStatus()
//	Param:
//		IN: NONE
//		OUT:NONE.
//	Returns:
//		NONE
//	Description:
//		This function read USBSpeed file from tmp directory and check usb status
//		and return status
//
//*****************************************************************************
static USB_CONNECTION_STATUS_e UsbConnectionStatus(void)
{
	FILE *						fp;
	INT32						speed = 0;
	USB_CONNECTION_STATUS_e 	status = USB_NOT_CONNECTED;

	fp = fopen(USB_SPEED_FILE, "r");
	if(fp == NULL)
	{
		EPRINT(WEBJVS_LOG,"fail to open usb speed information file");
		return USB_NOT_CONNECTED;
	}

	fscanf(fp,"%d", &speed);

	if(speed >= 480)
	{
		status = HIGH_SPEED_USB;
	}
	else if(speed < 480 && speed > 0)
	{
		status = LOW_SPEED_USB;
	}
	else if(speed == 0)
	{
		status = USB_NOT_CONNECTED;
	}
	fclose(fp);
	return status;
}

//*****************************************************************************
//	SaveDataAndGenerateTxData()
//	Param: WEB_JVS_HDR_t *pHeader
//		IN:
//		OUT:NONE.
//	Returns:
//		NONE
//	Description:
//		This function save data into non-volatile memory and generate TxData file
//			and fill it by appropriate information.
//*****************************************************************************
static void SaveDataAndGenerateTxData(FILE	*pTXDataFile)
{
	//write into non-volatile memory.
	WriteSysTestPara();

	WriteWebResponse(pTXDataFile, "0");
	//Write response data
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start

	FillSysTestBasicInfo(pTXDataFile);
	FillSysTestHWTest(pTXDataFile);
	FillSysTestReaderTest(pTXDataFile);
	FillSysTestGenericIO(pTXDataFile);
	FillSysTestOtherInfo(pTXDataFile);

	WriteJSONENDTag(pTXDataFile);	//JSON Data END
}

//////////////////////////////////////////////////////////////////////////////
/************	SYSTEM TEST -> Generate / Display / Send Report     *********/
//////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//	SysTestGenerateReport()
//	SysTestSendReportToFtp()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 SysTestGenerateReport(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	if(text2PdfConverterThreadStatus == THREAD_DONE)
	{
		text2PdfConverterThreadStatus = THREAD_INACTIVE;
		SaveDataAndGenerateTxData(pTXDataFile);
		return SUCCESS;
	}
	else if(text2PdfConverterThreadStatus == THREAD_ACTIVE)
	{
		return WAIT;
	}

	GenerateTestReport();
	return WAIT;
}

UINT8 SysTestSendReportToFtp(UNUSED_PARA FILE *pRXDataFile, UNUSED_PARA FILE *pTXDataFile)
{
	SendTestReport();
	return SUCCESS;
}
