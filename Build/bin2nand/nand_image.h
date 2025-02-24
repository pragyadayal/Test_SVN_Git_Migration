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
*   Filename   : nand_image.h
*
*______________________________________________________________________________
*
*   History:
*   28-Jul-08   Martin Zarzycki
*               Creation
*____________________________________________________________________________*/

#ifndef _NAND_IMAGE_H_
#define _NAND_IMAGE_H_

/*------------------------------- Include Files -----------------------------*/

#include <stdio.h>

#include "romcode_types.h"
#include "nand.h"

/*-------------------------------- Definitions ------------------------------*/

/*----------------------------------- Types ---------------------------------*/

/*-------------------------------  Functions --------------------------------*/

STATUS ni_CreateImage( NAND_Params_t* ipParams, U8* ipData, FILE* ipFile );

#endif /*_NAND_BCH_H_*/
/*-------------------------------- End Of File ------------------------------*/ 

