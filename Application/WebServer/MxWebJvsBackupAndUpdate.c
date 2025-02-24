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
//   Project      : ACS ( Access Control System)
//   Created By   : Bharat Gohil
//   File         : MxWebJvsBackupAndUpdate.c
//   Description  :
//					This file for Update and backup file(Master firmware,Door
//					firmware,Configration file and Event file).According to
//					selection user backup or update file to ACSMaster.
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
//system includes
#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

//Matrix include files
#include "MxWebJeeves.h"
#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxEventLog.h"
#include "MxLogs.h"
#include "MxSysUtils.h"
#include "MxAcmsSocket.h"
#include "MxAcmsTask.h"
#include "MxAcmsCommand.h"
#include "MxAcmsCommon.h"
#include "MxAcmsMsgHandler.h"
#include "MxBackupUpdate.h"
#include "MxDcTask.h"
#include "MxAvResp.h"
#include "MxCerTask.h"
#include "MxSysCommonDef.h"
#include "MxDeviceSpecific.h"
#include "MxUdpPing.h"
#include "MxReaderTask.h"
#include "MxFTPFirmwareUpgrade.h"
#include "MxConfigFileIo.h"
#include "MxWebJvsBackupAndUpdate.h"
#include "MxCommTask.h"
#include "MxBlockDevice.h"
#include "MxReaderTask.h"
#include "MxSysCommonUtils.h"

//******** Extern Variables **********
extern ETHERNET_PARA_t		EthernetParameters;
extern MMC_INFO_t 			MmcInfo;
extern BOOL					SfprSensorSTQCApprovedF;
extern READER_AUTO_DETECT_PROC_INFO_t				ReaderAutoDetectInfo;
extern READER_STATUS_e 								readerStatus[MAX_READERS];
extern SYS_BASIC_PARA_t								SysBasicPara;
extern CHAR 										BenchmarkFWzip[BENCHMARK_FW_STR_LEN];
//******** Defines and Data Types ****
#define MIN_SPACE_REQ_FOR_SDK_UPGRADE	(200*1024*1024)	// 200 MB size required in Memory card for SDK upgrade

//******** Function Prototypes *******
static VOIDPTR GetBackupAndUpdateThread(VOIDPTR dummy);
static VOIDPTR SetBackupAndUpdateThread(VOIDPTR dummy);

//******** Global Variables **********


//******** Static Variables **********
static pthread_t			backupAndUpdateThreadData = 0;
static THREAD_STATUS_e		backupAndUpdateThreadStatus = THREAD_INACTIVE;
static CHAR 				folderName[MAC_FOLDER_NAME_SIZE];
static CHAR					fileStr[100];
static FIRMWARE_UPGRADE_STATUS_e	updateStatus = FIRMWARE_UPGRADE_FAIL;
static BACKUP_UPDATE_TYPE_e			buType = BU_TYPE_NONE;


#define GEN_BACKUP_UPGRADE_ENUM(BACKUP_UPGRADE_ENUM, BACKUP_UPGRADE_NAME) BACKUP_UPGRADE_ENUM,
#define GEN_BACKUP_UPGRADE_NAME(BACKUP_UPGRADE_ENUM, BACKUP_UPGRADE_NAME) BACKUP_UPGRADE_NAME,

#define GEN_MMC_DETAIL_ENUM(MMC_DETAIL_ENUM, MMC_DETAIL_NAME) MMC_DETAIL_ENUM,
#define GEN_MMC_DETAIL_NAME(MMC_DETAIL_ENUM, MMC_DETAIL_NAME) MMC_DETAIL_NAME,

#define BACKUP_UPGRADE_SETTING(API)\
			API(BACKUP_UPGRADE_FILE,					"file")\
			API(BACKUP_UPGRADE_FILENAME,				"FileName")\
			API(BACKUP_UPGRADE_STATUS,					"status")\
			API(BACKUP_UPGRADE_BACKUP_UPGRADE_TYPE,		"buType")\
			API(BACKUP_UPGRADE_BENCHMARK_FW_VER,		"BenchmarkFwVer")\

#define MMC_DETAIL(API)\
			API(MMC_STATUS,			"status")\
			API(MMC_FREE_SPACE,		"freeSpace")\

#define SDK_VER_DETAIL(API)\
			API(SDK_VER_API,		"sdkVer")\

#define STQC_VALIDATION_DETAIL(API)\
			API(STQC_STATUS_API,		"stqcStatus")\

typedef enum
{
	BACKUP_UPGRADE_SETTING(GEN_BACKUP_UPGRADE_ENUM)
	MAX_BACKUP_UPGRADE_SETTING,
}BACKUP_UPGRADE_CONFIG_e;

typedef enum
{
	MMC_DETAIL(GEN_MMC_DETAIL_ENUM)
	MAX_MMC_DETAIL,
}MMC_DETAIL_e;

typedef enum
{
	SDK_VER_DETAIL(GEN_MMC_DETAIL_ENUM)
	MAX_SDK_VER_DETAIL,
}SDK_VER_DETAIL_e;

typedef enum
{
	STQC_VALIDATION_DETAIL(GEN_MMC_DETAIL_ENUM)
	MAX_STQC_VALIDATION_DETAIL,
}STQC_VALIDATION_DETAIL_e;

static CHARPTR backupUpgradeConfig [MAX_BACKUP_UPGRADE_SETTING]=
{
		BACKUP_UPGRADE_SETTING(GEN_BACKUP_UPGRADE_NAME)
};

static CHARPTR mmcDetail [MAX_MMC_DETAIL]=
{
		MMC_DETAIL(GEN_MMC_DETAIL_NAME)
};

static CHARPTR sdkVerDetail [MAX_SDK_VER_DETAIL]=
{
		SDK_VER_DETAIL(GEN_MMC_DETAIL_NAME)
};

static CHARPTR stqcValidationDetail [MAX_STQC_VALIDATION_DETAIL]=
{
		STQC_VALIDATION_DETAIL(GEN_MMC_DETAIL_NAME)
};

#define GEN_EXT_RDR_TYPE_ENUM(EXT_RDR_TYPE_ENUM, EXT_RDR_TYPE_NAME) EXT_RDR_TYPE_ENUM,
#define GEN_EXT_RDR_TYPE_NAME(EXT_RDR_TYPE_ENUM, EXT_RDR_TYPE_NAME) EXT_RDR_TYPE_NAME,

#define EXT_RDR_TYPE(API)\
		API(EXT_RDR_TYPE,			"rdrType")\
		API(EXT_RDR_HW_TYPE,			"rdrHwType")\
		API(EXT_RDR_FW_VER_DG_CTRL,		"atomFwVerDgCtrl")\
		
typedef enum
{
	EXT_RDR_TYPE(GEN_EXT_RDR_TYPE_ENUM)
	MAX_EXT_RDR_TYPE,
}EXT_RDR_TYPE_CONFIG_e;

static CHARPTR extReaderType[MAX_EXT_RDR_TYPE]=
{
		EXT_RDR_TYPE(GEN_EXT_RDR_TYPE_NAME)
};
//******** Function Definations ******

//*****************************************************************************
//	SetXmlBackupAndUpdate()
//	Param:
//		IN: pRXDataFile - pointer to  received data file (RxData.txt).
//		OUT:NONE.
//	Returns:NONE
//	Description:
//			When user select update file(firmware or config ) at client
//	 		side, function extract(unzip) file at ACSMaster folder.
//			This function activates thread to do functionalities in background.
//*****************************************************************************
UINT8 SetBackupAndUpgrade(FILE *pRXDataFile, FILE *pTXDataFile)
{
	UINT8 	status = WAIT;
	CHAR 	tempFieldName[MAX_WEB_JVS_FIELD_NAME_LENGTH], tempFieldValue[MAX_WEB_JVS_FIELD_VALUE_LENGTH];
	UINT8	parseStatus, getElementStatus, elementIdx;

	if(backupAndUpdateThreadStatus == THREAD_ACTIVE)
	{
		return WAIT;
	}

	if(backupAndUpdateThreadStatus == THREAD_DONE)
	{
		WriteWebResponse(pTXDataFile, "0");
		WriteJSONStartTag(pTXDataFile);	//JSON Data Start
		WriteJSONNumericData(pTXDataFile, backupUpgradeConfig[BACKUP_UPGRADE_BACKUP_UPGRADE_TYPE], buType);
		WriteJSONNumericData(pTXDataFile, backupUpgradeConfig[BACKUP_UPGRADE_STATUS], updateStatus);
		if((updateStatus == FIRMWARE_UPGRADE_BENCHMARK_FOUND) && (strlen(BenchmarkFWzip) != 0))
		{
			CHAR tempBmVer[BENCHMARK_FW_STR_LEN];
			strncpy(tempBmVer, &BenchmarkFWzip[(strlen(DIRECT_DOORFIRMWARE_FILE) + 1)], (strlen(BenchmarkFWzip) - (strlen(DIRECT_DOORFIRMWARE_FILE) + 1) - 4));
			WriteJSONStringData(pTXDataFile, backupUpgradeConfig[BACKUP_UPGRADE_BENCHMARK_FW_VER], tempBmVer);
		}
		WriteJSONENDTag(pTXDataFile);	//JSON Data END
		buType = BU_TYPE_NONE;

		backupAndUpdateThreadStatus = THREAD_INACTIVE;
		status = SUCCESS;

		return status;
	}

	// If thread is not active already then only activate thread
	if(backupAndUpdateThreadStatus == THREAD_INACTIVE)
	{
		while( (parseStatus = ParseQueryField(pRXDataFile, &tempFieldName[0], MAX_WEB_JVS_FIELD_NAME_LENGTH, &tempFieldValue[0], MAX_WEB_JVS_FIELD_VALUE_LENGTH) ) != FAIL)
		{
			DPRINT(WEBJVS_LOG, "%s=%s", tempFieldName, tempFieldValue);
			if( (getElementStatus = GetWebPageElementIdx(&backupUpgradeConfig[0], MAX_BACKUP_UPGRADE_SETTING, tempFieldName, &elementIdx)) == SUCCESS)
			{
				switch(elementIdx)
				{
					case BACKUP_UPGRADE_FILE:
					{
						snprintf(fileStr, sizeof(fileStr), "%s", tempFieldValue);
					}
					break;

					default:break;
				}
			}
			if( (parseStatus == WEB_JVS_LAST_FIELD) || (parseStatus == WEB_JVS_LAST_FIELD_WITHOUT_DATA) )
			{
				GetFolderName(folderName);
				if(pthread_create(&backupAndUpdateThreadData, NULL, SetBackupAndUpdateThread, NULL) == 0)
				{
					// In case thread is completed just before call reaches here (which is observed),
					// do not change thread status to active
					if(backupAndUpdateThreadStatus == THREAD_INACTIVE)
					{
						backupAndUpdateThreadStatus = THREAD_ACTIVE;
					}
					// Thread created successfully now detach thread
					pthread_detach(backupAndUpdateThreadData);
				}
				break;
			}
		}
	}

	return status;
}


//*****************************************************************************
//	SetBackupAndUpdateThread()
//	Param:
//		IN :dummy	- unused
//		OUT:NONE
//	Returns:NONE
//	Description:
//			When user select update file(firmware or config) at client side,
//	 		this function extract(unzip) files at ACSMaster folder.
//*****************************************************************************
static VOIDPTR SetBackupAndUpdateThread(UNUSED_PARA VOIDPTR dummy)
{
	BOOL		folderPresentF = FALSE;
	CHAR 		cmdBuf[255], zipFileName[25];
	FILE *		fdLogFile = NULL;
	struct stat buf;
	updateStatus = FALSE;

	strcpy(zipFileName, folderName);
	strcat(zipFileName, ZIP_FILE_EXT);

	if( (strcmp(fileStr, zipFileName) == 0) && (GetDoorApplType() == DIRECT_DOOR_APPL) )
	{

		buType = BU_TYPE_CFG;

		if(IsDoorBusy() == NO)
		{
			printf("door is free \n");
			LogoutAcms(ACMS_COMM_HOLD, DO_NOT_CLOSE_ALL_SERVER_SOCKETS);
			sprintf(cmdBuf, "unzip -o -q %s%s -d %s", WEB_BIN_PATH, zipFileName, ZIP_PATH);
			system(cmdBuf);

			sprintf(cmdBuf, "rm -f %s%s", WEB_BIN_PATH, zipFileName);
			system(cmdBuf);

			sprintf(cmdBuf, "%s%s/%s/%s", ZIP_PATH, folderName, CONFIG_FOLDER, CONFIG_LOG_FILE);
			// Open file to read config log status
			fdLogFile = fopen(cmdBuf, "r+");
			if(fdLogFile != NULL)
			{
				fgets(fileStr, sizeof(fileStr), fdLogFile);
				fclose(fdLogFile);

				// Check write success first
				if(strncmp(fileStr, "1", 1) == 0)
				{
					ApplyAcmsConfig(folderName);
					updateStatus = TRUE;
				}
			}
			else
			{
				EPRINT(WEBJVS_LOG, "Config Log file does not found");
			}
			// Now remove folder	function loadXMLDoc()

			sprintf(cmdBuf, "rm -rf %s%s", ZIP_PATH, folderName);
			system(cmdBuf);
			LogoutAcms(ACMS_COMM_LOGOUT, DO_NOT_CLOSE_ALL_SERVER_SOCKETS);
		}
		else
		{
			sprintf(cmdBuf, "rm -f %s%s", WEB_BIN_PATH, zipFileName);
			system(cmdBuf);
		}
	}
	// Extract(unzip) Firmware

	else if((strncmp(fileStr, DIRECT_DOORFIRMWARE_FILE, strlen(DIRECT_DOORFIRMWARE_FILE)) == 0) ||
			(strncmp(fileStr, COSEC_ARGO_SDK_FILE, strlen(COSEC_ARGO_SDK_FILE) ) == 0)			)
	{
		BOOL sdkFoundF = FALSE;
		if( strncmp(fileStr, COSEC_ARGO_SDK_FILE, strlen(COSEC_ARGO_SDK_FILE)) == 0 )
		{
			sdkFoundF = TRUE;
		}
		buType = BU_TYPE_FIRMWARE;
		updateStatus = FIRMWARE_UPGRADE_FAIL;
		if(GetDoorApplType() == DIRECT_DOOR_APPL)
		{
			if(IsDoorBusy() == NO)
			{
				LogoutAcms(ACMS_COMM_HOLD, DO_NOT_CLOSE_ALL_SERVER_SOCKETS);
				SetHttpFirmwareUploadState();
				if(sdkFoundF == FALSE)
				{
					sprintf(cmdBuf, "%s%s", WEB_BIN_PATH, fileStr);
				}
				else
				{
					sprintf(cmdBuf, "%s%s", TEMP_DIR, fileStr);
				}
				if(stat(cmdBuf, &buf) == 0)
				{
					if(sdkFoundF == FALSE)
					{
						/* Check existence of folder before moving a file into it */
						snprintf(cmdBuf, sizeof(cmdBuf), "%s", ZIP_PATH);
						if (access(cmdBuf, F_OK | R_OK | W_OK) != 0)
						{
							/* Create the folder because it does not exist */
							snprintf(cmdBuf, sizeof(cmdBuf), "mkdir -p %s", ZIP_PATH);
							if (ExeCmd(cmdBuf) == FALSE)
							{
								EPRINT(SYS_LOG, "Failed to create folder [%s]", ZIP_PATH);
								folderPresentF = FALSE;
							}
							else
							{
								folderPresentF = TRUE;
							}
						}
						else
						{
							folderPresentF = TRUE;
						}

						if (folderPresentF == TRUE)
						{
							/* Now move the file into 'zip' folder */
							snprintf(cmdBuf, sizeof(cmdBuf), "mv -f %s%s %s", WEB_BIN_PATH, fileStr, ZIP_PATH);
							if (ExeCmd(cmdBuf) == FALSE)
							{
								EPRINT(SYS_LOG, "Failed to move [%s%s] to [%s]", WEB_BIN_PATH, fileStr, ZIP_PATH);
							}
							else
							{
								/* Now start firmware upgrade */
								updateStatus = StartSystemFwUpgrade(fileStr, ONLY_BM_CHECK);
							}
						}
					}
					else
					{
						// if it is sdk then don't move to 'zip'.
						updateStatus = StartSystemSdkUpgrade(fileStr);
					}

					if(updateStatus == FIRMWARE_UPGRADE_SUCCESS)
					{
						SetAudioVisual(USER_RESP_BACKUP_UPDATE_XFER_SUCCESS, INTERNAL_PORT, &buType);
					}
					else if(updateStatus == FIRMWARE_UPGRADE_CRC_FAIL)
					{
						SetAudioVisual(USER_RESP_BACKUP_UPDATE_CHECKSUM_FAIL, INTERNAL_PORT, &buType);
					}
					else if(updateStatus == FIRMWARE_UPGRADE_DOWNGRADE_CNTRL)
					{
						SetAudioVisual(USER_RESP_BACKUP_UPDATE_DOWNGRAGE_CNTRL, INTERNAL_PORT, &buType);
					}
					else if(updateStatus == FIRMWARE_UPGRADE_BENCHMARK_FOUND)
					{
						SetAudioVisual(USER_RESP_BENCHMARK_FW_FOUND, INTERNAL_PORT, &buType);
					}
					else
					{
						SetAudioVisual(USER_RESP_BACKUP_UPDATE_XFER_FAILED, INTERNAL_PORT, &buType);
					}
				}
				else
				{
					SetAudioVisual(USER_RESP_BACKUP_UPDATE_XFER_FAILED, INTERNAL_PORT, &buType);
					EPRINT(WEBJVS_LOG, "Failed to upgrade, file not present[%s]", cmdBuf);
				}
				SwitchDcToIdleState();
				LogoutAcms(ACMS_COMM_LOGOUT, DO_NOT_CLOSE_ALL_SERVER_SOCKETS);
			}
		}
		// PANEL_DOOR_SUPPORT
		else
		{
			EPRINT(BU_LOG, "Firmware upgrade failed");
			updateStatus = FIRMWARE_UPGRADE_FAIL;
			StopCommTask();
			ChangeFirmwareUpgradeState(FIRMWARE_UPGRADE_PROGRESS);
			if(sdkFoundF == FALSE)
			{
				sprintf(cmdBuf, "%s%s", WEB_BIN_PATH, fileStr);
			}
			else
			{
				sprintf(cmdBuf, "%s%s", TEMP_DIR, fileStr);
			}

			if(stat(cmdBuf, &buf) == 0)
			{
				if(sdkFoundF == FALSE)
				{
					/* Check existence of folder before moving a file into it */
					snprintf(cmdBuf, sizeof(cmdBuf), "%s", ZIP_PATH);
					if (access(cmdBuf, F_OK | R_OK | W_OK) != 0)
					{
						/* Create the folder because it does not exist */
						snprintf(cmdBuf, sizeof(cmdBuf), "mkdir -p %s", ZIP_PATH);
						if (ExeCmd(cmdBuf) == FALSE)
						{
							EPRINT(SYS_LOG, "Failed to create folder [%s]", ZIP_PATH);
							folderPresentF = FALSE;
						}
						else
						{
							folderPresentF = TRUE;
						}
					}
					else
					{
						folderPresentF = TRUE;
					}

					if (folderPresentF == TRUE)
					{
						/* Now move the file into 'zip' folder */
						snprintf(cmdBuf, sizeof(cmdBuf), "mv -f %s%s %s", WEB_BIN_PATH, fileStr, ZIP_PATH);
						if (ExeCmd(cmdBuf) == FALSE)
						{
							EPRINT(SYS_LOG, "Failed to move [%s%s] to [%s]", WEB_BIN_PATH, fileStr, ZIP_PATH);
						}
						else
						{
							/* Now start firmware upgrade */
							updateStatus = StartSystemFwUpgrade(fileStr, ONLY_BM_CHECK);
						}
					}
				}
				else
				{
					// if it is sdk then don't move to 'zip'.
					updateStatus = StartSystemSdkUpgrade(fileStr);
				}

				if(updateStatus == FIRMWARE_UPGRADE_SUCCESS)
				{
					SetAudioVisual(USER_RESP_BACKUP_UPDATE_XFER_SUCCESS, INTERNAL_PORT, &buType);
				}
				else if(updateStatus == FIRMWARE_UPGRADE_CRC_FAIL)
				{
					SetAudioVisual(USER_RESP_BACKUP_UPDATE_CHECKSUM_FAIL, INTERNAL_PORT, &buType);
				}
				else if(updateStatus == FIRMWARE_UPGRADE_DOWNGRADE_CNTRL)
				{
					SetAudioVisual(USER_RESP_BACKUP_UPDATE_DOWNGRAGE_CNTRL, INTERNAL_PORT, &buType);
				}
				else if(updateStatus == FIRMWARE_UPGRADE_BENCHMARK_FOUND)
				{
					SetAudioVisual(USER_RESP_BENCHMARK_FW_FOUND, INTERNAL_PORT, &buType);
				}
				else
				{
					SetAudioVisual(USER_RESP_BACKUP_UPDATE_XFER_FAILED, INTERNAL_PORT, &buType);
				}
			}
			else
			{
				SetAudioVisual(USER_RESP_BACKUP_UPDATE_XFER_FAILED, INTERNAL_PORT, &buType);
				EPRINT(WEBJVS_LOG, "Failed to upgrade, file not present[%s]", fileStr);
			}
			ChangeFirmwareUpgradeState(FIRMWARE_UPGRADE_IDLE);
			OpenUdpPingSocket();
		}
	}
	//CER FW upgrade
	else if(strncmp(fileStr, COSEC_CER_FIRMWARE_FILE, strlen(COSEC_CER_FIRMWARE_FILE) ) == 0)
	{
		buType = BU_TYPE_FIRMWARE;
		if( (IsDoorBusy() == NO) && (IsCerFwInProgress() == NO) )
		{
			if(GetDoorApplType() == PANEL_DOOR_APPL)
			{
				StopCommTask();
				ChangeFirmwareUpgradeState(FIRMWARE_UPGRADE_PROGRESS);
			}
			sprintf(cmdBuf, "%s%s", WEB_BIN_PATH, fileStr);
			if(stat(cmdBuf, &buf) == 0)
			{
				BOOL md5sumMatchF = FALSE;
				md5sumMatchF = CheckMd5sumForReaderFw(COMBO_READER_CER,fileStr);

				if(md5sumMatchF == FALSE)
				{
				    updateStatus = FIRMWARE_UPGRADE_CRC_FAIL;
				}
				else
				{
				    // Now move the file into 'zip' folder
				    sprintf(cmdBuf, "mv -f %s%s %s%s%s", WEB_BIN_PATH, fileStr, ZIP_PATH,
				                    COSEC_CER_FIRMWARE_FILE, COSEC_CER_FIRMWARE_FILE_EXT);
				    system(cmdBuf);
				    updateStatus = TRUE;
				    // Now start firmware upgrade
				    StartCerFwUpgradeCmd(COMBO_READER_CER);
				}
			}
			else
			{
				SetAudioVisual(USER_RESP_FIRMWARE_UPLOAD_STATUS_FAIL, INTERNAL_PORT, NULL);
				EPRINT(WEBJVS_LOG, "Failed to upgrade, file not present[%s]\n", fileStr);
			}
			SwitchDcToIdleState();
			if(GetDoorApplType() == PANEL_DOOR_APPL)
			{
				ChangeFirmwareUpgradeState(FIRMWARE_UPGRADE_IDLE);
				OpenUdpPingSocket();
			}
		}
		else
		{
			EPRINT(WEBJVS_LOG, "Device is busy[%s]\n", fileStr);
		}
	}
	//CB Reader FW Upgrade.
	else if(strncmp(fileStr, COSEC_CBR_FIRMWARE_FILE, strlen(COSEC_CBR_FIRMWARE_FILE) ) == 0)
	{
		buType = BU_TYPE_FIRMWARE;
		if( (IsDoorBusy() == NO) && (IsCerFwInProgress() == NO) )
		{
			if(GetDoorApplType() == PANEL_DOOR_APPL)
			{
				StopCommTask();
				ChangeFirmwareUpgradeState(FIRMWARE_UPGRADE_PROGRESS);
			}
			sprintf(cmdBuf, "%s%s", WEB_BIN_PATH, fileStr);
			if(stat(cmdBuf, &buf) == 0)
			{
				// Now move the file into 'zip' folder
				sprintf(cmdBuf, "mv -f %s%s %s%s%s", WEB_BIN_PATH, fileStr, ZIP_PATH,
						COSEC_CER_FIRMWARE_FILE, COSEC_CER_FIRMWARE_FILE_EXT);	//kept as it is as cer as this file will be checked in StartCerFwUpgradeCmd().
				system(cmdBuf);
				updateStatus = TRUE;
				// Now start firmware upgrade
				if(ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_CB_U] == TRUE)
				{
					StartCerFwUpgradeCmd(COMBO_READER_CB_U);
				}
				else if (ReaderAutoDetectInfo.comboBlDetctedF[COMBO_READER_CB_U] == TRUE)
				{
					InitCerReaderRelatedInfo();
					StartCerFwUpgradeCmd(COMBO_READER_CB_U);
				}
			}
			else
			{
				SetAudioVisual(USER_RESP_FIRMWARE_UPLOAD_STATUS_FAIL, INTERNAL_PORT, NULL);
				EPRINT(WEBJVS_LOG, "Failed to upgrade, file not present[%s]\n", fileStr);
			}
			SwitchDcToIdleState();
			if(GetDoorApplType() == PANEL_DOOR_APPL)
			{
				ChangeFirmwareUpgradeState(FIRMWARE_UPGRADE_IDLE);
				OpenUdpPingSocket();
			}
		}
		else
		{
			EPRINT(WEBJVS_LOG, "Device is busy[%s]\n", fileStr);
		}
	}
	//ATOM RD100 Reader FW Upgrade.
	else if(strncmp(fileStr, COSEC_ATOM_FIRMWARE_FILE, strlen(COSEC_ATOM_FIRMWARE_FILE) ) == 0)
	{
		buType = BU_TYPE_FIRMWARE;
		if((((ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_ATOM_RD300] == TRUE) 
				    || (ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_ATOM_RD200] == TRUE)) 
			  && (ATOM_BLE_TYPE_TELIT == GetAtomReaderHwType())) && (ValidateAtomReaderFwVer(fileStr) == FAIL))
		{
				EPRINT(SYS_LOG, "Atom Reader Firmware Version Is Not Proper");
				updateStatus = FIRMWARE_UPGRADE_DOWNGRADE_CNTRL;
				EPRINT(WEBJVS_LOG, "Failed to upgrade, file not present[%s]\n", fileStr);
				SwitchDcToIdleState();
		}
		else
		{
			if( (IsDoorBusy() == NO) && (IsCerFwInProgress() == NO) )
			{
				if(GetDoorApplType() == PANEL_DOOR_APPL)
				{
					StopCommTask();
					ChangeFirmwareUpgradeState(FIRMWARE_UPGRADE_PROGRESS);
				}
				sprintf(cmdBuf, "%s%s", WEB_BIN_PATH, fileStr);
				if(stat(cmdBuf, &buf) == 0)
				{
					BOOL md5sumMatchF = FALSE;
					if (memmem(fileStr, sizeof(fileStr), COSEC_ATOM_RD200_300_FIRMWARE_FILE_EXT, strlen(COSEC_ATOM_RD200_300_FIRMWARE_FILE_EXT)) != NULL)
					{
						// Now move the file into 'zip' folder
						sprintf(cmdBuf, "mv -f %s%s %s%s%s", WEB_BIN_PATH, fileStr, ZIP_PATH,
								COSEC_ATOM_FIRMWARE_FILE, COSEC_ATOM_RD200_300_FIRMWARE_FILE_EXT);
						md5sumMatchF = TRUE;
					}
					else
					{
						md5sumMatchF = CheckMd5sumForReaderFw(COMBO_READER_ATOM_RD100,fileStr);

						if(md5sumMatchF == FALSE)
						{
						    updateStatus = FIRMWARE_UPGRADE_CRC_FAIL;
						}
						else
						{
						    // Now move the file into 'zip' folder
						    sprintf(cmdBuf, "mv -f %s%s %s%s%s", WEB_BIN_PATH, fileStr, ZIP_PATH,
						            COSEC_CER_FIRMWARE_FILE, COSEC_CER_FIRMWARE_FILE_EXT);	//kept as it is as cer as this file will be checked in StartCerFwUpgradeCmd().
						}
					}

					if(md5sumMatchF == TRUE)
					{
						system(cmdBuf);
						updateStatus = TRUE;
						// Now start firmware upgrade
						if(ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_ATOM_RD100] == TRUE)
						{
						    StartCerFwUpgradeCmd(COMBO_READER_ATOM_RD100);
						}
						else if (ReaderAutoDetectInfo.comboBlDetctedF[COMBO_READER_ATOM_RD100] == TRUE)
						{
						    InitCerReaderRelatedInfo();
						    StartCerFwUpgradeCmd(COMBO_READER_ATOM_RD100);
						}
						else if(ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_ATOM_RD300] == TRUE)
						{
						    StartCerFwUpgradeCmd(COMBO_READER_ATOM_RD300);
						}
						else if(ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_ATOM_RD200] == TRUE)
						{
						    StartCerFwUpgradeCmd(COMBO_READER_ATOM_RD200);
						}
					}
				}
				else
				{
					SetAudioVisual(USER_RESP_FIRMWARE_UPLOAD_STATUS_FAIL, INTERNAL_PORT, NULL);
					EPRINT(WEBJVS_LOG, "Failed to upgrade, file not present[%s]\n", fileStr);
				}
				SwitchDcToIdleState();
				if(GetDoorApplType() == PANEL_DOOR_APPL)
				{
					ChangeFirmwareUpgradeState(FIRMWARE_UPGRADE_IDLE);
					OpenUdpPingSocket();
				}
			}
			else
			{
				EPRINT(WEBJVS_LOG, "Device is busy[%s]\n", fileStr);
			}
		}
	}
	sprintf(cmdBuf, "rm -rf %s/%s", ATOM_DECRYPT_ZIP_PATH,fileStr);
	system(cmdBuf);
	memset(cmdBuf, 0 ,sizeof(cmdBuf));
	sprintf(cmdBuf, "rm -rf %s%s", WEB_BIN_PATH, fileStr);
	system(cmdBuf);
	backupAndUpdateThreadStatus = THREAD_DONE;
	pthread_exit(NULL);
}

//*****************************************************************************
//	GetXmlBackupAndUpdate()
//	Param:
//		IN:	pHeader - Pointer to Page Header.
//		OUT:pDataFile - Pointer to Xml file discriptor where data is stored.
//	Returns:SUCCESS/FAIL/WAIT
//	Description:
//			When client request for download file, this function zips all file
//			and send to client.
//			This function activates thread to do functionalities in background.
//*****************************************************************************
UINT8 GetEventBackup(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	UINT8	status = WAIT;
	CHAR	valueStr[MAX_WEB_JVS_FIELD_VALUE_LENGTH];

	// If thread is not active already then only activate thread
	if(backupAndUpdateThreadStatus == THREAD_INACTIVE)
	{
			// Get folder name first
			GetFolderName(folderName);

			if(pthread_create(&backupAndUpdateThreadData, NULL, GetBackupAndUpdateThread, NULL) == 0)
			{
				// In case thread is completed just before call reaches here (which is observed),
				// do not change thread status to active
				if(backupAndUpdateThreadStatus == THREAD_INACTIVE)
				{
					backupAndUpdateThreadStatus = THREAD_ACTIVE;
				}
				// Thread created successfully now detach thread ??
				pthread_detach(backupAndUpdateThreadData);
			}
			else
			{
				status = FAIL;
			}
	}

	else if(backupAndUpdateThreadStatus == THREAD_DONE)
	{
		WriteWebResponse(pTXDataFile, "0");
		WriteJSONStartTag(pTXDataFile);	//JSON Data Start
		sprintf(valueStr, "%s%s",folderName, ZIP_FILE_EXT);
		WriteJSONStringData(pTXDataFile, backupUpgradeConfig[BACKUP_UPGRADE_FILENAME], valueStr);
		WriteJSONNumericData(pTXDataFile, backupUpgradeConfig[BACKUP_UPGRADE_STATUS], status);

		backupAndUpdateThreadStatus = THREAD_INACTIVE;
		status = SUCCESS;
		WriteJSONENDTag(pTXDataFile);
	}
	return status;
}

//*****************************************************************************
//	GetBackupAndUpdateThread()
//	Param:
//		IN:	dummy	- unused
//		OUT:NONE
//	Returns:NONE
//	Description:
//			When user select update file(firmware or config) at client side,
//	 		this function extract(unzip) files at ACSMaster folder.
//*****************************************************************************
static VOIDPTR GetBackupAndUpdateThread(UNUSED_PARA VOIDPTR dummy)
{
	UINT16			rollOverCnt, length, writeCnt;
	UINT32			eventCnt, cnt, readTotEve;
	CHAR 			cmdBuf[255];
	FILE			*fd;

	if(GetDoorApplType() == PANEL_DOOR_APPL)
	{
		backupAndUpdateThreadStatus = THREAD_DONE;
		pthread_exit(NULL);
	}

	DPRINT(SYS_LOG,"Event Backup started");

	sprintf(cmdBuf, "rm -f %s%s%s", WEB_PATH, folderName, ZIP_FILE_EXT);
	system(cmdBuf);

	// Remove if folder already there
	sprintf(cmdBuf, "rm -rf /%s", folderName);
	system(cmdBuf);

	// Make directory first
	sprintf(cmdBuf, "mkdir -p /%s", folderName);
	system(cmdBuf);
	sprintf(cmdBuf, "mkdir -p /%s/%s", folderName, EVENT_BACKUP_FOLDER);
	system(cmdBuf);

	// Now write file
	eventCnt = GetCurEventCount();
	rollOverCnt = GetCurEventRollOverCount();

	if(rollOverCnt > 0)
	{
		rollOverCnt--;
		readTotEve = MAX_EVENT_COUNT;
	}
	else
	{
		readTotEve = eventCnt;
		eventCnt = 0;
	}

	sprintf(cmdBuf, "/%s/%s/%s", folderName, EVENT_BACKUP_FOLDER, EVENT_BACKUP_FILE);
	//Open file to write event
    fd = fopen(cmdBuf, "a");

	if(fd != NULL)
	{
		// First line write mac address
		sprintf(cmdBuf, "%s\n%d\n", EthernetParameters.macAddress, readTotEve);
		length = strlen(cmdBuf);
		writeCnt = fwrite(cmdBuf, 1, length, fd);

		if(writeCnt >= length)
		{
			for(cnt = 1; cnt <= readTotEve; cnt++)
			{
				eventCnt++;

				if(eventCnt > MAX_EVENT_COUNT)
				{
					eventCnt = 1;
					rollOverCnt++;
				}

				if(ReadAcmsEvent(rollOverCnt, eventCnt, cmdBuf, &length) == SUCCESS)
				{
					cmdBuf[length++] = '\n';

					writeCnt = fwrite(cmdBuf, 1, length, fd);

					if(writeCnt != length)
					{
						EPRINT(WEBJVS_LOG, "Event File Write Fail Event Count [%d]", eventCnt);
					}
				}
				else
				{
					EPRINT(WEBJVS_LOG, "Read Event Fail Event Count [%d]", eventCnt);
				}
			}

			fflush(fd);
			fclose(fd);

			//Open file to write event write status
			sprintf(cmdBuf, "/%s/%s/%s", folderName, EVENT_BACKUP_FOLDER, EVENT_BACKUP_LOG_FILE);
			fd = fopen(cmdBuf, "w");

			if(fd != NULL)
			{
				sprintf(cmdBuf, "%d", SUCCESS);
				length = strlen(cmdBuf);
				fwrite(cmdBuf, 1, length, fd);

				fflush(fd);
				fclose(fd);

				// Zip event file
				sprintf(cmdBuf, "zip -r -q /%s%s%s /%s", WEB_PATH, folderName, ZIP_FILE_EXT, folderName);
				system(cmdBuf);
				// Remove folder now
				sprintf(cmdBuf, "rm -rf /%s", folderName);
				system(cmdBuf);

				DPRINT(SYS_LOG,"Event Backup over");
			}
			else
			{
				EPRINT(SYS_LOG,"Event Backup File Open Error: %s", EVENT_BACKUP_LOG_FILE);
			}
		}
		else
		{
			fclose(fd);
			EPRINT(SYS_LOG,"Event Backup Fail");
		}

		backupAndUpdateThreadStatus = THREAD_DONE;
		pthread_exit(NULL);
	}
	else
	{
		EPRINT(SYS_LOG,"File Open Error: %s", EVENT_BACKUP_FILE);
	}
	return NULL;
}

//*****************************************************************************
//	GetMmcStatus()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:It will give memory card status either connected or not and free space available in memory card
//
//*****************************************************************************
UINT8 GetMmcStatus(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	UINT64 tempFreeSpace = 0;

	//Write responce data
	WriteWebResponse(pTXDataFile, "0");
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start

	WriteJSONNumericData(pTXDataFile, mmcDetail[MMC_STATUS], MmcInfo.status);
	tempFreeSpace = GetFreeSpaceAvailInSDcard();

	if(tempFreeSpace > MIN_SPACE_REQ_FOR_SDK_UPGRADE)
	{
		WriteJSONNumericData(pTXDataFile, mmcDetail[MMC_FREE_SPACE],	SUCCESS);
	}
	else
	{
		WriteJSONNumericData(pTXDataFile, mmcDetail[MMC_FREE_SPACE],	FAIL);
	}

	WriteJSONENDTag(pTXDataFile);	//JSON Data END

	return SUCCESS;
}

//*****************************************************************************
//	GetSdkVer()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:It will give sdk version
//
//*****************************************************************************
UINT8 GetSdkVer(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	//Write responce data
	WriteWebResponse(pTXDataFile, "0");
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	WriteJSONNumericData(pTXDataFile, sdkVerDetail[SDK_VER_API], ACMS_FIRMWARE_FILE_VER);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END

	return SUCCESS;
}

//*****************************************************************************
//	GetSTQCSensorValidation()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description:It will give STQC status, either FP sensor is STQC approved OR not
//
//*****************************************************************************
BOOL GetSTQCSensorValidation(UNUSED_PARA FILE *pRXDataFile, FILE *pTXDataFile)
{
	//Write responce data
	WriteWebResponse(pTXDataFile, "0");
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	WriteJSONNumericData(pTXDataFile, stqcValidationDetail[STQC_STATUS_API], SfprSensorSTQCApprovedF);
	WriteJSONENDTag(pTXDataFile);	//JSON Data END

	return SUCCESS;
}


//*****************************************************************************
//	GetExtReaderType()
//	Param:
//		IN:
//		OUT:
//	Returns:
//		NONE
//	Description: It will get the connected reader type (CER/CB).
//
//*****************************************************************************
UINT8 GetExtReaderType(UNUSED_PARA FILE *pRXDataFile,FILE *pTXDataFile)
{
	READER_TYPE_e	rdrType = NO_READER_PRESENT;


//	if((GetReaderStatus(READER3) == READER_STATUS_UNKNOWN)||(GetReaderStatus(READER3) == READER_IS_ACTIVE))
	{
		if(((ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_CB_U] == TRUE)||(ReaderAutoDetectInfo.comboBlDetctedF[COMBO_READER_CB_U] == TRUE)) &&
						(SysBasicPara.reader[READER4].type != SUP_FP_3030_READER))
		{
			rdrType = CB_U_READER;
		}
		else if(((ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_CER] == TRUE)||(ReaderAutoDetectInfo.comboBlDetctedF[COMBO_READER_CER] == TRUE)) &&
				(SysBasicPara.reader[READER4].type != CB_U_READER))
		{
			rdrType = COMBO_EXIT_READER;
		}
		else if(((ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_ATOM_RD100] == TRUE)||(ReaderAutoDetectInfo.comboBlDetctedF[COMBO_READER_ATOM_RD100] == TRUE)) &&
				(SysBasicPara.reader[READER4].type == ATOM_RD100_READER))
		{
			rdrType = ATOM_RD100_READER;
		}
		else if((ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_ATOM_RD200] == TRUE) && (SysBasicPara.reader[READER4].type == ATOM_RD200_READER))
		{
			rdrType = ATOM_RD200_READER;
		}
		else if((ReaderAutoDetectInfo.comboCardDetectF[COMBO_READER_ATOM_RD300] == TRUE) && (SysBasicPara.reader[READER4].type == ATOM_RD300_READER))
		{
			rdrType = ATOM_RD300_READER;
		}
	}

	WriteWebResponse(pTXDataFile, "0");
	WriteJSONStartTag(pTXDataFile);	//JSON Data Start
	WriteJSONNumericData(pTXDataFile,extReaderType[EXT_RDR_TYPE], rdrType);
	WriteJSONNumericData(pTXDataFile,extReaderType[EXT_RDR_HW_TYPE], GetAtomReaderHwType());
	WriteJSONStringData(pTXDataFile,extReaderType[EXT_RDR_FW_VER_DG_CTRL], GetAtomFwDetail());
	WriteJSONENDTag(pTXDataFile);	//JSON Data END

	return SUCCESS;
}
