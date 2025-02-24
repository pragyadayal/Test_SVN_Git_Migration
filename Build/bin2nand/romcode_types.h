/* ____________________________________________________________________________
*
*            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION
*
*  Property of Texas Instruments
*  For Unrestricted Internal Use Only
*  Unauthorized reproduction and/or distribution is strictly prohibited.
*  This product is protected under copyright law and trade secret law as an
*  unpublished work.
*  Created 2005, (C) Copyright 2005 Texas Instruments.  All rights reserved.
*
*  Component  : General
*
*  Filename   : romcode_types.h
*
*  Description: This file contains general definitions for use with OMAP SW.
*
*______________________________________________________________________________
*
*  History:
*  16-Sep-03 Nicholas Papachristos
*            1st Creation. Added basic types.
*  17-May-05 Nicolas Chevillot
*            Modified Types, port to 3430
*  05-Jun-05 Piotr Murawski
*            Some corrections and cleanup
*
*____________________________________________________________________________*/

#ifndef _ROMCODE_TYPES_H
#define _ROMCODE_TYPES_H


/*_________________________ Exported Type Definitions ________________________*/

typedef unsigned char            U8;
typedef unsigned short           U16;
typedef unsigned int             U32;
typedef unsigned long long       U64;

typedef volatile U8  VU8;
typedef volatile U16 VU16;
typedef volatile U32 VU32;
typedef volatile U64 VU64;

/*___________________________________________________________________________*/

#ifndef FALSE
typedef enum
{
	FALSE,
	TRUE
	
}	Boolean;

#define BOOL Boolean

#endif

typedef volatile Boolean VBOOL;

/*___________________________________________________________________________*/
/*
    This is the enum to be used for all function returns, and some statuses. In
    order to avoid unnecessary status translation, add a new status here.
*/
typedef enum
{
   NO_ERROR,
   FAILED,
   TIMED_OUT,
   PARAM_ERROR,
   WAITING,
   MALLOC_ERROR,
   INVALID_POINTER
   
} STATUS;


/*___________________________________________________________________________*/

#ifndef NULL
#define NULL (void*)0
#endif

/*___________________________________________________________________________*/

#endif	/* _ROMCODE_TYPES_H */

/*________________________________ End Of File _______________________________*/

