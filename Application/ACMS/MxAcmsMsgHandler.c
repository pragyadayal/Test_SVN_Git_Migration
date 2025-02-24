///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix ComSec Pvt. Ltd., Baroda, India.
//   Project      : Access Control System
//   Created By   : Viral Chavda
//   File         : MxAcmsMsgHandler.c
//   Description  : This handler will provide APIs for process acms & file
//					input output commands.
//
/////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
//system Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//matrix Includes.
#include "MxTypedef.h"
#include "MxAcmsMsgHandler.h"
#include "MxAcmsCommand.h"
#include "MxAcmsCommon.h"
#include "MxAcmsConfig.h"
#include "MxConfig.h"
#include "MxConfigFileIo.h"
#include "MxEnrollment.h"
#include "MxEventLog.h"
#include "MxLogs.h"
#include "MxSysCommonDef.h"
#include "MxDcSpecialFunc.h"
#include "MxReaderTask.h"
#include "MxSysTimer.h"
#include "MxSlaveMsg.h"
#include "MxAcmsConfigField.h"
#include "MxSysUtils.h"
#include "MxReaderUtils.h"
#include "MxCerTask.h"
#include "MxSmartCardFormatConfig.h"
#include "MxWiegandOutConfig.h"
#include "MxIdentificationServerConfig.h"
#include "MxMifareSectorWiseKeyConfig.h"
#include "MxSplFnScheduleConfig.h"
#include "MxUserConfig.h"
#include "MxAcmsTask.h"

//******** Extern Variables **********
extern SYS_ADVANCE_PARA_t				SysAdvPara;
extern const CHARPTR					EnrollmentStatusStr[];
extern SPECIAL_FUNCTION_t 				SpecialFunctions[];
extern READER_STATUS_e 					readerStatus[MAX_READERS];			// holds reader status (active/inactive/unknown)
extern READER_AUTO_DETECT_PROC_INFO_t	ReaderAutoDetectInfo;
extern SYS_BASIC_PARA_t					SysBasicPara;

extern APPL_TYPE_e						respReceivedFromCerApplType;
extern const USER_CONFIG_t			DfltUserConfig;
extern READER_STATUS_e 					readerStatus[MAX_READERS];
//******** Defines and Data Types ****

//Set Command from ACMS
#define GEN_ACMS_COMMAND_ENUM(ACMS_COMMAND_ENUM, ACMS_COMMAND_NAME, ACMS_COMMAND_FUNC)	ACMS_COMMAND_ENUM,
#define GEN_ACMS_COMMAND_NAME(ACMS_COMMAND_ENUM, ACMS_COMMAND_NAME, ACMS_COMMAND_FUNC)	ACMS_COMMAND_NAME,
#define GEN_ACMS_COMMAND_FUNC(ACMS_COMMAND_ENUM, ACMS_COMMAND_NAME, ACMS_COMMAND_FUNC)	ACMS_COMMAND_FUNC,

#define ACMS_COMMAND_LIST(ARG)\
		ARG(DATETIME,		"DATETIME",		AcmsProcDateTimeCmd)\
		ARG(ALARMACK,		"ALARMACK",		AcmsProcAckAlarmCmd)\
		ARG(ALARMCLR,		"ALARMCLR",		AcmsProcClrAlarmCmd)\
		ARG(DLOCK,			"DLOCK",		AcmsProcDoorLockCmd)\
		ARG(DUNLOCK,		"DUNLOCK",		AcmsProcDoorUnLockCmd)\
		ARG(DNORMAL,		"DNORMAL",		AcmsProcDoorNormalCmd)\
		ARG(DAIDSBL,		"DAIDSBL",		AcmsProcDoorAuxInDisableCmd)\
		ARG(DAINORMAL,		"DAINORMAL",	AcmsProcDoorAuxInNormalCmd)\
		ARG(DAOSBL,			"DAODSBL",		AcmsProcDoorAuxOutDisableCmd)\
		ARG(DAONORMAL,		"DAONORMAL",	AcmsProcDoorAuxOutNormalCmd)\
		ARG(DSENSEDSBL,		"DSENSEDSBL",	AcmsProcDoorSenseDisableCmd)\
		ARG(DSENSENORMAL,	"DSENSENORMAL",	AcmsProcDoorSenseNormalCmd)\
		ARG(CLRALLFP,		"CLRALLFP",		AcmsProcClrAllFpCmd)\
		ARG(DLATCHRST,		"DLATCHRST",	AcmsProcDoorAuxOutLatchRstCmd)\
		ARG(IOLATCHRST,		"IOLATCHRST",	AcmsProcIOLinkRstCmd)\
		ARG(LOGOUT,			"LOGOUT",		AcmsProcLogoutCmd)\
		ARG(RSTRUSER,		"RSTRUSER",		AcmsProcRestoreUserCmd)\
		ARG(DELFP,			"DELFP",		AcmsProcDeleteFpCmd)\
		ARG(DELCARD,		"DELCARD",		AcmsProcDeleteCardCmd)\
		ARG(SOFTREBOOT,		"SOFTREBOOT",	AcmsProcSoftRebootCmd)\
		ARG(UPDTFWR,		"UPDTFWR",		AcmsProcUpdateFirmwareCmd)\
		ARG(TTFRST,			"TTFRST",		AcmsProcTimeTriggerResetCmd)\
		ARG(DFLTDATA,		"DFLTDATA",		AcmsProcSystemDefaultCmd)\
		ARG(RECHARGE,		"RECHARGE",		AcmsProcRechargeCafeBalance)\
		ARG(RESET,			"RESET",		AcmsProcResetCafeBalance)\
		ARG(DELPHOTO,		"DELPHOTO",		AcmsProcDeleteUserPhotoCmd)\
		ARG(CALBRTFP,		"CALBRTFP",		CalibrateFpSensor)\
		ARG(CHGPASS,		"CHGPASS",		AcmsProcChangePasswordCmd)\
		ARG(DELITMIMG,		"DELITMIMG",	AcmsProcDeleteCafeItemImage)\
		ARG(DOPEN,			"DOPEN",		AcmsDoorOpenCmd)\
		ARG(DAOACT,			"DAOACT",		AcmsProcActAuxOutCmd)\
		ARG(UPDTFWRFTP,		"UPDTFWRFTP",	AcmsProcFtpFirmwareUpgradeCmd)\
		ARG(DELFACE,		"DELFACE",		AcmsProcDeleteOneFaceTmplOfUserCmd)\
		ARG(CLRALLFACE,		"CLRALLFACE",	AcmsProcDeleteAllFaceTmplOfUserCmd)\
		ARG(MANFRGV,		"MANFRGV",		AcmsProcAPBResetCmd)\
		ARG(UPDATEFACE,		"UPDATEFACE",	AcmsProcUpdateFace)\
		ARG(GETPHOTO,		"GETPHOTO",		AcmsProcPhotoSync)\

typedef enum
{
	ACMS_COMMAND_LIST(GEN_ACMS_COMMAND_ENUM)
	MAX_ACMS_CMD,
}ACMS_COMMAND_e;

//Enrollment field
typedef enum
{
	ACMS_ENROLL_FIELD_DOOR_ID = 1,
	ACMS_ENROLL_FIELD_CATEGORY,
	ACMS_ENROLL_FIELD_TYPE,
	ACMS_ENROLL_FIELD_USER_ID,
	ACMS_ENROLL_FIELD_TOTAL_CARD,
	ACMS_ENROLL_FIELD_TOTAL_FINGER,
	ACMS_ENROLL_FIELD_WRITE_FAC_CODE,
	ACMS_ENROLL_FIELD_WRITE_USER_ID,
	ACMS_ENROLL_FIELD_WRITE_ASC,
	ACMS_ENROLL_FIELD_WRITE_FP_TEMPLATE,
	ACMS_ENROLL_FIELD_WRITE_USER_NAME,
	ACMS_ENROLL_FIELD_USER_NAME,
	ACMS_ENROLL_FIELD_WRITE_DESIGNATION_NAME,
	ACMS_ENROLL_FIELD_DESIGNATION_NAME,
	ACMS_ENROLL_FIELD_WRITE_DEPARTMENT_NAME,
	ACMS_ENROLL_FIELD_DEPARTMENT_NAME,
	ACMS_ENROLL_FIELD_WRITE_BRANCH_NAME,
	ACMS_ENROLL_FIELD_BRANCH_NAME,
	ACMS_ENROLL_FIELD_WRITE_BLOOD_GROUP,
	ACMS_ENROLL_FIELD_BLOOD_GROUP,
	ACMS_ENROLL_FIELD_WRITE_EMERGENCY_CONTACT,
	ACMS_ENROLL_FIELD_EMERGENCY_CONTACT_DETAIL,
	ACMS_ENROLL_FIELD_WRITE_MEDICAL_HISTORY,
	ACMS_ENROLL_FIELD_MEDICAL_HISTORY,
	ACMS_ENROLL_FIELD_SPL_FN_ENRL_TYPE,
	ACMS_ENROLL_FIELD_SPL_FN_ID,
	ACMS_ENROLL_FIELD_SPL_FN_CARD,
    ACMS_ENROLL_FIELD_ENROLL_USING,
    ACMS_ENROLL_FIELD_MOBILE_ACCESS_CARD_SELECT,
    ACMS_ENROLL_FIELD_ACCESS_CARD_NUM,
	MAX_ACMS_ENROLL_FIELD
}ACMS_ENROLL_FIELD_e;

typedef enum
{
	FP_RD_WR_FAIL = 0,
	FP_RD_WR_SUCCESS,
	FP_RD_WR_WAIT,
	FP_WRONG_LOC,
	FP_WRONG_USER_ID,
	FP_WRONG_TEMPLATE,
	FP_MEM_FULL,
	FP_FRMT_CHK_FAIL,
}FP_TEMP_RESPONSE_e;

typedef enum
{
	GET_USER_CARD = 0,
	GET_SP_FN_CARD,
	MAX_GET_CARD_DATA
}GET_CRD_DATA_e;

typedef enum
{
	CARD_RD_BUSY = 2,
	CARD_WRONG_TYPE,
	CARD_WRONG_INDEX
}GET_CARD_ERR_RESP_e;

typedef enum
{
	PIN_INVALID_USER_ID = 4,
}GET_PIN_ERR_RESP_e;

#define DEL_FP_ACK_DELAY	3	//seconds

//******** Function Prototypes *******
static void LoadDeleteFpAckTimer(void);
static BOOL EnrollUser(CHARPTR * rxData);
static void FillNull(CHARPTR txBuff);

//******** Global Variables **********
FP_TRANS_PARA_t FpTransPara;

//******** Static Variables **********
//command string
static CHARPTR acmsCmdTable[MAX_ACMS_CMD] =
{
	ACMS_COMMAND_LIST(GEN_ACMS_COMMAND_NAME)
};

// Set Configration
// NOTE:This table entry must be match with table CONFIGURATION_TABLE_e
BOOL (*SetAcmsConfigFuncPtr[MAX_ACMS_CONFIG_TABLE+1])(CHARPTR *) =
{
	GEN_ACMS_DEVICE_CONFIGURATION_TABLE(GEN_CONFIGURATION_TABLE_SET_ACMS_FUNC)
};

//Set command form Acms
//NOTE:This table match with ACMS_COMMAND_e
UINT8 (*acmsCmdFuncPtr[MAX_ACMS_CMD])(CHARPTR *) =
{
	ACMS_COMMAND_LIST(GEN_ACMS_COMMAND_FUNC)
};


static CHARPTR acmsConfigTableIdStr [MAX_CONFIG_TABLE]=
{
		GEN_ACMS_DEVICE_CONFIGURATION_TABLE(GEN_CONFIGURATION_TABLE_NAME)
};
static TIMER_HANDLE		acmsDelFpAckTmrHandler;

//******** Function Definations ******

//*****************************************************************************
//	AcmsMsgHandlerGetEvent()
//	Param:
//		IN	: acmsRollCount		- event required from which roll over count
//			: acmsEventCount	- event count sequence number
//		OUT	: txData	-	event data to be filled in txData
//			: txDataLen	-	txData length information
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API will return the event detail as per requested.
//*****************************************************************************
BOOL AcmsMsgHandlerGetEvent(UINT16 acmsRollCount, UINT32 acmsEventCount,
											CHARPTR	txData, UINT16PTR txDataLen)
{
	BOOL						retVal = FAIL;
	EVENT_INFO_t				acmsEventData;
	UINT32						strLen;
	WORD_u 						itemInfo;
	SI_USER_OFFLINE_EVENT_t		usrEvt;
	CHAR						extraDetail5[EVT_ALPHANUM_ID_SIZE+1];

	if(ReadEventLog(acmsEventCount, &acmsEventData) == acmsEventCount)
	{
		//ACMS start event count from 1 while we use from zero so increment by 1
		sprintf(txData,"%d%c%d%c%d%c%02d%02d%04d%c%02d%02d%02d%c%d%c",
		acmsRollCount, FSP, acmsEventCount, FSP, acmsEventData.sourceId, FSP,
		acmsEventData.dateTime.tm_mday,	(acmsEventData.dateTime.tm_mon
		+ MONTH_OFFSET), acmsEventData.dateTime.tm_year + LINUX_YEAR_OFFSET_1900, FSP,
		acmsEventData.dateTime.tm_hour, acmsEventData.dateTime.tm_min,
		acmsEventData.dateTime.tm_sec, FSP, acmsEventData.evId, FSP);

		//read remaining data for event which will be alphanumID for SI user events and blank for other events
		ReadUsrIdForSIUserFromEventLog(acmsEventCount ,&usrEvt);
		strncpy(extraDetail5, usrEvt.alphaNumId, EVT_ALPHANUM_ID_SIZE);
		extraDetail5[EVT_ALPHANUM_ID_SIZE] = '\0';

		strLen = strlen(txData);

		if( (acmsEventData.evId == PREPAID_TXN_NORMAL) || (acmsEventData.evId == PREPAID_TXN_DISCOUNT) ||
			(acmsEventData.evId == POSTPAID_TXN_NORMAL) ||	(acmsEventData.evId == POSTPAID_TXN_DISCOUNT) ||
			(acmsEventData.evId == SOLD_OUT_ITEM) || (acmsEventData.evId == AVAILABLE_ITEM) )
		{
			UINT32	itemPriceDecimal = (UINT32)((UINT32)acmsEventData.detail4 / DOUBLE_PRECISION_FACTOR);
			UINT32	itemPriceFraction = (UINT32)((UINT32)acmsEventData.detail4 % DOUBLE_PRECISION_FACTOR);
			UINT32	itemDiscountDecimal	 = (UINT32)((UINT32)acmsEventData.detail5 / DOUBLE_PRECISION_FACTOR);
			UINT32	itemDiscountFraction = (UINT32)((UINT32)acmsEventData.detail5 % DOUBLE_PRECISION_FACTOR);
			itemInfo.word	= acmsEventData.detail2;

			strLen += sprintf(&txData[strLen],"%u%c%03u%02x%c", acmsEventData.detail1, FSP, itemInfo.byte[1], itemInfo.byte[0], FSP);
			if( ( (itemInfo.byte[0] & 0xf0) == 0) && ( (acmsEventData.evId ==  PREPAID_TXN_DISCOUNT) || (acmsEventData.evId ==  POSTPAID_TXN_DISCOUNT) ) )
			{
				WORD_u field3;

				field3.word = acmsEventData.detail3;
				sprintf(&txData[strLen],"%u%u%c%0u.%02u%c%0u.%02u%c", field3.byte[0], field3.byte[1], FSP, itemPriceDecimal, itemPriceFraction, FSP, itemDiscountDecimal, itemDiscountFraction, FSP);
			}
			else
			{
				sprintf(&txData[strLen],"%u%c%0u.%02u%c%0u.%02u%c", acmsEventData.detail3, FSP, itemPriceDecimal, itemPriceFraction, FSP, itemDiscountDecimal, itemDiscountFraction, FSP);
			}
		}
		else if( (acmsEventData.evId ==  INSUFFICIENT_BALANCE) || (acmsEventData.evId ==  RECHARGE_BALANCE) ||
				(acmsEventData.evId ==  RESET_BALANCE) || (acmsEventData.evId ==  PREPAID_TXN_INFO) ||
															(acmsEventData.evId ==  POSTPAID_TXN_INFO) )
		{
			UINT32		price1Decimal, price1Fraction;
			UINT32		price2Decimal, price2Fraction;

			price1Decimal  = (UINT32)((UINT32)acmsEventData.detail4 / DOUBLE_PRECISION_FACTOR);
			price1Fraction = (UINT32)((UINT32)acmsEventData.detail4 % DOUBLE_PRECISION_FACTOR);
			price2Decimal  = (UINT32)((UINT32)acmsEventData.detail5 / DOUBLE_PRECISION_FACTOR);
			price2Fraction = (UINT32)((UINT32)acmsEventData.detail5 % DOUBLE_PRECISION_FACTOR);
			//ACMS start event count from 1 while we use from zero so increment by 1
			sprintf(&txData[strLen],"%u%c%u%c%u%c%0u.%02u%c%0u.%02u%c", acmsEventData.detail1, FSP, acmsEventData.detail2, FSP,
							acmsEventData.detail3, FSP, price1Decimal, price1Fraction, FSP, price2Decimal, price2Fraction, FSP);
		}

		else if(acmsEventData.evId == INVALID_CARD_EVT)
		{
			sprintf(&txData[strLen], "%llu%c%u%c%u%c%u%c%s%c", GetCombinedEvent(acmsEventData.detail1, acmsEventData.detail2, acmsEventData.detail3), FSP,
							0, FSP, 0, FSP, acmsEventData.detail4, FSP, extraDetail5, FSP);
		}

		else if( (acmsEventData.evId == VISITOR_DENIED_FROM_WEB_API) || (acmsEventData.evId == VISITOR_ALLOWED_FORM_WEB_API) || (acmsEventData.evId == INTERCOM_PANIC_ALARM_EVENT) )
		{
			UINT64 phonePart1 = acmsEventData.detail1;
			UINT32 phonePart2 = acmsEventData.detail2;
			//for above event we need to send ascii characters we get from api evfield 1 and 2
			sprintf(&txData[strLen], "%s%c%s%c%u%c%u%c%s%c", (CHARPTR)&phonePart1, FSP, (CHARPTR)&phonePart2, FSP, acmsEventData.detail3, FSP, acmsEventData.detail4, FSP, extraDetail5, FSP);
			printf("%s%c%s%c%u%c%u%c%s%c\n", (CHARPTR)&phonePart1, FSP, (CHARPTR)&phonePart2, FSP, acmsEventData.detail3, FSP, acmsEventData.detail4, FSP, extraDetail5, FSP);
		}
		
		else if(acmsEventData.evId == QR_LONGITUDE_LATITUDE_EVT)
		{
			
			UINT32	latitudeDecimal 	= (UINT32)((UINT32)acmsEventData.detail4 / LOCATON_PRECISION_FACTOR);
			UINT32	latitudeFraction 	= (UINT32)((UINT32)acmsEventData.detail4 % LOCATON_PRECISION_FACTOR);
			UINT32	longitudeDecimal	= (UINT32)((UINT32)acmsEventData.detail5 / LOCATON_PRECISION_FACTOR);
			UINT32	longitudeFraction 	= (UINT32)((UINT32)acmsEventData.detail5 % LOCATON_PRECISION_FACTOR);
			
			sprintf(&txData[strLen], "%u%c%u%c%u%c%0u.%04u%c%0u.%04u%c", acmsEventData.detail1, FSP, acmsEventData.detail2, FSP,
				acmsEventData.detail3, FSP, latitudeDecimal, latitudeFraction, FSP, longitudeDecimal, longitudeFraction, FSP);
		}

		else
		{
			sprintf(&txData[strLen], "%u%c%u%c%u%c%u%c%s%c", acmsEventData.detail1, FSP, acmsEventData.detail2, FSP,
				acmsEventData.detail3, FSP, acmsEventData.detail4, FSP, extraDetail5, FSP);
		}

		*txDataLen = strlen(txData);
		retVal = SUCCESS;
	}
	else
	{
		// Cannot read event we want.
		EPRINT(ACMS_LOG,"Event no. [%d] not found", acmsEventCount);
	}
	return retVal;
}

//*****************************************************************************
//	AcmsMsgHandlerGetCardData()
//	Param:
//		IN	: rxData	-	request information to get configuration
//		OUT	: txData	-	configuration as per information requested
//			: txDataLen	-	txData length information
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API will return user/Sp fn card data
//*****************************************************************************
BOOL AcmsMsgHandlerGetCardData(CHARPTR *rxData, CHARPTR txData, UINT16PTR txDataLen)
{
	GET_CRD_DATA_e 		cardType;
	UINT16				cnt;
	UINT32 				index;
	UINT16				usrIdx;
	UINT64				field;
	CHARPTR 			tempPtr;
	GET_CARD_ERR_RESP_e	errId;
	UINT32				usrId;

	// Parse configuration card type
	if( (ParseField(rxData, &field) ) == FAIL)
	{
		sprintf(txData, "%d%c", CARD_WRONG_TYPE, FSP);
		*txDataLen = strlen(txData);
		return FAIL;
	}

	cardType = (GET_CRD_DATA_e)field;

	// Parse configuration index
	if( (ParseField(rxData, &field) ) == FAIL)
	{
		sprintf(txData, "%d%c", CARD_WRONG_INDEX, FSP);
		*txDataLen = strlen(txData);
		return FAIL;
	}

	index = (UINT32)field;

	if(index > 0)
	{
		tempPtr = txData;

		sprintf(tempPtr, "%d%c%d%c", cardType, FSP, index, FSP);
		tempPtr += strlen(tempPtr);

		if(cardType == GET_USER_CARD)
		{
			usrId = index;

			if(IsUserValid(usrId, &usrIdx) == YES)
			{
				UINT64 tCardId;
				for(cnt = 0; cnt < MAX_USER_CARD; cnt++)
				{
					tCardId = GetUserCardId(usrIdx, cnt);
					if(tCardId == 0)
					{
						sprintf(tempPtr, "%c%c", DUMMY_CHAR, FSP);
					}
					else
					{
						sprintf(tempPtr, "%llu%c", tCardId, FSP);
					}
					tempPtr += strlen(tempPtr);
				}
				*txDataLen = strlen(txData);
				FillNull(txData);
				return SUCCESS;
			}
			else
			{
				errId = CARD_WRONG_INDEX;
			}
		}
		else if(cardType == GET_SP_FN_CARD)
		{
			index -= ACMS_INDEX_OFFSET;

			if(index < MAX_SPECIAL_FUNC)
			{
				for(cnt = 0; cnt < MAX_SP_FUNC_CARD; cnt++)
				{
					if(SpecialFunctions[index].cardId[cnt] == 0)
					{
						sprintf(tempPtr, "%c%c", DUMMY_CHAR, FSP);
					}
					else
					{
						sprintf(tempPtr, "%llu%c", SpecialFunctions[index].cardId[cnt], FSP);
					}
					tempPtr += strlen(tempPtr);
				}
				*txDataLen = strlen(txData);
				FillNull(txData);
				return SUCCESS;
			}
			else
			{
				errId = CARD_WRONG_INDEX;
			}
		}
		else
		{
			errId = CARD_WRONG_TYPE;
		}
	}
	else
	{
		errId = CARD_WRONG_INDEX;
	}

	sprintf(txData, "%d%c", errId, FSP);
	*txDataLen = strlen(txData);
	return FAIL;
}

//*****************************************************************************
//	AcmsMsgHandlerGetPINData()
//	Param:
//		IN	: rxData	-	request information to get configuration
//		OUT	: txData	-	configuration as per information requested
//			: txDataLen	-	txData length information
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API will return USER PIN card data
//*****************************************************************************
BOOL AcmsMsgHandlerGetPINData(CHARPTR *rxData, CHARPTR txData, UINT16PTR txDataLen)
{
	UINT32 				index;
	UINT16				usrIdx;
	UINT64				field;
	GET_PIN_ERR_RESP_e	errId;
	UINT32				usrId;

	// Parse configuration card type
	if( (ParseField(rxData, &field) ) == FAIL)
	{
		sprintf(txData, "%d%c", PIN_INVALID_USER_ID, FSP);
		*txDataLen = strlen(txData);
		return FAIL;
	}

	index = (UINT32)field;

	if(index > 0)
	{
		usrId = index;
		if(IsUserValid(usrId, &usrIdx) == YES)
		{
			sprintf(txData, "%s%c%u%c", GetUserPinCode(usrIdx), FSP, usrId, FSP);
			*txDataLen = strlen(txData);
			FillNull(txData);
			return SUCCESS;
		}
		else
		{
			errId = PIN_INVALID_USER_ID;
		}
	}
	else
	{
		errId = PIN_INVALID_USER_ID;
	}

	sprintf(txData, "%d%c", errId, FSP);
	*txDataLen = strlen(txData);
	return FAIL;
}

//*****************************************************************************
//	AcmsMsgHandlerSetConfig()
//	Param:
//		IN	: rxData	-	request information to set configuration
//		OUT	: txData	-	acknowledgement of set configuration
//			: txDataLen	-	txData length information
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API will set configuration as requested if it is valid.
//*****************************************************************************
BOOL AcmsMsgHandlerSetConfig(CHARPTR * rxData, CHARPTR txData, UINT16PTR txDataLen)
{
	UINT8 	tableId, status = FAIL;
	UINT64	field;

	// Parse configuration table ID
	if( (ParseField(rxData, &field) ) == FAIL)
	{
		sprintf(txData, "%d%c", FAIL, FSP);
		*txDataLen = strlen(txData);
		return SUCCESS;
	}

	tableId = (UINT8)field;
	tableId -= 1;
	if(tableId >= MAX_ACMS_CONFIG_TABLE)
	{
		sprintf(txData,"%d%c", FAIL, FSP);
		*txDataLen = strlen(txData);
		return SUCCESS;
	}

	status = (*SetAcmsConfigFuncPtr[tableId])(rxData);
	if(status != SUCCESS)
	{
		EPRINT(ACMS_RXTX_LOG, "ACMS Message Handler Failed : Table ID [%d:%s]",(tableId + 1),acmsConfigTableIdStr[tableId]);
	}
	sprintf(txData, "%d%c",status , FSP);
	*txDataLen = strlen(txData);
	return SUCCESS;
}

//*****************************************************************************
//	AcmsMsgHandlerGetTemplate()
//	Param:
//		IN	: rxData	-	request information to set configuration
//		OUT	: txData	-	acknowledgement of set configuration
//			: txDataLen	-	txData length information
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API will return the event detail as per requested.
//*****************************************************************************
BOOL AcmsMsgHandlerGetTemplate(CHARPTR * rxData, CHARPTR txData, UINT16PTR txDataLen)
{
	UINT64				field;
	UINT16				usrIdx;
	UINT8				fpIdx;
	FP_TEMP_RESPONSE_e 	status;
	UINT32				usrId;
	BOOL				isDuressFp = CLEAR;

	// Parse user Id
	if( (ParseField(rxData, &field) ) == FAIL)
	{
		sprintf(txData, "%d%c%d%c", 0, FSP, FP_WRONG_USER_ID, FSP);
		*txDataLen = strlen(txData);
		return FAIL;
	}

	usrId = (UINT32)field;

	if(IsUserValid(usrId, &usrIdx) == NO)
	{
		sprintf(txData, "%d%c%d%c", 0, FSP, FP_WRONG_USER_ID, FSP);
		*txDataLen = strlen(txData);
		return FAIL;
	}

	// Parse finger index and its location
	if( (ParseField(rxData, &field) ) == FAIL)
	{
		sprintf(txData, "%d%c%d%c", 0, FSP, FP_WRONG_LOC, FSP);
		*txDataLen = strlen(txData);
		return FAIL;
	}

	fpIdx = (UINT8)field;

	DPRINT(ACMS_LOG, "usrId[%d] -> usrIdx[%d] FpIdx[%d]", usrId, usrIdx, fpIdx);
 	if((YES == IsSyncFingerIsDuress(usrIdx,(fpIdx/10))))
	{
	    isDuressFp = SET;
	}
	else
	{
	    isDuressFp = CLEAR;
	}
	// Fp Index is XY - X is index, Y - location
	// So pass only index to check fp present
	if(IsUserFpPresent(usrIdx, (fpIdx/10)) == YES)
	{
		sprintf(txData, "%d%c%02d%c%d%c", usrId, FSP, fpIdx, FSP, isDuressFp, FSP);
		*txDataLen = strlen(txData);

		// Fp Index is XY - X is index, Y - location
		// So pass only index to read fp
		if(ReadUserFpFromFile(usrIdx, (fpIdx/10),
										(txData + *txDataLen)) == SUCCESS)
		{
			*txDataLen += MAX_SIZE_SINGLE_SFM_TEMPLATE;
			return SUCCESS;
		}
		else
		{
			*txDataLen = 0;
			status  = FP_RD_WR_FAIL;
		}
	}
	else
	{
		status = FP_WRONG_LOC;
	}

	sprintf(txData, "%u%c%d%c", usrId, FSP, status, FSP);
	*txDataLen = strlen(txData);
	return FAIL;
}

//*****************************************************************************
//	AcmsMsgHandlerSetTemplate()
//	Param:
//		IN	: rxData	-	request information to set configuration
//		OUT	: txData	-	acknowledgement of set configuration
//			: txDataLen	-	txData length information
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API will return the event detail as per requested.
//*****************************************************************************
BOOL AcmsMsgHandlerSetTemplate(CHARPTR * rxData, CHARPTR txData, UINT16PTR txDataLen)
{
	BOOL				retVal = FALSE;
	UINT64				field;
	UINT16				usrIdx;
	FP_TEMP_RESPONSE_e 	status;

	InitFpTransPara();

	// Parse user Id
	if( (ParseField(rxData, &field) ) == FAIL)
	{
		sprintf(txData, "%d%c", FP_WRONG_USER_ID, FSP);
		*txDataLen = strlen(txData);
		return retVal;
	}

	FpTransPara.usrId = (UINT32)field;

	if(IsUserValid(FpTransPara.usrId, &usrIdx) == NO)
	{
		sprintf(txData, "%d%c", FP_WRONG_USER_ID, FSP);
		*txDataLen = strlen(txData);
		return retVal;
	}

	// Parse finger index and its location
	if((ParseField(rxData, &field) ) == FAIL)
	{
		sprintf(txData, "%d%c", FP_WRONG_LOC, FSP);
		*txDataLen = strlen(txData);
		return retVal;
	}

	// Fp Index is XY - X is index, Y - location
	// So take only index
	FpTransPara.fpIndex = ((UINT8)field/10);
	
	
	 // Parse Duress FP Flag 
	if((ParseField(rxData, &field) ) == FAIL)
	{
		sprintf(txData, "%d%c", FP_WRONG_LOC, FSP);
		*txDataLen = strlen(txData);
		return retVal;
	}
	FpTransPara.isDuressFp = (BOOL)field;

	DPRINT(ACMS_LOG, "usrId[%d] -> usrIdx[%d] RcvDuressF[%d]", FpTransPara.usrId, usrIdx, FpTransPara.isDuressFp);
	if(FpTransPara.isDuressFp == YES)
	{
	    UpdateDuressFpIdx(FpTransPara.fpIndex,usrIdx);
	}
	if(FpTransPara.fpIndex < SysAdvPara.maxFingerPerUser)
	{
		if((MAX_READERS != GetSupremaReaderId()) && (readerStatus[GetSupremaReaderId()] == READER_IS_ACTIVE))
		{
			FpTransPara.fpTransStatus = FP_TRANS_STATUS_IN_PROCESS; 
		}
		else
		{
			FpTransPara.fpTransStatus = FP_TRANS_STATUS_IDLE; 
		}
		status = WriteUserFpIntoFile(usrIdx, FpTransPara.fpIndex, (VOIDPTR)*rxData);

		switch(status)
		{
		default:
		case FILE_WRITE_FAIL:
			status = FP_RD_WR_FAIL;
			break;

		case FILE_WRITE_SUCCESS:
			//Template write successfully Send Fingerprint to all FPR
			retVal = EnrollUserFpOnAllFpr(usrIdx, FpTransPara.fpIndex, ENROLL_SOURCE_ACMS);
			if(retVal == WAIT)
			{
				status = FP_RD_WR_WAIT;
				retVal = SUCCESS;

			}
			else if(retVal == FAIL)
			{
				status = FP_RD_WR_FAIL;
			}
			else
			{
				status = FP_RD_WR_SUCCESS;
			}
			break;

		case FILE_WRITE_FAIL_NO_MEMORY:
			status = FP_MEM_FULL;
			break;

		case FILE_WRITE_FORMAT_CHECK_FAIL:
			status = FP_FRMT_CHK_FAIL;
			break;
		}
	}
	else
	{
		status = FP_WRONG_LOC;
	}

	sprintf(txData, "%d%c", status, FSP);
	*txDataLen = strlen(txData);
	return retVal;
}

//*****************************************************************************
//	AcmsMsgHandlerSetCmd()
//	Param:
//		IN	: rxData	-	request information to set configuration
//		OUT	: txData	-	acknowledgement of set configuration
//			: txDataLen	-	txData length information
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API will return the event detail as per requested.
//*****************************************************************************
BOOL AcmsMsgHandlerSetCmd(CHARPTR *rxData, CHARPTR txData, UINT16PTR txDataLen)
{
	BOOL	retVal = FAIL;
	UINT8	cmdType;
	UINT8 	status;
	CHAR 	rxTemp[MAX_TMP_BUFF_SIZE];

	// Parse command and find matching command type
	ParseString(rxData, rxTemp, MAX_TMP_BUFF_SIZE);

	cmdType = ConvertStringToIndex(rxTemp,  acmsCmdTable, MAX_ACMS_CMD);
	// check for command type
	if(cmdType < MAX_ACMS_CMD)
	{
		if(cmdType == DELFP)
		{
			InitFpTransPara();
			retVal = TRUE;
		}
		else if( (cmdType == DELFACE) || (cmdType == CLRALLFACE) )
		{
			retVal = TRUE;
		}
		status = (*acmsCmdFuncPtr[cmdType])(rxData);
	}
	else //Command not match
	{
		status = FAIL;
	}
	sprintf(txData, "%d%c", status, FSP);
	*txDataLen = strlen(txData);
	return retVal;
}

//*****************************************************************************
//	AcmsMsgHandlerProcessEnrollment()
//	Param:
//		IN	: rxData	-	request information to get configuration
//		OUT	: txData	-	configuration as per information requested
//			: txDataLen	-	txData length information
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API will return configuration as requested.
//*****************************************************************************
BOOL AcmsMsgHandlerProcessEnrollment(CHARPTR * rxData, CHARPTR txData, UINT16PTR txDataLen)
{
	sprintf(txData, "%d%c", EnrollUser(rxData), FSP);
	*txDataLen = strlen(txData);
	return TRUE;
}

//*****************************************************************************
//	EnrollUser()
//	Param:
//		IN:	rcvBuff- string which contains message to send
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//			This function enroll user form ACMS
//
//*****************************************************************************
static BOOL EnrollUser(CHARPTR * rxData)
{
	UINT8 					parseStatus, enrollmentStatus;
	CHAR 					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	ACMS_CONFIG_FIELD_ID	fieldId;		//store id for perticular Field
	ENROLLMENT_INPUTS_t 	enrollment;

	memset(&enrollment, 0, sizeof(ENROLLMENT_INPUTS_t) );

	//check upto FAIL or LAST_DATA
	while( ( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL) )
	{
		switch(fieldId)
		{
		default:
			return FAIL;

		// Ignore door id
		case ACMS_ENROLL_FIELD_DOOR_ID:
			break;

		case ACMS_ENROLL_FIELD_CATEGORY:
			enrollment.category = (ENROLL_CATEGORY_e)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_TYPE:
			enrollment.enrollmentType = (ENROLLMENT_TYPE_e)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_USER_ID:
			if( (strlen(tempConfigBuff) > 0) && (strlen(tempConfigBuff) <= MAX_USER_ID_DIGITS) )
			{
				enrollment.userId = (UINT32)atoi(tempConfigBuff);
			}
			else
			{
				return FAIL;
			}
			break;

		case ACMS_ENROLL_FIELD_TOTAL_CARD:
			enrollment.cardCnt = (UINT8)atoi(tempConfigBuff) + ACMS_INDEX_OFFSET;
			break;

		case ACMS_ENROLL_FIELD_TOTAL_FINGER:
			if(enrollment.enrollmentType == ENROLL_FACE_ONLY)
				enrollment.faceCnt = (UINT8)atoi(tempConfigBuff);
			else
				enrollment.fpCnt = (UINT8)atoi(tempConfigBuff) + ACMS_INDEX_OFFSET;
			break;

		case ACMS_ENROLL_FIELD_WRITE_FAC_CODE:
			enrollment.cardWriteFields.facCodeF = (BOOL)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_WRITE_USER_ID:
			enrollment.cardWriteFields.userIdF = (BOOL)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_WRITE_ASC:
			enrollment.cardWriteFields.addlSecCodeF = (BOOL)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_WRITE_FP_TEMPLATE:
			enrollment.cardWriteFields.noOfTemplates = (UINT8)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_WRITE_USER_NAME:
			enrollment.cardWriteFields.userNameF = (BOOL)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_USER_NAME:
			strncpy(enrollment.cardPersonalInfo.userName, tempConfigBuff,
												(CARD_FIELD_LEN_USER_NAME - 1));
			enrollment.cardPersonalInfo.userName[CARD_FIELD_LEN_USER_NAME - 1] = 0;
			break;

		case ACMS_ENROLL_FIELD_WRITE_DESIGNATION_NAME:
			enrollment.cardWriteFields.designationF = (BOOL)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_DESIGNATION_NAME:
			strncpy(enrollment.cardPersonalInfo.designation, tempConfigBuff,
												(CARD_FIELD_LEN_DESIGNATION - 1));
			enrollment.cardPersonalInfo.designation[CARD_FIELD_LEN_DESIGNATION - 1] = 0;
			break;

		case ACMS_ENROLL_FIELD_WRITE_DEPARTMENT_NAME:
			enrollment.cardWriteFields.departmentF = (BOOL)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_DEPARTMENT_NAME:
			strncpy(enrollment.cardPersonalInfo.department, tempConfigBuff,
												(CARD_FIELD_LEN_DEPARTMENT - 1));
			enrollment.cardPersonalInfo.department[CARD_FIELD_LEN_DEPARTMENT - 1] = 0;
			break;

		case ACMS_ENROLL_FIELD_WRITE_BRANCH_NAME:
			enrollment.cardWriteFields.branchF = (BOOL)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_BRANCH_NAME:
			strncpy(enrollment.cardPersonalInfo.branch, tempConfigBuff,
												(CARD_FIELD_LEN_BRANCH - 1));
			enrollment.cardPersonalInfo.branch[CARD_FIELD_LEN_BRANCH - 1] = 0;
			break;

		case ACMS_ENROLL_FIELD_WRITE_BLOOD_GROUP:
			enrollment.cardWriteFields.bloodGrpF = (BOOL)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_BLOOD_GROUP:
			strncpy(enrollment.cardPersonalInfo.bloodGrp, tempConfigBuff,
												(CARD_FIELD_LEN_BLOOD_GRP - 1));
			enrollment.cardPersonalInfo.bloodGrp[CARD_FIELD_LEN_BLOOD_GRP - 1] = 0;
			break;

		case ACMS_ENROLL_FIELD_WRITE_EMERGENCY_CONTACT:
			enrollment.cardWriteFields.emrgncyContactF = (BOOL)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_EMERGENCY_CONTACT_DETAIL:
			strncpy(enrollment.cardPersonalInfo.emrgncyContact, tempConfigBuff,
												(CARD_FIELD_LEN_CONTCAT_NO - 1));
			enrollment.cardPersonalInfo.emrgncyContact[CARD_FIELD_LEN_CONTCAT_NO - 1] = 0;
			break;

		case ACMS_ENROLL_FIELD_WRITE_MEDICAL_HISTORY:
			enrollment.cardWriteFields.medicalHistoryF = (BOOL)atoi(tempConfigBuff);
			break;

		case ACMS_ENROLL_FIELD_MEDICAL_HISTORY:
			strncpy(enrollment.cardPersonalInfo.medicalHistory, tempConfigBuff,
												(CARD_FIELD_LEN_MED_HISTORY - 1));
			enrollment.cardPersonalInfo.medicalHistory[CARD_FIELD_LEN_MED_HISTORY - 1] = 0;
			break;

		case ACMS_ENROLL_FIELD_SPL_FN_ENRL_TYPE:
			if(enrollment.category == ENROLL_CATEGORY_SP_FN)
			{
				enrollment.enrollmentType = (ENROLLMENT_TYPE_e)atoi(tempConfigBuff);
			}
			break;

		case ACMS_ENROLL_FIELD_SPL_FN_ID:
			enrollment.splFnIdx = ( (SPECIAL_FUNCTIONS_e)atoi(tempConfigBuff)
														  - ACMS_INDEX_OFFSET);
			break;

		case ACMS_ENROLL_FIELD_SPL_FN_CARD:
			if(enrollment.category == ENROLL_CATEGORY_SP_FN)
			{
				enrollment.cardCnt = ( (UINT8)atoi(tempConfigBuff)
														  + ACMS_INDEX_OFFSET);
			}
			break;
        case ACMS_ENROLL_FIELD_ENROLL_USING:
            break;

        case ACMS_ENROLL_FIELD_MOBILE_ACCESS_CARD_SELECT:
            enrollment.accessCardSelect = (UINT8)atoi(tempConfigBuff);
            break;

        case ACMS_ENROLL_FIELD_ACCESS_CARD_NUM:
            snprintf(enrollment.accessId, ACCESS_ID_SIZE, "%s", tempConfigBuff);
            break;
		}
		//check last data
		if(parseStatus == LAST_DATA)
		{
			enrollment.source = ENROLL_SOURCE_ACMS;

			if(NO == IsVariant_With_Internal_FP_Rdr()) //Not applicable for ARGO CAE/CAM/CAI Variant
			{
				if((ENROLL_BIOMETRIC_ONLY == enrollment.enrollmentType) || (ENROLL_BIOMETRIC_THEN_CARD == enrollment.enrollmentType))
				{ //TODO:Dharmendra : Logic must be modified when PVR module support added
					EPRINT(SYS_LOG,"Invalid Enrollment type[%d] for ARGO CAE/CAM/CAI",enrollment.enrollmentType);
					return FAIL;
				}
			}

			//start enrollment proccess
			enrollmentStatus = RequestEnrollment(&enrollment);
			DPRINT(ACMS_LOG,"Enrollment Status [%s]", EnrollmentStatusStr[enrollmentStatus]);
			return SUCCESS;
		}
	}
	return FAIL;
}

//*****************************************************************************
//	FillNull()
//	Param:
//		IN:	txBuff- string which contains message to send
//		OUT:NONE
//	Returns:
//		Length of transmitt buffer string
//	Description:
//			This function fill 0(NULL) at dummy character(';').
//
//*****************************************************************************
static void FillNull(CHARPTR txBuff)
{
	UINT16	cnt;
	UINT16	txStrLength;

	txStrLength = strlen(txBuff);

	// Dummy charecter  replace with 0
	for(cnt = 0; cnt < txStrLength; cnt++)
	{
		if(txBuff[cnt] == DUMMY_CHAR)
		{
			txBuff[cnt] = 0;
		}
	}
}

//*****************************************************************************
//	AcmsProcFpResp
//	Param:
//		IN:
//		OUT:
//	Returns:
//
//	Description:
//
//****************************************************************************
void AcmsProcFpResp(SLAVE_MSG_ID_e msgId , VOIDPTR dataPtr)
{
	switch(msgId)
	{
		case MSG_DELETE_FP_TEMPLATES_RESP:
		{
			DELETE_FP_TEMPLATES_RESP_t *respDataPtr = dataPtr;

			if( (respDataPtr -> status == READER_RESP_STATUS_SUCCESS ) ||
				(respDataPtr->status == READER_RESP_STATUS_NOT_FOUND ) )
			{
				if( respDataPtr -> fprUserId == FpTransPara.usrId )
				{
					if(CheckForFpWaitStatus(respDataPtr->readerId) == TRUE)
					{
						LoadDeleteFpAckTimer();
					}
					else
					{
						FpTransPara.fpTransStatus = FP_TRANS_STATUS_COMPLETE;
					}
				}
				else
				{
					EPRINT(DC_LOG,"MSG_DELETE_FP_TEMPLATES_RESP:Expecting Resp For UserId %d and Received is %d",FpTransPara.usrId,respDataPtr -> fprUserId);
				}
			}
			break;
		}

		case MSG_ENROLL_BY_TEMPLATE_RESP:
		{
			ENROLL_BY_TEMPLATE_RESP_DATA_t * respDataPtr = dataPtr;

			if(respDataPtr -> enrollmentSource != ENROLL_SOURCE_ACMS)
			{
				return;
			}

			if(respDataPtr -> status == READER_RESP_STATUS_SUCCESS)
			{
				if( ( respDataPtr -> fpIndex == FpTransPara.fpIndex ) &&
					( respDataPtr -> fprUserId == FpTransPara.usrId ) )
				{
					if(CheckForFpWaitStatus(respDataPtr->readerId) == FALSE)
					{
						// FP template transfer process for all reader is completer
						// And there is no need to to wait before replying ACK hence
						// change status to COMPLETE
						FpTransPara.fpTransStatus = FP_TRANS_STATUS_COMPLETE;
					}
				}
				else
				{
					EPRINT(DC_LOG,"MSG_ENROLL_BY_TEMPLATE_RESP:Expecting Resp For UserId %d fpIdx %d and Received User %d fpInx %d ",FpTransPara.usrId,FpTransPara.fpIndex,
								respDataPtr -> fprUserId,respDataPtr -> fpIndex);
				}
			}
			break;
		}

		default:
			break;
	}
}

//*****************************************************************************
//	CheckForFpWaitStatus
//	Param:
//		IN:
//		OUT:
//	Returns:
//
//	Description:
//
//****************************************************************************
BOOL CheckForFpWaitStatus(READER_ID_e readerId)
{
	if(GetReaderRespPort(readerId) == INTERNAL_PORT)
	{
		FpTransPara.intFpTransDoneF = TRUE;
	}
	else if(GetReaderRespPort(readerId) == EXTERNAL_PORT)
	{
		FpTransPara.extFpTransDoneF = TRUE;
	}

	if( (FpTransPara.intFpTransDoneF == FALSE ) || (FpTransPara.extFpTransDoneF == FALSE)  )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//*****************************************************************************
//	ReadFpTransStatus
//	Param:
//		IN:
//		OUT:
//	Returns:
//
//	Description: It replys Finger transfer status during DELETE FP TEMPLATE and
//				 SET TEMPLATE command
//
//****************************************************************************
FP_TRANS_STATUS_e ReadFpTransStatus(void)
{
	return (FpTransPara.fpTransStatus);
}

//*****************************************************************************
//	InitFpTransPara
//	Param:
//		IN:
//		OUT:
//	Returns:
//
//	Description:
//
//****************************************************************************
void InitFpTransPara(void)
{
	READER_ID_e		readerId;

	FpTransPara.intFpTransDoneF = TRUE;
	FpTransPara.extFpTransDoneF = TRUE;

	IsReaderPresent(FP_READER, INTERNAL_PORT, ANY_READER_MODE, &readerId);
	if(GetReaderStatus(readerId) == READER_IS_ACTIVE)
	{
		FpTransPara.intFpTransDoneF = FALSE;
	}

	IsReaderPresent(FP_READER, EXTERNAL_PORT, ANY_READER_MODE, &readerId);
	if(GetReaderStatus(readerId) == READER_IS_ACTIVE)
	{
		FpTransPara.extFpTransDoneF = FALSE;
	}
	FpTransPara.fpTransStatus = FP_TRANS_STATUS_IDLE;
	FpTransPara.fpIndex = 0;
	FpTransPara.usrId = 0;
}

//*****************************************************************************
//	LoadDeleteFpAckTimer
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function will Start timer to Reply Ack after some delay
//			for Delete FP command
//****************************************************************************
static void LoadDeleteFpAckTimer(void)
{
	TIMER_INFO_t timer;

	if(acmsDelFpAckTmrHandler == INVALID_TIMER_HANDLE)
	{
		timer.count   = (DEL_FP_ACK_DELAY * 10);
		timer.data 	  = 0;
		timer.funcPtr = &DeleteFpAckWaitTimer;

		StartTimer(timer, &acmsDelFpAckTmrHandler, __FUNCTION__);
	}
}

//*****************************************************************************
//	DeleteFpAckWaitTimer
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function will Start timer to Reply Ack after some delay
//			for Delete FP command
//****************************************************************************
void DeleteFpAckWaitTimer(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&acmsDelFpAckTmrHandler);
	FpTransPara.fpTransStatus = FP_TRANS_STATUS_COMPLETE;
}

//*****************************************************************************
//	AcmsMsgHandlerGetDeviceDetail()
//	Param:
//		IN	: none
//		OUT	: txData	-	configuration as per information requested
//			: txDataLen	-	txData length information
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API will return device detail as requested.
//*****************************************************************************
BOOL AcmsMsgHandlerGetDeviceDetail(CHARPTR txData, UINT16PTR txDataLen)
{
	UINT8	readerId, length = 0, readerType;

	// Fill the value for Field Application type
	sprintf(&txData[length], "%d%c%d%c", BASIC_CFG_FIELD_APPL, FVS, SysBasicPara.application, FSP);
	length = strlen(txData);

	for(readerId = 0; readerId <= READER3; readerId++)
	{
		if( (readerId == READER3) && (respReceivedFromCerApplType == CER_APPL) )
		{
			readerType = ConvertRdrType(readerId, COMBO_EXIT_READER, SYS_TO_ACMS);
		}
		else if( (readerId == READER3) && (respReceivedFromCerApplType == CBR_APPL) )
		{
			readerType = ConvertRdrType(readerId, CB_U_READER, SYS_TO_ACMS);
		}
		else if( (readerId == READER3) && (respReceivedFromCerApplType == ATOM_RD100_APPL) )
		{
			readerType = ConvertRdrType(readerId, ATOM_RD100_READER, SYS_TO_ACMS);
		}
		else if( (readerId == READER3) && (respReceivedFromCerApplType == ATOM_RD200_APPL) )
		{
			readerType = ConvertRdrType(readerId, ATOM_RD200_READER, SYS_TO_ACMS);
		}
		else if( (readerId == READER3) && (respReceivedFromCerApplType == ATOM_RD300_APPL) )
		{
			readerType = ConvertRdrType(readerId, ATOM_RD300_READER, SYS_TO_ACMS);
		}
		else
		{
			readerType = ConvertRdrType(readerId, ReaderAutoDetectInfo.readerType[readerId], SYS_TO_ACMS);
		}
		sprintf(&txData[length], "%d%c%d%c", GetAcmsFieldIdForReaderId(readerId), FVS, readerType, FSP);
		length = strlen(txData);
	}
	*txDataLen = length;
	return TRUE;
}
//******************************************************************************
//	UpdateDuressFpIdx()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:  NONE
//	Description:   UpdateDuress Fp Index in user Config
//******************************************************************************
void UpdateDuressFpIdx(UINT8 fpIndex,UINT16 usrIdx)
{
	UINT8 Idx,dualTempIdx;
	USER_CONFIG_t tUserConfig = DfltUserConfig;
	GetUserConfig(usrIdx, &tUserConfig);
	for(Idx = 0; Idx < MAX_DURESS_DUAL_FP_TEMPLATE; Idx++)
	{
		DPRINT(ACMS_LOG,"usrIdx[%d] fpIndex[%d] DuressIdx[%d] UserConfigFPIdx[%d]",usrIdx,fpIndex,Idx,tUserConfig.DuressFp[Idx]);
		if(tUserConfig.DuressFp[Idx] == DURESS_FP_IDX_NONE)
		{
			if(SysAdvPara.templatesPerFinger == TWO_TEMPLATES)
			{
				dualTempIdx = ((fpIndex) * MAX_USER_DURESS_FP);
				tUserConfig.DuressFp[Idx] = dualTempIdx;
				tUserConfig.DuressFp[Idx+1] = dualTempIdx+1;
			}
			else
			{
				tUserConfig.DuressFp[Idx] = (fpIndex);
			}
			SetUserConfig(usrIdx, tUserConfig);
			WriteSingleUserConfig(usrIdx);
			break;
		}
	}
}

//******************************************************************************
//	AcmsMsgHandlerGetUficmd()
//	Param:
//		IN	: rxData	-	request information to get configuration
//		OUT	: txData	-	configuration as per information requested
//			: txDataLen	-	txData length information
//	Returns:  GET_UFI_STATUS_e
//	Description:   This Function Will parse GET_UFI command and enter http FSM Request.
//******************************************************************************
GET_UFI_STATUS_e AcmsMsgHandlerGetUficmd(CHARPTR *rxData, CHARPTR txData, UINT16PTR txDataLen)
{
	UINT8	cmdType;
	GET_UFI_STATUS_e 	status;
	CHAR 	rxTemp[MAX_TMP_BUFF_SIZE] = {'0'};
	UINT16 userIdx = 0;
	UINT32 userId;

	if(ParseString(rxData, rxTemp, MAX_TMP_BUFF_SIZE) == FALSE)
	{
		EPRINT(DC_LOG,"GET_UFI command parsing failed, sending reason:[%d]",FAIL_UNKNOWN_REASON);
		return FAIL_UNKNOWN_REASON;
	}
	userId = (UINT32)atoi(rxTemp);
	
	if(IsUserValid(userId, &userIdx) == NO)
	{
		EPRINT(DC_LOG,"GET_UFI command: Invalid userID received, sending reason:[%d]",USER_NOT_AVAILABLE);
		return USER_NOT_AVAILABLE;
	}
	else
	{
		if(IsFaceRecoEnbl(userIdx) == TRUE)
		{
			if(ParseString(rxData, rxTemp, MAX_TMP_BUFF_SIZE) == FALSE)
			{
				EPRINT(DC_LOG,"GET_UFI command's face index value parsing failed, sending reason:[%d]",FAIL_UNKNOWN_REASON);
				return FAIL_UNKNOWN_REASON;
			}
			else
			{
				CHAR tempFaceIdx[SET_MAX_USER_FACE_INDEX] = {[0 ... (SET_MAX_USER_FACE_INDEX-1)] = 0};
				if(memcmp(tempFaceIdx, rxTemp, SET_MAX_USER_FACE_INDEX) == 0)
				{
					EPRINT(DC_LOG,"Rejecting GET_UFI command since all face index values are zero, sending reason:[%d]",FAIL_UNKNOWN_REASON);
					return FAIL_UNKNOWN_REASON;
				}

				SetUserFaceIndex(userIdx, rxTemp);
				
				SetFaceImageReq_t* userFaceSyncReqPtr = (SetFaceImageReq_t*)calloc(1,sizeof(SetFaceImageReq_t));
			
				if(userFaceSyncReqPtr != NULL)
				{
					userFaceSyncReqPtr->userIdx = userIdx;
					InsertHttpFSMReq((void*)userFaceSyncReqPtr, FALSE, SET_USER_FACE_PHOTO);

					EPRINT(DC_LOG,"GET_UFI Commannd: Sending SetFace event to Http FSM, sending reason:[%d]",GET_UFI_SUCCESS);
					return GET_UFI_SUCCESS;
				}
				else
				{
					EPRINT(DC_LOG,"GET_UFI Commannd:Unable allocate memory for the event creation, sending reason:[%d]",FAIL_UNKNOWN_REASON);
					return FAIL_UNKNOWN_REASON;
				}
			}
		}
		else
		{	
			EPRINT(DC_LOG,"GET_UFI Commannd: User's FR not enabled, or User's Faces are Not Available sending reason:[%d]",FACE_IMAGES_NOT_AVAILABLE);
			return FACE_IMAGES_NOT_AVAILABLE;
		}
	}
}