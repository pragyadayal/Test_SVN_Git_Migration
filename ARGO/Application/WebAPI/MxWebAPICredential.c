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
//   File         : MxWebAPICredential.c
//   Description  : Brief but meaningful description of what file provides.
//
///////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include <stdlib.h>

#include "MxWebAPICredential.h"
#include "MxConfig.h"
#include "MxConfigUpdateHandler.h"
#include "MxConfigFileIo.h"
#include "MxEnrollment.h"
#include "MxUserConfig.h"

//******** Extern Variables **********
extern const WEB_API_TABLE_INFO_t 	tableInfo[MAX_WEB_API_QUERY];
extern SYS_BASIC_PARA_t	  			SysBasicPara;

//******** Defines and Data Types ****

typedef enum
{
	CRED_TYPE_ALL,
	CRED_TYPE_FINGER,
	CRED_TYPE_CARD,
	MAX_CRED_TYPE
}CRED_TYPE_e;

//******** Function Prototypes *******



//******** Global Variables **********
extern SYS_ADVANCE_PARA_t	  		SysAdvPara;


//******** Static Variables **********
static UINT16		mandatoryFieldUserIdx = INVALID_MANDATORY_FIELD_UINT16;
static CRED_TYPE_e	mandatoryFieldCredentialType = MAX_CRED_TYPE;

//******** Function Definitions ******

//*****************************************************************************
//	GetQueryCredential()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function handles Get action request for Credential query
//
//*****************************************************************************
UINT8 GetQueryCredential(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	UINT8 	parseStatus, elementIdx, getElementStatus, fpIdx = 0xFF;
	CHAR 	tempFieldName[MAX_FIELD_NAME_LENGTH], tempFieldValue[MAX_FIELD_VALUE_LENGTH], tempUserAlphaNumId[ALPHANUM_ID_SIZE];
	UINT64 	tempFieldDigit;
	BOOL 	getCredCardReqF[MAX_USER_CARD] = {FALSE, FALSE}, getAllCardReqF = TRUE, invalidArgValueF = FALSE;

	mandatoryFieldUserIdx = INVALID_MANDATORY_FIELD_UINT16;
	mandatoryFieldCredentialType = MAX_CRED_TYPE;

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
				case CREDENTIAL_ARG_TYPE:
					if( (GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, 0) == SUCCESS) &&
						( (tempFieldDigit == CRED_TYPE_CARD) || (tempFieldDigit == CRED_TYPE_FINGER) ) )
					{
						mandatoryFieldCredentialType = (CRED_TYPE_e)tempFieldDigit;
					}
					else
					{
						invalidArgValueF = TRUE;
					}
					break;

				case CREDENTIAL_ARG_USER_ID:
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
							strcpy(tempUserAlphaNumId, tempFieldValue);
						}
					}
					else
					{
						invalidArgValueF = TRUE;
					}
					break;

				case CREDENTIAL_ARG_CARD1:
				case CREDENTIAL_ARG_CARD2:
					getAllCardReqF = FALSE;
					getCredCardReqF[elementIdx - CREDENTIAL_ARG_CARD1] = TRUE;
					break;

				case CREDENTIAL_ARG_FINGER_IDX:
					if(GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 1, MAX_USER_FP) == SUCCESS)
					{
						fpIdx = (UINT8)tempFieldDigit;
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
			if( (mandatoryFieldUserIdx == INVALID_MANDATORY_FIELD_UINT16) ||
				(mandatoryFieldCredentialType == MAX_CRED_TYPE) )
			{
				WriteErrorIncompleteCommand(pTXDataFile, pRXDataFile, pHeader->queryResp.type);
				return FAIL;
			}

			switch(mandatoryFieldCredentialType)
			{
				case CRED_TYPE_CARD:
					if(getAllCardReqF == TRUE)
					{
						getCredCardReqF[0] = TRUE;
						getCredCardReqF[1] = TRUE;
					}

					WriteStartXMLTag(pTXDataFile, pHeader->queryResp.type);

					if(getCredCardReqF[0] == TRUE)
					{
						FillCredentialElement(pTXDataFile, CREDENTIAL_ARG_CARD1, pHeader->queryResp.type, NORMAL_REQ);
					}

					if(getCredCardReqF[1] == TRUE)
					{
						FillCredentialElement(pTXDataFile, CREDENTIAL_ARG_CARD2, pHeader->queryResp.type, NORMAL_REQ);
					}

					WriteEndXMLTag(pTXDataFile, pHeader->queryResp.type);
					break;

				case CRED_TYPE_FINGER:
					if(fpIdx != 0xFF)
					{
						if(IsUserFpPresent(mandatoryFieldUserIdx, (fpIdx - WEB_API_FP_IDX_OFFSET) ) == YES)
						{
							sprintf(pHeader->queryResp.filePath, "%s%u_%d", SYS_USER_FP_FILE_PATH, GetUserId(mandatoryFieldUserIdx), fpIdx);
							sprintf(pHeader->queryResp.fileName, "%s_%d", tempUserAlphaNumId, fpIdx);
							pHeader->queryResp.fileSize = MAX_SIZE_SINGLE_SFM_TEMPLATE;
							pHeader->queryResp.type = RESP_TYPE_FILE;
						}
						else
						{
							WriteResponseCode(pTXDataFile, WEB_API_RESP_BIOMATRIC_CREDENTIAL_NOT_FOUND, pHeader->queryResp.type);
							return FAIL;
						}
					}
					else
					{
						WriteErrorIncompleteCommand(pTXDataFile, pRXDataFile, pHeader->queryResp.type);
						return FAIL;
					}
					break;

				default:
					break;
			}
			break;
		}
	}
	return SUCCESS;
}

//*****************************************************************************
//	FillCredentialElement()
//	Param:
//		IN:	 pTXDataFile - File Ptr where data is to be write
//			 idx
//			 respFormat
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL
//	Description:
//		This function fills value of a requested element and prints to the data file.
//*****************************************************************************
BOOL FillCredentialElement(FILE * pTXDataFile, UINT8 idx, WEB_API_RESP_TYPE_e respFormat, UNUSED_PARA UINT8 defaultF)
{
	CHAR valueStr[255];

	switch(idx)
	{
		case CREDENTIAL_ARG_CARD1:
		case CREDENTIAL_ARG_CARD2:
			sprintf(valueStr, "%llu", GetUserCardId(mandatoryFieldUserIdx, (idx - CREDENTIAL_ARG_CARD1)));
			break;

		default:
			return FAIL;
	}
	FillElementDataToFile(respFormat, pTXDataFile, GET_ARG_LABEL(QUERY_CREDENTIAL, idx), valueStr);
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
UINT8 SetQueryCredential(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	UINT8 			parseStatus, elementIdx, getElementStatus, fpIdx, buf[MAX_SIZE_SINGLE_SFM_TEMPLATE], status;
	CHAR 			tempFieldName[MAX_FIELD_NAME_LENGTH], tempFieldValue[MAX_FIELD_VALUE_LENGTH];
	UINT64 			tempFieldDigit;
	UINT32			contentLen = 0;
	UINT64 			tempCardValue;
	USER_CONFIG_t	tempUser;
	FILE 			* rxContentDataFile;
	BOOL			invalidArgValueF = FALSE;

	mandatoryFieldUserIdx = INVALID_MANDATORY_FIELD_UINT16;
	mandatoryFieldCredentialType = MAX_CRED_TYPE;

	//Check for response format
	if(ParseMandatoryField(pRXDataFile, "format", &tempFieldValue[0], MAX_FIELD_VALUE_LENGTH) == SUCCESS)
	{
		if(GetResponseType(tempFieldValue, &(pHeader->queryResp.type)) == FAIL)
		{
			WriteErrorInvalidArgumentValue(pTXDataFile, "format", tempFieldValue, pHeader->queryResp.type);
			return FAIL;
		}
	}

	if(ParseMandatoryField(pRXDataFile, GET_ARG_LABEL(pHeader->query, CREDENTIAL_ARG_USER_ID), &tempFieldValue[0], MAX_FIELD_VALUE_LENGTH) == SUCCESS)
	{
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
			WriteErrorInvalidArgumentValue(pTXDataFile, GET_ARG_LABEL(pHeader->query, CREDENTIAL_ARG_USER_ID), tempFieldValue, pHeader->queryResp.type);
			return FAIL;
		}
	}
	else
	{
		WriteErrorIncompleteCommand(pTXDataFile, pRXDataFile, pHeader->queryResp.type);
		return FAIL;
	}

	if(ParseMandatoryField(pRXDataFile, GET_ARG_LABEL(pHeader->query, CREDENTIAL_ARG_TYPE), &tempFieldValue[0], MAX_FIELD_VALUE_LENGTH) == SUCCESS)
	{
		if( (GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, 0) == SUCCESS) &&
			( (tempFieldDigit == CRED_TYPE_CARD) ||	(tempFieldDigit == CRED_TYPE_FINGER) ) )
		{
			mandatoryFieldCredentialType = (CRED_TYPE_e)tempFieldDigit;
		}
		else
		{
			WriteErrorInvalidArgumentValue(pTXDataFile, GET_ARG_LABEL(pHeader->query, CREDENTIAL_ARG_TYPE), tempFieldValue, pHeader->queryResp.type);
			return FAIL;
		}
	}
	else
	{
		WriteErrorIncompleteCommand(pTXDataFile, pRXDataFile, pHeader->queryResp.type);
		return FAIL;
	}

	GetUserConfig(mandatoryFieldUserIdx, &tempUser);

	while( (parseStatus = ParseAPIField(pRXDataFile, &tempFieldName[0], MAX_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_FIELD_VALUE_LENGTH) ) != FAIL)
	{
		if( (getElementStatus = GetElementIdx(pHeader->query, tempFieldName, &elementIdx)) == SUCCESS)
		{
			switch(elementIdx)
			{
				case CREDENTIAL_ARG_USER_ID:
				case CREDENTIAL_ARG_TYPE:
					// Ignore as we have already parsed them.
					break;

				case CREDENTIAL_ARG_CARD1:
				case CREDENTIAL_ARG_CARD2:
					if(mandatoryFieldCredentialType == CRED_TYPE_CARD)
					{
						if(GetCardValueFromString(tempFieldValue, &tempCardValue) == SUCCESS)
						{
							if(IsCardIdAlreadyEnrolled(tempCardValue, mandatoryFieldUserIdx, (elementIdx - CREDENTIAL_ARG_CARD1), WEB_API_USER_CARD) == NO)
							{
								tempUser.cardId[elementIdx - CREDENTIAL_ARG_CARD1] = tempCardValue;
							}
							else
							{
								WriteResponseCode(pTXDataFile, WEB_API_RESP_CARD_ID_EXISTS, pHeader->queryResp.type);
								return FAIL;
							}
						}
						else
						{
							WriteErrorInvalidArgumentValue(pTXDataFile, tempFieldName, tempFieldValue, pHeader->queryResp.type);
							return FAIL;
						}
					}
					else
					{
						WriteErrorInvalidSyntax(pTXDataFile, pRXDataFile, pHeader->queryResp.type);
						return FAIL;
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
			if( (mandatoryFieldUserIdx == INVALID_MANDATORY_FIELD_UINT16) ||
				(mandatoryFieldCredentialType == MAX_CRED_TYPE) )
			{
				WriteErrorIncompleteCommand(pTXDataFile, pRXDataFile, pHeader->queryResp.type);
				return FAIL;
			}

			switch(mandatoryFieldCredentialType)
			{
				case CRED_TYPE_CARD:
					if(IsDuplicateCardIdExists(&tempUser.cardId[0], MAX_USER_CARD) == YES)
					{
						WriteResponseCode(pTXDataFile, WEB_API_RESP_CARD1_AND_CARD2_ARE_IDENTICAL, pHeader->queryResp.type);
						return FAIL;
					}

					if(ConfigUpdateHandler(USER_CONFIG_TABLE_ID, mandatoryFieldUserIdx, (VOIDPTR)(&tempUser)) == SUCCESS)
					{
						WriteResponseCode(pTXDataFile, WEB_API_RESP_SUCCESS, pHeader->queryResp.type);
					}
					else
					{
						WriteResponseCode(pTXDataFile, WEB_API_RESP_INTERNAL_ERROR, pHeader->queryResp.type);
						return FAIL;
					}
					break;

				case CRED_TYPE_FINGER:
					// Read Content Length
					rxContentDataFile = fopen(RX_API_CONTENT_DATA_LEN_FILE, "r+");
					fscanf(rxContentDataFile, "%d", &contentLen);
					fclose(rxContentDataFile);

					//Check for content size
					if(contentLen != MAX_SIZE_SINGLE_SFM_TEMPLATE)
					{
						if( (contentLen > MAX_SIZE_SINGLE_SFM_TEMPLATE) || (SysBasicPara.fprTemplateFormat == SUPREMA_NATIVE_FORMAT) )
						{
							WriteResponseCode(pTXDataFile, WEB_API_RESP_INVALID_TEMPLATE, pHeader->queryResp.type);
							return FAIL;
						}
					}

					// Check for availability
					if(GetFreeUserFpIdx(mandatoryFieldUserIdx, &fpIdx) == NO)
					{
						WriteResponseCode(pTXDataFile, WEB_API_RESP_CRED_LIMIT_REACHED, pHeader->queryResp.type);
						return FAIL;
					}

					rxContentDataFile = fopen(RX_API_CONTENT_DATA_FILE, "r+");
					if(rxContentDataFile == NULL)
					{
						WriteResponseCode(pTXDataFile, WEB_API_RESP_INVALID_TEMPLATE, pHeader->queryResp.type);
						EPRINT(WEB_API_LOG, "Content Data File can not be opened");
						return FAIL;
					}
					fseek(rxContentDataFile, 0, SEEK_SET);
					if(fread(buf, 1, contentLen, rxContentDataFile) != contentLen)
					{
						WriteResponseCode(pTXDataFile, WEB_API_RESP_INVALID_TEMPLATE, pHeader->queryResp.type);
						EPRINT(WEB_API_LOG, "Content Data File can not be read");
						fclose(rxContentDataFile);
						return FAIL;
					}
					if(contentLen < MAX_SIZE_SINGLE_SFM_TEMPLATE)
					{
						memset(&buf[contentLen], CLEAR, (MAX_SIZE_SINGLE_SFM_TEMPLATE - contentLen));
					}
					fclose(rxContentDataFile);

					status = WriteUserFpIntoFile(mandatoryFieldUserIdx, fpIdx, (VOIDPTR)buf);
					if(status == FILE_WRITE_FORMAT_CHECK_FAIL)
					{
						WriteResponseCode(pTXDataFile, WEB_API_RESP_INVALID_TEMPLATE, pHeader->queryResp.type);
						EPRINT(WEB_API_LOG, "Content Data can not be written-INVALID TEMPLATE");
						return FAIL;
					}
					else if(status == FILE_WRITE_FAIL_NO_MEMORY)
					{
						WriteResponseCode(pTXDataFile, WEB_API_RESP_FP_MEMORY_FULL, pHeader->queryResp.type);
						EPRINT(WEB_API_LOG, "Content Data can not be written-FP MEMORY FULL");
						return FAIL;
					}
					else if(status != FILE_WRITE_SUCCESS)
					{
						WriteResponseCode(pTXDataFile, WEB_API_RESP_INTERNAL_ERROR, pHeader->queryResp.type);
						EPRINT(WEB_API_LOG, "Content Data can not be written-FILE WRITE FAIL");
						DeleteSingleUserFpFiles(mandatoryFieldUserIdx, fpIdx);
						return FAIL;
					}

					status = EnrollUserFpOnAllFpr(mandatoryFieldUserIdx, fpIdx, ENROLL_SOURCE_WEBAPI);
					if(status == SUCCESS)
					{
						WriteResponseCode(pTXDataFile, WEB_API_RESP_SUCCESS, pHeader->queryResp.type);
					}
					else
					{
						DeleteSingleUserFpFiles(mandatoryFieldUserIdx, fpIdx);
						if(status == WAIT)
						{
							WriteResponseCode(pTXDataFile, WEB_API_RESP_DEVICE_BUSY, pHeader->queryResp.type);
						}
						else
						{
							WriteResponseCode(pTXDataFile, WEB_API_RESP_INTERNAL_ERROR, pHeader->queryResp.type);
						}
						// Door is busy
						return FAIL;
					}
					break;

				default:
					WriteErrorInvalidSyntax(pTXDataFile, pRXDataFile, pHeader->queryResp.type);
					return FAIL;
			}
			break;
		}
	}
	return SUCCESS;
}

//*****************************************************************************
//	DeleteQueryCredential()
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
UINT8 DeleteQueryCredential(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	UINT8 	parseStatus, elementIdx, getElementStatus;
	CHAR 	tempFieldName[MAX_FIELD_NAME_LENGTH], tempFieldValue[MAX_FIELD_VALUE_LENGTH];
	UINT64 	tempFieldDigit;
	BOOL	invalidArgValueF = FALSE;

	mandatoryFieldUserIdx = INVALID_MANDATORY_FIELD_UINT16;
	mandatoryFieldCredentialType = MAX_CRED_TYPE;

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
				case CREDENTIAL_ARG_TYPE:
					if( (GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, 0) == SUCCESS) &&
						( (tempFieldDigit == CRED_TYPE_ALL) || (tempFieldDigit == CRED_TYPE_CARD) || (tempFieldDigit == CRED_TYPE_FINGER) ) )
					{
						mandatoryFieldCredentialType = (CRED_TYPE_e)tempFieldDigit;
					}
					else
					{
						invalidArgValueF = TRUE;
					}
					break;

				case CREDENTIAL_ARG_USER_ID:
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
			if( (mandatoryFieldUserIdx == INVALID_MANDATORY_FIELD_UINT16) ||
				(mandatoryFieldCredentialType == MAX_CRED_TYPE) )
			{
				WriteErrorIncompleteCommand(pTXDataFile, pRXDataFile, pHeader->queryResp.type);
				return FAIL;
			}

			switch(mandatoryFieldCredentialType)
			{
				case CRED_TYPE_ALL:
					DeleteUserCards(mandatoryFieldUserIdx);
					DeleteUserFp(mandatoryFieldUserIdx);
					break;

				case CRED_TYPE_CARD:
					DeleteUserCards(mandatoryFieldUserIdx);
					break;

				case CRED_TYPE_FINGER:
					DeleteUserFp(mandatoryFieldUserIdx);
					break;

				default:
					break;
			}
			WriteResponseCode(pTXDataFile, WEB_API_RESP_SUCCESS, pHeader->queryResp.type);
			break;
		}
	}
	return SUCCESS;
}
