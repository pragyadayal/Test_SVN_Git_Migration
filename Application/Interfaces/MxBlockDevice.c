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
//   File         : MxBlockDevice.c
//   Description  : This file provides functionalities for block device driver.
//					Block device will be MMC or USB.
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#include "MxTypedef.h"
#include "MxLogs.h"
#include "MxBlockDevice.h"
#include "MxAvResp.h"
#include "MxBackupUpdate.h"
#include "MxDcTask.h"
#include "MxConfigFileIo.h"

//******** Extern Variables **********

//******** Defines and Data Types ****

//******** Function Prototypes *******

//******** Global Variables **********
USB_INFO_t UsbInfo;
MMC_INFO_t MmcInfo;

//******** Static Variables **********
static CHARPTR	blockStatStr[MAX_USB_STATUS] =
{
	"Disconnected",
	"Connected",
	"Invalid Fs",
};

//******** Function Definations ******

//*****************************************************************************
//	InitBlockDevice()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//		NONE
//	Description:
//		Initializes block devices like USB, MMC, etc.
//*****************************************************************************
void InitBlockDevice(void)
{
	// USB device initialization
	UsbInfo.status = USB_DISCONNECTED;
	memset(UsbInfo.devPath, 0, sizeof(UsbInfo.devPath) );
	UsbInfo.portId = 0;

	// MMC device initialization
	MmcInfo.status = MMC_DISCONNECTED;
	memset(MmcInfo.devPath, 0, sizeof(MmcInfo.devPath) );
}

//*****************************************************************************
//	SetBlockDevStatus()
//	Param:
//		IN :	BLOCK_DEVICE_INFO_t *tmpblockInfo
//		OUT:	None
//	Returns:	None
//	Description:
//				This function set block device related information.
//*****************************************************************************
void SetBlockDevStatus(BLOCK_DEVICE_INFO_t *tmpblockInfo)
{
	CHAR	buff[100];
	struct stat		temp;

	if( strstr(tmpblockInfo->baseNode, "sd") != NULL)
	{
		USB_STATUS_EV_DATA_t	usbEvData;
		USB_INFO_t				tmpUsbInfo;

		if(strcmp(tmpblockInfo->action, "add") == 0)
		{
			if(strcmp(tmpblockInfo->status, "SUCCESS") == 0)
			{
				tmpUsbInfo.status = USB_CONNECTED;
			}
			else
			{
				tmpUsbInfo.status = USB_INVALID_FS;
			}
			tmpUsbInfo.portId = atoi(tmpblockInfo->path);
		}
		else
		{
			tmpUsbInfo.status = USB_DISCONNECTED;
			tmpUsbInfo.portId = UsbInfo.portId;
		}
		sprintf(tmpUsbInfo.devPath, "/media/%s", tmpblockInfo->baseNode);
		if(tmpUsbInfo.status != UsbInfo.status)
		{
			if( ( (tmpUsbInfo.status == USB_DISCONNECTED) && (strcmp(tmpUsbInfo.devPath, UsbInfo.devPath) != 0) ) ||
					( (tmpUsbInfo.status == USB_INVALID_FS) && (UsbInfo.status == USB_CONNECTED) ) )
			{
				return;
			}

			usbEvData.status = UsbInfo.status = tmpUsbInfo.status;
			usbEvData.portId = UsbInfo.portId = tmpUsbInfo.portId;
			strcpy(UsbInfo.devPath, tmpUsbInfo.devPath);

			ProcMsg(MSG_USB_STATUS_EVENT, &usbEvData, sizeof(usbEvData) );
			DPRINT(SYS_LOG, "USB[%d] %s[%s]", UsbInfo.portId, blockStatStr[UsbInfo.status], UsbInfo.devPath);
		}
		UpdateDisplayStatusField();
	}
	else if(strstr(tmpblockInfo->baseNode, "mmc") != NULL)
	{
		MMC_INFO_t				tmpMmcInfo;

		if(strcmp(tmpblockInfo->action, "add") == 0)
		{
			if(strcmp(tmpblockInfo->status, "SUCCESS") == 0)
			{
				tmpMmcInfo.status = MMC_CONNECTED;
			}
			else
			{
				tmpMmcInfo.status = MMC_INVALID_FS;
			}
		}
		else
		{
			tmpMmcInfo.status = MMC_DISCONNECTED;
		}
		sprintf(tmpMmcInfo.devPath, "/media/%s", tmpblockInfo->baseNode);
		if(tmpMmcInfo.status != MmcInfo.status)
		{
			if( ( (tmpMmcInfo.status == MMC_DISCONNECTED) && (strcmp(tmpMmcInfo.devPath, MmcInfo.devPath) != 0) ) ||
					( (tmpMmcInfo.status == MMC_INVALID_FS) && (MmcInfo.status == MMC_CONNECTED) ) )
			{
				return;
			}

			MmcInfo.status = tmpMmcInfo.status;
			strcpy(MmcInfo.devPath, tmpMmcInfo.devPath);

			if(MmcInfo.status == MMC_CONNECTED)
			{
				CreateDirInMmc();
				SetAudioVisual(USER_RESP_MMC_DEVICE_STATUS, INTERNAL_PORT, NULL);

				sprintf(buff, "rm -rf %s/%s* &", MmcInfo.devPath, SYS_TMP_PHOTO_DIR);
				system(buff);

				//Check photo delete signature file present.
				sprintf(buff,"%s", PHOTO_DEL_SIGNATURE_FILE_PATH);
				if(stat(buff, &temp) == 0)
				{
					//Signature File Found
					DeleteAllUsersPhotoFiles();
				}
			}
			DPRINT(SYS_LOG, "MMC %s[%s]", blockStatStr[MmcInfo.status], MmcInfo.devPath);
		}
	}
}

//*****************************************************************************
//	GetFreeSpaceAvailInSDcard()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:It will give the free space available in memory card
//
//*****************************************************************************
UINT64 GetFreeSpaceAvailInSDcard(void)
{
	UINT64 buff = 0;
	struct statvfs 	tempvsf;

	if(MmcInfo.status != MMC_DISCONNECTED)
	{
		if( statvfs(MmcInfo.devPath, &tempvsf) == 0)
		{
			// f_bsize gives filesystem block size
			// f_bavail gives free blocks for unprivileged users
			buff = ((tempvsf.f_bavail) * (tempvsf.f_bsize)); // free space in bytes
		}
	}
	return buff;
}
