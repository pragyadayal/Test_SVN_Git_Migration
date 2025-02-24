#if !defined MXCONFIGUPDATEHANDLER_H
#define MXCONFIGUPDATEHANDLER_H
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
//   Created By   : Kuldip Gor
//   File         : MxConfigUpdateHandler.h
//   Description  : This file consist of Prototypes of Configuration Update Handler
//					functions and defines.
//
///////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include "MxTypedef.h"

//******** Defines and Data Types ****

//******** Function Prototypes *******
BOOL ConfigUpdateHandler(CONFIGURATION_TABLE_e configUpdateTableIdx, UINT16 configIdx, VOIDPTR tempConfigPtr);


#endif  // MXCONFIGUPDATEHANDLER_H
