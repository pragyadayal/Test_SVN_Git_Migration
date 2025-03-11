#if !defined MXDCTASK_H
#define MXDCTASK_H
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
//   Project      : ACS (Access Control System)
//   Created By   : Ronak Tanna
//   File         : MxDcTask.h
//   Description  : This file declares DC task related defines and prototypes
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"
#include "MxSlaveMsg.h"
#include "MxSlaveCommon.h"
#include "MxSysCommonDef.h"
#include "MxDcTaskCommonDef.h"
#include "MxDcTaskFeatures.h"
#include "MxDcTaskUtils.h"
#include "MxConfigDef.h"
#include "MxSysTimer.h"
#include "MxEventLog.h"
#include "MxCardReaderCommon.h"
#include "MxAvResp.h"
#include "MxGuiHandler.h"
#include "MxSmartRouteAccess.h"
#include "MxVAM.h"
#include "MxWebAPIResponse.h"
#include "MxFpIdtServer.h"

//******** Defines and Data Types ****
#define KEY_CHANGE_WAIT_TIMER	5//second

typedef enum
{
	OCCUPANCY_CNTRL_CHECK,
	SPECIAL_FUNCTION_INPUT,
	DOOR_MODE_INPUT,
	JOB_COSTING_INPUT,
	GET_USER_IN_OUT_STATUS_FROM_PANEL,
	POLICY_CHECK_SUCCESS,
	MAX_POLICY_CHECK
}ACCESS_POLICY_CHECK_e;

typedef enum
{
	DC_STATE_NO_CHANGE,
	DC_STATE_SWITCH_TO_IDLE,
}DC_STATE_CHANGE_e;

typedef enum
{
	QR_DEVICE_INVALID,
	QR_CODE_USER_AUTHENTICATED,
	QR_CODE_USER_AUTHENTICATION_FAILED,
	QR_CODE_DEVICE_BUSY,
	QR_CODE_INVALID_READER_MODE,
	QR_CODE_API_DISABLED,
	QR_CODE_API_INVALID_INPUT,
	QR_ACCESS_MODE_RESTRICTED,
}QR_RESULT_e;

typedef enum
{
	WIGND_OUT_PROCESS_IDLE,
	WIGND_OUT_PROCESS_DONE,
	MAX_WIGND_OUT_PROCESS,
}WIGND_OUT_PROCESS_e;

typedef struct
{
	UINT64		cardId;
	READER_ID_e	readerId;
}CARD_DETAIL_t;

typedef enum
{
	PIN_CHANGE_IDLE_STATE,
	PIN_CHANGE_WAIT_FOR_OLD_PIN,
	PIN_CHANGE_WAIT_FOR_NEW_PIN,
	PIN_CHANGE_WAIT_FOR_CONFIRMATION,
}PIN_CHANGE_STATE_e;

typedef enum
{
	BLE_ACCESS_DEVICE_APTA,
	BLE_ACCESS_DEVICE_ACS,
	BLE_ACCESS_DEVICE_MAX,
}BLE_ACCESS_DEVICE_e;

// Structure encapsulating current door controller activity
typedef	struct 
{
	DC_FSM_STATE_e		curState;					// Current state of FSM

	UINT16 				userIdx;					// index of user being processed.
	USER_TYPE_e			userType;					// type of user like VIP or visitor or normal user
	UINT16				lastUserIdx;				// last user that was processed
	UINT16				zamUserIdx;					// user for which we are checking zam
	AUTH_REASON_e		authReason;					// reason for authentication or no authentication
	UINT8 				readerId;					// index of reader being processed.
	READER_MODE_e		readerMode;					// Entry/Exit reader
	RESPONSE_PORT_e		respPort;
	UINT16 				firstUserIdx; 				// index of first user
	UINT8				firstUsrGrpDtl;				// index of 2-Person Group of first user
	CREDENTIAL_CNT_e	credCnt;					// Number of credential presented so far
	CREDENTIAL_CNT_e	firstUserCredCnt;			// Number of credential presented so far
	CREDENTIAL_TYPE_e	credType[MAX_CREDENTIAL_CNT];	// Stores credential presented so far
	CREDENTIAL_TYPE_e	firstUserCredType[MAX_CREDENTIAL_CNT];	// Stores credential presented for first user
	BOOL				credInvalidF;				// Flag indication current cred type is invalid

	UINT8				useCount;					// Count of credential usage of same user
	TIMER_HANDLE   		interDigitTmrHandle[INT_EXT_PORT];		// Inter-digit timer handle
	TIMER_HANDLE		waitTmrHandle;				// Wait (delay) timer handle - used to delay next processing
	TIMER_HANDLE		menuWaitTmrHandle;			// Menu Wait timer handle
	TIMER_HANDLE		splFnStatusFieldTmrHandle;	// Timer to display Sp Func status on display
	TIMER_HANDLE		twoPersonTmrHandle;			// Handle for second person wait timer
	TIMER_HANDLE		multiAccessTmrHandle;		// Handle for multi-access timer for ZAM
	TIMER_HANDLE		useCountTmrHandle;			// User count timer handle
	TIMER_HANDLE 		lcdIdleTmrHandle;			// timer used to show screen with options like T&A, pin entry - idle state
	TIMER_HANDLE 		SmrtCardKeyChngTmrHandle;	//timer handle used for smart card key change
	DOOR_STATUS_e		doorStatus;					// Current door status
	TIMER_HANDLE		doorRelayPulseTmrHandle;	// Door Timer handle
	SPECIAL_FUNCTIONS_e splFnId;					// Special func Id being processed
	EV_USER_SPECIAL_CODE_FIELD_e 	singleUserSplCode;			// T&A related Special code, used for single user (special functions 1 to 8)
	EV_USER_SPECIAL_CODE_FIELD_e 	multiUserSplCode;			// T&A related Special code, used for multiple users (late in allowed and early out allowed)
	CARD_INDEX_e		splFnCardIdx;				// Special func card Idx
	TIMER_HANDLE		splFnGetUserInputTmrHandle;	// Timer handle to get user input for activating panic alarm or T&A related Special fn (1 to 8)
	TIMER_HANDLE		multiUserSplCodeTmrHandle; 	// Timer handle for activating Special code for multiple users
	TIMER_HANDLE		splFnNextInputTmrHandle;	// Wait (delay) timer handle - used to delay for asking next input
	TIMER_HANDLE		fpMatchTmrHandle;			// Used to abort finger mathcing process
	UINT8				freeFpScanStatusF;			// status of free scan (enabled or disabled) on door's FPR
	UINT8				verifyingFpByScanF;			// status of FP scanning (active/inactive) on door's FPR for verification
	CARD_TYPE_e			cardType;					// type of card detected last on door
	UINT16				addlSecCode;				// additional security code stored for last detected card
	BOOL				fpBeingProcessed;			// Finger print is being processed now
	TIMER_HANDLE		doorLockDebounceTmrHandle;	// Timer handle for debouonce time
	TIMER_HANDLE		auxIpDebounceTmrHandle;		// Timer handle for Aux input debounce time
	BOOL				dcTaskIdleF;				// Status for DC task idle/busy
	BOOL				procSSAF;
	BOOL				sendAphanumIdInEvent;
	UINT8				userFpcnt;					// Enrolled FP/No. of FP written in card
	BOOL				internalFpMemFullF;			// Internal FP Module Memory Full Flag
	BOOL				externalFpMemFullF;			// External FP Module Memory Full Flag

	BOOL				splFnThroughCard;			// Special Function Called from card

	UINT8				lastDoorLevel;				// Door Level which is in card
	UINT8				userRouteReaderMode;		// Door Mode which is in card
	BOOL				smartRouteActiveF;
	BOOL				firstInTempLockF;			//temperory first in lock status which is useful incase of 2 person
	ROUTE_USER_ALLOW_REASONS_e	routeUserAllowReason;
	//panel door specific timer
	TIMER_HANDLE		dgTmrHandle;				//Degrade timer handle
	BOOL				bypassBioCred;
	TIMER_HANDLE		selfEnrlInputTimerHandle;
	UINT16				vamConfigIndex;
	VAM_CARD_TYPE_e		detectedVamCardType;
	BOOL				detectedVamCards[MAX_VAM_CARD_TYPE];
	TIMER_HANDLE		vamCredWaitTimer;

	UINT64				visitorExtraIfo1;		//here size is UINT32 but to print as string we have taken UINT64
	UINT32				visitorExtraIfo2;		//here size is UINT16 but to print as string we have taken UINT32

	UINT8				userDetectCnt;
	BOOL				allowWithoutFinalCheck;
	TIMER_HANDLE		occupancyNextUsrWaitTimer;
	UINT16				firstOccupantIdx;
	AUTH_REASON_e		firstOccupantAuthReason;
	CREDENTIAL_CNT_e	firstOccupantCredCnt;			// Number of credential presented so far
	CREDENTIAL_TYPE_e	firstOccupatcredType[MAX_CREDENTIAL_CNT];	// Stores credential presented so far

	UINT32 				selectedJobIdx;					//selected job index in JCM config
	TIMER_HANDLE		jobInputWaitTimer;
	BOOL				showDoorModeOnAllowF;

	TIMER_HANDLE		userAllowWaitTmrHandle;
	BOOL				wiegandOutProcessF;
	CARD_DETAIL_t		lastDetectedCard;
	TIMER_HANDLE			doorModeInputWaitTimer;
	ACCESS_POLICY_CHECK_e	accessPolicyCheckState;
	USER_EVENT_INFO_e		doorEvtMode;
	UINT16					fCInCSN;
	UINT16					fCInCardMem;
	ACCESS_POLICY_CHECK_e	SSAPolicyCheckState;
	BOOL					wgndAuthTimeOut;
	TIMER_HANDLE			auxOutActTmrHandle;
	BOOL					apiUser;
	QR_AUTH_REASON_e		qrAuthReason;
	WEB_API_QR_CODE_RESP_MODE_e	qrResponseMode;
	FLOAT32				qrResponseTempValue;
	BOOL					cardCredntlDetectedF;
	TIMER_HANDLE			multiInputWaitTmrHandle;
	PIN_CHANGE_STATE_e		pinChangeModeState;
	UINT8					keyCnt;
	CHAR					keyBuf[MAX_PIN_SIZE];
	TIMER_HANDLE			firstInResetTimer;	// First In Reset Timer
	IDT_SERVER_TEMPLATE_DATA_t lastFpDataSendToServer;
	TIMER_HANDLE			spFnInputWaitTimer;
	TIMER_HANDLE			ftpFirmwrTrnsfrTmrHandle;
	TIMER_HANDLE			verifyApiWaitTimer;
	ENROLL_SOURCE_e			enrollSource;
	TIMER_HANDLE			doorOpenCloseEventWaitTimer;//to generate event that user has not operated door till timeout
	UINT32					userIdDoorOpenCloseEvent;	//store user id for whom door is opened to generate event 208
	CHAR					alphanumIddoorOpenCloseEvent[ALPHANUM_ID_SIZE]; //store alphanum Id id for whom door is opened to generate event 208
	TIMER_HANDLE			showAttendanceDetailTimer;
	BOOL					isLcdTimerRunning;
	TIMER_HANDLE			installationwaitTimerhandle;
	TIMER_HANDLE			sysTestScreenWaitTimerhandle;
	CHAR					designation[DESIGNATION_SIZE];	// designation
	UINT8					accessLevel;		//door SSA access level
	TIMER_HANDLE			DisplayFRStreamToIdleScreenTimer;	// FR free scan and tap and go mode Timer 
	TIMER_HANDLE			displayFRStreamTimer;	// Wait fot 200ms for proper FR image to be create
	BOOL					cbReaderBleAccess;
	UINT8					faceIdx;
	TIMER_HANDLE			usrApproachWaitTmrHandle;
	TIMER_HANDLE			usrTemperatureDetectTmrHandle;
	BLE_ACCESS_DEVICE_e		bleAccessDevice;
	FLOAT32					usrTemperature;
	BOOL					usrExceedTempThreshold;
	TEMPERATURE_UNIT_e		qrTempUnit;
	TEMP_ACCESS_DECISION_e	qrAccessDecision;
	FLOAT32					firstUsrTemperature;
	BOOL					firstUsrExceedTempThreshold;
	TEMPERATURE_UNIT_e		firstUsrQrTempUnit;
	TEMP_ACCESS_DECISION_e	firstUsrQrAccessDecision;
	BOOL					pinVerified;
#if defined(FACE_MASK_COMPULSION_ENABLE)
	FACE_MASK_ACCESS_DECISION_e	faceMaskAccessDecision;
	FACE_MASK_ACCESS_DECISION_e	firstUserFaceMaskAccessDecision;
	BOOL						isFaceMaskViolated;
	BOOL						isFirstUsrFaceMaskViolated;
#endif	
	BOOL				duressF;					// Flag indicating duress detected
    BOOL                    isUserAuthenticatedF;
	TIMER_HANDLE			waitForFrStreamtmrHandle;
    BOOL                    delayInActive;
	UINT32	credSubType[MAX_CREDENTIAL_CNT];	      	// Stores present credential Card1 or Card2
	UINT32	firstUserSubCredType[MAX_CREDENTIAL_CNT];	// Stores present credential Card1 or Card2 for first user
	UINT32	firstOccupatSubcredType[MAX_CREDENTIAL_CNT]; 	// Stores present credential Card1 or Card2 for first Occupat
}DOOR_ACTIVITY_INFO_t;

//******** Function Prototypes *******
void InitDcTask(void);

void SwitchDcToIdleState(void);
void SwitchDcToBackupupdateState(void);
void SwitchDcToZAMState(void);
void SwitchDcTo2PersonState(void);
void SwitchDcToCardKeyUpdateState(void);
void SwitchDcToPanelDoorFirmwareUploadState(void);
void SwitchDcToInstallationState(void);

void DcDefaultMsgHandler(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
void ProcMsg(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
void ProcessUserOnExitOnExternal(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr);
BOOL CheckTwoPersonFeature(void);
BOOL CheckSmartRouteFeature(void);

void UserAllowed(UINT16 userIdx, READER_MODE_e readerMode, RESPONSE_PORT_e respPort, DC_STATE_CHANGE_e dcSateChange);
void DenyUser(UINT16 userIdx, READER_MODE_e readerMode, RESPONSE_PORT_e respPort, DC_STATE_CHANGE_e dcSateChange);
void CafeDenyUser(UINT16 userIdx, READER_MODE_e readerMode, RESPONSE_PORT_e respPort, DC_STATE_CHANGE_e dcSateChange);

PIN_RESULT_e ProcessPinDetect(PIN_DATA_t * pin);
PIN_RESULT_e ProcessPinDetectOnWgnd(WGND_PIN_DATA_t *pin);
FP_RESULT_e ProcessFpDetect(FP_DETECTED_EV_DATA_t * fpDataPtr);
CARD_RESULT_e ProcessCardDetect(CARD_DETECTED_EV_DATA_t * cardDataPtr, CREDENTIAL_TYPE_e credType);
FP_RESULT_e ProcessFpScanTemplateResp(void * iDataPtr);

void CheckFinalPolicyAndAllow(void);
VOID InitCredInfo(void);
void ProcessUserOnEntry(void);
void ProcessUserOnExit(void);
void WaitOccupancyNextUserStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
void SwitchDcToOccupancyWaitForUserState(void);
void DeleteTimersForIdleState(void);
void DcDoorModeSelectionStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
void CheckFinalSSAPolicyAndAllow();
BOOL IsDoorModeSelectionRequired(void);
BOOL InstallationProcessWaitTimer(UINT16 timerValue);
void InstallationProcessTimerTimeOut(UINT32 dummy);
BOOL StartSystemTestScreenWaitTimer(UINT16 timerValue);
void StartSystemTestScreenTimerTimeOut(UINT32 dummy);

PIN_RESULT_e ProcessPinDetectOnExternalReader(READER_PIN_DATA_t *pin);

BOOL IsDetectedFingerIsDuress(UINT32 userID,UINT8 FpIdx);
BOOL IsSyncFingerIsDuress(UINT32 userID,UINT8 FpIdx);
BOOL IsUserAuthenticated(void);
void setDeviceInactiveStatus(BOOL status, BOOL forceFullyStatusUpdate);
BOOL getDeviceInactiveStatus(void);
void CheckAccessSchedule(void);
void ProcessUser(READER_MODE_e mode);
BOOL IsDeviceActive(void);
BOOL CheckIsDeviceInactiveDelayRequired(void);
BOOL IsPasswdValidationRqrd(SLAVE_MSG_ID_e msgId);
BOOL IsCredTypeCardOrQr(CREDENTIAL_TYPE_e	curCredType);
#endif  // MXDCTASK_H

