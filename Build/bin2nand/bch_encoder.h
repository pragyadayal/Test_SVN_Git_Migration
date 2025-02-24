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
*   Filename   : ecc.h
*
*______________________________________________________________________________
*
*   History:
*   28-Jul-08   Martin Zarzycki
*               Creation
* 
*____________________________________________________________________________*/

#ifndef _BCH_ENCODER_H_
#define _BCH_ENCODER_H_

/*------------------------------- Include Files -----------------------------*/

#include "romcode_types.h"

/*-------------------------------- Definitions ------------------------------*/

/*----------------------------------- Types ---------------------------------*/

/*-------------------------------  Functions --------------------------------*/

STATUS BCH_Init( U8 iBitCorrection );
STATUS BCH_Encode(U8* ipInput, U8* ipOutput, U32 iLength );

#endif /*_BCH_ENCODER_H_*/
/*-------------------------------- End Of File ------------------------------*/ 

