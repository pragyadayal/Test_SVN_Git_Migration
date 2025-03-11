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
//   Created By   : Bharat Gohil
//   File         : MxAcmsConfig.c
//   Description  : This file provides API for storing configuration received
//					from ACMS.
/////////////////////////////////////////////////////////////////////////////

//*** Place all include files here ***
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>

//matrix Includes.
#include "MxTypedef.h"
#include "MxAcmsConfigField.h"
#include "MxAcmsConfig.h"
#include "MxAcmsCommon.h"
#include "MxAcmsTask.h"
#include "MxCafeteria.h"
#include "MxConfig.h"
#include "MxConfigFileIo.h"
#include "MxConfigUpdateHandler.h"
#include "MxDst.h"
#include "MxEthernetPort.h"
#include "MxLogs.h"
#include "MxReaderRxTx.h"
#include "MxReaderTask.h"
#include "MxSysUtils.h"
#include "MxSysCommonDef.h"
#include "MxGuiHandler.h"
#include "MxDcSpecialFunc.h"
#include "MxSysTime.h"
#include "MxGpio.h"
#include "MxIOLink.h"
#include "MxCerTask.h"
#include "MxDcTask.h"
#include "MxOEM75Drv.h"
#include "MxNetworkUtils.h"
#include "MxMultiLanFileHandling.h"
#include "MxSysCommonUtils.h"
#include "MxBluetoothConfig.h"
#ifdef UPNP_SUPPORTED
#include "MxUPNPSupport.h"
#endif //#ifdef UPNP_SUPPORTED
#include "MxWebJvsLogin.h"
#include "MxFaceRecognitionConfig.h"
#include "MxTempProc.h"
#include "MxTemperatureSensor.h"
#include "MxReaderBluetoothConfig.h"
#include "MxBluetooth.h"
#include "MxUserConfig.h"
#include "MxAutoInterfaceManager.h"
#include "MxHttpEventQueue.h"
#include "AesCbcCrypto.h"

//******** Extern Variables **********
extern SYS_BASIC_PARA_t				SysBasicPara;
extern HOLIDAY_SCHEDULE_t			HolidaySchedule[MAX_HOLIDAY_SCHEDULE];
extern TWO_PERSON_GROUP_t			TwoPersonGroup[MAX_TWO_PERSON_GROUP];
extern IO_LINK_CONFIG_t				IoLinkConfig[MAX_IO_LINK];
extern FIRST_IN_LIST_t				FirstInUserGroup[MAX_FIRST_IN_LISTS];
extern ETHERNET_PARA_t 				EthernetParameters;
extern SERVER_PARA_t 				ServerParameters;
extern DST_CONFIG_t					DstConfig;
extern SPECIAL_FUNCTION_t			SpecialFunctions[MAX_SPECIAL_FUNC];
extern SYS_ADVANCE_PARA_t			SysAdvPara;
extern VISITOR_MANAGEMENT_t 		Visitor[MAX_VISITORS];
extern TIME_TRIG_FUNC_t				TimeTrigFunc[MAX_TIME_TRIG_FUNC];
extern CAFE_ITEM_CONFIG_t			CafeItemConfig[MAX_CAFE_ITEM_CODE];
extern CAFE_MENU_CONFIG_t			CafeMenuConfig[MAX_CAFE_MENU_CODE];
extern CAFE_MENU_SCHEDULE_t			CafeMenuSchConfig[MAX_CAFE_MENU_SCHEDULE];
extern PRINTER_CONFIG_t				PrinterConfig;
extern CAFE_RELOAD_e				reloadCafeMenuF;
extern FPR_CONFIG_t					FprConfig;
extern DVR_EVENT_ACTION_CONFIG_t	DvrEvtActionConfig[MAX_DVR_EVT_ACTN_CONFIG];
extern SMART_ROUTE_ACCESS_CONFIG_t	SmartRouteAccessConfig[MAX_SMART_ROUTE_ACCESS];
extern COMM_PORT_e					acmsCurActiveInterface;
extern IO_LINK_EXT_DEV_CONFIG_t		IoLinkExtDevConfig[MAX_IO_LINK_EXT_DEVICE];
extern IO_LINK_EXT_DEV_MAPPING_t	IoLinkExtDevMapping[MAX_IO_LINK];
extern CARD_FORMAT_CONFIG_t			CardFormatConf[MAX_CARD_FORMAT_PER_DOOR];
extern BOOL							DemoModeEnable;
extern BOOL							SamayModeEnable;
extern VAM_CONFIG_t					VamConfig[MAX_VAM_CONFIG];
extern MULTI_LANG_SUPPORT_t			MultiLangConfig;
extern const VAM_CONFIG_t			DfltVamConfig;
extern const JCM_CONFIG_t			DfltJcmConfig;
extern const SYS_BASIC_PARA_t  		DfltBasicSysPara;
extern IP_CAMERA_CONFIG_t			IpCameraConfig;
extern BLUETOOTH_CONFIG_t			BluetoothPara;
extern CARD_FORMAT_CONFIG_t			BlankCardFormatConf;
extern BACKUP_AND_UPDATE_t			BackUpAndUpdate;
extern FR_CAMERA_CONFIG_t			FRCameraConfig;
extern FR_SERVER_CONFIG_t			FRServerConfig;
extern READER_BLUETOOTH_CONFIG_t	ReaderBluetoothPara;
extern SERVER_MODE_CONFIG_t			ServerModeCfg;
extern ACCESS_SCHEDULE_t			AccessScheduleConfig[MAX_TIME_FRAME];
//******** Defines and Data Types ****

//******** Function Prototypes *******
static BOOL SetAcmsHolidayScheduleConfigFields(UINT8 tableId, CHARPTR * rxData);
static BOOL GetDateFromString(CHARPTR dateStr, DATE_DDMMYY_t * date, UINT16 yearOffset);
static BOOL SetAcmsCardFormatConfigFields(RESPONSE_PORT_e rdrPort, CHARPTR * rxData);

//******** Global Variables **********

//******** Static Variables **********

static const READER_UART_BAUD_e	baudRateValue[MAX_BAUD_RATE] =
							{ 	READER_UART_BAUD_9600,
								READER_UART_BAUD_19200,
								READER_UART_BAUD_38400,
								READER_UART_BAUD_57600,
								READER_UART_BAUD_115200,
							};

//******** Function Definitions ******

//*****************************************************************************
//	AcmsParseConfigIdx()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT:None
//	Returns:
//		STATUS-Always fail
//	Description:
//	    This function is just a dummy function to adjust for configuration table
//
//*****************************************************************************
UINT16 AcmsParseConfigIdx(CHARPTR * rxData)
{
	UINT64 	field;
	UINT16	configIdx;

	//parse and check Index
	if(ParseField(rxData, &field) == FAIL)
	{
		// send as max. index in this case
		configIdx = 0xFFFF;
	}
	else
	{
		configIdx = ( (UINT16)field - ACMS_INDEX_OFFSET);
	}

	//ACMS send configuration from 1 while we start from 0
	return configIdx;
}

//*****************************************************************************
//	SetAcmsUnknownConfigFields()
//	Param:
//		IN:	rxData-Pointer to receive buffer send by ACMS
//		OUT:None
//	Returns:
//		STATUS-Always fail
//	Description:
//	    This function is just a dummy function to adjust for configuration table
//
//*****************************************************************************
BOOL SetAcmsUnknownConfigFields(CHARPTR * rxData)
{
	EPRINT(ACMS_LOG," This function should not have been called !!! %p\n", rxData);
	return FAIL;
}

//*****************************************************************************
//	SetAcmsUserConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsUserConfigFields(CHARPTR * rxData)
{
	CHAR 					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 					parseStatus;
	UINT16 					configIdx;
	USER_CONFIG_t			tempUserConfig;
	UINT32					fieldValue;
	UINT8					fieldLen;
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);

	if(DemoModeEnable == TRUE)
	{
		if(configIdx >= MAX_USER_DEMO)
		{
			CONFIGPRINT(CONFIG_FAIL, "UserConfig");
			return FAIL;
		}
	}
	else if(SamayModeEnable == TRUE)
	{
		if(configIdx >= MAX_USER_SAMAY)
		{
			CONFIGPRINT(CONFIG_FAIL, "UserConfig");
			return FAIL;
		}
	}
	else
	{
		if(configIdx >= MAX_USERS)
		{
			CONFIGPRINT(CONFIG_FAIL, "UserConfig");
			return FAIL;
		}
	}

	//get copy of current configuration
	GetUserConfig(configIdx, &tempUserConfig);

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);
		fieldLen = UTF8Strlen(tempConfigBuff);

		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
		case USER_CFG_FIELD_ENABLE:
			tempUserConfig.enable = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_REF_ID:
			if(fieldLen > 0 && fieldLen <= MAX_USER_ID_DIGITS)
			{
				tempUserConfig.userId = (UINT32)fieldValue;
			}
			else
			{
				tempUserConfig.userId = INVALID_USER_ID;
			}
			break;

		case USER_CFG_FIELD_NAME:
			if(UTF8Strlen(tempConfigBuff) < MX_USER_NAME_SIZE)
			{
				sprintf(tempUserConfig.name, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "UserConfig");
				return FAIL;
			}
			break;

		case USER_CFG_FIELD_VIP:
			tempUserConfig.vip = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_BYPASS_FP:
			tempUserConfig.bypassFinger = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_CARD1:
		case USER_CFG_FIELD_CARD2:
			if(fieldLen > 0)
			{
				//sscanf for unsigned long long data because atoll function
				//return only signed data
				sscanf(tempConfigBuff, "%llu", &tempUserConfig.cardId[fieldId - USER_CFG_FIELD_CARD1]);
			}
			else
			{
				tempUserConfig.cardId[fieldId - USER_CFG_FIELD_CARD1] = INVALID_CARD_ID;
			}
			break;

		case USER_CFG_FIELD_PIN:
			if(fieldLen < MAX_PIN_SIZE)
			{
				sprintf(tempUserConfig.pinCode, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "UserConfig");
				return FAIL;
			}
			break;

		case USER_CFG_FIELD_TYPE:
			if((UINT16)fieldValue == ACMS_INVALID_VISITOR_IDX)
			{
				tempUserConfig.visitorIdx = INVALID_VISITOR_IDX;
			}
			else
			{
				tempUserConfig.visitorIdx = (UINT16)fieldValue - ACMS_INDEX_OFFSET;
			}
			break;

		case USER_CFG_FIELD_VALIDITY_ENABLE:
			tempUserConfig.terminationEnable = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_VALIDITY_DATE:
			if(GetDateFromString(tempConfigBuff, &tempUserConfig.terminationDate, YEAR_OFFSET_2000) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "UserConfig");
				return FAIL;
			}
			break;

		case USER_CFG_FIELD_HOLI_SCH_NO:
			tempUserConfig.holidaySchId = (UINT8)fieldValue;
			break;

		case USER_CFG_FIELD_ABS_RULE_ENABLE:
			tempUserConfig.absentee.enable = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_MAX_ABS_COUNT:
			tempUserConfig.absentee.dayCount = (UINT16)fieldValue;
			break;

		case USER_CFG_FIELD_RESTRICT_ACCESS:
			tempUserConfig.restrictAccess = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_ALPHANUM_ID:
			if(fieldLen < ALPHANUM_ID_SIZE)
			{
				sprintf(tempUserConfig.alphaNumId, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "UserConfig");
				return FAIL;
			}
			break;

		case USER_CFG_FIELD_CAFE_ENABLE:
			tempUserConfig.allowedForCafe = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_CAFE_PAYMENT_MODE:
			tempUserConfig.paymentModeForCafe = (USER_PAYMENT_MODE_e)fieldValue;
			break;

		case USER_CFG_FIELD_CAFE_DISCOUNT_LEVEL:
			tempUserConfig.discountLevelCafe = (CAFE_DISCNT_LEVEL_e)fieldValue;
			break;

		case USER_CFG_FIELD_CAFE_BLOCKED:
			tempUserConfig.blockForCafe = (USER_BLOCK_FOR_CAFE_e)fieldValue;
			break;

		case USER_CFG_FIELD_SMART_ROUTE_ID:
			if((UINT8)fieldValue != 0)
			{
				tempUserConfig.smartRouteId = (UINT8)(fieldValue - ACMS_INDEX_OFFSET);
			}
			else
			{
				tempUserConfig.smartRouteId = USER_SMART_ROUTE_NONE;
			}
			break;

		case USER_CFG_FIELD_SMART_ROUTE_LEVEL:
			tempUserConfig.smartRouteLevel = (UINT8)fieldValue;
			break;

		case USER_CFG_FIELD_SELF_ENRL_ENABLE:
			tempUserConfig.selfEnrlEnbl = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_JOB_TYPE:
			tempUserConfig.jobType = (USER_JOB_TYPE_e)fieldValue;
			break;

		case USER_CFG_FIELD_CHECK_BAL_ON_SERVER:
			tempUserConfig.checkBalanceBeforeTxnF = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_GRP_NO:
			if(fieldLen > 0 && fieldLen <= MAX_USER_GRP_DIGITS)
			{
				tempUserConfig.userGrpId = (UINT32)fieldValue;
			}
			else
			{
				tempUserConfig.userGrpId = INVALID_USER_GRP;
			}
			break;

		case USER_CFG_FIELD_CAFE_POLICY_CHECK:
			tempUserConfig.cafeteriaPolicyCheckF = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_SHOW_ATTNDC_DTL:
			tempUserConfig.showAttndcDtlF = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_ENABLE_FACE_RECOGNITION:
			tempUserConfig.enableFaceRecoF = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_BYPASS_FACE:
//as per HLD it will be blank
			break;

		case USER_CFG_FIELD_DOB_ENABLE:
			tempUserConfig.birthdayMsgEnable = (BOOL)fieldValue;
			break;

		case USER_CFG_FIELD_DOB:
			if(GetDateFromString(tempConfigBuff, &tempUserConfig.dateOfBirth, LINUX_YEAR_OFFSET_1900) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "UserConfig");
				return FAIL;
			}
			break;

		case USER_CFG_FIELD_CAFE_OFFLINE_TRANSACTION_TYPE:
		{
			UINT8 tOfflineTransactionType;
			tOfflineTransactionType = (UINT8)fieldValue;
			if( (CAFE_OFFLINE_TRANSACTION_NOT_ALLOWED <= tOfflineTransactionType) && (CAFE_OFFLINE_TRANSACTION_ALLOWED_WITH_DISCOUNT >= tOfflineTransactionType) )
				tempUserConfig.offlineCafeTransactionType = tOfflineTransactionType;
			break;
		}

		default:
			{
				if(fieldId >= MAX_USER_CFG_FIELD)
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in USER_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "UserConfig");
				}
			}break;
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(USER_CONFIG_TABLE_ID, configIdx, (VOIDPTR)(&tempUserConfig)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "UserConfig value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "UserConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsHolidaySchedule1ConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsHolidaySchedule1ConfigFields(CHARPTR * rxData)
{
	return(SetAcmsHolidayScheduleConfigFields(HOLIDAY_SCHEDULE_1, rxData) );
}

//*****************************************************************************
//	SetAcmsHolidaySchedule2ConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsHolidaySchedule2ConfigFields(CHARPTR * rxData)
{
	return(SetAcmsHolidayScheduleConfigFields(HOLIDAY_SCHEDULE_2, rxData) );
}

//*****************************************************************************
//	SetAcmsHolidaySchedule3ConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsHolidaySchedule3ConfigFields(CHARPTR * rxData)
{
	return(SetAcmsHolidayScheduleConfigFields(HOLIDAY_SCHEDULE_3, rxData) );
}

//*****************************************************************************
//	SetAcmsHolidaySchedule4ConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsHolidaySchedule4ConfigFields(CHARPTR * rxData)
{
	return(SetAcmsHolidayScheduleConfigFields(HOLIDAY_SCHEDULE_4, rxData) );
}

//*****************************************************************************
//	SetAcmsHolidayScheduleConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
static BOOL SetAcmsHolidayScheduleConfigFields(UINT8 tableId, CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT16					configIdx;
	UINT8					parseStatus, holidaySchIndex;
	HOLIDAY_SCHEDULE_t		tempHolidaySchedule;//take instance
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_HOLIDAYS)
	{
		CONFIGPRINT(CONFIG_FAIL, "HolidayScheduleConfig TableID: [%d]", tableId);
		return FAIL;
	}

	//This is for all four HolidaySchedule(0-3)
	holidaySchIndex = tableId;

	// Get current configuration
	tempHolidaySchedule = HolidaySchedule[holidaySchIndex];

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		switch(fieldId)
		{
		case HOLIDAY_SCH_FIELD_NAME:
			if(UTF8Strlen(tempConfigBuff) < HOLIDAY_NAME_SIZE)
			{
				snprintf(tempHolidaySchedule.holidays[configIdx].name, HOLIDAY_NAME_SIZE, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "HolidayScheduleConfig TableID: [%d]", tableId);
				return FAIL;
			}
			break;

		case HOLIDAY_SCH_FIELD_START_DATE:
			if(GetDateFromString(tempConfigBuff, &tempHolidaySchedule.holidays[configIdx].startDate, YEAR_OFFSET_2000) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "HolidayScheduleConfig TableID: [%d]", tableId);
				return FAIL;
			}
			break;

		case HOLIDAY_SCH_FIELD_END_DATE:
			if(GetDateFromString(tempConfigBuff, &tempHolidaySchedule.holidays[configIdx].endDate, YEAR_OFFSET_2000) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "HolidayScheduleConfig TableID: [%d]", tableId);
				return FAIL;
			}
			break;

		case HOLIDAY_SCH_FIELD_ID:
			tempHolidaySchedule.scheduleId = (UINT8)atoi(tempConfigBuff);
			break;

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "HolidayScheduleConfig TableID: [%d]", tableId);
			}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler((holidaySchIndex + HOLIDAY_SCH1_TABLE_ID), configIdx, (VOIDPTR)(&tempHolidaySchedule)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "Holiday schedule value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "HolidayScheduleConfig TableID: [%d]", tableId);
	return FAIL;
}

//*****************************************************************************
//	SetAcmsTwoPersonConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsTwoPersonConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8					parseStatus;
	UINT16					configIdx;
	TWO_PERSON_GROUP_t  	tempTwoPersonGroup;	//take instance
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_TWO_PERSON_GROUP)
	{
		CONFIGPRINT(CONFIG_FAIL, "TwoPersonConfig");
		return FAIL;
	}

	// Get current configuration
	tempTwoPersonGroup = TwoPersonGroup[configIdx];

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		switch(fieldId)
		{
		case TWO_PERSON_FIELD_NAME:
			if(UTF8Strlen(tempConfigBuff) < MX_USER_NAME_SIZE)
			{
				snprintf(tempTwoPersonGroup.name, MX_USER_NAME_SIZE, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "TwoPersonConfig");
				return FAIL;
			}
			break;

		case TWO_PERSON_FIELD_MEMBER1:
		case TWO_PERSON_FIELD_MEMBER2:
		case TWO_PERSON_FIELD_MEMBER3:
		case TWO_PERSON_FIELD_MEMBER4:
		case TWO_PERSON_FIELD_MEMBER5:
		case TWO_PERSON_FIELD_MEMBER6:
		case TWO_PERSON_FIELD_MEMBER7:
		case TWO_PERSON_FIELD_MEMBER8:
		case TWO_PERSON_FIELD_MEMBER9:
		case TWO_PERSON_FIELD_MEMBER10:
		case TWO_PERSON_FIELD_MEMBER11:
		case TWO_PERSON_FIELD_MEMBER12:
		case TWO_PERSON_FIELD_MEMBER13:
		case TWO_PERSON_FIELD_MEMBER14:
		case TWO_PERSON_FIELD_MEMBER15:
		case TWO_PERSON_FIELD_MEMBER16:
		case TWO_PERSON_FIELD_MEMBER17:
		case TWO_PERSON_FIELD_MEMBER18:
		case TWO_PERSON_FIELD_MEMBER19:
		case TWO_PERSON_FIELD_MEMBER20:
			if( (strlen(tempConfigBuff) > 0) && (strlen(tempConfigBuff) <= MAX_USER_ID_DIGITS) )
			{
				tempTwoPersonGroup.userId[fieldId - TWO_PERSON_FIELD_MEMBER1] = atoi(tempConfigBuff);
			}
			else
			{
				tempTwoPersonGroup.userId[fieldId - TWO_PERSON_FIELD_MEMBER1] = INVALID_USER_ID;
			}
			break;

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in TWO_PERSON_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "TwoPersonConfig");
			}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(TWO_PERSON_TABLE_ID, configIdx, (VOIDPTR)(&tempTwoPersonGroup)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG,"Two person value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "TwoPersonConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsIoLinkConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsIoLinkConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8					parseStatus;
	UINT16					configIdx;
	IO_LINK_CONFIG_t		tempIoLinkConfig;
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_IO_LINK)
	{
		CONFIGPRINT(CONFIG_FAIL, "IoLinkConfig");
		return FAIL;
	}

	// Get current configuration
	tempIoLinkConfig = IoLinkConfig[configIdx];

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		switch(fieldId)
		{
		case IO_LINK_FIELD_ENABLE:
			tempIoLinkConfig.enable = (BOOL)atoi(tempConfigBuff);
			break;

		case IO_LINK_FIELD_NAME:
			if(UTF8Strlen(tempConfigBuff) < MX_USER_NAME_SIZE)
			{
				snprintf(tempIoLinkConfig.name, MX_USER_NAME_SIZE, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "IoLinkConfig");
				return FAIL;
			}
			break;

		case IO_LINK_FIELD_INPUT_PORT:
			tempIoLinkConfig.inputPort = (INPUT_PORT_e)atoi(tempConfigBuff) - ACMS_INDEX_OFFSET;
			break;

		case IO_LINK_FIELD_OUTPUT_PORT:
			tempIoLinkConfig.outputPort = ((UINT8)atoi(tempConfigBuff)) - ACMS_INDEX_OFFSET;
			break;

		case IO_LINK_FIELD_OUTPUT_TYPE:
			tempIoLinkConfig.outputType = ((OUTPUT_TYPE_e)atoi(tempConfigBuff));
			break;

		case IO_LINK_FIELD_PULSE_TIME:
			tempIoLinkConfig.pulseTime = (UINT8)atoi(tempConfigBuff);
			break;

	    case IO_LINK_FIELD_RESET_LINK_FLAG:
			tempIoLinkConfig.resetLinkF = (BOOL)atoi(tempConfigBuff);
			break;

		case IO_LINK_FIELD_RESET_LINK_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempIoLinkConfig.resetLinkTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "IoLinkConfig");
				return FAIL;
			}
			break;

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in IO_LINK_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "IoLinkConfig");
			}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(IO_LINK_TABLE_ID, configIdx, (VOIDPTR)(&tempIoLinkConfig)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG,"IO Link value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "IoLinkConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsFirstInUserConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsFirstInUserConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 					parseStatus;
	UINT16					configIdx;
	FIRST_IN_LIST_t			tempFirstInUserGroup;
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_FIRST_IN_LISTS)
	{
		CONFIGPRINT(CONFIG_FAIL, "FirstInUserConfig");
		return FAIL;
	}

	// Get current configuration
	tempFirstInUserGroup = FirstInUserGroup[configIdx];

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		//check Field Id
		if(fieldId > MAX_FIRST_IN_USER_FIELD)
		{
			CONFIGPRINT(CONFIG_FAIL, "FirstInUserConfig");
			return FAIL;
		}

		// ACMS sends configuration from 1 while we start from 0
        fieldId -= ACMS_INDEX_OFFSET;

		if( (strlen(tempConfigBuff) > 0) && (strlen(tempConfigBuff) <= MAX_USER_ID_DIGITS) )
		{
			tempFirstInUserGroup.members[fieldId] = atoi(tempConfigBuff);
		}
		else
		{
			tempFirstInUserGroup.members[fieldId] = INVALID_USER_ID;
		}

        // Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(FIRST_IN_TABLE_ID, configIdx, (VOIDPTR)(&tempFirstInUserGroup)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG,"First In User value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "FirstInUserConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsGenSysParaBasicConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsGenSysParaBasicConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8					parseStatus, tempDigit;
	UINT8 					passwordStringInHex[MAX_HTTP_SERVER_PASSWORD_LEN] = {0}, iv[32] = {0}, outPwdBuffer[MAX_HTTP_SERVER_PASSWORD_LEN]={0};
	UINT8					key[32] = {0x73, 0x65, 0x65, 0x20, 0x50, 0x4f, 0x53, 0x49, 0x58, 0x2e, 0x31, 0x2d, 0x32, 0x30, 0x30, 0x38, 0x20, 0x52, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x61, 0x6c, 0x65, 0x20, 0x41, 0x2e, 0x34, 0x2e, 0x31};
	UINT16					configIdx;
	SYS_BASIC_PARA_t		tempSysBasicPara;
	UINT32					fieldValue;
	READER_TYPE_e			tempRdrType;
	ACMS_CONFIG_FIELD_ID	fieldId;
	BOOL					isHttpUserIdPswdChng = FALSE,httpPortF = FALSE,httpDirF = FALSE,httpParaChangeF = FALSE;
	SERVER_PARA_t			tempServerParameter;
	FLOAT32 				tempFieldVal;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
		return FAIL;
	}

	// Get current configuration
	GetSysBasicConfigWorkingCopy(&tempSysBasicPara);

	/** Get Current configuration of Server Parameters */
	memcpy((VOIDPTR)&tempServerParameter,(VOIDPTR)&ServerParameters,sizeof(tempServerParameter));

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE)) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);

		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
		case BASIC_CFG_FIELD_NAME:
			if(UTF8Strlen(tempConfigBuff) < DEVICE_NAME_SIZE)
			{
				sprintf(tempSysBasicPara.name, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
				return FAIL;
			}
			break;

		case BASIC_CFG_FIELD_DOOR_PULSE_TMR:
			if(IsGivenFloatInValidValueRange(tempConfigBuff,&tempFieldVal,MIN_DOOR_PULSE_TIMER_THRESHOLD,MAX_DOOR_PULSE_TIMER_THRESHOLD) == NO)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Para Receive = [%s]",tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "SysParaBasicConfig");
				return FAIL;
			}
			tempSysBasicPara.doorPulseTimeFloat = (FLOAT32)tempFieldVal;
			break;

		case BASIC_CFG_FIELD_DOOR_SENSE_ENABLE:
			tempSysBasicPara.doorSense.enable = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_DOOR_SENSE_TYPE:
			tempSysBasicPara.doorSense.type = (SENSE_TYPE_e)fieldValue;
			break;

		case BASIC_CFG_FIELD_DOOR_SENSE_MONITORED:
			tempSysBasicPara.doorSense.supervise = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_READER1_TYPE:
			tempDigit = (READER_TYPE_e)fieldValue;
			tempSysBasicPara.reader[READER1].type = ConvertRdrType(READER1, tempDigit, ACMS_TO_SYS);
			break;

		case BASIC_CFG_FIELD_READER1_MODE:
			tempSysBasicPara.reader[READER1].mode = (READER_MODE_e)fieldValue;
			break;

		case BASIC_CFG_FIELD_READER2_TYPE:
			tempDigit = (READER_TYPE_e)fieldValue;
			tempRdrType = ConvertRdrType(READER2, tempDigit, ACMS_TO_SYS);
			if((tempRdrType == SUP_FP_3030_READER) && (NO == IsVariant_With_Internal_FP_Rdr()))
			{
				DPRINT(ACMS_LOG,"Fp Reader is not applicable for ARGO CAE/CAM/CAI Variant");
			}
			else
			{
				tempSysBasicPara.reader[READER2].type = tempRdrType;
			}
			break;

		case BASIC_CFG_FIELD_READER2_MODE:
			tempSysBasicPara.reader[READER2].mode = (READER_MODE_e)fieldValue;
			break;

		case BASIC_CFG_FIELD_READER3_TYPE:
			tempDigit = (READER_TYPE_e)fieldValue;
			tempSysBasicPara.reader[READER3].type = ConvertRdrType(READER3, tempDigit, ACMS_TO_SYS);
			break;

		case BASIC_CFG_FIELD_READER3_MODE:
			tempSysBasicPara.reader[READER3].mode = (READER_MODE_e)fieldValue;
			tempSysBasicPara.reader[READER4].mode = (READER_MODE_e)fieldValue;
			tempSysBasicPara.reader[READER5].mode = (READER_MODE_e)fieldValue;
			break;

		case BASIC_CFG_FIELD_TAMPER_ALARM_ENABLE:
			tempSysBasicPara.tamperAlarm = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_REX_ENABLE:
			tempSysBasicPara.requestExitSw = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_EXIT_ON_LOCK:
			tempSysBasicPara.exitOnLock = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_AUTO_RELOCK:
			tempSysBasicPara.autoRelock = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_TIME_FORMAT:
			tempSysBasicPara.timeFormat = (TIME_FORMAT_e)fieldValue;
			break;

		case BASIC_CFG_FIELD_UTZ:
			tempSysBasicPara.utz = (UINT8)fieldValue;

			if(tempSysBasicPara.utz >= MAX_UTZ)
			{
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
				return FAIL;
			}
			break;

		case BASIC_CFG_FIELD_AUTO_TIME_ENABLE:
			tempSysBasicPara.timeUpdateMode = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_NTP_TYPE:
			tempSysBasicPara.userDefNtp = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_PREDEFINED_NTP_ADDR:
			if(UTF8Strlen(tempConfigBuff) < MAX_NTP_ADDRESS)
			{
				sprintf(tempSysBasicPara.preDefNtpAddr, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
				return FAIL;
			}
			break;

		case BASIC_CFG_FIELD_USER_DEFINED_NTP_ADDR:
			if(UTF8Strlen(tempConfigBuff) < MAX_USER_DEF_NTP_ADDRESS)
			{
				sprintf(tempSysBasicPara.userDefNtpAddr, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
				return FAIL;
			}
			break;

		case BASIC_CFG_FIELD_APPL:
			tempSysBasicPara.application = (ZONE_APPL_e)fieldValue;
			break;

		case BASIC_CFG_FIELD_DOOR_ACCESS_MODE:
			if((ACCESS_MODE_e)fieldValue < 	MAX_ACCESS_MODE)
			{
				tempSysBasicPara.doorAccessMode = (ACCESS_MODE_e)fieldValue;
			}
			else
			{
				DPRINT(ACMS_LOG,"Invalid value door access mode[%d]",(ACCESS_MODE_e)fieldValue);
				return FAIL;
			}
			break;

		case BASIC_CFG_FIELD_ACTIVE_DAY0:
		case BASIC_CFG_FIELD_ACTIVE_DAY1:
		case BASIC_CFG_FIELD_ACTIVE_DAY2:
		case BASIC_CFG_FIELD_ACTIVE_DAY3:
		case BASIC_CFG_FIELD_ACTIVE_DAY4:
		case BASIC_CFG_FIELD_ACTIVE_DAY5:
		case BASIC_CFG_FIELD_ACTIVE_DAY6:
			tempSysBasicPara.workingDays[fieldId - BASIC_CFG_FIELD_ACTIVE_DAY0] = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_HOLIDAY_OVERRIDE:
			tempSysBasicPara.holidayWorking = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_WORKING_START_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempSysBasicPara.workingHrStart) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
				return FAIL;
			}
			break;

		case BASIC_CFG_FIELD_WORKING_END_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempSysBasicPara.workingHrStop) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
				return FAIL;
			}
			break;

		case BASIC_CFG_FIELD_BUZ_MUTE_FLAG:
			tempSysBasicPara.buzMuteF = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_TAG_REDETECTION_DELAY:
			// Received value is in Minutes converting in seconds
			tempSysBasicPara.tagRedetectDelay = CONVERT_TIME_IN_SEC(0, (UINT16)fieldValue, 0);
			break;

		case BASIC_CFG_FIELD_EXIT_READER_TYPE:
			// Ignoring dummy value received from server
			// Using same flag internally for SM130/PN532 reader detection
			//tempSysBasicPara.cdcExitReaderF = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_EXTERNAL_READER_ACCESS_MODE:
			if((ACCESS_MODE_e)fieldValue < 	MAX_ACCESS_MODE)
			{
				tempSysBasicPara.extRdrAccessMode = (ACCESS_MODE_e)fieldValue;
			}
			else
			{
				DPRINT(ACMS_LOG,"Invalid value external reader access mode[%d]",(ACCESS_MODE_e)fieldValue);
				return FAIL;
			}
			break;

		case BASIC_CFG_FIELD_READER_SCHEDULE_MODE_ENABLE:
			tempSysBasicPara.doorModeSchdlEnblF = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_FINGER_TEMPLATE_FORMAT:
			tempSysBasicPara.fprTemplateFormat = (BOOL)fieldValue;
			break;

		case BASIC_CFG_WIGND_OUT_ENABLE:
			tempSysBasicPara.extWiegandRdrMode = (WIGND_RDR_MODE_e)fieldValue;
			break;

		case BASIC_CFG_WIGND_WAIT_FOR_PANEL_SIGNAL:
			tempSysBasicPara.waitForAccessFeedback = (BOOL)fieldValue;
			break;

		case BASIC_CFG_WIGND_WAIT_TIMER:
			tempSysBasicPara.accessFeedbackWaitTime = (UINT8)fieldValue;
			break;

		case BASIC_CFG_WIGND_OUT_FORMAT:
			tempSysBasicPara.wiegandOutFormat = (WIGND_OUT_FRMT_e)fieldValue;
			break;

		case BASIC_CFG_WIGND_OUT_DIRECTION:
			tempSysBasicPara.wiegandOutDir = (WIGND_OUT_DIR_e)fieldValue;
			break;

		case BASIC_CFG_FIELD_DOOR_MODE_SELECTION_ENABLE:
			tempSysBasicPara.doorModeSelectEnblF = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_WAIT_FOR_VERIFY_SIGNAL:
			tempSysBasicPara.waitForUsrVerification = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_WIEGNAD_OUT_FORMAT_NO_0:
		case BASIC_CFG_FIELD_WIEGNAD_OUT_FORMAT_NO_1:
		case BASIC_CFG_FIELD_WIEGNAD_OUT_FORMAT_NO_2:
		case BASIC_CFG_FIELD_WIEGNAD_OUT_FORMAT_NO_3:
		case BASIC_CFG_FIELD_WIEGNAD_OUT_FORMAT_NO_4:
		case BASIC_CFG_FIELD_WIEGNAD_OUT_FORMAT_NO_5:
			tempSysBasicPara.wgndOutCstmFrmtNo[fieldId - BASIC_CFG_FIELD_WIEGNAD_OUT_FORMAT_NO_0] = (UINT8)fieldValue;
			break;

		case BASIC_CFG_FIELD_WIEGNAD_EVENT_SPCL_CODE_0:
		case BASIC_CFG_FIELD_WIEGNAD_EVENT_SPCL_CODE_1:
		case BASIC_CFG_FIELD_WIEGNAD_EVENT_SPCL_CODE_2:
		case BASIC_CFG_FIELD_WIEGNAD_EVENT_SPCL_CODE_3:
		case BASIC_CFG_FIELD_WIEGNAD_EVENT_SPCL_CODE_4:
		case BASIC_CFG_FIELD_WIEGNAD_EVENT_SPCL_CODE_5:
			tempSysBasicPara.wgndOutEventSpclCode[fieldId - BASIC_CFG_FIELD_WIEGNAD_EVENT_SPCL_CODE_0] = (UINT32)fieldValue;
			break;

		case BASIC_CFG_FIELD_AUTO_RELOCK_TIME:
			if((UINT16)fieldValue == 0)
			{
				tempSysBasicPara.autoRelockTime = DfltBasicSysPara.autoRelockTime;
				DPRINT(DC_LOG, "autoRelockTime received zero thats why set it to default value");
			}
			else
			{
				tempSysBasicPara.autoRelockTime = (UINT16)fieldValue;
			}
			break;

		case BASIC_CFG_FIELD_IO_LINK_NW_INTERFACE:
			tempSysBasicPara.IoLinkNwInterface = (COMM_PORT_e)fieldValue;
			break;

		case BASIC_CFG_FIELD_API_ACCESS_ALLOW_ENABLE:
			tempSysBasicPara.apiAccessAllowEnable = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_API_ENTRY_ACCESS_MODE:
			if((API_ACCESS_MODE_e)fieldValue < (MAX_ACCESS_MODE_QR_CODE-1)) //TODO : Dharmendra : Here Restricted Mobile then Face
			{
				tempSysBasicPara.apiEntryAccessMode = (API_ACCESS_MODE_e)fieldValue;
			}
			else
			{
				return FAIL;
			}
			break;

		case BASIC_CFG_FIELD_API_EXIT_ACCESS_MODE:
			if((API_ACCESS_MODE_e)fieldValue < (MAX_ACCESS_MODE_QR_CODE-1)) //TODO : Dharmendra : Here Restricted Mobile then Face
			{
				tempSysBasicPara.apiExitAccessMode = (API_ACCESS_MODE_e)fieldValue;
			}
			else
			{
				return FAIL;
			}

			break;

		case BASIC_CFG_FIELD_API_SECURITY_KEY:
			if(strlen(tempConfigBuff) < SECURITY_KEY_SIZE+1)
			{
				memset(&tempSysBasicPara.apiSecurityKey, 0, sizeof(tempSysBasicPara.apiSecurityKey) );
				sprintf(tempSysBasicPara.apiSecurityKey, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
				return FAIL;
			}
			break;

		case BASIC_CFG_FIELD_CAD_DURATION:
			if(fieldValue < MAX_CAD_DURATION)
			{
				tempSysBasicPara.accessAllowCadDuration = (CAD_DURATION_e)fieldValue;
			}
			else
			{
				EPRINT(ACMS_LOG," Wrong value is received for allow cadence duration");
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
				return FAIL;
			}
            break;

		case BASIC_CFG_FIELD_VISITOR_DOOR_ACCESS_MODE:
			if((ACCESS_MODE_e)fieldValue < 	MAX_ACCESS_MODE)
			{
				tempSysBasicPara.visitorDoorAccessMode = (ACCESS_MODE_e)fieldValue;
			}
			else
			{
				DPRINT(ACMS_LOG,"Invalid value visitor door access mode[%d]",(ACCESS_MODE_e)fieldValue);
				return FAIL;
			}
			break;

		case BASIC_CFG_FIELD_VISITOR_EXT_RDR_ACCESS_MODE:
			if((ACCESS_MODE_e)fieldValue < 	MAX_ACCESS_MODE)
			{
				tempSysBasicPara.visitorExtRdrAccessMode = (ACCESS_MODE_e)fieldValue;
			}
			else
			{
				DPRINT(ACMS_LOG,"Invalid value visitor exit reader access mode[%d]",(ACCESS_MODE_e)fieldValue);
				return FAIL;
			}
			break;

		case BASIC_CFG_FIELD_PIN_CHANGE_CODE:
			memcpy(tempSysBasicPara.pinChangeCode,tempConfigBuff, MAX_PIN_CHANGE_CODE_SIZE);
			break;

		case BASIC_CFG_FIELD_PROMPT_SP_FN_F:
			tempSysBasicPara.promptSpFnF = (BOOL)fieldValue;
			break;

		case BASIC_CFG_FIELD_HTTP_SERVER_USER_ID:
			snprintf(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId, MAX_HTTP_SERVER_USER_ID_LEN, "%s",tempConfigBuff);
			snprintf(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerUserId, MAX_HTTP_SERVER_USER_ID_LEN, "%s",tempConfigBuff);
			if(0 != strncmp(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId,tempConfigBuff,MAX_HTTP_SERVER_USER_ID_LEN))
				isHttpUserIdPswdChng = TRUE;
			break;

		case BASIC_CFG_FIELD_HTTP_SERVER_PASSWORD:
			ConvertStringToHexArray(tempConfigBuff, passwordStringInHex, MAX_HTTP_SERVER_PASSWORD_LEN-1);
			memset(iv, 0, 32);
			AesCbc256EncryptData(passwordStringInHex, outPwdBuffer, (UINT16)strlen(tempConfigBuff)/2, key, iv, AES_DECRYPT);
			if(0 != strncmp(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword,outPwdBuffer,MAX_HTTP_SERVER_PASSWORD_LEN))
				isHttpUserIdPswdChng = TRUE;
			snprintf(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword, MAX_HTTP_SERVER_PASSWORD_LEN, "%s", outPwdBuffer);
			snprintf(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPassword, MAX_HTTP_SERVER_PASSWORD_LEN, "%s", outPwdBuffer);
			break;

		case BASIC_CFG_GENERATE_EVENTS:
			tempSysBasicPara.generateEventF = (BOOL)fieldValue;
			break;

		case BASIC_CFG_DISPLAY_ALLOW_DURATION:
			if( (fieldValue >= MIN_DISPLAY_DURATION) && (fieldValue <= MAX_DISPLAY_DURATION) )
			{
				tempSysBasicPara.accessAllowScreenDuration = (UINT16)fieldValue;
			}
			else
			{
				EPRINT(ACMS_LOG," Wrong value is received for allow display duration");
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
				return FAIL;
			}
			break;

		case BASIC_CFG_BUZZER_DENIED_DURATION:
			if(fieldValue < MAX_CAD_DURATION)
			{
				tempSysBasicPara.accessDenyCadDuration = (CAD_DURATION_e)fieldValue;
			}
			else
			{
				EPRINT(ACMS_LOG," Wrong value is received for deny cadence duration");
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
				return FAIL;
			}
            break;

		case BASIC_CFG_DISPLAY_DENIED_DURATION:
			if( (fieldValue >= MIN_DISPLAY_DURATION) && (fieldValue <= MAX_DISPLAY_DURATION) )
			{
				tempSysBasicPara.accessDenyScreenDuration = (UINT16)fieldValue;
			}
			else
			{
				EPRINT(ACMS_LOG," Wrong value is received for deny display duration");
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
				return FAIL;
			}
			break;

		case BASIC_CFG_DOOR_ABNORMAL_ALARM:
			tempSysBasicPara.abnormalAlarm = (BOOL)fieldValue;
			break;

		case BASIC_CFG_DOOR_FORCE_OPEN_ALARM:
			tempSysBasicPara.forceOpenAlarm = (BOOL)fieldValue;
			break;

		case BASIC_CFG_DOOR_FAULT_ALARM:
			tempSysBasicPara.faultAlarm = (BOOL)fieldValue;
			break;

		case BASIC_CFG_DOOR_PANIC_ALARM:
			tempSysBasicPara.panicAlarm = (BOOL)fieldValue;
			break;

		case BASIC_CFG_CAFETERIA_FACE_ACCESS_MODE:
			tempSysBasicPara.cafeteriaFrAccessMode = (CAFE_FR_ACCESS_MODE_e)fieldValue;
			break;

		case BASIC_CFG_ACCESS_DENIED_TEMP_THRESHOLD_EXCEED:
			tempSysBasicPara.tempThresholdAlarm = (BOOL)fieldValue;
			break;
		case BASIC_CFG_ACCESS_DENIED_FACE_MASK_NOT_DETECTED:
			tempSysBasicPara.faceMaskNotDetectedAlarm = (BOOL)fieldValue;
			break;

		case BASIC_CFG_HTTP_ENCRYPTION:
		{
			if(GetServerMode() == COSEC_VYOM)
			{
				if(((BOOL)fieldValue == FALSE) || ((BOOL)fieldValue == TRUE))
				{
					tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF = (BOOL)fieldValue;
					tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpEncryptionModeF = (BOOL)fieldValue;
				}
				else
				{
					EPRINT(ACMS_LOG,"Invalid value received : fieldId[%u]=value[%s]",fieldId,tempConfigBuff);
					return FAIL;
				}
			}
			else
			{
				EPRINT(ACMS_LOG, "Ignored for CENTRA, fieldId: [%d]", fieldId);
			}

		}break;

		case BASIC_CFG_HTTP_SERVER_URL:
		{
			if(GetServerMode() == COSEC_VYOM)
			{
				CHAR hostAddr[HTTP_MAX_URL_STR]="",hostDir[HTTP_MAX_URL_STR]="";
				UINT16 hostPort = 0;

				/** Parse URL Data and fill to respective fields */
				ParseHttpUrlData(tempConfigBuff,hostAddr,hostDir,&hostPort);

				if(strlen(hostAddr) > 0)
				{
					snprintf(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName,sizeof(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName),"%s",hostAddr);
					snprintf(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerHostName,sizeof(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerHostName),"%s",hostAddr);
				}
				else
				{
					EPRINT(ACMS_LOG,"Failed to parse HTTP Host Address");
					return FAIL;
				}

				snprintf(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerDirectory,sizeof(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerDirectory),"%s",hostDir);
				snprintf(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerDirectory,sizeof(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerDirectory),"%s",hostDir);
				httpParaChangeF = TRUE;

				if(0 != hostPort)
				{
					tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort = hostPort;
					tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPort = hostPort;
					httpPortF = TRUE;
				}
			}
			else
			{
				EPRINT(ACMS_LOG, "Ignored for CENTRA, fieldId: [%d]", fieldId);
			}
		}break;

		case BASIC_CFG_ACCESS_SCHEDULE_ID:
			tempSysBasicPara.accessScheduleID = (UINT8)fieldValue;
			if(tempSysBasicPara.accessScheduleID == 0)
			{
			    tempSysBasicPara.accessScheduleEnableF = FALSE;
			    tempSysBasicPara.hideMenuBarForAccessScheduleF = FALSE;
			    if(SUCCESS != LoadDfltAccessScheduleConfig())
			    {
				    EPRINT(ACMS_RXTX_LOG,"Failed to Load Dflt Access Schedule Configration On Device Schedule Id 0");
			    }
			}
			break;
		
		case BASIC_CFG_ACCESS_SCHEDULE_ENABLE_F:
			tempSysBasicPara.accessScheduleEnableF = (BOOL)fieldValue;
			break;
		
		case BASIC_CFG_HIDE_MENU_BAR_FOR_ACCESS_SCHEDULE_F:
			tempSysBasicPara.hideMenuBarForAccessScheduleF = (BOOL)fieldValue;
			break;
			
		default:
			{
				DPRINT(ACMS_RXTX_LOG, "Set Basic ACMS Config not handled:fieldId = [%d]",fieldId);
				if(fieldId >= MAX_BASIC_CFG_FIELD)
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in GEN_SYS_BASIC_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
				}
			}break;
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{			
			if( (TRUE == isHttpUserIdPswdChng) && (TRUE == IsHttpReqFSMActive()) )
			{
				InsertHttpReqEvent(UF_RestartEvent, NULL);
			}

			if(COSEC_VYOM == ServerModeCfg.serverMode)
			{
				if(FALSE == httpPortF)
				{
					if(TRUE == tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF)
					{
						tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort = DFLT_HTTPS_PORT;
						tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPort = DFLT_HTTPS_PORT;
					}
					else
					{
						tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort = DFLT_HTTP_PORT;
						tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPort = DFLT_HTTP_PORT;
					}
				}

				/** Update server parameters */
				if(FAIL ==  ConfigUpdateHandler(SERVER_PARA_TABLE_UPDATE_ID, configIdx, (VOIDPTR)(&tempServerParameter)))
				{
					return FAIL;
				}

				/** Update Http parameters */
				UpdateHttpServerHostNameService();
				if(TRUE == httpParaChangeF)
				{
					if(TRUE == IsHttpReqFSMActive())
					{
						InsertHttpReqEvent(UF_RestartEvent, NULL);
					}

					if(SysBasicPara.httpWebServerConfig.httpSyncConfig.interfaceSelctionF == AUTO_SELECTION)
					{
						SetAutoInterfaceRestartF(AUTO_HTTP_SERVER, TRUE);
					}
				}
			}
			else
			{
				DPRINT(ACMS_LOG,"HTTP Encryption and URL ignored ! Not applicable for server mode[%d]",ServerModeCfg.serverMode);
			}
			
			if(ConfigUpdateHandler(GEN_SYS_BASIC_TABLE_ID, configIdx, (VOIDPTR)(&tempSysBasicPara)) == SUCCESS)
			{
				#ifdef UPNP_SUPPORTED
				UpnpMx_UpdateService();
				#endif //#ifdef UPNP_SUPPORTED
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG,"GenSysParaBasic value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "GenSysParaBasicConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsGenSysParaAdvConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsGenSysParaAdvConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 					configIdx, parseStatus, tempDigit,length;
	SYS_ADVANCE_PARA_t		tempSysAdvPara;
	FR_CAMERA_CONFIG_t		tempFRCameraConfig;
	UINT32					fieldValue;
	FLOAT32 				tempValue;
	ACMS_CONFIG_FIELD_ID	fieldId;
	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
		return FAIL;
	}

	// Get current configuration
	tempSysAdvPara = SysAdvPara;
	tempFRCameraConfig = FRCameraConfig;

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		switch(fieldId)
		{
			case ADV_CFG_THRESHOLD_TEMP:
			case ADV_CFG_CALIBRATION_PARA:
			case ADV_CFG_EMISSIVITY:
			case ADV_CFG_APPROACH_TO_SENSER_WAIT_TMR:
			case ADV_CFG_TOLERANCE_BW_CONSE_READING:
			case ADV_CFG_MIN_TEMP_FOR_ACCESS:
			case ADV_CFG_IDT_FACE_MASK_DETECTION_THRESHOLD:
			case ADV_CFG_VISIBLE_FACE_FOR_ENRL:
			case ADV_CFG_APPROACH_TO_CAMERA_WAIT_TMR:
			{
				tempValue = (FLOAT32)atof(tempConfigBuff);
			}
			break;
			default:
			{
				// General statement for code optimization
				fieldValue = MxAsciiToUINT32(tempConfigBuff);
			}
			break;
		}

		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
		case ADV_CFG_FIELD_TWO_PERSON_TMR:
			tempSysAdvPara.twoPersonTimer = (UINT8)fieldValue;
            break;

		case ADV_CFG_FIELD_INTER_DIGIT_TMR:
			tempSysAdvPara.interDigitTimer = (UINT8)fieldValue;
			break;

		case ADV_CFG_FIELD_MULTI_ACCESS_TMR:
			tempSysAdvPara.multiAccessTimer = (UINT8)fieldValue;
			break;

		case ADV_CFG_FIELD_LATEIN_EARLYOUT_TMR:
			tempSysAdvPara.lateEarlyTimer = (UINT8)fieldValue;
			break;

		case ADV_CFG_FIELD_ENROLL_ENABLE:
			tempSysAdvPara.enrollmentEnable = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_ENROLL_TYPE:
		{
			ENROLLMENT_TYPE_e tmpEnrollType = (ENROLLMENT_TYPE_e)fieldValue;

			if((NO == IsVariant_With_Internal_FP_Rdr()) &&
			 ((ENROLL_BIOMETRIC_ONLY == tmpEnrollType) ||(ENROLL_BIOMETRIC_THEN_CARD == tmpEnrollType) || (ENROLL_DURESS_FP == tmpEnrollType))) //Ignore configurations
			{
				DPRINT(ACMS_LOG,"EnrollmentType[%d] invalid for ARGO CAE/CAM/CAI",tmpEnrollType);
			}
			else if((tmpEnrollType < ENROLL_READ_ONLY_CARD) || (tmpEnrollType >= MAX_ENROLLMENT_TYPE))
			{
				EPRINT(ACMS_LOG,"Unknown Enrollment type [%d] received",tmpEnrollType);
				return FAIL;
			}
			else
			{
				tempSysAdvPara.enrollmentType = tmpEnrollType;
			}
		}break;


		case ADV_CFG_FIELD_DFLT_NO_OF_FINGER:
			tempSysAdvPara.fpCount = (UINT8)fieldValue + ACMS_INDEX_OFFSET;
			break;

		case ADV_CFG_FIELD_DFLT_NO_OF_CARD:
			tempSysAdvPara.cardCount = (UINT8)fieldValue + ACMS_INDEX_OFFSET;
			break;

		case ADV_CFG_FIELD_ASC_ENABLE:
			tempSysAdvPara.addlSecurity.enable = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_ASC:
			tempSysAdvPara.addlSecurity.code = (UINT16)fieldValue;
			break;

		case ADV_CFG_FIELD_FP_MODE:
			tempSysAdvPara.templatesPerFinger = (TEMPLATES_PER_FINGER_e)fieldValue;
			break;

		case ADV_CFG_FIELD_FP_MAX:
			tempSysAdvPara.maxFingerPerUser = (UINT8)fieldValue + ACMS_INDEX_OFFSET;
			break;

		case ADV_CFG_FIELD_USE_COUNT_ENABLE:
			tempSysAdvPara.useCount.enable = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_USE_COUNT:
			tempSysAdvPara.useCount.count = (UINT8)fieldValue;
			break;

		case ADV_CFG_FIELD_FIRST_IN_ENABLE:
			tempSysAdvPara.firstIn.enable = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_FIRST_IN_LIST:
			tempSysAdvPara.firstIn.listId = (UINT8)fieldValue - ACMS_INDEX_OFFSET;
			break;

		case ADV_CFG_FIELD_TWO_PERSON_ENABLE:
			tempSysAdvPara.twoPerson.enable = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_TWO_PERSON_GRP1:
			tempSysAdvPara.twoPerson.grpId1 = (UINT8)fieldValue - ACMS_INDEX_OFFSET;
			break;

		case ADV_CFG_FIELD_TWO_PERSON_GRP2:
			tempDigit = (UINT8)fieldValue;
			if(tempDigit == ACMS_TWO_PERSON_GRP_NONE)
			{
				tempSysAdvPara.twoPerson.grpId2 = TWO_PERSON_GRP_NONE;
			}
			else
			{
				tempSysAdvPara.twoPerson.grpId2 = (tempDigit - ACMS_INDEX_OFFSET);
			}
			break;

		case ADV_CFG_FIELD_OCCUPANCY_ENABLE:
			tempSysAdvPara.occupancyCtrl.enable = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_OCCUPANCY_COUNT:
			tempSysAdvPara.occupancyCtrl.count = (UINT16)fieldValue;
			break;

		case ADV_CFG_FIELD_APB_ENTRY_ENABLE:
			tempSysAdvPara.antiPassBack.enableEntry = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_APB_EXIT_ENABLE:
			tempSysAdvPara.antiPassBack.enableExit = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_APB_HARD:
			tempSysAdvPara.antiPassBack.hard = (ANTIPASSBACK_STATUS_e)fieldValue;
			break;

		case ADV_CFG_FIELD_APB_FORGIVENESS:
			tempSysAdvPara.antiPassBack.forgiveness = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_ABSENTEE_ENABLE:
			tempSysAdvPara.absenteeRuleEnable = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_AUX_IN_ENABLE:
			tempSysAdvPara.auxInput.enable = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_AUX_IN_MONITOR:
			tempSysAdvPara.auxInput.supervise = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_AUX_IN_SENSE_TYPE:
			tempSysAdvPara.auxInput.sense= (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_AUX_IN_DEBOUNCE_TIME:
			tempSysAdvPara.auxInput.debounceTime = (UINT8)fieldValue;
			break;

		case ADV_CFG_FIELD_AUX_OUT_ENABLE:
			tempSysAdvPara.auxOutput.enable = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_GEN_EXIT_SW_EVENT:
			tempSysAdvPara.genExitSwEvent = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_GEN_INVALID_USER_EVENT:
			tempSysAdvPara.genInvalidUserEvent = (BOOL)fieldValue;
			break;

        case ADV_CFG_FIELD_SSA_ENABLE:
			tempSysAdvPara.SSAConfig.enable = (BOOL)fieldValue;
			break;

        case ADV_CFG_FIELD_SSA_ACCESS_LEVEL:
			tempSysAdvPara.SSAConfig.accessLevel = (UINT8)fieldValue;
			break;

        case ADV_CFG_FIELD_SSA_ACCESS_MODE:
			tempSysAdvPara.SSAConfig.accessMode = (ACCESS_MODE_e)fieldValue;
			break;

        case ADV_CFG_FIELD_DVR_INTIGRATION_ENBL:
        	tempSysAdvPara.dvrIntigrationEnbl = (BOOL)fieldValue;
        	break;

        case ADV_CFG_FIELD_DVR_IPADDR:
        	if(strlen(tempConfigBuff) < MAX_IP_ADDRESS)
        	{
        		strcpy(tempSysAdvPara.dvrIpAddr, tempConfigBuff);
        	}
        	break;

        case ADV_CFG_FIELD_DVR_PORT:
        	tempSysAdvPara.dvrPort = (UINT16)fieldValue;
        	break;

        case ADV_CFG_FIELD_DVR_INTERFACE:
        	// As per Requirement of SAD, we have to ignore interface.
        	//tempSysAdvPara.dvrInterface = (COMM_PORT_e)fieldValue;
        	break;

        case ADV_CFG_FIELD_SMART_ROUTE_ACCESS_ENBL:
        	tempSysAdvPara.smartRouteAccessEnbl = (BOOL)fieldValue;
        	break;

        case ADV_CFG_FIELD_GREETINGS_ENBL:
			tempSysAdvPara.enblGreetingsF = (BOOL)fieldValue;
			break;

        case ADV_CFG_FIELD_GREETING1_STRT_TIME:
        case ADV_CFG_FIELD_GREETING1_END_TIME:
        case ADV_CFG_FIELD_GREETING1_MSG:
        case ADV_CFG_FIELD_GREETING2_STRT_TIME:
		case ADV_CFG_FIELD_GREETING2_END_TIME:
		case ADV_CFG_FIELD_GREETING2_MSG:
		case ADV_CFG_FIELD_GREETING3_STRT_TIME:
		case ADV_CFG_FIELD_GREETING3_END_TIME:
		case ADV_CFG_FIELD_GREETING3_MSG:
		case ADV_CFG_FIELD_GREETING4_STRT_TIME:
		case ADV_CFG_FIELD_GREETING4_END_TIME:
		case ADV_CFG_FIELD_GREETING4_MSG:
		{
			UINT8	greetingIdx, configParaTyp;

			greetingIdx = (fieldId - ADV_CFG_FIELD_GREETING1_STRT_TIME)/3;
			configParaTyp = (fieldId - ADV_CFG_FIELD_GREETING1_STRT_TIME)%3;
			if(configParaTyp == 0)
			{
				if(GetTimeFromString(tempConfigBuff, &tempSysAdvPara.greetingConfig[greetingIdx].startTime) == FAIL)
				{
					CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
					return FAIL;
				}
			}
			else if(configParaTyp == 1)
			{
				if(GetTimeFromString(tempConfigBuff, &tempSysAdvPara.greetingConfig[greetingIdx].endTime) == FAIL)
				{
					CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
					return FAIL;
				}
			}
			else if(configParaTyp == 2)
			{
				UINT8		stringLen;

				stringLen = UTF8Strlen(tempConfigBuff);
				if(stringLen < GREETING_MSG_SIZE)
				{
					if(stringLen > 0)
					{
						sprintf(tempSysAdvPara.greetingConfig[greetingIdx].message, "%s", tempConfigBuff);
					}
					else
					{
						tempSysAdvPara.greetingConfig[greetingIdx].message[0] = '\0';
					}
				}
				else
				{
					CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
					return FAIL;
				}
			}
			break;
		}

		case ADV_CFG_FIELD_TWO_PERSON_MODE:
			tempSysAdvPara.twoPersonMode = (TWO_PERSON_MODE_e)fieldValue;
			break;

		case ADV_CFG_FIELD_AUTO_ALARM_ACK_ENABLE:
			tempSysAdvPara.autoAlarmAck= (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_AUTO_ALARM_ACK_TIMER:
			if(strlen(tempConfigBuff))
			{
				tempSysAdvPara.autoAlarmAckTimer = (UINT16)fieldValue;
			}
			else
			{
				tempSysAdvPara.autoAlarmAckTimer = 0;
			}
			break;

		case ADV_CFG_FIELD_MIFARE_CUSTOM_KEY_ENBL:
			tempSysAdvPara.mifareCustomKeyEnbl = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_MIFARE_CUSTOM_KEY:
			if(strlen(tempConfigBuff) <= (2*MIFARE_KEY_SIZE) )
			{
				ConvertStringToHexArray(tempConfigBuff, tempSysAdvPara.mifareCustomKey, MIFARE_KEY_SIZE);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			break;

		case ADV_CFG_FIELD_HID_ICLASS_CUSTOM_KEY_ENBL:
			tempSysAdvPara.hidIclasCustomKeyEnbl = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_HID_ICLASS_CUSTOM_KEY:
			if(strlen(tempConfigBuff) <= (2*HID_ICLASS_KEY_SIZE) )
			{
				ConvertStringToHexArray(tempConfigBuff, tempSysAdvPara.hidIclasCustomKey, HID_ICLASS_KEY_SIZE);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			break;

		case ADV_CFG_FIELD_CUSTOM_KEY_AUTO_UPDATE_F:
			tempSysAdvPara.customKeyAutoUpdateF = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_SMART_ROUTE_ORDER:
			tempSysAdvPara.smartRouteOrder = (SMART_ROUTE_ORDER_TYPE_e)fieldValue;
			break;

		case ADV_CFG_FIELD_VERIFY_ACCESS_POLICY_ON_EXIT:
			tempSysAdvPara.verifyAccessPolicyOnExitF = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_ACCEPT_EXT_IO_LINK:
			tempSysAdvPara.acceptExtIOLinkF = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_OUTPUT_WAIT_TIMER:
			tempSysAdvPara.outputWaitTime = (UINT8)fieldValue;
			break;

		case ADV_CFG_FIELD_SELF_ENRL_ENABLE:
			tempSysAdvPara.selfEnrlEnbl = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_SELF_ENRL_RETRY_CNT:
			tempSysAdvPara.selfEnrlRetryCnt = (UINT8)fieldValue;
			break;

		case ADV_CFG_FIELD_VAM_ENABLE_F:
			tempSysAdvPara.vamEnable = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_ACCESS_ALLOW_USER_TYPE:
			tempSysAdvPara.dvrAllowEventuserType = (DVR_EVENT_USER_TYPE_e)fieldValue;
			break;

		case ADV_CFG_FIELD_ACCESS_DENY_USER_TYPE:
			tempSysAdvPara.dvrDenyEventUserType = (DVR_EVENT_USER_TYPE_e)fieldValue;
			break;

		case ADV_CFG_FIELD_OCCUPANCY_MIN_USERS:
			tempSysAdvPara.occupancyCtrl.minUsers = (UINT8)fieldValue;
			break;

		case ADV_CFG_FIELD_ZERO_OCCUPANCY:
			tempSysAdvPara.occupancyCtrl.zeroOccupancy = (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_JCM_ENABLE:
			tempSysAdvPara.jcmEnblF	= (BOOL)fieldValue;
			break;

		case ADV_CFG_FIELD_FACILITY_CODE:
			tempSysAdvPara.facilityCode	= (UINT16)fieldValue;
			break;

		case ADV_CFG_FIELD_VISITOR_PASS_SURRENDER_ENBL:
			tempSysAdvPara.visitorPassSurrenderEnblF = (BOOL)fieldValue;
			break;

		case ADV_CFG_CAFE_INTER_TRANSACTION_DELAY:
			tempSysAdvPara.cafeInterTransactionDelay = (UINT16)fieldValue;
			break;

		case ADV_CFG_FIRST_IN_RESET_AFTER:
			tempSysAdvPara.firstInResetAfter = (FIRST_IN_RESET_AFTER_TYPE_e)fieldValue;
			break;

		case ADV_CFG_FIRST_IN_LOCK_EXPIRE_TIMER:
			tempSysAdvPara.firstInTimer = (UINT16)fieldValue;
			break;

		case ADV_CFG_TWO_CRED_MUST_MODE_ENABLE:
			tempSysAdvPara.twoCredMustModeF = (BOOL)fieldValue;
			break;
		case ADV_CFG_SEQ_IN_OUT_MODE_ENABLE:
			tempSysAdvPara.sequentialInOut = (BOOL)fieldValue;
			break;

		case ADV_CFG_SHOW_PIN:
			tempSysAdvPara.showPin = (BOOL)fieldValue;
			break;

		case ADV_CFG_SHOW_ATTENDANCE_DETAIL:
			tempSysAdvPara.showAttndcDtlF = (BOOL)fieldValue;
			break;

		case ADV_CFG_APB_RESET_AFTER:
			tempSysAdvPara.antiPassBack.resetAfter = (APB_RESET_TYPE_e)fieldValue;
			break;

		case ADV_CFG_APB_FORGIVENESS_TIMER:
			tempSysAdvPara.antiPassBack.forgivenessTimer = (UINT16)fieldValue;
			break;

		case ADV_CFG_AUTO_HIDE_MENU_BAR_F:
			tempSysAdvPara.autoHideMenuBarF = (BOOL)fieldValue;
			break;

		case ADV_CFG_BIRTHDAY_MESSAGE:
		{
			UINT8 stringLen;
			stringLen = UTF8Strlen(tempConfigBuff);
			if(stringLen < GREETING_MSG_SIZE)
			{
				if(stringLen > 0)
				{
					sprintf(tempSysAdvPara.birthdayMessage, "%s", tempConfigBuff);
				}
				else
				{
					tempSysAdvPara.birthdayMessage[0] = '\0';
				}
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
		}
		break;

		case ADV_CFG_MJPEG_STREAM_API:
		{
			length = sizeof(tempFRCameraConfig.mjpeQuery);
			Base64Decode(tempConfigBuff, tempFRCameraConfig.mjpeQuery, &length);

			// enableFrCamera flag handled manually.
			if(strlen(tempConfigBuff) > 0)
			{
				tempFRCameraConfig.enableFrCamera = TRUE;
			}
			else
			{
				tempFRCameraConfig.enableFrCamera = FALSE;
			}
		}
		break;

		case ADV_CFG_STREAM_API_USER_NAME:
		{
			length = sizeof(tempFRCameraConfig.frCameraUserName);
			Base64Decode(tempConfigBuff, tempFRCameraConfig.frCameraUserName, &length);
		}
		break;

		case ADV_CFG_STREAM_API_PASSWORD:
		{
			length = sizeof(tempFRCameraConfig.frCameraPassword);
			Base64Decode(tempConfigBuff, tempFRCameraConfig.frCameraPassword, &length);
		}
		break;

		case ADV_CFG_RETAIN_JOB_SELECTION:
		{
			tempSysAdvPara.retainJobSelectionF = (BOOL)fieldValue;
		}
		break;
		
		case ADV_CFG_FIELD_SAVE_FP_IMAGE_FLAG:
		{
			tempSysAdvPara.saveFpImageF = (BOOL)fieldValue;
		}
		break;

		case ADV_CFG_TEMP_LOG_ENABLE:
		{
			tempSysAdvPara.temperatureLogCnfg.isEnable = (BOOL)fieldValue;
		}
		break;
		
		case ADV_CFG_TEMP_UNIT:
		{
			if(MAX_TEMPERATURE_UNIT <= fieldValue)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Temp Unit Value Receive = [%d]",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.temperatureLogCnfg.tempUnit = (TEMPERATURE_UNIT_e)fieldValue;
		}
		break;
		
		case ADV_CFG_SENSOR_TYPE:
		{
			if(MAX_TEMPERATURE_SENSOR_TYPE <= fieldValue)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Temp Sensor Type Receive = [%d]",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.temperatureLogCnfg.sensorType = (TEMPERATURE_SENSOR_TYPE_e)fieldValue;
		}
		break;
		
		case ADV_CFG_SENSOR_INTERFACE:
		{
			if(MAX_SENSOR_INTERFACE <= fieldValue)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Temp Sensor Interface Receive = [%d]",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.temperatureLogCnfg.sensorInterface = (SENSOR_INTERFACE_e)fieldValue;
		}
		break;
		
		case ADV_CFG_RESTRICTION_TYPE:
		{
			if(MAX_USER_RESTRICTION <= fieldValue)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid User Ristriction Receive = [%d]",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.temperatureLogCnfg.restrictionType = (USER_RESTRICTION_e)fieldValue;
		}
		break;
		
		case ADV_CFG_BYPASS_IF_SENSOR_DISCONNECT:
		{
			tempSysAdvPara.temperatureLogCnfg.bypassOnDisconnect = (BOOL)fieldValue;
		}
		break;
		
		case ADV_CFG_THRESHOLD_TEMP:
		{
			if((IsGivenFloatInValidRange(tempValue,THRESHLD_TEMP_FAHRENHEIT_VAL_MIN,THRESHLD_TEMP_FAHRENHEIT_VAL_MAX) == NO)
			  && (IsGivenFloatInValidRange(tempValue,THRESHLD_TEMP_SELSIUS_VAL_MIN,THRESHLD_TEMP_SELSIUS_VAL_MAX) == NO))
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Threshold Temperature Receive = [%f]",tempValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.temperatureLogCnfg.thresholdTemp = (FLOAT32)tempValue;
		}
		break;
		
		case ADV_CFG_CALIBRATION_PARA:
		{
			if(IsGivenFloatInValidRange(tempValue,CALIBRATION_VAL_MIN,CALIBRATION_VAL_MAX) == NO)			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Calibration Para Receive = [%f]",tempValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.temperatureLogCnfg.calibrationPara = (FLOAT32)tempValue;
		}
		break;

		case ADV_CFG_EMISSIVITY:
		{
			if(IsGivenFloatInValidRange(tempValue,EMISSIVITY_VAL_MIN,EMISSIVITY_VAL_MAX) == NO)			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Emissivity Receive = [%f]",tempValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.temperatureLogCnfg.emissivity = (FLOAT32)tempValue;
		}
		break;

		case ADV_CFG_APPROACH_TO_SENSER_WAIT_TMR:
		{
			if(IsGivenFloatInValidRange(tempValue,ZERO_FLOAT_VAL,APPROACH_WAIT_TIME_VAL_MAX) == NO)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Approach Wait timer Receive = [%f]",tempValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.temperatureLogCnfg.approachWaitTmr = tempValue;
		}
		break;

		case ADV_CFG_TEMPERATURE_DETECTION_TIME_OUT:
		{
			if(IsGivenDigitInValidRange(fieldValue,TEMP_DETECTION_TIME_VAL_MIN,TEMP_DETECTION_TIME_VAL_MAX) == NO)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Temperature detection timer Receive = [%d]",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.temperatureLogCnfg.tempDetectionTimer = (UINT8)fieldValue;
		}
		break;

		case ADV_CFG_TOLERANCE_BW_CONSE_READING:
		{
			if(IsGivenFloatInValidRange(tempValue,ZERO_FLOAT_VAL,THRESHOLD_DEVIATION_VAL_MAX) == NO)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Approach Wait timer Receive = [%f]",tempValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.temperatureLogCnfg.thresholdDeviation = tempValue;
		}
		break;

		case ADV_CFG_CONSE_READ_COUNT_WITHIN_TOLE:
		{
			if(tempSysAdvPara.temperatureLogCnfg.sensorType != SENSOR_FEVO)
			{
				if(IsGivenDigitInValidRange(fieldValue,TEMP_SAMPLE_CNT_VAL_MIN,TEMP_SAMPLE_CNT_VAL_MAX) == NO)
				{
					EPRINT(ACMS_RXTX_LOG, "Invalid Consecutive reading count within tolerance Receive = [%d]",fieldValue);
					CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
					return FAIL;
				}
				tempSysAdvPara.temperatureLogCnfg.validSampleCnt = fieldValue;
			}
			else
			{	//More than one reading is not required for FEVO sensor
				tempSysAdvPara.temperatureLogCnfg.validSampleCnt = 1;
			}
		}
		break;

		case ADV_CFG_MIN_TEMP_FOR_ACCESS:
		{
			if((IsGivenFloatInValidRange(tempValue,USER_ACCESS_TEMP_IN_SELSIUS_VAL_MIN,USER_ACCESS_TEMP_IN_SELSIUS_VAL_MAX) == NO)
			  && (IsGivenFloatInValidRange(tempValue,USER_ACCESS_TEMP_IN_FAHRENHEIT_VAL_MIN,USER_ACCESS_TEMP_IN_FAHRENHEIT_VAL_MAX) == NO))
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Minimum temperature Receive = [%f] for Access",tempValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.temperatureLogCnfg.minimumThreshold = tempValue;
		}
		break;
		
		case ADV_CFG_FIELD_FACE_ENROLL_MAX:
		{
			if( ( fieldValue <= FACE_ENROLL_CNT_MIN) || (fieldValue > (FACE_ENROLL_CNT_MAX+1)) )
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Max Face Enroll Value Receive = [%d] for Access",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.maxFacePerUser = (UINT8)fieldValue;
		}
		break;

		// Face Mask Compulsion - Enable
		case ADV_CFG_FACE_MASK_COMPULSION_ENBL:
		{
			tempSysAdvPara.faceMaskComp_Enable = (BOOL)fieldValue;
		}
		break;

		// Identification Face Mask Detection Threshold
		case ADV_CFG_IDT_FACE_MASK_DETECTION_THRESHOLD:
		{
			if(IsGivenFloatInValidRange(tempValue,MIN_MASK_DETECTION_THRESHOLD,MAX_MASK_DETECTION_THRESHOLD) == NO)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Identification Face Mask Detection Threshold Receive = [%f]",tempValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.faceMaskComp_IdtFaceMaskDetectionThreshold = (FLOAT32)tempValue;
		}
		break;

		// Visible Face For Enrollment threshold
		case ADV_CFG_VISIBLE_FACE_FOR_ENRL:
		{
			if(IsGivenFloatInValidRange(tempValue,MIN_MASK_DETECTION_THRESHOLD,MAX_MASK_DETECTION_THRESHOLD) == NO)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Visible Face For Enrollment threshold Receive = [%f]",tempValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.faceMaskComp_VisibleFaceForEnrl = (FLOAT32)tempValue;
		}
		break;

		// Approach to Camera Wait-Timer
		case ADV_CFG_APPROACH_TO_CAMERA_WAIT_TMR:
		{
			if(IsGivenFloatInValidRange(tempValue,MIN_APPROACH_CAMERA_WAIT_TIMER,MAX_APPROACH_CAMERA_WAIT_TIMER) == NO)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Approach to Camera Wait-Timer Receive = [%f]",tempValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.faceMaskComp_ApproachCameraWaitTimer = (FLOAT32)tempValue;
		}
		break;

		// Mask Detection Time-Out
		case ADV_CFG_MASK_DETECTION_TIME_OUT:
		{
			if(IsGivenDigitInValidRange(fieldValue,MIN_MASK_DETECTION_TIMEOUT,MAX_MASK_DETECTION_TIMEOUT) == NO)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Mask Detection Time-Out Receive = [%d]",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.faceMaskComp_MaskDetectionTimeOut = (UINT8)fieldValue;
		}
		break;

		// Restriction Type
		case ADV_CFG_FACE_MASK_RESTRICTION:
		{
			if(MAX_FACE_MASK_RESTRICTION <= fieldValue)
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Restriction Type Receive = [%d]",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
			tempSysAdvPara.faceMaskComp_RestrictionType = (FACE_MASK_RESTRICTION_e)fieldValue;
		}
		break;
		
		case ADV_CFG_FIELD_DURESS_ENABLE:
		{
			tempSysAdvPara.duressDetection = (BOOL)fieldValue;
		}
		break;

		case ADV_CFG_OFFLINE_ITEM_QUANTITY_RESTRICTION_ENABLE:
		{
			if((ENABLED == fieldValue) || (DISABLED == fieldValue))
			{
				tempSysAdvPara.itemQuantityRestrictionF = (BOOL)fieldValue;
			}
			else
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Item Quantity Restriction Receive = [%d]",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
		}
		break;

		case ADV_CFG_ALLOWED_OFFLINE_ITEM_QUANTITY:
		{
			if(fieldValue <= ALLOWED_OFFLINE_ITEM_QUANTITY_PER_MENU )
			{
				tempSysAdvPara.allowedOfflineItemQuantity = (UINT8)fieldValue;
			}
			else
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Allowed Offline Item Quantity Receive = [%d]",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
		}
		break;

		case ADV_CFG_AUTO_ADD_TEMP_USER:
		{
			if(IsGivenDigitInValidRange(fieldValue,0,2) == YES)
			{
				tempSysAdvPara.temporaryUserGenerateInfo.autoAddTempUser = (UINT8)fieldValue;
			}
			else
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Auto Add Temporary User value Receive = [%d]",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
		}
		break;

		case ADV_CFG_AUTO_ADD_TEMP_USER_NAME_VIA_DEVICE:
		{
			if(IsGivenDigitInValidRange(fieldValue,0,1) == YES)
			{
				tempSysAdvPara.temporaryUserGenerateInfo.addNameViaDeviceF = (UINT8)fieldValue;
			}
			else
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Auto Add Temporary User Name Via Device Flag Receive = [%d]",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
		}
		break;

		case ADV_CFG_AUTO_ADD_TEMP_USER_CONFIRM:
		{
			if(IsGivenDigitInValidRange(fieldValue,0,1) == YES)
			{
				tempSysAdvPara.temporaryUserGenerateInfo.confirmReqF = (UINT8)fieldValue;
			}
			else
			{
				EPRINT(ACMS_RXTX_LOG, "Invalid Auto Add Temporary User Confirm Flag Receive = [%d]",fieldValue);
				CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				return FAIL;
			}
		}
		break;

		default:
			{
				if(fieldId >= MAX_ADV_CFG_FIELD)
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in GEN_SYS_ADV_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
				}
			}break;
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(	(ConfigUpdateHandler(GEN_SYS_ADV_TABLE_ID, configIdx, (VOIDPTR)(&tempSysAdvPara)) == SUCCESS) &&
				(ConfigUpdateHandler(FACE_RECO_FR_CAMERA_UPDATE_ID, configIdx, (VOIDPTR)(&tempFRCameraConfig)) == SUCCESS)
			)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "GenSysParaAdv value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "GenSysParaAdvConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsNetworkParaConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsNetworkParaConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE],cmdBuf[128];
	UINT8 					configIdx, parseStatus;
	ETHERNET_PARA_t			tempEthernetParameters;	//take instance
	SERVER_PARA_t			tempServerParameters;	//take instance
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		CONFIGPRINT(CONFIG_FAIL, "NetworkParaConfig");
		return FAIL;
	}

	//get copy of current configuration
	tempEthernetParameters = EthernetParameters;
	tempServerParameters = ServerParameters;

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		switch(fieldId)
		{
		case NW_PARA_FIELD_IP:
		  	if(strlen(tempConfigBuff) < MAX_IP_ADDRESS)
			{
				sprintf(tempEthernetParameters.ipAddress, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "NetworkParaConfig");
				return FAIL;
			}
			break;

		case NW_PARA_FIELD_SUBNET:
		  	if(strlen(tempConfigBuff) < MAX_SUBNET_MASK)
			{
				sprintf(tempEthernetParameters.subnet, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "NetworkParaConfig");
				return FAIL;
			}
			break;

		case NW_PARA_FIELD_GATEWAY:
			if(strlen(tempConfigBuff) < MAX_GATEWAY)
			{
				sprintf(tempEthernetParameters.gateway, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "NetworkParaConfig");
				return FAIL;
			}
			break;

		case NW_PARA_FIELD_DNS:
			if(UTF8Strlen(tempConfigBuff) < MAX_DNS_ADDRESS)
			{
				sprintf(tempEthernetParameters.dns1, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "NetworkParaConfig");
				return FAIL;
			}
			break;

			//store ip address into host name connection type is fixed as host name
		case NW_PARA_FIELD_SERVER_IP:
			if(strlen(tempConfigBuff) < MAX_IP_ADDRESS)
			{
				switch(acmsCurActiveInterface)
				{
					default:
					case ETHERNET_PORT:
					{
						sprintf(tempServerParameters.ethServerHostName, "%s", tempConfigBuff);
						break;
					}

					case WIFI_PORT:
					{
						sprintf(tempServerParameters.wifiServerHostName, "%s", tempConfigBuff);
						break;
					}

					case PPP_PORT:
					{
						sprintf(tempServerParameters.modemServerHostName, "%s", tempConfigBuff);
						break;
					}
				}
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "NetworkParaConfig");
				return FAIL;
			}
			break;

		case NW_PARA_FIELD_SERVER_PORT:
			switch(acmsCurActiveInterface)
			{
				default:
				case ETHERNET_PORT:
				{
					tempServerParameters.ethServerPort = (UINT16)atoi(tempConfigBuff);
					break;
				}

				case WIFI_PORT:
				{
					tempServerParameters.wifiServerPort = (UINT16)atoi(tempConfigBuff);
					break;
				}

				case PPP_PORT:
				{
					tempServerParameters.modemServerPort = (UINT16)atoi(tempConfigBuff);
					break;
				}
			}
			break;

			default:
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in NETWORK_PARA_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "NetworkParaConfig");
				}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(ETHERNET_PARA_TABLE_UPDATE_ID, configIdx, (VOIDPTR)(&tempEthernetParameters)) == SUCCESS)
			{
				//Delete deviceIpSignature.txt on change of LAN Settings
				memset(cmdBuf,0,sizeof(cmdBuf));
				sprintf(cmdBuf,"rm %s",DEVICE_IP_SIGN_FILE);
				system(cmdBuf);

				if(ConfigUpdateHandler(SERVER_PARA_TABLE_UPDATE_ID, configIdx, (VOIDPTR)(&tempServerParameters)) == SUCCESS)
				{
					//Delete serverIpSignature.txt on change of Server Settings
					memset(cmdBuf,0,sizeof(cmdBuf));
					sprintf(cmdBuf,"rm %s",SERVER_IP_SIGN_FILE);
					system(cmdBuf);
					return SUCCESS;
				}
			}
		}
	}

	EPRINT(ACMS_LOG, "NetworkPara value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "NetworkParaConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsDstConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsDstConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 					parseStatus;
	UINT16 					configIdx;
	DST_CONFIG_t			tempDstConfig;
	UINT32					fieldValue;
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		CONFIGPRINT(CONFIG_FAIL, "DstConfig");
		return FAIL;
	}

	// Get current configuration
	tempDstConfig = DstConfig;

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);

		switch(fieldId)
		{
			case DST_FIELD_MODE:
				tempDstConfig.mode = (DST_MODE_e)fieldValue;
				break;

			case DST_FIELD_FORWARD_DATE_WISE_DAY:
				tempDstConfig.forward.dateWise.day = (UINT8)fieldValue;
				break;

			case DST_FIELD_FORWARD_DATE_WISE_MONTH:
				tempDstConfig.forward.dateWise.month = (UINT8)fieldValue - MONTH_OFFSET;
				break;

			case DST_FIELD_FORWARD_DAY_WISE_ORDINAL:
				tempDstConfig.forward.dayWise.weekOrdinal = (UINT8) fieldValue - ORDINAL_OFFSET;
				break;

			case DST_FIELD_FORWARD_DAY_WISE_DAY:
				tempDstConfig.forward.dayWise.weekDay = (UINT8)fieldValue;
				break;

			case DST_FIELD_FORWARD_DAY_WISE_MONTH:
				tempDstConfig.forward.dayWise.month	= (UINT8)fieldValue - MONTH_OFFSET;
				break;

			case DST_FIELD_FORWARD_TIME:
				if(GetTimeFromString(tempConfigBuff, &tempDstConfig.forward.time) == FAIL)
				{
					CONFIGPRINT(CONFIG_FAIL, "DstConfig");
					return FAIL;
				}
				break;

			case DST_FIELD_BACKWARD_DATE_WISE_DAY:
				tempDstConfig.backward.dateWise.day = (UINT8)fieldValue;
				break;

			case DST_FIELD_BACKWARD_DATE_WISE_MONTH:
				tempDstConfig.backward.dateWise.month = (UINT8)fieldValue - MONTH_OFFSET;
				break;

			case DST_FIELD_BACKWARD_DAY_WISE_ORDINAL:
				tempDstConfig.backward.dayWise.weekOrdinal = (UINT8)fieldValue - ORDINAL_OFFSET;
				break;

			case DST_FIELD_BACKWARD_DAY_WISE_DAY:
				tempDstConfig.backward.dayWise.weekDay = (UINT8)fieldValue;
				break;

			case DST_FIELD_BACKWARD_DAY_WISE_MONTH:
				tempDstConfig.backward.dayWise.month = (UINT8)fieldValue - MONTH_OFFSET;
				break;

			case DST_FIELD_BACKWARD_TIME:
				if(GetTimeFromString(tempConfigBuff, &tempDstConfig.backward.time) == FAIL)
				{
					CONFIGPRINT(CONFIG_FAIL, "DstConfig");
					return FAIL;
				}
				break;

			case DST_FIELD_TIME_DURATION:
				if(GetTimeFromString(tempConfigBuff, &tempDstConfig.timePeriod) == FAIL)
				{
					CONFIGPRINT(CONFIG_FAIL, "DstConfig");
					return FAIL;
				}
				break;

			default:
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in DST_PARA_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "DstConfig");
				}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(DST_PARA_TABLE_ID, configIdx, (VOIDPTR)(&tempDstConfig)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "DST value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "DstConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsSplFunctionConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsSplFunctionConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8					parseStatus;
	UINT16					configIdx;
	SPECIAL_FUNCTION_t		tempSpecialFunctions;	//take instance
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_SPECIAL_FUNC)
	{
		CONFIGPRINT(CONFIG_FAIL, "SplFunctionConfig");
		return FAIL;
	}

	// Get current configuration
	tempSpecialFunctions = SpecialFunctions[configIdx];

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE)) != FAIL)
	{
		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
		case SPL_FN_FIELD_ENABLE:
			tempSpecialFunctions.enable = (BOOL)MxAsciiToUINT32(tempConfigBuff);
			break;

		case SPL_FN_FIELD_CARD1:
		case SPL_FN_FIELD_CARD2:
		case SPL_FN_FIELD_CARD3:
		case SPL_FN_FIELD_CARD4:
			if(strlen(tempConfigBuff) > 0)
			{
				//sscanf for unsigned long long data because atoll function
				//return only signed data
				sscanf(tempConfigBuff, "%llu", &tempSpecialFunctions.cardId[fieldId - SPL_FN_FIELD_CARD1]);
			}
			else
			{
				tempSpecialFunctions.cardId[fieldId - SPL_FN_FIELD_CARD1] = INVALID_CARD_ID;
			}
			break;
		case SPL_FN_FIELD_PROMPT_JOB_CODES:
			tempSpecialFunctions.promptJobCodeF = (BOOL)MxAsciiToUINT32(tempConfigBuff);
			break;

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in SPECIAL_FUNC_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "SplFunctionConfig");
			}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(SPECIAL_FUNC_TABLE_ID, configIdx, (VOIDPTR)(&tempSpecialFunctions)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG,"Special Function value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "SplFunctionConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsVisitorMgmtConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsVisitorMgmtConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 					parseStatus;
	UINT16 					configIdx;		//configuration index
	VISITOR_MANAGEMENT_t	tempVisitor;	//take instance
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_VISITORS)
	{
		CONFIGPRINT(CONFIG_FAIL, "VisitorMgmtConfig");
		return FAIL;
	}

	//get copy of current configuration
	tempVisitor = Visitor[configIdx];

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		switch(fieldId)
		{
		case VISITOR_CFG_FIELD_NAME:
			if(UTF8Strlen(tempConfigBuff) < MX_USER_NAME_SIZE)
			{
				snprintf(tempVisitor.visitorName, NAME_SIZE_UTF8, "%s", tempConfigBuff);
			}
			else
			{
				snprintf(tempVisitor.visitorName, GetActualUTF8Strlen(tempConfigBuff, MX_USER_NAME_SIZE), "%s", tempConfigBuff);
			}
			break;

		case VISITOR_CFG_FIELD_START_DATE:
			if(GetDateFromString(tempConfigBuff, &tempVisitor.startDate, YEAR_OFFSET_2000) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "VisitorMgmtConfig");
				return FAIL;
			}
			break;

		case VISITOR_CFG_FIELD_START_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempVisitor.startTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "VisitorMgmtConfig");
				return FAIL;
			}
			break;

		case VISITOR_CFG_FIELD_END_DATE:
			if(GetDateFromString(tempConfigBuff, &tempVisitor.endDate, YEAR_OFFSET_2000) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "VisitorMgmtConfig");
				return FAIL;
			}
			break;

		case VISITOR_CFG_FIELD_END_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempVisitor.endTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "VisitorMgmtConfig");
				return FAIL;
			}
			break;

		case VISITOR_CFG_FIELD_REPEAT_MODE:
			if(strlen(tempConfigBuff) > 0)
			{
				tempVisitor.repeatMode = (REPEAT_MODE_VMS_e)atoi(tempConfigBuff);
			}
			else
			{
				tempVisitor.repeatMode = DO_NOT_REPEAT;
			}
			break;


		case VISITOR_CFG_FIELD_REPEAT_DAYS:
		{
			UINT8 idx;
			if((strlen(tempConfigBuff) > 0) && (strlen(tempConfigBuff) <= MAX_REPEAT_DAYS))
			{
				memset(tempVisitor.repeatDays, 0, MAX_REPEAT_DAYS);
				for(idx = 0; idx < MAX_REPEAT_DAYS; idx++)
				{
					tempVisitor.repeatDays[idx] = (BOOL)(tempConfigBuff[idx] - '0');
				}
			}
			else
			{
				memset(tempVisitor.repeatDays, 0, MAX_REPEAT_DAYS);
				for(idx = 0; idx < MAX_REPEAT_DAYS; idx++)
				{
					tempVisitor.repeatDays[idx] = (BOOL)NO_REPEAT_DAY;
				}
			}
			break;
		}

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in VISITOR_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "VisitorMgmtConfig");
			}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(VISITOR_TABLE_ID, configIdx, (VOIDPTR)(&tempVisitor)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "Visitor value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "VisitorMgmtConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsTimeTriggerConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsTimeTriggerConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8					parseStatus;
	UINT16					configIdx;
	TIME_TRIG_FUNC_t		tempTimeTrigFunc;
	UINT32					fieldValue;
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_TIME_TRIG_FUNC)
	{
		CONFIGPRINT(CONFIG_FAIL, "TimeTriggerConfig");
		return FAIL;
	}

	// Get current configuration
	tempTimeTrigFunc = TimeTrigFunc[configIdx];

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE)) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);

		switch(fieldId)
		{
		case TIME_TRIG_FIELD_ENABLE:
			tempTimeTrigFunc.enable = (BOOL)fieldValue;
			break;

		case TIME_TRIG_FIELD_NAME:
			if(UTF8Strlen(tempConfigBuff) < MX_USER_NAME_SIZE)
			{
				snprintf(tempTimeTrigFunc.name, MX_USER_NAME_SIZE, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "TimeTriggerConfig");
				return FAIL;
			}
			break;

		case TIME_TRIG_FIELD_START_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempTimeTrigFunc.startTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "TimeTriggerConfig");
				return FAIL;
			}
			break;

		case TIME_TRIG_FIELD_DURATION:
			tempTimeTrigFunc.duration = (UINT16)fieldValue;
			break;

		case TIME_TRIG_FIELD_OP_PORT:
			tempTimeTrigFunc.outputPort = ( (UINT8)fieldValue) - ACMS_INDEX_OFFSET;
			break;

		case TIME_TRIG_FIELD_ACTIVE_DAY0:
		case TIME_TRIG_FIELD_ACTIVE_DAY1:
		case TIME_TRIG_FIELD_ACTIVE_DAY2:
		case TIME_TRIG_FIELD_ACTIVE_DAY3:
		case TIME_TRIG_FIELD_ACTIVE_DAY4:
		case TIME_TRIG_FIELD_ACTIVE_DAY5:
		case TIME_TRIG_FIELD_ACTIVE_DAY6:
			tempTimeTrigFunc.workingDays[fieldId - TIME_TRIG_FIELD_ACTIVE_DAY0] = (BOOL)fieldValue;
			break;

		case TIME_TRIG_FIELD_HOLIDAY_OVERRIDE:
			tempTimeTrigFunc.holidayWorking = (BOOL)fieldValue;
			break;

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in TIME_TRIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff );
				CONFIGPRINT(CONFIG_FAIL, "TimeTriggerConfig");
			}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(TIME_TRIG_TABLE_ID, configIdx, (VOIDPTR)(&tempTimeTrigFunc)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "Time Trigger set Fail");
	CONFIGPRINT(CONFIG_FAIL, "TimeTriggerConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsCafeItemConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//*****************************************************************************
BOOL SetAcmsCafeItemConfigFields(CHARPTR *rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8					parseStatus;
	UINT16					configIdx;
	CAFE_ITEM_CONFIG_t		tempCafeItemConfig;
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_CAFE_ITEM_CODE)
	{
		CONFIGPRINT(CONFIG_FAIL, "CafeItemConfig");
		return FAIL;
	}

	tempCafeItemConfig = CafeItemConfig[configIdx];

	//Check upto failure or upto last data extraction completed
	while( ( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL) )
	{
		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
			case CAFE_ITEM_FIELD_NAME:
				if(UTF8Strlen(tempConfigBuff) < SIZE_CAFE_ITEM_NAME)
				{
					sprintf( tempCafeItemConfig.name, "%s", tempConfigBuff);
				}
				else
				{
					CONFIGPRINT(CONFIG_FAIL, "CafeItemConfig");
					return FAIL;
				}
				break;

			case CAFE_ITEM_FIELD_PRICE:
				tempCafeItemConfig.price = (FLOAT32)atof(tempConfigBuff);
				break;

			case CAFE_ITEM_FIELD_DISCOUNT_ENABLE:
				tempCafeItemConfig.disCntEnabled  = (BOOL)atoi(tempConfigBuff);
				break;

			case CAFE_ITEM_FIELD_DISCOUNT_1:
			case CAFE_ITEM_FIELD_DISCOUNT_2:
			case CAFE_ITEM_FIELD_DISCOUNT_3:
			case CAFE_ITEM_FIELD_DISCOUNT_4:
				tempCafeItemConfig.disCnt[fieldId - CAFE_ITEM_FIELD_DISCOUNT_1] =
												(FLOAT32)atof(tempConfigBuff);
				break;

			default:
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in CAFE_ITEM_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "CafeItemConfig");
				}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(CAFE_ITEM_TABLE_ID, configIdx, (VOIDPTR)(&tempCafeItemConfig)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "Cafe Item Configuration set Fail");
	CONFIGPRINT(CONFIG_FAIL, "CafeItemConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsCafeMenuConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsCafeMenuConfigFields(CHARPTR *rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8					parseStatus;
	UINT16					configIdx;
	CAFE_MENU_CONFIG_t		tempCafeMenuConfig;
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_CAFE_MENU_CODE)
	{
		CONFIGPRINT(CONFIG_FAIL, "CafeMenuConfig");
		return FAIL;
	}

	//Get current data
	tempCafeMenuConfig = CafeMenuConfig[configIdx];

	//Check upto failure or upto last data extraction completed
	while( ( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL) )
	{
		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
			case CAFE_MENU_FIELD_ENABLE:
				tempCafeMenuConfig.active = (BOOL)atoi(tempConfigBuff);
				break;

			case CAFE_MENU_FIELD_DFLT_ITEM:
				tempCafeMenuConfig.dfltItemEnbl = (BOOL)atoi(tempConfigBuff);
				break;

			case CAFE_MENU_FIELD_NAME:
				// No need to save menu name as it is not going to used in
				// single door controller small screen.
				break;

			case CAFE_MENU_FIELD_ITEM_1 ... CAFE_MENU_FIELD_ITEM_99:

				if( (UINT8)atoi(tempConfigBuff) > 0)
				{
					tempCafeMenuConfig.itemCode[fieldId - CAFE_MENU_FIELD_ITEM_1] =
													( ( (UINT8)atoi(tempConfigBuff) ) - ACMS_INDEX_OFFSET);
				}
				else
				{
					tempCafeMenuConfig.itemCode[fieldId - CAFE_MENU_FIELD_ITEM_1] = INVALID_CAFE_ITEM_CODE;
				}
				break;

			default:
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in CAFE_MENU_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "CafeMenuConfig");
				}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(CAFE_MENU_TABLE_ID, configIdx, (VOIDPTR)(&tempCafeMenuConfig)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "Cafe Menu Configuration set Fail");
	CONFIGPRINT(CONFIG_FAIL, "CafeMenuConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsCafeMenuSchConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsCafeMenuSchConfigFields(CHARPTR *rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8					parseStatus;
	UINT16					configIdx;
	CAFE_MENU_SCHEDULE_t	tempCafeMenuSchConfig;
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_CAFE_MENU_SCHEDULE)
	{
		CONFIGPRINT(CONFIG_FAIL, "CafeMenuSchConfig");
		return FAIL;
	}

	//Get current data
	tempCafeMenuSchConfig = CafeMenuSchConfig[configIdx];

	//Check upto failure or upto last data extraction completed
	while( ( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL) )
	{
		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
			case CAFE_MENU_SCH_FIELD_MENU_CODE:
				if( (UINT8)atoi(tempConfigBuff) > 0)
				{
					tempCafeMenuSchConfig.menuCode = ( ( (UINT8)atoi(tempConfigBuff) ) - ACMS_INDEX_OFFSET);
				}
				else
				{
					tempCafeMenuSchConfig.menuCode = INVALID_CAFE_ITEM_CODE;
				}
				break;

			case CAFE_MENU_SCH_FIELD_START_TIME:
				if(GetTimeFromString(tempConfigBuff, &tempCafeMenuSchConfig.dayTimeInfo.start) == FAIL)
				{
					CONFIGPRINT(CONFIG_FAIL, "CafeMenuSchConfig");
					return FAIL;
				}
				break;

			case CAFE_MENU_SCH_FIELD_END_TIME:
				if(GetTimeFromString(tempConfigBuff, &tempCafeMenuSchConfig.dayTimeInfo.end) == FAIL)
				{
					CONFIGPRINT(CONFIG_FAIL, "CafeMenuSchConfig");
					return FAIL;
				}
				break;

			case CAFE_MENU_SCH_FIELD_ACTIVE_DAY0:
			case CAFE_MENU_SCH_FIELD_ACTIVE_DAY1:
			case CAFE_MENU_SCH_FIELD_ACTIVE_DAY2:
			case CAFE_MENU_SCH_FIELD_ACTIVE_DAY3:
			case CAFE_MENU_SCH_FIELD_ACTIVE_DAY4:
			case CAFE_MENU_SCH_FIELD_ACTIVE_DAY5:
			case CAFE_MENU_SCH_FIELD_ACTIVE_DAY6:
				tempCafeMenuSchConfig.dayTimeInfo.workingDays[(fieldId - CAFE_MENU_SCH_FIELD_ACTIVE_DAY0)] = (BOOL)atoi(tempConfigBuff);
				break;

			default:
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in CAFE_MENU_SCHEDULE_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "CafeMenuSchConfig");
				}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(CAFE_MENU_SCHEDULE_TABLE_ID, configIdx, (VOIDPTR)(&tempCafeMenuSchConfig)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "Cafe Menu Schedule Configuration set Fail");
	CONFIGPRINT(CONFIG_FAIL, "CafeMenuSchConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsPrinterConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsPrinterConfigFields(CHARPTR *rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8					parseStatus;
	UINT8					tempDigit;
	UINT64					field;
	PRINTER_CONFIG_t		tempPrinterConfig;
	ACMS_CONFIG_FIELD_ID	fieldId;

	//Parse and check index
	if(ParseField(rxData,&field) == FAIL)
	{
		CONFIGPRINT(CONFIG_FAIL, "PrinterConfig");
		return FAIL;
	}

	//Get current data
	tempPrinterConfig = PrinterConfig;

	//Check upto failure or upto last data extraction completed
	while( ( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL) )
	{
		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
			case PRINTER_TYPE:
				tempPrinterConfig.printerType =	(PRINTER_TYPE_e)atoi(tempConfigBuff);
				break;

			case PRINTER_COMM_TYPE:
				tempPrinterConfig.printerCommType = (PRINTER_COMM_TYPE_e)atoi(tempConfigBuff);
				break;

			case PRINTER_COMPANY_NAME:
				if(UTF8Strlen(tempConfigBuff) < MAX_PRINTEABLE_SIZE)
				{
					sprintf(tempPrinterConfig.companyName, "%s", tempConfigBuff);
				}
				else
				{
					CONFIGPRINT(CONFIG_FAIL, "PrinterConfig");
					return FAIL;
				}
				break;
			case PRINTER_COMPANY_ADDRESS1:
			case PRINTER_COMPANY_ADDRESS2:
			case PRINTER_COMPANY_ADDRESS3:
			case PRINTER_COMPANY_ADDRESS4:
				if(UTF8Strlen(tempConfigBuff) < MAX_PRINTEABLE_SIZE)
				{
					sprintf(tempPrinterConfig.companyAddr[(fieldId - PRINTER_COMPANY_ADDRESS1)], "%s", tempConfigBuff);
				}
				else
				{
					CONFIGPRINT(CONFIG_FAIL, "PrinterConfig");
					return FAIL;
				}
				break;
			case PRINTER_PUNCH_LINE:
				if(UTF8Strlen(tempConfigBuff) < MAX_PRINTEABLE_SIZE)
				{
					sprintf(tempPrinterConfig.punchLine, "%s", tempConfigBuff);
				}
				else
				{
					CONFIGPRINT(CONFIG_FAIL, "PrinterConfig");
					return FAIL;
				}
				break;

			case PRINTER_BAUD_RATE:
				tempDigit =(PRINTER_BAUD_RATE_e)atoi(tempConfigBuff);
				tempPrinterConfig.baudRate = baudRateValue[tempDigit];
				break;

			case PRINTER_EXCLUDE_PRICE_FLAG:
				tempPrinterConfig.excludePriceFrmCpn = (BOOL)atoi(tempConfigBuff);
				break;

			case PRINTER_USER_BALANCE_ON_COUPON_FLAG:
				tempPrinterConfig.printUserBalanceOnCouponF = (BOOL)atoi(tempConfigBuff);
				break;

			default:
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in PRINTER_PARA_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "PrinterConfig");
				}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(PRINTER_PARA_TABLE_ID, 0, (VOIDPTR)(&tempPrinterConfig)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "Printer Configuration set Fail");
	CONFIGPRINT(CONFIG_FAIL, "PrinterConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsTimeStampConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsTimeStampConfigFields(UNUSED_PARA CHARPTR * rxData)
{
	CONFIGPRINT(CONFIG_FAIL, "TimeStampConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsGreetingMsgConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsGreetingMsgConfigFields(UNUSED_PARA CHARPTR * rxData)
{
	CONFIGPRINT(CONFIG_FAIL, "GreetingMsgConfig");
	return FAIL;
}

//*****************************************************************************
//	GetDateFromString()
//	Param:
//		IN:	Date String
//			year offset
//		OUT:DATE_DDMMYY_t;
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function take date as string and return DATE_DDMMYY_t structure
//
//*****************************************************************************
static BOOL GetDateFromString(CHARPTR dateStr, DATE_DDMMYY_t * date, UINT16 yearOffset)
{
	BOOL	retVal = FAIL;
	UINT16	tmpYear;

	//we require date in specific format
	if(strlen(dateStr) == ACMS_DATE_STR_LEN)
	{
		date->day = (UINT8)GetUINT16FromStringAndLen ( (dateStr + ACMS_DATE_TIME_DAY_STR_OFFSET), ACMS_DATE_DAY_STR_LEN);
		date->month = (UINT8)GetUINT16FromStringAndLen ( (dateStr + ACMS_DATE_TIME_MONTH_STR_OFFSET), ACMS_DATE_MONTH_STR_LEN) - ACMS_INDEX_OFFSET;;
		tmpYear = GetUINT16FromStringAndLen( (dateStr + ACMS_DATE_TIME_YEAR_STR_OFFSET), ACMS_DATE_YEAR_STR_LEN);

		if(tmpYear >= yearOffset)
		{
			date->year = tmpYear - yearOffset;
			retVal = SUCCESS;
		}
	}
	return(retVal);
}

//*****************************************************************************
//	GetTimeFromString()
//	Param:
//		IN:	Time String
//		OUT:TIME_HHMM_t;
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function take time as string and return TIME_HHMM_t structure
//
//*****************************************************************************
BOOL GetTimeFromString(CHARPTR timeStr, TIME_HHMM_t * time)
{
	BOOL	retVal = FAIL;

	//we require date in specific format
	if(strlen(timeStr) == ACMS_TIME_HHMM_STR_LEN)
	{
		time->hour = (UINT8)GetUINT16FromStringAndLen ( (timeStr + ACMS_TIME_HOUR_STR_OFFSET), ACMS_TIME_HOUR_STR_LEN);
		time->minute = (UINT8)GetUINT16FromStringAndLen ( (timeStr + ACMS_TIME_MINUTE_STR_OFFSET), ACMS_TIME_MINUTE_STR_LEN);
		retVal = SUCCESS;
	}
	return(retVal);
}

//*****************************************************************************
//	SetAcmsFprConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsFprConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8			 		parseStatus, fprPara;
	UINT64					field;
	FPR_CONFIG_t 			tempFprConfig;
	ACMS_CONFIG_FIELD_ID	fieldId;

	//Parse and check index
	if(ParseField(rxData,&field) == FAIL)
	{
		CONFIGPRINT(CONFIG_FAIL, "FprConfig");
		return FAIL;
	}

	// Get current configuration
	tempFprConfig = FprConfig;

	//Check upto failure or upto last data extraction completed
	while( ( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL) )
	{
		// convert data
		fprPara = (UINT8)MxAsciiToUINT32(tempConfigBuff);

		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
		case FPR_CFG_FIELD_LIGHT_CONDITION:
			if(fprPara > MAX_LIGHT_CONDITION)
			{
				CONFIGPRINT(CONFIG_FAIL, "FprConfig");
				return FAIL;
			}
			tempFprConfig.lightCondition = fprPara;
			break;

		case FPR_CFG_FIELD_SENSIVITY_LEVEL:
			if(fprPara > MAX_SENSITIVITY_LEVEL)
			{
				CONFIGPRINT(CONFIG_FAIL, "FprConfig");
				return FAIL;
			}
			tempFprConfig.sensitivityLevel = fprPara;
			break;

		case FPR_CFG_FIELD_IMAGE_QUALITY:
			if(fprPara > MAX_IMAGE_QUALITY)
			{
				CONFIGPRINT(CONFIG_FAIL, "FprConfig");
				return FAIL;
			}
			tempFprConfig.imageQuality = fprPara;
			break;

		case FPR_CFG_FIELD_SEC_LEVEL:
			if(fprPara > MAX_SECURITY_LEVEL)
			{
				CONFIGPRINT(CONFIG_FAIL, "FprConfig");
				return FAIL;
			}
			tempFprConfig.secLevel = fprPara;
			break;

		case FPR_CFG_FIELD_FAST_MODE:
			if(fprPara > MAX_FAST_MODES)
			{
				CONFIGPRINT(CONFIG_FAIL, "FprConfig");
				return FAIL;
			}
			tempFprConfig.fastMode = fprPara;
			break;

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in FPR_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "FprConfig");
			}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(FPR_CONFIG_TABLE_ID, 0, (VOIDPTR)(&tempFprConfig)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	CONFIGPRINT(CONFIG_FAIL, "FprConfig");
	return FAIL;
}

//*****************************************************************************
// 	SetAcmsDvrEvtActnConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsDvrEvtActnConfigFields(CHARPTR * rxData)
{
	CHAR 						tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 						parseStatus;
	UINT16 						configIdx;
	DVR_EVENT_ACTION_CONFIG_t	tempDvrEvtActnConfig;
	UINT32						fieldValue;
	ACMS_CONFIG_FIELD_ID		fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_DVR_EVT_ACTN_CONFIG)
	{
		CONFIGPRINT(CONFIG_FAIL, "DvrEvtActnConfig");
		return FAIL;
	}

	// Get current configuration
	tempDvrEvtActnConfig = DvrEvtActionConfig[configIdx];

	//First we set selectCamera to zero and then oring with 1 at appropriate position
	tempDvrEvtActnConfig.selectCamera = 0;

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);

		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
		case DVR_INTEGRATION_FIELD_DOOR_ID:
			//we are ignoring door id because, for directdoor doorId is constant.
			//To keep same config table on acms side for panel and directdoor this field added.
			break;

		case DVR_INTEGRATION_FIELD_ACTIVE:
			tempDvrEvtActnConfig.active = (BOOL)fieldValue;
			break;

		case DVR_INTEGRATION_FIELD_EVT_TYPE:
			tempDvrEvtActnConfig.evtType = fieldValue;
			break;

		case DVR_INTEGRATION_FIELD_ACTN_TYPE:
			tempDvrEvtActnConfig.actionType = fieldValue;
			break;

		case DVR_INTEGRATION_FIELD_STRT_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempDvrEvtActnConfig.startTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "DvrEvtActnConfig");
				return FAIL;
			}
			break;

		case DVR_INTEGRATION_FIELD_END_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempDvrEvtActnConfig.endTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "DvrEvtActnConfig");
				return FAIL;
			}
			break;

		case DVR_INTEGRATION_FIELD_ACTIVE_DAY0:
		case DVR_INTEGRATION_FIELD_ACTIVE_DAY1:
		case DVR_INTEGRATION_FIELD_ACTIVE_DAY2:
		case DVR_INTEGRATION_FIELD_ACTIVE_DAY3:
		case DVR_INTEGRATION_FIELD_ACTIVE_DAY4:
		case DVR_INTEGRATION_FIELD_ACTIVE_DAY5:
		case DVR_INTEGRATION_FIELD_ACTIVE_DAY6:
			tempDvrEvtActnConfig.activeDaysF[(fieldId - DVR_INTEGRATION_FIELD_ACTIVE_DAY0)] = (BOOL)fieldValue;
			break;

		case DVR_INTEGRATION_FIELD_HOLIDAY_OVER:
			tempDvrEvtActnConfig.holiDayOverrideF = (BOOL)fieldValue;
			break;

		case DVR_INTEGRATION_FIELD_CAMERA1:
		case DVR_INTEGRATION_FIELD_CAMERA2:
		case DVR_INTEGRATION_FIELD_CAMERA3:
		case DVR_INTEGRATION_FIELD_CAMERA4:
		case DVR_INTEGRATION_FIELD_CAMERA5:
		case DVR_INTEGRATION_FIELD_CAMERA6:
		case DVR_INTEGRATION_FIELD_CAMERA7:
		case DVR_INTEGRATION_FIELD_CAMERA8:
		case DVR_INTEGRATION_FIELD_CAMERA9:
		case DVR_INTEGRATION_FIELD_CAMERA10:
		case DVR_INTEGRATION_FIELD_CAMERA11:
		case DVR_INTEGRATION_FIELD_CAMERA12:
		case DVR_INTEGRATION_FIELD_CAMERA13:
		case DVR_INTEGRATION_FIELD_CAMERA14:
		case DVR_INTEGRATION_FIELD_CAMERA15:
		case DVR_INTEGRATION_FIELD_CAMERA16:
			tempDvrEvtActnConfig.selectCamera = (tempDvrEvtActnConfig.selectCamera |
							( (fieldValue & 0x1) << (fieldId - DVR_INTEGRATION_FIELD_CAMERA1) ) );
			break;

		case DVR_INTEGRATION_FIELD_REC_DURATION:
			tempDvrEvtActnConfig.recDuration = fieldValue;
			break;

		case DVR_INTEGRATION_FIELD_POPUP_DURATION:
			tempDvrEvtActnConfig.popupDuration = fieldValue;
			break;

		case DVR_INTEGRATION_FIELD_PTZ_PRESET_POS:
			tempDvrEvtActnConfig.ptzPreset = fieldValue;
			break;

		case DVR_INTEGRATION_FIELD_EMAIL_ADDR:
			if(UTF8Strlen(tempConfigBuff) < MAX_EMAIL_ADDR_SIZE)
			{
				sprintf(tempDvrEvtActnConfig.emailAddr, "%s", tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "DvrEvtActnConfig");
				return FAIL;
			}
			break;

		case DVR_INTEGRATION_FIELD_READER_MODE:
			tempDvrEvtActnConfig.mode = (READER_MODE_e)fieldValue;
			break;

		case DVR_INTEGRATION_FIELD_CAMERA17:
		case DVR_INTEGRATION_FIELD_CAMERA18:
		case DVR_INTEGRATION_FIELD_CAMERA19:
		case DVR_INTEGRATION_FIELD_CAMERA20:
		case DVR_INTEGRATION_FIELD_CAMERA21:
		case DVR_INTEGRATION_FIELD_CAMERA22:
		case DVR_INTEGRATION_FIELD_CAMERA23:
		case DVR_INTEGRATION_FIELD_CAMERA24:
			//Earlier camera offset
			tempDvrEvtActnConfig.selectCamera = (tempDvrEvtActnConfig.selectCamera |
							( (fieldValue & 0x1) << (fieldId - DVR_INTEGRATION_FIELD_CAMERA17 + MAX_CAMERA_TILL_V6R3) ) );
			break;

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in DVR_EVT_ACTN_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "DvrEvtActnConfig");
			}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(DVR_EVT_ACTN_CONFIG_TABLE_ID, configIdx, (VOIDPTR)(&tempDvrEvtActnConfig)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	CONFIGPRINT(CONFIG_FAIL, "DvrEvtActnConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsSmartRouteAccessConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsSmartRouteAccessConfigFields(CHARPTR * rxData)
{
	CHAR 						tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 						parseStatus,tempDigit;
	UINT8 						configIdx;
	SMART_ROUTE_ACCESS_CONFIG_t	tempSmartRouteConfig;
	UINT32						fieldValue;
	ACMS_CONFIG_FIELD_ID		fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_SMART_ROUTE_ACCESS)
	{
		CONFIGPRINT(CONFIG_FAIL, "SmartRouteAccessConfig");
		return FAIL;
	}

	// Get current configuration
	tempSmartRouteConfig = SmartRouteAccessConfig[configIdx];

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);

		//set configuration data in temporary configuration structure
		switch(fieldId)
		{
		case SMART_ROUTE_ACCESS_FIELD_ENABLE:
			tempSmartRouteConfig.active = (BOOL)fieldValue;
			break;

		case SMART_ROUTE_ACCESS_FIELD_DOOR_LEVEL:
			tempDigit = fieldValue;
			if(tempDigit <= MAX_SMART_ROUTED_LEVEL)
			{
				tempSmartRouteConfig.doorLevel = (UINT8)tempDigit;
			}
			else
			{
				tempSmartRouteConfig.doorLevel = DEFAULT_DOOR_LEVEL;

			}
			break;

		case SMART_ROUTE_ACCESS_FIELD_TYPE:
			tempDigit = fieldValue;
			tempSmartRouteConfig.routeType = (SMART_ROUTE_ACCESS_TYPE_e)tempDigit;
			break;

		case SMART_ROUTE_ACCESS_FIELD_RESET_ON_START_LEVEL:
			tempSmartRouteConfig.resetOnStartLevel = (BOOL)fieldValue;
			break;

		case SMART_ROUTE_ACCESS_FIELD_ROUTE_MIN_LEVEL:
			tempSmartRouteConfig.minLevel = (UINT8)fieldValue;
			break;

		case SMART_ROUTE_ACCESS_FIELD_ROUTE_MAX_LEVEL:
			tempSmartRouteConfig.maxLevel = (UINT8)fieldValue;
			break;

		case SMART_ROUTE_ACCESS_FIELD_SQUENCED:
			tempSmartRouteConfig.sequencedF = (BOOL)fieldValue;
			break;

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in SMART_ROUTE_ACCESS_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "SmartRouteAccessConfig");
			}
		}

		//check for last data
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(SMART_ROUTE_ACCESS_CONFIG_TABLE_ID, configIdx, (VOIDPTR)(&tempSmartRouteConfig)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	CONFIGPRINT(CONFIG_FAIL, "SmartRouteAccessConfig");
	return FAIL;
}

//*****************************************************************************
// 	SetAcmsIoLinkExtDeviceConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsIoLinkExtDeviceConfigFields(CHARPTR * rxData)
{
	CHAR 						tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 						parseStatus;
	ACMS_CONFIG_FIELD_ID		fieldId;
	IO_LINK_EXT_DEV_CONFIG_t	tempIoLinkExtDevConfig[MAX_IO_LINK_EXT_DEVICE];

	//take temp copy of IoLinkExtDevConfig
	memcpy(&tempIoLinkExtDevConfig, &IoLinkExtDevConfig, sizeof(IoLinkExtDevConfig));

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		//set configuration data in temporary configuration structure
		if(fieldId > IO_LINK_EXT_DEVICE_CONFIG_FIELD_DEVICE_255_IP)
		{
			CONFIGPRINT(CONFIG_FAIL, "IoLinkExtDeviceConfig");
			return FAIL;
		}

		fieldId -= ACMS_INDEX_OFFSET;

		//set configuration data in configuration structure
		if(strlen(tempConfigBuff) < MAX_IP_ADDRESS)
		{
			strcpy(tempIoLinkExtDevConfig[fieldId].devIpAddr, tempConfigBuff);
		}
		else
		{
			CONFIGPRINT(CONFIG_FAIL, "IoLinkExtDeviceConfig");
			return FAIL;
		}

		//check for last data
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(IO_LINK_EXT_DEVICE_CONFIG_TABLE_ID, 0, &tempIoLinkExtDevConfig) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG,"Fialed to set External Io Link Device configuration.");
	CONFIGPRINT(CONFIG_FAIL, "IoLinkExtDeviceConfig");
	return FAIL;
}

//*****************************************************************************
// 	SetAcmsIoLinkExtDeviceMappingFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsIoLinkExtDeviceMappingFields(CHARPTR * rxData)
{
	CHAR 						tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 						parseStatus,tempDigit,configIdx;
	IO_LINK_EXT_DEV_MAPPING_t	tempIoLinkExtDevMapping;
	ACMS_CONFIG_FIELD_ID		fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_IO_LINK)
	{
		CONFIGPRINT(CONFIG_FAIL, "IoLinkExtDeviceMappingFields");
		return FAIL;
	}

	// Get current configuration
	tempIoLinkExtDevMapping = IoLinkExtDevMapping[configIdx];

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		//set configuration data in temporary configuration structure
		if(fieldId > IO_LINK_EXT_DEVICE_MAPPING_FIELD_DEVICE_255_ENABLE)
		{
			CONFIGPRINT(CONFIG_FAIL, "IoLinkExtDeviceMappingFields");
			return FAIL;
		}

		fieldId -= ACMS_INDEX_OFFSET;
		tempDigit = (UINT8)MxAsciiToUINT32(tempConfigBuff);

		// set mapping device enable flag
		tempIoLinkExtDevMapping.devEnable[fieldId] = (BOOL)tempDigit;

		//check for last data
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(IO_LINK_EXT_DEVICE_MAPPING_TABLE_ID, configIdx, (VOIDPTR)(&tempIoLinkExtDevMapping)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG,"Fialed to set External Io Link Device mapping.");
	CONFIGPRINT(CONFIG_FAIL, "IoLinkExtDeviceMappingFields");
	return FAIL;
}

//*****************************************************************************
// 	SetAcmsExtCardFormatConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//*****************************************************************************
BOOL SetAcmsExtCardFormatConfigFields(CHARPTR * rxData)
{
	return SetAcmsCardFormatConfigFields(EXTERNAL_PORT, rxData);
}

//*****************************************************************************
// 	SetAcmsIntCardFormatConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//*****************************************************************************
BOOL SetAcmsIntCardFormatConfigFields(CHARPTR * rxData)
{
	return SetAcmsCardFormatConfigFields(INTERNAL_PORT, rxData);
}

//*****************************************************************************
// 	SetAcmsCardFormatConfigFields()
//	Param:
//		IN: rdrPort -> internal or external reader
//			rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//*****************************************************************************
static BOOL SetAcmsCardFormatConfigFields(RESPONSE_PORT_e rdrPort, CHARPTR * rxData)
{
	CHAR						tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8						parseStatus, tmpIdx;
	UINT16						configIdx,cardFormatId=0;
	CARD_FORMAT_CONFIG_t		tempCardFormatConf;
	UINT32						fieldValue;
	ACMS_CONFIG_FIELD_ID		fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		CONFIGPRINT(CONFIG_FAIL, "CardFormatConfig Reader Port: [%d]", rdrPort);
		return FAIL;
	}

	tempCardFormatConf = BlankCardFormatConf;

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE)) != FAIL)
	{
		if(strlen(tempConfigBuff) > 0)
		{
			// General statement for code optimization
			fieldValue = MxAsciiToUINT32(tempConfigBuff);

			//Set configuration data in temporary configuration structure
			switch(fieldId)
			{
			case CARD_FORMAT_CONFIG_MAX_BIT:
				if(fieldValue <= MAX_CARD_ID_BITS)
				{
					tempCardFormatConf.maxCardBit[rdrPort] = (UINT8)fieldValue;
				}
				else
				{
					CONFIGPRINT(CONFIG_FAIL, "CardFormatConfig Reader Port: [%d]", rdrPort);
					return FAIL;
				}
				break;

			case CARD_FORMAT_CONFIG_CARD_STRUCT:
				if(strlen(tempConfigBuff) <= MAX_CARD_ID_BITS)
				{
					tempCardFormatConf.cardStructBit[rdrPort] = strlen(tempConfigBuff);
					memset(tempCardFormatConf.cardStruct[rdrPort], 0, sizeof(tempCardFormatConf.cardStruct[rdrPort]) );
					for(tmpIdx = 0; tmpIdx < tempCardFormatConf.cardStructBit[rdrPort]; tmpIdx++)
					{
						tempCardFormatConf.cardStruct[rdrPort][tmpIdx] = tempConfigBuff[tmpIdx] - 0x30;
					}
				}
				else
				{
					CONFIGPRINT(CONFIG_FAIL, "CardFormatConfig Reader Port: [%d]", rdrPort);
					return FAIL;
				}
				break;

			case CARD_FORMAT_CONFIG_REVERSE_ID:
				tempCardFormatConf.cardIdOrder[rdrPort] = (CARD_ID_ORDER_e)fieldValue;
				break;

			case CARD_FORMAT_CONFIG_FC_IN_ID:
				tempCardFormatConf.FCIncardIdF[rdrPort] = (BOOL)fieldValue;
				break;

			case CARD_FORMAT_CONFIG_SEQUENCE_OF_OPERATION:
				tempCardFormatConf.operationSeq[rdrPort] = (OPERATION_SEQUENCE_e)fieldValue;
				break;

			case CARD_FORMAT_ID:
				cardFormatId = (UINT16)fieldValue - ACMS_INDEX_OFFSET;
				break;

			default:
				{
					if(fieldId >= MAX_CARD_FORMAT_CONFIG_FIELD)
					{
						EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
						CONFIGPRINT(CONFIG_FAIL, "CardFormatConfig Reader Port: [%d]", rdrPort);
					}
				}break;
			}
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			UINT8 otherRdrPort;
			if(rdrPort == INTERNAL_PORT) // rdrPort == 0
			{
				otherRdrPort = EXTERNAL_PORT;
			}
			else // rdrPort == 1
			{
				otherRdrPort = INTERNAL_PORT;
			}

			tempCardFormatConf.FCIncardIdF[otherRdrPort] = CardFormatConf[cardFormatId].FCIncardIdF[otherRdrPort];
			tempCardFormatConf.cardIdOrder[otherRdrPort] = CardFormatConf[cardFormatId].cardIdOrder[otherRdrPort];
			tempCardFormatConf.cardStructBit[otherRdrPort] = CardFormatConf[cardFormatId].cardStructBit[otherRdrPort];
			tempCardFormatConf.maxCardBit[otherRdrPort] = CardFormatConf[cardFormatId].maxCardBit[otherRdrPort];
			tempCardFormatConf.operationSeq[otherRdrPort] = CardFormatConf[cardFormatId].operationSeq[otherRdrPort];
			memcpy(tempCardFormatConf.cardStruct[otherRdrPort], CardFormatConf[cardFormatId].cardStruct[otherRdrPort], MAX_CARD_ID_BITS);

			if(ConfigUpdateHandler(CARD_FORMAT_CONFIG_TABLE_UPDATE_ID, cardFormatId, (VOIDPTR)(&tempCardFormatConf)) == SUCCESS)
			{
				DPRINT(ACMS_LOG,"Card Format[%d] config value set Success", rdrPort);
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG,"Card Format[%d] config value set Fail", rdrPort);
	CONFIGPRINT(CONFIG_FAIL, "CardFormatConfig Reader Port: [%d]", rdrPort);
	return FAIL;
}

//*****************************************************************************
//	SetAcmsVamConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsVamConfigFields(CHARPTR * rxData)
{
	CHAR						tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8						parseStatus;
	UINT16						configIdx = INVALID_VAM_CONFIG_INDEX;
	VAM_CONFIG_t				tempVamConfig;
	ACMS_CONFIG_FIELD_ID		fieldId;
	VAM_CONFIG_CARD_STATUS_e	vamCardStatus = VAM_CARD_NOT_SURRENDER;
	UINT32						tmpPassId, fieldValue;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		CONFIGPRINT(CONFIG_FAIL, "VamConfig");
		return FAIL;
	}

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);
		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
		case VAM_CFG_FIELD_PASS_ID:
			tmpPassId = (UINT32)atoi(tempConfigBuff);
			if(GetVamConfigIndex(tmpPassId, &configIdx) == SUCCESS)
			{
				tempVamConfig = VamConfig[configIdx];
				tempVamConfig.passId = tmpPassId;
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "VamConfig");
				return FAIL;
			}
			break;

		case VAM_CFG_FIELD_DRIVER_CARD:
			if(strlen(tempConfigBuff) > 0)
			{
				//sscanf for unsigned long long data because atoll function
				//return only signed data
				sscanf(tempConfigBuff, "%llu", &tempVamConfig.vamCardId[VAM_DRIVER_CARD]);
			}
			else
			{
				tempVamConfig.vamCardId[VAM_DRIVER_CARD] = INVALID_CARD_ID;
			}
			break;

		case VAM_CFG_FIELD_HELPER_CARD:
			if(strlen(tempConfigBuff) > 0)
			{
				//sscanf for unsigned long long data because atoll function
				//return only signed data
				sscanf(tempConfigBuff, "%llu", &tempVamConfig.vamCardId[VAM_HELPER_CARD]);
			}
			else
			{
				tempVamConfig.vamCardId[VAM_HELPER_CARD] = INVALID_CARD_ID;
			}
			break;

		case VAM_CFG_FIELD_VEHICLE_CARD:
			if(strlen(tempConfigBuff) > 0)
			{
				//sscanf for unsigned long long data because atoll function
				//return only signed data
				sscanf(tempConfigBuff, "%llu", &tempVamConfig.vamCardId[VAM_VEHICLE_CARD]);
			}
			else
			{
				tempVamConfig.vamCardId[VAM_VEHICLE_CARD] = INVALID_CARD_ID;
			}
			break;

		case VAM_CFG_FIELD_START_DATE:
			if(GetDateFromString(tempConfigBuff, &tempVamConfig.startDate, YEAR_OFFSET_2000) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "VamConfig");
				return FAIL;
			}
			break;

		case VAM_CFG_FIELD_START_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempVamConfig.startTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "VamConfig");
				return FAIL;
			}
			break;

		case VAM_CFG_FIELD_END_DATE:
			if(GetDateFromString(tempConfigBuff, &tempVamConfig.endDate, YEAR_OFFSET_2000) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "VamConfig");
				return FAIL;
			}
			break;

		case VAM_CFG_FIELD_END_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempVamConfig.endTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "VamConfig");
				return FAIL;
			}
			break;

		case VAM_CFG_FIELD_SMART_ROUTE:
			if((UINT8)fieldValue != 0)
			{
				tempVamConfig.smartRouteId = (UINT8)(fieldValue - ACMS_INDEX_OFFSET);
			}
			else
			{
				tempVamConfig.smartRouteId = USER_SMART_ROUTE_NONE;
			}
			break;

		case VAM_CFG_FIELD_ACCESS_LEVEL:
			tempVamConfig.accessLevel = (UINT8)fieldValue;
			break;

		//this value will not be stored in configurations
		case VAM_CFG_FIELD_STATUS:
			vamCardStatus = (UINT8)fieldValue;
			break;

		default:
			{
				if(fieldId >= MAX_VAM_CONFIG_FIELD)
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in VAM_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "VamConfig");
				}
			}break;
		}

		//Check for data completion
		if( (parseStatus == LAST_DATA) && (configIdx < MAX_VAM_CONFIG) )
		{
			if(vamCardStatus == VAM_CARD_SURRENDER)
			{
				tempVamConfig = DfltVamConfig;
			}

			if(ConfigUpdateHandler(VAM_CONFIG_TABLE_ID, configIdx, (VOIDPTR)(&tempVamConfig)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "VamConfig value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "VamConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsJcmConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsJcmConfigFields(CHARPTR * rxData)
{
	CHAR						tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8						parseStatus;
	UINT16						configIdx = INVALID_JCM_CONFIG_INDEX;
	JCM_CONFIG_t				tempJcmConfig;
	ACMS_CONFIG_FIELD_ID		fieldId;
	UINT32						fieldValue;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		CONFIGPRINT(CONFIG_FAIL, "JcmConfig");
		return FAIL;
	}

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);
		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
		case JCM_CFG_REF_ID:
			if(fieldValue != INVALID_JOB_REF_ID)
			{
				tempJcmConfig.refId = fieldValue;
				if(GetJcmConfigIdxFromRefId(tempJcmConfig.refId, &configIdx) != SUCCESS)
				{
					EPRINT(ACMS_LOG,"JCM ref Id [%d] not found and config is full", tempJcmConfig.refId);
					CONFIGPRINT(CONFIG_FAIL, "JcmConfig");
					return FAIL;
				}
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "JcmConfig");
				return FAIL;
			}
			break;

		case JCM_CFG_JOB_CODE:
			if(strlen(tempConfigBuff) < JOB_CODE_SIZE)
			{
				strcpy(tempJcmConfig.jobCode, tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "JcmConfig");
				return FAIL;
			}

			break;

		case JCM_CFG_JOB_NAME:
			if(UTF8Strlen(tempConfigBuff) < JOB_NAME_SIZE)
			{
				strcpy(tempJcmConfig.jobName, tempConfigBuff);
			}
			else
			{
				CONFIGPRINT(CONFIG_FAIL, "JcmConfig");
				return FAIL;
			}
			break;

		case JCM_CFG_START_DATE:
			if(GetDateFromString(tempConfigBuff, &tempJcmConfig.startDate, YEAR_OFFSET_2000) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "JcmConfig");
				return FAIL;
			}
			break;

		case JCM_CFG_END_DATE:
			if(GetDateFromString(tempConfigBuff, &tempJcmConfig.endDate, YEAR_OFFSET_2000) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "JcmConfig");
				return FAIL;
			}
			break;

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in JCM_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "JcmConfig");
			}
		}

		if( (parseStatus == LAST_DATA) && (configIdx < MAX_JCM_CONFIG))
		{
			//to delete job code ACMS will send job code name blank.
			if(strlen(tempJcmConfig.jobName) == 0)
			{
				tempJcmConfig = DfltJcmConfig;
			}
			if(ConfigUpdateHandler(JCM_CONFIG_TABLE_ID, configIdx, (VOIDPTR)&tempJcmConfig) == SUCCESS)
			{
				DPRINT(ACMS_LOG,"JCM config set successfully configIdx [%d]", configIdx);
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "JCM Config value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "JcmConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsMultiLanConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsMultiLanConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 					configIdx, parseStatus;
	MULTI_LANG_SUPPORT_t	tempMultiLangConfig;
	UINT32					fieldValue;
	ACMS_CONFIG_FIELD_ID	fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		CONFIGPRINT(CONFIG_FAIL, "MultiLanConfig");
		return FAIL;
	}

	// Get current configuration
	tempMultiLangConfig = MultiLangConfig;

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);
		switch(fieldId)
		{
		case MULTI_LANG_CFG_ENABLE_FLAGE:
			tempMultiLangConfig.multiLangEnblF = (BOOL)fieldValue;
			break;

		case MULTI_LANG_CFG_ENABLE_DATA_INPUT_FLAGE:
			tempMultiLangConfig.multiLangDataInputEnblF = (BOOL)fieldValue;
			break;

		default:
			{
				if(fieldId >= MAX_MULTI_LANG_CONFIG_FIELD)
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in MULTI_LANGUAGE_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "MultiLanConfig");
				}
			}break;
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(MULTI_LANGUAGE_CONFIG_TABLE_ID, configIdx, (VOIDPTR)(&tempMultiLangConfig)) == SUCCESS)
			{
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "Gen MultiLanguage value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "MultiLanConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsDoorModeSchConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsDoorModeSchConfigFields(CHARPTR * rxData)
{
	CHAR						tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8						parseStatus;
	UINT16						configIdx;
	DOOR_MODE_SCH_CONFIG_t		tempDoorModeSchConfig;
	ACMS_CONFIG_FIELD_ID		fieldId;
	UINT32						fieldValue;

	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_DOOR_MODE_SCHEDULE)
	{
		CONFIGPRINT(CONFIG_FAIL, "DoorModeSchConfig");
		return FAIL;
	}

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);
		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
		case DOOR_MODE_SCH_CFG_FIELD_STRT_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempDoorModeSchConfig.startTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "DoorModeSchConfig");
				return FAIL;
			}
			break;

		case DOOR_MODE_SCH_CFG_FIELD_END_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempDoorModeSchConfig.endTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "DoorModeSchConfig");
				return FAIL;
			}
			break;


		case DOOR_MODE_SCH_CFG_FIELD_ENBL_SUN:
		case DOOR_MODE_SCH_CFG_FIELD_ENBL_MON:
		case DOOR_MODE_SCH_CFG_FIELD_ENBL_TUE:
		case DOOR_MODE_SCH_CFG_FIELD_ENBL_WED:
		case DOOR_MODE_SCH_CFG_FIELD_ENBL_THU:
		case DOOR_MODE_SCH_CFG_FIELD_ENBL_FRI:
		case DOOR_MODE_SCH_CFG_FIELD_ENBL_SAT:
			tempDoorModeSchConfig.days[(fieldId - DOOR_MODE_SCH_CFG_FIELD_ENBL_SUN)] = (BOOL)fieldValue;
			break;

		case DOOR_MODE_SCH_CFG_FIELD_DOOR_READER_MODE:
			tempDoorModeSchConfig.doorRdrMode = (READER_MODE_e)atoi(tempConfigBuff);
			break;

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in DOOR_MODE_SCH_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "DoorModeSchConfig");
			}

		}

		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(DOOR_MODE_SCH_CONFIG_TABLE_ID, configIdx, (VOIDPTR)&tempDoorModeSchConfig) == SUCCESS)
			{
				DPRINT(ACMS_LOG,"RMS config set successfully configIdx [%d]",configIdx);
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "RMS Config value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "DoorModeSchConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsBleConfig()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsBleConfig(UNUSED_PARA CHARPTR * rxData)
{
	CHAR						tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8						parseStatus;
	UINT16						configIdx;
	BLUETOOTH_CONFIG_t			tempBluetoothPara;
	ACMS_CONFIG_FIELD_ID		fieldId;
	UINT32						fieldValue;

	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		CONFIGPRINT(CONFIG_FAIL, "BleConfig");
		return FAIL;
	}

	//get copy of current configuration
	tempBluetoothPara = BluetoothPara;
	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);
		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{

			case BLE_CONFIG_ADVERTISE_BT:
				{
					tempBluetoothPara.btEnable = (BOOL)fieldValue;
				}
				break;

			case BLE_CONFIG_BT_NAME:
				if(UTF8Strlen(tempConfigBuff) < BT_NAME_SIZE)
				{
					sprintf(tempBluetoothPara.btName , "%s", tempConfigBuff);
				}
				else
				{
					CONFIGPRINT(CONFIG_FAIL, "BleConfig");
					return FAIL;
				}
				break;

			case BLE_CONFIG_RANGE:
				{
					tempBluetoothPara.btRange = (UINT8)fieldValue;
				}
				break;

			case BLE_CONFIG_CUSTOM_RANGE_STRENGTH:
				{
					INT16 tempBtCustomeRangeStrength = (INT16)atoi(tempConfigBuff);
					if((tempBtCustomeRangeStrength >= RANGE_MIN_DBM) && (tempBtCustomeRangeStrength <= RANGE_MAX_DBM))
					{
						tempBluetoothPara.btCustomeRangeStrength = tempBtCustomeRangeStrength;
					}
				}break;

			default:
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in BLE_CONFIG_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "BleConfig");
				}

		}

		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(BLE_CONFIG_ID, configIdx, (VOIDPTR)&tempBluetoothPara) == SUCCESS)
			{
				DPRINT(ACMS_LOG,"BLE set successfully configIdx [%d]",configIdx);
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "BLE Config value set Fail");
	CONFIGPRINT(CONFIG_FAIL, "BleConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsIpCameraConfig()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsIpCameraConfig(CHARPTR * rxData)
{
	CHAR 						tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 						parseStatus;
	UINT16 						configIdx;
	IP_CAMERA_CONFIG_t			tempIpCameraConfig;
	UINT32						fieldValue;
	ACMS_CONFIG_FIELD_ID		fieldId;

	tempIpCameraConfig = IpCameraConfig;
	//check upto FAIL or LAST_DATA

	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_CAPTURE_SNAPSHOT_SCH)
	{
		CONFIGPRINT(CONFIG_FAIL, "IpCameraConfig");
		return FAIL;
	}

	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);
		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
		case IP_CAMERA_CONFIG_SCHEDULE_NAME:
			if(UTF8Strlen(tempConfigBuff) < 40)
			{
				snprintf(tempIpCameraConfig.captureSnapshotSchedule[configIdx].scheduleName, MAX_IP_CAMERA_SCH_NAME, "%s", tempConfigBuff);
			}
			break;

		case IP_CAMERA_CONFIG_SCHEDULE_ACTIVE:
			tempIpCameraConfig.captureSnapshotSchedule[configIdx].eventActive = (BOOL)fieldValue;
			break;

		case IP_CAMERA_CONFIG_EVENT:
			tempIpCameraConfig.captureSnapshotSchedule[configIdx].captureEvent = (CAPTURE_IMG_EVENT_e)fieldValue;
			break;

		case IP_CAMERA_CONFIG_EVENT_MODE:
			tempIpCameraConfig.captureSnapshotSchedule[configIdx].captureEventMode = (CAPTURE_IMG_EVENT_MODE_e)fieldValue;
			break;

		case IP_CAMERA_CONFIG_START_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempIpCameraConfig.captureSnapshotSchedule[configIdx].startTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "IpCameraConfig");
				return FAIL;
			}
			break;

		case IP_CAMERA_CONFIG_END_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempIpCameraConfig.captureSnapshotSchedule[configIdx].endTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "IpCameraConfig");
				return FAIL;
			}
			break;

		case IP_CAMERA_CONFIG_ACTIVE_DAY0:
		case IP_CAMERA_CONFIG_ACTIVE_DAY1:
		case IP_CAMERA_CONFIG_ACTIVE_DAY2:
		case IP_CAMERA_CONFIG_ACTIVE_DAY3:
		case IP_CAMERA_CONFIG_ACTIVE_DAY4:
		case IP_CAMERA_CONFIG_ACTIVE_DAY5:
		case IP_CAMERA_CONFIG_ACTIVE_DAY6:
		case IP_CAMERA_CONFIG_ACTIVE_HOLIDAY:
			tempIpCameraConfig.captureSnapshotSchedule[configIdx].activeDay[fieldId - IP_CAMERA_CONFIG_ACTIVE_DAY0] = (BOOL)atoi(tempConfigBuff);
			break;

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in IP_CAMERA_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "IpCameraConfig");
			}
		}

		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(IP_CAMERA_CONFIG_TABLE_ID, configIdx, (VOIDPTR)&tempIpCameraConfig) == SUCCESS)
			{
				DPRINT(ACMS_LOG,"Ip Camera Config set successfully configIdx [%d]",configIdx);
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "ip camera config set fail");
	CONFIGPRINT(CONFIG_FAIL, "IpCameraConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsIpCameraConfig()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsIpCameraAdvanceConfig(CHARPTR * rxData)
{
	CHAR 						tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 						parseStatus;
	UINT16 						configIdx;
	IP_CAMERA_CONFIG_t			tempIpCameraConfig;
	ACMS_CONFIG_FIELD_ID		fieldId;
	UINT8						length;

	tempIpCameraConfig = IpCameraConfig;
	//check upto FAIL or LAST_DATA

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		CONFIGPRINT(CONFIG_FAIL, "IpCameraAdvanceConfig");
		return FAIL;
	}

	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		switch(fieldId)
		{
		case IP_CAMERA_ADVANCE_CONFIG_URL:
				length = sizeof(tempIpCameraConfig.captureSnapshotUrl);
				Base64Decode(tempConfigBuff, tempIpCameraConfig.captureSnapshotUrl, &length);
			break;

		case IP_CAMERA_ADVANCE_CONFIG_USERNAME:
				length = sizeof(tempIpCameraConfig.captureSnapshotUserName);
				Base64Decode(tempConfigBuff, tempIpCameraConfig.captureSnapshotUserName, &length);
			break;

		case IP_CAMERA_ADVANCE_CONFIG_PASSWORD:
				length = sizeof(tempIpCameraConfig.captureSnapshotPassword);
				Base64Decode(tempConfigBuff, tempIpCameraConfig.captureSnapshotPassword, &length);
			break;

		default:
			{
				EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in IP_CAMERA_ADVANCE_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
				CONFIGPRINT(CONFIG_FAIL, "IpCameraAdvanceConfig");
			}
		}

		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(IP_CAMERA_ADVANCE_CONFIG_TABLE_ID, configIdx, (VOIDPTR)&tempIpCameraConfig) == SUCCESS)
			{
				DPRINT(ACMS_LOG,"Ip Camera Advance Config set successfully configIdx [%d]",configIdx);
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "ip camera advance config set fail");
	CONFIGPRINT(CONFIG_FAIL, "IpCameraAdvanceConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsBackupUpdateConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function save Backup and Update related config(FTP server url, username and password)
//		which received from ACMS.
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//*****************************************************************************
BOOL SetAcmsBackupUpdateConfigFields(CHARPTR * rxData)
{
	CHAR						tempConfigBuff[MAX_TMP_BUFF_SIZE], curPswdStr[MAX_TMP_BUFF_SIZE];
	UINT8						parseStatus,pwdLen;
	UINT16						configIdx = INVALID_VAM_CONFIG_INDEX;
	BACKUP_AND_UPDATE_t			tmpBackUpAndUpdate;
	ACMS_CONFIG_FIELD_ID		fieldId;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		CONFIGPRINT(CONFIG_FAIL, "BackupUpdateConfig");
		return FAIL;
	}
	tmpBackUpAndUpdate = BackUpAndUpdate;

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
		case BACKUP_UPDATE_FTP_URL:
			snprintf(tmpBackUpAndUpdate.ftpServerUrl, MAX_FTP_SERVER_URL, "%s", tempConfigBuff);
			break;

		case BACKUP_UPDATE_FTP_USERNAME:
			snprintf(tmpBackUpAndUpdate.ftpServerUseName, MAX_FTP_USER_NAME, "%s", tempConfigBuff);
			break;

		case BACKUP_UPDATE_FTP_PASSWORD:
			pwdLen = (UINT8)sizeof(curPswdStr);
			Base64Decode(tempConfigBuff, curPswdStr, &pwdLen);
			snprintf(tmpBackUpAndUpdate.ftpServerUserPassword, MAX_FTP_USER_PASSWORD,"%s", curPswdStr);
			break;

		default:
			{
				if(fieldId >= MAX_BACKUP_UPDATE_CONFIG_FIELD)
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in BACKUP_AND_UPDATE_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "BackupUpdateConfig");
				}
			}break;
		}

		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(BACKUP_AND_UPDATE_CONFIG_TABLE_ID, configIdx, (VOIDPTR)&tmpBackUpAndUpdate) == SUCCESS)
			{
				DPRINT(ACMS_LOG,"Backup and Update config set successfully configIdx [%d]",configIdx);
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "Backup and Update config set Fail");
	CONFIGPRINT(CONFIG_FAIL, "BackupUpdateConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsFaceRecogntionConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function save Face Recognition related config(FR server url, Mode..etc)
//		which received from ACMS.
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//*****************************************************************************
BOOL SetAcmsFaceRecogntionConfigFields(CHARPTR * rxData)
{
	CHAR						tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8						parseStatus;
	UINT16						configIdx = INVALID_VAM_CONFIG_INDEX;
	FR_SERVER_CONFIG_t			tmpFRServerConfig;
	ACMS_CONFIG_FIELD_ID		fieldId;
	UINT32						fieldValue;
	FLOAT32 tempFieldVal = 0.0;

	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		CONFIGPRINT(CONFIG_FAIL, "FaceRecogntionConfig");
		return FAIL;
	}

	tmpFRServerConfig = FRServerConfig;

	//check upto FAIL or LAST_DATA
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		// General statement for code optimization
		fieldValue = MxAsciiToUINT32(tempConfigBuff);

		//Set configuration data in temporary configuration structure
		switch(fieldId)
		{
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_ENABLE_FR_SEREVR:
				tmpFRServerConfig.enableFrServer = (BOOL)fieldValue;
				break;

			case FACE_RECOGNITION_FR_SEREVR_CONFIG_FACE_CAPTURING_MODE:
				if(SysBasicPara.application == CAFETERIA)
				{
					tmpFRServerConfig.faceCapturing = FACE_SERVER_FREE_SCAN;
				}
				else
				{
					tmpFRServerConfig.faceCapturing = (FACE_CAPTURING_e)fieldValue;
				}
				break;

			case FACE_RECOGNITION_FR_SEREVR_CONFIG_FREE_SCAN_TIME_OUT:
				tmpFRServerConfig.faceScanTimeOut = (UINT16)fieldValue;
				break;

			case FACE_RECOGNITION_FR_SEREVR_CONFIG_FR_SERVER_HOSTNAME:
				snprintf(tmpFRServerConfig.hostName, MAX_SERVER_ADDR_254, "%s", tempConfigBuff);
				break;

			case FACE_RECOGNITION_FR_SEREVR_CONFIG_TCP_LISTENING_PORT:
				tmpFRServerConfig.port = (UINT16)fieldValue;
				break;

			case FACE_RECOGNITION_FR_SEREVR_CONFIG_IDENTIFICATION_TIMEOUT:
				tmpFRServerConfig.identificationTimeOut = (UINT16)fieldValue;
				break;

			case FACE_RECOGNITION_FR_SEREVR_CONFIG_FR_SERVER_MODE:
				tmpFRServerConfig.frMode = (FR_MODE_e)fieldValue;
				break;
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_EN_FACE_SCAN_TIMEOUT_FLG:
				if(SysBasicPara.application == CAFETERIA)
				{
					tmpFRServerConfig.enableFreeScanTimeOutF = DISABLED;
				}
				else
				{
					tmpFRServerConfig.enableFreeScanTimeOutF = (BOOL)fieldValue;
				}
				break;
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_GROUP_FR_SUPPORT_FLG:
				tmpFRServerConfig.groupFrSupportF = (BOOL)fieldValue;
				break;
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_ADAPTIVE_FACE_ENROLL_FLG:
				tmpFRServerConfig.adaptiveFaceEnrollCnfg.isAdaptiveFaceEnrollEnbled = (BOOL)fieldValue;
				break;
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_THRESHOLD_DEVIATION:
				tempFieldVal = (FLOAT32)atof(tempConfigBuff);
				if( ((FLOAT32)tempFieldVal < (FLOAT32)MIN_THRESHOLD_DEVIATION) || ((FLOAT32)tempFieldVal > (FLOAT32)MAX_THRESHOLD_DEVIATION) )
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed Invalid Value of Threshold Deviation:fieldId = [%d]",fieldId);
					CONFIGPRINT(CONFIG_FAIL, "FaceRecogntionConfig");
					return FAIL;
				}
				tmpFRServerConfig.adaptiveFaceEnrollCnfg.thresholdDeviation  = (FLOAT32)tempFieldVal;
				break;
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_MULTI_USR_MATCH_SCR_DEVIATION:
				tempFieldVal = (FLOAT32)atof(tempConfigBuff);
				if( ((FLOAT32)tempFieldVal < (FLOAT32)MIN_MULTI_USR_MTCH_SCR ) || ((FLOAT32)tempFieldVal >(FLOAT32)MAX_MULTI_USR_MTCH_SCR) )
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed Invalid Value of Multi User Match Scr Devication:fieldId = [%d]",fieldId);
					CONFIGPRINT(CONFIG_FAIL, "FaceRecogntionConfig");
					return FAIL;
				}
				tmpFRServerConfig.adaptiveFaceEnrollCnfg.multiUserMatchScoreDeviation  = (FLOAT32)tempFieldVal;
				break;
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_CNFRM_BEFORE_ADPTV_FACE_ENROLL:
				tmpFRServerConfig.adaptiveFaceEnrollCnfg.cnfrmBfrAdaptiveEnrollFlg = (BOOL)fieldValue;
				break;
				
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_CONFLICT_FLG:
				tmpFRServerConfig.conflictChkF = (BOOL)fieldValue;
				break;
				
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_CONFLICT_MATCHING_THRESHOLD:
				tempFieldVal = (FLOAT32)atof(tempConfigBuff);
				if( ((FLOAT32)tempFieldVal < (FLOAT32)CONFLICT_MATCHING_THRSHLD_MIN) || ((FLOAT32)tempFieldVal > (FLOAT32)CONFLICT_MATCHING_THRSHLD_MAX) )
				{
					EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed Invalid Value of Conflict Matching Threshold :fieldId = [%d]",fieldId);
					CONFIGPRINT(CONFIG_FAIL, "FaceRecogntionConfig");
					return FAIL;
				}
				tmpFRServerConfig.conflictMatchThrshld = (FLOAT32)tempFieldVal;
				break;

			// Face Anti-Spoofing
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_FACE_ANTI_SPOOFING:
				tmpFRServerConfig.frServerFaceAntiSpoofingF = (BOOL)fieldValue;
				break;

			// Face Anti-Spoofing Mode
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_FACE_ANTI_SPOOFING_MODE:
				if(IsGivenDigitInValidRange(fieldValue,FACE_ANTI_SPOOFING_MODE_BASIC,MAX_FACE_ANTI_SPOOFING_MODE) == NO)
				{
					EPRINT(ACMS_RXTX_LOG, "Invalid Face Anti-Spoofing Mode Receive = [%d]",fieldValue);
					CONFIGPRINT(CONFIG_FAIL, "FRServerConfig");
					return FAIL;
				}
				tmpFRServerConfig.frServerFaceAntiSpoofingMode = (FR_FACE_ANTI_SPOOFING_MODE_e)fieldValue;
				break;

			// Face Anti-Spoofing Threshold
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_FACE_ANTI_SPOOFING_THRESHOLD:
				tempFieldVal = (FLOAT32)atof(tempConfigBuff);
				if(IsGivenFloatInValidRange(tempFieldVal,FR_FACE_ANTI_SPOOFING_THRESHOLD_MIN,FR_FACE_ANTI_SPOOFING_THRESHOLD_MAX) == NO)
				{
					EPRINT(ACMS_RXTX_LOG, "Invalid Face Anti-Spoofing Threshold Receive = [%f]",tempFieldVal);
					CONFIGPRINT(CONFIG_FAIL, "FRServerConfig");
					return FAIL;
				}
				tmpFRServerConfig.frServerFaceAntiSpoofingThreshold = (FLOAT32)tempFieldVal;
				break;

            		case FACE_RECOGNITION_FR_SEREVR_CONFIG_MAX_ADEPTIVE_FACE:
		            if(IsGivenDigitInValidRange(fieldValue,MIN_ADEPTIVE_FACE_NUMBER,MAX_ADEPTIVE_FACE_NUMBER) == NO)
			    {
		                EPRINT(ACMS_RXTX_LOG, "Invalid Max Adeptive face Receive= [%d]",fieldValue);
		                CONFIGPRINT(CONFIG_FAIL, "FRServerConfig");
		                return FAIL;
                	    }
		            tmpFRServerConfig.AdeptiveMaxFaceCnt = fieldValue;
		            break;

			case FACE_RECOGNITION_FR_SEREVR_CONFIG_EXCEPTIONAL_FACE_EN:
			{
#ifdef FR_ADDITIONAL_FEATURE_SUPPORTED //FR_EXCEPTIONAL_FACE_FEATURE_ENABLE
				tmpFRServerConfig.frServerExceptionalFaceF = (BOOL)fieldValue;
				DPRINT(SYS_LOG,"@PC Exceptional Face Flg Config recvd from Server:[%d]",tmpFRServerConfig.frServerExceptionalFaceF);
#else
				tmpFRServerConfig.frServerExceptionalFaceF = FALSE;
#endif
			}break;

		    case FACE_RECOGNITION_FR_SEREVR_CONFIG_SHOW_UNKNOWN_USER_ACK:
		    {
		        tmpFRServerConfig.showUnknownUserAck = (BOOL)fieldValue;
		    }break;
			
			case FACE_RECOGNITION_FR_SEREVR_CONFIG_GENERATE_UNIDENTIFIED_FACE_EVENT:
			{
				tmpFRServerConfig.generateUnidentifiedFaceEvent = (BOOL)fieldValue;
			}break;

			default:
				{
					if(fieldId >= MAX_FACE_RECOGNITION_FR_SEREVR_CONFIG)
					{
						EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in FACE_RECO_FR_SERVER_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
						CONFIGPRINT(CONFIG_FAIL, "FaceRecogntionConfig");
					}
				}
				break;
		}
		//Check for data completion
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(FACE_RECO_FR_SERVER_CONFIG_TABLE_ID, configIdx, (VOIDPTR)&tmpFRServerConfig) == SUCCESS)
			{
				DPRINT(ACMS_LOG,"Face Recognition config set successfully configIdx [%d]",configIdx);
				return SUCCESS;
			}
		}
	}

	EPRINT(ACMS_LOG, "Face Recognition config set Fail");
	CONFIGPRINT(CONFIG_FAIL, "FaceRecogntionConfig");
	return FAIL;
}

//*****************************************************************************
//	SetAcmsExternalBleConfig()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsReaderBleConfig(CHARPTR * rxData)
{
	CHAR 							tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT8 							parseStatus;
	UINT16 							configIdx;
	UINT32							fieldValue;
	ACMS_CONFIG_FIELD_ID			fieldId;
	READER_BLUETOOTH_CONFIG_t		tempReaderBleConfig;
	BOOL							ChangeConfigFromServer = FALSE;
	BOOL							invalidValueF = FALSE;

	/** Validate Input Parameters */
	if(NULL == rxData)
	{
		EPRINT(ACMS_LOG,"rxData pointer is null");
		return FAIL;
	}

	/** Parse and get configuration Index */
	configIdx = AcmsParseConfigIdx(rxData);

	/** Validate configuration index */
	if(configIdx >= CONFIG_TABLE_SINGLE_ROW_PER_PAGE)
	{
		EPRINT(ACMS_LOG,"Failed to parse config index[%d]",configIdx);
		return FAIL;
	}

	/** Get copy of current configuration */
	memcpy((VOIDPTR)&tempReaderBleConfig,(VOIDPTR)&ReaderBluetoothPara,sizeof(tempReaderBleConfig));

	/** check upto FAIL or LAST_DATA */
	while( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId, MAX_TMP_BUFF_SIZE) ) != FAIL)
	{
		/** General statement for code optimization */
		fieldValue = MxAsciiToUINT32(tempConfigBuff);
		DPRINT(SYS_LOG,"fieldId[%u]=[%s]fieldValue",fieldId,tempConfigBuff);
		/** Set configuration data in temporary configuration structure */
		switch(fieldId)
		{
			case RDR_BLE_CFG_CONFIGURE_BLE_FROM_SERVER:
			{
				if((FALSE == (BOOL)fieldValue) || (TRUE == (BOOL)fieldValue))
				{
					ChangeConfigFromServer = (BOOL)fieldValue;
				}
				else
				{
					EPRINT(ACMS_LOG,"Invalid value received for field[%d]=value[%d]",fieldId,fieldValue);
					invalidValueF = TRUE;
				}
			}break;

			case RDR_BLE_CFG_BLE_NAME:
			{
				if(strlen(tempConfigBuff) < CB_BLE_NAME_SIZE)
				{
					snprintf(tempReaderBleConfig.btNameReader,sizeof(tempReaderBleConfig.btNameReader),"%s",tempConfigBuff);
				}
				else
				{
					EPRINT(ACMS_LOG,"Invalid value received for field[%d]=value[%s]",fieldId,tempConfigBuff);
					invalidValueF = TRUE;
				}
			}break;

			case RDR_BLE_CFG_ADVERTISE_BLE:
			{
				if((FALSE == (BOOL)fieldValue) || (TRUE == (BOOL)fieldValue))
				{
					tempReaderBleConfig.btEnableReader = (BOOL)fieldValue;
				}
				else
				{
					EPRINT(ACMS_LOG,"Invalid value received for field[%d]=value[%d]",fieldId,fieldValue);
					invalidValueF = TRUE;
				}
			}break;

			case RDR_BLE_CFG_BLE_RANGE:
			{
				if((UINT8)fieldValue < MAX_BLUETOOTH_RANGE)
				{
					if((UINT8)fieldValue < BLUETOOTH_RANGE_CUSTOM)
					{
					  tempReaderBleConfig.btRangeReader = (UINT8)fieldValue;
					}
				}
				else
				{
					EPRINT(ACMS_LOG,"Invalid value received for field[%d]=value[%d]",fieldId,fieldValue);
					invalidValueF = TRUE;
				}
			}break;

			case RDR_BLE_CFG_BLE_CUSTOM_RANGE_STRENGTH:
			{
			  
			}break;
			
			default:
				{
					EPRINT(ACMS_LOG, "Unknown Config field received for Reader Ble config in READER_BLE_CONFIG_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
					CONFIGPRINT(CONFIG_FAIL, "ReaderBleconfig");
				}break;
		}
		/** Check for data completion */
		if(parseStatus == LAST_DATA)
		{
			if(TRUE == invalidValueF)
			{
				return FAIL;
			}

			/** Update Configurations */
			if(TRUE == ChangeConfigFromServer)
			{
				if(SUCCESS == ConfigUpdateHandler(READER_BLE_CONFIG_TABLE_ID, configIdx, (VOIDPTR)(&tempReaderBleConfig)))
				{
					DPRINT(ACMS_LOG,"Reader BLE configuration set successfully");
					return SUCCESS;
				}
			}
			else
			{
				EPRINT(ACMS_LOG,"Reader Ble Config ignored");
				return SUCCESS;
			}
		}
	}
	EPRINT(ACMS_LOG, "Failed to parse Reader Ble Config value");
	CONFIGPRINT(CONFIG_FAIL, "ReaderBleconfig");
	return FAIL;
}


//*****************************************************************************
//	ParseHttpUrlData()
//	Param:
//		IN:urlStrPtr - Pointer to receive buffer send by ACMS
//		OUT:ipOrHostName  -- Parse Ip address or host name and fill here
//		OUT:directoryName  -- Parse directoryName and fill here
//		OUT:port  -- Parse port and fill here
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function will parse given fields.
//*****************************************************************************
BOOL ParseHttpUrlData(const CHARPTR const urlStrPtr, CHARPTR const ipOrHostName, CHARPTR const directoryName,UINT16PTR port)
{
	BOOL dnameF = FALSE, portF = FALSE;
	CHARPTR	tokenPtr;

	/** Validate Input Parameters */
	if((NULL == urlStrPtr) || (NULL == ipOrHostName) || (NULL == directoryName) || (NULL == port))
	{
		EPRINT(DC_LOG,"Invalid parameters received : urlStrPtr[%p] ipOrHostName[%p] directoryName[%p] port[%p]",urlStrPtr,ipOrHostName,directoryName,port);
		return FAIL;
	}

	if(0 == strlen(urlStrPtr))
	{
		EPRINT(DC_LOG,"URL String is empty");
		return FAIL;
	}

	/** check Port number present in string */
	if(NULL != strstr(urlStrPtr,":"))
	{
		portF = TRUE;
	}
	/** check directory name present in string */
	if(NULL != strstr(urlStrPtr,"/"))
	{
		dnameF = TRUE;
	}


	if((TRUE == portF) && (FALSE == dnameF))
	{
		/** Parsing IP Address or Host name */
		tokenPtr = strtok(urlStrPtr,":");
		if(NULL != tokenPtr)
		{
			/** Copy IP Address or Host name */
			snprintf(ipOrHostName,HTTP_MAX_URL_STR,"%s",tokenPtr);
		}
		/** Parsing Port Number  */
		tokenPtr = strtok(NULL,"/");
		if(NULL != tokenPtr)
		{
			/** Copy Port Number */
			* port = atoi(tokenPtr);
		}
	}
	else if((FALSE == portF) && (TRUE == dnameF))
	{

		/** Parsing IP Address or Host name */
		tokenPtr = strtok(urlStrPtr,"/");
		if(NULL != tokenPtr)
		{
			/** Copy IP Address or Host name */
			snprintf(ipOrHostName,HTTP_MAX_URL_STR,"%s",tokenPtr);
		}
		/** Parsing directory Name */
		tokenPtr = strtok(NULL,"/");
		if(NULL != tokenPtr)
		{
			/** Copy directory Name */
			snprintf(directoryName,HTTP_MAX_URL_STR,"%s",tokenPtr);
		}
	}
	else if((TRUE == portF) && (TRUE == dnameF))
	{
		/** Parsing IP Address or Host name */
		tokenPtr = strtok(urlStrPtr,":");
		if(NULL != tokenPtr)
		{
			/** Copy IP Address or Host name */
			snprintf(ipOrHostName,HTTP_MAX_URL_STR,"%s",tokenPtr);
		}
		/** Parsing Port Number  */
		tokenPtr = strtok(NULL,"/");
		if(NULL != tokenPtr)
		{
			/** Copy Port Number */
			* port = atoi(tokenPtr);
		}

		/** Parsing directory Name */
		tokenPtr = strtok(NULL,"/");
		if(NULL != tokenPtr)
		{
			/** Copy directory Name */
			snprintf(directoryName,HTTP_MAX_URL_STR,"%s",tokenPtr);
		}
	}
	else	/** Only URL is present at urlStrPtr*/
	{
		/** Copy IP Address or Host name */
		snprintf(ipOrHostName,HTTP_MAX_URL_STR,"%s",urlStrPtr);
	}
	return SUCCESS;
}

//*****************************************************************************
//	SetAcmsAccessScheduleConfigFields()
//	Param:
//		IN:rxData-Pointer to receive buffer send by ACMS
//		OUT:NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//	    This function validate data send by ACMS.If valid data found then
//		it fill configuration data in configuration structure and file else
//		return FAIL.
//
//*****************************************************************************
BOOL SetAcmsAccessScheduleConfigFields(CHARPTR * rxData)
{
	CHAR					tempConfigBuff[MAX_TMP_BUFF_SIZE];
	UINT16 					configIdx;			//configuration index
	ACCESS_SCHEDULE_t		tempAccessScheduleConfig;//take instance
	UINT8 					parseStatus;
	ACMS_CONFIG_FIELD_ID	fieldId;
	
	// Parse and get config Index
	configIdx = AcmsParseConfigIdx(rxData);
	if(configIdx >= MAX_TIME_FRAME)
		return(FAIL);
    
	//get copy of current configuration
	tempAccessScheduleConfig = AccessScheduleConfig[configIdx];

	//check upto FAIL or LAST_DATA
	while( ( (parseStatus = ParseFieldValue(rxData, tempConfigBuff, &fieldId,
										MAX_TMP_BUFF_SIZE) ) != FAIL) )
	{
		switch(fieldId)
		{
		case TIME_FRAME_FIELD_ENABLE:
			tempAccessScheduleConfig.timeFrameEnableF = (BOOL)atoi(tempConfigBuff);
			break;

		case TIME_FRAME_FIELD_START_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempAccessScheduleConfig.timeFrameStartTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "Access Schedule TimeFrameConfig");
				EPRINT(SYS_LOG, "Access Schedule TimeFrameConfig FAIL");
				return FAIL;
			}
			break;


		case TIME_FRAME_FIELD_END_TIME:
			if(GetTimeFromString(tempConfigBuff, &tempAccessScheduleConfig.timeFrameEndTime) == FAIL)
			{
				CONFIGPRINT(CONFIG_FAIL, "Access Schedule TimeFrameConfig");
				EPRINT(SYS_LOG, "Access Schedule TimeFrameConfig FAIL");
				return FAIL;
			}
			break;


		case TIME_FRAME_FIELD_ACTIVEDAY0:
		case TIME_FRAME_FIELD_ACTIVEDAY1:
		case TIME_FRAME_FIELD_ACTIVEDAY2:
		case TIME_FRAME_FIELD_ACTIVEDAY3:
		case TIME_FRAME_FIELD_ACTIVEDAY4:
		case TIME_FRAME_FIELD_ACTIVEDAY5:
		case TIME_FRAME_FIELD_ACTIVEDAY6:
			tempAccessScheduleConfig.timeFrameActiveDaysF[fieldId - TIME_FRAME_FIELD_ACTIVEDAY0] = (BOOL)atoi(tempConfigBuff);
			break;

		case TIME_FRAME_FIELD_ACTIVEHOLIDAY:
			tempAccessScheduleConfig.timeFrameHolidayF = (BOOL)atoi(tempConfigBuff);
			break;

		default:
			EPRINT(ACMS_RXTX_LOG, "Set ACMS Config Failed in ACCESS_SCHEDULE_TABLE_ID:fieldId = [%d], value = [%s]",fieldId,tempConfigBuff);
			EPRINT(SYS_LOG, "Access Schedule TimeFrameConfig FAIL");
			CONFIGPRINT(CONFIG_FAIL, "AccessScheduleConfig");
		}

		//check last data
		if(parseStatus == LAST_DATA)
		{
			if(ConfigUpdateHandler(ACCESS_SCHEDULE_TABLE_ID, configIdx, (VOIDPTR)(&tempAccessScheduleConfig)) == SUCCESS)
			{
				DPRINT(ACMS_LOG,"Access Schedule configuration set successfully");
				return SUCCESS;
			}
		}
	}
	EPRINT(SYS_LOG, "Access Schedule TimeFrameConfig FAIL");
	CONFIGPRINT(CONFIG_FAIL, "AccessScheduleConfig");
	return FAIL;
}

