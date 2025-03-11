#if !defined MXACMSMSGHANDLER_H
#define MXACMSMSGHANDLER_H
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
//   Project      : ACS (Access Control System)
//   Created By   : Viral Chavda
//   File         : MxAcmsMsgHandler.h
//   Description  : export APIs of the Acms Mesage Handler.
//
/////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include "MxTypedef.h"
#include "MxSysCommonDef.h"
#include "MxSlaveCommonMsgList.h"
#include "MxEnrollment.h"
#include "MxAcmsTask.h"

//******** Defines and Data Types ****

typedef enum
{
	FP_TRANS_STATUS_IDLE,
	FP_TRANS_STATUS_IN_PROCESS,
	FP_TRANS_STATUS_COMPLETE,
	MAX_FP_TRANS_STATUS,
}FP_TRANS_STATUS_e;

typedef struct
{
	BOOL				intFpTransDoneF;
	BOOL				extFpTransDoneF;
	FP_TRANS_STATUS_e	fpTransStatus;
	UINT32				usrId;
	UINT8				fpIndex;
	BOOL 				isDuressFp;
}FP_TRANS_PARA_t;

#define GetAcmsFieldIdForReaderId(readerId) (BASIC_CFG_FIELD_READER1_TYPE + (2 * readerId))

//******** Function Prototypes *******
BOOL AcmsMsgHandlerGetEvent(UINT16 acmsRollCount,UINT32 acmsEventCount, CHARPTR txData, UINT16PTR txDataLen);
BOOL AcmsMsgHandlerGetCardData(CHARPTR *rxData, CHARPTR txData, UINT16PTR txDataLen);
BOOL AcmsMsgHandlerSetConfig(CHARPTR *rxData, CHARPTR txData, UINT16PTR txDataLen);
BOOL AcmsMsgHandlerGetTemplate(CHARPTR *rxData, CHARPTR txData, UINT16PTR txDataLen);
BOOL AcmsMsgHandlerSetTemplate(CHARPTR *rxData, CHARPTR txData, UINT16PTR txDataLen);
BOOL AcmsMsgHandlerSetCmd(CHARPTR *rxData, CHARPTR txData, UINT16PTR txDataLen);
BOOL AcmsMsgHandlerProcessEnrollment(CHARPTR *rxData, CHARPTR txData, UINT16PTR txDataLen);
BOOL AcmsMsgHandlerGetPINData(CHARPTR *rxData, CHARPTR txData, UINT16PTR txDataLen);
GET_UFI_STATUS_e AcmsMsgHandlerGetUficmd(CHARPTR *rxData, CHARPTR txData, UINT16PTR txDataLen);

void AcmsProcFpResp(SLAVE_MSG_ID_e msgId ,VOIDPTR dataPtr);
BOOL CheckForFpWaitStatus(READER_ID_e readerId);
FP_TRANS_STATUS_e ReadFpTransStatus(void);
void InitFpTransPara(void);
void DeleteFpAckWaitTimer(UINT32 dummy);
BOOL AcmsMsgHandlerGetDeviceDetail(CHARPTR txData, UINT16PTR txDataLen);
void UpdateDuressFpIdx(UINT8 fpIndex,UINT16 usrIdx);
#endif  // MXACMSMSGHANDLER_H
