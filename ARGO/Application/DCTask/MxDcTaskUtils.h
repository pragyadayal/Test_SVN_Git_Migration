#if !defined MX_DC_TASK_UTILS_H
#define MX_DC_TASK_UTILS_H
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
//   Created By   : Mukesh Savaliya
//   File         : MxDcTaskUtils.h
//   Description  : This function contains DC task related Data types and 
//					function prototypes.
//
/////////////////////////////////////////////////////////////////////////////

#include "MxTypedef.h"
#include "MxConfigDef.h"
#include "MxSysTimer.h"
#include "MxSlaveMsg.h"
#include "MxEventLog.h"
#include "MxAvResp.h"
#include "MxDcTaskCommonDef.h"

//******** Defines and Data Types ****

//******** Function Prototypes *******
void LoadInterDigitTimer(void);
void InterDigitTimeout(UINT32 dummy);
void SetFpMatchTimer(UINT8	timerValue);
void FpMatchingTimeout(UINT32 data);
BOOL LoadShowCardForKeyChngTimer(UINT16	timerValue);

BOOL IsDoorBusy(void);
BOOL IsDoorBusyForBTRequest(void);
EV_FIELD3 GetUserCredEvFieldInfo(USER_CNT_e userCnt);
EV_FIELD3 GetEvMode(UINT8 readerMode);
EVENT_ID_e GetEventIdFromAuthReason(AUTH_REASON_e authReason);
USER_RESP_e GetUserRespFromAuthReason(AUTH_REASON_e authReason);
void DeleteFsmTimers(void);
void SetUserAsVisitor(void);
USER_EVENT_INFO_e GetEventModeFromSplCode(READER_MODE_e readerMode, EV_USER_SPECIAL_CODE_FIELD_e splCode);
void CreateDirInMmc(void);
MULTI_USER_SPL_STATUS_e GetLateInEarlOutStatus(void);
void StoreUserInfoAsFirstUser(void);
EV_FIELD2 GetEventField2FromAuthReason(AUTH_REASON_e authReason);
void StoreRcvdCred(CREDENTIAL_TYPE_e credType, UINT32 credSubType);
BOOL PinEntryPossible(RESPONSE_PORT_e pinRespPort);

UINT8 DeleteAllFpFromDoor(void);

void SmartCardKeyChngActn(void);
void SendMsgChangeHIDRdrKey(UINT8 readerId, UINT8 rdrKeyLoc, UINT8PTR keyPtr);

void VisitorDenyEvtAction(BOOL showAvResp);
void IntercomPanicEvtAction(void);
void VisitorAllowEvtAction(BOOL showAvResp);

READER_MODE_e GetReaderEntryExitMode(UINT8 readerId);
void UpdateDoorEntryExitMode(void);
void UpdateDoorIdleDisplay(void);
void DoorModeScheduleUpdate(void);
void UpdateDcInfoReaderMode(UINT8 readerId);
void SetQrCodeAuthReasonInDcInfo(void);

void LoadInterDigitTimerForExternal(void);
void InterDigitTimeoutOnExternal(UINT32 dummy);
BOOL SetTimerMultiInputWaitTimer(UINT32 timer);
void MultiInputWaitTimeoutOnExternal(UINT32 dummy);

void SendVerifyHostTemplateByScanCmd(VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t verifyHostTempleteByScan);
UINT8 GetGoingIN_OUTSpecialFunctionInfo(SLAVE_MSG_ID_e msgId,  UINT8PTR specialFuncIndexPtr);
BOOL IsConnectToPanelEnabled(void);
BOOL IsWaitingForAcsBleCredential(VOID);
BOOL IsFREnable(VOID);
VOID HandleUnExpectedCommCloseReq(VOID);
BOOL IsWaitingForAcsPin(VOID);
VOID CheckHttpServerConfig();
#endif  // MX_DC_TASK_UTILS_H
