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
//   File         : MxSysUtils.c
//   Description  : This file declares system utility API.
//
/////////////////////////////////////////////////////////////////////////////
// macro; to make use of recursive mutex
#define _GNU_SOURCE

//********Place all include files ****
#include <stdio.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <openssl/aes.h>
#include <openssl/bio.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxConfigFileIo.h"
#include "MxLogs.h"
#include "MxSysUtils.h"
#include "MxAvResp.h"
#include "MxDcTask.h"
#include "MxSysTimer.h"
#include "MxGpio.h"
#include "MxAcmsTask.h"
#include "MxSysTime.h"
#include "MxTimeTrigFunc.h"
#include "MxCafeteria.h"

//******** Extern Variables **********
extern SYS_BASIC_PARA_t		SysBasicPara;

//******** Defines and Data Types ****
#define DEMO_FW_STATUS_FILE				"/usr/lib/DemoControl.txt"
#define SAMAY_FW_STATUS_FILE			"/usr/lib/SamayControl.txt"
#define SYS_APPL_SIGN_FILE				"/tmp/applPresent.txt"
#define SYS_SAMAY_APPL_SIGN_FILE		"/tmp/samayApplPresent.txt"

#define SUPREMA_NATIVE_FPR_HEADER			0x45
#define ISO19794_FPR_HEADER_BYTE0	0x46
#define ISO19794_FPR_HEADER_BYTE1	0x4d
#define ISO19794_FPR_HEADER_BYTE2	0x52
#define ISO19794_FPR_HEADER_BYTE3	0x00
#define FILE_BLOCK_SIZE				100000
#define MD5_CHECKSUM_SIZE			16

//******** Function Prototypes *******

//******** Global Variables **********

//******** Static Variables **********

//******** Function Definations ******

//*****************************************************************************
//	IsStringDataNumeric()
//	Param:
//		IN:	rxData		- data pointer
//			size	    - length of the received data
//
//		OUT: NONE
//	Returns:
//		None
//	Description:
//			This function return true if all the characters of input string are numeric or else will return false.
//
//*****************************************************************************
BOOL IsStringDataNumeric(CHARPTR rxData, UINT8 size)
{
	UINT8		currentChar;

	for(currentChar = 0; currentChar < size; currentChar++)
	{
		if (isdigit(rxData[currentChar]) == 0)          // checks whether character is numeric or not
		{
			return FAIL;                                // returns false if character is not numeric
		}
	}

	return SUCCESS;
}

//*****************************************************************************
//	IsStringDataInRange()
//	Param:
//		IN:	rxData		- data pointer
//			size	    - length of the received data
//
//		OUT: NONE
//	Returns:
//		None
//	Description:
//		This function return SUCCESS if string data is in given range otherwise FAIL
//*****************************************************************************
BOOL IsStringDataInRange(CHARPTR rxData, UINT8 size, UINT8 startValue, UINT8 endValue)
{
	UINT8		currentChar, value;

	for(currentChar = 0; currentChar < size; currentChar++)
	{
		if (isdigit(rxData[currentChar]) == 0)
		{
			return FAIL;
		}
		value = rxData[currentChar] - 0x30;
		if( (value < startValue) || (value > endValue) )
		{
			return FAIL;
		}
	}

	return SUCCESS;
}

//*****************************************************************************
//	GetUINT8ChkSum()
//	Param:
//		IN :	dataPtr	- points to data from where to start calculating checksum
//				length	- length of data used for checksum
//		OUT:	none
//	Returns:	calculated checksum value (in a byte)
//	Description:
//				It calculates checksum for given data.
//*****************************************************************************
UINT8 GetUINT8ChkSum(UINT8PTR dataPtr, UINT16 length)
{
	UINT16	loc;
	UINT8	chkSum = 0;

	for(loc = 0; loc < length; loc++)
	{
		chkSum += *(dataPtr + loc);
	}

	return chkSum;
}

//*****************************************************************************
//	GetUINT16FromStringAndLen()
//	Param:
//		IN:
//		OUT: NONE
//	Returns:
//		None
//	Description:
//			This function converts given string as per given length into integer.
//*****************************************************************************
UINT16 GetUINT16FromStringAndLen(CHARPTR sourcePtr, UINT8 len)
{
	CHAR 	tempPtr[5];

	strncpy(tempPtr, sourcePtr, len);
	tempPtr[len] = NULL_CHAR;
	return ((UINT16)atoi(tempPtr));
}

//*****************************************************************************
//	AsciiToInt()
//	Param:
//		IN :	CHARPTR		ascii
//		OUT:	UINT64PTR	integer
//	Returns:
//				SUCCESS/FAIL
//	Description:
//		This API converts an ASCII string to integer
//
//*****************************************************************************
BOOL AsciiToInt(CHARPTR ascii, UINT64PTR integer)
{
	BOOL 		status = SUCCESS;
	UINT8 		asciiLen;
	UINT64 		multiplicant;

	// check for invalid input/output parameters
	if( (ascii != NULL) && (integer != NULL) )
	{
		*integer = 0;
		multiplicant = 1;

		// run the loop, length times, and sum up the individual element in integer
		for(asciiLen = strlen(ascii); (asciiLen > 0); asciiLen--)
		{
			// check if the value falls between '0' to '9'
			if( (ascii[asciiLen - 1] >= '0') && (ascii[asciiLen - 1] <= '9') )
			{
				*integer += ( (ascii[asciiLen - 1] - '0') * multiplicant);
				multiplicant *= 10;
			}
			else
			{
				*integer = 0;
				status = FAIL;
				break;
			}
		}
	}
	else
	{
		status = FAIL;
	}
	return status;
}

//*****************************************************************************
// CheckACSFeatureStatus()
// Param
//		Input:		NONE
//		Output:		NONE
//		Return:		NONE
// Description:
//		This function will Update Access Control System Features Status
//*****************************************************************************
void CheckACSFeatureStatus(void)
{
	UpdateFirstIn();
	CheckTimeTrigFuncActivation();
	CheckForCafeteria();
	CheckAccessSchedule();
	DoorModeScheduleUpdate();
}

//*****************************************************************************
//	in_cksum()
//	Param:
//		IN :	*buf - buffer pointer on which to calculate checksum
//				sz	 - size of buffer in bytes to calculate checksum
//		OUT:	NONE
//	Returns:	16 bit checksum
//	Description:
//				This function calculates 16 bit checksum to send in ICMP Ping data.
//*****************************************************************************
UINT16 in_cksum(UINT16PTR buf1, UINT16PTR buf2, UINT16 size1, UINT16 size2)
{
	UINT32 sum = 0;
	UINT16 ans = 0;

	while(size1 > 1)
	{
		sum += *buf1++;
		size1 -= 2;
	}

	// If one byte left, add that to the sum.
	if(size1 == 1)
	{
		*(UINT8PTR)(&ans) = *(UINT8PTR)buf1;
		sum += ans;
	}

	ans = 0;

	// Add 16 bit words
	while(size2 > 1)
	{
		sum += *buf2++;
		size2 -= 2;
	}

	// If one byte left, add that to the sum.
	if(size2 == 1)
	{
		*(UINT8PTR)(&ans) = *(UINT8PTR)buf2;
		sum += ans;
	}

	// Convert 32 bit sum to 16bit 2s-complement form
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	ans = ~sum;

	return (ans);
}

//*****************************************************************************
//	GetFileListInDir()
//	Param:
//		IN:	 CHARPTR dirPath,
//			 CHARPTR fileNameInitial
//			 UINT16 fileNameArrySize
//			 UINT16 maxResult
//		OUT: CHARPTR *result
//	Returns: filecount
//	Description:
//			This function returns no. of files found with specified filename initials in
//		specified directory(it wil not list file recursively), hidden file is also not listed.
//		it will search file upto maxResult.
//		Incase if just want to calculate number of file in directory than pass maxResult > 0
//		For ex GetFileListInDir(xyz,NULL,NULL,0,1(here value is not significant));.
//*****************************************************************************
UINT16 GetFileListInDir(CHARPTR dirPath, CHARPTR fileNameInitial, CHARPTR result, UINT16 fileNameArrySize, UINT16 maxResult)
{
	DIR					*d;
	struct dirent		*dir;
	struct dirent		dirBuf;
	UINT16				fileCount = 0;

	d = opendir(dirPath);
	if(d != NULL)
	{
		while( (readdir_r(d, &dirBuf, &dir) == 0) && (dir != NULL) && (maxResult > 0) )
		{
			if(dir->d_name[0] == '.')
			{
				continue;
			}
			if( (fileNameInitial == NULL) || (strncmp(fileNameInitial, dir->d_name, strlen(fileNameInitial) ) == 0) )
			{
				if(result != NULL)
				{
					strncpy( (result + (fileNameArrySize*fileCount) ), dir->d_name, fileNameArrySize);
					*( result + (fileNameArrySize*fileCount) + (fileNameArrySize - 1) ) = '\0';
					maxResult--;
				}
				fileCount++;
			}
		}
		closedir(d);
	}
	return(fileCount);
}

//*****************************************************************************
//	CheckFprTemplateFormat()
//	Param:
//		IN:	 dataPtr
//		OUT:
//	Returns: fail/success
//	Description:
//	This Function Will read Finger Template And Check that the template format
//	that device supports and format of current received template is same or not
//	if its not same  it will return fail.
//*****************************************************************************
BOOL CheckFprTemplateFormat(UINT8PTR dataPtr)
{
	EPRINT(WEB_API_LOG, "dataPtr[0] %02X %02X %02X %02X %02X %02X ", dataPtr[0], dataPtr[1], dataPtr[2], dataPtr[3], dataPtr[8], dataPtr[9]);
	if( (dataPtr[0] == ISO19794_FPR_HEADER_BYTE0) &&
		(dataPtr[1] == ISO19794_FPR_HEADER_BYTE1) &&
		(dataPtr[2] == ISO19794_FPR_HEADER_BYTE2) &&
		(dataPtr[3] == ISO19794_FPR_HEADER_BYTE3) &&
		(dataPtr[8] == 0x00) && (dataPtr[9] == 0x00) &&
		(SysBasicPara.fprTemplateFormat == ISO_19794_FORMAT))
	{
		return SUCCESS;
	}
	else if(SysBasicPara.fprTemplateFormat == SUPREMA_NATIVE_FORMAT)
	{
		return SUCCESS;
	}
	else
	{
		return FAIL;
	}
}

//*****************************************************************************
//	IsDemoSoftware()
//	Param:
//		IN:	NONE
//
//		OUT:BOOL DemoModeEnable
//
//	Returns:SUCCESS/FAIL
//
//	Description:
//			This function will read system is configured in demo mode or not
//		and system is demo or not is decide by another application so till another
//		application will not write file we return FAIL.
//
//*****************************************************************************
BOOL IsDemoSoftware(void)
{
	FILE	* fp;

	fp = fopen(DEMO_FW_STATUS_FILE, "r");
	if(fp != NULL)
	{
		fclose(fp);
		return YES;
	}
	else
	{
		return NO;
	}
}

//*****************************************************************************
//	IsSamaySoftware()
//	Param:
//		IN:	NONE
//
//		OUT:BOOL SamayModeEnable
//
//	Returns:SUCCESS/FAIL
//
//	Description:
//			This function will read system is configured in Samay mode or not
//		and system is Samay or not is decide by another application so till another
//		application will not write file we return FAIL.
//
//*****************************************************************************
BOOL IsSamaySoftware(void)
{
	FILE	* fp;
	fp = fopen(SAMAY_FW_STATUS_FILE, "r");
	if(fp != NULL)
	{
		fclose(fp);
		return YES;
	}
	else
	{
		return NO;
	}
}

//*****************************************************************************
//	WriteApplPresentFile()
//	Param:
//		IN:	NONE
//
//		OUT:NONE
//
//	Returns:NONE
//
//	Description:
//			This function will generate present.txt file into tmp directory to
//		indicate that this application is not much old and it is capable to restrict
//		demo board to act as full version board
//
//*****************************************************************************
void WriteApplPresentFile(void)
{
	FILE	* fp;

	fp = fopen(SYS_APPL_SIGN_FILE, "w");
	fclose(fp);
}

//*****************************************************************************
//	ConvertSysInDemo()
//	Param:
//		IN:	NONE
//
//		OUT:NONE
//
//	Returns:NONE
//
//	Description:
//			This function will convert system in demo software mode.
//*****************************************************************************
void ConvertSysInDemo(void)
{
	system("fw_setenv fwType demo");
	SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
}

//*****************************************************************************
//	ConvertSysInSamay()
//	Param:
//		IN:	NONE
//
//		OUT:NONE
//
//	Returns:NONE
//
//	Description:
//			This function will convert system in samay software mode.
//*****************************************************************************
void ConvertSysInSamay(void)
{
	system("fw_setenv appType samay");
	SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
}

//*****************************************************************************
//	WriteSamayApplPresentFile()
//	Param:
//		IN:	NONE
//
//		OUT:NONE
//
//	Returns:NONE
//
//	Description:
//			This function will generate present.txt file into tmp directory to
//		indicate that this application is not much old and it is capable to restrict
//		samay board to act as full version board
//
//*****************************************************************************
void WriteSamayApplPresentFile(void)
{
	FILE	* fp;

	fp = fopen(SYS_SAMAY_APPL_SIGN_FILE, "w");
	fclose(fp);
}

//*****************************************************************************
//	verifyChecksumFile()
//	Param:
//		IN:	CHARPTR fileName
//		OUT: NONE
//	Returns:
//			SUCCESS/FAIL
//	Description:
//			This function calculate MD5 checksum and create new file with
// 			checksum appended in filename.
//*****************************************************************************
BOOL verifyChecksumFile(CHARPTR fileName)
{
	INT32		fd, chksumFd;
	UINT8		buffer[FILE_BLOCK_SIZE];
	INT32		readSize;
	MD5_CTX		md5Context;
	UINT8		finalChecksum[MD5_CHECKSUM_SIZE];
	UINT8		readChecksum[MD5_CHECKSUM_SIZE];
	INT32		readlen = 0;
	CHAR		chksumFileName[100];

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

	// read checksum from checksum file.
	sprintf(chksumFileName, "%s_%s", fileName, "Checksum");
	chksumFd = open(chksumFileName, O_RDWR | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(chksumFd < 0)
	{
		DPRINT(BU_LOG,"unable to open Checksum file\n");
		return FAIL;
	}
	do
	{
		readSize = read(chksumFd, (VOIDPTR)buffer, FILE_BLOCK_SIZE);
		if(readSize > 0)
		{
			if((readlen+readSize) <= MD5_CHECKSUM_SIZE)
			{
				memcpy(&readChecksum[readlen], (VOIDPTR)buffer, readSize);
				readlen += readSize;
			}
		}
	}while(readSize > 0);

	close(chksumFd);
	if(memcmp(readChecksum, finalChecksum, MD5_CHECKSUM_SIZE) != 0)
	{
		DPRINT(BU_LOG,"Checksum not verified\n");
		return FAIL;
	}

	DPRINT(BU_LOG,"VerifyChksum:checksum Verified for [%s]\n", fileName);
	return SUCCESS;
}
