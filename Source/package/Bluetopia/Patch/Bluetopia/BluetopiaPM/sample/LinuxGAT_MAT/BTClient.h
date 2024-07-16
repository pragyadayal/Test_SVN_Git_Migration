/*****< linuxgatm_clt.h >******************************************************/
/*      Copyright 2012 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  LINUXGATM_CLT - Simple Linux application using Bluetopia Platform Manager */
/*                  Generic Attribute Profile (GATT) Manager Application      */
/*                  Programming Interface (API) - Client Role Only.           */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   10/27/12  T. Cook        Initial creation. (Based on LinuxHFRM_HF)       */
/******************************************************************************/
#ifndef __LINUXGATM_CLTH__
#define __LINUXGATM_CLTH__

#define 	MAX_REMOTE_DEVICE	5
#define 	AES_256_KEY_SIZE_IN_BITS 256

// e.g. tick 1 is systick and tick 2 is mark tick or
// tick 1 is main tick and tick 2 is a tick which you want to decrement from main tick
#define GetAbsTickDiff(tick1, tick2)	( (tick1 < tick2) ? (tick1 + (0xFFFFFFFF - tick2)) : (tick1 - tick2) )
#define 	VAL_OF_1MS			1000
#define 	NANO_TO_MILI_SEC	1000000



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

typedef enum
{
	RESP_STATUS_NONE = 0,		// 0
	RESP_USER_ALLOWED,			// 1
	RESP_WAITING_SECOND_CRED,	// 2
	RESP_DEVICE_BUSY,			// 3
	RESP_WRONG_DATA,			// 4
	RESP_INSUFFICIENT_DATA,		// 5
	RESP_WRONG_FORMAT,			// 6
	RESP_USER_DENIED,			// 7

	RESP_MAX_STATUS,
}READ_STATUS_RESP_e;

void AesCbc256EncryptData(unsigned char* inData, unsigned char* outData, unsigned int dataLen, unsigned char* keyPtr, unsigned char* iv, unsigned char mode);
unsigned short Crc16A(unsigned char* dataPtr, unsigned int len, unsigned char * crcPtr);
unsigned short Reflect(unsigned short number,unsigned int size);
UINT32 GetSysTick(void);
UINT32 ElapsedTick(UINT32 markTick);
#endif

