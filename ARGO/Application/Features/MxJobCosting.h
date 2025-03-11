#if !defined MXJOBCOSTING_H
#define MXJOBCOSTING_H
///////////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Comsec Pvt. Ltd., Baroda, India.
//   Project      : Access Control System
//   Created By   : Sourabh Kahalekar
//   File         : MxJobCosting.h
//   Description  :	Job Costing related data types etc.
//
///////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include <stdio.h>

#include "MxTypedef.h"
#include "MxSysCommonDef.h"
#include "MxConfig.h"

//******** Defines and Data Types ****
#define JOB_INPUT_WAIT_TIMER		10	//in seconds

//******** Function Prototypes *******
void InitJobCost(void);
void DcJobCodeDisplayStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
void SwitchDcToJobDisplayState(void);
BOOL IsJobCodeInputReq(void);
BOOL IsJobAssignmentTypeStatic(void);
BOOL IsRetainJobCodeAvailable(void);
VOID ClearRetainJobCode(void);
#endif  // MXJOBCOSTING_H
