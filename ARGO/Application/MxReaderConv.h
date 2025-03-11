#if !defined MXREADERCONV_H
#define MXREADERCONV_H
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
//   File         : MxReaderConv.h
//   Description  : This file declares System wide defines and API.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"
#include "MxSysCommonDef.h"
#include "MxConfigDef.h"

//******** Defines and Data Types ****

//******** Function Prototypes *******
UINT8 ConvertRdrType(UINT8 readerId, UINT8 inReaderType, READER_CONV_TYPE_e convType);
READER_TYPE_e ConvertPanelRdrTypToDD(PANEL_READER_TYPE_e status);
PANEL_READER_TYPE_e ConvertDDRdrTypToPanel(READER_TYPE_e ddRdrTyp);

#endif  // MXREADERCONV_H
