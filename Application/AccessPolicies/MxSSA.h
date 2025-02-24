#ifndef MXSSA_H
#define MXSSA_H
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
//   Created By   : Ankit Sohaliya
//   File         : MxSSA.h
//   Description  : This file contains Smart secure access related defines
//					and prototypes.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"
#include "MxSlaveCommonMsgList.h"

//******** Defines and Data Types ****

//******** Function Prototypes *******
void DcSSAStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
void ProcessSSAAddlCred(void);
BOOL IsSSAValid(void);
void ProcessUserForBioSI(void);
void CheckSSANextCredToProcess(void);

#endif  // MXSSA_H
