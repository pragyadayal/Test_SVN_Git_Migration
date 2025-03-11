#if !defined MXOLDCONFIGFILEIO_H
#define MXOLDCONFIGFILEIO_H
///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix ComSec Pvt. Ltd., Baroda, India.
//   Project      : Access Control System
//   Created By   : Ronak Tanna
//   File         : MxOldConfigFileIo.h
//   Description  : This file consist of Prototypes of Configuration File I/O
//					functions made for handling of earlier version
//					configuration structures.
//
/////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include "MxTypedef.h"

//******** Defines and Data Types ****
typedef struct
{
    CHAR			httpServerUserId[MAX_HTTP_SERVER_USER_ID_LEN];
    CHAR			httpServerPassword[MAX_HTTP_SERVER_PASSWORD_LEN];
    CHAR			httpServerDirectory[MAX_HTTP_SERVER_DIRECTORY_LEN];
    BOOL			interfaceSelctionF;		// 0=Auto, 1=Manual
}HTTP_SERVER_VER1_t;

//******** Function Prototypes *******
BOOL LoadOldModemPara(INT32 fd, UINT8 ver);
BOOL LoadOldAdvancePara(INT32 fd, UINT8 ver);
BOOL LoadOldUserConfig(INT32 fd, UINT8 ver, UINT16 start, UINT8 length);
BOOL LoadOldBasicConfig(INT32 fd, UINT8 ver);
BOOL LoadOldSmartRouteAccessConfig(INT32 fd, UINT8 ver);
BOOL LoadOldCafeMenuConfig(UINT32 fd, UINT8 ver);
BOOL LoadOldBioIdtServerConfig(INT32 fd, UINT8 ver);
BOOL LoadOldSysVarInfo(INT32 fd, UINT8 ver);
BOOL LoadOldPwdConfig(INT32 fd, UINT8 ver);
BOOL LoadOldPrinterConfig(INT32 fd, UINT8 ver);
BOOL LoadOldCafeItemConfig(UINT32 fd, UINT8 ver);
BOOL LoadOldJCMConfig(UINT32 fd, UINT8 ver);
BOOL LoadOldWebApiEventConfig(INT32 fd, UINT8 ver);
BOOL LoadOldDvrEvtActnConfig(UINT32 fd, UINT8 ver);
BOOL LoadOldEthParaConfig(UINT32 fd, UINT8 ver);
//BOOL LoadOldWifiParaConfig(INT32 fd, UINT8 ver);
BOOL LoadOldBlockedUserConfig(UINT32 fd, UINT8 ver, UINT16 start, UINT8 length ,UINT8 pg);
BOOL LoadOldVisitorMangmentConfig(UINT32 fd, UINT8 ver, UINT16 start, UINT8 length);
BOOL LoadOldSpecialFunctionConfig(INT32 fd, UINT8 ver);
BOOL LoadOldBackUpAndUpdateConfig(INT32 fd, UINT8 ver);
BOOL LoadOldIoLinkConfig(INT32 fd, UINT8 ver);
BOOL LoadOldCardFormatConfig(INT32 fd, UINT8 ver);
BOOL LoadOldWiFiConfig(INT32 fd, UINT8 ver);
BOOL LoadOldLicenseDongleParaConfig(INT32 fd, UINT8 ver);
BOOL LoadOldCmdCtrlConfig(INT32 fd, UINT8 ver);
BOOL LoadOldConnectToPanelConfig(INT32 fd, UINT8 ver);
BOOL LoadOldFRServerConfig(INT32 fd, UINT8 ver);
BOOL LoadOldFirstInRuleConfig(INT32 fd, UINT8 ver);
BOOL LoadOldIpCameraConfig(INT32 fd, UINT8 ver);
BOOL LoadOldFileLogConfig(INT32 fd, UINT8 ver);
BOOL LoadOldServerParaConfig(UINT32 fd, UINT8 ver);
BOOL LoadOldSavedWifiParaConfig( INT32 fd, UINT8 ver);
#endif  // MXOLDCONFIGFILEIO_H
