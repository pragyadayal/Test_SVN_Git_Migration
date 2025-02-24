////////////////////////////////////////////////////////////////////////
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
//   Created By   : Mukesh Savaliya
//   File         : MxDcTask.c
//   Description  : This file implements Main FSM logic for system.
/////////////////////////////////////////////////////////////////////////////

//*** Place all include files here ***
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <sys/stat.h>

#include "MxDeviceSpecific.h"
#include "MxTypedef.h"
#include "MxSysCommonDef.h"
#include "MxSysTime.h"
#include "MxSysTimer.h"
#include "MxConfig.h"
#include "MxSlaveMsg.h"
#include "MxEventLog.h"
#include "MxDst.h"
#include "MxAvResp.h"
#include "MxDcTask.h"
#include "MxDcSpecialFunc.h"
#include "MxEnrollment.h"
#include "MxMenu.h"
#include "MxBackupUpdate.h"
#include "MxIOLink.h"
#include "MxLogs.h"
#include "MxAlarms.h"
#include "MxReaderTask.h"
#include "MxGpio.h"
#include "MxSSA.h"
#include "MxConfigFileIo.h"
#include "MxCafeteria.h"
#include "MxAcmsMsgHandler.h"
#include "MxDvrIntegration.h"
#include "MxSmartRouteAccess.h"
#include "MxSysTest.h"
#include "MxSysUtils.h"
#include "MxPDDcTask.h"
#include "MxEventActionHandler.h"
#include "MxSelfEnrollment.h"
#include "MxDcTaskCommonDef.h"
#include "MxDcTaskCommon.h"
#include "MxVAM.h"
#include "MxVamSmartRouteAccess.h"
#include "MxJobCosting.h"
#include "MxFpIdtServer.h"
#include "MxSupremaReader.h"
#include "MxWiegandOutput.h"
#include "MxDisplayMessage.h"
#include "MxCafeBalReadWriteApi.h"
#include "MxCardFieldsReadWriteThroughApiHandler.h"
#include "MxQrCodeAccessApi.h"
#include "MxDcTaskCommon.h"
#include "MxPanelSocket.h"
#include "MxPanelCommTask.h"
#include "MxFTPFirmwareUpgrade.h"
#include "MxWebAPIVerifyFingerAndDoorOpen.h"
#include "MxAttendanceDetail.h"
#include "MxNetworkUtils.h"
#include "MxWebJvsAutoSystemTest.h"
#include "MxFaceRecognition.h"
#include "MxFaceRecognitionConfig.h"
#include "MxBluetooth.h"
#include "MxSplFnScheduleConfig.h"
#include "MxCbBluetooth.h"
#include "MxCommonBLE.h"
#include "MxTemperatureSensor.h"
#include "MxReaderGuiMessage.h"
#include "MxUserConfig.h"
#include "MxCommonEnrollment.h"
#include "MxTempUserTask.h"
#include "MxCommonConfig.h"
#include "MxSlaveCommon.h"
#include "MxCommTask.h"
#include "MxReaderCommonUtils.h"


//******** Extern Variables **********
extern SYS_ADVANCE_PARA_t				SysAdvPara;
extern SYS_BASIC_PARA_t					SysBasicPara;
extern SPECIAL_FUNCTION_t 				SpecialFunctions[];
extern VISITOR_MANAGEMENT_t 			Visitor[];
extern UINT8							FpReaderMemFullPort;
extern FPR_CONFIG_t						FprConfig;
extern const CHARPTR					ReaderRespStatusStr[];
extern SMART_CARD_KEY_UPDATE_t			SmartCardKeyUpdateDetail;
//extern const CHARPTR 					CardTypeStr[];
extern const USER_CONFIG_t  			DfltUserConfig;
extern ENROLLMENT_TEMP_INPUTS_t 		EnrollmentTempInputs;
extern SMART_ROUTE_ACCESS_CONFIG_t		SmartRouteAccessConfig[MAX_SMART_ROUTE_ACCESS];
extern DENIED_SMART_ROUTE_USER_LIST_t	DeniedSmartRouteUserList[MAX_DENIED_SMART_ROUTE_USER_LIST];
extern UINT32							paraValue[];
extern MMC_INFO_t 						MmcInfo;
extern JCM_CONFIG_t						JcmConfig[MAX_JCM_CONFIG+3]; //+3 for 'None', 'Default' and 'Continue' Job
extern BIO_IDENTIFY_SERVER_INFO_t		BioIdentifyServer;
extern CAFE_BAL_READ_WRITE_DETAIL_t		CafeBalReadWriteDetail;;
extern CARD_READ_WRITE_API_DETAIL_t		CardFieldsReadWriteDetail;
extern CONNECT_TO_PANEL_CONFIG_t		ConnectToPanelPara;
extern BOOL								WaitngForReaderRespFlag;
extern READER_RESP_STATUS_e				VerifyHostTemplateByScanStatus;
extern VERIFY_FINGER_API_OPERAION_e		VerifyFingerApiOperation;
extern SYS_ADVANCE_PARA_t				SysAdvPara;
extern SERVER_MODE_CONFIG_t				ServerModeCfg;
extern VYOM_SERVER_CONFIG_t				VyomServerPara;
extern const VYOM_SERVER_CONFIG_t 		DfltVyomServerPara;
extern BOOL								FRStreamDisplayF;
extern FACE_MATCHED_USER_LIST_t			FaceMatchUserList;
extern FR_SERVER_CONFIG_t				FRServerConfig;
extern UINT32 							RetainJobCode;
extern BOOL								IsFaceAlreadyIdentifyF;
extern struct tm 						CurrTimeDate;
extern CHARPTR 							specialCode[];
extern ADAPTIVE_FR_GUI_QUERY_t			AdaptiveFrQuery;
extern BOOL 							IsAdapativeErlRunningF;
extern uint32_t lastIdleTimeTick;
extern DEGRADE_STATE_e				DegradeState;
extern FLOAT32					longitudeQR;
extern FLOAT32					latitudeQR;
extern SERVER_PARA_t				ServerParameters;
extern CHARPTR 					InterfaceStr[MAX_PORT];
extern ENROLLMENT_TEMP_INPUTS_FOR_TEMP_USER_t 	EnrollmentTempInputsForTempUser;
extern TIMER_HANDLE				AddTempUserTmrHandle;
extern TIMER_HANDLE				TempUserPopupTmrHandle;
extern TIMER_HANDLE				TempUserProcessingTmrHandle;
extern PASSWORD_t			  	Password;
extern STATUS_BAR_INFO_t		CurrStatusbarData;
extern TEMP_USER_ADD_INFO_t 		TempUserAddActivity;
extern READ_TEMPLATE_RESP_DATA_t  readFpResp;
extern const TEMP_USER_COMMON_QT_t 			defltQtTempUsrCommon;
extern TIMER_HANDLE				SpcReqCnfmWaitTmrHandle;
extern CREDENTIAL_TYPE_e				SeqInOutCredType;
extern ACCESS_SCHEDULE_t                		AccessScheduleConfig[MAX_TIME_FRAME];
extern COMM_PORT_e					acmsCurActiveInterface;
extern COMM_STATE_e                 CommState;

//******** Defines and Data Types ****

//******** Function Prototypes *******
static void InitDoorActivityInfo(BOOL initAllF);
static void DcVerifyFingerApiWaitStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);

// DC task state functions
static void DcIdleStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static void DcSmartCardKeyUpdateStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static void DcTwoPersonStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static void DcWaitForAddlCredStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static void DcHttpFirmwareUploadStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static void DcAcmsFirmwareUploadStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static void DcPanelDoorFirmwareUploadStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static void DcInstallationStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);

static void ProcessZam(void);
static BOOL AuthenticateUser(void);
static void ValidateUserOnExitOnExternalInNonIdleState(UINT16 userIdx);

static void SwitchDcToSSAState(void);
//door mode selection related functions
static BOOL SetDoorModeWaitTimer(UINT8 timer);
static void DoorModeWaitTimeout(UINT32 dummy);
static void DisplayDoorModeSelectionList(void);
static void SwitchDcToDoorModeSelectionState(void);
static BOOL LoadKeyChangeWaitTimer(UINT16	timerValue);
static void KeyChangeWaitTimeout(UINT32 data);
static BOOL	IsSpFnInputReq(void);

//******** Global Variables **********
DOOR_ACTIVITY_INFO_t	DcInfo;				//Door Controller FSM info
DOOR_MONITOR_t			DcMonitorControl;	//door monitoring and control info
//CARD_DETAIL_t			LastDetectedCard;	//lastly detected card detail
TA_SCREEN_e				currentTnAScreen;
uint16_t                		curProcFaceUserIdx = UNKNOWN_USER_IDX;
uint32_t                		lastIdleTimeTick = 0;
//******** Static Variables **********

// Call FSM state function table
// Entries in this table should match DC_FSM_STATE enum
static void (*dcFsmFuncPtr[MAX_DC_FSM_STATE])(SLAVE_MSG_ID_e, VOIDPTR, UINT16) =
{
	DC_FSM_STATE(DC_FSM_STATE_FUNC)
};
static BOOL sDeviceInactive = FALSE;

//******** Function Definitions ******

//*****************************************************************************
//	InitDcTask()
//	Param:
//		IN : None
//		OUT: None
//	Returns:
//		NONE
//	Description:
//		This function initialises DC Task.
//*****************************************************************************
void InitDcTask(void)
{
	DcInfo.curState = DC_IDLE_STATE;

	// initialized once only
	DcInfo.doorStatus = STATUS_UNKNOWN;
	DcInfo.lastUserIdx = INVALID_USER;
	DcInfo.useCount = 0;

	DcInfo.singleUserSplCode = USER_SPECIAL_CODE_NONE;
	DcInfo.multiUserSplCode = USER_SPECIAL_CODE_NONE;
	DcInfo.useCountTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.interDigitTmrHandle[INTERNAL_PORT] = INVALID_TIMER_HANDLE;
	DcInfo.interDigitTmrHandle[EXTERNAL_PORT] = INVALID_TIMER_HANDLE;
	DcInfo.waitTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.menuWaitTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.twoPersonTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.doorRelayPulseTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.multiAccessTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.splFnGetUserInputTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.multiUserSplCodeTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.splFnNextInputTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.splFnStatusFieldTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.doorLockDebounceTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.lcdIdleTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.SmrtCardKeyChngTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.fpMatchTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.selfEnrlInputTimerHandle = INVALID_TIMER_HANDLE;
	DcInfo.vamCredWaitTimer = INVALID_TIMER_HANDLE;
	DcInfo.userAllowWaitTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.respPort = MAX_RESP_PORT;
	DcInfo.doorModeInputWaitTimer = INVALID_TIMER_HANDLE;
	DcInfo.auxOutActTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.ftpFirmwrTrnsfrTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.verifyApiWaitTimer = INVALID_TIMER_HANDLE;
	DcInfo.installationwaitTimerhandle = INVALID_TIMER_HANDLE;
	DcInfo.showAttendanceDetailTimer = INVALID_TIMER_HANDLE;
	DcInfo.sysTestScreenWaitTimerhandle = INVALID_TIMER_HANDLE;
	DcInfo.DisplayFRStreamToIdleScreenTimer = INVALID_TIMER_HANDLE;
	DcInfo.displayFRStreamTimer = INVALID_TIMER_HANDLE;
	DcInfo.usrApproachWaitTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.usrTemperatureDetectTmrHandle = INVALID_TIMER_HANDLE;

	SmartCardKeyUpdateDetail.smartCardKeyChangeF = FALSE;
	SmartCardKeyUpdateDetail.newKeyType = MATRIX_SMART_CARD_KEY;
	SmartCardKeyUpdateDetail.oldKeyType = MATRIX_SMART_CARD_KEY;
	SmartCardKeyUpdateDetail.reqRecFromAPI = FALSE;
	SmartCardKeyUpdateDetail.keyChngWaitTmrHandle = INVALID_TIMER_HANDLE;
	SmartCardKeyUpdateDetail.cardId = 0;
	SmartCardKeyUpdateDetail.cardType = UNKNOWN_CARD;

	CafeBalReadWriteDetail.balance = 0;
	CafeBalReadWriteDetail.cafeBAlReadF = FALSE;
	CafeBalReadWriteDetail.cafeBalReadWriteInProgressState = STATE_INACTIVE;
	CafeBalReadWriteDetail.cafeBalReadWriteResp = WEB_API_RESP_SUCCESS;
	CafeBalReadWriteDetail.cafeBalReadWriteWaitTmr = INVALID_TIMER_HANDLE;
	CafeBalReadWriteDetail.cardNo1 = 0;
	CafeBalReadWriteDetail.cardNo2 = 0;
	CafeBalReadWriteDetail.cardId = 0;
	CafeBalReadWriteDetail.cardType = UNKNOWN_CARD;
	memset(&CardFieldsReadWriteDetail, CLEAR, sizeof(CardFieldsReadWriteDetail));

	// All timer handles are invalid now, so we don't need to initalize them again in door activity
	InitDoorActivityInfo(TRUE);

	DcMonitorControl.doorLock = DOOR_LOCK_NORMAL;
	DcMonitorControl.doorSenseDsbl = DOOR_STATUS_NORMAL;
	DcMonitorControl.auxInDsbl = DOOR_STATUS_NORMAL;
	DcMonitorControl.auxOutDsbl = DOOR_STATUS_NORMAL;
	DcMonitorControl.DcIoStatus.curDoorSenseStatus = DOOR_CLOSE;
	DcMonitorControl.DcIoStatus.curAuxInputStatus = AUX_INPUT_INACTIVE;
	DcMonitorControl.DcIoStatus.curAuxOutputStatus = IO_INACTIVE;
	DcMonitorControl.DcIoStatus.curDoorTamperStatus = IO_INACTIVE;
	DcMonitorControl.DcIoStatus.curExitSwitchStatus = IO_INACTIVE;

	InitApbList();
	InitAbsenteeRule();
	InitEnrollmentActivity();
	InitMenuActivity();
	InitDeniedSmartRouteList();
	InitDeniedSmartRouteSIUser();

	DcInfo.lastDetectedCard.cardId = 0;
	DcInfo.lastDetectedCard.readerId = MAX_READERS;

	DcInfo.userFpcnt = 0;
	DcInfo.internalFpMemFullF = FALSE;
	DcInfo.externalFpMemFullF = FALSE;
	currentTnAScreen = TA_MAX_SCREEN;
	DcInfo.lastDoorLevel = 0;
	DcInfo.userRouteReaderMode  = MAX_READER_MODE;
	DcInfo.accessPolicyCheckState = MAX_POLICY_CHECK;
	DcInfo.SSAPolicyCheckState = MAX_POLICY_CHECK;

	DcInfo.multiInputWaitTmrHandle = INVALID_TIMER_HANDLE;
	DcInfo.pinChangeModeState = PIN_CHANGE_IDLE_STATE;
	DcInfo.keyCnt = 0;
	memset(DcInfo.keyBuf, CLEAR, MAX_PIN_SIZE);
	DcInfo.firstInResetTimer = INVALID_TIMER_HANDLE;
	DcInfo.doorOpenCloseEventWaitTimer = INVALID_TIMER_HANDLE;
	DcInfo.userIdDoorOpenCloseEvent = INVALID_USER_ID;
	memset(DcInfo.alphanumIddoorOpenCloseEvent, 0, sizeof(DcInfo.alphanumIddoorOpenCloseEvent));
	DcInfo.isLcdTimerRunning = FALSE;
    DcInfo.isUserAuthenticatedF = FALSE;
    CurrStatusbarData.statusBarExpandedF = FALSE;
	DcInfo.waitForFrStreamtmrHandle = INVALID_TIMER_HANDLE;
    DcInfo.delayInActive = FALSE;
}

//*****************************************************************************
//	InitDoorActivityInfo()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function initializes Door activity structure.
//*****************************************************************************
static void InitDoorActivityInfo(BOOL initAllF)
{
	UINT8 index;
	DcInfo.userIdx = INVALID_USER;
 	DcInfo.userType = NORMAL_USER;
 	DcInfo.authReason = AUTH_ALLOWED;	// initialize authentication reason as allowed
	DcInfo.firstUserIdx = INVALID_USER;
	DcInfo.firstUsrGrpDtl = MAX_TWO_PERSON_USER_GRP_DTL;
	DcInfo.zamUserIdx = INVALID_USER;
	DcInfo.firstInTempLockF = TRUE;
	InitCredInfo();
	DcInfo.duressF = NO;
	DcInfo.freeFpScanStatusF = ENABLED;
	DcInfo.verifyingFpByScanF = CLEAR;
	DcInfo.fpBeingProcessed = FALSE;
	DcInfo.dcTaskIdleF = TRUE;
	EnrollmentTempInputs.enrollRepeatF = FALSE;
	// Do not init following information if 'initALlF' is not set
	if(initAllF == TRUE)
	{
		DcInfo.splFnId = MAX_SPECIAL_FUNC;
	}
	SetUserConfig(SSA_USER_IDX, DfltUserConfig);
	DcInfo.designation[0] = 0;
	DcInfo.accessLevel = 0;
	DcInfo.procSSAF = FALSE;
	DcInfo.sendAphanumIdInEvent = FALSE;
	DcInfo.splFnThroughCard = FALSE;
	DcInfo.smartRouteActiveF = FALSE;
	DcInfo.routeUserAllowReason = MAX_USER_ALLOW_ROUTE;
	DcInfo.bypassBioCred = FALSE;
	DcInfo.wiegandOutProcessF = WIGND_OUT_PROCESS_IDLE;
	DcInfo.vamConfigIndex = INVALID_VAM_CONFIG_INDEX;
	DcInfo.detectedVamCardType = MAX_VAM_CARD_TYPE;
	for(index =0; index < MAX_VAM_CARD_TYPE; index++)
	{
		DcInfo.detectedVamCards[index] = FALSE;
	}
	DcInfo.userDetectCnt = 0;
	DcInfo.allowWithoutFinalCheck = FALSE;
	DcInfo.occupancyNextUsrWaitTimer = INVALID_TIMER_HANDLE;
	DcInfo.firstOccupantIdx = INVALID_USER;
	DcInfo.firstOccupantIdx = INVALID_USER;
	DcInfo.firstOccupantAuthReason = AUTH_ALLOWED;
	DcInfo.selectedJobIdx = INVALID_JCM_CONFIG_INDEX;
	DcInfo.jobInputWaitTimer = INVALID_TIMER_HANDLE;
	DcInfo.spFnInputWaitTimer = INVALID_TIMER_HANDLE;
	DcInfo.showDoorModeOnAllowF = FALSE;
	DcInfo.doorModeInputWaitTimer = INVALID_TIMER_HANDLE;
	DcInfo.accessPolicyCheckState = MAX_POLICY_CHECK;
	DcInfo.doorEvtMode = MAX_USER_EVENT_INFO;
	DcInfo.SSAPolicyCheckState = MAX_POLICY_CHECK;
	DcInfo.cardCredntlDetectedF = FALSE;
	memset(&DcInfo.lastFpDataSendToServer, 0 , sizeof(DcInfo.lastFpDataSendToServer));
	DcInfo.pinChangeModeState = PIN_CHANGE_IDLE_STATE;
	DcInfo.keyCnt = 0;
	memset(DcInfo.keyBuf, CLEAR, MAX_PIN_SIZE);
	DcInfo.enrollSource = MAX_ENROLL_SOURCE;
	IsFaceAlreadyIdentifyF = FALSE;
	IsAdapativeErlRunningF = FALSE;
	DcInfo.bleAccessDevice = BLE_ACCESS_DEVICE_MAX;
	DcInfo.usrTemperature = 0.0;
	DcInfo.usrExceedTempThreshold = FALSE;
	DcInfo.qrTempUnit = MAX_TEMPERATURE_UNIT;
	DcInfo.qrAccessDecision = TEMP_ACCESS_DECISION_MAX;
	DcInfo.firstUsrTemperature = 0.0;
	DcInfo.firstUsrExceedTempThreshold = FALSE;
	DcInfo.firstUsrQrTempUnit = MAX_TEMPERATURE_UNIT;
	DcInfo.firstUsrQrAccessDecision = TEMP_ACCESS_DECISION_MAX;
	DcInfo.pinVerified = INVALID_MANDATORY_FIELD_UINT8;
#if defined(FACE_MASK_COMPULSION_ENABLE)
	DcInfo.faceMaskAccessDecision = FACE_MASK_DECISION_MAX;
	DcInfo.firstUserFaceMaskAccessDecision = FACE_MASK_DECISION_MAX;
	DcInfo.isFaceMaskViolated = FALSE;
	DcInfo.isFirstUsrFaceMaskViolated = FALSE;
    DcInfo.isUserAuthenticatedF = FALSE;
#endif
}

//*****************************************************************************
//	DcDefaultMsgHandler()
//	Param:
//		IN :	msgId 	- msgId to be processed
//				dataPtr	- points to message information
//				length 	- length of message
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//		This function is default message handler for DC task. It will process
//		any message that are common to all DC task states.
//		This function should be called from default case in all states.
//*****************************************************************************
void DcDefaultMsgHandler(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UNUSED_PARA UINT16 length)
{
	switch(msgId)
	{
	default:
		break;
	case MSG_RPL_ERL_CONFIRM_EVENT:
	{
		RPL_ERL_CONFIRM_EV_DATA_t* data = dataPtr;
		DPRINT(SYS_LOG,"Adaptive Enroll Status: [%d]",data->adaptiveErlSts);
	}break;
	case MSG_REX_IP_CHANGE_EVENT :
		{
			if ((DcInfo.curState != DC_USER_TEMPERATURE_CHECK_STATE)
				#if defined (FACE_MASK_COMPULSION_ENABLE)
					|| (DcInfo.curState != DC_FACE_MASK_CHECK_STATE)
				#endif
				)
			{
				// Update logical status of the door based on the status received
				REXInputChangeActionHandler(((REX_IP_CHANGE_EV_DATA_t *)dataPtr)->status);
			}
			break;
		}

	// Allow user to exit from external port
	case MSG_CARD_DETECTED_EVENT:
		{
			UINT8 readerId;

			// We process exit request only in MENU/ENROLLMENT/SPL FN states
			if( ( (DcInfo.curState == DC_ENROLLMENT_STATE) ||
				(DcInfo.curState == DC_MENU_STATE) ||
				(DcInfo.curState == DC_SPL_FUNC_STATE) ||
				(DcInfo.curState == DC_BACKUP_UPDATE_STATE) ||
				(DcInfo.curState == DC_HTTP_FIRMWARE_UPLOAD_STATE)  ||
				(DcInfo.curState == DC_ACMS_FIRMWARE_UPLOAD_STATE)  ||
				(DcInfo.curState == DC_SMART_CARD_KEY_UPDATE_STATE) ||
				(DcInfo.curState == DC_PANELDOOR_FIRMWARE_UPLOAD_STATE)) &&
				(SysBasicPara.application != CAFETERIA) )
			{
				readerId = ((CARD_DETECTED_EV_DATA_t *)dataPtr)->readerId;

				// If message is from external reader and reader mode is exit, process exit request
				if( (GetReaderRespPort(readerId) == EXTERNAL_PORT) && (GetReaderEntryExitMode(readerId) == EXIT_READER) )
				{
					ProcessUserOnExitOnExternal(msgId, dataPtr);
				}
			}
			break;
		}

	// Allow user to exit from external port
	case MSG_FP_DETECTED_EVENT:
		{
			UINT8 readerId;

			// We process exit request only in MENU/ENROLLMENT/SPL FN states
			if( (	(DcInfo.curState == DC_ENROLLMENT_STATE) ||
					(DcInfo.curState == DC_MENU_STATE) ||
					(DcInfo.curState == DC_SPL_FUNC_STATE) ||
					(DcInfo.curState == DC_BACKUP_UPDATE_STATE) ||
					(DcInfo.curState == DC_HTTP_FIRMWARE_UPLOAD_STATE) ||
					(DcInfo.curState == DC_ACMS_FIRMWARE_UPLOAD_STATE) ||
					(DcInfo.curState == DC_SMART_CARD_KEY_UPDATE_STATE)||
					(DcInfo.curState == DC_PANELDOOR_FIRMWARE_UPLOAD_STATE) )
				&& 	(SysBasicPara.application != CAFETERIA) )
			{
				readerId = ((FP_DETECTED_EV_DATA_t *)dataPtr)->readerId;

				// If message is from external reader and reader mode is exit,
				// process exit request
				if( (GetReaderRespPort(readerId) == EXTERNAL_PORT) && (GetReaderEntryExitMode(readerId) == EXIT_READER) )
				{
					ProcessUserOnExitOnExternal(msgId, dataPtr);
				}
			}
			break;
		}

	case MSG_FACE_DETECTED_EVENT:
	{
		FACE_DETECTED_EV_DATA_t * respDataPtr = dataPtr;
		DPRINT(SYS_LOG, "UserId= [%u] : status[%u] showTimeOutScreenF [%u] currState[%u]\n", respDataPtr->userId, respDataPtr->status, respDataPtr->showTimeOutScreenF, DcInfo.curState);
		break;
	}


	case MSG_AUX_IP_CHANGE_EVENT:
		{
			AuxInputChangeActionHandler(((AUX_IN_CHANGE_EV_DATA_t *)dataPtr) -> status);
			break;
		}

	case MSG_DOOR_SENSE_CHANGE_EVENT:
		{
			// Update logical status of the door based on the status received
			DoorSenseInputChangeActionHandler(((DOOR_SENSE_CHANGE_EV_DATA_t *)dataPtr)->status);
			break;
		}

	case MSG_READER_TAMPER_CHANGE_EVENT:
	case MSG_TAMPER_IP_CHANGE_EVENT:
		{
			TamperInputChangeActionHandler(((TAMPER_IP_CHANGE_EV_DATA_t *)dataPtr)->status);
			break;
		}

	case MSG_ENROLL_BY_TEMPLATE_RESP:
		{
			ENROLL_BY_TEMPLATE_RESP_DATA_t * respDataPtr = dataPtr;

			DPRINT(DC_LOG, "MSG_ENROLL_BY_TEMPLATE_RESP: reader%d, userId %d, fpIndex[%d], status = %s",
							 (respDataPtr -> readerId+1), respDataPtr -> fprUserId, respDataPtr -> fpIndex, ReaderRespStatusStr[respDataPtr -> status]);

			// If not success then consider it's an error
			if(respDataPtr -> status != READER_RESP_STATUS_SUCCESS)
			{
				EPRINT(DC_LOG, "ERROR: MSG_ENROLL_BY_TEMPLATE_RESP: reader%d, userId %d, fpIndex[%d], status = %s",
						 (respDataPtr -> readerId+1), respDataPtr -> fprUserId, respDataPtr -> fpIndex, ReaderRespStatusStr[respDataPtr -> status]);

				if(respDataPtr->status == READER_RESP_STATUS_MEM_FULL)
				{
					if(GetReaderRespPort(respDataPtr->readerId) == INTERNAL_PORT)
					{
						DcInfo.internalFpMemFullF = TRUE;
					}
					else
					{
						DcInfo.externalFpMemFullF = TRUE;
					}
				}
			}
			if((IsTempUserApplicableForFPEnrollment() == YES) && (GetEnrollFpCmdSentFlag() == YES) 
			  && (GetReaderRespPort(respDataPtr->readerId) == INTERNAL_PORT)  && (respDataPtr-> enrollmentSource == ENROLL_SOURCE_TEMP_USER) )
			{
				TempUserProcessAction(MSG_TEMP_USER_ENROLL_BY_TEMPLATE_RESP , respDataPtr);
			}
			else if(respDataPtr-> enrollmentSource == ENROLL_SOURCE_TEMP_USER)
			{
				if(respDataPtr -> status == READER_RESP_STATUS_SUCCESS)
				{
					DPRINT(DC_LOG,"Temp User Finger Enrolled Successfully");
				}
			}
			else
			{
				AcmsProcFpResp(MSG_ENROLL_BY_TEMPLATE_RESP ,respDataPtr);
			}
			break;
		}

	case MSG_DELETE_FP_TEMPLATES_RESP:
		{
			DELETE_FP_TEMPLATES_RESP_t * respDataPtr = dataPtr;

			DPRINT(DC_LOG, "MSG_DELETE_FP_TEMPLATES_RESP: reader%d, userId %d, status = %s",
							 (respDataPtr -> readerId+1), respDataPtr -> fprUserId, ReaderRespStatusStr[respDataPtr -> status]);

			if(respDataPtr -> status == READER_RESP_STATUS_SUCCESS)
			{
				DcInfo.internalFpMemFullF = FALSE;
				DcInfo.externalFpMemFullF = FALSE;
			}

			if((IsTempUserApplicableForFPEnrollment() == YES) && (GetDelFpCmdSentFlag() == YES) && (GetReaderRespPort(respDataPtr->readerId) == INTERNAL_PORT))
			{
				TempUserProcessAction(MSG_TEMP_USER_DELETE_FP_TEMPLATES_RESP , respDataPtr);
			}
			else
			{
				AcmsProcFpResp(MSG_DELETE_FP_TEMPLATES_RESP ,respDataPtr);
			}
			break;
		}

		// Handle Fingerprint reader firmware upgrade response
	case MSG_SFPR_FW_UPGRADE_START_RESP:
		{
			if( ((SFPR_FW_UPGRADE_START_RESP_DATA_t *)dataPtr)->status == READER_RESP_STATUS_SUCCESS)
			{
				SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
			}
			break;
		}

	case MSG_READ_SFPR_PARA_RESP:
		{
			READER_ID_e 	readerId;
			readerId =  ( (SET_FPR_PARA_RESP_t * )dataPtr)->readerId;
			paraValue[readerId] = ( (SET_FPR_PARA_RESP_t * )dataPtr)->paraValue;
			break;
		}

		case MSG_BLE_QR_CODE_DETECTED:
		case MSG_BT_ACCESS_REQ_DETECTED:
		{
			DPRINT(DC_LOG, "Found Device Busy for proc QR or MSG_BT_ACCESS_REQ_DETECTED");
			DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			
			if(DcInfo.curState != DC_GET_USER_STATUS_FROM_PANEL_STATE)
			{
				ACS_BLE_ACCESS_REQ_RESPONSE_t	respBtData;

				respBtData.readerId = READER1;
				respBtData.respReason = DcInfo.qrAuthReason;
				respBtData.respStatus = DcInfo.qrResponseMode;

				SendBLEEventTask(APTA_BLE_ACCESS_REQ_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			}
			break;
		}

		case MSG_BLE_ACCESS_EVENT:
		{
			CB_READER_ACCESS_REQ_RESPONSE_t		respBtData;

			DPRINT(DC_LOG, "Found Device Busy for proc QR or MSG_BT_ACCESS_REQ_DETECTED");
			DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;

			respBtData.readerId = READER4;
			respBtData.respReason = DcInfo.qrAuthReason;
			respBtData.respStatus = DcInfo.qrResponseMode;

			ProcessReaderCommand(MSG_CB_BLE_ACCESS_RESP_CMD, &respBtData, sizeof(CB_READER_ACCESS_REQ_RESPONSE_t));
		}
		break;

		case MSG_ACCESS_ID_REQ_EVENT:
		{
			MOBILE_ACCESS_REQ_t 		*mblAccessRqPtr =(MOBILE_ACCESS_REQ_t *)dataPtr;

			DPRINT(DC_LOG, "Found Device Busy for proc QR or MSG_BT_ACCESS_REQ_DETECTED");

			DcInfo.readerId = mblAccessRqPtr->readerId;
			DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;

			if(DcInfo.curState != DC_GET_USER_STATUS_FROM_PANEL_STATE)
			{
				ACS_BLE_ACCESS_REQ_RESPONSE_t	respBtData;
				
				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = DcInfo.qrAuthReason;
				respBtData.respStatus = DcInfo.qrResponseMode;

				SendBLEEventTask(BLE_ACCESS_REQ_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			}
		}
		break;

		case MSG_MOBILE_PIN_EVENT:
		{
			MOBILE_ACCESS_PIN_REQ_t 	*mblAccessRqPtr =(MOBILE_ACCESS_PIN_REQ_t *)dataPtr;

			DPRINT(DC_LOG, "Found Device Busy for processing ACS pin event from ACS Application");

			DcInfo.readerId = mblAccessRqPtr->readerId;
			DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;

			if(DcInfo.curState != DC_GET_USER_STATUS_FROM_PANEL_STATE)
			{
				ACS_BLE_ACCESS_REQ_RESPONSE_t	respBtData;
				
				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = DcInfo.qrAuthReason;
				respBtData.respStatus = DcInfo.qrResponseMode;

				SendBLEEventTask(INT_BLE_PIN_ACCESS_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			}

		}
		break;

		case MSG_MOBILE_FACE_EVENT:
		{
			MOBILE_ACCESS_FACE_REQ_t 	*mblAccessRqPtr =(MOBILE_ACCESS_FACE_REQ_t *)dataPtr;

			DPRINT(DC_LOG, "Found Device Busy for processing ACS Face event from ACS Application");

			DcInfo.readerId = mblAccessRqPtr->readerId;
			DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;

			if(DcInfo.curState != DC_GET_USER_STATUS_FROM_PANEL_STATE)
			{
				ACS_BLE_ACCESS_REQ_RESPONSE_t	respBtData;
				
				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = DcInfo.qrAuthReason;
				respBtData.respStatus = DcInfo.qrResponseMode;

				SendBLEEventTask(INT_BLE_FACE_ACCESS_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			}

		}
		break;

		case MSG_PIN_DETECTED_ON_EXTERNAL_READER_EVENT:
		{
			DPRINT(DC_LOG,"External Pin received");
		}break;

    case GUI_BUTTON_SET_ETHERNET_CFG:
        if(IsUserAuthenticated() == TRUE)
        {
            ChangeMenuInfoStateToAdmin();
            DisplayMenuScreen(MENU_LAN_SETTINGS_SCREEN);
        }
        break;

    case GUI_BUTTON_SET_SERVER_CFG:
        if(IsUserAuthenticated() == TRUE)
        {
            ChangeMenuInfoStateToAdmin();
            DisplayMenuScreen(MENU_SERVER_SETTINGS_SCREEN);
        }
        break;

    case GUI_BUTTON_BROADBAND_CFG:
        if(IsUserAuthenticated() == TRUE)
        {
            ChangeMenuInfoStateToAdmin();
            DisplayMenuScreen(MENU_BROADBAND_PROFILE_SCREEN);
        }
        break;

    case GUI_BUTTON_SET_WIFI_CFG:
        if(IsUserAuthenticated() == TRUE)
        {
            ChangeMenuInfoStateToAdmin();
            DisplayMenuScreen(MENU_WIFI_SETTINGS_SCREEN);
        }
        break;

    case MSG_AUTH_STATUSBAR_MENU_ACCESS_EVENT:
    {
        AUTH_STATUSBAR_MENU_ACCESS_t 	*MsgidPassword =(AUTH_STATUSBAR_MENU_ACCESS_t *)dataPtr;

        if(CompareDisplayPswd(USER_ADMIN, MsgidPassword->loginPw) == FAIL)
        {
            SetAudioVisual(USER_RESP_MENU_LOGIN_FAILED, INTERNAL_PORT, NULL);
            LoadMenuWaitTimer(MENU_USER_RESP_TIMER, &DisplayMenuScreen, MENU_LOGIN_SCREEN);
        }
        else
        {
            SwitchDcToMenuState();
            ChangeMenuInfoStateToAdmin();
            DcInfo.isUserAuthenticatedF=TRUE;
            switch (MsgidPassword->msgId)
            {
            case GUI_BUTTON_SET_ETHERNET_CFG:
                DisplayMenuScreen(MENU_LAN_SETTINGS_SCREEN);
                break;

            case GUI_BUTTON_SET_SERVER_CFG:
                DisplayMenuScreen(MENU_SERVER_SETTINGS_SCREEN);
                break;

            case GUI_BUTTON_BROADBAND_CFG:
                DisplayMenuScreen(MENU_BROADBAND_PROFILE_SCREEN);
                break;

            case GUI_BUTTON_SET_WIFI_CFG:
                DisplayMenuScreen(MENU_WIFI_SETTINGS_SCREEN);
            }
        }
        break;
    }
    
	case MSG_READ_TEMPLATE_RESP:
	{
	      READ_TEMPLATE_RESP_DATA_t * readFpRespPtr = dataPtr;
	      
	      DPRINT(DC_LOG, "MSG_READ_TEMPLATE_RESP: reader%d, userId %d, status = %s",
							 (readFpRespPtr -> readerId+1), readFpRespPtr -> fprUserId, ReaderRespStatusStr[readFpRespPtr -> status]);
	      
	      if((IsTempUserApplicableForFPEnrollment() == YES) && (GetReadFpCmdSentFlag() == YES) && (GetReaderRespPort(readFpRespPtr->readerId) == INTERNAL_PORT))
	      {
		      TempUserProcessAction(MSG_TEMP_USER_READ_TEMPLATE_RESP , readFpRespPtr);
	      }
	      break;
	}      
    
	}
}

//*****************************************************************************
//	DeleteTimersForIdleState()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//			Delete all timers that may be running in different states along with
//*****************************************************************************
void DeleteTimersForIdleState(void)
{
	// 'inter digit timer handle'
	DeleteTimer(&DcInfo.interDigitTmrHandle[INTERNAL_PORT]);
	DeleteTimer(&DcInfo.interDigitTmrHandle[EXTERNAL_PORT]);
	DeleteTimer(&DcInfo.multiAccessTmrHandle);
	DeleteTimer(&DcInfo.twoPersonTmrHandle);
	DeleteTimer(&DcInfo.fpMatchTmrHandle);
	DeleteTimer(&DcInfo.selfEnrlInputTimerHandle);
	DeleteTimer(&DcInfo.userAllowWaitTmrHandle);
	DeleteTimer(&DcInfo.vamCredWaitTimer);
	DeleteTimer(&(DcInfo.occupancyNextUsrWaitTimer));
	DeleteTimer(&(DcInfo.jobInputWaitTimer));
	DeleteTimer(&(DcInfo.spFnInputWaitTimer));
	DeleteTimer(&(DcInfo.doorModeInputWaitTimer));
	DeleteTimer(&CafeBalReadWriteDetail.cafeBalReadWriteWaitTmr);
	DeleteTimer(&CardFieldsReadWriteDetail.cardFieldReadWriteWaitTmr);
	if(SmartCardKeyUpdateDetail.smartCardKeyChangeF == FALSE)
	{
		DeleteTimer(&SmartCardKeyUpdateDetail.keyChngWaitTmrHandle);
		DeleteTimer(&DcInfo.SmrtCardKeyChngTmrHandle);
	}
	DeleteTimer(&(DcInfo.multiInputWaitTmrHandle) );
	DeleteTimer(&(DcInfo.ftpFirmwrTrnsfrTmrHandle));
	DeleteTimer(&DcInfo.verifyApiWaitTimer);
	DeleteTimer(&DcInfo.usrApproachWaitTmrHandle);
	DeleteTimer(&DcInfo.usrTemperatureDetectTmrHandle);
	DeleteTimer(&AddTempUserTmrHandle);
	DeleteTimer(&TempUserPopupTmrHandle);
	DeleteTimer(&TempUserProcessingTmrHandle);
	DeleteTimer(&SpcReqCnfmWaitTmrHandle);
}

//*****************************************************************************
//	SwitchDcToIdleState()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//		This function makes all the necessary changes for door to switch in
//		idle mode.
//*****************************************************************************
void SwitchDcToIdleState(void)
{
	ABORT_FINGER_SCAN_CMD_DATA_t	abortScan;

	// Delete all timers that may be running in different states along with
	// 'inter digit timer handle'
	DeleteTimersForIdleState();

	// Check if system is waiting for user finger to be scanned, if so then abort scanning
	if( (DcInfo.verifyingFpByScanF == SET) || (DcInfo.fpBeingProcessed == TRUE) )
	{
		// Abort scanning on internal port with entry mode only
		if(IsReaderPresent(FP_READER, DcInfo.respPort, ANY_READER_MODE, &abortScan.readerId) == YES)
		{
			ProcessReaderCommand(MSG_ABORT_FINGER_SCAN_CMD, &abortScan, sizeof(ABORT_FINGER_SCAN_CMD_DATA_t));
		}
	}

	DcInfo.fCInCSN = 0;
	DcInfo.fCInCardMem = 0;

	InitDoorActivityInfo(TRUE);
	if(SysBasicPara.application == CAFETERIA)
	{
		SwitchToCafeteriaState(FALSE);
	}
	else
	{
		DcInfo.curState = DC_IDLE_STATE;
	}

    if(DcInfo.delayInActive == TRUE)
    {
        if(DcInfo.lcdIdleTmrHandle == INVALID_TIMER_HANDLE)
        {
            DcInfo.delayInActive = FALSE;
            BOOL status = getDeviceInactiveStatus();
            setDeviceInactiveStatus(status, TRUE);
        }
    }

	if(PANEL_DOOR_APPL == GetDoorApplType())
	{
		DegradeState = DGRD_IDLE_STATE;
	}

	if(SmartCardKeyUpdateDetail.smartCardKeyChangeF == TRUE)
	{
		SwitchDcToCardKeyUpdateState();
	}

	if(CafeBalReadWriteDetail.cafeBalReadWriteInProgressState == STATE_ACTIVE)
	{
		SwitchDcToCafeBalReadWriteState();
	}

	if(CardFieldsReadWriteDetail.cardFieldReadWriteInProgressState == STATE_ACTIVE)
	{
		SwitchDcToCardFieldsReadWriteState();
	}

	if(BioIdentifyServer.enblBioServer == ENABLED)
	{
		AbortServerFpIdentificationProcess();
	}
}

//*****************************************************************************
//	SwitchDcToCardKeyUpdateState()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//		This function makes all the necessary changes for door to switch in
//		smart card key update mode.
//*****************************************************************************
void SwitchDcToCardKeyUpdateState(void)
{
	BOOL showEscBtnF = TRUE;
	if(SmartCardKeyUpdateDetail.reqRecFromAPI == TRUE)
	{
		LoadKeyChangeWaitTimer(KEY_CHANGE_WAIT_TIMER);
	}

	CleanUpTempUserFP();
	DcInfo.curState = DC_SMART_CARD_KEY_UPDATE_STATE;
	SetAudioVisual(USER_RESP_KEY_CHANGE_SHOW_CARD, INTERNAL_PORT, (VOIDPTR)&showEscBtnF);
}

//*****************************************************************************
//	SwitchDcToPanelDoorFirmwareUploadState()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//		This function makes all the necessary changes for door to switch in
//		panel Door Firmware Upgrade State
//*****************************************************************************
void SwitchDcToPanelDoorFirmwareUploadState(void)
{
	CleanUpTempUserFP();
	DcInfo.curState = DC_PANELDOOR_FIRMWARE_UPLOAD_STATE;
}

//*****************************************************************************
//	ProcMsg()
//	Param:
//		IN :	msgId - Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length - Length of the message
//		OUT:	None
//	Returns:	None
//	Description:
//				This function processes message (events and responses) from
//				other modules based on its current state.
//*****************************************************************************
void ProcMsg(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
    if(IsPasswdValidationRqrd(msgId) == FALSE)
    {
        return;
    }

	if(GetDoorApplType() == PANEL_DOOR_APPL)
	{
		DispatchMessage(msgId, dataPtr, length);
	}
	else
	{
		// Based on current state, call appropriate state function
		// Pass all parameters to state function.
		(*dcFsmFuncPtr[DcInfo.curState])(msgId, dataPtr, length);
	}
}

//*****************************************************************************
//	DcIdleStateFn()
//	Param:
//		IN: msgId 	- Message which needs to be processed
//			dataPtr - Pointer to data of the message
//			length 	- Length of the message
//		OUT: None
//	Returns:
//		NONE
//	Description:
//		This function processes User access related events.
//*****************************************************************************
static void DcIdleStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	// Based on message ID, process it
	UINT8 activeSpecialFuncIndex = 0, activeSpecialFunCnt = 0;
	switch(msgId)
	{
		//idle
		case MSG_RPL_ERL_CONFIRM_EVENT:
		{
			RPL_ERL_CONFIRM_EV_DATA_t* data = dataPtr;
			switch(data->adaptiveErlSts)
			{
			case ADAPTIVE_ERL_CONFIRM:
			{
				UINT16 userIdx;
				IsUserValid(data->userId, &userIdx);
				strncpy(AdaptiveFrQuery.usrId, GetAlphaNumId(userIdx), ALPHANUM_ID_SIZE);
				strncpy(AdaptiveFrQuery.usrName, GetUserName(userIdx), NAME_SIZE_UTF8);
				DPRINT(DC_LOG,"RPL_ERL Confirm recvd in DC_IDLE:UID:[%u] FID[%d] %s : %s",data->userId, data->faceIdx ,AdaptiveFrQuery.usrId,AdaptiveFrQuery.usrName);
				DcInfo.userIdx = userIdx;
				DcInfo.faceIdx = data->faceIdx;
				StopFRStreamForRplErlConfirm();
				SendAdaptiveConfirmScreenToQt();
			}break;
			case ADAPTIVE_ERL_CONFIRM_RESP_FAIL:
				StopFRStreamForRplErlConfirm();
				DcInfo.authReason = AUTH_ADAPTIVE_ERL_REJECT;
				SetAudioVisual(USER_RESP_ACCESS_DENIED, INTERNAL_PORT, NULL);
				SwitchDcToIdleState();
				break;

			case ADAPTIVE_ERL_CONFIRM_RESP_TIMEOUT:
				StopFRStreamForRplErlConfirm();
				DcInfo.authReason = AUTH_ADAPTIVE_ERL_TIMEOUT;
				SetAudioVisual(USER_RESP_ACCESS_DENIED, INTERNAL_PORT, NULL);
				SwitchDcToIdleState();
				break;

			case ADAPTIVE_ERL_CONFIRM_NO:
				SwitchDcToIdleState();
				DisplayIdleScreen(0);
				SetBuzLedResp(USER_RESP_IDLE,INTERNAL_PORT);
				break;
			case ADAPTIVE_ERL_CONFIRM_TIMEOUT:
				SwitchDcToIdleState();
				DisplayIdleScreen(0);
				SetBuzLedResp(USER_RESP_IDLE,INTERNAL_PORT);
				break;
			case ADAPTIVE_ERL_CONFIRM_YES:
			case ADAPTIVE_ERL_CONFIRM_RESP_SUCCESS:
			case MAX_ADAPTIVE_ERL_STATUS:
			default:
				break;
			}
		}break;
		case GUI_BUTTON_GOING_IN:
			if(IsDoorBusy() == NO)
			{
				activeSpecialFunCnt = GetGoingIN_OUTSpecialFunctionInfo(msgId,  &activeSpecialFuncIndex);
				currentTnAScreen = TA_GOING_IN_SCREEN;
				SwitchDcToMenuState();
				if(activeSpecialFunCnt > 1)
				{
					SetAudioVisual(USER_RESP_DISPLAY_GOING_IN_SCREEN, INTERNAL_PORT, NULL);
				}
				else if(activeSpecialFunCnt == 1)
				{
					DcInfo.splFnId = activeSpecialFuncIndex;
					DcInfo.respPort = INTERNAL_PORT;
					StopMenuActivity();
					if(ProcessSplFn() == FALSE)
					{
						SwitchDcToIdleState();
					}
					SetAudioVisual(USER_RESP_GET_USER_CREDENTIAL, DcInfo.respPort, NULL);
				}
				StopFRStreamingOnDeviceIdleScreen(TRUE);
			}
			break;

		case GUI_BUTTON_GOING_OUT:
			if(IsDoorBusy() == NO)
			{
				activeSpecialFunCnt = GetGoingIN_OUTSpecialFunctionInfo(msgId,  &activeSpecialFuncIndex);
				currentTnAScreen = TA_GOING_OUT_SCREEN;
				SwitchDcToMenuState();
				if(activeSpecialFunCnt > 1)
				{
					SetAudioVisual(USER_RESP_DISPLAY_GOING_OUT_SCREEN, INTERNAL_PORT, NULL);
				}
				else if(activeSpecialFunCnt == 1)
				{
					DcInfo.splFnId = activeSpecialFuncIndex;
					DcInfo.respPort = INTERNAL_PORT;
					StopMenuActivity();
					if(ProcessSplFn() == FALSE)
					{
						SwitchDcToIdleState();
					}
					SetAudioVisual(USER_RESP_GET_USER_CREDENTIAL, DcInfo.respPort, NULL);
				}
				StopFRStreamingOnDeviceIdleScreen(TRUE);
			}
			break;

		case GUI_BUTTON_MENU:
			if(IsDoorBusy() == NO)
			{
				SwitchDcToMenuState();
				SetAudioVisual(USER_RESP_DISPLAY_MENU_SCREEN, INTERNAL_PORT, NULL);
				StopFRStreamingOnDeviceIdleScreen(TRUE);
			}
			break;

		case GUI_ALNUM_KEEP_ALIVE:
		case GUI_BUTTON_FIELD_CLEAR:
		{
			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;

			DcInfo.respPort = pinRespPort;
			// Is user entering PIN then only reload timer
			if(DcInfo.interDigitTmrHandle[pinRespPort] != INVALID_TIMER_HANDLE)
			{
				if(pinRespPort == INTERNAL_PORT)
				{
					LoadInterDigitTimer();
					// Reload display idle timer
					SetLcdIdleTimer(SysAdvPara.interDigitTimer);
				}
				else if(pinRespPort == EXTERNAL_PORT)
				{
					LoadInterDigitTimerForExternal();
					// Reload display idle timer
					SetReaderLcdIdleTimer(SysAdvPara.interDigitTimer);
				}
			}
			else if( (pinRespPort == EXTERNAL_PORT) && (YES == CheckComboRdrPresent(COMBO_READER_ATOM_RD100)) )
			{
				LoadInterDigitTimerForExternal();
			}
			break;
		}

		case GUI_BUTTON_CANCEL:
		case GUI_BUTTON_HOME:
		{
 			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;
			DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
			if( (pinRespPort == INTERNAL_PORT)   || ((NO == CheckComboRdrPresent(COMBO_READER_ATOM_RD100)) &&(pinRespPort == EXTERNAL_PORT )))
			{
			      SetAudioVisual(USER_RESP_IDLE, pinRespPort, NULL);
			}
			DontShowAttendanceDetail();
			SwitchDcToIdleState();
			if(pinRespPort == INTERNAL_PORT)
			{
				StopFRStreamingOnDeviceIdleScreen(TRUE);
			}
			break;

		}
		case GUI_BUTTON_BACK:
		{
			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;
			if(DcInfo.splFnThroughCard == TRUE)
			{
				DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
				SetAudioVisual(USER_RESP_IDLE, pinRespPort, NULL);
				SwitchDcToIdleState();
			}
			else
			{
				switch(currentTnAScreen)
				{
					default:
						break;

					case TA_GOING_IN_SCREEN:
						activeSpecialFunCnt = GetGoingIN_OUTSpecialFunctionInfo(GUI_BUTTON_GOING_IN,  &activeSpecialFuncIndex);
						if(activeSpecialFunCnt > 1)
						{
							//Going-In Screen
							SetAudioVisual(USER_RESP_BUZ_AUDIO_IDLE, INTERNAL_PORT, NULL);
							DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
							SwitchDcToMenuState();
							SetAudioVisual(USER_RESP_DISPLAY_GOING_IN_SCREEN, INTERNAL_PORT, NULL);
						}
						else
						{
							//Home Screen
							DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
							SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
							SwitchDcToIdleState();
						}
					break;

					case TA_GOING_OUT_SCREEN:
						activeSpecialFunCnt = GetGoingIN_OUTSpecialFunctionInfo(GUI_BUTTON_GOING_IN,  &activeSpecialFuncIndex);
						if(activeSpecialFunCnt > 1)
						{
							//Going-Out Screen
							SetAudioVisual(USER_RESP_BUZ_AUDIO_IDLE, INTERNAL_PORT, NULL);
							DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
							SwitchDcToMenuState();
							SetAudioVisual(USER_RESP_DISPLAY_GOING_OUT_SCREEN, INTERNAL_PORT, NULL);
						}
						else
						{
							//Home Screen
							DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
							SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
							SwitchDcToIdleState();
						}
						break;
					}
				}
				if(pinRespPort == INTERNAL_PORT)
				{
					StopFRStreamingOnDeviceIdleScreen(TRUE);
				}
			break;
		}

		case GUI_BUTTON_PIN_ENTER_SCREEN:
		{
			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;

			if(PinEntryPossible(pinRespPort) == YES)
			{
				// Set Response Port and Reader Mode
				DcInfo.respPort = pinRespPort;
				if(DcInfo.respPort == INTERNAL_PORT)
				{
					UpdateDcInfoReaderMode(READER1);
					LoadInterDigitTimer();
					StopFRStreamingOnDeviceIdleScreen(FALSE);
				}
				else
				{
					UpdateDcInfoReaderMode(READER3);
					LoadInterDigitTimerForExternal();
				}
				//DcInfo.readerMode = GetReaderEntryExitMode(READER1);
				// Delete timer used to get user input for special function to process single user
				DeleteTimer(&(DcInfo.splFnGetUserInputTmrHandle));
				SetAudioVisual(USER_RESP_ENTER_PIN, pinRespPort, NULL);
			}
			break;
		}

		case MSG_USB_STATUS_EVENT:
		{
			if(IsDoorBusy() == NO)
			{
				USB_STATUS_EV_DATA_t * evData = dataPtr;

				switch(evData->status)
				{
					case USB_DISCONNECTED:
						SetAudioVisual(USER_RESP_USB_DISCONNECTED, INTERNAL_PORT, dataPtr);
						break;

					case USB_CONNECTED:
						SetAudioVisual(USER_RESP_USB_CONNECTED, INTERNAL_PORT, dataPtr);
						break;

					case USB_INVALID_FS:
						SetAudioVisual(USER_RESP_USB_INVALID_FS, INTERNAL_PORT, dataPtr);
						break;

					default:
						break;
				}
			}
			break;
		}

		case MSG_PIN_DETECTED_ON_WGND_EVENT:
		{
			PIN_RESULT_e pinResp;

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetectOnWgnd((WGND_PIN_DATA_t *)dataPtr);

			if(IsDeviceActive() != TRUE)
			{
				ShowEntryRestrictedForDeviceInactive();
				break;
			}

			if(pinResp == PIN_MATCH)
			{
				DeleteTimer(&(DcInfo.interDigitTmrHandle[EXTERNAL_PORT]));
				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			else if(pinResp == PIN_CHANGE_CODE_MATCH)
			{
				DeleteTimer(&(DcInfo.interDigitTmrHandle[EXTERNAL_PORT]));
				SwitchDcToPinChangeState();
			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		// Verify the complete PIN entered by user
		case MSG_PIN_VALIDATION_EVENT:
		{
			PIN_RESULT_e pinResp;

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetect((PIN_DATA_t *)dataPtr);

			if(IsDeviceActive() != TRUE)
			{
				ShowEntryRestrictedForDeviceInactive();
				break;
			}

			if(pinResp == PIN_MATCH)
			{
				DeleteTimer(&(DcInfo.interDigitTmrHandle[INTERNAL_PORT]));
				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
                		ProcessUser(DcInfo.readerMode);
			}
			else if(pinResp == PIN_CHANGE_CODE_MATCH)
			{
				DeleteTimer(&(DcInfo.interDigitTmrHandle[INTERNAL_PORT]));
				SwitchDcToPinChangeState();
			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case MSG_SCAN_TEMPLATE_RESP:
		case MSG_FP_DETECTED_EVENT:
		{
			FP_RESULT_e	fpResp;
			FP_DETECTED_EV_DATA_t * fpDetail = (FP_DETECTED_EV_DATA_t *)dataPtr;

			DontShowAttendanceDetail();
			// Process Finger Print detected to find a match
			if(msgId == MSG_SCAN_TEMPLATE_RESP)
			{
				fpResp = ProcessFpScanTemplateResp(dataPtr);
			}
			else
			{
				fpResp = ProcessFpDetect((FP_DETECTED_EV_DATA_t *)dataPtr);
			}

			if(IsDeviceActive() != TRUE)
			{
				if(fpResp == FP_CONTINUE)
				    break;
			  
				ShowEntryRestrictedForDeviceInactive();
				break;
			}

			if(fpResp == FP_MATCH)
			{
				if(DcInfo.userIdx == SSA_USER_IDX)
				{
					ProcessUserForBioSI();
				}
				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				else
				{
					ProcessUser(DcInfo.readerMode);
				}
			}
			// No match found or Timeout for identification
			else if( (fpResp == FP_INVALID) || (fpResp == FP_MATCHING_TIMEOUT) )
			{
				if(fpResp == FP_MATCHING_TIMEOUT)
				{
					DcInfo.authReason = AUTH_FP_MATCHING_TIMEOUT;
				}
				else
				{
					DcInfo.authReason = AUTH_INVALID_FINGER;
				}
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(fpResp == FP_NOT_MATCH)
			{
				DPRINT(DC_LOG,"FP_NOT_MATCH %d\n",fpDetail -> fprUserId);
				if(ENABLED == BioIdentifyServer.enblBioServer)
				{
					DcInfo.authReason = AUTH_INVALID_FINGER_TEMP_USER;
					DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				}
				else
				{
					CheckForTempUserProcessFP(fpDetail -> fprUserId);
				}
			}
			break;
		}

		case MSG_FACE_DETECTED_EVENT:
		{
			//Idle State
			FACE_RESULT_e faceResp;

			DontShowAttendanceDetail();

			faceResp = ProcessFaceDetect((FACE_DETECTED_EV_DATA_t *)dataPtr);

			if(IsDeviceActive() != TRUE)
			{
				break;
			}

			if(faceResp == FACE_MATCH)
			{
				IsFaceAlreadyIdentifyF = TRUE;
				curProcFaceUserIdx = DcInfo.userIdx;
				if(IsFaceRecoEnbl(DcInfo.userIdx) == DISABLED)
				{
					DPRINT(SYS_LOG, "User Denied FR Disabled");
					DcInfo.authReason = AUTH_FR_DISABLED;
					DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
					break;
				}
				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			else if(faceResp == FACE_INVALID)
			{
				if(FRServerConfig.showUnknownUserAck == true || FRServerConfig.generateUnidentifiedFaceEvent == true)
				{
					DcInfo.authReason = AUTH_INVALID_FACE;
					DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				}
			}
			else if(faceResp == FACE_MATCHING_TIMEOUT)
			{
				if(((FACE_DETECTED_EV_DATA_t *)dataPtr) -> showTimeOutScreenF == TRUE)
				{
					SetAudioVisual(USER_RESP_TIMEOUT, INTERNAL_PORT, NULL);
				}
				SwitchDcToIdleState();
			}
			break;
		}

		case MSG_CARD_DETECTED_EVENT:
		{
			CARD_RESULT_e	cardResp;
			CREDENTIAL_TYPE_e credType = CARD_CREDENTIAL;

			DontShowAttendanceDetail();
			// Process card detected to find a match
			cardResp = ProcessCardDetect((CARD_DETECTED_EV_DATA_t *)dataPtr,credType);

			if(IsDeviceActive() != TRUE)
			{
				if(cardResp == SP_FUNC_CARD_MATCH)
				{
					switch(DcInfo.splFnId)
					{
						case CLK_IN_ENTRY_OFFICIAL_WORK: // Clock-IN as Entry after Official work
						case CLK_OUT_EXIT_OFFICIAL_WORK: // Clock-OUT as exit for Official work
						case SHORT_LEAVE_MARKING_IN:     // Short Leave IN
						case SHORT_LEAVE_MARKING_OUT:    // Short Leave OUT(single user)
						case MARK_CLOCK_IN:              // Clock-IN
						case MARK_CLOCK_OUT:             // Clock-OUT
						case CLK_IN_MARKING_BREAK:       // Clock-IN as Entry after Break
						case CLK_OUT_MARKING_BREAK:      // Clock-OUT as exit for Break
						case OVER_TIME_MARKING_IN:       // Clock-IN for over-time
						case OVER_TIME_MARKING_OUT:      // Clock-OUT after over-time
						{
							ShowEntryRestrictedForDeviceInactive();
							return;
						}break;

						default:
						{
							break;
						}
					}
				}
				else
				{
					ShowEntryRestrictedForDeviceInactive();
					return;
				}
			}

			if(cardResp == USER_CARD_MATCH)
			{
				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			else if(cardResp == SP_FUNC_CARD_MATCH)
			{
				DcInfo.splFnThroughCard = TRUE;
				// process special function
				if(ProcessSplFn() == FALSE)
				{
					SwitchDcToIdleState();
				}
			}
			else if(cardResp == CARD_VAM_MATCH)
			{
				// Now VAM card is identified.
				// Now process VAM card further based on entry/exit reader.
				if(DcInfo.readerMode == ENTRY_READER)
				{
					ProcessVamCardOnEntry();
				}
				else
				{
					ProcessVamCardOnExit();
				}
			}
			else if(cardResp == CARD_INVALID)  // No match found
			{
				if(IsSSAValid() == YES)
				{
					SwitchDcToSSAState();
				}
				else
				{	
					CheckTempUserProcessCard((CARD_DETECTED_EV_DATA_t *)dataPtr);
				}
			}
			break;
		}
		case MSG_BLE_QR_CODE_DETECTED:
		{
			QR_RESULT_e     qrResp;

			qrResp = ProcessQrCodeDetect((QR_CODE_DETECTED_EV_DATA_t *)dataPtr);

			if(IsDeviceActive() != TRUE)
			{
				ShowEntryRestrictedForDeviceInactive();

				DcInfo.qrAuthReason = QR_AUTH_ENTRY_RESTRICTED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ACCESS_DENIED;
				break;
			}

			if(qrResp == QR_CODE_USER_AUTHENTICATED)
			{
				DontShowAttendanceDetail();

				ProcessUser(DcInfo.readerMode);
			}
			else if(qrResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DontShowAttendanceDetail();

				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(qrResp == QR_CODE_DEVICE_BUSY)
			{
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else if((qrResp == QR_CODE_INVALID_READER_MODE) || (QR_CODE_API_INVALID_INPUT == qrResp))
			{
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ACCESS_DENIED;
			}
			else
			{
				DontShowAttendanceDetail();

				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				DcInfo.qrAuthReason = QR_AUTH_UNAUTHORIZED_ACCESS;
			}
		}
		break;

		case GUI_BUTTON_FTP_FMW_UPGRADE_CANCEL:
		{
			if(DcInfo.ftpFirmwrTrnsfrTmrHandle != INVALID_TIMER_HANDLE)
			{
				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
				SwitchDcToIdleState();
			}
			break;
		}

		case GUI_BUTTON_FTP_FMW_UPGRADE_OK:
		{
			if(DcInfo.ftpFirmwrTrnsfrTmrHandle != INVALID_TIMER_HANDLE)
			{
				FTPFirmWareInitilizeTimerTimeout(0);
				break;
			}
			break;
		}

		case MSG_BT_ACCESS_REQ_DETECTED:
		{
			/** APTA Access Request on internal bluetooth */
			DPRINT(DC_LOG, "MSG_BT_ACCESS_REQ_DETECTED");
			QR_RESULT_e	btResp;

			btResp = ProcessBtAccessReqDetect((BT_ACCESS_REQ_DETECTED_EV_DATA_t *)dataPtr);

			if((IsDeviceActive() != TRUE) && (btResp != QR_CODE_API_DISABLED))
			{
				ShowEntryRestrictedForDeviceInactive();

				ACS_BLE_ACCESS_REQ_RESPONSE_t		respBtData;

				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = QR_AUTH_ENTRY_RESTRICTED;
				respBtData.respStatus = WEB_API_QR_CODE_RESP_ACCESS_DENIED;

				SendBLEEventTask(APTA_BLE_ACCESS_REQ_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
				break;
			}

			if(btResp == QR_CODE_USER_AUTHENTICATED)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATED");
				/* Set flag to indicate request received from ble */
				DcInfo.bleAccessDevice = BLE_ACCESS_DEVICE_APTA;
				ProcessUser(DcInfo.readerMode);
			}
			else if(btResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATION_FAILED");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(btResp == QR_CODE_DEVICE_BUSY)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_DEVICE_BUSY");
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else if(btResp == QR_CODE_INVALID_READER_MODE)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_INVALID_READER_MODE");
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ACCESS_DENIED;
				SwitchDcToIdleState();
			}
			else if(btResp == QR_DEVICE_INVALID)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_DEVICE_INVALID ");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				// CRC is mismatched, Wrong Encryption.
				DcInfo.qrResponseMode = RESP_WRONG_ENC_CRC_MISMATCH;
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			}
			else if(QR_ACCESS_MODE_RESTRICTED == btResp)
			{
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ACCESS_DENIED;
				SwitchDcToIdleState();
			}
			else
			{
				DPRINT(DC_LOG, "btResp == else(QR_CODE_API_DISABLED) ");
				DcInfo.qrResponseMode = RESP_USER_DENIED;
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			}
			
			if(DcInfo.curState != DC_GET_USER_STATUS_FROM_PANEL_STATE)
			{
				ACS_BLE_ACCESS_REQ_RESPONSE_t		respBtData;

				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = DcInfo.qrAuthReason;
				respBtData.respStatus = DcInfo.qrResponseMode;

				SendBLEEventTask(APTA_BLE_ACCESS_REQ_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			}
			break;
		}

		case GUI_BUTTON_REFRESH_ATTNDNS_DTL:
		{
			UINT16 usreIdx = *(UINT16PTR)dataPtr;
			StartUpdateAtdncDtlForSnglUsrThrd(usreIdx);
			// Reload display idle timer
			SetLcdIdleTimer(MENU_TIMER);
			break;
		}

		case GUI_BUTTON_RELOAD_ATTNDNS_DETAIL_TIMER:
		{
			UINT8 attendanceTimer = *(UINT8PTR)dataPtr;
			// On press and release, attendence hold screen for 3 sec
			// On swipe, hold screen for 5 sec
			SetLcdIdleTimer(attendanceTimer);
			DcInfo.isLcdTimerRunning = FALSE;
			break;
		}

		case GUI_BUTTON_DELETE_IDLE_SCREEN_TIMER:
		{
			// delete idle timer
			SetLcdIdleTimer(0);
			DcInfo.isLcdTimerRunning = TRUE;
			break;
		}

		case GUI_BUTTON_APPLY_LEAVE:
		{
			// delete idle timer
			SetLcdIdleTimer(11);
			StartAbsentDateLeaveApplyThread();
			break;
		}

		case GUI_BUTTON_TAP_FOR_FR:
		{

			if(StartFRStreamingOnDeviceIdleScreen() == SUCCESS)
			{
				if(FRServerConfig.faceCapturing == FACE_SERVER_TAP_AND_GO)
				{
					LoadDisplayFRStreamToIdleScreenTimer(FRServerConfig.identificationTimeOut,TRUE);
				}
				else
				{
					LoadDisplayFRStreamToIdleScreenTimer(FRServerConfig.faceScanTimeOut,FALSE);
				}
			}
			break;
		}

		case GUI_BUTTON_FR_FOOTER_FOR_SPL_FUN:
		{
			if(StartFRStreamingOnDeviceIdleScreen() == SUCCESS)
			{
				StreamPayload_t         streamInfo;
				streamInfo.reason = START_STREAM_GUI_FOOTER_IDLE;
				streamInfo.captureMode = STREAM_CAPTURE_FACE;
				StartWaitForFrStreamTimer(streamInfo);
// 				LoadDisplayFRStreamToIdleScreenTimer(FRServerConfig.identificationTimeOut,TRUE);
// 				SetFRStreamOverlayInfo(FALSE, MSG_MAX_LANG_STRING, FALSE, FOOTER_HOME, TRUE);
// 				ShowFRStreamOnDeviceDisplayTimer(ACCESS_WAIT_RESP);
			}
			else
			{
				DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
			}
			break;
		}

		case MSG_BLE_ACCESS_EVENT:
		{
			QR_RESULT_e	btResp;
			CB_READER_ACCESS_REQ_RESPONSE_t		respBtData;
			/** APTA Access Request on External Ble Reader */
			DPRINT(DC_LOG, "MSG_BLE_ACCESS_EVENT");

			btResp = ProcessExtBleRdrBtAccessReqDetect((CB_READER_BLE_ACCESS_REQ_t *)dataPtr);

			if((IsDeviceActive() != TRUE) && (btResp != QR_CODE_API_DISABLED))
			{
				ShowEntryRestrictedForDeviceInactive();

				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = QR_AUTH_ENTRY_RESTRICTED;
				respBtData.respStatus = WEB_API_QR_CODE_RESP_ACCESS_DENIED;

				ProcessReaderCommand(MSG_CB_BLE_ACCESS_RESP_CMD, &respBtData, sizeof(CB_READER_ACCESS_REQ_RESPONSE_t));
				break;
			}

			if(btResp == QR_CODE_USER_AUTHENTICATED)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATED");
				ProcessUser(DcInfo.readerMode);
			}
			else if(btResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATION_FAILED");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(btResp == QR_CODE_DEVICE_BUSY)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_DEVICE_BUSY");
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else if(btResp == QR_DEVICE_INVALID)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_DEVICE_INVALID ");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				// CRC is mismatched, Wrong Encryption.
				DcInfo.qrResponseMode = CBR_RESP_WRONG_ENC_CRC_MISMATCH;
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			}
			else
			{
				DPRINT(DC_LOG, "btResp == else(QR_CODE_API_DISABLED) ");
				DcInfo.qrResponseMode = CBR_RESP_USER_DENIED;
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			}

			respBtData.readerId = DcInfo.readerId;
			respBtData.respReason = DcInfo.qrAuthReason;
			respBtData.respStatus = DcInfo.qrResponseMode;

			ProcessReaderCommand(MSG_CB_BLE_ACCESS_RESP_CMD, &respBtData, sizeof(CB_READER_ACCESS_REQ_RESPONSE_t));
		}
		break;

		case MSG_ACCESS_ID_REQ_EVENT:
		{
			QR_RESULT_e						btResp;
			ACS_BLE_ACCESS_REQ_RESPONSE_t		respBtData;

			DPRINT(DC_LOG, "MSG_MOBILE_ACCESS_EVENT");

			btResp = ProcessMobileBtAccessReqDetect((MOBILE_ACCESS_REQ_t *)dataPtr);

			if((IsDeviceActive() != TRUE)&&(btResp != QR_CODE_API_DISABLED))
			{
				ShowEntryRestrictedForDeviceInactive();

				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = QR_AUTH_ENTRY_RESTRICTED;
				respBtData.respStatus = WEB_API_QR_CODE_RESP_ACCESS_DENIED;

				SendBLEEventTask(BLE_ACCESS_REQ_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
				break;
			}

			if(btResp == QR_CODE_USER_AUTHENTICATED)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATED");
				/* Set flag to indicate request received from ble */
				DcInfo.bleAccessDevice = BLE_ACCESS_DEVICE_ACS;

				ProcessUser(DcInfo.readerMode);
			}
			else if(btResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATION_FAILED");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(btResp == QR_CODE_DEVICE_BUSY)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_DEVICE_BUSY");
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else if(btResp == QR_DEVICE_INVALID)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_DEVICE_INVALID ");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				// CRC is mismatched, Wrong Encryption.
				DcInfo.qrResponseMode = CBR_RESP_WRONG_ENC_CRC_MISMATCH;
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			}
			else
			{
				DPRINT(DC_LOG, "btResp == else(QR_CODE_API_DISABLED) ");
				DcInfo.qrResponseMode = CBR_RESP_USER_DENIED;
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			}

			if(DcInfo.curState != DC_GET_USER_STATUS_FROM_PANEL_STATE)
			{
				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = DcInfo.qrAuthReason;
				respBtData.respStatus = DcInfo.qrResponseMode;

				SendBLEEventTask(BLE_ACCESS_REQ_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			}
		}
		break;

		/* To detect ambient temperature in background*/
		case MSG_TEMPERATURE_DETECTED_EVENT:
		{
			FLOAT32 detectedTemperature = 0.00;

			/* Get user's temperature detected by temperature sensor */
			detectedTemperature = *((FLOAT32*)dataPtr);

			/* Do ambient temperature detection Process */
			ProcAmbientTemperatureDetection(detectedTemperature);
		}
		break;

		//Pin Detected from External Reader
		case MSG_PIN_DETECTED_ON_EXTERNAL_READER_EVENT:
		{
			PIN_RESULT_e pinResp;

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetectOnExternalReader((READER_PIN_DATA_t *)dataPtr);

			if(IsDeviceActive() != TRUE)
			{
				ShowEntryRestrictedForDeviceInactive();
				break;
			}

 			if(pinResp == PIN_MATCH)
 			{
 				// User is identified as Valid User.
 				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
 			}
 			else if(pinResp == PIN_CHANGE_CODE_MATCH)
			{
				SwitchDcToPinChangeState();
			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		default:
		{
			// Events that are not handled in this state are passed to default message handler.
			DcDefaultMsgHandler(msgId ,dataPtr, length);
			break;
		}
	}
}

//*****************************************************************************
//	DcSmartCardKeyUpdateStateFn()
//	Param:
//		IN: msgId 	- Message which needs to be processed
//			dataPtr - Pointer to data of the message
//			length 	- Length of the message
//		OUT: None
//	Returns:
//		NONE
//	Description:
//		This function processes User access related events.
//*****************************************************************************
static void DcSmartCardKeyUpdateStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	// Based on message ID, process it
	switch(msgId)
	{
		case GUI_BUTTON_HOME:
		case GUI_BUTTON_BACK:
		{
		    DeleteTimer(&SmartCardKeyUpdateDetail.keyChngWaitTmrHandle);
		    SmartCardKeyUpdateDetail.keyChngResp = WEB_API_RESP_CARD_READ_WRITE_FAILED;
		    SmartCardKeyUpdateDetail.smartCardKeyChangeF = FALSE;
		    SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
		    SwitchDcToIdleState();
		    break;
		}

		case MSG_CARD_DETECTED_EVENT:
		{
			CARD_DETECTED_EV_DATA_t *cardDataPtr = dataPtr;
			BOOL	cardTypeVerified = FALSE;

			SmartCardKeyUpdateDetail.cardId = cardDataPtr->cardId;
			SmartCardKeyUpdateDetail.cardType = cardDataPtr->cardType;
			if(SmartCardKeyUpdateDetail.reqRecFromAPI == TRUE)
			{
				DeleteTimer(&SmartCardKeyUpdateDetail.keyChngWaitTmrHandle);
				if(SysBasicPara.reader[READER1].type == MIFARE_U_READER)
				{
					//TODO: if PN532 then desfire
					if( (cardDataPtr->cardType == MIFARE_1K) || (cardDataPtr->cardType == MIFARE_4K) )
					{
						cardTypeVerified = TRUE;
					}
				}
				else if(SysBasicPara.reader[READER1].type == HID_ICLASS_U_READER)
				{
					if( (cardDataPtr->cardType == HID_2K2) || (cardDataPtr->cardType == HID_16K2) || (cardDataPtr->cardType == HID_16K16) )
					{
						cardTypeVerified = TRUE;
					}
				}
				if(cardTypeVerified == FALSE)
				{
					SetAudioVisual(USER_RESP_KEY_CHANGE_FAIL, INTERNAL_PORT, NULL);
					DPRINT(WEB_API_LOG,"KEY CHANGE WRONG CARD TYPE");
					SmartCardKeyUpdateDetail.keyChngResp = WEB_API_RESP_WRONG_CARD_TYPE;
					SmartCardKeyUpdateDetail.smartCardKeyChangeF = FALSE;
					SwitchDcToIdleState();
					break;
				}
			}

			if(GetReaderRespPort(cardDataPtr->readerId) == INTERNAL_PORT)
			{
				SetAudioVisual(USER_RESP_CHANGING_CARD_KEY, INTERNAL_PORT, NULL);
				SendMsgChangeSmartCardKey(cardDataPtr->readerId, cardDataPtr->cardType);
				if(SmartCardKeyUpdateDetail.reqRecFromAPI == TRUE)
				{
					LoadKeyChangeWaitTimer(ENROLLMENT_TIMER);
				}
			}
			else if(GetReaderEntryExitMode(cardDataPtr->readerId) == EXIT_READER)
			{
				ProcessUserOnExitOnExternal(msgId, dataPtr);
			}
			break;
		}

		case MSG_CHANGE_SMART_CARD_KEY_RESP:
		{
			SMART_CARD_KEY_UPDATE_RESP_DATA_t	*chngKeyDataPtr = (SMART_CARD_KEY_UPDATE_RESP_DATA_t *)dataPtr;

			if(chngKeyDataPtr -> status == READER_RESP_STATUS_SUCCESS)
			{
				SetAudioVisual(USER_RESP_KEY_CHANGE_SUCCESS, INTERNAL_PORT, NULL);
				SmartCardKeyUpdateDetail.keyChngResp = WEB_API_RESP_SUCCESS;
			}
			else
			{
				SetAudioVisual(USER_RESP_KEY_CHANGE_FAIL, INTERNAL_PORT, NULL);
				SmartCardKeyUpdateDetail.keyChngResp = WEB_API_RESP_CARD_READ_WRITE_FAILED;
			}
			if(SmartCardKeyUpdateDetail.reqRecFromAPI == TRUE)
			{
				DPRINT(WEB_API_LOG,"KEY CHANGE REQUEST PROCESSED");
				SmartCardKeyUpdateDetail.smartCardKeyChangeF = FALSE;
				SwitchDcToIdleState();
			}
			else
			{
				LoadShowCardForKeyChngTimer(LCD_IDLE_TIMER);
			}
		}
		break;

		default:
		{
			// Events that are not handled in this state are passed to default message handler.
			DcDefaultMsgHandler(msgId ,dataPtr, length);
			break;
		}
	}
}

//*****************************************************************************
//	ProcessPinDetect()
//	Param:
//		IN :	pin	- pin entred by user
//		OUT:    NONE
//	Returns:	PIN_RESULT_e
//	Description:
//				This function processes if valid PIN is entered by the user.
//				It checks if it is valid PIN for any user.
//*****************************************************************************
PIN_RESULT_e ProcessPinDetect(PIN_DATA_t *pin)
{
	UINT16					userIdx;
	PIN_RESULT_e			retValue = PIN_CONTINUE;

	// We ignore user input if response port is different for following conditions
	// 1. while waiting for user input for special function
	// 2. DC task state is other than IDLE state
	if( (DcInfo.splFnGetUserInputTmrHandle != INVALID_TIMER_HANDLE) ||
		(DcInfo.curState != DC_IDLE_STATE) )
	{
		if(DcInfo.respPort != INTERNAL_PORT)
		{
			printf("\nDcInfo.respPort != EXTERNAL_PORT\n");
			return PIN_CONTINUE;
		}
	}

	DcInfo.respPort = INTERNAL_PORT;
	DeleteFsmTimers();

	// Store rcvd credential
	StoreRcvdCred(PIN_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
	sprintf(DcInfo.keyBuf, "%s", pin->pinData);

	if(DcInfo.procSSAF == TRUE)
	{
		if( (strlen((CHARPTR)(pin->pinData)) > 0) &&
			(strcmp((CHARPTR)(pin->pinData), GetUserPinCode(DcInfo.userIdx)) == 0) )
		{
			retValue = PIN_MATCH;
		}
		else
		{
			retValue = PIN_INVALID;
		}
	}
	else if(IsPinValid((CHARPTR)(pin->pinData) , &userIdx) == YES)
	{
		// Store identified user
		DcInfo.userIdx = userIdx;
		retValue = PIN_MATCH;
	}
	else if( (strlen((CHARPTR)(pin->pinData)) > 0) &&
			 (strcmp((CHARPTR)(pin->pinData), SysBasicPara.pinChangeCode) == 0) &&
			 ( (DcInfo.curState == DC_IDLE_STATE) ||(DcInfo.curState == DC_PIN_CHANGE_STATE) ) )
	{
		retValue =  PIN_CHANGE_CODE_MATCH;
	}
	else
	{
		retValue =  PIN_INVALID;
	}

	return retValue;
}

//*****************************************************************************
//	ProcessFpDetect()
//	Param:
//		IN :	fpDataPtr	- points to finger event data
//		OUT:	none
//	Returns:	FP_RESULT_e
//	Description:
//				This function processes FP event to determine
//				if user index is available or not.
//*****************************************************************************
FP_RESULT_e ProcessFpDetect(FP_DETECTED_EV_DATA_t * fpDataPtr)
{
	UINT16					userIdx;
	FP_RESULT_e				retVal = FP_CONTINUE;
	RESPONSE_PORT_e			curRespPort;

	// Ignore FP request if entering PIN or wait timer is running
	if( (DcInfo.waitTmrHandle != INVALID_TIMER_HANDLE) ||
		(DcInfo.interDigitTmrHandle[INTERNAL_PORT] != INVALID_TIMER_HANDLE) ||
		(DcInfo.interDigitTmrHandle[EXTERNAL_PORT] != INVALID_TIMER_HANDLE) )
	{
		return retVal;
	}

	curRespPort = GetReaderRespPort(fpDataPtr -> readerId);

	// We ignore user input if response port is different for following conditions
	// 1. while waiting for user input for special function
	// 2. DC task state is other than IDLE state
	// 3. If entering FP
	if( (DcInfo.splFnGetUserInputTmrHandle != INVALID_TIMER_HANDLE) ||
		(DcInfo.curState != DC_IDLE_STATE) ||
		(DcInfo.fpBeingProcessed == TRUE) )
	{
		if(DcInfo.respPort != curRespPort)
		{
			return retVal;
		}
	}

    // Ignore input from Internal reader If Slide Screen is open
    if((CurrStatusbarData.statusBarExpandedF == TRUE) && (curRespPort == INTERNAL_PORT))
    {
        return retVal;
    }

	// Ignore input from External reader for cafeteria
	if( (SysBasicPara.application == CAFETERIA) && (curRespPort != INTERNAL_PORT) )
	{
		return retVal;
	}

	DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
	DeleteTimer(&DcInfo.fpMatchTmrHandle);
    SetLCDDimTimer();

	// Set DC task Activity parameters
	DcInfo.readerId = fpDataPtr -> readerId;
	UpdateDcInfoReaderMode(fpDataPtr -> readerId);
	DcInfo.respPort = curRespPort;

	if(fpDataPtr -> status == READER_RESP_STATUS_SCAN_SUCCESS)
	{
		DcInfo.fpBeingProcessed = TRUE;
		// This is just to inform user that scan is successfull and user is
		// being identified by reader. We tell user to remove finger.
		SetAudioVisual(USER_RESP_REMOVE_FINGER, DcInfo.respPort, NULL);
		SetFpMatchTimer(FP_MATCH_ABORT_TIMER);
		return retVal;
	}
	else if(fpDataPtr -> status == READER_RESP_STATUS_SUCCESS)
	{
		// Store rcvd credential
		StoreRcvdCred(FP_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);

		DcInfo.fpBeingProcessed = FALSE;
		// FP reader detected finger with some userID.
		// Lets check it exists in our database.
		if( IsUserValid(fpDataPtr -> fprUserId, &userIdx) == NO)
		{
			// if user not found or user id is not configured on that index then deny
			if((fpDataPtr -> fprUserId == TEMP_USER_ID) && (DcInfo.curState == DC_IDLE_STATE) 
				&& (DISABLED == BioIdentifyServer.enblBioServer) && (IsTempUserApplicableForFPEnrollment() == YES) && (DcInfo.singleUserSplCode == USER_SPECIAL_CODE_NONE))
			{
				return FP_NOT_MATCH;
			}
			else
			{
				return FP_INVALID;
			}
			
		}

		//Store identified user
		DcInfo.userIdx = userIdx;
		if(YES == IsDetectedFingerIsDuress(DcInfo.userIdx,fpDataPtr -> subId))
		{
		        DcInfo.duressF = YES;
		}
		retVal = FP_MATCH;
	}
	else if((fpDataPtr -> status == READER_RESP_STATUS_NOT_FOUND) && (DISABLED == BioIdentifyServer.enblBioServer) 
			&& (IsTempUserApplicableForFPEnrollment() == YES) && (DcInfo.curState == DC_IDLE_STATE) && (DcInfo.singleUserSplCode == USER_SPECIAL_CODE_NONE))
	{
		// Store rcvd credential
		StoreRcvdCred(FP_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
		DcInfo.fpBeingProcessed = FALSE;
		fpDataPtr->fprUserId = 0;
		retVal = FP_NOT_MATCH;
	}
	else if((fpDataPtr -> status == READER_RESP_STATUS_NOT_MATCH) && (ENABLED == BioIdentifyServer.enblBioServer)
			&& (IsTempUserApplicableForFPEnrollment() == YES) && (DcInfo.curState == DC_IDLE_STATE) && (fpDataPtr -> source == FP_DETECT_SOURCE_SERVER) && (DcInfo.singleUserSplCode == USER_SPECIAL_CODE_NONE))
	{
		// Store rcvd credential
		StoreRcvdCred(FP_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
		DcInfo.fpBeingProcessed = FALSE;
		retVal = FP_NOT_MATCH;
	}
	else if(fpDataPtr -> status == READER_RESP_STATUS_TIME_OUT)
	{
		if( (BioIdentifyServer.enblBioServer == DISABLED) || (fpDataPtr -> source != FP_DETECT_SOURCE_MODULE) )
		{
			// Store rcvd credential
			StoreRcvdCred(FP_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
			DcInfo.fpBeingProcessed = FALSE;
		}
		retVal = FP_MATCHING_TIMEOUT;
	}
	else
	{
		if( (BioIdentifyServer.enblBioServer == DISABLED) || (fpDataPtr -> source != FP_DETECT_SOURCE_MODULE) )
		{
			// Store rcvd credential
			StoreRcvdCred(FP_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
			// Cannot detect finger or template not available in reader.
			DcInfo.fpBeingProcessed = FALSE;
		}
		retVal = FP_INVALID;
	}

	return retVal;
}

//*****************************************************************************
//	ProcessCardDetect()
//	Param:
//		IN : cardDataPtr	- points to card event data
//		OUT: userIdx
//	Returns:
//		CARD_RESULT_e
//	Description:
//		This function processes card detected event.
//		It checks if cardId match any user in Database
//*****************************************************************************
CARD_RESULT_e ProcessCardDetect(CARD_DETECTED_EV_DATA_t * cardDataPtr,CREDENTIAL_TYPE_e credType)
{
	UINT16					userIdx;
	SPECIAL_FUNCTIONS_e 	splFnIdx;
	CARD_INDEX_e			splFncardIdx;
	BOOL					splFnCardDet = NO;
	RESPONSE_PORT_e			curRespPort;
	UINT32 					credSubType = ACCESS_CARD1;

	//copy card detail into last detected card detail holder structure
	DcInfo.lastDetectedCard.cardId		= cardDataPtr -> cardId;
	DcInfo.lastDetectedCard.readerId	= cardDataPtr -> readerId;
	DcInfo.fCInCSN = cardDataPtr -> fCInCSN;
	DcInfo.fCInCardMem = cardDataPtr -> fCInCardMem;

	// Ignore card request if entering PIN or FP or wait timer is running
	if( (DcInfo.waitTmrHandle != INVALID_TIMER_HANDLE) ||
		(DcInfo.interDigitTmrHandle[INTERNAL_PORT] != INVALID_TIMER_HANDLE)  ||
		(DcInfo.interDigitTmrHandle[EXTERNAL_PORT] != INVALID_TIMER_HANDLE) ||
		(DcInfo.fpBeingProcessed == TRUE) )
	{
		return CARD_CONTINUE;
	}

	curRespPort = GetReaderRespPort(cardDataPtr -> readerId);

	// We ignore user input if response port is different for following conditions
	// 1. while waiting for user input for special function
	// 2. DC task state is other than IDLE state
	if( (DcInfo.splFnGetUserInputTmrHandle != INVALID_TIMER_HANDLE) ||
		(DcInfo.curState != DC_IDLE_STATE) )
	{
		if(DcInfo.respPort != curRespPort)
		{
			return CARD_CONTINUE;
		}
	}

    // Ignore input from Internal reader If Slide Screen is open
    if((CurrStatusbarData.statusBarExpandedF == TRUE) && (curRespPort == INTERNAL_PORT))
    {
        return CARD_CONTINUE;
    }

	// Check if card is special function card. If so get Card Idx (0-3)
	if(IsSplFnCard(cardDataPtr -> cardId, &splFnIdx, &splFncardIdx) == YES)
	{
		// If waiting for user input for special function to process single user then ignore special function card
		if(DcInfo.splFnGetUserInputTmrHandle != INVALID_TIMER_HANDLE)
			return CARD_CONTINUE;

		splFnCardDet = YES;
	}

	// Ignore input from External reader for cafeteria
	if( (SysBasicPara.application == CAFETERIA) && (curRespPort != INTERNAL_PORT) )
	{
		return CARD_CONTINUE;
	}

	DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
    SetLCDDimTimer();

	// Set DC task Activity parameters
	DcInfo.readerId = cardDataPtr -> readerId;
	DcInfo.respPort = GetReaderRespPort(DcInfo.readerId);
	UpdateDcInfoReaderMode(DcInfo.readerId);
	DcInfo.cardType = cardDataPtr -> cardType;
	DcInfo.addlSecCode = cardDataPtr -> addlSecCode;

	if(cardDataPtr -> doorLevelF == SET)
	{
		DcInfo.lastDoorLevel = cardDataPtr -> doorLevel;
	}
	else
	{
		DcInfo.lastDoorLevel = DEFAULT_DOOR_LEVEL;
	}

	if( (DcInfo.lastDoorLevel == DEFAULT_DOOR_LEVEL)
			 || (cardDataPtr -> doorModeF != SET) )
	{
		DcInfo.userRouteReaderMode  = ANY_READER_MODE;
	}
	else
	{
		DcInfo.userRouteReaderMode  = (READER_MODE_e)cardDataPtr -> doorMode;
	}

	
	DcInfo.cardCredntlDetectedF = TRUE;

	if(splFnCardDet == YES)
	{
		// Store rcvd credential
		StoreRcvdCred(CARD_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);

		// Valid special function card is found, update door activity info
		DcInfo.splFnId = splFnIdx;
		DcInfo.splFnCardIdx = splFncardIdx;

		return SP_FUNC_CARD_MATCH;
	}

	// card is not found in special function database
	// let's check in user database
	if(IsCardValid(cardDataPtr -> cardId, &userIdx, &credSubType) == YES)
	{
		// Store identified user
		DcInfo.userIdx = userIdx;
		// Store rcvd credential
		StoreRcvdCred(credType,credSubType);
		return USER_CARD_MATCH;
	}
	// Store rcvd credential
	StoreRcvdCred(credType,credSubType);

	//card is not found in special function database and user database
	//if VAM is enable then check in vam database
	if( (SysBasicPara.application != CAFETERIA) && (SysAdvPara.vamEnable == TRUE) && (IsVamCardValid(cardDataPtr -> cardId) == YES) )
	{
		DPRINT(DC_LOG, "DcInfo.detectedVamCardType= %u cardId[%llu]", DcInfo.detectedVamCardType, cardDataPtr -> cardId);
		StoreRcvdCred(CARD_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
		return CARD_VAM_MATCH;
	}

	if( (SysAdvPara.genInvalidUserEvent == ENABLED) && (IsDeviceActive() == TRUE) )
	{
		WriteCombinedFieldEventLog(INVALID_CARD_EVT, cardDataPtr -> cardId);
	}
	return CARD_INVALID;
}

//*****************************************************************************
//	AuthenticateUser()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			 ALLOWED/DENIED
//	Description:
//		This function applies access policies to users. It validates user and
//		returns results whether the user is allowed or not with reason.
//*****************************************************************************
static BOOL AuthenticateUser(void)
{
	UINT16					userIdx;
	ANTIPASSBACK_STATUS_e 	apbStatus;

	userIdx = DcInfo.userIdx;

	// For entry mode, check unconditionally and for exit mode if access policy check is enabled
	if( (DcInfo.readerMode == ENTRY_READER) || (SysAdvPara.verifyAccessPolicyOnExitF == ENABLED) )
	{
		// Check if user blocked
		if(IsUserBlocked(userIdx, NULL) == YES)
		{
			DcInfo.authReason = AUTH_USER_BLOCKED;
			return DENIED;
		}
	}

	if(DcInfo.readerMode == ENTRY_READER)
	{
		// Check for use count violation,
		// NOTE: 'authReason' updated inside
		if(IsUseCountViolated() == YES)
		{
			return DENIED;
		}

	}
	else
	{
		// Check if user is violating Use Count
		// we allow user to exit but will be added to blocked list in case of violation.
		IsUseCountViolated();
	}

	if( (DcInfo.readerMode == ENTRY_READER) || (SysAdvPara.verifyAccessPolicyOnExitF == ENABLED) )
	{
		// Check for occupancy count violation
		if(IsOccupancyControlViolated() == YES)
		{
			DPRINT(DC_LOG, "User denied on Door, Occupancy Check Failed");
			return DENIED;
		}
	}

	// Check if user violated anti pass back
	apbStatus = GetAntiPassBackStatus(userIdx, DcInfo.readerMode);
	if(apbStatus == HARD_VIOLATION)
	{
		if(DcInfo.readerMode == ENTRY_READER)
		{
			DcInfo.authReason = AUTH_PASSBACK_ENTRY_HARD_VIOLATION;
		}
		else
		{
			DcInfo.authReason = AUTH_PASSBACK_EXIT_HARD_VIOLATION;
		}
		return DENIED;
	}
	else if(apbStatus == SOFT_VIOLATION)
	{
		if(DcInfo.readerMode == ENTRY_READER)
		{
			DcInfo.authReason = AUTH_PASSBACK_ENTRY_SOFT_VIOLATION;
		}
		else
		{
			DcInfo.authReason = AUTH_PASSBACK_EXIT_SOFT_VIOLATION;
		}
	}

	// For entry mode, check unconditionally and for exit mode if access policy check is enabled
	if( (DcInfo.readerMode == ENTRY_READER) || (SysAdvPara.verifyAccessPolicyOnExitF == ENABLED) )
	{
		// normal user- not blocked, not VIP, valid visitor
		// Check if user access valid based on time of access
		// NOTE: 'authReason' updated inside
		if(CheckTimeBasedAccess() == ALLOWED)
		{
			// All neccessary authetication sucessful, Allow access
			return ALLOWED;
		}
		else
		{
			// User not authenticated, Deny access
			return DENIED;
		}
	}

	// All neccessary authetication sucessful, Allow access
	return ALLOWED;
}

//*****************************************************************************
//	ProcessUserOnEntry()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function processes user with userIdx stored in doorActivity
//				to determine whether to allow access on Entry. If allowed,
//				door will be opened. Audio-visual indication will be given to
//				user to inform him of access response. Appropriate event will
//				also be logged. If additional credentials are required for
//				verification, FSM state will be changed to wait for more
//				user's credential.
//*****************************************************************************
void ProcessUserOnEntry(void)
{
	UINT16					userIdx;
	CREDENTIAL_TYPE_e 		credentialType;

	credentialType = DcInfo.credType[( (DcInfo.credCnt) - 1)];
	userIdx = DcInfo.userIdx;

	// Initialize auth reason
	DcInfo.authReason = AUTH_ALLOWED;

	DcInfo.userDetectCnt += 1;

	if(IsUserVIP(userIdx) == ENABLED)
	{
		DcInfo.userType = VIP_USER;
	}
	else if(GetVisitorIdx(userIdx) != INVALID_VISITOR_IDX)
	{
		SetUserAsVisitor();
	}
	else
	{
		DcInfo. userType = NORMAL_USER;
	}

#if defined(FACE_MASK_COMPULSION_ENABLE)
	if((DcInfo.apiUser != SET) || 
		((DcInfo.apiUser == SET) && (DcInfo.faceMaskAccessDecision == FACE_MASK_DECISION_MAX))) 
	{
		/* check if FR is enabled when Face mask compulsion is enabled */
		if ((SysAdvPara.faceMaskComp_Enable == TRUE) && (FRServerConfig.enableFrServer == FALSE) && 
			((FALSE == DcInfo.duressF) || ((TRUE == DcInfo.duressF) && (TA == SysBasicPara.application))) )
		{
			/* Check if user marks attendance through MOD/APTA API */
			if ((DcInfo.apiUser == SET) && (DcInfo.bleAccessDevice == BLE_ACCESS_DEVICE_MAX))
			{
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ENABLE_FACE_RECOGNITION_ON_DEVICE;
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			}
			/* Check if user marks attendance through BLE */
			else if (((DcInfo.apiUser == SET) && (DcInfo.bleAccessDevice != BLE_ACCESS_DEVICE_MAX))
					|| (DcInfo.cbReaderBleAccess == SET))
			{
				DcInfo.qrResponseMode = ACS_FR_DISABLED_ON_DEVICE;
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			}
			DcInfo.authReason = AUTH_FR_DISABLED_ON_DEVICE;
			SetAudioVisual(GetUserRespFromAuthReason(DcInfo.authReason), DcInfo.respPort, NULL);
			return;
		}	
	}	
#endif

	// Deny user unconditionally if door is locked
	if(DcMonitorControl.doorLock == DOOR_LOCKED)
	{
		DcInfo.authReason = AUTH_DOOR_LOCK;
		DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}
	// Allow user unconditionally if door is unlocked
	else if(DcMonitorControl.doorLock == DOOR_UNLOCKED)
	{
		DcInfo.allowWithoutFinalCheck = TRUE;
		DcInfo.authReason = AUTH_ALLOWED;
		CheckFinalPolicyAndAllow();
		return;
	}

	// Deny access to disabled user on entry
	if(IsUserEnable(userIdx) == DISABLED)
	{
		DcInfo.authReason = AUTH_USER_DISABLED;
		DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}

	// Deny access to user whose validity is expired
	if( (IsTerminationEnbl(userIdx) == ENABLED) &&
		(IsUserValidityExpired(GetTerminationDate(userIdx)) == YES) )
	{
		DcInfo.authReason = AUTH_VALIDITY_EXPIRED;
		DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}

	// For entry mode, check unconditionally and for exit mode if access policy check is enabled
	if( (DcInfo.readerMode == ENTRY_READER) || (SysAdvPara.verifyAccessPolicyOnExitF == ENABLED) )
	{
		// Check if user is visitor
		if(DcInfo.userType == VISITOR_USER)
		{
			// Check validity period of visitor
			if(IsVisitorValid(GetVisitorIdx(userIdx)) == NO)
			{
				// Visitor is not in valid period.
				// Deny access
				DPRINT(DC_LOG, "visitor validity expired");
				DcInfo.authReason = AUTH_NO_ACCESS;
				DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				return;
			}
		}
	}

	// If T&A application is enabled, we allow access to user
	// without checking any access policies.
	if(SysBasicPara.application == TA)
	{
		if(DcInfo.userType == VIP_USER)
		{
			DcInfo.allowWithoutFinalCheck = TRUE;
			if(GetVisitorIdx(userIdx) != INVALID_VISITOR_IDX)
			{
				//TODO: if user type is VIP and visitor
				// if pass surrender mode enabled, allow with auth reason pass surrender
				if(SysAdvPara.visitorPassSurrenderEnblF == ENABLED)
				{
					DcInfo.authReason = AUTH_VISITOR_ALLOWED_SURRENDER_PASS;
				}
			}
			// All access policies is checked and user is allowed
			CheckFinalPolicyAndAllow();
			return;
		}

		// 	All other users have to show proper credential as per ZAM policy
		if(IsCredentialValid() == NO)
		{
			// set userIdx as ZAM user for updating display and log an event
			DcInfo.zamUserIdx = userIdx;
			DcInfo.authReason = AUTH_INVALID_INPUT;
			// Store current credential as invalid
			StoreRcvdCred(INVALID_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
	 		DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			return;
		}

		if((GetCredentialCnt()) > DcInfo.credCnt)
		{
			if(CheckForWgndWaitforVerify() == TRUE)
			{
				DPRINT(DC_LOG, "Switch To WIEGAND User Verify Wait Stat");
				SwitchToUserVerifyWaitStat();
				return;
			}
			else
			{
				if(DcInfo.apiUser == SET )
				{
					if(SysBasicPara.apiEntryAccessMode == ACCESS_MODE_QR_CODE_THEN_BIO)
					{
						READER_ID_e bioReaderId;
						if (IsReaderPresent(FP_READER, DcInfo.respPort, ANY_READER_MODE, &bioReaderId)==NO)
						{
							DcInfo.authReason = AUTH_INVALID_INPUT;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						else
						{
							DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
					}
					else if(SysBasicPara.apiEntryAccessMode == ACCESS_MODE_QR_CODE_THEN_CARD)
					{
						READER_ID_e cardReaderId;
						if (IsReaderPresent(CARD_READER, DcInfo.respPort, ANY_READER_MODE, &cardReaderId)==NO)
						{
							DcInfo.authReason = AUTH_INVALID_INPUT;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						else
						{
							DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
					}
					else if(SysBasicPara.apiEntryAccessMode == ACCESS_MODE_MOBILE_THEN_PIN)
					{
						if(CB_READER_BLE_CREDENTIAL == DcInfo.credType[DcInfo.credCnt-1])
						{
							DcInfo.qrResponseMode = RESP_ASK_FOR_ENTER_PIN;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
						else if(BLE_QR_CREDENTIAL == DcInfo.credType[DcInfo.credCnt-1])
						{
							READER_ID_e pinReaderId;
							if((DcInfo.respPort  == EXTERNAL_PORT) && (IsReaderPresent(EXTERNAL_PIN_READER, DcInfo.respPort, ANY_READER_MODE, &pinReaderId)==NO))
							{
								DcInfo.authReason = AUTH_INVALID_INPUT;
								DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
								return;
							}
							DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
						else
						{
							EPRINT(DC_LOG,"Unknown Credential Received for Mobile then Pin API Mode");
							SwitchDcToIdleState();
							return;
						}
					}
#if 0
					else if(SysBasicPara.apiEntryAccessMode == ACCESS_MODE_MOBILE_THEN_FACE)
					{
						// Restrict user if FR is disable in device/user.
						if(IsFREnable() == NO)
						{
							DPRINT(SYS_LOG, "User Denied FR Disabled");
							DcInfo.authReason = AUTH_FR_DISABLED;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						DcInfo.qrResponseMode = RESP_ASK_FOR_SHOW_FACE;
						DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
					}
#endif
					else
					{
						EPRINT(DC_LOG,"Unknown apiEntryAccessMode received");
						SwitchDcToIdleState();
						return;
					}
				}

				// will check for fp verification, will ask for next credential
				CheckNextCredToProcess();
				// set userIdx as ZAM user so that we can verify him/her later
				DcInfo.zamUserIdx = userIdx;
				// Change DC FSM state to wait for additional credential state
				SwitchDcToZAMState();
				return;
			}
		}
		else
		{
			if(DcInfo.userType == VISITOR_USER)
			{
				// if pass surrender mode enabled, allow with auth reason pass surrender
				if(SysAdvPara.visitorPassSurrenderEnblF == ENABLED)
				{
					DcInfo.authReason = AUTH_VISITOR_ALLOWED_SURRENDER_PASS;
				}
			}
			else if(DcInfo.userType == NORMAL_USER)
			{
				DcInfo.allowWithoutFinalCheck = TRUE;
				DcInfo.authReason = AUTH_ALLOWED;
			}

			CheckFinalPolicyAndAllow();
			return;
		}
	}

	// We are here, so appl = ACCESS_CTRL and mode = ENTRY
	// If latest credential detected is CARD and ASC enabled globally,
	// We are not waiting for additional credential for an User with QR CODE API credential,
	// verify ASC
	if( (DcInfo.apiUser == CLEAR) && (DcInfo.cbReaderBleAccess == CLEAR))
	{
		// If duress don't check any access policies
		if(DcInfo.duressF == YES)
		{
			DcInfo.allowWithoutFinalCheck =TRUE;
			DcInfo.authReason = AUTH_ALLOWED_DURESS;
			DPRINT(DC_LOG,"Duress activated, User allowed without final check");
			CheckFinalPolicyAndAllow();
			return;
		}
		
		
		if(credentialType == CARD_CREDENTIAL)
		{
		    if(SysAdvPara.addlSecurity.enable == ENABLED)
		    {
			    // if ASC is invalid, deny user
			    if((DcInfo.addlSecCode == 0) || (DcInfo.addlSecCode != SysAdvPara.addlSecurity.code))
			    {
				    DcInfo.authReason = AUTH_NO_ACCESS;
				    DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				    return;
			    }
		    }
		}	
	}

	// VIP has certain special priveleges. So lets handle VIP separately
	if(DcInfo.userType == VIP_USER)
	{
		DcInfo.allowWithoutFinalCheck = TRUE;
		if( (DcInfo.firstInTempLockF == TRUE) && (IsFirstInLock() == YES) && (CheckTimeBasedAccess() == ALLOWED) )
		{
			// Door has first-in lock active.
			// Only valid first-in user can deactivated first in lock
			if(IsValidFirstInUser() == YES)
			{
				DcInfo.firstInTempLockF = FALSE;
			}
		}

		if(GetVisitorIdx(userIdx) != INVALID_VISITOR_IDX)
		{
			//TODO: if user type is VIP and visitor
			// if pass surrender mode enabled, allow with auth reason pass surrender
			if(SysAdvPara.visitorPassSurrenderEnblF == ENABLED)
			{
				DcInfo.authReason = AUTH_VISITOR_ALLOWED_SURRENDER_PASS;
			}
		}
		// All access policies is checked and user is allowed
		CheckFinalPolicyAndAllow();
		return;
	}

	// Now perform user authentication based on user type and time
	if((AuthenticateUser()) == DENIED)
	{
		DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}

	// User authenticated
	// Lets process user access further based on access policies
	// For all other users, first check for first-in lock
	if( (IsFirstInLock() == YES) && (DcInfo.firstInTempLockF == TRUE) )
	{
		// Door has first-in lock active.
		// Only valid first-in user can deactivated first in lock
		if(IsValidFirstInUser() == NO)
		{
			DcInfo.authReason = AUTH_FIRST_IN;
			DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			return;
		}
		else	// Valid user to activate first-in lock
		{
			DcInfo.firstInTempLockF = FALSE;
		}
	}

	// Lets handle Visitor separately as ZAM policy	don't apply to visitors
	if(DcInfo.userType == VISITOR_USER)
	{
		// Visitor are not allowed if 2person rule is applicable on door
		if(IsTwoPersonApplicable() == YES)
		{
			DPRINT(DC_LOG, "visitor not allowed if 2 person rule is applicable");
			DcInfo.authReason = AUTH_NO_ACCESS;
			DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			return;
		}

		// 	All other users have to show proper credential as per ZAM policy
		if(IsCredentialValid() == NO)
		{
			// set userIdx as ZAM user for updating display and log an event
			DcInfo.zamUserIdx = userIdx;
			DcInfo.authReason = AUTH_INVALID_INPUT;
			// Store current credential as invalid
			StoreRcvdCred(INVALID_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
	 		DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			return;
		}

		// If more than one credentials are required to access this door,
		// we switch state where we wait for additional credential of the same user.
		if((GetCredentialCnt()) > DcInfo.credCnt)
		{
			if(CheckForWgndWaitforVerify() == TRUE)
			{
				DPRINT(DC_LOG, "Switch To WIEGAND User Verify Wait Stat");
				SwitchToUserVerifyWaitStat();
				return;
			}
			else
			{
				if(DcInfo.apiUser == SET )
				{
					if(SysBasicPara.apiEntryAccessMode == ACCESS_MODE_QR_CODE_THEN_BIO)
					{
						READER_ID_e bioReaderId;
						if (IsReaderPresent(FP_READER, DcInfo.respPort, ANY_READER_MODE, &bioReaderId)==NO)
						{
							DcInfo.authReason = AUTH_INVALID_INPUT;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						else
						{
							DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
					}
					else if(SysBasicPara.apiEntryAccessMode == ACCESS_MODE_QR_CODE_THEN_CARD)
					{
						READER_ID_e cardReaderId;
						if (IsReaderPresent(CARD_READER, DcInfo.respPort, ANY_READER_MODE, &cardReaderId)==NO)
						{
							DcInfo.authReason = AUTH_INVALID_INPUT;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						else
						{
							DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
					}
					else if(SysBasicPara.apiEntryAccessMode == ACCESS_MODE_MOBILE_THEN_PIN)
					{
						if(CB_READER_BLE_CREDENTIAL == DcInfo.credType[DcInfo.credCnt-1])
						{
							DcInfo.qrResponseMode = RESP_ASK_FOR_ENTER_PIN;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
						else if(BLE_QR_CREDENTIAL == DcInfo.credType[DcInfo.credCnt-1])
						{
							READER_ID_e pinReaderId;
							if((DcInfo.respPort  == EXTERNAL_PORT) && (IsReaderPresent(EXTERNAL_PIN_READER, DcInfo.respPort, ANY_READER_MODE, &pinReaderId)==NO))
							{
								DcInfo.authReason = AUTH_INVALID_INPUT;
								DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
								return;
							}
							DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
						else
						{
							EPRINT(DC_LOG,"Unknown Credential Received for Mobile then Pin API Mode");
							SwitchDcToIdleState();
							return;
						}
					}
#if 0
					else if(SysBasicPara.apiEntryAccessMode == ACCESS_MODE_MOBILE_THEN_FACE)
					{
						// Restrict user if FR is disable in device/user.
						if(IsFREnable() == NO)
						{
							DPRINT(SYS_LOG, "User Denied FR Disabled");
							DcInfo.authReason = AUTH_FR_DISABLED;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						DcInfo.qrResponseMode = RESP_ASK_FOR_SHOW_FACE;
						DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
					}
#endif
					else
					{
						EPRINT(DC_LOG,"Unknown apiEntryAccessMode received");
						SwitchDcToIdleState();
						return;
					}
				}
				// will check for fp verification, will ask for next credential
				CheckNextCredToProcess();
				// set userIdx as ZAM user so that we can verify him/her later
				DcInfo.zamUserIdx = userIdx;
				// Change DC FSM state to wait for additional credential state
				SwitchDcToZAMState();
				return;
			}
		}

		//If device is in pass surrender mode and user is visitor, then set auth reason as surrender pass
		if(SysAdvPara.visitorPassSurrenderEnblF == TRUE)
		{
			DcInfo.authReason = AUTH_VISITOR_ALLOWED_SURRENDER_PASS;
		}

		// We have gone thru all the checks and finally we can let the user in.
		CheckFinalPolicyAndAllow();
		return;
	}

	if((DcInfo.apiUser == CLEAR) && (DcInfo.cbReaderBleAccess == CLEAR))
	{
		if(IsBioCredSelfEnrollReq() == YES)
		{
			SwitchToSelfEnrlInputState();
			return;
		}
	}

	// 	All other users have to show proper credential as per ZAM policy
	if(IsCredentialValid() == NO)
	{
		// set userIdx as ZAM user for updating display and log an event
		DcInfo.zamUserIdx = userIdx;
		DcInfo.authReason = AUTH_INVALID_INPUT;
		// Store current credential as invalid
		StoreRcvdCred(INVALID_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
 		DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}

	if((DcInfo.apiUser == CLEAR) && (DcInfo.cbReaderBleAccess == CLEAR))
	{
		if(CheckSmartRouteFeature() == PROCESS_DONE)
		{
			return;
		}
	}

	// If more than one credentials are required to access this door,
	// we switch state where we wait for additional credential of the same user.
	if((GetCredentialCnt()) > DcInfo.credCnt)
	{
		if(CheckForWgndWaitforVerify() == TRUE)
		{
			DPRINT(DC_LOG, "Switch To WIEGAND User Verify Wait Stat");
			SwitchToUserVerifyWaitStat();
			return;
		}
		else
		{
			if(DcInfo.apiUser == SET )
			{
				if(SysBasicPara.apiEntryAccessMode == ACCESS_MODE_QR_CODE_THEN_BIO)
				{
					READER_ID_e bioReaderId;
					if (IsReaderPresent(FP_READER, DcInfo.respPort, ANY_READER_MODE, &bioReaderId)==NO)
					{
						DcInfo.authReason = AUTH_INVALID_INPUT;
						DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
						return;
					}
					else
					{
						DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
						DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
					}
				}
				else if(SysBasicPara.apiEntryAccessMode == ACCESS_MODE_QR_CODE_THEN_CARD)
				{
					READER_ID_e cardReaderId;
					if (IsReaderPresent(CARD_READER, DcInfo.respPort, ANY_READER_MODE, &cardReaderId)==NO)
					{
						DcInfo.authReason = AUTH_INVALID_INPUT;
						DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
						return;
					}
					else
					{
						DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
						DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
					}
				}
				else if(SysBasicPara.apiEntryAccessMode == ACCESS_MODE_MOBILE_THEN_PIN)
				{
					if(CB_READER_BLE_CREDENTIAL == DcInfo.credType[DcInfo.credCnt-1])
					{
						DcInfo.qrResponseMode = RESP_ASK_FOR_ENTER_PIN;
						DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
					}
					else if(BLE_QR_CREDENTIAL == DcInfo.credType[DcInfo.credCnt-1])
					{
						READER_ID_e pinReaderId;
						if((DcInfo.respPort  == EXTERNAL_PORT) && (IsReaderPresent(EXTERNAL_PIN_READER, DcInfo.respPort, ANY_READER_MODE, &pinReaderId)==NO))
						{
							DcInfo.authReason = AUTH_INVALID_INPUT;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
						DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
					}
					else
					{
						EPRINT(DC_LOG,"Unknown Credential Received for Mobile then Pin API Mode");
						SwitchDcToIdleState();
						return;
					}
				}
#if 0
				else if(SysBasicPara.apiEntryAccessMode == ACCESS_MODE_MOBILE_THEN_FACE)
				{
					// Restrict user if FR is disable in device/user.
					if(IsFREnable() == NO)
					{
						DPRINT(SYS_LOG, "User Denied FR Disabled");
						DcInfo.authReason = AUTH_FR_DISABLED;
						DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
						return;
					}
					DcInfo.qrResponseMode = RESP_ASK_FOR_SHOW_FACE;
					DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				}
#endif
				else
				{
					EPRINT(DC_LOG,"Unknown apiEntryAccessMode received");
					SwitchDcToIdleState();
					return;
				}
			}
			// will check for fp verification, will ask for next credential
			CheckNextCredToProcess();
			// set userIdx as ZAM user so that we can verify him/her later
			DcInfo.zamUserIdx = userIdx;
			// Change DC FSM state to wait for additional credential state
			SwitchDcToZAMState();
			return;
		}
	}

	if(CheckTwoPersonFeature() == PROCESS_DONE)
	{
		return;
	}

	// We have gone thru all the checks and finally we can let the user in.
	CheckFinalPolicyAndAllow();
}

//*****************************************************************************
//	ProcessUserOnExit()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function processes user with userIdx stored in doorActivity
//				to determine whether to allow access on Exit. If allowed,
//				door will be opened. Audio-visual indication will be given to
//				user to inform him of access response. Appropriate event will
//				also be logged.
//*****************************************************************************
void ProcessUserOnExit(void)
{
	UINT16					userIdx;
	CREDENTIAL_TYPE_e 		credentialType;

	userIdx = DcInfo.userIdx;

	credentialType = DcInfo.credType[( (DcInfo.credCnt) - 1)];

	DcInfo.userDetectCnt += 1;
	userIdx = DcInfo.userIdx;

	// Initialize auth reason
	DcInfo.authReason = AUTH_ALLOWED;

	if(IsUserVIP(userIdx) == ENABLED)
	{
		DcInfo.userType = VIP_USER;
	}
	else if(GetVisitorIdx(userIdx) != INVALID_VISITOR_IDX)
	{
		SetUserAsVisitor();
	}
	else
	{
		DcInfo. userType = NORMAL_USER;
	}

	// If door is locked, check whether the exit is allowed when door is locked
	// Deny exit if not allowed
	if( (DcMonitorControl.doorLock == DOOR_LOCKED) && (SysBasicPara.exitOnLock == DISABLED) )
	{
		DcInfo.authReason = AUTH_DOOR_LOCK;
		DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}

	// Allow user to exit unconditionally if door is unlocked
	if(DcMonitorControl.doorLock == DOOR_UNLOCKED)
	{
		DcInfo.allowWithoutFinalCheck = TRUE;
		DcInfo.authReason = AUTH_ALLOWED;
		CheckFinalPolicyAndAllow();
		return;
	}

	// Access policy check on exit
	if(SysAdvPara.verifyAccessPolicyOnExitF == ENABLED)
	{
		// Deny access to disabled user on entry
		if(IsUserEnable(userIdx) == DISABLED)
		{
			DcInfo.authReason = AUTH_USER_DISABLED;
			DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			return;
		}

		// Deny access to user whose validity is expired
		if( (IsTerminationEnbl(userIdx) == ENABLED) &&
			(IsUserValidityExpired(GetTerminationDate(userIdx)) == YES) )
		{
			DcInfo.authReason = AUTH_VALIDITY_EXPIRED;
			DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			return;
		}
	}

	// For entry mode, check unconditionally and for exit mode if access policy check is enabled
	if( (DcInfo.readerMode == ENTRY_READER) || (SysAdvPara.verifyAccessPolicyOnExitF == ENABLED) )
	{
		// Check if user is visitor
		if(DcInfo.userType == VISITOR_USER)
		{
			// Check validity period of visitor
			if(IsVisitorValid(GetVisitorIdx(userIdx)) == NO)
			{
				// Visitor is not in valid period.
				// Deny access
				DPRINT(DC_LOG, "visitor validity expired");
				DcInfo.authReason = AUTH_NO_ACCESS;
				DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				return;
			}
		}
	}
	// If T&A application is enabled, we allow access to user
	// without checking any access policies.
	if(SysBasicPara.application == TA)
	{
		// VIP has certain special priveleges. So lets handle VIP separately
		if(IsUserVIP(userIdx) == ENABLED)
		{
			DcInfo.allowWithoutFinalCheck = TRUE;
			if(GetVisitorIdx(userIdx) != INVALID_VISITOR_IDX)
			{
				//TODO: if user type is VIP and visitor
				// if pass surrender mode enabled, allow with auth reason pass surrender
				if(SysAdvPara.visitorPassSurrenderEnblF == ENABLED)
				{
					DcInfo.authReason = AUTH_VISITOR_ALLOWED_SURRENDER_PASS;
				}
			}
			//open door to allow exit to the user(s)
			CheckFinalPolicyAndAllow();
			return;
		}

		// Check presented credential is as per ZAM policy or not
		if(IsCredentialValid() == NO)
		{
			// set userIdx as ZAM user for updating display and log an event
			DcInfo.zamUserIdx = userIdx;
			DcInfo.authReason = AUTH_INVALID_INPUT;
			// Store current credential as invalid
			StoreRcvdCred(INVALID_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
	 		DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			return;
		}

		if((GetCredentialCnt()) > DcInfo.credCnt)
		{
			if(CheckForWgndWaitforVerify() == TRUE)
			{
				DPRINT(DC_LOG, "Switch To WIEGAND User Verify Wait Stat");
				SwitchToUserVerifyWaitStat();
				return;
			}
			else
			{
				if(DcInfo.apiUser == SET )
				{
					if(SysBasicPara.apiExitAccessMode == ACCESS_MODE_QR_CODE_THEN_BIO)
					{
						READER_ID_e bioReaderId;
						if (IsReaderPresent(FP_READER, DcInfo.respPort, ANY_READER_MODE, &bioReaderId)==NO)
						{
							DcInfo.authReason = AUTH_INVALID_INPUT;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						else
						{
							DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
					}
					else if(SysBasicPara.apiExitAccessMode == ACCESS_MODE_QR_CODE_THEN_CARD)
					{
						READER_ID_e cardReaderId;
						if (IsReaderPresent(CARD_READER, DcInfo.respPort, ANY_READER_MODE, &cardReaderId)==NO)
						{
							DcInfo.authReason = AUTH_INVALID_INPUT;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						else
						{
							DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
					}
					else if(SysBasicPara.apiExitAccessMode == ACCESS_MODE_MOBILE_THEN_PIN)
					{
						if(CB_READER_BLE_CREDENTIAL == DcInfo.credType[DcInfo.credCnt-1])
						{
							DcInfo.qrResponseMode = RESP_ASK_FOR_ENTER_PIN;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
						else if(BLE_QR_CREDENTIAL == DcInfo.credType[DcInfo.credCnt-1])
						{
							READER_ID_e pinReaderId;
							if((DcInfo.respPort  == EXTERNAL_PORT) && (IsReaderPresent(EXTERNAL_PIN_READER, DcInfo.respPort, ANY_READER_MODE, &pinReaderId)==NO))
							{
								DcInfo.authReason = AUTH_INVALID_INPUT;
								DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
								return;
							}
							DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
						else
						{
							EPRINT(DC_LOG,"Unknown Credential Received for Mobile then Pin API Mode");
							SwitchDcToIdleState();
							return;
						}
					}
#if 0
					else if(SysBasicPara.apiExitAccessMode == ACCESS_MODE_MOBILE_THEN_FACE)
					{
						// Restrict user if FR is disable in device/user.
						if(IsFREnable() == NO)
						{
							DPRINT(SYS_LOG, "User Denied FR Disabled");
							DcInfo.authReason = AUTH_FR_DISABLED;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						DcInfo.qrResponseMode = RESP_ASK_FOR_SHOW_FACE;
						DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
					}
#endif
					else
					{
						EPRINT(DC_LOG,"Unknown apiExitAccessMode received");
						SwitchDcToIdleState();
						return;
					}
				}
				// will check for fp verification, will ask for next credential
				CheckNextCredToProcess();
				// set userIdx as ZAM user so that we can verify him/her later
				DcInfo.zamUserIdx = userIdx;
				// Change DC FSM state to wait for additional credential state
				SwitchDcToZAMState();
				return;
			}
		}
		else
		{
			if(DcInfo.userType == VISITOR_USER)
			{
				// if pass surrender mode enabled, allow with auth reason pass surrender
				if(SysAdvPara.visitorPassSurrenderEnblF == ENABLED)
				{
					DcInfo.authReason = AUTH_VISITOR_ALLOWED_SURRENDER_PASS;
				}
			}
			else if(DcInfo.userType == NORMAL_USER)
			{
				DcInfo.allowWithoutFinalCheck = TRUE;
				DcInfo.authReason = AUTH_ALLOWED;
			}

			CheckFinalPolicyAndAllow();
			return;
		}
	}

	if((DcInfo.apiUser == CLEAR) && (DcInfo.cbReaderBleAccess == CLEAR))
	{
	  
		// If duress don't check any access policies
		if(DcInfo.duressF == YES)
		{
			DcInfo.allowWithoutFinalCheck = TRUE;
			DcInfo.authReason = AUTH_ALLOWED_DURESS;
			DPRINT(DC_LOG,"Duress activated, User allowed without final check");
			CheckFinalPolicyAndAllow();
			return;
		}

		// Access policy ckeck on exit
		if(SysAdvPara.verifyAccessPolicyOnExitF == ENABLED)
		{
			// We are here, so appl = ACCESS_CTRL and mode = ENTRY
			// If latest credential detected is CARD and ASC enabled globally, verify ASC
			if(credentialType == CARD_CREDENTIAL)
			{
				if(SysAdvPara.addlSecurity.enable == ENABLED)
				{
					// if ASC is invalid, deny user
					if((DcInfo.addlSecCode == 0) || (DcInfo.addlSecCode != SysAdvPara.addlSecurity.code))
					{
						DcInfo.authReason = AUTH_NO_ACCESS;
						DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
						return;
					}
				}
			}
		}
	}

	// VIP has certain special priveleges. So lets handle VIP separately
	if(IsUserVIP(userIdx) == ENABLED)
	{
		DcInfo.allowWithoutFinalCheck = TRUE;
		if(GetVisitorIdx(userIdx) != INVALID_VISITOR_IDX)
		{
			//TODO: if user type is VIP and visitor
			// if pass surrender mode enabled, allow with auth reason pass surrender
			if(SysAdvPara.visitorPassSurrenderEnblF == ENABLED)
			{
				DcInfo.authReason = AUTH_VISITOR_ALLOWED_SURRENDER_PASS;
			}
		}
		//open door to allow exit to the user(s)
		CheckFinalPolicyAndAllow();
		return;
	}

	// Now perform user authentication based on user type and time
	if((AuthenticateUser()) == DENIED)
	{
		DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}

	// Lets handle Visitor separately as ZAM policy	don't apply to visitors
	if(DcInfo.userType == VISITOR_USER)
	{
		// Visitor are not allowed if 2person rule is applicable on door
		if(IsTwoPersonApplicable() == YES)
		{
			DPRINT(DC_LOG, "visitor not allowed if 2 person rule is applicable");
			DcInfo.authReason = AUTH_NO_ACCESS;
			DenyUser(userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			return;
		}

		// Check presented credential is as per ZAM policy or not
		if(IsCredentialValid() == NO)
		{
			// set userIdx as ZAM user for updating display and log an event
			DcInfo.zamUserIdx = userIdx;
			DcInfo.authReason = AUTH_INVALID_INPUT;
			// Store current credential as invalid
			StoreRcvdCred(INVALID_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
	 		DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			return;
		}

		// If more than one credentials are required to access this door,
		// we switch state where we wait for additional credential of the same user.
		if((GetCredentialCnt()) > DcInfo.credCnt)
		{
			if(CheckForWgndWaitforVerify() == TRUE)
			{
				DPRINT(DC_LOG, "Switch To WIEGAND User Verify Wait Stat");
				SwitchToUserVerifyWaitStat();
				return;
			}
			else
			{
				if(DcInfo.apiUser == SET )
				{
					if(SysBasicPara.apiExitAccessMode == ACCESS_MODE_QR_CODE_THEN_BIO)
					{
						READER_ID_e bioReaderId;
						if (IsReaderPresent(FP_READER, DcInfo.respPort, ANY_READER_MODE, &bioReaderId)==NO)
						{
							DcInfo.authReason = AUTH_INVALID_INPUT;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						else
						{
							DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
					}
					else if(SysBasicPara.apiExitAccessMode == ACCESS_MODE_QR_CODE_THEN_CARD)
					{
						READER_ID_e cardReaderId;
						if (IsReaderPresent(CARD_READER, DcInfo.respPort, ANY_READER_MODE, &cardReaderId)==NO)
						{
							DcInfo.authReason = AUTH_INVALID_INPUT;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						else
						{
							DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
					}
					else if(SysBasicPara.apiExitAccessMode == ACCESS_MODE_MOBILE_THEN_PIN)
					{
						if(CB_READER_BLE_CREDENTIAL == DcInfo.credType[DcInfo.credCnt-1])
						{
							DcInfo.qrResponseMode = RESP_ASK_FOR_ENTER_PIN;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
						else if(BLE_QR_CREDENTIAL == DcInfo.credType[DcInfo.credCnt-1])
						{
							READER_ID_e pinReaderId;
							if((DcInfo.respPort  == EXTERNAL_PORT) && (IsReaderPresent(EXTERNAL_PIN_READER, DcInfo.respPort, ANY_READER_MODE, &pinReaderId)==NO))
							{
								DcInfo.authReason = AUTH_INVALID_INPUT;
								DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
								return;
							}
							DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
							DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
						}
						else
						{
							EPRINT(DC_LOG,"Unknown Credential Received for Mobile then Pin API Mode");
							SwitchDcToIdleState();
							return;
						}
					}
#if 0
					else if(SysBasicPara.apiExitAccessMode == ACCESS_MODE_MOBILE_THEN_FACE)
					{
						// Restrict user if FR is disable in device/user.
						if(IsFREnable() == NO)
						{
							DPRINT(SYS_LOG, "User Denied FR Disabled");
							DcInfo.authReason = AUTH_FR_DISABLED;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						DcInfo.qrResponseMode = RESP_ASK_FOR_SHOW_FACE;
						DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
					}
#endif
					else
					{
						EPRINT(DC_LOG,"Unknown apiExitAccessMode received");
						SwitchDcToIdleState();
						return;
					}
				}
				// will check for fp verification, will ask for next credential
				CheckNextCredToProcess();
				// set userIdx as ZAM user so that we can verify him/her later
				DcInfo.zamUserIdx = userIdx;
				// Change DC FSM state to wait for additional credential state
				SwitchDcToZAMState();
				return;
			}
		}

		//If device is in pass surrender mode and user is visitor, then set auth reason as surrender pass
		if(SysAdvPara.visitorPassSurrenderEnblF == TRUE)
		{
			DcInfo.authReason = AUTH_VISITOR_ALLOWED_SURRENDER_PASS;
		}

		// We have gone thru all the checks and finally we can let the user in.
		CheckFinalPolicyAndAllow();
		return;
	}

	if((DcInfo.apiUser == CLEAR) && (DcInfo.cbReaderBleAccess == CLEAR))
	{
		if(IsBioCredSelfEnrollReq() == YES)
		{
			SwitchToSelfEnrlInputState();
			return;
		}
	}

	// Check presented credential is as per ZAM policy or not
	if(IsCredentialValid() == NO)
	{
		// set userIdx as ZAM user for updating display and log an event
		DcInfo.zamUserIdx = userIdx;
		DcInfo.authReason = AUTH_INVALID_INPUT;
		// Store current credential as invalid
		StoreRcvdCred(INVALID_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
 		DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}

	if((DcInfo.apiUser == CLEAR) && (DcInfo.cbReaderBleAccess == CLEAR))
	{
		if(CheckSmartRouteFeature() == PROCESS_DONE)
		{
			return;
		}
	}

	// If more than one credentials are required to access this door,
	// we switch state where we wait for additional credential of the same user.
	if((GetCredentialCnt()) > DcInfo.credCnt)
	{
		if(CheckForWgndWaitforVerify() == TRUE)
		{
			DPRINT(DC_LOG, "Switch To WIEGAND User Verify Wait Stat");
			SwitchToUserVerifyWaitStat();
			return;
		}
		else
		{
			if(DcInfo.apiUser == SET )
			{
				if(SysBasicPara.apiExitAccessMode == ACCESS_MODE_QR_CODE_THEN_BIO)
				{
					READER_ID_e bioReaderId;
					if (IsReaderPresent(FP_READER, DcInfo.respPort, ANY_READER_MODE, &bioReaderId)==NO)
					{
						DcInfo.authReason = AUTH_INVALID_INPUT;
						DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
						return;
					}
					else
					{
						DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
						DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
					}
				}
				else if(SysBasicPara.apiExitAccessMode == ACCESS_MODE_QR_CODE_THEN_CARD)
				{
					READER_ID_e cardReaderId;
					if (IsReaderPresent(CARD_READER, DcInfo.respPort, ANY_READER_MODE, &cardReaderId)==NO)
					{
						DcInfo.authReason = AUTH_INVALID_INPUT;
						DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
						return;
					}
					else
					{
						DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
						DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
					}
				}
				else if(SysBasicPara.apiExitAccessMode == ACCESS_MODE_MOBILE_THEN_PIN)
				{
					if(CB_READER_BLE_CREDENTIAL == DcInfo.credType[DcInfo.credCnt-1])
					{
						DcInfo.qrResponseMode = RESP_ASK_FOR_ENTER_PIN;
						DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
					}
					else if(BLE_QR_CREDENTIAL == DcInfo.credType[DcInfo.credCnt-1])
					{
						READER_ID_e pinReaderId;
						if((DcInfo.respPort  == EXTERNAL_PORT) && (IsReaderPresent(EXTERNAL_PIN_READER, DcInfo.respPort, ANY_READER_MODE, &pinReaderId)==NO))
						{
							DcInfo.authReason = AUTH_INVALID_INPUT;
							DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
						DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_CREDENTIAL;
						DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
					}
					else
					{
						EPRINT(DC_LOG,"Unknown Credential Received for Mobile then Pin API Mode");
						SwitchDcToIdleState();
						return;
					}
				}
#if 0
				else if(SysBasicPara.apiExitAccessMode == ACCESS_MODE_MOBILE_THEN_FACE)
				{
					// Restrict user if FR is disable in device/user.
					if(IsFREnable() == NO)
					{
						DPRINT(SYS_LOG, "User Denied FR Disabled");
						DcInfo.authReason = AUTH_FR_DISABLED;
						DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
						return;
					}
					DcInfo.qrResponseMode = RESP_ASK_FOR_SHOW_FACE;
					DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				}
#endif
				else
				{
					EPRINT(DC_LOG,"Unknown apiExitAccessMode received");
					SwitchDcToIdleState();
					return;
				}
			}
			// will check for fp verification, will ask for next credential
			CheckNextCredToProcess();
			// set userIdx as ZAM user so that we can verify him/her later
			DcInfo.zamUserIdx = userIdx;
			// Change DC FSM state to wait for additional credential state
			SwitchDcToZAMState();
			return;
		}
	}

	if(CheckTwoPersonFeature() == PROCESS_DONE)
	{
		return;
	}

	// Done with all the checks, Allow user to exit.
	CheckFinalPolicyAndAllow();
}

//*****************************************************************************
//	DcTwoPersonStateFn()
//	Param:
//		IN: msgId 	- Message which needs to be processed
//			dataPtr - Pointer to data of the message
//			length 	- Length of the message
//		OUT:None
//	Returns:
//		NONE
//	Description:
//		This is DC FSM state which waits for second valid user to present credential
//		required as 2-person rule.
//*****************************************************************************
void DcTwoPersonStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	// Based on message ID, process it
	switch(msgId)
	{
		case MSG_RPL_ERL_CONFIRM_EVENT:
		{
			RPL_ERL_CONFIRM_EV_DATA_t* data = dataPtr;
			switch(data->adaptiveErlSts)
			{
			case ADAPTIVE_ERL_CONFIRM:
			{
				UINT16 userIdx;
				IsUserValid(data->userId, &userIdx);
				strncpy(AdaptiveFrQuery.usrId, GetAlphaNumId(userIdx), ALPHANUM_ID_SIZE);
				strncpy(AdaptiveFrQuery.usrName, GetUserName(userIdx), NAME_SIZE_UTF8);
				DcInfo.userIdx = userIdx;
				DcInfo.faceIdx = data->faceIdx;
				SetTwoPersonTimer(AdaptiveFrQuery.adaptiveFrTime + 1);
				StopFRStreamForRplErlConfirm();
				SendAdaptiveConfirmScreenToQt();
			}break;

			case ADAPTIVE_ERL_CONFIRM_RESP_FAIL:
				StopFRStreamForRplErlConfirm();
				DcInfo.authReason = AUTH_ADAPTIVE_ERL_REJECT;
				SetAudioVisual(USER_RESP_ACCESS_DENIED, INTERNAL_PORT, NULL);
				SwitchDcToIdleState();
//				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
				break;

			case ADAPTIVE_ERL_CONFIRM_RESP_TIMEOUT:
				StopFRStreamForRplErlConfirm();
				DcInfo.authReason = AUTH_ADAPTIVE_ERL_TIMEOUT;
				SetAudioVisual(USER_RESP_ACCESS_DENIED, INTERNAL_PORT, NULL);
//				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
				SwitchDcToIdleState();
				break;

			case ADAPTIVE_ERL_CONFIRM_NO:
				SwitchDcToIdleState();
//				StopFRStreamForRplErlConfirm();
				DisplayIdleScreen(0);
				SetBuzLedResp(USER_RESP_IDLE,INTERNAL_PORT);
				break;

			case ADAPTIVE_ERL_CONFIRM_TIMEOUT:
//				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
				SwitchDcToIdleState();
				DisplayIdleScreen(0);
				SetBuzLedResp(USER_RESP_IDLE,INTERNAL_PORT);
				break;
			case ADAPTIVE_ERL_CONFIRM_YES:
				SetTwoPersonTimer(AdaptiveFrQuery.adaptiveFrTime + 1);
				break;
			case ADAPTIVE_ERL_CONFIRM_RESP_SUCCESS:
			case MAX_ADAPTIVE_ERL_STATUS:
			default:
				break;
			}
		}break;
		case GUI_ALNUM_KEEP_ALIVE:
		case GUI_BUTTON_FIELD_CLEAR:
		{
			RESPONSE_PORT_e respPort = *(RESPONSE_PORT_e *)dataPtr;

			// Is user entering PIN then only reload timer
			if( (DcInfo.twoPersonTmrHandle != INVALID_TIMER_HANDLE)
				&& (DcInfo.respPort == respPort) )
			{
				SetTwoPersonTimer(SysAdvPara.interDigitTimer);
				if(respPort == INTERNAL_PORT)
				{
					// Reload display idle timer
					SetLcdIdleTimer(SysAdvPara.interDigitTimer);
				}
				else
				{
					SetReaderLcdIdleTimer(SysAdvPara.interDigitTimer);
				}
			}
			break;
		}

		case GUI_BUTTON_CANCEL:
		case GUI_BUTTON_HOME:
		{
			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;
			DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
			// User response must be set idle to overcome incomplete process' response
			if( (pinRespPort == INTERNAL_PORT)   || ((NO == CheckComboRdrPresent(COMBO_READER_ATOM_RD100)) &&(pinRespPort == EXTERNAL_PORT )))
			{
			    SetAudioVisual(USER_RESP_IDLE, pinRespPort, NULL);
			}
			SwitchDcToIdleState();
			if(pinRespPort == INTERNAL_PORT)
			{
				if(FRServerConfig.faceCapturing == FACE_SERVER_TAP_AND_GO)
				{
					StopFRStreamingOnDeviceIdleScreen(TRUE);
				}
				else
				{
					if(StartFRStreamingOnDeviceIdleScreen() == SUCCESS)
					{
						if(FRServerConfig.enableFreeScanTimeOutF == TRUE)
						{
							LoadDisplayFRStreamToIdleScreenTimer(FRServerConfig.faceScanTimeOut, FALSE);
						}
					}
				}
			}
			break;
		}
		case GUI_BUTTON_PIN_ENTER_SCREEN:
		{
			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;

			if(PinEntryPossible(pinRespPort) == YES)
			{
				SetAudioVisual(USER_RESP_ENTER_PIN, pinRespPort, NULL);
				SetTwoPersonTimer(SysAdvPara.twoPersonTimer);

				if(pinRespPort == INTERNAL_PORT)
				{
					// Reload display idle timer
					SetLcdIdleTimer(SysAdvPara.twoPersonTimer);
					StopFRStreamingOnDeviceIdleScreen(FALSE);
				}
				else
				{
					SetReaderLcdIdleTimer(SysAdvPara.twoPersonTimer);
				}
			}
			break;
		}

		case MSG_PIN_DETECTED_ON_WGND_EVENT:
		{
			PIN_RESULT_e pinResp;

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetectOnWgnd((WGND_PIN_DATA_t *)dataPtr);
			if(pinResp == PIN_MATCH)
			{
				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case GUI_BUTTON_FR_FOOTER_FOR_SPL_FUN:
		{
			if(StartFRStreamingOnDeviceIdleScreen() == SUCCESS)
			{
				StreamPayload_t         streamInfo;
				streamInfo.reason = START_STREAM_GUI_FOOTER_IDLE;
				streamInfo.captureMode = STREAM_CAPTURE_FACE;
				StartWaitForFrStreamTimer(streamInfo);
// 				LoadDisplayFRStreamToIdleScreenTimer(FRServerConfig.identificationTimeOut,TRUE);
// 				SetFRStreamOverlayInfo(FALSE, MSG_MAX_LANG_STRING, FALSE, FOOTER_HOME, TRUE);
// 				ShowFRStreamOnDeviceDisplayTimer(ACCESS_WAIT_RESP);
			}
			else
			{
				SwitchDcToIdleState();
			}
			break;
		}

		// Verify the complete PIN entered by user
		case MSG_PIN_VALIDATION_EVENT:
		{
			PIN_RESULT_e pinResp;

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetect((PIN_DATA_t *)dataPtr);
			if(pinResp == PIN_MATCH)
			{
				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case MSG_SCAN_TEMPLATE_RESP:
		case MSG_FP_DETECTED_EVENT:
		{
			FP_RESULT_e	fpResp;

			// Process Finger Print detected to find a match
			if(msgId == MSG_SCAN_TEMPLATE_RESP)
			{
				fpResp = ProcessFpScanTemplateResp(dataPtr);
			}
			else
			{
				fpResp = ProcessFpDetect((FP_DETECTED_EV_DATA_t *)dataPtr);
			}

			if(fpResp == FP_MATCH)
			{
				// delete timer used For two person
				DeleteTimer(&(DcInfo.twoPersonTmrHandle));

				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			// No match found or Timeout for identification
			else if( (fpResp == FP_INVALID) || (fpResp == FP_MATCHING_TIMEOUT) )
			{
				if(fpResp == FP_MATCHING_TIMEOUT)
				{
					DcInfo.authReason = AUTH_FP_MATCHING_TIMEOUT;
				}
				else
				{
					DcInfo.authReason = AUTH_INVALID_FINGER;
				}
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case MSG_FACE_DETECTED_EVENT:
		{
			//Two Person State
			FACE_RESULT_e	faceResp;

			faceResp = ProcessFaceDetect((FACE_DETECTED_EV_DATA_t *)dataPtr);
			lastIdleTimeTick = GetSysTick();
			DPRINT(SYS_LOG, "Update lastIdleTimeTick in 2-Person [%u]", lastIdleTimeTick);
			DPRINT(DC_LOG, "Two Person State with user Idx [%d]", DcInfo.userIdx);
			if(faceResp == FACE_MATCH)
			{
				curProcFaceUserIdx = DcInfo.userIdx;
				if(IsFaceRecoEnbl(DcInfo.userIdx) == DISABLED)
				{
					DcInfo.authReason = AUTH_FR_DISABLED;
					DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
					break;
				}
				// delete timer used For two person
				DeleteTimer(&(DcInfo.twoPersonTmrHandle));

				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			// No match found or Timeout for identification
			else if(faceResp == FACE_INVALID)
			{
				if(FRServerConfig.showUnknownUserAck == true || FRServerConfig.generateUnidentifiedFaceEvent == true)
				{
					DcInfo.authReason = AUTH_INVALID_FACE;
					DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				}
			}
			else if(faceResp == FACE_MATCHING_TIMEOUT)
			{
				DcInfo.authReason = AUTH_TIMEOUT;
				DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
		}break;

		case MSG_CARD_DETECTED_EVENT:
		{
			CARD_RESULT_e	cardResp;
			CREDENTIAL_TYPE_e credType = CARD_CREDENTIAL;

			// Process card detected to find a match
			cardResp = ProcessCardDetect((CARD_DETECTED_EV_DATA_t *)dataPtr, credType);
			if(cardResp == USER_CARD_MATCH)
			{
				// delete timer used For two person
				DeleteTimer(&(DcInfo.twoPersonTmrHandle));

				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			else if(cardResp == CARD_INVALID || cardResp == SP_FUNC_CARD_MATCH)  // No match found or Spl Card
			{
				DcInfo.authReason = AUTH_INVALID_CARD;
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case MSG_BLE_QR_CODE_DETECTED:
		{
			QR_RESULT_e     qrResp;

			qrResp = ProcessQrCodeDetect((QR_CODE_DETECTED_EV_DATA_t *)dataPtr);
			if(qrResp == QR_CODE_USER_AUTHENTICATED)
			{
				DeleteTimer(&DcInfo.twoPersonTmrHandle);
				ProcessUser(DcInfo.readerMode);
			}
			else if(qrResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(qrResp == QR_CODE_DEVICE_BUSY)
			{
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else if(qrResp == QR_CODE_INVALID_READER_MODE)
			{
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ACCESS_DENIED;
			}
			else
			{
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				DcInfo.qrAuthReason = QR_AUTH_UNAUTHORIZED_ACCESS;
			}
			break;
		}

		case MSG_BT_ACCESS_REQ_DETECTED:
		{
			DPRINT(DC_LOG, "MSG_BT_ACCESS_REQ_DETECTED");
			QR_RESULT_e	btResp;

			btResp = ProcessBtAccessReqDetect((BT_ACCESS_REQ_DETECTED_EV_DATA_t *)dataPtr);
			if(btResp == QR_CODE_USER_AUTHENTICATED)
			{
				/* Set flag to indicate request received from ble */
				DcInfo.bleAccessDevice = BLE_ACCESS_DEVICE_APTA;
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATED");
				ProcessUser(DcInfo.readerMode);
			}
			else if(btResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATION_FAILED");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(btResp == QR_CODE_DEVICE_BUSY)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_DEVICE_BUSY");
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else if(btResp == QR_CODE_INVALID_READER_MODE)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_INVALID_READER_MODE");
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ACCESS_DENIED;
			}
			else if(btResp == QR_DEVICE_INVALID)
			{
				DPRINT(DC_LOG, "btResp == QR_DEVICE_INVALID ");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				// CRC is mismatched, Wrong Encryption.
				DcInfo.qrResponseMode = RESP_WRONG_ENC_CRC_MISMATCH;
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			}
			else
			{
				DPRINT(DC_LOG, "btResp == else(QR_CODE_API_DISABLED) ");
				DcInfo.qrResponseMode = RESP_USER_DENIED;
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			}
			
			if(DcInfo.curState != DC_GET_USER_STATUS_FROM_PANEL_STATE)
			{
				ACS_BLE_ACCESS_REQ_RESPONSE_t		respBtData;

				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = DcInfo.qrAuthReason;
				respBtData.respStatus = DcInfo.qrResponseMode;

				SendBLEEventTask(APTA_BLE_ACCESS_REQ_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			}
			
			break;
		}

		case MSG_BLE_ACCESS_EVENT:
		{
			QR_RESULT_e	btResp;
			CB_READER_ACCESS_REQ_RESPONSE_t		respBtData;

			DPRINT(DC_LOG, "MSG_BLE_ACCESS_EVENT");

			btResp = ProcessExtBleRdrBtAccessReqDetect((CB_READER_BLE_ACCESS_REQ_t *)dataPtr);
			if(btResp == QR_CODE_USER_AUTHENTICATED)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATED");
				ProcessUser(DcInfo.readerMode);
			}
			else if(btResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATION_FAILED");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(btResp == QR_CODE_DEVICE_BUSY)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_DEVICE_BUSY");
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else if(btResp == QR_DEVICE_INVALID)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_DEVICE_INVALID ");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				// CRC is mismatched, Wrong Encryption.
				DcInfo.qrResponseMode = CBR_RESP_WRONG_ENC_CRC_MISMATCH;
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			}
			else
			{
				DPRINT(DC_LOG, "btResp == else(QR_CODE_API_DISABLED) ");
				DcInfo.qrResponseMode = CBR_RESP_USER_DENIED;
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			}

			respBtData.readerId = DcInfo.readerId;
			respBtData.respReason = DcInfo.qrAuthReason;
			respBtData.respStatus = DcInfo.qrResponseMode;

			ProcessReaderCommand(MSG_CB_BLE_ACCESS_RESP_CMD, &respBtData, sizeof(CB_READER_ACCESS_REQ_RESPONSE_t));
		}
		break;

		case MSG_ACCESS_ID_REQ_EVENT:
		{
			QR_RESULT_e						btResp;
			ACS_BLE_ACCESS_REQ_RESPONSE_t		respBtData;

			DPRINT(DC_LOG, "MSG_MOBILE_ACCESS_EVENT");

			btResp = ProcessMobileBtAccessReqDetect((MOBILE_ACCESS_REQ_t *)dataPtr);
			if(btResp == QR_CODE_USER_AUTHENTICATED)
			{
				/* Set flag to indicate request received from ble */
				DcInfo.bleAccessDevice = BLE_ACCESS_DEVICE_ACS;
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATED");
				ProcessUser(DcInfo.readerMode);
			}
			else if(btResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATION_FAILED");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(btResp == QR_CODE_DEVICE_BUSY)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_DEVICE_BUSY");
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else if(btResp == QR_DEVICE_INVALID)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_DEVICE_INVALID ");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				// CRC is mismatched, Wrong Encryption.
				DcInfo.qrResponseMode = CBR_RESP_WRONG_ENC_CRC_MISMATCH;
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			}
			else
			{
				DPRINT(DC_LOG, "btResp == else(QR_CODE_API_DISABLED) ");
				DcInfo.qrResponseMode = CBR_RESP_USER_DENIED;
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			}

			if(DcInfo.curState != DC_GET_USER_STATUS_FROM_PANEL_STATE)
			{
				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = DcInfo.qrAuthReason;
				respBtData.respStatus = DcInfo.qrResponseMode;

				SendBLEEventTask(BLE_ACCESS_REQ_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			}
		}
		break;

		//Pin Detected from External Reader
		case MSG_PIN_DETECTED_ON_EXTERNAL_READER_EVENT:
		{
			PIN_RESULT_e pinResp;

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetectOnExternalReader((READER_PIN_DATA_t *)dataPtr);
 			if(pinResp == PIN_MATCH)
 			{
 				// User is identified as Valid User.
 				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
 			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		default:
		{
			// Events that are not handled in this state are passed to default message handler .
			DcDefaultMsgHandler(msgId ,dataPtr, length);
			break;
		}
	}
}

//*****************************************************************************
//	DcWaitForAddlCredStateFn()
//	Param:
//		IN :	msgId - Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length - Length of the message
//		OUT:	None
//	Returns:
//		NONE
//	Description:
//		This is DC FSM state which waits for user to present all credentials
//		required as per access mode policy.
//*****************************************************************************
void DcWaitForAddlCredStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	switch(msgId)
	{
		case MSG_RPL_ERL_CONFIRM_EVENT:
		{
			RPL_ERL_CONFIRM_EV_DATA_t* data = dataPtr;
			switch(data->adaptiveErlSts)
			{
			case ADAPTIVE_ERL_CONFIRM:
			{
				UINT16 userIdx;
				IsUserValid(data->userId, &userIdx);
				strncpy(AdaptiveFrQuery.usrId, GetAlphaNumId(userIdx), ALPHANUM_ID_SIZE);
				strncpy(AdaptiveFrQuery.usrName, GetUserName(userIdx), NAME_SIZE_UTF8);
				DcInfo.userIdx = userIdx;
				DcInfo.faceIdx = data->faceIdx;
				LoadMultiAccessTimer(AdaptiveFrQuery.adaptiveFrTime + 1);
				StopFRStreamForRplErlConfirm();
				SendAdaptiveConfirmScreenToQt();
			}break;

			case ADAPTIVE_ERL_CONFIRM_RESP_FAIL:
				StopFRStreamForRplErlConfirm();
				DcInfo.authReason = AUTH_ADAPTIVE_ERL_REJECT;
				SetAudioVisual(USER_RESP_ACCESS_DENIED, INTERNAL_PORT, NULL);
				SwitchDcToIdleState();
//				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
				break;

			case ADAPTIVE_ERL_CONFIRM_RESP_TIMEOUT:
				StopFRStreamForRplErlConfirm();
				DcInfo.authReason = AUTH_ADAPTIVE_ERL_TIMEOUT;
				SetAudioVisual(USER_RESP_ACCESS_DENIED, INTERNAL_PORT, NULL);
//				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
				SwitchDcToIdleState();
				break;

			case ADAPTIVE_ERL_CONFIRM_YES:
				LoadMultiAccessTimer(AdaptiveFrQuery.adaptiveFrTime + 1);
				break;
			case ADAPTIVE_ERL_CONFIRM_NO:
				SwitchDcToIdleState();
//				StopFRStreamForRplErlConfirm();
				DisplayIdleScreen(0);
				SetBuzLedResp(USER_RESP_IDLE,INTERNAL_PORT);
				break;
			case ADAPTIVE_ERL_CONFIRM_TIMEOUT:
//				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
				SwitchDcToIdleState();
				DisplayIdleScreen(0);
				SetBuzLedResp(USER_RESP_IDLE,INTERNAL_PORT);
				break;

			case ADAPTIVE_ERL_CONFIRM_RESP_SUCCESS:
			case MAX_ADAPTIVE_ERL_STATUS:
			default:
				break;
			}
		}break;
		case GUI_ALNUM_KEEP_ALIVE:
		case GUI_BUTTON_FIELD_CLEAR:
		{
			RESPONSE_PORT_e respPort = *(RESPONSE_PORT_e *)dataPtr;

			// Is user entering PIN then only reload timer
			if( (DcInfo.multiAccessTmrHandle != INVALID_TIMER_HANDLE)
				&& (DcInfo.respPort == respPort) )
			{
				LoadMultiAccessTimer(SysAdvPara.interDigitTimer);

				if(respPort == INTERNAL_PORT)
				{
					// Reload display idle timer
					SetLcdIdleTimer(SysAdvPara.interDigitTimer);
				}
				else
				{
					SetReaderLcdIdleTimer(SysAdvPara.interDigitTimer);
				}
			}
			break;
		}

		case GUI_BUTTON_GOING_IN:
		case GUI_BUTTON_GOING_OUT:
		case GUI_BUTTON_MENU:
		{
			if(IsWaitingForAcsBleCredential() == YES)
			{
				SetAudioVisual(USER_RESP_DEVICE_BUSY,INTERNAL_PORT,NULL);
			}
		}
		break;

		case GUI_BUTTON_CANCEL:
		case GUI_BUTTON_HOME:
		{
			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;
			DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
			// User response must be set idle to overcome incomplete process' response
			if( (pinRespPort == INTERNAL_PORT)   || ((NO == CheckComboRdrPresent(COMBO_READER_ATOM_RD100)) &&(pinRespPort == EXTERNAL_PORT )))
			{
			    SetAudioVisual(USER_RESP_IDLE, pinRespPort, NULL);
			}
			SwitchDcToIdleState();
			if(pinRespPort == INTERNAL_PORT)
			{
				if(FRServerConfig.faceCapturing == FACE_SERVER_TAP_AND_GO)
				{
					StopFRStreamingOnDeviceIdleScreen(TRUE);
				}
				else
				{
					if(StartFRStreamingOnDeviceIdleScreen() == SUCCESS)
					{
						if(FRServerConfig.enableFreeScanTimeOutF == TRUE)
						{
							LoadDisplayFRStreamToIdleScreenTimer(FRServerConfig.faceScanTimeOut, FALSE);
						}
					}
				}
			}
			break;
		}
		
		case GUI_BUTTON_PIN_ENTER_SCREEN:
		{
			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;

			if(IsWaitingForAcsBleCredential() == YES)
			{
				SetAudioVisual(USER_RESP_DEVICE_BUSY, pinRespPort, NULL);
				break;
			}

			if(PinEntryPossible(pinRespPort) == YES)
			{
				SetAudioVisual(USER_RESP_ENTER_PIN, pinRespPort, NULL);
				LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);

				if(pinRespPort == INTERNAL_PORT)
				{
					// Reload display idle timer
					SetLcdIdleTimer(SysAdvPara.multiAccessTimer);
					StopFRStreamingOnDeviceIdleScreen(FALSE);
				}
				else
				{
					SetReaderLcdIdleTimer(SysAdvPara.multiAccessTimer);
				}
			}
			break;
		}

		case MSG_PIN_DETECTED_ON_WGND_EVENT:
		{
			PIN_RESULT_e pinResp;

			if(IsWaitingForAcsBleCredential() == YES)
			{
				SetAudioVisual(USER_RESP_DEVICE_BUSY,GetReaderRespPort(((WGND_PIN_DATA_t *)dataPtr)->readerId),NULL);
				break;
			}

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetectOnWgnd((WGND_PIN_DATA_t *)dataPtr);
			if(pinResp == PIN_MATCH)
			{
				// User is identified as Valid User.
				// Now process him further on entry as ZAM is applicable only on entry
				ProcessZam();
			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		// Verify the complete PIN entered by user
		case MSG_PIN_VALIDATION_EVENT:
		{
			PIN_RESULT_e pinResp;

			if(IsWaitingForAcsBleCredential() == YES)
			{
				SetAudioVisual(USER_RESP_DEVICE_BUSY,INTERNAL_PORT,NULL);
				break;
			}

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetect((PIN_DATA_t *)dataPtr);
			if(pinResp == PIN_MATCH)
			{
				// User is identified as Valid User.
				// Now process him further on entry as ZAM is applicable only on entry
				ProcessZam();
			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case MSG_SCAN_TEMPLATE_RESP:
		case MSG_FP_DETECTED_EVENT:
		{
			FP_RESULT_e	fpResp;

			if(IsWaitingForAcsBleCredential() == YES)
			{
				if(msgId == MSG_SCAN_TEMPLATE_RESP)
				{
					SetAudioVisual(USER_RESP_DEVICE_BUSY,GetReaderRespPort(((SCAN_TEMPLATE_RESP_DATA_t *)dataPtr)->readerId),NULL);
				}
				else
				{
					SetAudioVisual(USER_RESP_DEVICE_BUSY,GetReaderRespPort(((FP_DETECTED_EV_DATA_t *)dataPtr)->readerId),NULL);
				}
				break;
			}

			// Process Finger Print detected to find a match
			if(msgId == MSG_SCAN_TEMPLATE_RESP)
			{
				fpResp = ProcessFpScanTemplateResp(dataPtr);
			}
			else
			{
				fpResp = ProcessFpDetect((FP_DETECTED_EV_DATA_t *)dataPtr);
			}

			if(fpResp == FP_MATCH)
			{
 				// User is identified as Valid User.
				// Now process him further for ZAM.
				ProcessZam();
			}
			// No match found or Timeout for identification
			else if( (fpResp == FP_INVALID) || (fpResp == FP_MATCHING_TIMEOUT) )
			{
				if(fpResp == FP_MATCHING_TIMEOUT)
				{
					DcInfo.authReason = AUTH_FP_MATCHING_TIMEOUT;
				}
				else
				{
					DcInfo.authReason = AUTH_INVALID_FINGER;
				}

				DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case MSG_FACE_DETECTED_EVENT:
		{
			//Additional Credential State
			FACE_RESULT_e	faceResp;

			if(IsWaitingForAcsBleCredential() == YES)
			{
				SetAudioVisual(USER_RESP_DEVICE_BUSY,INTERNAL_PORT,NULL);
				break;
			}

			DPRINT(DC_LOG, "Additional Credential State");
			lastIdleTimeTick = GetSysTick();
			DPRINT(SYS_LOG, "Update lastIdleTimeTick in Additional Cred [%u]", lastIdleTimeTick);
			// Process Finger Print detected to find a match
			faceResp = ProcessFaceDetect((FACE_DETECTED_EV_DATA_t *)dataPtr);

			if(faceResp == FACE_MATCH)
			{
				curProcFaceUserIdx = DcInfo.userIdx;
 				// User is identified as Valid User.
				// Now process him further for ZAM.
				ProcessZam();
			}
			// No match found or Timeout for identification
			else if(faceResp == FACE_INVALID)
			{
				if(FRServerConfig.showUnknownUserAck == true || FRServerConfig.generateUnidentifiedFaceEvent == true)
				{
					DcInfo.authReason = AUTH_INVALID_FACE;
					DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				}
			}
			else if(faceResp == FACE_MATCHING_TIMEOUT)
			{
				DcInfo.authReason = AUTH_TIMEOUT;
				DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case GUI_BUTTON_FR_FOOTER_FOR_SPL_FUN:
		{
			if(StartFRStreamingOnDeviceIdleScreen() == SUCCESS)
			{
				StreamPayload_t         streamInfo;
				streamInfo.reason = START_STREAM_GUI_FOOTER_IDLE;
				streamInfo.captureMode = STREAM_CAPTURE_FACE;
				StartWaitForFrStreamTimer(streamInfo);
// 				LoadDisplayFRStreamToIdleScreenTimer(FRServerConfig.identificationTimeOut,TRUE);
// 				SetFRStreamOverlayInfo(FALSE, MSG_MAX_LANG_STRING, FALSE, FOOTER_HOME, TRUE);
// 				ShowFRStreamOnDeviceDisplayTimer(ACCESS_WAIT_RESP);
			}
			else
			{
				SwitchDcToIdleState();
			}
			break;
		}

		case MSG_CARD_DETECTED_EVENT:
		{
			CARD_RESULT_e	cardResp;
			CREDENTIAL_TYPE_e credType = CARD_CREDENTIAL;

			if(IsWaitingForAcsBleCredential() == YES)
			{
				SetAudioVisual(USER_RESP_DEVICE_BUSY,GetReaderRespPort(((CARD_DETECTED_EV_DATA_t *)dataPtr)->readerId),NULL);
				break;
			}

			// Process card detected to find a match
			cardResp = ProcessCardDetect((CARD_DETECTED_EV_DATA_t *)dataPtr, credType);
			if(cardResp == USER_CARD_MATCH)
			{
 				// We are here, so appl should be ACCESS_CTRL and mode should be ENTRY
				// If ASC enabled, verify ASC

				if(SysBasicPara.application == ACCESS_CTRL)
				{
					if(SysAdvPara.addlSecurity.enable == ENABLED)
					{
						// if ASC is invalid, deny user
						if( (DcInfo.addlSecCode == 0) ||
									(DcInfo.addlSecCode != SysAdvPara.addlSecurity.code) )
						{
							DcInfo.authReason = AUTH_NO_ACCESS;
							DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
							return;
						}
					}
				}

				// User is identified as Valid User.
				// Now process him further on entry as ZAM is applicable only on entry
				ProcessZam();
			}
			else if( (cardResp == CARD_INVALID) || (cardResp == SP_FUNC_CARD_MATCH) )
			{
 				DcInfo.authReason = AUTH_INVALID_CARD;
				DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		// for FP 1:1 verification (provided template v/s live scanned finger)
		case MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_RESP:
		// for FP 1:1 verification (user's template that is on reader v/s live scanned finger)
		case MSG_VERIFY_BY_SCAN_RESP:
		{
			READER_RESP_STATUS_e 	status;

			if(msgId == MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_RESP)
			{
				status = ( (VERIFY_HOST_TEMPLATE_BY_SCAN_RESP_DATA_t *) dataPtr) -> status;
				DPRINT(DC_LOG, "rcvd MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_RESP, status = %s", ReaderRespStatusStr[status]);
			}
			else
			{
				status = ((VERIFY_BY_SCAN_RESP_DATA_t *)dataPtr)->status;
				DPRINT(DC_LOG, "rcvd MSG_VERIFY_BY_SCAN_RESP, status = %s", ReaderRespStatusStr[status]);
			}

			DcInfo.verifyingFpByScanF = CLEAR;	// fp scanning is done

			if(status == READER_RESP_STATUS_SUCCESS)
			{
				// Store credential
				StoreRcvdCred(FP_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
				// Process user for ZAM
				ProcessZam();
			}
			// if finger scan is succesful tell user to remove his finger
			else if(status == READER_RESP_STATUS_SCAN_SUCCESS)
			{
				SetAudioVisual(USER_RESP_REMOVE_FINGER, DcInfo.respPort, NULL);
				// extend time to wait for getting verification response completely
				LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);
			}
			// ignore timeout response of reader
			else if( (status != READER_RESP_STATUS_TIME_OUT) &&
									(status != READER_RESP_STATUS_NOT_FOUND) )
			{
				DcInfo.authReason = AUTH_INVALID_FINGER;
				// Store credential
				StoreRcvdCred(FP_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
				DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		// response for read finger print template from card
		case MSG_READ_FP_RESP:
		{
			READ_FP_RESP_DATA_t	* readFpFromCard = dataPtr;

			DPRINT(DC_LOG, "rcvd MSG_READ_FP_RESP, status = %s", ReaderRespStatusStr[readFpFromCard -> status]);

			// If template is read successfully
			if(readFpFromCard -> status == READER_RESP_STATUS_SUCCESS)
			{
				if( (readFpFromCard -> template1F == SET) || (readFpFromCard -> template2F == SET) )
				{
					READER_ID_e fpReaderId;

					if(IsReaderPresent(FP_READER, DcInfo.respPort, ANY_READER_MODE, &fpReaderId) == YES)
					{
						DPRINT(DC_LOG, "Read fp from card successfully, verify host template by scan, 1:1 matching");

						// SEND command VERIFY_BY_SCAN to finger print reader
						VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t	verifyByScan;
						verifyByScan.readerId = fpReaderId;
						// set no. of templates
						verifyByScan.noOfTemplates = 0;
						if(readFpFromCard -> template1F == SET)
							verifyByScan.noOfTemplates++;
						if(readFpFromCard -> template2F == SET)
							verifyByScan.noOfTemplates++;

						verifyByScan.responseTime = 0;
						// copy template data
						if( (readFpFromCard -> template1F == SET) && (readFpFromCard -> template2F == SET) )
							memcpy(&verifyByScan.template, &readFpFromCard -> template, MAX_SIZE_DOUBLE_SFM_TEMPLATES);
						else
							memcpy(&verifyByScan.template, &readFpFromCard -> template, MAX_SIZE_SINGLE_SFM_TEMPLATE);


						SendVerifyHostTemplateByScanCmd(verifyByScan);

						// set flag- system is waiting for user finger to be scanned
						DcInfo.verifyingFpByScanF = SET;
					}
				}

				if(GetAccessModeFromRespPort(DcInfo.userType, DcInfo.respPort) == FINGER_PRINT_ONLY )
				{
					SetAudioVisual(USER_RESP_PLACE_FINGER_BIO_ONLY_MODE, DcInfo.respPort, NULL);
				}

				// Set audio visual for next credential
				else if(DcInfo.credCnt == SECOND_CREDENTIAL)
				{
					// Update audio/visual to prompt for more credentials
					SetAudioVisual(USER_RESP_SHOW_2ND_CREDENTIAL, DcInfo.respPort, NULL);
				}
				else
				{
					// Update audio/visual to prompt for more credentials
					SetAudioVisual(USER_RESP_SHOW_3RD_CREDENTIAL, DcInfo.respPort, NULL);
				}
				LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);
			}
			else
			{
				DcInfo.authReason = AUTH_CARD_READ_FAILED;
				DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case MSG_USER_TEMPLATE_CNT_RESP:
		{
			USER_TEMPLATE_CNT_RESP_DATA_t * userFpCntRespPtr = dataPtr;

			DPRINT(DC_LOG, "MSG_USER_TEMPLATE_CNT_RESP: status = %s", ReaderRespStatusStr[userFpCntRespPtr -> status]);

			if(userFpCntRespPtr -> status == READER_RESP_STATUS_SUCCESS)
			{
				DPRINT(DC_LOG, "MSG_USER_TEMPLATE_CNT_RESP: userFpcnt %d", userFpCntRespPtr -> templateCnt);

				DcInfo.verifyingFpByScanF = SET;

				// userFpCntRespPtr -> templateCnt will be double in case of TWO Template mode
				if(userFpCntRespPtr -> templateCnt >= DcInfo.userFpcnt)
				{
					DPRINT(DC_LOG, "Need To Send MSG_VERIFY_BY_SCAN_CMD");

					VERIFY_BY_SCAN_CMD_DATA_t	verifyByScan;

					verifyByScan.readerId = userFpCntRespPtr->readerId;
					verifyByScan.fprUserId = GetUserId(DcInfo.userIdx);
					verifyByScan.responseTime = SysAdvPara.multiAccessTimer;
					ProcessReaderCommand(MSG_VERIFY_BY_SCAN_CMD, &verifyByScan, sizeof(VERIFY_BY_SCAN_CMD_DATA_t));
				}
				else
				{
					DPRINT(DC_LOG, "Need To Send MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD, responseTime[%u]",SysAdvPara.multiAccessTimer);

					VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t	verifyByScan;
					UINT8 fpIdx;

					verifyByScan.readerId = userFpCntRespPtr->readerId;
					verifyByScan.noOfTemplates = DcInfo.userFpcnt;
					verifyByScan.responseTime = SysAdvPara.multiAccessTimer;

					for(fpIdx = 0; fpIdx < verifyByScan.noOfTemplates; fpIdx++)
					{
						ReadUserFpFromFile(DcInfo.userIdx, fpIdx, &verifyByScan.template[(fpIdx * MAX_SIZE_SINGLE_SFM_TEMPLATE)]);
					}
					SendVerifyHostTemplateByScanCmd(verifyByScan);
				}

				if( (GetAccessModeFromRespPort(DcInfo.userType, DcInfo.respPort) == FINGER_PRINT_ONLY ) && (DcInfo.apiUser == CLEAR) && (DcInfo.cbReaderBleAccess == CLEAR) )
				{
					SetAudioVisual(USER_RESP_PLACE_FINGER_BIO_ONLY_MODE, DcInfo.respPort, NULL);
				}
				// Set audio visual for next credential
				else if(DcInfo.credCnt == SECOND_CREDENTIAL)
				{
					// Update audio/visual to prompt for more credentials
					SetAudioVisual(USER_RESP_SHOW_2ND_CREDENTIAL, DcInfo.respPort, NULL);
				}
				else
				{
					// Update audio/visual to prompt for more credentials
					SetAudioVisual(USER_RESP_SHOW_3RD_CREDENTIAL, DcInfo.respPort, NULL);
				}
			}
			else
			{
				EPRINT(DC_LOG, "ERROR MSG_USER_TEMPLATE_CNT_RESP, status = %s", ReaderRespStatusStr[userFpCntRespPtr -> status]);
			}

			break;
		}

		case MSG_MOBILE_PIN_EVENT:
		{
			QR_RESULT_e						btResp;
			ACS_BLE_ACCESS_REQ_RESPONSE_t	respBtData;

			DPRINT(DC_LOG, "MSG_MOBILE_PIN_EVENT");
			btResp = ProcessMobileBtAccessPinDetect((MOBILE_ACCESS_PIN_REQ_t *)dataPtr);

			if(btResp == QR_CODE_USER_AUTHENTICATED)
			{
				DontShowAttendanceDetail();
				ProcessZam();
			}
			else if(btResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DontShowAttendanceDetail();
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(btResp == QR_CODE_DEVICE_BUSY)
			{
				DcInfo.qrAuthReason = ACS_RESP_STATUS_NONE;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else
			{	//QR_CODE_API_DISABLED
				DcInfo.qrResponseMode = ACS_RESP_USER_DENIED;
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			}

			if(DcInfo.curState != DC_GET_USER_STATUS_FROM_PANEL_STATE)
			{
				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = DcInfo.qrAuthReason;
				respBtData.respStatus = DcInfo.qrResponseMode;
				
				SendBLEEventTask(INT_BLE_PIN_ACCESS_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			}
		}
		break;

		case MSG_MOBILE_FACE_EVENT:
		{
			QR_RESULT_e						btResp;
			ACS_BLE_ACCESS_REQ_RESPONSE_t	respBtData;

			DPRINT(DC_LOG, "MSG_MOBILE_FACE_EVENT");
			btResp = ProcessMobileBtAccessFaceDetect((MOBILE_ACCESS_FACE_REQ_t *)dataPtr);

			if(btResp == QR_CODE_USER_AUTHENTICATED)
			{
				DontShowAttendanceDetail();
				ProcessZam();
			}
			else if(btResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DontShowAttendanceDetail();
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(btResp == QR_CODE_DEVICE_BUSY)
			{
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else
			{	//QR_CODE_API_DISABLED
				DcInfo.qrResponseMode = ACS_RESP_USER_DENIED;
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			}

			if(DcInfo.curState != DC_GET_USER_STATUS_FROM_PANEL_STATE)
			{
				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = DcInfo.qrAuthReason;
				respBtData.respStatus = DcInfo.qrResponseMode;
				
				SendBLEEventTask(INT_BLE_FACE_ACCESS_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			}
		}
		break;

		//Pin Detected from External Reader
		case MSG_PIN_DETECTED_ON_EXTERNAL_READER_EVENT:
		{
			PIN_RESULT_e pinResp;

			/** Ignore External Pin Request if door waiting for pin on ACS */
			if(IsWaitingForAcsBleCredential() == YES)
			{
				SetAudioVisual(USER_RESP_DEVICE_BUSY,((READER_PIN_DATA_t *)dataPtr)->respPort,NULL);
				break;
			}

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetectOnExternalReader((READER_PIN_DATA_t *)dataPtr);
			if(pinResp == PIN_MATCH)
			{
				// User is identified as Valid User.
				// Now process him further on entry as ZAM is applicable only on entry
				ProcessZam();
			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		default:
		{
			// Events that are not handled in this state are passed to default message handler .
			DcDefaultMsgHandler(msgId, dataPtr, length);
			break;
		}
	}
}

//*****************************************************************************
//	DcHttpFirmwareUploadStateFn()
//	Param:
//		IN :	msgId - Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length - Length of the message
//		OUT:	None
//	Returns:
//		NONE
//	Description:
//		This is DC FSM state which waits till firmware upload complete through
//		http.
//*****************************************************************************
static void DcHttpFirmwareUploadStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	switch(msgId)
	{
		default:
			DcDefaultMsgHandler(msgId, dataPtr, length);
			break;
	}
}

//*****************************************************************************
//	DcAcmsFirmwareUploadStateFn()
//	Param:
//		IN :	msgId - Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length - Length of the message
//		OUT:	None
//	Returns:
//		NONE
//	Description:
//		This is DC FSM state which waits till firmware upload complete through
//		acms
//*****************************************************************************
static void DcAcmsFirmwareUploadStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	switch(msgId)
	{
		default:
			DcDefaultMsgHandler(msgId, dataPtr, length);
			break;
	}
}

//*****************************************************************************
//	DcPanelDoorFirmwareUploadStateFn()
//	Param:
//		IN :	msgId - Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length - Length of the message
//		OUT:	None
//	Returns:
//		NONE
//	Description:
//		This is DC FSM state which waits till Panel Door firmware upload complete
//		through panel
//*****************************************************************************
static void DcPanelDoorFirmwareUploadStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	switch(msgId)
	{
		default:
			DcDefaultMsgHandler(msgId, dataPtr, length);
			break;
	}
}

//*****************************************************************************
//	ProcessZam
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//			This function will check whether user's additional credential is as
//			per access mode policy.
//			This function should be called only after identifying user.
//	Prerequisite:
//			- User identified and authenticated using first credential.
//			- DCTask state = DC_WAIT_ADDL_CRED_STATE
//*****************************************************************************
static void ProcessZam(void)
{
	DeleteTimer(&(DcInfo.multiAccessTmrHandle) );

	// If user is invalid or not the same which needs to be processed, deny user
	if(DcInfo.userIdx != DcInfo.zamUserIdx)
	{
 		// If second input is from different user, we are denying the first user
		// who is being processed for zam
		DcInfo.authReason = AUTH_INVALID_INPUT;
		// Store current credential as invalid
		StoreRcvdCred(INVALID_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
		DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}
	
	if(DcInfo.duressF == YES)
	{
		DcInfo.authReason = AUTH_ALLOWED_DURESS;

		CheckFinalPolicyAndAllow();
		return;
	}
	
	if( (DcInfo.credType[DcInfo.credCnt - 1] == FACE_CREDENTIAL) && (IsFaceRecoEnbl(DcInfo.userIdx) == DISABLED) )
	{
		DcInfo.authReason = AUTH_FR_DISABLED;
		DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}

	// If credential is not valid as per access mode, deny user
	if(IsCredentialValid() == NO)
	{
 		DcInfo.authReason = AUTH_INVALID_INPUT;
		// Store current credential as invalid
		StoreRcvdCred(INVALID_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
		DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
		return;
	}

	if(SysBasicPara.application == TA)
	{
		if(GetCredentialCnt() > DcInfo.credCnt)
		{
			//More credentials are required. Prompt user for additional credential
			CheckNextCredToProcess();
			LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);
			return;
		}
		else
		{
			if(DcInfo.userType == VISITOR_USER)
			{
				// if pass surrender mode enabled, allow with auth reason pass surrender
				if(SysAdvPara.visitorPassSurrenderEnblF == ENABLED)
				{
					DcInfo.authReason = AUTH_VISITOR_ALLOWED_SURRENDER_PASS;
				}
			}
			else if(DcInfo.userType == NORMAL_USER)
			{
				DcInfo.allowWithoutFinalCheck = TRUE;
				DcInfo.authReason = AUTH_ALLOWED;
			}
			CheckFinalPolicyAndAllow();
			return;
		}
	}

	if(DcInfo.userType == VISITOR_USER)
	{
		//Valid credential found
		//Check if more credentials are required for the user
		if(GetCredentialCnt() > DcInfo.credCnt)
		{
	 		//More credentials are required. Prompt user for additional credential
			CheckNextCredToProcess();
			LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);
			return;
		}

		//If device is in pass surrender mode and user is visitor, then set auth reason as surrender pass
		if(SysAdvPara.visitorPassSurrenderEnblF == TRUE)
		{
			DcInfo.authReason = AUTH_VISITOR_ALLOWED_SURRENDER_PASS;
		}
	 	//open door to allow access to user
		CheckFinalPolicyAndAllow();
		return;
	}

	if(DcInfo.smartRouteActiveF == TRUE)
	{
		// If current credential is card then only process route access further
		if(DcInfo.credType[(DcInfo.credCnt) - 1] == CARD_CREDENTIAL)
		{
			if(SmartRouteAccessConfig[GetUserSmartRouteId(DcInfo.userIdx)].routeType == SMART_ROUTE_ACCESS_HARD)
			{
				ProcessHardRouteOnEntry();
			}
			else
			{
				ProcessSoftRouteOnEntry();
			}
			return;
		}
	}

	//Valid credential found
	//Check if more credentials are required for the user
	if(GetCredentialCnt() > DcInfo.credCnt)
	{
 		//More credentials are required. Prompt user for additional credential
		CheckNextCredToProcess();
		LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);
		return;
	}

	// All necessary credentials have been verified
	// Process user access request further for door policies

	if(CheckTwoPersonFeature() == PROCESS_DONE)
	{
		return;
	}

 	//open door to allow access to user
	CheckFinalPolicyAndAllow();
}

//*****************************************************************************
//	ProcessUserOnExitOnExternal
//	Param:
//		IN :
//				msgId	- message id
//				dataPtr	- points to data tied with message id
//		OUT:	none
//	Returns:	none
//	Description:
//				This function processes user request for exit on external port.
//				This function should be called from state other than IDLE
//				where user can be allowed for exit while continuing
//				process as per state requirement.
//	NOTE:	1.	This function doesn't change DC task state.
//			2.	It changes variables related to door activity info
//*****************************************************************************
void ProcessUserOnExitOnExternal(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr)
{
	UINT16					userIdx;
	BOOL					dcActivityInitF = FALSE;

	// First identify user
	switch(msgId)
	{
		case MSG_CARD_DETECTED_EVENT:
		{
			CARD_DETECTED_EV_DATA_t * ptr = dataPtr;
			CREDENTIAL_TYPE_e credType = CARD_CREDENTIAL;
			UINT32 credSubType = ACCESS_CARD1;

			if(msgId == MSG_CAMERA_QR_CODE_DETECTED_EVENT)
			{
				credType = ACCESS_CARD_QR_CREDENTIAL;
			}

			DcInfo.cardCredntlDetectedF = TRUE;
			// Deny user if card is not user card
			if(IsCardValid(ptr -> cardId, &userIdx, &credSubType) == NO)
			{
				StoreRcvdCred(credType, credSubType);
				DcInfo.authReason = AUTH_INVALID_CARD;
				DenyUser(userIdx, EXIT_READER, EXTERNAL_PORT, DC_STATE_NO_CHANGE);
				dcActivityInitF = TRUE;
			}
			break;
		}

		case MSG_FP_DETECTED_EVENT:
		{
			FP_DETECTED_EV_DATA_t * ptr = dataPtr;

			// Ignore scan success response
			if(ptr -> status == READER_RESP_STATUS_SCAN_SUCCESS)
			{
				DcInfo.fpBeingProcessed = TRUE;
				return;
			}
			else if(ptr -> status == READER_RESP_STATUS_SUCCESS)
			{
				// FP reader detected finger with some userID.
				IsUserValid(ptr -> fprUserId, &userIdx);
			}
			else
			{
				userIdx = INVALID_USER;
			}

			StoreRcvdCred(FP_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
			DcInfo.fpBeingProcessed = FALSE;

			// Lets check if user exists in our database.
			if( (userIdx == INVALID_USER) || (GetUserId(userIdx) == INVALID_USER_ID) )
			{
				DcInfo.authReason = AUTH_INVALID_FINGER;
				DenyUser(userIdx, EXIT_READER, EXTERNAL_PORT, DC_STATE_NO_CHANGE);
				dcActivityInitF = TRUE;
			}
			break;
		}
		//Will Face Be processed on external??

		default:
		{
			return;
		}
	}

	// User Identified, now apply access policies
	if(dcActivityInitF == TRUE)
	{
		InitDoorActivityInfo(FALSE);
		return;
	}

	ValidateUserOnExitOnExternalInNonIdleState(userIdx);

	if(IsAuthAllowedReason(DcInfo.authReason) == YES)
	{
		//UserAllowed(userIdx, EXIT_READER, EXTERNAL_PORT, DC_STATE_NO_CHANGE);
		/* Check additional Features */
		ProcAdditionalFeatureCheck(DcInfo.curState);
	}
	else
	{
		DenyUser(userIdx, EXIT_READER, EXTERNAL_PORT, DC_STATE_NO_CHANGE);
	}

	// We came here means input (card or finger) is processed, so we need to init door activity
	// irrespective of user allowed/denied or input is ignored
	InitDoorActivityInfo(FALSE);
}

//*****************************************************************************
//	ValidateUserOnExitOnExternalInNonIdleState()
//	Param:
//		IN :	UINT16		userIdx
//		OUT:	None
//	Returns:	None
//	Description:
//				This function validate user satisfy all required condition
//			to allow or not.
//*****************************************************************************
static void ValidateUserOnExitOnExternalInNonIdleState(UINT16 userIdx)
{
	ANTIPASSBACK_STATUS_e 	apbStatus;

	DcInfo.authReason = AUTH_ALLOWED;

	if(IsUserVIP(userIdx) == ENABLED)
	{
		DcInfo.userType = VIP_USER;
	}
	else if(GetVisitorIdx(userIdx) != INVALID_VISITOR_IDX)
	{
		DcInfo.userType = VISITOR_USER;
	}
	else
	{
		DcInfo.userType = NORMAL_USER;
	}

	if( (DcMonitorControl.doorLock == DOOR_LOCKED) && (SysBasicPara.exitOnLock == DISABLED) )
	{
		DcInfo.authReason = AUTH_DOOR_LOCK;
		return;
	}

	// Allow user to exit unconditionally if door is unlocked
	if(DcMonitorControl.doorLock == DOOR_UNLOCKED)
	{
		DcInfo.authReason = AUTH_ALLOWED;
		return;
	}

	// Access policy ckeck on exit
	if(SysAdvPara.verifyAccessPolicyOnExitF == ENABLED)
	{
		if(IsUserEnable(userIdx) == DISABLED)
		{
			DcInfo.authReason = AUTH_USER_DISABLED;
			return;
		}

		if( (IsTerminationEnbl(userIdx) == ACTIVE) &&
			(IsUserValidityExpired(GetTerminationDate(userIdx)) == YES) )
		{
			DcInfo.authReason = AUTH_VALIDITY_EXPIRED;
			return;
		}
	}

	// For entry mode, check unconditionally and for exit mode if access policy check is enabled
	if( (DcInfo.readerMode == ENTRY_READER) || (SysAdvPara.verifyAccessPolicyOnExitF == ENABLED) )
	{
		// Check if user is visitor
		if(DcInfo.userType == VISITOR_USER)
		{
			// Check validity period of visitor
			if(IsVisitorValid(GetVisitorIdx(userIdx)) == NO)
			{
				// Visitor is not in valid period.
				DPRINT(DC_LOG, "visitor validity expired");
				DcInfo.authReason = AUTH_NO_ACCESS;
				return;
			}
		}
	}

	if(SysBasicPara.application == TA)
	{
		DcInfo.authReason = AUTH_ALLOWED;
		return;
	}

	if(SysAdvPara.verifyAccessPolicyOnExitF == ENABLED)
	{
		if( (SysAdvPara.addlSecurity.enable == ENABLED) &&
			(DcInfo.credType[DcInfo.credCnt - 1] == CARD_CREDENTIAL) )
		{
			if( (DcInfo.addlSecCode != SysAdvPara.addlSecurity.code) || (DcInfo.addlSecCode == 0) )
			{
				DcInfo.authReason = AUTH_NO_ACCESS;
				return;
			}
		}
	}

	if(IsUserVIP(userIdx) == ENABLED)
	{
		DcInfo.userType = VIP_USER;
		DcInfo.authReason = AUTH_ALLOWED;
		if(GetVisitorIdx(userIdx) != INVALID_VISITOR_IDX)
		{
			//TODO: if user type is VIP and visitor
			// if pass surrender mode enabled, allow with auth reason pass surrender
			if(SysAdvPara.visitorPassSurrenderEnblF == ENABLED)
			{
				DcInfo.authReason = AUTH_VISITOR_ALLOWED_SURRENDER_PASS;
			}
		}
		return;
	}

	if(SysAdvPara.verifyAccessPolicyOnExitF == ENABLED)
	{
		if(IsUserBlocked(userIdx, NULL) == YES)
		{
			DcInfo.authReason = AUTH_USER_BLOCKED;
			return;
		}
	}

	// Check if user violated anti pass back
	apbStatus = GetAntiPassBackStatus(userIdx, EXIT_READER);
	if(apbStatus == HARD_VIOLATION)
	{
		DcInfo.authReason = AUTH_PASSBACK_EXIT_HARD_VIOLATION;
		return;
	}
	else if(apbStatus == SOFT_VIOLATION)
	{
		// Save the authentication reason
		DcInfo.authReason = AUTH_PASSBACK_EXIT_SOFT_VIOLATION;
	}

	// Access policy check on exit
	if(SysAdvPara.verifyAccessPolicyOnExitF == ENABLED)
	{
		if(CheckTimeBasedAccess() == DENIED)
		{
			DcInfo.authReason = AUTH_NO_ACCESS;
			return;
		}
	}

	if(IsCredentialValid() == NO)
	{
		DcInfo.authReason = AUTH_INVALID_INPUT;
		return;
	}

	if( (SysAdvPara.smartRouteAccessEnbl == ENABLED) &&
		 (GetUserSmartRouteId(userIdx) != USER_SMART_ROUTE_NONE) &&
		 (IsDoorInAnySmartRoute() == TRUE) )
	{
		DcInfo.authReason = AUTH_NO_ACCESS;
		return;
	}

	// iF required more than 1 credential then deny user
	if(GetCredentialCnt() > 1)
	{
		DcInfo.authReason = AUTH_NO_ACCESS;
		return;
	}

	if(IsTwoPersonApplicable() == YES)
	{
		DcInfo.authReason = AUTH_TWO_PERSON;
		return;
	}

	//if state change require ignore
	DPRINT(DC_LOG,"IsStateChangeRequiresForOccupancyCheck() = %u\n",IsStateChangeRequiresForOccupancyCheck());

	if(IsStateChangeRequiresForOccupancyCheck() == YES)
	{
		DcInfo.authReason = AUTH_NO_ACCESS;
		return;
	}
}

//*****************************************************************************
//	SwitchDcTo2PersonState()
//	Param:
//		IN :	none
//		OUT:	None
//	Returns:	None
//	Description:
//				This function switches DCTask FSM to two person state and
//				initializes required parameters.
//*****************************************************************************
void SwitchDcTo2PersonState(void)
{
	// Initialise all necessary members of door activity structure for next person
	InitCredInfo();
	DcInfo.qrAuthReason = QR_AUTH_TWO_PERSON;
	DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_USER;

	// Set current user index as invalid to process next user
	DcInfo.userIdx = INVALID_USER;
	DcInfo.zamUserIdx = INVALID_USER;

	// Start 2-person timer
	if(SetTwoPersonTimer(SysAdvPara.twoPersonTimer) == FAIL)
	{
		EPRINT(DC_LOG,"Unable to start Two Person Timer");
	}

	IsFaceAlreadyIdentifyF = FALSE;
	CleanUpTempUserFP();
	// Change DC slave FSM state to 2 person state
	DcInfo.curState = DC_WAIT_FOR_2ND_PERSON_STATE;
	DcInfo.usrExceedTempThreshold = FALSE;
	DcInfo.usrTemperature = 0.0;
#if defined(FACE_MASK_COMPULSION_ENABLE)
	DcInfo.isFaceMaskViolated = FALSE;
#endif
}

//*****************************************************************************
//	SwitchDcToZAMState()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function switches DCTask FSM to wait for additional
//				credential state and initializes required parameters.
//*****************************************************************************
void SwitchDcToZAMState(void)
{
	/* Check if FR stream is started for Face Credential */
	if(TRUE == IsStartFrStreamForFaceCredFailed())
	{
		return;
	}
	curProcFaceUserIdx = UNKNOWN_USER_IDX;
	IsFaceAlreadyIdentifyF = FALSE;
	// load time to get next credential
	LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);

	CleanUpTempUserFP();
	// Change DC slave FSM state to wait for additional credential state
	DcInfo.curState = DC_WAIT_ADDL_CRED_STATE;
}

//*****************************************************************************
//	SwitchDcToBackupupdateState()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns: NONE
//	Description:
//		This function makes all the necessary changes for door to switch in
//		backup and update state.
//*****************************************************************************
void SwitchDcToBackupupdateState(void)
{
	CleanUpTempUserFP();
	DcInfo.curState = DC_BACKUP_UPDATE_STATE;
}

//*****************************************************************************
//	SwitchDcToSSAState()
//	Param:
//		IN :	doorId
//		OUT:	None
//	Returns:	None
//	Description:
//				This function switches DCTask FSM to smart secure accessGroup
//				state
//*****************************************************************************
static void SwitchDcToSSAState(void)
{
	READ_SSA_INFO_CMD_DATA_t	readSSCDataCmd;

	DPRINT(DC_LOG, "This is SSA card and preparing to read data");

	DcInfo.userIdx = MAX_USERS;
	// SEND command READ_SSA_INFO_CMD_DATA_t to card reader
	readSSCDataCmd.readerId = DcInfo.readerId;
	readSSCDataCmd.cardType = DcInfo.cardType;
	readSSCDataCmd.userIdF = SET;
	readSSCDataCmd.alphaNumIdF = SET;
	readSSCDataCmd.userNameF = SET;
	readSSCDataCmd.accessLevelF = SET;
	readSSCDataCmd.validityF = SET;
	readSSCDataCmd.vipF = SET;
	readSSCDataCmd.bypassFingerF = SET;
	readSSCDataCmd.pinF = SET;

	//if smart route enabled and door in route then only reader routid and route level
	if((GetSmartRouteAccessEnblCfg() == ENABLED) && (IsDoorInAnySmartRoute() == YES))
	{
		readSSCDataCmd.accessRouteF = SET;
		readSSCDataCmd.smartAccessRouteLevelF = SET;
	}
	else
	{
		readSSCDataCmd.accessRouteF = CLEAR;
		readSSCDataCmd.smartAccessRouteLevelF = CLEAR;
	}

	// load time to get next credential
	LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);
	ProcessReaderCommand(MSG_READ_SSA_INFO_CMD, &readSSCDataCmd, sizeof(READ_SSA_INFO_CMD_DATA_t));
	SetAudioVisual(USER_RESP_READING_FROM_CARD, DcInfo.respPort, NULL);

	DcInfo.procSSAF = TRUE;
	CleanUpTempUserFP();
	DcInfo.curState = DC_SSA_STATE;
}

//*****************************************************************************
//	UserAllowed
//	Param:
//		IN :	userIdx		- user index for which allowed event is to be logged
//				readerMode	- entry or exit reader mode
//				respPort 	- internal or external response port
//		OUT:	none
//	Returns:	none
//	Description:
//				It opens door if not opened already so that user can
//				enter/exit.
//				It logs user allowed event as per authentication reason.
//				It gives AV response (not if allowed through REX).
//				It updates occupant, anti pass back list, absentee info.
//				It activates IO linking- input port-user allowed.
//
//	NOTES:	1.  If user index is unknown pass it as 'UNKNOWN_USER_IDX'.
//			2.	Upon Mantrap timeout, this function will be called and
//				mantrap could have been be initiated through REX.
//*****************************************************************************
void UserAllowed(UINT16 userIdx, READER_MODE_e readerMode, RESPONSE_PORT_e respPort, DC_STATE_CHANGE_e dcSateChange)
{
	EVENT_ID_e				evId;
	UINT32 					userId, jobRefId = INVALID_JOB_REF_ID;
	USER_EVENT_INFO_e 		evMode;
	EV_FIELD3				evField3;
	UINT8					splCode;
	USER_RESP_e				userResp;
	BOOL					evWriteF = SUCCESS;
	UINT8					deniedUserIdx;
	BOOL					isTempSoftThresholdViolated = FALSE;
	BOOL					isToSendAllow101Event = TRUE;
	UINT32					eventSeqNumForUserLocation;
	UINT32					eventRollOverCntForUserLocation;

	// If user index is unknown then user is allowed through REX
	// NOTE: no user response to door if allowing though REX
	if(userIdx == UNKNOWN_USER_IDX)
	{
		DcInfo.userIdDoorOpenCloseEvent = INVALID_USER_ID;
		memset(DcInfo.alphanumIddoorOpenCloseEvent, 0, sizeof(DcInfo.alphanumIddoorOpenCloseEvent));
		OpenDoor();
		// Decrement occupancy count by one as at least one user is exiting
		DecrementOccupancyCount();
		// Log event
		if(SysAdvPara.genExitSwEvent == ENABLED)
		{
			evMode = GetEvMode(EXIT_READER);
			// Write common event log
			evField3 = (EV_FIELD3)(GetUserCredEvFieldInfo(CURRENT_USER) + evMode);
			WriteEventLog(USER_ALLOW, INVALID_USER_ID, USER_SPECIAL_CODE_NONE, evField3, UNUSED_FIELD, UNUSED_FIELD);

			if(SysAdvPara.dvrAllowEventuserType != VISITORS_ONLY)
			{
				WriteDvrEvt(DVR_CONFIG_EVENT_USER_ALLOWED, DVR_SEND_EVENT_USER_ALLOWED, INVALID_USER, EXIT_READER);
			}
		}
		if(SysAdvPara.dvrAllowEventuserType != VISITORS_ONLY)
		{
			// Update IO Link
			UpdateIoLinkInPortStatus(USER_ALLOWED_TRIGGER, ACTIVE);
		}
	}
	else if(DcInfo.procSSAF == TRUE)
	{
		if( (SysBasicPara.extWiegandRdrMode == WIGND_RDR_MODE_OUTPUT) && (DcInfo.wiegandOutProcessF == WIGND_OUT_PROCESS_IDLE)
			&& ( (SysBasicPara.wiegandOutFormat != WIGND_OUT_FRMT_CUSTOM) || (SysBasicPara.wgndOutCstmFrmtNo[ALLOWED_EV] != 0) ) )
		{
			BOOL	resVal;

			resVal = SendUserInfoOnExtWgndRdrPort(userIdx, ALLOWED_EV, 0);
			if( (resVal == SUCCESS) && (SysBasicPara.waitForAccessFeedback == TRUE) )
			{
				SwitchToUserAllowWaitStat();
				return;
			}
		}
		// set user id for event log
		userId = GetUserId(userIdx);
		// Store given index into doorActivityInfo which will be used for AV response
		// Update last user processed
		DcInfo.lastUserIdx = userIdx;
		splCode = GetCurrSplCode();

 		if( splCode == USER_SPECIAL_CODE_NONE)
 		{
 			splCode = GetActiveScheduleSplFnCode(&CurrTimeDate);
 			splCode != 0 ? DPRINT(DC_LOG, "SCHEDULED SPECIAL FUNTION: %s",specialCode[splCode]): 0;
 		}
 		else
 		{
 			DPRINT(DC_LOG, "SPECIAL FUNTION: %s",specialCode[splCode]);
 		}

 		/* Check if soft restriction is violated */
		if ((DcInfo.usrExceedTempThreshold == TRUE) &&
				((SysAdvPara.temperatureLogCnfg.restrictionType == USER_RESTRICTION_SOFT)
				|| (DcInfo.qrAccessDecision == TEMP_ACCESS_DECISION_ACCESS_ALLOWED_SOFT)))
		{
			isTempSoftThresholdViolated = TRUE;
			isToSendAllow101Event = FALSE;
		}

#if defined (FACE_MASK_COMPULSION_ENABLE)
		/* Check if user face mask not detected*/
		if(IsUserFaceMaskSoftViolated(DcInfo.faceMaskAccessDecision))
		{
			isToSendAllow101Event = FALSE;
		}
#endif

		evMode = GetEventModeFromSplCode(readerMode, splCode);

		// Log event as per authentication reason
		evId = GetEventIdFromAuthReason(DcInfo.authReason);
		// Write common event log
		evField3 = (EV_FIELD3)(GetUserCredEvFieldInfo(CURRENT_USER) | evMode);
		/* Check if user allowed 101 event to be sent */
		if ((evId != USER_ALLOW) || ((evId == USER_ALLOW) && (isToSendAllow101Event == TRUE)))
		{
			evWriteF = (evWriteF && WriteEventLog(evId, userId, splCode, evField3, UNUSED_FIELD, UNUSED_FIELD));
		}

		if(evWriteF == SUCCESS)
		{
			DcInfo.userIdDoorOpenCloseEvent = userId;
			strncpy(DcInfo.alphanumIddoorOpenCloseEvent, GetAlphaNumId(SSA_USER_IDX), ALPHANUM_ID_SIZE);

			OpenDoor();
			userResp = USER_RESP_ACCESS_ALLOWED;
			// Send AV response to door
			SetAudioVisual(userResp, respPort, NULL);
			if( (SysAdvPara.dvrAllowEventuserType == ALL_USERS) || ((SysAdvPara.dvrAllowEventuserType == USERS_ONLY) && (DcInfo.userType != VISITOR_USER)) )
			{
				WriteDvrEvt(DVR_CONFIG_EVENT_USER_ALLOWED, DVR_SEND_EVENT_USER_ALLOWED, userIdx, readerMode);
				// Update IO Link
				UpdateIoLinkInPortStatus(USER_ALLOWED_TRIGGER, ACTIVE);
			}

#if defined (FACE_MASK_COMPULSION_ENABLE)
			/* Check if user face mask not detected*/
			if (IsUserFaceMaskSoftViolated(DcInfo.faceMaskAccessDecision))
			{
				// Write face mask soft violation event
				WriteEventLog(USER_ALLOW_FACE_MASK_SOFT_VIOLATION, userId, splCode, evField3, UNUSED_FIELD, UNUSED_FIELD);
			}
#endif

			/* Check if soft restriction is violated */
			if (isTempSoftThresholdViolated == TRUE)
			{
				// Write soft threshold reached event log
				WriteEventLog(USER_ALLOW_TEMP_SOFT_THRESHOLD_EXCEED, userId, splCode, evField3, UNUSED_FIELD, UNUSED_FIELD);
			}

			/* Check if user temperature is valid */
			if (DcInfo.usrTemperature != 0.0)
			{
				/* Generate user temperature event */
				GenerateUserTemperatureEvent(userIdx, DcInfo.usrTemperature, DcInfo.qrTempUnit, DcInfo.qrAccessDecision);
			}
		}
		else
		{
			if( (SysAdvPara.dvrDenyEventUserType == ALL_USERS) || ((SysAdvPara.dvrDenyEventUserType == USERS_ONLY) && (DcInfo.userType != VISITOR_USER)) )
			{
				WriteDvrEvt(DVR_CONFIG_EVENT_USER_DENIED, DVR_SEND_EVENT_USER_DENIED, userIdx, readerMode);
			}

			userResp = USER_RESP_ACCESS_DENIED;
			DcInfo.authReason = AUTH_EVENT_LOG_FAIL;
			SetAudioVisual(userResp, respPort, NULL);
		}

		//Remover User From Deny list
		if(DcInfo.smartRouteActiveF == TRUE)
		{
			RemoveSiUserFromDeniedRoute();
		}

		// Deactivate single user special code in case it is active
		DcInfo.singleUserSplCode = USER_SPECIAL_CODE_NONE;
	}
	else // known user is allowed
	{
		if( (SysBasicPara.extWiegandRdrMode == WIGND_RDR_MODE_OUTPUT) && (DcInfo.wiegandOutProcessF == WIGND_OUT_PROCESS_IDLE)
			&& ( (SysBasicPara.wiegandOutFormat != WIGND_OUT_FRMT_CUSTOM) || (SysBasicPara.wgndOutCstmFrmtNo[ALLOWED_EV] != 0) ) )
		{
			BOOL	resVal;

			resVal = SendUserInfoOnExtWgndRdrPort(userIdx, ALLOWED_EV, 0);
			if( (resVal == SUCCESS) && (SysBasicPara.waitForAccessFeedback == TRUE) )
			{
				SwitchToUserAllowWaitStat();
				return;
			}
		}
		// set user id for event log
		userId = GetUserId(userIdx);
		// Store given index into doorActivityInfo which will be used for AV response
		DcInfo.userIdx = userIdx;
		// Update last user processed
		DcInfo.lastUserIdx = userIdx;
 		splCode = GetCurrSplCode();

 		if( splCode == USER_SPECIAL_CODE_NONE)
		{
			splCode = GetActiveScheduleSplFnCode(&CurrTimeDate);
			splCode != 0 ? DPRINT(DC_LOG, "SCHEDULED SPECIAL FUNTION: %s",specialCode[splCode]): 0;
		}
		else
		{
			DPRINT(DC_LOG, "SPECIAL FUNTION: %s",specialCode[splCode]);
		}

		evMode = GetEventModeFromSplCode(readerMode, splCode);

		if(SysAdvPara.firstInResetAfter == FIRST_IN_RESET_AFTER_TIMER_EXPIRE)
		{
			if(IsValidFirstInUser() == YES)
			{
				if(LoadFirstInResetTimer() == FAIL)
				{
					EPRINT(DC_LOG,"fail to load first in reset timer");
				}
			}
		}

		//get Job ref ID
		if(DcInfo.selectedJobIdx < (MAX_JCM_CONFIG + 3) )
		{
			jobRefId = JcmConfig[DcInfo.selectedJobIdx].refId;
		}

		// Write common event log
		if(DcInfo.firstUserIdx != INVALID_USER)
		{
			if(DcInfo.firstUsrGrpDtl == SECONDARY_GRP)
			{
				evId = USER_ALLOW_2PERSON_SECONDARY;
			}
			else if(DcInfo.firstUsrGrpDtl == PRIMARY_GRP)
			{
				evId = USER_ALLOW_2PERSON_PRIMARY;
			}
			else
			{
				if(DcInfo.authReason == AUTH_ALLOW_2PERSON_PRIMARY)
				{
					evId = USER_ALLOW_2PERSON_SECONDARY;
				}
				else
				{
					evId = USER_ALLOW_2PERSON_PRIMARY;
				}
			}

			evField3 = (EV_FIELD3)(GetUserCredEvFieldInfo(FIRST_USER) + evMode);
			/* check if first person exceeded soft temperature threshold */
            if (((IsTempratureConfigApplicable() == TRUE)
                    || (DcInfo.qrAccessDecision != TEMP_ACCESS_DECISION_ACCESS_DENIED_TEMPERATURE_EXCEEDED))
					&& (DcInfo.firstUsrExceedTempThreshold == TRUE))
			{
				isTempSoftThresholdViolated = TRUE;
				isToSendAllow101Event = FALSE;
			}

#if defined (FACE_MASK_COMPULSION_ENABLE)
			/* Check if user face mask not detected*/
			if (IsUserFaceMaskSoftViolated(DcInfo.firstUserFaceMaskAccessDecision))
			{
				isToSendAllow101Event = FALSE;
			}
#endif

			/* Check if user allowed 101 event to be sent */
			if ((evId != USER_ALLOW) || ((evId == USER_ALLOW) && (isToSendAllow101Event == TRUE)))
			{
				evWriteF = (evWriteF && WriteEventLog(evId, GetUserId(DcInfo.firstUserIdx),
													splCode, evField3, jobRefId, UNUSED_FIELD) );
			}

#if defined (FACE_MASK_COMPULSION_ENABLE)
			/* Check if user face mask not detected*/
			if (IsUserFaceMaskSoftViolated(DcInfo.firstUserFaceMaskAccessDecision))
			{
				// Write face mask soft violation event
				evWriteF = (evWriteF && WriteEventLog(USER_ALLOW_FACE_MASK_SOFT_VIOLATION, GetUserId(DcInfo.firstUserIdx),
								splCode, evField3, UNUSED_FIELD, UNUSED_FIELD));
			}
#endif

			/* check if first person exceeded soft temperature threshold */
			if (isTempSoftThresholdViolated == TRUE)
			{
				// Write soft threshold reached event log
				evWriteF = (evWriteF && WriteEventLog(USER_ALLOW_TEMP_SOFT_THRESHOLD_EXCEED, GetUserId(DcInfo.firstUserIdx),
								splCode, evField3, UNUSED_FIELD, UNUSED_FIELD));
			}

			/* Check if first user temperature is valid */
			if (DcInfo.firstUsrTemperature != 0.0)
			{
				/* Generate first user temperature event */
				evWriteF = (evWriteF && GenerateUserTemperatureEvent(DcInfo.firstUserIdx, DcInfo.firstUsrTemperature, DcInfo.firstUsrQrTempUnit, DcInfo.firstUsrQrAccessDecision));
			}
		}

		if((DcInfo.firstOccupantIdx != INVALID_USER) && (DcInfo.firstOccupantIdx != DcInfo.firstUserIdx))
		{
			evId = GetEventIdFromAuthReason(DcInfo.firstOccupantAuthReason);
			evField3 = (EV_FIELD3)(GetUserCredEvFieldInfo(FIRST_OCCUPANT) + evMode);
			/* Reset required variables */
			isTempSoftThresholdViolated = FALSE;
			isToSendAllow101Event = TRUE;

			/* check if first person exceeded soft temperature threshold */
            if (((IsTempratureConfigApplicable() == TRUE)
					|| (DcInfo.qrAccessDecision != TEMP_ACCESS_DECISION_ACCESS_DENIED_TEMPERATURE_EXCEEDED))
					&& (DcInfo.firstUsrExceedTempThreshold == TRUE))
			{
				isTempSoftThresholdViolated = TRUE;
				isToSendAllow101Event = FALSE;
			}

#if defined (FACE_MASK_COMPULSION_ENABLE)
			/* Check if user face mask not detected*/
			if (IsUserFaceMaskSoftViolated(DcInfo.firstUserFaceMaskAccessDecision))
			{
				isToSendAllow101Event = FALSE;
			}
#endif

			/* Check if user allowed 101 event to be sent */
			if ((evId != USER_ALLOW) || ((evId == USER_ALLOW) && (isToSendAllow101Event == TRUE)))
			{
				evWriteF = (evWriteF && WriteEventLog(evId, GetUserId(DcInfo.firstOccupantIdx), splCode, evField3, jobRefId, UNUSED_FIELD));
			}

#if defined (FACE_MASK_COMPULSION_ENABLE)
			/* Check if user face mask not detected*/
			if (IsUserFaceMaskSoftViolated(DcInfo.firstUserFaceMaskAccessDecision))
			{
				// Write face mask soft violation event
				evWriteF = (evWriteF && WriteEventLog(USER_ALLOW_FACE_MASK_SOFT_VIOLATION, GetUserId(DcInfo.firstOccupantIdx),
								splCode, evField3, UNUSED_FIELD, UNUSED_FIELD));
			}
#endif

			/* check if first person exceeded soft temperature threshold */
			if (isTempSoftThresholdViolated == TRUE)
			{
				// Write soft threshold reached event log
				evWriteF = (evWriteF && WriteEventLog(USER_ALLOW_TEMP_SOFT_THRESHOLD_EXCEED, GetUserId(DcInfo.firstOccupantIdx),
								splCode, evField3, UNUSED_FIELD, UNUSED_FIELD));
			}

			/* Check if first user temperature is valid */
			if (DcInfo.firstUsrTemperature != 0.0)
			{
				/* Generate first user temperature event */
				evWriteF = (evWriteF && GenerateUserTemperatureEvent(DcInfo.firstOccupantIdx, DcInfo.firstUsrTemperature, DcInfo.firstUsrQrTempUnit, DcInfo.firstUsrQrAccessDecision));
			}
		}

		/* Reset required variables */
		isToSendAllow101Event = TRUE;
		isTempSoftThresholdViolated = FALSE;

 		/* Check if soft restriction is violated */
		if ((DcInfo.usrExceedTempThreshold == TRUE) &&
				((SysAdvPara.temperatureLogCnfg.restrictionType == USER_RESTRICTION_SOFT)
				|| (DcInfo.qrAccessDecision == TEMP_ACCESS_DECISION_ACCESS_ALLOWED_SOFT)))
		{
			isTempSoftThresholdViolated = TRUE;
			isToSendAllow101Event = FALSE;
		}

#if defined (FACE_MASK_COMPULSION_ENABLE)
		/* Check if user face mask not detected*/
			if (IsUserFaceMaskSoftViolated(DcInfo.firstUserFaceMaskAccessDecision))
		{
			isToSendAllow101Event = FALSE;
		}
#endif

		// Log event as per authentication reason
		evId = GetEventIdFromAuthReason(DcInfo.authReason);

		evField3 = (EV_FIELD3)(GetUserCredEvFieldInfo(CURRENT_USER) + evMode);
		if ((evId != USER_ALLOW) || ((evId == USER_ALLOW) && (isToSendAllow101Event == TRUE)))
		{
			if(evId == USER_ALLOW)
			{
				eventSeqNumForUserLocation = GetCurEventCount() + 1;
				eventRollOverCntForUserLocation = GetCurEventRollOverCount();
			}
			evWriteF = (evWriteF && WriteEventLog(evId, userId, splCode, evField3, jobRefId, UNUSED_FIELD) );
		}

		if(evWriteF == SUCCESS)
		{
			#if defined (FACE_MASK_COMPULSION_ENABLE)
			/* Check if user face mask not detected*/
			if (IsUserFaceMaskSoftViolated(DcInfo.faceMaskAccessDecision))
			{
				// Write face mask soft violation event
				WriteEventLog(USER_ALLOW_FACE_MASK_SOFT_VIOLATION, userId, splCode, evField3, UNUSED_FIELD, UNUSED_FIELD);
			}
			#endif

			/* Check if soft restriction is violated */
			if (isTempSoftThresholdViolated == TRUE)
			{
				// Write soft threshold reached event log
				WriteEventLog(USER_ALLOW_TEMP_SOFT_THRESHOLD_EXCEED, userId, splCode, evField3, UNUSED_FIELD, UNUSED_FIELD);
			}

			if (DcInfo.usrTemperature != 0.0)
			{
				/* Generate user temperature event */
				GenerateUserTemperatureEvent(userIdx, DcInfo.usrTemperature, DcInfo.qrTempUnit, DcInfo.qrAccessDecision);
			}

			if((latitudeQR != INVALID_LATITUDE_VALUE) && (longitudeQR != INVALID_LONGITUDE_VALUE) && ((evId == USER_ALLOW) && (isToSendAllow101Event == TRUE)) )
			{
				/* Generate user location event */
				GenerateUserLocationEvent(eventRollOverCntForUserLocation,eventSeqNumForUserLocation);
			}

			// Open door only if access not restricted
			if(IsUserAccessRestricted(userIdx) == FALSE)
			{
				DcInfo.userIdDoorOpenCloseEvent = userId;
				strncpy(DcInfo.alphanumIddoorOpenCloseEvent, GetAlphaNumId(userIdx), sizeof(DcInfo.alphanumIddoorOpenCloseEvent));
				OpenDoor();
				//If valid job reference ID
				if(jobRefId != INVALID_JOB_REF_ID)
				{
					userResp = USER_RESP_ACCESS_ALLOWED_WITH_JOBCODE;
				}
				else
				{
					userResp = USER_RESP_ACCESS_ALLOWED;
				}
			}
			else
			{
				userResp = USER_RESP_ACCESS_EVENT_REGISTERED;
				DcInfo.authReason = AUTH_EVENT_REGISTERED;
			}

			if( (SysAdvPara.showAttndcDtlF == TRUE) && (respPort == INTERNAL_PORT) &&
				(DcInfo.userType != VISITOR_USER) && (IsAttndcDtlShow(DcInfo.userIdx) == TRUE) &&
				(CheckAttedanceSummaryForUserIdx(DcInfo.userIdx) == SUCCESS) )
			{
				LoadShowAtdncDtlTmr();
			}

			// Send AV response to door
			SetAudioVisual(userResp, respPort, NULL);

			if(readerMode == ENTRY_READER)
			{
				// Add user into anti pass back list and update absentee info
				if(DcInfo.firstUserIdx != INVALID_USER)
				{
					UpdateUserAbsentee(DcInfo.firstUserIdx);
					AddUserIntoApbList(DcInfo.firstUserIdx);
				}
				if((DcInfo.firstOccupantIdx != INVALID_USER) && (DcInfo.firstOccupantIdx != DcInfo.firstUserIdx))
				{
					UpdateUserAbsentee(DcInfo.firstOccupantIdx);
					AddUserIntoApbList(DcInfo.firstOccupantIdx);
				}
				UpdateUserAbsentee(userIdx);
				AddUserIntoApbList(userIdx);
			}
			else
			{
				// Remove user entry from anti pass back list
				if(DcInfo.firstUserIdx != INVALID_USER)
				{
					RemoveUserFromApbList(DcInfo.firstUserIdx, USER_NOT_REMOVEDFROM_SYS);
				}
				if((DcInfo.firstOccupantIdx != INVALID_USER) && (DcInfo.firstOccupantIdx != DcInfo.firstUserIdx))
				{
					RemoveUserFromApbList(DcInfo.firstOccupantIdx, USER_NOT_REMOVEDFROM_SYS);
				}

				RemoveUserFromApbList(userIdx, USER_NOT_REMOVEDFROM_SYS);
			}
			if(DcInfo.firstUserIdx != INVALID_USER)
			{
				if( (SysAdvPara.dvrAllowEventuserType == ALL_USERS) || ((SysAdvPara.dvrAllowEventuserType == USERS_ONLY) && (DcInfo.userType != VISITOR_USER)) )
				{
					WriteDvrEvt(DVR_CONFIG_EVENT_USER_ALLOWED, DVR_SEND_EVENT_USER_ALLOWED, DcInfo.firstUserIdx, readerMode);
				}

				if( (DcInfo.smartRouteActiveF == TRUE) && (IsUserInDeniedRouteList( DcInfo.firstUserIdx , &deniedUserIdx) == YES) )
				{
					RemoveUserFromDeniedRouteList(deniedUserIdx);
				}
			}
			if((DcInfo.firstOccupantIdx != INVALID_USER) && (DcInfo.firstOccupantIdx != DcInfo.firstUserIdx))
			{
				if( (SysAdvPara.dvrAllowEventuserType == ALL_USERS) || ((SysAdvPara.dvrAllowEventuserType == USERS_ONLY) && (GetVisitorIdx(DcInfo.firstOccupantIdx) != INVALID_VISITOR_IDX)) )
				{
					WriteDvrEvt(DVR_CONFIG_EVENT_USER_ALLOWED, DVR_SEND_EVENT_USER_ALLOWED, DcInfo.firstOccupantIdx, readerMode);
				}

				if( (DcInfo.smartRouteActiveF == TRUE) && (IsUserInDeniedRouteList( DcInfo.firstOccupantIdx , &deniedUserIdx) == YES) )
				{
					RemoveUserFromDeniedRouteList(deniedUserIdx);
				}
			}

			if( (SysAdvPara.dvrAllowEventuserType == ALL_USERS) || ((SysAdvPara.dvrAllowEventuserType == USERS_ONLY) && (DcInfo.userType != VISITOR_USER)) )
			{
				WriteDvrEvt(DVR_CONFIG_EVENT_USER_ALLOWED, DVR_SEND_EVENT_USER_ALLOWED, DcInfo.userIdx, readerMode);
			}

			if( (DcInfo.smartRouteActiveF == TRUE) && (IsUserInDeniedRouteList( DcInfo.userIdx , &deniedUserIdx) == YES) )
			{
				RemoveUserFromDeniedRouteList(deniedUserIdx);
			}
			
			// Update IO Link
			if(DcInfo.duressF == YES)
			{
				DcInfo.duressF = NO;
				UpdateIoLinkInPortStatus(USER_ALLOWED_WITH_DURESS, ACTIVE);
			}
			else
			{
				if( (SysAdvPara.dvrAllowEventuserType == ALL_USERS) || ((SysAdvPara.dvrAllowEventuserType == USERS_ONLY) && (DcInfo.userType != VISITOR_USER)) )
				{
					// Update IO Link
					UpdateIoLinkInPortStatus(USER_ALLOWED_TRIGGER, ACTIVE);
				}
			}
			
			if(DcInfo.firstInTempLockF == FALSE)
			{
				UnlockFirstInAccess();
				DPRINT(DC_LOG, "FirstIn Lock status=Unlocked");
				if(SysAdvPara.firstInResetAfter == FIRST_IN_RESET_AFTER_TIMER_EXPIRE)
				{
					if(LoadFirstInResetTimer() == FAIL)
					{
						EPRINT(DC_LOG,"fail to load first in reset timer");
					}
				}
			}
		}
		else
		{
			if(DcInfo.firstUserIdx != INVALID_USER)
			{
				if( (SysAdvPara.dvrDenyEventUserType == ALL_USERS) || ((SysAdvPara.dvrDenyEventUserType == USERS_ONLY) && (DcInfo.userType != VISITOR_USER)) )
				{
					WriteDvrEvt(DVR_CONFIG_EVENT_USER_DENIED, DVR_SEND_EVENT_USER_DENIED, DcInfo.firstUserIdx, readerMode);
				}
			}

			if((DcInfo.firstOccupantIdx != INVALID_USER) && (DcInfo.firstOccupantIdx != DcInfo.firstUserIdx))
			{
				if( (SysAdvPara.dvrDenyEventUserType == ALL_USERS) || ((SysAdvPara.dvrDenyEventUserType == USERS_ONLY) && (GetVisitorIdx(DcInfo.firstOccupantIdx) != INVALID_VISITOR_IDX)) )
				{
					WriteDvrEvt(DVR_CONFIG_EVENT_USER_DENIED, DVR_SEND_EVENT_USER_DENIED, DcInfo.firstOccupantIdx, readerMode);
				}
			}

			if( (SysAdvPara.dvrDenyEventUserType == ALL_USERS) || ((SysAdvPara.dvrDenyEventUserType == USERS_ONLY) && (DcInfo.userType != VISITOR_USER)) )
			{
				WriteDvrEvt(DVR_CONFIG_EVENT_USER_DENIED, DVR_SEND_EVENT_USER_DENIED, DcInfo.userIdx, readerMode);
			}

			userResp = USER_RESP_ACCESS_DENIED;
			DcInfo.authReason = AUTH_EVENT_LOG_FAIL;
			SetAudioVisual(userResp, respPort, NULL);
		}

		// Deactivate single user special code in case it is active
		DcInfo.singleUserSplCode = USER_SPECIAL_CODE_NONE;
	}

	if((DcInfo.apiUser == SET) || (DcInfo.cbReaderBleAccess == SET))
	{
		if(evWriteF == SUCCESS)
		{
			if(IsUserAccessRestricted(userIdx) == FALSE)
			{
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ACCESS_ALLOWED;
			}
			else  // for specially if restrict access flag is true than we have to show denied response for qr code
			{
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ACCESS_DENIED;
			}
			SetQrCodeAuthReasonInDcInfo();
		}
		else
		{
			DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ACCESS_DENIED;
			DcInfo.qrAuthReason = QR_AUTH_SYS_ERROR;
		}
		if(DcInfo.curState == DC_GET_USER_STATUS_FROM_PANEL_STATE)
		{
			SeqInOutCredType = DcInfo.credType[((DcInfo.credCnt)-1)];
		}
	}

	if((DISABLED == BioIdentifyServer.enblBioServer))
	{
		CleanUpTempUserFP();
	}

	if(dcSateChange == DC_STATE_SWITCH_TO_IDLE)
	{
		SwitchDcToIdleState();
	}
}

//*****************************************************************************
//	DenyUser
//	Param:
//		IN :	userIdx	- user index for which allowed event is to be logged
//				raederMode	- entry or exit reader mode
//				respPort 	- internal or external response port
//		OUT:	none
//	Returns:	none
//	Description:
//			It logs user denied event as per authentication reason, checks
//			for the maximum attempts for ENTRY mode and also sets Av Response.
//
//	NOTE:		If user index is unknown pass it as 'UNKNOWN_USER_IDX'
//*****************************************************************************
void DenyUser(UINT16 userIdx, READER_MODE_e readerMode, RESPONSE_PORT_e respPort, DC_STATE_CHANGE_e dcSateChange)
{
	EVENT_ID_e				evId;
	UINT32					userId;
	USER_EVENT_INFO_e		evMode;
	EV_FIELD3				evField3;
	EV_FIELD2				evField2;
	USER_RESP_e				userResp;
	AUTH_REASON_e			tempAuthReason = AUTH_INVALID_FACE;

	if(SysBasicPara.application == CAFETERIA)
	{
		SendCafeRespToAcs(USER_RESP_CAFE_DENIED);
		CafeDenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, dcSateChange);
		return;
	}

	userId = GetUserIdFromIdx(userIdx);

	// Update last user processed
	if(DcInfo.zamUserIdx != UNKNOWN_USER_IDX)
	{
		DcInfo.lastUserIdx = DcInfo.zamUserIdx;
	}
	else if(DcInfo.userIdx != UNKNOWN_USER_IDX)
	{
		DcInfo.lastUserIdx = DcInfo.userIdx;
	}
	else if((DcInfo.userIdx == UNKNOWN_USER_IDX) && (DcInfo.firstUserIdx != UNKNOWN_USER_IDX))
	{
		DcInfo.lastUserIdx = DcInfo.firstUserIdx;
	}
	else if( (DcInfo.userIdx == UNKNOWN_USER_IDX) && (DcInfo.firstOccupantIdx != UNKNOWN_USER_IDX) )
	{
		DcInfo.lastUserIdx = DcInfo.firstOccupantIdx;
	}

	// check if second user and auth reason is not timeout. If so then it is required to
	// override auth reason.
	// NOTE: audio visual indications and event log will be based on new auth reason
	if( (DcInfo.firstUserIdx != INVALID_USER) && (DcInfo.authReason != AUTH_TIMEOUT) )
	{
		DcInfo.authReason = AUTH_TWO_PERSON;
		if(DcInfo.credType[DcInfo.credCnt-1] == FACE_CREDENTIAL)
		{
			tempAuthReason = DcInfo.authReason;
		}
	}

	// Store given index into doorActivityInfo which will be used for AV response
	DcInfo.userIdx = userIdx;

	if( (SysBasicPara.extWiegandRdrMode == WIGND_RDR_MODE_OUTPUT) && (SysBasicPara.wiegandOutFormat == WIGND_OUT_FRMT_CUSTOM) )
	{
		if(DcInfo.authReason == AUTH_TIMEOUT)
		{
			if(SysBasicPara.wgndOutCstmFrmtNo[DENY_CRED_TIMEOUT_EV] != 0)
				SendUserInfoOnExtWgndRdrPort(userIdx, DENY_CRED_TIMEOUT_EV, 0);
		}
		else if(DcInfo.authReason != AUTH_WIEGAND_WAIT_TIMEOUT)
		{
			if(DcInfo.credType[DcInfo.credCnt-1] == CARD_CREDENTIAL)
			{
				if(SysBasicPara.wgndOutCstmFrmtNo[DENY_INVALID_CARD_EV] != 0)
					SendUserInfoOnExtWgndRdrPort(userIdx, DENY_INVALID_CARD_EV, 0);
			}
			else if(DcInfo.credType[DcInfo.credCnt-1] == FP_CREDENTIAL)
			{
				if(SysBasicPara.wgndOutCstmFrmtNo[DENY_INVALID_BIOMETRIC_EV] != 0)
					SendUserInfoOnExtWgndRdrPort(userIdx, DENY_INVALID_BIOMETRIC_EV, 0);
			}
			else if(DcInfo.credType[DcInfo.credCnt-1] == PIN_CREDENTIAL)
			{
				if(SysBasicPara.wgndOutCstmFrmtNo[DENY_INVALID_PIN_EV] != 0)
					SendUserInfoOnExtWgndRdrPort(userIdx, DENY_INVALID_PIN_EV, 0);
			}
		}
	}
	if(DcInfo.authReason == AUTH_WIEGAND_WAIT_TIMEOUT)
	{
		DcInfo.authReason = AUTH_NO_ACCESS;
	}

	/* Check if this is the case of temperature threshold violation with hard restriction */
	if ((DcInfo.usrExceedTempThreshold == TRUE) &&
		((SysAdvPara.temperatureLogCnfg.restrictionType == USER_RESTRICTION_HARD)
		|| (DcInfo.qrAccessDecision == TEMP_ACCESS_DECISION_ACCESS_DENIED_TEMPERATURE_EXCEEDED)))
	{
		DcInfo.authReason = AUTH_TEMP_HARD_THRESHOLD_EXCEED;
	}

#if defined (FACE_MASK_COMPULSION_ENABLE)
	if (IsUserFaceMaskHardViolated(DcInfo.faceMaskAccessDecision))
	{
		DcInfo.authReason = AUTH_FACE_MASK_HARD_VIOLATION;
	}
#endif

	// TO DO:-- Take One Structure and from GetUserResp Function it will return with
	//			all field
	userResp = GetUserRespFromAuthReason(DcInfo.authReason);
	// Send AV response to door
	if((DcInfo.authReason != tempAuthReason) || (DcInfo.authReason == tempAuthReason && FRServerConfig.showUnknownUserAck == true))
	{
		SetAudioVisual(userResp, respPort, NULL);
	}
	evMode = GetEvMode(readerMode);
	evId = GetEventIdFromAuthReason(DcInfo.authReason);
	if(DcInfo.procSSAF == TRUE)
	{
		evField2 = SSA_USER;

		//If SI user deny in route access then evField2 auth reason
		if((evId == USER_DENIED_INVALID_ROUTE_ACCESS) || (evId == USER_DENIED_CONTROL_ZONE))
		{
			evField2 = GetEventField2FromAuthReason(DcInfo.authReason);
		}
	}
	else
	{
		evField2 = GetEventField2FromAuthReason(DcInfo.authReason);
	}
	if(DcInfo.firstUserIdx != INVALID_USER)
	{
		evField3 = (EV_FIELD3)(GetUserCredEvFieldInfo(FIRST_USER) + evMode);
		WriteEventLog(evId, GetUserId(DcInfo.firstUserIdx), evField2, evField3,
																	UNUSED_FIELD, UNUSED_FIELD);

		if (DcInfo.firstUsrTemperature != 0.0)
		{
			/* Generate user temperature event */
			GenerateUserTemperatureEvent(DcInfo.firstUserIdx, DcInfo.firstUsrTemperature, DcInfo.firstUsrQrTempUnit, DcInfo.firstUsrQrAccessDecision);
		}

		if( (SysAdvPara.dvrDenyEventUserType == ALL_USERS) || ((SysAdvPara.dvrDenyEventUserType == USERS_ONLY) && (DcInfo.userType != VISITOR_USER)) )
		{
			WriteDvrEvt(DVR_CONFIG_EVENT_USER_DENIED, DVR_SEND_EVENT_USER_DENIED, DcInfo.firstUserIdx, readerMode);
		}
	}

	if( (DcInfo.firstOccupantIdx != INVALID_USER) && (DcInfo.firstOccupantIdx != DcInfo.firstUserIdx))
	{
		evField3 = (EV_FIELD3)(GetUserCredEvFieldInfo(FIRST_OCCUPANT) + evMode);
		WriteEventLog(USER_DENIED_MINIMUM_OCCUPANCY, GetUserId(DcInfo.firstOccupantIdx), evField2, evField3,
																	UNUSED_FIELD, UNUSED_FIELD);

		if (DcInfo.firstUsrTemperature != 0.0)
		{
			/* Generate user temperature event */
			GenerateUserTemperatureEvent(DcInfo.firstOccupantIdx, DcInfo.firstUsrTemperature, DcInfo.firstUsrQrTempUnit, DcInfo.firstUsrQrAccessDecision);
		}

		if( (SysAdvPara.dvrDenyEventUserType == ALL_USERS) || ((SysAdvPara.dvrDenyEventUserType == USERS_ONLY) && (GetVisitorIdx(DcInfo.firstOccupantIdx) != INVALID_VISITOR_IDX)) )
		{
			WriteDvrEvt(DVR_CONFIG_EVENT_USER_DENIED, DVR_SEND_EVENT_USER_DENIED, DcInfo.firstOccupantIdx, readerMode);
		}
	}

	if( (userId != INVALID_USER_ID) || (DcInfo.authReason == tempAuthReason && FRServerConfig.generateUnidentifiedFaceEvent == true) || 
	    (DcInfo.authReason != AUTH_INVALID_FACE && SysAdvPara.genInvalidUserEvent == ENABLED) )
	{
		evField3 = (EV_FIELD3)(GetUserCredEvFieldInfo(CURRENT_USER) + evMode);
		WriteEventLog(evId, userId, evField2, evField3, UNUSED_FIELD, UNUSED_FIELD);

		/* Send user temperature event for user denied due to temperature threshold violation */
		if ((evId == USER_DENIED_TEMP_HARD_THRESHOLD_EXCEED) && (DcInfo.usrTemperature != 0.0))
		{
			/* Generate user temperature event */
			GenerateUserTemperatureEvent(userIdx, DcInfo.usrTemperature, DcInfo.qrTempUnit, DcInfo.qrAccessDecision);
		}

		if( (SysAdvPara.dvrDenyEventUserType == ALL_USERS) || ((SysAdvPara.dvrDenyEventUserType == USERS_ONLY) && (DcInfo.userType != VISITOR_USER)) )
		{
			if(userId != INVALID_USER_ID)
			{
				WriteDvrEvt(DVR_CONFIG_EVENT_USER_DENIED, DVR_SEND_EVENT_USER_DENIED, DcInfo.userIdx, readerMode);
			}
			else
			{
				WriteDvrEvt(DVR_CONFIG_EVENT_USER_NOT_IDENTIFY, DVR_SEND_EVENT_USER_NOT_IDENTIFY, INVALID_USER, readerMode);
			}
		}
	}
	if( (SysAdvPara.dvrDenyEventUserType == ALL_USERS) || ((SysAdvPara.dvrDenyEventUserType == USERS_ONLY) && (DcInfo.userType != VISITOR_USER)) )
	{
		// Update IO Link
		UpdateIoLinkInPortStatus(USER_DENIED_TRIGGER, ACTIVE);
	}

	// Deactivate single user special code in case it is active
	DcInfo.singleUserSplCode = USER_SPECIAL_CODE_NONE;

	if((DcInfo.apiUser == SET) || (DcInfo.cbReaderBleAccess == SET))
	{
		DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ACCESS_DENIED;
		SetQrCodeAuthReasonInDcInfo();
		if(DcInfo.curState == DC_GET_USER_STATUS_FROM_PANEL_STATE)
		{
			SeqInOutCredType = DcInfo.credType[((DcInfo.credCnt)-1)];
		}
	}

	if(DcInfo.authReason != AUTH_INVALID_FINGER_TEMP_USER)
	{
		if((DISABLED == BioIdentifyServer.enblBioServer))
		{
			CleanUpTempUserFP();
		}
	}

	if(dcSateChange == DC_STATE_SWITCH_TO_IDLE)
	{
		if((DcInfo.authReason != tempAuthReason) || (DcInfo.authReason == tempAuthReason && FRServerConfig.showUnknownUserAck == true))
		{
			if((DcInfo.DisplayFRStreamToIdleScreenTimer != INVALID_TIMER_HANDLE) && (DcInfo.credType[DcInfo.credCnt-1] == CARD_CREDENTIAL || DcInfo.credType[DcInfo.credCnt-1] == FP_CREDENTIAL))
			{
				HandleDisplayFrStreamToIdleScreenTimer();
			}
			SwitchDcToIdleState();
		}
		else
		{
			//This Scenario is For unidentified face feature where showunidentifiedface = false and generateunidentifiedevent = true
		    //so, Only unidentified event will generated so need to reset following parameter only for next unidentified event data.
			DcInfo.credCnt--;
			DcInfo.credType[DcInfo.credCnt] = INVALID_CREDENTIAL;
		}
	}
}

//*****************************************************************************
//	CafeDenyUser
//	Param:
//		IN :	userIdx	- user index for which allowed event is to be logged
//				raederMode	- entry or exit reader mode
//				respPort 	- internal or external response port
//		OUT:	none
//	Returns:	none
//	Description:
//			It logs user denied event as per authentication reason, checks
//			for the maximum attempts for ENTRY mode and also sets Av Response.
//
//	NOTE:		If user index is unknown pass it as 'UNKNOWN_USER_IDX'
//*****************************************************************************
void CafeDenyUser(UINT16 userIdx, READER_MODE_e readerMode, RESPONSE_PORT_e respPort, DC_STATE_CHANGE_e dcSateChange)
{
	EVENT_ID_e				evId;
	UINT32					userId;
	USER_EVENT_INFO_e		evMode;
	EV_FIELD2				evField2 = UNUSED_FIELD;
	EV_FIELD3				evField3;
	USER_RESP_e				userResp;

	userId = GetUserIdFromIdx(userIdx);
	userResp = GetUserRespFromAuthReason(DcInfo.authReason);
	// Send AV response to door
	SetAudioVisual(userResp, respPort, NULL);

	// Here For these reason event should not be generated so we are ignoring this part.
	if( (DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_WEEK_OFF) &&
			(DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_NON_WORKING_DAY) &&
			(DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_INVALID_SHIFT) &&
			(DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_DAILY_TRANSACTION) &&
			(DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_DAILY_MENU_LIMIT) &&
			(DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_DAILY_ITEM_LIMIT) &&
			(DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_MONTHLY_TRANSACTION) &&
			(DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_MONTHLY_MENU_LIMIT) &&
			(DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_MONTHLY_ITEM_LIMIT) &&
			(DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_PREORDER_LIMIT) &&
			(DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_WEEKLY_LIMIT) &&
			(DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_OFFLINE_ITEM_PER_MENU_LIMIT) &&
			(DcInfo.authReason != AUTH_CAFE_POLICY_DENIED_OFFLINE_TRANSACTION_NOT_ALLOWED) &&
			(DcInfo.authReason != AUTH_CAFE_USER_ID_MISMATCH) )
	{
		evMode = GetEvMode(readerMode);
		evId = GetEventIdFromAuthReason(DcInfo.authReason);

		if(DcInfo.authReason == AUTH_CAFE_TXN_REASON_INVALID_TRANSACTION)
		{
			evField2 = UNUSED_FIELD;
			evField3 = UNUSED_FIELD;
		}
		else
		{
			evField2 = E_CANTEEN_USER; //(special code)
			evField3 = (EV_FIELD3)(GetUserCredEvFieldInfo(CURRENT_USER) | evMode);
		}

		if( (userId != INVALID_USER_ID) || (DcInfo.authReason == AUTH_INVALID_FACE && FRServerConfig.generateUnidentifiedFaceEvent == true) || 
		    (DcInfo.authReason != AUTH_INVALID_FACE && SysAdvPara.genInvalidUserEvent == ENABLED) )
		{
			WriteEventLog(evId, userId, evField2, evField3,UNUSED_FIELD,UNUSED_FIELD);
		}
	}

	if( (SysAdvPara.dvrDenyEventUserType == ALL_USERS) || ((SysAdvPara.dvrDenyEventUserType == USERS_ONLY) && (DcInfo.userType != VISITOR_USER)) )
	{
		// Update IO Link
		UpdateIoLinkInPortStatus(USER_DENIED_TRIGGER, ACTIVE);
	}

	if(dcSateChange == DC_STATE_SWITCH_TO_IDLE)
	{
		// Switch to idle
		SwitchCafeMenuToIdle(CAFE_MENU_IDLE_DELAYED);
	}
}

//*****************************************************************************
// IsAuthAllowedReason()
// Param:       authReason
// Returns:	    YES/NO
// Description: It checks if given authentication reason is for allow.
//*****************************************************************************
BOOL IsAuthAllowedReason(AUTH_REASON_e authReason)
{
    BOOL status = NO;

    switch(authReason)
    {
        default:
            break;

        case AUTH_ALLOWED:
		case AUTH_ALLOWED_DURESS:
		case AUTH_PASSBACK_ENTRY_SOFT_VIOLATION:
		case AUTH_PASSBACK_EXIT_SOFT_VIOLATION:
		case AUTH_ALLOW_SSA_USER:
		case AUTH_ALLOW_USER_SOFT_ROUTE_ACCESS:
		case AUTH_ALLOW_2PERSON_PRIMARY:
		case AUTH_ALLOW_2PERSON_SECONDARY:
		case AUTH_VISITOR_ALLOWED_SURRENDER_PASS:
            status = YES;
            break;
    }

    return status;
}

//*****************************************************************************
//	InitCredInfo()
//	Param:
//		IN:	NONE
//		OUT:NONE
//	Returns:
//		NONE
//	Description:
//			It initializes credential related information.
//*****************************************************************************
VOID InitCredInfo(void)
{
	UINT8	idx;

	DcInfo.credCnt = 0;
	for(idx = 0; idx < MAX_CREDENTIAL_CNT; idx++)
	{
		DcInfo.credType[idx] = INVALID_CREDENTIAL;
	}
	DcInfo.credInvalidF = TRUE;
	DcInfo.apiUser = CLEAR;
	DcInfo.cbReaderBleAccess = CLEAR;
}

//*****************************************************************************
//	CheckSmartRouteFeature()
//	Param:
//		IN: NONE
//		OUT: None
//	Returns:
//		SUCCESS/FAIL
//	Description:
//			This function checks smart route feature related checks and allow
//		or deny user based on that.
//*****************************************************************************
BOOL CheckSmartRouteFeature(void)
{
	BOOL	retVal = PROCESS_FURTHER;

	// Check if route feature is  enabled on device, user is assigned any route and door is a member of any route
	// If all these condition doesn't match, check next policy.
	if(IsSmartRouteAccessEnabled() == TRUE)
	{
		// Check if Door is part of the User route, if not deny user
		if(SmartRouteAccessConfig[GetUserSmartRouteId(DcInfo.userIdx)].active == ACTIVE)
		{
			if(IsRouteLevelAllowedToUser() == YES)
			{
				if(SmartRouteAccessConfig[GetUserSmartRouteId(DcInfo.userIdx)].sequencedF == ENABLED)
				{
					// Check if user smart route level and provided credential are valid
					if(IsValidCredforSmartRoute() == YES)
					{
						DcInfo.smartRouteActiveF = TRUE;
						// If current credential is card then only process smart route access further
						if(DcInfo.credType[(DcInfo.credCnt) - 1] == CARD_CREDENTIAL)
						{
							if(SmartRouteAccessConfig[GetUserSmartRouteId(DcInfo.userIdx)].routeType == SMART_ROUTE_ACCESS_HARD)
							{
								if(DcInfo.readerMode == ENTRY_READER)
								{
									ProcessHardRouteOnEntry();
								}
								else
								{
									ProcessHardRouteOnExit();
								}
							}
							else
							{
								if(DcInfo.readerMode == ENTRY_READER)
								{
									ProcessSoftRouteOnEntry();
								}
								else
								{
									ProcessSoftRouteOnExit();
								}
							}
							retVal = PROCESS_DONE;
						}
					}
					else
					{
						DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
						retVal = PROCESS_DONE;
					}
				}
			}
			else
			{
				DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				retVal = PROCESS_DONE;
			}
		}
		else
		{
			DcInfo.authReason = AUTH_DOOR_NOT_IN_ROUTE;
			DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			retVal = PROCESS_DONE;
		}
	}

	return retVal;
}

//*****************************************************************************
//	CheckTwoPersonFeature()
//	Param:
//		IN: NONE
//		OUT: None
//	Returns:
//		SUCCESS/FAIL
//	Description:
//			This function checks two person feature related checks and allow
//		or deny user based on that.
//*****************************************************************************
BOOL CheckTwoPersonFeature(void)
{
	BOOL	retVal = PROCESS_FURTHER;

	// Check if 2 person rule is applicable
	if(IsTwoPersonApplicable() == YES)
	{
		// Check if first user
		if(DcInfo.firstUserIdx == INVALID_USER)
		{
			// check if first user a member of any 2 person group
			if(IsUserIn2PersonGrp(DcInfo.userIdx, &(DcInfo.firstUsrGrpDtl)) == NO)
			{
				// Deny since user not valid for 2-person rule
				DcInfo.authReason = AUTH_TWO_PERSON;
				DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else // Valid first user in 2-person user
			{
				/* Check additional Features */
				ProcAdditionalFeatureCheck(DC_WAIT_FOR_2ND_PERSON_STATE);
			}
		}
		else		//second user in 2-person
		{
			DeleteTimer(&(DcInfo.twoPersonTmrHandle));
			// check if user is valid as 2nd person.
			if(Is2PersonSatisfied(DcInfo.userIdx, DcInfo.firstUsrGrpDtl) == YES)
			{
				//open door to allow access to user
				CheckFinalPolicyAndAllow();
			}
			else // Not valid user of 2 person group
			{
				DcInfo.authReason = AUTH_TWO_PERSON;
				DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
		}
		retVal = PROCESS_DONE;
	}

	return retVal;
}


//*****************************************************************************
//	CheckFinalPolicyAndAllow()
//	Param:
//		IN:		doorId
//		OUT:NONE
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
void CheckFinalPolicyAndAllow(void)
{
	//breaks intentionally not put
	switch(	DcInfo.accessPolicyCheckState )
	{
		default:
			DcInfo.accessPolicyCheckState = OCCUPANCY_CNTRL_CHECK;
			//break not put intentionally - check occupancy if default
		case OCCUPANCY_CNTRL_CHECK:
			if(DcInfo.allowWithoutFinalCheck == FALSE)
			{
				if(CheckOccupancyControlViolation() == PROCESS_DONE)
				{
					return;
				}
			}
		case SPECIAL_FUNCTION_INPUT:
			if(IsSpFnInputReq() == YES)
			{
				DcInfo.accessPolicyCheckState = DOOR_MODE_INPUT; //access policy to be checked next
				SwitchDcToSpFnDisplayState();
				return;
			}
		case DOOR_MODE_INPUT:
			if(IsDoorModeSelectionRequired() == YES)
			{
				DcInfo.accessPolicyCheckState = JOB_COSTING_INPUT;//access policy to be checked next
				SwitchDcToDoorModeSelectionState();
				return;
			}
		case JOB_COSTING_INPUT:
			if(IsJobCodeInputReq() == YES)
			{
				DcInfo.accessPolicyCheckState = GET_USER_IN_OUT_STATUS_FROM_PANEL; //access policy to be checked next
				SwitchDcToJobDisplayState();
				return;
			}
			if(SysAdvPara.retainJobSelectionF == ENABLED)
			{
				if(IsJobAssignmentTypeStatic() == YES)
				{
					DcInfo.accessPolicyCheckState = GET_USER_IN_OUT_STATUS_FROM_PANEL; //access policy to be checked next

					if(IsRetainJobCodeAvailable() == YES)
					{
						DcInfo.selectedJobIdx = RetainJobCode;
						CheckFinalPolicyAndAllow();
						return;
					}
					else
					{
						DcInfo.selectedJobIdx = NONE_JOB_CODE_CONFIG_INDEX;
					}
				}
			}
		case GET_USER_IN_OUT_STATUS_FROM_PANEL:
			{
				if( (SysAdvPara.sequentialInOut == ENABLED) &&
						(DcInfo.userIdx < SSA_USER_IDX) &&
						(IsDoorModeSelectionRequired() == NO) &&
						(GetCurrSplCode() == USER_SPECIAL_CODE_NONE) &&
						(GetActiveScheduleSplFnCode(&CurrTimeDate) == USER_SPECIAL_CODE_NONE) &&
						(SysBasicPara.doorModeSchdlEnblF == DISABLED))
				{
					if((ConnectToPanelPara.connectToPanel == ENABLED) && (GetPanelStatus() == PANEL_COMM_ONLINE))
					{
						DcInfo.accessPolicyCheckState = POLICY_CHECK_SUCCESS; //access policy to be checked next
						SwitchDcToGetUserInOutState();
						return;
					}
					else
					{
						USER_IN_OUT_STATUS_e inOutStatus;
						inOutStatus = GetUserInOutStatus(DcInfo.userIdx);
						if(inOutStatus == USER_IS_IN)
						{
							SetUserInOutStatus(DcInfo.userIdx, USER_IS_OUT);
							DcInfo.doorEvtMode = USER_EXIT;
						}
						else
						{
							SetUserInOutStatus(DcInfo.userIdx, USER_IS_IN);
							DcInfo.doorEvtMode = USER_ENTRY;
						}
					}
				}
			}
		case POLICY_CHECK_SUCCESS:
		{
			/* Check additional Features */
			ProcAdditionalFeatureCheck(DC_IDLE_STATE);

			/* Update Dc policy check state */
			DcInfo.accessPolicyCheckState = MAX_POLICY_CHECK;
		}
		break;
	}

}

//*****************************************************************************
//	WaitOccupancyNextUserStateFn()
//	Param:
//		IN : doorId - Index of Door controller
//		OUT: None
//	Returns:
//		NONE
//	Description:
//		This is DC FSM state which waits for second valid user.
//
//*****************************************************************************
void WaitOccupancyNextUserStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	// Based on message ID, process it
	switch(msgId)
	{
		case MSG_RPL_ERL_CONFIRM_EVENT:
			{
				RPL_ERL_CONFIRM_EV_DATA_t* data = dataPtr;
				switch(data->adaptiveErlSts)
				{
				case ADAPTIVE_ERL_CONFIRM:
				{
					UINT16 userIdx;
					IsUserValid(data->userId, &userIdx);
					strncpy(AdaptiveFrQuery.usrId, GetAlphaNumId(userIdx), ALPHANUM_ID_SIZE);
					strncpy(AdaptiveFrQuery.usrName, GetUserName(userIdx), NAME_SIZE_UTF8);
					DcInfo.userIdx = userIdx;
					DcInfo.faceIdx = data->faceIdx;
					SetOccupancyNextUserWaitTimer(AdaptiveFrQuery.adaptiveFrTime + 1);
					StopFRStreamForRplErlConfirm();
					SendAdaptiveConfirmScreenToQt();
				}
					break;

				case ADAPTIVE_ERL_CONFIRM_RESP_FAIL:
					StopFRStreamForRplErlConfirm();
					DcInfo.authReason = AUTH_ADAPTIVE_ERL_REJECT;
					SetAudioVisual(USER_RESP_ACCESS_DENIED, INTERNAL_PORT, NULL);
					SwitchDcToIdleState();
					break;

				case ADAPTIVE_ERL_CONFIRM_RESP_TIMEOUT:
					StopFRStreamForRplErlConfirm();
					DcInfo.authReason = AUTH_ADAPTIVE_ERL_TIMEOUT;
					SetAudioVisual(USER_RESP_ACCESS_DENIED, INTERNAL_PORT, NULL);
					SwitchDcToIdleState();
					break;
				case ADAPTIVE_ERL_CONFIRM_YES:
					SetOccupancyNextUserWaitTimer(AdaptiveFrQuery.adaptiveFrTime + 1);
					break;
				case ADAPTIVE_ERL_CONFIRM_NO:
					SwitchDcToIdleState();
					DisplayIdleScreen(0);
					SetBuzLedResp(USER_RESP_IDLE,INTERNAL_PORT);
					break;

				case ADAPTIVE_ERL_CONFIRM_TIMEOUT:
					SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
					SwitchDcToIdleState();
					break;
				case ADAPTIVE_ERL_CONFIRM_RESP_SUCCESS:
				case MAX_ADAPTIVE_ERL_STATUS:
				default:
					break;
				}
			}break;
		case GUI_ALNUM_KEEP_ALIVE:
		case GUI_BUTTON_FIELD_CLEAR:
		{
			RESPONSE_PORT_e respPort = *(RESPONSE_PORT_e *)dataPtr;

			// Is user entering PIN then only reload timer
			if( (DcInfo.occupancyNextUsrWaitTimer != INVALID_TIMER_HANDLE)
				&& (DcInfo.respPort == respPort) )
			{
				SetOccupancyNextUserWaitTimer(SysAdvPara.interDigitTimer);

				if(respPort == INTERNAL_PORT)
				{
					// Reload display idle timer
					SetLcdIdleTimer(SysAdvPara.interDigitTimer);
				}
				else
				{
					SetReaderLcdIdleTimer(SysAdvPara.interDigitTimer);
				}
			}
			break;
		}

		case GUI_BUTTON_CANCEL:
		case GUI_BUTTON_HOME:
		{
			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;
			DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
			// User response must be set idle to overcome incomplete process' response
			if( (pinRespPort == INTERNAL_PORT)   || ((NO == CheckComboRdrPresent(COMBO_READER_ATOM_RD100)) &&(pinRespPort == EXTERNAL_PORT )))
			{
			    SetAudioVisual(USER_RESP_IDLE, pinRespPort, NULL);
			}
			SwitchDcToIdleState();
			if(pinRespPort == INTERNAL_PORT)
			{
				if(FRServerConfig.faceCapturing == FACE_SERVER_TAP_AND_GO)
				{
					StopFRStreamingOnDeviceIdleScreen(TRUE);
				}
				else
				{
					if(StartFRStreamingOnDeviceIdleScreen() == SUCCESS)
					{
						if(FRServerConfig.enableFreeScanTimeOutF == TRUE)
						{
							LoadDisplayFRStreamToIdleScreenTimer(FRServerConfig.faceScanTimeOut, FALSE);
						}
					}
				}
			}
			break;
		}
		
		case GUI_BUTTON_PIN_ENTER_SCREEN:
		{
			RESPONSE_PORT_e pinRespPort = *(RESPONSE_PORT_e *)dataPtr;

			if(PinEntryPossible(pinRespPort) == YES)
			{
				SetAudioVisual(USER_RESP_ENTER_PIN, pinRespPort, NULL);
				SetOccupancyNextUserWaitTimer(SysAdvPara.twoPersonTimer);

				if(pinRespPort == INTERNAL_PORT)
				{
					// Reload display idle timer
					SetLcdIdleTimer(SysAdvPara.twoPersonTimer);
					StopFRStreamingOnDeviceIdleScreen(FALSE);
				}
				else
				{
					SetReaderLcdIdleTimer(SysAdvPara.twoPersonTimer);
				}
			}
			break;
		}

		case MSG_PIN_DETECTED_ON_WGND_EVENT:
		{
			PIN_RESULT_e pinResp;

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetectOnWgnd((WGND_PIN_DATA_t *)dataPtr);
			if(pinResp == PIN_MATCH)
			{
				// delete timer used For two person
				DeleteTimer(&(DcInfo.occupancyNextUsrWaitTimer));
				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		// Verify the complete PIN entered by user
		case MSG_PIN_VALIDATION_EVENT:
		{
			PIN_RESULT_e pinResp;

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetect((PIN_DATA_t *)dataPtr);
			if(pinResp == PIN_MATCH)
			{
				// delete timer used For two person
				DeleteTimer(&(DcInfo.occupancyNextUsrWaitTimer));
				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case MSG_SCAN_TEMPLATE_RESP:
		case MSG_FP_DETECTED_EVENT:
		{
			FP_RESULT_e	fpResp;

			// Process Finger Print detected to find a match
			if(msgId == MSG_SCAN_TEMPLATE_RESP)
			{
				fpResp = ProcessFpScanTemplateResp(dataPtr);
			}
			else
			{
				fpResp = ProcessFpDetect((FP_DETECTED_EV_DATA_t *)dataPtr);
			}

			if(fpResp == FP_MATCH)
			{
				// delete timer used For two person
				DeleteTimer(&(DcInfo.occupancyNextUsrWaitTimer));

				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			// No match found or Timeout for identification
			else if( (fpResp == FP_INVALID) || (fpResp == FP_MATCHING_TIMEOUT) )
			{
				if(fpResp == FP_MATCHING_TIMEOUT)
				{
					DcInfo.authReason = AUTH_FP_MATCHING_TIMEOUT;
				}
				else
				{
					DcInfo.authReason = AUTH_INVALID_FINGER;
				}
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case MSG_FACE_DETECTED_EVENT:
		{
			//Occupancy Next User State Function
			FACE_RESULT_e	faceResp;

			DPRINT(DC_LOG, "Occupancy Next User Wait State");
			faceResp = ProcessFaceDetect((FACE_DETECTED_EV_DATA_t *)dataPtr);
			lastIdleTimeTick = GetSysTick();
			DPRINT(SYS_LOG, "Update lastIdleTimeTick in Occupancy [%u]", lastIdleTimeTick);
			if(faceResp == FACE_MATCH)
			{
				curProcFaceUserIdx = DcInfo.userIdx;
				if(IsFaceRecoEnbl(DcInfo.userIdx) == DISABLED)
				{
					DcInfo.authReason = AUTH_FR_DISABLED;
					DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
					break;
				}
				// delete timer used For two person
				DeleteTimer(&(DcInfo.occupancyNextUsrWaitTimer));

				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			// No match found or Timeout for identification
			else if(faceResp == FACE_INVALID)
			{
				if(FRServerConfig.showUnknownUserAck == true || FRServerConfig.generateUnidentifiedFaceEvent == true)
				{
					DcInfo.authReason = AUTH_INVALID_FACE;
					DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				}
			}
			else if(faceResp == FACE_MATCHING_TIMEOUT)
			{
				UINT8 idx;
				//OccupancyNextUserWaitTimeout
				if(SysAdvPara.occupancyCtrl.minUsers == 1)
				{
					DcInfo.authReason = AUTH_MIN_OCCUPANCY_IS_1;
				}

				if(SysAdvPara.occupancyCtrl.minUsers == 2)
				{
					DcInfo.authReason = AUTH_MIN_OCCUPANCY_IS_2;
				}

				DcInfo.userIdx = DcInfo.firstOccupantIdx;
				DcInfo.credCnt = DcInfo.firstOccupantCredCnt;

				for(idx = 0; idx < DcInfo.credCnt; idx++)
				{
					DcInfo.credType[idx] = DcInfo.firstOccupatcredType[idx];
				}
				DcInfo.firstOccupantIdx = INVALID_USER;

				DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		case GUI_BUTTON_FR_FOOTER_FOR_SPL_FUN:
		{
			if(StartFRStreamingOnDeviceIdleScreen() == SUCCESS)
			{
				StreamPayload_t         streamInfo;
				streamInfo.reason = START_STREAM_GUI_FOOTER_IDLE;
				streamInfo.captureMode = STREAM_CAPTURE_FACE;
				StartWaitForFrStreamTimer(streamInfo);
// 				LoadDisplayFRStreamToIdleScreenTimer(FRServerConfig.identificationTimeOut,TRUE);
// 				SetFRStreamOverlayInfo(FALSE, MSG_MAX_LANG_STRING, FALSE, FOOTER_HOME, TRUE);
// 				ShowFRStreamOnDeviceDisplayTimer(ACCESS_WAIT_RESP);
			}
			else
			{
				SwitchDcToIdleState();
			}
			break;
		}

		case MSG_CARD_DETECTED_EVENT:
		{
			CARD_RESULT_e	cardResp;
			CREDENTIAL_TYPE_e credType = CARD_CREDENTIAL;

			// Process card detected to find a match
			cardResp = ProcessCardDetect((CARD_DETECTED_EV_DATA_t *)dataPtr, credType);
			if(cardResp == USER_CARD_MATCH)
			{
				// delete timer used For two person
				DeleteTimer(&(DcInfo.occupancyNextUsrWaitTimer));

				// User is identified as Valid User.
				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
			}
			else if(cardResp == CARD_INVALID || cardResp == SP_FUNC_CARD_MATCH)  // No match found or Spl Card
			{
				DcInfo.authReason = AUTH_INVALID_CARD;
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}
		case MSG_BLE_QR_CODE_DETECTED:
		{
			QR_RESULT_e     qrResp;

			qrResp = ProcessQrCodeDetect((QR_CODE_DETECTED_EV_DATA_t *)dataPtr);
			if(qrResp == QR_CODE_USER_AUTHENTICATED)
			{
				DeleteTimer(&(DcInfo.occupancyNextUsrWaitTimer));
				ProcessUser(DcInfo.readerMode);
			}
			else if(qrResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(qrResp == QR_CODE_DEVICE_BUSY)
			{
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else if(qrResp == QR_CODE_INVALID_READER_MODE)
			{
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ACCESS_DENIED;
			}
			else
			{
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				DcInfo.qrAuthReason = QR_AUTH_UNAUTHORIZED_ACCESS;
			}
			break;
		}

		case MSG_BT_ACCESS_REQ_DETECTED:
		{
			DPRINT(DC_LOG, "MSG_BT_ACCESS_REQ_DETECTED");
			QR_RESULT_e	btResp;

			btResp = ProcessBtAccessReqDetect((BT_ACCESS_REQ_DETECTED_EV_DATA_t *)dataPtr);
			if(btResp == QR_CODE_USER_AUTHENTICATED)
			{
				/* Set flag to indicate request received from ble */
				DcInfo.bleAccessDevice = BLE_ACCESS_DEVICE_APTA;
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATED");
				ProcessUser(DcInfo.readerMode);
			}
			else if(btResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATION_FAILED");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(btResp == QR_CODE_DEVICE_BUSY)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_DEVICE_BUSY");
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else if(btResp == QR_CODE_INVALID_READER_MODE)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_INVALID_READER_MODE");
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_ACCESS_DENIED;
			}
			else if(btResp == QR_DEVICE_INVALID)
			{
				DPRINT(DC_LOG, "btResp == QR_DEVICE_INVALID ");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				// CRC is mismatched, Wrong Encryption.
				DcInfo.qrResponseMode = RESP_WRONG_ENC_CRC_MISMATCH;
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			}
			else
			{
				DPRINT(DC_LOG, "btResp == else(QR_CODE_API_DISABLED) ");
				DcInfo.qrResponseMode = RESP_USER_DENIED;
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			}
			
			if(DcInfo.curState != DC_GET_USER_STATUS_FROM_PANEL_STATE)
			{
				ACS_BLE_ACCESS_REQ_RESPONSE_t		respBtData;

				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = DcInfo.qrAuthReason;
				respBtData.respStatus = DcInfo.qrResponseMode;

				SendBLEEventTask(APTA_BLE_ACCESS_REQ_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			}
			break;
		}

		case MSG_BLE_ACCESS_EVENT:
		{
			QR_RESULT_e	btResp;
			CB_READER_ACCESS_REQ_RESPONSE_t		respBtData;

			DPRINT(DC_LOG, "MSG_BLE_ACCESS_EVENT");

			btResp = ProcessExtBleRdrBtAccessReqDetect((CB_READER_BLE_ACCESS_REQ_t *)dataPtr);
			if(btResp == QR_CODE_USER_AUTHENTICATED)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATED");
				ProcessUser(DcInfo.readerMode);
			}
			else if(btResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATION_FAILED");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(btResp == QR_CODE_DEVICE_BUSY)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_DEVICE_BUSY");
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else if(btResp == QR_DEVICE_INVALID)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_DEVICE_INVALID ");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				// CRC is mismatched, Wrong Encryption.
				DcInfo.qrResponseMode = CBR_RESP_WRONG_ENC_CRC_MISMATCH;
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			}
			else
			{
				DPRINT(DC_LOG, "btResp == else(QR_CODE_API_DISABLED) ");
				DcInfo.qrResponseMode = CBR_RESP_USER_DENIED;
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			}

			respBtData.readerId = DcInfo.readerId;
			respBtData.respReason = DcInfo.qrAuthReason;
			respBtData.respStatus = DcInfo.qrResponseMode;

			ProcessReaderCommand(MSG_CB_BLE_ACCESS_RESP_CMD, &respBtData, sizeof(CB_READER_ACCESS_REQ_RESPONSE_t));
		}
		break;

		case MSG_ACCESS_ID_REQ_EVENT:
		{
			QR_RESULT_e						btResp;
			ACS_BLE_ACCESS_REQ_RESPONSE_t		respBtData;

			DPRINT(DC_LOG, "MSG_MOBILE_ACCESS_EVENT");

			btResp = ProcessMobileBtAccessReqDetect((MOBILE_ACCESS_REQ_t *)dataPtr);
			if(btResp == QR_CODE_USER_AUTHENTICATED)
			{
				/* Set flag to indicate request received from ble */
				DcInfo.bleAccessDevice = BLE_ACCESS_DEVICE_ACS;
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATED");
				ProcessUser(DcInfo.readerMode);
			}
			else if(btResp == QR_CODE_USER_AUTHENTICATION_FAILED)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_CODE_USER_AUTHENTICATION_FAILED");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			else if(btResp == QR_CODE_DEVICE_BUSY)
			{
				DPRINT(DC_LOG, "btResp == QR_CODE_DEVICE_BUSY");
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
				DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_DEVICE_BUSY;
			}
			else if(btResp == QR_DEVICE_INVALID)
			{
				DontShowAttendanceDetail();
				DPRINT(DC_LOG, "btResp == QR_DEVICE_INVALID ");
				DcInfo.authReason = AUTH_API_INVALID_INPUT;
				DenyUser(INVALID_USER, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				// CRC is mismatched, Wrong Encryption.
				DcInfo.qrResponseMode = CBR_RESP_WRONG_ENC_CRC_MISMATCH;
				DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
			}
			else
			{
				DPRINT(DC_LOG, "btResp == else(QR_CODE_API_DISABLED) ");
				DcInfo.qrResponseMode = CBR_RESP_USER_DENIED;
				DcInfo.qrAuthReason = QR_AUTH_INVALID_INPUT;
			}

			if(DcInfo.curState != DC_GET_USER_STATUS_FROM_PANEL_STATE)
			{
				respBtData.readerId = DcInfo.readerId;
				respBtData.respReason = DcInfo.qrAuthReason;
				respBtData.respStatus = DcInfo.qrResponseMode;

				SendBLEEventTask(BLE_ACCESS_REQ_RESP, &respBtData, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			}
		}
		break;

		//Pin Detected from External Reader
		case MSG_PIN_DETECTED_ON_EXTERNAL_READER_EVENT:
		{
			PIN_RESULT_e pinResp;

			// Process key pressed to check for user's PIN
			pinResp = ProcessPinDetectOnExternalReader((READER_PIN_DATA_t *)dataPtr);
 			if(pinResp == PIN_MATCH)
 			{
 				// User is identified as Valid User.
 				// Now process user further based on entry/exit reader.
				ProcessUser(DcInfo.readerMode);
 			}
			else if(pinResp == PIN_INVALID)
			{
				DcInfo.authReason = AUTH_INVALID_PIN;
				DenyUser(UNKNOWN_USER_IDX, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
			}
			break;
		}

		default:
		{
			// Events that are not handled in this state are passed to default message handler .
			DcDefaultMsgHandler(msgId ,dataPtr, length);
			break;
		}
	}
}

//*****************************************************************************
//	SwitchDcToOccupancyWaitForUserState()
//	Param:
//		IN :	doorId
//		OUT:	None
//	Returns:	None
//	Description:
//				This function switches DCTask FSM to DC_WAIT_OCCUPANCY_NEXT_USER_STATE
//				and initializes required parameters.
//*****************************************************************************
void SwitchDcToOccupancyWaitForUserState(void)
{
	UINT8					idx;

	//Firs copy user detail in backup copy to allow user finally.........
	if(DcInfo.firstOccupantIdx == INVALID_USER)
	{
		DcInfo.firstOccupantIdx = DcInfo.userIdx;
		DcInfo.firstOccupantAuthReason = DcInfo.authReason;
		DcInfo.firstOccupantCredCnt = DcInfo.credCnt;

		for(idx = 0; idx < DcInfo.credCnt; idx++)
		{
			DcInfo.firstOccupatcredType[idx] = DcInfo.credType[idx];
			DcInfo.firstOccupatSubcredType[idx] = DcInfo.credSubType[idx];
		}

	}

	// Initialise all necessary members of door activity structure for next person
	InitCredInfo();
	DcInfo.qrAuthReason = QR_AUTH_ALLOWED;
	DcInfo.qrResponseMode = WEB_API_QR_CODE_RESP_WAITING_FOR_SECOND_USER;

	// Set current user index as invalid to process next user
	DcInfo.userIdx = INVALID_USER;
	DcInfo.zamUserIdx = INVALID_USER;

	// Start occupancy wait for next user timer
	if(SetOccupancyNextUserWaitTimer(SysAdvPara.twoPersonTimer) == FAIL)
	{
		EPRINT(DC_LOG,"Unable to start Occupancy Next User Wait Timer for Door");
	}

	IsFaceAlreadyIdentifyF = FALSE;
	CleanUpTempUserFP();
	DcInfo.curState = DC_WAIT_OCCUPANCY_NEXT_USER_STATE;
	DcInfo.usrExceedTempThreshold = FALSE;
	DcInfo.usrTemperature = 0.0;
#if defined(FACE_MASK_COMPULSION_ENABLE)
	DcInfo.isFaceMaskViolated = FALSE;
#endif
}

//*****************************************************************************
//	ProcessFpScanTemplateResp()
//	Param:
//		IN :	fpDataPtr	- points to finger event data
//		OUT:	none
//	Returns:	FP_RESULT_e
//	Description:
//				This function processes FP event to determine
//				if user index is available or not.
//*****************************************************************************
FP_RESULT_e ProcessFpScanTemplateResp(void * iDataPtr)
{
	FP_RESULT_e				retVal = FP_CONTINUE;
	RESPONSE_PORT_e			curRespPort;

	// Ignore FP request if entering PIN or wait timer is running
	if( (DcInfo.waitTmrHandle != INVALID_TIMER_HANDLE) ||
			(DcInfo.interDigitTmrHandle[INTERNAL_PORT] != INVALID_TIMER_HANDLE)  ||
			(DcInfo.interDigitTmrHandle[EXTERNAL_PORT] != INVALID_TIMER_HANDLE) )
	{
		return retVal;
	}

	if(NULL == iDataPtr)
		return FP_INVALID;
	
	IDT_SERVER_TEMPLATE_DATA_t tDataPtr;
	READER_ID_e tReaderId = ((SCAN_TEMPLATE_RESP_DATA_t *)iDataPtr)->readerId;
	memset(tDataPtr.template, 0, sizeof(tDataPtr.template));
	
	#ifdef LUMIDIGM_FP_READER_SUPPORT
	if(YES == IsProcessingLumidigmReader(tReaderId))
	{
		M300_SCAN_TEMPLATE_RESP_DATA_t * tTempRespData = (M300_SCAN_TEMPLATE_RESP_DATA_t *) iDataPtr;
		tDataPtr.readerId			= tTempRespData->readerId;
		tDataPtr.status				= tTempRespData->status;
		tDataPtr.imageQualityScore	= 0;
		tDataPtr.templateLen		= sizeof(tTempRespData->template);
		memcpy(tDataPtr.template, tTempRespData->template, sizeof(tTempRespData->template));
	}
	else
	#endif //#ifdef LUMIDIGM_FP_READER_SUPPORT
	{
		SCAN_TEMPLATE_RESP_DATA_t * tTempRespData = (SCAN_TEMPLATE_RESP_DATA_t *) iDataPtr;
		tDataPtr.readerId			= tTempRespData->readerId;
		tDataPtr.status				= tTempRespData->status;
		tDataPtr.imageQualityScore	= tTempRespData->imageQualityScore;
		tDataPtr.templateLen		= sizeof(tTempRespData->template);
		memcpy(tDataPtr.template, tTempRespData->template, sizeof(tTempRespData->template));
	}
	
	curRespPort = GetReaderRespPort(tDataPtr.readerId);

	// We ignore user input if response port is different for following conditions
	// 1. while waiting for user input for special function
	// 2. DC task state is other than IDLE state
	// 3. If entering FP
	if( (DcInfo.splFnGetUserInputTmrHandle != INVALID_TIMER_HANDLE) ||
		(DcInfo.curState != DC_IDLE_STATE) ||
		(DcInfo.fpBeingProcessed == TRUE) )
	{
		if(DcInfo.respPort != curRespPort)
		{
			return retVal;
		}
	}

    // Ignore input from Internal reader If Slide Screen is open
    if((CurrStatusbarData.statusBarExpandedF == TRUE) && (curRespPort == INTERNAL_PORT))
    {
        return retVal;
    }

	// Ignore input from External reader for cafeteria
	if( (SysBasicPara.application == CAFETERIA) && (curRespPort != INTERNAL_PORT) )
	{
		return retVal;
	}

	DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
	DeleteTimer(&DcInfo.fpMatchTmrHandle);
    SetLCDDimTimer();

	// Set DC task Activity parameters
	DcInfo.readerId = tDataPtr.readerId;
	UpdateDcInfoReaderMode(tDataPtr.readerId);
	DcInfo.respPort = curRespPort;

	if(tDataPtr.status == READER_RESP_STATUS_SCAN_SUCCESS)
	{
		DcInfo.fpBeingProcessed = TRUE;
		SetFpMatchTimer(BioIdentifyServer.identificationMatchTimer);
		// This is just to inform user that scan is successfull and user is
		// being identified by reader. We tell user to remove finger.
		SetAudioVisual(USER_RESP_REMOVE_FINGER, DcInfo.respPort, NULL);

		return retVal;
	}
	else if(tDataPtr.status == READER_RESP_STATUS_SUCCESS)
	{
		SetFpMatchTimer(BioIdentifyServer.identificationMatchTimer);
		//DcInfo.fpBeingProcessed = FALSE;
		// FP reader detected finger with some userID.
		// Lets check it exists in our database.
		if(BioIdentifyServer.enblBioServer == ENABLED)
		{
			DPRINT(DC_LOG, "Finger template ready to send to idt server");
			if(IsUserIdtRequestRequireForFp() == TRUE)
			{
				//USR_IDT Requset
				SendFpToIdentificationServer(&tDataPtr, BioIdentifyServer.ipAddr, BioIdentifyServer.port);
			}
			else
			{
				//USR_CHK Request
				SendFpUserCheckReqToIdentificationServer(&tDataPtr, BioIdentifyServer.ipAddr, BioIdentifyServer.port);
			}
		}
	}
	else
	{
		// Cannot detect finger or template not available in reader.
		DcInfo.fpBeingProcessed = FALSE;
		//TODO: ADD deny screen
	}

	return retVal;
}

//*****************************************************************************
//	SwitchDcToDoorModeSelectionState()
//	Param:
//		IN :	doorId
//		OUT:	None
//	Returns:	None
//	Description:
//				This function switches DCTask FSM to door mode selection state
//				and initializes required parameters.
//*****************************************************************************
static void SwitchDcToDoorModeSelectionState(void)
{
	// start timer for user input
	if(SetDoorModeWaitTimer(SysAdvPara.multiAccessTimer) == FAIL)
	{
		EPRINT(DC_LOG,"Unable to start door mode Wait Timer for Door");
	}
	DcInfo.doorEvtMode = MAX_USER_EVENT_INFO;
	DisplayDoorModeSelectionList();
	CleanUpTempUserFP();
	DcInfo.curState = DC_DOOR_MODE_SELECTION_STATE;
}

//*****************************************************************************
//	DisplayDoorModeSelectionList()
//	Param:
//		IN :	doorId
//		OUT:	None
//	Returns:	None
//	Description:
//				This function displays list of door modes to choose from.
//*****************************************************************************
static void DisplayDoorModeSelectionList(void)
{
	SendMessageToQt("", GUI_MSG_DOOR_MODE_SELECTION_SCREEN);
	SetLcdIdleTimer(SysAdvPara.multiAccessTimer);
}
//*****************************************************************************
//	SetDoorModeWaitTimer()
//	Param:
//		IN :	timer value
//		OUT: 	NONE
//	Returns: SUCCESS/FAIL
//	Description: shows door mode selection list
//
//*****************************************************************************
static BOOL SetDoorModeWaitTimer(UINT8 timer)
{
	if(DcInfo.doorModeInputWaitTimer == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;
		tInfo.count = (timer * 10);
		tInfo.data = 0;
		tInfo.funcPtr = &DoorModeWaitTimeout;
		return (StartTimer(tInfo, &DcInfo.doorModeInputWaitTimer, __FUNCTION__) );
	}
	else //reload timer
	{
		return (ReloadTimer(DcInfo.doorModeInputWaitTimer, (timer * 10)) );
	}
}
//*****************************************************************************
//	DoorModeWaitTimeout
//	Param:
//		IN :	dummy
//		OUT:	none
//	Returns:	none
//	Description:
//				It switches DC to idle state and sends audio visual response to
//				DC for timeout. If special code for single user is active,
//				it deactivates special code.
//
//*****************************************************************************
static void DoorModeWaitTimeout(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&DcInfo.doorModeInputWaitTimer);
	DcInfo.authReason = AUTH_TIMEOUT;
	DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
}

//*****************************************************************************
//	DcDoorModeSelectionStateFn()
//	Param:
//		IN : msgId - ID of the received message
//			 dataPtr - Pointer to the data
//			 length - length of the data
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//			This Function will be called by the dc task FSM.
//
//*****************************************************************************
void DcDoorModeSelectionStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	switch(msgId)
	{
		case GUI_BUTTON_DOOR_MODE_IN:
		{
			DcInfo.doorEvtMode = USER_ENTRY;
			DeleteTimer(&DcInfo.doorModeInputWaitTimer);
			if(DcInfo.SSAPolicyCheckState == MAX_POLICY_CHECK) //non-SI user
			{
				CheckFinalPolicyAndAllow();
			}
			else
			{
				CheckFinalSSAPolicyAndAllow();
			}
			break;
		}
		case GUI_BUTTON_DOOR_MODE_OUT:
		{
			DcInfo.doorEvtMode = USER_EXIT;
			DeleteTimer(&DcInfo.doorModeInputWaitTimer);
			if(DcInfo.SSAPolicyCheckState == MAX_POLICY_CHECK)
			{
				CheckFinalPolicyAndAllow();
			}
			else
			{
				CheckFinalSSAPolicyAndAllow();
			}
			break;
		}

		default:
		{
			DcDefaultMsgHandler(msgId, dataPtr, length);
			break;
		}
	}
}

//*****************************************************************************
//	IsSpFnInputReq()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			 TRUE / FALSE
//	Description:
//			This Function will check if Special Function selection required
//
//*****************************************************************************
static BOOL	IsSpFnInputReq(void)
{
	if( (SysBasicPara.promptSpFnF == TRUE) && (SysBasicPara.doorModeSelectEnblF == TRUE) && (GetCurrSplCode() == USER_SPECIAL_CODE_NONE) &&
			(DcInfo.respPort == INTERNAL_PORT) && (DcInfo.apiUser == CLEAR) && (DcInfo.cbReaderBleAccess == CLEAR))
	{
		return TRUE;
	}
	return FALSE;
}

//*****************************************************************************
//	IsDoorModeSelectionRequired()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			 TRUE / FALSE
//	Description:
//			This Function will check if door mode selection enabled and required
//
//*****************************************************************************
BOOL IsDoorModeSelectionRequired(void)
{
	if( (SysBasicPara.doorModeSelectEnblF == TRUE) && (GetCurrSplCode() == USER_SPECIAL_CODE_NONE) &&
			(DcInfo.respPort == INTERNAL_PORT) && (DcInfo.apiUser == CLEAR) && (DcInfo.cbReaderBleAccess == CLEAR))
	{
		return TRUE;
	}
	return FALSE;
}

//*****************************************************************************
//	CheckFinalSSAPolicyAndAllow()
//	Param:
//		IN:	None
//		OUT:NONE
//	Returns:NONE
//	Description:checks remaining policies on SI user before allowing
//
//*****************************************************************************
void CheckFinalSSAPolicyAndAllow()
{
	switch(DcInfo.SSAPolicyCheckState)
	{
		default:
			DcInfo.SSAPolicyCheckState = SPECIAL_FUNCTION_INPUT;
			//break not put intentionally - check occupancy if default
		case SPECIAL_FUNCTION_INPUT:
		{
			DPRINT(DC_LOG,"Door special function check");
			DeleteTimer(&DcInfo.multiAccessTmrHandle); //multi access timer has to be deleted in case of SSA user
			if(IsSpFnInputReq() == YES)
			{
				DcInfo.SSAPolicyCheckState = DOOR_MODE_INPUT; //access policy to be checked next
				SwitchDcToSpFnDisplayState();
				return;
			}
		}
		case DOOR_MODE_INPUT:
		{
			DPRINT(DC_LOG,"Door mode input check");

			DeleteTimer(&DcInfo.multiAccessTmrHandle); //multi access timer has to be deleted in case of SSA user
			if(IsDoorModeSelectionRequired() == YES)
			{
				DcInfo.SSAPolicyCheckState = POLICY_CHECK_SUCCESS;//access policy to be checked next
				SwitchDcToDoorModeSelectionState();
				return;
			}
		}
		case POLICY_CHECK_SUCCESS:// other policies need not to be checked for SI users so allow directly
		{
			/* Check additional Features */
			ProcAdditionalFeatureCheck(DC_IDLE_STATE);

			DcInfo.SSAPolicyCheckState = MAX_POLICY_CHECK;
			break;
		}
	}
}

//*****************************************************************************
//	LoadShowCardForKeyChngTimer()
//	Param:
//		IN :	timeValue -- delay in second
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function starts/reloads lcdIdleTmrHandle to show 'show card'
//		audio visual response.
//*****************************************************************************
static BOOL LoadKeyChangeWaitTimer(UINT16	timerValue)
{
	TIMER_INFO_t	   tInfo;

	if(SmartCardKeyUpdateDetail.keyChngWaitTmrHandle == INVALID_TIMER_HANDLE)
	{
		tInfo.funcPtr = &KeyChangeWaitTimeout;
		tInfo.count = (timerValue*10*TIMER_100MS);
		tInfo.data = 0;

		return (StartTimer(tInfo, &SmartCardKeyUpdateDetail.keyChngWaitTmrHandle, __FUNCTION__) );
	}
	else
	{
		return (ReloadTimer(SmartCardKeyUpdateDetail.keyChngWaitTmrHandle, timerValue) );
	}
}

//*****************************************************************************
//	KeyChangeWaitTimeout()
//	Param:
//		IN :	timeValue -- delay in second
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function handle time out in key change state
//*****************************************************************************
static void KeyChangeWaitTimeout(UNUSED_PARA UINT32 data)
{
	DPRINT(DC_LOG,"KEY CHANGE REQUEST TIMED OUT");
	SmartCardKeyUpdateDetail.keyChngResp = WEB_API_RESP_TIME_OUT;
	SetAudioVisual(USER_RESP_KEY_CHANGE_FAIL, INTERNAL_PORT, NULL);
	SmartCardKeyUpdateDetail.smartCardKeyChangeF = FALSE;
	SwitchDcToIdleState();
}

//*****************************************************************************
//	ProcessPinDetectOnWgnd()
//	Param:
//		IN :	pin	- pin entred by user
//		OUT:    NONE
//	Returns:	PIN_RESULT_e
//	Description:
//				This function processes if valid PIN is entered by the user.
//				It checks if it is valid PIN for any user.
//*****************************************************************************
PIN_RESULT_e ProcessPinDetectOnWgnd(WGND_PIN_DATA_t *pin)
{
	UINT16					userIdx;
	PIN_RESULT_e			retValue = PIN_CONTINUE;

	// We ignore user input if response port is different for following conditions
	// 1. while waiting for user input for special function
	// 2. DC task state is other than IDLE state
	if( (DcInfo.splFnGetUserInputTmrHandle != INVALID_TIMER_HANDLE) ||
		(DcInfo.curState != DC_IDLE_STATE) )
	{
		if(DcInfo.respPort != EXTERNAL_PORT)
		{
			printf("\nDcInfo.respPort != EXTERNAL_PORT\n");
			return PIN_CONTINUE;
		}
	}

	// Ignore input from External reader for cafeteria
	if(SysBasicPara.application == CAFETERIA)
	{
		return PIN_CONTINUE;
	}

	if( (DcInfo.keyCnt == 0) && (PinEntryPossible(EXTERNAL_PORT) == NO) )
	{
		return PIN_CONTINUE;
	}

	if( (DcInfo.keyCnt == 0) && ( (pin->rcvdDataBitLen == 16) || (pin->rcvdDataBitLen == 4) ) )
	{
		DcInfo.readerId = READER3;
		UpdateDcInfoReaderMode(READER3);
		DcInfo.respPort = EXTERNAL_PORT;
		DeleteFsmTimers();
		// DC task is busy now
		DcInfo.dcTaskIdleF = FALSE;
	}

	if(pin->rcvdDataBitLen == 16)
	{
		DcInfo.keyCnt = 0;
		sprintf(DcInfo.keyBuf, "%u", pin->rcvdData);
	}
	else if(pin->rcvdDataBitLen == 4)
	{
		if(pin->rcvdData == 0x0B)
		{
			DcInfo.keyBuf[DcInfo.keyCnt] = '\0';
			DcInfo.keyCnt = 0;
		}
		else if(DcInfo.keyCnt < 6)
		{
			if(pin->rcvdData < 0x0A)
			{
				DcInfo.keyBuf[DcInfo.keyCnt++] = pin->rcvdData + '0';
				DcInfo.readerId = READER3;
				UpdateDcInfoReaderMode(READER3);
				DcInfo.respPort = EXTERNAL_PORT;
				DeleteFsmTimers();
				// DC task is busy now
				DcInfo.dcTaskIdleF = FALSE;
			}
			else if(pin->rcvdData == 0x0A)
			{
				DcInfo.keyBuf[DcInfo.keyCnt++] = '*';
			}
			switch(DcInfo.curState)
			{
				case DC_IDLE_STATE:
					LoadInterDigitTimerForExternal();
					break;

				case DC_WAIT_FOR_2ND_PERSON_STATE:
					SetTwoPersonTimer(SysAdvPara.interDigitTimer);
					break;

				case DC_WAIT_ADDL_CRED_STATE:
					LoadMultiAccessTimer(SysAdvPara.multiAccessTimer);
					break;

				case DC_WAIT_OCCUPANCY_NEXT_USER_STATE:
					SetOccupancyNextUserWaitTimer(SysAdvPara.interDigitTimer);
					break;

				case DC_PIN_CHANGE_STATE:
					SetTimerMultiInputWaitTimer(SysAdvPara.interDigitTimer);
					break;

				default:
					break;
			}
			return PIN_CONTINUE;
		}
		else
		{
			DcInfo.keyCnt = 0;
			memset(DcInfo.keyBuf, CLEAR, MAX_PIN_SIZE);
		}
	}
	else
	{
		DcInfo.keyCnt = 0;
		memset(DcInfo.keyBuf, CLEAR, MAX_PIN_SIZE);
	}

	switch(DcInfo.curState)
	{
		case DC_IDLE_STATE:
			DeleteTimer(&(DcInfo.interDigitTmrHandle[EXTERNAL_PORT]));
			break;

		case DC_WAIT_FOR_2ND_PERSON_STATE:
			DeleteTimer(&(DcInfo.twoPersonTmrHandle));
			break;

		case DC_WAIT_ADDL_CRED_STATE:
			DeleteTimer(&(DcInfo.multiAccessTmrHandle));
			break;

		case DC_WAIT_OCCUPANCY_NEXT_USER_STATE:
			DeleteTimer(&(DcInfo.occupancyNextUsrWaitTimer));
			break;

		case DC_PIN_CHANGE_STATE:
			DeleteTimer(&(DcInfo.multiInputWaitTmrHandle));
			break;

		default:
			break;
	}

	// Store rcvd credential
	StoreRcvdCred(PIN_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
	if(DcInfo.procSSAF == TRUE)
	{
		if( (strlen((CHARPTR)(DcInfo.keyBuf)) > 0) &&
			(strcmp((CHARPTR)(DcInfo.keyBuf), GetUserPinCode(DcInfo.userIdx)) == 0) )
		{
			retValue = PIN_MATCH;
		}
		else
		{
			retValue = PIN_INVALID;
		}
	}
	else if(IsPinValid((CHARPTR)(DcInfo.keyBuf) , &userIdx) == YES)
	{
		// Store identified user
		DcInfo.userIdx = userIdx;
		retValue = PIN_MATCH;
	}
	else if( (strlen( (CHARPTR)(DcInfo.keyBuf) ) > 0) &&
			 (strcmp( (CHARPTR)(DcInfo.keyBuf), SysBasicPara.pinChangeCode) == 0) &&
			 ( (DcInfo.curState == DC_IDLE_STATE) ||(DcInfo.curState == DC_PIN_CHANGE_STATE) ) )
	{
		retValue =  PIN_CHANGE_CODE_MATCH;
	}
	else
	{
		retValue =  PIN_INVALID;
	}

	return retValue;
}

//*****************************************************************************
//	DcWaitForAddlCredStateFn()
//	Param:
//		IN :	msgId - Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length - Length of the message
//		OUT:	None
//	Returns:
//		NONE
//	Description:
//		This is DC FSM state which waits for user to present all credentials
//		required as per access mode policy.
//*****************************************************************************
void DcVerifyFingerApiWaitStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	switch(msgId)
	{
		case MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_RESP:
		{
			READER_RESP_STATUS_e 	status;
			EV_FIELD3				evField3;

			status = ( (VERIFY_HOST_TEMPLATE_BY_SCAN_RESP_DATA_t *) dataPtr) -> status;
			DPRINT(DC_LOG, "rcvd MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_RESP, status = %s", ReaderRespStatusStr[status]);
			DcInfo.verifyingFpByScanF = CLEAR;	// fp scanning is done
			VerifyHostTemplateByScanStatus = status;

			if(status == READER_RESP_STATUS_SUCCESS)
			{
				WaitngForReaderRespFlag = FALSE;
				StoreRcvdCred(BLE_QR_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
				// Store credential
				StoreRcvdCred(FP_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
				// Process user for ZAM
				if(VerifyFingerApiOperation == OPERATION_VERIFY_ONLY)
				{
					SetAudioVisual(USER_RESP_SUCCESS_SCREEN, DcInfo.respPort, NULL);
				}
				else
				{
					DcInfo.userIdDoorOpenCloseEvent = INVALID_USER_ID;
					strncpy(DcInfo.alphanumIddoorOpenCloseEvent, GetAlphaNumId(SSA_USER_IDX), sizeof(DcInfo.alphanumIddoorOpenCloseEvent));

					OpenDoor();
					evField3 = (EV_FIELD3)(GetUserCredEvFieldInfo(CURRENT_USER) + SysBasicPara.reader[READER2].mode);
					WriteEventLog(USER_ALLOW, INVALID_USER_ID, USER_SPECIAL_CODE_NONE, evField3, UNUSED_FIELD, UNUSED_FIELD);
					SetAudioVisual(USER_RESP_ACCESS_ALLOWED, INTERNAL_PORT, NULL);
				}

				SwitchDcToIdleState();
			}
			// if finger scan is succesful tell user to remove his finger
			else if(status == READER_RESP_STATUS_SCAN_SUCCESS)
			{
				SetAudioVisual(USER_RESP_REMOVE_FINGER, DcInfo.respPort, NULL);
			}
			// ignore timeout response of reader
			else if(status != READER_RESP_STATUS_TIME_OUT)
			{
				USER_RESP_e			userResp;
				USER_EVENT_INFO_e	evMode;

				WaitngForReaderRespFlag = FALSE;
				DcInfo.authReason = AUTH_INVALID_FINGER;
				//			all field
				userResp = GetUserRespFromAuthReason(DcInfo.authReason);

				StoreRcvdCred(BLE_QR_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
				// Store credential
				StoreRcvdCred(FP_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
				if(VerifyFingerApiOperation == OPERATION_VERIFY_ONLY)
				{
					// Send AV response to door
					SetAudioVisual(userResp, DcInfo.respPort, NULL);
				}
				else
				{
					// Send AV response to door
					SetAudioVisual(userResp, DcInfo.respPort, NULL);
					evMode = GetEvMode(DcInfo.readerMode);
					evField3 = (EV_FIELD3)(GetUserCredEvFieldInfo(CURRENT_USER) + evMode);
					WriteEventLog(USER_DENIED_INVALID, UNUSED_FIELD, UNUSED_FIELD, evField3, UNUSED_FIELD, UNUSED_FIELD);
				}
				SwitchDcToIdleState();
			}
		}break;

		default:
		{
			// Events that are not handled in this state are passed to default message handler .
			DcDefaultMsgHandler(msgId, dataPtr, length);
			break;
		}
	}
}

//*****************************************************************************
//	DcInstallationStateFn()
//	Param:
//		IN :	msgId - Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length - Length of the message
//		OUT:	None
//	Returns:
//		NONE
//	Description:
//		This is DC FSM state in which installation of Door Controller will be processed
//*****************************************************************************
void DcInstallationStateFn(SLAVE_MSG_ID_e msgId, UNUSED_PARA VOIDPTR dataPtr, UNUSED_PARA UINT16 length)
{
	switch(msgId)
	{
		case GUI_ALNUM_KEEP_ALIVE:
		case GUI_BUTTON_FIELD_CLEAR:
			// Is user entering IP Address then only reload timer
			if(DcInfo.installationwaitTimerhandle != INVALID_TIMER_HANDLE)
			{
				//reload installation wait timer
				InstallationProcessWaitTimer(INSTALLATION_PROCESS_WAIT_TIMER);
			}
			break;

		case GUI_BUTTON_CANCEL_INSTALLATION_PROCESS:
		{
			DeleteTimer(&DcInfo.installationwaitTimerhandle);
			SwitchDcToIdleState();
			SendMessageToQt("", GUI_MSG_DISPLAY_IDLE_SCREEN);
		}break;

		case GUI_BUTTON_OK_ASSIGN_STATIC_DEVICE_IP:
		case GUI_BUTTON_SKIP_DYNAMIC_IP_PROCESS:
		{
			struct stat signFileInfo;

			if(stat(SERVER_IP_SIGN_FILE,&signFileInfo) == 0)
			{
				SendMessageToQt("", GUI_MSG_INSTALLATION_SCREEN_SERVER_IP);
				//reload installation wait timer
				InstallationProcessWaitTimer(INSTALLATION_PROCESS_WAIT_TIMER);
			}
			else
			{
				DeleteTimer(&DcInfo.installationwaitTimerhandle);
				SwitchDcToIdleState();
				SendMessageToQt("", GUI_MSG_DISPLAY_IDLE_SCREEN);
			}
		}break;

		case GUI_BUTTON_OK_ASSIGN_SERVER_URL:
		{
			DeleteTimer(&DcInfo.installationwaitTimerhandle);
			SwitchDcToIdleState();
			SendMessageToQt("", GUI_MSG_DISPLAY_IDLE_SCREEN);
		}break;

		case GUI_BUTTON_OK_ASSIGN_TENANT_ID:
		{
			DeleteTimer(&DcInfo.installationwaitTimerhandle);
			SwitchDcToIdleState();
			SendMessageToQt("", GUI_MSG_DISPLAY_IDLE_SCREEN);
		}break;

		case GUI_BUTTON_SWITCH_TO_VYOM:
		{
			ServerModeCfg.serverMode = COSEC_VYOM;
			if(WriteServerModeConfig() == SUCCESS)
			{
				if( LoadDfltServerParaConfig() == SUCCESS)
				{
					ChangeNetworkInterfacePara(SERVER_PARA);
					SendMessageToQt("", GUI_MSG_INSTALLATION_SCREEN_VYOM_TENANT_ID);
					//reload installation wait timer
					InstallationProcessWaitTimer(INSTALLATION_PROCESS_WAIT_TIMER);
				}
				else
				{
					EPRINT(SYS_LOG, "Installation State : Server Parameter Vyom Config set failed");
				}
			}
		}break;

		case GUI_BUTTON_SWITCH_TO_CENTRA:
		{
			DPRINT(SYS_LOG,"case : GUI_BUTTON_SWITCH_TO_CENTRA......");
			ServerModeCfg.serverMode = COSEC_CENTRA;
			if(WriteServerModeConfig() == SUCCESS)
			{
				if( LoadDfltServerParaConfig() == SUCCESS)
				{
					ChangeNetworkInterfacePara(SERVER_PARA);
					SendMessageToQt("", GUI_MSG_INSTALLATION_SCREEN_SERVER_IP);
					//reload installation wait timer
					InstallationProcessWaitTimer(INSTALLATION_PROCESS_WAIT_TIMER);
				}
				else
				{
					EPRINT(SYS_LOG, "Installation State : Server Parameter Centra Config set failed");
				}
			}
		}break;

		default:
        // Events that are not handled in this state are passed to default message handler .
        DcDefaultMsgHandler(msgId, dataPtr, length);
			break;
	}
}
//*****************************************************************************
//	SwitchDcToInstallationState()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:
//		NONE
//	Description:
//		This function will switch Door Controller to installation state
//*****************************************************************************
void SwitchDcToInstallationState(void)
{
	struct stat signFileInfo;

	CleanUpTempUserFP();
	if(stat(DEVICE_IP_SIGN_FILE,&signFileInfo) == 0)
	{
		DcInfo.curState = DC_INSTALLATION_STATE;
		SendMessageToQt("",GUI_MSG_INSTALLATION_SCREEN_DEVICE_IP);
		DeleteTimer(&DcInfo.lcdIdleTmrHandle);

		// It will start 30 Seconds timer when installation screen 1(Device IP assignment) is loaded successfully
		InstallationProcessWaitTimer(INSTALLATION_PROCESS_WAIT_TIMER);
	}
	else if(stat(SERVER_IP_SIGN_FILE,&signFileInfo) == 0)
	{
		DcInfo.curState = DC_INSTALLATION_STATE;
		SendMessageToQt("",GUI_MSG_INSTALLATION_SCREEN_SERVER_IP);
		DeleteTimer(&DcInfo.lcdIdleTmrHandle);

		// It will start 30 Seconds timer when installation screen 1(Device IP assignment) is loaded successfully
		InstallationProcessWaitTimer(INSTALLATION_PROCESS_WAIT_TIMER);
	}
	else
	{
		EPRINT(DC_LOG,"Installation Signature is not present");
	}
}

//*****************************************************************************
//	InstallationProcessWaitTimer()
//	Param:
//		IN :	Time in Seconds
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//		NONE
//	Description:
//			This function loads timer to put lcd into DC idle state after
//			given time.
//			If display is updated before timeout then timer will be
//			reloaded with new value.
//*****************************************************************************
BOOL InstallationProcessWaitTimer(UINT16 timerValue)
{
	TIMER_INFO_t tInfo;

	if(timerValue > 0)
	{
		if(DcInfo.installationwaitTimerhandle == INVALID_TIMER_HANDLE)
		{
			tInfo.count = (timerValue * 10);
			tInfo.data  = 0;
			tInfo.funcPtr = &InstallationProcessTimerTimeOut;

			return(StartTimer(tInfo, &DcInfo.installationwaitTimerhandle, __FUNCTION__) );
		}
		else
		{
			return (ReloadTimer(DcInfo.installationwaitTimerhandle, (timerValue * 10)));
		}
	}
	else
	{
		DeleteTimer(&DcInfo.installationwaitTimerhandle);
		return SUCCESS;
	}
}

void InstallationProcessTimerTimeOut(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&DcInfo.installationwaitTimerhandle);
	SwitchDcToIdleState();
	SendMessageToQt("", GUI_MSG_DISPLAY_IDLE_SCREEN);
}

//*****************************************************************************
//	StartSystemTestScreenWaitTimer()
//	Param:
//		IN :	Time in Seconds
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//		NONE
//	Description:
//			This function loads timer to put lcd into DC idle state after
//			given time.
//*****************************************************************************
BOOL StartSystemTestScreenWaitTimer(UINT16 timerValue)
{
	TIMER_INFO_t tInfo;

	if(DcInfo.sysTestScreenWaitTimerhandle == INVALID_TIMER_HANDLE)
	{
		tInfo.count = (timerValue * 10);
		tInfo.data  = 0;
		tInfo.funcPtr = &StartSystemTestScreenTimerTimeOut;

		return(StartTimer(tInfo, &DcInfo.sysTestScreenWaitTimerhandle, __FUNCTION__) );
	}
	else
	{
		return (ReloadTimer(DcInfo.sysTestScreenWaitTimerhandle, (timerValue * 10)));
	}
}

//*****************************************************************************
//	StartSystemTestScreenTimerTimeOut()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	VOID
//		NONE
//	Description:
//			This function will delete Start System Test Screen Wait Timer
//*****************************************************************************
VOID StartSystemTestScreenTimerTimeOut(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&DcInfo.sysTestScreenWaitTimerhandle);
	SetAudioVisual(USER_RESP_TIMEOUT, INTERNAL_PORT, NULL);
    IsESCButtonPressedF = TRUE;
	SwitchDcToIdleState();
}

//*****************************************************************************
//	ProcessPinDetectOnExternalReader()
//	Param:
//		IN :	pin	- pin entred by user
//		OUT:    NONE
//	Returns:	PIN_RESULT_e
//	Description:
//				This function processes if valid PIN is entered by the user.
//				It checks if it is valid PIN for any user.
//*****************************************************************************
PIN_RESULT_e ProcessPinDetectOnExternalReader(READER_PIN_DATA_t *pin)
{
	UINT16					userIdx;
	PIN_RESULT_e			retValue = PIN_CONTINUE;

	// Ignore PIN request if entering PIN on internal port or wait timer is running
	if( (DcInfo.waitTmrHandle != INVALID_TIMER_HANDLE) ||
		(DcInfo.interDigitTmrHandle[INTERNAL_PORT] != INVALID_TIMER_HANDLE) )
	{
		EPRINT(DC_LOG, "Ignoring PIN request as PIN entry is going on at internal port or wait timer is running");
		return retValue;
	}

	// We ignore user input if response port is different for following conditions
	// 1. while waiting for user input for special function
	// 2. DC task state is other than IDLE state
	// 3. If entering FP
	if( (DcInfo.splFnGetUserInputTmrHandle != INVALID_TIMER_HANDLE) ||
		(DcInfo.curState != DC_IDLE_STATE) ||
		(DcInfo.fpBeingProcessed == TRUE) )
	{
		if(DcInfo.respPort != pin->respPort)
		{
			EPRINT(DC_LOG, "Ignoring PIN request as it is from different port than DcInfo.respPort, in DC state[%u]", DcInfo.curState);
			return retValue;
		}
	}

	//WE are sending to External Port so Selecet Rader 3 or Reader 4
 	DcInfo.readerId = READER4;
 	UpdateDcInfoReaderMode(READER4);
	DcInfo.respPort = pin->respPort;
	DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
	DeleteTimer(&(DcInfo.interDigitTmrHandle[DcInfo.respPort]));

	// Store rcvd credential
	StoreRcvdCred(PIN_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
	sprintf(DcInfo.keyBuf, "%s", pin->pinData);

	if(DcInfo.procSSAF == TRUE)
	{
		if( (strlen((CHARPTR)(pin->pinData)) > 0) &&
			(strcmp((CHARPTR)(pin->pinData), GetUserPinCode(DcInfo.userIdx)) == 0) )
		{
			retValue = PIN_MATCH;
		}
		else
		{
			retValue = PIN_INVALID;
		}
	}
	else if(IsPinValid((CHARPTR)(pin->pinData) , &userIdx) == YES)
	{
		// Store identified user
		DcInfo.userIdx = userIdx;
		retValue = PIN_MATCH;
	}
	else if( (strlen((CHARPTR)(pin->pinData)) > 0) &&
			 (strcmp((CHARPTR)(pin->pinData), SysBasicPara.pinChangeCode) == 0) &&
			 ( (DcInfo.curState == DC_IDLE_STATE) ||(DcInfo.curState == DC_PIN_CHANGE_STATE) ) )
	{
		retValue =  PIN_CHANGE_CODE_MATCH;
	}
	else
	{
		retValue =  PIN_INVALID;
	}

	return retValue;
}

//*****************************************************************************
//	IsDetectedFingerIsDuress
//	Param:
//		IN : 	user Index
//			 FP Index
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if ENROLL Duress  Flag is enable 
//		
//
//*****************************************************************************
BOOL IsDetectedFingerIsDuress(UINT32 userID,UINT8 FpIdx)
{
	UINT8 status = NO, Idx;
	USER_CONFIG_t tUserConfig = DfltUserConfig;

	GetUserConfig(userID, &tUserConfig);

	if(SysAdvPara.duressDetection == ENABLED) 
	{
		for(Idx = 0; Idx < MAX_DURESS_DUAL_FP_TEMPLATE; Idx++)
		{
			DPRINT(ACMS_LOG, "usrIdx[%d] FpIdx[%d] DuressIdx[%d] UserConfigFPIdx[%d]", userID, FpIdx, Idx, tUserConfig.DuressFp[Idx]);
			if(FpIdx == (tUserConfig.DuressFp[Idx]))
			{
				status = YES;
				break;
			}
		}
	}
	return status;
}

//*****************************************************************************
//	IsSyncFingerIsDuress
//	Param:
//		IN : 	user Index
//			 FP Index
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if Sync FP is Duress FP 
//		
//
//*****************************************************************************
BOOL IsSyncFingerIsDuress(UINT32 userID,UINT8 FpIdx)
{
	UINT8 status = NO, Idx,dualTempIdx;
	USER_CONFIG_t tUserConfig = DfltUserConfig;

	GetUserConfig(userID, &tUserConfig);

	if(SysAdvPara.templatesPerFinger == ONE_TEMPLATE)
	{
	    for(Idx = 0; Idx < MAX_DURESS_DUAL_FP_TEMPLATE; Idx++)
	    {
		    DPRINT(ACMS_LOG, "usrIdx[%d] FpIdx[%d] DuressIdx[%d] UserConfigFPIdx[%d]", userID, FpIdx, Idx, tUserConfig.DuressFp[Idx]);
		    if(FpIdx == (tUserConfig.DuressFp[Idx]))
		    {
			    status = YES;
			    break;
		    }
	    }
	}
	else
	{
	    dualTempIdx = (FpIdx * MAX_USER_DURESS_FP);
	    for(Idx = 0; Idx < MAX_DURESS_DUAL_FP_TEMPLATE; Idx++)
	    {
		    DPRINT(ACMS_LOG, "usrIdx[%d] FpIdx[%d] DuressIdx[%d] UserConfigFPIdx[%d]", userID, FpIdx, Idx, tUserConfig.DuressFp[Idx]);
		    if((dualTempIdx == (tUserConfig.DuressFp[Idx])) && (dualTempIdx+1 == (tUserConfig.DuressFp[Idx+1])))
		    {
			    status = YES;
			    break;
		    }
	    }
	}
	return status;
}

//*****************************************************************************
//	IsUserAuthenticated
//	Param:
//		IN :none
//		OUT: BOOL
//	Returns:
//	Description:
//          Returns the authentication status of the user
//*****************************************************************************
BOOL IsUserAuthenticated(void)
{
    if(DcInfo.isUserAuthenticatedF == FALSE)
   {
        if(GetDoorApplType() == DIRECT_DOOR_APPL)
        {
            SwitchDcToMenuState();
            ChangeMenuInfoStateToAdmin();
        }
        else
        {
            DegradeState = DGRD_MENU_NAVIGATION_STATE;
        }
       SetAudioVisual(USER_RESP_ENTER_PASSWD, INTERNAL_PORT, NULL);
       if(strlen(Password.displayPswd[USER_ADMIN]) > 0)
       {
           DisplayMenuScreen(MENU_LOGIN_SCREEN);
       }
       else
       {
           if(GetDoorApplType() == DIRECT_DOOR_APPL)
           {
               DisplayMenuScreen(MENU_CHANGE_PSWD_SCREEN);
           }
           else
           {
               DisplayMenuScreen(MENU_LOGIN_SCREEN);
           }
       }
       return FALSE;
   }
   return TRUE;
}

//*****************************************************************************
//	setDeviceInactiveStatus
//	Param:
//		IN :none
//		OUT: BOOL
//	Returns:
//	Description:
//          set Device Inactive Status
//*****************************************************************************
void setDeviceInactiveStatus(BOOL status, BOOL forceFullyStatusUpdate)
{
	if( (DcInfo.curState == DC_AUTO_SYSTEM_TEST_WAIT_STATE) || (DcInfo.curState == DC_AUTO_SYSTEM_TEST_STATE) )
	{
		return;
	}

    if( (sDeviceInactive != status) || (TRUE == forceFullyStatusUpdate) )
    {
        sDeviceInactive = status;
        DPRINT(DC_LOG,"sDeviceInactive Status = [%d]",sDeviceInactive);
		if(sDeviceInactive)
		{
			  UpdateDoorIdleDisplay();
			  if( TRUE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER1) )
			  {
                  if( CheckIsDeviceInactiveDelayRequired() == TRUE )
                  {
                      return;
                  }
		  DeleteTimer(&DcInfo.splFnGetUserInputTmrHandle);
                  SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
                  SwitchDcToIdleState();
                  SetAudioVisual(USER_RESP_DEVICE_INACTIVE, INTERNAL_PORT, NULL);
				  DisplayIdleScreen(0);
				  UpdateDisplayStatusField();
			  }
			  
			  if( TRUE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER3) )
			  {
                  if( CheckIsDeviceInactiveDelayRequired() == TRUE )
                  {
                      return;
                  }
				  SendGuiMessageToReader(USER_RESP_DEVICE_INACTIVE, (VOIDPTR)" ", GUI_MSG_MAX_SCREEN);
				  SetNwStatusCad(acmsCurActiveInterface, INVALID_LED_STATUS);
			  }
		}
		else
		{
		    //Make Device Normal mode
		    SetAudioVisual(USER_RESP_DEVICE_INACTIVE, INTERNAL_PORT, NULL);
		    if( CheckIsDeviceInactiveDelayRequired() == FALSE )
		    {
		   	SwitchDcToIdleState();
			DisplayIdleScreen(0);
		    }
		    UpdateDisplayStatusField();
		    SendGuiMessageToReader(USER_RESP_DEVICE_INACTIVE, (VOIDPTR)" ", GUI_MSG_MAX_SCREEN);
		    SetNwStatusCad(acmsCurActiveInterface, INVALID_LED_STATUS);
		}
    }
}

//*****************************************************************************
//	getDeviceInactiveStatus
//	Param:
//		IN :
//		OUT:
//	Returns : BOOL
//	Description:
//          getDeviceInactiveStatus
//*****************************************************************************
BOOL getDeviceInactiveStatus(void)
{
    return sDeviceInactive;
}

//*****************************************************************************
//	CheckAccessSchedule
//	Param:
//		IN :
//		OUT:
//	Returns:
//	Description:
//          Check Current Time  for Assign Access Schedule.
//*****************************************************************************
void CheckAccessSchedule(void)
{
    if(SysBasicPara.accessScheduleEnableF == FALSE)
    {
        setDeviceInactiveStatus(FALSE, FALSE);
        return;
    }

    if(SysBasicPara.application == CAFETERIA)  //Access schedule not applicable for the cafeteria
    {
        setDeviceInactiveStatus(FALSE, FALSE);
        return;
    }

    BOOL deviceInactive = TRUE;
    static UINT8 timeFrameIdx = 0;

    if( (0 <= timeFrameIdx) && (timeFrameIdx < MAX_TIME_FRAME) )
    {
        if( (AccessScheduleConfig[timeFrameIdx].timeFrameEnableF == ACTIVE) && (CheckCurrentTimeInGivenTime(timeFrameIdx) == YES) )
        {
             deviceInactive = FALSE;
             setDeviceInactiveStatus(deviceInactive, FALSE);
             return;
        }
    }

    for(timeFrameIdx = 0 ; timeFrameIdx < MAX_TIME_FRAME ; timeFrameIdx++)
    {
        if( (AccessScheduleConfig[timeFrameIdx].timeFrameEnableF == ACTIVE) && (CheckCurrentTimeInGivenTime(timeFrameIdx) == YES) )
        {
             deviceInactive = FALSE;
             break;
        }
    }

    setDeviceInactiveStatus(deviceInactive, FALSE);
}

//*****************************************************************************
//	ProcessUser
//	Param:
//		IN :READER_MODE_e
//		OUT:
//	Returns:
//	Description:
//          Process User for Entry or Exit.
//*****************************************************************************
void ProcessUser(READER_MODE_e mode)
{
    if(mode == ENTRY_READER)
    {
        DPRINT(DC_LOG, "going for ProcessUserOnEntry");
        ProcessUserOnEntry();
    }
    else
    {
        DPRINT(DC_LOG, "going for ProcessUserOnExit");
        ProcessUserOnExit();
    }
}

//*****************************************************************************
//	IsDeviceActive
//	Param:
//		IN :
//		OUT:
//	Returns:BOOL
//	Description:
//          Check Device Active or Not for process Access Policy.
//*****************************************************************************
BOOL IsDeviceActive(void)
{
    BOOL deviceInActive = getDeviceInactiveStatus();
    BOOL proceedPolicy  = TRUE;

    if(deviceInActive)
       proceedPolicy = FALSE;

    if( (deviceInActive) && (DcInfo.readerMode == EXIT_READER) && (SysAdvPara.verifyAccessPolicyOnExitF == DISABLED) )
       proceedPolicy = TRUE;

    return proceedPolicy;
}

//*****************************************************************************
//	CheckIsDeviceInactiveDelayRequired
//	Param:
//		IN :
//		OUT:
//	Returns:BOOL
//	Description:
//          Check Device Active Delay Is Required or Not
//*****************************************************************************
BOOL CheckIsDeviceInactiveDelayRequired(void)
{
    switch(DcInfo.curState)
    {
    case DC_WAIT_ADDL_CRED_STATE:
    case DC_WAIT_FOR_2ND_PERSON_STATE:
    case DC_WAIT_OCCUPANCY_NEXT_USER_STATE:
    case DC_TEMP_ADD_USER_STATE:
    case DC_ENROLLMENT_STATE:
    case DC_USER_TEMPERATURE_CHECK_STATE:
    case DC_FACE_MASK_CHECK_STATE:
    case DC_WAIT_FOR_SELF_ENROLL_REQ_STATE:
    case DC_SMART_CARD_KEY_UPDATE_STATE:
    case DC_USER_CARD_FIELD_READ_WRITE_STATE:
    case DC_PIN_CHANGE_STATE:
    {
        DcInfo.delayInActive = TRUE;
        return TRUE;
    }break;

    default:
        break;
    }

    return FALSE;
}

//*****************************************************************************
//	IsPasswdValidationRqrd()
//	Param:
//		IN : msgId
//		OUT: NONE
//	Returns:
//			 BOOL
//	Description:
//		This function Checks Display Password Login Restrict Timer Is Already Running For User
//
//*****************************************************************************
BOOL IsPasswdValidationRqrd(SLAVE_MSG_ID_e msgId)
{
    CONFIG_USER_TYPE_e userType = MAX_USER_TYPE;
    BOOL status = TRUE;
	UINT16 sourceMsg;

    switch (msgId)
    {
    case  GUI_BUTTON_ADMIN_MENU:
    case GUI_BUTTON_SYSTEM_STATUS:
        userType = USER_ADMIN;
		sourceMsg = msgId;
        break;

    case  GUI_BUTTON_HRD_MENU:
        userType = USER_HRD;
		sourceMsg = msgId;
        break;
    default:
        break;
    }

    if(userType != MAX_USER_TYPE && Password.DispPwdLoginRestrictTimer[userType] != INVALID_TIMER_HANDLE)
    {
        if(GetDoorApplType() == PANEL_DOOR_APPL && (CommState != SLAVE_ONLINE || DegradeState == DGRD_PROCESS_STATE))
        {
            SetAudioVisual(USER_RESP_PD_INVALID_PASSWORD, INTERNAL_PORT, (VOIDPTR)&userType);
			SendMsg(GUI_MSG_USER_RESTRICT_TIMER, &sourceMsg, sizeof(sourceMsg));
        }
        else if(GetDoorApplType() == DIRECT_DOOR_APPL)
		{
			SetAudioVisual(USER_RESP_MENU_LOGIN_RESTRICTED, INTERNAL_PORT, (VOIDPTR)&userType);
			LoadMenuWaitTimer(MENU_USER_RESP_TIMER, &DisplayMenuScreen, MENU_MAIN_MENU_SCREEN);
		}
		else if(GetDoorApplType() == PANEL_DOOR_APPL && (CommState == SLAVE_ONLINE))
        {
			SetAudioVisual(USER_RESP_MENU_LOGIN_RESTRICTED, INTERNAL_PORT, (VOIDPTR)&userType);
			SendMsg(GUI_MSG_USER_RESTRICT_TIMER, &sourceMsg, sizeof(sourceMsg));
            SetBuzLedResp(USER_RESP_ACCESS_DENIED, INTERNAL_PORT);
        }
        status = FALSE;
    }
    return status;
}

//*****************************************************************************
//	IsCredTypeCardOrQr
//	Param:
//		IN :
//		OUT:
//	Returns:BOOL
//	Description:
//          
//*****************************************************************************
BOOL IsCredTypeCardOrQr(CREDENTIAL_TYPE_e	curCredType)
{
	if((curCredType == CARD_CREDENTIAL) || (curCredType ==  ACCESS_CARD_QR_CREDENTIAL))
	{
		return YES;
	}
	else
	{
		return NO;
	}
}
