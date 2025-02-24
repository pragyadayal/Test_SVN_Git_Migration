///////////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Comsec Pvt. Ltd., Baroda, India.
//   Project      : Access Control System
//   Created By   : Kuldip Gor
//   File         : MxWebAPIUserPhoto.c
//   Description  : Brief but meaningful description of what file provides.
//
///////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include <stdlib.h>
#include <sys/stat.h>

#include "MxWebAPIUserPhoto.h"
#include "MxConfig.h"
#include "MxConfigUpdateHandler.h"
#include "MxConfigFileIo.h"
#include "MxEnrollment.h"
#include "MxBlockDevice.h"
#include "MxUserConfig.h"

//******** Extern Variables **********
extern const WEB_API_TABLE_INFO_t 	tableInfo[MAX_WEB_API_QUERY];
extern MMC_INFO_t					MmcInfo;


//******** Defines and Data Types ****
#define INVALID_MANDATORY_FIELD		0xFFFF
#define MAX_SIZE_USER_PHOTO			(50 * 1024)

typedef enum
{
	PHOTO_FORMAT_JPEG,
	PHOTO_FORMAT_JPG,
	PHOTO_FORMAT_PNG,
	PHOTO_FORMAT_BMP,
	MAX_PHOTO_FORMAT,
}PHOTO_FORMAT_e;

//******** Function Prototypes *******
static BOOL IsUserPhotoPresent(UINT32 userIdx, CHARPTR filePath, UINT32PTR fileSize, CHARPTR extStr);
static void GetUserPhotoPath(UINT32 userIdx, CHARPTR filePath);
static void DeleteUserPhoto(UINT32 userIdx);


//******** Global Variables **********

//******** Static Variables **********
static UINT16		mandatoryFieldUserIdx = INVALID_MANDATORY_FIELD;
static UINT16		mandatoryFieldPhotoFormat = INVALID_MANDATORY_FIELD;
static const CHAR 	photoFormatExt[MAX_PHOTO_FORMAT][5] = {"jpeg", "jpg", "png", "bmp"};

//******** Function Definitions ******

//*****************************************************************************
//	GetQueryUserPhoto()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function handles Get action request for UserPhoto query
//
//*****************************************************************************
UINT8 GetQueryUserPhoto(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	UINT8 	parseStatus, elementIdx, getElementStatus;
	CHAR 	tempFieldName[MAX_FIELD_NAME_LENGTH], tempFieldValue[MAX_FIELD_VALUE_LENGTH], filePath[255], extStr[10];
	UINT32	fileSize;
	BOOL 	invalidArgValueF = FALSE;
	CHAR	tempUserId[MAX_FIELD_VALUE_LENGTH];

	mandatoryFieldUserIdx = INVALID_MANDATORY_FIELD;

	//Check for response format
	if(ParseMandatoryField(pRXDataFile, "format", &tempFieldValue[0], MAX_FIELD_VALUE_LENGTH) == SUCCESS)
	{
		if(GetResponseType(tempFieldValue, &(pHeader->queryResp.type)) == FAIL)
		{
			WriteErrorInvalidArgumentValue(pTXDataFile, "format", tempFieldValue, pHeader->queryResp.type);
			return FAIL;
		}
	}

	while( (parseStatus = ParseAPIField(pRXDataFile, &tempFieldName[0], MAX_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_FIELD_VALUE_LENGTH) ) != FAIL)
	{
		if( (getElementStatus = GetElementIdx(pHeader->query, tempFieldName, &elementIdx)) == SUCCESS)
		{
			switch(elementIdx)
			{
				case USER_PHOTO_ARG_USER_ID:
					if(ValidateAlphaNumIdStr(tempFieldValue) == YES)
					{
						if(IsAlphaNumIdValid(tempFieldValue, &mandatoryFieldUserIdx) == NO)
						{
							WriteResponseCode(pTXDataFile, WEB_API_RESP_USER_ID_NOT_FOUND, pHeader->queryResp.type);
							return FAIL;
						}
						else
						{
							//To be used for sending file name
							strcpy(tempUserId, tempFieldValue);
						}
					}
					else
					{
						invalidArgValueF = TRUE;
					}
					break;

				default:
					invalidArgValueF = TRUE;
					break;
			}
		}
		else if(getElementStatus == ARG_RESP_FORMAT)
		{
			if(GetResponseType(tempFieldValue, &(pHeader->queryResp.type)) == FAIL)
			{
				invalidArgValueF = TRUE;
			}
		}
		else if(parseStatus != LAST_FIELD_WITHOUT_DATA)
		{
			 invalidArgValueF = TRUE;
		}

		if(invalidArgValueF == TRUE)
		{
			WriteErrorInvalidArgumentValue(pTXDataFile, tempFieldName, tempFieldValue, pHeader->queryResp.type);
			return FAIL;
		}

		if( (parseStatus == LAST_FIELD) || (parseStatus == LAST_FIELD_WITHOUT_DATA) )
		{
			if(mandatoryFieldUserIdx == INVALID_MANDATORY_FIELD)
			{
				WriteErrorIncompleteCommand(pTXDataFile, pRXDataFile, pHeader->queryResp.type);
				return FAIL;
			}

			if(MmcInfo.status != MMC_CONNECTED)
			{
				WriteResponseCode(pTXDataFile, WEB_API_RESP_MEMORY_CARD_NOT_FOUND, pHeader->queryResp.type);
				return FAIL;
			}

			if(IsUserPhotoPresent(mandatoryFieldUserIdx, filePath, &fileSize, extStr) == YES)
			{
				sprintf(pHeader->queryResp.filePath, "%s", filePath);
				sprintf(pHeader->queryResp.fileName, "%s%s", tempUserId, extStr);
				pHeader->queryResp.fileSize = fileSize;
				pHeader->queryResp.type = RESP_TYPE_FILE;
			}
			else
			{
				WriteResponseCode(pTXDataFile, WEB_API_RESP_NO_RECORD_FOUND, pHeader->queryResp.type);
				return FAIL;
			}
			break;
		}
	}
	return SUCCESS;
}

//*****************************************************************************
//	SetQueryCredential()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function handles Set action request for Credential query
//
//*****************************************************************************
UINT8 SetQueryUserPhoto(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	UINT8 			parseStatus, elementIdx, getElementStatus;
	CHAR 			tempFieldName[MAX_FIELD_NAME_LENGTH], tempFieldValue[MAX_FIELD_VALUE_LENGTH], sysBuf[255], photoPath[255];
	UINT64 			tempFieldDigit;
	UINT32			contentLen = 0;
	FILE 			* rxContentDataFile;
	BOOL			invalidArgValueF = FALSE;
	struct stat 	fileStat;

	mandatoryFieldUserIdx = INVALID_MANDATORY_FIELD;
	mandatoryFieldPhotoFormat = INVALID_MANDATORY_FIELD;

	//Check for response format
	if(ParseMandatoryField(pRXDataFile, "format", &tempFieldValue[0], MAX_FIELD_VALUE_LENGTH) == SUCCESS)
	{
		if(GetResponseType(tempFieldValue, &(pHeader->queryResp.type)) == FAIL)
		{
			WriteErrorInvalidArgumentValue(pTXDataFile, "format", tempFieldValue, pHeader->queryResp.type);
			return FAIL;
		}
	}

	while( (parseStatus = ParseAPIField(pRXDataFile, &tempFieldName[0], MAX_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_FIELD_VALUE_LENGTH) ) != FAIL)
	{
		if( (getElementStatus = GetElementIdx(pHeader->query, tempFieldName, &elementIdx)) == SUCCESS)
		{
			switch(elementIdx)
			{
				case USER_PHOTO_ARG_USER_ID:
					if(ValidateAlphaNumIdStr(tempFieldValue) == YES)
					{
						if(IsAlphaNumIdValid(tempFieldValue, &mandatoryFieldUserIdx) == NO)
						{
							WriteResponseCode(pTXDataFile, WEB_API_RESP_USER_ID_NOT_FOUND, pHeader->queryResp.type);
							return FAIL;
						}
					}
					else
					{
						invalidArgValueF = TRUE;
					}
					break;

				case USER_PHOTO_ARG_PHOTO_FORMAT:
					if(GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, (MAX_PHOTO_FORMAT - 1)) == SUCCESS)
					{
						mandatoryFieldPhotoFormat = (UINT16)tempFieldDigit;
					}
					else
					{
						invalidArgValueF = TRUE;
					}
					break;

				default:
					invalidArgValueF = TRUE;
					break;
			}
		}
		else if(getElementStatus == ARG_RESP_FORMAT)
		{
			if(GetResponseType(tempFieldValue, &(pHeader->queryResp.type)) == FAIL)
			{
				invalidArgValueF = TRUE;
			}
		}
		else if(parseStatus != LAST_FIELD_WITHOUT_DATA)
		{
			invalidArgValueF = TRUE;
		}

		if(invalidArgValueF == TRUE)
		{
			WriteErrorInvalidArgumentValue(pTXDataFile, tempFieldName, tempFieldValue, pHeader->queryResp.type);
			return FAIL;
		}

		if( (parseStatus == LAST_FIELD) || (parseStatus == LAST_FIELD_WITHOUT_DATA) )
		{
			if( (mandatoryFieldUserIdx == INVALID_MANDATORY_FIELD) ||
				(mandatoryFieldPhotoFormat == INVALID_MANDATORY_FIELD) )
			{
				sprintf(sysBuf, "rm -rf %s", RX_API_CONTENT_DATA_FILE);
				system(sysBuf);

				WriteErrorIncompleteCommand(pTXDataFile, pRXDataFile, pHeader->queryResp.type);
				return FAIL;
			}

			// Check for memory card
			if(MmcInfo.status != MMC_CONNECTED)
			{
				// Remove Image
				sprintf(sysBuf, "rm -rf %s", RX_API_CONTENT_DATA_FILE);
				system(sysBuf);

				WriteResponseCode(pTXDataFile, WEB_API_RESP_MEMORY_CARD_NOT_FOUND, pHeader->queryResp.type);
				return FAIL;
			}

			// Read Content Length
			rxContentDataFile = fopen(RX_API_CONTENT_DATA_LEN_FILE, "r+");
			fscanf(rxContentDataFile, "%d", &contentLen);
			fclose(rxContentDataFile);

			//Check for content size
			if(contentLen > MAX_SIZE_USER_PHOTO)
			{
				// Remove Image
				sprintf(sysBuf, "rm -rf %s", RX_API_CONTENT_DATA_FILE);
				system(sysBuf);

				WriteResponseCode(pTXDataFile, WEB_API_RESP_IMAGE_SIZE_TOO_BIG, pHeader->queryResp.type);
				return FAIL;
			}

			sprintf(sysBuf, "%s", RX_API_CONTENT_DATA_FILE);
			if(stat(sysBuf, &fileStat) >= 0)
			{
				// Remove old photo
				DeleteUserPhoto(mandatoryFieldUserIdx);
				GetUserPhotoPath(mandatoryFieldUserIdx, photoPath);

				// Move photo into mmc
				sprintf(sysBuf, "mv %s %s%s", RX_API_CONTENT_DATA_FILE, photoPath, photoFormatExt[mandatoryFieldPhotoFormat]);
				system(sysBuf);

				WriteResponseCode(pTXDataFile, WEB_API_RESP_SUCCESS, pHeader->queryResp.type);
			}
			else
			{
				WriteResponseCode(pTXDataFile, WEB_API_RESP_INTERNAL_ERROR, pHeader->queryResp.type);
				return FAIL;
			}
			break;
		}
	}
	return SUCCESS;
}

//*****************************************************************************
//	DeleteQueryUserPhoto()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function handles Get Delete action request for Credential query
//
//*****************************************************************************
UINT8 DeleteQueryUserPhoto(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	UINT8 	parseStatus, elementIdx, getElementStatus;
	CHAR 	tempFieldName[MAX_FIELD_NAME_LENGTH], tempFieldValue[MAX_FIELD_VALUE_LENGTH];
	BOOL	invalidArgValueF = FALSE;

	mandatoryFieldUserIdx = INVALID_MANDATORY_FIELD;

	//Check for response format
	if(ParseMandatoryField(pRXDataFile, "format", &tempFieldValue[0], MAX_FIELD_VALUE_LENGTH) == SUCCESS)
	{
		if(GetResponseType(tempFieldValue, &(pHeader->queryResp.type)) == FAIL)
		{
			WriteErrorInvalidArgumentValue(pTXDataFile, "format", tempFieldValue, pHeader->queryResp.type);
			return FAIL;
		}
	}

	while( (parseStatus = ParseAPIField(pRXDataFile, &tempFieldName[0], MAX_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_FIELD_VALUE_LENGTH) ) != FAIL)
	{
		if( (getElementStatus = GetElementIdx(pHeader->query, tempFieldName, &elementIdx)) == SUCCESS)
		{
			switch(elementIdx)
			{
				case USER_PHOTO_ARG_USER_ID:
					if(ValidateAlphaNumIdStr(tempFieldValue) == YES)
					{
						if(IsAlphaNumIdValid(tempFieldValue, &mandatoryFieldUserIdx) == NO)
						{
							WriteResponseCode(pTXDataFile, WEB_API_RESP_USER_ID_NOT_FOUND, pHeader->queryResp.type);
							return FAIL;
						}
					}
					else
					{
						invalidArgValueF = TRUE;
					}
					break;

				default:
					invalidArgValueF = TRUE;
					break;
			}
		}
		else if(getElementStatus == ARG_RESP_FORMAT)
		{
			if(GetResponseType(tempFieldValue, &(pHeader->queryResp.type)) == FAIL)
			{
				invalidArgValueF = TRUE;
			}
		}
		else if(parseStatus != LAST_FIELD_WITHOUT_DATA)
		{
			invalidArgValueF = TRUE;
		}

		if(invalidArgValueF == TRUE)
		{
			WriteErrorInvalidArgumentValue(pTXDataFile, tempFieldName, tempFieldValue, pHeader->queryResp.type);
			return FAIL;
		}

		if( (parseStatus == LAST_FIELD) || (parseStatus == LAST_FIELD_WITHOUT_DATA) )
		{
			if(mandatoryFieldUserIdx == INVALID_MANDATORY_FIELD)
			{
				WriteErrorIncompleteCommand(pTXDataFile, pRXDataFile, pHeader->queryResp.type);
				return FAIL;
			}

			// Check for memory card
			if(MmcInfo.status != MMC_CONNECTED)
			{
				WriteResponseCode(pTXDataFile, WEB_API_RESP_MEMORY_CARD_NOT_FOUND, pHeader->queryResp.type);
				return FAIL;
			}

			DeleteUserPhoto(mandatoryFieldUserIdx);
			WriteResponseCode(pTXDataFile, WEB_API_RESP_SUCCESS, pHeader->queryResp.type);
			break;
		}
	}
	return SUCCESS;
}

//*****************************************************************************
//	IsUserPhotoPresent()
//	Param:
//		IN:	 userIdx
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: filePath
//			 fileSize
//			 extStr
//	Returns:
//		YES/NO
//	Description:
//		This function handles checks for User photo and returns file path, size and format.
//
//*****************************************************************************
static BOOL IsUserPhotoPresent(UINT32 userIdx, CHARPTR filePath, UINT32PTR fileSize, CHARPTR extStr)
{
	CHAR 	buf[255], photoPath[255];
	BOOL	status = NO;
	FILE	* fp;
	struct stat fileStat;

	GetUserPhotoPath(userIdx, photoPath);

	sprintf(buf, "ls %s* > /tmp/UserPhotoPath.txt", photoPath);
	system(buf);

	fp = fopen("/tmp/UserPhotoPath.txt", "r");
	if(fp)
	{
		if(fgets(buf, sizeof(buf), fp) == NULL)
		{
			EPRINT(CONFIG_LOG, "Failed to read temp UserPhotoPath file");
		}
		else
		{
			buf[(strlen(buf) - 1)] = '\0';
			if(stat(buf, &fileStat) >= 0)
			{
				// File is present
				strcpy(filePath, buf);
				*fileSize = (UINT32)(fileStat.st_size);
				strcpy(extStr, strstr(buf, "."));
				status = YES;
			}
		}
		fclose(fp);
	}
	return status;
}

//*****************************************************************************
//	GetUserPhotoPath()
//	Param:
//		IN:	 userIdx
//		OUT: NONE
//	Returns:
//		filePath
//	Description:
//		This function returns filepath for user photo
//
//*****************************************************************************
static void GetUserPhotoPath(UINT32 userIdx, CHARPTR filePath)
{
	sprintf(filePath, "%s/%s/%s%u.", MmcInfo.devPath, USER_PICS_FOLDER, USER_PICS_PREFIX, GetUserId(userIdx));
}

//*****************************************************************************
//	DeleteUserPhoto()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function deletes user photo
//
//*****************************************************************************
static void DeleteUserPhoto(UINT32 userIdx)
{
	CHAR cmdBuf[255], filePath[255];

	GetUserPhotoPath(userIdx, filePath);
	sprintf(cmdBuf, "rm -rf %s*", filePath);
	system(cmdBuf);
}
