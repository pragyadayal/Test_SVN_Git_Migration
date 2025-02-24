#if !defined MXWEBJVSDOORMODEWALLPAPER_H
#define MXWEBJVSDOORMODEWALLPAPER_H
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
//   File         : MxWebJvsDoorModeWallpaper.h
//   Description  : This Header File declares WebJvs Door mode Custom 
//					Wallpaper related defines and function prototypes.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include <stdio.h>

//******** Defines and Data Types ****

//******** Function Prototypes *******
UINT8 SetModeSpecificWallpaper(FILE *pRXDataFile, FILE *pTXDataFile );
UINT8 GetModeSpecificWallpaper(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 FillModeSpecificWallpaper(FILE* pTXDataFile);
UINT8 RemoveModeSpecificWallpaper(FILE *pRXDataFile, FILE *pTXDataFile);
//*************************************************************************
//End of MxWebJvsDoorModeWallpaper.h
//*************************************************************************
#endif //MXWEBJVSDOORMODEWALLPAPER_H
