#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include <unistd.h>           /* Include for getpid().                        */
#include <pthread.h>
#include "SS1BTPM.h"

#ifndef NONE
#define	NONE		0xff
#endif

#ifndef FALSE
#define	FALSE		0
#endif

#ifndef TRUE
#define	TRUE		1
#endif

#ifndef NULL
#define	NULL		(void *)0
#endif

#define	NULL_CHAR	0

enum
{
	CLEAR = 0,
	GET = 0,
	SET,
	//
	INVALID = 0,
	VALID,
	//
	ABSENT = 0,
	PRESENT,
	//
	DISABLED = 0,
	ENABLED,
	MAX_BOOL_ENUM,
	//
	OFF = 0,
	ON,
	//
	CLOSE = 0,
	OPEN,
	//
	NO = 0,
	YES,
	WAIT,
	//
	DOWN = 0,
	UP,
	//
	LOW = 0,
	HIGH,
	//
	FREE = 0,
	BUSY,
	//
	INACTIVE = 0,
	ACTIVE,
	//
	UNMASK = 0,
	MASK,
	//
	DENIED = 0,
	ALLOWED,
	//
	LESS = -1,
	EQUAL = 0,
	GREATER,
	//
	NIL = -1,
	FAIL = 0,
	SUCCESS,
	//
	BUF_FREE = 0,
	BUF_FULL,
	BUF_PARTIAL_FULL,
	//
	Q_STATUS_FREE = 0,
	Q_STATUS_ALLOCATED,
	Q_STATUS_PENDING,
	Q_STATUS_PROCESSING,
	//
	PROCESS_FURTHER = 0,
	PROCESS_DONE,
};

typedef enum
{
	THREAD_INACTIVE,
	THREAD_ACTIVE,
	THREAD_DONE,
}THREAD_STATUS_e;

typedef unsigned char			BOOL;

typedef char					CHAR;

typedef signed char				INT8;
typedef unsigned char			UINT8;

typedef signed short			INT16;
typedef unsigned short			UINT16;

typedef signed int				INT32;
typedef unsigned int  			UINT32;

typedef signed long long		INT64;
typedef unsigned long long 		UINT64;

typedef float					FLOAT32;
typedef long double				FLOAT64;

typedef INT8 *					INT8PTR;
typedef char *					CHARPTR;
typedef UINT8 *					UINT8PTR;
typedef UINT8 const *			UINT8CONSTPTR;

typedef INT16 *					INT16PTR;
typedef UINT16 *				UINT16PTR;
typedef UINT16 const *			UINT16CONSTPTR;

typedef INT32 *					INT32PTR;
typedef UINT32 *				UINT32PTR;
typedef UINT32 const *			UINTCONST32PTR;

typedef INT64 *					INT64PTR;
typedef UINT64 *				UINT64PTR;
typedef UINT64 const*			UINT64CONSTPTR;

typedef void					VOID;
typedef void *					VOIDPTR;

typedef struct
{
	BOOL	btEnable;
	CHAR 	btMacAddress[18];
	CHAR	btName[11];
	UINT8	btServiceUuid[16];
	UINT8	btWriteCharUuid[16];
	UINT8	btReadChar1Uuid[16];
	UINT8	btReadChar2Uuid[16];
}BLUETOOTH_CONFIG_t;

BLUETOOTH_CONFIG_t			BluetoothPara;
const BLUETOOTH_CONFIG_t	DfltBluetoothPara =
{
		.btEnable			= ENABLED,
		.btMacAddress 		= "",
		.btName				= "TestAppTI",
		.btServiceUuid  	= { 0x07, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC},
		.btWriteCharUuid 	= { 0x08, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC},
		.btReadChar1Uuid 	= { 0x09, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC},
		.btReadChar2Uuid 	= { 0x0A, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC},
};

#define 	WRITE_ACCESS_REQ_CHAR_OFFSET	1
#define 	READ_RANDOM_KEY_CHAR_OFFSET		3
#define		READ_STATUS_CHAR_OFFSET			5

#define 	SERVICE_TABLE_FLAGS_USE_PERSISTENT_UID		0x00000001
#define 	SERVICE_TABLE_FLAGS_SECONDARY_SERVICE 		0x00000002
#define 	NUMBER_OF_SERVICES (sizeof(serviceTable)/sizeof(SERVICE_INFO_t))

#define FUNCTION_ERROR              (-4)
#define INVALID_PARAMETERS_ERROR    (-6)
#define INDENT_LENGTH               3

// The following type definition represents the container structure
// for a Characteristic Attribute.
typedef struct
{
	UINT32		characteristicPropertiesMask;
	UINT32		securityPropertiesMask;
	UUID_128_t	characteristicUUID;
	Boolean_t	allocatedValue;
	UINT32		maximumValueLength;
	UINT32		valueLength;
	Byte_t		*value;
}CHARECTERISTIC_INFO_t;

// The following type definition represents the container structure
// for a Descriptor Attribute.
typedef struct
{
	UINT32		descriptorPropertiesMask;
	UINT32		securityPropertiesMask;
	UUID_128_t	characteristicUUID;
	Boolean_t	allocatedValue;
	UINT32		maximumValueLength;
	UINT32		valueLength;
	Byte_t		*value;
}DESCRIPTOR_INFO_t;

// The following enumeration represents all of the different
// attributes that may be added in a service table.
typedef enum
{
   AT_INCLUDE,
   AT_CHARACTERISTIC,
   AT_DESCRIPTOR
}ATTRIBUTE_TYPE_e;

// The following type definition represents the container structure for a Service Table.
// NOTE:For an AttributeType of AT_INCLUDE the AttributeParameter is not used
// (the include will automatically reference the previous service that was registered).
typedef struct
{
	ATTRIBUTE_TYPE_e	attributeType;
	UINT32				attributeOffset;
	void				*attribute;
}ATTRIBUTE_INFO_t;

// The following type definition represents the container structure
// for an dynamically allocated service.
typedef struct
{
	UINT32							flags;
	UINT32                   		serviceID;
	DWord_t                        	persistentUID;
	UUID_128_t                     	serviceUUID;
	GATT_Attribute_Handle_Group_t  	serviceHandleRange;
	UINT32                   		numberAttributes;
	ATTRIBUTE_INFO_t               	*attributeList;
}SERVICE_INFO_t;

BOOL StartStopBluetoothServiceThread(VOID);
static VOIDPTR StartStopBluetoothServiceThreadFn(VOIDPTR dummy);
static VOID StartStopBluetoothService(VOID);
VOID StartBluetoothServiceFn(VOID);
VOID StopBluetoothServiceFn(VOID);
VOID SetBtDeviceNameProperty(VOID);
static VOID BTPSAPI DEVM_Event_Callback(DEVM_Event_Data_t *eventData, VOID *callbackParameter);
static VOID BTPSAPI GATM_Event_Callback(GATM_Event_Data_t *eventData, VOID *callbackParameter);
static VOID ProcessWriteRequestEvent(GATM_Write_Request_Event_Data_t *writeRequestData);
static VOID ProcessReadRequestEvent(GATM_Read_Request_Event_Data_t *readRequestData);
static ATTRIBUTE_INFO_t *SearchServiceListByOffset(UINT32 serviceID, UINT32 attributeOffset);
static VOID InitAllUuidInfo(VOID);
static void FormatAdvertisingData(DEVM_Advertising_Information_t *AdvertisingInformation, Byte_t *AdvertisingBuffer);
static INT32 RegisterService(UINT32 serviceIndex);
static INT32 UnRegisterService(UINT32 serviceIndex);
static BOOL StartBtAdvertisement(VOID);
static VOID ConvUUIDToStr(GATT_UUID_t *uuid, CHARPTR strPtr);
VOID RunBluetoothTask(VOID);
VOID ClearWriteCharCommonBuff(VOID);
VOID BdAddrToStr(BD_ADDR_t Board_Address, CHARPTR BoardStr);

UINT32	ElapsedTick(UINT32);
UINT32	GetSysTick(void);
VOID	DisconnectRemoteDevice(VOID);

static DEVM_Advertising_Information_t 	advertisingInfo;
static pthread_t 		startStopBtServiceThreadFnId=0;
static THREAD_STATUS_e	startStopBtServiceThreadStatus = THREAD_INACTIVE;
static Mutex_t          btServiceMutex;
static BD_ADDR_t		CurrentRemoteBD_ADDR;
static UINT32        	devmCallbackID;
static UINT32        	gatmCallbackID;
static BOOL        		btInitialized = FALSE;
static BOOL        		btPoweredOn = FALSE;
static BOOL        		btAdvertised = FALSE;
static volatile BOOL    btConnected = FALSE;
static BOOL        		btServiceStatus = FALSE;
static volatile BOOL	btReadReqTimerF = FALSE;
static volatile UINT32	sysTick;
static volatile UINT32	prevSysTick = 0;
//static CHAR ReadRandResp[] = "This is Test data";
//static BOOL ReadStsResp = TRUE;
static CHAR ReadRandResp[] = {2,0,1,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static CHAR ReadStsResp[] =  {3,0,2,1,0,1,0,1,0};
static volatile BOOL ReceiveDone = FALSE;
static volatile UINT32 btTimerValue = 0 ;
static BOOL		startAdvtF = FALSE;

//***********************************************************************
//  				Bluetooth Service Tables
//***********************************************************************
//    * NOTE * For simplicity this application will not include Client
//             Characteristic Configuration Descriptors (CCCD) for
//             characteristics that are indicatable/notifiable.  This is
//             because the CCCD is a per client value that is stored
//             persistently for bonded devices.  This application, whose
//             only purpose is showing the usage of the APIs, does not
//             store per client values and also does not store values
//             persistently.
//    * NOTE * To Calculate the attributeOffset apply the following
//             formula:
//
//                attributeOffset = 1 + (NumPrevIncludes * 1) +
//                                  (NumPrevCharacteristics * 2) +
//                                  (NumPrevDescriptors * 1)
//
//             where:
//
//                NumPrevIncludes = The number of previous Include
//                                  Definition that exist in the
//                                  service table prior to the attribute
//                                  (Include, Characteristic or
//                                  Descriptor) that is being added.
//
//                NumPrevCharacteristics = The number of previous
//                                  Characteristics that exist in the
//                                  service table prior to the attribute
//                                  (Include, Characteristic or
//                                  Descriptor) that is being added.
//
//                NumPrevDescriptors = The number of previous
//                                  Descriptors that exist in the
//                                  service table prior to the attribute
//                                  (Include, Characteristic or
//                                  Descriptor) that is being added.
//***********************************************************************
//  Service Declaration.
//***********************************************************************
static CHARECTERISTIC_INFO_t writeAcceessReqCharAttr =
{
	.characteristicPropertiesMask = GATM_CHARACTERISTIC_PROPERTIES_WRITE,
	.securityPropertiesMask = GATM_SECURITY_PROPERTIES_NO_SECURITY,
	// LSB First
	// .characteristicUUID = { 0x08, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC, 0xAF},
	// UUID will be fill from Bluetoothpara config in InitAllUuidInfo function.
	.allocatedValue = TRUE,
	.maximumValueLength = 512,
	.valueLength = 0,
	.value = (Byte_t *)NULL
};

static CHARECTERISTIC_INFO_t readRandomKeyCharAttr =
{
	.characteristicPropertiesMask = GATM_CHARACTERISTIC_PROPERTIES_READ,
	.securityPropertiesMask = GATM_SECURITY_PROPERTIES_NO_SECURITY,
	// LSB First
	//.characteristicUUID = { 0x09, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC, 0xAF},
	// UUID will be fill from Bluetoothpara config in InitAllUuidInfo function.
	.allocatedValue = TRUE,
	.maximumValueLength = 512,
	.valueLength = sizeof(ReadRandResp),
	.value = (Byte_t *)&ReadRandResp,
};

static CHARECTERISTIC_INFO_t readStatusCharAttr =
{
	.characteristicPropertiesMask = GATM_CHARACTERISTIC_PROPERTIES_READ | GATM_CHARACTERISTIC_PROPERTIES_NOTIFY,
	.securityPropertiesMask = GATM_SECURITY_PROPERTIES_NO_SECURITY,
	// LSB First
	//.characteristicUUID = { 0x0A, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC, 0xAF},
	// UUID will be fill from Bluetoothpara config in InitAllUuidInfo function.
	.allocatedValue = TRUE,
	.maximumValueLength = 512,
	.valueLength = sizeof(ReadStsResp),
	.value = (Byte_t *)&ReadStsResp,
};

// The following array is the array containing attributes that are
// registered by this service.
#define SPPLE_ASSIGN_SPPLE_SERVICE_UUID_128(_x)                 ASSIGN_BLUETOOTH_UUID_128(*((UUID_128_t *)(_x)), 0x14, 0x83, 0x9A, 0xC4, 0x7D, 0x7E, 0x41, 0x5c, 0x9A, 0x42, 0x16, 0x73, 0x40, 0xCF, 0x23, 0x39)
static ATTRIBUTE_INFO_t attrList[] =
{
   { AT_CHARACTERISTIC, WRITE_ACCESS_REQ_CHAR_OFFSET, (VOID *)&writeAcceessReqCharAttr },
   { AT_CHARACTERISTIC, READ_RANDOM_KEY_CHAR_OFFSET, (VOID *)&readRandomKeyCharAttr },
   { AT_CHARACTERISTIC, READ_STATUS_CHAR_OFFSET, (VOID *)&readStatusCharAttr }
};
//***********************************************************************
//  End of Service Declaration.
//***********************************************************************

// Main service for registration
static SERVICE_INFO_t serviceTable[] =
{
   {
      .flags = SERVICE_TABLE_FLAGS_USE_PERSISTENT_UID,
      .serviceID = 0,
      .persistentUID = 0,
      //.serviceUUID = { 0x07, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC, 0xAF},
      // UUID will be fill from Bluetoothpara config in InitAllUuidInfo function.
      .serviceHandleRange = {0, 0},
      .numberAttributes = 3,
      .attributeList = attrList
   }
};


INT32 main(INT32 argc, CHARPTR argv[])
{
	BluetoothPara = DfltBluetoothPara;
	StartStopBluetoothServiceThread();

	while(1)
	{
		if(btConnected == TRUE)
		{
			if(btReadReqTimerF == TRUE)
			{
				btTimerValue = 1000;	//	1000msec timer value
			}
			else
			{
				btTimerValue = 10000;	//	10sec timer value
			}

			if((GetSysTick() - prevSysTick) >= btTimerValue)
			{
				if(btReadReqTimerF == TRUE)
				{
					printf("BtSession btTimerValue = 1000msec Timeout\n");
				}
				else
				{
					printf("BtSession btTimerValue = 10000msec Timeout\n");
				}
				
				prevSysTick = GetSysTick();
				startAdvtF = TRUE;
				DisconnectRemoteDevice();
				btReadReqTimerF = FALSE;
			}
		}
		else if(startAdvtF == TRUE) 
		{
			if((GetSysTick() - prevSysTick) >= 10000)
			{
				printf("Disconnect Timeout \n");
				prevSysTick = GetSysTick();
				StartBtAdvertisement();
			}
		}
		RunBluetoothTask();
	}
	
	return 0;
}

BOOL StartStopBluetoothServiceThread(VOID)
{
	BOOL status = FAIL;

	if(startStopBtServiceThreadStatus == THREAD_INACTIVE)
	{
		if(pthread_create(&startStopBtServiceThreadFnId, NULL, StartStopBluetoothServiceThreadFn, NULL) == 0)
		{
			status = SUCCESS;
			// In case thread is completed just before call reaches here (which is observed),
			// do not change thread status to active
			if(startStopBtServiceThreadStatus == THREAD_INACTIVE)
			{
				startStopBtServiceThreadStatus = THREAD_ACTIVE;
			}
		}
	}

	return status;
}

static VOIDPTR StartStopBluetoothServiceThreadFn(VOIDPTR dummy)
{
	printf("StartStopBluetoothServiceThreadFn thread start\n");
	StartStopBluetoothService();
	startStopBtServiceThreadStatus = THREAD_DONE;
	pthread_exit(NULL);
}

static VOID StartStopBluetoothService(VOID)
{
	if((BluetoothPara.btEnable == TRUE) && (btServiceStatus != ENABLED))
	{
		// if btEnable flag is on and btService is not enabled then start it.
		StartBluetoothServiceFn();
	}
	else if((BluetoothPara.btEnable == FALSE) && (btServiceStatus == ENABLED))
	{
		// if btEnable flag is off and btService is enabled then stop it.
		StopBluetoothServiceFn();
	}
}

VOID StartBluetoothServiceFn(VOID)
{
	struct sigaction signalAction;
	INT32  	result = 0;

#ifdef BLUETOOTH_DEBUG
	// Used for debug purpose.
	unsigned char ParameterData[] = "/matrix/BTSLog.txt";
	//unsigned char ParameterData[] = "/matrix/BTSDebugLog.cfa";
	unsigned int ParameterDataLength = strlen(ParameterData) + 1;
#endif

	// if Bluetooth Service is already enabled then return.
	if(btServiceStatus == ENABLED)
	{
		printf("Bluetooth Service is already Enabled.\n");
		return;
	}

	printf("Start Bluetooth Service is in Progress...\n");
	// Create a Mutex for Bluetooth Service.
	btServiceMutex = BTPS_CreateMutex(FALSE);

	// Initialize the signal set to empty.
	sigemptyset(&signalAction.sa_mask);
	// Flag that we want to ignore this signal.
	signalAction.sa_handler = SIG_IGN;
	signalAction.sa_flags = SA_RESTART;
	// Set the SIGTTIN signal's action.  Note that the default behavior
	// for SIGTTIN is to suspend the application.  We want to ignore the
	// signal instead so that this doesn't happen.  When the read()
	// function fails the we can check the error number to determine if
	// it was interrupted by a signal, and if it was then we can wait
	// until we have re-entered the foreground before attempting to read
	// from standard input again.
	sigaction(SIGTTIN, &signalAction, NULL);

	if(btInitialized != TRUE)
	{
		// Initializes Bluetooth Platform manager input:- PID, Init info,
		// Unregistration call back and Unregistration paramters
		result = BTPM_Initialize((unsigned long)getpid(), NULL, NULL, NULL);
		if(result == 0)
		{
			btInitialized = TRUE;
			printf("BTPM_Initialize() Success: [%d].\n", result);
			//Device callback function input:- Callback function and callback paramters
			if((result = DEVM_RegisterEventCallback(DEVM_Event_Callback, NULL)) > 0)
			{
				printf("DEVM_RegisterEventCallback() Success: %d.\n", result);
				devmCallbackID = (UINT32) result;
			}
			else
			{
				printf( "DEVM_RegisterEventCallback() Failure: %d, %s.\n", result, ERR_ConvertErrorCodeToString(result));
				return;
			}
		}
		else
		{
			printf( "BTPM_Initialize() Failure: [%d].\n", result);
			return;
		}
	}
	else
	{
		printf( "Initialization Success: Already initialized.\n");
		//return;
	}

	if(btPoweredOn == FALSE)
	{
		//Power on chip, enable's bt_en gpio, gives firmware file and tty* node to chip.
		result = DEVM_PowerOnDevice();
		if(result == 0)
		{
#ifdef BLUETOOTH_DEBUG
			if(!DEVM_EnableBluetoothDebug((Boolean_t)1, 3, 0, ParameterDataLength, ParameterData))
			{
				printf( "DEVM_EnableBluetoothDebug: Success\n");
			}
#endif
			printf("DEVM_PowerOnDevice() Success: [%d].\n", result);
			btPoweredOn = TRUE;
		}
		else
		{
			printf( "DEVM_PowerOnDevice() Failure: [%d].\n", result);
			return;
		}
	}

	// Set Device Name properties.
	SetBtDeviceNameProperty();

	//Local GATT related events callback, input:- call back function and callback parameters
	if((result = GATM_RegisterEventCallback(GATM_Event_Callback, NULL)) > 0)
	{
		printf("GATM_RegisterEventCallback() Success: %d.\n", result);
		// Note the Callback ID and flag success.
		gatmCallbackID = (UINT32)result;
	}
	else
	{
		// Error registering the Callback, inform user and flag an error.
		printf( "GATM_RegisterEventCallback() Failure: %d, %s.\n", result, ERR_ConvertErrorCodeToString(result));
		return;
	}

	if(btAdvertised != TRUE)
	{
		//Fills local memory with advertising parameters:- flags, timing
		BTPS_MemInitialize(&advertisingInfo, 0, sizeof(DEVM_Advertising_Information_t));
		advertisingInfo.AdvertisingFlags =  DEVM_ADVERTISING_INFORMATION_FLAGS_CONNECTABLE | DEVM_ADVERTISING_INFORMATION_FLAGS_ADVERTISE_DEVICE_NAME ;//| DEVM_ADVERTISING_INFORMATION_FLAGS_USE_PUBLIC_ADDRESS;		//flags
		advertisingInfo.AdvertisingDuration = 0xFFFFFFFF;	//Duration

		InitAllUuidInfo();
		//Registers service to GATT profile.
		RegisterService(0);
	}
	else
	{
		// Not initialized, flag an error.
		printf( "Platform Manager has not been initialized.\n");
	}

	DEVM_Set_Advertising_Intervals((Word_t)20, (Word_t)40);
	if(StartBtAdvertisement() == SUCCESS)
	{
		// here Bt service is successfully start.
		btServiceStatus = ENABLED;
		printf( "Bluetooth Service is Enabled.\n");
	}
	else
	{
		btServiceStatus = DISABLED;
		printf( "Bluetooth Service is not able to advertise.\n");
	}
}

VOID StopBluetoothServiceFn(VOID)
{
	INT32 result;

	if(btAdvertised == TRUE)
	{
		//Fills local memory with advertising parameters:- flags, timing
		BTPS_MemInitialize(&advertisingInfo, 0, sizeof(DEVM_Advertising_Information_t));
		advertisingInfo.AdvertisingFlags = 0x00000001;		//flags to force stop advertising.

		if((result = DEVM_StopAdvertising(advertisingInfo.AdvertisingFlags)) == 0)
		{
			btAdvertised = FALSE;
			printf("DEVM_StopAdvertising() Success.\n");
		}
		else
		{
			btAdvertised = FALSE;
			printf( "DEVM_StopAdvertising() Failure: [%d] [%s].\n", result, ERR_ConvertErrorCodeToString(result));
			//return;
		}
	}

	// UnRegister the current Bt Service
	UnRegisterService(0);

	// Unregister GATT event callback functions.
	if(gatmCallbackID != 0)
	{
		if((result = GATM_UnRegisterEventCallback(gatmCallbackID)) == 0)
		{
			printf("GATM_UnRegisterEventCallback() Success: [%d].\n", result);
			// Flag that there is no longer a Callback registered.
			gatmCallbackID = 0;
		}
		else
		{
			gatmCallbackID = 0;
			// Error un-registering the Callback, inform user and flag an error.
			printf( "GATM_UnRegisterEventCallback() Failure: [%d] [%s].\n", result, ERR_ConvertErrorCodeToString(result));
			//return;
		}
	}

	// Unregister DEVM event callback functions.
	if(devmCallbackID != 0)
	{
		DEVM_UnRegisterEventCallback(devmCallbackID);
		devmCallbackID = 0;
		printf("DEVM_UnRegisterEventCallback() Success.\n");
	}

	// if bt device is powered on then turn off it.
	if(btPoweredOn == TRUE)
	{
		//Power off chip.
		result = DEVM_PowerOffDevice();
		if(result == 0)
		{
			printf("DEVM_PowerOffDevice() Success: [%d].\n", result);
			btPoweredOn = FALSE;
		}
		else
		{
			printf( "DEVM_PowerOffDevice() Failure: [%d].\n", result);
		}
	}

	if(btInitialized == TRUE)
	{
		BTPM_Cleanup();
		printf("BTPM_Cleanup() Success:.\n");
	}

	// Here Bluetooth service is completely disabled.
	btServiceStatus = DISABLED;
	ClearWriteCharCommonBuff();
	if(btPoweredOn == FALSE)
	{
		printf( "Bluetooth Service is Disabled successfully.\n");
	}
	else
	{
		btPoweredOn = FALSE;
		printf( "Bluetooth Service is Disabled forcefully.\n");
	}
}

VOID SetBtDeviceNameProperty(VOID)
{
	INT32 result;
	DEVM_Local_Device_Properties_t localDeviceProperties;

	if(btPoweredOn == TRUE)
	{
		//fills local memory with local device information.
		BTPS_MemInitialize(&localDeviceProperties, 0, sizeof(localDeviceProperties));
		localDeviceProperties.DeviceNameLength = strlen(BluetoothPara.btName);
		snprintf(localDeviceProperties.DeviceName, 11, "%s", BluetoothPara.btName);

		//updates local device properties, used here to change name of device.
		if((result = DEVM_UpdateLocalDeviceProperties(DEVM_UPDATE_LOCAL_DEVICE_PROPERTIES_DEVICE_NAME, &localDeviceProperties)) >= 0)
		{
			printf("DEVM_UpdateLocalDeviceProperties() Success: %d.\n", result);
		}
		else
		{
			// Error updating the Local Device Properties, inform the user and flag an error.
			printf("DEVM_UpdateLocalDeviceProperties() Failure: %d, %s.\n", result, ERR_ConvertErrorCodeToString(result));
		}
	}
}

static VOID InitAllUuidInfo(VOID)
{
	// copy service UUID
	memcpy((VOIDPTR)&serviceTable[0].serviceUUID, (VOIDPTR)BluetoothPara.btServiceUuid, sizeof(serviceTable[0].serviceUUID));
	memcpy((VOIDPTR)&writeAcceessReqCharAttr.characteristicUUID, (VOIDPTR)BluetoothPara.btWriteCharUuid, sizeof(writeAcceessReqCharAttr.characteristicUUID));
	memcpy((VOIDPTR)&readRandomKeyCharAttr.characteristicUUID, (VOIDPTR)BluetoothPara.btReadChar1Uuid, sizeof(readRandomKeyCharAttr.characteristicUUID));
	memcpy((VOIDPTR)&readStatusCharAttr.characteristicUUID, &BluetoothPara.btReadChar2Uuid, sizeof(readStatusCharAttr.characteristicUUID));
}

static void FormatAdvertisingData(DEVM_Advertising_Information_t *AdvertisingInformation, Byte_t *AdvertisingBuffer)
{
   Byte_t Length;

   /* Verify that the input parameters are semi-valid.                  */
   if((AdvertisingInformation) && (AdvertisingBuffer))
   {
      /* Format the data.                                               */
      Length = (UUID_128_SIZE + NON_ALIGNED_BYTE_SIZE + NON_ALIGNED_WORD_SIZE);
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(AdvertisingBuffer[0]), Length);
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(AdvertisingBuffer[NON_ALIGNED_BYTE_SIZE]), HCI_LE_ADVERTISING_REPORT_DATA_TYPE_SERVICE_DATA);
      SPPLE_ASSIGN_SPPLE_SERVICE_UUID_128(&(AdvertisingBuffer[NON_ALIGNED_WORD_SIZE]));
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(AdvertisingBuffer[18]), 03);
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(AdvertisingBuffer[19]), 10);
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(AdvertisingBuffer[20]), 04);
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(AdvertisingBuffer[21]),HCI_LE_ADVERTISING_REPORT_DATA_TYPE_MANUFACTURER_SPECIFIC);
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(AdvertisingBuffer[22]),0x03);
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(AdvertisingBuffer[23]),0x94);
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(AdvertisingBuffer[24]),0x3);
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(AdvertisingBuffer[25]), (UUID_128_SIZE + NON_ALIGNED_BYTE_SIZE));
      ASSIGN_HOST_BYTE_TO_LITTLE_ENDIAN_UNALIGNED_BYTE(&(AdvertisingBuffer[26]),HCI_LE_ADVERTISING_REPORT_DATA_TYPE_128_BIT_SERVICE_UUID_COMPLETE);
      SPPLE_ASSIGN_SPPLE_SERVICE_UUID_128(&(AdvertisingBuffer[27]));
      /* Format the structure.                                          */
      AdvertisingInformation->AdvertisingDataLength = (Length + NON_ALIGNED_BYTE_SIZE +05+UUID_128_SIZE + NON_ALIGNED_BYTE_SIZE+1);
      AdvertisingInformation->AdvertisingData       = AdvertisingBuffer;
   }
}

static INT32 RegisterService(UINT32 serviceIndex)
{
	CHAR							tmpUuidBuf[256];
	INT32                          	retVal;
	INT32                          	result;
	Boolean_t                      	primaryService;
	GATT_UUID_t                    	uuid;
	UINT32                   		tmpIndex;
	UINT32							numberOfAttributes;
	DESCRIPTOR_INFO_t				*descriptorInfo;
	CHARECTERISTIC_INFO_t			*characteristicInfo;
	GATT_Attribute_Handle_Group_t	serviceHandleRangeResult;
	GATT_Attribute_Handle_Group_t	serviceHandleRange;

	//XXX: serviceIndex = 0 is passed when this function is called.
	serviceIndex = 0;
	serviceTable[serviceIndex].serviceID = 0;
	// Verify that the input parameter is valid.
	// Verify that a GATM Event Callback is registered.
	if(gatmCallbackID)
	{
		// Verify that the service is not already registered.
		if(serviceTable[serviceIndex].serviceID == 0)
		{
			// Calculate the Number of Attributes needed for this service manually.
			numberOfAttributes = 6;
			//CalculateNumberAttributes(&(ServiceTable));

			// Check to see if we need to register a persistent UID.
			if((serviceTable[serviceIndex].flags & SERVICE_TABLE_FLAGS_USE_PERSISTENT_UID)
					&& (serviceTable[serviceIndex].persistentUID == 0))
			{
				// Attempt to register a persistent UID.
				retVal = GATM_RegisterPersistentUID(numberOfAttributes, &(serviceTable[serviceIndex].persistentUID), &serviceHandleRangeResult);
				if(!retVal)
				{
					printf("Registered Persistent UID: [0x%08X].\n", (UINT32)serviceTable[serviceIndex].persistentUID);
					printf("Start Handle:              [0x%04X].\n", serviceHandleRangeResult.Starting_Handle);
					printf("End Handle:                [0x%04X].\n", serviceHandleRangeResult.Ending_Handle);
				}
				else
				{
					printf("Error - GATM_RegisterPersistentUID() [%d] [%s]\n", retVal, ERR_ConvertErrorCodeToString(retVal));
					retVal = FUNCTION_ERROR;
				}
			}
			else
			{
				retVal = 0;
			}

            // Continue only if no error has occurred.
			if(!retVal)
			{
				// Determine if this is a primary service.
				primaryService = (Boolean_t)(!(serviceTable[serviceIndex].flags & SERVICE_TABLE_FLAGS_SECONDARY_SERVICE));

				// Configure the Service UUID.
				// Note:  this sample application only supports registering 128 bit UUIDs,
				// however GATM allows other types as well.
				uuid.UUID_Type = guUUID_128;
				uuid.UUID.UUID_128 = serviceTable[serviceIndex].serviceUUID;

				ConvUUIDToStr(&uuid, tmpUuidBuf);
				printf("Registering Service, UUID:[%s]\n", tmpUuidBuf);

				// Go ahead and attempt to register the service.
				retVal = GATM_RegisterService(primaryService, numberOfAttributes, &uuid, (serviceTable[serviceIndex].persistentUID?&(serviceTable[serviceIndex].persistentUID):NULL));
				if(retVal > 0)
				{
					printf("Registered Service, Service ID: [%u].\n", (UINT32)retVal);

					// Save the Service ID.
					serviceTable[serviceIndex].serviceID = (UINT32)retVal;

					// Loop through all of the attributes and register them.
					retVal = 0;
					for(tmpIndex = 0;(!retVal) && (tmpIndex<serviceTable[serviceIndex].numberAttributes);tmpIndex++)
					{
						// Handle the registration based on the type of attribute.
						switch(serviceTable[serviceIndex].attributeList[tmpIndex].attributeType)
						{
                        	case AT_INCLUDE:
                        	{
                        		// We can only register an include in this application if the previous entry in the
                        		// ServiceTable is already registered.  This is a functioning of the application and
                        		// not the GATM APIs.
                        		retVal = GATM_AddServiceInclude(0, serviceTable[serviceIndex].serviceID, serviceTable[serviceIndex].attributeList[tmpIndex].attributeOffset, serviceTable[serviceIndex].serviceID);
                        		if(!retVal)
                        		{
                        			printf("Registered Include to Service ID %u.\n", serviceTable[serviceIndex].serviceID);
                        		}
                        		else
                        		{
                        			printf("Error - GATM_AddServiceInclude() %d, %s\n", retVal, ERR_ConvertErrorCodeToString(retVal));
                        			retVal = FUNCTION_ERROR;
                        		}
                        	}
                        	break;
                        	case AT_CHARACTERISTIC:
                        	{
                        		// Get a pointer to the characteristic information.
                        		if((characteristicInfo = (CHARECTERISTIC_INFO_t *)serviceTable[serviceIndex].attributeList[tmpIndex].attribute) != NULL)
                        		{
                        			// Configure the Characteristic UUID.
                        			uuid.UUID_Type     = guUUID_128;
                        			uuid.UUID.UUID_128 = characteristicInfo->characteristicUUID;

                        			// Attempt to add this characteristic to the table.
                        			retVal = GATM_AddServiceCharacteristic(serviceTable[serviceIndex].serviceID, serviceTable[serviceIndex].attributeList[tmpIndex].attributeOffset, characteristicInfo->characteristicPropertiesMask, characteristicInfo->securityPropertiesMask, &uuid);
                        			if(!retVal)
                        			{
                        				ConvUUIDToStr(&uuid, tmpUuidBuf);
                        				printf("Registering Service, UUID:[%s]\n", tmpUuidBuf);
                        			}
                        			else
                        			{
                        				printf("Error - GATM_AddServiceCharacteristic() %d, %s\n", retVal, ERR_ConvertErrorCodeToString(retVal));
                        				retVal = FUNCTION_ERROR;
                        			}
                        		}
                        	}
                        	break;
                        	case AT_DESCRIPTOR:
                        	{
                        		// Get a pointer to the descriptor information.
                        		if((descriptorInfo = (DESCRIPTOR_INFO_t *)serviceTable[serviceIndex].attributeList[tmpIndex].attribute) != NULL)
                        		{
                        			// Configure the Descriptor UUID.
                        			uuid.UUID_Type     = guUUID_128;
                        			uuid.UUID.UUID_128 = descriptorInfo->characteristicUUID;

                        			// Attempt to add this descriptor to the table.
                        			retVal = GATM_AddServiceDescriptor(serviceTable[serviceIndex].serviceID, serviceTable[serviceIndex].attributeList[tmpIndex].attributeOffset, descriptorInfo->descriptorPropertiesMask, descriptorInfo->securityPropertiesMask, &uuid);
                        			if(!retVal)
                        			{
                        				ConvUUIDToStr(&uuid, tmpUuidBuf);
                        				printf("Registering Service, UUID:[%s]\n", tmpUuidBuf);
                        			}
                        			else
                        			{
                        				printf("Error - GATM_AddServiceDescriptor() %d, %s\n", retVal, ERR_ConvertErrorCodeToString(retVal));
                        				retVal = FUNCTION_ERROR;
                        			}
                        		}
                        	}
                        	break;
						}
					}

					// If no error occurred go ahead and publish the service.
					if(!retVal)
					{
						// Attempt to register the Service into the GATT database.
						if((retVal = GATM_PublishService(serviceTable[serviceIndex].serviceID, gatmCallbackID, (GATM_SERVICE_FLAGS_SUPPORT_LOW_ENERGY), &serviceHandleRange)) == 0)
						{
							// Store the Published Handle Range for this service.
							serviceTable[serviceIndex].serviceHandleRange = serviceHandleRange;

							printf(" Service Registered.\n");
							printf(" Service Start Handle: 0x%04X\n", serviceHandleRange.Starting_Handle);
							printf(" Service End Handle:   0x%04X\n", serviceHandleRange.Ending_Handle);

							// Print the Handles of all of the attributes in the table.
							for(tmpIndex=0;tmpIndex<serviceTable[serviceIndex].numberAttributes;tmpIndex++)
							{
								switch(serviceTable[serviceIndex].attributeList[tmpIndex].attributeType)
								{
									case AT_INCLUDE:
									{
										printf("Include Attribute @ Handle: 0x%04X\n", (serviceHandleRange.Starting_Handle + serviceTable[serviceIndex].attributeList[tmpIndex].attributeOffset));
									}
									break;
									case AT_CHARACTERISTIC:
									{
										// Get a pointer to the characteristic information.
										if((characteristicInfo = (CHARECTERISTIC_INFO_t *)serviceTable[serviceIndex].attributeList[tmpIndex].attribute) != NULL)
										{
											// Configure the Characteristic UUID.
											uuid.UUID_Type     = guUUID_128;
											uuid.UUID.UUID_128 = characteristicInfo->characteristicUUID;
											ConvUUIDToStr(&uuid, tmpUuidBuf);
											printf("Registering Service, UUID:[%s]\n", tmpUuidBuf);
											printf("Characteristic Declaration @ Handle: 0x%04X\n", (serviceHandleRange.Starting_Handle + serviceTable[serviceIndex].attributeList[tmpIndex].attributeOffset));
											printf("Characteristic Value       @ Handle: 0x%04X\n", (serviceHandleRange.Starting_Handle + serviceTable[serviceIndex].attributeList[tmpIndex].attributeOffset + 1));
										}
									}
									break;
									case AT_DESCRIPTOR:
									{
										// Get a pointer to the descriptor information.
										if((descriptorInfo = (DESCRIPTOR_INFO_t *)serviceTable[serviceIndex].attributeList[tmpIndex].attribute) != NULL)
										{
											// Configure the Descriptor UUID.
											uuid.UUID_Type     = guUUID_128;
											uuid.UUID.UUID_128 = descriptorInfo->characteristicUUID;
											ConvUUIDToStr(&uuid, tmpUuidBuf);
											printf("Registering Service, UUID:[%s]\n", tmpUuidBuf);
											printf("Characteristic Descriptor @ Handle: 0x%04X\n", (serviceHandleRange.Starting_Handle + serviceTable[serviceIndex].attributeList[tmpIndex].attributeOffset));
										}
									}
									break;
								}
							}
						}
						else
						{
							printf("Error - GATM_PublishService() %d, %s\n", retVal, ERR_ConvertErrorCodeToString(retVal));
							retVal = FUNCTION_ERROR;
						}
					}

					// Check to see if we need to delete the registered
					// service if an error occcurred.
					if(retVal)
					{
						// If an error occurred go ahead and delete the service.
						result = GATM_DeleteService(serviceTable[serviceIndex].serviceID);
						if(result)
						{
							printf("Error - GATM_DeleteService() %d, %s\n", result, ERR_ConvertErrorCodeToString(result));
						}

						// Flag that this service is not registered.
						serviceTable[serviceIndex].serviceID = 0;
					}
				}
				else
				{
					printf("Error - GATM_RegisterService() %d, %s\n", retVal, ERR_ConvertErrorCodeToString(retVal));
					retVal = FUNCTION_ERROR;
				}
			}
		}
		else
		{
			printf("Service already registered.\n");
			retVal = FUNCTION_ERROR;
		}
	}
	else
	{
		// Callback not registered, go ahead and notify the user.
		printf("Generic Attribute Profile Manager Event Callback is not registered.\n");
		retVal = FUNCTION_ERROR;
	}
	return(retVal);
}

static INT32 UnRegisterService(UINT32 serviceIndex)
{
	INT32 	result;
	INT32	retVal;

	// Verify that the input parameter is valid.
	if(serviceIndex < NUMBER_OF_SERVICES)
	{
		// Verify that a GATM Event Callback is registered.
		if(gatmCallbackID)
		{
			// Verify that the service is already registered.
			if(serviceTable[serviceIndex].serviceID != 0)
			{
				// Go ahead and attempt to delete the service.
				if((result = GATM_DeleteService(serviceTable[serviceIndex].serviceID)) == 0)
				{
					printf("Service ID [%u], successfully un-registered.\n", serviceTable[serviceIndex].serviceID);
					serviceTable[serviceIndex].serviceID = 0;
				}
				else
				{
					printf("Error - GATM_DeleteService() [%d], [%s]\n", result, ERR_ConvertErrorCodeToString(result));
					retVal = FUNCTION_ERROR;
				}
			}
			else
			{
				printf("Service not registered.\n");
				retVal = FUNCTION_ERROR;
			}
		}
		else
		{
			// Callback not registered, go ahead and notify the user.
			printf("Generic Attribute Profile Manager Event Callback is not registered.\n");
			retVal = FUNCTION_ERROR;
		}
	}
	else
	{
		printf("Invalid Service Index [%u], Maximum [%u].\n", serviceIndex, (NUMBER_OF_SERVICES-1));
		// One or more of the necessary parameters is/are invalid.
		retVal = INVALID_PARAMETERS_ERROR;
	}

	return(retVal);
}

VOID RunBluetoothTask(VOID)
{
	// if ReceiveDone is set in writeCharCommonBuff that means message request is
	// pending and we have to process this message when door in idle mode.
	if(ReceiveDone == TRUE)
	{
		printf("Found one BT Access Request Pending\n");

		// Send Notification, Status Response is ready to read
		GATM_SendHandleValueNotification(serviceTable[0].serviceID, CurrentRemoteBD_ADDR, 5, sizeof(ReadStsResp), (Byte_t *)&ReadStsResp);
		printf("Notification send to mobile device ReadStsResp=[%u]\n", ReadStsResp);
		ClearWriteCharCommonBuff();
		
		// Reload BtSession Timer with 500 msec
		btReadReqTimerF = TRUE;
	}
}

VOID ClearWriteCharCommonBuff(VOID)
{
	ReceiveDone = FALSE;
}

//***************************************************************************
//	DEVM_Event_Callback()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	Success / fail
//	Description: The following function is the Device Manager Event Callback
// 				 function that is Registered with the Device Manager.This
// 				 is responsible for processing all Device Manager Events.
//**************************************************************************
static VOID BTPSAPI DEVM_Event_Callback(DEVM_Event_Data_t *eventData, VOID *callbackParameter)
{
	if(eventData)
	{
		switch(eventData->EventType)
		{
			case detDevicePoweredOn:
			{
				printf("BT Device Powered On.\n");
			}
			break;
			case detDevicePoweringOff:
			{
				printf("BT Device Powering Off Event, Timeout:[0x%08X].\n", eventData->EventData.DevicePoweringOffEventData.PoweringOffTimeout);
			}
			break;
			case detDevicePoweredOff:
			{
				printf("BT Device Powered Off.\n");
			}
			break;
			case detLocalDevicePropertiesChanged:
			{
				if(eventData->EventData.LocalDevicePropertiesChangedEventData.LocalDeviceProperties.ConnectableMode != YES)
				{
					StartBtAdvertisement();
				}
				printf("BT Local Device Properties Changed.\n");

				// Fill BLE MAC Address into Config, Do not write into flash it is temporary.
				BdAddrToStr(eventData->EventData.LocalDevicePropertiesChangedEventData.LocalDeviceProperties.BLEBD_ADDR, BluetoothPara.btMacAddress);
			}
			break;
			case detDeviceScanStarted:
			{
				printf("LE Device Discovery Started.\n");
			}
			break;
			case detDeviceScanStopped:
			{
				printf("LE Device Discovery Stopped.\n");
			}
			break;
			case detDeviceAdvertisingStarted:
			{
				btAdvertised = TRUE;
				printf("LE Advertising Started.\n");
			}
			break;
			case detDeviceAdvertisingStopped:
			{
				btAdvertised = FALSE;
				printf("LE Advertising Stopped.\n");
			}
			break;
			case detAdvertisingTimeout:
			{
				printf("LE Advertising Timeout.\n");
				StartBtAdvertisement();
			}
			break;
			case detDeviceDiscoveryStarted:
			{
				printf("BT Device Discovery Started.\n");
			}
			break;
			case detDeviceDiscoveryStopped:
			{
				printf("BT Device Discovery Stopped.\n");
			}
			break;
			case detRemoteDeviceFound:
			{
				printf("BT Remote Device Found.\n");
			}
			break;
			case detRemoteDeviceDeleted:
			{
				printf("BT Remote Device Deleted.\n");
			}
			break;
			case detRemoteDevicePropertiesChanged:
			{
				printf("BT Remote Device Properties Changed.\n");
			}
			break;
			case detRemoteDeviceAddressChanged:
			{
				printf("BT Remote Device Address Changed.\n");
			}
			break;
			case detRemoteDevicePropertiesStatus:
			{
				printf("BT Remote Device Properties Status:[%s].\n", eventData->EventData.RemoteDevicePropertiesStatusEventData.Success?"SUCCESS":"FAILURE");
			}
			break;
			case detRemoteDeviceServicesStatus:
			{
				printf("BT Remote Device Services Status:[%s] [%s].\n"
						,(eventData->EventData.RemoteDeviceServicesStatusEventData.StatusFlags & DEVM_REMOTE_DEVICE_SERVICES_STATUS_FLAGS_LOW_ENERGY)?"LE":"BR/EDR"
						,(eventData->EventData.RemoteDeviceServicesStatusEventData.StatusFlags & DEVM_REMOTE_DEVICE_SERVICES_STATUS_FLAGS_SUCCESS)?"SUCCESS":"FAILURE");
			}
			break;
			case detRemoteDevicePairingStatus:
			{
				printf("[%s] BT Remote Device Pairing Status:[%s] [0x%02X]\n"
						, ((eventData->EventData.RemoteDevicePairingStatusEventData.AuthenticationStatus & DEVM_REMOTE_DEVICE_PAIRING_STATUS_FLAGS_LOW_ENERGY)?"LE":"BR/EDR")
						, (eventData->EventData.RemoteDevicePairingStatusEventData.Success)?"SUCCESS":"FAILURE"
						, eventData->EventData.RemoteDevicePairingStatusEventData.AuthenticationStatus);
			}
			break;
			case detRemoteDeviceAuthenticationStatus:
			{
				printf("BT Remote Device Authentication Status:[%d] [%s]\n"
						, eventData->EventData.RemoteDeviceAuthenticationStatusEventData.Status
						, (eventData->EventData.RemoteDeviceAuthenticationStatusEventData.Status)?ERR_ConvertErrorCodeToString(eventData->EventData.RemoteDeviceAuthenticationStatusEventData.Status):"SUCCESS");
			}
			break;
			case detRemoteDeviceEncryptionStatus:
			{
				printf("BT Remote Device Encryption Status:[%d] [%s]\n"
						, eventData->EventData.RemoteDeviceEncryptionStatusEventData.Status
						, (eventData->EventData.RemoteDeviceEncryptionStatusEventData.Status)?ERR_ConvertErrorCodeToString(eventData->EventData.RemoteDeviceEncryptionStatusEventData.Status):"SUCCESS");
			}
			break;
			case detRemoteDeviceConnectionStatus:
			{
				printf("BT Remote Device Connection Status: [%d] [%s]\n"
						, eventData->EventData.RemoteDeviceConnectionStatusEventData.Status
						, (eventData->EventData.RemoteDeviceConnectionStatusEventData.Status)?ERR_ConvertErrorCodeToString(eventData->EventData.RemoteDeviceConnectionStatusEventData.Status):"SUCCESS");
			}
			break;
			default:
			{
				printf("Unknown BT Device Manager Event Received: [0x%08X], Length: [0x%08X].\n", (UINT32)eventData->EventType, eventData->EventLength);
			}
			break;
		}
	}
	else
	{
		printf("BT DEVM Event Data is NULL.\n");
	}
}

//***************************************************************************
//	SearchServiceListByOffset()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	Success / fail
//	Description:
// 			The following function is used to search the Service Info list to
// 			return an attribute based on the ServiceID and the attributeOffset.
// 			This function returns a pointer to the attribute
// 			or NULL if not found.
//**************************************************************************
static ATTRIBUTE_INFO_t *SearchServiceListByOffset(UINT32 serviceID, UINT32 attributeOffset)
{
	UINT32		tmpServiceIndex;
	UINT32		tmpAttrIndex;
	ATTRIBUTE_INFO_t *attributeInfo = NULL;

	// Verify that the input parameters are semi-valid.
	if((serviceID) && (attributeOffset))
	{
		//Loop through the services and find the correct attribute.
		for(tmpServiceIndex = 0;(attributeInfo == NULL) && (tmpServiceIndex < NUMBER_OF_SERVICES);tmpServiceIndex++)
		{
			if(serviceTable[tmpServiceIndex].serviceID == serviceID)
			{
				// Loop through the attribute list for this service and
				// locate the correct attribute based on the Attribute Offset.
				for(tmpAttrIndex = 0;(attributeInfo == NULL) && (tmpAttrIndex < serviceTable[tmpServiceIndex].numberAttributes);tmpAttrIndex++)
				{
					if(serviceTable[tmpServiceIndex].attributeList[tmpAttrIndex].attributeOffset == attributeOffset)
					{
						attributeInfo = &(serviceTable[tmpServiceIndex].attributeList[tmpAttrIndex]);
					}
				}
			}
		}
	}
	return(attributeInfo);
}

//***************************************************************************
//	ProcessWriteRequestEvent()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	Success / fail
//	Description:
// 			The following function is a utility function which is used to
// 			process a GATM Write Request Event.
//**************************************************************************
static VOID ProcessWriteRequestEvent(GATM_Write_Request_Event_Data_t *writeRequestData)
{
	INT32				result;
	Byte_t**			valuePtr;
	Byte_t				errorCode;
	Boolean_t*			allocatedMemoryPtr;
	UINT32PTR			valueLengthPtr;
	UINT32				maximumValueLength;
	ATTRIBUTE_INFO_t*	attributeInfoPtr;
	UINT16				tmpLen;

	// Verify that the input parameter is valid.
	if(writeRequestData)
	{
		// Initialize the error code.
		errorCode = 0;

		// Wait on access to the Service Info List Mutex.
		if(BTPS_WaitMutex(btServiceMutex, BTPS_INFINITE_WAIT))
		{
			// Search for the Attribute for this write request
			// (which must be a characteristic or descriptor).
			attributeInfoPtr = SearchServiceListByOffset(writeRequestData->ServiceID, writeRequestData->AttributeOffset);
			if((attributeInfoPtr) && ((attributeInfoPtr->attributeType == AT_CHARACTERISTIC) || (attributeInfoPtr->attributeType == AT_DESCRIPTOR)))
			{
				// Handle the read based on the type of attribute.
				switch(attributeInfoPtr->attributeType)
				{
					case AT_CHARACTERISTIC:
					{
						maximumValueLength	= ((CHARECTERISTIC_INFO_t *)attributeInfoPtr->attribute)->maximumValueLength;
						allocatedMemoryPtr	= &(((CHARECTERISTIC_INFO_t *)attributeInfoPtr->attribute)->allocatedValue);
						valueLengthPtr		= &(((CHARECTERISTIC_INFO_t *)attributeInfoPtr->attribute)->valueLength);
						valuePtr			= &(((CHARECTERISTIC_INFO_t *)attributeInfoPtr->attribute)->value);
					}
					break;
					case AT_DESCRIPTOR:
					{
						maximumValueLength	= ((DESCRIPTOR_INFO_t *)attributeInfoPtr->attribute)->maximumValueLength;
						allocatedMemoryPtr	= &(((DESCRIPTOR_INFO_t *)attributeInfoPtr->attribute)->allocatedValue);
						valueLengthPtr		= &(((DESCRIPTOR_INFO_t *)attributeInfoPtr->attribute)->valueLength);
						valuePtr			= &(((DESCRIPTOR_INFO_t *)attributeInfoPtr->attribute)->value);
					}
					break;
					default:
					{
						// Do nothing.
					}
					break;
				}

				// Verify that the length of the write is less than the
				// maximum length of this characteristic.
				if(writeRequestData->DataLength <= maximumValueLength)
				{
					// Free any previously existing buffer.
					if((*allocatedMemoryPtr) && (*valuePtr))
					{
						BTPS_FreeMemory(*valuePtr);
						*allocatedMemoryPtr = FALSE;
						*valueLengthPtr     = 0;
						*valuePtr           = NULL;
					}

					// Allocate a buffer to hold this new value
					// (note we will always allocate a worst case buffer for this attribute
					// to make handling prepare writes later easier).
					if((writeRequestData->DataLength == 0) || ((*valuePtr = (Byte_t *)BTPS_AllocateMemory(maximumValueLength)) != NULL))
					{
						// Store the information on the write.
						if(writeRequestData->DataLength)
						{
							// Copy the value over into the allocated buffer.
							BTPS_MemCopy(*valuePtr, writeRequestData->Data, writeRequestData->DataLength);

							// Save the Value Length and flag that a buffer is
							// allocated for this attribute.
							*allocatedMemoryPtr = TRUE;
							*valueLengthPtr     = writeRequestData->DataLength;

							if(ReceiveDone == FALSE)
							{
								CHAR buf[32];
								if(writeRequestData->DataLength < 32)
								{
									snprintf(buf, (writeRequestData->DataLength + 1), "%s", writeRequestData->Data);
								}
								else
								{
									snprintf(buf, 32, "%s", writeRequestData->Data);
								}

								printf("Write Request recived, Data=[%s]\n", buf);
								ReceiveDone = TRUE;
								printf("set ReceiveDone = [%u]\n", ReceiveDone);
							}
						}
						else
						{
							// Simply reset the state since this is a 0 byte write.
							*allocatedMemoryPtr = FALSE;
							*valueLengthPtr     = 0;
							*valuePtr           = NULL;
						}

						// If we need to respond to this request go ahead and do so.
						if(writeRequestData->RequestID)
						{
							if((result = GATM_WriteResponse(writeRequestData->RequestID)) == 0)
							{
								printf("GATM_WriteResponse() success.\n");
								//GATM_SendHandleValueNotification(serviceTable[0].serviceID, CurrentRemoteBD_ADDR, 5, sizeof(READ_STATUS_CHAR_RESP_t), (Byte_t *)&readStatusResp);
							}
							else
							{
								printf("Error - GATM_WriteResponse() [%d], [%s]\n", result, ERR_ConvertErrorCodeToString(result));
							}
						}
					}
					else
					{
						errorCode = ATT_PROTOCOL_ERROR_CODE_INSUFFICIENT_RESOURCES;
					}
				}
				else
				{
					errorCode = ATT_PROTOCOL_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LENGTH;
				}
			}
			else
			{
				errorCode = ATT_PROTOCOL_ERROR_CODE_INVALID_HANDLE;
			}

			// Release the mutex that was previously acquired.
			BTPS_ReleaseMutex(btServiceMutex);
		}
		else
		{
			// Simply send an error response.
			errorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
		}

		// If requested go ahead and issue an error response to the
		// request (if a response is required for this request).
		if((errorCode) && (writeRequestData->RequestID))
		{
			if((result = GATM_ErrorResponse(writeRequestData->RequestID, errorCode)) == 0)
			{
				printf("GATM_ErrorResponse() success.\n");
			}
			else
			{
				printf("Error - GATM_ErrorResponse() [%d], [%s]\n", result, ERR_ConvertErrorCodeToString(result));
			}
		}
	}
}

//***************************************************************************
//	ProcessReadRequestEvent()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	Success / fail
//	Description:
//		The following function is a utility function which is used to
//		process a GATM Read Request Event.
//**************************************************************************
static VOID ProcessReadRequestEvent(GATM_Read_Request_Event_Data_t *readRequestData)
{
	INT32				result;
	Byte_t          	*valuePtr;
	Byte_t         		errorCode;
	UINT32				valueLength;
	ATTRIBUTE_INFO_t 	*attributeInfoPtr;

	// Verify that the input parameter is semi-valid.
	if(readRequestData)
	{
		// Initialize the error code.
		errorCode = 0;

		// Wait on access to the Service Info List Mutex.
		if(BTPS_WaitMutex(btServiceMutex, BTPS_INFINITE_WAIT))
		{
			// if read request is received on readRandomKeyCharAttr then generate
			// random session key.
			if((readRequestData->AttributeOffset == READ_RANDOM_KEY_CHAR_OFFSET) && (readRequestData->AttributeValueOffset == 0) )
			{
				//randomKey is allready written.
				// New Request start
				printf("Read Data request received, Response = [%s]\n", ReadRandResp);
			}

			// Search for the Attribute for this read request
			// (which must be a characteristic or descriptor).
			printf("readRequestData->AttributeOffset[%u]\n",readRequestData->AttributeOffset);
			attributeInfoPtr = SearchServiceListByOffset(readRequestData->ServiceID, readRequestData->AttributeOffset);
			if((attributeInfoPtr) && ((attributeInfoPtr->attributeType == AT_CHARACTERISTIC) || (attributeInfoPtr->attributeType == AT_DESCRIPTOR)) && (attributeInfoPtr->attribute))
			{
				// Handle the read based on the type of attribute.
				switch(attributeInfoPtr->attributeType)
				{
					case AT_CHARACTERISTIC:
					{
						valueLength 	= ((CHARECTERISTIC_INFO_t *)attributeInfoPtr->attribute)->valueLength;
						valuePtr       	= ((CHARECTERISTIC_INFO_t *)attributeInfoPtr->attribute)->value;
					}
					break;
					case AT_DESCRIPTOR:
					{
						valueLength 	= ((DESCRIPTOR_INFO_t *)attributeInfoPtr->attribute)->valueLength;
						valuePtr       	= ((DESCRIPTOR_INFO_t *)attributeInfoPtr->attribute)->value;
					}
					break;
					default:
					{
						// Do nothing.
					}
					break;
				}

				// Next check the requested offset of the read versus the length.
				printf("readRequestData->AttributeValueOffset[%u]\n",readRequestData->AttributeValueOffset);
				printf("valueLength[%u]\n",valueLength);
				printf("valuePtr[%x]\n",valuePtr);
				if(readRequestData->AttributeValueOffset <= valueLength)
				{
					// Verify that the length and pointer is valid.
					if((valueLength) && (valuePtr))
					{
						// Calculate the length of the value from the specified offset.
						valueLength -= readRequestData->AttributeValueOffset;

						// Get a pointer to the value at the specified offset.
						if(valueLength)
						{
							valuePtr     = &(valuePtr[readRequestData->AttributeValueOffset]);
						}
						else
						{
							valuePtr     = NULL;
							printf("Read offset out of bound\n");
						}
					}
					else
					{
						printf("Error - No data available to read\n");
						valueLength = 0;
					}

					// Go ahead and respond to the read request.
					if((result = GATM_ReadResponse(readRequestData->RequestID, valueLength, valuePtr)) == 0)
					{
						printf("GATM_ReadResponse() success. valueLength[%u]\n", valueLength);
					}
					else
					{
						printf("Error - GATM_ReadResponse() [%d] [%s]\n", result, ERR_ConvertErrorCodeToString(result));
					}
				}
				else
				{
					errorCode = ATT_PROTOCOL_ERROR_CODE_INVALID_OFFSET;
					printf("Error - ATT_PROTOCOL_ERROR_CODE_INVALID_OFFSET\n");
				}
			}
			else
			{
				errorCode = ATT_PROTOCOL_ERROR_CODE_INVALID_HANDLE;
				printf("Error - ATT_PROTOCOL_ERROR_CODE_INVALID_HANDLE\n");
			}

			// Release the mutex that was previously acquired.
			BTPS_ReleaseMutex(btServiceMutex);
		}
		else
		{
			// Simply send an error response.
			errorCode = ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR;
			printf("Error - ATT_PROTOCOL_ERROR_CODE_UNLIKELY_ERROR\n");
		}

		// If requested go ahead and issue an error response to the request.
		if(errorCode)
		{
			if((result = GATM_ErrorResponse(readRequestData->RequestID, errorCode)) == 0)
			{
				printf("GATM_ErrorResponse() success.\n");
			}
			else
			{
				printf("Error - GATM_ErrorResponse() [%d] [%s]\n", result, ERR_ConvertErrorCodeToString(result));
			}
		}
	}
}

//***************************************************************************
//	GATM_Event_Callback()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	Success / fail
//	Description:
// 		The following function is the GATM Event Callback function that is
// 		Registered with the Generic Attribute Profile Manager.  This
// 		callback is responsible for processing all GATM Events.
//**************************************************************************
static VOID BTPSAPI GATM_Event_Callback(GATM_Event_Data_t *eventData, VOID *callbackParameter)
{
	if(eventData)
	{
		switch(eventData->EventType)
		{
			// GATM Connection Events.
			case getGATTConnected:
			{
				printf("GATT Connection Event\n");
				CurrentRemoteBD_ADDR = eventData->EventData.ReadRequestData.RemoteDeviceAddress;
				btConnected = TRUE;
				btAdvertised = FALSE;
				btReadReqTimerF = FALSE;
				prevSysTick = GetSysTick();
			}
			break;
			case getGATTDisconnected:
			{
				printf("GATT Disconnected Event\n");
				btConnected = FALSE;
				// If connection is closed then do not process any waiting write request of that user
				// so clear Write Char CommonBuff.
				ClearWriteCharCommonBuff();
				StartBtAdvertisement();
			}
			break;
			case getGATTConnectionMTUUpdate:
			{
				btConnected = TRUE;
				btAdvertised = FALSE;
				printf("GATT MTU Update Event\n");
			}
			break;
			case getGATTHandleValueData:
			{
				printf("GATT Handle Value Data Event\n");
			}
			break;

			// GATM Server Events.
			case getGATTWriteRequest:
			{
				btConnected = TRUE;
				btAdvertised = FALSE;
				printf("GATT Write Request Event\n");
				ProcessWriteRequestEvent(&(eventData->EventData.WriteRequestData));
			}
			break;
			case getGATTSignedWrite:
			{
				printf("GATT Signed Write Event\n");
			}
			break;
			case getGATTReadRequest:
			{
				btConnected = TRUE;
				btAdvertised = FALSE;
				printf("GATT Read Request Event\n");
				// Go ahead and process the Read Request.
				ProcessReadRequestEvent(&(eventData->EventData.ReadRequestData));
				
				// if read request is received on readStatusCharAttr then after
				// 500 msec if client does not close the connection then close it.
				if(eventData->EventData.ReadRequestData.AttributeOffset == READ_STATUS_CHAR_OFFSET)
				{
					prevSysTick = GetSysTick();
					// Reload BtSession Timer with 500 msec
					btReadReqTimerF = TRUE;
				}
			}
			break;
			case getGATTPrepareWriteRequest:
			{
				printf("GATT Prepare Write Request Event\n");
			}
			break;
			case getGATTCommitPrepareWrite:
			{
				printf("GATT Commit Prepare Write Event\n");
			}
			break;
			case getGATTHandleValueConfirmation:
			{
				printf("GATT Handle-Value Confirmation Event\n");
			}
			break;
			default:
			{
				printf("GATT Unhandled event.\n");
			}
			break;
		}
	}
	else
	{
		printf("GATM Event Data is NULL.\n");
	}
}

//***************************************************************************
//	StartBtAdvertisement()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	Set/Reset the flag btAdvertised.
//	Description:
//			This function will start Bluetooth Advertisement and set/reset
//			the btAdvertised Flag.
//**************************************************************************
static BOOL StartBtAdvertisement(VOID)
{
	Byte_t  AdvertisingBuffer[64] = {0};
	INT32  	result = 0;

	memset(&AdvertisingBuffer, 0, sizeof(AdvertisingBuffer));
	FormatAdvertisingData(&advertisingInfo, AdvertisingBuffer);
	//Starts advertising. input:- Above given paramters.
	if((result = DEVM_StartAdvertising(&advertisingInfo)) == 0)
	{
		btAdvertised = TRUE;
		startAdvtF = FALSE;
		printf("DEVM_StartAdvertising() Success: Duration %lu seconds.\n", advertisingInfo.AdvertisingDuration);
	}
	else
	{
		btAdvertised = FALSE;
		printf("DEVM_StartAdvertising() Failure: %d, %s.\n", result, ERR_ConvertErrorCodeToString(result));
		return FAIL;
	}

	return SUCCESS;
}

//***************************************************************************
//	ConvUUIDToStr()
//	Param:
//		IN 	:	UUID
//			:	strPtr
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
// 		The following function is a utility function that is used to convert
//		UUID to string for Display purpose.
//**************************************************************************
static VOID ConvUUIDToStr(GATT_UUID_t *uuid, CHARPTR strPtr)
{
	if((uuid != NULL) && (strPtr != NULL))
	{
		if(uuid->UUID_Type == guUUID_16)
		{
			sprintf(strPtr, "%02X%02X", uuid->UUID.UUID_16.UUID_Byte1, uuid->UUID.UUID_16.UUID_Byte0);
		}
		else
		{
			if(uuid->UUID_Type == guUUID_128)
			{
				sprintf(strPtr, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
						uuid->UUID.UUID_128.UUID_Byte15, uuid->UUID.UUID_128.UUID_Byte14, uuid->UUID.UUID_128.UUID_Byte13,
						uuid->UUID.UUID_128.UUID_Byte12, uuid->UUID.UUID_128.UUID_Byte11, uuid->UUID.UUID_128.UUID_Byte10,
						uuid->UUID.UUID_128.UUID_Byte9,  uuid->UUID.UUID_128.UUID_Byte8,  uuid->UUID.UUID_128.UUID_Byte7,
						uuid->UUID.UUID_128.UUID_Byte6,  uuid->UUID.UUID_128.UUID_Byte5,  uuid->UUID.UUID_128.UUID_Byte4,
						uuid->UUID.UUID_128.UUID_Byte3,  uuid->UUID.UUID_128.UUID_Byte2,  uuid->UUID.UUID_128.UUID_Byte1,
						uuid->UUID.UUID_128.UUID_Byte0);
			}
		}
	}
}

//*****************************************************************************
//	BdAddrToStr()
//	Param:
//		IN:	NONE
//		OUT:NONE
//	Returns:
//		NONE
//	Description:
//		This function print Bluetooth Address to String.
//*****************************************************************************
VOID BdAddrToStr(BD_ADDR_t Board_Address, CHARPTR BoardStr)
{
	sprintf(BoardStr, "%02X:%02X:%02X:%02X:%02X:%02X", Board_Address.BD_ADDR5,
												  	   Board_Address.BD_ADDR4,
												  	   Board_Address.BD_ADDR3,
												  	   Board_Address.BD_ADDR2,
												  	   Board_Address.BD_ADDR1,
												  	   Board_Address.BD_ADDR0);
}

//***************************************************************************
//	DisconnectRemoteDevice()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	Success / fail
//	Description:
//			This function will Disconnect Remote Device using
// 			address stored in CurrentRemoteBD_ADDR.
//**************************************************************************
VOID DisconnectRemoteDevice(VOID)
{
	INT32		result;
	UINT32 		DisconnectFlags;

	DisconnectFlags = DEVM_DISCONNECT_FROM_REMOTE_DEVICE_FLAGS_LOW_ENERGY | DEVM_DISCONNECT_FROM_REMOTE_DEVICE_FLAGS_FORCE;

	if((result = DEVM_DisconnectRemoteDevice(CurrentRemoteBD_ADDR, DisconnectFlags)) >= 0)
	{
		btConnected = FALSE;
		printf("DEVM_DisconnectRemoteDevice() : Success: %d. \n", result);
	}
	else
	{
		btConnected = FALSE;
		// Error Disconnecting Remote Device, inform the user and flag an error.
		printf("DEVM_DisconnectRemoteDevice() : Failure: %d, %s. \n", result, ERR_ConvertErrorCodeToString(result));
	}
	// Clear if any current remote device request is pending.
	ClearWriteCharCommonBuff();
}

//*****************************************************************************
//	GetSysTick()
//	Param: 
//		IN :	None 
//		OUT:	None
//	Returns:	sysTick - Tick count.
//	Description:
//				This function returns current system Tick count. 
//				This function is useful when timing less then 100ms is required.
//*****************************************************************************
UINT32 GetSysTick(void)
{
	//struct timeval	curTime;
	UINT64 				currMsecCnt;
	struct timespec		timeVal;

	//this function read continuous increasing time kernel and it has no relation with realtime in
	//kernel so event though we change time from application it doesn't get affected.
	if(clock_gettime(CLOCK_MONOTONIC, &timeVal) == 0)
	{
		currMsecCnt = (timeVal.tv_sec * 1000) + (timeVal.tv_nsec / 1000000);
		sysTick = (currMsecCnt % 0xFFFFFFFF);
	}
	else
	{
		printf("clock_gettime failed\n");
	}
	return(sysTick);
}

//*****************************************************************************
//	ElapsedTick()
//	Param: 
//		IN :	markTick - tick count with which to check
//		OUT:	None
//	Returns:	elasped tick 
//	Description:
//				Returns difference between current system tick count and 
//				specified tick. Application can use this function to find 
//				elapsed tick since the last mark.
//*****************************************************************************
UINT32 ElapsedTick(UINT32 markTick)
{
	// First, update system tick
	GetSysTick();
	return(( (sysTick < markTick) ? (sysTick + (0xFFFFFFFF - markTick)) : (sysTick - markTick) ));
}
