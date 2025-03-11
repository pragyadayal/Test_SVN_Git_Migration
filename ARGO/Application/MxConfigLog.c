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
//   Created By   : Manish Chandra
//   File         : MxConfigLog.c
//   Description  : This file consist of Configuration File I/O related Functions.
//
///////////////////////////////////////////////////////////////////////////

//******** Include files *******
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "MxConfigLog.h"
#include "MxConfig.h"
#include "MxTypedef.h"
#include "MxConfigFileIo.h"
#include "MxLogs.h"
#include "MxBlockDevice.h"
#include "MxConfigDef.h"
#include "MxOldConfigFileIo.h"
#include "MxSplFnScheduleConfig.h"
#ifdef PUSH_API_SUPPORTED
#include "MxPushApiConfig.h"
#include "MxPushApiEvent.h"
#endif
#include "MxUserConfig.h"

//******** Extern Variables **********
extern const CHARPTR					InputPortStr[MAX_INPUT_PORT];
extern const CHARPTR					ActiveInactiveStr[MAX_BOOL_ENUM];              //Active Inactive string
extern const CHARPTR					OutputPortStr[MAX_OUTPUT_PORT];					//Output port string
extern const CHARPTR					OutputTypeStr[MAX_OUTPUT_TYPE];					//Output type
extern const CHARPTR					DstStateStr[MAX_DST_STATE];						//DST state string
extern const CHARPTR					BlockedUserStr[MAX_BLOCK_REASON];
extern const CHARPTR					DstModeStr[MAX_DST_MODE];               		 //DST Mode
extern const CHARPTR					WeekOrdinalStr[MAX_WEEKORDINAL];
extern const CHARPTR					WeekDaysStr[MAX_WDAY];
extern const CHARPTR					MonthStr[MAX_MONTH];
extern const CHARPTR					CommPortStr[MAX_PORT];							//Communication port type
extern const CHARPTR					ServerConnectionStr[MAX_SERVER_CONN];			// Server connection
extern const CHARPTR					IpModeStr[MAX_IP_MODE];
extern const CHARPTR					SecTypeStr[MAX_SEC_TYPE];						//Sector type
extern const CHARPTR					EncryptionStr[MAX_ENCRYPTION];
extern const CHARPTR					WifiKeyMgmtStr[MAX_KEY_MGMT];
extern const CHARPTR					WifiAuthTypeStr[MAX_AUTH_TYPE];					//WIFI Authentication type
extern const CHARPTR					PeapVersionStr[MAX_PEAP_VER];					//Peap version(WIFI related version)
extern const CHARPTR					InnerAuthStr[MAX_INNER_AUTH];					//Inner Authentication
extern const CHARPTR					ModemTypeStr[MAX_MODEM_TYPE];					//Modem Type(GSM/CDMA)
extern const CHARPTR					CafeDiscountStr[CAFE_DISCNT_LEVEL_MAX];
extern const CHARPTR					PaymentModeStr[MAX_PAYMENT_MODE];
extern const CHARPTR 					BlockedForCafeStr[MAX_BLOCK_FOR_CAFE];
extern const CHARPTR					LightconditionStr[MAX_LIGHT_CONDITION];			//Light Condition
extern const CHARPTR					SensitivityLevelStr[MAX_SENSITIVITY_LEVEL];
extern const CHARPTR					ImageQualityStr[MAX_IMAGE_QUALITY];             //Image Quality
extern const CHARPTR					SecurityLevelStr[MAX_SECURITY_LEVEL];
extern const CHARPTR					FastModeStr[MAX_FAST_MODES];
extern const CHARPTR					PrinterTypeStr[MAX_PRINTER];					//Printer Type
extern const CHARPTR					PrinterCommTypStr[MAX_PRINTER_TYPE];			//Printer Communication Type
extern const CHARPTR					DvrConfigEventTypStr[MAX_DVR_CONFIG_EVENT_TYPE];//DVR Configuration event
extern const CHARPTR					ReaderModeStr[MAX_READER_MODE];					//Reader Mode
extern const CHARPTR					DvrActionTypeStr[MAX_DVR_ACTION_TYPE];
extern const CHARPTR					SmartRouteAccessTypStr[MAX_SMART_ROUTE_ACCESS_TYPE];
extern const CHARPTR					CardIdOrderStr[MAX_CARD_ID_ORDER_TYPE];
extern const CHARPTR					EnrollmentTypeStr[MAX_ENROLLMENT_TYPE];
extern const CHARPTR					TemplatePerFingureStr[MAX_TEMPLATES_PER_FINGER];
extern const CHARPTR					AccessModeStr[MAX_ACCESS_MODE];                  //Type of Access
extern const CHARPTR					TwoPersonModeStr[MAX_TWO_PERSON_MODE];
extern const CHARPTR					SmartRouteOrderTypeStr[MAX_SMART_ROUTE_ORDER];
extern const CHARPTR					FprTemplateFormatStr[MAX_FINGER_TEMPLATE_FORMAT];
extern const CHARPTR					DvrUserEventTypeStr[MAX_DVR_EVENT_USER];
extern const CHARPTR					TimeFormatStr[MAX_TIME_FORMAT];
extern const CHARPTR					ZoneApplTypStr[MAX_ZONE_APPL];                     //zone(TA,Access control and Cafeteria)mode
extern const CHARPTR					SenseTypStr[MAX_SENSE_TYPE];
extern const CHARPTR					NtpModeStr[MAX_NTP_MODE];               		   //Time mode(Auto time or Manual time)
extern const CHARPTR					ReaderTypeStr[MAX_READER_TYPE];
extern const CHARPTR 					UserTypStr[MAX_USER_TYPE];
extern const CHARPTR					VamCardTypeStr[MAX_VAM_CARD_TYPE];
extern const CHARPTR             		WPSTrigerTypeStr[MAX_WPS_TRIGER_TYP];
extern const CHARPTR  					KeypadStr[MAX_KEYPAD];
extern const CHARPTR 					DoorLockTypeStr[MAX_DOOR_LOCK_TYPE];
extern const CHARPTR					JobTypeStr[MAX_JCM_TYPE];
extern const CHARPTR					WiegandOutEventTypeStr[MAX_WIEGAND_EVENT_FORMAT];
extern const CHARPTR					WigandRedrModeStr[MAX_WIGND_RDR_MODE];
extern const CHARPTR					WigandOutFormtStr[MAX_WIGND_OUT_FRMT];
extern const CHARPTR					WigandOutDirStr[MAX_WIGND_OUT_DIR];
extern const CHARPTR					EnrollIdTypeStr[MAX_ENROLL_ID];

extern SYS_VAR_t						SysVar;
extern SYS_ADVANCE_PARA_t				SysAdvPara;
extern SYS_BASIC_PARA_t					SysBasicPara;
extern BLOCKED_USER_GROUP_t  			BlockedUser[MAX_BLOCKED_USER];
extern DST_CONFIG_t						DstConfig;
extern FIRST_IN_LIST_t       			FirstInUserGroup[MAX_FIRST_IN_LISTS];
extern HOLIDAY_SCHEDULE_t    			HolidaySchedule[MAX_HOLIDAY_SCHEDULE];
extern ETHERNET_PARA_t					EthernetParameters;
extern SERVER_PARA_t					ServerParameters;
extern WIFI_PARA_t						WifiParameters;
extern MODEM_CONFIG_t					ModemProfile[MAX_MODEM_PROFILE];
extern COMMAND_CTRL_CONFIG_t			CmdCtrlConfig;
extern PASSWORD_t						Password;
extern TWO_PERSON_GROUP_t   			TwoPersonGroup[MAX_TWO_PERSON_GROUP];
extern VISITOR_MANAGEMENT_t  			Visitor[MAX_VISITORS];
extern SPECIAL_FUNCTION_t 				SpecialFunctions[MAX_SPECIAL_FUNC];
extern TIME_TRIG_FUNC_t					TimeTrigFunc[MAX_TIME_TRIG_FUNC];
extern FPR_CONFIG_t						FprConfig;
extern READERS_PARAMETER_t				ReadersParaConfig;
extern IO_LINK_CONFIG_t					IoLinkConfig[MAX_IO_LINK];
extern IO_LINK_EXT_DEV_CONFIG_t			IoLinkExtDevConfig[MAX_IO_LINK_EXT_DEVICE];
extern IO_LINK_EXT_DEV_MAPPING_t		IoLinkExtDevMapping[MAX_IO_LINK];
extern CAFE_ITEM_CONFIG_t				CafeItemConfig[MAX_CAFE_ITEM_CODE];
extern CAFE_MENU_CONFIG_t				CafeMenuConfig[MAX_CAFE_MENU_CODE];
extern CAFE_MENU_SCHEDULE_t				CafeMenuSchConfig[MAX_CAFE_MENU_SCHEDULE];
extern PRINTER_CONFIG_t					PrinterConfig;
extern DVR_EVENT_ACTION_CONFIG_t		DvrEvtActionConfig[MAX_DVR_EVT_ACTN_CONFIG];
extern SMART_ROUTE_ACCESS_CONFIG_t		SmartRouteAccessConfig[MAX_SMART_ROUTE_ACCESS];
extern FIRST_IN_INFO_t					FirstInInfo;
extern FILE_LOG_PARA_CONFIG_t			FileLogPara;
extern WEB_API_EVENTS_CONFIG_t			WebAPIEventsConfig;
extern WEB_API_EVENTS_ACTIVITY_t		WebApiEventActivity;
extern CARD_FORMAT_CONFIG_t				CardFormatConf[MAX_CARD_FORMAT_PER_DOOR];
extern VAM_CONFIG_t						VamConfig[MAX_VAM_CONFIG];
extern JCM_CONFIG_t						JcmConfig[MAX_JCM_CONFIG+3]; //+3 for 'None', 'Default' and 'Continue' Job
extern DOOR_MODE_SCH_CONFIG_t			DoorModeSchCfg[MAX_DOOR_MODE_SCHEDULE];
extern SERVER_MODE_CONFIG_t				ServerModeCfg;
extern VYOM_SERVER_CONFIG_t				VyomServerPara;
extern BACKUP_AND_UPDATE_t				BackUpAndUpdate;
extern V11R3_SERVER_PARA_t	  			V11R3ServerParameters;
extern CONNECT_TO_PANEL_CONFIG_t		ConnectToPanelPara;
extern LICENSE_DONGLE_CONFIG_t			LicenseDongleCfg;
extern IP_CAMERA_CONFIG_t				IpCameraConfig;
extern BRIGHTNESS_DETAIL_t				DisplayBrightness;
extern SPECIAL_FN_SCHEDULE_t			SplFnSchedule[MAX_SPECIAL_FN_SCHEDULE];
#ifdef PUSH_API_SUPPORTED
extern PUSH_API_t						PushAPIConfig;
extern PUSH_API_EVENT_t PushApiEvent;
#endif
extern ACCESS_SCHEDULE_t			AccessScheduleConfig[MAX_TIME_FRAME];

//******** Function Definitions ******
//*****************************************************************************
//	IoLinkConfigDebug( )
//	Param:
//		IN : index			- IO Link index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes IO link index and print respected parameter of IO link.
//*****************************************************************************
void IoLinkConfigDebug(UINT8 index)
{
	UINT8	tmpIdx, tmpMaxIdx;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "IO Link Config Para");
		if(index >= MAX_IO_LINK)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_IO_LINK;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index+1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "IoLinkIndex[0-11][%d]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "Enable[%s]", ActiveInactiveStr[IoLinkConfig[tmpIdx].enable]);
			PRINTCNFG(CONFIG_LOG, "InputPort[%s]", InputPortStr[IoLinkConfig[tmpIdx].inputPort]);
			PRINTCNFG(CONFIG_LOG, "OutputPort[%s]", OutputPortStr[IoLinkConfig[tmpIdx].outputPort]);
			PRINTCNFG(CONFIG_LOG, "OutputType[%s]", OutputTypeStr[IoLinkConfig[tmpIdx].outputType]);
			PRINTCNFG(CONFIG_LOG, "PulseTime[0-255][%u]", IoLinkConfig[tmpIdx].pulseTime);
			PRINTCNFG(CONFIG_LOG, "Reset Time Flag[%d]", IoLinkConfig[tmpIdx].resetLinkF);
			PRINTCNFG(CONFIG_LOG, "Reset Time[HH-MM][%u:%u]", IoLinkConfig[tmpIdx].resetLinkTime.hour,IoLinkConfig[tmpIdx].resetLinkTime.minute);
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	SystemVariableConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected System Variable Configuration parameter.
//*****************************************************************************
void SystemVariableConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "System Variable Config Para");
		PRINTCNFG(CONFIG_LOG, "DST State[%s]", DstStateStr[SysVar.dstState]);
		PRINTCNFG(CONFIG_LOG, "BackgroundImage[%u]", SysVar.backgroundImage);
		PRINTCNFG(CONFIG_LOG, "CurrentClock[%u]", SysVar.currentClock);
		PRINTCNFG(CONFIG_LOG, "currentKeypad[%s]",KeypadStr[SysVar.currentKeypad] );
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	SystemAdvanceParaConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected System Advance Configuration parameter.
//*****************************************************************************
void SystemAdvanceParaConfigDebug(void)
{
	UINT8 tmpIdx;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "System Advance Config Para");
		PRINTCNFG(CONFIG_LOG, "AbsenteeRuleEnable[%s]", ActiveInactiveStr[SysAdvPara.absenteeRuleEnable]);
		//addtional security
		PRINTCNFG(CONFIG_LOG, "AddlSecurityEnable[%s]", ActiveInactiveStr[SysAdvPara.addlSecurity.enable]);
		PRINTCNFG(CONFIG_LOG, "AddlSecurityCode[%u]", SysAdvPara.addlSecurity.code);
		//user count
		PRINTCNFG(CONFIG_LOG, "UseCountEnable[%s]", ActiveInactiveStr[SysAdvPara.useCount.enable]);
		PRINTCNFG(CONFIG_LOG, "UserCount[%u]", SysAdvPara.useCount.count);
		//FirstInUser
		PRINTCNFG(CONFIG_LOG, "FirstInEnable[%s]", ActiveInactiveStr[SysAdvPara.firstIn.enable]);
		PRINTCNFG(CONFIG_LOG, "FirstInListId[%u]", SysAdvPara.firstIn.listId);
		//Two Person
		PRINTCNFG(CONFIG_LOG, "TwoPersonEnable[%s]", ActiveInactiveStr[SysAdvPara.twoPerson.enable]);
		PRINTCNFG(CONFIG_LOG, "TwoPersonGrpId1[%u]", SysAdvPara.twoPerson.grpId1);
		PRINTCNFG(CONFIG_LOG, "TwoPersonGrpId2[%u]", SysAdvPara.twoPerson.grpId2);
		//OldOccupancyCtrl
		PRINTCNFG(CONFIG_LOG, "OldOccupancyCtrlEnable[%s]", ActiveInactiveStr[SysAdvPara.oldOccupancyCtrl.enable]);
		PRINTCNFG(CONFIG_LOG, "OldOccupancyCtrlCount[%u]", SysAdvPara.oldOccupancyCtrl.count);
		//AntiPassBack
		PRINTCNFG(CONFIG_LOG, "AntiPassBackEntryEnable[%s]", ActiveInactiveStr[SysAdvPara.antiPassBack.enableEntry]);
		PRINTCNFG(CONFIG_LOG, "AntiPassBackExitEnable[%s]", ActiveInactiveStr[SysAdvPara.antiPassBack.enableExit]);
		PRINTCNFG(CONFIG_LOG, "AntiPassBackHardEnable[%s]", ActiveInactiveStr[SysAdvPara.antiPassBack.hard]);
		PRINTCNFG(CONFIG_LOG, "AntiPassBackForgivenessEnable[%s]", ActiveInactiveStr[SysAdvPara.antiPassBack.forgiveness]);
		//aux input
		PRINTCNFG(CONFIG_LOG, "AuxInputEnable[%s]", ActiveInactiveStr[SysAdvPara.auxInput.enable]);
		PRINTCNFG(CONFIG_LOG, "AuxInputSupervise[%s]", ActiveInactiveStr[SysAdvPara.auxInput.supervise]);
		PRINTCNFG(CONFIG_LOG, "AuxInputSense[%s]", SenseTypStr[SysAdvPara.auxInput.sense]);
		PRINTCNFG(CONFIG_LOG, "AuxInputDebounceTime[%u]", SysAdvPara.auxInput.debounceTime);
		//aux output
		PRINTCNFG(CONFIG_LOG, "AuxOutputEnable[%s]", ActiveInactiveStr[SysAdvPara.auxOutput.enable]);
		//Enrollment
		PRINTCNFG(CONFIG_LOG, "EnrollmentEnable[%s]", ActiveInactiveStr[SysAdvPara.enrollmentEnable]);
		PRINTCNFG(CONFIG_LOG, "EnrollmentType[%s]", EnrollmentTypeStr[SysAdvPara.enrollmentType]);
		PRINTCNFG(CONFIG_LOG, "FpCount[%u]", SysAdvPara.fpCount);
		PRINTCNFG(CONFIG_LOG, "CardCount[%u]", SysAdvPara.cardCount);
		PRINTCNFG(CONFIG_LOG, "MaxFingerPerUser[%u]", SysAdvPara.maxFingerPerUser);
		PRINTCNFG(CONFIG_LOG, "TemplatesPerFinger[%s]", TemplatePerFingureStr[SysAdvPara.templatesPerFinger]);
		//system timer
		PRINTCNFG(CONFIG_LOG, "TwoPersonTimer[%u]", SysAdvPara.twoPersonTimer);
		PRINTCNFG(CONFIG_LOG, "InterDigitTimer[%u]", SysAdvPara.interDigitTimer);
		PRINTCNFG(CONFIG_LOG, "MultiAccessTimer[%u]", SysAdvPara.multiAccessTimer);
		PRINTCNFG(CONFIG_LOG, "LateEarlyTimer[%u]", SysAdvPara.lateEarlyTimer);
		PRINTCNFG(CONFIG_LOG, "GenerateExitSwitchEnable[%s]", ActiveInactiveStr[SysAdvPara.genExitSwEvent]);
		PRINTCNFG(CONFIG_LOG, "GenerateInvalidUserEnable[%s]", ActiveInactiveStr[SysAdvPara.genInvalidUserEvent]);
		PRINTCNFG(CONFIG_LOG, "SSAConfigEnable[%s]", ActiveInactiveStr[SysAdvPara.SSAConfig.enable]);
		PRINTCNFG(CONFIG_LOG, "SSAConfigMode[%s]", AccessModeStr[SysAdvPara.SSAConfig.accessMode]);
		PRINTCNFG(CONFIG_LOG, "AccessLevel[%u]", SysAdvPara.SSAConfig.accessLevel);
		//DVR Intergration
		PRINTCNFG(CONFIG_LOG, "DvrIntigrationEnable[%s]", ActiveInactiveStr[SysAdvPara.dvrIntigrationEnbl]);
		PRINTCNFG(CONFIG_LOG, "DvrIpAddr[%s]", SysAdvPara.dvrIpAddr);
		PRINTCNFG(CONFIG_LOG, "DvrPort[%u]", SysAdvPara.dvrPort);
		PRINTCNFG(CONFIG_LOG, "DvrInterface[%s]", CommPortStr[SysAdvPara.dvrInterface]);
		// Smart route access related
		PRINTCNFG(CONFIG_LOG, "SmartRouteAccessEnable[%s]", ActiveInactiveStr[SysAdvPara.smartRouteAccessEnbl]);
		//greeting related
		PRINTCNFG(CONFIG_LOG, "GreetingsEnable[%s]", ActiveInactiveStr[SysAdvPara.enblGreetingsF]);
		for(tmpIdx = 0 ; tmpIdx < MAX_GREETING_CONFIG; tmpIdx++)
		{
			PRINTCNFG(CONFIG_LOG, "Index[%d] -> GreetingMessage[%s]", tmpIdx, SysAdvPara.greetingConfig[tmpIdx].message);
			TimeParameter("Greeting start ->", &(SysAdvPara.greetingConfig[tmpIdx].startTime));
			TimeParameter("Greeting end   ->", &(SysAdvPara.greetingConfig[tmpIdx].endTime));
		}
		PRINTCNFG(CONFIG_LOG, "TwoPersonMode[%s]", TwoPersonModeStr[SysAdvPara.twoPersonMode]);
		PRINTCNFG(CONFIG_LOG, "AutoAlarmAckEnable[%s]", ActiveInactiveStr[SysAdvPara.autoAlarmAck]);
		PRINTCNFG(CONFIG_LOG, "AutoAlarmAckTimer[%u]", SysAdvPara.autoAlarmAckTimer);
		//Mifare card
		PRINTCNFG(CONFIG_LOG, "MifareCustomKeyEnable[%s]", ActiveInactiveStr[SysAdvPara.mifareCustomKeyEnbl]);
		PRINTCNFG(CONFIG_LOG, "MifareCustomKey[%02x %02x %02x %02x %02x %02x]", SysAdvPara.mifareCustomKey[0],SysAdvPara.mifareCustomKey[1],SysAdvPara.mifareCustomKey[2],SysAdvPara.mifareCustomKey[3],SysAdvPara.mifareCustomKey[4],SysAdvPara.mifareCustomKey[5]);
		//HId card
		PRINTCNFG(CONFIG_LOG, "HidIclasCustomKeyEnbl[%s]", ActiveInactiveStr[SysAdvPara.hidIclasCustomKeyEnbl]);
		PRINTCNFG(CONFIG_LOG, "HidIclasCustomKey[%02x %02x %02x %02x %02x %02x %02x %02x ]", SysAdvPara.hidIclasCustomKey[0],SysAdvPara.hidIclasCustomKey[1],SysAdvPara.hidIclasCustomKey[2],SysAdvPara.hidIclasCustomKey[3],SysAdvPara.hidIclasCustomKey[4],SysAdvPara.hidIclasCustomKey[5],SysAdvPara.hidIclasCustomKey[6],SysAdvPara.hidIclasCustomKey[7]);
		PRINTCNFG(CONFIG_LOG, "CustomKeyAutoUpdateEnable[%s]", ActiveInactiveStr[SysAdvPara.customKeyAutoUpdateF]);
		PRINTCNFG(CONFIG_LOG, "SmartRouteType[%s]", SmartRouteOrderTypeStr[SysAdvPara.smartRouteOrder]);
		PRINTCNFG(CONFIG_LOG, "VerifyAccessPolicyOnExitFEnable[%s]", ActiveInactiveStr[SysAdvPara.verifyAccessPolicyOnExitF]);
		PRINTCNFG(CONFIG_LOG, "AcceptExtIOLinkFEnbl[%s]", ActiveInactiveStr[SysAdvPara.acceptExtIOLinkF]);
		PRINTCNFG(CONFIG_LOG, "OutputWaitTime[%u]", SysAdvPara.outputWaitTime);
		PRINTCNFG(CONFIG_LOG, "WebEncryptionModeFFEnable[%s]", ActiveInactiveStr[SysAdvPara.webEncryptionModeF]);
		PRINTCNFG(CONFIG_LOG, "HTTPPort[%d]", SysAdvPara.httpPort);
		PRINTCNFG(CONFIG_LOG, "HTTPsPort[%d]", SysAdvPara.httpsPort);
		PRINTCNFG(CONFIG_LOG, "FacilityCode[%u]", SysAdvPara.facilityCode);
		PRINTCNFG(CONFIG_LOG, "DoorLockType[%s]", DoorLockTypeStr[SysAdvPara.lockType]);
		PRINTCNFG(CONFIG_LOG, "SelfEnrlEnable[%s]", ActiveInactiveStr[SysAdvPara.selfEnrlEnbl]);
		PRINTCNFG(CONFIG_LOG, "selfEnrlRetryCnt[%u]", SysAdvPara.selfEnrlRetryCnt);
		PRINTCNFG(CONFIG_LOG, "VamEnable[%s]", ActiveInactiveStr[SysAdvPara.vamEnable]);
		//occupancy control
		PRINTCNFG(CONFIG_LOG, "OccupancyCtrlEnable[%s]", ActiveInactiveStr[SysAdvPara.occupancyCtrl.enable]);
		PRINTCNFG(CONFIG_LOG, "OccupancyCtrlCnt[%u]", SysAdvPara.occupancyCtrl.count);
		PRINTCNFG(CONFIG_LOG, "OccupancyCtrlMinUsers[%u]", SysAdvPara.occupancyCtrl.minUsers);
		PRINTCNFG(CONFIG_LOG, "ZeroOccupancyEnable[%s]", ActiveInactiveStr[SysAdvPara.occupancyCtrl.zeroOccupancy]);
		//DVR Allow/Deny Event
		PRINTCNFG(CONFIG_LOG, "DvrUserAllowEventType[%s]", DvrUserEventTypeStr[SysAdvPara.dvrAllowEventuserType]);
		PRINTCNFG(CONFIG_LOG, "DvrUserDenyEventType[%s]", DvrUserEventTypeStr[SysAdvPara.dvrDenyEventUserType]);
		PRINTCNFG(CONFIG_LOG, "JcmEnable[%s]", ActiveInactiveStr[SysAdvPara.jcmEnblF]);
		PRINTCNFG(CONFIG_LOG, "Inter Transaction Delay[%u]", SysAdvPara.cafeInterTransactionDelay);
		PRINTCNFG(CONFIG_LOG, "First In Lock Reset Type[%u]", SysAdvPara.firstInResetAfter);
		PRINTCNFG(CONFIG_LOG, "First In Lock Reset Timer[%u]", SysAdvPara.firstInTimer);
		PRINTCNFG(CONFIG_LOG, "EnrollIdType[%s]", EnrollIdTypeStr[SysAdvPara.enrollIdType]);
		PRINTCNFG(CONFIG_LOG, "two Credential Must Mode Enable[%u]", SysAdvPara.twoCredMustModeF);
		PRINTCNFG(CONFIG_LOG, "sequentialInOut Enable[%u]", SysAdvPara.sequentialInOut);
		PRINTCNFG(CONFIG_LOG, "connectToPanel Enable[%u]", SysAdvPara.connectToPanel);
		PRINTCNFG(CONFIG_LOG, "connectToPanel panelIpAddr[%s]", SysAdvPara.panelIpAddr);
		PRINTCNFG(CONFIG_LOG, "connectToPanel panel port[%u]", SysAdvPara.panelPort);
		PRINTCNFG(CONFIG_LOG, "DesfireCustomKeyEnable[%s]", ActiveInactiveStr[SysAdvPara.desfireCustomKeyEnbl]);
		PRINTCNFG(CONFIG_LOG, "MifareCustomKey[%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x]",
				SysAdvPara.desfireCustomKey[0],SysAdvPara.desfireCustomKey[1],SysAdvPara.desfireCustomKey[2],SysAdvPara.desfireCustomKey[3],
				SysAdvPara.desfireCustomKey[4],SysAdvPara.desfireCustomKey[5],SysAdvPara.desfireCustomKey[6],SysAdvPara.desfireCustomKey[7],
				SysAdvPara.desfireCustomKey[8],SysAdvPara.desfireCustomKey[9],SysAdvPara.desfireCustomKey[10],SysAdvPara.desfireCustomKey[11],
				SysAdvPara.desfireCustomKey[12],SysAdvPara.desfireCustomKey[13],SysAdvPara.desfireCustomKey[14],SysAdvPara.desfireCustomKey[15]);
		PRINTCNFG(CONFIG_LOG, "Show Pin[%d]", SysAdvPara.showPin);
		PRINTCNFG(CONFIG_LOG, "APB Reset After[%d]",  SysAdvPara.antiPassBack.resetAfter);
		PRINTCNFG(CONFIG_LOG, "APB Forgiveness Timer[%d]",  SysAdvPara.antiPassBack.forgivenessTimer);
		PRINTCNFG(CONFIG_LOG, "birthdayMsg %s", SysAdvPara.birthdayMessage);
		PRINTCNFG(CONFIG_LOG, "MJPEGStreamAPI [%s]", SysAdvPara.MJPEGStreamAPI);
		PRINTCNFG(CONFIG_LOG, "retainJobSelection[%s]", ActiveInactiveStr[SysAdvPara.retainJobSelectionF]);
		PRINTCNFG(CONFIG_LOG, "saveFpImageF[%u]", SysAdvPara.saveFpImageF);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor Enable [%u]", SysAdvPara.temperatureLogCnfg.isEnable);
		PRINTCNFG(CONFIG_LOG, "Temperature Detection Unit [%u]", SysAdvPara.temperatureLogCnfg.tempUnit);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor Type [%u]", SysAdvPara.temperatureLogCnfg.sensorType);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor Interface [%u]", SysAdvPara.temperatureLogCnfg.sensorInterface);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor Emissivity [%.2f]", SysAdvPara.temperatureLogCnfg.emissivity);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor Restiction Type [%u]", SysAdvPara.temperatureLogCnfg.restrictionType);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor By Pass On Disconnect [%u]", SysAdvPara.temperatureLogCnfg.bypassOnDisconnect);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor Threshold Temperature [%.2f]", SysAdvPara.temperatureLogCnfg.thresholdTemp);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor Calibration Para [%.2f]", SysAdvPara.temperatureLogCnfg.calibrationPara);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor approachWaitTmr [%.2f]", SysAdvPara.temperatureLogCnfg.approachWaitTmr);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor tempDetectionTimer [%d]", SysAdvPara.temperatureLogCnfg.tempDetectionTimer);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor validSampleCnt [%d]", SysAdvPara.temperatureLogCnfg.validSampleCnt);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor thresholdDeviation [%.2f]", SysAdvPara.temperatureLogCnfg.thresholdDeviation);
		PRINTCNFG(CONFIG_LOG, "Temperature Sensor minimumThreshold [%.2f]", SysAdvPara.temperatureLogCnfg.minimumThreshold);
		PRINTCNFG(CONFIG_LOG, "maxFacePerUser[%u]", SysAdvPara.maxFacePerUser);
		PRINTCNFG(CONFIG_LOG, "Face Mask Compulsion - Enable [%u]", SysAdvPara.faceMaskComp_Enable);
		PRINTCNFG(CONFIG_LOG, "Identification Face Mask Detection Threshold [%.2f]", SysAdvPara.faceMaskComp_IdtFaceMaskDetectionThreshold);
		PRINTCNFG(CONFIG_LOG, "Visible Face For Enrollment Threshold [%.2f]", SysAdvPara.faceMaskComp_VisibleFaceForEnrl);

		PRINTCNFG(CONFIG_LOG, "Approach to Camera Wait-Timer [%.2f]", SysAdvPara.faceMaskComp_ApproachCameraWaitTimer);
		PRINTCNFG(CONFIG_LOG, "Mask Detection Time-Out [%d]", SysAdvPara.faceMaskComp_MaskDetectionTimeOut);
		PRINTCNFG(CONFIG_LOG, "Restriction Type [%d]", SysAdvPara.faceMaskComp_RestrictionType);
		PRINTCNFG(CONFIG_LOG, "Allowed TLS Version [%u]", SysAdvPara.alwTLSVer);
		PRINTCNFG(CONFIG_LOG, "Password Policy [%s]", ActiveInactiveStr[SysAdvPara.passwordPolicyF]);

		PRINTCNFG(CONFIG_LOG, "maxFacePerUser[%d]", SysAdvPara.maxFacePerUser);
//		PRINTCNFG(CONFIG_LOG, "displayDistinctiveFeedbackF[%d]", SysAdvPara.displayDistinctiveFeedbackF);
//		PRINTCNFG(CONFIG_LOG, "InPunchColor[%d]", SysAdvPara.InPunchColor);
//		PRINTCNFG(CONFIG_LOG, "OutPunchColor[%d]", SysAdvPara.OutPunchColor);
		PRINTCNFG(CONFIG_LOG, "Duress Detection [%s]", ActiveInactiveStr[SysAdvPara.duressDetection]);
		PRINTCNFG(CONFIG_LOG, "Facility Code Enable [%s]", ActiveInactiveStr[SysAdvPara.facilityCodeCheckEnableF]);
		//Temporary User Config
		if(SysAdvPara.temporaryUserGenerateInfo.autoAddTempUser == 1)
		{
		 PRINTCNFG(CONFIG_LOG, "Auto Add Temporary User [%d]",SysAdvPara.temporaryUserGenerateInfo.autoAddTempUser);
		}
		else if(SysAdvPara.temporaryUserGenerateInfo.autoAddTempUser == 2)
		{
		 PRINTCNFG(CONFIG_LOG, "Auto Add Temporary Worker [%d]", SysAdvPara.temporaryUserGenerateInfo.autoAddTempUser);
		}
		else
		{
		  PRINTCNFG(CONFIG_LOG, "Auto Add Temporary feature disabled[%d]", SysAdvPara.temporaryUserGenerateInfo.autoAddTempUser);
		}
		PRINTCNFG(CONFIG_LOG, "Auto Add Temporary User Name Via Device Flag [%s]", ActiveInactiveStr[SysAdvPara.temporaryUserGenerateInfo.addNameViaDeviceF]);
		PRINTCNFG(CONFIG_LOG, "Auto Add Temporary User Confirm Flag [%s]", ActiveInactiveStr[SysAdvPara.temporaryUserGenerateInfo.confirmReqF]);
        PRINTCNFG(CONFIG_LOG, "Authentication Type [%u]", SysAdvPara.authenticationType);
        PRINTCNFG(CONFIG_LOG, "Authentication Algorithm [%u]", SysAdvPara.authenticationAlgorithm);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	SysBasicParaConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected System Basic Configuration parameter.
//*****************************************************************************
void SysBasicParaConfigDebug(void)
{
	UINT8 tmpIdx;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "System Basic Config Para");
		//General
		PRINTCNFG(CONFIG_LOG, "Device Id[%u]", SysBasicPara.devId);
		PRINTCNFG(CONFIG_LOG, "Name[%s]", SysBasicPara.name);
		PRINTCNFG(CONFIG_LOG, "TimeFormat[%s]", TimeFormatStr[SysBasicPara.timeFormat]);
		PRINTCNFG(CONFIG_LOG, "DoorAccessMode[%s]", AccessModeStr[SysBasicPara.doorAccessMode]);
		PRINTCNFG(CONFIG_LOG, "Application[%s]", ZoneApplTypStr[SysBasicPara.application]);
		PRINTCNFG(CONFIG_LOG, "ExitOnLockEnable[%s]", ActiveInactiveStr[SysBasicPara.exitOnLock]);
		PRINTCNFG(CONFIG_LOG, "TamperAlarmEnable[%s]", ActiveInactiveStr[SysBasicPara.tamperAlarm]);
		PRINTCNFG(CONFIG_LOG, "DoorSenseEnable[%s]",ActiveInactiveStr[SysBasicPara.doorSense.enable]);
		PRINTCNFG(CONFIG_LOG, "DoorSenseSuperviseEnable[%s]", ActiveInactiveStr[SysBasicPara.doorSense.supervise]);
		PRINTCNFG(CONFIG_LOG, "DoorSenseSuperviseEnable[%s]", SenseTypStr[SysBasicPara.doorSense.type]);
		PRINTCNFG(CONFIG_LOG, "DoorPulseTime[%u]", SysBasicPara.doorPulseTime);
		PRINTCNFG(CONFIG_LOG, "DoorPulseTimeFloat[%0.1f]", SysBasicPara.doorPulseTimeFloat);
		PRINTCNFG(CONFIG_LOG, "AutoRelockEnable[%s]", ActiveInactiveStr[SysBasicPara.autoRelock]);
		PRINTCNFG(CONFIG_LOG, "AutoRelockTime[%u]", SysBasicPara.autoRelockTime);
		//TimeZone
		PRINTCNFG(CONFIG_LOG, "UserDefinedNtpEnable[%s]", ActiveInactiveStr[SysBasicPara.userDefNtp]);
		PRINTCNFG(CONFIG_LOG, "PreDefinedNtpAddr[%s]", SysBasicPara.preDefNtpAddr);
		PRINTCNFG(CONFIG_LOG, "UserDefinedNtpAddr[%s]", SysBasicPara.userDefNtpAddr);
		PRINTCNFG(CONFIG_LOG, "UniversalTimeZone[%u]", SysBasicPara.utz);
		PRINTCNFG(CONFIG_LOG, "TimeUpdateMode[%s]", NtpModeStr[SysBasicPara.timeUpdateMode]);
		TimeParameter("WorkingHourStart ->" , &(SysBasicPara.workingHrStart));
		TimeParameter("WorkingHourStop  ->" , &(SysBasicPara.workingHrStop));
		for(tmpIdx = 0; tmpIdx < MAX_WDAY; tmpIdx++)
		{
			PRINTCNFG(CONFIG_LOG, "[%s] WorkingEnable[%s]", WeekDaysStr[tmpIdx], ActiveInactiveStr[SysBasicPara.workingDays[tmpIdx] ] );
		}
		PRINTCNFG(CONFIG_LOG, "HolidayWorkingEnable[%s]", ActiveInactiveStr[SysBasicPara.holidayWorking]);
		//reader
		for(tmpIdx = 0; tmpIdx < MAX_READERS; tmpIdx++)
		{
			PRINTCNFG(CONFIG_LOG, "[Reader-%u] ReaderType[%s] ReaderMode[%s]", tmpIdx, ReaderTypeStr[SysBasicPara.reader[tmpIdx].type], ReaderModeStr[SysBasicPara.reader[tmpIdx].mode] );
		}
		PRINTCNFG(CONFIG_LOG, "CerPresentEnable[%s]", ActiveInactiveStr[SysBasicPara.cerPresentF]);
		PRINTCNFG(CONFIG_LOG, "RequestExitSwitchEnable[%s]", ActiveInactiveStr[SysBasicPara.requestExitSw]);
		PRINTCNFG(CONFIG_LOG, "BuzMuteEnable[%s]", ActiveInactiveStr[SysBasicPara.buzMuteF]);
		PRINTCNFG(CONFIG_LOG, "TagRedetectDelay[%u]", SysBasicPara.tagRedetectDelay);
		PRINTCNFG(CONFIG_LOG, "CdcExitReaderEnable[%s]", ActiveInactiveStr[SysBasicPara.cdcExitReaderF]);
		PRINTCNFG(CONFIG_LOG, "ExternalReaderAccessModeEnable[%s]", AccessModeStr[SysBasicPara.extRdrAccessMode]);
		PRINTCNFG(CONFIG_LOG, "DegradeModeEnable[%s]", ActiveInactiveStr[SysBasicPara.degradeModeEnable]);
		PRINTCNFG(CONFIG_LOG, "DegradeWaitTimer[%u]", SysBasicPara.degradeWaitTimer);
		PRINTCNFG(CONFIG_LOG, "DoorModeSchdlEnblF[%s]", ActiveInactiveStr[SysBasicPara.doorModeSchdlEnblF]);
		PRINTCNFG(CONFIG_LOG, "FprTemplateFormat[%s]", FprTemplateFormatStr[SysBasicPara.fprTemplateFormat]);
		PRINTCNFG(CONFIG_LOG, "ExtWiegandRdrMode[%s]", WigandRedrModeStr[SysBasicPara.extWiegandRdrMode]);
		PRINTCNFG(CONFIG_LOG, "WiegandOutFormat[%s]", WigandOutFormtStr[SysBasicPara.wiegandOutFormat]);
		PRINTCNFG(CONFIG_LOG, "WiegandOutDir[%s]", WigandOutDirStr[SysBasicPara.wiegandOutDir]);
		PRINTCNFG(CONFIG_LOG, "WaitForAccessFeedback[%s]", ActiveInactiveStr[SysBasicPara.waitForAccessFeedback]);
		PRINTCNFG(CONFIG_LOG, "AccessFeedbackWaitTime[%u]", SysBasicPara.accessFeedbackWaitTime);
		PRINTCNFG(CONFIG_LOG, "WaitForUserVerification[%u]", SysBasicPara.waitForUsrVerification);
		for(tmpIdx = 0; tmpIdx < MAX_WIEGAND_EVENT_FORMAT; tmpIdx++)
		{
			PRINTCNFG(CONFIG_LOG, "wgndOutCstmFrmtNo[%s] --> [%u]", WiegandOutEventTypeStr[tmpIdx], SysBasicPara.wgndOutCstmFrmtNo[tmpIdx] );
		}
		for(tmpIdx = 0; tmpIdx < MAX_WIEGAND_EVENT_FORMAT; tmpIdx++)
		{
			PRINTCNFG(CONFIG_LOG, "wgndOutEventSpclCode[%s] --> [%u]", WiegandOutEventTypeStr[tmpIdx], SysBasicPara.wgndOutEventSpclCode[tmpIdx] );
		}
		PRINTCNFG(CONFIG_LOG, "IoLinkNwInterface[%s]", CommPortStr[SysBasicPara.IoLinkNwInterface] );
		PRINTCNFG(CONFIG_LOG, "apiAccessAllowEnable[%u]", SysBasicPara.apiAccessAllowEnable);
		PRINTCNFG(CONFIG_LOG, "apiEntryAccessMode[%u]", SysBasicPara.apiEntryAccessMode);
		PRINTCNFG(CONFIG_LOG, "apiExitAccessMode[%u]", SysBasicPara.apiExitAccessMode);
		PRINTCNFG(CONFIG_LOG, "apiSecurityKey[%s]", SysBasicPara.apiSecurityKey);
		PRINTCNFG(CONFIG_LOG, "visitorDoorAccessMode[%s]", AccessModeStr[SysBasicPara.visitorDoorAccessMode]);
		PRINTCNFG(CONFIG_LOG, "visitorExtRdrAccessMode[%s]", AccessModeStr[SysBasicPara.visitorExtRdrAccessMode]);
		PRINTCNFG(CONFIG_LOG, "pinChangeCode[%s]", SysBasicPara.pinChangeCode);
		PRINTCNFG(CONFIG_LOG, "promptSpFnF[%u]", SysBasicPara.promptSpFnF);
		PRINTCNFG(CONFIG_LOG, "generateEventF[%u]", SysBasicPara.generateEventF);
		PRINTCNFG(CONFIG_LOG, "CbrPresentEnable[%s]", ActiveInactiveStr[SysBasicPara.cbrPresentF]);
		PRINTCNFG(CONFIG_LOG, "Access Allow cad duration[%u]", SysBasicPara.accessAllowCadDuration );
		PRINTCNFG(CONFIG_LOG, "Access Allow screen duration[%u]", SysBasicPara.accessAllowScreenDuration );
		PRINTCNFG(CONFIG_LOG, "Access Deny cad duration[%u]", SysBasicPara.accessDenyCadDuration );
		PRINTCNFG(CONFIG_LOG, "Access Deny screen duration[%u]", SysBasicPara.accessDenyScreenDuration );
		PRINTCNFG(CONFIG_LOG, "AbnormalAlarmEnable[%s]", ActiveInactiveStr[SysBasicPara.abnormalAlarm]);
		PRINTCNFG(CONFIG_LOG, "ForceOpenAlarmEnable[%s]", ActiveInactiveStr[SysBasicPara.forceOpenAlarm]);
		PRINTCNFG(CONFIG_LOG, "FaultAlarmEnable[%s]", ActiveInactiveStr[SysBasicPara.faultAlarm]);
		PRINTCNFG(CONFIG_LOG, "PanicAlarmEnable[%s]", ActiveInactiveStr[SysBasicPara.panicAlarm]);
		PRINTCNFG(CONFIG_LOG, "Cafe FR Access Mode [%u]", SysBasicPara.cafeteriaFrAccessMode);
		PRINTCNFG(CONFIG_LOG, "Temperature Threshold Alaram [%u]", SysBasicPara.tempThresholdAlarm );
		PRINTCNFG(CONFIG_LOG, "Face Mask Not Detected Alaram [%u]", SysBasicPara.faceMaskNotDetectedAlarm);
		PRINTCNFG(CONFIG_LOG, "atomRD100PresentF[%d]", SysBasicPara.atomRD100PresentF);
		PRINTCNFG(CONFIG_LOG, "atomRD200PresentF[%d]", SysBasicPara.atomRD200PresentF);
		PRINTCNFG(CONFIG_LOG, "atomRD300PresentF[%d]", SysBasicPara.atomRD300PresentF);
		PRINTCNFG(CONFIG_LOG, "degradeModeType[%u]", SysBasicPara.degradeModeType);
		PRINTCNFG(CONFIG_LOG, "accessScheduleID[%u]", SysBasicPara.accessScheduleID);
		PRINTCNFG(CONFIG_LOG, "accessScheduleEnableF[%d]", SysBasicPara.accessScheduleEnableF);
		PRINTCNFG(CONFIG_LOG, "hideMenuBarForAccessScheduleF[%d]", SysBasicPara.hideMenuBarForAccessScheduleF);

		PRINTCNFG(CONFIG_LOG, "httpEncryptionModeF[%d]", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF);
        PRINTCNFG(CONFIG_LOG, "interfaceSelctionF[%d]", SysBasicPara.httpWebServerConfig.httpSyncConfig.interfaceSelctionF);
        if(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface >= 0 && SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface < INVALID_PORT)
            PRINTCNFG(CONFIG_LOG, "HttpInterface[%d]", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface);
        PRINTCNFG(CONFIG_LOG, "HttpServerIpAddress[%s]", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress);
        PRINTCNFG(CONFIG_LOG, "httpServerPort[%u]", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort);
        PRINTCNFG(CONFIG_LOG, "httpServerDirectory[%s]", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerDirectory);
        PRINTCNFG(CONFIG_LOG, "httpServerHostName[%s]", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName);
        PRINTCNFG(CONFIG_LOG, "httpServerUserId[%s]", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId);
        PRINTCNFG(CONFIG_LOG, "httpServerPassword[%s]", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword);

		PRINTCNFG(CONFIG_LOG, "httpEncryptionModeF[%d]", SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpEncryptionModeF);
        PRINTCNFG(CONFIG_LOG, "interfaceSelctionF[%d]", SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.interfaceSelctionF);
        if(SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpInterface >= 0 && SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpInterface < INVALID_PORT)
            PRINTCNFG(CONFIG_LOG, "HttpInterface[%d]", SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpInterface);
        PRINTCNFG(CONFIG_LOG, "HttpServerIpAddress[%s]", SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerIpAddress);
        PRINTCNFG(CONFIG_LOG, "httpServerPort[%u]", SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPort);
        PRINTCNFG(CONFIG_LOG, "httpServerDirectory[%s]", SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerDirectory);
        PRINTCNFG(CONFIG_LOG, "httpServerHostName[%s]", SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerHostName);
        PRINTCNFG(CONFIG_LOG, "httpServerUserId[%s]", SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerUserId);
        PRINTCNFG(CONFIG_LOG, "httpServerPassword[%s]", SysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPassword);
        PRINTCNFG(CONFIG_LOG, "httpSyncRequestRetryTimer[%d]", SysBasicPara.httpWebServerConfig.httpSyncRequestRetryTimer);
		
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	SystemAdvanceParaConfigDebug( )
//	Param:
//		IN : start			- start index to read
//			 pg				- Page to be read
//			 length	        - count to be read
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes Blocked User index and print respected parameter of Blocked User.
//*****************************************************************************
void BlockedUserConfigDebug(UINT8 pg, UINT8 start, UINT8 length)
{
	UINT8	tmpIdx;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, " Blocked User Config Para for page[%u]", pg);
		for(tmpIdx = start; (tmpIdx < (start + length) && tmpIdx < MAX_BLOCKED_USER); tmpIdx++)
		{
			PRINTCNFG(CONFIG_LOG, "BlockedUserIndex[0-98][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "UserIndex[%u] , UserName[%s] , TimeStamp[%ld]", BlockedUser[tmpIdx].userIdx, BlockedUser[tmpIdx].userName, BlockedUser[tmpIdx].date);
			PRINTCNFG(CONFIG_LOG, "Reason[%s]", BlockedUserStr[BlockedUser[tmpIdx].reason]);
			PRINTCNFG(CONFIG_LOG, " ");
		}
	}
}

//*****************************************************************************
//	DayLightSavingConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Day Light Saving Configuration parameter.
//*****************************************************************************
void DayLightSavingConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Day Light Saving Config Para");
		PRINTCNFG(CONFIG_LOG, "Mode[%s]", DstModeStr[DstConfig.mode]);
		//forward
		PRINTCNFG(CONFIG_LOG, "ForwardWeekOrdinal[%s]", WeekOrdinalStr[DstConfig.forward.dayWise.weekOrdinal] );
		PRINTCNFG(CONFIG_LOG, "ForwardWeekDay[%s]", WeekDaysStr[DstConfig.forward.dayWise.weekDay] );
		PRINTCNFG(CONFIG_LOG, "ForwardMonth[%s]", MonthStr[DstConfig.forward.dayWise.month]);
		PRINTCNFG(CONFIG_LOG, "ForwardDay[%u]", DstConfig.forward.dateWise.day);
		PRINTCNFG(CONFIG_LOG, "ForwardMonth[%u]", DstConfig.forward.dateWise.month);
		TimeParameter("ForwardTime ->" , &(DstConfig.forward.time));
		//backword
		PRINTCNFG(CONFIG_LOG, "BackwardWeekOrdinal[%s]", WeekOrdinalStr[DstConfig.backward.dayWise.weekOrdinal]);
		PRINTCNFG(CONFIG_LOG, "BackwardWeekDays[%s]", WeekDaysStr[DstConfig.backward.dayWise.weekDay]);
		PRINTCNFG(CONFIG_LOG, "BackwardMonth[%s]", MonthStr[DstConfig.backward.dayWise.month]);
		PRINTCNFG(CONFIG_LOG, "BackwardDay[%u]", DstConfig.backward.dateWise.day);
		PRINTCNFG(CONFIG_LOG, "BackwardMonth[%u]", DstConfig.backward.dateWise.month);
		TimeParameter("BackwardTime  ->" , &(DstConfig.backward.time));
		TimeParameter("DstTimePeriod ->" , &(DstConfig.timePeriod));
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	FirstInUserGroupConfigDebug( )
//	Param:
//		IN : index			- First In User Group index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes FirstInUserGroup index and print respected parameter of FirstInGroup User.
//*****************************************************************************
void FirstInUserGroupConfigDebug(UINT8 index)
{
    UINT32	tmpIdx, tmpMaxIdx,tmpcnt;

	DPRINT(CONFIG_LOG, "First User In Group Config Para");
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		if(index >= MAX_FIRST_IN_LISTS)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_FIRST_IN_LISTS;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index+1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "FirstInUserGroupIndex[0-3][%d]",tmpIdx);
			for(tmpcnt = 0; tmpcnt < MAX_FIRST_IN_MEMBERS; tmpcnt++)
			{
				PRINTCNFG(CONFIG_LOG, "memberIdx[%d]->UserId[%u]", tmpcnt, FirstInUserGroup[tmpIdx].members[tmpcnt]);
			}
			tmpIdx++;
			PRINTCNFG(CONFIG_LOG, " ");
		}
	}
}

//*****************************************************************************
//	DayParameter( )
//	Param:
//		IN : Detail	- Day,Month and Year detail from which this function was called
//		OUT:
//	Returns:None
//
//	Description:
//			This function print respected Detail of Day,Month and Year.
//*****************************************************************************
void DayParameter(CHARPTR preStrPtr,DATE_DDMMYY_t *Detail)
{
	PRINTCNFG(CONFIG_LOG, "%s day[%u] month[%u] year[%u]" , preStrPtr , Detail->day ,Detail->month ,Detail->year);
}

//*****************************************************************************
//	HolidayScheduleConfigDebug( )
//	Param:
//		IN : index			- Holiday Schedule index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes HolidaySchedule index and print respected parameter of HolidaySchedule.
//*****************************************************************************
void HolidayScheduleConfigDebug(UINT8 index)
{
	UINT8	tmpIdx, tmpMaxIdx,tmpcnt;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Holiday Schedule Config Para");
		if(index >= MAX_HOLIDAYS)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_HOLIDAYS;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index + 1;
		}

		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "HolidayScheduleIndex[0-3][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "HolidayScheduleId[%u]", HolidaySchedule[tmpIdx].scheduleId);
			for(tmpcnt=0 ; tmpcnt < MAX_HOLIDAYS ; tmpcnt++)
			{
				PRINTCNFG(CONFIG_LOG, "Index[%u]->  HolidayScheduleName[%s]", tmpIdx, HolidaySchedule[tmpIdx].holidays[tmpcnt].name);
				DayParameter("HolidayStartDate ->" , &(HolidaySchedule[tmpIdx].holidays[tmpcnt].startDate));
				DayParameter("HolidayEndDate   ->" , &(HolidaySchedule[tmpIdx].holidays[tmpcnt].endDate));
			}
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	EthernetParameterConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Ethernet Configuration parameter.
//*****************************************************************************
void EthernetParameterConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Ethernet Parameter Config Para");
		PRINTCNFG(CONFIG_LOG, "IpAddress[%s]", EthernetParameters.ipAddress);
		PRINTCNFG(CONFIG_LOG, "Subnet[%s]", EthernetParameters.subnet);
		PRINTCNFG(CONFIG_LOG, "gateway[%s]", EthernetParameters.gateway);
		PRINTCNFG(CONFIG_LOG, "MacAddress[%s]", EthernetParameters.macAddress);
		PRINTCNFG(CONFIG_LOG, "Dns1[%s]", EthernetParameters.dns1);
		PRINTCNFG(CONFIG_LOG, "Dns2[%s]", EthernetParameters.dns2);
		PRINTCNFG(CONFIG_LOG, "NwType[%d]", EthernetParameters.nwType);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	V11R3ServerParameterConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected V11R3 Server Configuration parameter.
//*****************************************************************************
void V11R3ServerParameterConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "V11R3 Server Config Para");
		PRINTCNFG(CONFIG_LOG, "FirstInterface[%s]", CommPortStr[V11R3ServerParameters.interface]);
		PRINTCNFG(CONFIG_LOG, "SecondInterface[%s]", CommPortStr[V11R3ServerParameters.secondInterface]);
		PRINTCNFG(CONFIG_LOG, "ThirdInterface[%s]", CommPortStr[V11R3ServerParameters.thirdInterface]);
		//ethernet parameter
		PRINTCNFG(CONFIG_LOG, "EthernetServerIpAddress[%s]", V11R3ServerParameters.ethServerIpAddress);
		PRINTCNFG(CONFIG_LOG, "EthernetServerHostName[%s]", V11R3ServerParameters.ethServerHostName);
		PRINTCNFG(CONFIG_LOG, "EthernetServerConnection[%s]", ServerConnectionStr[V11R3ServerParameters.ethServerConnection]);
		PRINTCNFG(CONFIG_LOG, "EthernetServerPort[%u]", V11R3ServerParameters.ethServerPort);
		//wifi parameter
		PRINTCNFG(CONFIG_LOG, "WifiServerIpAddress[%s]", V11R3ServerParameters.wifiServerIpAddress);
		PRINTCNFG(CONFIG_LOG, "WifiServerHostName[%s]", V11R3ServerParameters.wifiServerHostName);
		PRINTCNFG(CONFIG_LOG, "WifiServerConnection[%s]", ServerConnectionStr[V11R3ServerParameters.wifiServerConnection]);
		PRINTCNFG(CONFIG_LOG, "WifiServerPort[%u]", V11R3ServerParameters.wifiServerPort);
		//modem parameter
		PRINTCNFG(CONFIG_LOG, "ModemServerIpAddress[%s]", V11R3ServerParameters.modemServerIpAddress);
		PRINTCNFG(CONFIG_LOG, "ModemServerHostName[%s]", V11R3ServerParameters.modemServerHostName);
		PRINTCNFG(CONFIG_LOG, "ModemServerConnection[%s]", ServerConnectionStr[V11R3ServerParameters.modemServerConnection]);
		PRINTCNFG(CONFIG_LOG, "ModemServerPort[%u]", V11R3ServerParameters.modemServerPort);
		//http parameter
		PRINTCNFG(CONFIG_LOG, "HttpInterface[%s]", CommPortStr[V11R3ServerParameters.httpInterface]);
		PRINTCNFG(CONFIG_LOG, "HttpServerIpAddress[%s]", V11R3ServerParameters.httpServerIpAddress);
		PRINTCNFG(CONFIG_LOG, "HttpServerHostName[%s]", V11R3ServerParameters.httpServerHostName);
		PRINTCNFG(CONFIG_LOG, "HttpServerConnection[%s]", ServerConnectionStr[V11R3ServerParameters.httpServerConnection]);
		PRINTCNFG(CONFIG_LOG, "HttpServerPort[%u]", V11R3ServerParameters.httpServerPort);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	ServerParameterConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Server Configuration parameter.
//*****************************************************************************
void ServerParameterConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Server Config Para");
		PRINTCNFG(CONFIG_LOG, "FirstInterface[%s]", CommPortStr[ServerParameters.interface]);
		PRINTCNFG(CONFIG_LOG, "SecondInterface[%s]", CommPortStr[ServerParameters.secondInterface]);
		PRINTCNFG(CONFIG_LOG, "ThirdInterface[%s]", CommPortStr[ServerParameters.thirdInterface]);
		//ethernet parameter
		PRINTCNFG(CONFIG_LOG, "EthernetServerIpAddress[%s]", ServerParameters.ethServerIpAddress);
		PRINTCNFG(CONFIG_LOG, "EthernetServerHostName[%s]", ServerParameters.ethServerHostName);
		PRINTCNFG(CONFIG_LOG, "EthernetServerConnection[%s]", ServerConnectionStr[ServerParameters.ethServerConnection]);
		PRINTCNFG(CONFIG_LOG, "EthernetServerPort[%u]", ServerParameters.ethServerPort);
		//wifi parameter
		PRINTCNFG(CONFIG_LOG, "WifiServerIpAddress[%s]", ServerParameters.wifiServerIpAddress);
		PRINTCNFG(CONFIG_LOG, "WifiServerHostName[%s]", ServerParameters.wifiServerHostName);
		PRINTCNFG(CONFIG_LOG, "WifiServerConnection[%s]", ServerConnectionStr[ServerParameters.wifiServerConnection]);
		PRINTCNFG(CONFIG_LOG, "WifiServerPort[%u]", ServerParameters.wifiServerPort);
		//modem parameter
		PRINTCNFG(CONFIG_LOG, "ModemServerIpAddress[%s]", ServerParameters.modemServerIpAddress);
		PRINTCNFG(CONFIG_LOG, "ModemServerHostName[%s]", ServerParameters.modemServerHostName);
		PRINTCNFG(CONFIG_LOG, "ModemServerConnection[%s]", ServerConnectionStr[ServerParameters.modemServerConnection]);
		PRINTCNFG(CONFIG_LOG, "ModemServerPort[%u]", ServerParameters.modemServerPort);
		//http parameter
		PRINTCNFG(CONFIG_LOG, "HttpServerConnection[%s]", ServerConnectionStr[ServerParameters.httpServerConnection]);
		PRINTCNFG(CONFIG_LOG, " ");

	}
}

//*****************************************************************************
//	WifiParameterConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Wifi Configuration parameter.
//*****************************************************************************
void WifiParameterConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Wifi Config Para");
		PRINTCNFG(CONFIG_LOG, "IpAddress[%s]", WifiParameters.ipAddress);
		PRINTCNFG(CONFIG_LOG, "Subnet[%s]", WifiParameters.subnet);
		PRINTCNFG(CONFIG_LOG, "Gateway[%s]", WifiParameters.gateway);
		PRINTCNFG(CONFIG_LOG, "MacAddress[%s]", WifiParameters.macAddress);
		PRINTCNFG(CONFIG_LOG, "Dns1[%s]", WifiParameters.dns1);
		PRINTCNFG(CONFIG_LOG, "Dns2[%s]", WifiParameters.dns2);
		PRINTCNFG(CONFIG_LOG, "IpMode[%s]", IpModeStr[WifiParameters.nwType]);
		PRINTCNFG(CONFIG_LOG, "SecurityType[%s]", SecTypeStr[WifiParameters.secType]);
		PRINTCNFG(CONFIG_LOG, "Encryption[%s]", EncryptionStr[WifiParameters.encType]);
		PRINTCNFG(CONFIG_LOG, "Passphresh[%s]", WifiParameters.passphresh);
		PRINTCNFG(CONFIG_LOG, "SSID[%s]", WifiParameters.ssid);
		PRINTCNFG(CONFIG_LOG, "KeyManagement[%s]", WifiKeyMgmtStr[WifiParameters.keymgmt]);
		PRINTCNFG(CONFIG_LOG, "WPSTrigerTyp[%s]", WPSTrigerTypeStr[WifiParameters.wpsTrigerTyp]);
		PRINTCNFG(CONFIG_LOG, "WpsPin[%s]", WifiParameters.wpsPin);
		PRINTCNFG(CONFIG_LOG, "AdvAuthType[%s]", WifiAuthTypeStr[WifiParameters.advAuthType]);
		PRINTCNFG(CONFIG_LOG, "UserName[%s]", WifiParameters.userName);
		PRINTCNFG(CONFIG_LOG, "AnonymousIdentity[%s]", WifiParameters.anonymousIdentity);
		PRINTCNFG(CONFIG_LOG, "PeapVer[%s]", PeapVersionStr[WifiParameters.peapVer]);
		PRINTCNFG(CONFIG_LOG, "InnerAuth[%s]", InnerAuthStr[WifiParameters.innerAuth]);
		PRINTCNFG(CONFIG_LOG, "Enable Wifi [%d]", WifiParameters.enableWifi);
		PRINTCNFG(CONFIG_LOG, "Wifi bend Frequency type [%d]", WifiParameters.bandFreqType);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	ModemProfileConfigDebug( )
//	Param:
//		IN : index			- Modem Profile index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes ModemProfile index and print respected parameter of Modem Profile.
//*****************************************************************************
void ModemProfileConfigDebug(UINT8 index)
{
	UINT8	tmpIdx, tmpMaxIdx;

	DPRINT(CONFIG_LOG, " Modem Profile Config Para");
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		if(index >= MAX_MODEM_PROFILE)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_MODEM_PROFILE;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index+1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "ModemProfileIndex[0-4][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "Enable[%s]", ActiveInactiveStr[ModemProfile[tmpIdx].enable]);
			PRINTCNFG(CONFIG_LOG, "ProfileName[%s]", ModemProfile[tmpIdx].profileName);
			PRINTCNFG(CONFIG_LOG, "DialNo[%s]", ModemProfile[tmpIdx].dialNo);
			PRINTCNFG(CONFIG_LOG, "UserName[%s]", ModemProfile[tmpIdx].userName);
			PRINTCNFG(CONFIG_LOG, "UserPswd[%s]", ModemProfile[tmpIdx].userPswd);
			PRINTCNFG(CONFIG_LOG, "ModemType[%s]",ModemTypeStr[ModemProfile[tmpIdx].modemType]);
			PRINTCNFG(CONFIG_LOG, "APN[%s]", ModemProfile[tmpIdx].apn);
			PRINTCNFG(CONFIG_LOG, "PreferedPort[%u]", ModemProfile[tmpIdx].preferedPort);
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	CommandControlConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Command Control Configuration parameter.
//*****************************************************************************
void CommandControlConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Command Control Config Para");
		PRINTCNFG(CONFIG_LOG, "Enable[%s]", ActiveInactiveStr[CmdCtrlConfig.enable]);
		PRINTCNFG(CONFIG_LOG, "IpAddress[%s]", CmdCtrlConfig.ipAddress);
		PRINTCNFG(CONFIG_LOG, "Port[%u]", CmdCtrlConfig.port);
		PRINTCNFG(CONFIG_LOG, "cccInterface[%s]", CommPortStr[CmdCtrlConfig.cccInterface]);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	PasswordConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Password Configuration parameter.
//*****************************************************************************
void PasswordConfigDebug(void)
{
	UINT8	tmpIdx;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, " Password Config Para");
		for(tmpIdx = 0; tmpIdx < MAX_USER_TYPE; tmpIdx++)
		{
			PRINTCNFG(CONFIG_LOG, "[%s]->Password[%s]", UserTypStr[tmpIdx], Password.displayPswd[tmpIdx] );
		}
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	TwoPersonGroupConfigDebug( )
//	Param:
//		IN : index			- TwoPersonGroup index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes TwoPersonGroup index and print respected parameter of TwoPersonGroup.
//*****************************************************************************
void TwoPersonGroupConfigDebug(UINT8 index)
{
	UINT8	tmpIdx, tmpMaxIdx,tmpcnt;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Two Person Group Config Para");
		if(index >= MAX_TWO_PERSON_GROUP)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_TWO_PERSON_GROUP;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index+1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "TwoPersonGroupIndex[0-15][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "Name[%s]", TwoPersonGroup[tmpIdx].name);
			for(tmpcnt = 0; tmpcnt < MAX_TWO_PERSON_MEMBERS; tmpcnt++)
			{
				PRINTCNFG(CONFIG_LOG, "[%u] -> userId[%u]", tmpcnt, TwoPersonGroup[tmpIdx].userId[tmpcnt]);
			}
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	TimeParameter( )
//	Param:
//		IN : Detail	- Hour and Minute detail from which this function was called
//		OUT:
//	Returns:None
//
//	Description:
//			This function print respected Detail of Hour and Minute.
//*****************************************************************************
void TimeParameter(CHARPTR preStrPtr, TIME_HHMM_t *TimeDetail)
{
	PRINTCNFG(CONFIG_LOG, "%s hour[%u] , minute[%u]", preStrPtr, TimeDetail->hour , TimeDetail->minute);
}

//*****************************************************************************
//	VisitorConfigDebug( )
//	Param:
//		IN : index			-Visitor index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes Visitor index and print respected parameter of Visitors configuration.
//*****************************************************************************
void VisitorConfigDebug(UINT32 index)
{
	UINT32	tmpIdx, tmpMaxIdx;
	UINT8 idx;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, " Visitor Config Para");
		if(index >= MAX_VISITORS)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_VISITORS;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index+1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "VisitorIndex[0-49999][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "visitorName[%s]", Visitor[tmpIdx].visitorName);
			DayParameter("StartDate  ->" , &(Visitor[tmpIdx].startDate));
			DayParameter("EndDate    ->" , &(Visitor[tmpIdx].endDate));
			TimeParameter("StartTime ->" , &(Visitor[tmpIdx].startTime));
			TimeParameter("EndTime   ->" , &(Visitor[tmpIdx].endTime));
			PRINTCNFG(CONFIG_LOG, "RepeatMode[%d]", Visitor[tmpIdx].repeatMode);
			for(idx = 0; idx < MAX_REPEAT_DAYS; idx++)
			{
				PRINTCNFG(CONFIG_LOG, "RepeatDays[%d]: [%d] ", idx, Visitor[tmpIdx].repeatDays[idx]);
			}
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	SpecialFunctionsConfigDebug( )
//	Param:
//		IN : index			-Special Functions index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes Special Functions index and print respected parameter
//		of SpecialFunctions configuration.
//*****************************************************************************
void SpecialFunctionsConfigDebug(UINT8 index)
{
	UINT8	tmpIdx, tmpMaxIdx,tmpcnt;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, " Special Functions Config Para");
		if(index >= MAX_SPECIAL_FUNC)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_SPECIAL_FUNC;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index+1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "SpecialFunctionIndex[0-22][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "SpecialFunctionEnable[%s]", ActiveInactiveStr[SpecialFunctions[tmpIdx].enable]);
			PRINTCNFG(CONFIG_LOG, "UserGroupId[%u]", SpecialFunctions[tmpIdx].userGrpId);
			for(tmpcnt=0; tmpcnt < MAX_SP_FUNC_CARD; tmpcnt++)
			{
				PRINTCNFG(CONFIG_LOG, "Card[%u]->[%llu]", (tmpcnt+1), SpecialFunctions[tmpIdx].cardId[tmpcnt] );
			}
			PRINTCNFG(CONFIG_LOG, "promptJobCodeF[%u]", SpecialFunctions[tmpIdx].promptJobCodeF);
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	TimeTrigerFuncConfigDebug( )
//	Param:
//		IN :  index			-Time Triger Functions index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes TimeTrigerFunc index and print respected parameter
//      of Time Trigger Function configuration.
//*****************************************************************************
void TimeTrigerFuncConfigDebug(UINT8 index)
{
	UINT8	tmpIdx,tmpMaxIdx,tmpcnt;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, " Time Trigger Config Para");
		if(index >= MAX_TIME_TRIG_FUNC)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_TIME_TRIG_FUNC;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index+1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "Index[0-98][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "Enable[%s]", ActiveInactiveStr[TimeTrigFunc[tmpIdx].enable]);
			PRINTCNFG(CONFIG_LOG, "OutputPort[%s] , Name[%s]", OutputPortStr[TimeTrigFunc[tmpIdx].outputPort] , TimeTrigFunc[tmpIdx].name);
			TimeParameter("StartTime ->" , &(TimeTrigFunc[tmpIdx].startTime));
			PRINTCNFG(CONFIG_LOG, "Duration[%u]", TimeTrigFunc[tmpIdx].duration);
			for(tmpcnt = 0; tmpcnt < MAX_WDAY; tmpcnt++)
			{
				PRINTCNFG(CONFIG_LOG, "[%s] -> WorkingEnable[%s]", WeekDaysStr[tmpcnt], ActiveInactiveStr[TimeTrigFunc[tmpIdx].workingDays[tmpcnt] ] );
			}
			PRINTCNFG(CONFIG_LOG, "HolidayWorkingEnable[%s]", ActiveInactiveStr[TimeTrigFunc[tmpIdx].holidayWorking]);
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	FingerPrintConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Finger Print Configuration parameter.
//*****************************************************************************
void FingerPrintConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Finger Print Config Para");
		PRINTCNFG(CONFIG_LOG, "LightCondition[%s]", LightconditionStr[FprConfig.lightCondition]);
		PRINTCNFG(CONFIG_LOG, "SensitivityLevel[%s]", SensitivityLevelStr[FprConfig.sensitivityLevel]);
		PRINTCNFG(CONFIG_LOG, "ImageQuality[%s]", ImageQualityStr[FprConfig.imageQuality]);
		PRINTCNFG(CONFIG_LOG, "SecLevel[%s]", SecurityLevelStr[FprConfig.secLevel]);
		PRINTCNFG(CONFIG_LOG, "FastMode[%s]", FastModeStr[FprConfig.fastMode]);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	ReadersParaConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Readers Configuration parameter.
//*****************************************************************************
void ReadersParaConfigDebug(void)
{
	UINT8	tmpIdx;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Readers Config Para");
		for(tmpIdx = 0 ; tmpIdx < MAX_READERS; tmpIdx++)
		{
			PRINTCNFG(CONFIG_LOG, "[Reader-%u]->Sm125Radf[%u]", tmpIdx, ReadersParaConfig.sm125Radf[tmpIdx]);
		}
		PRINTCNFG(CONFIG_LOG, "retryWaitTimer[%d]", ReadersParaConfig.ambientPara.retryWaitTimer);
		PRINTCNFG(CONFIG_LOG, "tempCount[%d]",  ReadersParaConfig.ambientPara.tempCount);
		PRINTCNFG(CONFIG_LOG, "tempDeviation[%f]", ReadersParaConfig.ambientPara.tempDeviation);
		PRINTCNFG(CONFIG_LOG, "ambCycle[%d]", ReadersParaConfig.ambientPara.ambCycle);
		PRINTCNFG(CONFIG_LOG, "ambDeviation[%f]", ReadersParaConfig.ambientPara.ambDeviation);
		PRINTCNFG(CONFIG_LOG, "showAmbonStatus[%d]", ReadersParaConfig.ambientPara.showAmbonStatus);
		PRINTCNFG(CONFIG_LOG, "checkAmbReading[%d]",  ReadersParaConfig.ambientPara.checkAmbReading);
		PRINTCNFG(CONFIG_LOG, "ambUserDeviation[%f]",  ReadersParaConfig.ambientPara.ambUserDeviation);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	IoLinkExtentionDevConfigDebug( )
//	Param:
//		IN : index			-IoLinkExtentionDev index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes IoLinkExtentionDev index and print respected parameter of IoLink Extension configuration.
//*****************************************************************************
void IoLinkExtentionDevConfigDebug(UINT8 index)
{
	UINT8	tmpIdx, tmpMaxIdx;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "External Io Link Config Para");
		if(index >= MAX_IO_LINK_EXT_DEVICE)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_IO_LINK_EXT_DEVICE;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index+1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "Index[0-254][%u]-> DeviceIpAddr[%s]", tmpIdx, IoLinkExtDevConfig[tmpIdx].devIpAddr);
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	IoLinkExtDevMappingDebug( )
//	Param:
//		IN : index			-IoLinkExtDevMapping index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes IoLinkExtDevMapping index and print respected parameter of IoLinkExtDevice Mapping configuration.
//*****************************************************************************
void IoLinkExtDevMappingDebug(UINT8 index)
{
	UINT8	tmpIdx, tmpMaxIdx, tmpActvdevice;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "External IoLink Mapping Para");
		if(index >= MAX_IO_LINK)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_IO_LINK;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index + 1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "ExtLinkMapingIndex[0-11][%u]", tmpIdx);
			for(tmpActvdevice = 0; tmpActvdevice < MAX_IO_LINK_EXT_DEVICE; tmpActvdevice++)
			{
				PRINTCNFG(CONFIG_LOG, "[%u] -> DeviceEnable[%s]", tmpActvdevice, ActiveInactiveStr[IoLinkExtDevMapping[tmpIdx].devEnable[tmpActvdevice] ] );
			}
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	CafeItemConfigDebug( )
//	Param:
//		IN : index			-CafeItemConfig index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes CafeItemConfig index and print respected parameter of CafeItem configuration.
//*****************************************************************************
void CafeItemConfigDebug(UINT8 index)
{
	UINT8	tmpIdx, tmpMaxIdx,tempcunt;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, " Cafe Item Config Para");
		if(index >= MAX_CAFE_ITEM_CODE)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_CAFE_ITEM_CODE;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index+1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "CafeItemIndex[0-254][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "ItemName[%s] Price[%f]", CafeItemConfig[tmpIdx].name, CafeItemConfig[tmpIdx].price);
			PRINTCNFG(CONFIG_LOG, "DisCntEnabled[%s]", ActiveInactiveStr[CafeItemConfig[tmpIdx].disCntEnabled]);
			for(tempcunt = 0; tempcunt < MAX_CAFE_ITEM_DISCOUNT_LEVEL; tempcunt++)
			{
				PRINTCNFG(CONFIG_LOG, "[%u]->Discount[%f]", tempcunt, CafeItemConfig[tmpIdx].disCnt[tempcunt]);
			}
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	CafeMenuConfigDebug( )
//	Param:
//		IN : index			-CafeMenuConfig index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes CafeMenuConfig index and print respected parameter of Cafe Menu configuration.
//*****************************************************************************
void CafeMenuConfigDebug(UINT16 index)
{
	UINT16	tmpIdx, tmpMaxIdx, tempcunt;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, " Cafe Menu Config Para");
		if(index >= MAX_CAFE_MENU_CODE)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_CAFE_MENU_CODE;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index + 1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "CafeMenuIndex[0-8][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "Active[%s]", ActiveInactiveStr[CafeMenuConfig[tmpIdx].active]);
			PRINTCNFG(CONFIG_LOG, "DfltItemEnbl[%s]", ActiveInactiveStr[CafeMenuConfig[tmpIdx].dfltItemEnbl]);

			for(tempcunt = 0; tempcunt < MAX_CAFE_MENU_ITEMS; tempcunt++)
			{
				if(CafeMenuConfig[tmpIdx].itemCode[tempcunt] == INVALID_CAFE_ITEM_CODE)
				{
					PRINTCNFG(CONFIG_LOG, "Items from [%d] to [98] are having default value in menu:[%d]", tempcunt, tmpIdx);
					break;
				}
				PRINTCNFG(CONFIG_LOG, "[%u] -> ItemCode[%u]", tempcunt, CafeMenuConfig[tmpIdx].itemCode[tempcunt]);
			}
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	CafeMenuSchConfigDebug( )
//	Param:
//		IN : index			-CafeMenuSchConfig index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes CafeMenuSch index and print respected parameter of Cafe Menu schedule configuration .
//*****************************************************************************
void CafeMenuSchConfigDebug(UINT8 index)
{
	UINT8	tmpIdx, tmpMaxIdx, tempcunt;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, " Cafe Menu Sch Config Para");
		if(index >= MAX_CAFE_MENU_SCHEDULE)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_CAFE_MENU_SCHEDULE;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index + 1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "CafeMenuIndex[0-47][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "MenuCode[%u]", CafeMenuSchConfig[tmpIdx].menuCode);
			TimeParameter("StartTime ->" , &(CafeMenuSchConfig[tmpIdx].dayTimeInfo.start));
			TimeParameter("EndTime   ->" , &(CafeMenuSchConfig[tmpIdx].dayTimeInfo.end));
			for(tempcunt = 0; tempcunt < MAX_WDAY; tempcunt++)
			{
				PRINTCNFG(CONFIG_LOG, "[%s]->WorkingEnbl[%s]", WeekDaysStr[tempcunt], ActiveInactiveStr[CafeMenuSchConfig[tmpIdx].dayTimeInfo.workingDays[tempcunt]]);
			}
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	PrinterConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Printer Configuration parameter.
//*****************************************************************************
void PrinterConfigDebug(void)
{
	UINT8 tmpidx;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, " Printer Config Para");
		PRINTCNFG(CONFIG_LOG, "PrinterType[%s]", PrinterTypeStr[PrinterConfig.printerType]);
		PRINTCNFG(CONFIG_LOG, "PrinterCommType[%s]", PrinterCommTypStr[PrinterConfig.printerCommType]);
		PRINTCNFG(CONFIG_LOG, "CompanyName[%s]", PrinterConfig.companyName);
		for(tmpidx=0; tmpidx < MAX_ADDR_LINE_PRINT; tmpidx++)
		{
			PRINTCNFG(CONFIG_LOG, "CompanyAddr[%s]", PrinterConfig.companyAddr[tmpidx]);
		}
		PRINTCNFG(CONFIG_LOG, "PunchLine[%s]", PrinterConfig.punchLine);
		PRINTCNFG(CONFIG_LOG, "BaudRate[%u]", PrinterConfig.baudRate);
		PRINTCNFG(CONFIG_LOG, "PrintReceiptCnt[%u]", PrinterConfig.printReceiptCnt);
		PRINTCNFG(CONFIG_LOG, "ExcludePriceFromCoupon[%s]", ActiveInactiveStr[PrinterConfig.excludePriceFrmCpn] );
		PRINTCNFG(CONFIG_LOG, "PrintUserBalanceOnCouponF[%s]", ActiveInactiveStr[PrinterConfig.printUserBalanceOnCouponF] );
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	DvrEvtActionConfigDebug( )
//	Param:
//		IN : index			-DvrEvtAction index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes DvrEvtAction index and print respected parameter of Dvr Event Action configuration.
//*****************************************************************************
void DvrEvtActionConfigDebug(UINT8 index)
{
	UINT8	tmpIdx,tmpMaxIdx,tmpcnt;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Dvr Event Action Config Para");
		if(index >= MAX_DVR_EVT_ACTN_CONFIG)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_DVR_EVT_ACTN_CONFIG;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index+1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "DvrEventIndex[0-19][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "Active[%s]", ActiveInactiveStr[DvrEvtActionConfig[tmpIdx].active]);
			PRINTCNFG(CONFIG_LOG, "EventType[%s]", DvrConfigEventTypStr[DvrEvtActionConfig[tmpIdx].evtType]);
			PRINTCNFG(CONFIG_LOG, "ActionType[%s]", DvrActionTypeStr[DvrEvtActionConfig[tmpIdx].actionType]);
			TimeParameter("StartTime ->" , &(DvrEvtActionConfig[tmpIdx].startTime));
			TimeParameter("EndTime   ->", &(DvrEvtActionConfig[tmpIdx].endTime));
			PRINTCNFG(CONFIG_LOG, "ReaderMode[%s]", ReaderModeStr[DvrEvtActionConfig[tmpIdx].mode]);
			PRINTCNFG(CONFIG_LOG, "SelectCamera[%u]", DvrEvtActionConfig[tmpIdx].selectCamera);
			for(tmpcnt = 0 ; tmpcnt < MAX_WDAY; tmpcnt++)
			{
				PRINTCNFG(CONFIG_LOG, "[%s]->ActiveEnbl[%s]", WeekDaysStr[tmpcnt], ActiveInactiveStr[DvrEvtActionConfig[tmpIdx].activeDaysF[tmpcnt] ] );
			}
			PRINTCNFG(CONFIG_LOG, "HoliDayOverrideEnbl[%s]", ActiveInactiveStr[DvrEvtActionConfig[tmpIdx].holiDayOverrideF]);
			PRINTCNFG(CONFIG_LOG, "RecDuration[%u]", DvrEvtActionConfig[tmpIdx].recDuration);
			PRINTCNFG(CONFIG_LOG, "PopupDuration[%u]", DvrEvtActionConfig[tmpIdx].popupDuration);
			PRINTCNFG(CONFIG_LOG, "PtzPresent[%u]", DvrEvtActionConfig[tmpIdx].ptzPreset);
			PRINTCNFG(CONFIG_LOG, "EmailAddr[%s]", DvrEvtActionConfig[tmpIdx].emailAddr);
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	SmartRouteAccessConfigDebug( )
//	Param:
//		IN : index			-SmartRouteAccess index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes SmartRouteAccess index and print respected parameter of Smart Route Access configuration.
//*****************************************************************************
void SmartRouteAccessConfigDebug(UINT8 index)
{
	UINT8	tmpIdx,tmpMaxIdx;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Smart Route Access Config Para");
		if(index >= MAX_SMART_ROUTE_ACCESS)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_SMART_ROUTE_ACCESS;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index+1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "Index[0-98][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "Enable[%s]", ActiveInactiveStr[SmartRouteAccessConfig[tmpIdx].active]);
			PRINTCNFG(CONFIG_LOG, "DoorLevel[%u]", SmartRouteAccessConfig[tmpIdx].doorLevel);
			PRINTCNFG(CONFIG_LOG, "RouteType[%s]", SmartRouteAccessTypStr[SmartRouteAccessConfig[tmpIdx].routeType]);
			PRINTCNFG(CONFIG_LOG, "ResetOnStartLevel[%s]", ActiveInactiveStr[SmartRouteAccessConfig[tmpIdx].resetOnStartLevel]);
			PRINTCNFG(CONFIG_LOG, "MinLevel[%u]", SmartRouteAccessConfig[tmpIdx].minLevel);
			PRINTCNFG(CONFIG_LOG, "MaxLevel[%u]", SmartRouteAccessConfig[tmpIdx].maxLevel);
			PRINTCNFG(CONFIG_LOG, "SequencedEnbl[%s]", ActiveInactiveStr[SmartRouteAccessConfig[tmpIdx].sequencedF]);
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	FirstInInfoConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected First In Information Configuration parameter.
//*****************************************************************************
void FirstInInfoConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "First In Information Config Para");
		PRINTCNFG(CONFIG_LOG, "FirstInInfoEnable[%s]", ActiveInactiveStr[FirstInInfo.lockF]);
		PRINTCNFG(CONFIG_LOG, "StartTimeInSec[%ld]", FirstInInfo.startTimeInSec);
		PRINTCNFG(CONFIG_LOG, "EndTimeInSec[%ld]", FirstInInfo.endTimeInSec);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	WebAPIEventsConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Web API Events Configuration parameter.
//*****************************************************************************
void WebAPIEventsConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Web API Event Config Para");
		PRINTCNFG(CONFIG_LOG, "Enable[%s]", ActiveInactiveStr[WebAPIEventsConfig.enable]);
		PRINTCNFG(CONFIG_LOG, "IpAddress[%s]", WebAPIEventsConfig.ipAddress);
		PRINTCNFG(CONFIG_LOG, "Port[%u]", WebAPIEventsConfig.port);
		PRINTCNFG(CONFIG_LOG, "RespTime[%u]", WebAPIEventsConfig.respTime);
		PRINTCNFG(CONFIG_LOG, "Interface[%s]", CommPortStr[WebAPIEventsConfig.interface]);
		PRINTCNFG(CONFIG_LOG, "Trigger[%s]", ActiveInactiveStr[WebAPIEventsConfig.trigger]);
		PRINTCNFG(CONFIG_LOG, "KeepLiveEvent[%s]", ActiveInactiveStr[WebAPIEventsConfig.keepLiveEvent]);
		PRINTCNFG(CONFIG_LOG, "RollOverCnt[%u]", WebApiEventActivity.rollOverCnt);
		PRINTCNFG(CONFIG_LOG, "SeqNo[%u]", WebApiEventActivity.seqNo);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	CardFormatConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Card Format Configuration parameter.
//*****************************************************************************
void CardFormatConfigDebug(void)
{
	UINT8	tmpIdx, tmpcnt, cardFmtIdx;
	CHAR	valStr[MAX_CARD_ID_BITS+1];

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Card Format Config Para");
		for(cardFmtIdx = 0; cardFmtIdx < MAX_CARD_FORMAT_PER_DOOR; cardFmtIdx++)
		{
			for(tmpIdx = 0; tmpIdx < INT_EXT_PORT; tmpIdx++)
			{
				PRINTCNFG(CONFIG_LOG, "cardFmtIdx[%u]", cardFmtIdx);
				PRINTCNFG(CONFIG_LOG, "ConfigIndex[%u]", tmpIdx);
				PRINTCNFG(CONFIG_LOG, "MaxCardBit[%u]", CardFormatConf[cardFmtIdx].maxCardBit[tmpIdx]);
				PRINTCNFG(CONFIG_LOG, "CardStructBit[%u]", CardFormatConf[cardFmtIdx].cardStructBit[tmpIdx]);
				for(tmpcnt = 0; tmpcnt < MAX_CARD_ID_BITS; tmpcnt++)
				{
					sprintf(&valStr[tmpcnt],"%u",CardFormatConf[cardFmtIdx].cardStruct[tmpIdx][tmpcnt]);
				}
				PRINTCNFG(CONFIG_LOG, "CardStruct[%s]",valStr);
				PRINTCNFG(CONFIG_LOG, "cardIdOrder[%d]", CardFormatConf[cardFmtIdx].cardIdOrder[tmpIdx] );
				PRINTCNFG(CONFIG_LOG, "FCIncardId[%d]", CardFormatConf[cardFmtIdx].FCIncardIdF[tmpIdx]);
				PRINTCNFG(CONFIG_LOG, "operationSeq[%d]", CardFormatConf[cardFmtIdx].operationSeq[tmpIdx]);
				PRINTCNFG(CONFIG_LOG, " ");
			}
			PRINTCNFG(CONFIG_LOG, " ");
		}
	}
}

//*****************************************************************************
//	VamConfigDebug( )
//	Param:
//		IN : index			-Vam index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes Vam index and print respected parameter of Vam configuration.
//*****************************************************************************
void VamConfigDebug(UINT32 index)
{
	UINT32	tmpIdx, tmpMaxIdx, tmpcnt;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Vam Config Para");
		if(index >= MAX_VAM_CONFIG)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_VAM_CONFIG;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index + 1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "Index[0-11999][%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "PassId[%u]", VamConfig[tmpIdx].passId);
			for(tmpcnt = 0; tmpcnt < MAX_VAM_CARD_TYPE ; tmpcnt++)
			{
				PRINTCNFG(CONFIG_LOG, "[%s]->vamCardId[%llu]", VamCardTypeStr[tmpcnt], VamConfig[tmpIdx].vamCardId[tmpcnt] );
			}
			DayParameter("startDate  ->" , &(VamConfig[tmpIdx].startDate));
			TimeParameter("startTime ->" , &(VamConfig[tmpIdx].startTime));
			DayParameter("endDate    ->" , &(VamConfig[tmpIdx].endDate));
			TimeParameter("endTime   ->" , &(VamConfig[tmpIdx].endTime));
			PRINTCNFG(CONFIG_LOG, "SmartRouteId[%u]", VamConfig[tmpIdx].smartRouteId);
			PRINTCNFG(CONFIG_LOG, "AccessLevel[%u]", VamConfig[tmpIdx].accessLevel);
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}


//*****************************************************************************
//	JcmConfigDebug( )
//	Param:
//		IN : index			-jcm index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes Vam index and print respected parameter of Vam configuration.
//*****************************************************************************
void JcmConfigDebug(UINT16 index)
{
	UINT32	tmpIdx, tmpMaxIdx;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Jcm Config Para");
		if(index >= (MAX_JCM_CONFIG+3) )
		{
			tmpIdx = 0;
			tmpMaxIdx = (MAX_JCM_CONFIG+3);
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index + 1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "Index [%d]",tmpIdx);
			PRINTCNFG(CONFIG_LOG, "Reference ID: [%d] Job Code:[%s] Job Name[%s]",JcmConfig[tmpIdx].refId,JcmConfig[tmpIdx].jobCode,JcmConfig[tmpIdx].jobName);
			DayParameter("Start Date ->",&(JcmConfig[tmpIdx].startDate) );
			DayParameter("End Date ->",&(JcmConfig[tmpIdx].endDate) );
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	DoorModeSchedulingConfigDebug( )
//	Param:
//		IN : index			-DoorModeScheduleConfig index
//		OUT:
//	Returns:None
//
//	Description:
//			This function takes DoorModeScheduleConfig index and print respected
//			parameter of DoorMode Schedule configuration.
//*****************************************************************************
void DoorModeSchedulingConfigDebug(UINT8 index)
{
	UINT32	tmpIdx, tmpMaxIdx, tmpcnt;

	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Door Mode Swapping ConfigPara");
		if(index >= MAX_DOOR_MODE_SCHEDULE)
		{
			tmpIdx = 0;
			tmpMaxIdx = MAX_DOOR_MODE_SCHEDULE;
		}
		else
		{
			tmpIdx = index;
			tmpMaxIdx = index + 1;
		}
		while(tmpIdx < tmpMaxIdx)
		{
			PRINTCNFG(CONFIG_LOG, "Index[0-14][%u]", tmpIdx);
			TimeParameter("startTime ->" , &(DoorModeSchCfg[tmpIdx].startTime));
			TimeParameter("endTime   ->" , &(DoorModeSchCfg[tmpIdx].endTime));
			for(tmpcnt = 0; tmpcnt < MAX_WDAY; tmpcnt++)
			{
				PRINTCNFG(CONFIG_LOG, "[%s]->ActiveEnbl[%s]", WeekDaysStr[tmpcnt], ActiveInactiveStr[DoorModeSchCfg[tmpIdx].days[tmpcnt] ] );
			}
			PRINTCNFG(CONFIG_LOG, "DoorRdrMode[%s]",ReaderModeStr[DoorModeSchCfg[tmpIdx].doorRdrMode]);
			PRINTCNFG(CONFIG_LOG, " ");
			tmpIdx++;
		}
	}
}

//*****************************************************************************
//	ServerModeConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Web API Events Configuration parameter.
//*****************************************************************************
void ServerModeConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Server Mode Config Para");
		PRINTCNFG(CONFIG_LOG, "ServerMode[%u]", ServerModeCfg.serverMode);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	VyomServerConfigDebug( )
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//			This function print respected Vyom Configuration parameter.
//*****************************************************************************
void VyomServerConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, 	 "Vyom server Config Para");
		PRINTCNFG(CONFIG_LOG, "interfaceVyom[%u]", VyomServerPara.interfaceVyom);
		PRINTCNFG(CONFIG_LOG, "secondInterfaceVyom[%u]", VyomServerPara.secondInterfaceVyom);
		PRINTCNFG(CONFIG_LOG, "thirdInterfaceVyom[%u]", VyomServerPara.thirdInterfaceVyom);
		PRINTCNFG(CONFIG_LOG, "ServerHostNameVyom[%s]", VyomServerPara.ServerHostNameVyom);
		PRINTCNFG(CONFIG_LOG, "ServerPortVyom[%u]", VyomServerPara.ServerPortVyom);
		PRINTCNFG(CONFIG_LOG, "TenantIdVyom[%s]", VyomServerPara.TenantIdVyom);
		PRINTCNFG(CONFIG_LOG, "httpInterfaceVyom[%u]", VyomServerPara.httpInterfaceVyom);
		PRINTCNFG(CONFIG_LOG, "httpServerHostNameVyom[%s]", VyomServerPara.httpServerHostNameVyom);
		PRINTCNFG(CONFIG_LOG, "httpServerPortVyom[%u]", VyomServerPara.httpServerPortVyom);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	BackUpAndUpdateConfigDebug()
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//
//*****************************************************************************
void BackUpAndUpdateConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, 	 "BackUpAndUpdate Config Para");
		PRINTCNFG(CONFIG_LOG, "interface[%u]", BackUpAndUpdate.interface);
		PRINTCNFG(CONFIG_LOG, "ftpServerUrl[%s]", BackUpAndUpdate.ftpServerUrl);
		PRINTCNFG(CONFIG_LOG, "autoUpdate[%u]", BackUpAndUpdate.autoUpdate);
		PRINTCNFG(CONFIG_LOG, "upgradeBasedOn[%d]", BackUpAndUpdate.upgradeBasedOn);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	ConnectToPanelConfigDebug()
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//
//*****************************************************************************
void ConnectToPanelConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, 	 "ConnectToPanel Config Para");
		PRINTCNFG(CONFIG_LOG, "connectToPanel Enable[%u]", ConnectToPanelPara.connectToPanel);
		PRINTCNFG(CONFIG_LOG, "connectToPanel NW Interface[%u]", ConnectToPanelPara.panelNwInterface);
		PRINTCNFG(CONFIG_LOG, "connectToPanel panelIpAddr[%s]", ConnectToPanelPara.panelIpAddr);
		PRINTCNFG(CONFIG_LOG, "connectToPanel panel port[%u]", ConnectToPanelPara.panelPort);
		PRINTCNFG(CONFIG_LOG, "connectToPanel panel encryption[%u]", ConnectToPanelPara.panelEncryptionF);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	LicenseDongleConfigDebug()
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//
//*****************************************************************************
void LicenseDongleConfigDebug(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, 	 "LicenseDongleConfig");
		PRINTCNFG(CONFIG_LOG, "licenseServer Url[%s]", LicenseDongleCfg.licenseServerUrl);
		PRINTCNFG(CONFIG_LOG, "licenseServer Port[%u]", LicenseDongleCfg.licenseServerPort);
		PRINTCNFG(CONFIG_LOG, "licenseServer IpAddress[%s]", LicenseDongleCfg.licenseServerIpAddress);
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	UserAttndnsDetailConfigDebug()
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//
//*****************************************************************************
void UserAttndnsDetailConfigDebug(USER_ATTNDNS_DETAIL_t *singleUserAttndnsDetailConfig)
{
	UINT8 	tmpIdx;
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, 	 "UserAttndnsDetailConfig");
		PRINTCNFG(CONFIG_LOG, "processedMonth[%u]", singleUserAttndnsDetailConfig->processedMonth);
		PRINTCNFG(CONFIG_LOG, "processedYear[%u]", singleUserAttndnsDetailConfig->processedYear);
		for(tmpIdx=0; tmpIdx < MAX_DAYS_IN_MONTH; tmpIdx++)
		{
			PRINTCNFG(CONFIG_LOG, "dayStatus[%u] = [%u]", tmpIdx, singleUserAttndnsDetailConfig->dayStatus[tmpIdx]);
		}
		PRINTCNFG(CONFIG_LOG, " ");
	}
}

//*****************************************************************************
//	IpCameraConfigDebug()
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//
//*****************************************************************************
void IpCameraConfigDebug(void)
{
	UINT8	tmpIdx;
	if((GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED))
	{
		DPRINT(CONFIG_LOG, "ip camera config debug");
		PRINTCNFG(CONFIG_LOG, "interfaceSelectionF[%u]", IpCameraConfig.interfaceSelectionF);
		PRINTCNFG(CONFIG_LOG, "url[%s]", IpCameraConfig.captureSnapshotUrl);
		for(tmpIdx = 0; tmpIdx < MAX_CAPTURE_SNAPSHOT_SCH; tmpIdx++)
		{
			PRINTCNFG(CONFIG_LOG, "ipcamera schedule index[%u]", tmpIdx);
			PRINTCNFG(CONFIG_LOG, "scheduleName[%s]", IpCameraConfig.captureSnapshotSchedule[tmpIdx].scheduleName);
			PRINTCNFG(CONFIG_LOG, "eventActive[%u]", IpCameraConfig.captureSnapshotSchedule[tmpIdx].eventActive);
			PRINTCNFG(CONFIG_LOG, "captureEvent[%u]", IpCameraConfig.captureSnapshotSchedule[tmpIdx].captureEvent);
			PRINTCNFG(CONFIG_LOG, "captureEventMode[%u]", IpCameraConfig.captureSnapshotSchedule[tmpIdx].captureEventMode);
			PRINTCNFG(CONFIG_LOG, "startTime[%u:%u]", IpCameraConfig.captureSnapshotSchedule[tmpIdx].startTime.hour,
					IpCameraConfig.captureSnapshotSchedule[tmpIdx].startTime.minute);
			PRINTCNFG(CONFIG_LOG, "endTime[%u:%u]", IpCameraConfig.captureSnapshotSchedule[tmpIdx].endTime.hour,
					IpCameraConfig.captureSnapshotSchedule[tmpIdx].endTime.minute);
			PRINTCNFG(CONFIG_LOG, "activeDays %u %u %u %u %u %u %u %u",
					IpCameraConfig.captureSnapshotSchedule[tmpIdx].activeDay[SUNDAY],
					IpCameraConfig.captureSnapshotSchedule[tmpIdx].activeDay[MONDAY],
					IpCameraConfig.captureSnapshotSchedule[tmpIdx].activeDay[TUESDAY],
					IpCameraConfig.captureSnapshotSchedule[tmpIdx].activeDay[WEDNESDAY],
					IpCameraConfig.captureSnapshotSchedule[tmpIdx].activeDay[THURSDAY],
					IpCameraConfig.captureSnapshotSchedule[tmpIdx].activeDay[FRIDAY],
					IpCameraConfig.captureSnapshotSchedule[tmpIdx].activeDay[SATURDAY],
					IpCameraConfig.captureSnapshotSchedule[tmpIdx].activeDay[MAX_WDAY]);
		}
	}
}
//*****************************************************************************
//	DisplayBrightnessConfigDebug()
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//
//*****************************************************************************
void DisplayBrightnessConfigDebug(void)
{
	if((GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED))
	{
		PRINTCNFG(CONFIG_LOG, "sleepTimer [%u]", DisplayBrightness.sleepTimer);
		PRINTCNFG(CONFIG_LOG, "setBrightness [%u]", DisplayBrightness.setBrightness);
	}
}

//*****************************************************************************
//	SplFnScheduleConfigDebug()
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//
//*****************************************************************************
VOID SplFnScheduleConfigDebug(VOID)
{
	UINT8 idx;
	if((GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED))
	{
		for(idx = 0; idx < MAX_SPECIAL_FN_SCHEDULE; idx++)
		{
			PRINTCNFG(CONFIG_LOG,"Special Function Schedule Index [%u]",idx);
			PRINTCNFG(CONFIG_LOG,"Special Fn Id [%u]",SplFnSchedule[idx].specailFnId);
			PRINTCNFG(CONFIG_LOG,"Start time [%u]:[%u]",SplFnSchedule[idx].startTime.hour,SplFnSchedule[idx].startTime.minute);
			PRINTCNFG(CONFIG_LOG,"End time [%u]:[%u]",SplFnSchedule[idx].endTime.hour,SplFnSchedule[idx].endTime.minute);
			PRINTCNFG(CONFIG_LOG,"Active Days[sun-sat] [%u %u %u %u %u %u %u]",
					SplFnSchedule[idx].activeDays[SUNDAY],
					SplFnSchedule[idx].activeDays[MONDAY],
			        SplFnSchedule[idx].activeDays[TUESDAY],
			        SplFnSchedule[idx].activeDays[WEDNESDAY],
			        SplFnSchedule[idx].activeDays[THURSDAY],
			        SplFnSchedule[idx].activeDays[FRIDAY],
			        SplFnSchedule[idx].activeDays[SATURDAY]);
		}
	}

}

#ifdef PUSH_API_SUPPORTED
//***************************************************************************
//PushAPIConfigDebug()
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//***************************************************************************
VOID PushAPIConfigDebug(VOID)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		PRINTCNFG(CONFIG_LOG, "pushApiInterfaceSel[%u]", PushAPIConfig.pushApiInterfaceSel);
		PRINTCNFG(CONFIG_LOG, "pushApiNetworkInterface[%u]", PushAPIConfig.pushApiNetworkInterface);
		PRINTCNFG(CONFIG_LOG, "pushApiUrl[%s]", PushAPIConfig.pushApiUrl);
		PRINTCNFG(CONFIG_LOG, "pushApiPort[%u]", PushAPIConfig.pushApiPort);
		PRINTCNFG(CONFIG_LOG, "pushApiDirectoryPath[%s]", PushAPIConfig.pushApiDirectoryPath);
		PRINTCNFG(CONFIG_LOG, "pushApiEncryptionF[%u]", PushAPIConfig.pushApiEncryptionF);
		PRINTCNFG(CONFIG_LOG, "pushApiResolvedIp[%s]", PushAPIConfig.pushApiResolvedIp);

	}
}
//***************************************************************************
//PushApiEventCount()
//	Param:
//		IN :None
//		OUT:None
//	Returns:None
//
//	Description:
//***************************************************************************
void PushApiEventCount(void)
{
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		PRINTCNFG(CONFIG_LOG, "Push API Event Count [%d]", PushApiEvent.UploadEventCnt);
	}
}
#endif //PUSH_API_SUPPORTED

//*****************************************************************************
//	AccessScheduleConfigDebug( )
//	Param:
//		IN : index			-TimeStamp inde
//		OUT:
//	Returns:None
//
//			
//	Description: 
//*****************************************************************************
void AccessScheduleConfigDebug()
{
      UINT8	tmpIdx,tmpcnt;
	if( (GetConsoleLogFlag(CONFIG_LOG) == ENABLED) || (GetFileLogFlag(CONFIG_LOG) == ENABLED) )
	{
		DPRINT(CONFIG_LOG, "Access Schedule Config Para");
		for(tmpIdx = 0; tmpIdx < MAX_TIME_FRAME;tmpIdx++)
		{
			PRINTCNFG(CONFIG_LOG, "Access Schedule TimeFrameIndex[0-31][%u]", tmpIdx);
			if(AccessScheduleConfig[tmpIdx].timeFrameEnableF >= 0 && AccessScheduleConfig[tmpIdx].timeFrameEnableF < MAX_BOOL_ENUM)
				PRINTCNFG(CONFIG_LOG, "Enable[%s]", ActiveInactiveStr[AccessScheduleConfig[tmpIdx].timeFrameEnableF]);
			TimeParameter("StartTime ->" , &(AccessScheduleConfig[tmpIdx].timeFrameStartTime));
			TimeParameter("EndTime   ->", &(AccessScheduleConfig[tmpIdx].timeFrameEndTime));
			for(tmpcnt = 0 ; tmpcnt < MAX_WDAY; tmpcnt++)
			{
				if(AccessScheduleConfig[tmpIdx].timeFrameActiveDaysF[tmpcnt] >= 0 && AccessScheduleConfig[tmpIdx].timeFrameActiveDaysF[tmpcnt] < MAX_BOOL_ENUM)
					PRINTCNFG(CONFIG_LOG, "[%s]->ActiveEnbl[%s]", WeekDaysStr[tmpcnt], ActiveInactiveStr[AccessScheduleConfig[tmpIdx].timeFrameActiveDaysF[tmpcnt] ] );
			}
			if(AccessScheduleConfig[tmpIdx].timeFrameHolidayF >= 0 && AccessScheduleConfig[tmpIdx].timeFrameHolidayF < MAX_BOOL_ENUM)
				PRINTCNFG(CONFIG_LOG, "ActiveHoliday[%s]",ActiveInactiveStr[ AccessScheduleConfig[tmpIdx].timeFrameHolidayF] );
			PRINTCNFG(CONFIG_LOG, " ");
		}
	}
  
}
