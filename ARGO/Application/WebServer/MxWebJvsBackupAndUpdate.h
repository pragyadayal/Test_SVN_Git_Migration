#if !defined MXWEBJVSBACKUPANDUPDATE_H
#define MXWEBJVSBACKUPANDUPDATE_H
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
//   Created By   : Bharat Gohil
//   File         : MxWebJvsBackupAndUpdate.h
//   Description  : This Header File declares WebJvs Backup And update related 
//					defines and function prototypes.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include <stdio.h>

//******** Defines and Data Types ****

//******** Function Prototypes *******
UINT8 SetBackupAndUpgrade(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 GetEventBackup(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 GetMmcStatus(FILE *pRXDataFile, FILE *pTXDataFile);
void  SetHttpFirmwareUploadState(void);
UINT8 GetSdkVer(FILE *pRXDataFile, FILE *pTXDataFile);
BOOL  GetSTQCSensorValidation(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 GetExtReaderType(UNUSED_PARA FILE *pRXDataFile,FILE *pTXDataFile);
//*************************************************************************
//End of MxWebJvsBackupAndUpdate.h
//*************************************************************************

#endif //MXWEBJVSBACKUPANDUPDATE_H
