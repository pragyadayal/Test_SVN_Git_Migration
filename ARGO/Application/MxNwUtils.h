#if !defined MXNWUTILS_H
#define MXNWUTILS_H
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
//   File         : MxNwUtils.h
//   Description  : This file declares System wide defines and API.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"

//******** Defines and Data Types ****

//******** Function Prototypes *******
BOOL ValidateNetworkAddr(CHARPTR nwString, CHARPTR ipAddrPtr, CHECK_NW_ADDR_e addrType);
BOOL ValidateServerPort(CHARPTR portString, UINT16PTR serverPortPtr);
BOOL ValidateSubnetAddr(CHARPTR nwString, CHARPTR sbAddrPtr);
BOOL ValidateEnterUrl(CHARPTR urlString, UINT8 len);
BOOL ValidateUrl(CHARPTR urlString, UINT8 len);

void SetInterfacePara(void);
void GetServerInterfaceIpAddr(CHARPTR intfaceAddr);

#endif  // MXNWUTILS_H
