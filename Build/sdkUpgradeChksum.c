//////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Telecom Pvt. Ltd., Baroda, India.
//   Project      : Gate Controller
//   Owner        : Jaydevsinh Jadeja
//   File         : sdkUpgradeChksum.c
//   Description  : This file creates checksum file for sdk upgrade.
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <unistd.h>


//******** Extern Variables **********

//******** Defines and Data Types ****
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
};

//###########################################################################

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



#define FILE_BLOCK_SIZE				100000
#define MD5_CHECKSUM_SIZE			16

//******** Function Prototypes *******
static BOOL CreateChecksumFile(CHARPTR fileName);

//******** Global Variables **********

//******** Static Variables **********

//******** Function Definations ******

//*****************************************************************************
//	main()
//	Param:
//		IN:	 INT32 argc -
//			 CHARPTR argv[]
//		OUT: NONE
//	Returns:
//	Description:
//			This function identify kernel file name, filesystem file name and filesystem
//		type and generate image by combining all the images and headers.
//			to call this binary format of argument is given bellow.
//			<binary name> <kernel file name> <fs file name> <fs type> <output image name>
//*****************************************************************************
INT32 main(INT32 argc, CHARPTR *argv)
{
	char	buffer[100];
	INT32 	tmpIdx;

	if (argc > 1)
	{
		printf("sdkUpgradeChksum:Calculate Checksum process started for sdk upgrade\n");
		
		for(tmpIdx=1; tmpIdx < argc; tmpIdx++)
		{
			printf("sdkUpgradeChksum:argv[%u] = [%s]\n", tmpIdx, argv[tmpIdx]);
		}
		
		for(tmpIdx=1; tmpIdx < argc; tmpIdx++)
		{
			CreateChecksumFile(argv[tmpIdx]);
		}
	}
	else
	{
		printf("sdkUpgradeChksum:Invalid argument please give argument in following order\n");
		printf("<binary name> <filename1> <filename2> ..... \n");
	}
	return 0;
}

//*****************************************************************************
//	CreateChecksumFile()
//	Param:
//		IN:	CHARPTR fileName
//		OUT: NONE
//	Returns:
//			SUCCESS/FAIL
//	Description:
//			This function calculate MD5 checksum and create new file with 
// 			checksum appended in filename.
//*****************************************************************************
static BOOL CreateChecksumFile(CHARPTR fileName)
{
	INT32		fd, outFd;
	UINT8		buffer[FILE_BLOCK_SIZE];
	INT32		readSize;
	MD5_CTX		md5Context;
	UINT8		finalChecksum[MD5_CHECKSUM_SIZE];
	CHAR		outFileName[100];
	CHARPTR		dotPtr;

	printf("sdkUpgradeChksum:calculate checksum started for [%s]\n", fileName);
	fd = open(fileName, O_RDWR | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		return FAIL;
	}

	MD5_Init(&md5Context);
	do
	{
		readSize = read(fd, (VOIDPTR)buffer, FILE_BLOCK_SIZE);
		if(readSize > 0)
		{
			MD5_Update(&md5Context, (VOIDPTR)buffer, readSize);
		}
	}while(readSize > 0);

	// get Final MD5 sum
	MD5_Final(finalChecksum, &md5Context);
	close(fd);

	// craete new checksum output file using given name. 
	sprintf(outFileName, "%s_%s", fileName, "Checksum");
	printf("sdkUpgradeChksum: going to create file for checksum[%s]\n", outFileName);
	outFd = open(outFileName, O_CREAT | O_RDWR | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(outFd < 0)
	{
		printf("sdkUpgradeChksum: unable to craete new file for checksum\n");
		return FAIL;
	}
	if(write(outFd, (VOIDPTR)finalChecksum, MD5_CHECKSUM_SIZE) != MD5_CHECKSUM_SIZE)
	{
		close(outFd);
		sprintf(buffer, "rm -rf %s", outFileName);
		system(buffer);
		printf("sdkUpgradeChksum:FAIL to calculate checksum\n");
		return FAIL;
	}
	
	close(outFd);
	printf("sdkUpgradeChksum:calculate checksum checksum completed\n");
	return SUCCESS;
}


