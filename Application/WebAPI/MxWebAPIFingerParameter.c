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
//   File         : MxWebAPIFingerParameter.c
//   Description  : Brief but meaningful description of what file provides.
//
///////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include <stdlib.h>

#include "MxWebAPIFingerParameter.h"
#include "MxConfig.h"
#include "MxConfigUpdateHandler.h"


//******** Extern Variables **********
extern const WEB_API_TABLE_INFO_t 	tableInfo[MAX_WEB_API_QUERY];
extern FPR_CONFIG_t					FprConfig;
extern const FPR_CONFIG_t  			DfltFprConfig;

//******** Defines and Data Types ****


//******** Function Prototypes *******



//******** Global Variables **********



//******** Static Variables **********


//******** Function Definitions ******

//*****************************************************************************
//	GetQueryFingerParameter()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function handles Get action request for FingerParameter query
//
//*****************************************************************************
UINT8 GetQueryFingerParameter(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	return GetQueryCommonHandler(pHeader, pRXDataFile, pTXDataFile);
}

//*****************************************************************************
//	FillFingerParameterElement()
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
BOOL FillFingerParameterElement(FILE * pTXDataFile, UINT8 idx, WEB_API_RESP_TYPE_e respFormat, UINT8 defaultF)
{
	CHAR 						valueStr[255];
	const FPR_CONFIG_t 			* tempFprConfigPtr;

	if(defaultF == DEFAULT_REQ)
	{
		tempFprConfigPtr = &DfltFprConfig;
	}
	else
	{
		tempFprConfigPtr = &FprConfig;
	}

	switch(idx)
	{
		case FINGER_PARAMETER_ARG_SECURITY:
			sprintf(valueStr, "%d", tempFprConfigPtr->secLevel);
			break;

		case FINGER_PARAMETER_ARG_LIGHTNING_COND:
			sprintf(valueStr, "%d", tempFprConfigPtr->lightCondition);
			break;

		case FINGER_PARAMETER_ARG_SENSITIVITY:
			sprintf(valueStr, "%d", tempFprConfigPtr->sensitivityLevel);
			break;

		case FINGER_PARAMETER_ARG_FAST_MODE:
			sprintf(valueStr, "%d", tempFprConfigPtr->fastMode);
			break;

		case FINGER_PARAMETER_ARG_IMAGE_QUALITY:
			sprintf(valueStr, "%d", tempFprConfigPtr->imageQuality);
			break;

		default:
			return FAIL;
	}
	FillElementDataToFile(respFormat, pTXDataFile, GET_ARG_LABEL(QUERY_FINGER_PARAMETER, idx), valueStr);
	return SUCCESS;
}

//*****************************************************************************
//	SetQueryFingerParameter()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function handles Set action request for FingerParameter query
//
//*****************************************************************************
UINT8 SetQueryFingerParameter(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	UINT8 				parseStatus, getElementStatus, idx;
	CHAR 				tempFieldName[MAX_FIELD_NAME_LENGTH], tempFieldValue[MAX_FIELD_VALUE_LENGTH];
	UINT64 				tempFieldDigit;
	FPR_CONFIG_t 		tempFprConfig;
	BOOL				invalidArgValueF = FALSE;

	// initialise
	tempFprConfig = FprConfig;

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
		if( (getElementStatus = GetElementIdx(pHeader->query, tempFieldName, &idx)) == SUCCESS)
		{
			switch(idx)
			{
				case FINGER_PARAMETER_ARG_SECURITY:
					if(GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, (MAX_SECURITY_LEVEL - 1)) == SUCCESS)
					{
						tempFprConfig.secLevel = (SECURITY_LEVEL_e)tempFieldDigit;
					}
					else
					{
						invalidArgValueF = TRUE;
					}
					break;

				case FINGER_PARAMETER_ARG_LIGHTNING_COND:
					if(GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, (MAX_LIGHT_CONDITION - 1)) == SUCCESS)
					{
						tempFprConfig.lightCondition = (LIGHT_CONDITION_e)tempFieldDigit;
					}
					else
					{
						invalidArgValueF = TRUE;
					}
					break;

				case FINGER_PARAMETER_ARG_SENSITIVITY:
					if(GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, (MAX_SENSITIVITY_LEVEL - 1)) == SUCCESS)
					{
						tempFprConfig.sensitivityLevel = (SENSITIVITY_LEVEL_e)tempFieldDigit;
					}
					else
					{
						invalidArgValueF = TRUE;
					}
					break;

				case FINGER_PARAMETER_ARG_FAST_MODE:
					if(GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, (MAX_FAST_MODES - 1)) == SUCCESS)
					{
						tempFprConfig.fastMode = (FAST_MODE_e)tempFieldDigit;
					}
					else
					{
						invalidArgValueF = TRUE;
					}
					break;

				case FINGER_PARAMETER_ARG_IMAGE_QUALITY:
					if(GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, (MAX_IMAGE_QUALITY - 1)) == SUCCESS)
					{
						tempFprConfig.imageQuality = (IMAGE_QUALITY_e)tempFieldDigit;
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
			if(ConfigUpdateHandler(FPR_CONFIG_TABLE_ID, 0, (VOIDPTR)(&tempFprConfig)) == SUCCESS)
			{
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
//	SetDefaultQueryFingerParameter()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function handles Default action request for FingerParameter query
//
//*****************************************************************************
UINT8 SetDefaultQueryFingerParameter(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	return SetDefaultQueryCommonHandler(pHeader, pRXDataFile, pTXDataFile);
}

//*****************************************************************************
//	SetDefaultFingerParameter()
//	Param:
//		IN:	 defaultElementReqF
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL
//	Description:
//		This function handles Default action request for FingerParameter query
//
//*****************************************************************************
UINT8 SetDefaultFingerParameter(BOOL * defaultElementReqF)
{
	UINT8 				idx;
	FPR_CONFIG_t 		tempFprConfig;

	// initialise
	tempFprConfig = FprConfig;

	for(idx = 0; idx < GET_MAX_ARG(QUERY_FINGER_PARAMETER); idx++)
	{
		if(defaultElementReqF[idx] == TRUE)
		{
			switch(idx)
			{
				case FINGER_PARAMETER_ARG_SECURITY:
					tempFprConfig.secLevel = DfltFprConfig.secLevel;
					break;

				case FINGER_PARAMETER_ARG_LIGHTNING_COND:
					tempFprConfig.lightCondition = DfltFprConfig.lightCondition;
					break;

				case FINGER_PARAMETER_ARG_SENSITIVITY:
					tempFprConfig.sensitivityLevel = DfltFprConfig.sensitivityLevel;
					break;

				case FINGER_PARAMETER_ARG_FAST_MODE:
					tempFprConfig.fastMode = DfltFprConfig.fastMode;
					break;

				case FINGER_PARAMETER_ARG_IMAGE_QUALITY:
					tempFprConfig.imageQuality = DfltFprConfig.imageQuality;
					break;

				default:
					break;
			}
		}
	}

	if(ConfigUpdateHandler(FPR_CONFIG_TABLE_ID, 0, (VOIDPTR)(&tempFprConfig)) == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	GetDefaultQueryFingerParameter()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function handles Get Default action request for FingerParameter query
//
//*****************************************************************************
UINT8 GetDefaultQueryFingerParameter(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	return GetDefaultQueryCommonHandler(pHeader, pRXDataFile, pTXDataFile);
}
