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
*   Component  : bin2nand
*
*   Filename   : nand_hamming.h
*
*______________________________________________________________________________
*
*   History:
*   28-Jul-08   Martin Zarzycki
*               Creation
*____________________________________________________________________________*/

#ifndef _NAND_ERRORS_H_
#define _NAND_ERRORS_H_

/*------------------------------- Include Files -----------------------------*/

#include "romcode_types.h"

/*-------------------------------- Definitions ------------------------------*/

/*----------------------------------- Types ---------------------------------*/

/*-------------------------------  Functions --------------------------------*/

STATUS NE_GenerateRandomErrors( U8* ipData, U32 ipSize, U32 iErrors );

#endif /*_NAND_ERRORS_H_*/
/*-------------------------------- End Of File ------------------------------*/ 

