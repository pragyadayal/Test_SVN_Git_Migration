#if !defined MXWEBAPIFINGERPARAMETER_H
#define MXWEBAPIFINGERPARAMETER_H
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
//   File         : MxWebAPIFingerParameter.h
//   Description  : Brief but meaningful description of what file provides.
//
///////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include <string.h>

#include "MxWebAPI.h"
#include "MxWebAPIConfig.h"

//******** Defines and Data Types ****


//******** Function Prototypes *******
UINT8 GetQueryFingerParameter(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile);
UINT8 SetQueryFingerParameter(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile);
UINT8 SetDefaultQueryFingerParameter(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile);
BOOL FillFingerParameterElement(FILE * pTXDataFile, UINT8 idx, WEB_API_RESP_TYPE_e respFormat, UINT8 defaultF);
UINT8 SetDefaultFingerParameter(BOOL * defaultElementReqF);
UINT8 GetDefaultQueryFingerParameter(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile);

#endif  // MXWEBAPIFINGERPARAMETER_H
