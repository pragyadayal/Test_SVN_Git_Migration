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
//   File         : MxConfigFileIo.c
//   Description  : This file consist of Configuration File I/O related Functions
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>

#include "MxDeviceSpecific.h"
#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxConfigLog.h"
#include "MxConfigOld.h"
#include "MxConfigFileIo.h"
#include "MxOldConfigFileIo.h"
#include "MxLogs.h"
#include "MxBlockDevice.h"
#include "MxEthernetPort.h"
#include "MxMobileBroadband.h"
#include "MxEventLog.h"
#include "MxSysUtils.h"
#include "MxConfigUpdateHandler.h"
#include "MxWifiPort.h"
#include "MxNetworkUtils.h"
#include "MxHttpClient.h"
#include "MxSmartCardFormatConfig.h"
#include "MxFpIdtServer.h"
#include "MxSysCommonUtils.h"
#include "MxSysParaConfig.h"
#include "MxWiegandOutConfig.h"
#include "MxIdtServerBioSyncConfig.h"
#include "MxMifareSectorWiseKeyConfig.h"
#include "MxAttendanceDetailConfig.h"
#include "MxIpCamera.h"
#include "MxFaceRecognitionConfig.h"
#include "MxAutoSwitchWifiConfig.h"
#include "MxBluetoothConfig.h"
#include "MxSplFnScheduleConfig.h"
#include "MxReaderBluetoothConfig.h"
#include "MxDeviceSpecific.h"
#ifdef PUSH_API_SUPPORTED
#include "MxPushApiConfig.h"
#include "MxPushApiEvent.h"
#endif
#ifdef OEM_SUPPORTED
#include "OEM.h"
#endif //#ifdef OEM_SUPPORTED
#include "MxUserConfig.h"

#ifdef READER_WIFI_SUPPORTED
#include "MxReaderWifiConfig.h"
#endif //#ifdef READER_WIFI_SUPPORTED
#include "MxTimeTrigFunc.h"
#include "HttpReqFSM.h"

//******** Extern Variables **********

extern SYS_VAR_t						SysVar;
extern const SYS_VAR_t					DefltSysVar;

extern const SYS_ADVANCE_PARA_t			DfltSysAdvPara;
extern SYS_ADVANCE_PARA_t				SysAdvPara;

extern const SYS_BASIC_PARA_t	  		DfltBasicSysPara;
extern SYS_BASIC_PARA_t					SysBasicPara;

extern BLOCKED_USER_GROUP_t  			BlockedUser[MAX_BLOCKED_USER];
extern const BLOCKED_USER_GROUP_t 		DfltBlockedUser;

extern const DST_CONFIG_t 				DfltDstConfig;
extern DST_CONFIG_t						DstConfig;

extern const FIRST_IN_LIST_t			DfltFirstInUser;
extern FIRST_IN_LIST_t       			FirstInUserGroup[MAX_FIRST_IN_LISTS];

extern HOLIDAY_SCHEDULE_t    			HolidaySchedule[MAX_HOLIDAY_SCHEDULE];
extern const HOLIDAY_t					DfltHoliday;

extern const ETHERNET_PARA_t  			DfltEthPara;
extern ETHERNET_PARA_t					EthernetParameters;

extern const SERVER_PARA_t  			DfltServerParaCentra;
extern const SERVER_PARA_t  			DfltServerParaVyom;
extern SERVER_PARA_t					ServerParameters;

extern const WIFI_PARA_t				DfltWifiPara;
extern WIFI_PARA_t						WifiParameters;

extern MODEM_CONFIG_t					ModemProfile[MAX_MODEM_PROFILE];
extern const MODEM_CONFIG_t				DfltModemProfile[MAX_MODEM_PROFILE];
extern UINT8							SelectedModemProfile;

extern COMMAND_CTRL_CONFIG_t			CmdCtrlConfig;
extern const COMMAND_CTRL_CONFIG_t 		DfltCmdCtrlConfig;

extern const PASSWORD_t 				DfltPassword;
extern PASSWORD_t						Password;

extern TWO_PERSON_GROUP_t   			TwoPersonGroup[MAX_TWO_PERSON_GROUP];
extern const TWO_PERSON_GROUP_t 		DfltTwoPerson;

extern const  USER_CONFIG_t 			DfltUserConfig;

extern VISITOR_MANAGEMENT_t  			Visitor[MAX_VISITORS];
extern const VISITOR_MANAGEMENT_t		DfltVisitor;

extern SPECIAL_FUNCTION_t 				SpecialFunctions[MAX_SPECIAL_FUNC];
extern const SPECIAL_FUNCTION_t 		DfltSpecialFunc;

extern TIME_TRIG_FUNC_t					TimeTrigFunc[MAX_TIME_TRIG_FUNC];
extern const TIME_TRIG_FUNC_t			DfltTimeTrigFunc;

extern FPR_CONFIG_t						FprConfig;
extern const FPR_CONFIG_t				DfltFprConfig;

extern READERS_PARAMETER_t				ReadersParaConfig;
extern const READERS_PARAMETER_t		DfltReadersParaConfig;

extern IO_LINK_CONFIG_t					IoLinkConfig[MAX_IO_LINK];
extern const IO_LINK_CONFIG_t			DefltIoLinkConfig;

extern IO_LINK_EXT_DEV_CONFIG_t			IoLinkExtDevConfig[MAX_IO_LINK_EXT_DEVICE];
extern const IO_LINK_EXT_DEV_CONFIG_t	DefltIoLinkExtDevConfig;

extern IO_LINK_EXT_DEV_MAPPING_t		IoLinkExtDevMapping[MAX_IO_LINK];
extern const IO_LINK_EXT_DEV_MAPPING_t	DefltIoLinkExtDevMapping;

extern CAFE_ITEM_CONFIG_t				CafeItemConfig[MAX_CAFE_ITEM_CODE];
extern const CAFE_ITEM_CONFIG_t			DefltCafeItemConfig;

extern CAFE_MENU_CONFIG_t				CafeMenuConfig[MAX_CAFE_MENU_CODE];
extern const CAFE_MENU_CONFIG_t			DefltCafeMenuConfig;

extern CAFE_MENU_SCHEDULE_t				CafeMenuSchConfig[MAX_CAFE_MENU_SCHEDULE];
extern const CAFE_MENU_SCHEDULE_t		DefltCafeMenuSchConfig;

extern PRINTER_CONFIG_t					PrinterConfig;
extern const PRINTER_CONFIG_t			DefltPrinterConfig;

extern DVR_EVENT_ACTION_CONFIG_t		DvrEvtActionConfig[MAX_DVR_EVT_ACTN_CONFIG];
extern const DVR_EVENT_ACTION_CONFIG_t	DfltDvrEvtActionConfig;

extern MMC_INFO_t 						MmcInfo;

extern SMART_ROUTE_ACCESS_CONFIG_t		SmartRouteAccessConfig[MAX_SMART_ROUTE_ACCESS];
extern const SMART_ROUTE_ACCESS_CONFIG_t DfltSmartRouteAccessConfig;

extern FIRST_IN_INFO_t					FirstInInfo;
extern const FIRST_IN_INFO_t			DfltFristInInfo;

extern FILE_LOG_PARA_CONFIG_t			FileLogPara;
extern const FILE_LOG_PARA_CONFIG_t		DfltFileLogPara;

extern WEB_API_EVENTS_CONFIG_t			WebAPIEventsConfig;
extern const WEB_API_EVENTS_CONFIG_t	DfltWebAPIEventsConfig;

extern CARD_FORMAT_CONFIG_t				CardFormatConf[MAX_CARD_FORMAT_PER_DOOR];
extern const CARD_FORMAT_CONFIG_t		DfltCardFormatConf;
extern const CARD_FORMAT_CONFIG_t		BlankCardFormatConf;

extern VAM_CONFIG_t						VamConfig[MAX_VAM_CONFIG];
extern VAM_CONFIG_t						DfltVamConfig;

extern JCM_CONFIG_t						JcmConfig[MAX_JCM_CONFIG+3]; //+3 for 'None', 'Default' and 'Continue' Job
extern const JCM_CONFIG_t				DfltJcmConfig;
extern const JCM_CONFIG_t				NoneJobConfig;
extern const JCM_CONFIG_t				DefaultJobConfig;
extern const JCM_CONFIG_t				ContinueJobConfig;

extern DOOR_MODE_SCH_CONFIG_t			DoorModeSchCfg[MAX_DOOR_MODE_SCHEDULE];
extern DOOR_MODE_SCH_CONFIG_t 			DfltDoorModeSchedul;

extern SERVER_MODE_CONFIG_t				ServerModeCfg;
extern SERVER_MODE_CONFIG_t 			DfltServerMode;
extern VYOM_SERVER_CONFIG_t				VyomServerPara;
extern VYOM_SERVER_CONFIG_t 			DfltVyomServerPara;
extern CHAR								TenentIdStr[MAX_LENGTH_TEN_ID_STR];

extern BACKUP_AND_UPDATE_t				BackUpAndUpdate;
extern const BACKUP_AND_UPDATE_t		DfltBackUpAndUpdateParaForCentraMode;
extern const BACKUP_AND_UPDATE_t		DfltBackUpAndUpdateParaForVyomMode;

extern UINT8							UserInOutStatus[USER_IN_OUT_STATUS_SIZE];

extern V11R3_SERVER_PARA_t		  		V11R3ServerParameters;
extern const V11R3_SERVER_PARA_t  		DfltV11R3ServerPara;

extern CONNECT_TO_PANEL_CONFIG_t 		DfltConnectToPanelPara;
extern CONNECT_TO_PANEL_CONFIG_t		ConnectToPanelPara;

extern LICENSE_DONGLE_CONFIG_t			LicenseDongleCfg;
extern const LICENSE_DONGLE_CONFIG_t 	DfltLicenseDonglePara;

extern IP_CAMERA_CONFIG_t			IpCameraConfig;
extern const IP_CAMERA_CONFIG_t		DfltIpCameraConfig;

extern BRIGHTNESS_DETAIL_t			DisplayBrightness;
extern const BRIGHTNESS_DETAIL_t    DfltDisplayBrightness;

extern EVENT_PICS_INFO_t				EventPicsInfo;
extern pthread_mutex_t 				MutexLockAutoSwitchWiFi;

extern UINT16						UserInitActiVeF;

extern const ACCESS_SCHEDULE_t			DfltAccessScheduleConfig;
extern ACCESS_SCHEDULE_t			AccessScheduleConfig[MAX_TIME_FRAME];

#ifdef OEM_SUPPORTED
extern OEM_INFO_t gOEMCurrDeviceInfo;
#endif //#ifdef OEM_SUPPORTED

//******** Defines and Data Types ****

// Total storage of finger templates in the system
#define TOTAL_SYS_FP_CAPACITY			100000

//******** Function Prototypes *******
static BOOL IsSysConfigFirstTime(void);

static VOID LoadDfltAllBlockedUserConfig(VOID);
static VOID LoadDfltAllVisitorMngmentConfig(VOID);
static void LoadDfltAllVamConfig(void);

static void GetFpTemplateFilePath(CHARPTR filePath, UINT32 userIdx, UINT8 fpIdx);

static BOOL WriteNewServerParaConfig(void);
static BOOL LoadDfltNewServerParaConfig(void);

static void DefaultEventPicsInfo(void);
static BOOL WriteEventPicsInfo(void);
static BOOL ReadEventPicsInfo(void);
static void InitNetworkEthWifiConfigAndPort();
static void DfltNetworkEthWifiConfigAndPort();

extern BOOL IsConfigFilePresent(CHARPTR fileNameInitial);

extern void StartUserConfigThreadFunc(void);
extern VOIDPTR UserConfigThread(VOIDPTR dummy);
static void StartVisitormngmentConfigThreadFunc(void);
static VOIDPTR VisitormngmentConfigThread(VOIDPTR dummy);
extern THREAD_STATUS_e GetUserConfigThreadSatus(void);
extern void LoadDefaultAllUserConfig(void);
void LoadDefaultAllVisitomngmentConfig(void);
//******** Global Variables **********
SERVER_PARA_t					TempServerParaVyom;

//******** Static Variables **********
static UINT32	sysFpEnrolledCnt;				// No of Finger Has been enrolled in panel
static BOOL		sysBeingDfltF = FALSE;
extern pthread_t			userConfigThreadData;
static pthread_t            visitormngmentConfigThreadData = 0;
extern THREAD_STATUS_e		userConfigThreadSatus;
static THREAD_STATUS_e      visitormngmentConfigThreadSatus = THREAD_INACTIVE;
//******** Function Definations ******
void InitNetworkEthWifiConfigAndPort()
{
    InitEthParaConfig();
    InitWifiParaConfig();
    
    if(WifiParameters.enableWifi == FALSE)
    {
        StartWifiEnableDisableThreadFunc(WifiParameters.enableWifi);
    }

    // Init Ethernet port
    InitEthernetPort();
    // Init Wifi Port
    InitWifiPort();
}



//*****************************************************************************
//	GetVisitorConfigThreadSatus()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			User config thread status
//	Description:
//			Function will return user config thread status.
//*****************************************************************************
THREAD_STATUS_e GetVisitorConfigThreadSatus(void)
{
    return visitormngmentConfigThreadSatus;
}
//*****************************************************************************
//	StartVisitormngmentConfigThreadFunc()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Start visitor mngment config thread function.
//*****************************************************************************
void StartVisitormngmentConfigThreadFunc(void)
{
    if(pthread_create(&visitormngmentConfigThreadData, NULL, VisitormngmentConfigThread, NULL) == 0)
    {
        visitormngmentConfigThreadSatus = THREAD_ACTIVE;
//        pthread_detach(visitormngmentConfigThreadData);
    }
}

//*****************************************************************************
//	VisitormngmentConfigThread()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Creating visitor mngment config and Vam file using thread for reduce
//          boot up time.
//*****************************************************************************
VOIDPTR VisitormngmentConfigThread(VOIDPTR dummy)
{
    InitVisitorMngmentConfig();
    InitVamConfig();
    visitormngmentConfigThreadSatus = THREAD_DONE;
    return NULL;
}

void LoadDefaultAllVisitomngmentConfig(void)
{
    CHAR 	buf[100];
    int status = -1;
    void* res;
    if(visitormngmentConfigThreadSatus == THREAD_ACTIVE){
        status = pthread_cancel(visitormngmentConfigThreadData);
        if(status != 0)
        {
            EPRINT(SYS_LOG,"Visitor config thread not canceled");
            return;
        }
        DPRINT(SYS_LOG,"Visitor config thread canceled successfully");
        status = pthread_join(visitormngmentConfigThreadData, &res);
        if(status != 0)
        {
            EPRINT(SYS_LOG,"Visitor config thread not able to join");
            return;
        }
        DPRINT(SYS_LOG,"Visitor config thread joined successfully");
        visitormngmentConfigThreadSatus = THREAD_INACTIVE;
    }

    //delete visitor config
    sprintf(buf,"rm -rf %sMxVisitorMngmntPg*",SYS_CONFIG_PATH);
    ExeCmd(buf);

    sprintf(buf,"rm -rf %sMxVamPg*",SYS_CONFIG_PATH);
    ExeCmd(buf);

    //call our thread function
    DPRINT(SYS_LOG,"Configuration defaulted - Visitor Config");
    DPRINT(SYS_LOG,"Configuration defaulted - Vam Config");

    StartVisitormngmentConfigThreadFunc();
}
//*****************************************************************************
//	InitSysConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initializes system configuration.
//*****************************************************************************
void InitSysConfig(void)
{
	char buf[MAX_SYS_CMD_BUF_SZ];
	struct 	stat	stBuf;

	CHAR filePath[MAX_FILENAME_LENGTH] = {'\0'};
	
	sprintf(buf, "rm -rf %s* &",SYS_TMP_FP_DIR);
	system(buf);

	sprintf(buf, "rm -rf %s* &",SYS_USER_ATTNDNS_TMP_FOLDR);
	system(buf);

	snprintf(filePath, MAX_FILENAME_LENGTH, "%s/%s",MmcInfo.devPath,DLT_USER_PICS_FOLDER);
	if(IsDirExist(filePath))
	{
		InsertRemoveDirectoryReq(filePath);
	}

	if(stat(SYS_ATTENDACE_DETAIL_PATH,&stBuf) != 0)
	{
		sprintf(buf, "mkdir -p %s", SYS_ATTENDACE_DETAIL_PATH);
		system(buf);
	}
	if(stat(SYS_PRVS_ATTENDACE_DETAIL_PATH,&stBuf) != 0)
	{
		sprintf(buf, "mkdir -p %s", SYS_PRVS_ATTENDACE_DETAIL_PATH);
		system(buf);
	}
	if(stat(SYS_FP_IMAGE_DIR,&stBuf) != 0)
	{
		if(mkdir(SYS_FP_IMAGE_DIR,0777) != 0)
		{
			EPRINT(SYS_LOG,"Failed to create FPImage directory");
		}
	}
	
	if(stat(SYS_TEMP_USER_DIR,&stBuf) != 0)
	{
		if(mkdir(SYS_TEMP_USER_DIR,0777) != 0)
		{
			EPRINT(SYS_LOG,"Failed to create TempUserfpTemplate directory");
		}
	}

	InitHttpReqFSM();
	if(IsSysConfigFirstTime() == YES)
	{
		DfltAllConfig();
	}
	else
	{
        InitNetworkEthWifiConfigAndPort();
		
		InitAdvParaConfig();
		InitBasicParaConfig();
		InitBlockedUserConfig();
		InitDSTConfig();
		InitFirstInConfig();
		InitHolidaySchConfig();
		InitServerParaConfig();
		InitConnectToPanelConfig();
		InitPswdConfig();
		InitWebUserPswdHistory();
		InitTwoPersonConfig();
		//InitUserConfig();
		StartUserConfigThreadFunc();
		//InitVisitorMngmentConfig();
		StartVisitormngmentConfigThreadFunc();
		InitSpecialFunctionsConfig();
		InitTimeTrigFuncConfig();
		InitFprConfig();
		InitReadersParaConfig();
		InitIoLinkConfig();
		InitIoLinkExtDevConfig();
		InitIoLinkExtDevMapping();
		InitModemParaConfig();
		InitCmdCtrlConfig();
		InitCafeItemConfig();
		InitCafeMenuConfig();
		InitCafeMenuScheduleConfig();
		InitPrinterConfig();
		InitDvrEvtActnConfig();
		InitSmartRouteAccessConfig();
		InitFirstInAccessInfo();
		InitCardFormatConfig();
//		InitVamConfig();
		InitJcmConfig();
		// Note:- InitBioIdtServerConfig must be called after InitAdvParaConfig and InitServerParaConfig, InitConnectToPanelConfig.
		InitBioIdtServerConfig();
		InitReaderSchedulConfig();
		InitMultiLangConfig();
		InitMultiLangString();
		InitCustomSmartCardConfig();
		InitSmartCardFormat();
		InitSysParaConfig();
		InitWiegnadOutCstmFrmtConfig();
		InitIdtServerBioSyncConfig();
		InitWebAPIEventsConfig();
		InitMifareSectorWiseKeyCnfg();
		InitBackUpAndUpdateConfig();
		InitUserInOutStatus();
		InitLicenseDongleConfig();
		InitBluetoothConfig();
		InitDisplayBrightnessConfig();
		InitSavedWifiParaConfig();
		InitFRServerConfig();
		InitFRCameraConfig();
		InitFRGeneralConfig();
		InitIpCameraConfig();
		ReadEventPicsInfo();
		InitSplFnScheduleConfig();
		InitReaderBluetoothConfig();
#ifdef PUSH_API_SUPPORTED
		InitPushApiConfig();
		InitPushApiEventConfig();
#endif //PUSH_API_SUPPORTED

#ifdef READER_WIFI_SUPPORTED
		InitReaderWifiConfig();
#endif //#ifdef READER_WIFI_SUPPORTED
		InitAccessScheduleConfig();
	}
}

void DfltNetworkEthWifiConfigAndPort(void)
{
    LoadDfltEthParaConfig();
    LoadDfltWifiParaConfig();

    if(WifiParameters.enableWifi == FALSE)
    {
        StartWifiEnableDisableThreadFunc(WifiParameters.enableWifi);
    }
    
    // Init Ethernet port
    InitEthernetPort();
    // Init Wifi Port
    InitWifiPort();
}

//*****************************************************************************
//	DfltAllConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			 It will default all the configuration in system.
//*****************************************************************************
void DfltAllConfig(void)
{
	DfltNetworkEthWifiConfigAndPort();
	LoadDfltEncryptionModeAndPortsConfig();
	LoadDfltSysConfig(SYS_DFLT_ALL);
	LoadDfltPswdConfig();
	LoadDfltWebUserPswdHistory();
	LoadDfltServerModeConfig();
	LoadDfltServerParaConfig();
	LoadDfltVyomServerConfig();
	LoadDfltCmdCtrlConfig();
	LoadDfltModemParaConfig();
	LoadDfltReadersParaConfig();
	LoadDfltSysParaCtrlConfig();
	LoadDfltWiegnadOutCstmFrmtConfig();
	LoadDfltConnectToPanelConfig();
	LoadDfltSavedWifiParaConfig();
	LoadDfltBluetoothConfig();
	LoadDfltDisplayBrightnessConfig();
	LoadDfltReaderBluetoothConfig();
}

//*****************************************************************************
//	LoadDfltSysConfig()
//	Param:
//		IN : sysDfltType
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			 Load entire system	default configuration.
//
// NOTE: 	 Network configuration and Password configuration will not be
//			 defaulted.
//*****************************************************************************
void LoadDfltSysConfig(UINT8 sysDfltType)
{
	sysBeingDfltF = TRUE;

	LoadDfltSysVars();
	LoadDfltAdvParaExceptEncryptionModeAndPortsConfig();

	if(sysDfltType == SYS_DFLT_BASIC)
    {
        LoadDfltBasicSysParaExceptWebServerConfig();
    }
    else
    {
        LoadDfltBasicParaConfig();
    }
	LoadDfltAllBlockedUserConfig();

	LoadDfltDSTConfig();
	LoadDfltFirstInConfig();
	LoadDfltHolidaySchConfig();

	LoadDfltIoLinkConfig();
	LoadDfltIoLinkExtDevConfig();
	LoadDfltIoLinkExtDevMapping();

	LoadDfltSpecialFunctionsConfig();
	LoadDfltTwoPersonConfig();

	// Default configuration of all users
	LoadDfltAllUsersConfig();
	DeleteAllUsersAttndnsDetail();

	// This will remove all FP templates of all users.
	DeleteAllUsersFpFiles();
	DeleteAllUsersFpImageFiles();
	WritePhotoDelSignFile();

	LoadDfltAllVisitorMngmentConfig();

	LoadDfltTimeTrigFuncConfig();
	LoadDfltCafeItemConfig();
	LoadDfltCafeMenuConfig();
	LoadDfltCafeMenuScheduleConfig();
	LoadDfltPrinterConfig();

	LoadDfltFprConfig();

	LoadDfltDvrEvtActnConfig();
	LoadDfltSmartRouteAccessConfig();
	LoadDfltFirstInAccessInfo();
	LoadDfltSysLogConfig();

	LoadDfltCardFormatConfig();

	LoadDfltAllVamConfig();
	LoadDfltJcmConfig();
	LoadDfltReaderSchedulConfig();
	LoadDfltMultiLanguageConfig();

	LoadDfltSmartCardFormat();
	LoadDfltCustomSmartCardConfig();

	LoadDfltWiegnadOutCstmFrmtConfig();
	LoadDfltBioIdtServerConfig();
	LoadDfltIdtServerBioSyncConfig();
	LoadDfltWebAPIEventsConfig();
	LoadDfltMifareSectorWiseKeyCnfg();
	LoadDfltBackUpAndUpdateConfig();
	InitUserInOutStatus();
	LoadDfltIpCameraConfig();
	LoadDfltFRServerConfig();
	LoadDfltFRCameraConfig();
	LoadDfltFRGeneralConfig();
	DefaultEventPicsInfo();
	LoadDfltSplFnScheduleConfig();
#ifdef PUSH_API_SUPPORTED
	LoadDfltPushApiEvent();
#endif //PUSH_API_SUPPORTED

	LoadDefaultTimeTrigFuncManuallyResetStatus();

	sysBeingDfltF = FALSE;

#ifdef READER_WIFI_SUPPORTED
	LoadDfltReaderWifiConfig();
#endif //#ifdef READER_WIFI_SUPPORTED
	LoadDfltSysParaCtrlConfig();
	LoadDfltAccessScheduleConfig();
}

//*****************************************************************************
//	DfltConfForDoorModeChng
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It defaults some config which require to default during door mode
//			change.
//*****************************************************************************
void DfltConfForDoorModeChng(void)
{
	LoadDfltServerModeConfig();
	LoadDfltServerParaConfig();
	LoadDfltVyomServerConfig();
	LoadDfltConnectToPanelConfig();
#ifdef PUSH_API_SUPPORTED
		LoadDfltPushApiConfig();
#endif //PUSH_API_SUPPORTED
	LoadDfltBluetoothConfig();
	LoadDfltReaderBluetoothConfig();
	LoadDfltPasswordPolicyF();
}

//*****************************************************************************
//	InitSysVars()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initializes system variables.
//*****************************************************************************
void InitSysVars(void)
{
	if(SUCCESS != ReadSysVars())
	{
		if(SUCCESS != LoadDfltSysVars())
		{
			EPRINT(SYS_LOG, "Failed to initialize system variables");
		}
	}
}

//*****************************************************************************
//	ReadSysVars()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads system variables stored in file.
//*****************************************************************************
BOOL ReadSysVars(void)
{
	INT32 	fd;
	UINT8	ver;
	BOOL status;

	fd = open(SYS_VARIABLE_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open system variable file");
		return FAIL;
	}

	// File opened successfully
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to read system variable file");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != SYS_VAR_VER)
	{
		EPRINT(SYS_LOG, "system variable version doesn't match");
		status = LoadOldSysVarInfo(fd, ver);
		close(fd);
		return status;
	}

	if((sizeof(SysVar)) != read(fd, &SysVar, sizeof(SysVar)))
	{
		EPRINT(SYS_LOG, "Failed to read system variable file");
		close(fd);
		return FAIL;
	}

	//File read successfully
	close(fd);
	SystemVariableConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltSysVars()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default system variable's values.
//*****************************************************************************
BOOL LoadDfltSysVars(void)
{
	SysVar = DefltSysVar;
	DPRINT(SYS_LOG,"Configuration defaulted - SysVar");
	if(WriteSysVars() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "SysVars");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteSysVars()
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes system variables into file.
//*********************************************************************************
BOOL WriteSysVars(void)
{
	INT32 	fd;
	UINT8	ver;

	fd = open(SYS_VARIABLE_FILE, O_RDWR | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open system variable file");
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = SYS_VAR_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to write system variable version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if((sizeof(SysVar)) != write(fd,&SysVar,(sizeof(SysVar))))
	{
		EPRINT(SYS_LOG, "Failed to write system variables into file");
		close(fd);
		return FAIL;
	}

	//File written successfully
	close(fd);
	SystemVariableConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitAdvParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Advance parameters
//*****************************************************************************
void InitAdvParaConfig(void)
{
	if(SUCCESS != ReadAdvParaConfig())
	{
		if(SUCCESS != LoadDfltAdvParaConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise System's Advance Parameters");
		}
	}
}

//*****************************************************************************
//	ReadAdvParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Advance system parameter from file
//*****************************************************************************
BOOL ReadAdvParaConfig(void)
{
	INT32 		fd;
	UINT8		ver;
	BOOL		status = FAIL;

	//	Open System Advance Parameter File
	if( (fd = open(SYS_ADV_PARA_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Advance System parameter");
		return FAIL;
	}

	// Read version number from the file
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Advance System parameter version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != ADVANCE_SYS_PARA_VER)
	{
		EPRINT(CONFIG_LOG,"Advance System parameter version mismatch : Saving Old data in New configuration");
		status = LoadOldAdvancePara(fd, ver);
		close(fd);
		return status;
	}

	if(sizeof(SysAdvPara) != read(fd,&SysAdvPara,sizeof(SysAdvPara)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Advance System parameter");
		close(fd);
		return FAIL;
	}

    if(GetDoorApplType() == PANEL_DOOR_APPL)
    {
        SysAdvPara.authenticationType = BASIC_AUTHENTICATION;
    }

	close(fd);
	SystemAdvanceParaConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltEncryptionModeAndPortsConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns: void
//	Description:
//			Load Dflt Encryption Mode And Ports Config
//*****************************************************************************
void LoadDfltEncryptionModeAndPortsConfig(void)
{
	SysAdvPara.passwordPolicyF = DfltSysAdvPara.passwordPolicyF;
	SysAdvPara.webEncryptionModeF = DfltSysAdvPara.webEncryptionModeF;
	SysAdvPara.httpPort = DfltSysAdvPara.httpPort;
	SysAdvPara.httpsPort = DfltSysAdvPara.httpsPort;
}

//*****************************************************************************
//	LoadDfltAdvParaExceptEncryptionModeAndPortsConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Load Dflt Adv Para Except Encryption Mode And Ports Config
//*****************************************************************************
BOOL LoadDfltAdvParaExceptEncryptionModeAndPortsConfig(void)
{
	BOOL	tWebEncryptionModeF, tPasswordPolicyF;
	UINT16	tHttpPort;
	UINT16	tHttpsPort;

	tPasswordPolicyF = SysAdvPara.passwordPolicyF;
	tWebEncryptionModeF = SysAdvPara.webEncryptionModeF;
	tHttpPort = SysAdvPara.httpPort;
	tHttpsPort = SysAdvPara.httpsPort;

	SysAdvPara = DfltSysAdvPara;

	SysAdvPara.passwordPolicyF = tPasswordPolicyF;
	SysAdvPara.webEncryptionModeF = tWebEncryptionModeF;
	SysAdvPara.httpPort = tHttpPort;
	SysAdvPara.httpsPort = tHttpsPort;

    if(GetDoorApplType() == PANEL_DOOR_APPL)
    {
        SysAdvPara.authenticationType = BASIC_AUTHENTICATION;
    }

    WritePasswordToWebAuthFile();
	SendDfltConfigEvent(GEN_SYS_ADV_TABLE_ID);

	if(WriteAdvParaConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "AdvParaConfig - Except Encryption Mode And Ports");
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	LoadDfltPasswordPolicyF()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Load Dflt PasswordPolicyF
//*****************************************************************************
BOOL LoadDfltPasswordPolicyF(void)
{
	SysAdvPara.passwordPolicyF = DfltSysAdvPara.passwordPolicyF;
	if(WriteAdvParaConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "AdvParaConfig - PasswordPolicyF");
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	LoadDfltAdvParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Advance system parameters
//*****************************************************************************
BOOL LoadDfltAdvParaConfig(void)
{
	SysAdvPara = DfltSysAdvPara;

    if(GetDoorApplType() == PANEL_DOOR_APPL)
    {
        SysAdvPara.authenticationType = BASIC_AUTHENTICATION;
    }

    WritePasswordToWebAuthFile();
	SendDfltConfigEvent(GEN_SYS_ADV_TABLE_ID);

	if(WriteAdvParaConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "AdvParaConfig");
		return SUCCESS;
	}
	return FAIL;
}
//******************************************************************************
//	WriteAdvParaConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Advance system parameter into configuration file.
//******************************************************************************
BOOL WriteAdvParaConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if( (fd = open(SYS_ADV_PARA_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Advance System parameter");
		return FAIL;
	}

	// Write version number
	ver = ADVANCE_SYS_PARA_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Advance System parameter version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(SysAdvPara) != write(fd,&SysAdvPara,sizeof(SysAdvPara)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Advance System parameter");
		close(fd);
		return FAIL;
	}

	close(fd);
	SystemAdvanceParaConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitBasicParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's basic parameters
//*****************************************************************************
void InitBasicParaConfig(void)
{
	if(SUCCESS != ReadBasicParaConfig())
	{
		if(SUCCESS != LoadDfltBasicParaConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise System's Basic Parameters");
		}
	}
}

//*****************************************************************************
//	ReadBasicParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads basic system parameter from file
//*****************************************************************************
BOOL ReadBasicParaConfig(void)
{
	INT32 		fd;
	UINT8		ver;
	BOOL		status = FAIL;

	if((fd = open(SYS_BASIC_PARA_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open system basic configuration file");
		return FAIL;
	}

	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read system basic configuration version");
		close(fd);
		return FAIL;
	}

	// Check whether the version number matches
	if(ver != BASIC_SYS_PARA_VER)
	{
		EPRINT(CONFIG_LOG,"Basic configuration version[%d] mismatch : Saving Old data in new configuration", ver);
		status = LoadOldBasicConfig(fd, ver);
		close(fd);
		return status;
	}

	// Read configuration
	if(sizeof(SysBasicPara) != read(fd,&SysBasicPara,sizeof(SysBasicPara) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read system basic configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	SysBasicParaConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltBasicParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads basic system parameters
//*****************************************************************************
BOOL LoadDfltBasicParaConfig(void)
{
	SysBasicPara = DfltBasicSysPara;

	SendDfltConfigEvent(GEN_SYS_BASIC_TABLE_ID);

	if(WriteBasicParaConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "BasicParaConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	LoadDfltBasicSysParaExceptWebServerConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads basic system parameters except web server configuration
//*****************************************************************************
BOOL LoadDfltBasicSysParaExceptWebServerConfig(void)
{
    HTTP_WEB_SERVER_CONFIG_t  temphttpWebServerConfig;

    memcpy((VOIDPTR)&temphttpWebServerConfig,(VOIDPTR)&(SysBasicPara.httpWebServerConfig),sizeof(temphttpWebServerConfig));
    SysBasicPara = DfltBasicSysPara;
    memcpy((VOIDPTR)&(SysBasicPara.httpWebServerConfig),(VOIDPTR)&temphttpWebServerConfig,sizeof(temphttpWebServerConfig));

    SendDfltConfigEvent(GEN_SYS_BASIC_TABLE_ID);

    return WriteBasicParaConfig();
}

//*********************************************************************************
//	WriteBasicParaConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes basic system parameter into configuration file.
//*********************************************************************************
BOOL WriteBasicParaConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_BASIC_PARA_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open system basic configuration file");
		return FAIL;
	}

	// Write version number
	ver = BASIC_SYS_PARA_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write system basic configuration version");
		close(fd);
		return FAIL;
	}

	if(sizeof(SysBasicPara) != write(fd,&SysBasicPara,sizeof(SysBasicPara)))
	{
		EPRINT(CONFIG_LOG,"Failed to write system basic configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	SysBasicParaConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitBlockedUserConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises Blocked User Configuration
//*****************************************************************************
void InitBlockedUserConfig(void)
{
	UINT8 pg;

	// Since Blocked Users are stored in multiple files,
	// we read all files and store all in one ram copy
	for(pg = 0 ; pg <= TOTAL_BLOCKED_USER_PAGES ; pg++)
	{
		if(SUCCESS != ReadBlockedUserConfig(pg))
		{
			if(SUCCESS != LoadDfltBlockedUserConfig(pg))
			{
				EPRINT(CONFIG_LOG,"Failed to intialise Blocked User Configuration");
			}
		}
	}
}

//*****************************************************************************
//	RemoveDirectory()
//	Param:
//		IN : filename, filesize
//		OUT: bool
//	Returns:
//	Description: It Will write provided file with given data.
//*****************************************************************************
void RemoveDirectory(VOIDPTR dataPtr)
{
	CHAR filePath[MAX_FILENAME_LENGTH] = {'\0'};
	CHAR cmdBuff[MAX_FILENAME_LENGTH] = {'\0'};
	
	memcpy(filePath, dataPtr, MAX_FILENAME_LENGTH);
	EPRINT(DC_LOG,"received Directory Path for deletion = [%s]",filePath);
	
	snprintf(cmdBuff, MAX_FILENAME_LENGTH, "rm -rf %s", filePath);
	ExeCmd(cmdBuff);
}

//*****************************************************************************
//	InsertRemoveDirectoryReq()
//	Param:
//		IN : filename
//		OUT: 
//	Returns:
//	Description: It Will insert work item for deletion of directory.
//*****************************************************************************
BOOL InsertRemoveDirectoryReq(char* fileName)
{
	char* data = NULL;	
	data = (char*)calloc(1,MAX_FILENAME_LENGTH);
	if(data == NULL)
	{
		EPRINT(DC_LOG,"Failed TO allocate Memory for Removing Directory");
		return FALSE;
	}
	snprintf(data, MAX_FILENAME_LENGTH, "%s",fileName);
	
	RegisterWorkItem(RemoveDirectory, data);
	return TRUE;
}

//*****************************************************************************
//	ReadBlockedUseronfig()
//	Param:
//		IN : pg		- Page to read
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Blocked User from configuration file
//*****************************************************************************
BOOL ReadBlockedUserConfig(UINT8 pg)
{
	INT32 	fd;
	CHAR 	filePath[MAX_CNFG_FILE_NAME_SZ];
	UINT8 	recordOffset;
	UINT8	recordCount;
	UINT8	ver, status;

	//file name
	sprintf(filePath, "%s%d%s", SYS_BLOCKED_USER_FILE, (pg+1), SYS_CFG_FILE_EXT);

	//Start index from where to store in RAM
	recordOffset = (pg * BLOCKED_USER_PER_PAGE);

	if(pg == TOTAL_BLOCKED_USER_PAGES)	//If last page
	{
		//Remainig entries in file
		recordCount = (MAX_BLOCKED_USER - (pg * BLOCKED_USER_PER_PAGE));
	}
	else	//If not last page
	{
		recordCount = BLOCKED_USER_PER_PAGE;
	}

	if((fd = open(filePath, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Blocked User Configuration file");
		return FAIL;
	}

	//File opened successfully
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Blocked User Configuration version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != BLOCKED_USER_CFG_VER)
	{
		EPRINT(CONFIG_LOG,"Blocked User Configuration version doesn't match");
		status = LoadOldBlockedUserConfig(fd, ver, recordOffset, recordCount ,pg);
		close(fd);
		return status;
	}

	// Read configuration
	if( (recordCount*sizeof(BlockedUser[0])) != (UINT32)read(fd,&BlockedUser[recordOffset],(recordCount*sizeof(BlockedUser[0]))))
	{
		EPRINT(CONFIG_LOG, "Failed to read file: %s", filePath);
		close(fd);
		return FAIL;
	}

	close(fd);
	BlockedUserConfigDebug(pg, recordOffset, recordCount);
	return SUCCESS;

}

//*****************************************************************************
//	LoadDfltBlockedUserConfig()
//	Param:
//		IN : pg		- Page for which to load default
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Blocked User Configuration
//*****************************************************************************
BOOL LoadDfltBlockedUserConfig(UINT8 pg)
{
	UINT8 	recordOffset;
	UINT8	index;
	UINT8	recordCount;

	recordOffset = (pg * BLOCKED_USER_PER_PAGE);		//Start of index

	//Length of index
	//If last page
	if(pg == TOTAL_BLOCKED_USER_PAGES)
	{
		recordCount = (MAX_BLOCKED_USER - (pg * BLOCKED_USER_PER_PAGE));
	}
	else	//If not last page
	{
		recordCount = BLOCKED_USER_PER_PAGE;
	}

	for(index = recordOffset ; index < (recordOffset + recordCount); index++)
	{
		BlockedUser[index] = DfltBlockedUser;
	}

	if(pg == 0)
	{
		DPRINT(SYS_LOG,"Configuration defaulted - BlockedUser");
	}
	return WriteBlockedUserConfig(pg);
}

//*****************************************************************************
//	LoadDfltAllBlockedUserConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default All Blocked User Configuration
//*****************************************************************************
static VOID LoadDfltAllBlockedUserConfig(VOID)
{

	UINT8	pg;

	for(pg = 0; pg <= TOTAL_BLOCKED_USER_PAGES; pg++)
	{
		LoadDfltBlockedUserConfig(pg);
	}
	CONFIGPRINT(CONFIG_DEFAULT, "AllBlockedUserConfig");
}

//*********************************************************************************
//	WriteSingleBlockedUserConfig()
//	Param:
//		IN : 	Index - Index for which to load config
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes System User config into configuration file for single blocked
//				user.
//
//*********************************************************************************
BOOL WriteSingleBlockedUserConfig(UINT8 index)
{
	UINT8 	pg;
	INT32 	fd;
	CHAR 	filePath[MAX_CNFG_FILE_NAME_SZ];
	UINT8 	recordOffset;

	// If requested index is greater than maximum limit, no need to write it in file
	if(index >= MAX_BLOCKED_USER)
	{
		return FAIL;
	}

	pg = (index / BLOCKED_USER_PER_PAGE);

	//file name
	sprintf(filePath, "%s%d%s", SYS_BLOCKED_USER_FILE, (pg+1), SYS_CFG_FILE_EXT);

	//Start index in file
	recordOffset = (pg * BLOCKED_USER_PER_PAGE);

	if((fd = open(filePath, O_WRONLY | O_SYNC)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Blocked User configuration file");
		return FAIL;
	}

	// Set pointer to write requested index, consider version number
	lseek(fd,(((index - recordOffset) * (sizeof(BlockedUser[0]))) + 1),SEEK_SET);

	if((sizeof(BlockedUser[0])) != write(fd,&BlockedUser[index],sizeof(BlockedUser[0])))
	{
		EPRINT(CONFIG_LOG,"Failed to write Blocked User configuration for the given index");
		close(fd);
		return FAIL;
	}

	DPRINT(CONFIG_LOG,"blocked user [%s] is written in the page %d", GetAlphaNumId(index),pg);
	close(fd);
	BlockedUserConfigDebug(pg, index, 1);
	return SUCCESS;
}

//*********************************************************************************
//	WriteBlockedUserConfig()
//	Param:
//		IN : pg		- Page for which to load default
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Blocked User into configuration file.
//*********************************************************************************
BOOL WriteBlockedUserConfig(UINT8 pg)
{
	INT32 	fd;
	CHAR 	filePath[MAX_CNFG_FILE_NAME_SZ];
	UINT8 	recordOffset;
	UINT8	recordCount;
	UINT8	ver;

	//file name
	sprintf(filePath, "%s%d%s", SYS_BLOCKED_USER_FILE, (pg+1), SYS_CFG_FILE_EXT);

	//Start index Number in file
	recordOffset = (pg * BLOCKED_USER_PER_PAGE);

	if(pg == TOTAL_BLOCKED_USER_PAGES)	//If last page
	{
		//Remainig entries in file
		recordCount = (MAX_BLOCKED_USER - (pg * BLOCKED_USER_PER_PAGE));
	}
	else	//If not last page
	{
		recordCount = BLOCKED_USER_PER_PAGE;
	}

	if((fd = open(filePath, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Blocked User Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = BLOCKED_USER_CFG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Blocked User Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if((recordCount*sizeof(BlockedUser[0]))
	  != (UINT32)write(fd,&BlockedUser[recordOffset],(recordCount*sizeof(BlockedUser[0]))))
	{
		EPRINT(CONFIG_LOG,"Failed to write Blocked User Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	BlockedUserConfigDebug(pg, recordOffset, recordCount);
	return SUCCESS;
}

//*****************************************************************************
//	InitDSTConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Day Light Saving parameters
//*****************************************************************************
void InitDSTConfig(void)
{
	if(SUCCESS != ReadDSTConfig())
	{
		if(SUCCESS != LoadDfltDSTConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise System's Day Light Saving Configuration");
		}
	}
}

//*****************************************************************************
//	ReadDSTConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Day Light Saving parameters from file
//*****************************************************************************
BOOL ReadDSTConfig(void)
{
	INT32 		fd;
	UINT8		ver;

	if((fd = open(SYS_DAY_LIGHT_SAVING_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Day Light Saving");
		return FAIL;
	}

	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Day Light Saving version");
		close(fd);
		return FAIL;
	}

	// Check whether the version number matches
	if(ver != DST_CFG_VER)
	{
		EPRINT(CONFIG_LOG,"Day Light Saving version doesn't match");
		close(fd);
		return FAIL;
	}

	if(sizeof(DstConfig) != read(fd,&DstConfig,sizeof(DstConfig)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Day Light Saving");
		close(fd);
		return FAIL;
	}

	close(fd);
	DayLightSavingConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltDSTConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Day Light Saving parameters
//*****************************************************************************
BOOL LoadDfltDSTConfig(void)
{
	DstConfig = DfltDstConfig;

	SendDfltConfigEvent(DST_PARA_TABLE_ID);

	if(WriteDSTConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "DSTConfig");
		return SUCCESS;
	}
	return FAIL;
}

//******************************************************************************
//	WriteDSTConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Day Light Saving Parameters into configuration file.
//******************************************************************************
BOOL WriteDSTConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_DAY_LIGHT_SAVING_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Day Light Saving");
		return FAIL;
	}

	// Write version number
	ver = DST_CFG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Day Light Saving version");
		close(fd);
		return FAIL;
	}
	// Write configuration
	if(sizeof(DstConfig) != write(fd,&DstConfig,sizeof(DstConfig)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Day Light Saving");
		close(fd);
		return FAIL;
	}

	close(fd);
	DayLightSavingConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitFirstInConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises First In User Configuration
//*****************************************************************************
void InitFirstInConfig(void)
{
	if(SUCCESS != ReadFirstInConfig())
	{
		if(SUCCESS != LoadDfltFirstInConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise First In User Configuration");
		}
	}
}

//*****************************************************************************
//	ReadFirstInConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads First In User Configuration from file
//*****************************************************************************
BOOL ReadFirstInConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_FIRST_IN_USER_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open First in user configuration file");
		return FAIL;
	}

	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read First in user configuration version");
		close(fd);
		return FAIL;
	}

	// Check whether the version number matches
	if(ver != FIRST_IN_CFG_VER)
	{
		if(SUCCESS == LoadOldFirstInRuleConfig(fd, ver))
		{
			DPRINT(CONFIG_LOG,"First in user configuration Previous version:%d matches", ver);
			close(fd);
			return SUCCESS;
		}
		EPRINT(CONFIG_LOG,"First in user configuration version doesn't matches");
		close(fd);
		return FAIL;
	}

	if(sizeof(FirstInUserGroup) != read(fd, FirstInUserGroup, sizeof(FirstInUserGroup) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read First in user configuration file");
		close(fd);
		return FAIL;
	}

	close(fd);
	FirstInUserGroupConfigDebug(MAX_FIRST_IN_LISTS);
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltFirstInConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads First In User Configuration
//*****************************************************************************
BOOL LoadDfltFirstInConfig(void)
{
	UINT8 idx;

	for(idx = 0; idx < MAX_FIRST_IN_LISTS ; idx++)
	{
		FirstInUserGroup[idx] = DfltFirstInUser;
	}

	SendDfltConfigEvent(FIRST_IN_TABLE_ID);

	if(WriteFirstInConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "FirstInConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteSingleFirstInConfig()
//	Param:
//		IN : list	- list in which index should be written
//		OUT: NONE
//	Returns:
//			SUCCESS - If successful
//			FAILURE otherwise
//	Description:
//			Writes single first in record into configuration file.
//*********************************************************************************
BOOL WriteSingleFirstInConfig(UINT8 list)
{
	INT32 	fd;

	// If requested index is greater than maximum limit, no need to write it in file
	if(list >= MAX_FIRST_IN_LISTS)
	{
		return FAIL;
	}

	if((fd = open(SYS_FIRST_IN_USER_FILE, O_WRONLY | O_SYNC)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open first in file");
		return FAIL;
	}

	// Set pointer to write requested index, consider byte of version number
	lseek(fd,((list * (sizeof(FIRST_IN_LIST_t))) + 1),SEEK_SET);

	if((sizeof(FirstInUserGroup[0])) != write(fd,&FirstInUserGroup[list],sizeof(FirstInUserGroup[0])))
	{
		EPRINT(CONFIG_LOG,"Failed to write first in entry for the given list");
		close(fd);
		return FAIL;
	}

	DPRINT(CONFIG_LOG,"1st-in:list %d is written",list);
	close(fd);
	FirstInUserGroupConfigDebug(list);
	return SUCCESS;
}

//*********************************************************************************
//	WriteFirstInConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes First In User Configuration into configuration file.
//*********************************************************************************
BOOL WriteFirstInConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_FIRST_IN_USER_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open First in user configuration file");
		return FAIL;
	}

	// Write version number
	ver = FIRST_IN_CFG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write First in user configuration version");
		close(fd);
		return FAIL;
	}
	// Write configuration
	if(sizeof(FirstInUserGroup) != write(fd,FirstInUserGroup,sizeof(FirstInUserGroup)))
	{
		EPRINT(CONFIG_LOG,"Failed to write First in user configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	FirstInUserGroupConfigDebug(MAX_FIRST_IN_LISTS);
	return SUCCESS;
}

//*****************************************************************************
//	InitHolidaySchConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises Holiday Configuration
//*****************************************************************************
void InitHolidaySchConfig(void)
{
	if(SUCCESS != ReadHolidaySchConfig())
	{
		if(SUCCESS != LoadDfltHolidaySchConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise Holiday Configuration");
		}
	}
}

//*****************************************************************************
//	ReadHolidaySchConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Holiday from configuration file
//*****************************************************************************
BOOL ReadHolidaySchConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	if((fd = open(SYS_HOLIDAY_SCH_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Holiday Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Holiday version");
		close(fd);
		return FAIL;
	}

	// Check whether the version number matches
	if(ver != HOLIDAY_SCH_VER)
	{
		EPRINT(CONFIG_LOG,"Holiday version doesn't match");
		close(fd);
		return FAIL;
	}

	// Read configuration
	if(sizeof(HolidaySchedule) != read(fd, HolidaySchedule, sizeof(HolidaySchedule) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read Holiday Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	HolidayScheduleConfigDebug(MAX_HOLIDAYS);
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltHolidaySchConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Holiday Configuration
//*****************************************************************************
BOOL LoadDfltHolidaySchConfig(void)
{

	UINT8 configIdx;
	UINT8 schIdx;
	UINT8 holidayTableId;

	for(schIdx = 0 ; schIdx < MAX_HOLIDAY_SCHEDULE ; schIdx ++)
	{
		HolidaySchedule[schIdx].scheduleId = INVALID_SCHEDULE_ID;
		for(configIdx = 0 ; configIdx < MAX_HOLIDAYS ; configIdx++)
		{
			HolidaySchedule[schIdx].holidays[configIdx] = DfltHoliday;
		}

		switch(schIdx)
		{
		case 0:
			holidayTableId = HOLIDAY_SCH1_TABLE_ID;
			break;

		case 1:
			holidayTableId = HOLIDAY_SCH2_TABLE_ID;
			break;

		case 2:
			holidayTableId = HOLIDAY_SCH3_TABLE_ID;
			break;

		case 3:
			holidayTableId = HOLIDAY_SCH4_TABLE_ID;
			break;
		}

		SendDfltConfigEvent(holidayTableId);
	}

	if(WriteHolidaySchConfig(0) == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "HolidaySchConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteSingleHolidaySchConfig()
//	Param:
//		IN : index	- index which needs to be written
//			 sch	- holiday schedule	for which index should be written
//		OUT: NONE
//	Returns:
//			SUCCESS - If successful
//			FAILURE otherwise
//	Description:
//			Writes single holiday schedule into configuration file.
//*********************************************************************************
BOOL WriteSingleHolidaySchConfig(UINT8 index, UINT8 sch)
{
	INT32 	fd;

	// If requested index is greater than maximum limit, no need to write it in file
	if((index >= MAX_HOLIDAYS) || (sch > TOTAL_HOLIDAY_SCH))
	{
		return FAIL;
	}

	if((fd = open(SYS_HOLIDAY_SCH_FILE, O_WRONLY | O_SYNC)) < 0)
	{
		EPRINT(CONFIG_LOG,"holiday sch:Failed to open holiday scheule file");
		return FAIL;
	}

	// Write requested index, consider version number (1 byte) and Holiday Schedule Id (1 Byte)
	lseek(fd, ( ( ( (sch * MAX_HOLIDAYS) + index) * sizeof(HOLIDAY_t) ) + 1 + OFFSET_SCHEDULE_ID +  sch), SEEK_SET);

	if( (sizeof(HolidaySchedule[0].holidays[0]) ) != write(fd, &(HolidaySchedule[sch].holidays[index]), sizeof(HolidaySchedule[0].holidays[0]) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write holiday schedule for the given index");
		close(fd);
		return FAIL;
	}

	DPRINT(CONFIG_LOG,"index %d is written in sch %d",index,sch);
	close(fd);
	HolidayScheduleConfigDebug(index);
	return SUCCESS;
}

//*********************************************************************************
//	WriteHolidaySchConfig()
//	Param:
//		IN : sch		- Schedule for which to load default
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Holiday into configuration file.
//*********************************************************************************
BOOL WriteHolidaySchConfig(UNUSED_PARA UINT8 sch)
{
	INT32 	fd;
	UINT8	ver;

	// NOTE:- sch is not used in of Writing Schedule file. We write all schedule not single schedule
	if((fd = open(SYS_HOLIDAY_SCH_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Holiday Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Write version number in file
	ver = HOLIDAY_SCH_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Holiday version");
		close(fd);
		return FAIL;
	}

	// Write version number
	if(sizeof(HolidaySchedule) != write(fd, HolidaySchedule, sizeof(HolidaySchedule) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write Holiday Configuration file");
		close(fd);
		return FAIL;
	}

	close(fd);
	HolidayScheduleConfigDebug(MAX_HOLIDAYS);
	return SUCCESS;
}

//*****************************************************************************
//	InitEthParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Ethernet parameters
//*****************************************************************************
void InitEthParaConfig(void)
{
	if(SUCCESS != ReadEthParaConfig())
	{
		if(SUCCESS != LoadDfltEthParaConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to initialise System's Ethernet Parameters");
		}
	}
}

//*****************************************************************************
//	ReadEthParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Ethernet parameter from file
//*****************************************************************************
BOOL ReadEthParaConfig(void)
{
	INT32 	fd, retVal;
	UINT8	ver;

	if( (fd = open(SYS_ETH_PARA_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Ethernet Parameters");
		return FAIL;
	}

	// Read network parameters
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Ethernet Parameter version");
		close(fd);
		return FAIL;
	}

	retVal = read(fd, &EthernetParameters, sizeof(ETHERNET_PARA_t));

	if(((UINT32)retVal) != sizeof(ETHERNET_PARA_t))
	{
		EPRINT(CONFIG_LOG,"Failed to read Ethernet Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	EthernetParameterConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltEthParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Ethernet parameters
//*****************************************************************************
BOOL LoadDfltEthParaConfig(void)
{
	EthernetParameters = DfltEthPara;
	if(GetDoorApplType() == PANEL_DOOR_APPL)
	{
		EthernetParameters.nwType = LAN_IP_STATIC;
	}
	// Update MAC Address into structure 'EthernetParameters'
	GetEthernetMacAddress();

	SendDfltConfigEvent(ETHERNET_PARA_TABLE_UPDATE_ID);
	if(WriteEthParaConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "EthParaConfig");
		return SUCCESS;
	}
	return FAIL;
}

//******************************************************************************
//	WriteEthParaConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Ethernet parameter into configuration file.
//******************************************************************************
BOOL WriteEthParaConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if( (fd = open(SYS_ETH_PARA_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Ethernet Parameters");
		return FAIL;
	}

	// Write Ethernet parameter version number
	ver = ETHERNET_PARA_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Ethernet Parameter version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(ETHERNET_PARA_t) != write(fd, &EthernetParameters, sizeof(ETHERNET_PARA_t) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write Ethernet Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	EthernetParameterConfigDebug();
	return SUCCESS;
}
//*****************************************************************************
//	InitV11R3ServerParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Server parameters
//*****************************************************************************
void InitV11R3ServerParaConfig(void)
{
	if(SUCCESS != ReadV11R3ServerParaConfig())
	{
		if(SUCCESS != LoadDfltV11R3ServerParaConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to initialise System's Server Parameters");
		}
	}
}

//*****************************************************************************
//	ReadV11R3ServerParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads V11R3 Server parameter from file
//*****************************************************************************
BOOL ReadV11R3ServerParaConfig(void)
{
	INT32 	fd;
	UINT8	ver;
	BOOL 	status = FAIL;

	if( (fd = open(V11R3_SYS_SERVER_PARA_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Server Parameters");
		return FAIL;
	}

	// Read network parameters
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"V11R3 Failed to read Server Parameter version");
		close(fd);
		return FAIL;
	}

	// Check whether the versions matches
	if(ver != V11R3_SERVER_PARA_VER)
	{
		EPRINT(CONFIG_LOG, "V11R3 Server Configuration: version mismatch");
		close(fd);
		return status;
	}

	if(sizeof(V11R3ServerParameters) != read(fd, &V11R3ServerParameters, sizeof(V11R3ServerParameters)))
	{
		EPRINT(CONFIG_LOG,"Failed to read V11R3 Server Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	V11R3ServerParameterConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltV11R3ServerParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads V11R3 Server parameters
//*****************************************************************************
BOOL LoadDfltV11R3ServerParaConfig(void)
{
	V11R3ServerParameters = DfltV11R3ServerPara;
	return WriteV11R3ServerParaConfig();
}

//******************************************************************************
//	WriteV11R3ServerParaConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Server parameter into configuration file.
//******************************************************************************
BOOL WriteV11R3ServerParaConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if( (fd = open(V11R3_SYS_SERVER_PARA_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open V11R3 Server Parameters");
		return FAIL;
	}

	// Write network parameter version number
	ver = V11R3_SERVER_PARA_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write V11R3 Server Parameter version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(V11R3_SERVER_PARA_t) != write(fd, &V11R3ServerParameters, sizeof(V11R3_SERVER_PARA_t) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write V11R3 Server Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	V11R3ServerParameterConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitServerParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Server parameters
//*****************************************************************************
void InitServerParaConfig(void)
{
	BOOL	writeRequiredF = FALSE;
	CHAR	cmdBuf[MAX_SYS_CMD_BUF_SZ];
	struct 	stat	buf;

	InitServerModeConfig();
	InitV11R3ServerParaConfig();
	InitVyomServerConfig();

	if(SUCCESS != ReadServerParaConfig())
	{
		if(SUCCESS != LoadDfltNewServerParaConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to initialise System's Server Parameters");
		}
	}

	if(stat(UPGRADING_FIRMWARE_SIGN,&buf) == 0)
	{
		if(ServerModeCfg.serverMode == COSEC_CENTRA)
		{
			if(V11R3ServerParameters.ethServerConnection == SERVER_CONN_IP_ADDR)
			{
				V11R3ServerParameters.ethServerConnection = SERVER_CONN_HOST_NAME;
				ServerParameters.ethServerConnection = SERVER_CONN_HOST_NAME;
				writeRequiredF = TRUE;
				snprintf(ServerParameters.ethServerHostName, MAX_IP_ADDRESS, "%s", V11R3ServerParameters.ethServerIpAddress);
				snprintf(V11R3ServerParameters.ethServerHostName, MAX_SERVER_ADDR, "%s", ServerParameters.ethServerHostName);
			}
			else
			{
				V11R3ServerParameters.ethServerConnection = SERVER_CONN_HOST_NAME;
				ServerParameters.ethServerConnection = SERVER_CONN_HOST_NAME;
				if(strncmp(V11R3ServerParameters.ethServerHostName, ServerParameters.ethServerHostName, MAX_SERVER_ADDR) != 0)
				{
					snprintf(ServerParameters.ethServerHostName, MAX_SERVER_ADDR, "%s", V11R3ServerParameters.ethServerHostName);
					writeRequiredF = TRUE;
				}
			}

			if(V11R3ServerParameters.ethServerPort != ServerParameters.ethServerPort)
			{
				ServerParameters.ethServerPort = V11R3ServerParameters.ethServerPort;
				writeRequiredF = TRUE;
			}

			if(V11R3ServerParameters.modemServerConnection == SERVER_CONN_IP_ADDR)
			{
				V11R3ServerParameters.modemServerConnection = SERVER_CONN_HOST_NAME;
				ServerParameters.modemServerConnection = SERVER_CONN_HOST_NAME;
				writeRequiredF = TRUE;
				snprintf(ServerParameters.modemServerHostName, MAX_IP_ADDRESS, "%s", V11R3ServerParameters.modemServerIpAddress);
				snprintf(V11R3ServerParameters.modemServerHostName, MAX_SERVER_ADDR, "%s", ServerParameters.modemServerHostName);
			}
			else
			{
				V11R3ServerParameters.modemServerConnection = SERVER_CONN_HOST_NAME;
				ServerParameters.modemServerConnection = SERVER_CONN_HOST_NAME;
				if(strncmp(V11R3ServerParameters.modemServerHostName, ServerParameters.modemServerHostName, MAX_SERVER_ADDR) != 0)
				{
					snprintf(ServerParameters.modemServerHostName, MAX_SERVER_ADDR, "%s", V11R3ServerParameters.modemServerHostName);
					writeRequiredF = TRUE;
				}
			}

			if(V11R3ServerParameters.modemServerPort != ServerParameters.modemServerPort)
			{
				ServerParameters.modemServerPort = V11R3ServerParameters.modemServerPort;
				writeRequiredF = TRUE;
			}

			if(V11R3ServerParameters.wifiServerConnection == SERVER_CONN_IP_ADDR)
			{
				V11R3ServerParameters.wifiServerConnection = SERVER_CONN_HOST_NAME;
				ServerParameters.wifiServerConnection = SERVER_CONN_HOST_NAME;
				writeRequiredF = TRUE;
				snprintf(ServerParameters.wifiServerHostName, MAX_IP_ADDRESS, "%s", V11R3ServerParameters.wifiServerIpAddress);
				snprintf(V11R3ServerParameters.wifiServerHostName, MAX_SERVER_ADDR, "%s", ServerParameters.wifiServerHostName);
			}
			else
			{
				V11R3ServerParameters.wifiServerConnection = SERVER_CONN_HOST_NAME;
				ServerParameters.wifiServerConnection = SERVER_CONN_HOST_NAME;
				if(strncmp(V11R3ServerParameters.wifiServerHostName, ServerParameters.wifiServerHostName, MAX_SERVER_ADDR) != 0)
				{
					snprintf(ServerParameters.wifiServerHostName, MAX_SERVER_ADDR, "%s", V11R3ServerParameters.wifiServerHostName);
					writeRequiredF = TRUE;
				}
			}

			if(V11R3ServerParameters.wifiServerPort != ServerParameters.wifiServerPort)
			{
				ServerParameters.wifiServerPort = V11R3ServerParameters.wifiServerPort;
				writeRequiredF = TRUE;
			}

			if(V11R3ServerParameters.httpInterface != SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface)
			{
				SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface = (UINT8)V11R3ServerParameters.httpInterface;
				SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpInterface = (UINT8)V11R3ServerParameters.httpInterface;
				writeRequiredF = TRUE;
			}
			if(V11R3ServerParameters.httpServerConnection == SERVER_CONN_IP_ADDR)
			{
				V11R3ServerParameters.httpServerConnection = SERVER_CONN_HOST_NAME;
				ServerParameters.httpServerConnection = SERVER_CONN_HOST_NAME;
				writeRequiredF = TRUE;
				snprintf(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName, MAX_IP_ADDRESS, "%s", V11R3ServerParameters.httpServerIpAddress);
				snprintf(SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerHostName, MAX_IP_ADDRESS, "%s", V11R3ServerParameters.httpServerIpAddress);
				snprintf(V11R3ServerParameters.httpServerHostName, MAX_SERVER_ADDR, "%s", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName);
			}
			else
			{
				V11R3ServerParameters.httpServerConnection = SERVER_CONN_HOST_NAME;
				ServerParameters.httpServerConnection = SERVER_CONN_HOST_NAME;
				if(strncmp(V11R3ServerParameters.httpServerHostName, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName, MAX_SERVER_ADDR) != 0)
				{
					snprintf(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName, MAX_SERVER_ADDR, "%s", V11R3ServerParameters.httpServerHostName);
					snprintf(SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerHostName, MAX_SERVER_ADDR, "%s", V11R3ServerParameters.httpServerHostName);
					writeRequiredF = TRUE;
				}
			}

			if(V11R3ServerParameters.httpServerPort  != SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort)
			{
				SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort = V11R3ServerParameters.httpServerPort;
				SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPort = V11R3ServerParameters.httpServerPort;
				writeRequiredF = TRUE;
			}
		}
		else if(ServerModeCfg.serverMode == COSEC_VYOM)
		{
			if(VyomServerPara.serverConnectionVyom == SERVER_CONN_IP_ADDR)
			{
				VyomServerPara.serverConnectionVyom = SERVER_CONN_HOST_NAME;
				ServerParameters.ethServerConnection = SERVER_CONN_HOST_NAME;
				writeRequiredF = TRUE;
				snprintf(ServerParameters.ethServerHostName, MAX_IP_ADDRESS, "%s", VyomServerPara.serverIpAddressVyom);
				snprintf(VyomServerPara.ServerHostNameVyom, MAX_SERVER_ADDR, "%s", ServerParameters.ethServerHostName);

				ServerParameters.wifiServerConnection = SERVER_CONN_HOST_NAME;
				snprintf(ServerParameters.wifiServerHostName, MAX_IP_ADDRESS, "%s", VyomServerPara.serverIpAddressVyom);

				ServerParameters.modemServerConnection = SERVER_CONN_HOST_NAME;
				snprintf(ServerParameters.modemServerHostName, MAX_IP_ADDRESS, "%s", VyomServerPara.serverIpAddressVyom);
			}
			else
			{
				VyomServerPara.serverConnectionVyom = SERVER_CONN_HOST_NAME;

				ServerParameters.ethServerConnection = SERVER_CONN_HOST_NAME;
				if(strncmp(VyomServerPara.ServerHostNameVyom, ServerParameters.ethServerHostName, MAX_SERVER_ADDR_254) != 0)
				{
					snprintf(ServerParameters.ethServerHostName, MAX_SERVER_ADDR_254, "%s", VyomServerPara.ServerHostNameVyom);
					writeRequiredF = TRUE;
				}

				ServerParameters.wifiServerConnection = SERVER_CONN_HOST_NAME;
				if(strncmp(VyomServerPara.ServerHostNameVyom, ServerParameters.wifiServerHostName, MAX_SERVER_ADDR_254) != 0)
				{
					snprintf(ServerParameters.wifiServerHostName, MAX_SERVER_ADDR_254, "%s", VyomServerPara.ServerHostNameVyom);
					writeRequiredF = TRUE;
				}

				ServerParameters.modemServerConnection = SERVER_CONN_HOST_NAME;
				if(strncmp(VyomServerPara.ServerHostNameVyom, ServerParameters.modemServerHostName, MAX_SERVER_ADDR_254) != 0)
				{
					snprintf(ServerParameters.modemServerHostName, MAX_SERVER_ADDR_254, "%s", VyomServerPara.ServerHostNameVyom);
					writeRequiredF = TRUE;
				}
			}

			if(VyomServerPara.ServerPortVyom  != ServerParameters.ethServerPort)
			{
				ServerParameters.ethServerPort = VyomServerPara.ServerPortVyom;
				writeRequiredF = TRUE;
			}

			if(VyomServerPara.ServerPortVyom  != ServerParameters.wifiServerPort)
			{
				ServerParameters.wifiServerPort = VyomServerPara.ServerPortVyom;
				writeRequiredF = TRUE;
			}

			if(VyomServerPara.ServerPortVyom  != ServerParameters.modemServerPort)
			{
				ServerParameters.modemServerPort = VyomServerPara.ServerPortVyom;
				writeRequiredF = TRUE;
			}

			if(VyomServerPara.httpInterfaceVyom != SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface)
			{
				SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface = (UINT8)VyomServerPara.httpInterfaceVyom;
				SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpInterface = (UINT8)VyomServerPara.httpInterfaceVyom;
				writeRequiredF = TRUE;
			}

			ServerParameters.httpServerConnection = SERVER_CONN_HOST_NAME;
			if(strncmp(VyomServerPara.httpServerHostNameVyom, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName, MAX_SERVER_ADDR_254) != 0)
			{
				snprintf(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName, MAX_SERVER_ADDR_254, "%s", VyomServerPara.httpServerHostNameVyom);
				snprintf(SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerHostName, MAX_SERVER_ADDR_254, "%s", VyomServerPara.httpServerHostNameVyom);
				writeRequiredF = TRUE;
			}

			if(VyomServerPara.httpServerPortVyom  != SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort)
			{
				SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort = VyomServerPara.httpServerPortVyom;
				SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPort = VyomServerPara.httpServerPortVyom;
				writeRequiredF = TRUE;
			}

			if(strncmp(VyomServerPara.TenantIdVyom, ServerParameters.TenantIdVyom, MAX_TENANT_ID) != 0)
			{
				snprintf(ServerParameters.TenantIdVyom, MAX_TENANT_ID, "%s", VyomServerPara.TenantIdVyom);
				writeRequiredF = TRUE;
			}

			DPRINT(SYS_LOG, "current server host name: %s, %s, %s", ServerParameters.ethServerHostName, ServerParameters.wifiServerHostName, ServerParameters.modemServerHostName);
			DPRINT(SYS_LOG, "updating server host name to : %s", DFLT_HOST_NAME_VYOM);

			snprintf(ServerParameters.ethServerHostName, sizeof(ServerParameters.ethServerHostName), "%s", DFLT_HOST_NAME_VYOM);
			snprintf(ServerParameters.wifiServerHostName, sizeof(ServerParameters.wifiServerHostName), "%s", DFLT_HOST_NAME_VYOM);
			snprintf(ServerParameters.modemServerHostName, sizeof(ServerParameters.modemServerHostName), "%s", DFLT_HOST_NAME_VYOM);
			ServerParameters.ethServerIpAddress[0] = '\0';
			ServerParameters.wifiServerIpAddress[0] = '\0';
			ServerParameters.modemServerIpAddress[0] = '\0';
			ServerParameters.ethServerPort = DFLT_PORT_NO_VYOM;
			ServerParameters.wifiServerPort = DFLT_PORT_NO_VYOM;
			ServerParameters.modemServerPort = DFLT_PORT_NO_VYOM;

			writeRequiredF = TRUE;
		}
		if(writeRequiredF == TRUE)
		{
			if(ServerModeCfg.serverMode == COSEC_CENTRA)
			{
				ServerParameters.type = 1;
			}
			else
			{
				ServerParameters.type = 0;
			}
			WriteServerParaConfig();
		}
		sprintf(cmdBuf, "rm -f %s", UPGRADING_FIRMWARE_SIGN);
		system(cmdBuf);
	}

	//check for http server configuration
	UpdateHttpServerHostNameService();

	TempServerParaVyom = ServerParameters;
	if(IsServerModeVyom() == YES)
	{
		sprintf(TenentIdStr, "T%s%c", ServerParameters.TenantIdVyom, FSP);
	}
	else
	{
		sprintf(TenentIdStr,"%s","");
	}
}

//*****************************************************************************
//	ReadServerParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Server parameter from file
//*****************************************************************************
BOOL ReadServerParaConfig(void)
{
	INT32 	fd;
	UINT8	ver, subVer;
	BOOL status = FALSE;

	if( (fd = open(SYS_SERVER_PARA_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Server Parameters");
		return FAIL;
	}

	// Read network parameters
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Server Parameter version");
		close(fd);
		return FAIL;
	}

	// Read network parameters
	if(sizeof(subVer) != read(fd, &subVer, sizeof(subVer)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Server Parameter sub version");
		close(fd);
		return FAIL;
	}

	// Check whether the versions matches
	if(ver != SERVER_PARA_VER)
	{
		EPRINT(CONFIG_LOG, "Server Configuration: version mismatch");
		status = LoadOldServerParaConfig(fd, ver);
		close(fd);
		ServerParameterConfigDebug();
		return status;
	}

	if(sizeof(ServerParameters) != read(fd, &ServerParameters, sizeof(ServerParameters)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Server Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	ServerParameterConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltServerParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Server parameters
//*****************************************************************************
static BOOL LoadDfltNewServerParaConfig(void)
{
	if(ServerModeCfg.serverMode == COSEC_VYOM)
	{
		ServerParameters = DfltServerParaVyom;
	}
	else
	{
		ServerParameters = DfltServerParaCentra;
	}
	return WriteNewServerParaConfig();
}

//*****************************************************************************
//	LoadDfltServerParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Server parameters
//*****************************************************************************
BOOL LoadDfltServerParaConfig(void)
{
	if(ServerModeCfg.serverMode == COSEC_VYOM)
	{
		ServerParameters = DfltServerParaVyom;
	}
	else
	{
		ServerParameters = DfltServerParaCentra;
	}

	SendDfltConfigEvent(SERVER_PARA_TABLE_UPDATE_ID);
	if(WriteServerParaConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "ServerParaConfig");
		return SUCCESS;
	}
	return FAIL;
}

//******************************************************************************
//	WriteServerParaConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Server parameter into configuration file.
//******************************************************************************
BOOL WriteServerParaConfig(void)
{
	BOOL status = SUCCESS;

	if(ServerModeCfg.serverMode == COSEC_CENTRA)
	{
		V11R3ServerParameters.interface  = ServerParameters.interface;
		V11R3ServerParameters.secondInterface  = ServerParameters.secondInterface;
		V11R3ServerParameters.thirdInterface  = ServerParameters.thirdInterface;

		V11R3ServerParameters.ethServerConnection = ServerParameters.ethServerConnection;
		snprintf(V11R3ServerParameters.ethServerIpAddress, MAX_IP_ADDRESS, "%s", ServerParameters.ethServerIpAddress);
		snprintf(V11R3ServerParameters.ethServerHostName, MAX_SERVER_ADDR, "%s", ServerParameters.ethServerHostName);
		V11R3ServerParameters.ethServerPort = ServerParameters.ethServerPort;

		V11R3ServerParameters.modemServerConnection = ServerParameters.modemServerConnection;
		snprintf(V11R3ServerParameters.modemServerIpAddress, MAX_IP_ADDRESS, "%s", ServerParameters.modemServerIpAddress);
		snprintf(V11R3ServerParameters.modemServerHostName, MAX_SERVER_ADDR, "%s", ServerParameters.modemServerHostName);
		V11R3ServerParameters.modemServerPort = ServerParameters.modemServerPort;

		V11R3ServerParameters.wifiServerConnection = ServerParameters.wifiServerConnection;
		snprintf(V11R3ServerParameters.wifiServerIpAddress, MAX_IP_ADDRESS, "%s", ServerParameters.wifiServerIpAddress);
		snprintf(V11R3ServerParameters.wifiServerHostName, MAX_SERVER_ADDR, "%s", ServerParameters.wifiServerHostName);
		V11R3ServerParameters.wifiServerPort = ServerParameters.wifiServerPort;

		V11R3ServerParameters.httpInterface  = SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface;
		V11R3ServerParameters.httpServerConnection = ServerParameters.httpServerConnection;
		snprintf(V11R3ServerParameters.httpServerIpAddress, MAX_IP_ADDRESS, "%s", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress);
		snprintf(V11R3ServerParameters.httpServerHostName, MAX_SERVER_ADDR, "%s", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName);
		V11R3ServerParameters.httpServerPort = SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort;
		status = WriteV11R3ServerParaConfig();
	}
	else if(ServerModeCfg.serverMode == COSEC_VYOM)
	{
		sprintf(TenentIdStr, "T%s%c", ServerParameters.TenantIdVyom, FSP);
		VyomServerPara.interfaceVyom = ServerParameters.interface;
		VyomServerPara.secondInterfaceVyom = ServerParameters.secondInterface;
		VyomServerPara.thirdInterfaceVyom = ServerParameters.thirdInterface;

		snprintf(VyomServerPara.ServerHostNameVyom, MAX_VYOM_SERVER_ADDR, "%s", ServerParameters.ethServerHostName);
		VyomServerPara.ServerPortVyom = ServerParameters.ethServerPort;

		snprintf(VyomServerPara.TenantIdVyom, MAX_TENANT_ID, "%s", ServerParameters.TenantIdVyom);

		snprintf(VyomServerPara.serverIpAddressVyom, MAX_IP_ADDRESS, "%s", ServerParameters.ethServerIpAddress);
		snprintf(VyomServerPara.ServerHostNameVyom, MAX_VYOM_SERVER_ADDR, "%s", ServerParameters.modemServerHostName);

		VyomServerPara.serverConnectionVyom = SERVER_CONN_HOST_NAME;
		VyomServerPara.httpInterfaceVyom = SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface;
		snprintf(VyomServerPara.httpServerHostNameVyom, MAX_VYOM_SERVER_ADDR, "%s", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName);
		VyomServerPara.httpServerPortVyom = SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort;
		status = WriteVyomServerConfig();
	}
	status &= WriteNewServerParaConfig();
	return status;
}

//******************************************************************************
//	WriteNewServerParaConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Server parameter into configuration file.
//******************************************************************************
static BOOL WriteNewServerParaConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if( (fd = open(SYS_SERVER_PARA_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Server Parameters");
		return FAIL;
	}

	// Write network parameter version number
	ver = SERVER_PARA_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Server Parameter version");
		close(fd);
		return FAIL;
	}

	// Write network parameter version number
	ver = SERVER_PARA_SUB_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Server Parameter version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(SERVER_PARA_t) != write(fd, &ServerParameters, sizeof(SERVER_PARA_t) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write Server Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	ServerParameterConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitWifiParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's wifi parameters
//*****************************************************************************
void InitWifiParaConfig(void)
{
	if(SUCCESS != ReadWifiParaConfig())
	{
		if(SUCCESS != LoadDfltWifiParaConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise System's WIFI Parameters");
		}
	}
}

//*****************************************************************************
//	ReadWifiParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads wifi parameter from file
//*****************************************************************************
BOOL ReadWifiParaConfig(void)
{
	INT32 		fd;
	UINT8		ver, status;

	if( (fd = open(SYS_WIFI_PARA_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open WIFI Parameters");
		return FAIL;
	}

	// Read wifi parameters
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read WIFI Parameter version");
		close(fd);
		return FAIL;
	}

	// Check whether the versions matches
	if(ver != WIFI_PARA_VER)
	{
		EPRINT(CONFIG_LOG,"Wifi Parameter version doesn't matches [%d] ", ver);
		status = LoadOldWiFiConfig(fd, ver);
		close(fd);
		return status;
	}

	if(sizeof(WifiParameters) != read(fd, &WifiParameters, sizeof(WifiParameters)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Wifi Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	WifiParameterConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltWifiParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Wifi parameters
//*****************************************************************************
BOOL LoadDfltWifiParaConfig(void)
{
	pthread_mutex_lock(&MutexLockAutoSwitchWiFi);
	WifiParameters = DfltWifiPara;
	pthread_mutex_unlock(&MutexLockAutoSwitchWiFi);

	if(GetWiFiStatus() != NO_WIFI)
	{
		GetWifiMacAddress();
	}
	DPRINT(SYS_LOG,"Configuration defaulted - WifiParameters");
	if(WriteWifiParaConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "WifiParaConfig");
		return SUCCESS;
	}
	return FAIL;
}

//******************************************************************************
//	WriteWifiParaConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Wifi parameter into configuration file.
//******************************************************************************
BOOL WriteWifiParaConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_WIFI_PARA_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Wifi Parameters");
		return FAIL;
	}

	// Write network parameter version number
	ver = WIFI_PARA_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Wifi Parameter version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(WifiParameters)
						  != write(fd,&WifiParameters,sizeof(WifiParameters)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Wifi Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	WifiParameterConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitModemParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises Broadband Modem parameters
//*****************************************************************************

void InitModemParaConfig(void)
{
	UINT8		tmpIdx = 0;
	if(SUCCESS != ReadModemParaConfig())
	{
		if(SUCCESS != LoadDfltModemParaConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise Modem Parameters");
		}
	}
	while(tmpIdx < MAX_MODEM_PROFILE)
	{
		if(ModemProfile[tmpIdx].enable == ENABLED)
		{
			SelectedModemProfile = tmpIdx;
			break;
		}
		tmpIdx++;
	}
}

//*****************************************************************************
//	ReadModemParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Modem parameter from file	.
//*****************************************************************************
BOOL ReadModemParaConfig(void)
{
	INT32 		fd,configLen;
	UINT8		ver;
	BOOL		status;

	if((fd = open(SYS_MODEM_PARA_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Modem Parameters");
		return FAIL;
	}

	// Read Modem parameters
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Modem Parameter version");
		close(fd);
		return FAIL;
	}

	// Check whether the versions matches
	if(ver != MODEM_PARA_VER)
	{
		EPRINT(CONFIG_LOG,"Modem Parameter version doesn't matches [%d]", ver);
		status = LoadOldModemPara(fd, ver);
		close(fd);
		return status;
	}

	configLen = (sizeof(MODEM_CONFIG_t) * MAX_MODEM_PROFILE);

	if(configLen != read(fd, &ModemProfile, configLen))
	{
		EPRINT(CONFIG_LOG,"Failed to read Modem Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	ModemProfileConfigDebug(MAX_MODEM_PROFILE);
	return SUCCESS;
}

//*****************************************************************************

//	LoadDfltModemParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Default Modem configuartion parameters.
//*****************************************************************************
BOOL LoadDfltModemParaConfig(void)
{
	UINT8 profileIdx;

	for(profileIdx = 0 ; profileIdx < MAX_MODEM_PROFILE ; profileIdx++)
	{
		ModemProfile[profileIdx] = DfltModemProfile[profileIdx];
	}
	// Remove previously created option files.
	RemoveOptionFile();
	DPRINT(SYS_LOG,"Configuration defaulted - ModemProfile");
	if(WriteModemParaConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "ModemParaConfig");
		return SUCCESS;
	}
	return FAIL;
}

//******************************************************************************
//	WriteModemParaConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Modem parameter into configuration file.
//******************************************************************************
BOOL WriteModemParaConfig(void)
{
	INT32	fd,configLen;
	UINT8	ver;

	if((fd = open(SYS_MODEM_PARA_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Modem Parameters");
		return FAIL;
	}

	// Write Modem parameter version number
	ver = MODEM_PARA_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Modem Parameter version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	configLen = (sizeof(MODEM_CONFIG_t) * MAX_MODEM_PROFILE);

	if(configLen != write(fd, &ModemProfile, configLen))
	{
		EPRINT(CONFIG_LOG,"Failed to write M Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	ModemProfileConfigDebug(MAX_MODEM_PROFILE);
	return SUCCESS;
}

//*****************************************************************************
//	InitCmdCtrlConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Command and Control Centre Parameter.
//*****************************************************************************
void InitCmdCtrlConfig(void)
{
	if(SUCCESS != ReadCmdCtrlConfig())
	{
		if(SUCCESS != LoadDfltCmdCtrlConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise Command and Control Centre Parameters");
		}
	}
}

//*****************************************************************************
//	ReadCmdCtrlConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Command and Control Centre parameter from file.
//*****************************************************************************
BOOL ReadCmdCtrlConfig(void)
{
	INT32 		fd;
	UINT8		ver;
	BOOL		status;

	if( (fd = open(SYS_COMMAND_CTRL_FILE , O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Command and Control Centre Parameters");
		return FAIL;
	}

	// Read network parameters
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Command and Control Centre Parameter version");
		close(fd);
		return FAIL;
	}

	// Check whether the versions matches
	if(ver != COMMAND_CTRL_PARA_VER)
	{
		EPRINT(CONFIG_LOG,"Command and Control Centre Parameter version doesn't matches");
		status = LoadOldCmdCtrlConfig(fd, ver);
		close(fd);
		return status;
	}

	if(sizeof(COMMAND_CTRL_CONFIG_t) != read(fd, &CmdCtrlConfig, sizeof(COMMAND_CTRL_CONFIG_t)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Command and Control Centre Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	CommandControlConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltCmdCtrlConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Command and Control Centre parameters
//*****************************************************************************
BOOL LoadDfltCmdCtrlConfig(void)
{
	CmdCtrlConfig = DfltCmdCtrlConfig;
	DPRINT(SYS_LOG,"Configuration defaulted - CmdCtrlConfig");
	if(WriteCmdCtrlConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "CmdCtrlConfig");
		return SUCCESS;
	}
	return FAIL;
}

//******************************************************************************
//	WriteCmdCtrlConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Command and Control Centre Parameter into configuration
//				file.
//******************************************************************************
BOOL WriteCmdCtrlConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if( (fd = open(SYS_COMMAND_CTRL_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Command and Control Centre Parameters");
		return FAIL;
	}

	// Write network parameter version number
	ver = COMMAND_CTRL_PARA_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Command and Control Centre Parameter version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(COMMAND_CTRL_CONFIG_t) != write(fd,&CmdCtrlConfig,
													sizeof(COMMAND_CTRL_CONFIG_t)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Command and Control Centre Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	CommandControlConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitTwoPersonConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises Two Person Configuration
//*****************************************************************************
void InitTwoPersonConfig(void)
{
	if(SUCCESS != ReadTwoPersonConfig())
	{
		if(SUCCESS != LoadDfltTwoPersonConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise Two Person Configuration");
		}
	}
}

//*****************************************************************************
//	ReadTwoPersonrConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Two Person configuration from file
//*****************************************************************************
BOOL ReadTwoPersonConfig(void)
{
	INT32 		fd;
	UINT8		ver;

	if((fd = open(SYS_TWO_PERSON_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Two Person Configuration File");
		return FAIL;
	}

	// Read two person version
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Two Person Configuration version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != TWO_PERSON_CFG_VER)
	{
		EPRINT(CONFIG_LOG,"Two Person Configuration version doesn't match");
		close(fd);
		return FAIL;
	}

	// Read configuration
	if(sizeof(TwoPersonGroup) != read(fd,TwoPersonGroup,sizeof(TwoPersonGroup)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Two Person Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	TwoPersonGroupConfigDebug(MAX_TWO_PERSON_GROUP);
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltTwoPersonConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Two Person configuration
//*****************************************************************************
BOOL LoadDfltTwoPersonConfig(void)
{
	UINT8 twoprsn;

	for(twoprsn = 0 ; twoprsn < MAX_TWO_PERSON_GROUP ; twoprsn++)
	{
		TwoPersonGroup[twoprsn] = DfltTwoPerson;
	}

	SendDfltConfigEvent(TWO_PERSON_TABLE_ID);

	if(WriteTwoPersonConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "TwoPersonConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteSingleTwoPersonConfig()
//	Param:
//		IN : index		- index which needs to be written
//		OUT: NONE
//	Returns:
//			SUCCESS - If successful
//			FAILURE otherwise
//	Description:
//			Writes single two person record into configuration file.
//*********************************************************************************
BOOL WriteSingleTwoPersonConfig(UINT8 index)
{
	INT32 	fd;

	// If requested index is greater than maximum limit, no need to write it in file
	if(index >= MAX_TWO_PERSON_GROUP)
	{
		return FAIL;
	}

	if((fd = open(SYS_TWO_PERSON_FILE, O_WRONLY | O_SYNC)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open guard tour file");
		return FAIL;
	}

	// Set pointer to write requested index, consider version number byte
	lseek(fd,((index * (sizeof(TWO_PERSON_GROUP_t))) + 1),SEEK_SET);

	if((sizeof(TwoPersonGroup[0]))
				!= write(fd,&TwoPersonGroup[index],sizeof(TwoPersonGroup[0])))
	{
		EPRINT(CONFIG_LOG,"Failed to write two person record for the given index");
		close(fd);
		return FAIL;
	}

	close(fd);
	DPRINT(CONFIG_LOG,"index %d written successfully",index);
	TwoPersonGroupConfigDebug(index);
	return SUCCESS;
}

//*********************************************************************************
//	WriteTwoPersonConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Two Person config into configuration file.
//*********************************************************************************
BOOL WriteTwoPersonConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_TWO_PERSON_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Two Person Configuration File");
		return FAIL;
	}

	// Write two person group version
	ver = TWO_PERSON_CFG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Two Person Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(TwoPersonGroup) != write(fd,TwoPersonGroup,sizeof(TwoPersonGroup)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Two Person Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	TwoPersonGroupConfigDebug(MAX_TWO_PERSON_GROUP);
	return SUCCESS;
}

//*****************************************************************************
//	GetSysEnrolledFpCnt()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - YES
//			 Otherwise - NO
//	Description:
//			It updates total enrolled fp counts in the system.
//*****************************************************************************
void GetSysEnrolledFpCnt(void)
{
	DIR		*fpDir;
	struct dirent *ent;
	struct dirent dirBuf;

// do not use ls it will not work when there is too many files in folder

	sysFpEnrolledCnt = 0;

	fpDir = opendir(SYS_FP_TEMPLATE_DIR);

	if(fpDir != NULL)
	{
		while(1)
		{
			readdir_r(fpDir, &dirBuf, &ent);
			if(ent == 0)
				break;
			if(strncmp(ent -> d_name, "UserFP", 6) == 0)
			{
				sysFpEnrolledCnt++;
			}
		}
		closedir(fpDir);
	}

	DPRINT(CONFIG_LOG, "Total no. of finger templates = %d", sysFpEnrolledCnt);
//	printf("GetSysEnrolledFpCnt() elapsedTicks = [%u]\nsysFpEnrolledCnt=[%u]\n", ElapsedTick(currectionTick), sysFpEnrolledCnt);
}

//*****************************************************************************
//	InitVisitorMngmentConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises visitor management configuration.
//*****************************************************************************
void InitVisitorMngmentConfig(void)
{
	UINT16 pg;
    BOOL firstTimeBootF = !IsConfigFilePresent("MxVisitorMngmntPg");
    if(firstTimeBootF)
        DPRINT(SYS_LOG, "First time booting, All visitor mngmnt config files will be created");
	// Since  There are multiple vositors supported,multiple files are required
	for(pg = 0 ; pg <= TOTAL_VISITOR_PAGES ; pg++)
	{
        if(SUCCESS != ReadVisitorMngmentConfig(pg,firstTimeBootF))
		{
			if(SUCCESS != LoadDfltVisitorMngmentConfig(pg))
			{
				EPRINT(CONFIG_LOG,"Failed to intialise Visitor Management");
			}
		}
	}
}

//*****************************************************************************
//	ReadVisitorMngmentConfig()
//	Param:
//		IN : pg		- Page to read
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Visitor Group from configuration file
//*****************************************************************************
BOOL ReadVisitorMngmentConfig(UINT16 pg,BOOL firstTimeBootF)
{
	INT32 	fd;
	CHAR 	filePath[MAX_CNFG_FILE_NAME_SZ];
	UINT8 	recordCount;
	UINT16 	recordOffset;
	UINT8	ver, status;

	//file name
	sprintf(filePath, "%s%d%s", SYS_VISITOR_MNGMENT_FILE, (pg+1), SYS_CFG_FILE_EXT);

	//Start index from where to store in RAM
	recordOffset = (pg * VISITORS_PER_PAGE);

	if(pg == TOTAL_VISITOR_PAGES)		//If last page
	{
		recordCount = (MAX_VISITORS - (pg * VISITORS_PER_PAGE));
	}
	else	//If not last page
	{
		recordCount = VISITORS_PER_PAGE;
	}

    if((fd = open(filePath, O_RDWR | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
        if(firstTimeBootF == FALSE)
		EPRINT(CONFIG_LOG,"Failed to open Visitor Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Visitor Configuration version page[%d]", pg);
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != VISITOR_MANAGEMENT_VER)
	{
		EPRINT(CONFIG_LOG,"Visitor Configuration version doesn't match page[%d] read ver[%d]", pg, ver);
		status = LoadOldVisitorMangmentConfig(fd, ver, recordOffset, recordCount);
		close(fd);
		return status;
	}

	// Read configuration
	if((recordCount*sizeof(Visitor[0]))
			!= (UINT32)read(fd,&Visitor[recordOffset],(recordCount*sizeof(Visitor[0]))))
	{
		EPRINT(CONFIG_LOG,"Failed to read Visitor Configuration");
		close(fd);
		return FAIL;
	}
	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltVisitorMngmentConfig()
//	Param:
//		IN : pg		- Page for which to load default
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Visitor Group Configuration
//*****************************************************************************
BOOL LoadDfltVisitorMngmentConfig(UINT16 pg)
{
	UINT8 	recordCount;
	UINT16 	index;
	UINT16	recordOffset;

	recordOffset = (pg * VISITORS_PER_PAGE);		//Start of index

	//Length of index
	//If last page
	if(pg == TOTAL_VISITOR_PAGES)
	{
		recordCount = (MAX_VISITORS - (pg * VISITORS_PER_PAGE));
	}
	else	//If not last page
	{
		recordCount = VISITORS_PER_PAGE;
	}

	for(index = recordOffset ; index < (recordOffset + recordCount); index++)
	{
		Visitor[index] = DfltVisitor;
	}

	// If fisrt page, then only Generate event
	if(pg == 0)
	{
		SendDfltConfigEvent(VISITOR_TABLE_ID);
	}

	return WriteVisitorMngmentConfig(pg);
}

//*****************************************************************************
//	LoadDfltAllVisitorMngmentConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Visitor Group Configuration
//*****************************************************************************
static VOID LoadDfltAllVisitorMngmentConfig(VOID)
{
	UINT16	pg;

	for(pg = 0; pg <= TOTAL_VISITOR_PAGES; pg++)
	{
		LoadDfltVisitorMngmentConfig(pg);
	}
	CONFIGPRINT(CONFIG_DEFAULT, "AllVisitorMngmentConfig");
}

//*********************************************************************************
//	WriteSingleVisitorMngmentConfig()
//	Param:
//		IN : 	Index - Index for which to load config
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes visitor config into configuration file for single visitor.
//*********************************************************************************
BOOL WriteSingleVisitorMngmentConfig(UINT16 index)
{
	UINT16 	pg;
	INT32 	fd;
	CHAR 	filePath[MAX_CNFG_FILE_NAME_SZ];
	UINT16	recordOffset;

	// If requested index is greater than maximum limit, no need to write it in file
	if(index >= MAX_VISITORS)
	{
		return FAIL;
	}

	pg = (index / VISITORS_PER_PAGE);

	//file name
	sprintf(filePath, "%s%d%s", SYS_VISITOR_MNGMENT_FILE, (pg+1), SYS_CFG_FILE_EXT);

	//Start index in file
	recordOffset = (pg * VISITORS_PER_PAGE);

	if((fd = open(filePath, O_WRONLY | O_SYNC)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open visitor configuration file");
		return FAIL;
	}

	// Set pointrer to write requested index
	lseek(fd, (((index - recordOffset) * (sizeof(Visitor[0]))) + 1), SEEK_SET);

	if((sizeof(Visitor[0])) != write(fd,&Visitor[index],sizeof(Visitor[0])))
	{
		EPRINT(CONFIG_LOG,"Failed to write visitor configuration for the given index");
		close(fd);
		return FAIL;
	}

	DPRINT(CONFIG_LOG,"visitor:Index %d is written in the page %d",index,pg);
	close(fd);
	VisitorConfigDebug(index);
	return SUCCESS;
 }

//*********************************************************************************
//	WriteVisitorMngmentConfig()
//	Param:
//		IN : pg		- Page for which to load default
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Visitor Group into configuration file.
//*********************************************************************************
BOOL WriteVisitorMngmentConfig(UINT16 pg)
{
	INT32 	fd;
	CHAR 	filePath[MAX_CNFG_FILE_NAME_SZ];
	UINT8 	recordCount;
	UINT16 	recordOffset;
	UINT8	ver;

	//file name
	sprintf(filePath, "%s%d%s", SYS_VISITOR_MNGMENT_FILE, (pg+1), SYS_CFG_FILE_EXT);

	//Start index Number in file
	recordOffset = (pg * VISITORS_PER_PAGE);

	if(pg == TOTAL_VISITOR_PAGES)		//If last page
	{
		//Remainig entries in file
		recordCount = (MAX_VISITORS - (pg * VISITORS_PER_PAGE));
	}
	else	//If not last page
	{
		recordCount = VISITORS_PER_PAGE;
	}

	if((fd = open(filePath, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Visitor Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Write index number
	ver = VISITOR_MANAGEMENT_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Visitor Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if((recordCount*sizeof(Visitor[0])) != (UINT32)write(fd,&Visitor[recordOffset],(recordCount*sizeof(Visitor[0]))))
	{
		EPRINT(CONFIG_LOG,"Failed to write Visitor Configuration");
		close(fd);
		return FAIL;
	}
	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	InitSpecialCardsConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Special Cards and Codes.
//*****************************************************************************
void InitSpecialFunctionsConfig(void)
{
	if(SUCCESS != ReadSpecialFunctionsConfig())
	{
		if(SUCCESS != LoadDfltSpecialFunctionsConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise System's Special Cards");
		}
	}
}

//*****************************************************************************
//	ReadSpecialFunctionsConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Special Cards and Codes from file
//*****************************************************************************
BOOL ReadSpecialFunctionsConfig(void)
{
	INT32 	fd;
	UINT8	ver;
	BOOL	status;

	if((fd = open(SYS_SPECIAL_FUNCTION_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Special Functions");
		return FAIL;
	}

	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Special Functions' version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != SPECIAL_FUNCTION_VER)
	{
		EPRINT(CONFIG_LOG,"Special Functions' version doesn't match");
		status = LoadOldSpecialFunctionConfig(fd, ver);
		close(fd);
		return status;
	}

	// Read configuration
	if(sizeof(SpecialFunctions) != read(fd,&SpecialFunctions,sizeof(SpecialFunctions)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Special Function configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	SpecialFunctionsConfigDebug(MAX_SPECIAL_FUNC);
	return SUCCESS;
}

//*********************************************************************************
//	LoadDfltSpecialFunctionsConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Special Cards and Codes.
//*********************************************************************************
BOOL LoadDfltSpecialFunctionsConfig(void)
{
	UINT8 idx;

	for(idx = 0 ; idx < MAX_SPECIAL_FUNC ; idx++)
	{
		SpecialFunctions[idx] = DfltSpecialFunc;
	}

	SendDfltConfigEvent(SPECIAL_FUNC_TABLE_ID);

	if(WriteSpecialFunctionsConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "SpecialFunctionsConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteSingleSpecialFunctionsConfig()
//	Param:
//		IN : 	Index - Index for which to write config
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes cards and code config into configuration file for single index.
//*********************************************************************************
BOOL WriteSingleSpecialFunctionsConfig(UINT8 index)
{
	INT32 fd;

	// If requested index is greater than maximum limit, no need to write it in file
	if(index >= MAX_SPECIAL_FUNC)
	{
		return FAIL;
	}

	if((fd = open(SYS_SPECIAL_FUNCTION_FILE, O_WRONLY | O_SYNC)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Special Function file");
		return FAIL;
	}

	// Set pointer to write requested index, consider version number byte
	lseek(fd, ((index * (sizeof(SPECIAL_FUNCTION_t))) + 1), SEEK_SET);

	if((sizeof(SpecialFunctions[0]))
			!= write(fd,&SpecialFunctions[index],sizeof(SpecialFunctions[0])))
	{
		EPRINT(CONFIG_LOG,"Failed to write Function for the given index");
		close(fd);
		return FAIL;
	}

	DPRINT(CONFIG_LOG,"spl func:index %d is written",index);
	close(fd);
	SpecialFunctionsConfigDebug(index);
	return SUCCESS;
}

//*********************************************************************************
//	WriteSpecialFunctionsConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Special Cards and Codes into configuration file.
//*********************************************************************************
BOOL WriteSpecialFunctionsConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_SPECIAL_FUNCTION_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Special Function file");
		return FAIL;
	}

	// Write version number
	ver = SPECIAL_FUNCTION_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Special Functions' version");
		close(fd);
		return FAIL;
	}

	if(sizeof(SpecialFunctions)
					  != write(fd,&SpecialFunctions,sizeof(SpecialFunctions)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Special Function");
		close(fd);
		return FAIL;
	}

	close(fd);
	SpecialFunctionsConfigDebug(MAX_SPECIAL_FUNC);
	return SUCCESS;
}

//*****************************************************************************
//	InitTimeTrigFuncConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises Time Trigger Functions Groups.
//*****************************************************************************
void InitTimeTrigFuncConfig(void)
{
	if(SUCCESS != ReadTimeTrigFuncConfig())
	{
		if(SUCCESS != LoadDfltTimeTrigFuncConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise Time Triggered Function Configuration");
		}
	}
}

//*****************************************************************************
//	ReadTimeTrigFuncConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Time Trigger Function from configuration file
//*****************************************************************************
BOOL ReadTimeTrigFuncConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	if((fd = open(SYS_TIME_TRIG_FUNC_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Time Triggered Function Configuration file");
		return FAIL;
	}

	// Successfully opened file
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Time Triggered Function Configuration version");
		close(fd);
		return FAIL;
	}

	// Check whether the version number matches
	if(ver != TIME_TRIG_FUNC_CFG_VER)
	{
		EPRINT(CONFIG_LOG,"Time Triggered Function Configuration version doesn't match");
		close(fd);
		return FAIL;
	}

	// Read configuration
	if(sizeof(TimeTrigFunc) != read(fd, TimeTrigFunc, sizeof(TimeTrigFunc) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read Time Triggered Function Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	TimeTrigerFuncConfigDebug(MAX_TIME_TRIG_FUNC);
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltTimeTrigFuncConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Time Triggered Function Configuration.
//*****************************************************************************
BOOL LoadDfltTimeTrigFuncConfig(void)
{
	UINT8 	configIdx;

	for(configIdx = 0 ; configIdx < MAX_TIME_TRIG_FUNC ; configIdx++)
	{
		TimeTrigFunc[configIdx] = DfltTimeTrigFunc;
	}

	SendDfltConfigEvent(TIME_TRIG_TABLE_ID);

	if(WriteTimeTrigFuncConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "TimeTrigFuncConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteTimeTrigFuncConfig()
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Time Trigger Functions into configuration file.
//*********************************************************************************
BOOL WriteTimeTrigFuncConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_TIME_TRIG_FUNC_FILE,O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Time Triggered Function Configuration  file");
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = TIME_TRIG_FUNC_CFG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Time Triggered Function Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(TimeTrigFunc) != write(fd, TimeTrigFunc, sizeof(TimeTrigFunc) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write Time Triggered Function Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	TimeTrigerFuncConfigDebug(MAX_TIME_TRIG_FUNC);
	return SUCCESS;
}

//*****************************************************************************
//	InitIoLinkConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Io Linking.
//*****************************************************************************
void InitIoLinkConfig(void)
{
	if(SUCCESS != ReadIoLinkConfig())
	{
		if(SUCCESS != LoadDfltIoLinkConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise System's Io Linking");
		}
	}
}

//*****************************************************************************
//	ReadIoLinkConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Io Linking from file
//*****************************************************************************
BOOL ReadIoLinkConfig(void)
{
	INT32 	fd;
	UINT8	ver;
	BOOL	returnValue;

	if((fd = open(SYS_IO_LINKING_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Io Linking");
		return FAIL;
	}

	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Io Linking version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != IO_LINKING_VER)
	{
		EPRINT(CONFIG_LOG,"Io Linking version doesn't match");
		returnValue = LoadOldIoLinkConfig(fd, ver);
		close(fd);
		return returnValue;
	}

	// Read configuration
	if(sizeof(IoLinkConfig) != read(fd, &IoLinkConfig, sizeof(IoLinkConfig)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Io Link configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	IoLinkConfigDebug(MAX_IO_LINK);
	return SUCCESS;
}

//*********************************************************************************
//	LoadDfltIoLinkConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Io Linking
//*********************************************************************************
BOOL LoadDfltIoLinkConfig(void)
{
	UINT8 idx;

	for(idx = 0 ; idx < MAX_IO_LINK; idx++)
	{
		IoLinkConfig[idx] = DefltIoLinkConfig;
	}

	SendDfltConfigEvent(IO_LINK_TABLE_ID);

	if(WriteIoLinkConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "IoLinkConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteSingleIoLinkConfig()
//	Param:
//		IN : 	Index - Index for which to write config
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Io Linking config into configuration file for single index.
//*********************************************************************************
BOOL WriteSingleIoLinkConfig(UINT8 index)
{
	INT32 fd;
	// If requested index is greater than maximum limit, no need to write it in file
	if(index >= MAX_IO_LINK)
	{
		return FAIL;
	}

	if((fd = open(SYS_IO_LINKING_FILE, O_WRONLY | O_SYNC)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Io Linking file");
		return FAIL;
	}

	// Set pointer to write requested index, consider version number byte
	lseek(fd, ((index * (sizeof(IO_LINK_CONFIG_t))) + 1), SEEK_SET);

	if((sizeof(IoLinkConfig[0])) != write(fd,&IoLinkConfig[index],sizeof(IoLinkConfig[0])))
	{
		EPRINT(CONFIG_LOG,"Failed to write Function for the given index");
		close(fd);
		return FAIL;
	}

	DPRINT(CONFIG_LOG,"Io Linking:index %d is written",index);
	close(fd);
	IoLinkConfigDebug(index);
	return SUCCESS;
}

//*********************************************************************************
//	WriteIoLinkConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Io Linking into configuration file.
//*********************************************************************************
BOOL WriteIoLinkConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_IO_LINKING_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Io Linking file");
		return FAIL;
	}

	// Write version number
	ver = IO_LINKING_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Io Linking' version");
		close(fd);
		return FAIL;
	}

	if(sizeof(IoLinkConfig) != write(fd, &IoLinkConfig, sizeof(IoLinkConfig)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Io Linking");
		close(fd);
		return FAIL;
	}

	close(fd);
	IoLinkConfigDebug(MAX_IO_LINK);
	return SUCCESS;
}

//*****************************************************************************
//	InitIoLinkExtDevConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Io Link External Device Configuration.
//*****************************************************************************
void InitIoLinkExtDevConfig(void)
{
	if(SUCCESS != ReadIoLinkExtDevConfig())
	{
		if(SUCCESS != LoadDfltIoLinkExtDevConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise System's Io Link External Device");
		}
	}
}

//*****************************************************************************
//	ReadIoLinkExtDevConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Io Link External Device from file
//*****************************************************************************
BOOL ReadIoLinkExtDevConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	if((fd = open(SYS_IO_LINK_EXT_DEV_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Io Link External Device file");
		return FAIL;
	}

	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Io Link External Device version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != IO_LINK_EXT_DEV_VER)
	{
		EPRINT(CONFIG_LOG,"Io Link External Device version mismatch");
		close(fd);
		return FAIL;
	}

	// Read configuration
	if(sizeof(IoLinkExtDevConfig) != read(fd, &IoLinkExtDevConfig, sizeof(IoLinkExtDevConfig)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Io Link External Device configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	return SUCCESS;
}

//*********************************************************************************
//	LoadDfltIoLinkExtDevConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Io Link External Device
//*********************************************************************************
BOOL LoadDfltIoLinkExtDevConfig(void)
{
	UINT8 idx;

	for(idx = 0 ; idx < MAX_IO_LINK_EXT_DEVICE; idx++)
	{
		IoLinkExtDevConfig[idx] = DefltIoLinkExtDevConfig;
	}

	SendDfltConfigEvent(IO_LINK_EXT_DEVICE_CONFIG_TABLE_ID);

	if(WriteIoLinkExtDevConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "IoLinkExtDevConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteSingleIoLinkExtDevConfig()
//	Param:
//		IN : 	Index - Index for which to write config
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Io Link External Device config into config file for single index.
//*********************************************************************************
BOOL WriteSingleIoLinkExtDevConfig(UINT8 index)
{
	INT32 fd;
	// If requested index is greater than maximum limit, no need to write it in file
	if(index >= MAX_IO_LINK_EXT_DEVICE)
	{
		return FAIL;
	}

	if((fd = open(SYS_IO_LINK_EXT_DEV_FILE, O_WRONLY | O_SYNC)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Io Link External Device file");
		return FAIL;
	}

	// Set pointer to write requested index, consider version number byte
	lseek(fd, ((index * (sizeof(IO_LINK_EXT_DEV_CONFIG_t))) + 1), SEEK_SET);

	if((sizeof(IO_LINK_EXT_DEV_CONFIG_t)) != write(fd,&IoLinkExtDevConfig[index],sizeof(IO_LINK_EXT_DEV_CONFIG_t)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Io Link External Device config for the given index");
		close(fd);
		return FAIL;
	}

	DPRINT(CONFIG_LOG,"Io Link External Device:index %d is written",index);
	close(fd);
	IoLinkExtentionDevConfigDebug(index);
	return SUCCESS;
}

//*********************************************************************************
//	WriteIoLinkExtDevConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Io Link External Device into configuration file.
//*********************************************************************************
BOOL WriteIoLinkExtDevConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_IO_LINK_EXT_DEV_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Io Link External Device file");
		return FAIL;
	}

	// Write version number
	ver = IO_LINK_EXT_DEV_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Io Link External Device' version");
		close(fd);
		return FAIL;
	}

	if(sizeof(IoLinkExtDevConfig) != write(fd, &IoLinkExtDevConfig, sizeof(IoLinkExtDevConfig)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Io Link External Device");
		close(fd);
		return FAIL;
	}

	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	InitIoLinkExtDevConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Io Link External Device Mapping Info.
//*****************************************************************************
void InitIoLinkExtDevMapping(void)
{
	if(SUCCESS != ReadIoLinkExtDevMapping())
	{
		if(SUCCESS != LoadDfltIoLinkExtDevMapping())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise System's Io Link External Device Mapping Info");
		}
	}
}

//*****************************************************************************
//	ReadIoLinkExtDevMapping()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Io Link External Device Mapping info from file
//*****************************************************************************
BOOL ReadIoLinkExtDevMapping(void)
{
	INT32 	fd;
	UINT8	ver;

	if((fd = open(SYS_IO_LINK_EXT_DEV_MAPPING_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Io Link External Device Mapping file");
		return FAIL;
	}

	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Io Link External Device Mapping version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != IO_LINK_EXT_DEV_MAPPING_VER)
	{
		EPRINT(CONFIG_LOG,"Io Link External Device version Mapping mismatch");
		close(fd);
		return FAIL;
	}

	// Read configuration
	if(sizeof(IoLinkExtDevMapping) != read(fd, &IoLinkExtDevMapping, sizeof(IoLinkExtDevMapping)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Io Link External Device Mapping information");
		close(fd);
		return FAIL;
	}

	close(fd);
	return SUCCESS;
}

//*********************************************************************************
//	LoadDfltIoLinkExtDevMapping()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Default Io Link External Device Mapping
//*********************************************************************************
BOOL LoadDfltIoLinkExtDevMapping(void)
{
	UINT8 idx;

	for(idx = 0 ; idx < MAX_IO_LINK; idx++)
	{
		IoLinkExtDevMapping[idx] = DefltIoLinkExtDevMapping;
	}

	SendDfltConfigEvent(IO_LINK_EXT_DEVICE_MAPPING_TABLE_ID);

	if(WriteIoLinkExtDevMapping() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "IoLinkExtDevMapping");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteSingleIoLinkExtDevMapping()
//	Param:
//		IN : 	Index - Index for which to write config
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Io Link External Device Mapping info into file for single index.
//*********************************************************************************
BOOL WriteSingleIoLinkExtDevMapping(UINT8 index)
{
	INT32 fd;
	// If requested index is greater than maximum limit, no need to write it in file
	if(index >= MAX_IO_LINK)
	{
		return FAIL;
	}

	if((fd = open(SYS_IO_LINK_EXT_DEV_MAPPING_FILE, O_WRONLY | O_SYNC)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Io Link External Device Mapping file");
		return FAIL;
	}

	// Set pointer to write requested index, consider version number byte
	lseek(fd, ((index * (sizeof(IO_LINK_EXT_DEV_MAPPING_t))) + 1), SEEK_SET);

	if((sizeof(IO_LINK_EXT_DEV_MAPPING_t)) != write(fd,&IoLinkExtDevConfig[index],sizeof(IO_LINK_EXT_DEV_MAPPING_t)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Io Link External Device Mapping for the given index");
		close(fd);
		return FAIL;
	}

	DPRINT(CONFIG_LOG,"Io Link External Device Mapping[%d] is written",index);
	close(fd);
	IoLinkExtDevMappingDebug(index);
	return SUCCESS;
}

//*********************************************************************************
//	WriteIoLinkExtDevConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Io Link External Device into configuration file.
//*********************************************************************************
BOOL WriteIoLinkExtDevMapping(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_IO_LINK_EXT_DEV_MAPPING_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Io Link External Device Mapping file");
		return FAIL;
	}

	// Write version number
	ver = IO_LINK_EXT_DEV_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Io Link External Device Mapping version");
		close(fd);
		return FAIL;
	}

	if(sizeof(IoLinkExtDevMapping) != write(fd, &IoLinkExtDevMapping, sizeof(IoLinkExtDevMapping)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Io Link External Device Mapping");
		close(fd);
		return FAIL;
	}

	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	InitFprConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises FPR Configuration
//*****************************************************************************
void InitFprConfig(void)
{
	if(SUCCESS != ReadFprConfig())
	{
		if(SUCCESS != LoadDfltFprConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise FPR Configuration");
		}
	}
}

//*****************************************************************************
//	ReadFprConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads FPR's configuartion
//*****************************************************************************
BOOL ReadFprConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	if((fd = open(SYS_FPR_CFNG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open FPR Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read FPR Configuration version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != FPR_CFG_VER)
	{
		EPRINT(CONFIG_LOG,"FPR Configuration version doesn't match");
		close(fd);
		return FAIL;
	}

	if((sizeof(FPR_CONFIG_t))!=read(fd,&FprConfig,sizeof(FPR_CONFIG_t)))
	{
		EPRINT(CONFIG_LOG,"Failed to read FPR Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	FingerPrintConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltFprConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default FPR Configuration.
//*****************************************************************************
BOOL LoadDfltFprConfig(void)
{
	FprConfig = DfltFprConfig;

	SendDfltConfigEvent(FPR_CONFIG_TABLE_ID);

	if(WriteFprConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "FprConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteFprConfig()
//	Param:
//		IN : pg		- Page for which to load default
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes FPR configuration into file.
//*********************************************************************************
BOOL WriteFprConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	if((fd = open(SYS_FPR_CFNG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Door Ccontrol Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = FPR_CFG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write FPR Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if((sizeof(FPR_CONFIG_t)) != write(fd,&FprConfig,(sizeof(FPR_CONFIG_t))))
	{
		EPRINT(CONFIG_LOG,"Failed to write FPR configuartion into file");
		close(fd);
		return FAIL;
	}

	close(fd);
	FingerPrintConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitReadersParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises Readers parameter Configuration
//*****************************************************************************
void InitReadersParaConfig(void)
{
	if(SUCCESS != ReadReadersParaConfig())
	{
		if(SUCCESS != LoadDfltReadersParaConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to initialise Reader Parameters Configuration");
		}
	}
}

//*****************************************************************************
//	ReadReadersParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads FPR's configuration
//*****************************************************************************
BOOL ReadReadersParaConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	fd = open(SYS_READER_PARA_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Readers Parameter Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Readers Parameter Configuration version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != READER_PARA_CFG_VER)
	{
		EPRINT(CONFIG_LOG,"Readers Parameter Configuration version doesn't match");
		close(fd);
		return FAIL;
	}

	if((sizeof(READERS_PARAMETER_t))!=read(fd,&ReadersParaConfig,sizeof(READERS_PARAMETER_t)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Readers Parameter Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	ReadersParaConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDefltReadersParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//		This function will load default reader parameters configuration.
//*****************************************************************************
BOOL LoadDfltReadersParaConfig(void)
{
	GetSM125DefltRadfValue();
	// Write default configuration into non-volatile memory
	DPRINT(SYS_LOG,"Configuration defaulted - ReadersParaConfig");
	if(WriteReadersParaConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "ReadersParaConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteReadersParaConfig()
//	Param:
//		IN : pg		- Page for which to load default
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes FPR configuration into file.
//*********************************************************************************
BOOL WriteReadersParaConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	if((fd = open(SYS_READER_PARA_CNFG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Readers Parameter Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = READER_PARA_CFG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Readers Parameter Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if((sizeof(READERS_PARAMETER_t)) != write(fd,&ReadersParaConfig,(sizeof(READERS_PARAMETER_t))))
	{
		EPRINT(CONFIG_LOG,"Failed to write Readers Parameter configuration into file");
		close(fd);
		return FAIL;
	}

	close(fd);
	ReadersParaConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	WriteUserFpIntoFile
//	Param:
//		IN :	userIdx	- user index for which finger print template is to be write
//				fpIdx	- index of finger print template (0, .. , (max user fp-1) )
//				dataPtr	- points to data that is to be written
//		OUT:	none
//	Returns:	FILE_WRITE_FAIL	- template can't be written
//				FILE_WRITE_SUCCESS	- template written successfully
//				FILE_WRITE_FAIL_NO_MEMORY - system fp template memory is full
//	Description:
//			It stores given finger print template data for a given user into file.
//
//*****************************************************************************
FILE_WRITE_STATUS_e WriteUserFpIntoFile(UINT32 userIdx, UINT8 fpIdx, VOIDPTR dataPtr)
{
	INT32 	fd;
	CHAR 	filePath[MAX_CNFG_FILE_NAME_SZ];
	BOOL	fileAlreadyPresentF = FALSE;

	//if fingerprint template format does not match do not write it
	if(CheckFprTemplateFormat(dataPtr) == FAIL)
	{
		EPRINT(DC_LOG, "Template Format does not matched [%s] FpIdx [%d]", GetAlphaNumId(userIdx), (fpIdx+1));
		return FILE_WRITE_FORMAT_CHECK_FAIL;
	}

	// If all fp template memory is used, we won't store any more template
	//for panel door and direct door fp capacity is same
	if(sysFpEnrolledCnt >= TOTAL_SYS_FP_CAPACITY)
	{
		EPRINT(DC_LOG, "Memory Full, failed to store user finger print file for User [%s] FpIdx [%d]", GetAlphaNumId(userIdx), (fpIdx+1));
		return FILE_WRITE_FAIL_NO_MEMORY;
	}

	//For naming the file
	//for panel door userIdx is taken as userId
	GetFpTemplateFilePath(filePath, userIdx, (fpIdx+1));

	if(access(filePath, F_OK) == 0)
	{
		fileAlreadyPresentF = TRUE;
	}

	if((fd = open(filePath, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT,S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(DC_LOG, "Failed to create user finger print file for User [%s] FpIdx [%d]", GetAlphaNumId(userIdx), (fpIdx+1));
		return FILE_WRITE_FAIL;
	}

	if(MAX_SIZE_SINGLE_SFM_TEMPLATE != write(fd, dataPtr, MAX_SIZE_SINGLE_SFM_TEMPLATE) )
	{
		EPRINT(DC_LOG, "Failed to write user finger print file for User [%s] FpIdx [%d]", GetAlphaNumId(userIdx), (fpIdx+1));
		close(fd);
		// We don't want half cooked thing. So, remove file that was just created.
		DeleteSingleUserFpFiles(userIdx, fpIdx);
		return FILE_WRITE_FAIL;
	}
	else
	{
		close(fd);

		if(fileAlreadyPresentF != TRUE)
		{
			sysFpEnrolledCnt++;
		}

		return FILE_WRITE_SUCCESS;
	}
}

//*****************************************************************************
//	WriteTempUserFpIntoFile
//	Param:
//		IN :	dataPtr	- points to data that is to be written
//		OUT:	none
//	Returns:	FILE_WRITE_FAIL	- template can't be written
//				FILE_WRITE_SUCCESS	- template written successfully
//				FILE_WRITE_FAIL_NO_MEMORY - system fp template memory is full
//	Description:
//			It stores given finger print template data for a given user into file.
//
//*****************************************************************************
FILE_WRITE_STATUS_e WriteTempUserFpIntoFile(VOIDPTR dataPtr)
{
	INT32 	fd;
	CHAR 	filePath[MAX_CNFG_FILE_NAME_SZ];
	BOOL	fileAlreadyPresentF = FALSE;

	//if fingerprint template format does not match do not write it
	if(CheckFprTemplateFormat(dataPtr) == FAIL)
	{
		EPRINT(DC_LOG, "Template Format does not matched ");
		return FILE_WRITE_FORMAT_CHECK_FAIL;
	}

	// If all fp template memory is used, we won't store any more template
	//for panel door and direct door fp capacity is same
	if(sysFpEnrolledCnt >= TOTAL_SYS_FP_CAPACITY)
	{
		EPRINT(DC_LOG, "Memory Full, failed to store user finger print file for Temporary User");
		return FILE_WRITE_FAIL_NO_MEMORY;
	}

	//For naming the file
	//for panel door userIdx is taken as userId
	sprintf(filePath, "%s/TempUserFP",SYS_TEMP_USER_DIR);

	if(access(filePath, F_OK) == 0)
	{
		fileAlreadyPresentF = TRUE;
	}


	if((fd = open(filePath, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT,S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(DC_LOG, "Failed to create user finger print file for Temporary User ");
		return FILE_WRITE_FAIL;
	}

	if(MAX_SIZE_SINGLE_SFM_TEMPLATE != write(fd, dataPtr, MAX_SIZE_SINGLE_SFM_TEMPLATE) )
	{
		EPRINT(DC_LOG, "Failed to write user finger print file for Temporary User ");
		close(fd);
		// We don't want half cooked thing. So, remove file that was just created.
		unlink(filePath);
		return FILE_WRITE_FAIL;
	}
	else
	{
		close(fd);
		return FILE_WRITE_SUCCESS;
	}
}

//*****************************************************************************
//	ReadUserFpFromFile
//	Param:
//		IN :	userIdx	- user index for which finger print template is to be read
//				fpIdx	- index of finger print template
//		OUT:	dataPtr	- where to copy read data
//	Returns:	SUCCESS - if read successfully, Else FAIL
//	Description:
//			It reads specified finger print template of a given user.
//
//*****************************************************************************
BOOL ReadUserFpFromFile(UINT32 userIdx, UINT8 fpIdx, VOIDPTR dataPtr)
{
	INT32 		fd;
	CHAR 		filePath[MAX_CNFG_FILE_NAME_SZ];
	UINT32		tUserRefId = 0;

	if(GetDoorApplType() == DIRECT_DOOR_APPL)
	{
		if(userIdx < MAX_USERS)
		{
			tUserRefId = GetUserId(userIdx);
		}
		else
		{
			EPRINT(DC_LOG, "userIdx [%u] received is larger than MAX_USERS", userIdx);
			return FAIL;
		}
	}
	else
	{
		tUserRefId = userIdx;
	}

	//For naming the file
	GetFpTemplateFilePath(filePath, userIdx, (fpIdx+1));

	if((fd = open(filePath, O_RDONLY | O_SYNC,S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		DPRINT(DC_LOG, "Failed to open User finger print file for User [%u] fpIdx [%d]", tUserRefId, fpIdx);
		return FAIL;
	}
	//File opened successfully
	else
	{
		if(MAX_SIZE_SINGLE_SFM_TEMPLATE != read(fd, dataPtr, MAX_SIZE_SINGLE_SFM_TEMPLATE) )
		{
			EPRINT(DC_LOG, "Failed to read User finger print file for User [%u] fpIdx [%d]", tUserRefId, fpIdx);
			close(fd);
			return FAIL;
		}
		else
		{
			close(fd);
			return SUCCESS;
		}
	}
}

//*****************************************************************************
//	DeleteSingleUserFpFiles
//	Param:
//		IN :	userIdx	- user index for which finger print template is to be deleted
//				fpIdx	- index of finger print to be deleted if specific
//						  otherwise ALL_USER_FP
//		OUT:	none
//	Returns:	SUCCESS - if deleted successfully, Else FAIL
//	Description:
//			It removes finger print files of a given user from system.
//
//*****************************************************************************
void DeleteSingleUserFpFiles(UINT32 userIdx, UINT8 fpIdx)
{
	UINT8	userFpEnrollAvailableCnt = 0;
	CHAR 	filePath[MAX_CNFG_FILE_NAME_SZ];

	if(fpIdx == ALL_USER_FP)
	{
		for(fpIdx = 0; fpIdx < MAX_USER_FP; fpIdx++)
		{
			if(IsUserFpPresent(userIdx, fpIdx) == YES)
			{
				GetFpTemplateFilePath(filePath, userIdx, (fpIdx+1));
				unlink(filePath);
				userFpEnrollAvailableCnt++;
			}
		}
		// During user initialization, do not get file count, only remove files
		if(UserInitActiVeF == TRUE)
		{
			userFpEnrollAvailableCnt = 0;
		}
	}
	else
	{
		if(IsUserFpPresent(userIdx, fpIdx) == YES)
		{
			GetFpTemplateFilePath(filePath, userIdx, (fpIdx+1));
			unlink(filePath);
			userFpEnrollAvailableCnt++;
		}
		// system command-remove file (rm -f /matrix/fpTemplate/userFPxxxxx_xx)
	}
	// Now decrement total no. of fp enrolled count by no. of fp
	// deleted (which was present before that) for this user
	if(sysFpEnrolledCnt >= userFpEnrollAvailableCnt)
	{
		sysFpEnrolledCnt -= userFpEnrollAvailableCnt;
	}
}

//*****************************************************************************
//	DeleteAllUsersFpFiles
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It removes finger print files of all users from system.
//
//*****************************************************************************
void DeleteAllUsersFpFiles(void)
{
	CHAR 	buf[MAX_SYS_CMD_BUF_SZ], tmpbuf[MAX_SYS_CMD_BUF_SZ];
	UINT8	randNum = 1;

	while(1)
	{
		sprintf(tmpbuf, "%s%d", SYS_TMP_FP_DIR, randNum);
		if( (access(tmpbuf, F_OK) != 0) || (randNum == 255) )
			break;

		randNum++;
	}

	//first we move fp template to another folder to reduce time to delete files
	sprintf(buf, "mv -f %s %s", SYS_FP_TEMPLATE_DIR, tmpbuf);
	system(buf);

	//Create empty fp template folder
	sprintf(buf, "mkdir -p %s", SYS_FP_TEMPLATE_DIR);
	system(buf);

	sprintf(buf, "rm -rf %s &", tmpbuf);
	system(buf);
	// Clear enrolled fp count
	sysFpEnrolledCnt = 0;
}

//*****************************************************************************
//	DeleteAllUsersFpImageFiles
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It removes finger print image files of all users from system.
//
//*****************************************************************************
void DeleteAllUsersFpImageFiles(void)
{
	CHAR 	buf[MAX_SYS_CMD_BUF_SZ], tmpbuf[MAX_SYS_CMD_BUF_SZ];
	UINT8	randNum = 1;

	while(1)
	{
		sprintf(tmpbuf, "%s%d", SYS_TMP_FP_DIR, randNum);
		if( (access(tmpbuf, F_OK) != 0) || (randNum == 255) )
			break;

		randNum++;
	}

	//first we move fp template to another folder to reduce time to delete files
	sprintf(buf, "mv -f %s %s", SYS_FP_IMAGE_DIR, tmpbuf);
	system(buf);

	//Create empty fp template folder
	sprintf(buf, "mkdir -p %s", SYS_FP_IMAGE_DIR);
	system(buf);

	sprintf(buf, "rm -rf %s &", tmpbuf);
	system(buf);

	sprintf(tmpbuf, "rm -rf %s", RECORD_FILE_PATH);
	system(buf);
}
//*****************************************************************************
//	WritePhotoDelSignFile
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It removes finger print files of all users from system.
//
//*****************************************************************************
BOOL WritePhotoDelSignFile()
{
	FILE			*fd;

	fd = fopen(PHOTO_DEL_SIGNATURE_FILE_PATH,"w");
	if(fd == NULL)			//File is not created
	{
		EPRINT(DC_LOG, "Photo Del Signature file Is Not Created.");
		fclose(fd);
		return NO;
	}
	EPRINT(SYS_LOG, "Photo Del Signature file Is Created Success.");
	fclose(fd);
	return YES;
}
//*****************************************************************************
//	DeleteAllUsersPhotoFiles
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It removes finger print files of all users from system.
//
//*****************************************************************************
void DeleteAllUsersPhotoFiles(void)
{
	CHAR 	buf[MAX_SYS_CMD_BUF_SZ], tmpbuf[MAX_SYS_CMD_BUF_SZ];
	UINT8	randNum = 1;

	while(1)
	{
		sprintf(tmpbuf, "%s/%s%d", MmcInfo.devPath, SYS_TMP_PHOTO_DIR, randNum);
		if( (access(tmpbuf, F_OK) != 0) || (randNum == 255) )
			break;

		randNum++;
	}

	//first we move User Pics to another folder to reduce time to delete files
	sprintf(buf, "mv -f %s/%s %s", MmcInfo.devPath, USER_PICS_FOLDER, tmpbuf);
	system(buf);

	//Create empty User Pics folder
	sprintf(buf, "mkdir -p %s/%s", MmcInfo.devPath, USER_PICS_FOLDER);
	system(buf);

	//after moving delete that folder in background
	sprintf(buf, "rm -rf %s &", tmpbuf);
	system(buf);

	//Delete Signature File.
	sprintf(buf, "rm -rf %s &", PHOTO_DEL_SIGNATURE_FILE_PATH);
	system(buf);
}
//*****************************************************************************
//	IsUserFpPresent
//	Param:
//		IN :	userIdx	- user index for which finger print template is to be write
//				fpIdx	- index of finger print template (0, .. , (max user fp-1) )
//		OUT:	none
//	Returns:	YES if present, else NO
//	Description:
//			It checks if finger print for given user is present.
//
//*****************************************************************************
BOOL IsUserFpPresent(UINT32 userIdx, UINT8 fpIdx)
{
	CHAR 		buf[MAX_SYS_CMD_BUF_SZ];
	BOOL		status = 0;

	GetFpTemplateFilePath(buf, userIdx, (fpIdx+1) );
	if(access(buf, F_OK) == 0)
	{
		// File is present
		status = YES;
	}
	return status;
}

//*****************************************************************************
//	IsSysConfigFirstTime
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	YES if required, else NO
//	Description:
//			It checks if any configuration file is present or loading
//			configuration first time.
//*****************************************************************************
static BOOL IsSysConfigFirstTime(void)
{
	BOOL	status = NO;
	UINT16	fileCnt;

	fileCnt = GetFileListInDir(SYS_CONFIG_PATH, NULL, NULL, 0, 1);
	if(fileCnt < 10)
	{
		status = YES;
		DPRINT(SYS_LOG, "System is loading all configuration first time");
	}
	return status;
}

//*****************************************************************************
//	GetEnrolledUserFp
//	Param:
//		IN :	userIdx	- index of user in user configuartion database
//		OUT:	none
//	Returns:	no. of available finger print already enrolled
//	Description:
//			It gives no. of finger print available for enrolled user
//
//*****************************************************************************
UINT8 GetEnrolledUserFp(UINT32 userIdx)
{
	UINT8 	fpCnt = 0, fpIdx;
	CHAR 	filePath[MAX_CNFG_FILE_NAME_SZ];

	for(fpIdx = 0; fpIdx < MAX_USER_FP; fpIdx++)
	{
		GetFpTemplateFilePath(filePath, userIdx, (fpIdx+1));
		if(access(filePath, F_OK) == 0)
		{
			fpCnt++;
		}
	}
	return fpCnt;
}

//*****************************************************************************
//	InitCafeItemConfig
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises Cafeteria Item
//*****************************************************************************
void InitCafeItemConfig(void)
{
	if(SUCCESS != ReadCafeItemConfig())
	{
		if(SUCCESS != LoadDfltCafeItemConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise Cafeteria Item Configuration");
		}
	}
}

//*****************************************************************************
//	ReadCafeItemConfig
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Cafeteria Item from configuration file
//*****************************************************************************
BOOL ReadCafeItemConfig(void)
{
	INT32 	fd;
	UINT8	ver;
	BOOL  	status;

	if((fd = open(SYS_CAFE_ITEM_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Cafe Item Configuration file");
		return FAIL;
	}

	// Successfully opened file
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read cafe Item Configuration version");
		close(fd);
		return FAIL;
	}

	// Check whether the version number matches
	if(ver != CAFE_ITEM_CFG_VER)
	{
		EPRINT(CONFIG_LOG,"Cafe Item Configuration version doesn't match");
		status=LoadOldCafeItemConfig(fd, ver);
		close(fd);
		return status;
	}

	// Read configuration
	if(sizeof(CafeItemConfig) != read(fd, CafeItemConfig, sizeof(CafeItemConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read cafe Item Configuration");
		close(fd);
		return FAIL;
	}
	// successfully read file
	close(fd);
	CafeItemConfigDebug(MAX_CAFE_ITEM_CODE);
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltCafeItemConfig
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Cafe Item Configuration.
//*****************************************************************************
BOOL LoadDfltCafeItemConfig(void)
{
	UINT16 	configIdx;

	for(configIdx = 0 ; configIdx < MAX_CAFE_ITEM_CODE  ; configIdx++)
	{
		CafeItemConfig[configIdx] = DefltCafeItemConfig;
	}

	SendDfltConfigEvent(CAFE_ITEM_TABLE_ID);

	if(WriteCafeItemConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "CafeItemConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteCafeItemConfig
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Cafe Item into configuration file.
//*********************************************************************************
BOOL WriteCafeItemConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_CAFE_ITEM_CNFG_FILE,O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open cafe Item Configuration  file");
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = CAFE_ITEM_CFG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write cafe Item Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(CafeItemConfig) != write(fd, CafeItemConfig, sizeof(CafeItemConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write cafe Item Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	CafeItemConfigDebug(MAX_CAFE_ITEM_CODE);
	return SUCCESS;
}

//*****************************************************************************
//	InitCafeMenuConfig
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Cafe Menu
//*****************************************************************************
void InitCafeMenuConfig(void)
{
	if(SUCCESS != ReadCafeMenuConfig())
	{
		if(SUCCESS != LoadDfltCafeMenuConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise System's Cafe Menu");
		}
	}
}

//*****************************************************************************
//	ReadCafeMenuConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Cafe Menu from file
//*****************************************************************************
BOOL ReadCafeMenuConfig(void)
{
	INT32 	fd;
	UINT8	ver;
	BOOL 	status;

	if((fd = open(SYS_CAFE_MENU_CNFG_FILE,O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Cafe Menus");
		return FAIL;
	}

	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Cafe Menu version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != CAFE_MENU_CFG_VER)
	{
		EPRINT(CONFIG_LOG,"Cafe Menu version doesn't match");
		status = LoadOldCafeMenuConfig(fd, ver);
		close(fd);
		return status;
	}

	// Read configuration
	if(sizeof(CafeMenuConfig) != read(fd, &CafeMenuConfig, sizeof(CafeMenuConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read Cafe Menu configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	CafeMenuConfigDebug(MAX_CAFE_MENU_CODE);
	return SUCCESS;
}

//*********************************************************************************
//	LoadDfltCafeMenuConfig
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Cafe Menus.
//*********************************************************************************
BOOL LoadDfltCafeMenuConfig(void)
{
	UINT8 idx;

	for(idx = 0 ; idx < MAX_CAFE_MENU_CODE ; idx++)
	{
		CafeMenuConfig[idx] = DefltCafeMenuConfig;
	}

	SendDfltConfigEvent(CAFE_MENU_TABLE_ID);

	if(WriteCafeMenuConfig(MAX_CAFE_MENU_CODE) == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "CafeMenuConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteCafeMenuConfig
//	Param:
//		IN :	menu index
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Cafe Menu into configuration file.
//*********************************************************************************
BOOL WriteCafeMenuConfig(UINT16 menuIdx)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_CAFE_MENU_CNFG_FILE,O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Cafe Menu file");
		return FAIL;
	}

	// Write version number
	ver = CAFE_MENU_CFG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write cafe Menu version");
		close(fd);
		return FAIL;
	}

	if( sizeof(CafeMenuConfig) != write(fd , CafeMenuConfig, sizeof(CafeMenuConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write Cafe Menu");
		close(fd);
		return FAIL;
	}

	close(fd);
	CafeMenuConfigDebug(menuIdx);
	return SUCCESS;
}

//*****************************************************************************
//	InitCafeMenuScheduleConfig
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises Cafeteria Menu Schedule
//*****************************************************************************
void InitCafeMenuScheduleConfig(void)
{
	if(SUCCESS != ReadCafeMenuScheduleConfig())
	{
		if(SUCCESS != LoadDfltCafeMenuScheduleConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise Cafeteria Menu Schedule Configuration");
		}
	}
}

//*****************************************************************************
//	ReadCafeMenuScheduleConfig
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Cafeteria Menu Schedule from configuration file
//*****************************************************************************
BOOL ReadCafeMenuScheduleConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	if((fd = open(SYS_MENU_SCHEDULE_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Cafe Menu Schedule Configuration file");
		return FAIL;
	}

	// Successfully opened file
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read cafe Menu Schedule Configuration version");
		close(fd);
		return FAIL;
	}

	// Check whether the version number matches
	if(ver != CAFE_MENU_SCHEDULE_CFG_VER)
	{
		EPRINT(CONFIG_LOG,"Cafe Menu Schedule Configuration version doesn't match");
		close(fd);
		return FAIL;
	}

	// Read configuration
	if(sizeof(CafeMenuSchConfig) != read(fd, CafeMenuSchConfig, sizeof(CafeMenuSchConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read cafe Menu Schedule Configuration");
		close(fd);
		return FAIL;
	}

	// successfully read file
	close(fd);
	CafeMenuSchConfigDebug(MAX_CAFE_MENU_SCHEDULE);
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltCafeMenuScheduleConfig
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Cafe Item Configuration.
//*****************************************************************************
BOOL LoadDfltCafeMenuScheduleConfig(void)
{
	UINT8 	configIdx;

	for(configIdx = 0 ; configIdx < MAX_CAFE_MENU_SCHEDULE ; configIdx++)
	{
		CafeMenuSchConfig[configIdx] = DefltCafeMenuSchConfig;
	}

	SendDfltConfigEvent(CAFE_MENU_SCHEDULE_TABLE_ID);

	if(WriteCafeMenuScheduleConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "CafeMenuScheduleConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteCafeMenuScheduleConfig
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Cafe Menu Schedule into configuration file.
//*********************************************************************************
BOOL WriteCafeMenuScheduleConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_MENU_SCHEDULE_CNFG_FILE,O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open cafe Menu Schedule Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = CAFE_MENU_SCHEDULE_CFG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write cafe Menu Schedule Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(CafeMenuSchConfig) != write(fd, CafeMenuSchConfig, sizeof(CafeMenuSchConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write cafe Menu Schedule Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	CafeMenuSchConfigDebug(MAX_CAFE_MENU_SCHEDULE);
	return SUCCESS;
}

//*****************************************************************************
//	InitPrinterConfig
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Printer parameters
//*****************************************************************************
void InitPrinterConfig(void)
{
	if(SUCCESS != ReadPrinterConfig())
	{
		if(SUCCESS != LoadDfltPrinterConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise Printer Configuration");
		}
	}
}

//*****************************************************************************
//	ReadPrinterConfig
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Printer parameters from file
//*****************************************************************************
BOOL ReadPrinterConfig(void)
{
	INT32 		fd;
	UINT8		ver;
	BOOL		status = SUCCESS;

	if((fd = open(SYS_PRINTER_CNFG_FILE,O_RDWR | O_SYNC | O_CREAT,
											S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Printer config file");
		return FAIL;
	}

	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Printer config version");
		close(fd);
		return FAIL;
	}

	// Check whether the version number matches
	if(ver != CAFE_PRINTER_CFG_VER)
	{
		EPRINT(CONFIG_LOG,"Printer config version doesn't match");
		status = LoadOldPrinterConfig(fd, ver);
		close(fd);
		return status;
	}

	if(sizeof(PrinterConfig) != read(fd,&PrinterConfig,sizeof(PrinterConfig)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Printer Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	PrinterConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltPrinterConfig
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Printer Default parameters
//*****************************************************************************
BOOL LoadDfltPrinterConfig(void)
{
	PrinterConfig = DefltPrinterConfig;

	SendDfltConfigEvent(PRINTER_PARA_TABLE_ID);

	if(WritePrinterConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "PrinterConfig");
		return SUCCESS;
	}
	return FAIL;
}

//******************************************************************************
//	WritePrinterConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONESYS_CAFE_ITEM_CNFG_FILE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Printer Parameters into configuration file.
//******************************************************************************
BOOL WritePrinterConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_PRINTER_CNFG_FILE,O_WRONLY | O_TRUNC | O_SYNC | O_CREAT,
											S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to Open Printer config File");
		return FAIL;
	}

	// Write version number
	ver = CAFE_PRINTER_CFG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Printer config version");
		close(fd);
		return FAIL;
	}
	// Write configuration
	if(sizeof(PrinterConfig) != write(fd,&PrinterConfig,sizeof(PrinterConfig)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Printer Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	PrinterConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitDvrEvtActnConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			This function initalise dvr event action configuration and if appropriate
//				configuration not found in flash then it loads default configuration
//				in ram as well as in flash.
//*****************************************************************************
void InitDvrEvtActnConfig(void)
{
	if(ReadDvrEvtActnConfig() != SUCCESS)
	{
		LoadDfltDvrEvtActnConfig();
	}
}

//*****************************************************************************
//	LoadDfltDvrEvtActnConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			This function loads default parameter into flash and into ram.
//*****************************************************************************
void LoadDfltDvrEvtActnConfig(void)
{
	UINT8		configIdx;

	for(configIdx = 0; configIdx < MAX_DVR_EVT_ACTN_CONFIG; configIdx++)
	{
		DvrEvtActionConfig[configIdx] = DfltDvrEvtActionConfig;
	}

	SendDfltConfigEvent(DVR_EVT_ACTN_CONFIG_TABLE_ID);
	WriteDvrEvtActnConfig();
	CONFIGPRINT(CONFIG_DEFAULT, "DvrEvtActnConfig");
}

//*****************************************************************************
//	WriteDvrEvtActnConfig()
//	Param:
//		IN : configIdx - dvr event action configuration index
//		OUT: NONE
//	Returns:
//			SUCCESS/FAIL
//	Description:
//			This function write dvr event action parameter into flash from ram.
//				To write config parameter we have choosen differen file for each
//				config.
//*****************************************************************************
BOOL WriteDvrEvtActnConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_DVR_EVT_ACTN_CNFG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Dvr event action config");
		return FAIL;
	}

	// Write version number
	ver = DVR_EVT_ACTN_CONFIG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write dvr event action configuration version");
		close(fd);
		return FAIL;
	}

	if(sizeof(DvrEvtActionConfig) != write(fd, DvrEvtActionConfig, sizeof(DvrEvtActionConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write dvr event action config");
		close(fd);
		return FAIL;
	}

	close(fd);
	DvrEvtActionConfigDebug(MAX_DVR_EVT_ACTN_CONFIG);
	return SUCCESS;
}

//*****************************************************************************
//	ReadDvrEvtActnConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			SUCCESS/FAIL
//	Description:
//			This function read dvr event action parameter into ram from flash.
//				As for each config we have written seperate file, so we need
//				to red it from all the file.
//*****************************************************************************
BOOL ReadDvrEvtActnConfig(void)
{
	INT32 		fd;
	UINT8		ver, status;

	//	Open System Advance Parameter File
	if((fd = open(SYS_DVR_EVT_ACTN_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG, "Failed to open dvr event action config");
		return FAIL;
	}

	// Read version number from the file
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read dvr event action config version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != DVR_EVT_ACTN_CONFIG_VER)
	{
		EPRINT(CONFIG_LOG,"Dvr event action config version doesn't match");
		status = LoadOldDvrEvtActnConfig(fd, ver);
		close(fd);
		return status;
	}

	if(sizeof(DvrEvtActionConfig) != read(fd, DvrEvtActionConfig, sizeof(DvrEvtActionConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read Dvr event action config");
		close(fd);
		return FAIL;
	}

	close(fd);
	DvrEvtActionConfigDebug(MAX_DVR_EVT_ACTN_CONFIG);
	return SUCCESS;
}

//*****************************************************************************
//	InitSmartRouteAccessConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initializes Route access configuration.

//*****************************************************************************
void InitSmartRouteAccessConfig()
{
	if(SUCCESS != ReadSmartRouteAccessConfig())
	{
		if(SUCCESS != LoadDfltSmartRouteAccessConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise Route Access Config");
		}
	}
}

//*****************************************************************************
//	ReadSmartRouteAccessConfig
//	Param:
//		IN :	pgIdx - PAGE INDEX
//		OUT:	NONE
//	Returns:	SUCCESS - if read successfully, Else FAIL
//	Description:
//			It reads route access configuration parameter from flash.
//
//*****************************************************************************
BOOL ReadSmartRouteAccessConfig(void)
{
	INT32 		fd;
	UINT8		ver;
	BOOL		status = SUCCESS;

	if((fd = open(SYS_SMART_ROUTE_ACCESS_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open smart route access configuration file");
		return FAIL;
	}

	// Read version number from the file
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read RouteAccess Configuration version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != SMART_ROUTE_ACCESS_CONFIG_VER)
	{
		EPRINT(CONFIG_LOG,"Smart route access Configuration version doesn't match");
		status = LoadOldSmartRouteAccessConfig(fd, ver);
		close(fd);
		return status;
	}

	if(sizeof(SmartRouteAccessConfig) != read(fd, SmartRouteAccessConfig, sizeof(SmartRouteAccessConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read smart route access Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	SmartRouteAccessConfigDebug(MAX_SMART_ROUTE_ACCESS);
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltSmartRouteAccessConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default smart route access configuration into RAM.
//
//*****************************************************************************
BOOL LoadDfltSmartRouteAccessConfig(void)
{
	UINT8		configIdx;

	for(configIdx = 0 ; configIdx < MAX_SMART_ROUTE_ACCESS ; configIdx++)
	{
		SmartRouteAccessConfig[configIdx] = DfltSmartRouteAccessConfig;
	}

	SendDfltConfigEvent(SMART_ROUTE_ACCESS_CONFIG_TABLE_ID);

	if(WriteSmartRouteAccessConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "SmartRouteAccessConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteSmartRouteAccessConfig()
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Route Access config into configuration file.
//*********************************************************************************
BOOL WriteSmartRouteAccessConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if((fd = open(SYS_SMART_ROUTE_ACCESS_CNFG_FILE, O_RDWR | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Route Access Configuration  file");
		return FAIL;
	}

	// Write version number
	ver = SMART_ROUTE_ACCESS_CONFIG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Route Access Configuration version");
		close(fd);
		return FAIL;
	}

	if(sizeof(SmartRouteAccessConfig) != write(fd, SmartRouteAccessConfig, sizeof(SmartRouteAccessConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write Route Access Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	SmartRouteAccessConfigDebug(MAX_SMART_ROUTE_ACCESS);
	return SUCCESS;
}

//*****************************************************************************
//	InitFirstInAccessInfo
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's FirstInAccessDetail parameters
//*****************************************************************************
void InitFirstInAccessInfo(void)
{
	if(SUCCESS != ReadFirstInAccessInfo() )
	{
		if(SUCCESS != LoadDfltFirstInAccessInfo() )
		{
			EPRINT(CONFIG_LOG,"Failed to intialise FirstIn Access Detail");
		}
	}
}

//******************************************************************************
//	WriteFirstInAccessInfo()
//	Param:
//		IN :	NONE
//		OUT:    NONESYS_CAFE_ITEM_CNFG_FILE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes FirstIn access detail into the file.
//******************************************************************************
BOOL WriteFirstInAccessInfo(void)
{
	INT32	fd;
	UINT8	ver;

	fd = open(SYS_FIRST_IN_ACCS_DETAIL_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to Open FirstInInfo");
		return FAIL;
	}
	// Write version number
	ver = FIRST_IN_INFO;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write FirstInInfo version");
		close(fd);
		return FAIL;
	}
	// Write configuration
	if(sizeof(FirstInInfo) != write(fd,&FirstInInfo,sizeof(FirstInInfo) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write FirstInInfo Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	FirstInInfoConfigDebug();
	return SUCCESS;
}

//******************************************************************************
//	ReadFirstInAccessInfo()
//	Param:
//		IN :	NONE
//		OUT:    NONESYS_CAFE_ITEM_CNFG_FILE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				read FirstIn access detail from the file.
//******************************************************************************
BOOL ReadFirstInAccessInfo(void)
{
	INT32 		fd;
	UINT8		ver;

	fd = open(SYS_FIRST_IN_ACCS_DETAIL_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open FirstInInfo file");
		return FAIL;
	}

	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read FirstInInfo version");
		close(fd);
		return FAIL;
	}

	// Check whether the version number matches
	if(ver != FIRST_IN_INFO)
	{
		EPRINT(CONFIG_LOG,"FirstInInfo version doesn't match");
		close(fd);
		return FAIL;
	}

	if(sizeof(FirstInInfo) != read(fd,&FirstInInfo,sizeof(FirstInInfo) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read FirstInInfo");
		close(fd);
		return FAIL;
	}

	close(fd);
	FirstInInfoConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltFirstInAccessInfo
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads FirstIn access detail Default parameters
//*****************************************************************************
BOOL LoadDfltFirstInAccessInfo(void)
{
	FirstInInfo = DfltFristInInfo;

	SendDfltConfigEvent(FIRST_IN_TABLE_ID);

	if(WriteFirstInAccessInfo() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "FirstInAccessInfo");
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	InitFileLogParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises File Log parameters.
//*****************************************************************************
void InitFileLogParaConfig(void)
{
	ReadFileLogParaConfig();
}

//*****************************************************************************
//	ReadFileLogParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads File Log parameter from file.
//*****************************************************************************
BOOL ReadFileLogParaConfig(void)
{
	INT32 		fd;
	UINT8		ver,status;

	//	Open System Advance Parameter File
	if( (fd = open(SYS_FILE_LOG_PARA_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open File Log parameter");
		return FAIL;
	}

	// Read version number from the file
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read File Log parameter version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != FILE_LOG_PARA_CONFIG_VER)
	{
		EPRINT(CONFIG_LOG,"File Log parameter version mismatch: saving old data in new configuration");
		status = LoadOldFileLogConfig(fd,ver);
		close(fd);
		/** Update Syslog log config from Old File log config */
		UpdateSyslogConfig();
		return status;
	}

	if(sizeof(FileLogPara) != read(fd, &FileLogPara, sizeof(FileLogPara) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read File Log parameter");
		close(fd);
		return FAIL;
	}

	close(fd);

	/** Update Syslog log config from Old File log config */
	UpdateSyslogConfig();
	return SUCCESS;
}

//*****************************************************************************
//	InitWebAPIEventsConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Web API Events Parameter.
//*****************************************************************************
void InitWebAPIEventsConfig(void)
{
	if(ReadWebAPIEventsConfig() != SUCCESS)
	{
		if(SUCCESS != LoadDfltWebAPIEventsConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialize Web API Events Parameters");
		}
	}
}

//*****************************************************************************
//	ReadWebAPIEventsConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Web API Events parameter from file.
//*****************************************************************************
BOOL ReadWebAPIEventsConfig(void)
{
	INT32 		fd;
	UINT8		ver;
	BOOL		status;

	if( (fd = open(SYS_WEB_API_EVENTS_CFG_FILE , O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Web API Events Parameters");
		return FAIL;
	}

	// Read network parameters
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Web API Events Parameter version");
		close(fd);
		return FAIL;
	}

	// Check whether the versions matches
	if(ver != WEB_API_EVENTS_PARA_VER)
	{
		EPRINT(CONFIG_LOG,"Web API Events Parameter version doesn't matches");
		status = LoadOldWebApiEventConfig(fd, ver);
		close(fd);
		return status;
	}

	if(sizeof(WEB_API_EVENTS_CONFIG_t) != read(fd, &WebAPIEventsConfig, sizeof(WEB_API_EVENTS_CONFIG_t)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Web API Events Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	WebAPIEventsConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltWebAPIEventsConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Web API Events parameters
//*****************************************************************************
BOOL LoadDfltWebAPIEventsConfig(void)
{
	WebAPIEventsConfig = DfltWebAPIEventsConfig;
	SendDfltConfigEvent(WEB_API_EVENTS_CONFIG_TABLE_UPDATE_ID);
	if(WriteWebAPIEventsConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "WebAPIEventsConfig");
		return SUCCESS;
	}
	return FAIL;
}

//******************************************************************************
//	WriteWebAPIEventsConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Web API Events Parameter into configuration
//				file.
//******************************************************************************
BOOL WriteWebAPIEventsConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if( (fd = open(SYS_WEB_API_EVENTS_CFG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Web API Events Parameters");
		return FAIL;
	}

	// Write network parameter version number
	ver = WEB_API_EVENTS_PARA_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Web API Events Parameter version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(WEB_API_EVENTS_CONFIG_t) != write(fd,&WebAPIEventsConfig, sizeof(WEB_API_EVENTS_CONFIG_t)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Web API Events Centre Parameters");
		close(fd);
		return FAIL;
	}

	close(fd);
	WebAPIEventsConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
// DeleteSingleUserPhoto()
// Param
//		Input:		userId- user whose photo is to be deleted
//		Output:		NONE
//		Return:		NONE
// Description:
//		This function will delete single user photo from memory card.
//*****************************************************************************
void DeleteSingleUserPhoto(UINT32 userId)
{
	CHAR	sysCmdBuf[255];

	if( (userId != INVALID_USER_ID) && (strlen(MmcInfo.devPath) != 0))
	{
		sprintf(sysCmdBuf, "rm -rf %s/%s/%s%u.*", MmcInfo.devPath, USER_PICS_FOLDER, USER_PICS_PREFIX, userId);
		system(sysCmdBuf);
	}
}

//*****************************************************************************
// DeleteAllUserPhoto()
// Param
//		Input:		NONE
//		Output:		NONE
//		Return:		NONE
// Description:
//		This function will delete all user photo from memory card.
//*****************************************************************************
void DeleteAllUserPhoto(void)
{
	char buf[MAX_SYS_CMD_BUF_SZ] = {0};
	if(strlen(MmcInfo.devPath))
	{
		snprintf(buf,sizeof(buf), "mv -f %s/%s %s/%s",MmcInfo.devPath, USER_PICS_FOLDER, MmcInfo.devPath, DLT_USER_PICS_FOLDER);
		if(ExeCmd(buf))
		{
			snprintf(buf, sizeof(buf),"mkdir -p %s/%s", MmcInfo.devPath, USER_PICS_FOLDER);
			if(ExeCmd(buf))
			{
				snprintf(buf, sizeof(buf),"%s/%s", MmcInfo.devPath, DLT_USER_PICS_FOLDER);
				InsertRemoveDirectoryReq(buf);
			}
		}
	}
}

//******************************************************************************
//	SendDfltConfigEvent()
//	Param:
//		IN :	TableId
//		OUT:    NONE
//	Returns:
//				NONE
//	Description:
//				Generates Event Log When Configuration Is Default
//******************************************************************************
void SendDfltConfigEvent(CONFIGURATION_TABLE_e tableId)
{
	if(sysBeingDfltF == FALSE)
	{
		WriteEventLog(CONFIGURATION_DEFAULT, (tableId + 1), UNUSED_FIELD, UNUSED_FIELD, UNUSED_FIELD, UNUSED_FIELD);
	}
}

//*****************************************************************************
//	InitExtCardFormatConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Card Format Config
//*****************************************************************************
void InitCardFormatConfig(void)
{
	if(SUCCESS != ReadCardFormatConfig() )
	{
		if(SUCCESS != LoadDfltCardFormatConfig() )
		{
			EPRINT(CONFIG_LOG,"Failed to intialise System's  card format config");
		}
	}
}

//*****************************************************************************
//	ReadCardFormatConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Card format config from file
//*****************************************************************************
BOOL ReadCardFormatConfig(void)
{
	INT32 		fd;
	UINT8		ver;
	BOOL		status = FAIL;

	//	Open System Advance Parameter File
	if( (fd = open(SYS_CARD_FORMAT_CFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Ext Card fmt cfg file");
		return FAIL;
	}

	// Read version number from the file
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Ext Card fmt cfg version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != CARD_FORMAT_CONFIG_VER)
	{
		EPRINT(CONFIG_LOG, "Card format config version mismatch : Saving Old data in New configuration");
		status = LoadOldCardFormatConfig(fd, ver);
		close(fd);
		return status;
	}

	if(sizeof(CardFormatConf) != read(fd, &CardFormatConf, sizeof(CardFormatConf) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read Card format config");
		close(fd);
		return FAIL;
	}

	close(fd);
	CardFormatConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltExtCardFormatConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//*****************************************************************************
BOOL LoadDfltCardFormatConfig(void)
{
	UINT8 cardFmtIdx;

	for(cardFmtIdx = 0; cardFmtIdx < MAX_CARD_FORMAT_PER_DOOR; cardFmtIdx++)
	{
		if(cardFmtIdx == 0)
		{
			CardFormatConf[cardFmtIdx] = DfltCardFormatConf;
		}
		else
		{
			CardFormatConf[cardFmtIdx] = BlankCardFormatConf;
		}
	}
	SendDfltConfigEvent(EXT_CARD_FORMAT_CONFIG_TABLE_ID);
	SendDfltConfigEvent(INT_CARD_FORMAT_CONFIG_TABLE_ID);

	if(WriteCardFormatConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "CardFormatConfig");
		return SUCCESS;
	}
	return FAIL;
}

//******************************************************************************
//	WriteCardFormatConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Advance system parameter into configuration file.
//******************************************************************************
BOOL WriteCardFormatConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if( (fd = open(SYS_CARD_FORMAT_CFG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open card format config file");
		return FAIL;
	}

	// Write version number
	ver = CARD_FORMAT_CONFIG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write card format config version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(CardFormatConf) != write(fd, &CardFormatConf, sizeof(CardFormatConf) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write card format config");
		close(fd);
		return FAIL;
	}

	close(fd);
	CardFormatConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitVamConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system Vam Config
//*****************************************************************************
void InitVamConfig(void)
{
	UINT16		pg;
	BOOL		dfltCfgF = FALSE;

	// Since vam config is stored in multiple files, we read all files and
	// store all in one ram copy
    BOOL firstTimeBootF = !IsConfigFilePresent("MxVamPg");
    if(firstTimeBootF)
        DPRINT(SYS_LOG, "First time booting, All vam config files will be created");
	for(pg = 0; pg <= TOTAL_VAM_CONFIG_PAGES; pg++)
	{
        if(SUCCESS != ReadVamConfig(pg,firstTimeBootF))
		{
			dfltCfgF = TRUE;
			if(SUCCESS != LoadDfltVamConfig(pg) )
			{
				EPRINT(CONFIG_LOG, "Failed to initialise System Vam Configuration");
			}
		}
	}
	// If any page read fail, then only Generate event
	if(dfltCfgF == TRUE)
	{
		SendDfltConfigEvent(VAM_CONFIG_TABLE_ID);
	}
}

//*****************************************************************************
//	ReadVamConfig()
//	Param:
//		IN : pg		- Page to read
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Vam configuration from file
//*****************************************************************************
BOOL ReadVamConfig(UINT16 pg,BOOL firstTimeBootF)
{
	INT32 	fd;
	CHAR 	filePath[MAX_CNFG_FILE_NAME_SZ];
	UINT8 	recordCount;
	UINT16 	recordOffset;
	UINT8	ver;

	//file name
	sprintf(filePath, "%s%d%s", SYS_VAM_CNFG_FILE, (pg+1), SYS_CFG_FILE_EXT);

	//Start index from where to store in RAM
	recordOffset = (pg * VAM_CONFIG_PER_PAGE);

	if(pg == TOTAL_VAM_CONFIG_PAGES)	//If last page of file
	{
		//remaining entries in file
		recordCount = (MAX_VAM_CONFIG - (pg * VAM_CONFIG_PER_PAGE));
	}
	else	//If not last page
	{
		recordCount = VAM_CONFIG_PER_PAGE;
	}

    if((fd = open(filePath, O_RDWR | O_SYNC , S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
        if(firstTimeBootF == FALSE)
            EPRINT(CONFIG_LOG,"Failed to open vam configuration file");
		return FAIL;
	}

	// Read version number from the file
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Vam cfg version");
		close(fd);
		return FAIL;
	}

	// Read configuration
	if( (recordCount * sizeof(VamConfig[0]) ) != (UINT32)read(fd, &VamConfig[recordOffset], (recordCount * sizeof(VamConfig[0]) ) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read Vam Configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltVamConfig()
//	Param:
//		IN : pg		- Page for which to load default
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default system Vam configuration
//*****************************************************************************
BOOL LoadDfltVamConfig(UINT16 pg)
{
	UINT8	recordCount;
	UINT16	index;
	UINT16	recordOffset;

	recordOffset = (pg * VAM_CONFIG_PER_PAGE);		//Start of index

	//Length of index
	//If last page
	if(pg == TOTAL_VAM_CONFIG_PAGES)
	{
		recordCount = (MAX_VAM_CONFIG - (pg * VAM_CONFIG_PER_PAGE));
	}
	else	//If not last page
	{
		recordCount = VAM_CONFIG_PER_PAGE;
	}

	for(index = recordOffset; index < (recordOffset + recordCount); index++)
	{
		VamConfig[index] = DfltVamConfig;
	}

	return WriteVamConfig(pg);
}

//*****************************************************************************
//	LoadDfltAllVamConfig()
//	Param:
//		IN : pg	- Page for which to load default
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default system vam configuration for all vam configurations
//*****************************************************************************
static void LoadDfltAllVamConfig(void)
{
	UINT16 	pg;

	for(pg = 0 ; pg <= TOTAL_VAM_CONFIG_PAGES ; pg++)
	{
		LoadDfltVamConfig(pg);
	}
	CONFIGPRINT(CONFIG_DEFAULT, "AllVamConfig");
	SendDfltConfigEvent(VAM_CONFIG_TABLE_ID);
}

//*****************************************************************************
//	LoadDfltSingleVamConfig()
//	Param:
//		IN : userIdx
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default system Vam configuration for given index.
//*****************************************************************************
BOOL LoadDfltSingleVamConfig(UINT16 Idx)
{
	// Set default Vam configuration @ Idx
	VamConfig[Idx] = DfltVamConfig;
	return(WriteSingleVamConfig(Idx));
}

//*********************************************************************************
//	WriteSingleVamConfig()
//	Param:
//		IN : 	Index - index which needs to be written
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes System Vam config into configuration file for single config.
//*********************************************************************************
BOOL WriteSingleVamConfig(UINT16 index)
{
	UINT16	pg;
	INT32	fd;
	CHAR	filePath[MAX_CNFG_FILE_NAME_SZ];
	UINT16	recordOffset;

	// If requested index is greater than maximum limit, no need to write it in file
	if(index >= MAX_VAM_CONFIG)
	{
		return FAIL;
	}

	pg = (index / VAM_CONFIG_PER_PAGE);

	//file name
	sprintf(filePath, "%s%d%s", SYS_VAM_CNFG_FILE, (pg+1), SYS_CFG_FILE_EXT);

	//Start index in file
	recordOffset = (pg * VAM_CONFIG_PER_PAGE);

	if((fd = open(filePath, O_WRONLY | O_SYNC)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open vam configuration file");
		return FAIL;
	}
	else
	{
		// Set pointer to write requested index, consider version number
		lseek(fd,(((index - recordOffset) * (sizeof(VamConfig[0]))) + 1),SEEK_SET);

		if( (sizeof(VamConfig[0]) ) != write(fd, &VamConfig[index], sizeof(VamConfig[0])))
		{
			EPRINT(CONFIG_LOG,"Failed to write Vam configuration for the given index");
			close(fd);
			return FAIL;
		}
		else
		{
			DPRINT(CONFIG_LOG,"Vam [%d] is written in the page %d", index, pg);
			close(fd);
			VamConfigDebug(index);
			return SUCCESS;
		}
	}
}

//*********************************************************************************
//	WriteVamConfig()
//	Param:
//		IN : pg		- Page which needs to be written
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes System User config into configuration file.
//*********************************************************************************
BOOL WriteVamConfig(UINT16 pg)
{
	INT32 	fd;
	CHAR 	filePath[MAX_CNFG_FILE_NAME_SZ];
	UINT8 	recordCount;
	UINT16 	recordOffset;
	UINT8	ver;

	//file name
	sprintf(filePath, "%s%d%s", SYS_VAM_CNFG_FILE, (pg+1), SYS_CFG_FILE_EXT);

	//Start index Number in file
	recordOffset = (pg * VAM_CONFIG_PER_PAGE);

	if(pg == TOTAL_VAM_CONFIG_PAGES)	//If last page
	{
		//Remainig entries in file
		recordCount = (MAX_VAM_CONFIG - (pg * VAM_CONFIG_PER_PAGE));
	}
	else	//If not last page
	{
		recordCount = VAM_CONFIG_PER_PAGE;
	}

	if((fd = open(filePath, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open/create Vam configuration file");
		return FAIL;
	}

	// Opened file successfully
	// Write version number
	ver = VAM_CONFIG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Vam Configuration version");
		close(fd);
		return FAIL;
	}

	if( (recordCount*sizeof(VamConfig[0]) ) != (UINT32)write(fd,&VamConfig[recordOffset],(recordCount*sizeof(VamConfig[0]) ) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write Vam configuration");
		close(fd);
		return FAIL;
	}

	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	InitJcmConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's JcmConfig
//*****************************************************************************
void InitJcmConfig(void)
{
	if(SUCCESS != ReadJcmConfig() )
	{
		if(SUCCESS != LoadDfltJcmConfig() )
		{
			EPRINT(CONFIG_LOG,"Failed to initialise System's JCM config");
		}
	}
}

//*****************************************************************************
//	ReadJcmConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads JCM Config from file
//*****************************************************************************
BOOL ReadJcmConfig(void)
{
	INT32 	fd;
	UINT8	ver,returnValue;

	fd = open(SYS_JCM_CONFIG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open JCM Config file");
		return FAIL;
	}

	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read JCM Config version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != JCM_CONFIG_VER)
	{
		EPRINT(CONFIG_LOG,"JCM Config version doesn't match: saving old data in new configuration");
		returnValue = LoadOldJCMConfig(fd, ver);
		close(fd);
		return returnValue;
	}

	// Read configuration
	if(sizeof(JcmConfig) != read(fd, &JcmConfig[0], sizeof(JcmConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read JCM configuration");
		close(fd);
		return FAIL;
	}
	close(fd);

	//here None job code will not come from ACMS, so taken locally.
	JcmConfig[MAX_JCM_CONFIG] = NoneJobConfig;
	JcmConfig[MAX_JCM_CONFIG + 1] = DefaultJobConfig;
	JcmConfig[MAX_JCM_CONFIG + 2] = ContinueJobConfig;

	return SUCCESS;
}

//*********************************************************************************
//	LoadDfltJcmConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads JCM Config
//*********************************************************************************
BOOL LoadDfltJcmConfig(void)
{
	UINT16	idx;

	for(idx = 0; idx < MAX_JCM_CONFIG; idx++)
	{
		JcmConfig[idx] = DfltJcmConfig;
	}

	//here None job code will not come from ACMS, so taken locally.
	JcmConfig[MAX_JCM_CONFIG] = NoneJobConfig;
	JcmConfig[MAX_JCM_CONFIG + 1] = DefaultJobConfig;
	JcmConfig[MAX_JCM_CONFIG + 2] = ContinueJobConfig;

	SendDfltConfigEvent(JCM_CONFIG_TABLE_ID);

	if(WriteJcmConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "JcmConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteSingleJcmConfig()
//	Param:
//		IN : 	Index - Index for which to write config
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes JCM Config config into configuration file for single index.
//*********************************************************************************
BOOL WriteSingleJcmConfig(UINT16 index)
{
	INT32	fd;

	// If requested index is greater than maximum limit, no need to write it in file
	if(index >= MAX_JCM_CONFIG)
	{
		return FAIL;
	}

	fd = open(SYS_JCM_CONFIG_FILE, O_WRONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open JCM Config file");
		return FAIL;
	}

	// Set pointer to write requested index, consider version number byte
	lseek(fd, ((index * (sizeof(JCM_CONFIG_t))) + 1), SEEK_SET);

	if( (sizeof(JCM_CONFIG_t) ) != write(fd, &JcmConfig[index], sizeof(JCM_CONFIG_t) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write Function for the given index");
		close(fd);
		return FAIL;
	}
	close(fd);
	JcmConfigDebug(index);

	return SUCCESS;
}

//*********************************************************************************
//	WriteJcmConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes JCM Config into configuration file.
//*********************************************************************************
BOOL WriteJcmConfig(void)
{
	INT32	fd;
	UINT8	ver;

	fd = open(SYS_JCM_CONFIG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open JCM Config file");
		return FAIL;
	}

	// Write version number
	ver = JCM_CONFIG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write JCM Config' version");
		close(fd);
		return FAIL;
	}

	if(sizeof(JcmConfig) != write(fd, &JcmConfig[0], sizeof(JcmConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write JCM Config");
		close(fd);
		return FAIL;
	}
	close(fd);

	return SUCCESS;
}

//*****************************************************************************
//	InitReaderSchedulConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initializes Reader Swapping/scheduling Configuration variables.
//*****************************************************************************
void InitReaderSchedulConfig(void)
{
	if(SUCCESS != ReadReaderSchedulConfig() )
	{
		if(SUCCESS != LoadDfltReaderSchedulConfig() )
		{
			EPRINT(SYS_LOG, "Failed to initialize Schedule Configuration variables");
		}
	}
}

//*****************************************************************************
//	ReadReaderSchedulConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Reader Swapping/scheduling  Configuration stored in file.
//*****************************************************************************
BOOL ReadReaderSchedulConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	fd = open(SYS_RDR_SWAP_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open Reader Schedule Configuration variable file");
		return FAIL;
	}

	// File opened successfully
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to read Reader Schedule Configuration file");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != READER_MODE_SCHEDULE_VER)
	{
		EPRINT(SYS_LOG, "Reader Schedule Configuration version doesn't match");
		close(fd);
		return FAIL;
	}

	if((sizeof(DoorModeSchCfg)) != read(fd, &DoorModeSchCfg[0], sizeof(DoorModeSchCfg)))
	{
		EPRINT(SYS_LOG, "Failed to read Reader Swapping Configuration file");
		close(fd);
		return FAIL;
	}

	//File read successfully
	close(fd);
	DoorModeSchedulingConfigDebug(MAX_DOOR_MODE_SCHEDULE);
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltReaderSchedulConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Reader Schedule Configuration.
//*****************************************************************************
BOOL LoadDfltReaderSchedulConfig(void)
{
	UINT8 idx;
	for(idx = 0; idx < MAX_DOOR_MODE_SCHEDULE; idx++)
	{
		DoorModeSchCfg[idx] = DfltDoorModeSchedul;
	}
	SendDfltConfigEvent(DOOR_MODE_SCH_CONFIG_TABLE_ID);

	if(WriteReaderSchedulConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "ReaderSchedulConfig");
		return SUCCESS;
	}
	return FAIL;
}

//*********************************************************************************
//	WriteSingleReaderSchedulConfig()
//	Param:
//		IN : 	Index - Index for which to write config
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Reader Schedule config into configuration file for single index.
//*********************************************************************************
BOOL WriteSingleReaderSchedulConfig(UINT16 index)
{
	INT32	fd;

	// If requested index is greater than maximum limit, no need to write it in file
	if(index >= MAX_DOOR_MODE_SCHEDULE)
	{
		return FAIL;
	}

	fd = open(SYS_RDR_SWAP_CNFG_FILE, O_WRONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Reader Reader Swapping file");
		return FAIL;
	}

	// Set pointer to write requested index, consider version number byte
	lseek(fd, ((index * (sizeof(DOOR_MODE_SCH_CONFIG_t))) + 1), SEEK_SET);

	if( (sizeof(DOOR_MODE_SCH_CONFIG_t) ) != write(fd, &DoorModeSchCfg[index], sizeof(DOOR_MODE_SCH_CONFIG_t) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write Function for the given index");
		close(fd);
		return FAIL;
	}
	close(fd);
	DoorModeSchedulingConfigDebug(index);
	return SUCCESS;
}

//*********************************************************************************
//	WriteReaderSchedulConfig()
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Reader Schedule Configuration into file.
//*********************************************************************************
BOOL WriteReaderSchedulConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	fd = open(SYS_RDR_SWAP_CNFG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open Reader Swapping Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = READER_MODE_SCHEDULE_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to write Reader Swapping Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if((sizeof(DoorModeSchCfg)) != write(fd, &DoorModeSchCfg[0], (sizeof(DoorModeSchCfg) ) ) )
	{
		EPRINT(SYS_LOG, "Failed to write Reader Swapping Configuration into file");
		close(fd);
		return FAIL;
	}

	//File written successfully
	close(fd);
	DoorModeSchedulingConfigDebug(MAX_DOOR_MODE_SCHEDULE);
	return SUCCESS;
}

//*********************************************************************************
//	GetFpTemplateFilePath()
//	Param:
//		IN : 	fpIdx - Index for which to write config, userIdx
//		OUT:    filePath
//	Returns:	none
//
//	Description:retruns path of finger template of user
//*********************************************************************************
static void GetFpTemplateFilePath(CHARPTR filePath, UINT32 userIdx, UINT8 fpIdx)
{
	//for panel door usridx is used as userid
	if(GetDoorApplType() == PANEL_DOOR_APPL)
	{
		sprintf(filePath, "%s%u_%d", SYS_USER_FP_FILE_PATH, userIdx, fpIdx);
	}
	else
	{
		sprintf(filePath, "%s%u_%d", SYS_USER_FP_FILE_PATH, GetUserId(userIdx), fpIdx);
	}
}

//*****************************************************************************
//	InitServerModeConfig()
//	Parameters:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialise Server Mode Configuration variables.
//*****************************************************************************
void InitServerModeConfig(void)
{
	if(SUCCESS != ReadServerModeConfig() )
	{
		if(SUCCESS != LoadDfltServerModeConfig() )
		{
			EPRINT(SYS_LOG, "Failed to init Server mode Configuration variables");
		}
	}
}

//*****************************************************************************
//	ReadServerModeConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Server Mode Configuration stored in file.
//*****************************************************************************
BOOL ReadServerModeConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	fd = open(SYS_SERVER_MODE_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open Server Mode Configuration variable file");
		return FAIL;
	}

	// File opened successfully
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to read Server Mode Configuration file");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != SERVER_MODE_CONFIG_VER)
	{
		EPRINT(SYS_LOG, "Server Mode Configuration version doesn't match");
		close(fd);
		return FAIL;
	}

	if((sizeof(SERVER_MODE_CONFIG_t)) != read(fd, &ServerModeCfg, sizeof(SERVER_MODE_CONFIG_t)))
	{
		EPRINT(SYS_LOG, "Failed to read Server Mode Configuration file");
		close(fd);
		return FAIL;
	}

	//File read successfully
	close(fd);
	ServerModeConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltServerModeConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Server Mode Configuration.
//*****************************************************************************
BOOL LoadDfltServerModeConfig(void)
{
	ServerModeCfg = DfltServerMode;
	DPRINT(SYS_LOG,"Configuration defaulted - ServerModeCfg");
	if(WriteServerModeConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "ServerModeConfig");
		return SUCCESS;
	}
	return FAIL;
}
//*********************************************************************************
//	WriteServerModeConfig()
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Server Mode Configuration into file.
//*********************************************************************************
BOOL WriteServerModeConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	fd = open(SYS_SERVER_MODE_CNFG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open Server Mode Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = SERVER_MODE_CONFIG_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to write Server Mode Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if((sizeof(SERVER_MODE_CONFIG_t)) != write(fd, &ServerModeCfg, (sizeof(SERVER_MODE_CONFIG_t) ) ) )
	{
		EPRINT(SYS_LOG, "Failed to write Server Mode Configuration into file");
		close(fd);
		return FAIL;
	}

	//File written successfully
	close(fd);
	ServerModeConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitVyomServerConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initializes Vyom Server Configuration variables.
//*****************************************************************************
void InitVyomServerConfig(void)
{
	if(SUCCESS != ReadVyomServerConfig() )
	{
		if(SUCCESS != LoadDfltVyomServerConfig() )
		{
			EPRINT(SYS_LOG, "Failed to initialize Vyom Server Configuration variables");
		}
	}
}
//*****************************************************************************
//	ReadVyomServerConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Vyom Server  Configuration stored in file.
//*****************************************************************************
BOOL ReadVyomServerConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	fd = open(SYS_VYOM_SERVER_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open Vyom Server Configuration variable file");
		return FAIL;
	}

	// File opened successfully
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to read Vyom Server Configuration file");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != VYOM_SERVER_CONFIG_VER)
	{
		EPRINT(SYS_LOG, "Vyom Server Configuration version doesn't match");
		close(fd);
		return FAIL;
	}

	if((sizeof(VYOM_SERVER_CONFIG_t)) != read(fd, &VyomServerPara, sizeof(VYOM_SERVER_CONFIG_t)))
	{
		EPRINT(SYS_LOG, "Failed to read Vyom server Configuration file");
		close(fd);
		return FAIL;
	}

	//File read successfully
	close(fd);
	VyomServerConfigDebug();
	return SUCCESS;
}
//*****************************************************************************
//	LoadDfltVyomServerConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Vyom Server Configuration.
//*****************************************************************************
BOOL LoadDfltVyomServerConfig(void)
{
	VyomServerPara = DfltVyomServerPara;
	SendDfltConfigEvent(SERVER_PARA_TABLE_UPDATE_ID);
	if(WriteVyomServerConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "VyomServerConfig");
		return SUCCESS;
	}
	return FAIL;
}
//*********************************************************************************
//	WriteVyomServerConfig()
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Vyom Server Configuration into file.
//*********************************************************************************
BOOL WriteVyomServerConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	fd = open(SYS_VYOM_SERVER_CNFG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open Vyom Server Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = VYOM_SERVER_CONFIG_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to write Vyom Server Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if((sizeof(VYOM_SERVER_CONFIG_t)) != write(fd, &VyomServerPara, (sizeof(VYOM_SERVER_CONFIG_t) ) ) )
	{
		EPRINT(SYS_LOG, "Failed to write Vyom Server Configuration into file");
		close(fd);
		return FAIL;
	}

	//File written successfully
	close(fd);
	VyomServerConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitBackUpAndUpdateConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Web API Events Parameter.
//*****************************************************************************
void InitBackUpAndUpdateConfig(void)
{
	if(SUCCESS != ReadBackUpAndUpdateConfig())
	{
		if(SUCCESS != LoadDfltBackUpAndUpdateConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialize BackUp And Update Parameters");
		}
	}
}
//*****************************************************************************
//	InitBackUpAndUpdateConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises system's Web API Events Parameter.
//*****************************************************************************
BOOL ReadBackUpAndUpdateConfig(void)
{
	INT32 	fd;
	UINT8	ver;
	BOOL 	status;

	fd = open(SYS_BACKUP_AND_UPDATE_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open [%s] file", SYS_BACKUP_AND_UPDATE_CNFG_FILE);
		return FAIL;
	}

	// File opened successfully
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to read [%s] file", SYS_BACKUP_AND_UPDATE_CNFG_FILE);
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != BACKUP_AND_UPDATE_CONFIG_VER)
	{
		EPRINT(SYS_LOG, "BACKUP_AND_UPDATE version doesn't match");
		status = LoadOldBackUpAndUpdateConfig(fd, ver);
		close(fd);
		return status;
	}

	if((sizeof(BackUpAndUpdate)) != read(fd, &BackUpAndUpdate, sizeof(BackUpAndUpdate)))
	{
		EPRINT(SYS_LOG,  "Failed to read Config of [%s] file", SYS_BACKUP_AND_UPDATE_CNFG_FILE);
		close(fd);
		return FAIL;
	}

	//File read successfully
	close(fd);
	BackUpAndUpdateConfigDebug();
	return SUCCESS;
}
//*****************************************************************************
//	LoadDfltVyomServerConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Vyom Server Configuration.
//*****************************************************************************
BOOL LoadDfltBackUpAndUpdateConfig(void)
{
	if(GetServerMode() == COSEC_CENTRA)
	{
		BackUpAndUpdate = DfltBackUpAndUpdateParaForCentraMode;
	}
	else // VYOM
	{
		BackUpAndUpdate = DfltBackUpAndUpdateParaForVyomMode;
	}
    #ifdef OEM_SUPPORTED
    if(TRUE == OEM_IsCustomApplied())
    {
        snprintf(BackUpAndUpdate.ftpServerUrl, MAX_FTP_SERVER_URL, "%s", gOEMCurrDeviceInfo.doorInfo.ftpServerUrl);
        snprintf(BackUpAndUpdate.ftpServerUseName, MAX_FTP_USER_NAME, "%s", gOEMCurrDeviceInfo.doorInfo.ftpServerUserName);
        snprintf(BackUpAndUpdate.ftpServerUserPassword, MAX_FTP_USER_PASSWORD, "%s", gOEMCurrDeviceInfo.doorInfo.ftpServerPasswd);
    }
    #endif //#ifdef OEM_SUPPORTED		
	DPRINT(SYS_LOG,"Configuration defaulted - BackUpAndUpdate");
	if(WriteBackUpAndUpdateConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "BackUpAndUpdateConfig");
		return SUCCESS;
	}
	return FAIL;
}
//*********************************************************************************
//	WriteBackUpAndUpdateConfig()
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Vyom Server Configuration into file.
//*********************************************************************************
BOOL WriteBackUpAndUpdateConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	fd = open(SYS_BACKUP_AND_UPDATE_CNFG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open [%s] file", SYS_BACKUP_AND_UPDATE_CNFG_FILE);
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = BACKUP_AND_UPDATE_CONFIG_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to write BackUp And Update Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if((sizeof(BACKUP_AND_UPDATE_t)) != write(fd, &BackUpAndUpdate, (sizeof(BACKUP_AND_UPDATE_t) ) ) )
	{
		EPRINT(SYS_LOG, "Failed to write BackUp And Update Configuration into file");
		close(fd);
		return FAIL;
	}

	//File written successfully
	close(fd);
	BackUpAndUpdateConfigDebug();
	return SUCCESS;
}

//*********************************************************************************
//	InitUserInOutStatus()
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				NONE
//	Description:
//				Initiate user In/Out status
//*********************************************************************************
void InitUserInOutStatus(void)
{
		memset(UserInOutStatus, 0xFF, sizeof(UserInOutStatus));
}

//*********************************************************************************
//	GetDoorFpCount()
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				NONE
//	Description:
//				it will return Door Fp count
//*********************************************************************************
UINT32 GetDoorFpCount(void)
{
	return sysFpEnrolledCnt;
}

//*****************************************************************************
//	InitConnectToPanelConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialise Connect To Panel Configuration variables.
//*****************************************************************************
void InitConnectToPanelConfig(void)
{
	if(SUCCESS != ReadConnectToPanelConfig() )
	{
		if(SUCCESS != LoadDfltConnectToPanelConfig() )
		{
			EPRINT(SYS_LOG, "Failed to initialise Connect to Panel Configuration variables");
		}
	}
}

//*****************************************************************************
//	ReadConnectToPanelConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Connect to Panel  Configuration stored in file.
//*****************************************************************************
BOOL ReadConnectToPanelConfig(void)
{
	INT32 	fd;
	UINT8	ver;
	BOOL	status;

	fd = open(SYS_CONENCT_TO_PANEL_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open Connect to Panel Configuration variable file");
		return FAIL;
	}

	// File opened successfully
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to read version from Connect to Panel Configuration file");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != CONNECT_TO_PANEL_CONFIG_VER)
	{
		EPRINT(SYS_LOG, "Connect to Panel Configuration version doesn't match");
		status = LoadOldConnectToPanelConfig(fd, ver);
		close(fd);
		return status;
	}

	if((sizeof(CONNECT_TO_PANEL_CONFIG_t)) != read(fd, &ConnectToPanelPara, sizeof(CONNECT_TO_PANEL_CONFIG_t)))
	{
		EPRINT(SYS_LOG, "Failed to read Connect to Panel Configuration file");
		close(fd);
		return FAIL;
	}

	//File read successfully
	close(fd);
	ConnectToPanelConfigDebug();
	return SUCCESS;
}
//*****************************************************************************
//	LoadDfltConnectToPanelConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default Connect to Panel Configuration.
//*****************************************************************************
BOOL LoadDfltConnectToPanelConfig(void)
{
	ConnectToPanelPara = DfltConnectToPanelPara;
	DPRINT(SYS_LOG,"Configuration defaulted - ConnectToPanelPara");
	if(WriteConnectToPanelConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "ConnectToPanelConfig");
		return SUCCESS;
	}
	return FAIL;
}
//*********************************************************************************
//	WriteConnectToPanelConfig()
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes Connect to Panel Configuration into file.
//*********************************************************************************
BOOL WriteConnectToPanelConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	fd = open(SYS_CONENCT_TO_PANEL_CNFG_FILE, O_WRONLY | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open Connect to Panel Configuration file");
		return FAIL;
	}

	// File opened successfully
	// Write version number
	ver = CONNECT_TO_PANEL_CONFIG_VER;
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(SYS_LOG, "Failed to write Connect to Panel Configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if((sizeof(CONNECT_TO_PANEL_CONFIG_t)) != write(fd, &ConnectToPanelPara, (sizeof(CONNECT_TO_PANEL_CONFIG_t) ) ) )
	{
		EPRINT(SYS_LOG, "Failed to write Connect to Panel Configuration into file");
		close(fd);
		return FAIL;
	}

	//File written successfully
	close(fd);
	ConnectToPanelConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitLicenseDongleConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises LicenseDongle Config
//*****************************************************************************
void InitLicenseDongleConfig(void)
{
	if(SUCCESS != ReadLicenseDongleConfig())
	{
		if(SUCCESS != LoadDfltLicenseDongleConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise License Dongle Configuration");
		}
	}
}

//*****************************************************************************
//	ReadLicenseDongleConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads LicenseDongle from configuration file
//*****************************************************************************
BOOL ReadLicenseDongleConfig(void)
{
	INT32 		fd;
	UINT8		ver;
	BOOL		status = FAIL;

	if( (fd = open(SYS_LICENSE_DONGLE_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open LicenseDongle Configuration file");
		return FAIL;
	}

	// read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read LicenseDongle Configuration version");
		close(fd);
		return FAIL;
	}

	// Check whether the version number matches
	if(ver != LICENSE_DONGLE_VER)
	{
		EPRINT(CONFIG_LOG,"LicenseDongle Configuration version doesn't match");
		status = LoadOldLicenseDongleParaConfig(fd, ver);
		close(fd);
		return status;
	}

	// Read the configuration
	if(sizeof(LICENSE_DONGLE_CONFIG_t) != read(fd, &LicenseDongleCfg, sizeof(LICENSE_DONGLE_CONFIG_t)))
	{
		EPRINT(CONFIG_LOG,"Failed to read LicenseDongle Configuration file");
		close(fd);
		return FAIL;
	}

	close(fd);
	LicenseDongleConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltLicenseDongleConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads default system password
//*****************************************************************************
BOOL LoadDfltLicenseDongleConfig(void)
{
	LicenseDongleCfg = DfltLicenseDonglePara;
	return WriteLicenseDongleConfig();
}

//*********************************************************************************
//	WriteLicenseDongleConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes LicenseDongle into configuration file.
//*********************************************************************************
BOOL WriteLicenseDongleConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if( (fd = open(SYS_LICENSE_DONGLE_CNFG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open LicenseDongle Configuration file");
		return FAIL;
	}

	ver = LICENSE_DONGLE_VER;
	// write version number
	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write LicenseDongle Configuration version");
		close(fd);
		return FAIL;
	}

	if(sizeof(LICENSE_DONGLE_CONFIG_t) != write(fd, &LicenseDongleCfg, sizeof(LICENSE_DONGLE_CONFIG_t)))
	{
		EPRINT(CONFIG_LOG,"Failed to write LicenseDongle Configuration");
		return FAIL;
	}

	close(fd);
	LicenseDongleConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	InitIpCameraConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises Ip Camera Configuration parameters
//*****************************************************************************
void InitIpCameraConfig(void)
{
	if(SUCCESS != ReadIpCameraConfig())
	{
		if(SUCCESS != LoadDfltIpCameraConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise Ip Camera Configration");
		}
	}

	LoadIpCameraIpResolveTimer(0);
}

//*****************************************************************************
//	ReadIpCameraConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Ip Camera Configurations from file
//*****************************************************************************
BOOL ReadIpCameraConfig(void)
{
	INT32 		fd;
	UINT8		ver;
	BOOL		status = FAIL;

	//	Open System Advance Parameter File
	if( (fd = open(SYS_IP_CAMERA_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open ip camera configurations");
		return FAIL;
	}

	// Read version number from the file
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read ip camera configurations version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != IP_CAMERA_CONFIG_VER)
	{
		EPRINT(CONFIG_LOG,"ip camera config version mismatch");
		status = LoadOldIpCameraConfig(fd, ver);
		close(fd);
		return status;
	}

	if(sizeof(IpCameraConfig) != read(fd,&IpCameraConfig,sizeof(IpCameraConfig)))
	{
		EPRINT(CONFIG_LOG,"Failed to read ip camera config");
		close(fd);
		return FAIL;
	}

	close(fd);
	IpCameraConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltIpCameraConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Default Ip camera capture snapshot configuration
//*****************************************************************************
BOOL LoadDfltIpCameraConfig(void)
{
	IpCameraConfig = DfltIpCameraConfig;

	SendDfltConfigEvent(IP_CAMERA_CONFIG_TABLE_ID);
	SendDfltConfigEvent(IP_CAMERA_ADVANCE_CONFIG_TABLE_ID);

	if(WriteIpCameraConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "IpCameraConfig");
		return SUCCESS;
	}
	return FAIL;
}

//******************************************************************************
//	WriteIpCameraConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes ip camera configuration into configuration file.
//******************************************************************************
BOOL WriteIpCameraConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if( (fd = open(SYS_IP_CAMERA_CNFG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open ip camera config");
		return FAIL;
	}

	// Write version number
	ver = IP_CAMERA_CONFIG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write ip camera configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(IpCameraConfig) != write(fd,&IpCameraConfig,sizeof(IpCameraConfig)))
	{
		EPRINT(CONFIG_LOG,"Failed to write ip camera config");
		close(fd);
		return FAIL;
	}

	close(fd);
	IpCameraConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	DefaultEventPics
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			This function default event pics related information and remove all
//		event pics
//*****************************************************************************
static void DefaultEventPicsInfo(void)
{
	CHAR	buff[MAX_SYS_CMD_BUF_SZ];
	UINT16	evtInfoIdx;

	for(evtInfoIdx = 0; evtInfoIdx < MAX_EVENT_PICS_COUNT; evtInfoIdx++)
	{
		EventPicsInfo.eventInfo[evtInfoIdx].evtCnt = 0;
		EventPicsInfo.eventInfo[evtInfoIdx].evtRollCnt = 0;
	}
	EventPicsInfo.picsIdx = 0;

	//delete all the event photo from event pics folder if mmc card is present
	if(strlen(MmcInfo.devPath))
	{
		// System call to process command
		sprintf(buff, "rm -rf %s/%s/*", MmcInfo.devPath, EVENT_PICS_FOLDER);
		system(buff);
	}

	DPRINT(SYS_LOG,"Configuration defaulted - EventPicsInfo");
	CONFIGPRINT(CONFIG_DEFAULT, "EventPicsInfo");
	WriteEventPicsInfo();
}

//*****************************************************************************
//	WriteEventPicsInfo
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function write event pics related information
//*****************************************************************************
static BOOL WriteEventPicsInfo(void)
{
	INT32 		fd;

	fd = open(SYS_EVENT_PICS_INFO_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Event Pics file");
		return FAIL;
	}
	if(sizeof(EventPicsInfo) != write(fd,&EventPicsInfo,sizeof(EventPicsInfo) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write FPR configuartion into file");
		close(fd);
		return FAIL;
	}
	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	WriteSingleEventPicsInfo
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function write event pics related information
//*****************************************************************************
BOOL WriteSingleEventPicsInfo(void)
{
	INT32 		fd;

	fd = open(SYS_EVENT_PICS_INFO_FILE, O_WRONLY | O_SYNC);
	if(fd < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open event pics info file");
		return FAIL;
	}

	if(sizeof(EventPicsInfo.picsIdx) != write(fd,&EventPicsInfo.picsIdx,
													sizeof(EventPicsInfo.picsIdx) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write event pics info for the given index");
		close(fd);
		return FAIL;
	}

	// Set pointer to write requested index, consider version number
	lseek(fd,(EventPicsInfo.picsIdx * (sizeof(EVENT_PICS_EVENT_INFO_t) ) ),SEEK_CUR);

	if(sizeof(EVENT_PICS_EVENT_INFO_t) != write(fd,&EventPicsInfo.eventInfo[EventPicsInfo.picsIdx],
												sizeof(EVENT_PICS_EVENT_INFO_t) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to write event pics info for the given index");
		close(fd);
		return FAIL;
	}
	DPRINT(CONFIG_LOG,"eventPicsInfo is written for evtPicsIdx %d", EventPicsInfo.picsIdx);
	close(fd);
	return SUCCESS;
}
//*****************************************************************************
//	ReadEventPicsInfo
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function write event pics related information
//*****************************************************************************
static BOOL ReadEventPicsInfo(void)
{
	INT32 		fd;

	fd = open(SYS_EVENT_PICS_INFO_FILE, O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open event pics info file");
		//if fail to open file then default it
		DefaultEventPicsInfo();
		return FAIL;
	}

	if(sizeof(EventPicsInfo) != read(fd, &EventPicsInfo, sizeof(EventPicsInfo) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read event pics info file");
		close(fd);
		return FAIL;
	}
	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	InitDisplayBrightnessConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises Display Brightness Configuration parameters
//*****************************************************************************
void InitDisplayBrightnessConfig(void)
{
	if(SUCCESS != ReadDisplayBrightnessConfig())
	{
		if(SUCCESS != LoadDfltDisplayBrightnessConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to intialise Display Brightness Configration");
		}
	}
}

//*****************************************************************************
//	ReadDisplayBrightnessConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Reads Display Brightness Configurations from file
//*****************************************************************************
BOOL ReadDisplayBrightnessConfig(void)
{
	INT32 		fd;
	UINT8		ver;

	//	Open System Advance Parameter File
	if( (fd = open(SYS_DISPLAY_BRIGHTNESS_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to Display Brightness configurations File");
		return FAIL;
	}

	// Read version number from the file
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read isplay Brightness configurations version");
		close(fd);
		return FAIL;
	}

	// Check whether the version matches
	if(ver != BRIGHTNESS_CONFIG_VER)
	{
		EPRINT(CONFIG_LOG,"Display Brightness configurations version mismatch");
		close(fd);
		return FAIL;
	}

	if(sizeof(DisplayBrightness) != read(fd,&DisplayBrightness,sizeof(DisplayBrightness)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Display Brightness config");
		close(fd);
		return FAIL;
	}

	close(fd);
	DisplayBrightnessConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadDfltIpCameraConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Default Ip camera capture snapshot configuration
//*****************************************************************************
BOOL LoadDfltDisplayBrightnessConfig(void)
{
	DisplayBrightness = DfltDisplayBrightness;
	if(WriteDisplayBrightnessConfig() == SUCCESS)
	{
		CONFIGPRINT(CONFIG_DEFAULT, "DisplayBrightnessConfig");
		return SUCCESS;
	}
	return FAIL;
}

//******************************************************************************
//	WriteDisplayBrightnessConfig()
//	Param:
//		IN :	NONE
//		OUT:    NONE
//	Returns:
//				SUCCESS - If successful
//				FAILURE otherwise
//	Description:
//				Writes ip camera configuration into configuration file.
//******************************************************************************
BOOL WriteDisplayBrightnessConfig(void)
{
	INT32	fd;
	UINT8	ver;

	if( (fd = open(SYS_DISPLAY_BRIGHTNESS_CNFG_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Display Brightness config");
		return FAIL;
	}

	// Write version number
	ver = BRIGHTNESS_CONFIG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Display Brightness configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(DisplayBrightness) != write(fd,&DisplayBrightness,sizeof(DisplayBrightness)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Display Brightness config");
		close(fd);
		return FAIL;
	}

	close(fd);
	DisplayBrightnessConfigDebug();
	return SUCCESS;
}

//*********************************************************************************
//	IncreaseDoorFpCount()
//	Param:
//		IN : 	NONE
//		OUT:    NONE
//	Returns:
//				NONE
//	Description:
//				it will increase Door Fp count
//*********************************************************************************
VOID IncreaseDoorFpCount(void)
{
	sysFpEnrolledCnt++;
}

//*****************************************************************************
//	InitAccessScheduleConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Initialises Device Access schedule Configuration parameters
//*****************************************************************************
void InitAccessScheduleConfig(void)
{
	if(SUCCESS != ReadAccessScheduleConfig())
	{
	      EPRINT(CONFIG_LOG,"Failed to Read Access Schedule Configration");
		if(SUCCESS != LoadDfltAccessScheduleConfig())
		{
			EPRINT(CONFIG_LOG,"Failed to Load Dflt Access Schedule Configration");
		}
	}
}
//*****************************************************************************
//	ReadAccessScheduleConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Reads Device Access schedule from Configuration file
//*****************************************************************************
BOOL ReadAccessScheduleConfig(void)
{
	INT32 	fd;
	UINT8	ver;

	if((fd = open(SYS_ACCESS_SCHEDULE_FILE, O_RDONLY | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Access Schedule Configuration file");
		return FAIL;
	}

	// Successfully opened file
	// Read version number
	if(sizeof(ver) != read(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to read Access Schedule Configuration version");
		close(fd);
		return FAIL;
	}

	// Check whether the version number matches
	if(ver != ACCESS_SCHEDULE_CONFIG_VER)
	{
 		EPRINT(CONFIG_LOG,"Device Access Schedule Configuration version doesn't match");
		close(fd);
		return FAIL;
	}

	// Read configuration
	if(sizeof(AccessScheduleConfig) !=  read(fd, AccessScheduleConfig, sizeof(AccessScheduleConfig) ) )
	{
		EPRINT(CONFIG_LOG,"Failed to read Access Schedule Configuration");
		close(fd);
		return FAIL;
	}

	// successfully read file
	close(fd);
	AccessScheduleConfigDebug();
	return SUCCESS;
}
//*****************************************************************************
//	WriteAccessScheduleConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Writes Device Access schedule into Configuration file
//*****************************************************************************
BOOL WriteAccessScheduleConfig(void)
{
	INT32	fd;
	UINT8	ver;


	if( (fd = open(SYS_ACCESS_SCHEDULE_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO) ) < 0)
	{
		EPRINT(CONFIG_LOG,"Failed to open Access Schedule config");
		return FAIL;
	}

	// Write version number
	ver = ACCESS_SCHEDULE_CONFIG_VER;

	if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Access Schedule configuration version");
		close(fd);
		return FAIL;
	}

	// Write configuration
	if(sizeof(AccessScheduleConfig) != write(fd,&AccessScheduleConfig,sizeof(AccessScheduleConfig)))
	{
		EPRINT(CONFIG_LOG,"Failed to write Access Schedule config");
		close(fd);
		return FAIL;
	}

	close(fd);
 	AccessScheduleConfigDebug();
	return SUCCESS;
}
//*****************************************************************************
//	LoadDfltAccessScheduleConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			NONE
//	Description:
//			Load Defaults Device Access schedule 
//*****************************************************************************
BOOL LoadDfltAccessScheduleConfig(void)
{
	UINT8 	configIdx;

	for(configIdx = 0 ; configIdx < MAX_TIME_FRAME ; configIdx++)
	{
		AccessScheduleConfig[configIdx] = DfltAccessScheduleConfig;
	}
	SendDfltConfigEvent(ACCESS_SCHEDULE_TABLE_ID);
	return WriteAccessScheduleConfig();
}
