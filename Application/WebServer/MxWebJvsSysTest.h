#if !defined(MXWEBJVSSYSTEST_H)
#define MXWEBJVSSYSTEST_H
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
//   Created By   : Meetkumar Bariya
//   File         : MxWebJvsSysTest.h
//   Description  :
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
//system includes
#include <stdlib.h>

//Matrix include files
#include "MxTypedef.h"

//******** Defines and Data Types ****

//******** Function Prototypes *******
UINT8 SetSysTestBasicInfo(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 GetSysTestBasicInfo(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 FillSysTestBasicInfo(FILE* pTXDataFile);

UINT8 SetSysTestHWTest(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 GetSysTestHWTest(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 FillSysTestHWTest(FILE* pTXDataFile);

UINT8 SetSysTestReaderTest(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 GetSysTestReaderTest(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 FillSysTestReaderTest(FILE* pTXDataFile);

UINT8 SetSysTestGenericIO(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 GetSysTestGenericIO(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 FillSysTestGenericIO(FILE* pTXDataFile);

UINT8 SetSysTestOtherInfo(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 GetSysTestOtherInfo(FILE *pRXDataFile, FILE *pTXDataFile);
UINT8 FillSysTestOtherInfo(FILE* pTXDataFile);

UINT8 SysTestGenerateReport();
UINT8 SysTestSendReportToFtp();

#endif //MXWEBJVSSYSTEST_H
