#if !defined MX_DC_SPECIAL_FUNC_H
#define MX_DC_SPECIAL_FUNC_H
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
//   Created By   : Ronak Tanna
//   File         : MxDcSpecialFunc.h
//   Description  : This function contains DC task's Special func state related 
//					Data types and function prototypes.
//					
//
/////////////////////////////////////////////////////////////////////////////

#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxSlaveCommonMsgList.h"
#include "MxGuiHandler.h"
#include "MxSlaveMsg.h"

//******** Defines and Data Types ****
#define SP_FN_INPUT_WAIT_TIMER		10	//in seconds

#define MAX_INOUT_SPL_FUNC_LIST 	(OVER_TIME_MARKING_OUT + 1)

//******** Function Prototypes *******
void DcSplFnStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
BOOL ProcessSplFn(void);
UINT8 GetCurrSplCode(void);
void TerminateSplFnActivity(void);
void SendMsgSearchedUserList(CHARPTR userName, SLAVE_MSG_ID_e msgId);
void SendUserNameFromId(CHARPTR alphaNumId, SLAVE_MSG_ID_e msgId);
void SendUserNameFromRefId(UINT32 userRefId, SLAVE_MSG_ID_e msgId);
void GetMultiUserSplFnStatus(MUL_USR_SPL_FUN_QT_t * mulUsrSplFnStatus);
void SwitchDcToSpFnDisplayState(void);
void DcSelectSpFnState(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
void SplFnGetUserInputTimeout(UINT32 dummy);
BOOL SetSpFnDisplayWaitTimer(UINT8 timer);
VOID ReloadSpecialFunctionInputTimer(void);
void ChkUpdateDeleteTypeForFace(DELETE_USER_CREDEN_t * inputDataPtr, ENROLLMENT_SOURCE_e SourceType);
#endif // MX_DC_SPECIAL_FUNC_H

