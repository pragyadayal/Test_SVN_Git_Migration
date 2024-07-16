#if !defined MXTYPEDEF_H
#define MXTYPEDEF_H
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*                              Matrix Telecom
*               (c) Copyright 2005, Matrix Telecom Pvt. Ltd.
* 
* All right reserved. Matrix's source code is an unpublished work and the use 
* of copyright notice does not imply otherwise. This source code contains 
* confidential, trade secret material of Matrix Telecom. Any attempt or 
* participation in deciphering, decoding, reverse engineering or in any way 
* altering the source code is strictly prohibited, unless the prior written 
* consent of Matrix is obtained.
*
* @FILENAME
* Typedef.h
*
* @DESCRIPTION
* Include this file in your project directory. 
*
* @HISTORY
*
* V1R00 : 20-Nov-2008 : Matrix R&D Team
*   1. Initial Release
*
****************************************************************************/


//###################################################################
// @INCLUDES
//###################################################################

//###################################################################
// @DEFINES 
//###################################################################
#ifndef NONE
#define	NONE		0xff
#endif

#ifndef FALSE
#define	FALSE		0
#endif

#ifndef TRUE
#define	TRUE		1
#endif

#define	NULL_CHAR	0

enum
{
	CLEAR = 0,
	GET = 0,
	SET,
	//
	INVALID = 0,
	VALID,
	//
	ABSENT = 0,
	PRESENT,
	//
	DISABLED = 0,
	ENABLED,
	MAX_BOOL_ENUM,
	//
	OFF = 0,
	ON,
	//
	CLOSE = 0,
	OPEN,
	//
	NO = 0,
	YES,
	WAIT,
	//
	DOWN = 0,
	UP,
	//
	LOW = 0,
	HIGH,
	//
	FREE = 0,
	BUSY,
	//
	INACTIVE = 0,
	ACTIVE,
	//
	UNMASK = 0,
	MASK,
	//
	DENIED = 0,
	ALLOWED,
	//
	LESS = -1,
	EQUAL = 0,
	GREATER,
	//
	NIL = -1,
	FAIL = 0,
	SUCCESS,
	//
	BUF_FREE = 0,
	BUF_FULL,
	BUF_PARTIAL_FULL,
	//
	Q_STATUS_FREE = 0,
	Q_STATUS_ALLOCATED,
	Q_STATUS_PENDING,
	Q_STATUS_PROCESSING,
	//
	PROCESS_FURTHER = 0,
	PROCESS_DONE,
};

//###########################################################################
// @MACRO
//###########################################################################
#define	GENERATE_ENUM(ENUM)			ENUM,
#define	GENERATE_ENUM_STR(ENUM) 	#ENUM,

//###########################################################################
// @DATATYPE
//###########################################################################

/* @NOTE
 * Change according to your platform (KEIL, C55x, MpLab, ARM or others).
 * Use standard "char" for normal character variable or character strings. 
 * Do no use other than these datatypes in projects. 
 */

typedef unsigned char			BOOL;

typedef char					CHAR;

typedef signed char				INT8;
typedef unsigned char			UINT8;

typedef signed short			INT16;
typedef unsigned short			UINT16;

typedef signed int				INT32;
typedef unsigned int  			UINT32;

typedef signed long long		INT64;
typedef unsigned long long 		UINT64;

typedef float					FLOAT32;
typedef long double				FLOAT64;

typedef INT8 *					INT8PTR;
typedef char *					CHARPTR;
typedef UINT8 *					UINT8PTR;
typedef UINT8 const *			UINT8CONSTPTR;

typedef INT16 *					INT16PTR;
typedef UINT16 *				UINT16PTR;
typedef UINT16 const *			UINT16CONSTPTR;

typedef INT32 *					INT32PTR;
typedef UINT32 *				UINT32PTR;
typedef UINT32 const *			UINTCONST32PTR;

typedef INT64 *					INT64PTR;
typedef UINT64 *				UINT64PTR;
typedef UINT64 const*			UINT64CONSTPTR;

typedef void					VOID;
typedef void *					VOIDPTR;

typedef union
{
	UINT16	word;
	UINT8	byte[2];
}WORD_u;

typedef union 
{
 	UINT32 	dWord;				// double word	
	UINT16	word[2];			// word- 2 no.s
	UINT8 	byte[4];			// byte- 4 no.s 
}DOUBLE_WORD_u;					

typedef union 
{
 	UINT32 	dWord[2];			// double word- 2 no.s	
	UINT16	word[4];			// word- 4 no.s
	UINT8 	byte[8];			// byte- 8 no.s 
}QUAD_WORD_u;

//###########################################################################
// @PROTOTYPES
//###########################################################################

#endif //MXTYPEDEF_H
