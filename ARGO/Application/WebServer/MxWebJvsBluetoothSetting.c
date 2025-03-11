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
//   Created By   : vaibahv darji
//   File         : MxWebJvsBluetoothSetting.c
//   Description  :
//					This file defines the API related to Upload/Edit Door
//					Mode Customize wallpapers.
/////////////////////////////////////////////////////////////////////////////

#include "MxDeviceSpecific.h"

//********Place all include files ****
//system includes
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//Matrix include files
#include "MxBluetooth.h"
#include "MxWebJeeves.h"
#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxLogs.h"
#include "MxWebJvsBluetoothSetting.h"
#include "MxSysUtils.h"
#include "MxAvResp.h"
#include "MxGuiHandler.h"
#include "MxDcTaskUtils.h"
#include "MxConfigFileIo.h"
#include "MxBluetoothConfig.h"
#include "MxSysTest.h"
#include "MxGpio.h"
#include "MxConfigUpdateHandler.h"
#include "MxCommonBLE.h"

//******** Extern Variables **********
extern BLUETOOTH_CONFIG_t		BluetoothPara;
extern BT_DISCOVERY_INFO_t 		BtDiscoveryInfo[MAX_BT_DISCOVER_DEVICE_COUNT];
extern UINT8					BtDiscoveredCount;
extern BOOL						IsBtDiscoveryRunningF;
extern SYSTEM_TEST_PARA_t		SystemTestPara;
extern BLUETOOTH_SERVICE_CHARACTERISTICS_UUID_t BleServiceCharacteristicUUID;
//******** Defines and Data Types ****

#define GEN_BLUETOOTH_SETTING_ENUM(MODE_SPECIFIC_WALLPAPER_IMAGE_ENUM, MODE_SPECIFIC_WALLPAPER_IMAGE_NAME) MODE_SPECIFIC_WALLPAPER_IMAGE_ENUM,
#define GEN_BLUETOOTH_SETTING_NAME(MODE_SPECIFIC_WALLPAPER_IMAGE_ENUM, MODE_SPECIFIC_WALLPAPER_IMAGE_NAME) MODE_SPECIFIC_WALLPAPER_IMAGE_NAME,

#define BLUETOOTH_SETTING(API)\
	API(BLUETOOTH_SETTING_ADVERTISE_BLT,	"AdvertiseBluetooth")\
	API(BLUETOOTH_SETTING_MAC_ADDRESS,  	"MACAddress")\
	API(BLUETOOTH_SETTING_NAME,  			"name")\
	API(BLUETOOTH_SETTING_UUID,  			"UUID")\
	API(BLUETOOTH_SETTING_RANGE,  			"btRange")\
	API(BLUETOOTH_CUSTOM_SETTING_RANGE,  	"CustomBtRange")\

typedef enum
{
	BLUETOOTH_SETTING(GEN_BLUETOOTH_SETTING_ENUM)
	MAX_BLUETOOTH_SETTING,
}MAX_BLUETOOTH_SETTING_e;

#define BT_DISCOVERY_LIST(API)\
			API(BT_DISCOVERY_ADDITIONAL_FRAME,		"btDisAddFrameReqF")\
			API(BT_DISCOVERY_STS,					"btDiscoverySts")\
			API(BT_DISCOVER_CNT,					"btDiscoverCnt")\
			API(BT_DEVICE_NAME,						"btDeviceName")\
			API(BT_SIGNAL_STRENGTH,					"btStrength")\

typedef enum
{
	BT_DISCOVERY_LIST(GEN_QUERY_PARA_ENUM)
	MAX_TOTAL_BT_DISCOVERY_LIST,
}BT_DISCOVERY_LIST_PARA_e;

//******** Function Prototypes *******

//******** Global Variables **********

//******** Static Variables **********
static CHARPTR bluethoothSettingsStr[MAX_BLUETOOTH_SETTING]=
{
	BLUETOOTH_SETTING(GEN_BLUETOOTH_SETTING_NAME)
};

static CHARPTR btDiscoveryList[MAX_TOTAL_BT_DISCOVERY_LIST]=
{
	BT_DISCOVERY_LIST(GEN_QUERY_PARA_NAME)
};

static pthread_t			btDiscoveryThreadId;
static THREAD_STATUS_e		btDiscoveryThreadStatus = THREAD_INACTIVE;
static BOOL					btDiscoveryStatus = FALSE;
static BOOL					sendBtDiscoveryRespF = FALSE;

//******** Function Definations ******
static BOOL StartBtDeviceDiscoveryThread(void);
static VOIDPTR BtDeviceDiscoveryThreadFn(VOIDPTR dummy);

//*****************************************************************************
//	SetDfltBluetoothSetting()
//	Param:
//		IN: pRXDataFile - pointer to  received data file (RxData.txt).
//		OUT:NONE.
//	Returns:NONE
//	Description:
//				Set Default BluetoothSetting
//*****************************************************************************
UINT8 SetDfltBluetoothSetting(__attribute__((unused))FILE *pRXDataFile, FILE *pTXDataFile)
{
	LoadDfltBluetoothConfig();
	SetBtDeviceNameProperty();
	WriteWebResponse(pTXDataFile, "0");
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	FillBluetoothSetting(pTXDataFile);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END
	return SUCCESS;
}

//*****************************************************************************
//	SetBluetoothSetting()
//	Param:
//		IN: pRXDataFile - pointer to  received data file (RxData.txt).
//		OUT:NONE.
//	Returns:NONE
//	Description:
//			When user select update file(wallpaper image) at client
//	 		side, function uploads file at respective folder.
//*****************************************************************************
UINT8 SetBluetoothSetting(FILE *pRXDataFile, FILE *pTXDataFile)
{
	CHAR 	tempFieldName[MAX_WEB_JVS_FIELD_NAME_LENGTH], tempFieldValue[MAX_WEB_JVS_FIELD_VALUE_LENGTH];
	UINT8 	parseStatus, getElementStatus, elementIdx,status = FAIL;
	BLUETOOTH_CONFIG_t tmpBluetoothPara = BluetoothPara;

	while( (parseStatus = ParseQueryField(pRXDataFile, &tempFieldName[0], MAX_WEB_JVS_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_WEB_JVS_FIELD_VALUE_LENGTH) ) != FAIL)
	{
		DPRINT(WEBJVS_LOG,"%s=%s", tempFieldName, tempFieldValue);
		if( (getElementStatus = GetWebPageElementIdx(&bluethoothSettingsStr[0], MAX_BLUETOOTH_SETTING, tempFieldName, &elementIdx)) == SUCCESS)
		{
			switch(elementIdx)
			{
				case BLUETOOTH_SETTING_ADVERTISE_BLT:
				{
					if(tmpBluetoothPara.btEnable != (BOOL)atoi(tempFieldValue))
					{
						tmpBluetoothPara.btEnable = (BOOL)atoi(tempFieldValue);
					}
				}break;

				case BLUETOOTH_SETTING_NAME:
				{
					if(strcmp(tmpBluetoothPara.btName, tempFieldValue) != 0)
					{
						snprintf(tmpBluetoothPara.btName, BT_NAME_SIZE, "%s", tempFieldValue);
					}
				}break;

				case BLUETOOTH_SETTING_RANGE:
				{
					if(tmpBluetoothPara.btRange != (UINT8)atoi(tempFieldValue))
					{
						tmpBluetoothPara.btRange = (UINT8)atoi(tempFieldValue);
					}
				}break;

				case BLUETOOTH_CUSTOM_SETTING_RANGE:
				{
					if(tmpBluetoothPara.btRange == BLE_RANGE_CUSTOM)
					{
						INT16 tempBtCustomeRangeStrength = (INT16)atoi(tempFieldValue);
						if((tempBtCustomeRangeStrength >= RANGE_MIN_DBM) && (tempBtCustomeRangeStrength <= RANGE_MAX_DBM))
						{
							tmpBluetoothPara.btCustomeRangeStrength = tempBtCustomeRangeStrength;
							DPRINT(WEBJVS_LOG,"tmpBluetoothPara.btCustomeRangeStrength : %d",tmpBluetoothPara.btCustomeRangeStrength);
						}
					}
				}break;
				default:
				break;
			}
		}
		if( (parseStatus == LAST_FIELD) || (parseStatus == LAST_FIELD_WITHOUT_DATA) )
		{
			if(ConfigUpdateHandler(BLE_CONFIG_ID, UNUSED_FIELD, (VOIDPTR)&tmpBluetoothPara) == SUCCESS)
			{
				DPRINT(WEBJVS_LOG,"BLE set successfully configIdx");
				status= SUCCESS;
			}
			break;
		}
	}
	WriteWebResponse(pTXDataFile, "0");
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	FillBluetoothSetting(pTXDataFile);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END
	return status;
}

//*****************************************************************************
//	GetBluetoothSetting()
//	Param:
//		IN:
//		OUT:
//	Returns:SUCCESS/FAIL/WAIT
//	Description:This function Get Bluetooth Setting
//*****************************************************************************
UINT8 GetBluetoothSetting(__attribute__((unused))FILE *pRXDataFile, FILE *pTXDataFile)
{
	WriteWebResponse(pTXDataFile, "0");
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	FillBluetoothSetting(pTXDataFile);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END
	return SUCCESS;
}
//*****************************************************************************
//	FillBluetoothSetting()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 FillBluetoothSetting(FILE* pTXDataFile)
{
	UINT8 status = SUCCESS, idx, uuidIdx, valueStrIdx;
	CHAR valueStr[MAX_WEB_JVS_FIELD_VALUE_LENGTH];

	for(idx = BLUETOOTH_SETTING_ADVERTISE_BLT; idx < MAX_BLUETOOTH_SETTING; idx++)
	{
		switch(idx)
		{
		case BLUETOOTH_SETTING_ADVERTISE_BLT:
			WriteJSONNumericData(pTXDataFile, bluethoothSettingsStr[idx], BluetoothPara.btEnable);
			break;

		case BLUETOOTH_SETTING_NAME:
			WriteJSONStringData(pTXDataFile, bluethoothSettingsStr[idx], BluetoothPara.btName);
			break;

		case BLUETOOTH_SETTING_MAC_ADDRESS:
			WriteJSONStringData(pTXDataFile, bluethoothSettingsStr[idx], BluetoothPara.btMacAddress);
			break;

		case BLUETOOTH_SETTING_UUID:
			uuidIdx = 16;
			valueStrIdx = 0;
			while( (uuidIdx--) != 0)
			{
				valueStrIdx += sprintf(&valueStr[valueStrIdx], "%02X", BleServiceCharacteristicUUID.btServiceUuid[uuidIdx]);
			}
			WriteJSONStringData(pTXDataFile, bluethoothSettingsStr[idx], valueStr);
			break;

		case BLUETOOTH_SETTING_RANGE:
			WriteJSONNumericData(pTXDataFile, bluethoothSettingsStr[idx], BluetoothPara.btRange);
			break;
		case BLUETOOTH_CUSTOM_SETTING_RANGE:
		{
			WriteJSONIntData(pTXDataFile, bluethoothSettingsStr[idx], BluetoothPara.btCustomeRangeStrength);
		}break;
		}
	}

	status = SUCCESS;
	return status;
}

//*****************************************************************************
//	GetBtDeviceList()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 GetBtDeviceList(__attribute__((unused))FILE *pRXDataFile, FILE *pTXDataFile)
{
	// if startStopBtServiceThread is already running then do not start this thread
	if((IsStartStopBtServiceThreadIsRunning() == TRUE) || (IsBtHealthMonitorThreadIsRunning() == TRUE))
	{
		return WAIT;
	}

	if(btDiscoveryThreadStatus == THREAD_ACTIVE)
	{
		if(sendBtDiscoveryRespF == TRUE)
		{
			sendBtDiscoveryRespF = FALSE;
			FillBtDeviceList(pTXDataFile, YES);
			return SUCCESS;
		}
		return WAIT;
	}
	else if(btDiscoveryThreadStatus == THREAD_DONE)
	{
		sendBtDiscoveryRespF = FALSE;
		FillBtDeviceList(pTXDataFile, NO);

		// Bluetooth Systemtest stops here so modify and store test result.
		if(BtDiscoveredCount > 0)
		{
			DPRINT(DC_LOG, "Bluetooth System Test: SUCCESS");
			SystemTestPara.bluetoothResult = SUCCESS;
		}
		else
		{
			DPRINT(DC_LOG, "Bluetooth System Test: FAIL");
			SystemTestPara.bluetoothResult = FAIL;
		}
		WriteSysTestPara();

		btDiscoveryThreadStatus = THREAD_INACTIVE;
		return SUCCESS;
	}

	// Start Bluetooth Systemtest - Initialise with zero count to start Bt device Discovery
	DPRINT(DC_LOG, "Bluetooth System Test: START");
	SystemTestPara.bluetoothResult = FAIL;
	BtDiscoveredCount = 0;
	memset((VOIDPTR)BtDiscoveryInfo, 0, sizeof(BtDiscoveryInfo));
	if(StartBtDeviceDiscoveryThread() == SUCCESS)
	{
		return WAIT;
	}
	else
	{
		return FAIL;
	}
}
//*****************************************************************************
//	StartBtAutoSystemTest()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 StartBtAutoSystemTest(void)
{
	// if startStopBtServiceThread is already running then do not start this thread
	if((IsStartStopBtServiceThreadIsRunning() == TRUE) || (IsBtHealthMonitorThreadIsRunning() == TRUE))
	{
		return WAIT;
	}

	if(btDiscoveryThreadStatus == THREAD_ACTIVE)
	{
		if(sendBtDiscoveryRespF == TRUE)
		{
			sendBtDiscoveryRespF = FALSE;
			return SUCCESS;
		}
		return WAIT;
	}
	else if(btDiscoveryThreadStatus == THREAD_DONE)
	{
		sendBtDiscoveryRespF = FALSE;
		// Bluetooth Systemtest stops here so modify and store test result.
		if(BtDiscoveredCount > 0)
		{
			DPRINT(SYS_LOG, "Bluetooth System Test: SUCCESS");
		}
		else
		{
			DPRINT(SYS_LOG, "Bluetooth System Test: FAIL");
		}

		btDiscoveryThreadStatus = THREAD_INACTIVE;
		return SUCCESS;
	}

	// Start Bluetooth Systemtest - Initialise with zero count to start Bt device Discovery
	DPRINT(SYS_LOG, "Bluetooth System Test: START");
	SystemTestPara.bluetoothResult = FAIL;
	BtDiscoveredCount = 0;
	memset((VOIDPTR)BtDiscoveryInfo, 0, sizeof(BtDiscoveryInfo));
	if(StartBtDeviceDiscoveryThread() == SUCCESS)
	{
		return WAIT;
	}
	else
	{
		return FAIL;
	}
}

//*****************************************************************************
//	FillBluetoothSetting()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
UINT8 FillBtDeviceList(FILE* pTXDataFile, BOOL btDiscoveryAddFrameReqF)
{
	UINT8	btDeviceIdx, bufCurIdx = 0;
	CHAR	buf[1536];
	CHAR	tmpStr[512];

	//Write response code
	WriteWebResponse(pTXDataFile, "0");
	//Write response data
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	WriteJSONNumericData(pTXDataFile, btDiscoveryList[BT_DISCOVERY_ADDITIONAL_FRAME], btDiscoveryAddFrameReqF);
	WriteJSONNumericData(pTXDataFile, btDiscoveryList[BT_DISCOVERY_STS], btDiscoveryStatus);
	WriteJSONNumericData(pTXDataFile, btDiscoveryList[BT_DISCOVER_CNT], BtDiscoveredCount);

	// fill bt Device name
	bufCurIdx = sprintf(buf,"[");
	for(btDeviceIdx = 0; btDeviceIdx < BtDiscoveredCount; btDeviceIdx++)
	{
		bufCurIdx += sprintf(&buf[bufCurIdx] , "\"%s\"", PrepJsonStringData(BtDiscoveryInfo[btDeviceIdx].btDeviceName, tmpStr));
		if( (btDeviceIdx+1) != BtDiscoveredCount)
		{
			bufCurIdx += sprintf(&buf[bufCurIdx] , ",");
		}
	}
	sprintf(&buf[bufCurIdx] , "],\n");
	fprintf(pTXDataFile , "\"%s\":%s", btDiscoveryList[BT_DEVICE_NAME], buf);

	// fill bt Device Signal Strength
	bufCurIdx = sprintf(buf,"[");
	for(btDeviceIdx = 0; btDeviceIdx < BtDiscoveredCount; btDeviceIdx++)
	{
		bufCurIdx += sprintf(&buf[bufCurIdx] , "\"%s\"", PrepJsonStringData(BtDiscoveryInfo[btDeviceIdx].btSignalStrength, tmpStr));
		if( (btDeviceIdx+1) != BtDiscoveredCount)
		{
			bufCurIdx += sprintf(&buf[bufCurIdx] , ",");
		}
	}
	sprintf(&buf[bufCurIdx] , "],\n");
	fprintf(pTXDataFile , "\"%s\":%s", btDiscoveryList[BT_SIGNAL_STRENGTH], buf);

	WriteJSONENDTag(pTXDataFile);	//JSON Data END
	return SUCCESS;
}

//*****************************************************************************
//	StartBtDeviceDiscoveryThread()
//	Param:
//		IN:dummy
//		OUT:dummy
//	Returns:
//		NONE
//	Description:
//		This function start Bluetooth Device Discovery.
//
//*****************************************************************************
static BOOL StartBtDeviceDiscoveryThread(void)
{
	BOOL status = FAIL;

	if(btDiscoveryThreadStatus == THREAD_INACTIVE)
	{
		if(pthread_create(&btDiscoveryThreadId, NULL, BtDeviceDiscoveryThreadFn, NULL) == 0)
		{
			// In case thread is completed just before call reaches here (which is observed),
			// do not change thread status to active
			if(btDiscoveryThreadStatus == THREAD_INACTIVE)
			{
				btDiscoveryThreadStatus = THREAD_ACTIVE;
			}
			// Thread created successfully now detach thread ??
			pthread_detach(btDiscoveryThreadId);
			status = TRUE;
		}
		else
		{
			EPRINT(DC_LOG, "Unable to start BtDeviceDiscoveryThread");
		}
	}

	return status;
}

//*****************************************************************************
//	BtDeviceDiscoveryThreadFn()
//	Param:
//		IN:dummy
//		OUT:dummy
//	Returns:
//		NONE
//	Description:
//		This function get Bluetooth Device list.
//
//*****************************************************************************
static VOIDPTR BtDeviceDiscoveryThreadFn(__attribute__((unused))VOIDPTR dummy)
{
	UINT8 tmpIdx;
	BOOL	manualBtStartReqF = FALSE;

	// Before start Bluetooth discovery check bluetooth service is enabled or not
	// if it is not enabled then start it and after discovery and stop it.
	if(IsBluetoothServiceEnabled() == FALSE)
	{
		manualBtStartReqF = TRUE;
		StartBluetoothServiceFn();
	}

	// Start Bluetooth Device Discovery
	btDiscoveryStatus = StartBtDeviceDiscovery(BLE_SCAN, MAX_BT_DISCOVERY_TIME);
	if(btDiscoveryStatus == SUCCESS)
	{
		IsBtDiscoveryRunningF = TRUE;
		for(tmpIdx = 0; tmpIdx < (MAX_BT_DISCOVERY_TIME - 1); tmpIdx++)
		{
			sendBtDiscoveryRespF = TRUE;

			// if Maximum device is discovered then break
			if(BtDiscoveredCount >= MAX_BT_DISCOVER_DEVICE_COUNT)
			{
				break;
			}
            sleep(1);
		}

		// Stop Bluetooth Device Discovery
		StopBtDeviceDiscovery(BLE_SCAN);
		IsBtDiscoveryRunningF = FALSE;
	}

	// If bluetooth started manually then stop it
	if(manualBtStartReqF == TRUE)
	{
		StopBluetoothServiceFn();
	}

	btDiscoveryThreadStatus = THREAD_DONE;
	// unlock the resource
	pthread_exit(NULL);
}

//*****************************************************************************
//	IsBtDeviceDiscoveryThreadIsRunning()
//	Param:
//		IN:	NONE
//		OUT:NONE
//	Returns:
//		NONE
//	Description:
//		This function check if Is BtDeviceDiscoveryThread Is Running or not.
//*****************************************************************************
BOOL IsBtDeviceDiscoveryThreadIsRunning(VOID)
{
	if(btDiscoveryThreadStatus == THREAD_INACTIVE)
	{
		return FALSE;
	}
	return TRUE;
}

