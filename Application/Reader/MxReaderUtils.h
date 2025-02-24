#if !defined MXREADERUTILS_H
#define MXREADERUTILS_H
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
//   File         : MxReaderUtils.h
//   Description  : This file declares reader uitls related defines and API.
//
/////////////////////////////////////////////////////////////////////////////


//******** Include files **********
#include "MxTypedef.h"
#include "MxSlaveCommonMsgList.h"
#include "MxCardReaderCommon.h"
#include "MxSysCommonDef.h"
#include "MxCommTask.h"
#include "MxSlaveMsg.h"

//******** Defines and Data Types ****

//******** Function Prototypes *******
void DefltAllFpr(void);
void CheckAndDisableFreeScanOnFpr(UINT8 readerId);
void SetFprPara(void);
UINT8 CalibrateFpSensor(CHARPTR * dummy);
BOOL IsSensorCalibrationRequire(void);
void StartFprFwUpgradeCmd(READER_ID_e readerId);
BOOL SetSfprFwUpgradeTimer(UINT16 timerValue);
void ReadSfprPara(UINT8 paraId);
void ChangeFingerPrintReaderSettings(COMM_STATE_e commState);
VOID SetCbBlePara();
#endif  // MXREADERUTILS_H
