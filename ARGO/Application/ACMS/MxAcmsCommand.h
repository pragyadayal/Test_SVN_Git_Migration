#if !defined MXACMSCOMMAND_H
#define MXACMSCOMMAND_H
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
//   Project      : ACS (Access Control System)
//   Created By   : Bharat Gohil
//   File         : MxACMSCommand.h
//   Description  : This is Header File for MxACMSCommand.c
//
/////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include "MxTypedef.h"
#include "MxConfigDef.h"
#include "MxIdtServerBioSyncTask.h"

//******** Defines and Data Types ****

typedef enum
{
	UF_TYPE_UPDATEDFACE,
	UF_TYPE_RESYNC,
	UF_TYPE_DELFACE,
	UF_TYPE_CLEARALL,
	UF_TYPE_MAX
}UF_TYPE_e;

typedef enum
{
	CAFE_RECHARGE_TRANSACTION,
	CAFE_RESET_TRANSACTION,
	CAFE_READ_BAL_TRANSACTION,
	MAX_CAFE_TRANSACTION,
}CAFE_CMD_e;

typedef enum
{
	PROFILE_PHOTO_TYPE = 1,
	MAX_PHOTO_SYNC_TYPE
}PHOTO_SYNC_TYPE_e;

typedef struct
{
	CAFE_CMD_e		cmd;
	UINT32			userId;
	INT32			amount;
	INT32			usrBalance;
	CHAR			usrBalanceStr[MAX_USER_BALANCE_STR_LENGTH];
}CAFE_CMD_DETAIL_t;

//******** Function Prototypes *******

UINT8 AcmsProcDateTimeCmd(CHARPTR * rxData);
UINT8 AcmsProcAckAlarmCmd(CHARPTR * rxData);
UINT8 AcmsProcClrAlarmCmd(CHARPTR * rxData);
UINT8 AcmsProcDoorLockCmd(CHARPTR * rxData);
UINT8 AcmsProcDoorUnLockCmd(CHARPTR * rxData);
UINT8 AcmsProcDoorNormalCmd(CHARPTR * rxData);
UINT8 AcmsProcDoorAuxInDisableCmd(CHARPTR * rxData);
UINT8 AcmsProcDoorAuxInNormalCmd(CHARPTR * rxData);
UINT8 AcmsProcDoorAuxOutDisableCmd(CHARPTR * rxData);
UINT8 AcmsProcDoorAuxOutNormalCmd(CHARPTR * rxData);
UINT8 AcmsProcDoorSenseDisableCmd(CHARPTR * rxData);
UINT8 AcmsProcDoorSenseNormalCmd(CHARPTR * rxData);
UINT8 AcmsProcDoorAuxOutLatchRstCmd(CHARPTR * rxData);
UINT8 AcmsProcClrAllFpCmd(CHARPTR * rxData);
UINT8 AcmsProcIOLinkRstCmd(CHARPTR * rxData);
UINT8 AcmsProcLogoutCmd(CHARPTR * rxData);
UINT8 AcmsProcRestoreUserCmd(CHARPTR * rxData);
UINT8 AcmsProcDeleteFpCmd(CHARPTR * rxData);
UINT8 AcmsProcDeleteCardCmd(CHARPTR * rxData);
UINT8 AcmsProcSoftRebootCmd(CHARPTR * rxData);
UINT8 AcmsProcUpdateFirmwareCmd(CHARPTR * rxData);
UINT8 AcmsProcTimeTriggerResetCmd(CHARPTR * rxData);
UINT8 AcmsProcSystemDefaultCmd(CHARPTR * rxData);
UINT8 AcmsProcDoorOpenCmd(CHARPTR * rxData);
UINT8 AcmsProcRechargeCafeBalance(CHARPTR * rxData);
UINT8 AcmsProcResetCafeBalance(CHARPTR * rxData);
UINT8 AcmsProcChangePasswordCmd(CHARPTR * rxData);
UINT8 AcmsProcDeleteUserPhotoCmd(CHARPTR * rxData);
UINT8 AcmsProcDeleteCafeItemImage(CHARPTR * rxData);
UINT8 AcmsDoorOpenCmd(CHARPTR * rxData);
UINT8 AcmsProcActAuxOutCmd(CHARPTR * rxData);
UINT8 AcmsProcFtpFirmwareUpgradeCmd(CHARPTR * rxData);
UINT8 AcmsProcAPBResetCmd(CHARPTR * rxData);
UINT8 AcmsProcDeleteOneFaceTmplOfUserCmd(CHARPTR * rxData);
UINT8 AcmsProcDeleteAllFaceTmplOfUserCmd(CHARPTR * rxData);
UINT8 AcmsProcUpdateFace(CHARPTR * rxData);
UINT8 AcmsProcPhotoSync(CHARPTR * rxData);
#endif  // MXACMSCOMMAND_H
