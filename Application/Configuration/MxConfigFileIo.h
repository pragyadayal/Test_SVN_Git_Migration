#if !defined MXCONFIGFILEIO_H
#define MXCONFIGFILEIO_H
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
//   Created By   : Priti Purohit
//   File         : MxConfigFileIo.h
//   Description  : This file consist of Prototypes of Configuration File I/O 
//					functions and defines.
//
/////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include<stdio.h>
#include "MxTypedef.h"
#include"MxConfig.h"

//******** Defines and Data Types ****

//FILE NAMES
//For single page File Name is MxXXX.cfg
//For Multiple pages File Name Format is MxXXXXPg%d.cfg
//For Multiple pages of holidaySchedule Format is MxXXXSch%d.cfg 
//For Multiple pages of cafeteria Item Config is MxCafeItemPg%d.cfg
//For Multiple pages of cafeteria Menu Schedule is MxCafeMenuSchPg%d.cfg

// Where all configurations are stored

#define	SYS_VARIABLE_FILE					SYS_CONFIG_PATH"MxSysVars.cfg"
#define	SYS_ADV_PARA_FILE					SYS_CONFIG_PATH"MxAdvSysPara.cfg"
#define	SYS_BASIC_PARA_FILE					SYS_CONFIG_PATH"MxBasicSysPara.cfg"
#define	SYS_BLOCKED_USER_FILE				SYS_CONFIG_PATH"MxBlockedUserPg"
#define	SYS_DAY_LIGHT_SAVING_FILE			SYS_CONFIG_PATH"MxDayLightSaving.cfg"
#define	SYS_FIRST_IN_USER_FILE				SYS_CONFIG_PATH"MxFirstInUser.cfg"
#define	SYS_HOLIDAY_SCH_FILE				SYS_CONFIG_PATH"MxHolidaySch.cfg"
#define	SYS_ETH_PARA_FILE					SYS_CONFIG_PATH"MxSysEthernetPara.cfg"
#define	V11R3_SYS_SERVER_PARA_FILE			SYS_CONFIG_PATH"MxSysServerPara.cfg"
#define	SYS_SERVER_PARA_FILE				SYS_CONFIG_PATH"MxNewSysServerPara.cfg"
#define	SYS_WIFI_PARA_FILE					SYS_CONFIG_PATH"MxSysWifiPara.cfg"
#define	SYS_MODEM_PARA_FILE					SYS_CONFIG_PATH"MxSysModemPara.cfg"
#define SYS_COMMAND_CTRL_FILE				SYS_CONFIG_PATH"MxSysCmdCtrl.cfg"
#define	SYS_TWO_PERSON_FILE					SYS_CONFIG_PATH"MxTwoPersonRule.cfg"
#define	SYS_USER_CNFG_FILE					SYS_CONFIG_PATH"MxUserPg"
#define	SYS_USER_1_CNFG_FILE				SYS_CONFIG_PATH"MxUserPg1.cfg"
#define	SYS_VISITOR_MNGMENT_FILE			SYS_CONFIG_PATH"MxVisitorMngmntPg"
#define	SYS_SPECIAL_FUNCTION_FILE			SYS_CONFIG_PATH"MxSpecialFunc.cfg"
#define	SYS_TIME_TRIG_FUNC_FILE				SYS_CONFIG_PATH"MxTimeTrigFunc.cfg"
#define	SYS_IO_LINKING_FILE					SYS_CONFIG_PATH"MxIoLinking.cfg"
#define	SYS_IO_LINK_EXT_DEV_FILE			SYS_CONFIG_PATH"MxIoLinkExtDev.cfg"
#define	SYS_IO_LINK_EXT_DEV_MAPPING_FILE	SYS_CONFIG_PATH"MxIoLinkExtDevMapping.cfg"
#define	SYS_FPR_CFNG_FILE					SYS_CONFIG_PATH"MxFprCnfg.cfg"
#define SYS_READER_PARA_CNFG_FILE			SYS_CONFIG_PATH"MxReaderParaCnfg.cfg"
#define SYS_CAFE_ITEM_CNFG_FILE				SYS_CONFIG_PATH"MxCafeItem.cfg"
#define	SYS_CAFE_MENU_CNFG_FILE				SYS_CONFIG_PATH"MxCafeMenu.cfg"
#define	SYS_MENU_SCHEDULE_CNFG_FILE			SYS_CONFIG_PATH"MxCafeMenuSch.cfg"
#define	SYS_CAFE_SCREEN_MIRROR_CNFG_FILE	SYS_CONFIG_PATH"MxCafeScreenMirror.cfg"
#define	SYS_PRINTER_CNFG_FILE				SYS_CONFIG_PATH"MxPrinter.cfg"
#define	SYS_DVR_EVT_ACTN_CNFG_FILE			SYS_CONFIG_PATH"MxDvrEvtActn.cfg"
#define SYS_SMART_ROUTE_ACCESS_CNFG_FILE	SYS_CONFIG_PATH"MxSmartRouteAccess.cfg"
#define	SYS_FIRST_IN_ACCS_DETAIL_FILE		SYS_CONFIG_PATH"MxFirstInAccsDetail.cfg"
#define	SYS_FILE_LOG_PARA_CNFG_FILE			SYS_CONFIG_PATH"MxFileLogPara.cfg"
#define SYS_WEB_API_EVENTS_CFG_FILE			SYS_CONFIG_PATH"MxSysWebApiEvents.cfg"
#define SYS_CARD_FORMAT_CFG_FILE			SYS_CONFIG_PATH"MxCardFmt.cfg"
#define	SYS_VAM_CNFG_FILE					SYS_CONFIG_PATH"MxVamPg"
#define SYS_JCM_CONFIG_FILE					SYS_CONFIG_PATH"MxJcm.cfg"
#define SYS_RDR_SWAP_CNFG_FILE				SYS_CONFIG_PATH"MxRdrModeSch.cfg"
#define SYS_SERVER_MODE_CNFG_FILE			SYS_CONFIG_PATH"MxServerMode.cfg"
#define SYS_VYOM_SERVER_CNFG_FILE			SYS_CONFIG_PATH"MxVYomServer.cfg"
#define SYS_BACKUP_AND_UPDATE_CNFG_FILE		SYS_CONFIG_PATH"MxBackUpAndUpdate.cfg"
#define SYS_CONENCT_TO_PANEL_CNFG_FILE		SYS_CONFIG_PATH"MxConnectToPanel.cfg"
#define SYS_LICENSE_DONGLE_CNFG_FILE		SYS_CONFIG_PATH"MxLicenseDongle.cfg"
#define SYS_HTTP_SERVER_CNFG_FILE			SYS_CONFIG_PATH"MxHttpServer.cfg"
#define SYS_IP_CAMERA_CNFG_FILE				SYS_CONFIG_PATH"MxIpCamera.cfg"
#define SYS_DISPLAY_BRIGHTNESS_CNFG_FILE	SYS_CONFIG_PATH"MxDisplayBrightness.cfg"
#define SYS_ACCESS_SCHEDULE_FILE			SYS_CONFIG_PATH"MxAccessSchedule.cfg"

//	Upgrading firmware signature
#define UPGRADING_FIRMWARE_SIGN				"/matrix/UpgradingFirmware"

//	temporary fp template dir
#define SYS_TMP_FP_DIR						"/matrix/applData/fp_"
// fp template dir
#define SYS_FP_TEMPLATE_DIR					"/matrix/applData/fpTemplate"
// fp image dir
#define SYS_FP_IMAGE_DIR					"/matrix/applData/fpImage"
#define SYS_TEMP_USER_DIR					"/tmp/TempUser"

// User File extention
#define SYS_CFG_FILE_EXT					".cfg"

// Finger template file
#define	SYS_USER_FP_FILE_PATH				"/matrix/applData/fpTemplate/UserFP"
// Finger image file
#define	SYS_USER_FP_IMG_FILE_PATH			SYS_FP_IMAGE_DIR"/UserFP"

// Script to copy user files
#define	COPY_USER_CFG_FILES_SCRIPT			"/matrix/scripts/copy_user_files.sh"

#define	SYS_EVENT_PICS_INFO_FILE			SYS_CONFIG_PATH"MxEventPicsInfo.cfg"

#define	USER_PER_PAGE					25
#define	BLOCKED_USER_PER_PAGE			25
#define	HOLIDAYS_PER_PAGE				32
#define	VISITORS_PER_PAGE				25
#define VAM_CONFIG_PER_PAGE				100

#define	TOTAL_BLOCKED_USER_PAGES		((MAX_BLOCKED_USER - 1) / (BLOCKED_USER_PER_PAGE))
#define	TOTAL_HOLIDAY_SCH				(MAX_HOLIDAY_SCHEDULE - 1)
#define	TOTAL_USER_PAGES				((MAX_USERS - MAX_SSA_USERS - 1) / (USER_PER_PAGE))
#define	TOTAL_VISITOR_PAGES				((MAX_VISITORS - 1) / (VISITORS_PER_PAGE))			
#define TOTAL_VAM_CONFIG_PAGES			((MAX_VAM_CONFIG - 1)/ (VAM_CONFIG_PER_PAGE))

// status of fp file write
typedef enum
{
	FILE_WRITE_FAIL = 0,
	FILE_WRITE_SUCCESS,
	FILE_WRITE_FAIL_NO_MEMORY,
	FILE_WRITE_FORMAT_CHECK_FAIL,
}FILE_WRITE_STATUS_e;

//******** Function Prototypes *******

void InitSysConfig(void);
void LoadDfltSysConfig(UINT8 sysDfltType);
void DfltConfForDoorModeChng(void);
void DfltAllConfig(void);

void InitSysVars(void);
BOOL ReadSysVars(void);
BOOL LoadDfltSysVars(void);
BOOL WriteSysVars(void);

void InitAdvParaConfig(void);
void LoadDfltEncryptionModeAndPortsConfig(void);
BOOL LoadDfltAdvParaExceptEncryptionModeAndPortsConfig(void);
BOOL LoadDfltPasswordPolicyF(void);
BOOL LoadDfltAdvParaConfig(void);
BOOL ReadAdvParaConfig(void);
BOOL WriteAdvParaConfig(void);

void InitBasicParaConfig(void);
BOOL LoadDfltBasicParaConfig(void);
BOOL LoadDfltBasicSysParaExceptWebServerConfig(void);
BOOL ReadBasicParaConfig(void);
BOOL WriteBasicParaConfig(void);

void RemoveDirectory(VOIDPTR dataPtr);
BOOL InsertRemoveDirectoryReq(char* fileName);

void InitBlockedUserConfig(void);
BOOL LoadDfltBlockedUserConfig(UINT8 pg);
BOOL ReadBlockedUserConfig(UINT8 pg);
BOOL WriteBlockedUserConfig(UINT8 pg);
BOOL WriteSingleBlockedUserConfig(UINT8 index);

void InitDSTConfig(void);
BOOL LoadDfltDSTConfig(void);
BOOL ReadDSTConfig(void);
BOOL WriteDSTConfig(void);

void InitFirstInConfig(void);
BOOL LoadDfltFirstInConfig(void);
BOOL ReadFirstInConfig(void);
BOOL WriteFirstInConfig(void);
BOOL WriteSingleFirstInConfig(UINT8 list);

void InitHolidaySchConfig(void);
BOOL LoadDfltHolidaySchConfig(void);
BOOL ReadHolidaySchConfig(void);
BOOL WriteHolidaySchConfig(UINT8 sch);
BOOL WriteSingleHolidaySchConfig(UINT8 index, UINT8 sch);

void InitEthParaConfig(void);
BOOL LoadDfltEthParaConfig(void);
BOOL ReadEthParaConfig(void);
BOOL WriteEthParaConfig(void);

void InitServerParaConfig(void);
BOOL LoadDfltServerParaConfig(void);
BOOL ReadServerParaConfig(void);
BOOL WriteServerParaConfig(void);

void InitWifiParaConfig(void);
BOOL LoadDfltWifiParaConfig(void);
BOOL ReadWifiParaConfig(void);
BOOL WriteWifiParaConfig(void);

void InitModemParaConfig(void);
BOOL LoadDfltModemParaConfig(void);
BOOL ReadModemParaConfig(void);
BOOL WriteModemParaConfig(void);

void InitCmdCtrlConfig(void);
BOOL LoadDfltCmdCtrlConfig(void);
BOOL ReadCmdCtrlConfig(void);
BOOL WriteCmdCtrlConfig(void);

void InitPswdConfig(void);
BOOL LoadDfltPswdConfig(void);
BOOL LoadDfltSAPswdConfig(void);
BOOL ReadPswdConfig(void);
BOOL WritePswdConfig(void);

void InitTwoPersonConfig(void);
BOOL LoadDfltTwoPersonConfig(void);
BOOL ReadTwoPersonConfig(void);
BOOL WriteTwoPersonConfig(void);
BOOL WriteSingleTwoPersonConfig(UINT8 index);

void InitVisitorMngmentConfig(void);
BOOL LoadDfltVisitorMngmentConfig(UINT16 pg);
BOOL ReadVisitorMngmentConfig(UINT16 pg,BOOL firstTimeBootF);
BOOL WriteVisitorMngmentConfig(UINT16 pg);
BOOL WriteSingleVisitorMngmentConfig(UINT16 index);

void InitSpecialFunctionsConfig(void);
BOOL LoadDfltSpecialFunctionsConfig(void);
BOOL ReadSpecialFunctionsConfig(void);
BOOL WriteSpecialFunctionsConfig(void);
BOOL WriteSingleSpecialFunctionsConfig(UINT8 index);

void InitTimeTrigFuncConfig(void);
BOOL ReadTimeTrigFuncConfig(void);
BOOL LoadDfltTimeTrigFuncConfig(void);
BOOL WriteTimeTrigFuncConfig(void);

void InitTimeStampConfig(void);
BOOL ReadTimeStampConfig(void);
BOOL LoadDfltTimeStampConfig(void);
BOOL WriteTimeStampConfig(void);

void InitGreetingMsgConfig(void);
BOOL ReadGreetingMsgConfig(void);
BOOL LoadDfltGreetingMsgConfig(void);
BOOL WriteGreetingMsgConfig(void);

void InitIoLinkConfig(void);
BOOL LoadDfltIoLinkConfig(void);
BOOL ReadIoLinkConfig(void);
BOOL WriteIoLinkConfig(void);
BOOL WriteSingleIoLinkConfig(UINT8 index);

void InitIoLinkExtDevConfig(void);
BOOL LoadDfltIoLinkExtDevConfig(void);
BOOL ReadIoLinkExtDevConfig(void);
BOOL WriteIoLinkExtDevConfig(void);
BOOL WriteSingleIoLinkExtDevConfig(UINT8 index);

void InitIoLinkExtDevMapping(void);
BOOL LoadDfltIoLinkExtDevMapping(void);
BOOL ReadIoLinkExtDevMapping(void);
BOOL WriteIoLinkExtDevMapping(void);
BOOL WriteSingleIoLinkExtDevMapping(UINT8 index);

void InitFprConfig(void);
BOOL ReadFprConfig(void);
BOOL LoadDfltFprConfig(void);
BOOL WriteFprConfig(void);

void InitReadersParaConfig(void);
BOOL ReadReadersParaConfig(void);
BOOL LoadDfltReadersParaConfig(void);
BOOL WriteReadersParaConfig(void);

void InitCafeItemConfig(void);
BOOL LoadDfltCafeItemConfig(void);
BOOL ReadCafeItemConfig(void);
BOOL WriteCafeItemConfig(void);

void InitCafeMenuConfig(void);
BOOL LoadDfltCafeMenuConfig(void);
BOOL ReadCafeMenuConfig(void);
BOOL WriteCafeMenuConfig(UINT16 menuIdx);

void InitCafeMenuScheduleConfig(void);
BOOL LoadDfltCafeMenuScheduleConfig(void);
BOOL ReadCafeMenuScheduleConfig(void);
BOOL WriteCafeMenuScheduleConfig(void);

void InitPrinterConfig(void);
BOOL LoadDfltPrinterConfig(void);
BOOL ReadPrinterConfig(void);
BOOL WritePrinterConfig(void);

FILE_WRITE_STATUS_e WriteUserFpIntoFile(UINT32 userIdx, UINT8 fpIdx, VOIDPTR dataPtr);
BOOL IsUserFpPresent(UINT32 userIdx, UINT8 fpIdx);
BOOL ReadUserFpFromFile(UINT32 userIdx, UINT8 fpIdx, VOIDPTR dataPtr);
void DeleteSingleUserFpFiles(UINT32 userIdx, UINT8 fpIdx);
void DeleteAllUsersFpFiles(void);
void DeleteAllUsersFpImageFiles(void);
void DeleteAllUsersPhotoFiles(void);
BOOL WritePhotoDelSignFile();
UINT8 GetEnrolledUserFp(UINT32 userIdx);
FILE_WRITE_STATUS_e WriteTempUserFpIntoFile(VOIDPTR dataPtr);

BOOL WriteSingleEventPicsInfo(void);

void InitDvrEvtActnConfig(void);
BOOL ReadDvrEvtActnConfig(void);
void LoadDfltDvrEvtActnConfig(void);
BOOL WriteDvrEvtActnConfig(void);

void InitSmartRouteAccessConfig(void);
BOOL ReadSmartRouteAccessConfig(void);
BOOL LoadDfltSmartRouteAccessConfig(void);
BOOL WriteSmartRouteAccessConfig(void);

void InitFirstInAccessInfo(void);
BOOL WriteFirstInAccessInfo(void);
BOOL ReadFirstInAccessInfo(void);
BOOL LoadDfltFirstInAccessInfo(void);

void InitFileLogParaConfig(void);
BOOL ReadFileLogParaConfig(void);

void InitWebAPIEventsConfig(void);
BOOL LoadDfltWebAPIEventsConfig(void);
BOOL ReadWebAPIEventsConfig(void);
BOOL WriteWebAPIEventsConfig(void);

void DeleteSingleUserPhoto(UINT32 userId);
void DeleteAllUserPhoto(void);

void InitCardFormatConfig(void);
BOOL ReadCardFormatConfig(void);
BOOL LoadDfltCardFormatConfig(void);
BOOL WriteCardFormatConfig(void);

void InitVamConfig(void);
BOOL LoadDfltVamConfig(UINT16 pg);
BOOL LoadDfltSingleVamConfig(UINT16 userIdx);
BOOL ReadVamConfig(UINT16 pg,BOOL firstTimeBootF);
BOOL WriteVamConfig(UINT16 pg);
BOOL WriteSingleVamConfig(UINT16 index);

void InitJcmConfig(void);
BOOL LoadDfltJcmConfig(void);
BOOL ReadJcmConfig(void);
BOOL WriteJcmConfig(void);
BOOL WriteSingleJcmConfig(UINT16 index);

void InitReaderSchedulConfig(void);
BOOL ReadReaderSchedulConfig(void);
BOOL LoadDfltReaderSchedulConfig(void);
BOOL WriteReaderSchedulConfig(void);
BOOL WriteSingleReaderSchedulConfig(UINT16 index);

void InitServerModeConfig(void);
BOOL ReadServerModeConfig(void);
BOOL LoadDfltServerModeConfig(void);
BOOL WriteServerModeConfig(void);

void InitVyomServerConfig(void);
BOOL ReadVyomServerConfig(void);
BOOL LoadDfltVyomServerConfig(void);
BOOL WriteVyomServerConfig(void);

void InitV11R3ServerParaConfig(void);
BOOL LoadDfltV11R3ServerParaConfig(void);
BOOL ReadV11R3ServerParaConfig(void);
BOOL WriteV11R3ServerParaConfig(void);

void InitBackUpAndUpdateConfig(void);
BOOL ReadBackUpAndUpdateConfig(void);
BOOL LoadDfltBackUpAndUpdateConfig(void);
BOOL WriteBackUpAndUpdateConfig(void);

void InitConnectToPanelConfig(void);
BOOL ReadConnectToPanelConfig(void);
BOOL LoadDfltConnectToPanelConfig(void);
BOOL WriteConnectToPanelConfig(void);

void SendDfltConfigEvent(CONFIGURATION_TABLE_e tableId);
void InitUserInOutStatus(void);
UINT32 GetDoorFpCount(void);

void InitLicenseDongleConfig(void);
BOOL ReadLicenseDongleConfig(void);
BOOL LoadDfltLicenseDongleConfig(void);
BOOL WriteLicenseDongleConfig(void);
     
void InitIpCameraConfig(void);
BOOL ReadIpCameraConfig(void);
BOOL LoadDfltIpCameraConfig(void);
BOOL WriteIpCameraConfig(void);

void InitDisplayBrightnessConfig(void);
BOOL ReadDisplayBrightnessConfig(void);
BOOL LoadDfltDisplayBrightnessConfig(void);
BOOL WriteDisplayBrightnessConfig(void);

void GetSysEnrolledFpCnt(void);
VOID IncreaseDoorFpCount(void);

void InitAccessScheduleConfig(void);
BOOL ReadAccessScheduleConfig(void);
BOOL WriteAccessScheduleConfig(void);
BOOL LoadDfltAccessScheduleConfig(void);

#endif  // MXCONFIGFILEIO_H
