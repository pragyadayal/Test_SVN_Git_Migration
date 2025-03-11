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
//   Created By   : Kunjal Garach
//   File         : MxMain.c
//   Description  : This file defines ACS main and other startup functions.
//
/////////////////////////////////////////////////////////////////////////////

//*** Place all include files here ***
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pthread.h>

//Application Includes.
#include "MxDeviceSpecific.h"
#include "MxTypedef.h"
#include "MxSysTimer.h"
#include "MxConfig.h"
#include "MxConfigFileIo.h"
#include "MxWebJeeves.h"
#include "MxDcTask.h"
#include "MxEventLog.h"
#include "MxPDEventLog.h"
#include "MxAlarms.h"
#include "MxSysTime.h"
#include "MxGpio.h"
#include "MxLogs.h"
#include "MxAcmsTask.h"
#include "MxIOLink.h"
#include "MxTimeTrigFunc.h"
#include "MxDst.h"
#include "MxEthernetPort.h"
#include "MxWifiPort.h"
#include "MxDelay.h"
#include "MxReaderTask.h"
#include "MxGuiHandler.h"
#include "MxSysUtils.h"
#include "MxBackupUpdate.h"
#include "MxBlockDevice.h"
#include "MxMobileBroadband.h"
#include "MxCmdCtrl.h"
#include "MxAcmsSocket.h"
#include "MxCafeteria.h"
#include "MxPrinter.h"
#include "MxDvrIntegration.h"
#include "MxSmartRouteAccess.h"
#include "MxSysTest.h"
#include "MxCerTask.h"
#include "MxCommTask.h"
#include "MxNetworkManager.h"
#include "MxWebAPI.h"
#include "MxWebAPITcpEventsTask.h"
#include "MxMsgHandler.h"
#include "MxJobCosting.h"
#include "MxFpIdtServer.h"
#include "MxIdtServerBioSyncTask.h"
#include "MxCafeScreenMirror.h"
#include "MxDcFileSyncTask.h"
#include "MxFTPFirmwareUpgrade.h"
#include "MxPanelSocket.h"
#include "MxPanelCommTask.h"
#include "MxSysCommonDef.h"
#include "MxAttendanceDetail.h"
#ifdef UPNP_SUPPORTED
#include "MxUPNPSupport.h"
#endif //#ifdef UPNP_SUPPORTED
#include "MxAutoInterfaceManager.h"
#include "MxUdpPing.h"
#include "MxWebJvsAutoFirmwareAndUpgrade.h"
#include "MxWebJvsAutoSystemTest.h"
#include "MxSysCommonUtils.h"
#include"MxFaceRecognitionConfig.h"
#include "MxFaceRecognition.h"
#include "MxWebJvsWifiSetting.h"
#include "MxAutoSwitchWifi.h"
#include "MxBluetooth.h"
#ifdef	PUSH_API_SUPPORTED
#include "MxThirdPartyServerTask.h"
#endif

#include "MxTemperatureSensor.h"

#ifdef OEM_SUPPORTED
#include "OEM.h"
#endif //#ifdef OEM_SUPPORTED
#include "Syslog.h"

#include "MxRegistryFile.h"
//******** Extern Variables **********
extern struct tm 				CurrTimeDate;
extern SYS_BASIC_PARA_t			SysBasicPara;
extern WEB_API_EVT_INFO_t		WebAPIEvtInfo;
extern WEB_API_EVENTS_CONFIG_t 	WebAPIEventsConfig;
extern BACKUP_AND_UPDATE_t		BackUpAndUpdate;
extern THREAD_STATUS_e			checkFTPFirmwareReadyThreadStatus;
extern THREAD_STATUS_e			UpdateAtdncDtlForSnglUsrThrdSts;
extern UINT16	  				UpdateAtdncDtlForSnglUserIdx;
extern SYS_ADVANCE_PARA_t		SysAdvPara;
extern BOOL						RunForgivenessTimerBasedF;
extern SYS_PARA_CONFIG_t		SysParaConfig;
extern SERVER_MODE_CONFIG_t		ServerModeCfg;
extern FW_UPDATE_STAGES_e 			fwUpdateStage;
extern BOOL							BenchmarkF;
//****** Defines and Data Types ******
#define DUMP_FILE_SIZE				102400
#define DUMP_FILE1					"/matrix/applData/log/crashReport1.txt"
#define DUMP_FILE2					"/matrix/applData/log/crashReport2.txt"
#define BLE_HEALTH_CHECK_TIME					2	//minute

//******** Function Prototypes *******
static void CheckForOneMinuteUpdates(void);
static void *CheckGoAheadRunStatusThreadFn(VOIDPTR dummy);
static BOOL StartCheckGoAheadThread(void);
static void CheckForOneMinuteUpdatesInPanelDoor(void);

#ifdef GDB_DEBUG_ON
static void HandlerFun(int dummy);
#endif

//******** Global Variables **********
BOOL	WhileOneRunningF 		= FALSE;
BOOL	DemoModeEnable 			= FALSE;
BOOL 	SamayModeEnable 		= FALSE;
BOOL 	FirmwareUpgradeTriger 	= FALSE;
BOOL 	IsFirmwareReady 		= FALSE;

//******** static Variables **********
static pthread_t CheckForWebServerStatusThreadFnId=0;

//******** Function Definitions ******

//***************************************************************************
//	main()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	application exit status (always 1)
//	Description:
//				This is system's main function.
//				First all modules are initialized and then forever loop
//				is started processing each task in a loop.
//***************************************************************************
INT32 main(INT32 argc, CHARPTR argv[])
{
	BOOL	watchDogEnabled = TRUE;
	CHAR 	cmdBuf[255];
	UINT8	cnt;
	struct 	stat tempStat;

	ExeCmdWithResp("echo 1 > /proc/omap_tty/write");
	signal(SIGPIPE, SIG_IGN);
	#ifdef GDB_DEBUG_ON
	signal(SIGSEGV, HandlerFun);
	#endif

	// The Second Argument is to enable/ disable watchdog
	if (argc == 2)
	{
		if (atoi(argv[1]) == (INT32)FALSE)
		{
			watchDogEnabled = FALSE;
		}
	}

	/** Initalize syslog */
	SyslogInit();

	InitSysTime();
	WriteApplPresentFile();
	WriteSamayApplPresentFile();

	//Get door is paneldoor or directdoor
	ReadDoorApplType();

	// Clearing or Creating Config Default Log file
	ClearOrCreateFile(CONFIG_DEFAULT_LOG_FILE);
	SetWriteFlag(CONFIG_DEFAULT, YES);

	//Update time zone into RAM so that we can update time
	InitBasicParaConfig();
	UpdateSysDateTime();

	// First initial Logs so that all other module initialization can use it.
	InitLogs();

	for(cnt = 0; cnt < 5; cnt++)
	{
		if(stat("/media/mmcblk0p1",&tempStat) != 0)
		{
			sleep(1);
			continue;
		}
		sprintf(cmdBuf, "/media/mmcblk0p1/%s",FIRMWARE_SIGN_FILE);
		if(stat(cmdBuf,&tempStat) == 0)
		{
			DPRINT(SYS_LOG,"Backup after SDK Upgraded start\n");
			BackupAfterSDKUpgrade();
			DPRINT(SYS_LOG,"Backup completed\n");
			DPRINT(SYS_LOG,"SDK Upgraded successfully \n");
			//Get door is paneldoor or directdoor
			ReadDoorApplType();
			//Init file log para again
			InitFileLogParaConfig();
		}
		break;
	}

	DPRINT(SYS_LOG, "******* ARGO Direct Door V%02dR%02d.%02d: Application last Modified On %s at %s", SW_VER, SW_REV, SW_MINOR_REV, __DATE__, __TIME__);

	InitGpioDriver();
	InitNetworkManager();
	InitAlarms();
	// Initialize system timers which may be required by many modules during initialization
	InitSysTimer();
	// Initialize event log module which may be required by many modules during initialization
	if(GetDoorApplType() == DIRECT_DOOR_APPL)
 	{
 		InitEventLog();
 		InitWebApiEventLog();
 	}
 	else
 	{
 		InitPDEventLog();
 	}
	// Load system variables to RAM before doing any thing else
	InitSysVars();
	InitDcTask();
	InitBlockDevice();
	//This function must kept before any kind of reader initialisation
	InitReaderRxTxPara();
	// Load entire system configuration to RAM before doing any thing else
	// InitSysConfig function must be called before CheckSysDefltReq function
	// because while defaulting finger print reader, it needs to be check if
	// reader is previously configured or not.
	InitRegistry();

	//Starting WorkerThread.
	StartWorkItemThread();
	InitSysConfig();
	SetWriteFlag(CONFIG_DEFAULT, NO);
	InitReaderRelatedVars();
	// Init Combo Exit Reader
	InitCER();
	//Get time-date after time zone configuration is read
	UpdateSysDateTime();
	InitGuiHandler();
	// Check if we need to default system configuration
	// NOTE: InitBlockDevice() must be called before so that photos from memory
	// card can be removed in case of system default.
	CheckSysDefltReq();
	
	#ifdef	OEM_SUPPORTED
	OEM_Init();
	#endif //#ifdef	OEM_SUPPORTED
	
	 //Added this because when Door Type change Panel to Direct or viceversa, BLE Config blank
	//This is use in OEM_Init process
	WriteDfltSysFlag(SYS_DFLT_FALSE);

	//SetFpUsbPower(ON);

	/* If FP is detected on USB interface means no need to check further
	 * else go for switching interface from UART to USB 
	 */
	if(TRUE == Init_Internal_FP_Rdr_UsbNode_Detection())
	{
		// Check and switch communication interface from UART to USB
		SupremaFingerReaderCommProtocolCheck();
	}

        // Update GUI Logs Severity and other Info
        UpdateGuiLogInfo();
	// Send message to QT for date time format and background image
	SendMessageToQt(" ", GUI_MSG_STARTUP_INFO);
	SendMessageToQt(" ", GUI_MSG_MULTILANG_FLAG);
	SendMessageToQt(" ", GUI_MSG_MILTILANG_FONT_LOAD);
	
	ChngReqConfigForVariantWoIntFPReader(); // Change config As per ARGO CAE/CAM/CAI Variant
	
	// If wifi usb dongle is inserted and already powered on and system default
	// is to be done then system crashes, So give power to USB devices only after
	// system default is done.
	SetUsbPower(ON);
	// Init Mobile Broadband Port
	InitMobileBroadbandPort();

	// Init backup and update process
	InitBackupAndUpdate();
	InitIoLink();
	InitTimeTrigFunc();

	// Run auto reader detection related task
//	SendBootingInfoMsgToExternalReader(READER_CONFIG_SET);
	AutoReaderDetectionTask();
//	SendBootingInfoMsgToExternalReader(AUTO_DETECTION_COMPLETE);
	InitReaderTask();
//	SendBootingInfoMsgToExternalReader(INIT_PROCESS_COMPLETE);

	//Read and set last FSM sync state in Status bar
	ReadAndSetLastFSMStatus();

	// Init Printer UART for cafeteria application
	//This must be called after InitReaderTask()
	InitPrinter();

	//InitAcmsTask() must be initialized before InitWebJvsTask(), InitWebAPITask() and InitWebAPIEventsTask(FALSE)
	//Becuse socket initialization happens in InitAcmsTask()
	InitAcmsTask();
	InitWebJvsTask();
	InitWebAPITask();
	InitWebAPIEventsTask(FALSE);

#ifdef PUSH_API_SUPPORTED
	InitThirdPartyServerTask();
#endif
	InitCCCTask();
	InitPanelCommTask();
	StartSyncRtcTimer();
	CheckForDstUpdate();

	if(GetDoorApplType() == DIRECT_DOOR_APPL)
	{
		InitCafeteria();
		InitAutoInterface();
	}

	StartInputScanTimer();
	UpdateSysDateTime();
	RunDateTimeTimer();

	//initialise dvr event action
	InitDvrEvtActn();

	InitJobCost();

	//Initialise system test parameter
	InitSysTest();
	InitAutoSysTest();

	//NOTE: Do all file write related operation before this line
	sync();

	//NOTE:CheckRtcStatus function must be after power on event, because it is generating alarm and as requirement
	//from monitor side, any alarm event must be after power on event.
	CheckRtcStatus();

	InitCommTask();
	
	/* Initialise random number generator*/
	if(FALSE == InitRandomNumberGenerator())
	{
		EPRINT(SYS_LOG, "%s Failed to open file /dev/urandom");
	}

	//display firmware upgrade status.
	DisplayFwUpgradeStatusMsg();

	if( (GetDoorApplType() == DIRECT_DOOR_APPL) && (SysBasicPara.application == CAFETERIA) )
	{
		SwitchToCafeteriaState(FALSE);
	}

	// Start Watchdog
	if (watchDogEnabled == TRUE)
	{
		StartWatchDog();
	}

	// Update buzzer mute flag status
	SetBuzMuteFlagStatus(SysBasicPara.buzMuteF);
	//Set Cadence duration for Buzzer and LED
	SetCadenceDuration(SysBasicPara.accessAllowCadDuration, SysBasicPara.accessDenyCadDuration);

	InitHttp();

	//check for demo software mode
	DemoModeEnable = IsDemoSoftware();
	//check for SAMAY software mode
	SamayModeEnable = IsSamaySoftware();

	DPRINT(SYS_LOG, "Main Application started successfully!");

	// Send message to QT for start processing
	UpdateDisplayStatusField();
	SendDoorConnectivityStatusToExternalReader();
	SendMessageToQt(" ", GUI_MSG_START_GUI_APPLICATION);

	InitDoorLockState();

	if(GetDoorApplType() == PANEL_DOOR_APPL)
	{
		StartFileSyncRcvThread();
	}

	//thread to check server status
	StartCheckGoAheadThread();

	StartStopBluetoothServiceThread();

	if(GetDoorApplType() == DIRECT_DOOR_APPL)
	{
		SwitchDcToInstallationState();
	}

	//SetAudioVisualTest();
	if(BackUpAndUpdate.autoUpdate == TRUE)
	{
		FirmwareUpgradeTriger = TRUE;
	}

	#ifdef UPNP_SUPPORTED
	UpnpMx_UpdateService();
	#endif //#ifdef UPNP_SUPPORTED
	CountTotalSavedWifiNetworks();

#ifdef FACE_RECOGNITION_SUPPORT
	SendMessageToQt("", GUI_MSG_FR_SERV_ENABLE);
	StartDisplayImgUsngMjpegThread();
	StartImageSupplierThread();
#endif
	//NOTE: For Security purpose this two flag initially should be in default mode.

	/* By default disable ssh and sftp (controlling ssh also controls sftp) */
	StartStopSftp(SysParaConfig.sftpEnable);
	StartStopTelnet(FALSE);
	/* Init Temperature Sensor */
	InitTemperatureSensor(SysAdvPara.temperatureLogCnfg);

	WhileOneRunningF = TRUE;
	// DO NOT WRITE ANYTHING HERE, WRITE ABOVE THE STATEMENT[ InitTelnetFTPDemon();]

	if(GetDoorApplType() == DIRECT_DOOR_APPL)
	{
		while(1)
		{
			RunSysTimer();
			ReaderDistributionHandler();
			CerHandler();
			RunWebJvsTask();
			RunWebAPITask();
			GuiMsgHandler();
			RunIdtServerBioSyncTask();
			// Run Connection socket level task
			RunSocketTask();

#ifdef	PUSH_API_SUPPORTED
			if(ServerModeCfg.serverMode == COSEC_THIRD_PARTY)
			{
				RunThirdPartyServerTask();
			}
			else
			{
				RunAcmsTask();
			}
#else
			 RunAcmsTask();
#endif

			RunAutoInterfaceTask();
			RunPanelSocketTask();
			RunPanelCommTask();
			RunCCCTask();
			RunWebAPIEventsTask();
			CheckForOneMinuteUpdates();
			BackupUpdateHandler();
			ProcessQtMessage();
			if(SysBasicPara.application == CAFETERIA)
			{
				RunCafeScreenMirrorTask();
			}
			RunServerBioIdentificationTask();
#ifdef FACE_RECOGNITION_SUPPORT
			HandleFRIDTResponses();
#endif
			RunDvrEvntActnTask();
			RunCafeTask();
			RunServerCmdRespRcvTask();
			RunNetworkManager();
			RunIoLinkRecvTask();
			RunPingTask();
			RunFtpFrmwrUpgrdFromAcmsTask();
			RunAutoSwitchWifiTask();
			RunBluetoothTask();
			RunTemperatureSensorTask();
			MonitorProcessGetFaceReq();
			if (watchDogEnabled == TRUE)
			{
				KickWatchDog(FALSE);
			}
			usleep(2000);
		}
	}
	else
	{
		while(1)
		{
			RunSysTimer();
			ReaderDistributionHandler();
			CerHandler();
			RunWebJvsTask();
			GuiMsgHandler();
			ProcessQtMessage();
			RunCommTask();
			MsgHandler();
			RunNetworkManager();
			CheckForOneMinuteUpdatesInPanelDoor();
			RunAutoSwitchWifiTask();
			RunBluetoothTask();

			if (watchDogEnabled == TRUE)
			{
				KickWatchDog(FALSE);
			}
			usleep(2000);
		}
	}

	return 1;
}

//***************************************************************************
//	CheckForOneMinuteUpdates()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//			This function will update system every minute at 00 seconds.
//
//NOTE:  Here DST must be checked first as it needs to be applied before
//		 any other update in system. So order is important.
//***************************************************************************
static void CheckForOneMinuteUpdates(void)
{
	static UINT8	lastMin = 0xFF;
	static UINT8	lastDay = 0xFF;
	static UINT8	lastSec = 0xFF;
	static INT32	lastMonth = 0x8FFF;
	static INT32	lastYear = 0x8FFF;
	static UINT8	secCount = 0;
	static UINT8	upTimeMin = 0;
	static BOOL		startUpdateAtdncDtlForAllUsrF = FALSE;

	if( (checkFTPFirmwareReadyThreadStatus == THREAD_DONE) && (fwUpdateStage == FW_UPDATE_IDLE) )
	{
		checkFTPFirmwareReadyThreadStatus = THREAD_INACTIVE;
	}
	if(UpdateAtdncDtlForSnglUsrThrdSts == THREAD_DONE)
	{
		UpdateAtdncDtlForSnglUsrThrdSts = THREAD_INACTIVE;
		ShowAtdncDtl(UpdateAtdncDtlForSnglUserIdx);
	}

	CheckApplyLeaveResponse();

	if(CurrTimeDate.tm_sec != lastSec)
	{
		lastSec = CurrTimeDate.tm_sec;
		if( (WebAPIEventsConfig.keepLiveEvent == FALSE) &&
			(WebAPIEventsConfig.trigger == TRUE) && (WebAPIEvtInfo.start == OFF) )
		{
			secCount++;
			if( (secCount%WEB_API_RETRY_DELAY_TIME) == 0)
			{
				secCount = 0;
				WebAPIEvtInfo.start = ON;
			}
		}
		else
		{
			secCount = 0;
		}
		if(RunForgivenessTimerBasedF == TRUE)
		{
			CheckForAPBForgivenessRule();
		}
	}
	// We check to update system tasks only once every minute at 00 sec
	if(CurrTimeDate.tm_min != lastMin)
	{
		lastMin = CurrTimeDate.tm_min;
		if(RunForgivenessTimerBasedF == FALSE)
		{
			RunForgivenessTimerBasedF = TRUE;
			CheckForAPBForgivenessRule();
		}
		CheckForDstUpdate();
		CheckACSFeatureStatus();
		ProcUserSmartRouteList();
		ResetIoLinkAccToTime();
		if(startUpdateAtdncDtlForAllUsrF == TRUE)
		{
			if( (SysAdvPara.showAttndcDtlF == FALSE) || (StartUpdateAtdncDtlForAllUsrThrd() == SUCCESS) )
			{
				startUpdateAtdncDtlForAllUsrF = FALSE;
			}
		}

		// Check For FTP Firmware Upgrade
		if((BackUpAndUpdate.autoUpdate == TRUE) && (GetDoorApplType() == DIRECT_DOOR_APPL) &&
		    (IsDoorBusy() == NO) && (FirmwareUpgradeTriger == TRUE) && (upTimeMin >= FTP_FW_WAIT_ON_PWR_ON) )
		{
			if((IsFirmwareReady == FALSE) && (BenchmarkF == FALSE))
			{
				InitCheckFTPFirmwareReady();
			}
			else if(checkFTPFirmwareReadyThreadStatus == THREAD_INACTIVE)
			{
				FirmwareUpgradeTriger = FALSE;
				IsFirmwareReady = FALSE;
				DPRINT(SYS_LOG, "One MinuteTask Set Av Resp For FirmWare Upgrade.");
				SetFTPFirmWareInitilizeTimer(FTP_FIRMWARE_UPGRADE_AV_RESP_TIMER);
			}
		}
		else if(upTimeMin < FTP_FW_WAIT_ON_PWR_ON)
		{
			++upTimeMin;
		}

		// After every 5 min check Bluetooth Health.
		if((lastMin % BLE_HEALTH_CHECK_TIME) == 0)
		{
			StartBtHealthMonitorThread();
		}
	}

	if( ( (CurrTimeDate.tm_mday != lastDay) || (CurrTimeDate.tm_mon != lastMonth) || (CurrTimeDate.tm_year != lastYear)) && ( GetDstState() != DST_STATE_BACKWARD ) )
	{
		lastDay = CurrTimeDate.tm_mday;
		lastMonth = CurrTimeDate.tm_mon;
		lastYear = CurrTimeDate.tm_year;
		CheckForgivenessRule();
		CheckAbsenteeRule();
		PswdExpireCount();
		if(SysAdvPara.showAttndcDtlF == TRUE)
		{
			if(StartUpdateAtdncDtlForAllUsrThrd() == SUCCESS)
			{
				startUpdateAtdncDtlForAllUsrF = FALSE;
			}
			else
			{
				startUpdateAtdncDtlForAllUsrF = TRUE;
			}
		}

		// FTP Firmware Upgrade
		if( BackUpAndUpdate.autoUpdate == TRUE )
		{
			FirmwareUpgradeTriger = TRUE;
			IsFirmwareReady = FALSE;
		}
	}
}

//***************************************************************************
//	StartCheckGoAheadThread()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	Success / fail
//	Description:
//			This function will start thread to check server status
//**************************************************************************
static BOOL StartCheckGoAheadThread(void)
{
	BOOL status = FAIL;
	if(pthread_create(&CheckForWebServerStatusThreadFnId, NULL, CheckGoAheadRunStatusThreadFn, NULL) == 0)
	{
		status = SUCCESS;
		pthread_detach(CheckForWebServerStatusThreadFnId);
	}
	return status;
}

//***************************************************************************
//	CheckForWebServerStatusThreadFn()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//			This function will check web-server running status continuously every 10s or when server has to be
//			manually restarted
//***************************************************************************
static void *CheckGoAheadRunStatusThreadFn(VOIDPTR dummy)
{
	INT32 startGoaheadRetVal;
	CHAR	cmdBuf[255];

    snprintf(cmdBuf, 255, "cd /matrix/web/bin && start-stop-daemon -S --background -x /matrix/web/bin/goahead -- --tlsVer %d --authAlg %d --authType %d web http://:%d https://:%d", SysAdvPara.alwTLSVer, SysAdvPara.authenticationAlgorithm, SysAdvPara.authenticationType, SysAdvPara.httpPort, SysAdvPara.httpsPort);
	startGoaheadRetVal = ExeCmd(cmdBuf);

	if(startGoaheadRetVal == TRUE)
	{
		EPRINT(SYS_LOG, "webserver started successfully");
	}
	else
	{
		EPRINT(SYS_LOG, "Error while starting webserver, Error = [%d]",startGoaheadRetVal);
	}

	while(1)
	{
		sleep(10);
        snprintf(cmdBuf, 255, "pidof goahead > /dev/null || ( cd /matrix/web/bin && start-stop-daemon -S --background -x /matrix/web/bin/goahead -- --tlsVer %d --authAlg %d --authType %d web http://:%d https://:%d && exit 1 )", SysAdvPara.alwTLSVer, SysAdvPara.authenticationAlgorithm, SysAdvPara.authenticationType, SysAdvPara.httpPort, SysAdvPara.httpsPort);
		if(system(cmdBuf) != 0)
		{
			EPRINT(SYS_LOG, "webserver crashed and restarted successfully");
		}
	}
	return dummy;
}

#ifdef GDB_DEBUG_ON
static void HandlerFun(__attribute__ ((unused)) int dummy)
{
	char 			nameBuf[128];
	char 			sysCmd[256];
	char			fileName[100];
	struct stat 	buf1, buf2;

	if(stat(DUMP_FILE1, &buf1) < 0)
	{
		sprintf(fileName, ">> %s", DUMP_FILE1);
	}
	else if(buf1.st_size < DUMP_FILE_SIZE)
	{
		sprintf(fileName, ">> %s", DUMP_FILE1);
	}
	else if(stat(DUMP_FILE2, &buf2) < 0)
	{
		sprintf(fileName, ">> %s", DUMP_FILE2);
	}
	else if(buf2.st_size < DUMP_FILE_SIZE)
	{
		sprintf(fileName, ">> %s", DUMP_FILE2);
	}
	else
	{
		// last modify file if LOG_FILE
		if(buf1.st_mtime > buf2.st_mtime)
		{
			sprintf(fileName, "> %s", DUMP_FILE2);
		}
		else
		{
			sprintf(fileName, "> %s", DUMP_FILE1);
		}
	}
	sprintf(sysCmd, "echo \"##### Dump Started ######\" %s", fileName);
	system(sysCmd);
	nameBuf[readlink("/proc/self/exe", nameBuf, 127)] = 0;

	sprintf(sysCmd, "gdb --batch -n %s %d -ex 'thread apply all bt full' %s", nameBuf, getpid(), fileName);
	system(sysCmd);
	exit(0);
}
#endif
//***************************************************************************
//	CheckForOneMinuteUpdatesInPanelDoor()
//	Param: 
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//			This function will update system every minute at 00 seconds.
//
//NOTE:  Here DST must be checked first as it needs to be applied before
//		 any other update in system. So order is important.
//***************************************************************************
static void CheckForOneMinuteUpdatesInPanelDoor(void)
{
	static UINT8	lastMin = 0xFF;
	
	
	// We check to update system tasks only once every minute at 00 sec
	if(CurrTimeDate.tm_min != lastMin)
	{
		lastMin = CurrTimeDate.tm_min;
		// After every 5 min check Bluetooth Health.
 		if((lastMin % BLE_HEALTH_CHECK_TIME) == 0)
 		{
 			StartBtHealthMonitorThread();
 		}
	}
}
//*************************************************************************
//End of MxMain.c
//*************************************************************************
