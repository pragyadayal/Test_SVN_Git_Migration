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
//   Created By   : Ronak Tanna
//   File         : MxSysUpgrade.h
//   Description  : This file declares system utility API.
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>

#include "MxTypedef.h"	
#include "MxSysCommonDef.h"
#include "MxLogs.h"
#include "MxSysUpgrade.h"
#include "MxSysTimer.h"
#include "MxEventLog.h"
#include "MxSysUtils.h"
#include "MxAvResp.h"
#include "MxGuiHandler.h"
#include "MxConfig.h"
#include "MxConfigFileIo.h"
#include "MxSysCommonUtils.h"
#include "MxBlockDevice.h"
#include "MxWebJeeves.h"
#include "MxWifiPort.h"
#include "MxMobileBroadband.h"
#include "MxCommTask.h"
#include "MxDcTask.h"
#include "MxWebJvsAutoSystemTest.h"
#ifdef OEM_SUPPORTED
#include "OEM.h"
#endif //#ifdef OEM_SUPPORTED
#include "MxDisplayMessage.h"

//******** Extern Variables **********
extern BOOL					DemoModeEnable;
extern BOOL  				SamayModeEnable;
extern struct tm 			CurrTimeDate;
extern MMC_INFO_t 			MmcInfo;
extern BOOL					WifiApParaChngF;
extern UINT8				SelectedModemProfile;
extern MODEM_CONFIG_t		ModemProfile[MAX_MODEM_PROFILE];
extern pthread_t		fwUpgradeFromPanelThreadData;
extern THREAD_STATUS_e	fwUpgradeFromPanelThreadStatus;
extern CHAR 				BenchmarkFWzip[BENCHMARK_FW_STR_LEN];
extern BOOL					FirmwareUpgradeTriger;
extern DOOR_ACTIVITY_INFO_t DcInfo;

//******** Defines and Data Types ****
#define FW_UPGRADE_STAT_FILE_NAME		"UpgradeStatus.txt"
#define VER_SIGNATURE_FILE				"argo_verSign_"

#define SDK_BIN_FOLDER					"AM335X_CosecARGO"
#define APPL_DATA_BACKUP_FOLDER			"backupApplData"
#define MAX_REQUIRED_CHECKSUM_COUNT		8          // 8 file's checksum should be verified 

/* System can't downgrade to version revision V1R2 or lower as V1R3 is fist production release -
* (If present then omit only the leading zero otherwise it will take this number as octal number ) */
#define DOWNGRADE_CONTROL_VER			102
/* System can't downgrade to version revision V1R5 or lower due to Demo Software */
#define DEMO_DOWNGRADE_CONTROL_VER		0000

#ifdef OEM_SUPPORTED
// System can't downgrade to version revision V1R17 or lower due to OEM change
#define OEM_DOWNGRADE_CONTROL_VER		"0117"
#endif //#ifdef OEM_SUPPORTED

// System can't downgrade to version revision V1R13 or lower due to SAMAY Software
#define SAMAY_DOWNGRADE_CONTROL_VER		113

/* Defines firmware version to indicate need of new library.
 * So if below define is 147 then From V01R47 we change certain library so if firmware older than v01R47
 * found than change soft links to older library at boot time */
#define NEW_LIB_SOFT_LINK_FW_VER		119

/* Define for name of file in which ver rev and minor revision are read from firmware zip file */
#define VER_REV_FILE_NAME				"/tmp/verrev.txt"
#define BENCHMARK_FOLDER				"/tmp/benchmark"
/* Define for length of first line in VER_REV_FILE_NAME file which contains ver, rev and minor ver */
#define VER_REV_STR_LEN_MAX				15
/* Define for wait time in seconds for door firmware upgrade through panel */
#define FW_VALIDATE_UPGRADE_TIMER		300

// System can't downgrade to version revision V1R19("VER""REV") or lower due for CAE/CAM/CAI (without FP reader) variants
#define CAE_CAM_CAI_DOWNGRADE_CONTROL_VER	"0119"

//******** Function Prototypes *******

//******** Global Variables **********
CHAR 	BenchmarkFWzip[BENCHMARK_FW_STR_LEN] = "";
struct tm LastUpgradeTime;
BOOL	BenchmarkF = FALSE;
//******** Static Variables **********
static INT32 				upgradeFd;
static FIRMWARE_PKT_INFO_t 	upgradePktInfo;
static TIMER_HANDLE			fwUpgradeFromPanelTmrHandle = INVALID_TIMER_HANDLE;
static const OLD_FW_INFO_t oldFirmwareInfo[19] = {
    {"COSEC_ARGO_DOOR_V01R01.zip",	"fc9b08cbd3b1485651558f35dc83c9d9"},
    {"COSEC_ARGO_DOOR_V01R02.zip",	"4bc3b5f9931b09abfb5db598fda157d8"},
    {"COSEC_ARGO_DOOR_V01R03.zip",	"249d189e31a69ddbe12fe14ed23ce6c7"},
    {"COSEC_ARGO_DOOR_V01R04.zip",	"8532447b27800e1206b290f99e1b95bb"},
    {"COSEC_ARGO_DOOR_V01R05.zip",	"0388ff369cc0f5da5a3d775a10b0ec7c"},
    {"COSEC_ARGO_DOOR_V01R06.zip",	"828d3d41cd637d3a32b36fcdfc1e514f"},
    {"COSEC_ARGO_DOOR_V01R07.zip",	"317e5d873bfea622d2a79ffc0dc7e9f8"},
    {"COSEC_ARGO_DOOR_V01R08.zip",	"b7cf06fc0c049e25dc7a7cbc4201e5a8"},
    {"COSEC_ARGO_DOOR_V01R09.zip",	"271b9cb95198107d6456a7445187ea39"},
    {"COSEC_ARGO_DOOR_V01R10.zip",	"f68331b5e44a9bec684e381e4c7120e9"},
    {"COSEC_ARGO_DOOR_V01R11.zip",	"b03b406c2e330e93cd91b2c8b23b9859"},
    {"COSEC_ARGO_DOOR_V01R12.zip",	"84c98e46a379a19b9a903112fc62baaa"},
    {"COSEC_ARGO_DOOR_V01R13.zip",	"ea8c67d5c5ebe3c33babd845ba216c2f"},
    {"COSEC_ARGO_DOOR_V01R14.zip",	"a879b5c3174857d910e3e2e57e393964"},
    {"COSEC_ARGO_DOOR_V01R15.zip",	"26247901ccd06d5619993684529e6523"},
    {"COSEC_ARGO_DOOR_V01R16.zip",	"20f6cac06cebef974b3d3eab07aa42b7"},
    {"COSEC_ARGO_DOOR_V01R17.zip",	"5aedbd98f37d1840162550e1fe43c3ec"},
    {"COSEC_ARGO_DOOR_V01R18.zip",	"51af5b962603c7faa0d7a1efe1509e4b"},
    {"COSEC_ARGO_DOOR_V01R19.zip",	"e3d49b97b6830b5c0996e1e322954478"},
};
//********Static Function******
static CHECK_BM_FW_RESULT CheckBenchmarkFW(CHARPTR decryptedFile, CHARPTR verRevStr);
//******** Function Definations ******

//*****************************************************************************
//	StartSystemFwUpgrade
//	Param: 
//		IN :	File Name
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//	Description:
//			It will upgrade system firmware.
//*****************************************************************************
FIRMWARE_UPGRADE_STATUS_e StartSystemFwUpgrade(CHARPTR fileName, BENCHMARK_UPGRADE_TYPE_e benchmarkUpgradeType)
{
	CHAR						cmdBuf[FIRMWARE_FILENAME_LEN_MAX], parseStrFmt[VER_REV_STR_LEN_MAX];
	CHAR						firstLine[VER_REV_STR_LEN_MAX];
	CHAR						encryptedFile[FIRMWARE_FILENAME_LEN_MAX];
	CHAR						decryptedFile[FIRMWARE_FILENAME_LEN_MAX];
	SW_BACKUP_UPDATE_FIELD3_e	failReason = BU_EVT_FAIL;
	FIRMWARE_UPGRADE_STATUS_e	fwStatus = FIRMWARE_UPGRADE_FAIL;
	struct stat 				st;
	FILE						*fp = NULL;
	UINT32						ver = 0, rev = 0, minorVer = 0, verRev = 0;
	EnableSystemReboot(FALSE);
	#ifdef OEM_SUPPORTED
	BOOL tIsDeleteOEMcode = FALSE;
	#endif //#ifdef OEM_SUPPORTED
	
	DPRINT(SYS_LOG, "Upgrading Firmware...");

	/* Control will return from below do while loop upon successful validation of firmware file */
	do
	{
		/* First check if dir where zip will be decrypted is present */
		snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX,"%s", DECRYPTED_ZIP_PATH);
		if(stat(cmdBuf, &st) < 0)
		{
			/* Create dir if it does not exists */
			snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX,"mkdir -p %s", DECRYPTED_ZIP_PATH);
			if (ExeCmd(cmdBuf) == FALSE)
			{
				/* Failed to create directory */
				failReason = BU_EVT_FAIL;
				fwStatus = FIRMWARE_UPGRADE_FAIL;
				snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "rm -f %s%s", ZIP_PATH, fileName);
				ExeCmd(cmdBuf);
				EnableSystemReboot(TRUE);
				WriteEventLog(SW_BACKUP_UPDATE, SW_UPDATE, SW_FIRMWARE, failReason, UNUSED_FIELD, UNUSED_FIELD);
				BenchmarkF = FALSE;
				return fwStatus;
			}
		}

		/* Prepare absolute path for encrypted and decrypted firmware zip files */
		snprintf(encryptedFile, FIRMWARE_FILENAME_LEN_MAX, "%s%s", ZIP_PATH, fileName);
		snprintf(decryptedFile, FIRMWARE_FILENAME_LEN_MAX, "%s/%s", DECRYPTED_ZIP_PATH, fileName);

		/* Check whether encrypted firmware zip file is present or not */
		if (access(encryptedFile, F_OK | R_OK) != 0)
		{
			/* Encrypted firmware zip file is not present */
			failReason = BU_EVT_FAIL;
			fwStatus = FIRMWARE_UPGRADE_FAIL;
			EPRINT(SYS_LOG, "Firmware file [%s] not present", encryptedFile);
			BenchmarkF = FALSE;
			break;
		}

		/* Checking integrity of firmware zip file. If it found ok then received
		 * firmware zip is non encrypted so it might be for downgrade else we will
		 * try to decrypt the received firmware zip */
		snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "zip -T %s", encryptedFile);
		if (ExeCmd(cmdBuf) == TRUE)
		{
			BOOL md5sumMatchF = FALSE;
			UINT8 index;
			for(index = 0;index < 19;index++)
			{
				if(strcmp(oldFirmwareInfo[index].firmwareName,fileName) == 0)
				{
				   CHAR	filePath[FIRMWARE_FILENAME_LEN_MAX];
				   snprintf(filePath, FIRMWARE_FILENAME_LEN_MAX, "%s%s", ZIP_PATH, fileName);
				   if(CheckMd5sumForOldFw(filePath,oldFirmwareInfo[index].firmwareMd5sum) == SUCCESS)
				   {
				       EPRINT(SYS_LOG, "Firmware file Md5sum Match");
				       md5sumMatchF = TRUE;
				   }
				   break;
				}
			}
			if(md5sumMatchF == FALSE)
			{
				EPRINT(SYS_LOG, "Failed to check integrity of firmware file");
				failReason = BU_EVT_CRC_ERROR;
				fwStatus = FIRMWARE_UPGRADE_CRC_FAIL;
				BenchmarkF = FALSE;
				break;
			}

			// Downgrade control because of file system change
			snprintf(cmdBuf, sizeof(cmdBuf), "if [ $(unzip -l %s%s | awk -F\" \" '{print $4}' | grep %s | awk '{split($0,a,\"/\");print a[2]}' | awk '{split($0,a,\"_\");print a[3]}') -gt %d ]; then exit 0; else exit 1; fi",
					ZIP_PATH, fileName, VER_SIGNATURE_FILE, DOWNGRADE_CONTROL_VER);
			if(ExeCmd(cmdBuf) == FALSE)
			{
				failReason = BU_EVT_FAIL;
				fwStatus = FIRMWARE_UPGRADE_DOWNGRADE_CNTRL;
				EPRINT(SYS_LOG, "Firmware can not be downgrade to version revision[%d] or lower version", DOWNGRADE_CONTROL_VER);
				break;
			}

			// Downgrade control for new Variants CAE/CAM/CAI because 1st release
			if( (ARGO_VARIANT_CAE200 == GetAutoTestParaDevType())
					|| (ARGO_VARIANT_CAM200 == GetAutoTestParaDevType())
					|| (ARGO_VARIANT_CAI200 == GetAutoTestParaDevType())
			)
			{
				sprintf(cmdBuf,"if [ $(unzip -l %s%s | awk -F\" \" '{print $4}' | grep %s | awk '{split($0,a,\"/\");print a[2]}' | awk '{split($0,a,\"_\");print a[3]}') -gt %s ]; then exit 0; else exit 1; fi",
						ZIP_PATH, fileName, VER_SIGNATURE_FILE, CAE_CAM_CAI_DOWNGRADE_CONTROL_VER);
				if(ExeCmd(cmdBuf) == FALSE)
				{
					failReason = BU_EVT_FAIL;
					fwStatus = FIRMWARE_UPGRADE_DOWNGRADE_CNTRL;
					EPRINT(SYS_LOG, "ARGO CAE/CAM/CAI/Unknown: Firmware can not be downgraded beyond 1st release <= [%s]", CAE_CAM_CAI_DOWNGRADE_CONTROL_VER);
					break;
				}
			}

			#ifdef OEM_SUPPORTED
				//Downgrade Control Because of OEM change
				cmdBuf[0] = '\0';
				snprintf(cmdBuf, sizeof(cmdBuf), "if [ $(unzip -l %s%s | awk -F\" \" '{print $4}' | grep %s | awk '{split($0,a,\"/\");print a[2]}' | awk '{split($0,a,\"_\");print a[3]}') -gt %s ]; then exit 0; else exit 1; fi",
				ZIP_PATH, fileName, VER_SIGNATURE_FILE, OEM_DOWNGRADE_CONTROL_VER);
				if(system(cmdBuf) != 0)
				{
					if(TRUE == OEM_IsCustomApplied())
					{
						sprintf(cmdBuf, "rm -rf %s%s", ZIP_PATH, fileName);
						system(cmdBuf);
						failReason = BU_EVT_FAIL;
						fwStatus = FIRMWARE_UPGRADE_DOWNGRADE_CNTRL;
						EPRINT(SYS_LOG, "Firmware can not be downgrade to version revision[%s] or lower version due to OEM Support", OEM_DOWNGRADE_CONTROL_VER);
						break;
					}
					else
					{
						tIsDeleteOEMcode = TRUE;
					}
				}
			#endif //#ifdef OEM_SUPPORTED

			// Downgrade control because of demo software
			if(DemoModeEnable == TRUE)
			{
				snprintf(cmdBuf, sizeof(cmdBuf), "if [ $(unzip -l %s%s | awk -F\" \" '{print $4}' | grep %s | awk '{split($0,a,\"/\");print a[2]}' | awk '{split($0,a,\"_\");print a[3]}') -gt %d ]; then exit 0; else exit 1; fi",
						ZIP_PATH, fileName, VER_SIGNATURE_FILE, DEMO_DOWNGRADE_CONTROL_VER);
				if(ExeCmd(cmdBuf) == FALSE)
				{
					failReason = BU_EVT_FAIL;
					fwStatus = FIRMWARE_UPGRADE_DOWNGRADE_CNTRL;
					EPRINT(SYS_LOG, "Demo Software, Firmware can not be downgrade to vresion revision[%d] or lower version", DEMO_DOWNGRADE_CONTROL_VER);
					break;
				}
			}

			if(SamayModeEnable == TRUE)
			{
				snprintf(cmdBuf, sizeof(cmdBuf), "if [ $(unzip -l %s%s | awk -F\" \" '{print $4}' | grep %s | awk '{split($0,a,\"/\");print a[2]}' | awk '{split($0,a,\"_\");print a[3]}') -gt %d ]; then exit 0; else exit 1; fi",
						ZIP_PATH, fileName, VER_SIGNATURE_FILE, SAMAY_DOWNGRADE_CONTROL_VER);
				if(ExeCmd(cmdBuf) == FALSE)
				{
					failReason = BU_EVT_FAIL;
					fwStatus = FIRMWARE_UPGRADE_DOWNGRADE_CNTRL;
					EPRINT(SYS_LOG, "Samay Software, Firmware can not be downgrade to vresion revision[%d] or lower version", SAMAY_DOWNGRADE_CONTROL_VER);
					break;
				}
			}

			snprintf(cmdBuf, sizeof(cmdBuf), "if [ $(unzip -l %s%s | awk '{print $4}' | grep %s | awk -F \"_\" '{print $3}') -lt %d ]; then exit 0; else exit 1; fi",
								ZIP_PATH, fileName, VER_SIGNATURE_FILE, NEW_LIB_SOFT_LINK_FW_VER);
			if(ExeCmd(cmdBuf) == TRUE)
			{
				/* Make a file that indicates soft links need to be made for old lib because this is downgrading the firmware */
				snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "touch /%s", CHANGE_LIB_SOFT_LINK_SIGN_FILE);
				if (ExeCmd(cmdBuf) == FALSE)
				{
					EPRINT(SYS_LOG, "Failed to create indication for unencrypted firmware file");
					failReason = BU_EVT_FAIL;
					fwStatus = FIRMWARE_UPGRADE_FAIL;
					break;
				}
				else
				{
					DPRINT(SYS_LOG, "Created %s because need to change soft links on reboot", CHANGE_LIB_SOFT_LINK_SIGN_FILE);
				}
			}
		}
		else
		{
			/* Decrypt the zip file */
			if (FirmwareDecrypt(encryptedFile, decryptedFile, FIRMWARE_ENCRYPTION_KEY) == -1)
			{
				EPRINT(SYS_LOG, "Failed to decrypt firmware file %s", fileName);
				failReason = BU_EVT_CRC_ERROR;
				fwStatus = FIRMWARE_UPGRADE_CRC_FAIL;
				BenchmarkF = FALSE;
				break;
			}

			/* Checking integrity of firmware zip file. If it found not ok
			 * then we give fail response and exit from firmware upgrade */
			snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "zip -T %s", decryptedFile);
			if (ExeCmd(cmdBuf) == FALSE)
			{
				EPRINT(SYS_LOG, "Failed to check integrity of firmware file");
				failReason = BU_EVT_CRC_ERROR;
				fwStatus = FIRMWARE_UPGRADE_CRC_FAIL;
				BenchmarkF = FALSE;
				break;
			}

			/* Prepare command that lists content of a zip file; finds string that represents ver, rev and minor ver
			 * from name of verSign file e.g. 01_05_02 and prints that in a file */
			snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "unzip -l %s | grep %s | awk -F\"verSign_\" '{print $NF}' > %s",
					decryptedFile, VER_SIGNATURE_FILE, VER_REV_FILE_NAME);
			if (ExeCmd(cmdBuf) == FALSE)
			{
				EPRINT(SYS_LOG, "Failed to parse ver rev from [%s]", VER_SIGNATURE_FILE);
				failReason = BU_EVT_FAIL;
				fwStatus = FIRMWARE_UPGRADE_FAIL;
				BenchmarkF = FALSE;
				break;
			}

			/* Check if file has any content? */
			stat(VER_REV_FILE_NAME , &st);
			if (st.st_size <= 0)
			{
				EPRINT(BU_LOG, "Nothing parsed in ver rev file [%s]", VER_REV_FILE_NAME);
				failReason = BU_EVT_FAIL;
				fwStatus = FIRMWARE_UPGRADE_FAIL;
				BenchmarkF = FALSE;
				/* Delete the file */
				snprintf(cmdBuf,sizeof(cmdBuf),"rm %s",VER_REV_FILE_NAME);
				ExeCmd(cmdBuf);
				break;
			}

			/* Open the file */
			fp = fopen(VER_REV_FILE_NAME, "r");
			if (fp == NULL)
			{
				EPRINT(BU_LOG, "Failed to open parsed file [%s]", VER_REV_FILE_NAME);
				failReason = BU_EVT_FAIL;
				fwStatus = FIRMWARE_UPGRADE_FAIL;
				BenchmarkF = FALSE;
				/* Delete the file */
				snprintf(cmdBuf,sizeof(cmdBuf),"rm %s",VER_REV_FILE_NAME);
				ExeCmd(cmdBuf);
				break;
			}

			/* Get first line - this file contains only one line e.g 01_05_02 */
			if (fgets(firstLine, VER_REV_STR_LEN_MAX, fp) == NULL)
			{
				EPRINT(BU_LOG, "Failed to read ver rev from file [%s]", VER_REV_FILE_NAME);
				failReason = BU_EVT_FAIL;
				fwStatus = FIRMWARE_UPGRADE_FAIL;
				BenchmarkF = FALSE;
				/* Close and delete the file */
				fclose(fp);
				snprintf(cmdBuf,sizeof(cmdBuf),"rm %s",VER_REV_FILE_NAME);
				ExeCmd(cmdBuf);
				break;
			}
			/* Close and delete the file */
			fclose(fp);
			snprintf(cmdBuf,sizeof(cmdBuf),"rm %s",VER_REV_FILE_NAME);
			ExeCmd(cmdBuf);
			firstLine[8] = '\0';
			if(BenchmarkF == FALSE)
			{
				CHECK_BM_FW_RESULT checkBmFwResult = CheckBenchmarkFW(decryptedFile, firstLine);
				if (CHECK_BM_FW_NEEDED == checkBmFwResult)
				{
					if(benchmarkUpgradeType == AUTO_BM_UPGRADE)
					{
						BenchmarkF = TRUE;
						FirmwareUpgradeTriger = TRUE;
					}
					else if(benchmarkUpgradeType == FORCE_BM_UPGRADE)
					{
						CleanUpTempUserFP();
						DcInfo.curState = DC_IDLE_STATE;
						BenchmarkF = TRUE;
					}
					EPRINT(BU_LOG, "Benchmark Firmware Found..! Please update with [%s]",BenchmarkFWzip);
					failReason = BU_EVT_FAIL;
					fwStatus = FIRMWARE_UPGRADE_BENCHMARK_FOUND;
					break;
				}
				else if (CHECK_BM_FW_FAIL == checkBmFwResult)
				{
					EPRINT(BU_LOG, "Checking process for Benchmark Firmware is failed, firmware will not be upgraded");
					failReason = BU_EVT_FAIL;
					fwStatus = FIRMWARE_UPGRADE_FAIL;
					break;
				}
				else
				{
					DPRINT(BU_LOG, "Benchmark firmware not found in uploaded firmware zip file");
				}
			}
			/* Parse ver, rev and minor ver */
			snprintf(parseStrFmt, VER_REV_STR_LEN_MAX, "%%u_%%u_%%u");
			if (sscanf(firstLine, parseStrFmt, &ver, &rev, &minorVer) != 3)
			{
				BenchmarkF = FALSE;
				EPRINT(SYS_LOG, "Error in parsing ver, rev and minor rev from ver rev file");
				failReason = BU_EVT_FAIL;
				fwStatus = FIRMWARE_UPGRADE_FAIL;
				break;
			}

			DPRINT(SYS_LOG, "ver=%d rev=%d minorVer=%d\n", ver, rev, minorVer);

			/* Create integer value of version and revision so that can be directly compared with downgrade control macros */
			snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "%02u%02u", ver, rev);
			if (sscanf(cmdBuf, "%u", &verRev) != 1)
			{
				BenchmarkF = FALSE;
				EPRINT(SYS_LOG, "Error in creating integer value of version and revision");
				failReason = BU_EVT_FAIL;
				fwStatus = FIRMWARE_UPGRADE_FAIL;
				break;
			}
			/* Downgrade control because of file system change */
			if (verRev < DOWNGRADE_CONTROL_VER)
			{
				BenchmarkF = FALSE;
				EPRINT(SYS_LOG, "Firmware can not be downgrade to version revision[%u] or lower version, uploaded ver[%u]", DOWNGRADE_CONTROL_VER, verRev);
				failReason = BU_EVT_FAIL;
				fwStatus = FIRMWARE_UPGRADE_DOWNGRADE_CNTRL;
				break;
			}
			/* Downgrade control because of demo software */
			if ((DemoModeEnable == TRUE) && (DEMO_DOWNGRADE_CONTROL_VER !=0) && (verRev < DEMO_DOWNGRADE_CONTROL_VER))
			{
				BenchmarkF = FALSE;
				EPRINT(SYS_LOG, "Demo Software, Firmware can not be downgrade to version revision[%u] or lower version", DEMO_DOWNGRADE_CONTROL_VER);
				failReason = BU_EVT_FAIL;
				fwStatus = FIRMWARE_UPGRADE_DOWNGRADE_CNTRL;
				break;
			}
			/* If samay mode is enable then check downgrade control for it */
			if ((SamayModeEnable == TRUE) && (verRev < SAMAY_DOWNGRADE_CONTROL_VER))
			{
				BenchmarkF = FALSE;
				EPRINT(SYS_LOG, "Samay Software, Firmware can not be downgrade to version revision[%u] or lower version", SAMAY_DOWNGRADE_CONTROL_VER);
				failReason = BU_EVT_FAIL;
				fwStatus = FIRMWARE_UPGRADE_DOWNGRADE_CNTRL;
				break;
			}
		}

		/* Move zip file from /matrix/zip folder to root folder */
		snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "mv %s%s /%s%s", ZIP_PATH, fileName, ROOT_FS_FIRMWARE_FILE_NAME, ZIP_FILE_EXT);
		if (ExeCmd(cmdBuf) == FALSE)
		{
			BenchmarkF = FALSE;
			EPRINT(SYS_LOG, "Failed to move firmware file into root directory");
			failReason = BU_EVT_FAIL;
			fwStatus = FIRMWARE_UPGRADE_FAIL;
			break;
		}

		/* Create signature file to trigger firmware upgrade at device bootup */
		snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "touch /%s", FIRMWARE_SIGN_FILE);
		if (ExeCmd(cmdBuf) == FALSE)
		{
			BenchmarkF = FALSE;
			EPRINT(SYS_LOG, "Failed to create Firmware upgrade Signature file into root directory");
			snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "rm -rf %s%s", ROOT_FS_FIRMWARE_FILE_NAME, ZIP_FILE_EXT);
			ExeCmd(cmdBuf);
			failReason = BU_EVT_FAIL;
			fwStatus = FIRMWARE_UPGRADE_FAIL;
			break;
		}

		#ifdef OEM_SUPPORTED
		if(TRUE == tIsDeleteOEMcode)
		{
			OEM_DelEnv("OEM_ID");
			DPRINT(SYS_LOG, "OEM Code Deleted");
		}
		#endif //#ifdef OEM_SUPPORTED
			
		/* Remove encrypted and decrypted firmware files */
		snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "rm -rf %s%s %s", ZIP_PATH, fileName, DECRYPTED_ZIP_PATH);
		ExeCmd(cmdBuf);
		EnableSystemReboot(TRUE);

		DPRINT(SYS_LOG, "Firmware Transfer successfully! Rebooting...");
		SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
		return FIRMWARE_UPGRADE_SUCCESS;
	}while(1);

	/* Control should reach here if firmware can not be upgraded */
	/* Remove encrypted and decrypted firmware file */
	snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "rm -rf %s%s %s /%s", ZIP_PATH, fileName, DECRYPTED_ZIP_PATH, CHANGE_LIB_SOFT_LINK_SIGN_FILE);
	ExeCmd(cmdBuf);

	EnableSystemReboot(TRUE);
	WriteEventLog(SW_BACKUP_UPDATE, SW_UPDATE, SW_FIRMWARE, failReason, UNUSED_FIELD, UNUSED_FIELD);
	return fwStatus;
}

//*****************************************************************************
//	DisplayFwUpgradeStatusMsg()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//		NONE
//	Description:
//		This function wiil display firmware upgrade status success or fail.
//
//*****************************************************************************
void DisplayFwUpgradeStatusMsg(void)
{
	FILE		*fp;
	CHAR		buf[50], cmdBuf[255];
	BACKUP_UPDATE_TYPE_e buType = BU_TYPE_FIRMWARE;
	BOOL		pswdWebAuthUpdateReqd = FALSE;

	sprintf(cmdBuf,"rm -rf /media/mmcblk0p1/%s_* &",APPL_DATA_BACKUP_FOLDER);
	if(system(cmdBuf) < 0)
	{
		EPRINT(SYS_LOG,"%s_* is not deleted successfully", APPL_DATA_BACKUP_FOLDER);
	}
	
	sprintf(cmdBuf,"rm -rf /media/mmcblk0p1/%s_* &",SDK_BIN_FOLDER);
	if(system(cmdBuf) < 0)
	{
		EPRINT(SYS_LOG,"%s_* is not deleted successfully", SDK_BIN_FOLDER);
	}
	
	fp = fopen(FW_UPGRADE_STAT_FILE, "r");
	if(fp)
	{
		pswdWebAuthUpdateReqd = TRUE;
		if(fread(buf, 1, 1, fp) == 1)
		{
			if(strncmp(buf,"S", 1) == 0)
			{
				if(GetDoorApplType() == PANEL_DOOR_APPL)
				{
					SetAudioVisual(USER_RESP_PD_FIRMWARE_UPLOAD_SUCCESS, INTERNAL_PORT, &buType);
				}
				else
				{
					WriteEventLog(SW_BACKUP_UPDATE, SW_UPDATE, SW_FIRMWARE, BU_EVT_SUCCESS, UNUSED_FIELD, UNUSED_FIELD);
					SetAudioVisual(USER_RESP_FIRMWARE_UPLOAD_STATUS_SUCCESS, INTERNAL_PORT, &buType);
				}
				DPRINT(SYS_LOG, "Firmware upgraded SUCCESSFULLY");

				LastUpgradeTime = CurrTimeDate;

				//write date and time into file
				WriteLastUpgradeTime();

				RemoveOldWebPages();
			}
			else if(strncmp(buf,"F", 1) == 0)
			{
				if(GetDoorApplType() == PANEL_DOOR_APPL)
				{
					SetAudioVisual(USER_RESP_PD_FIRMWARE_UPLOAD_FAIL, INTERNAL_PORT, &buType);
				}
				else
				{
					WriteEventLog(SW_BACKUP_UPDATE, SW_UPDATE, SW_FIRMWARE, BU_EVT_FAIL, UNUSED_FIELD, UNUSED_FIELD);
					SetAudioVisual(USER_RESP_FIRMWARE_UPLOAD_STATUS_FAIL, INTERNAL_PORT, &buType);
				}
				DPRINT(SYS_LOG, "Firmware upgrade FAIL");
			}
			//Sleep for 3 Sec to display message on LCD
			sleep(3);
		}
		fclose(fp);
		sprintf(buf, "rm -rf %s", FW_UPGRADE_STAT_FILE);
		system(buf);
	}

	if(pswdWebAuthUpdateReqd == TRUE)
	{
		//write current password in auth.txt
		WritePasswordToWebAuthFile();
	}

	/* Delete file if present */
	if (-1 != access("/"CHANGE_LIB_SOFT_LINK_SIGN_FILE, F_OK))
	{
		if (0 != remove("/"CHANGE_LIB_SOFT_LINK_SIGN_FILE))
		{
			DPRINT(SYS_LOG, "Failed to remove /%s", CHANGE_LIB_SOFT_LINK_SIGN_FILE);
		}
	}
}

//***************************************************************************
//	WriteZipFile()
//	Param:
//		IN :	pktNo : packet no.
//				pktDataPtr : Packet Data Pointer
//		OUT:	SUCCESS or FAIL
//	Returns:	NONE
//	Description:
//				This function write the packet data on its location based
//				on packet no.
//***************************************************************************
BOOL WriteZipFile(UINT32 pktNo, CHARPTR pktDataPtr)
{
	UINT8 						percentageCnt = 0;
	BACKUP_UPDATE_PROGRESS_QT_t buData;
	CHAR 						progressBuff[MAX_LANG_DISP_STR_LEN];

	if(lseek(upgradeFd, (pktNo * upgradePktInfo.pktSize), SEEK_SET) != (INT32)(pktNo * upgradePktInfo.pktSize))
	{
		return FAIL;
	}
	else
	{
		if(pktNo < upgradePktInfo.noOfPkt)
		{
			if(write(upgradeFd, (VOIDPTR)pktDataPtr, upgradePktInfo.pktSize ) != (INT32)(upgradePktInfo.pktSize) )
			{
				return FAIL;
			}
		}
		else if (pktNo == upgradePktInfo.noOfPkt)
		{
			if((upgradePktInfo.applSize % upgradePktInfo.pktSize) != 0)
			{
				if(write(upgradeFd, (VOIDPTR)pktDataPtr, (upgradePktInfo.applSize % upgradePktInfo.pktSize) ) != (INT32)(upgradePktInfo.applSize % upgradePktInfo.pktSize) )
				{
					return FAIL;
				}
			}
		}
		else
		{
			return FAIL;
		}
	}
	if(percentageCnt != (((pktNo + 1) * 100)/upgradePktInfo.noOfPkt))
	{
		percentageCnt = ((pktNo + 1) * 100)/upgradePktInfo.noOfPkt;
		snprintf(progressBuff,sizeof(progressBuff),"%s",GetDisplayMessage(FALSE,NULL_CHAR,MSG_FIRMWARE_TRANSFERING_1027));
		strncpy(buData.splFunc,progressBuff,sizeof(buData.splFunc));
		buData.prog = percentageCnt;
		strcpy(buData.genInfo.line1,"");
		SendMessageToQt(&buData,GUI_MSG_BACKUP_UPDATE_PROG);
	}
	
	DPRINT(BU_LOG, "Write packet[%d] successfully....!! Upgrading : %d%%",pktNo,((pktNo * 100)/upgradePktInfo.noOfPkt));
	return SUCCESS;
}

//***************************************************************************
//	CloseDownloadZip()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function closes the file.
//***************************************************************************
void CloseDownloadZip(void)
{
	if(upgradeFd != -1)
	{
		close(upgradeFd);
		upgradeFd = -1;
	}
}

//***************************************************************************
//	StartPDFirmwareUpgrade()
//	Param:
//		IN :	FIRMWARE_PKT_INFO_t
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function sets the basic firmware upgrade parameters and create
//				.zip file for firmware
//***************************************************************************
void StartPDFirmwareUpgrade(FIRMWARE_PKT_INFO_t* pktInfo)
{
	CHAR sysBuf[100];
	CHAR fileBuf[100];

	memcpy(&upgradePktInfo, pktInfo, sizeof(FIRMWARE_PKT_INFO_t) );

	// Remove file if already exists
	sprintf(sysBuf, "rm -rf %s%s%s", ZIP_PATH, DIRECT_DOORFIRMWARE_FILE, ZIP_FILE_EXT);
	system(sysBuf);

	sprintf(fileBuf, "%s%s%s", ZIP_PATH, DIRECT_DOORFIRMWARE_FILE, ZIP_FILE_EXT);
	upgradeFd = open(fileBuf, O_WRONLY | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(upgradeFd < 0)
	{
		EPRINT(BU_LOG,"Error in creating Firmware Zip File");
	}
}

//*****************************************************************************
//	StartSystemSdkUpgrade
//	Param: 
//		IN :	File Name
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//	Description:
//			It will upgrade system firmware.
//*****************************************************************************
FIRMWARE_UPGRADE_STATUS_e StartSystemSdkUpgrade(CHARPTR fileStr)
{
	CHAR 						cmdBuf[255],fileName[255],buff[1024],tempBuf[255];
	UINT8 						checkSumCount = 0, idx=0;
	SW_BACKUP_UPDATE_FIELD3_e	failReason = BU_EVT_FAIL;
	FIRMWARE_UPGRADE_STATUS_e	fwStatus = FIRMWARE_UPGRADE_FAIL;
	INT32						fs, fd;
	UINT32						fileCount = 0,len;
	DIR							*fpDir, *fpDir1;
	struct dirent 				*ent, *ent1;
	struct dirent 				dirBuf, dirBuf1;

	EnableSystemReboot(FALSE);
	
	//Remove the device signature & user auth files.
	//In first version of SDK upgrade(V01R21) they were copied instead of moved
	//so delete them here
	unlink("/media/mmcblk0p1/"DEMO_STATUS_FILE);
	unlink("/media/mmcblk0p1/"SAMAY_STATUS_FILE);
	unlink("/media/mmcblk0p1/"DOOR_APPL_TYPE_SIGNATURE_FILE);
	unlink("/media/mmcblk0p1/"AUTH_TXT_FILE);

	//ZIP test
	sprintf(cmdBuf, "zip -T %s%s", TEMP_DIR, fileStr);
	if(system(cmdBuf) != 0)
	{
		EPRINT(BU_LOG, "Failed to check integrity of SDK firmware file");
		sprintf(cmdBuf, "rm -rf %s%s", TEMP_DIR, fileStr);
		system(cmdBuf);
		failReason = BU_EVT_CRC_ERROR;
		fwStatus = FIRMWARE_UPGRADE_CRC_FAIL;
		goto kernelFSUpgradeFail;
	}
	DPRINT(SYS_LOG, "Zip Test Successfull");
	
	sprintf(cmdBuf, "umount %s",MmcInfo.devPath);
	system(cmdBuf);
	
	//																	action		path		status		basenode
	sprintf(cmdBuf, "/matrix/appl/evtDetect \"STAT_DETECT\" \"block\" \"remove\" \"mmcblk0p1\" \"SUCCESS\" \"\" &");
	system(cmdBuf);
	for(idx = 0; MmcInfo.status != MMC_DISCONNECTED && idx < 30; idx++)
	{
		//Wait for some to receive mmc remove event in network manager from main loop and set status accordingly
		sleep(1);
	}
	
	sprintf(cmdBuf, "mkdir -p %s",MmcInfo.devPath);
	system(cmdBuf);
	
	sprintf(cmdBuf, "mount -t vfat -o async /dev/mmcblk0p1 %s",MmcInfo.devPath);
	system(cmdBuf);
	
	//																	action		path		status		basenode
	sprintf(cmdBuf, "/matrix/appl/evtDetect \"STAT_DETECT\" \"block\" \"add\" \"mmcblk0p1\" \"SUCCESS\" \"\" &");
	system(cmdBuf);
	for(idx = 0; MmcInfo.status != MMC_CONNECTED && idx < 30; idx++)
	{
		//Wait for some to receive mmc add event in network manager from main loop and set status accordingly
		sleep(1);
	}
	
	if(MmcInfo.status != MMC_CONNECTED)
	{
		EPRINT(BU_LOG, "Failed to mount Memory Card");
		sprintf(cmdBuf, "rm -rf %s%s", TEMP_DIR, fileStr);
		system(cmdBuf);
		goto kernelFSUpgradeFail;
	}

	DPRINT(BU_LOG,"unmount and mount process of memory card done successful");

	// if ZIP test is successful then unzip SDK zip into memory card
	sprintf(cmdBuf, "unzip -o -q %s%s -d %s/", TEMP_DIR,fileStr , MmcInfo.devPath);
	DPRINT(SYS_LOG, "Started unzipping process for [%s]", fileStr);
	if(system(cmdBuf) < 0)
	{
		EPRINT(BU_LOG,"SDK unzip failure");
		sprintf(cmdBuf, "rm -rf %s%s", TEMP_DIR, fileStr);
		system(cmdBuf);
		goto kernelFSUpgradeFail;
	}
	DPRINT(BU_LOG, "SDK unzip Successfull");
	
	sprintf(cmdBuf,"%s/%s", MmcInfo.devPath, SDK_BIN_FOLDER);
	fpDir = opendir(cmdBuf);
	if(fpDir != NULL)
	{
		while( (readdir_r(fpDir, &dirBuf, &ent) == 0) && (ent != NULL) )
		{
			if(ent->d_name[0] != '.')
			{
				if( (strstr(ent->d_name, "Checksum") != 0) && (ent->d_type == DT_REG) )
				{
					//checksum file is generated with name "fimename_Checksum"
					//NULL character will be placed in place of "_"(underscore)
					snprintf(tempBuf, (strlen(ent->d_name)-(strlen("Checksum"))), "%s", ent->d_name);
					sprintf(cmdBuf, "%s/%s/%s", MmcInfo.devPath, SDK_BIN_FOLDER, tempBuf);
			
					if(verifyChecksumFile(cmdBuf) == FAIL)
						break;
			
					//Increment checksum verified file count
					checkSumCount++;
				}
			}
		}
		closedir(fpDir);
	}
	if(checkSumCount != MAX_REQUIRED_CHECKSUM_COUNT)
	{
		EPRINT(BU_LOG, "Check Sum verification failed after [%s] files",ent->d_name);
		goto kernelFSUpgradeFail;
	}
	DPRINT(BU_LOG, "Check Sum verification done successfully");
	
	fpDir = opendir(APPL_DATA_PATH);
	if(fpDir == NULL)
	{
		EPRINT(BU_LOG, "Failed to open dir[%s]", APPL_DATA_PATH);
		goto kernelFSUpgradeFail;
	}
	sprintf(cmdBuf,"mkdir %s/%s",MmcInfo.devPath, APPL_DATA_BACKUP_FOLDER);
	if(system(cmdBuf) < 0)
	{
		EPRINT(BU_LOG,"Failed to create folder [%s] in Memory card", APPL_DATA_BACKUP_FOLDER);
		goto kernelFSUpgradeFail;
	}

	while( (readdir_r(fpDir, &dirBuf, &ent) == 0) && (ent != NULL) )
	{
		if(ent->d_name[0] != '.')
		{
			if((strcmp(ent->d_name, "fpTemplate") == 0) && (ent->d_type == DT_DIR))
			{
				sprintf(cmdBuf,"%s%s",APPL_DATA_PATH,ent->d_name);	//	fpTemplate folder path string
				fpDir1 = opendir(cmdBuf);
				if(fpDir1 != NULL)
				{
					idx = 0;
					while( (readdir_r(fpDir1, &dirBuf1, &ent1) == 0) && (ent1 != NULL) )
					{
						if((ent1->d_name[0] != '.') && (ent1->d_type == DT_REG))
						{
							if( (fileCount == 10000) || (fileCount == 0) )
							{
								if(fileCount == 10000)
								{
									DPRINT(SYS_LOG, "[%d] files copied to [fpTemplate_%d] folder",fileCount,idx);
									fileCount = 0;
									idx++;
								}

								sprintf(cmdBuf,"mkdir %s/%s/fpTemplate_%d",MmcInfo.devPath, APPL_DATA_BACKUP_FOLDER, idx);
								if(system(cmdBuf) < 0)
								{
									DPRINT(BU_LOG, "Directory creation failed of fpTemplate_%d",idx);
									goto kernelFSUpgradeFail;
								}
							}

							sprintf(fileName,"%s%s/%s",APPL_DATA_PATH,ent->d_name, ent1->d_name);
							fs=open(fileName,O_RDONLY);
							if(fs < 0)
							{
								DPRINT(BU_LOG,"Can't open source file [%s]",fileName);
								goto kernelFSUpgradeFail;
							}

							sprintf(fileName,"%s/%s/fpTemplate_%d/%s", MmcInfo.devPath, APPL_DATA_BACKUP_FOLDER, idx, ent1->d_name);
                            fd=open(fileName,O_WRONLY | O_CREAT);

                            if(fd < 0)
							{
								DPRINT(BU_LOG,"Can't open destination file [%s]",fileName);
								close(fs);
								goto kernelFSUpgradeFail;
							}

							while((len=read(fs,buff,sizeof(buff))) > 0)
							{
								write(fd, buff, len);
							}
							close(fs);
							close(fd);
							fileCount ++;
						}
					}
					closedir(fpDir1);
				}
			}
			else if((strcmp(ent->d_name, "config") == 0) && (ent->d_type == DT_DIR))
			{
				DPRINT(BU_LOG,"Started backup applData for [%s] using dirent",ent->d_name);
				sprintf(cmdBuf,"mkdir %s/%s/config",MmcInfo.devPath, APPL_DATA_BACKUP_FOLDER);
				if(system(cmdBuf) < 0)
				{
					DPRINT(BU_LOG, "Directory creation failed of config");
					goto kernelFSUpgradeFail;
				}

				sprintf(cmdBuf,"%s%s",APPL_DATA_PATH,ent->d_name);	//	fpTemplate folder path string
				fpDir1 = opendir(cmdBuf);
				if(fpDir1 != NULL)
				{
					while( (readdir_r(fpDir1, &dirBuf1, &ent1) == 0) && (ent1 != NULL) )
					{
						if((ent1->d_name[0] != '.') && (ent1->d_type == DT_REG))
						{
							sprintf(fileName,"%s%s/%s",APPL_DATA_PATH,ent->d_name, ent1->d_name);
							fs=open(fileName,O_RDONLY);
							if(fs < 0)
							{
								DPRINT(BU_LOG,"Can't open config source file [%s]",fileName);
								goto kernelFSUpgradeFail;
							}

							sprintf(fileName,"%s/%s/config/%s", MmcInfo.devPath, APPL_DATA_BACKUP_FOLDER, ent1->d_name);
                            fd=open(fileName,O_WRONLY | O_CREAT);
							if(fd < 0)
							{
								DPRINT(BU_LOG,"Can't open config destination file [%s]",fileName);
								close(fs);
								goto kernelFSUpgradeFail;
							}

							while((len=read(fs,buff,sizeof(buff))) > 0)
							{
								write(fd, buff, len);
							}
							close(fs);
							close(fd);
						}
					}
					closedir(fpDir1);
				}
				DPRINT(BU_LOG,"Completed copy of [%s] using dirent",ent->d_name);
			}
			else
			{
				DPRINT(BU_LOG,"Started backup applData for [%s]",ent->d_name);
				sprintf(cmdBuf,"cp -r %s%s %s/%s/", APPL_DATA_PATH, ent->d_name, MmcInfo.devPath, APPL_DATA_BACKUP_FOLDER);
				if(system(cmdBuf) < 0)
				{
					EPRINT(BU_LOG, "copy failed of [%s] folder",ent->d_name);
					goto kernelFSUpgradeFail;
				}
				DPRINT(BU_LOG,"Completed copy of [%s]",ent->d_name);
			}
		}
	}
	closedir(fpDir);
	DPRINT(BU_LOG,"Completed backup of applData into memory card");
	
	sprintf(cmdBuf, "cp %s%s %s/",WEB_BIN_AUTH_PATH, AUTH_TXT_FILE, MmcInfo.devPath);
	if( system(cmdBuf) < 0 )
	{
		EPRINT(BU_LOG, "Failed to copy auth.txt file into Memory Card");
		goto kernelFSUpgradeFail;
	}
	
	//If device is DEMO device
	if(DemoModeEnable == TRUE)
	{
		sprintf(cmdBuf, "cp %s%s %s",USR_LIB_PATH,DEMO_STATUS_FILE, MmcInfo.devPath);
		if( system(cmdBuf) < 0 )
		{
			EPRINT(SYS_LOG, "Failed to copy Demo Mode Signature file into Memory Card");
			failReason = BU_EVT_FAIL;
			fwStatus = FIRMWARE_UPGRADE_FAIL;
			goto kernelFSUpgradeFail;
		}
	}
	
	//If device is SAMAY device
	if(SamayModeEnable == TRUE)
	{
		sprintf(cmdBuf, "cp %s%s %s",USR_LIB_PATH,SAMAY_STATUS_FILE, MmcInfo.devPath);
		if( system(cmdBuf) < 0 )
		{
			EPRINT(SYS_LOG, "Failed to copy Samay Mode Signature file into Memory Card");
			failReason = BU_EVT_FAIL;
			fwStatus = FIRMWARE_UPGRADE_FAIL;
			goto kernelFSUpgradeFail;
		}
	}
	
	if(GetDoorApplType() == PANEL_DOOR_APPL)
	{
		sprintf(cmdBuf, "cp %s %s",DOOR_APPL_TYPE_SIGNATURE_FILE, MmcInfo.devPath);
		if( system(cmdBuf) < 0 )
		{
			EPRINT(SYS_LOG, "Failed to copy Panel Door Mode Signature file into Memory Card");
			failReason = BU_EVT_FAIL;
			fwStatus = FIRMWARE_UPGRADE_FAIL;
			goto kernelFSUpgradeFail;
		}
	}
	
	sprintf(cmdBuf, "touch %s/%s && echo \"S\" >> %s/%s", MmcInfo.devPath, FW_UPGRADE_STAT_FILE_NAME, MmcInfo.devPath, FW_UPGRADE_STAT_FILE_NAME);
	if( system(cmdBuf) < 0 )
	{
		EPRINT(SYS_LOG, "Failed to create Firmware upgrade status file into Memory Card");
		failReason = BU_EVT_FAIL;
		fwStatus = FIRMWARE_UPGRADE_FAIL;
		goto kernelFSUpgradeFail;
	}

	//Write Signature file into memory card
	sprintf(cmdBuf, "touch %s/%s", MmcInfo.devPath, FIRMWARE_SIGN_FILE);
	if( system(cmdBuf) < 0 )
	{
		EPRINT(SYS_LOG, "Failed to create Firmware upgrade Signature file into Memory Card");
		failReason = BU_EVT_FAIL;
		fwStatus = FIRMWARE_UPGRADE_FAIL;
		goto kernelFSUpgradeFail;
	}

	// Sync All Data from thread itself because it takes time around 25 sec.
	system("/bin/sync");

	DPRINT(SYS_LOG,"mmc_sdk_update script running...");
	sprintf(cmdBuf, "%s/%s/%s", MmcInfo.devPath, SDK_BIN_FOLDER, "mmc_sdk_update.sh");
	if( system(cmdBuf) < 0 )
	{
		EPRINT(SYS_LOG, "Failed to run mmc sdk update");
		failReason = BU_EVT_FAIL;
		fwStatus = FIRMWARE_UPGRADE_FAIL;
		goto kernelFSUpgradeFail;		
	}
	DPRINT(SYS_LOG,"mmc_sdk_update script run successfully");
	//to ensure mac address written in memory card properly from mmc_sdk_upgrade.sh
	system("/bin/sync");
	
	EnableSystemReboot(TRUE);
	SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
	
	return FIRMWARE_UPGRADE_SUCCESS;
kernelFSUpgradeFail:
	sprintf(cmdBuf, "rm -rf %s/%s %s/%s", MmcInfo.devPath, SDK_BIN_FOLDER, MmcInfo.devPath, APPL_DATA_BACKUP_FOLDER);
	system(cmdBuf);
	WriteEventLog(SW_BACKUP_UPDATE, SW_UPDATE, SW_FIRMWARE, failReason, UNUSED_FIELD, UNUSED_FIELD);
	return fwStatus;

}

//*****************************************************************************
//	CheckBenchmarkFW
//	Param:
//		IN :	File Name
//		   :	Version revision str
//		OUT:	None
//	Returns:	Will return value based on - if checking is failed, benchmark fw
//				not needed or benchmark fw needed
//	Description:
//			It will check the benchmark firmware.
//*****************************************************************************
static CHECK_BM_FW_RESULT CheckBenchmarkFW(CHARPTR decryptedFile, CHARPTR verRevStr)
{
		CHAR benchMarkFile[BENCHMARK_FW_STR_LEN],benchMarkName[BENCHMARK_FW_STR_LEN],parseStr[BENCHMARK_FW_STR_LEN];
		UINT32 benchmarkVer,benchmarkRev,benchmarkMinorRev;
		FILE	*fp = NULL;
		CHAR 	cmdBuf[FIRMWARE_FILENAME_LEN_MAX];

		snprintf(&cmdBuf[0], FIRMWARE_FILENAME_LEN_MAX, "unzip -j %s matrix/%s%s -d %s", decryptedFile, VER_SIGNATURE_FILE, verRevStr, BENCHMARK_FOLDER);
		if (FALSE == ExeCmd(cmdBuf))
		{
			EPRINT(BU_LOG,"Failed to execute cmd [%s]", cmdBuf);
			BenchmarkFWzip[0] = '\0';
			return CHECK_BM_FW_FAIL;
		}

		snprintf(benchMarkFile, BENCHMARK_FW_STR_LEN, "%s/%s%s",BENCHMARK_FOLDER,VER_SIGNATURE_FILE, verRevStr);
		fp = fopen(benchMarkFile, "r");	//BM=V01R02.03
		if(fp != NULL)
		{
			while(fgets(benchMarkName, BENCHMARK_FW_STR_LEN, fp) != NULL)
			{
				if (strncmp(benchMarkName, "BM=", 3) == 0)
				{
					snprintf(parseStr, BENCHMARK_FW_STR_LEN, "%sV%%uR%%u.%%u","BM=");
					if (sscanf(benchMarkName, parseStr, &benchmarkVer, &benchmarkRev, &benchmarkMinorRev) == 3)
					{
						if ((benchmarkVer > SW_VER) || ((benchmarkVer == SW_VER) && (benchmarkRev > SW_REV)) || ((benchmarkVer == SW_VER) && (benchmarkRev == SW_REV) && (benchmarkMinorRev > SW_MINOR_REV)))
						{
							snprintf(BenchmarkFWzip, BENCHMARK_FW_STR_LEN, "%s_V%02uR%02u.%02u%s", DIRECT_DOORFIRMWARE_FILE, benchmarkVer, benchmarkRev, benchmarkMinorRev, ZIP_FILE_EXT);
							fclose(fp);
							snprintf(&cmdBuf[0], sizeof(cmdBuf), "rm %s", benchMarkFile);
							if (FALSE == ExeCmd(cmdBuf))
							{
								EPRINT(BU_LOG,"Failed to remove file [%s]", benchMarkFile);
							}
							return CHECK_BM_FW_NEEDED;
						}
						else
							continue;
					}
				}
				else
					continue;
			}
			fclose(fp);
			snprintf(&cmdBuf[0], sizeof(cmdBuf), "rm %s", benchMarkFile);
			if (FALSE == ExeCmd(cmdBuf))
			{
				EPRINT(BU_LOG,"Failed to remove file [%s]", benchMarkFile);
			}
		}
		BenchmarkFWzip[0] = '\0';
		return CHECK_BM_FW_NOT_NEEDED;
}

//***************************************************************************
//	BackupAfterSDKUpgrade()
//	Param:
//		IN :	
//		OUT:	NONE
//	Returns:	NONE
//	Description:This will restore backup of config and FP template into Device after
//				SDK upgraded successfully
//***************************************************************************
void BackupAfterSDKUpgrade()
{
	CHAR 			cmdBuf[255], buff[1024];
	INT32			fs, fd, randnum;
	UINT32			len;
	UINT8			selProfileIdx = 0;
	DIR				*fpDir, *fpDir1;
	struct dirent 	*ent, *ent1;
	struct dirent 	dirBuf, dirBuf1;
	struct stat		tempStat;
	
	sprintf(cmdBuf, "mv %s/%s/%s /%s/", "/media/mmcblk0p1", SDK_BIN_FOLDER, "UpgradingFirmware","matrix");
	if(system(cmdBuf) < 0)
	{
		EPRINT(SYS_LOG,"Can't move \"UpgradingFirmware\" from mmc to matrix folder");
		return ;
	}
	
	sprintf(cmdBuf, "mv %s/%s %s", "/media/mmcblk0p1", FW_UPGRADE_STAT_FILE_NAME, FW_UPGRADE_STAT_FILE);
	if(system(cmdBuf) < 0)
	{
		EPRINT(SYS_LOG,"Can't move \"%s\" from mmc to matrix folder", FW_UPGRADE_STAT_FILE_NAME);
		return ;
	}

	sprintf(cmdBuf, "/media/mmcblk0p1/%s", APPL_DATA_BACKUP_FOLDER);
	fpDir = opendir(cmdBuf);
	if(fpDir != NULL)
	{
		while( (readdir_r(fpDir, &dirBuf, &ent) == 0) && (ent != NULL) )
		{
			if(ent->d_name[0] != '.')
			{
				if( (strstr(ent->d_name, "fpTemplate_") != NULL) && (ent->d_type == DT_DIR) )
				{
					sprintf(cmdBuf,"/media/mmcblk0p1/backupApplData/%s", ent->d_name);
					fpDir1 = opendir(cmdBuf);
					if(fpDir1 != NULL)
					{
						while( (readdir_r(fpDir1, &dirBuf1, &ent1) == 0) && (ent1 != NULL) )
						{							
							if((ent1->d_name[0] != '.') && (ent1->d_type == DT_REG) )
							{
								sprintf(cmdBuf, "/media/mmcblk0p1/backupApplData/%s/%s", ent->d_name, ent1->d_name);
								fs=open(cmdBuf,O_RDONLY);
								if(fs < 0)
								{
									EPRINT(SYS_LOG,"Restoring Backup failed for [%s/%s]\n", ent->d_name, ent1->d_name);
									return ;
								}
								
								sprintf(cmdBuf, "%sfpTemplate/%s", APPL_DATA_PATH, ent1->d_name);
                                fd=open(cmdBuf, O_WRONLY | O_CREAT );
								if(fd < 0)
								{
									EPRINT(SYS_LOG,"Restoring Backup failed for [%s/%s]\n", ent->d_name, ent1->d_name);
									close(fs);
									return ;
								}
								
								printf("coping [%s] file\n",ent1->d_name);
								while((len=read(fs,buff,sizeof(buff))) > 0)
								{
									write(fd, buff, len);
								}
								close(fs);
								close(fd);
							}
						}
						closedir(fpDir1);
					}
				}
				else
				{
					sprintf(cmdBuf,"cp -r /media/mmcblk0p1/backupApplData/%s %s", ent->d_name, APPL_DATA_PATH);
					DPRINT(SYS_LOG,"Restoring Backup from Memory card for [%s]\n", ent->d_name);
					if(system(cmdBuf) < 0)
					{
						EPRINT(SYS_LOG,"Restoring Backup failed for [%s]\n", ent->d_name);
						return;
					}
				}
			}
		}
		closedir(fpDir);
	}

	sprintf(cmdBuf, "/media/mmcblk0p1/%s",AUTH_TXT_FILE);
	if(stat(cmdBuf,&tempStat) == 0)
	{
		sprintf(cmdBuf, "mv %s/%s %s", "/media/mmcblk0p1",AUTH_TXT_FILE,WEB_BIN_AUTH_PATH);
		if( system(cmdBuf) < 0 )
		{
			EPRINT(SYS_LOG,"Failed to backup auth.txt file from Memory Card");
		}
	}

	sprintf(cmdBuf, "/media/mmcblk0p1/%s",DEMO_STATUS_FILE);
	if(stat(cmdBuf,&tempStat) == 0)
	{
		sprintf(cmdBuf, "mv %s/%s %s", "/media/mmcblk0p1",DEMO_STATUS_FILE,USR_LIB_PATH);
		if( system(cmdBuf) < 0 )
		{
			EPRINT(SYS_LOG,"Failed to backup Demo Status file from Memory Card");
		}
	}

	sprintf(cmdBuf, "/media/mmcblk0p1/%s",SAMAY_STATUS_FILE);
	if(stat(cmdBuf,&tempStat) == 0)
	{
		sprintf(cmdBuf, "mv %s/%s %s", "/media/mmcblk0p1",SAMAY_STATUS_FILE,USR_LIB_PATH);
		if( system(cmdBuf) < 0 )
		{
			EPRINT(SYS_LOG,"Failed to backup Samay Status file from Memory Card");
		}
	}

	sprintf(cmdBuf, "/media/mmcblk0p1/%s",DOOR_APPL_TYPE_SIGNATURE_FILE);
	if(stat(cmdBuf,&tempStat) == 0)
	{
		sprintf(cmdBuf, "mv %s/%s /", "/media/mmcblk0p1",DOOR_APPL_TYPE_SIGNATURE_FILE);
		if( system(cmdBuf) < 0 )
		{
			EPRINT(SYS_LOG,"Failed to backup Panel Door Signature file from Memory Card");
		}
	}

	//These folder will be deleted at power on
	randnum = rand();
	sprintf(cmdBuf,"mv /media/mmcblk0p1/%s /media/mmcblk0p1/%s_%u",APPL_DATA_BACKUP_FOLDER, APPL_DATA_BACKUP_FOLDER, randnum);
	if(system(cmdBuf) < 0)
	{
		EPRINT(SYS_LOG,"%s_%u is not created successfully", APPL_DATA_BACKUP_FOLDER, randnum);
	}

	randnum = rand();
	sprintf(cmdBuf,"mv /media/mmcblk0p1/%s /media/mmcblk0p1/%s_%u",SDK_BIN_FOLDER, SDK_BIN_FOLDER, randnum);
	if(system(cmdBuf) < 0)
	{
		EPRINT(SYS_LOG,"%s_%u is not created successfully", SDK_BIN_FOLDER, randnum);
	}

	//remove Signature.txt after complete backup of config,log,event and fpTemplate
	sprintf(cmdBuf,"rm %s/%s", "/media/mmcblk0p1",FIRMWARE_SIGN_FILE);
	if(system(cmdBuf) < 0)
	{
		EPRINT(SYS_LOG,"Signature.txt is not removed successfully");
	}
	printf("Signature.txt is removed successfully from memory card\n");
	
	InitWifiParaConfig();
	WifiApParaChngF = TRUE;
	StartWifiConfFileWriteThread();
	
	InitModemParaConfig();
	for(selProfileIdx = 0; selProfileIdx < MAX_MODEM_PROFILE; selProfileIdx++)
	{
		if(ModemProfile[selProfileIdx].enable == TRUE)
		{
			SelectedModemProfile = selProfileIdx;
			MakeModemConfigFile();
			break;
		}
	}
	return;
}

//*****************************************************************************
//	FirmwareUpgradeFromPanelThread
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//			This function will start processing firmwares received from panel,
//			validates it, and gives appropriate response to panel.
//****************************************************************************
VOIDPTR FirmwareUpgradeFromPanelThread(UNUSED_PARA VOIDPTR arg)
{
	CHAR 						fileName[255];
	UPGRADE_RESPONSE_DATA_t 	response;
	FIRMWARE_UPGRADE_STATUS_e	updateStatus;
	BACKUP_UPDATE_TYPE_e		buType = BU_TYPE_FIRMWARE;

	/* Start firmware upgrade timer */
	SetFwUpgradeFromPanelTimer();

	sprintf(fileName, "%s%s", DIRECT_DOORFIRMWARE_FILE, ZIP_FILE_EXT);

	updateStatus = StartSystemFwUpgrade(fileName, ONLY_BM_CHECK);
	if(updateStatus != FIRMWARE_UPGRADE_SUCCESS)
	{
		if(updateStatus == FIRMWARE_UPGRADE_CRC_FAIL)
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
		response.status = FAIL;
	}
	else
	{
		DPRINT(BU_LOG, "CMD_DOWNLOAD_OVER SUCCESS");
		SetAudioVisual(USER_RESP_BACKUP_UPDATE_XFER_SUCCESS, INTERNAL_PORT, &buType);
		response.status = SUCCESS;
	}
	/* Delete firmware upgrade timer */
	DeleteTimer(&fwUpgradeFromPanelTmrHandle);
	SendMsg(MSG_DOWNLOAD_OVER, &response, sizeof(UPGRADE_RESPONSE_DATA_t));
	fwUpgradeFromPanelThreadStatus = THREAD_DONE;
	pthread_exit(NULL);
}

//*****************************************************************************
//	SetFwUpgradeFromPanelTimer
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function starts timer to limit max execution time
//			for firmware upgrade from panel thread
//
//*****************************************************************************
BOOL SetFwUpgradeFromPanelTimer(void)
{
    if(fwUpgradeFromPanelTmrHandle == INVALID_TIMER_HANDLE)
    {
        TIMER_INFO_t tInfo;

        tInfo.count = (FW_VALIDATE_UPGRADE_TIMER * 10 * TIMER_100MS);
        tInfo.data = 0;
        tInfo.funcPtr = &FwUpgradeFromPanelTimeout;
        return (StartTimer(tInfo, &fwUpgradeFromPanelTmrHandle, __FUNCTION__));
    }
    else //reload timer
    {
        return (ReloadTimer(fwUpgradeFromPanelTmrHandle, (FW_VALIDATE_UPGRADE_TIMER * 10 * TIMER_100MS)));
    }
}

//*****************************************************************************
//	FwUpgradeFromPanelTimeout
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			This function handles timeout for firmware validate and upgrade from
//			panel thread
//
//*****************************************************************************
void FwUpgradeFromPanelTimeout(UNUSED_PARA UINT32 dummy)
{
	INT8 retCode;

	/* Delete firmware upgrade timer */
	DeleteTimer(&fwUpgradeFromPanelTmrHandle);

	/* Kill the firmware upgrade from panel thread */
	if ((retCode = pthread_kill(fwUpgradeFromPanelThreadData, SIGKILL)) != 0)
	{
		EPRINT(SYS_LOG, "Failed to send Signal to Firmware Upgrade From Panel Thread Error [%s]", strerror(retCode));
	}
	else
	{
		DPRINT(SYS_LOG, "Signal sent to Firmware Upgrade From Panel Thread due to timeout");
	}
}

//*****************************************************************************
//	SigHandlerForFirmwareUpgradeFromPanelThread
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//			This function will be called when timeout occurs for firmware upgrade
//			from panel thread, it will clean encrypted and decrypted file and
//			sends fail response to panel.
//****************************************************************************
void SigHandlerForFirmwareUpgradeFromPanelThread(INT32 signal)
{
	CHAR						cmdBuf[FIRMWARE_FILENAME_LEN_MAX];
	UPGRADE_RESPONSE_DATA_t 	response;

	DPRINT(SYS_LOG, "Received signal [%d]", signal);

	/* Prepare cmd to delete encrypted file */
	snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "rm -f %s%s%s", ZIP_PATH, DIRECT_DOORFIRMWARE_FILE, ZIP_FILE_EXT);
	if (ExeCmd(cmdBuf) == FALSE)
	{
		EPRINT(SYS_LOG, "Failed to execute [%s]", cmdBuf);
	}
	/* Prepare cmd to delete decrypted file */
	snprintf(cmdBuf, FIRMWARE_FILENAME_LEN_MAX, "rm -rf %s", DECRYPTED_ZIP_PATH);
	if (ExeCmd(cmdBuf) == FALSE)
	{
		EPRINT(SYS_LOG, "Failed to execute [%s]", cmdBuf);
	}

	response.status = FAIL;
	SendMsg(MSG_DOWNLOAD_OVER, &response, sizeof(UPGRADE_RESPONSE_DATA_t));
	fwUpgradeFromPanelThreadStatus = THREAD_DONE;
	DPRINT(SYS_LOG, "Killing Firmware Upgrade From Panel Thread");
	pthread_exit(NULL);
}
