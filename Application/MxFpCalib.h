#if !defined MXFPCALIB_H
#define MXFPCALIB_H
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
//   File         : MxFpCalib.h
//   Description  : This file declares System wide defines and API.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"

//******** Defines and Data Types ****

//******** Function Prototypes *******
BOOL WriteCalibrateSfprSign(UINT16 calibSensorSign);
BOOL ReadCalibrateSfprSign(UINT16 * calibSensorSign);

#endif  // MXFPCALIB_H
