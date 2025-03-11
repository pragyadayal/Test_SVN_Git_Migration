/* ____________________________________________________________________________
*
*            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION
*
*   Property of Texas Instruments
*   For Unrestricted Internal Use Only
*   Unauthorized reproduction and/or distribution is strictly prohibited.
*   This product is protected under copyright law and trade secret law as an
*   unpublished work.
*   Created 2008, (C) Copyright 2009 Texas Instruments.  All rights reserved.
*
*   Component  : nand tool
*
*   Filename   : nand_tool.h
*
*______________________________________________________________________________
*
*   History:
*   28-Jul-08   Martin Zarzycki
*               Creation
* 
*____________________________________________________________________________*/

#ifndef _NAND_H_
#define _NAND_H_

/*_______________________________ Include Files _____________________________*/

#include "romcode_types.h"

/*____________________________ Exported Definitions _________________________*/

// ECC types
#define NAND_ECC_NONE     0
#define NAND_ECC_BCH4     4
#define NAND_ECC_BCH8     8

// NAND types
#define NAND_TYPE_RAW     0
#define NAND_TYPE_ONE     1

// general defines
#define NAND_SECTOR_SIZE            512
#define NAND_ONENAND_SECTOR_LIMIT   4

/*______________________________ Exported Types _____________________________*/


typedef struct 
{
  /* parameters */
  U32 PageSize;
  U32 SpareSize;
  U32 BlockSize;
  U8  EccType;
  U8  NandType;
  
  U8  CorrectableCount;
  U8  UnCorrectableCount;  
  U32 CorrectableAddresses[64];
  U32 UnCorrectableAddresses[64]; 
  
  /* flags */
  BOOL  Verbose;  
  BOOL  BadBlock;
  BOOL  BigSpare;
  
} NAND_Params_t;

/*_________________________________ Functions _______________________________*/

/*___________________________________________________________________________*/

#endif /*_NAND_H_*/
/*________________________________ End Of File ______________________________*/

