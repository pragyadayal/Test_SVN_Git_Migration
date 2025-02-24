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
//   Created By   : Niraj Usadadiya
//   File         : MxBluetoothConfig.c
//   Description  : This file consist of Bluetooth Configuration File I/O
//					related Functions
/////////////////////////////////////////////////////////////////////////////
#include "MxDeviceSpecific.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>

#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxConfigLog.h"
#include "MxBluetoothConfig.h"
#include "MxLogs.h"
#include "MxBluetooth.h"
#ifdef OEM_SUPPORTED
#include "OEM.h"
#endif //#ifdef OEM_SUPPORTED

//******** Extern Variables **********
#ifdef OEM_SUPPORTED
extern OEM_INFO_t gOEMCurrDeviceInfo;
#endif //#ifdef OEM_SUPPORTED


//******** Defines and Data Types ****


//******** Function Prototypes ******

//******** Global Variables **********
BLUETOOTH_CONFIG_t			BluetoothPara;
const BLUETOOTH_CONFIG_t	DfltBluetoothPara =
{
		.btEnable			= DISABLED,
		.btMacAddress 		= "",
		.btName				= "MATRIX",
		.btServiceUuid  				= { 0x07, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC},
		.btWriteCharUuid 				= { 0x08, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC},
		.btReadChar1Uuid 				= { 0x09, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC},
		.btReadChar2Uuid 				= { 0x0A, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC},
		.btAcsReadRandomNoCharUuid 		= { 0x0B, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC},
		.btAcsReadAccessIdCharUuid 		= { 0x0C, 0x89, 0x1A, 0xCA, 0xFB, 0xCA, 0xEF, 0xBD, 0xAA, 0x63, 0x08, 0x76, 0x0D, 0x8A, 0xE1, 0xBC},
		.btRange						= 1,
		.btCustomeRangeStrength         = -30,
};

//******** Static Variables **********

//******** Function Definations ******
//*****************************************************************************
//	InitBluetoothConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialise Bluetooth Configuration variables.
//*****************************************************************************
void InitBluetoothConfig(void)
{
	if(SUCCESS != ReadBluetoothConfig() )
	{
		if(SUCCESS != LoadDfltBluetoothConfig() )
		{
			EPRINT(SYS_LOG, "Failed to initialise Bluetooth Configuration variables");
		}
	}
}

//*****************************************************************************
//	ReadBluetoothConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Bluetooth Configuration stored in file.
//*****************************************************************************
BOOL ReadBluetoothConfig(void)
{
	INT32 	fd;
	UINT8	ver;
	BOOL 	status;
	fd = open(SYS_BLUETOOTH_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open Bluetooth Configuration variable file");
		return FAIL;
	}

	// File opened successfully
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to read version from Bluetooth Configuration file");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != BLUETOOTH_CONFIG_VER)
	{
		EPRINT(SYS_LOG, "Bluetooth Configuration version doesn't match");
		status = LoadOldBluetoothConfig(fd, ver);
		close(fd);
		return status;
	}

	if((sizeof(BLUETOOTH_CONFIG_t)) != read(fd, &BluetoothPara, sizeof(BLUETOOTH_CONFIG_t)))
	{
		EPRINT(SYS_LOG, "Failed to read Bluetooth Configuration file");
		close(fd);
		return FAIL;
	}

	//File read successfully
	close(fd);
	BluetoothConfigDebug();
	return SUCCESS;
}
//*****************************************************************************
//	LoadDfltBluetoothConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Bluetooth Configuration.
//*****************************************************************************
BOOL LoadDfltBluetoothConfig(void)
{
	CHAR valueStr[255];

	BluetoothPara = DfltBluetoothPara;
	#ifdef OEM_SUPPORTED
	snprintf(BluetoothPara.btName, BT_NAME_SIZE, "%s", gOEMCurrDeviceInfo.doorInfo.btName);
	#endif //#ifdef OEM_SUPPORTED

	sprintf(valueStr,"%s %s %s","cp",BT_RANGE_FILE_MEDIUM,BT_RANGE_FILE_IN_DEVICE);
	system(valueStr);
	system("echo 1 > /matrix/btRangeFiles/curBtRange.txt");

	SendDfltConfigEvent(BLE_CONFIG_ID);
	if(WriteBluetoothConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "BluetoothConfig");
		return SUCCESS;
	}
	return FAIL;
}
//*********************************************************************************
//	WriteBluetoothConfig()
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Bluetooth Configuration into file.
//*********************************************************************************
BOOL WriteBluetoothConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	fd = open(SYS_BLUETOOTH_CNFG_FILE, O_WRONLY | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open Bluetooth Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = BLUETOOTH_CONFIG_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to write Bluetooth Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if((sizeof(BLUETOOTH_CONFIG_t)) != write(fd, &BluetoothPara, (sizeof(BLUETOOTH_CONFIG_t) ) ) )
	{
		EPRINT(SYS_LOG, "Failed to write Bluetooth Configuration into file");
		close(fd);
		return FAIL;
	}

	//File written successfully
	close(fd);
	BluetoothConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	BluetoothConfigDebug()
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//
//*****************************************************************************
void BluetoothConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, 	 "Bluetooth Config Para");
		PRINTCNFG(CONFIG_LOG, "btEnable[%u]", BluetoothPara.btEnable);
		PRINTCNFG(CONFIG_LOG, "btMacAddress[%s]", BluetoothPara.btMacAddress);
		PRINTCNFG(CONFIG_LOG, "btName[%s]", BluetoothPara.btName);
		PRINTCNFG(CONFIG_LOG, "btServiceUuid");
		DPRINT_BINARY_DATA(CONFIG_LOG, (CHARPTR)BluetoothPara.btServiceUuid, BT_UUID_SIZE);
		PRINTCNFG(CONFIG_LOG, "btWriteCharUuid");
		DPRINT_BINARY_DATA(CONFIG_LOG, (CHARPTR)BluetoothPara.btWriteCharUuid, BT_UUID_SIZE);
		PRINTCNFG(CONFIG_LOG, "btReadChar1Uuid");
		DPRINT_BINARY_DATA(CONFIG_LOG, (CHARPTR)BluetoothPara.btReadChar1Uuid, BT_UUID_SIZE);
		PRINTCNFG(CONFIG_LOG, "btReadChar2Uuid");
		DPRINT_BINARY_DATA(CONFIG_LOG, (CHARPTR)BluetoothPara.btReadChar2Uuid, BT_UUID_SIZE);
		PRINTCNFG(CONFIG_LOG, "btAcsReadRandomNoCharUuid");
		DPRINT_BINARY_DATA(CONFIG_LOG, (CHARPTR)BluetoothPara.btAcsReadRandomNoCharUuid, BT_UUID_SIZE);
		PRINTCNFG(CONFIG_LOG, "btAcsReadAccessIdCharUuid");
		DPRINT_BINARY_DATA(CONFIG_LOG, (CHARPTR)BluetoothPara.btAcsReadAccessIdCharUuid, BT_UUID_SIZE);
		PRINTCNFG(CONFIG_LOG, "btRange[%u]", BluetoothPara.btRange);
		PRINTCNFG(CONFIG_LOG, "btCustomeRangeStrength[%d]", BluetoothPara.btCustomeRangeStrength);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	LoadOldBluetoothConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:

//*****************************************************************************
BOOL LoadOldBluetoothConfig(INT32 fd, UINT8 ver)
{
	lseek(fd, 1, SEEK_SET);
	switch(ver)
	{
	case 1:
		if(sizeof(BLUETOOTH_CONFIG_VER1_t) != read(fd, &BluetoothPara, sizeof(BLUETOOTH_CONFIG_VER1_t) ) )
		{
			EPRINT(CONFIG_LOG,"Failed to read BluetoothPara according to version 1");
			return FAIL;
		}
		break;
	case 2:
		if(sizeof(BLUETOOTH_CONFIG_VER2_t) != read(fd, &BluetoothPara, sizeof(BLUETOOTH_CONFIG_VER2_t) ) )
		{
			EPRINT(CONFIG_LOG,"Failed to read BluetoothPara according to version 2");
			return FAIL;
		}
		break;
		
	default:
		return FAIL;
	}
	if(ver == 1)
	{	
		memcpy(BluetoothPara.btAcsReadRandomNoCharUuid,DfltBluetoothPara.btAcsReadRandomNoCharUuid,sizeof(DfltBluetoothPara.btAcsReadRandomNoCharUuid));
		memcpy(BluetoothPara.btAcsReadAccessIdCharUuid,DfltBluetoothPara.btAcsReadAccessIdCharUuid,sizeof(DfltBluetoothPara.btAcsReadAccessIdCharUuid));
		ver = 2;
	}
	
	if(ver == 2)
	{	
		BluetoothPara.btCustomeRangeStrength = DfltBluetoothPara.btCustomeRangeStrength;
		ver = 3;
	}

	BluetoothConfigDebug();
	if(lseek(fd, 0, SEEK_SET) != -1)
	{
		ver = BLUETOOTH_CONFIG_VER;

		if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write BluetoothPara version");
			return FAIL;
		}
		if( (sizeof(BLUETOOTH_CONFIG_t) ) != write(fd, &BluetoothPara, sizeof(BLUETOOTH_CONFIG_t) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write BluetoothPara");
			return FAIL;
		}
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}
