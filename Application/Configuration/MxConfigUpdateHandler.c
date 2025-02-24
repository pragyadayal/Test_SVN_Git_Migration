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
//   File         : MxConfigUpdateHandler.c
//   Description  : This file consist of Functions related to actions on
//					configuration update.
//
///////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include <stdio.h>
#include <string.h>

#include "MxTypedef.h"
#include "MxAcmsTask.h"
#include "MxCafeteria.h"
#include "MxConfig.h"
#include "MxConfigFileIo.h"
#include "MxConfigUpdateHandler.h"
#include "MxDst.h"
#include "MxLogs.h"
#include "MxIOLink.h"
#include "MxCerTask.h"
#include "MxDcTask.h"
#include "MxOEM75Drv.h"
#include "MxNetworkUtils.h"
#include "MxWebAPITcpEventsTask.h"
#include "MxReaderUtils.h"
#include "MxSysTime.h"
#include "MxSmartCardFormatConfig.h"
#include "MxWiegandOutConfig.h"
#include "MxIdentificationServerConfig.h"
#include "MxCafeScreenMirror.h"
#include "MxDcTask.h"
#include "MxMifareSectorWiseKeyConfig.h"
#include "MxBluetoothConfig.h"
#include "MxAttendanceDetail.h"
#include "MxAutoInterfaceManager.h"
#include "MxIpCamera.h"
#include "MxBluetooth.h"
#include "MxCmdCtrl.h"
#include "MxWebJvsFaceRecognitionSetting.h"
#include "MxFaceRecognitionConfig.h"
#include "MxSplFnScheduleConfig.h"
#include "MxJobCosting.h"
#include "MxReaderBluetoothConfig.h"
#include "MxCommonBLE.h"
#include "MxTemperatureSensor.h"
#include "MxUserConfig.h"
#include "MxTempUserTask.h"
#include "MxReaderCommonUtils.h"
#include "MxCommonFunctions.h"

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
extern WEB_API_EVENTS_CONFIG_t		WebAPIEventsConfig;
extern IO_LINK_EXT_DEV_CONFIG_t		IoLinkExtDevConfig[MAX_IO_LINK_EXT_DEVICE];
extern IO_LINK_EXT_DEV_MAPPING_t	IoLinkExtDevMapping[MAX_IO_LINK];
extern CARD_FORMAT_CONFIG_t			CardFormatConf[MAX_CARD_FORMAT_PER_DOOR];
extern VAM_CONFIG_t					VamConfig[MAX_VAM_CONFIG];
extern JCM_CONFIG_t					JcmConfig[MAX_JCM_CONFIG+3]; //+3 for 'None', 'Default' and 'Continue' Job
extern DOOR_MODE_SCH_CONFIG_t		DoorModeSchCfg[MAX_DOOR_MODE_SCHEDULE];
extern READER_MODE_e				DoorEntryExitMode;
extern BOOL							FprUninitReqdF[MAX_READERS];
extern DOOR_ACTIVITY_INFO_t			DcInfo;				//Door Controller FSM info
extern IP_CAMERA_CONFIG_t			IpCameraConfig;
extern BLUETOOTH_CONFIG_t			BluetoothPara;
extern BACKUP_AND_UPDATE_t			BackUpAndUpdate;
extern COMMAND_CTRL_CONFIG_t		CmdCtrlConfig;
extern READER_BLUETOOTH_CONFIG_t	ReaderBluetoothPara;
extern CURRENT_ACTIVE_BLE_e 		CurrentActiveBLE;
extern ACCESS_SCHEDULE_t			AccessScheduleConfig[MAX_TIME_FRAME];

//******** Defines and Data Types ****



//******** Function Prototypes *******
static BOOL ConfigUpdateUnknownConfigFields(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL UserConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL HolidayScheduleConfigUpdateHandler1(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL HolidayScheduleConfigUpdateHandler2(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL HolidayScheduleConfigUpdateHandler3(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL HolidayScheduleConfigUpdateHandler4(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL HolidayScheduleConfigUpdateHandler(UINT8 holidaySchIndex, UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL TwoPersonConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL IoLinkConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL FirstInUserConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL GenSysParaBasicConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL GenSysParaAdvConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL EthernetParaConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL ServerParaConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL DstConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL SplFunctionConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL VisitorMgmtConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL TimeTriggerConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL CafeItemConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL CafeMenuConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL CafeMenuSchConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL PrinterConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL FprConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL DvrEvtActnConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL SmartRouteAccessConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL WebAPIEventsConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL IoLinkExtDevConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL IoLinkExtDevMappingUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL CardFormatConfigUpdateHandler(UINT16 cardFormatId, VOIDPTR tempConfigPtr);
static BOOL VamConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL JcmConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL RmsConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL BleConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL IpCameraConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL IpCameraAdvanceConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL BackupUpdateConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL CccParaConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL ReaderBleConfigUpdateHandler(__attribute__((unused))UINT16 configIdx, VOIDPTR tempConfigPtr);
static BOOL AccessScheduleConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr);

//******** Global Variables **********


//******** Static Variables **********
// Set Configration
// NOTE:This table entry must be match with table CONFIGURATION_TABLE_e
static BOOL (*configUpdateHandlerFuncPtr[MAX_CONFIG_TABLE])(UINT16, VOIDPTR) =
{
	GEN_ACMS_DEVICE_CONFIGURATION_TABLE(GEN_CONFIGURATION_TABLE_CONFIG_UPDATE_FUNC)
	GEN_DEVICE_CONFIGURATION_TABLE(GEN_CONFIGURATION_TABLE_CONFIG_UPDATE_FUNC)
};

//******** Function Definations ******
//*****************************************************************************
//	ConfigUpdateHandler()
//	Param:
//		IN	: configUpdateTableIdx	- updated config table index
//			: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API will Navigate to particular config update handler.
//*****************************************************************************
BOOL ConfigUpdateHandler(CONFIGURATION_TABLE_e configUpdateTableIdx, UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	return (*configUpdateHandlerFuncPtr[configUpdateTableIdx])(configIdx, tempConfigPtr);
}

//*****************************************************************************
//	ConfigUpdateUnknownConfigFields()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//	Returns:
//		STATUS-Always fail
//	Description:
//	    This function is just a dummy function to adjust for configuration table
//
//*****************************************************************************
static BOOL ConfigUpdateUnknownConfigFields(UINT16 configIdx, UNUSED_PARA VOIDPTR tempConfigPtr)
{
	EPRINT(CONFIG_LOG," This function should not have been called !!! ConfigIndex=%u\n",configIdx);
	return FAIL;
}

//*****************************************************************************
//	UserConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating USER_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL UserConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	USER_CONFIG_t tempUserConfig = *(USER_CONFIG_t *)(tempConfigPtr);

	// Check if user is being removed from configuartion
	if( (tempUserConfig.enable == FALSE) && (tempUserConfig.userId == INVALID_USER_ID) )
	{
		if( (IsUserEnable(configIdx) != FALSE) || (GetUserId(configIdx) != INVALID_USER_ID) )
		{
			// Remove user from system
			RemoveUserFromSystem(configIdx);
			return SUCCESS;
		}
	}
	//store data in configuration structure
	SetUserConfig(configIdx, tempUserConfig);

	//Write data to file.
	if(WriteSingleUserConfig(configIdx) == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	HolidayScheduleConfigUpdateHandler1()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API Navigates to HolidayScheduleConfigUpdateHandler with proper holiday
//		schedule index.
//*****************************************************************************
static BOOL HolidayScheduleConfigUpdateHandler1(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	return HolidayScheduleConfigUpdateHandler(HOLIDAY_SCHEDULE_1, configIdx, tempConfigPtr);
}

//*****************************************************************************
//	HolidayScheduleConfigUpdateHandler2()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API Navigates to HolidayScheduleConfigUpdateHandler with proper holiday
//		schedule index.
//*****************************************************************************
static BOOL HolidayScheduleConfigUpdateHandler2(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	return HolidayScheduleConfigUpdateHandler(HOLIDAY_SCHEDULE_2, configIdx, tempConfigPtr);
}

//*****************************************************************************
//	HolidayScheduleConfigUpdateHandler3()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API Navigates to HolidayScheduleConfigUpdateHandler with proper holiday
//		schedule index.
//*****************************************************************************
static BOOL HolidayScheduleConfigUpdateHandler3(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	return HolidayScheduleConfigUpdateHandler(HOLIDAY_SCHEDULE_3, configIdx, tempConfigPtr);
}

//*****************************************************************************
//	HolidayScheduleConfigUpdateHandler4()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API Navigates to HolidayScheduleConfigUpdateHandler with proper holiday
//		schedule index.
//*****************************************************************************
static BOOL HolidayScheduleConfigUpdateHandler4(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	return HolidayScheduleConfigUpdateHandler(HOLIDAY_SCHEDULE_4, configIdx, tempConfigPtr);
}

//*****************************************************************************
//	HolidayScheduleConfigUpdateHandler()
//	Param:
//		IN	: holidaySchIndex		- holiday Schedule Index
//			: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating HOLIDAY_SCHEDULE_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL HolidayScheduleConfigUpdateHandler(UINT8 holidaySchIndex, UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	BOOL				writeSingleHolidayConfigF = TRUE;
	HOLIDAY_SCHEDULE_t	tempHolidaySchedule = *(HOLIDAY_SCHEDULE_t *)(tempConfigPtr);

	//Write data to file.
	if(tempHolidaySchedule.scheduleId != HolidaySchedule[holidaySchIndex].scheduleId)
	{
		writeSingleHolidayConfigF = FALSE;
	}

	//store data in configuration structure
	HolidaySchedule[holidaySchIndex] = tempHolidaySchedule;

	if( ( (writeSingleHolidayConfigF == TRUE) && (WriteSingleHolidaySchConfig(configIdx, holidaySchIndex) == SUCCESS) ) ||
		( (writeSingleHolidayConfigF == FALSE) && (WriteHolidaySchConfig(holidaySchIndex) == SUCCESS) ) )
	{
		// Time Trigger Feature depends on Holiday Schedule
		return SUCCESS;
	}

	return FAIL;
}

//*****************************************************************************
//	TwoPersonConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating TWO_PERSON_GROUP_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL TwoPersonConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	//store data in configuration structure
	TwoPersonGroup[configIdx] = *(TWO_PERSON_GROUP_t *)(tempConfigPtr);
	if(WriteSingleTwoPersonConfig(configIdx) == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	IoLinkConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating IO_LINK_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL IoLinkConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	IO_LINK_CONFIG_t	tempIoLinkConfig = *(IO_LINK_CONFIG_t *)(tempConfigPtr);

	// If output type is changed then deactivate output port
	// before changing  IO link configuration
	if( ( (IoLinkConfig[configIdx].outputType != tempIoLinkConfig.outputType)
		&& (IoLinkConfig[configIdx].enable == ENABLED) ) ||  (IoLinkConfig[configIdx].resetLinkF != tempIoLinkConfig.resetLinkF)
		 || (IoLinkConfig[configIdx].resetLinkTime.hour != tempIoLinkConfig.resetLinkTime.hour) || (IoLinkConfig[configIdx].resetLinkTime.minute != tempIoLinkConfig.resetLinkTime.minute) )
	{
		DeactivateIoLink(configIdx);
	}

	// Save configuration
	IoLinkConfig[configIdx] = tempIoLinkConfig;
	if(WriteSingleIoLinkConfig(configIdx) == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	FirstInUserConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating FIRST_IN_LIST_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL FirstInUserConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	// Save new configuration
	FirstInUserGroup[configIdx] = *(FIRST_IN_LIST_t *)(tempConfigPtr);
	if(WriteSingleFirstInConfig(configIdx) == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	GenSysParaBasicConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating SYS_BASIC_PARA_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL GenSysParaBasicConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	SYS_BASIC_PARA_t	tempSysBasicPara = *(SYS_BASIC_PARA_t *)(tempConfigPtr);
	UINT8 				readerId;
	BOOL                		updateDeviceStatusF = FALSE;


	if( tempSysBasicPara.reader[READER3].type != SysBasicPara.reader[READER3].type ) //Set Device/Reader Inactive Status if reader Type change
	{
		updateDeviceStatusF = TRUE;
	}
	else if( (tempSysBasicPara.reader[READER1].mode != SysBasicPara.reader[READER1].mode) || (tempSysBasicPara.reader[READER3].mode != SysBasicPara.reader[READER3].mode) ) //Set Device/Reader Inactive Status if readermode change
	{
		updateDeviceStatusF = TRUE;
	}

	if(SysBasicPara.extWiegandRdrMode != tempSysBasicPara.extWiegandRdrMode)
	{
		SetWiegandRdrMode(READER3, tempSysBasicPara.extWiegandRdrMode);
	}


	if(( (SysBasicPara.cerPresentF == TRUE) && (tempSysBasicPara.reader[READER3].type != COMBO_EXIT_READER) ) ||
		( (SysBasicPara.cbrPresentF == TRUE) && (tempSysBasicPara.reader[READER3].type != CB_U_READER) ) ||
		( (SysBasicPara.reader[READER3].type == CB_W_READER) && (tempSysBasicPara.reader[READER3].type != CB_W_READER) ) ||
		((SysBasicPara.atomRD100PresentF == TRUE) && (tempSysBasicPara.reader[READER3].type != ATOM_RD100_READER)) ||
		((SysBasicPara.atomRD200PresentF == TRUE) && (tempSysBasicPara.reader[READER3].type != ATOM_RD200_READER)) ||
		((SysBasicPara.atomRD300PresentF == TRUE) && (tempSysBasicPara.reader[READER3].type != ATOM_RD300_READER)))
	{
		tempSysBasicPara.reader[READER4].type = NO_READER_PRESENT;
		tempSysBasicPara.reader[READER5].type = NO_READER_PRESENT;
		tempSysBasicPara.cerPresentF = FALSE;
		tempSysBasicPara.cbrPresentF = FALSE;
		tempSysBasicPara.atomRD100PresentF = FALSE;
		tempSysBasicPara.atomRD200PresentF = FALSE;
		tempSysBasicPara.atomRD300PresentF = FALSE;
	}

	if( (tempSysBasicPara.reader[READER3].type == COMBO_EXIT_READER) )
	{
		// In case of CER get reader3 type from auto detected table
		tempSysBasicPara.reader[READER3].type = GetAutoDetectedReaderType(READER3);
		tempSysBasicPara.reader[READER4].type = SUP_FP_3030_READER;
		tempSysBasicPara.reader[READER5].type = NO_READER_PRESENT;
		tempSysBasicPara.cerPresentF = TRUE;
		tempSysBasicPara.cbrPresentF = FALSE;
		tempSysBasicPara.atomRD100PresentF = FALSE;
		tempSysBasicPara.atomRD200PresentF = FALSE;
		tempSysBasicPara.atomRD300PresentF = FALSE;
	}


	if( (tempSysBasicPara.reader[READER3].type == CB_U_READER) )
	{
		// In case of CER get reader3 type from auto detected table
		tempSysBasicPara.reader[READER3].type = GetAutoDetectedReaderType(READER3);
		tempSysBasicPara.reader[READER4].type = CB_U_READER;
		tempSysBasicPara.reader[READER5].type = NO_READER_PRESENT;
		tempSysBasicPara.cbrPresentF = TRUE;
		tempSysBasicPara.cerPresentF = FALSE;
		tempSysBasicPara.atomRD100PresentF = FALSE;
		tempSysBasicPara.atomRD200PresentF = FALSE;
		tempSysBasicPara.atomRD300PresentF = FALSE;
	}

	if(tempSysBasicPara.reader[READER3].type == CB_W_READER)
	{
		tempSysBasicPara.reader[READER3].type = CB_W_READER;
		tempSysBasicPara.reader[READER4].type = CB_W_READER;
		tempSysBasicPara.reader[READER5].type = NO_READER_PRESENT;
		tempSysBasicPara.cerPresentF = FALSE;
	}

	if( (tempSysBasicPara.reader[READER3].type == ATOM_RD100_READER) )
	{
		tempSysBasicPara.reader[READER3].type = GetAutoDetectedReaderType(READER3);
		tempSysBasicPara.reader[READER4].type = ATOM_RD100_READER;
		tempSysBasicPara.reader[READER5].type = NO_READER_PRESENT;
		tempSysBasicPara.atomRD100PresentF = TRUE;
		tempSysBasicPara.cbrPresentF = FALSE;
		tempSysBasicPara.cerPresentF = FALSE;
		tempSysBasicPara.atomRD200PresentF = FALSE;
		tempSysBasicPara.atomRD300PresentF = FALSE;
	}

	if((ATOM_RD200_READER == tempSysBasicPara.reader[READER3].type))
	{
		tempSysBasicPara.reader[READER3].type = GetAutoDetectedReaderType(READER3);
		tempSysBasicPara.reader[READER4].type = ATOM_RD200_READER;
		tempSysBasicPara.reader[READER5].type = SUP_FP_3030_READER;
		tempSysBasicPara.atomRD200PresentF = TRUE;
		tempSysBasicPara.atomRD100PresentF = FALSE;
		tempSysBasicPara.atomRD300PresentF = FALSE;
		tempSysBasicPara.cbrPresentF = FALSE;
		tempSysBasicPara.cerPresentF = FALSE;
	}

	if((ATOM_RD300_READER == tempSysBasicPara.reader[READER3].type))
	{
		tempSysBasicPara.reader[READER3].type = GetAutoDetectedReaderType(READER3);
		tempSysBasicPara.reader[READER4].type = ATOM_RD300_READER;
		tempSysBasicPara.reader[READER5].type = SUP_FP_3030_READER;
		tempSysBasicPara.atomRD300PresentF = TRUE;
		tempSysBasicPara.atomRD100PresentF = FALSE;
		tempSysBasicPara.atomRD200PresentF = FALSE;
		tempSysBasicPara.cbrPresentF = FALSE;
		tempSysBasicPara.cerPresentF = FALSE;
	}

	for(readerId = 0; readerId < MAX_READERS; readerId++)
	{
		/* Reader2 is used for internal FP reader, so if it is not detected then 
		 * do not start any process for FP reader type */
		if((readerId == READER2) && (FALSE == IsVariant_With_Internal_FP_Rdr()))
		{
			SysBasicPara.reader[readerId].type = tempSysBasicPara.reader[readerId].type;
			continue;
		}
		
		// In case of reader type as CER is changed
		if( (readerId == READER3) && (SysBasicPara.cerPresentF != tempSysBasicPara.cerPresentF) )
		{
			SysBasicPara.cerPresentF = tempSysBasicPara.cerPresentF;
			SysBasicPara.reader[readerId].type = tempSysBasicPara.reader[readerId].type;
			StartReaderInitProcess(readerId);
		}
		else if( (readerId == READER3) && (SysBasicPara.cbrPresentF != tempSysBasicPara.cbrPresentF) )
		{
			SysBasicPara.cbrPresentF = tempSysBasicPara.cbrPresentF;
			SysBasicPara.reader[readerId].type = tempSysBasicPara.reader[readerId].type;
			StartReaderInitProcess(readerId);
		}
		else if( (readerId == READER3) && (SysBasicPara.atomRD100PresentF != tempSysBasicPara.atomRD100PresentF) )
		{
			SysBasicPara.atomRD100PresentF = tempSysBasicPara.atomRD100PresentF;
			SysBasicPara.reader[readerId].type = tempSysBasicPara.reader[readerId].type;
			StartReaderInitProcess(readerId);
		}
		else if( (readerId == READER3) && (SysBasicPara.atomRD200PresentF != tempSysBasicPara.atomRD200PresentF) )
		{
			SysBasicPara.atomRD200PresentF = tempSysBasicPara.atomRD200PresentF;
			SysBasicPara.reader[readerId].type = tempSysBasicPara.reader[readerId].type;
			StartReaderInitProcess(readerId);
		}
		else if( (readerId == READER3) && (SysBasicPara.atomRD300PresentF != tempSysBasicPara.atomRD300PresentF) )
		{
			SysBasicPara.atomRD300PresentF = tempSysBasicPara.atomRD300PresentF;
			SysBasicPara.reader[readerId].type = tempSysBasicPara.reader[readerId].type;
			StartReaderInitProcess(readerId);
		}
		else if(SysBasicPara.reader[readerId].type != tempSysBasicPara.reader[readerId].type)
		{
			if(SysBasicPara.reader[readerId].type == SUP_FP_3030_READER)
			{
				// if FPR was configured un-initialization required
				FprUninitReqdF[readerId] = TRUE;
			}
			SysBasicPara.reader[readerId].type = tempSysBasicPara.reader[readerId].type;
			StartReaderInitProcess(readerId);
		}
	}

	SendReader4TypeConfigToExternalReader();
	// Clear door sense status, if sense type is changed or supervision option is changed or
	// door sense is enabled from disabled state
	if( ( (SysBasicPara.doorSense.enable == DISABLED) && (tempSysBasicPara.doorSense.enable == ENABLED) ) ||
		(SysBasicPara.doorSense.type != tempSysBasicPara.doorSense.type) ||
		(SysBasicPara.doorSense.supervise != tempSysBasicPara.doorSense.supervise) )
	{
		ClearDoorSenseStatus();
	}

	if(SysBasicPara.application != tempSysBasicPara.application)
	{
		SysBasicPara.application = tempSysBasicPara.application;

		if(SysBasicPara.application == CAFETERIA)
		{
			tempSysBasicPara.reader[READER3].type = NO_READER_PRESENT;
			InitCafeScreenMirrorTask();
			SysBasicPara.cafeteriaFrAccessMode = tempSysBasicPara.cafeteriaFrAccessMode;
		}
		else
		{
			ExitCafeScreenMirrorTask();
		}
		SendMessageToQt(&tempSysBasicPara.application, GUI_MSG_APPL_MODE_INFO);
		reloadCafeMenuF = CAFE_RELOAD_COMPLETE;
		ReloadIdleScreen();
	}

	if((SysBasicPara.application == CAFETERIA) && (SysBasicPara.cafeteriaFrAccessMode != tempSysBasicPara.cafeteriaFrAccessMode))
	{
		SysBasicPara.cafeteriaFrAccessMode = tempSysBasicPara.cafeteriaFrAccessMode;
		reloadCafeMenuF = CAFE_RELOAD_ITEM;
		ReloadIdleScreen();
	}

	if(tempSysBasicPara.buzMuteF != SysBasicPara.buzMuteF)
	{
		SysBasicPara.buzMuteF = tempSysBasicPara.buzMuteF;
		SetBuzMuteFlagStatus(SysBasicPara.buzMuteF);
		SendCerBuzzerStatus();
	}

	if(tempSysBasicPara.timeFormat != SysBasicPara.timeFormat)
	{
		SysBasicPara.timeFormat = tempSysBasicPara.timeFormat;
		//To update DST related Info.
		SendMessageToQt(" ", GUI_MSG_SEND_CONFIG_INFO); //For Sending  Time Format
	}

	// Update door idle screen. If door mode schedule is enabled than IN/OUT mode will be updated on display
	if(tempSysBasicPara.doorModeSchdlEnblF != SysBasicPara.doorModeSchdlEnblF)
	{
		SysBasicPara.doorModeSchdlEnblF = tempSysBasicPara.doorModeSchdlEnblF;
		DoorModeScheduleUpdate();
	}

	// Finger print template format change
	if(tempSysBasicPara.fprTemplateFormat != SysBasicPara.fprTemplateFormat)
	{
		SysBasicPara.fprTemplateFormat = tempSysBasicPara.fprTemplateFormat;
		DeleteAllFpFromDoor();
		SetFprPara();
	}

	if(tempSysBasicPara.IoLinkNwInterface != SysBasicPara.IoLinkNwInterface)
	{
		SysBasicPara.IoLinkNwInterface = tempSysBasicPara.IoLinkNwInterface;
		CloseIoLinkSocket();
		OpenIoLinkSocket();
	}

	if(SysBasicPara.doorAccessMode != tempSysBasicPara.doorAccessMode)
	{
		CleanUpTempUserFP();
	}

    if( (tempSysBasicPara.hideMenuBarForAccessScheduleF != SysBasicPara.hideMenuBarForAccessScheduleF) &&
            (TRUE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER1)) )
    {
        SysBasicPara.hideMenuBarForAccessScheduleF = tempSysBasicPara.hideMenuBarForAccessScheduleF;
        SetAudioVisual(USER_RESP_DEVICE_INACTIVE, INTERNAL_PORT, NULL);
    }

	if(updateDeviceStatusF == TRUE)
	{
		SysBasicPara.reader[READER1].mode = tempSysBasicPara.reader[READER1].mode;
		SysBasicPara.reader[READER3].mode = tempSysBasicPara.reader[READER3].mode;
	
		if(TRUE == getDeviceInactiveStatus())
		{
		      if( (FALSE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER1)) || (FALSE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER3)))
		      {
			    setDeviceInactiveStatus(FALSE, TRUE);
		      }
		      setDeviceInactiveStatus(TRUE, TRUE);
		}
	}

    if(strcmp(SysBasicPara.userDefNtpAddr, tempSysBasicPara.userDefNtpAddr) != 0)
    {
        if(!( is_valid_ipv4(tempSysBasicPara.userDefNtpAddr) || is_valid_domain(tempSysBasicPara.userDefNtpAddr) ))
        {
            memcpy(tempSysBasicPara.userDefNtpAddr, SysBasicPara.userDefNtpAddr, sizeof(tempSysBasicPara.userDefNtpAddr));
        }
    }
    SysBasicPara = tempSysBasicPara;

	if( (SysBasicPara.accessAllowCadDuration < MAX_CAD_DURATION) && (SysBasicPara.accessDenyCadDuration < MAX_CAD_DURATION) )
	{
		SetCadenceDuration(SysBasicPara.accessAllowCadDuration, SysBasicPara.accessDenyCadDuration);
	}

	//store device MID to show in web page even when door is offline.
	SysBasicPara.devId = GetDeviceMID();

	// Update system time. If auto mode SNTP will be started.
	// If manual mode date time will be updated from hardware clock.
	StartTimeSync();

	if(WriteBasicParaConfig() == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	GenSysParaAdvConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating SYS_ADVANCE_PARA_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL GenSysParaAdvConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	SYS_ADVANCE_PARA_t	tempSysAdvPara = *(SYS_ADVANCE_PARA_t *)(tempConfigPtr);
	BOOL				fprParaChangeF = FALSE, hidKeyChangeF = FALSE;
	UINT8				readerId;

	if(SysAdvPara.templatesPerFinger != tempSysAdvPara.templatesPerFinger)
	{
		fprParaChangeF = TRUE;
	}

	// Clear occupancy count if required
	if(SysAdvPara.occupancyCtrl.enable != tempSysAdvPara.occupancyCtrl.enable)
	{
		if(tempSysAdvPara.occupancyCtrl.enable == ENABLED)
		{
			ClearOccupancyCount();
		}
	}

	// Clear current Aux In physical status, if sense type is changed or supervision option is changed or
	// Aux input is enabled from disabled state
	if( ( (SysAdvPara.auxInput.enable == DISABLED) && (tempSysAdvPara.auxInput.enable == ENABLED) ) ||
		(SysAdvPara.auxInput.sense != tempSysAdvPara.auxInput.sense) ||
		(SysAdvPara.auxInput.supervise != tempSysAdvPara.auxInput.supervise) )
	{
		ClearAuxInStatus();
	}

	if(tempSysAdvPara.hidIclasCustomKeyEnbl == ENABLED)
	{
		if( (SysAdvPara.hidIclasCustomKeyEnbl == DISABLED) || (memcmp(tempSysAdvPara.hidIclasCustomKey, SysAdvPara.hidIclasCustomKey, HID_ICLASS_KEY_SIZE) != 0) )
		{
			hidKeyChangeF = TRUE;
		}
	}

	if((tempSysAdvPara.firstInResetAfter != SysAdvPara.firstInResetAfter) || (tempSysAdvPara.firstIn.enable != SysAdvPara.firstIn.enable))
	{
		DeleteTimer(&(DcInfo.firstInResetTimer));
		if(LoadDfltFirstInAccessInfo() != SUCCESS)
		{
			return FAIL;
		}
	}

	if(tempSysAdvPara.showAttndcDtlF != SysAdvPara.showAttndcDtlF)
	{
		EnableOrDisableShowAttendanceDetailF(tempSysAdvPara.showAttndcDtlF);
	}

	// Check DVR-NVR related para first to avoid race-condition
	if((SysAdvPara.dvrInterfaceSelctionF != tempSysAdvPara.dvrInterfaceSelctionF) ||
			(SysAdvPara.dvrIntigrationEnbl != tempSysAdvPara.dvrIntigrationEnbl) ||
			(SysAdvPara.dvrInterface != tempSysAdvPara.dvrInterface) ||
			(SysAdvPara.dvrPort != tempSysAdvPara.dvrPort) ||
			(strcmp(SysAdvPara.dvrIpAddr, tempSysAdvPara.dvrIpAddr) != 0)  )
	{
		SysAdvPara.dvrInterfaceSelctionF = tempSysAdvPara.dvrInterfaceSelctionF;
		SysAdvPara.dvrIntigrationEnbl = tempSysAdvPara.dvrIntigrationEnbl;
		SysAdvPara.dvrInterface = tempSysAdvPara.dvrInterface;
		SysAdvPara.dvrPort = tempSysAdvPara.dvrPort;
		strcpy(SysAdvPara.dvrIpAddr, tempSysAdvPara.dvrIpAddr);

		// If any change in DVR-NVR related data then only we have to restart auto interface functionality.
		if(SysAdvPara.dvrInterfaceSelctionF == AUTO_SELECTION)
		{
			SetAutoInterfaceRestartF(AUTO_DVR_NVR, TRUE);
		}
		else
		{
			EPRINT(COMM_LOG, "DVR_NVR interface selection must be auto");
			SysAdvPara.dvrInterfaceSelctionF = AUTO_SELECTION;
			SetAutoInterfaceRestartF(AUTO_DVR_NVR, TRUE);
		}
	}

	if(tempSysAdvPara.retainJobSelectionF != SysAdvPara.retainJobSelectionF)
	{
		ClearRetainJobCode();
	}

	/*  Update temperature log config and act upon it */
	if ((SysAdvPara.temperatureLogCnfg.isEnable != tempSysAdvPara.temperatureLogCnfg.isEnable)
			&& (tempSysAdvPara.temperatureLogCnfg.isEnable == INACTIVE))
	{
		/* Deactivate temperature sensor */
		DeactivateTemperatureSensor();
	}
	else if (tempSysAdvPara.temperatureLogCnfg.isEnable == ACTIVE)
	{
		if ((SysAdvPara.temperatureLogCnfg.isEnable != tempSysAdvPara.temperatureLogCnfg.isEnable)
				|| (SysAdvPara.temperatureLogCnfg.sensorType != tempSysAdvPara.temperatureLogCnfg.sensorType)
				|| (SysAdvPara.temperatureLogCnfg.sensorInterface != tempSysAdvPara.temperatureLogCnfg.sensorInterface))
		{
			/* Init Temperature sensor with new values */
			InitTemperatureSensor(tempSysAdvPara.temperatureLogCnfg);
		}

		/* check if there is any change in temperature unit */
		if (SysAdvPara.temperatureLogCnfg.tempUnit != tempSysAdvPara.temperatureLogCnfg.tempUnit)
		{
			/* Update Stored Ambient and Current Temperature as per new unit */
			UpdateStoredTemperature(tempSysAdvPara.temperatureLogCnfg.tempUnit);
		}

		/* Check if emissivity update required */
		if ((SysAdvPara.temperatureLogCnfg.emissivity != tempSysAdvPara.temperatureLogCnfg.emissivity)
				&& (tempSysAdvPara.temperatureLogCnfg.sensorType != SENSOR_WEB_BASED))
		{
			SetTemperatureSensorEmissivity(&tempSysAdvPara.temperatureLogCnfg.emissivity);
		}
	}
	//send inter digit wait timmer to External reader
	if(SysAdvPara.interDigitTimer != tempSysAdvPara.interDigitTimer)
	{
		SendInterDigitTimerToExternalReader(tempSysAdvPara.interDigitTimer);
	}

	if(SysAdvPara.multiAccessTimer != tempSysAdvPara.multiAccessTimer)
	{
		SendMultiAccessTimerToExtRdr(tempSysAdvPara.multiAccessTimer);
	}

	if(SysAdvPara.itemQuantityRestrictionF != tempSysAdvPara.itemQuantityRestrictionF)
	{
		DefaultConsumedItemQuantityForAllUser();
	}

	if(SysAdvPara.temporaryUserGenerateInfo.autoAddTempUser != tempSysAdvPara.temporaryUserGenerateInfo.autoAddTempUser)
	{
		ResetTempUserAddInfo();
	}
	
	 //Set Device/Reader Inactive Status if readermode/below change
	if(tempSysAdvPara.verifyAccessPolicyOnExitF != SysAdvPara.verifyAccessPolicyOnExitF)
	{
		SysAdvPara.verifyAccessPolicyOnExitF = tempSysAdvPara.verifyAccessPolicyOnExitF;

		if(TRUE == getDeviceInactiveStatus())
		{
		      if( (FALSE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER1)) || (FALSE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER3)))
		      {
			    setDeviceInactiveStatus(FALSE, TRUE);
		      }
		      setDeviceInactiveStatus(TRUE, TRUE);
		}
	}

    if(SysAdvPara.authenticationAlgorithm != tempSysAdvPara.authenticationAlgorithm || SysAdvPara.authenticationType != tempSysAdvPara.authenticationType)
    {
        SysAdvPara.authenticationType = tempSysAdvPara.authenticationType;
        SysAdvPara.authenticationAlgorithm = tempSysAdvPara.authenticationAlgorithm;
        if(SysAdvPara.authenticationType == BASIC_AUTHENTICATION && SysAdvPara.authenticationAlgorithm != MD5_ALGORITM)
        {
            SysAdvPara.authenticationAlgorithm = MD5_ALGORITM;
        }
        WritePasswordToWebAuthFile();
    }
	
	//store data in configuration structure
	SysAdvPara = tempSysAdvPara;

	if(hidKeyChangeF == TRUE)
	{
		for(readerId = 0; readerId < MAX_READERS; readerId++)
		{
			if(SysBasicPara.reader[readerId].type == HID_ICLASS_U_READER)
			{
				SendMsgChangeHIDRdrKey(readerId, READER_KEY_16, SysAdvPara.hidIclasCustomKey);
			}
		}
	}

	//Write data to file.
	if(WriteAdvParaConfig() == SUCCESS)
	{
		/* Update reading on status bar */
		UpdateDisplayStatusField();

		if(fprParaChangeF == TRUE)
		{
			SetFprPara();
		}
		SendMessageToQt(" ", GUI_MSG_SEND_CONFIG_INFO); //For send "auto-hide-menu-bar" flag to Qt
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	EthernetParaConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating ETHERNET_PARA_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL EthernetParaConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	ETHERNET_PARA_t	tempEthernetParameters = *(ETHERNET_PARA_t *)(tempConfigPtr);
	BOOL			ethParaChangeF = FALSE;
	CHAR			cmdBuf[128];

	if( (strcmp(EthernetParameters.ipAddress, tempEthernetParameters.ipAddress) != 0) ||
		(strcmp(EthernetParameters.subnet, tempEthernetParameters.subnet) != 0) ||
		(strcmp(EthernetParameters.gateway, tempEthernetParameters.gateway) != 0) ||
		(strcmp(EthernetParameters.dns1, tempEthernetParameters.dns1) != 0) ||
		(strcmp(EthernetParameters.dns2, tempEthernetParameters.dns1) != 0) )
	{
		ethParaChangeF = TRUE;
	}
	//store data in configuration structure
	EthernetParameters = tempEthernetParameters;

	//Write data to file.
	if(WriteEthParaConfig() == SUCCESS)
	{
		DPRINT(ACMS_LOG, "Ethernet Parameter Config set successfully");

		//Delete deviceIpSignature.txt on change of LAN Settings
		memset(cmdBuf,0,sizeof(cmdBuf));
		sprintf(cmdBuf,"rm %s",DEVICE_IP_SIGN_FILE);
		system(cmdBuf);

		if(ethParaChangeF == TRUE)
		{
			ChangeNetworkInterfacePara(ETHERNET_PARA);
		}
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	ServerParaConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating SERVER_PARA_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL ServerParaConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	SERVER_PARA_t	tempServerParameters = *(SERVER_PARA_t *)(tempConfigPtr);
	BOOL			serverParaChangeF = FALSE;
	CHAR			cmdBuf[128];

	if( (strcmp(ServerParameters.ethServerHostName, tempServerParameters.ethServerHostName) != 0) ||
		(strcmp(ServerParameters.wifiServerHostName, tempServerParameters.wifiServerHostName) != 0) ||
		(strcmp(ServerParameters.modemServerHostName, tempServerParameters.modemServerHostName) != 0) ||
		(ServerParameters.ethServerPort != tempServerParameters.ethServerPort) ||
		(ServerParameters.wifiServerPort != tempServerParameters.wifiServerPort) ||
		(ServerParameters.modemServerPort != tempServerParameters.modemServerPort) )
	{
		serverParaChangeF = TRUE;
	}

//	ServerParameters.ethServerConnection = SERVER_CONN_HOST_NAME;
//	ServerParameters.modemServerConnection = SERVER_CONN_HOST_NAME;
//	ServerParameters.wifiServerConnection = SERVER_CONN_HOST_NAME;
// Connection type is fixed always host name

	//store data in configuration structure
	ServerParameters = tempServerParameters;

	//Write data to file.
	if(WriteServerParaConfig() == SUCCESS)
	{
		DPRINT(ACMS_LOG, "Server Parameter Config set successfully");

		//Delete serverIpSignature.txt on change of Server Settings
		memset(cmdBuf,0,sizeof(cmdBuf));
		sprintf(cmdBuf,"rm %s",SERVER_IP_SIGN_FILE);
		system(cmdBuf);

		if(serverParaChangeF == TRUE)
		{
			ChangeNetworkInterfacePara(SERVER_PARA);
		}
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	DstConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating DST_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL DstConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	//store data in configuration structure
	DisableDstMode();
	DstConfig = *(DST_CONFIG_t *)(tempConfigPtr);
	//Write data to DST file.
	if(WriteDSTConfig() == SUCCESS)
	{
		CheckForDstUpdate();
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	SplFunctionConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating SPECIAL_FUNCTION_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL SplFunctionConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	BOOL updateDispF = FALSE;
	switch(configIdx)
	{
		//Going-In							//Going-Out
		case MARK_CLOCK_IN :				case MARK_CLOCK_OUT :
		case CLK_IN_ENTRY_OFFICIAL_WORK :	case CLK_OUT_EXIT_OFFICIAL_WORK :
		case CLK_IN_MARKING_BREAK :			case CLK_OUT_MARKING_BREAK :
		case OVER_TIME_MARKING_IN :			case OVER_TIME_MARKING_OUT :
		case SHORT_LEAVE_MARKING_IN :		case SHORT_LEAVE_MARKING_OUT :

			if(SpecialFunctions[configIdx].enable != ((SPECIAL_FUNCTION_t *)(tempConfigPtr))->enable)
			{
				updateDispF = TRUE;
			}

		break;

		default:
			break;
	}

	//store data in configuration structure
	SpecialFunctions[configIdx] = *(SPECIAL_FUNCTION_t *)(tempConfigPtr);
	if(WriteSingleSpecialFunctionsConfig(configIdx) == SUCCESS)
	{
		if(updateDispF == TRUE)
		{
			UpdateDisplayStatusField();
		}
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	VisitorMgmtConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating VISITOR_MANAGEMENT_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL VisitorMgmtConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	//store data in configuration structure
	Visitor[configIdx] = *(VISITOR_MANAGEMENT_t *)(tempConfigPtr);
	if(WriteSingleVisitorMngmentConfig(configIdx) == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	TimeTriggerConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating TIME_TRIG_FUNC_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL TimeTriggerConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	//store data in configuration structure
	TimeTrigFunc[configIdx] = *(TIME_TRIG_FUNC_t *)(tempConfigPtr);
	if(WriteTimeTrigFuncConfig() == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	CafeItemConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating CAFE_ITEM_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL CafeItemConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	CafeItemConfig[configIdx] = *(CAFE_ITEM_CONFIG_t *)(tempConfigPtr);

	//Write data to file.
	if(WriteCafeItemConfig() == SUCCESS)
	{
		reloadCafeMenuF = CAFE_RELOAD_ITEM;
	//	ReloadCafeMenu();
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	CafeMenuConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating CAFE_MENU_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL CafeMenuConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	//Get current data
	CafeMenuConfig[configIdx] = *(CAFE_MENU_CONFIG_t *)(tempConfigPtr);

	if(IsCafeMenuIsCurrMenu(configIdx) == TRUE)
	{
		reloadCafeMenuF = CAFE_RELOAD_COMPLETE;
		ReloadCafeMenu();
	}

	if(WriteCafeMenuConfig(configIdx) == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	CafeMenuSchConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating CAFE_MENU_SCHEDULE_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL CafeMenuSchConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	//Get current data
	CafeMenuSchConfig[configIdx] = *(CAFE_MENU_SCHEDULE_t *)(tempConfigPtr);
	//check current menuschedule is changed then reload menu
	if(IsCafeMenuSchIsCurrSch(configIdx) == TRUE)
	{
		reloadCafeMenuF = CAFE_RELOAD_COMPLETE;
		ReloadCafeMenu();
	}
	if(WriteCafeMenuScheduleConfig() == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	PrinterConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating FIRST_IN_LIST_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL PrinterConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	//store data in Printer configuration structure
	PrinterConfig = *(PRINTER_CONFIG_t *)(tempConfigPtr);
	if(WritePrinterConfig() == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	FprConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating FPR_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL FprConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	// Save new configuration
	FprConfig = *(FPR_CONFIG_t *)(tempConfigPtr);
	if(WriteFprConfig() == SUCCESS)
	{
		SetFprPara();
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	DvrEvtActnConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating DVR_EVENT_ACTION_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL DvrEvtActnConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	// Save new configuration
	DvrEvtActionConfig[configIdx] = *(DVR_EVENT_ACTION_CONFIG_t *)(tempConfigPtr);
	if(WriteDvrEvtActnConfig() == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	SmartRouteAccessConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating SMART_ROUTE_ACCESS_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL SmartRouteAccessConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	// Save new configuration
	SmartRouteAccessConfig[configIdx] = *(SMART_ROUTE_ACCESS_CONFIG_t *)(tempConfigPtr);
	if(WriteSmartRouteAccessConfig() == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	WebAPIEventsConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating WEB_API_EVENTS_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL WebAPIEventsConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	// Save new configuration
	WebAPIEventsConfig = *(WEB_API_EVENTS_CONFIG_t *)(tempConfigPtr);
	if(WriteWebAPIEventsConfig() == SUCCESS)
	{
		InitWebAPIEventsTask(TRUE);
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	IoLinkExtDevConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating IO_LINK_EXT_DEV_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL IoLinkExtDevConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	// Save new configuration
	memcpy(&IoLinkExtDevConfig, tempConfigPtr, sizeof(IoLinkExtDevConfig));
	if(WriteIoLinkExtDevConfig() == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	IoLinkExtDevMappingUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating IO_LINK_EXT_DEV_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL IoLinkExtDevMappingUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	// Save new configuration
	IoLinkExtDevMapping[configIdx] = *(IO_LINK_EXT_DEV_MAPPING_t *)(tempConfigPtr);
	if(WriteIoLinkExtDevMapping() == SUCCESS)
	{
		DPRINT(CONFIG_LOG,"Io Link External Device Mapping[%d] is written",configIdx);
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	CardFormatConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating CARD_FORMAT_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL CardFormatConfigUpdateHandler(UINT16 cardFormatId, VOIDPTR tempConfigPtr)
{
	// Save new configuration
	CardFormatConf[cardFormatId] = *(CARD_FORMAT_CONFIG_t *)(tempConfigPtr);
	if(WriteCardFormatConfig() == SUCCESS)
	{
		DPRINT(CONFIG_LOG,"card format config is written");
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	VamConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating VAM_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL VamConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	VAM_CONFIG_t tempVamConfig = *(VAM_CONFIG_t *)(tempConfigPtr);

	//store data in configuration structure
	VamConfig[configIdx] = tempVamConfig;

	//Write data to file.
	if(WriteSingleVamConfig(configIdx) == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	JcmConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating JCM_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL JcmConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	JCM_CONFIG_t tempJcmConfig = *(JCM_CONFIG_t *)(tempConfigPtr);

	//store data in configuration structure
	JcmConfig[configIdx] = tempJcmConfig;

	//Write data to file.
	if(WriteSingleJcmConfig(configIdx) == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	RmsConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating DOOR_MODE_SCH_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL RmsConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	DOOR_MODE_SCH_CONFIG_t tempDoorModeSchedul = *(DOOR_MODE_SCH_CONFIG_t *)(tempConfigPtr);

	//store data in configuration structure
	DoorModeSchCfg[configIdx] = tempDoorModeSchedul;

	// Update device display if door mode schedule is enabled
	DoorModeScheduleUpdate();

	if(WriteSingleReaderSchedulConfig(configIdx) == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	BleConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating BLUETOOTH_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL BleConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, UNUSED_PARA VOIDPTR tempConfigPtr)
{
	BOOL	btEnableChangeF = FALSE, btNameChangeF = FALSE, btRangeChangeF = FALSE;

	BLUETOOTH_CONFIG_t tempBluetoothPara = *(BLUETOOTH_CONFIG_t*)(tempConfigPtr);

	if(memcmp(&BluetoothPara,&tempBluetoothPara,sizeof(BluetoothPara)) != 0)
	{
		if(BluetoothPara.btEnable != tempBluetoothPara.btEnable)
		{
			DPRINT(SYS_LOG, "BLE Flag is Changed [%d] to [%d]",BluetoothPara.btEnable, tempBluetoothPara.btEnable);
			btEnableChangeF = TRUE;
		}

		if(strcmp(BluetoothPara.btName,tempBluetoothPara.btName)!=0)
		{
			DPRINT(SYS_LOG, "BLE Name is Changed [%s] to [%s]",BluetoothPara.btName, tempBluetoothPara.btName);
			btNameChangeF = TRUE;
		}

		if(BluetoothPara.btRange != tempBluetoothPara.btRange)
		{
			DPRINT(SYS_LOG, "BLE Range is Changed [%d] to [%d]",BluetoothPara.btRange, tempBluetoothPara.btRange);
			btRangeChangeF = TRUE;
		}
		
		//store data in configuration structure
		BluetoothPara = tempBluetoothPara;

		if(WriteBluetoothConfig() == SUCCESS)
		{
			if(btRangeChangeF == TRUE)
			{
			  DPRINT(BLE_LOG,"Calling SetBleRange Fun\n");
			  SetBleRange(BluetoothPara.btRange);
			}
			else if(BluetoothPara.btRange == BLE_RANGE_CUSTOM)
			{
				SetBleRange(BluetoothPara.btRange);
			}
			if(btEnableChangeF == TRUE)
			{
				StartStopBluetoothServiceThread();
			}
			if(btNameChangeF == TRUE)
			{
				SetBtDeviceNameProperty();
			}

		return SUCCESS;
		}
		else
		{
			return FAIL;
		}
	}
	return SUCCESS;
}

//*****************************************************************************
//	IpCameraConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating IP_CAMERA_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL IpCameraConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	IP_CAMERA_CONFIG_t		tempIpCameraConfig = *(IP_CAMERA_CONFIG_t*)(tempConfigPtr);

	IpCameraConfig = tempIpCameraConfig;

	if(WriteIpCameraConfig() == SUCCESS)
	{
		return SUCCESS;
	}
	else
	{
		return FAIL;
	}
}

//*****************************************************************************
//	IpCameraAdvanceConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating IP_CAMERA_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL IpCameraAdvanceConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	IP_CAMERA_CONFIG_t		tempIpCameraConfig = *(IP_CAMERA_CONFIG_t*)(tempConfigPtr);

	IpCameraConfig = tempIpCameraConfig;

	LoadIpCameraIpResolveTimer(0);
	if(WriteIpCameraConfig() == SUCCESS)
	{
		return SUCCESS;
	}
	else
	{
		return FAIL;
	}
}

//*****************************************************************************
//	BackupUpdateConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL BackupUpdateConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	// Save new configuration
	BackUpAndUpdate = *(BACKUP_AND_UPDATE_t *)(tempConfigPtr);
	if(WriteBackUpAndUpdateConfig() == SUCCESS)
	{
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	CccParaConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating FIRST_IN_LIST_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL CccParaConfigUpdateHandler(UNUSED_PARA UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	//store data in Printer configuration structure
	CmdCtrlConfig = *(COMMAND_CTRL_CONFIG_t *)(tempConfigPtr);
	if(WriteCmdCtrlConfig() == SUCCESS)
	{
		ChangeCCCTaskState();
		if(CmdCtrlConfig.interfaceSelctionF == AUTO_SELECTION)
		{
			SetAutoInterfaceRestartF(AUTO_CCC_SERVER, TRUE);
		}

		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	ReaderBleConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating READER_BLUETOOTH_CONFIG_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL ReaderBleConfigUpdateHandler(__attribute__((unused))UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	READER_BLUETOOTH_CONFIG_t tempReaderBluetoothPara = *(READER_BLUETOOTH_CONFIG_t*)(tempConfigPtr);

	if(memcmp(&ReaderBluetoothPara,&tempReaderBluetoothPara,sizeof(ReaderBluetoothPara)) != 0)
	{
		if(ReaderBluetoothPara.btEnableReader != tempReaderBluetoothPara.btEnableReader)
		{
			DPRINT(SYS_LOG, "Reader-BLE Flag is Changed [%d] to [%d]",ReaderBluetoothPara.btEnableReader, tempReaderBluetoothPara.btEnableReader);
		}

		if(strcmp(ReaderBluetoothPara.btNameReader,tempReaderBluetoothPara.btNameReader)!=0)
		{
			DPRINT(SYS_LOG, "Reader- BLE Name is Changed [%s] to [%s]",ReaderBluetoothPara.btNameReader, tempReaderBluetoothPara.btNameReader);
		}

		if(ReaderBluetoothPara.btRangeReader != tempReaderBluetoothPara.btRangeReader)
		{
			DPRINT(SYS_LOG, "Reader-BLE Range is Changed [%d] to [%d]",ReaderBluetoothPara.btRangeReader, tempReaderBluetoothPara.btRangeReader);
		}

		//store data in configuration structure
		ReaderBluetoothPara = tempReaderBluetoothPara;

		if(WriteReaderBluetoothConfig() == SUCCESS)
		{
			SetCbBlePara();
			return SUCCESS;
		}
		else
		{
			return FAIL;
		}
	}
	return SUCCESS;
}

//*****************************************************************************
//	AccessScheduleConfigUpdateHandler()
//	Param:
//		IN	: configIdx				- config list index
//			: tempConfigPtr			- updated config pointer
//		OUT	: NONE
//
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This API checks for any action required on updating ACCESS_SCHEDULE_t, acts on it,
//		saves configuration and returns SUCCESS/FAIL.
//*****************************************************************************
static BOOL AccessScheduleConfigUpdateHandler(UINT16 configIdx, VOIDPTR tempConfigPtr)
{
	//store data in configuration structure
	AccessScheduleConfig[configIdx] = *(ACCESS_SCHEDULE_t *)tempConfigPtr;

	//Write data to file.
	if(WriteAccessScheduleConfig() == SUCCESS)
	{
		return SUCCESS;
	}
	EPRINT(SYS_LOG,"AccessScheduleConfigUpdateHandler Fail");
	return FAIL;

}

