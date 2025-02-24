#if !defined MXWEBJVSBLUETOOTHSETTING_H
#define MXWEBJVSBLUETOOTHSETTING_H
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
//   Created By   : Manish chandra
//   File         : MxWebJvsBluetoothSetting.h
//   Description  : This Header File declares WebJvs BluetoothSetting
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include <stdio.h>

//******** Defines and Data Types ****

//******** Function Prototypes *******
#include "MxDeviceSpecific.h"
UINT8 SetBluetoothSetting(FILE *pRXDataFile, FILE *pTXDataFile );
UINT8 GetBluetoothSetting(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 FillBluetoothSetting(FILE* pTXDataFile);
UINT8 SetDfltBluetoothSetting(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 GetBtDeviceList(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 FillBtDeviceList(FILE* pTXDataFile, BOOL btDiscoveryAddFrameReqF);
BOOL IsBtDeviceDiscoveryThreadIsRunning(VOID);
UINT8 StartBtAutoSystemTest(void);

//*************************************************************************
//End of MxWebJvsDoorModeWallpaper.h
//*************************************************************************
#endif //MXWEBJVSDOORMODEWALLPAPER_H
