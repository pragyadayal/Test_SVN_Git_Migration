#if !defined MXACMSTASK_H
#define MXACMSTASK_H
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
//   Created By   : Kunjal Garach
//   File         : MxAcmsTask.h
//   Description  : This is Header File for MxAcmsTask.c
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"
#include "MxAcmsCommon.h"

//******** Defines and Data Types ****
typedef enum
{
	ACMS_OFFLINE = 0,
	ACMS_ONLINE
}ACMS_COMM_STATUS_e;

#define DO_NOT_CLOSE_ALL_SERVER_SOCKETS		0
#define CLOSE_ALL_SERVER_SOCKETS			1

#define GEN_ACMS_ENUM(ACMS_ENUM, ACMS_NAME)	ACMS_ENUM,
#define GEN_ACMS_NAME(ACMS_ENUM, ACMS_NAME)	ACMS_NAME,

#define ACMS_RCV_CMD_HDR(API)\
		API(ACK_BAL,	"ACK_BAL")\
		API(ACK_PIN,	"ACK_PIN")\
		API(ACK_CUP,	"ACK_CUP")\

typedef enum
{
	ACMS_RCV_CMD_HDR(GEN_ACMS_ENUM)
	MAX_RCV_CMD_HDR
}RCV_CMD_HDR_e;

#define ACMS_SND_CMD_HDR(API)\
		API(REQ_BAL,	"REQ_BAL")\
		API(REQ_PIN,	"REQ_PIN")\
		API(REQ_CUP,	"REQ_CUP")\

typedef enum
{
	ACMS_SND_CMD_HDR(GEN_ACMS_ENUM)
	MAX_SND_CMD_HDR
}SND_CMD_HDR_e;

typedef enum
{
	FAIL_UNKNOWN_REASON = 0,
	DEVICE_TYPE_NOT_SUPPORTED,
	INVALID_MID,
	INVALID_PDID,
	WRONG_FACE_LOCATION,
	USER_NOT_AVAILABLE,
	FACE_IMAGES_NOT_AVAILABLE,
	FR_SERVER_ERROR,
	GET_UFI_SUCCESS,
	MAX_ERR_UFI_STATUS
}GET_UFI_STATUS_e;

//******** Function Prototypes *******
void InitAcmsTask(void);
void RunAcmsTask(void);
void LogoutAcms(ACMS_COMM_LOGOUT_MODE_e logoutMode, BOOL commPortParaChangeF);
BOOL ReadAcmsEvent(UINT16 rollCnt, UINT32 evCnt, CHARPTR txData, UINT16PTR lenPtr);
BOOL WriteAcmsConfig(CHARPTR confDataPtr);
ACMS_COMM_STATUS_e GetAcmsStatus(void);
BOOL AcmsStartFwUpgradeThread(CHARPTR fileStr);
UINT16 GetDeviceMID(void);
void RunServerCmdRespRcvTask(void);
void AbortServerCmd(void);
UINT16 GetMaxTimeRequiredForAcmsTransaction(VOID);

#endif  // MXACMSTASK_H

