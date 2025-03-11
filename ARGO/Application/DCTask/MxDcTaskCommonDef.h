#if !defined MxDcTaskCommonDef_H
#define MxDcTaskCommonDef_H
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
//   File         : MxDcTaskCommonDef.h
//   Description  : This file declares DC task related defines and prototypes
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"

//******** Defines and Data Types ****

#define DOOR_PULSE_TIMER_CNT				(DOOR_PULSE_TIME * 10 * TIMER_100MS)
#define SOFT_DISABLED						0xff
#define DOOR_STATUS_NORMAL					0xee	// It should not match any other used value

#define	INTER_DIGIT_TIMER_CNT				(SysAdvPara.interDigitTimer * 10)		// Resolution in multiple of 100 msec
#define OP_PORT_LATCH_TIME					0
#define DOOR_OPEN_WAIT_TIMER				3
#define MAX_DEG_EV_IN_SINGLE_MSG			10

#define ACCESS_CARD1					0x100
#define ACCESS_CARD2					0x200
#define CARD_NUMBER_NOT_APPLICABLE			0x000

#define CRED_TYPE(FUNC) \
	FUNC(PIN_CREDENTIAL,			MSG_SORRY_INVALID_PIN_570) \
	FUNC(CARD_CREDENTIAL,			MSG_SORRY_INVALID_CARD_569) \
	FUNC(FP_CREDENTIAL,				MSG_FINGER_NOT_IDENTIFIED_237) \
	FUNC(BLE_QR_CREDENTIAL,			MSG_ACCESS_DENIED_0)  /*For QR code no any QR code display screen defined*/\
	FUNC(INVALID_CREDENTIAL,		MSG_INVALID_INPUT_286) \
	FUNC(FACE_CREDENTIAL,			MSG_SORRY_FACE_NOT_IDENTIFIED_941)\
	FUNC(CB_READER_BLE_CREDENTIAL,	MSG_ACCESS_DENIED_0)\
	FUNC(ACCESS_CARD_QR_CREDENTIAL,	MSG_SORRY_INVALID_CARD_569)\

typedef enum
{
	CRED_TYPE(GEN_ARG1_FROM_2)
	MAX_CREDENTIAL_TYPE,
}CREDENTIAL_TYPE_e;

typedef enum
{
	FIRST_CREDENTIAL,
	SECOND_CREDENTIAL,
	THIRD_CREDENTIAL,
	MAX_CREDENTIAL_CNT,
}CREDENTIAL_CNT_e;

typedef enum
{
	CURRENT_USER,	// Current user or first user in case of only one user
	FIRST_USER,		// First user (who already accessed system) in case of two persons
	FIRST_OCCUPANT,
	MAX_USER_CNT
}USER_CNT_e;

// Enum defining different states of DC task FSM.
#define DC_FSM_STATE_ENUM(DC_FSM_STATE_ENUM, DC_FSM_STATE_FUNC)	DC_FSM_STATE_ENUM,
#define DC_FSM_STATE_FUNC(DC_FSM_STATE_ENUM, DC_FSM_STATE_FUNC)	DC_FSM_STATE_FUNC,

#define DC_FSM_STATE(ARG)\
		ARG(DC_IDLE_STATE,						DcIdleStateFn)\
		ARG(DC_WAIT_FOR_2ND_PERSON_STATE,		DcTwoPersonStateFn)\
		ARG(DC_WAIT_ADDL_CRED_STATE,			DcWaitForAddlCredStateFn)\
		ARG(DC_ENROLLMENT_STATE,				DcEnrollmentStateFn)\
		ARG(DC_MENU_STATE,						DcMenuStateFn)\
		ARG(DC_SPL_FUNC_STATE,					DcSplFnStateFn)\
		ARG(DC_BACKUP_UPDATE_STATE,				DcBackupUpdateStateFn)\
		ARG(DC_HTTP_FIRMWARE_UPLOAD_STATE,		DcHttpFirmwareUploadStateFn)\
		ARG(DC_ACMS_FIRMWARE_UPLOAD_STATE,		DcAcmsFirmwareUploadStateFn)\
		ARG(DC_SSA_STATE,						DcSSAStateFn)\
		ARG(DC_CAFETERIA_STATE,					DcCafeMenuStateFn)\
		ARG(DC_CAFE_CMD_STATE,					DcCafeCmdStateFn)\
		ARG(DC_ROUTE_ACCESS_STATE,				DcSmartRouteAccessStateFn)\
		ARG(DC_SYSTEM_TEST_STATE,				DcSystemTestStateFn)\
		ARG(DC_SMART_CARD_KEY_UPDATE_STATE,		DcSmartCardKeyUpdateStateFn)\
		ARG(DC_PANELDOOR_FIRMWARE_UPLOAD_STATE,	DcPanelDoorFirmwareUploadStateFn)\
		ARG(DC_WAIT_FOR_SELF_ENROLL_REQ_STATE,	DcWaitForSelfEnrlInputState)\
		ARG(DC_VAM_CARD_ACCESS_STATE,			DcVamCardAccessStateFn)\
		ARG(DC_VAM_ROUTE_ACCESS_STATE,			DcVamRouteAccessState)\
		ARG(DC_WAIT_OCCUPANCY_NEXT_USER_STATE,	WaitOccupancyNextUserStateFn)\
		ARG(DC_JOB_CODE_DISPLAY_STATE,			DcJobCodeDisplayStateFn)\
		ARG(DC_USER_ALLOW_WAIT_STATE,			DcUsrAllowWaitState)\
		ARG(DC_DOOR_MODE_SELECTION_STATE,		DcDoorModeSelectionStateFn)\
		ARG(DC_USER_VERIFY_WAIT_STATE,			DcUsrVerifyWaitState)\
		ARG(DC_USER_CAFE_BAL_READ_WRITE_STATE,	DcCafeBalReadWriteState)\
		ARG(DC_USER_CARD_FIELD_READ_WRITE_STATE,DcCardFieldsReadWriteState)\
		ARG(DC_PIN_CHANGE_STATE,				DcPinChangeState)\
		ARG(DC_SELECT_SP_FN_STATE,				DcSelectSpFnState)\
		ARG(DC_GET_USER_STATUS_FROM_PANEL_STATE,DcGetUserStatusFromPanelStateFn)\
		ARG(DC_VERIFY_FINGER_API_WAIT_STATE,	DcVerifyFingerApiWaitStateFn)\
		ARG(DC_INSTALLATION_STATE,				DcInstallationStateFn)\
		ARG(DC_AUTO_SYSTEM_TEST_STATE,			DcAutoSystemTestStateFn)\
		ARG(DC_AUTO_SYSTEM_TEST_WAIT_STATE,		DcAutoSystemTestWaitStateFn)\
		ARG(DC_USER_TEMPERATURE_CHECK_STATE,	DcUserTemperatureCheckStateFn)\
		ARG(DC_FACE_MASK_CHECK_STATE,			DcFaceMaskCheckStateFn)\
		ARG(DC_TEMP_ADD_USER_STATE,			    DcTempAddUserStateFn)\

typedef enum
{
	DC_FSM_STATE(DC_FSM_STATE_ENUM)
	MAX_DC_FSM_STATE,
}DC_FSM_STATE_e;

typedef enum 
{
	NORMAL_USER,
	VIP_USER,
	VISITOR_USER,
}USER_TYPE_e;

typedef enum 
{
    // allowed reason
	AUTH_ALLOWED,					// user is allowed (no specific reason)
	AUTH_ALLOWED_DURESS,			// user is allowed With duress
	AUTH_PASSBACK_ENTRY_SOFT_VIOLATION,	// user is allowed (soft violation of anti pass back rule)
	AUTH_PASSBACK_EXIT_SOFT_VIOLATION,	// user is allowed (soft violation of anti pass back rule)
	AUTH_ALLOW_SSA_USER,
	AUTH_ALLOW_USER_SOFT_ROUTE_ACCESS,
	AUTH_ALLOW_2PERSON_PRIMARY,
	AUTH_ALLOW_2PERSON_SECONDARY,
	// denied reason
	AUTH_USER_DISABLED,				// user is disabled
	AUTH_USER_BLOCKED,				// user has been blocked
	AUTH_PASSBACK_ENTRY_HARD_VIOLATION,	// hard violation -used in case of denied	
	AUTH_PASSBACK_EXIT_HARD_VIOLATION,	// hard violation -used in case of denied	
	AUTH_INVALID_PIN,				// pin is not identified
	AUTH_INVALID_FINGER,			// finger is not identified
	AUTH_INVALID_FACE,				// face is not identified
	AUTH_INVALID_CARD,				// card is not identified
	AUTH_FP_MATCHING_TIMEOUT,		// FP matching timeout
	AUTH_NO_ACCESS,					// user has no access for the zone
	AUTH_TWO_PERSON,				// 2-person rule is not satisfied
	AUTH_OCCUPANCY_VIOLATION,		// occupancy control rule is not satisfied
	AUTH_DOOR_LOCK,					// Deny user for door lock
	AUTH_FIRST_IN,					// Deny user for first - in lock
	AUTH_TIMEOUT,					// Deny user for not giving input within time
	AUTH_CARD_READ_FAILED,			// Deny user for card read failed
	AUTH_VALIDITY_EXPIRED,			// Deny user for validity expired
	AUTH_INVALID_INPUT,				// Input is not as per access mode
	AUTH_INVALID_ROUTE_INPUT,		// Input Credential is not as per route
	AUTH_INVALID_ROUTE_ACCESS,		// Input is not as per route policy
	AUTH_DOOR_NOT_IN_ROUTE,			// Door is not Part of Route
	AUTH_EVENT_LOG_FAIL,			// Failed to write Event into file
	AUTH_DENIED_DOOR_NOT_ASSIGNED,
	// e-Canteen Reasons
	AUTH_CAFE_TXN_REASON_PREPAID_DEVICE,
	AUTH_CAFE_TXN_REASON_PREPAID_DEVICE_DISCNT,
	AUTH_CAFE_TXN_REASON_PREPAID_SERVER,
	AUTH_CAFE_TXN_REASON_PREPAID_SERVER_DISCNT,
	AUTH_CAFE_TXN_REASON_POSTPAID,
	AUTH_CAFE_TXN_REASON_POSTPAID_DISCNT,
	AUTH_CAFE_TXN_REASON_USER_IS_NOT_ACTIVE,
	AUTH_CAFE_TXN_REASON_USER_NOT_VALID,
	AUTH_CAFE_TXN_REASON_USER_NOT_ALLOWED,
	AUTH_CAFE_TXN_REASON_INSUFFICIENT_BALANCE,
	AUTH_CAFE_TXN_REASON_INVALID_CREDENTIAL,
	AUTH_CAFE_TXN_REASON_CARD_READ_FAIL,
	AUTH_CAFE_TXN_REASON_CARD_WRITE_FAIL,
	AUTH_CAFE_TXN_REASON_NO_ITEM_SELECTED,
	AUTH_ROUTE_ACCESS_CARD_WRITE_FAILED,
	AUTH_CAFE_TXN_REASON_INVALID_TRANSACTION,
	AUTH_CAFE_POLICY_DENIED_WEEK_OFF,
	AUTH_CAFE_POLICY_DENIED_NON_WORKING_DAY,
	AUTH_CAFE_POLICY_DENIED_INVALID_SHIFT,
	AUTH_CAFE_POLICY_DENIED_DAILY_TRANSACTION,
	AUTH_CAFE_POLICY_DENIED_DAILY_MENU_LIMIT,
	AUTH_CAFE_POLICY_DENIED_DAILY_ITEM_LIMIT,
	AUTH_CAFE_POLICY_DENIED_MONTHLY_TRANSACTION,
	AUTH_CAFE_POLICY_DENIED_MONTHLY_MENU_LIMIT,
	AUTH_CAFE_POLICY_DENIED_MONTHLY_ITEM_LIMIT,
	AUTH_CAFE_POLICY_DENIED_WEEKLY_LIMIT,
	AUTH_CAFE_POLICY_DENIED_OFFLINE_TRANSACTION_NOT_ALLOWED,
	AUTH_CAFE_USER_ID_MISMATCH,
	AUTH_CAFE_POLICY_DENIED_PREORDER_LIMIT,
	AUTH_CAFE_POLICY_DENIED_OFFLINE_ITEM_PER_MENU_LIMIT,
	//VAM Reasons
	AUTH_VAM_ALLOWED,
	AUTH_VAM_ALLOWED_SOFT_ROUTE,
	AUTH_VAM_INVALID_CARD,
	AUTH_VAM_DOOR_LOCK,
	AUTH_VAM_VALIDITY_EXPIRED,
	AUTH_VAM_NO_ACCESS,
	AUTH_VAM_CRED_INVALID_SEQUENCE,
	AUTH_VAM_TIMEOUT,
	AUTH_VAM_INVALID_ROUTE_ACCESS,
	AUTH_VAM_DOOR_NOT_IN_ROUTE,
	AUTH_VAM_INVALID_CRED_FOR_ROUTE_ACCESS,
	AUTH_VAM_ROUTE_ACCESS_CARD_WRITE_FAILED,

	AUTH_MIN_OCCUPANCY_IS_1,
	AUTH_MIN_OCCUPANCY_IS_2,
	AUTH_ZERO_OCCUPANCY_NOT_ALLOWED_1,
	AUTH_ZERO_OCCUPANCY_NOT_ALLOWED_2,
	//wiegand out
	AUTH_WIEGAND_WAIT_TIMEOUT,

	// denied reason Qr Code
	AUTH_API_INVALID_INPUT,
	AUTH_CAFE_TXN_REASON_SERVER_TIMEOUT,
	AUTH_VISITOR_ALLOWED_SURRENDER_PASS,
	AUTH_EVENT_REGISTERED,

	AUTH_USER_NOT_PRESENT_ON_PANEL,
	AUTH_FACE_MATCHING_TIMEOUT,
	AUTH_FR_DISABLED,
	AUTH_ADAPTIVE_ERL_REJECT,
	AUTH_ADAPTIVE_ERL_TIMEOUT,
	AUTH_TEMP_HARD_THRESHOLD_EXCEED,
	AUTH_AMBIENT_TEMPERATURE_CONFLICT,
	AUTH_CAFE_USER_SELECTED_NO,
#if defined(FR_ADDITIONAL_FEATURE_SUPPORTED)
	AUTH_USER_FR_DISABLED,
	AUTH_FR_DISABLED_ON_DEVICE,
#endif
#if defined(FACE_MASK_COMPULSION_ENABLE)
	AUTH_FACE_MASK_HARD_VIOLATION,
#endif
	AUTH_INVALID_ZAM_INPUT,
	AUTH_INVALID_FINGER_TEMP_USER,
	MAX_AUTH_REASON
}AUTH_REASON_e;


typedef enum
{
	PIN_INVALID,
	PIN_CONTINUE,
	PIN_MATCH,
	PIN_CHANGE_CODE_MATCH,
}PIN_RESULT_e;

typedef enum
{
	CARD_INVALID,
	CARD_CONTINUE,
	USER_CARD_MATCH,
	SP_FUNC_CARD_MATCH,
	CARD_VAM_MATCH,
}CARD_RESULT_e;

typedef enum
{
	FP_INVALID,
	FP_CONTINUE,
	FP_MATCH,
	FP_MATCHING_TIMEOUT,
	FP_NOT_MATCH,
}FP_RESULT_e;

// Individual door lock state
typedef enum
{
	DOOR_LOCK_NORMAL,
	DOOR_LOCKED,
	DOOR_UNLOCKED,
	DOOR_LOCK_MAX,
}DOOR_LOCK_STATE_e;

typedef enum
{
	NO_VIOLATION,
	SOFT_VIOLATION,
	HARD_VIOLATION,
}ANTIPASSBACK_STATUS_e;

typedef enum
{
	USER_OUT,
	USER_IN,
	USER_UNKNOWN,
}USER_STATE_e;

// Soft status which needs to be changed
typedef enum
{
	CHANGE_DC_SENSE,	// Change door sense status
	CHANGE_DC_AUX_IN,	// Change aux in status
	CHANGE_DC_AUX_OUT,	// Change aux out status
	CLEAR_ALL_FP,		// Clear All FP Command
	RESET_AUX_OUT,		// Reset aux out flag
}DOOR_SOFT_CONTROL_e;

// Door status to show on jeeves
typedef enum
{
	MDS_INACTIVE,
	MDS_ACTIVE,
	MDS_SHORT,
	MDS_OPEN,
	MDS_DISABLE,
	MDS_OVERRIDEN,
	MDS_MAX
}MONITORED_DOOR_STATUS_e;

// Logical status of door lock
typedef enum
{
	STATUS_UNKNOWN,					// When we cannot sense door status
	STATUS_DOOR_NORMAL_OPEN,		
	STATUS_DOOR_NORMAL_CLOSE,		
	//STATUS_DOOR_HELD_OPEN,		// Door is open after pulse timer expires
	STATUS_DOOR_ABNORMAL,			// Door is open after held timer expires
	STATUS_DOOR_FORCE_OPEN,			// Door is open when it was supposed to be close
	STATUS_DOOR_SENSE_FAULT,		// Cannot detect lock status due to sense fault
	STATUS_DOOR_NORMAL_SENSE_OPEN,
	STATUS_DECLARE_FORCED_OPEN,		// Intermediate status to declare door force open
}DOOR_STATUS_e;

typedef enum
{
	NORMAL_TXN,
	RESET_TXN,
	RECHARGE_TXN,
	READ_BALLANCE_TXN,	
}CAFE_CARD_TXN_e;

typedef enum
{
	FUNC_INACTIVE,
	LATE_IN_ACTIVE,
	EARLY_OUT_ACTIVE,
}MULTI_USER_SPL_STATUS_e;

typedef enum
{
	PRIMARY_AND_SECONADAY_BOTH_GRP,
	PRIMARY_GRP,
	SECONDARY_GRP,
	MAX_TWO_PERSON_USER_GRP_DTL,
}TWO_PERSON_USER_GRP_DTL_e;

typedef enum
{
	TA_GOING_IN_SCREEN,
	TA_GOING_OUT_SCREEN,
	TA_MAX_SCREEN,
}TA_SCREEN_e;

typedef enum
{
	FACE_INVALID,
	FACE_CONTINUE,
	FACE_MATCH,
	FACE_MATCHING_TIMEOUT
}FACE_RESULT_e;
//******** Function Prototypes *******

#endif  // MxDcTaskCommonDef_H
