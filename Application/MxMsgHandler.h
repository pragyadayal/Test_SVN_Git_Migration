#if !defined MXMSGHANDLER_H
#define MXMSGHANDLER_H
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
//   File         : MxMsgHandler.h
//   Description  : This file implements message handler realted defines and API.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"
#include "MxBluetooth.h"

//******** Defines and Data Types ****


//******** Function Prototypes *******
void MsgHandler(void);
void SendStatusResponse(void);

#endif  // MXMSGHANDLER_H


