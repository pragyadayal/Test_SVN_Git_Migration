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
//   Project      : NVR ( Network Video Recorder)
//   Owner        : Darshan Patel
//   File         : fsUpgrade.c
//   Description  : This file creates Firmware Upgrade file. It appends MD5
//					cheksum in the file.
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

typedef enum
{
	UBIFS_FILESYS,
	JFFS2_FILESYS,
	MAX_FILESYS_TYPE,
}FILESYS_TYPE_e;

//******** Function Prototypes *******
static BOOL CreateKernelFsImage(CHARPTR kernelFileName, CHARPTR fsFileName, CHARPTR fileSysType, CHARPTR outputImgName, CHARPTR bootArg);
static BOOL ConcateImagesWithHeader(CHARPTR kernelFileName, CHARPTR fsFileName, CHARPTR fileSysType, CHARPTR outputImgName, CHARPTR bootArg);
static BOOL AddChecksumInImage(CHARPTR fileName);

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

	if (argc == 6)
	{
		printf("fsUpgrade:Image generation started\n");
		printf("fsUpgrade:argv1=%s\n", argv[1]);
		printf("fsUpgrade:argv2=%s\n", argv[2]);
		printf("fsUpgrade:argv3=%s\n", argv[3]);
		printf("fsUpgrade:argv4=%s\n", argv[4]);
		printf("fsUpgrade:argv5=%s\n", argv[5]);
		if(CreateKernelFsImage(argv[1], argv[2], argv[3], argv[4], argv[5]) == FAIL)
		{

			sprintf(buffer, "rm -rf %s", argv[4]);
			system(buffer);
			printf("fsUpgrade:FAIL to generate image\n");
		}
		else
		{
			printf("fsUpgrade:Image is generated Successfully \n");
		}
	}
	else
	{
		printf("fsUpgrade:Invalid argument please give argument in following order\n");
		printf("<binary name> <kernel file name> <fs file name> <fs type> <output image name> <boot arg>\n");
	}
	return 0;
}

//*****************************************************************************
//	ConcateImagesWithHeader()
//	Param:
//		IN:	CHARPTR kernelFileName
//			CHARPTR fsFileName
//			CHARPTR outputImgName
//		OUT: NONE
//	Returns:
//			SUCCESS/FAIL
//	Description:
//			This function concate two input images and generate one output image.
//*****************************************************************************
static BOOL ConcateImagesWithHeader(CHARPTR kernelFileName, CHARPTR fsFileName, CHARPTR fileSysType, CHARPTR outputImgName, CHARPTR bootArg)
{
	INT32		outFd;
	INT32		kernFd;
	INT32		fsFd;
	UINT8		buffer[FILE_BLOCK_SIZE];
	INT32		readSize;
	INT32		imageSize;
	UINT8		fsType;
	INT32		bootArgLen;

	printf("fsUpgrade:Image concatenation started\n");
	outFd = open(outputImgName, O_RDWR | O_SYNC | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(outFd < 0)
	{
		return FAIL;
	}
	kernFd = open(kernelFileName, O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(kernFd < 0)
	{
		close(outFd);
		return FAIL;
	}
	fsFd = open(fsFileName, O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fsFd < 0)
	{
		close(outFd);
		close(kernFd);
		return FAIL;
	}

	printf("fsUpgrade:FS concatenation started\n");
	do
	{
		readSize = read(fsFd, (VOIDPTR)buffer, FILE_BLOCK_SIZE);
		if(readSize > 0)
		{
			if(write(outFd, (VOIDPTR)buffer, readSize) != readSize)
			{
				goto concateFailure;
			}
		}
	}while(readSize > 0);
	printf("fsUpgrade:FS concatenation completed\n");
	printf("fsUpgrade:Kernel concatenation started\n");
	do
	{
		readSize = read(kernFd, (VOIDPTR)buffer, FILE_BLOCK_SIZE);
		if(readSize > 0)
		{
			if(write(outFd, (VOIDPTR)buffer, readSize) != readSize)
			{
				goto concateFailure;
			}
		}
	}while(readSize > 0);
	printf("fsUpgrade:kernel concatenation completed\n");

	//concating kernel image size
	imageSize = lseek(kernFd, 0, SEEK_END);
	if(imageSize == -1)
	{
		goto concateFailure;
	}
	if(write(outFd, (VOIDPTR)&imageSize, sizeof(imageSize) ) != sizeof(imageSize))
	{
		goto concateFailure;
	}
	//concating filesystem type
	if(strcmp(fileSysType, "jffs2") == 0)
	{
		fsType = JFFS2_FILESYS;
	}
	else if(strcmp(fileSysType, "ubifs") == 0)
	{
		fsType = UBIFS_FILESYS;
	}
	else
	{
		goto concateFailure;
	}
	if(write(outFd, (VOIDPTR)&fsType, sizeof(fsType) ) != sizeof(fsType) )
	{
		goto concateFailure;
	}
	
	bootArgLen = strlen(bootArg);
	printf("bootArgLen =%d\n", bootArgLen);
	printf("bootArg =%s\n", bootArg);
	if(bootArgLen > 0)
	{
		if(write(outFd, (VOIDPTR)bootArg, bootArgLen) != bootArgLen)
		{
			goto concateFailure;
		}	
	}
	if(write(outFd, (VOIDPTR)&bootArgLen, sizeof(bootArgLen) ) != sizeof(bootArgLen) )
	{
		goto concateFailure;
	}

	printf("fsUpgrade:image concatenation completed\n");
	close(outFd);
	close(kernFd);
	close(fsFd);
	return SUCCESS;

concateFailure:
	close(outFd);
	close(kernFd);
	close(fsFd);
	return FAIL;
}

//*****************************************************************************
//	AddChecksumInImage()
//	Param:
//		IN:	CHARPTR fileName
//		OUT: NONE
//	Returns:
//			SUCCESS/FAIL
//	Description:
//			This function calculate MD5 checksum and append in given file.
//*****************************************************************************
static BOOL AddChecksumInImage(CHARPTR fileName)
{
	INT32		fd;
	UINT8		buffer[FILE_BLOCK_SIZE];
	INT32		readSize;
	MD5_CTX		md5Context;
	UINT8		finalChecksum[MD5_CHECKSUM_SIZE];

	printf("fsUpgrade:image checksum started\n");
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

	//now append final checksum into the file
	if(lseek(fd, 0, SEEK_END) == -1)
	{
		close(fd);
		return FAIL;
	}
	if(write(fd, (VOIDPTR)finalChecksum, MD5_CHECKSUM_SIZE) != MD5_CHECKSUM_SIZE)
	{
		close(fd);
		return FAIL;
	}
	printf("fsUpgrade:image checksum completed\n");
	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	CreateKernelFsImage()
//	Param:
//		IN:	CHARPTR kernelFileName
//			CHARPTR fsFileName
//			CHARPTR fileSysType
//			CHARPTR outputImgName
//		OUT: NONE
//	Returns:
//			SUCCESS/FAIL
//	Description:
//			This function create one output image from two input image.
//*****************************************************************************
static BOOL CreateKernelFsImage(CHARPTR kernelFileName, CHARPTR fsFileName, CHARPTR fileSysType, CHARPTR outputImgName, CHARPTR bootArg)
{
	if(ConcateImagesWithHeader(kernelFileName, fsFileName, fileSysType, outputImgName, bootArg) == FAIL)
	{
		return FAIL;
	}

	if(AddChecksumInImage(outputImgName) == FAIL)
	{
		return FAIL;
	}

	return SUCCESS;
}
