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
//   Created By   : Ronak Tanna
//   File         : MxOldConfigFileIo.c
//   Description  : This file consist functionality related to earlier
//					configuration table management.
/////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include <stdio.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxConfigLog.h"
#include "MxConfigOld.h"
#include "MxConfigFileIo.h"
#include "MxOldConfigFileIo.h"
#include "MxLogs.h"
#include "MxUserConfig.h"
#include "MxSysTime.h"
#include "MxMenu.h"
#include "MxAutoSwitchWifiConfig.h"
#include "MxAutoSwitchWifiConfig.h"
#include "MxConfigUpdateHandler.h"
#include "MxFileIO.h"

//******** Extern Variables **********
extern const SYS_ADVANCE_PARA_t			DfltSysAdvPara;
extern SYS_ADVANCE_PARA_t				SysAdvPara;

extern const SYS_BASIC_PARA_t	  		DfltBasicSysPara;
extern SYS_BASIC_PARA_t					SysBasicPara;

extern MODEM_CONFIG_t					ModemProfile[MAX_MODEM_PROFILE];
extern const MODEM_CONFIG_t				DfltModemProfile[MAX_MODEM_PROFILE];

extern const  USER_CONFIG_t 			DfltUserConfig;

extern SMART_ROUTE_ACCESS_CONFIG_t		SmartRouteAccessConfig[MAX_SMART_ROUTE_ACCESS];
extern const SMART_ROUTE_ACCESS_CONFIG_t DfltSmartRouteAccessConfig;

extern CAFE_MENU_CONFIG_t				CafeMenuConfig[MAX_CAFE_MENU_CODE];
extern const CAFE_MENU_CONFIG_t			DefltCafeMenuConfig;

extern const BIO_IDENTIFY_SERVER_INFO_t	DfltBioIdentifyServer;
extern BIO_IDENTIFY_SERVER_INFO_t		BioIdentifyServer;

extern V11R3_SERVER_PARA_t		  		V11R3ServerParameters;
extern const SERVER_PARA_t  			DfltV11R3ServerPara;

extern SYS_VAR_t						SysVar;
extern const SYS_VAR_t					DefltSysVar;

extern PASSWORD_t						Password;
extern PASSWORD_t 						DfltPasswordVer1;
extern PASSWORD_t 						DfltPassword;

extern PRINTER_CONFIG_t					PrinterConfig;
extern const PRINTER_CONFIG_t			DefltPrinterConfig;

extern CAFE_ITEM_CONFIG_t				CafeItemConfig[MAX_CAFE_ITEM_CODE];
extern JCM_CONFIG_t						JcmConfig[MAX_JCM_CONFIG+3]; //+3 for 'None', 'Default' and 'Continue' Job
extern const JCM_CONFIG_t				DfltJcmConfig;
extern const JCM_CONFIG_t				NoneJobConfig;
extern const JCM_CONFIG_t				DefaultJobConfig;
extern const JCM_CONFIG_t				ContinueJobConfig;

extern WEB_API_EVENTS_CONFIG_t			WebAPIEventsConfig;
extern const WEB_API_EVENTS_CONFIG_t	DfltWebAPIEventsConfig;

extern DVR_EVENT_ACTION_CONFIG_t		DvrEvtActionConfig[MAX_DVR_EVT_ACTN_CONFIG];
extern WIFI_PARA_t						WifiParameters;
extern const WIFI_PARA_t				DfltWifiPara;

extern ETHERNET_PARA_t		  			EthernetParameters;
extern const ETHERNET_PARA_t			DfltEthPara;

extern VISITOR_MANAGEMENT_t  			Visitor[MAX_VISITORS];
extern BLOCKED_USER_GROUP_t				BlockedUser[MAX_BLOCKED_USER];

extern SPECIAL_FUNCTION_t				SpecialFunctions[MAX_SPECIAL_FUNC];;
extern const SPECIAL_FUNCTION_t 		DfltSpecialFunc;

extern BACKUP_AND_UPDATE_t				BackUpAndUpdate;
extern const BACKUP_AND_UPDATE_t		DfltBackUpAndUpdateParaForCentraMode;
extern const BACKUP_AND_UPDATE_t		DfltBackUpAndUpdateParaForVyomMode;

extern SERVER_PARA_t		  			ServerParameters;
extern SERVER_PARA_t		  			DfltServerParaCentra;
extern SERVER_PARA_t		  			DfltServerParaVyom;

extern	IO_LINK_CONFIG_t				IoLinkConfig[MAX_IO_LINK];
extern	const IO_LINK_CONFIG_t			DefltIoLinkConfig;
extern CARD_FORMAT_CONFIG_t				CardFormatConf[MAX_CARD_FORMAT_PER_DOOR];
extern const CARD_FORMAT_CONFIG_t		DfltCardFormatConf;
extern const CARD_FORMAT_CONFIG_t		BlankCardFormatConf;

extern CONNECT_TO_PANEL_CONFIG_t		ConnectToPanelPara;
extern const CONNECT_TO_PANEL_CONFIG_t 	DfltConnectToPanelPara;

extern COMMAND_CTRL_CONFIG_t			CmdCtrlConfig;
extern const COMMAND_CTRL_CONFIG_t		DfltCmdCtrlConfig;

extern LICENSE_DONGLE_CONFIG_t			LicenseDongleCfg;
extern const LICENSE_DONGLE_CONFIG_t 	DfltLicenseDonglePara;

extern const VISITOR_MANAGEMENT_t		DfltVisitor;

extern pthread_mutex_t 					MutexLockAutoSwitchWiFi;
extern const SYS_ADVANCE_PARA_t			DfltSysAdvPara;

extern FR_SERVER_CONFIG_t				FRServerConfig;

extern FIRST_IN_LIST_t		FirstInUserGroup[MAX_FIRST_IN_LISTS];

extern IP_CAMERA_CONFIG_t				IpCameraConfig;
extern FILE_LOG_PARA_CONFIG_t			FileLogPara;
extern const FILE_LOG_PARA_CONFIG_t		DfltFileLogPara;

extern SERVER_MODE_CONFIG_t		ServerModeCfg;
extern WIFI_PARA_t						WifiParameters;
extern SAVED_WIFI_LIST_t			SavedWifiList[MAX_SAVED_WIFI];

//******** Defines and Data Types ****

//******** Function Prototypes *******

//******** Global Variables **********

//******** Static Variables **********

//******** Function Definitions ******

//*****************************************************************************
//	LoadOldModemPara()
//	Param:
//		IN :	INT32 fd
//				UINT8 ver
//		OUT:	NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			This function loads modem configuration according to old version
//		structure format from the flash.
//*****************************************************************************
BOOL LoadOldModemPara(UNUSED_PARA INT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}


//*****************************************************************************
//	LoadOldAdvancePara()
//	Param:
//		IN : INT32 fd
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old advance parameter.
//*****************************************************************************
BOOL LoadOldAdvancePara(INT32 fd, UINT8 ver)
{
	//Because of version detail
	lseek(fd, 1, SEEK_SET);

	// Read configuration
	switch(ver)
	{
		case 1:
		if( (sizeof(SYS_ADVANCE_PARA_VER1_t) ) != read(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_VER1_t) ) )
		{
			EPRINT(CONFIG_LOG, "Failed to read Advance configuration file Ver %d", ver);
			return FAIL;
		}
		break;
		
		case 2:
		{
			if( (sizeof(SYS_ADVANCE_PARA_VER2_t) ) != read(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_VER2_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read Advance configuration file Ver %d", ver);
				return FAIL;
			}
		}
		break;
		
		case 3:
		{
			if( (sizeof(SYS_ADVANCE_PARA_VER3_t) ) != read(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_VER3_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read Advance configuration file Ver %d", ver);
				return FAIL;
			}
		}
		break;

		case 4:
		{
			if( (sizeof(SYS_ADVANCE_PARA_VER4_t) ) != read(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_VER4_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read Advance configuration file Ver %d", ver);
				return FAIL;
			}
		}
		break;

		case 5:
		{
			if( (sizeof(SYS_ADVANCE_PARA_VER5_t) ) != read(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_VER5_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read Advance configuration file Ver %d", ver);
				return FAIL;
			}
		}
		break;

		case 6:
		{
			if( (sizeof(SYS_ADVANCE_PARA_VER6_t) ) != read(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_VER6_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read Advance configuration file Ver %d", ver);
				return FAIL;
			}
		}
		break;

		case 7:
		{
			if( (sizeof(SYS_ADVANCE_PARA_VER7_t) ) != read(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_VER7_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read Advance configuration file Ver %d", ver);
				return FAIL;
			}
		}
		break;

		case 8:
		{
			if( (sizeof(SYS_ADVANCE_PARA_VER8_t) ) != read(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_VER8_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read Advance configuration file Ver %d", ver);
				return FAIL;
			}
		}
		break;

		case 9:
		{
			if( (sizeof(SYS_ADVANCE_PARA_VER9_t) ) != read(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_VER9_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read Advance configuration file Ver %d", ver);
				return FAIL;
			}
		}
		break;

		case 10:
		{
			if( (sizeof(SYS_ADVANCE_PARA_VER10_t) ) != read(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_VER10_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read Advance configuration file Ver %d", ver);
				return FAIL;
			}
		}
		break;

		case 11:
		{
			if( (sizeof(SYS_ADVANCE_PARA_VER11_t) ) != read(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_VER11_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read Advance configuration file Ver %d", ver);
				return FAIL;
			}
		}
		break;

        case 12:
        {
            if( (sizeof(SYS_ADVANCE_PARA_VER12_t) ) != read(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_VER12_t) ) )
            {
                EPRINT(CONFIG_LOG, "Failed to read Advance configuration file Ver %d", ver);
                return FAIL;
            }
        }
        break;

		default:
		return FAIL;
	}

	if(ver == 1)
	{
		SysAdvPara.retainJobSelectionF = DfltSysAdvPara.retainJobSelectionF;
		ver = 2;
	}
	if(2 == ver)
	{
		SysAdvPara.saveFpImageF = DfltSysAdvPara.saveFpImageF;
		ver = 3;
	}
	if(3 == ver)
	{
		SysAdvPara.temperatureLogCnfg.isEnable = DfltSysAdvPara.temperatureLogCnfg.isEnable;
		SysAdvPara.temperatureLogCnfg.tempUnit = DfltSysAdvPara.temperatureLogCnfg.tempUnit;
		SysAdvPara.temperatureLogCnfg.sensorType = DfltSysAdvPara.temperatureLogCnfg.sensorType;
		SysAdvPara.temperatureLogCnfg.sensorInterface = DfltSysAdvPara.temperatureLogCnfg.sensorInterface;
		SysAdvPara.temperatureLogCnfg.emissivity = DfltSysAdvPara.temperatureLogCnfg.emissivity;
		SysAdvPara.temperatureLogCnfg.restrictionType = DfltSysAdvPara.temperatureLogCnfg.restrictionType;
		SysAdvPara.temperatureLogCnfg.bypassOnDisconnect = DfltSysAdvPara.temperatureLogCnfg.bypassOnDisconnect;
		SysAdvPara.temperatureLogCnfg.thresholdTemp = DfltSysAdvPara.temperatureLogCnfg.thresholdTemp;
		SysAdvPara.temperatureLogCnfg.calibrationPara = DfltSysAdvPara.temperatureLogCnfg.calibrationPara;
		SysAdvPara.temperatureLogCnfg.approachWaitTmr = DfltSysAdvPara.temperatureLogCnfg.approachWaitTmr;
		SysAdvPara.temperatureLogCnfg.tempDetectionTimer = DfltSysAdvPara.temperatureLogCnfg.tempDetectionTimer;
		SysAdvPara.temperatureLogCnfg.validSampleCnt = DfltSysAdvPara.temperatureLogCnfg.validSampleCnt;
		SysAdvPara.temperatureLogCnfg.thresholdDeviation = DfltSysAdvPara.temperatureLogCnfg.thresholdDeviation;
		SysAdvPara.temperatureLogCnfg.minimumThreshold = DfltSysAdvPara.temperatureLogCnfg.minimumThreshold;


		ver = 4;
	}
	if(ver == 4)
	{
		ver = 5;
	}
	if(ver == 5)
	{
		SysAdvPara.faceMaskComp_Enable = DfltSysAdvPara.faceMaskComp_Enable;
		SysAdvPara.faceMaskComp_IdtFaceMaskDetectionThreshold = DfltSysAdvPara.faceMaskComp_IdtFaceMaskDetectionThreshold;
		SysAdvPara.faceMaskComp_VisibleFaceForEnrl = DfltSysAdvPara.faceMaskComp_VisibleFaceForEnrl;
		SysAdvPara.faceMaskComp_ApproachCameraWaitTimer = DfltSysAdvPara.faceMaskComp_ApproachCameraWaitTimer;
		SysAdvPara.faceMaskComp_MaskDetectionTimeOut = DfltSysAdvPara.faceMaskComp_MaskDetectionTimeOut;
		SysAdvPara.faceMaskComp_RestrictionType = DfltSysAdvPara.faceMaskComp_RestrictionType;
		ver = 6;
	}
	if(ver == 6)
	{
		SysAdvPara.httpPort = DfltSysAdvPara.httpPort;
		SysAdvPara.httpsPort = DfltSysAdvPara.httpsPort;

		ver = 7;
	}
	if(ver == 7)
	{
		SysAdvPara.maxFacePerUser = DfltSysAdvPara.maxFacePerUser;
//		SysAdvPara.displayDistinctiveFeedbackF = DfltSysAdvPara.displayDistinctiveFeedbackF;
//		SysAdvPara.InPunchColor = DfltSysAdvPara.InPunchColor;
//		SysAdvPara.OutPunchColor = DfltSysAdvPara.OutPunchColor;
		SysAdvPara.alwTLSVer = DfltSysAdvPara.alwTLSVer;
		SysAdvPara.passwordPolicyF = DfltSysAdvPara.passwordPolicyF;

		ver = 8;
	}

	if(ver == 8)
	{
		SysAdvPara.itemQuantityRestrictionF = DfltSysAdvPara.itemQuantityRestrictionF;
		SysAdvPara.allowedOfflineItemQuantity = DfltSysAdvPara.allowedOfflineItemQuantity;
		SysAdvPara.cafeOfflineItemRestrict.activeMenuCode = DfltSysAdvPara.cafeOfflineItemRestrict.activeMenuCode;
		SysAdvPara.cafeOfflineItemRestrict.start = DfltSysAdvPara.cafeOfflineItemRestrict.start;
		SysAdvPara.cafeOfflineItemRestrict.end = DfltSysAdvPara.cafeOfflineItemRestrict.end;
		SysAdvPara.cafeOfflineItemRestrict.workingDay = DfltSysAdvPara.cafeOfflineItemRestrict.workingDay;
		SysAdvPara.cafeOfflineItemRestrict.startDate = DfltSysAdvPara.cafeOfflineItemRestrict.startDate;

		ver = 9;
	}

	if(ver == 9)
	{
		SysAdvPara.duressDetection = DfltSysAdvPara.duressDetection;

		ver = 10;
	}
	
	if(ver == 10)
	{
		SysAdvPara.facilityCodeCheckEnableF = DfltSysAdvPara.facilityCodeCheckEnableF;

		ver = 11;
	}

	if(ver == 11)
	{
		SysAdvPara.temporaryUserGenerateInfo.autoAddTempUser = DfltSysAdvPara.temporaryUserGenerateInfo.autoAddTempUser;
		SysAdvPara.temporaryUserGenerateInfo.confirmReqF = DfltSysAdvPara.temporaryUserGenerateInfo.confirmReqF;
		SysAdvPara.temporaryUserGenerateInfo.addNameViaDeviceF = DfltSysAdvPara.temporaryUserGenerateInfo.addNameViaDeviceF;

		ver = 12;
	}
    if(ver == 12)
    {
        SysAdvPara.authenticationType = BASIC_AUTHENTICATION;
        SysAdvPara.authenticationAlgorithm = DfltSysAdvPara.authenticationAlgorithm;
        ver = 13;
    }
	else
	{
		return FAIL;
	}

	if(lseek(fd, 0, SEEK_SET) != -1)
	{
		ver = ADVANCE_SYS_PARA_VER;
		if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write advance system config version");
			return FAIL;
		}
		if( (sizeof(SYS_ADVANCE_PARA_t) ) != write(fd, &SysAdvPara, sizeof(SYS_ADVANCE_PARA_t) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write advance system config");
			return FAIL;
		}
	}
	else
	{
		return FAIL;
	}

	SystemAdvanceParaConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadOldUserConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old system User configuration
//*****************************************************************************
BOOL LoadOldUserConfig(INT32 fd, UINT8 ver, UINT16 start, UINT8 length)
{
	UINT16		userIdx;
	UINT8		tempIdx;
	UINT8		tempVer;
	USER_CONFIG_VER1_t tempUserConfigVer1;
	USER_CONFIG_VER2_t tempUserConfigVer2;
	USER_CONFIG_VER3_t tempUserConfigVer3;
	USER_CONFIG_VER4_t tempUserConfigVer4;
	USER_CONFIG_VER5_t tempUserConfigVer5;
	USER_CONFIG_VER6_t tempUserConfigVer6;
	USER_CONFIG_VER7_t tempUserConfigVer7;
	USER_CONFIG_VER8_t tempUserConfigVer8;

	USER_CONFIG_t tUserConfig;

	lseek(fd, 1, SEEK_SET);
	for(userIdx = start; userIdx < (start+length); userIdx++)
	{
		tempVer = ver;
		switch(tempVer)
		{
		case 1:
			if(sizeof(USER_CONFIG_VER1_t) != read(fd, &tempUserConfigVer1, sizeof(USER_CONFIG_VER1_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read User configuration file");
				return FAIL;
			}
			break;

		case 2:
			if(sizeof(USER_CONFIG_VER2_t) != read(fd, &tempUserConfigVer2, sizeof(USER_CONFIG_VER2_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read User configuration file VER-2");
				return FAIL;
			}
			break;
		case 3:
			if(sizeof(USER_CONFIG_VER3_t) != read(fd, &tempUserConfigVer3, sizeof(USER_CONFIG_VER3_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read User configuration file VER-3");
				return FAIL;
			}
			break;
		case 4:
			if(sizeof(USER_CONFIG_VER4_t) != read(fd, &tempUserConfigVer4, sizeof(USER_CONFIG_VER4_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read User configuration file VER-4");
				return FAIL;
			}
			break;
		
		case 5:
			if(sizeof(USER_CONFIG_VER5_t) != read(fd, &tempUserConfigVer5, sizeof(USER_CONFIG_VER5_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read User configuration file VER-5");
				return FAIL;
			}
			break;
		case 6:
			if(sizeof(USER_CONFIG_VER6_t) != read(fd, &tempUserConfigVer6, sizeof(USER_CONFIG_VER6_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read User configuration file VER-6");
				return FAIL;
			}
			break;
		case 7:
			if(sizeof(USER_CONFIG_VER7_t) != read(fd, &tempUserConfigVer7, sizeof(USER_CONFIG_VER7_t) ) )
			{
				EPRINT(CONFIG_LOG, "Failed to read User configuration file VER-7");
				return FAIL;
			}
			break;
		case 8:
			if(sizeof(USER_CONFIG_VER8_t) != read(fd, &tempUserConfigVer8, sizeof(USER_CONFIG_VER8_t) ) )
			{
			EPRINT(CONFIG_LOG, "Failed to read User configuration file VER-8");
			return FAIL;
			}
			break;
		default:
			return FAIL;
			break;
		}
		if(tempVer == 1)
		{
			tempUserConfigVer2.enable = tempUserConfigVer1.enable;
			tempUserConfigVer2.userId = tempUserConfigVer1.userId;
			strcpy(tempUserConfigVer2.name, tempUserConfigVer1.name);
			strcpy(tempUserConfigVer2.alphaNumId,tempUserConfigVer1.alphaNumId);
			tempUserConfigVer2.vip = tempUserConfigVer1.vip;
			tempUserConfigVer2.bypassFinger  = tempUserConfigVer1.bypassFinger;
			for(tempIdx=0 ; tempIdx < MAX_USER_CARD ; tempIdx++)
			{
				tempUserConfigVer2.cardId[tempIdx] = tempUserConfigVer1.cardId[tempIdx] ;
			}
			memcpy(tempUserConfigVer2.pinCode,  tempUserConfigVer1.pinCode, MAX_PIN_SIZE_6_BYTES);
			tempUserConfigVer2.visitorIdx  = tempUserConfigVer1.visitorIdx;
			tempUserConfigVer2.terminationEnable  = tempUserConfigVer1.terminationEnable;
			tempUserConfigVer2.terminationDate = tempUserConfigVer1.terminationDate;
			tempUserConfigVer2.holidaySchId  = tempUserConfigVer1.holidaySchId;
			tempUserConfigVer2.absentee = tempUserConfigVer1.absentee ;
			tempUserConfigVer2.restrictAccess = tempUserConfigVer1.restrictAccess;
			tempUserConfigVer2.allowedForCafe = tempUserConfigVer1.allowedForCafe;
			tempUserConfigVer2.discountLevelCafe = tempUserConfigVer1.discountLevelCafe;
			tempUserConfigVer2.paymentModeForCafe = tempUserConfigVer1.paymentModeForCafe;
			tempUserConfigVer2.blockForCafe = tempUserConfigVer1.blockForCafe;
			tempUserConfigVer2.smartRouteId = tempUserConfigVer1.smartRouteId;
			tempUserConfigVer2.smartRouteLevel = tempUserConfigVer1.smartRouteLevel;
			tempUserConfigVer2.selfEnrlEnbl = tempUserConfigVer1.selfEnrlEnbl;
			tempUserConfigVer2.selfEnrlSkipedCnt = tempUserConfigVer1.selfEnrlSkipedCnt;
			tempUserConfigVer2.jobType = tempUserConfigVer1.jobType;
			tempUserConfigVer2.checkBalanceBeforeTxnF = tempUserConfigVer1.checkBalanceBeforeTxnF;
			tempUserConfigVer2.userGrpId = tempUserConfigVer1.userGrpId;
			tempUserConfigVer2.cafeteriaPolicyCheckF = tempUserConfigVer1.cafeteriaPolicyCheckF;
			tempUserConfigVer2.showAttndcDtlF = tempUserConfigVer1.showAttndcDtlF;
			tempUserConfigVer2.birthdayMsgEnable = tempUserConfigVer1.birthdayMsgEnable;
			tempUserConfigVer2.dateOfBirth = tempUserConfigVer1.dateOfBirth;
			tempUserConfigVer2.enableFaceRecoF = tempUserConfigVer1.enableFaceRecoF;
			tempUserConfigVer2.bypassFaceF = tempUserConfigVer1.bypassFaceF;

			tempVer = 2;
		}

		if(tempVer == 2)
		{
            memcpy(&tempUserConfigVer3, &tempUserConfigVer2, sizeof(USER_CONFIG_VER2_t));
            tempUserConfigVer3.offlineCafeTransactionType = DfltUserConfig.offlineCafeTransactionType;
			tempVer = 3;
		}
		if(tempVer == 3)
		{
			memcpy(&tempUserConfigVer4, &tempUserConfigVer3, sizeof(USER_CONFIG_VER3_t) );
			tempUserConfigVer4.lastfaceReceivedtime = DfltUserConfig.faceCredData.lastFaceReceivetime;
			tempUserConfigVer4.faceUpdateCount = DfltUserConfig.faceCredData.getFacePokeCount;
			tempUserConfigVer4.totalFaceInServer = DfltUserConfig.faceCredData.faceCountInServer;
			tempVer = 4;
		}
		if(tempVer == 4)
		{
			memcpy(&tempUserConfigVer5, &tempUserConfigVer4, sizeof(USER_CONFIG_VER4_t) );
			memcpy(&tempUserConfigVer5.consumedItemQuantity, &DfltUserConfig.consumedItemQuantity, MAX_CAFE_ITEM_CODE);
			tempVer = 5;
		}

		if(tempVer == 5)
		{
			memcpy(&tempUserConfigVer6, &tempUserConfigVer5, sizeof(USER_CONFIG_VER5_t) );
 			memcpy(&tempUserConfigVer6.DuressFp,&DfltUserConfig.DuressFp,sizeof(DfltUserConfig.DuressFp));
			tempVer = 6;
		}
		if(tempVer == 6)
		{
			memcpy(&tempUserConfigVer7, &tempUserConfigVer6, sizeof(USER_CONFIG_VER6_t) );
 			tempUserConfigVer7.panelDoorAssignF = DfltUserConfig.panelDoorAssignF ;
			tempVer = 7;
		}
		if(tempVer == 7)
		{
			memcpy(&tempUserConfigVer8, &tempUserConfigVer7, sizeof(USER_CONFIG_VER7_t) );
			tempUserConfigVer8.profilePhotoSyncPendingF = DfltUserConfig.profilePhotoSyncPendingF ;
			tempVer = 8;
		}
		if(tempVer == 8)
		{
		    CHAR	lastfaceupdatedtimeformate[HTTP_SYNC_DATE_LENGTH_MAX];

		    tUserConfig.enable = tempUserConfigVer8.enable;
		    tUserConfig.userId = tempUserConfigVer8.userId;
		    strcpy(tUserConfig.name, tempUserConfigVer8.name);
		    strcpy(tUserConfig.alphaNumId,tempUserConfigVer8.alphaNumId);
		    tUserConfig.vip = tempUserConfigVer8.vip;
		    tUserConfig.bypassFinger = tempUserConfigVer8.bypassFinger;
            	    for(tempIdx=0 ; tempIdx < MAX_USER_CARD ; tempIdx++)
    		    {
                	tUserConfig.cardId[tempIdx] = tempUserConfigVer8.cardId[tempIdx];
            	    }
		    strcpy(tUserConfig.pinCode,tempUserConfigVer8.pinCode);
		    tUserConfig.visitorIdx = tempUserConfigVer8.visitorIdx;
		    tUserConfig.terminationEnable = tempUserConfigVer8.terminationEnable;
		    memcpy(&tUserConfig.terminationDate, &tempUserConfigVer8.terminationDate, sizeof(DATE_DDMMYY_t));
		    tUserConfig.holidaySchId = tempUserConfigVer8.holidaySchId;
		    memcpy(&tUserConfig.absentee, &tempUserConfigVer8.absentee, sizeof(ABSENTEE_t));
		    tUserConfig.restrictAccess = tempUserConfigVer8.restrictAccess;
		    tUserConfig.allowedForCafe = tempUserConfigVer8.allowedForCafe;
		    tUserConfig.discountLevelCafe = tempUserConfigVer8.discountLevelCafe;
		    tUserConfig.paymentModeForCafe = tempUserConfigVer8.paymentModeForCafe;
		    tUserConfig.blockForCafe = tempUserConfigVer8.blockForCafe;
		    tUserConfig.smartRouteId = tempUserConfigVer8.smartRouteId;
		    tUserConfig.smartRouteLevel = tempUserConfigVer8.smartRouteLevel;
		    tUserConfig.selfEnrlEnbl = tempUserConfigVer8.selfEnrlEnbl;
		    tUserConfig.selfEnrlSkipedCnt = tempUserConfigVer8.selfEnrlSkipedCnt;
		    tUserConfig.jobType = tempUserConfigVer8.jobType;
		    tUserConfig.checkBalanceBeforeTxnF = tempUserConfigVer8.checkBalanceBeforeTxnF;
		    tUserConfig.userGrpId = tempUserConfigVer8.userGrpId;
		    tUserConfig.cafeteriaPolicyCheckF = tempUserConfigVer8.cafeteriaPolicyCheckF;
		    tUserConfig.showAttndcDtlF = tempUserConfigVer8.showAttndcDtlF;
		    tUserConfig.birthdayMsgEnable = tempUserConfigVer8.birthdayMsgEnable;
		    memcpy(&tUserConfig.dateOfBirth, &tempUserConfigVer8.dateOfBirth, sizeof(DATE_DDMMYY_t));
		    tUserConfig.enableFaceRecoF = tempUserConfigVer8.enableFaceRecoF;
		    tUserConfig.bypassFaceF = tempUserConfigVer8.bypassFaceF;
		    tUserConfig.offlineCafeTransactionType = tempUserConfigVer8.offlineCafeTransactionType;
		    memcpy(tUserConfig.consumedItemQuantity, tempUserConfigVer8.consumedItemQuantity, MAX_CAFE_ITEM_CODE);
		    memcpy(tUserConfig.DuressFp, tempUserConfigVer8.DuressFp, MAX_DURESS_DUAL_FP_TEMPLATE);
		    tUserConfig.panelDoorAssignF = tempUserConfigVer8.panelDoorAssignF;
		    tUserConfig.profilePhotoSyncPendingF = tempUserConfigVer8.profilePhotoSyncPendingF;
		    tUserConfig.faceCredData.faceCountInServer = tempUserConfigVer8.totalFaceInServer;
		    tUserConfig.faceCredData.getFacePokeCount = tempUserConfigVer8.faceUpdateCount;
		    tUserConfig.faceCredData.setFacePokeCount = DfltUserConfig.faceCredData.setFacePokeCount;
            	    for(tempIdx=0 ; tempIdx < SET_MAX_USER_FACE_INDEX ; tempIdx++)
    		    {
        		tUserConfig.faceCredData.setFaceIndex[tempIdx] = DfltUserConfig.faceCredData.setFaceIndex[tempIdx];
            	    }
		    strncpy(lastfaceupdatedtimeformate,"%d%m%Y%H%M%S" , HTTP_SYNC_DATE_LENGTH_MAX);
			time_t epoch_time;
			if((epoch_time = DateTimeStringConvertToEpoch(tempUserConfigVer8.lastfaceupdatedtime,lastfaceupdatedtimeformate)) >= 0)
			{
				tUserConfig.faceCredData.lastFaceEnrollTime = epoch_time;
			}
			else
			{
				tUserConfig.faceCredData.lastFaceEnrollTime = 0;
			}
            	    tUserConfig.faceCredData.lastFaceReceivetime = tempUserConfigVer8.lastfaceReceivedtime;

		    tempVer = 9;
		}
		else
		{
			return FAIL;
		}
        SetUserConfig(userIdx, tUserConfig);
	}

	if(lseek(fd, 0, SEEK_SET) != -1)
	{
		return WriteUserConfigByFd(fd, start, length);
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;

}

//*****************************************************************************
//	LoadOldBasicConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old basic system parameters
//*****************************************************************************
BOOL LoadOldBasicConfig(INT32 fd, UINT8 ver)
{
	SYS_BASIC_PARA_VER1_t tempSysBasicConfigVer1;
	SYS_BASIC_PARA_VER2_t tempSysBasicConfigVer2;
	SYS_BASIC_PARA_VER3_t tempSysBasicConfigVer3;
	SYS_BASIC_PARA_VER4_t tempSysBasicConfigVer4;
	SYS_BASIC_PARA_VER5_t tempSysBasicConfigVer5;
	SYS_BASIC_PARA_VER6_t tempSysBasicConfigVer6;
	SYS_BASIC_PARA_VER7_t tempSysBasicConfigVer7;
	SYS_BASIC_PARA_VER8_t tempSysBasicConfigVer8;
	SYS_BASIC_PARA_VER9_t tempSysBasicConfigVer9;
	SYS_BASIC_PARA_VER10_t tempSysBasicConfigVer10;
	SYS_BASIC_PARA_VER11_t tempSysBasicConfigVer11;
	SYS_BASIC_PARA_VER12_t tempSysBasicConfigVer12;
	SYS_BASIC_PARA_VER13_t tempSysBasicConfigVer13;
	HTTP_WEB_SERVER_CONFIG_t    temphttpWebServerConfig;
	UINT8 tempIdx, tempReader;

	if (lseek(fd, 1, SEEK_SET) != -1)
	{
		switch(ver)
		{
		case 1:
			if( sizeof(SYS_BASIC_PARA_VER1_t) != read(fd, &tempSysBasicConfigVer1, sizeof(SYS_BASIC_PARA_VER1_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver 1");
				return FAIL;
			}
			break;
		case 2:
			if( sizeof(SYS_BASIC_PARA_VER2_t) != read(fd, &tempSysBasicConfigVer2, sizeof(SYS_BASIC_PARA_VER2_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver 2");
				return FAIL;
			}
			break;
		case 3:
			if( sizeof(SYS_BASIC_PARA_VER3_t) != read(fd, &tempSysBasicConfigVer3, sizeof(SYS_BASIC_PARA_VER3_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver 3");
				return FAIL;
			}
			break;
		case 4:
			if( sizeof(SYS_BASIC_PARA_VER4_t) != read(fd, &tempSysBasicConfigVer4, sizeof(SYS_BASIC_PARA_VER4_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver 4");
				return FAIL;
			}
			break;

		case 5:
			if( sizeof(SYS_BASIC_PARA_VER5_t) != read(fd, &tempSysBasicConfigVer5, sizeof(SYS_BASIC_PARA_VER5_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver %u", ver);
				return FAIL;
			}
			break;

		case 6:
			if( sizeof(SYS_BASIC_PARA_VER6_t) != read(fd, &tempSysBasicConfigVer6, sizeof(SYS_BASIC_PARA_VER6_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver %u", ver);
				return FAIL;
			}
			break;
		case 7:
			if( sizeof(SYS_BASIC_PARA_VER7_t) != read(fd, &tempSysBasicConfigVer7, sizeof(SYS_BASIC_PARA_VER7_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver %u", ver);
				return FAIL;
			}
			break;

		case 8:
			if( sizeof(SYS_BASIC_PARA_VER8_t) != read(fd, &tempSysBasicConfigVer8, sizeof(SYS_BASIC_PARA_VER8_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver %u", ver);
				return FAIL;
			}
			break;

		case 9:
			if( sizeof(SYS_BASIC_PARA_VER9_t) != read(fd, &tempSysBasicConfigVer9, sizeof(SYS_BASIC_PARA_VER9_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver %u", ver);
				return FAIL;
			}
			break;

		case 10:
			if( sizeof(SYS_BASIC_PARA_VER10_t) != read(fd, &tempSysBasicConfigVer10, sizeof(SYS_BASIC_PARA_VER10_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver %u", ver);
				return FAIL;
			}
			break;
			
		case 11:
			if( sizeof(SYS_BASIC_PARA_VER11_t) != read(fd, &tempSysBasicConfigVer11, sizeof(SYS_BASIC_PARA_VER11_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver %u", ver);
				return FAIL;
			}
			break;
		case 12:
			if( sizeof(SYS_BASIC_PARA_VER12_t) != read(fd, &tempSysBasicConfigVer12, sizeof(SYS_BASIC_PARA_VER12_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver %u", ver);
				return FAIL;
			}
			break;

		case 13:
			if( sizeof(SYS_BASIC_PARA_VER13_t) != read(fd, &tempSysBasicConfigVer13, sizeof(SYS_BASIC_PARA_VER13_t) ) )
			{
				EPRINT(CONFIG_LOG," Failed to read basic config ver %u", ver);
				return FAIL;
			}
			break;
		default:
			return FAIL;
		}

		if(ver == 1)
		{
			memcpy(&tempSysBasicConfigVer2, &tempSysBasicConfigVer1, sizeof(SYS_BASIC_PARA_VER1_t));
			tempSysBasicConfigVer2.accessAllowScreenDuration = DfltBasicSysPara.accessAllowScreenDuration;
			tempSysBasicConfigVer2.accessDenyCadDuration = tempSysBasicConfigVer2.accessAllowCadDuration;
			tempSysBasicConfigVer2.accessDenyScreenDuration = DfltBasicSysPara.accessDenyScreenDuration;
			ver = 2;
		}
		if(ver == 2)
		{

			tempSysBasicConfigVer3.devId = tempSysBasicConfigVer2.devId;
			strncpy(tempSysBasicConfigVer3.name, tempSysBasicConfigVer2.name, DEVICE_NAME_SIZE_UTF8);
			tempSysBasicConfigVer3.timeFormat = tempSysBasicConfigVer2.timeFormat;
			tempSysBasicConfigVer3.doorAccessMode = tempSysBasicConfigVer2.doorAccessMode;
			tempSysBasicConfigVer3.application = tempSysBasicConfigVer2.application;
			tempSysBasicConfigVer3.exitOnLock = tempSysBasicConfigVer2.exitOnLock;
			tempSysBasicConfigVer3.tamperAlarm = tempSysBasicConfigVer2.tamperAlarm;
			tempSysBasicConfigVer3.doorSense = tempSysBasicConfigVer2.doorSense ;
			tempSysBasicConfigVer3.doorPulseTime = tempSysBasicConfigVer2.doorPulseTime;
			tempSysBasicConfigVer3.autoRelock = tempSysBasicConfigVer2.autoRelock;
			tempSysBasicConfigVer3.userDefNtp = tempSysBasicConfigVer2.userDefNtp;
			strncpy(tempSysBasicConfigVer3.preDefNtpAddr, tempSysBasicConfigVer2.preDefNtpAddr, MAX_NTP_ADDRESS_UTF8);
			strncpy(tempSysBasicConfigVer3.userDefNtpAddr, tempSysBasicConfigVer2.userDefNtpAddr, MAX_USER_DEF_NTP_ADDRESS_UTF8);
			tempSysBasicConfigVer3.utz = tempSysBasicConfigVer2.utz;
			tempSysBasicConfigVer3.timeUpdateMode = tempSysBasicConfigVer2.timeUpdateMode ;
			tempSysBasicConfigVer3.workingHrStart = tempSysBasicConfigVer2.workingHrStart;
			tempSysBasicConfigVer3.workingHrStop= tempSysBasicConfigVer2.workingHrStop;
			for(tempIdx = 0; tempIdx < MAX_WDAY ;tempIdx++)
			{
				tempSysBasicConfigVer3.workingDays[tempIdx] = tempSysBasicConfigVer2.workingDays[tempIdx];
			}
			tempSysBasicConfigVer3.holidayWorking= tempSysBasicConfigVer2.holidayWorking;
			for(tempReader = 0; tempReader < OLD_MAX_READERS_VER9 ; tempReader++)
			{
				tempSysBasicConfigVer3.reader[tempReader].mode = tempSysBasicConfigVer2.reader[tempReader].mode;
				tempSysBasicConfigVer3.reader[tempReader].type = tempSysBasicConfigVer2.reader[tempReader].type;
			}
			tempSysBasicConfigVer3. requestExitSw = tempSysBasicConfigVer2.requestExitSw;;
			tempSysBasicConfigVer3.buzMuteF = tempSysBasicConfigVer2.buzMuteF;
			tempSysBasicConfigVer3.tagRedetectDelay = tempSysBasicConfigVer2. tagRedetectDelay;
			tempSysBasicConfigVer3.cdcExitReaderF = tempSysBasicConfigVer2.cdcExitReaderF;
			tempSysBasicConfigVer3.cerPresentF = tempSysBasicConfigVer2.cerPresentF;
			tempSysBasicConfigVer3.extRdrAccessMode = tempSysBasicConfigVer2 .extRdrAccessMode;
			tempSysBasicConfigVer3.degradeModeEnable = tempSysBasicConfigVer2. degradeModeEnable;
			tempSysBasicConfigVer3.degradeWaitTimer = tempSysBasicConfigVer2.degradeWaitTimer;
			tempSysBasicConfigVer3.doorModeSchdlEnblF= tempSysBasicConfigVer2.doorModeSchdlEnblF;
			tempSysBasicConfigVer3.entryModeWallpaperF = tempSysBasicConfigVer2.entryModeWallpaperF;
			tempSysBasicConfigVer3.exitModeWallpaperF = tempSysBasicConfigVer2.exitModeWallpaperF;
			tempSysBasicConfigVer3.fprTemplateFormat = tempSysBasicConfigVer2.fprTemplateFormat;
			tempSysBasicConfigVer3.extWiegandRdrMode = tempSysBasicConfigVer2.extWiegandRdrMode;
			tempSysBasicConfigVer3.wiegandOutFormat = tempSysBasicConfigVer2.wiegandOutFormat;
			tempSysBasicConfigVer3.wiegandOutDir = tempSysBasicConfigVer2.wiegandOutDir;
			tempSysBasicConfigVer3.waitForAccessFeedback = tempSysBasicConfigVer2.waitForAccessFeedback;
			tempSysBasicConfigVer3.accessFeedbackWaitTime = tempSysBasicConfigVer2.accessFeedbackWaitTime;
			tempSysBasicConfigVer3.doorModeSelectEnblF = tempSysBasicConfigVer2.doorModeSelectEnblF;
			tempSysBasicConfigVer3.waitForUsrVerification = tempSysBasicConfigVer2.waitForUsrVerification;
			memcpy(tempSysBasicConfigVer3.wgndOutCstmFrmtNo, tempSysBasicConfigVer2.wgndOutCstmFrmtNo, sizeof(tempSysBasicConfigVer2.wgndOutCstmFrmtNo) );
			memcpy(tempSysBasicConfigVer3.wgndOutEventSpclCode, tempSysBasicConfigVer2.wgndOutEventSpclCode, sizeof(tempSysBasicConfigVer2.wgndOutEventSpclCode) );
			tempSysBasicConfigVer3.IoLinkNwInterface = tempSysBasicConfigVer2.IoLinkNwInterface;
			tempSysBasicConfigVer3.autoRelockTime = tempSysBasicConfigVer2.autoRelockTime;
			tempSysBasicConfigVer3.apiAccessAllowEnable = tempSysBasicConfigVer2.apiAccessAllowEnable;
			tempSysBasicConfigVer3.apiEntryAccessMode = tempSysBasicConfigVer2.apiEntryAccessMode;
			tempSysBasicConfigVer3.apiExitAccessMode = tempSysBasicConfigVer2.apiExitAccessMode;
			strncpy(tempSysBasicConfigVer3.apiSecurityKey,tempSysBasicConfigVer2.apiSecurityKey, (SECURITY_KEY_SIZE+1));
			tempSysBasicConfigVer3.accessAllowCadDuration = tempSysBasicConfigVer2.accessAllowCadDuration;
			tempSysBasicConfigVer3.visitorDoorAccessMode = tempSysBasicConfigVer2.visitorDoorAccessMode;
			tempSysBasicConfigVer3.visitorExtRdrAccessMode = tempSysBasicConfigVer2.visitorExtRdrAccessMode;
			strncpy(tempSysBasicConfigVer3.pinChangeCode, tempSysBasicConfigVer2.pinChangeCode, MAX_PIN_CHANGE_CODE_SIZE);
			tempSysBasicConfigVer3.promptSpFnF = tempSysBasicConfigVer2.promptSpFnF;
			tempSysBasicConfigVer3.generateEventF = tempSysBasicConfigVer2.generateEventF;
			tempSysBasicConfigVer3.cbrPresentF = tempSysBasicConfigVer2.cbrPresentF;
			tempSysBasicConfigVer3.accessAllowScreenDuration = tempSysBasicConfigVer2.accessAllowScreenDuration;
			tempSysBasicConfigVer3.accessDenyCadDuration = tempSysBasicConfigVer2.accessDenyCadDuration;
			tempSysBasicConfigVer3.accessDenyScreenDuration = tempSysBasicConfigVer2.accessDenyScreenDuration;
			tempSysBasicConfigVer3.abnormalAlarm = tempSysBasicConfigVer2.alarmIndication;
			tempSysBasicConfigVer3.forceOpenAlarm = tempSysBasicConfigVer2.alarmIndication;
			tempSysBasicConfigVer3.faultAlarm = tempSysBasicConfigVer2.alarmIndication;
			tempSysBasicConfigVer3.panicAlarm = tempSysBasicConfigVer2.alarmIndication;

			ver = 3;
		}
		if(ver == 3)
		{
			memcpy(&tempSysBasicConfigVer4, &tempSysBasicConfigVer3, sizeof(SYS_BASIC_PARA_VER3_t));
			tempSysBasicConfigVer4.tempThresholdAlarm = DfltBasicSysPara.tempThresholdAlarm;
			ver = 4;
		}
		if(ver == 4)
		{
			memcpy(&tempSysBasicConfigVer5, &tempSysBasicConfigVer4, sizeof(tempSysBasicConfigVer4));
			tempSysBasicConfigVer5.cafeteriaFrAccessMode = DfltBasicSysPara.cafeteriaFrAccessMode;
			ver = 5;
		}
		if(ver == 5)
		{
			memcpy(&tempSysBasicConfigVer6, &tempSysBasicConfigVer5, sizeof(tempSysBasicConfigVer5));
			ver = 6;
		}

		if(ver == 6)
		{
			memcpy(&tempSysBasicConfigVer7, &tempSysBasicConfigVer6, sizeof(tempSysBasicConfigVer6));
			//Added new access mode in apiEntryAccessMode and apiExitAccessMode
			ver = 7;
		}

		if(ver == 7)
		{
			memcpy(&tempSysBasicConfigVer8, &tempSysBasicConfigVer7, sizeof(tempSysBasicConfigVer7));
			tempSysBasicConfigVer8.faceMaskNotDetectedAlarm = DfltBasicSysPara.faceMaskNotDetectedAlarm;
			ver = 8;
		}
		
		if(ver == 8)
		{
			memcpy(&tempSysBasicConfigVer9, &tempSysBasicConfigVer8, sizeof(tempSysBasicConfigVer8));
			tempSysBasicConfigVer9.atomRD100PresentF = DfltBasicSysPara.atomRD100PresentF;
			ver = 9;
		}

		if(ver == 9)
		{	
			tempSysBasicConfigVer10.devId = tempSysBasicConfigVer9.devId;
			strncpy(tempSysBasicConfigVer10.name,tempSysBasicConfigVer9.name,DEVICE_NAME_SIZE_UTF8);
			tempSysBasicConfigVer10.timeFormat = tempSysBasicConfigVer9.timeFormat;
			tempSysBasicConfigVer10.doorAccessMode = tempSysBasicConfigVer9.doorAccessMode;
			tempSysBasicConfigVer10.application = tempSysBasicConfigVer9.application;
			tempSysBasicConfigVer10.exitOnLock = tempSysBasicConfigVer9.exitOnLock;
			tempSysBasicConfigVer10.tamperAlarm = tempSysBasicConfigVer9.tamperAlarm;
			tempSysBasicConfigVer10.doorSense = tempSysBasicConfigVer9.doorSense;
			tempSysBasicConfigVer10.doorPulseTime = tempSysBasicConfigVer9.doorPulseTime;
			tempSysBasicConfigVer10.autoRelock = tempSysBasicConfigVer9.autoRelock;
			tempSysBasicConfigVer10.userDefNtp = tempSysBasicConfigVer9.userDefNtp;
			strncpy(tempSysBasicConfigVer10.preDefNtpAddr,tempSysBasicConfigVer9.preDefNtpAddr,MAX_NTP_ADDRESS_UTF8);
			strncpy(tempSysBasicConfigVer10.userDefNtpAddr,tempSysBasicConfigVer9.userDefNtpAddr,MAX_USER_DEF_NTP_ADDRESS_UTF8);
			tempSysBasicConfigVer10.utz = tempSysBasicConfigVer9.utz;
			tempSysBasicConfigVer10.timeUpdateMode = tempSysBasicConfigVer9.timeUpdateMode;
			tempSysBasicConfigVer10.workingHrStart = tempSysBasicConfigVer9.workingHrStart;
			tempSysBasicConfigVer10.workingHrStop = tempSysBasicConfigVer9.workingHrStop;
			memcpy((VOIDPTR)tempSysBasicConfigVer10.workingDays,(VOIDPTR)tempSysBasicConfigVer9.workingDays,sizeof(tempSysBasicConfigVer10.workingDays));
			tempSysBasicConfigVer10.holidayWorking = tempSysBasicConfigVer9.holidayWorking;

			UINT8 tmpReaderId;
			for(tmpReaderId=0 ;tmpReaderId < OLD_MAX_READERS_VER9;tmpReaderId++)
			{
				tempSysBasicConfigVer10.reader[tmpReaderId] = tempSysBasicConfigVer9.reader[tmpReaderId];
			}
			tempSysBasicConfigVer10.reader[READER5].type = DfltBasicSysPara.reader[READER5].type;
			tempSysBasicConfigVer10.reader[READER5].mode = DfltBasicSysPara.reader[READER5].mode;
			tempSysBasicConfigVer10.requestExitSw = tempSysBasicConfigVer9.requestExitSw;
			tempSysBasicConfigVer10.buzMuteF = tempSysBasicConfigVer9.buzMuteF;
			tempSysBasicConfigVer10.tagRedetectDelay = tempSysBasicConfigVer9.tagRedetectDelay;
			tempSysBasicConfigVer10.cdcExitReaderF = tempSysBasicConfigVer9.cdcExitReaderF;
			tempSysBasicConfigVer10.cerPresentF = tempSysBasicConfigVer9.cerPresentF;
			tempSysBasicConfigVer10.extRdrAccessMode = tempSysBasicConfigVer9.extRdrAccessMode;
			tempSysBasicConfigVer10.degradeModeEnable = tempSysBasicConfigVer9.degradeModeEnable;
			tempSysBasicConfigVer10.degradeWaitTimer = tempSysBasicConfigVer9.degradeWaitTimer;
			tempSysBasicConfigVer10.doorModeSchdlEnblF = tempSysBasicConfigVer9.doorModeSchdlEnblF;
			tempSysBasicConfigVer10.entryModeWallpaperF = tempSysBasicConfigVer9.entryModeWallpaperF;
			tempSysBasicConfigVer10.exitModeWallpaperF = tempSysBasicConfigVer9.exitModeWallpaperF;
			tempSysBasicConfigVer10.fprTemplateFormat = tempSysBasicConfigVer9.fprTemplateFormat;
			tempSysBasicConfigVer10.extWiegandRdrMode = tempSysBasicConfigVer9.extWiegandRdrMode;
			tempSysBasicConfigVer10.wiegandOutFormat = tempSysBasicConfigVer9.wiegandOutFormat;
			tempSysBasicConfigVer10.wiegandOutDir = tempSysBasicConfigVer9.wiegandOutDir;
			tempSysBasicConfigVer10.waitForAccessFeedback = tempSysBasicConfigVer9.waitForAccessFeedback;
			tempSysBasicConfigVer10.accessFeedbackWaitTime = tempSysBasicConfigVer9.accessFeedbackWaitTime;
			tempSysBasicConfigVer10.doorModeSelectEnblF = tempSysBasicConfigVer9.doorModeSelectEnblF;
			tempSysBasicConfigVer10.waitForUsrVerification = tempSysBasicConfigVer9.waitForUsrVerification;
			memcpy((VOIDPTR)tempSysBasicConfigVer10.wgndOutCstmFrmtNo,(VOIDPTR)tempSysBasicConfigVer9.wgndOutCstmFrmtNo,sizeof(tempSysBasicConfigVer10.wgndOutCstmFrmtNo));
			memcpy((VOIDPTR)tempSysBasicConfigVer10.wgndOutEventSpclCode,(VOIDPTR)tempSysBasicConfigVer9.wgndOutEventSpclCode,sizeof(tempSysBasicConfigVer10.wgndOutEventSpclCode));
			tempSysBasicConfigVer10.IoLinkNwInterface = tempSysBasicConfigVer9.IoLinkNwInterface;
			tempSysBasicConfigVer10.autoRelockTime = tempSysBasicConfigVer9.autoRelockTime;
			tempSysBasicConfigVer10.apiAccessAllowEnable = tempSysBasicConfigVer9.apiAccessAllowEnable;
			tempSysBasicConfigVer10.apiEntryAccessMode = tempSysBasicConfigVer9.apiEntryAccessMode;
			tempSysBasicConfigVer10.apiExitAccessMode = tempSysBasicConfigVer9.apiExitAccessMode;
			strncpy(tempSysBasicConfigVer10.apiSecurityKey,tempSysBasicConfigVer9.apiSecurityKey,SECURITY_KEY_SIZE+1);
			tempSysBasicConfigVer10.accessAllowCadDuration = tempSysBasicConfigVer9.accessAllowCadDuration;
			tempSysBasicConfigVer10.visitorDoorAccessMode = tempSysBasicConfigVer9.visitorDoorAccessMode;
			tempSysBasicConfigVer10.visitorExtRdrAccessMode = tempSysBasicConfigVer9.visitorExtRdrAccessMode;
			strncpy(tempSysBasicConfigVer10.pinChangeCode,tempSysBasicConfigVer9.pinChangeCode,MAX_PIN_CHANGE_CODE_SIZE);
			tempSysBasicConfigVer10.promptSpFnF = tempSysBasicConfigVer9.promptSpFnF;
			tempSysBasicConfigVer10.generateEventF = tempSysBasicConfigVer9.generateEventF;
			tempSysBasicConfigVer10.cbrPresentF = tempSysBasicConfigVer9.cbrPresentF;
			tempSysBasicConfigVer10.accessAllowScreenDuration = tempSysBasicConfigVer9.accessAllowScreenDuration;
			tempSysBasicConfigVer10.accessDenyCadDuration = tempSysBasicConfigVer9.accessDenyCadDuration;
			tempSysBasicConfigVer10.accessDenyScreenDuration = tempSysBasicConfigVer9.accessDenyScreenDuration;
			tempSysBasicConfigVer10.abnormalAlarm = tempSysBasicConfigVer9.abnormalAlarm;
			tempSysBasicConfigVer10.forceOpenAlarm = tempSysBasicConfigVer9.forceOpenAlarm;
			tempSysBasicConfigVer10.faultAlarm = tempSysBasicConfigVer9.faultAlarm;
			tempSysBasicConfigVer10.panicAlarm = tempSysBasicConfigVer9.panicAlarm;
			tempSysBasicConfigVer10.tempThresholdAlarm = tempSysBasicConfigVer9.tempThresholdAlarm;
			tempSysBasicConfigVer10.cafeteriaFrAccessMode = tempSysBasicConfigVer9.cafeteriaFrAccessMode;
			tempSysBasicConfigVer10.faceMaskNotDetectedAlarm = tempSysBasicConfigVer9.faceMaskNotDetectedAlarm;
			tempSysBasicConfigVer10.atomRD100PresentF = tempSysBasicConfigVer9.atomRD100PresentF;
			tempSysBasicConfigVer10.atomRD200PresentF = DfltBasicSysPara.atomRD200PresentF;
			tempSysBasicConfigVer10.atomRD300PresentF = DfltBasicSysPara.atomRD300PresentF;
			ver = 10;
		}
		if(ver == 10)
		{	
			memcpy(&tempSysBasicConfigVer11, &tempSysBasicConfigVer10, sizeof(tempSysBasicConfigVer10));
			tempSysBasicConfigVer11.degradeModeType = DfltBasicSysPara.degradeModeType;
			ver = 11;
		}
		if(ver == 11)
		{	
			memcpy(&tempSysBasicConfigVer12, &tempSysBasicConfigVer11, sizeof(tempSysBasicConfigVer11));
			tempSysBasicConfigVer12.doorPulseTimeFloat = (FLOAT32)tempSysBasicConfigVer11.doorPulseTime;
			if(tempSysBasicConfigVer12.doorPulseTimeFloat == 0)
			{
				tempSysBasicConfigVer12.doorPulseTimeFloat = DfltBasicSysPara.doorPulseTimeFloat;
			}
			ver = 12;
		}
		
		if(ver == 12)
		{	
			memcpy(&tempSysBasicConfigVer13, &tempSysBasicConfigVer12, sizeof(tempSysBasicConfigVer12));
			tempSysBasicConfigVer13.accessScheduleID = DfltBasicSysPara.accessScheduleID;
			tempSysBasicConfigVer13.accessScheduleEnableF = DfltBasicSysPara.accessScheduleEnableF;
			tempSysBasicConfigVer13.hideMenuBarForAccessScheduleF = DfltBasicSysPara.hideMenuBarForAccessScheduleF;
			ver = 13;
		}

		if(ver == 13)
		{
		    memcpy(&SysBasicPara, &tempSysBasicConfigVer13, sizeof(tempSysBasicConfigVer13));
		    temphttpWebServerConfig = DfltBasicSysPara.httpWebServerConfig;
		    memcpy((VOIDPTR)&(SysBasicPara.httpWebServerConfig),(VOIDPTR)&temphttpWebServerConfig,sizeof(temphttpWebServerConfig));


		    INT32 		file;
            UINT8		httpConfigVer;
		    //BOOL 		status;
		    HTTP_SERVER_VER1_t  httpServerConfigVer1;

		    if( (file = open(SYS_HTTP_SERVER_CNFG_FILE, O_RDWR | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
		    {
		        EPRINT(CONFIG_LOG,"Failed to open http server Configuration file");
		    }
		    else
		    {
		        // read version number
                if(sizeof(httpConfigVer) != read(file, &httpConfigVer, sizeof(httpConfigVer)))
		        {
		            EPRINT(CONFIG_LOG,"Failed to read  http server Configuration version");
		            close(file);
		        }
		        else
		        {
		            lseek(file, 1, SEEK_SET);

		            if( sizeof(HTTP_SERVER_VER1_t) != read(file, &httpServerConfigVer1, sizeof(HTTP_SERVER_VER1_t)) )
		            {
		                EPRINT(CONFIG_LOG,"Failed to read Http Server Configuration in ver1");
		                close(file);
		            }
		            else
		            {
		                snprintf(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId, MAX_HTTP_SERVER_USER_ID_LEN, "%s", httpServerConfigVer1.httpServerUserId);
		                snprintf(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword, MAX_HTTP_SERVER_PASSWORD_LEN, "%s", httpServerConfigVer1.httpServerPassword);
		                snprintf(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerDirectory, MAX_HTTP_SERVER_DIRECTORY_LEN, "%s", httpServerConfigVer1.httpServerDirectory);
		                SysBasicPara.httpWebServerConfig.httpSyncConfig.interfaceSelctionF = httpServerConfigVer1.interfaceSelctionF;
						snprintf(SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerUserId, MAX_HTTP_SERVER_USER_ID_LEN, "%s", httpServerConfigVer1.httpServerUserId);
		                snprintf(SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPassword, MAX_HTTP_SERVER_PASSWORD_LEN, "%s", httpServerConfigVer1.httpServerPassword);
		                snprintf(SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerDirectory, MAX_HTTP_SERVER_DIRECTORY_LEN, "%s", httpServerConfigVer1.httpServerDirectory);
						SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.interfaceSelctionF = httpServerConfigVer1.interfaceSelctionF;

				close(file);
		                //remove file after copy config

                        RemoveFile(SYS_HTTP_SERVER_CNFG_FILE);
		            }
		        }
		    }
			ver = 14;
		}
		else
		{
			return FAIL;
		}

		SysBasicParaConfigDebug();
		if(lseek(fd, 0, SEEK_SET) != -1)
		{
			ver = BASIC_SYS_PARA_VER;
			if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
			{
				EPRINT(CONFIG_LOG, "Fail to write basic system config version");
				return FAIL;
			}
			if( (sizeof(SYS_BASIC_PARA_t) ) != write(fd, &SysBasicPara, sizeof(SYS_BASIC_PARA_t) ) )
			{
				EPRINT(CONFIG_LOG, "Fail to write basic system config");
				return FAIL;
			}
		}
		else
		{
			EPRINT(CONFIG_LOG, "Fail to write basic system config LSEEK ");
			return FAIL;
		}
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	LoadOldSmartRouteAccessConfig()
//	Param:
//		IN : INT32 fd
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old smart route access configuration and update new fileds
//			with default value.
//*****************************************************************************
BOOL LoadOldSmartRouteAccessConfig(UNUSED_PARA INT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldCafeMenuConfig()
//	Param:
//		IN : INT32 fd
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old Cafe Menu Parameter configuration.
//*****************************************************************************
BOOL LoadOldCafeMenuConfig(UNUSED_PARA UINT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldBioIdtServerConfig()
//	Param:
//		IN : INT32 fd
//		OUT: UINT8 ver
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old bio identification sever configuration and update new fields
//			with default value.
//*****************************************************************************
BOOL LoadOldBioIdtServerConfig(UNUSED_PARA INT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldSysVarInfo()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:

//*****************************************************************************
BOOL LoadOldSysVarInfo(UNUSED_PARA INT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldPwdConfig()
//	Param:
//		IN : INT32 fd
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old LoadOldPwdConfig.
//*****************************************************************************
BOOL LoadOldPwdConfig(INT32 fd, UINT8 ver)
{
	//Because of version detail
	lseek(fd, 1, SEEK_SET);

	// Read configuration
	switch(ver)
	{
	case 1:
		if( (sizeof(PASSWORD_VER1_t) ) != read(fd, &Password, sizeof(PASSWORD_VER1_t) ) )
		{
			EPRINT(CONFIG_LOG, "Failed to read password configuration file Ver %d",ver);
			return FAIL;
		}
		break;

    case 2:
        if( (sizeof(PASSWORD_VER2_t) ) != read(fd, &Password, sizeof(PASSWORD_VER2_t) ) )
        {
            EPRINT(CONFIG_LOG, "Failed to read password configuration file Ver %d",ver);
            return FAIL;
        }
        break;

	default:
		return FAIL;
	}

	if(ver == 1)
	{
		UINT8	tmpIdx;
		for(tmpIdx = 0; tmpIdx < MAX_USER_TYPE; tmpIdx++)
		{
			// Common Display & Web Password is Now separated so copy old password as web password.
			sprintf(Password.webPswd[tmpIdx], "%s", Password.displayPswd[tmpIdx]);

			// Set password expiry days to default
			Password.PswdResetDaysCount[tmpIdx] = DfltPassword.PswdResetDaysCount[tmpIdx];
		}

		ver = 2;
	}

    if(ver == 2)
    {
        UINT8	tmpIdx;
        for(tmpIdx = 0; tmpIdx < MAX_USER_TYPE; tmpIdx++)
        {
            Password.DispPwdAttemptCount[tmpIdx] = MAX_DISP_PWD_ATTEMPT_COUNT;
            Password.DispPwdLoginRestrictTimer[tmpIdx] = INVALID_TIMER_HANDLE;
        }
        ver = 3;
    }
	else
	{
		return FAIL;
	}
	PasswordConfigDebug();
	if(lseek(fd, 0, SEEK_SET) != -1)
	{
		ver = PASSWORD_VER;
		if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write password config version");
			return FAIL;
		}
		if( (sizeof(PASSWORD_t) ) != write(fd, &Password, sizeof(PASSWORD_t) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write password config");
			return FAIL;
		}
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}

//*****************************************************************************
//	LoadOldPrinterConfig()
//	Param:
//		IN : INT32 fd
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old printer config
//*****************************************************************************
BOOL LoadOldPrinterConfig(UNUSED_PARA INT32 fd, UNUSED_PARA UINT8 ver)
{
	UINT8 idx;
	lseek(fd, 1, SEEK_SET);

	// Read configuration
	switch(ver)
	{
		case 1:
			if( sizeof(PRINTER_CONFIG_VER_1_t) != read(fd, &PrinterConfig, sizeof(PRINTER_CONFIG_VER_1_t)) )
			{
				EPRINT(CONFIG_LOG,"Failed to read Printer Configuration in ver1");
				close(fd);
				return FAIL;
			}
			break;

		default:
			return FAIL;
	}

	if(ver == 1)
	{
		PrinterConfig.printUserBalanceOnCouponF = DefltPrinterConfig.printUserBalanceOnCouponF;
		ver = 2;
	}

	PrinterConfigDebug();

	if(lseek(fd, 0, SEEK_SET) != -1)
	{
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
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}

//*****************************************************************************
//	LoadOldCafeItemConfig()
//	Param:
//		IN : INT32 fd
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old Cafe Item Parameter configuration.
//*****************************************************************************
BOOL LoadOldCafeItemConfig(UNUSED_PARA UINT32 fd,UNUSED_PARA  UINT8 ver)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldJCMConfig()
//	Param:
//		IN : INT32 fd
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old Cafe Item Parameter configuration.
//*****************************************************************************
BOOL LoadOldJCMConfig(UINT32 fd, UINT8 ver)
{
	UINT8				tempver;
	JCM_CONFIG_VER1_t 	tempJCMConfigVer1;
	UINT16 				configIdx;

	//Because of version detail
	lseek(fd, 1, SEEK_SET);

	for(configIdx = 0; configIdx < MAX_JCM_CONFIG; configIdx++)
	{
		tempver = ver;
		switch(tempver)
		{
			case 1:
			{
				if( (sizeof(JCM_CONFIG_VER1_t) ) != read(fd, &tempJCMConfigVer1, sizeof(JCM_CONFIG_VER1_t ) ) )
				{
					EPRINT(CONFIG_LOG,"Failed to read JCM Item configuration version %d", ver);
					return FAIL;
				}
				break;
			}

			default:
			{
				return FAIL;
			}
		}

		if(tempver == 1)
		{
			JcmConfig[configIdx].refId = tempJCMConfigVer1.refId ;
			memset((VOIDPTR)JcmConfig[configIdx].jobCode,0,sizeof(JcmConfig[configIdx].jobCode));
			strncpy(JcmConfig[configIdx].jobCode, tempJCMConfigVer1.jobCode,strlen(tempJCMConfigVer1.jobCode)+1);
			strncpy(JcmConfig[configIdx].jobName, tempJCMConfigVer1.jobName,strlen(tempJCMConfigVer1.jobName)+1);
			memcpy((VOIDPTR)&JcmConfig[configIdx].startDate,(VOIDPTR)&tempJCMConfigVer1.startDate,sizeof(tempJCMConfigVer1.startDate));
			memcpy((VOIDPTR)&JcmConfig[configIdx].endDate,(VOIDPTR)&tempJCMConfigVer1.endDate,sizeof(tempJCMConfigVer1.endDate));
			tempver = 2;
		}
		else
		{
			return FAIL;
		}

	}

	memcpy((VOIDPTR)&JcmConfig[MAX_JCM_CONFIG],(VOIDPTR)&NoneJobConfig,sizeof(NoneJobConfig));
	memcpy((VOIDPTR)&JcmConfig[MAX_JCM_CONFIG + 1],(VOIDPTR)&DefaultJobConfig,sizeof(DefaultJobConfig));
	memcpy((VOIDPTR)&JcmConfig[MAX_JCM_CONFIG + 2],(VOIDPTR)&ContinueJobConfig,sizeof(ContinueJobConfig));

	JcmConfigDebug(MAX_JCM_CONFIG+3);
	if(lseek(fd, 0, SEEK_SET) != -1)
	{
		ver = JCM_CONFIG_VER;
		if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write JCM configuration version");
			return FAIL;
		}

		if(sizeof(JcmConfig) != write(fd, &JcmConfig[0], sizeof(JcmConfig) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write JCM configuration");
			return FAIL;
		}
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}

//*****************************************************************************
//	LoadOldWebApiEventConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:

//*****************************************************************************
BOOL LoadOldWebApiEventConfig(UNUSED_PARA INT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldEthParaConfig()
//	Param:
//		IN : INT32 fd
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old DVREVT Parameter configuration.
//*****************************************************************************
BOOL LoadOldEthParaConfig(UNUSED_PARA UINT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldDvrEvtActnConfig()
//	Param:
//		IN : INT32 fd
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old DVREVT Parameter configuration.
//*****************************************************************************
BOOL LoadOldDvrEvtActnConfig(UNUSED_PARA UINT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldBlockedUserConfig()
//	Param:
//		IN : INT32 fd
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old Blocked User config configuration.
//*****************************************************************************
BOOL LoadOldBlockedUserConfig(UNUSED_PARA UINT32 fd, UNUSED_PARA UINT8 ver, UNUSED_PARA UINT16 start, UNUSED_PARA UINT8 length , UNUSED_PARA UINT8 pg)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldVisitorMangmentConfig()
//	Param:
//		IN : INT32 fd
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old VisitorMangment config configuration.
//*****************************************************************************
BOOL LoadOldVisitorMangmentConfig(UNUSED_PARA UINT32 fd, UNUSED_PARA UINT8 ver, UNUSED_PARA UINT16 start, UNUSED_PARA UINT8 length)
{
	UINT8	configIdx, tempver;

	//Because of version detail
	lseek(fd, 1, SEEK_SET);

	for(configIdx = start; configIdx < (start+length); configIdx++)
	{
		tempver = ver;
		switch(tempver)
		{
			case 1:
			{
				if( (sizeof(VISITOR_MANAGEMENT_VER1_t) ) != read(fd, &Visitor[configIdx], sizeof(VISITOR_MANAGEMENT_VER1_t ) ) )
				{
					EPRINT(CONFIG_LOG,"Failed to read VisitorMangment configuration version %d", ver);
					return FAIL;
				}
				break;
			}

			default:
			{
				return FAIL;
			}
		}

		if(tempver == 1)
		{
			UINT8 idx;
			Visitor[configIdx].repeatMode = DfltVisitor.repeatMode;
			for(idx = 0; idx < MAX_REPEAT_DAYS; idx++)
			{
				Visitor[configIdx].repeatDays[idx] = DfltVisitor.repeatDays[idx];
			}
			tempver = 2;
		}
		else
		{
			return FAIL;
		}
	}

	if(lseek(fd, 0, SEEK_SET) != -1)
	{
		ver = VISITOR_MANAGEMENT_VER;
		if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write VisitorMangment configuration version");
			return FAIL;
		}

		if((INT32)(sizeof(VISITOR_MANAGEMENT_t)*length) != write(fd, &Visitor[start], (sizeof(VISITOR_MANAGEMENT_t)*length) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write VisitorMangment configuration");
			return FAIL;
		}
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}

//*****************************************************************************
//	LoadOldSpecialFunctionConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old Special Function configuration
//*****************************************************************************
BOOL LoadOldSpecialFunctionConfig(UNUSED_PARA INT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldBackUpAndUpdateConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:

//*****************************************************************************
BOOL LoadOldBackUpAndUpdateConfig(INT32 fd, UINT8 ver)
{
	BACKUP_AND_UPDATE_VER1_t tempBackUpAndUpdateVer1;

	//Because of version detail
	lseek(fd, 1, SEEK_SET);

	switch(ver)
	{
		case 1:
		{
			if( (sizeof(BACKUP_AND_UPDATE_VER1_t) ) != read(fd, &tempBackUpAndUpdateVer1, sizeof(BACKUP_AND_UPDATE_VER1_t ) ) )
			{
				EPRINT(CONFIG_LOG,"Failed to read BackUpAndUpdate configuration version %d", ver);
				return FAIL;
			}
			break;
		}

		default:
		{
			return FAIL;
		}
	}

	if(ver == 1)
	{
		BackUpAndUpdate.interface = tempBackUpAndUpdateVer1.interface;
		snprintf(BackUpAndUpdate.ftpServerUrl, MAX_FTP_SERVER_URL, "%s", tempBackUpAndUpdateVer1.ftpServerUrl);
		snprintf(BackUpAndUpdate.ftpServerUseName, MAX_FTP_USER_NAME, "%s", tempBackUpAndUpdateVer1.ftpServerUseName);
		snprintf(BackUpAndUpdate.ftpServerUserPassword, MAX_FTP_USER_PASSWORD_40_BYTES, "%s", tempBackUpAndUpdateVer1.ftpServerUserPassword);
		BackUpAndUpdate.autoUpdate = tempBackUpAndUpdateVer1.autoUpdate;
		BackUpAndUpdate.upgradeBasedOn = tempBackUpAndUpdateVer1.upgradeBasedOn;
		BackUpAndUpdate.interfaceSelctionF = tempBackUpAndUpdateVer1.interfaceSelctionF;
		snprintf(BackUpAndUpdate.ftpServerResolveIpAddress, MAX_IP_ADDRESS, "%s", tempBackUpAndUpdateVer1.ftpServerResolveIpAddress);
		ver = 2;
	}
	else
	{
		return FAIL;
	}

	if(lseek(fd, 0, SEEK_SET) != -1)
	{
		ver = BACKUP_AND_UPDATE_CONFIG_VER;
		if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
		{
			EPRINT(CONFIG_LOG, "Failed to write BackUpAndUpdate configuration version");
			return FAIL;
		}

		if((INT32)sizeof(BackUpAndUpdate) != write(fd, &BackUpAndUpdate, sizeof(BackUpAndUpdate)) )
		{
			EPRINT(CONFIG_LOG, "Failed to write BackUpAndUpdate configuration");
			return FAIL;
		}
	}
	else
	{
		return FAIL;
	}

	BackUpAndUpdateConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadOldCmdCtrlConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:

//*****************************************************************************
BOOL LoadOldCmdCtrlConfig(UNUSED_PARA INT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldIoLinkConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old IO Link configuration
//*****************************************************************************
BOOL LoadOldIoLinkConfig(UNUSED_PARA INT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}
//*****************************************************************************
//  LoadOldCardFormatConfig()
//	Param:
//		IN :
//		OUT:
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description: Loads old card format configuration and update new fields
//				 with default value.
//*****************************************************************************
BOOL LoadOldCardFormatConfig(UNUSED_PARA INT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldWiFiConfig()
//	Param:
//		IN : fd
//			 ver
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old basic system parameters
//*****************************************************************************
BOOL LoadOldWiFiConfig( INT32 fd,  UINT8 ver)
{
	WIFI_PARA_VER1_t wifiParameterVer1;
    WIFI_PARA_VER2_t wifiParameterVer2;
	
	if(lseek(fd, 1, SEEK_SET)!= -1)
	{

		// Read configuration
		switch(ver)
		{
			case 1:
				if( (sizeof(WIFI_PARA_VER1_t) ) != read(fd, &wifiParameterVer1, sizeof(WIFI_PARA_VER1_t) ) )
				{
					EPRINT(CONFIG_LOG, "Failed to read wifi parameter configuration file Ver %d",ver);
					return FAIL;
				}
				break;
            case 2:
                if( (sizeof(WIFI_PARA_VER2_t) ) != read(fd, &wifiParameterVer2, sizeof(WIFI_PARA_VER2_t) ) )
                {
                    EPRINT(CONFIG_LOG, "Failed to read wifi parameter configuration file Ver %d",ver);
                    return FAIL;
                }
                break;

			default:
				return FAIL;
		}
		
		if(ver == 1)
		{
            memcpy(&wifiParameterVer2, &wifiParameterVer1, sizeof(WIFI_PARA_VER1_t) );
            wifiParameterVer2.bandFreqType = WIFI_BAND_FREQ_2GHz;
			ver = 2;
		}
        if(ver == 2)
        {
            snprintf(WifiParameters.ipAddress, MAX_IP_ADDRESS, "%s", wifiParameterVer2.ipAddress);
            snprintf(WifiParameters.subnet, MAX_SUBNET_MASK, "%s", wifiParameterVer2.subnet);
            snprintf(WifiParameters.gateway, MAX_GATEWAY, "%s", wifiParameterVer2.gateway);
            snprintf(WifiParameters.macAddress, MAX_MAC_ADDRESS, "%s", wifiParameterVer2.macAddress);
            snprintf(WifiParameters.dns1, MAX_DNS_ADDRESS, "%s", wifiParameterVer2.dns1);
            snprintf(WifiParameters.dns2, MAX_DNS_ADDRESS, "%s", wifiParameterVer2.dns2);
            WifiParameters.nwType = wifiParameterVer2.nwType;
            memset(WifiParameters.ssid, 0, sizeof(WifiParameters.ssid));
            snprintf(WifiParameters.ssid, MAX_SSID_NAME_OLD, "%s", wifiParameterVer2.ssid);
            WifiParameters.secType = wifiParameterVer2.secType;
            WifiParameters.encType = wifiParameterVer2.encType;
            snprintf(WifiParameters.passphresh, MAX_PASSPHRESH_SIZE, "%s", wifiParameterVer2.passphresh);
            WifiParameters.keymgmt = wifiParameterVer2.keymgmt;
            WifiParameters.wpsTrigerTyp = wifiParameterVer2.wpsTrigerTyp;
            snprintf(WifiParameters.wpsPin, MAX_WPS_PIN_SIZE, "%s", wifiParameterVer2.wpsPin);
            WifiParameters.advAuthType = wifiParameterVer2.advAuthType;
            snprintf(WifiParameters.userName, MAX_WIFI_IDENTITY_SIZE, "%s", wifiParameterVer2.userName);
            snprintf(WifiParameters.anonymousIdentity, MAX_WIFI_IDENTITY_SIZE, "%s", wifiParameterVer2.anonymousIdentity);
            WifiParameters.peapVer = wifiParameterVer2.peapVer;
            WifiParameters.innerAuth = wifiParameterVer2.innerAuth;
            WifiParameters.enableWifi = wifiParameterVer2.enableWifi;
            WifiParameters.bandFreqType = wifiParameterVer2.bandFreqType;

            ver = 3;
        }
		else
		{
			return FAIL;
		}
		
		WifiParameterConfigDebug();
		if(lseek(fd, 0, SEEK_SET) != -1)
		{
			ver = WIFI_PARA_VER;
			if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
			{
				EPRINT(CONFIG_LOG, "Fail to write wifi parameter config version");
				return FAIL;
			}
			if( (sizeof(WIFI_PARA_t) ) != write(fd, &WifiParameters, sizeof(WIFI_PARA_t) ) )
			{
				EPRINT(CONFIG_LOG, "Fail to write wifi parameter config");
				return FAIL;
			}
		}
		else
		{
			EPRINT(CONFIG_LOG, "Fail to write wifi parameter config LSEEK ");
			return FAIL;
		}
		return SUCCESS;
	}
	
	return FAIL;
}

//*****************************************************************************
//	LoadOldConnectToPanelConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:

//*****************************************************************************
BOOL LoadOldConnectToPanelConfig(UNUSED_PARA INT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}
//*****************************************************************************
//	LoadOldLicenseDongleParaConfig()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:

//*****************************************************************************
BOOL LoadOldLicenseDongleParaConfig(UNUSED_PARA INT32 fd, UNUSED_PARA UINT8 ver)
{
	return FAIL;
}

//*****************************************************************************
//	LoadOldFRServerConfig()
//	Param:
//		IN :fd
//			ver
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Old FR server config in new config structure.
//*****************************************************************************
BOOL LoadOldFRServerConfig(INT32 fd, UINT8 ver)
{
	lseek(fd, 1, SEEK_SET);

	switch(ver)
	{
	case 1:
		if(sizeof(FR_SERVER_CONFIG_VER1_t) != read(fd, &FRServerConfig, sizeof(FR_SERVER_CONFIG_VER1_t) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to read FR server Configuration according to version1");
			return FAIL;
		}break;
	case 2:
	{
		if(sizeof(FR_SERVER_CONFIG_VER2_t) != read(fd, &FRServerConfig, sizeof(FR_SERVER_CONFIG_VER2_t) ) )
		{
            		EPRINT(CONFIG_LOG, "Fail to read FR server Configuration according to version2");
			return FAIL;
		}
	}break;
	case 3:
	if(sizeof(FR_SERVER_CONFIG_VER3_t) != read(fd, &FRServerConfig, sizeof(FR_SERVER_CONFIG_VER3_t) ) )
	{
		EPRINT(CONFIG_LOG, "Fail to read FR server Configuration according to version3");
		return FAIL;
	}
	break;

	case 4:
		if(sizeof(FR_SERVER_CONFIG_VER4_t) != read(fd, &FRServerConfig, sizeof(FR_SERVER_CONFIG_VER4_t) ) )
		{
            		EPRINT(CONFIG_LOG, "Fail to read FR server Configuration according to version4");
			return FAIL;
		}
		break;
	case 5:
		if(sizeof(FR_SERVER_CONFIG_VER5_t) != read(fd, &FRServerConfig, sizeof(FR_SERVER_CONFIG_VER5_t) ) )
		{
            		EPRINT(CONFIG_LOG, "Fail to read FR server Configuration according to version5");
			return FAIL;
		}
		break;
	case 6:
		if(sizeof(FR_SERVER_CONFIG_VER6_t) != read(fd, &FRServerConfig, sizeof(FR_SERVER_CONFIG_VER6_t) ) )
		{
		    EPRINT(CONFIG_LOG, "Fail to read FR server Configuration according to version6");
		    return FAIL;
		}
		break;
	case 7:
		if(sizeof(FR_SERVER_CONFIG_VER7_t) != read(fd, &FRServerConfig, sizeof(FR_SERVER_CONFIG_VER7_t) ) )
		{
		    EPRINT(CONFIG_LOG, "Fail to read FR server Configuration according to version7");
		    return FAIL;
		}
		break;

	default:
		break;
	}

	FR_SERVER_CONFIG_t tmpDfltFrServerSettings = GetDefaultFrServerConfig();
	if(ver == 1)
	{
		FRServerConfig.enableFreeScanTimeOutF = tmpDfltFrServerSettings.enableFreeScanTimeOutF;
		ver = 2;
	}
	if(ver == 2)
	{
		FRServerConfig.encryptionSsl = tmpDfltFrServerSettings.encryptionSsl;
		ver = 3;
	}
	if(ver == 3)
	{
		FRServerConfig.groupFrSupportF = tmpDfltFrServerSettings.groupFrSupportF;
		FRServerConfig.adaptiveFaceEnrollCnfg.isAdaptiveFaceEnrollEnbled = tmpDfltFrServerSettings.adaptiveFaceEnrollCnfg.isAdaptiveFaceEnrollEnbled;
		FRServerConfig.adaptiveFaceEnrollCnfg.thresholdDeviation = tmpDfltFrServerSettings.adaptiveFaceEnrollCnfg.thresholdDeviation;
		FRServerConfig.adaptiveFaceEnrollCnfg.multiUserMatchScoreDeviation = tmpDfltFrServerSettings.adaptiveFaceEnrollCnfg.multiUserMatchScoreDeviation;
		FRServerConfig.adaptiveFaceEnrollCnfg.cnfrmBfrAdaptiveEnrollFlg = tmpDfltFrServerSettings.adaptiveFaceEnrollCnfg.cnfrmBfrAdaptiveEnrollFlg;
		ver = 4;
	}
	if(ver == 4)
	{
		FRServerConfig.frServerFaceAntiSpoofingF = tmpDfltFrServerSettings.frServerFaceAntiSpoofingF;
		FRServerConfig.frServerFaceAntiSpoofingMode = tmpDfltFrServerSettings.frServerFaceAntiSpoofingMode;
		FRServerConfig.frServerFaceAntiSpoofingThreshold = tmpDfltFrServerSettings.frServerFaceAntiSpoofingThreshold;
		ver = 5;
	}
	if(ver == 5)
	{
		FRServerConfig.conflictChkF = tmpDfltFrServerSettings.conflictChkF;
		FRServerConfig.conflictMatchThrshld = tmpDfltFrServerSettings.conflictMatchThrshld;
        FRServerConfig.frServerExceptionalFaceF = tmpDfltFrServerSettings.frServerExceptionalFaceF;
        FRServerConfig.AdeptiveMaxFaceCnt = tmpDfltFrServerSettings.AdeptiveMaxFaceCnt;
		ver = 6;
	}
	if(ver == 6)
	{
		FRServerConfig.showUnknownUserAck = tmpDfltFrServerSettings.showUnknownUserAck;
		ver = 7;
	}
    if(ver == 7)
	{
		FRServerConfig.generateUnidentifiedFaceEvent = tmpDfltFrServerSettings.generateUnidentifiedFaceEvent;
		ver = 8;
	}
	else
	{
		return FAIL;
	}

	FRServerConfigDebug();
	if(lseek(fd, 0, SEEK_SET) != -1)
	{
		ver = FR_SERVER_CONFIG_VER;
		if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write FR Server Config Ver");
			return FAIL;
		}

		if( (sizeof(FR_SERVER_CONFIG_t) ) != write(fd, &FRServerConfig, sizeof(FR_SERVER_CONFIG_t) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write FR Server Configuration  Para");
			return FAIL;
		}

	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}

//*****************************************************************************
//	LoadOldFirstInRuleConfig()
//	Param:
//		IN : INT32 fd, UINT8 ver
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:

//*****************************************************************************
BOOL LoadOldFirstInRuleConfig(INT32 fd, UINT8 ver)
{
    FIRST_IN_LIST_VER1_t  tempFirstInUser1;
    FIRST_IN_LIST_VER2_t  tempFirstInUser2;
	UINT8 configIdx;
	UINT8 tempVer;

	lseek(fd, 1, SEEK_SET);
	
	for(configIdx = 0, tempVer = ver; configIdx < MAX_FIRST_IN_LISTS; tempVer = ver, configIdx++)
	{
		switch(tempVer)
		{
			case 1:
			{
                if(sizeof(FIRST_IN_LIST_VER1_t) != read(fd, &tempFirstInUser1, sizeof(FIRST_IN_LIST_VER1_t)))
				{
					EPRINT(CONFIG_LOG, "FirstInUser configuration index [%d] version [%d] read failed..!!", configIdx, tempVer);
					return FAIL;
				}
			}
			break;

            case 2:
            {
                if(sizeof(FIRST_IN_LIST_VER2_t) != read(fd, &tempFirstInUser2, sizeof(FIRST_IN_LIST_VER2_t)))
                {
                    EPRINT(CONFIG_LOG, "FirstInUser configuration index [%d] version [%d] read failed..!!", configIdx, tempVer);
                    return FAIL;
                }
            }
            break;

			default:
				EPRINT(CONFIG_LOG, "Unexpected Ver received from First In User configuration file");
				return FAIL;
		}

		if(tempVer == 1)
		{
            memset(tempFirstInUser2.members, INVALID_USER_ID, sizeof(tempFirstInUser2.members));
            memcpy(tempFirstInUser2.members, tempFirstInUser1.members, sizeof(tempFirstInUser1.members));
            tempVer = 2;
		}
        if(tempVer == 2)
        {
             memset(FirstInUserGroup[configIdx].members, INVALID_USER_ID, sizeof(FirstInUserGroup[configIdx].members));
            memcpy(FirstInUserGroup[configIdx].members, tempFirstInUser2.members, sizeof(tempFirstInUser2.members));
            tempVer = 3;
        }
		else
		{
			return FAIL;
		}
	}

	if(lseek(fd, 0, SEEK_SET) != -1)
	{
		ver = FIRST_IN_CFG_VER;
		if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
		{
			EPRINT(CONFIG_LOG, "Fail to write First In User configuration file version");
			return FAIL;
		}
		if(sizeof(FirstInUserGroup) != write(fd, &FirstInUserGroup, sizeof(FirstInUserGroup)))
		{
			EPRINT(CONFIG_LOG, "Fail to write First In User configuration file data");
			return FAIL;
		}
	}
	
	return SUCCESS;
}

//*****************************************************************************
//	LoadOldIpCameraConfig()
//	Param:
//		IN : INT32 fd, UINT8 ver
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old IpCameraConfig into new version structure
//*****************************************************************************
BOOL LoadOldIpCameraConfig(INT32 fd, UINT8 ver)
{
	IP_CAMERA_CONFIG_VER1_t tempIpCameraConfigVer1;

	lseek(fd, 1, SEEK_SET);

	switch(ver)
	{
		case 1:
		{
			if(sizeof(IP_CAMERA_CONFIG_VER1_t) != read(fd, &tempIpCameraConfigVer1, sizeof(IP_CAMERA_CONFIG_VER1_t)))
			{
				EPRINT(CONFIG_LOG, "IpCameraConfig version [%d] read failed..!!", ver);
				return FAIL;
			}
		}
		break;

		default:
			EPRINT(CONFIG_LOG, "Unexpected Ver received from IpCameraConfig file");
			return FAIL;
	}

	if(ver == 1)
	{
		IpCameraConfig.interfaceSelectionF = tempIpCameraConfigVer1.interfaceSelectionF;
		snprintf(IpCameraConfig.captureSnapshotUrl, CAPTURE_SNAPSHOT_URL_LENGTH, "%s", tempIpCameraConfigVer1.captureSnapshotUrl);
		snprintf(IpCameraConfig.captureSnapshotUserName, CAPTURE_SNAPSHOT_USERNAME_LENGTH, "%s", tempIpCameraConfigVer1.captureSnapshotUserName);
		snprintf(IpCameraConfig.captureSnapshotPassword, CAPTURE_SNAPSHOT_PASSWORD_LENGTH_40_Bytes, "%s", tempIpCameraConfigVer1.captureSnapshotPassword); //Change in this version
		memcpy(IpCameraConfig.captureSnapshotSchedule, tempIpCameraConfigVer1.captureSnapshotSchedule, sizeof(IpCameraConfig.captureSnapshotSchedule));
		ver = 2;
	}
	else
	{
		return FAIL;
	}

	if(lseek(fd, 0, SEEK_SET) != -1)
	{
		ver = IP_CAMERA_CONFIG_VER;
		if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
		{
			EPRINT(CONFIG_LOG, "Fail to write IpCameraConfig file version");
			return FAIL;
		}
		if(sizeof(IpCameraConfig) != write(fd, &IpCameraConfig, sizeof(IpCameraConfig)))
		{
			EPRINT(CONFIG_LOG, "Fail to write IpCameraConfig file data");
			return FAIL;
		}
	}

	IpCameraConfigDebug();
	return SUCCESS;
}

//*****************************************************************************
//	LoadOldFileLogConfig()
//	Param:
//		IN :fd
//			ver
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Old FR server config in new config structure.
//*****************************************************************************
BOOL LoadOldFileLogConfig(INT32 fd, UINT8 ver)
{

	if(lseek(fd, 1, SEEK_SET) == -1)
	{
		return FAIL;
	}

	switch(ver)
	{
		case 1:
			if(sizeof(FILE_LOG_PARA_CONFIG_VER1_t) != read(fd, &FileLogPara, sizeof(FILE_LOG_PARA_CONFIG_VER1_t)))
			{
				EPRINT(CONFIG_LOG, "Fail to read file log para Configuration version[%d]", ver);
				return FAIL;
			}break;
			
		case 2:
			if(sizeof(FILE_LOG_PARA_CONFIG_VER2_t) != read(fd, &FileLogPara, sizeof(FILE_LOG_PARA_CONFIG_VER2_t)))
			{
				EPRINT(CONFIG_LOG, "Fail to read file log para Configuration version[%d]", ver);
				return FAIL;
			}break;
		case 3:
			if(sizeof(FILE_LOG_PARA_CONFIG_VER3_t) != read(fd, &FileLogPara, sizeof(FILE_LOG_PARA_CONFIG_VER3_t)))
			{
				EPRINT(CONFIG_LOG, "Fail to read file log para Configuration version[%d]", ver);
				return FAIL;
			}break;

		default:
			break;
	}

	if(ver == 1)
	{
		FileLogPara.bleLog = DfltFileLogPara.bleLog;
		ver = 2;
	}
	if(ver == 2)
	{
		FileLogPara.readerDebugLog = DfltFileLogPara.readerDebugLog;
		ver=3;
	}
	if(ver == 3)
	{
		FileLogPara.httpSyncLog = DfltFileLogPara.httpSyncLog;
	}
	else
	{
		return FAIL;
	}

	if(lseek(fd, 0, SEEK_SET) != -1)
	{
		ver = FILE_LOG_PARA_CONFIG_VER;
		if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
		{
			EPRINT(CONFIG_LOG, "Fail to write File log Configuration Ver");
			return FAIL;
		}

		if( (sizeof(FILE_LOG_PARA_CONFIG_t)) != write(fd, &FileLogPara, sizeof(FILE_LOG_PARA_CONFIG_t)))
		{
			EPRINT(CONFIG_LOG, "Fail to write File log Configuration  Para");
			return FAIL;
		}
	}
	else
	{
		return FAIL;
	}
	return SUCCESS;
}

//*****************************************************************************
//	LoadOldServerParaConfig()
//	Param:
//		IN :fd
//			ver
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads Old server config in new config structure.
//*****************************************************************************
BOOL LoadOldServerParaConfig(UINT32 fd, UINT8 ver)
{
	SERVER_PARA_VER1_t tempServerParameterVer1;
	SERVER_PARA_VER2_t tempServerParameterVer2;
	
	//Because of version detail
	lseek(fd, 2, SEEK_SET);
	switch(ver)
	{
		case 1:
		{
			if( (sizeof(SERVER_PARA_VER1_t) ) != read(fd, &tempServerParameterVer1, sizeof(SERVER_PARA_VER1_t ) ) )
			{
				EPRINT(CONFIG_LOG,"Failed to read Server Parameters configuration version %d", ver);
				return FAIL;
			}
			break;
		}
		case 2:
		{
			if( (sizeof(SERVER_PARA_VER2_t) ) != read(fd, &tempServerParameterVer2, sizeof(SERVER_PARA_VER2_t ) ) )
			{
				EPRINT(CONFIG_LOG,"Failed to read Server Parameters configuration version %d", ver);
				return FAIL;
			}
			break;
		}
		default:
		{
			EPRINT(CONFIG_LOG,"Unexpected Server Parameters configuration version: %d", ver);
			return FAIL;
		}
	}

	if(ver == 1)
	{
		if(ServerModeCfg.serverMode == COSEC_VYOM)
		{
			tempServerParameterVer2.httpSyncRequestRetryTimer = DfltBasicSysPara.httpWebServerConfig.httpSyncRequestRetryTimer;
		}
		else
		{
			tempServerParameterVer2.httpSyncRequestRetryTimer = DfltBasicSysPara.httpWebServerConfig.httpSyncRequestRetryTimer;
            //added
            if((strlen(tempServerParameterVer2.httpServerHostName) == 0) || (strlen(V11R3ServerParameters.httpServerHostName)==0))
            {
                snprintf(tempServerParameterVer2.httpServerHostName, MAX_SERVER_ADDR_254, "%s", V11R3ServerParameters.ethServerHostName);
                snprintf(V11R3ServerParameters.httpServerHostName, MAX_SERVER_ADDR_254, "%s", V11R3ServerParameters.ethServerHostName);
            }
		}
		ver = 2;
	}
	if(ver == 2)
	{
		ServerParameters.interface = tempServerParameterVer2.interface;
		ServerParameters.secondInterface = tempServerParameterVer2.secondInterface;
		ServerParameters.thirdInterface = tempServerParameterVer2.thirdInterface;

		snprintf(ServerParameters.ethServerIpAddress, MAX_IP_ADDRESS, "%s", tempServerParameterVer2.ethServerIpAddress);
		snprintf(ServerParameters.ethServerHostName, MAX_SERVER_ADDR_254, "%s", tempServerParameterVer2.ethServerHostName);
		ServerParameters.ethServerConnection = tempServerParameterVer2.ethServerConnection;
		ServerParameters.ethServerPort = tempServerParameterVer2.ethServerPort;

		snprintf(ServerParameters.wifiServerIpAddress, MAX_IP_ADDRESS, "%s", tempServerParameterVer2.wifiServerIpAddress);
		snprintf(ServerParameters.wifiServerHostName, MAX_SERVER_ADDR_254, "%s", tempServerParameterVer2.wifiServerHostName);
		ServerParameters.wifiServerConnection = tempServerParameterVer2.wifiServerConnection;
		ServerParameters.wifiServerPort = tempServerParameterVer2.wifiServerPort;

		snprintf(ServerParameters.modemServerIpAddress, MAX_IP_ADDRESS, "%s", tempServerParameterVer2.modemServerIpAddress);
		snprintf(ServerParameters.modemServerHostName, MAX_SERVER_ADDR_254, "%s", tempServerParameterVer2.modemServerHostName);
		ServerParameters.modemServerConnection = tempServerParameterVer2.modemServerConnection;
		ServerParameters.modemServerPort = tempServerParameterVer2.modemServerPort;
		
		SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface = (UINT8)tempServerParameterVer2.httpInterface;
		snprintf(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress, MAX_IP_ADDRESS, "%s", tempServerParameterVer2.httpServerIpAddress);
		snprintf(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName, MAX_SERVER_FR_SERVER_HOSTNAME, "%s", tempServerParameterVer2.httpServerHostName);
		SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpInterface = (UINT8)tempServerParameterVer2.httpInterface;
		snprintf(SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerIpAddress, MAX_IP_ADDRESS, "%s", tempServerParameterVer2.httpServerIpAddress);
		snprintf(SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerHostName, MAX_SERVER_FR_SERVER_HOSTNAME, "%s", tempServerParameterVer2.httpServerHostName);

		ServerParameters.httpServerConnection = tempServerParameterVer2.httpServerConnection;
		
		SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort = tempServerParameterVer2.httpServerPort;
		SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPort = tempServerParameterVer2.httpServerPort;

		ServerParameters.type = tempServerParameterVer2.type;
		ServerParameters.tcpEncryptionModeF = tempServerParameterVer2.tcpEncryptionModeF;
	
		SysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF = tempServerParameterVer2.httpEncryptionModeF;
		SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpEncryptionModeF = tempServerParameterVer2.httpEncryptionModeF;

		snprintf(ServerParameters.TenantIdVyom, MAX_TENANT_ID, "%s", tempServerParameterVer2.TenantIdVyom);
		snprintf(ServerParameters.unusedBytes, 512, "%s", tempServerParameterVer2.unusedBytes);
		
		SysBasicPara.httpWebServerConfig.httpSyncRequestRetryTimer = tempServerParameterVer2.httpSyncRequestRetryTimer;

		if(WriteBasicParaConfig() == FAIL)
		{
		    EPRINT(CONFIG_LOG,"Failed to write system basic configuration");
		}

		ver = 3;
	}
	else
	{
		return FAIL;
	}

	if(lseek(fd, 0, SEEK_SET) != -1)
	{
		ver = SERVER_PARA_VER;
		if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
		{
			EPRINT(CONFIG_LOG, "Failed to write ServerParameters configuration version");
			return FAIL;
		}

        // Write network parameter version number
        //added
        ver = SERVER_PARA_SUB_VER;
        if(sizeof(ver) != write(fd, &ver, sizeof(ver)))
        {
            EPRINT(CONFIG_LOG,"Failed to write Server Parameter revision");
            close(fd);
            return FAIL;
        }

		if((INT32)sizeof(ServerParameters) != write(fd, &ServerParameters, sizeof(ServerParameters)) )
		{
			EPRINT(CONFIG_LOG, "Failed to write ServerParameters configuration");
			return FAIL;
		}
	}
	else
	{
		return FAIL;
    }
	ServerParameterConfigDebug();
    return SUCCESS;
}
//*****************************************************************************
//	LoadOldSavedWifiParaConfig()
//	Param:
//		IN : fd
//			 ver
//		OUT: NONE
//	Returns:If successful - SUCCESS
//			 Otherwise - FAILURE
//	Description:
//			Loads old basic system parameters
//*****************************************************************************
BOOL LoadOldSavedWifiParaConfig( INT32 fd, UINT8 ver)
{
  
  SAVED_WIFI_LIST_VER1_t savedwifiParameterVer1[MAX_SAVED_WIFI];
  SAVED_WIFI_LIST_VER2_t savedwifiParameterVer2[MAX_SAVED_WIFI];
  UINT8 idx = 0;
	if(lseek(fd, 1, SEEK_SET)!= -1)
	{

		// Read configuration
		switch(ver)
		{
			case 1:
				if( (MAX_SAVED_WIFI *(sizeof(SAVED_WIFI_LIST_VER1_t) )) != read(fd, &savedwifiParameterVer1, (MAX_SAVED_WIFI*sizeof(SAVED_WIFI_LIST_VER1_t)) ) )
				{
					EPRINT(CONFIG_LOG, "Failed to read saved wifi parameter configuration file Ver %d",ver);
					return FAIL;
				}
				break;
            case 2:
                if( (MAX_SAVED_WIFI *(sizeof(SAVED_WIFI_LIST_VER2_t) )) != read(fd, &savedwifiParameterVer2, (MAX_SAVED_WIFI*sizeof(SAVED_WIFI_LIST_VER2_t)) ) )
                {
                    EPRINT(CONFIG_LOG, "Failed to read saved wifi parameter configuration file Ver %d",ver);
                    return FAIL;
                }
                break;

			default:
				return FAIL;
		}
		
		if(ver == 1)
		{
			for(idx = 0 ; idx < MAX_SAVED_WIFI; idx++)
			{
                memcpy(&savedwifiParameterVer2[idx], &savedwifiParameterVer1[idx], sizeof(SAVED_WIFI_LIST_VER1_t) );
                savedwifiParameterVer2[idx].bandFreqType = WIFI_BAND_FREQ_2GHz;
			}
			ver = 2;
		}

        if(ver == 2)
        {
            for(idx = 0 ; idx < MAX_SAVED_WIFI; idx++)
            {
                memset(SavedWifiList[idx].ssid, 0, sizeof(SavedWifiList[idx].ssid));
                snprintf(SavedWifiList[idx].ssid, MAX_SSID_NAME_OLD, "%s", savedwifiParameterVer2[idx].ssid);
                SavedWifiList[idx].secType = savedwifiParameterVer2[idx].secType;
                SavedWifiList[idx].encType = savedwifiParameterVer2[idx].encType;
                snprintf(SavedWifiList[idx].passphresh, MAX_PASSPHRESH_SIZE, "%s", savedwifiParameterVer2[idx].passphresh);
                SavedWifiList[idx].advAuthType = savedwifiParameterVer2[idx].advAuthType;
                snprintf(SavedWifiList[idx].anonymousIdentity, MAX_WIFI_IDENTITY_SIZE, "%s", savedwifiParameterVer2[idx].anonymousIdentity);
                snprintf(SavedWifiList[idx].userName, MAX_WIFI_IDENTITY_SIZE, "%s", savedwifiParameterVer2[idx].userName);
                SavedWifiList[idx].peapVer = savedwifiParameterVer2[idx].peapVer;
                SavedWifiList[idx].innerAuth = savedwifiParameterVer2[idx].innerAuth;
                SavedWifiList[idx].keymgmt = savedwifiParameterVer2[idx].keymgmt;
                SavedWifiList[idx].lastModifiedTimeInSec = savedwifiParameterVer2[idx].lastModifiedTimeInSec;
                SavedWifiList[idx].bandFreqType = savedwifiParameterVer2[idx].bandFreqType;
            }
            ver = 3;
        }
		else
		{
			return FAIL;
		}
		
		if(lseek(fd, 0, SEEK_SET) != -1)
		{
			ver = SAVED_WIFI_LIST_VER;
			if(sizeof(ver) != write(fd, &ver, sizeof(ver) ) )
			{
				EPRINT(CONFIG_LOG, "Fail to write saved wifi parameter config version");
				return FAIL;
			}
			if( (MAX_SAVED_WIFI* sizeof(SAVED_WIFI_LIST_t) ) != write(fd, &SavedWifiList, (MAX_SAVED_WIFI*sizeof(SAVED_WIFI_LIST_t)) ) )
			{
				EPRINT(CONFIG_LOG, "Fail to write saved wifi parameter config");
				return FAIL;
			}
		}
		else
		{
			EPRINT(CONFIG_LOG, "Fail to write saved wifi parameter config LSEEK ");
			return FAIL;
		}
		return SUCCESS;
	}
	
	return FAIL;
  
}
