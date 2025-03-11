#if !defined MXBLUETOOTHCONFIG_H
#define MXBLUETOOTHCONFIG_H
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
//   File         : MxBluetoothConfig.h
//   Description  : This file consist of Prototypes of Configuration File I/O 
//					functions and defines.
//
/////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include<stdio.h>
#include "MxTypedef.h"
#include"MxConfig.h"

//******** Defines and Data Types ****
#define SYS_BLUETOOTH_CNFG_FILE				SYS_CONFIG_PATH"MxBluetooth.cfg"

#define BLUETOOTH_CONFIG_VER 		3		//added uuids for acs application.
typedef struct
{
	BOOL	btEnable;
	CHAR 	btMacAddress[MAX_MAC_ADDRESS];
	CHAR	btName[BT_NAME_SIZE];
	UINT8	btServiceUuid[BT_UUID_SIZE];
	UINT8	btWriteCharUuid[BT_UUID_SIZE];
	UINT8	btReadChar1Uuid[BT_UUID_SIZE];
	UINT8	btReadChar2Uuid[BT_UUID_SIZE];
	UINT8	btRange;
	UINT8	btAcsReadRandomNoCharUuid[BT_UUID_SIZE];
	UINT8	btAcsReadAccessIdCharUuid[BT_UUID_SIZE];
	INT16   btCustomeRangeStrength;
}BLUETOOTH_CONFIG_t;

typedef struct
{
	BOOL	btEnable;
	CHAR 	btMacAddress[MAX_MAC_ADDRESS];
	CHAR	btName[BT_NAME_SIZE];
	UINT8	btServiceUuid[BT_UUID_SIZE];
	UINT8	btWriteCharUuid[BT_UUID_SIZE];
	UINT8	btReadChar1Uuid[BT_UUID_SIZE];
	UINT8	btReadChar2Uuid[BT_UUID_SIZE];
	UINT8	btRange;
}BLUETOOTH_CONFIG_VER1_t;

typedef struct
{
	BOOL	btEnable;
	CHAR 	btMacAddress[MAX_MAC_ADDRESS];
	CHAR	btName[BT_NAME_SIZE];
	UINT8	btServiceUuid[BT_UUID_SIZE];
	UINT8	btWriteCharUuid[BT_UUID_SIZE];
	UINT8	btReadChar1Uuid[BT_UUID_SIZE];
	UINT8	btReadChar2Uuid[BT_UUID_SIZE];
	UINT8	btRange;
	UINT8	btAcsReadRandomNoCharUuid[BT_UUID_SIZE];
	UINT8	btAcsReadAccessIdCharUuid[BT_UUID_SIZE];
}BLUETOOTH_CONFIG_VER2_t;
//******** Function Prototypes *******
void InitBluetoothConfig(void);
BOOL ReadBluetoothConfig(void);
BOOL LoadDfltBluetoothConfig(void);
BOOL WriteBluetoothConfig(void);
void BluetoothConfigDebug(void);
BOOL LoadOldBluetoothConfig(INT32 fd, UINT8 ver);
#endif  // MXBLUETOOTHCONFIG_H
