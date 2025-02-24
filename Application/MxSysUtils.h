#if !defined MXSYSUTILS_H
#define MXSYSUTILS_H
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
//   File         : MxSysUtils.h
//   Description  : This file declares System wide defines and API.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"
#include "MxSysCommonDef.h"
#include "MxConfigDef.h"
#include "MxSysDefault.h"
#include "MxWatchDog.h"
#include "MxSysUpgrade.h"
#include "MxReaderConv.h"
#include "MxSysReboot.h"
#include "MxNwUtils.h"

//******** Defines and Data Types ****

//can convert UINT16 HEX to BCD
#define HEX2BCD(n) ( ( ( (n >> 12) & 0xf) * 1000) + ( ( (n >> 8) & 0xf) * 100) + ( ( (n >> 4) & 0xf) * 10) + ( n & 0x0f) )
//can convert UINT8 BCD to HEX
#define BCD2HEX(n) ( ( ( ( (UINT8)n) / 10) << 4) + ( ( (UINT8)n) % 10) )

//******** Function Prototypes *******
UINT8 GetUINT8ChkSum(UINT8PTR dataPtr, UINT16 length);
UINT16 GetUINT16FromStringAndLen(CHARPTR sourcePtr, UINT8 len);
BOOL IsStringDataNumeric(CHARPTR rxData, UINT8 size);
BOOL IsStringDataInRange(CHARPTR rxData, UINT8 size, UINT8 startValue, UINT8 endValue);
BOOL AsciiToInt(CHARPTR ascii, UINT64PTR integer);

void CheckACSFeatureStatus(void);

UINT16 in_cksum(UINT16PTR buf1, UINT16PTR buf2, UINT16 size1, UINT16 size2);

UINT16 GetFileListInDir(CHARPTR dirPath, CHARPTR fileNameInitial, CHARPTR result, UINT16 fileNameArrySize, UINT16 maxResult);

BOOL IsDemoSoftware(void);
BOOL IsSamaySoftware(void);
void WriteApplPresentFile(void);
void ConvertSysInDemo(void);
void ConvertSysInSamay(void);
void WriteSamayApplPresentFile(void);

BOOL CheckFprTemplateFormat(UINT8PTR dataPtr);
BOOL verifyChecksumFile(CHARPTR fileName);

#endif  // MXSYSUTILS_H
