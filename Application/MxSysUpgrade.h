#if !defined MXSYSUPGRADE_H
#define MXSYSUPGRADE_H
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
//   Created By   : Ronak Tanna
//   File         : MxSysUpgrade.h
//   Description  : This file declares System wide defines and API.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"
#define FW_UPGRADE_STAT_FILE			"/matrix/UpgradeStatus.txt"

//******** Defines and Data Types ****
typedef enum
{
	FIRMWARE_UPGRADE_FAIL,
	FIRMWARE_UPGRADE_SUCCESS,
	FIRMWARE_UPGRADE_CRC_FAIL,
	FIRMWARE_UPGRADE_DOWNGRADE_CNTRL,
	FIRMWARE_UPGRADE_BENCHMARK_FOUND,
	MAX_FIRMWARE_UPGRADE_STATUS,
}FIRMWARE_UPGRADE_STATUS_e;

typedef enum
{
	ONLY_BM_CHECK,
	AUTO_BM_UPGRADE,
	FORCE_BM_UPGRADE,
}BENCHMARK_UPGRADE_TYPE_e;

typedef struct 
{
	UINT32	firmwareSize;
	UINT16	index;
	UINT16	noOfPacket;
	CHAR	fileName[50];
}FPR_FW_UPGRADE_STATUS_t;

/* Enum to check 3 values while checking whether we first needs to upgrade with benchmark firmware */
typedef enum
{
	CHECK_BM_FW_FAIL=0,
	CHECK_BM_FW_NOT_NEEDED,
	CHECK_BM_FW_NEEDED,
}CHECK_BM_FW_RESULT;

//******** Function Prototypes *******
FIRMWARE_UPGRADE_STATUS_e StartSystemFwUpgrade(CHARPTR fileName, BENCHMARK_UPGRADE_TYPE_e benchmarkUpgradeType);

void DisplayFwUpgradeStatusMsg(void);

void StartPDFirmwareUpgrade(FIRMWARE_PKT_INFO_t* pktInfo);
BOOL WriteZipFile(UINT32 pktNo, CHARPTR pktDataPtr);
void CloseDownloadZip(void);
void BackupAfterSDKUpgrade();
FIRMWARE_UPGRADE_STATUS_e StartSystemSdkUpgrade(CHARPTR fileStr);
VOIDPTR FirmwareUpgradeFromPanelThread(UNUSED_PARA VOIDPTR arg);
BOOL SetFwUpgradeFromPanelTimer(void);
void FwUpgradeFromPanelTimeout(UINT32 dummy);
void SigHandlerForFirmwareUpgradeFromPanelThread(INT32 signal);

#endif  // MXSYSUPGRADE_H
