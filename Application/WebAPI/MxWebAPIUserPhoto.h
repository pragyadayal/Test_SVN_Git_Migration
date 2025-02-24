#if !defined MXWEBAPIUSERPHOTO_H
#define MXWEBAPIUSERPHOTO_H
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
//   File         : MxWebAPIUserPhoto.h
//   Description  : Brief but meaningful description of what file provides.
//
///////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include <string.h>

#include "MxWebAPI.h"
#include "MxWebAPIConfig.h"

//******** Defines and Data Types ****


//******** Function Prototypes *******
UINT8 GetQueryUserPhoto(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile);
UINT8 SetQueryUserPhoto(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile);
UINT8 DeleteQueryUserPhoto(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile);

#endif  // MXWEBAPIUSERPHOTO_H
