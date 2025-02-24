#if !defined MXWEBAPICREDENTIAL_H
#define MXWEBAPICREDENTIAL_H
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
//   File         : MxWebAPICredential.h
//   Description  : Brief but meaningful description of what file provides.
//
///////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include <string.h>

#include "MxWebAPI.h"
#include "MxWebAPIConfig.h"

//******** Defines and Data Types ****


//******** Function Prototypes *******
UINT8 GetQueryCredential(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile);
UINT8 SetQueryCredential(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile);
UINT8 DeleteQueryCredential(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile);
BOOL FillCredentialElement(FILE * pTXDataFile, UINT8 idx, WEB_API_RESP_TYPE_e respFormat, UINT8 defaultF);

#endif  // MXWEBAPICREDENTIAL_H
