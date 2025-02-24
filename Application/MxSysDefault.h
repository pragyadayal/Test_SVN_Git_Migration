#if !defined MXSYSDEFAULT_H
#define MXSYSDEFAULT_H
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
//   File         : MxSysDefault.h
//   Description  : This file declares System wide defines and API.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"
#include "MxConfigDef.h"

//******** Defines and Data Types ****
typedef enum
{
	SYSTEM_DEFAULT_START = 0,
	SYSTEM_DEFAULT_COMPLETE,
	SYSTEM_DEFAULT_STATUS_MAX
}SYSTEM_DEFAULT_STATUS_e;

#define SSL_ENABLE_FLAG_FILE			"/matrix/SSLEnableF"

//******** Function Prototypes *******
void CheckSysDefltReq(void);
void DefaultSystem(SYS_DFLT_TYPE_e sysDfltType);
BOOL ReadDfltSysFlag(SYS_DFLT_TYPE_e * sysDfltTypePtr);
BOOL WriteDfltSysFlag(SYS_DFLT_TYPE_e sysDfltType);
SYS_DFLT_TYPE_e GetSysDefaultType(VOID);

#endif  // MXSYSDEFAULT_H
