#if !defined MXBLOCKDEVICE_H
#define MXBLOCKDEVICE_H
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
//   Project      : ACS - Master
//   Created By   : Ronak Tanna
//   File         : MxBlockDevice.h
//   Description  : This file 
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"
#include "MxSysCommonDef.h"
#include "MxNetworkManager.h"
#include "MxSlaveCommon.h"

//******** Defines and Data Types ****

// MMC device machine state
enum
{
	MMC_DISCONNECTED = 0,	// MMC Device is disconnected
	MMC_CONNECTED,			// MMC Device is connected
	MMC_INVALID_FS,			// MMC Device can not process due to Invalid File system
	MAX_MMC_STATUS,
};
typedef UINT8	MMC_DEVICE_STATUS_e;

typedef struct
{
	USB_DEVICE_STATUS_e	status;
	CHAR				devPath[MAX_USB_PATH_SIZE];
	UINT8				portId;
}USB_INFO_t;

typedef struct
{
	MMC_DEVICE_STATUS_e	status;
	CHAR				devPath[MAX_MMC_PATH_SIZE];
}MMC_INFO_t;

//******** Function Prototypes *******
void InitBlockDevice(void);
void SetBlockDevStatus(BLOCK_DEVICE_INFO_t *tmpblockInfo);
UINT64 GetFreeSpaceAvailInSDcard(void);

#endif	//MXBLOCKDEVICE_H


