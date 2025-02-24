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
//   Created By   : Yashavadan
//   File         : MxWebAPIWiegandInterface.c
//   Description  : This is WebAPI task file for set parameters
//					wiegandout config.
/////////////////////////////////////////////////////////////////////////////
//********Place all include files ****
//system Includes
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>

//matrix Includes.
#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxWebAPI.h"
#include "MxWebAPICommonDef.h"
#include "MxConfigUpdateHandler.h"
#include "MxWebAPIConfig.h"
#include "MxSysUtils.h"
#include "MxReaderRxTx.h"
#include "MxWebAPIWiegandInterface.h"

//******** Extern Variables **********
extern SYS_BASIC_PARA_t	SysBasicPara;
extern SYS_BASIC_PARA_t	DfltBasicSysPara;
extern const WEB_API_TABLE_INFO_t tableInfo[MAX_WEB_API_QUERY];

//******** Defines and Data Types ****

//******** Function Prototypes *******

//******** Global Variables **********

//******** Static Variables **********

//******** Function Definitions ******

//*****************************************************************************
//	SetQueryWiegandInterface()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function handles Set action request.
//*****************************************************************************
UINT8 SetQueryWiegandInterface(WEB_API_HDR_t *pHeader, FILE *pRXDataFile, FILE *pTXDataFile)
{
	UINT8 				parseStatus, getElementStatus, idx;
	CHAR 				tempFieldName[MAX_FIELD_NAME_LENGTH], tempFieldValue[MAX_FIELD_VALUE_LENGTH];
	UINT64 				tempFieldDigit;
	SYS_BASIC_PARA_t 	tempSysBasicPara;
	BOOL				invalidArgValueF = FALSE;

	// initialise
	GetSysBasicConfigWorkingCopy(&tempSysBasicPara);

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

			case WIEGAND_OUT_ARG_ENABLE:
				if(GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, (MAX_WIGND_RDR_MODE - 1) ) == SUCCESS)
				{
					tempSysBasicPara.extWiegandRdrMode = (WIGND_RDR_MODE_e)tempFieldDigit;
				}
				else
				{
					invalidArgValueF = TRUE;
				}
				break;

			case WIEGAND_OUT_ARG_EN_WAIT_FOR_PANEL_SIGNAL:
				if(GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, 1) == SUCCESS)
				{
					tempSysBasicPara.waitForAccessFeedback = (BOOL)tempFieldDigit;
				}
				else
				{
					invalidArgValueF = TRUE;
				}
				break;

			case WIEGAND_OUT_ARG_SIGNAL_WAIT_TIMER:
				if(GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, 255) == SUCCESS)
				{
					tempSysBasicPara.accessFeedbackWaitTime = (UINT8)tempFieldDigit;
				}
				else
				{
					invalidArgValueF = TRUE;
				}
				break;

			case WIEGAND_OUT_ARG_OUTPUT_FROMAT:
				if(GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, (MAX_WIGND_OUT_FRMT -1) ) == SUCCESS)
				{
					tempSysBasicPara.wiegandOutFormat = (WIGND_OUT_FRMT_e)tempFieldDigit;
				}
				else
				{
					invalidArgValueF = TRUE;
				}
				break;

			case WIEGAND_OUT_ARG_SEND_FROM:
				if(GetUINT64ValueFromString(tempFieldValue, &tempFieldDigit, 0, (MAX_WIGND_OUT_DIR - 1) ) == SUCCESS)
				{
					tempSysBasicPara.wiegandOutDir = (WIGND_OUT_DIR_e)tempFieldDigit;
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
			if(ConfigUpdateHandler(GEN_SYS_BASIC_TABLE_ID, 0, (VOIDPTR)(&tempSysBasicPara)) == SUCCESS)
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
//	GetQueryWiegandInterface()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function handles Get action request for wiegand out
//
//*****************************************************************************
UINT8 GetQueryWiegandInterface(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	return GetQueryCommonHandler(pHeader, pRXDataFile, pTXDataFile);
}

//*****************************************************************************
//	FillWiegandInterfaceElements()
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
UINT8 FillWiegandInterfaceElements(FILE * pTXDataFile, UINT8 idx, WEB_API_RESP_TYPE_e respFormat, UINT8 defaultF)
{
	CHAR 						valueStr[255];
	const SYS_BASIC_PARA_t 		* tempSysBasicParaPtr;

	if(defaultF == DEFAULT_REQ)
	{
		tempSysBasicParaPtr = &DfltBasicSysPara;
	}
	else
	{
		tempSysBasicParaPtr = &SysBasicPara;
	}

	switch(idx)
	{
		case WIEGAND_OUT_ARG_ENABLE:
			sprintf(valueStr, "%u", tempSysBasicParaPtr->extWiegandRdrMode);
			break;

		case WIEGAND_OUT_ARG_EN_WAIT_FOR_PANEL_SIGNAL:
			sprintf(valueStr, "%u", tempSysBasicParaPtr->waitForAccessFeedback);
			break;

		case WIEGAND_OUT_ARG_SIGNAL_WAIT_TIMER:
			sprintf(valueStr, "%u", tempSysBasicParaPtr->accessFeedbackWaitTime);
			break;

		case WIEGAND_OUT_ARG_OUTPUT_FROMAT:
			sprintf(valueStr, "%u", tempSysBasicParaPtr->wiegandOutFormat);
			break;

		case WIEGAND_OUT_ARG_SEND_FROM:
			sprintf(valueStr, "%u", tempSysBasicParaPtr->wiegandOutDir);
			break;

		default:
			return FAIL;
	}
	FillElementDataToFile(respFormat, pTXDataFile, GET_ARG_LABEL(QUERY_WIEGAND_INTERFACE, idx), valueStr);
	return SUCCESS;

}

//*****************************************************************************
//	SetDefaultWiegandInterface()
//	Param:
//		IN:	 defaultElementReqF
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL
//	Description:
//		This function handles Default action request for DeviceBasicConfig query
//
//*****************************************************************************
UINT8 SetDefaultWiegandInterface(BOOL * defaultElementReqF)
{
	UINT8 				idx;
	SYS_BASIC_PARA_t 	tempSysBasicPara;

	// initialise
	GetSysBasicConfigWorkingCopy(&tempSysBasicPara);

	for(idx = 0; idx < GET_MAX_ARG(QUERY_WIEGAND_INTERFACE); idx++)
	{
		if(defaultElementReqF[idx] == TRUE)
		{
			switch(idx)
			{
			case WIEGAND_OUT_ARG_ENABLE:
				tempSysBasicPara.extWiegandRdrMode = DfltBasicSysPara.extWiegandRdrMode;
				break;

			case WIEGAND_OUT_ARG_EN_WAIT_FOR_PANEL_SIGNAL:
				tempSysBasicPara.waitForAccessFeedback = DfltBasicSysPara.waitForAccessFeedback;
				break;

			case WIEGAND_OUT_ARG_SIGNAL_WAIT_TIMER:
				tempSysBasicPara.accessFeedbackWaitTime = DfltBasicSysPara.accessFeedbackWaitTime;
				break;

			case WIEGAND_OUT_ARG_OUTPUT_FROMAT:
				tempSysBasicPara.wiegandOutFormat = DfltBasicSysPara.wiegandOutFormat;
				break;

			case WIEGAND_OUT_ARG_SEND_FROM:
				tempSysBasicPara.wiegandOutDir = DfltBasicSysPara.wiegandOutDir;
				break;

				default:
					break;
			}
		}
	}

	if(ConfigUpdateHandler(GEN_SYS_BASIC_TABLE_ID, 0, (VOIDPTR)(&tempSysBasicPara)) == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	GetDefaultQueryWiegandInterface()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function handles Get Default action request for DeviceBasicConfig query
//
//*****************************************************************************
UINT8 GetDefaultQueryWiegandInterface(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	return GetDefaultQueryCommonHandler(pHeader, pRXDataFile, pTXDataFile);
}

//*****************************************************************************
//	SetDefaultQueryWiegandInterface()
//	Param:
//		IN:	 pHeader 	 - Header
//			 pRXDataFile - File Ptr where query is stored
//			 pTXDataFile - File Ptr where data is to be stored
//		OUT: NONE
//	Returns:
//		SUCCESS/FAIL/WAIT
//	Description:
//		This function handles Default action request for Wiegand Interface query
//
//*****************************************************************************
UINT8 SetDefaultQueryWiegandInterface(WEB_API_HDR_t * pHeader, FILE * pRXDataFile, FILE * pTXDataFile)
{
	return SetDefaultQueryCommonHandler(pHeader, pRXDataFile, pTXDataFile);
}
