/////////////////////////////////////////////////////////////////////////////////
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
//   Created By   : Ronak Tanna
//   File         : MxEnrollment.c
//   Description  : This file provides complete functionalities for enrolling
//					user or special function.
//
//			For user enrollment, following modes are supported:
//					- Read only card
//					- Smart card
//					- Finger print
//					- Finger print then card
//
//			For Special function enrollment, following mode is supported:
//					- Read only card
//					- Smart card, but it is enrolled as read only card because
//					  card write feature is not to be supported for spl fn now.
//
//			Enrollment can be triggered through ACMS, web jeeves or spl fn.
//
//			It provides API to delete user finger print from all finger print
//			readers for specific user.
//
/////////////////////////////////////////////////////////////////////////////

//******** Include files **********
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "MxTypedef.h"
#include "MxEnrollment.h"
#include "MxSlaveMsg.h"
#include "MxConfig.h"
#include "MxDcTask.h"
#include "MxSysTimer.h"
#include "MxConfigFileIo.h"
#include "MxEventLog.h"
#include "MxAvResp.h"
#include "MxDcSpecialFunc.h"
#include "MxLogs.h"
#include "MxAcmsMsgHandler.h"
#include "MxMenu.h"
#include "MxReaderUtils.h"
#include "MxSmartCardFormatConfig.h"
#include "MxCafeScreenMirror.h"
#include "MxCardReaderCommon.h"
#include "MxSysCommonUtils.h"
#include "MxReaderCommonUtils.h"
#include "MxThirdPartyServerCommand.h"
#include "MxCommonBLE.h"
#include "MxSupremaReader.h"
#include "MxUserConfig.h"
#include "MxFaceRecognition.h"
#include "MxFaceRecognitionConfig.h"
#include "MxMjpegStream.h"

//******** Extern Variables **********
extern DOOR_ACTIVITY_INFO_t DcInfo;
extern SYS_BASIC_PARA_t		SysBasicPara;
extern SPECIAL_FUNCTION_t 	SpecialFunctions[];
extern SYS_ADVANCE_PARA_t	SysAdvPara;
extern SERVER_PARA_t		ServerParameters;
extern const CHARPTR		ReaderRespStatusStr[];
extern FP_TRANS_PARA_t 		FpTransPara;
extern BOOL					selectionInProgress;
extern CAFE_MIRROR_PROC_INFO_t	cafeScreenMirrorProcInfo;
extern CHARPTR 					InterfaceStr[MAX_PORT];
extern BOOL    				EnrollmentCmdRunningF;
extern SERVER_MODE_CONFIG_t	ServerModeCfg;
extern CURNT_ENRLENT_INFO_t CurEnrlInfo;
extern FR_SERVER_CONFIG_t				FRServerConfig;
extern BOOL FrMultiReqF;
extern BOOL						FRStreamDisplayF;
extern const USER_CONFIG_t			DfltUserConfig;
extern const CHARPTR					EnrollmentStatusStr[];

//****** Defines and Data Types ******
#define CURL_SERVER_API_TIMEOUT					20//Second
#define USER_CONFIG_SYNC_WAIT_TIME				30//Second
#define SERVER_API_RESP_FILE					"ServerApiResp.txt"
#define SERVER_API_RESP_FILE_PATH				"/tmp/"SERVER_API_RESP_FILE
#define CURL_CMD_HEADER							"curl --interface %s -k -m %u --user %s:%s -X POST -H content-length:0 -s "
#define SERVER_API_IS_USER_EXIST				"\"http://%s:%d/%s/api.svc/v2/user?action=get;Id=%s;format=xml\" -H \"Host: %s\" >%s"
#define SERVER_API_ADD_USER						"\"http://%s:%d/%s/api.svc/v2/user?action=set;id=%s;name=%s;module=U;\" -H \"Host: %s\" >%s"
#define SERVER_API_ASSIGN_USER					"\"http://%s:%d/%s/api.svc/v2/device?action=assign;device=d_%d;id=%s;\" -H \"Host: %s\" >%s"
#define ENCRPTD_SERVER_API_IS_USER_EXIST		"\"https://%s:%d/%s/api.svc/v2/user?action=get;Id=%s;format=xml\" -H \"Host: %s\" >%s"
#define ENCRPTD_SERVER_API_ADD_USER				"\"https://%s:%d/%s/api.svc/v2/user?action=set;id=%s;name=%s;module=U;\" -H \"Host: %s\" >%s"
#define ENCRPTD_SERVER_API_ASSIGN_USER			"\"https://%s:%d/%s/api.svc/v2/device?action=assign;device=d_%d;id=%s;\" -H \"Host: %s\" >%s"


#define FACE_ENROLL_START_WAIT_TMR								3 //second
#define FACE_ENROLL_GLOBLE_WAIT_TMR								10 //second
#define FACE_ENROLL_ABORT_STOP_USRINPUT_WAIT_TMR				5 //second

typedef enum
{
	ENROLL_USER_PROX_CARD_STATE = 0,
	ENROLL_USER_SMARTCARD_STATE,
	ENROLL_USER_FP_STATE,
	ENROLL_SPL_FN_CARD_STATE,
	ENROLL_GET_INPUT_STATE,
	ENROLL_ADD_OR_ASSIGNUSER_STATE,
    ENROLL_USER_MOBILE_STATE,
	ENROLL_USER_FACE_STATE,
	ENROLL_STATE_END,
}ENROLL_STATE_e;

typedef enum
{
	PROC_CARD_DETECTED_EV,
	PROC_WRITE_FP_RESP,
	PROC_WRITE_SEC_INFO_RESP,
	PROC_WRITE_PERS_INFO_RESP,
	PROC_CHANGE_SMART_CARD_KEY_RESP,
	PROC_ERASE_SMART_CARD_RESP,
	PROC_WRITE_CARD_ID_RESP,
}ENROLL_CARD_STATE_e;

typedef enum
{
	CARD_FIELD_ANY,
	CARD_FIELD_FP,
	CARD_FIELD_PERS_INFO,
	CARD_FIELD_SEC_INFO,
	CARD_FIELD_ID,
}CARD_FIELD_CATEGORY_e;




typedef struct
{
	BOOL						isServerRespPending;		// respons from server is pending
	ENROLL_FACE_STATE_e			curstate;					// current state of face enrollment activity
	ENROLL_FACE_STATE_e			prevstate;					// previous  state of face enrollment activity
	UINT8						curCmdSend;					// current cmd send to IDS
	UINT8						prevCmdSend;				// previous cmd send to IDS
	UINT8						nextCmdSend;				// next cmd send to IDS
	TIMER_HANDLE				abortwaitTmrHandle;			// used to wait for abort face enrollemnt process
	TIMER_HANDLE				stopwaitTmrHandle;			// used to wait for stop face enrollemnt process
    TIMER_HANDLE				terminateTmrHandle; 		// used to wait for terminate face enrollemnt process
    TIMER_HANDLE				gbloblewaitRespTmrHandle; 	// used to wait for overall terminate face enrollemnt process
}ENROLLMENT_FACE_ACTIVITY_t;

typedef struct
{
	ENROLL_STATE_e			state;				// state of enrollment activity
	ENROLL_CARD_STATE_e		cardState;			// state of smart card enrollment
	UINT8					fpCnt;				// enrolled finger print during enrollment mode
	UINT8					cardCnt;			// enrolled card during enrollment mode
	TIMER_HANDLE			waitNextCredEnrollTmrHandle;	// used to wait for next credential enrollment
	TIMER_HANDLE			exitTmrHandle;		// used to wait for user credential to be enroll
    TIMER_HANDLE			exitMobileTmrHandle;   // used to wait for Mobile user credential to be enroll
	TIMER_HANDLE			waitNewEnrollmentTmrHandle; // used to wait for new enrollment
	TIMER_HANDLE			getEnrollInputTmrHandle;	// to get inputs for enrollment
	UINT16					userIdx;			// index of user being enrolled
	UINT64					cardId;				// card id of card being processed for enrollment
	CARD_TYPE_e				cardType;			// card type of card being processed for enrollment
    UINT8                   accessCardSelect;   // either access card 1 or 2? Which needs to be enrolled?
    CHAR                    accessId[ACCESS_ID_SIZE]; // access ID according to selected access card
	READER_ID_e				readerId;			// reader id of door being communicated currently
	UINT8					imageQualityScore;	// image quality score of finger print scanned
	UINT8					scannedFpCnt;		// no. of times finger is scanned for a enrollment
	UINT8					enrolledTemplateIdx;// location in reader module of latest enrolled
	BOOL					sendFc;				// indicate that fc needs to send or not in ACS Enrollment	
	BOOL					fpEnrollmentStartF;// indicate that  fp enrollment is in process
	UINT8					faceCnt;			// enrolled face during enrollment mode
	ENROLLMENT_FACE_ACTIVITY_t	faceEnrollActivity; // face enroll activity
}ENROLLMENT_ACTIVITY_t;

//******** Function Prototypes *******
static void SwitchDcToEnrollmentState(void);

// FSM functions for ENROLLMENT state
static BOOL EnrollFpStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static BOOL EnrollReadOnlyCardStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static BOOL EnrollSplFnCardStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static BOOL EnrollSmartCardStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static BOOL EnrollGetInputStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static BOOL	 EnrollmentAddOrAssignUserFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static BOOL EnrollMobileStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static BOOL EnrollFaceEnrollStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);


//FSM function for Face Enrollment FaceEnrollmentStateStr
static BOOL FaceEnrollmentWait(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static BOOL FaceEnrollmentStart(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static BOOL FaceEnrollmentInprogress(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static BOOL FaceEnrollmentConflict(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static BOOL FaceEnrollmentAbort(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
static BOOL FaceEnrollmentStop(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);

static void ChangeFaceEnrollState(ENROLL_FACE_STATE_e newState);
static void ChangeFaceEnrollStartState(void);
static BOOL SetFaceEnrollAbortWaitTimer(void);
static BOOL SetFaceEnrollStopWaitTimer(void);
static BOOL SetFaceEnrollStopGlobleWaitTimer(void);
// Funcitons for starting/exiting ENROLLMENT
static ENROLLMENT_STATUS_e StartEnrollment(void);
static ENROLLMENT_STATUS_e CheckEnrollmentPsbl(ENROLLMENT_INPUTS_t * inputDataPtr);
static void ExitEnrollmentMode(void);
static void StartEnrollAbrtReq(void);

// Informative functions
static BOOL IsCardWriteRequired(CARD_FIELD_CATEGORY_e category, CARD_WRITE_FIELDS_t * cardFieldsPtr);
static BOOL IsCardWritePsbl(CARD_TYPE_e cardType);
static BOOL IsNextCredToBeEnroll(void);
static BOOL IsDuplicateCard(UINT64 cardId);
static BOOL IsCustomCardWritePsbl(CARD_TYPE_e cardType, CARD_WRITE_FIELDS_t * cardFieldsPtr);

// functions to set timers
static BOOL SetWaitNewEnrollmentTimer(void);
static BOOL SetEnrollmentTimer(void);
static BOOL SetMobileEnrollmentTimer(void);
static BOOL SetWaitNextCredEnrollTimer(void);
static BOOL SetGetEnrollInputTimer(UINT16 timerValue);

// functions used @ timeout
static void EnrollNextCredential(UINT32 dummy);
static void EnrollmentTimeout(UINT32 dummy);
static void MobileEnrollmentTimeout(UINT32 dummy);
static void AskForEnrollmentInput(UINT32 dummy);
static void GetEnrollInputTimeout(UINT32 dummy);

// functions to send specific message to DC
static void SendMsgEnrollByScan(UINT16 userIdx);
static BOOL SendMsgEnrollTemplate(READER_ID_e readerId, UINT16 userIdx,
								UINT8 fpIdx, FP_ENROLL_METHOD_e enrollMethod, ENROLL_SOURCE_e enrollmentSource);
static BOOL SendMsgDeleteSingleUserFp(READER_ID_e readerId, UINT16 userIdx);
static void SendMsgCancelEnrollment(void);
static void SendMsgEraseSmartCard(UINT8 readerId, CARD_TYPE_e cardType);
static BOOL SendMsgWriteDataIntoCard(SLAVE_MSG_ID_e msgId);
static void SendMsgGetUserTemplateCnt(UINT16 userIdx);
static void SendMsgReadUserTemplate(void);
static void BackEnrollmentScreen(void);
static BOOL SendMsgWriteSpclFunCardId(UINT8 splFnIdx);

//thread function
static VOIDPTR CheckUserExistOnServerThreadFn(VOIDPTR dataPtr);
static VOIDPTR AssignUserOnDeviceThreadFn(VOIDPTR dataPtr);
static VOIDPTR AddUserOnServerThreadFn(VOIDPTR dataPtr);

static bool LoadFaceEnrollWaitCountDownTimer();
static void FaceEnrollCountDownStart(UNUSED_PARA UINT32 dummy);
static bool LoadCameraImageWaitCountDownTimer();
static void CameraImageWaitTimeout(UNUSED_PARA UINT32 dummy);

//******** Global Variables **********
UINT8						FpReaderMemFullPort;	// used to log event information
ENROLLMENT_TEMP_INPUTS_t 	EnrollmentTempInputs;
ENROLLMENT_SCREEN_e 		currentEnrollScreen;
BOOL						FpDeletedFromModuleF = FALSE;
BOOL						isCurrentEnrllmentSourceQT = FALSE;
TIMER_HANDLE				FaceEnrollWaitTimerHandler = INVALID_TIMER_HANDLE;
TIMER_HANDLE				CameraImageWaitTimerHandler = INVALID_TIMER_HANDLE;
BOOL EnrollFaceHandlingF = FALSE;
// For logging status of enrollment status
// Reference: ENROLLMENT_STATUS_e

const CHARPTR FaceEnrollmentStateStr[ENROLL_FACE_STATE_END+1] =
{
	"ENROLL_FACE_STATE_WAIT",
	"ENROLL_FACE_STATE_START",
	"ENROLL_FACE_STATE_PROGRESS",
	"ENROLL_FACE_STATE_CONFLICT",
	"ENROLL_FACE_STATE_ABORT",
	"ENROLL_FACE_STATE_STOP",
    "ENROLL_FACE_STATE_END",
};
//******** Static Variables **********
static ENROLLMENT_ACTIVITY_t		enrollActivity;
static ENROLLMENT_INPUTS_t			enrollInputs;
static	UINT32 						enrollUserReferenceId;
static	UINT8						enrollUserfpIdx;
static pthread_t 					CheckUserExistOnServerThreadId=0;
static pthread_t 					AssignUserOnDeviceThreadId=0;
static pthread_t 					AddUserOnServerThreadFnId=0;


// Call FSM state function table
// Entries in this table should match ENROLL_STATE_e enum
static BOOL (*enrollmentFsmFuncPtr[ENROLL_STATE_END])(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length) =
{
	EnrollReadOnlyCardStateFn,
	EnrollSmartCardStateFn,
	EnrollFpStateFn,
	EnrollSplFnCardStateFn,
	EnrollGetInputStateFn,
	EnrollmentAddOrAssignUserFn,
    EnrollMobileStateFn,
	EnrollFaceEnrollStateFn
};

static BOOL (*faceenrollmentFsmFuncPtr[ENROLL_FACE_STATE_END])(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length) =
{
	FaceEnrollmentWait,
	FaceEnrollmentStart,
	FaceEnrollmentInprogress,
	FaceEnrollmentConflict,
	FaceEnrollmentAbort,
	FaceEnrollmentStop,
};

//******** Function Definations ******

//*****************************************************************************
//	InitEnrollmentActivity
//	Param:
//		IN : none
//		OUT: none
//	Returns:
//		none
//	Description:
//		It initializes variables used for enrollment activity.
//
//*****************************************************************************
void InitEnrollmentActivity(void)
{
	// Initialize timer handles
	enrollActivity.exitTmrHandle = INVALID_TIMER_HANDLE;
	enrollActivity.waitNextCredEnrollTmrHandle = INVALID_TIMER_HANDLE;
	enrollActivity.waitNewEnrollmentTmrHandle = INVALID_TIMER_HANDLE;
	enrollActivity.getEnrollInputTmrHandle = INVALID_TIMER_HANDLE;
    enrollActivity.exitMobileTmrHandle = INVALID_TIMER_HANDLE;
	enrollActivity.faceEnrollActivity.abortwaitTmrHandle = INVALID_TIMER_HANDLE;
	enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle = INVALID_TIMER_HANDLE;
	enrollActivity.faceEnrollActivity.stopwaitTmrHandle = INVALID_TIMER_HANDLE;
	enrollActivity.faceEnrollActivity.terminateTmrHandle = INVALID_TIMER_HANDLE;
}

ENROLL_FACE_STATE_e GetCurrentFaceEnrollmentState(void)
{
    return enrollActivity.faceEnrollActivity.curstate;

}
static void ChangeFaceEnrollState(ENROLL_FACE_STATE_e newState)
{
	if(newState == enrollActivity.faceEnrollActivity.curstate)
		return;
    DPRINT(SYS_LOG, "Face Enrollment Previous State: %s, Current State: %s, Next State: %s", FaceEnrollmentStateStr[enrollActivity.faceEnrollActivity.prevstate],
                                                                                            FaceEnrollmentStateStr[enrollActivity.faceEnrollActivity.curstate],
                                                                                            FaceEnrollmentStateStr[newState]);
	
    if(newState != ENROLL_FACE_STATE_PROGRESS)
           DeleteTimer(&CameraImageWaitTimerHandler); //Delete camera image wait timer


    switch (newState) {
    case ENROLL_FACE_STATE_WAIT:
    case ENROLL_FACE_STATE_START:
    case ENROLL_FACE_STATE_PROGRESS:
        if(newState == ENROLL_FACE_STATE_PROGRESS)//|| newState==ENROLL_FACE_STATE_ABORT
        {
            char ProgressBarData[QT_MSG_SIZE] = {'\0'};
            sprintf(ProgressBarData, "%d%c%d%c%s", enrollInputs.faceFrameCnt,
                    UI_MSG_FSP, EnrollmentTempInputs.faceFrameCnt, UI_MSG_FSP, "");
            SendMessageToQt(&ProgressBarData[0], GUI_MSG_FACE_ENROLL_PROCESSING_SCREEN);
        }
        //Play Buzzer wait tone
        if(enrollActivity.faceEnrollActivity.curstate > ENROLL_FACE_STATE_PROGRESS)
            SetBuzLedResp(USER_RESP_FACE_ENROLL_PROCESSING,INTERNAL_PORT);
        break;
    case ENROLL_FACE_STATE_CONFLICT:
    case ENROLL_FACE_STATE_ABORT:
    case ENROLL_FACE_STATE_STOP:
        SetBuzLedResp(USER_RESP_FACE_ENROLL_STOP,INTERNAL_PORT);
        //Stop Buzzer
        break;
    case ENROLL_FACE_STATE_END:
        break;

    default:
        DPRINT(SYS_LOG, " Unhandeled case for face enrollment buzzer");
        break;
    }


    enrollActivity.faceEnrollActivity.prevstate = enrollActivity.faceEnrollActivity.curstate;
	enrollActivity.faceEnrollActivity.curstate = newState;
}

static void ChangeFaceEnrollStartState(void)
{
	ChangeFaceEnrollState(ENROLL_FACE_STATE_START);
}
//*****************************************************************************
//	IsCardWriteRequired
//	Param:
//		IN : category		- category of writable field
//			 cardFieldsPtr	- pointer where writable fields are stored
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if any data to be written into smart card and if so it
//		returns YES else FALSE.
//
//*****************************************************************************
static BOOL IsCardWriteRequired(CARD_FIELD_CATEGORY_e category, CARD_WRITE_FIELDS_t * cardFieldsPtr)
{
	switch(category)
	{
		case CARD_FIELD_ANY:
		{
			if(	(cardFieldsPtr -> addlSecCodeF == SET)		||
				(cardFieldsPtr -> facCodeF == SET)			||
				(cardFieldsPtr -> userIdF == SET)			||
				(cardFieldsPtr -> noOfTemplates)			||
				(cardFieldsPtr -> userNameF == SET)			||
				(cardFieldsPtr -> designationF == SET)		||
				(cardFieldsPtr -> branchF == SET)			||
				(cardFieldsPtr -> departmentF == SET)		||
				(cardFieldsPtr -> bloodGrpF == SET)			||
				(cardFieldsPtr -> emrgncyContactF == SET)	||
				(cardFieldsPtr -> medicalHistoryF == SET) 	||
				(cardFieldsPtr -> cardIdF == SET))
				return YES;
			else
				return NO;
		}

		case CARD_FIELD_PERS_INFO:
		{
			if(	(cardFieldsPtr -> userNameF == SET)			||
				(cardFieldsPtr -> designationF == SET)		||
				(cardFieldsPtr -> branchF == SET)			||
				(cardFieldsPtr -> departmentF == SET)		||
				(cardFieldsPtr -> bloodGrpF == SET)			||
				(cardFieldsPtr -> emrgncyContactF == SET)	||
				(cardFieldsPtr -> medicalHistoryF == SET) )
				return YES;
			else
				return NO;
		}

		case CARD_FIELD_FP:
		{
			if(cardFieldsPtr -> noOfTemplates)
				return YES;
			else
				return NO;
		}

		case CARD_FIELD_SEC_INFO:
		{
			if(	(cardFieldsPtr -> addlSecCodeF == SET) 	||
				(cardFieldsPtr -> facCodeF == SET)		||
				(cardFieldsPtr -> userIdF == SET) )
				return YES;
			else
				return NO;
		}

		case CARD_FIELD_ID:
		{
			if(cardFieldsPtr ->cardIdF == SET)
				return YES;
			else
				return NO;
		}

		default:
			return NO;
	}
}

void ChkUpdateEnrollType(ENROLLMENT_INPUTS_t * inputDataPtr,ENROLLMENT_SOURCE_e SourceType)
{
	if(NULL == inputDataPtr)
		return;

	//TODO:Jigar update	
	switch(SourceType)
	{
		case ENROLLMENT_SOURCE_QT_GUI:
		{
			if(inputDataPtr->enrollmentType == 4)
			{
				inputDataPtr->enrollmentType = ENROLL_FACE_ONLY;
			}
			if(inputDataPtr->enrollmentType == 5)
			{
				inputDataPtr->enrollmentType = ENROLL_DURESS_FP;
			}
			//Internal finger print reader Supported or not(Condition Add To Do Face Enrollment from ARGO CAX Door)
		        if(IsVariant_With_Internal_FP_Rdr() == FALSE)
			{
				if(inputDataPtr->enrollmentType == 2)
				{			
				    inputDataPtr->enrollmentType = ENROLL_FACE_ONLY;
				}
		        }
			isCurrentEnrllmentSourceQT = TRUE;
			break;
		}
		case ENROLLMENT_SOURCE_API:
			if(inputDataPtr->enrollmentType == 5)
			{
				inputDataPtr->enrollmentType = ENROLL_FACE_ONLY;
			}
			isCurrentEnrllmentSourceQT = FALSE;
			break;
			
		case ENROLLMENT_SOURCE_ACMS_GUI: //NOTE: ACMS already send ENROLL_FACE_ONLY value
		case ENROLLMENT_SOURCE_DEVICE_GUI: //NOTE: In Device side handle it its GUI code
			isCurrentEnrllmentSourceQT = FALSE;
		default:
			break;
	}
}
/**
 * @brief ...
 * 
 * @param inputDataPtr ...
 * @return void
 */

BOOL ChkAndDeriveFaceCapturedFrame(ENROLLMENT_INPUTS_t * inputDataPtr)
{
	if(NULL == inputDataPtr)
		return NO;

	if(inputDataPtr->faceCnt > SysAdvPara.maxFacePerUser)
	{
		EPRINT(SYS_LOG, "Not Valid Face Enroll Count Received: [%d] it should be same or less than: [%d]",inputDataPtr->faceCnt, SysAdvPara.maxFacePerUser);
		return NO;
	}
	
	if( (ENROLLMENT_FACECNT_1 <= inputDataPtr->faceCnt) && (ENROLLMENT_FACECNT_10 >= inputDataPtr->faceCnt) )
	{
		inputDataPtr->faceFrameCnt = ENROLLMENT_FACECNT_10;
	}
	else if( (ENROLLMENT_FACECNT_11 <= inputDataPtr->faceCnt) && (ENROLLMENT_FACECNT_20 >= inputDataPtr->faceCnt) )
	{
		inputDataPtr->faceFrameCnt = ENROLLMENT_FACECNT_20;
	}
	else if( (ENROLLMENT_FACECNT_21 <= inputDataPtr->faceCnt) && (ENROLLMENT_FACECNT_30 >= inputDataPtr->faceCnt) )
	{
		inputDataPtr->faceFrameCnt = ENROLLMENT_FACECNT_30;
	}
	else
	{
		EPRINT(SYS_LOG, "Invalid Face Enroll Count Received: [%d]",inputDataPtr->faceCnt);
		//TODO:Error msg that enrollface should not be 0 or minus
		return NO;
	}
	return YES;
}
//*****************************************************************************
//	RequestEnrollment()
//	Param:
//		IN : inputData	- input data to start enrollment
//		OUT: none
//	Returns:
//		enrollment status
//	Description:
//		This function processes enrollment request on door.
//		It validates given inputs and resources on door and starts enrollment.
//
//*****************************************************************************
ENROLLMENT_STATUS_e RequestEnrollment(ENROLLMENT_INPUTS_t * inputDataPtr)
{
	ENROLLMENT_STATUS_e status;

	// No point in going ahead if enrollment is not possible
	if((status = CheckEnrollmentPsbl(inputDataPtr)) != ENROLLMENT_PSBL)
	{
		DPRINT(DC_LOG, "Enrollment not possible: reason: %s", EnrollmentStatusStr[status]);
		return status;
	}
	DPRINT(DC_LOG, "inputDataPtr->userId[%d]", inputDataPtr->userId);
	// Copy input data for further operations
	memcpy(&enrollInputs, inputDataPtr, sizeof(ENROLLMENT_INPUTS_t));
	DcInfo.enrollSource = enrollInputs.source;
	DPRINT(DC_LOG, "enrollInputs->userId[%d]", enrollInputs.userId);
	if(inputDataPtr -> source == SOURCE_SPECIAL_FUNC)
	{
		// Change DC slave FSM state to ENROLLMENT
		SwitchDcToEnrollmentState();
		// Ask for enrollment inputs (user id or spl fn id)
		AskForEnrollmentInput(0);
		return ENROLLMENT_STARTED;
	}
	else
	{
		if( (status = StartEnrollment()) != ENROLLMENT_STARTED)
		{
			DPRINT(DC_LOG, "Enrollment not started: reason: %s", EnrollmentStatusStr[status]);
		}
		return status;
	}
}

//*****************************************************************************
//	CheckEnrollmentPsbl()
//	Param:
//		IN : enrollment inputs	- input data to start enrollment
//		OUT: none
//	Returns:
//		enrollment status (ENROLLMENT_STATUS_e)
//	Description:
//		It checks if enrollment possible on door.
//		It validates inputs and resources available on door.
//		It doesn't validate inputs which depends on user or spl fn.
//
//*****************************************************************************
static ENROLLMENT_STATUS_e CheckEnrollmentPsbl(ENROLLMENT_INPUTS_t * inputDataPtr)
{

	NonIDTRequest_t tmpNonIDTRequestData;
	GetFRNonIDTRequestData(&tmpNonIDTRequestData,NON_IDT_TYPE_FTC);
	// If already any other NonIDT Command is processing, then we don't have to enroll
	if(tmpNonIDTRequestData.isLocallyProcessing == TRUE 
		|| tmpNonIDTRequestData.frProcessRequestStatus == INQUEUE_STATUS_SET)
	{
		EPRINT(DC_LOG, "FR NonIDT operation is going on..FR is busy");
		return DOOR_IS_BUSY;
	}

	// If waiting for user input for special function (which are deactivated at timeout only),
	// do not start enrollment
	if(DcInfo.splFnGetUserInputTmrHandle != INVALID_TIMER_HANDLE)
	{
		return DOOR_IS_BUSY;
	}

	// If door FSM state is not IDLE and source is other than special function
	// then do not start enrollment
    if( ((inputDataPtr -> source != SOURCE_SPECIAL_FUNC) &&
        ( ( DcInfo.curState != DC_IDLE_STATE) && (DcInfo.curState != DC_CAFETERIA_STATE) )) ||
		(selectionInProgress == TRUE) || ( cafeScreenMirrorProcInfo.transactionInitiatedF == TRUE))
	{
        	return DOOR_IS_BUSY;
	}

    //This condition is specific to face enrollment
    //to allow next face enrollment after previous one is successfully completed
    if(inputDataPtr->enrollmentType == ENROLL_FACE_ONLY && inputDataPtr -> source != SOURCE_SPECIAL_FUNC &&
		    DcInfo.lcdIdleTmrHandle != INVALID_TIMER_HANDLE)
      {
                    return DOOR_IS_BUSY;
      }

	// First lets check if we have proper resources on door to start enrollment.
	if(inputDataPtr->category == ENROLL_CATEGORY_USER)
	{
		switch(inputDataPtr->enrollmentType)
		{
			case ENROLL_READ_ONLY_CARD:
			{
				// If card reader is not present at Entry on door
				if(IsReaderPresent(CARD_READER, INTERNAL_PORT, ANY_READER_MODE, NULL) == NO)
				{
					SetAudioVisual(USER_RESP_VERIFY_READER, INTERNAL_PORT, NULL);
					return VERIFY_READER;
				}

				// If no. of cards to be enrolled are more than max user allowed card
				if(inputDataPtr->cardCnt > MAX_USER_CARD)
				{
					SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
					return VERIFY_SELECTION;
				}

				break;
			}

			case ENROLL_SMART_CARD:
			{
				// If Data to be written to card
				if(IsCardWriteRequired(CARD_FIELD_ANY, &(inputDataPtr->cardWriteFields)) == YES)
				{
					// If smart card reader is not present at Entry on door
					if(IsReaderPresent(SMARTCARD_READER, INTERNAL_PORT, ANY_READER_MODE, NULL) == NO)
					{
						SetAudioVisual(USER_RESP_VERIFY_READER, INTERNAL_PORT, NULL);
						return VERIFY_READER;
					}
				}
				else   // No data to be written, verify reader
				{
					// If card reader is not present at Entry on door
					if(IsReaderPresent(CARD_READER, INTERNAL_PORT, ANY_READER_MODE, NULL) == NO)
					{
						SetAudioVisual(USER_RESP_VERIFY_READER, INTERNAL_PORT, NULL);
						return VERIFY_READER;
					}
				}

				// If no. of cards to be enrolled are more than max user allowed card
				if(inputDataPtr->cardCnt > MAX_USER_CARD)
				{
					SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
					return VERIFY_SELECTION;
				}

				break;
			}
			
			case ENROLL_DURESS_FP:
			case ENROLL_BIOMETRIC_ONLY:
			{
				// If finger print reader is not present at Entry on door
				if(IsReaderPresent(FP_READER, INTERNAL_PORT, ANY_READER_MODE, NULL) == NO)
				{
					SetAudioVisual(USER_RESP_VERIFY_READER, INTERNAL_PORT, NULL);
					return VERIFY_READER;
				}
				
				if(inputDataPtr->enrollmentType == ENROLL_DURESS_FP)
				{
				        if(inputDataPtr->fpCnt > MAX_USER_DURESS_FP)
				        {
					      SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
					      return VERIFY_SELECTION;
				        }
				}
				// If no. of finger prints to be enrolled are more than max user allowed FP
				if(inputDataPtr->fpCnt > SysAdvPara.maxFingerPerUser)
				{
					SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
					return VERIFY_SELECTION;
				}

				break;
			}

			case ENROLL_BIOMETRIC_THEN_CARD:
			{
				// If finger print reader is not present at Entry on door
				if(IsReaderPresent(FP_READER, INTERNAL_PORT, ANY_READER_MODE, NULL) == NO)
				{
					SetAudioVisual(USER_RESP_VERIFY_READER, INTERNAL_PORT, NULL);
					return VERIFY_READER;
				}

				// If Data to be written to card
				if(IsCardWriteRequired(CARD_FIELD_ANY, &(inputDataPtr->cardWriteFields)) == YES)
				{
					// If smart card reader is not present at Entry on door
					if(IsReaderPresent(SMARTCARD_READER, INTERNAL_PORT, ANY_READER_MODE, NULL) == NO)
					{
						SetAudioVisual(USER_RESP_VERIFY_READER, INTERNAL_PORT, NULL);
						return VERIFY_READER;
					}
				}
				else   // No data to be written, verify reader
				{
					// If card reader is not present at Entry on door
					if(IsReaderPresent(CARD_READER, INTERNAL_PORT, ANY_READER_MODE, NULL) == NO)
					{
						SetAudioVisual(USER_RESP_VERIFY_READER, INTERNAL_PORT, NULL);
						return VERIFY_READER;
					}
				}

				// If no. of finger prints to be enrolled are more than max user allowed FP
				if(inputDataPtr->fpCnt > SysAdvPara.maxFingerPerUser)
				{
					SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
					return VERIFY_SELECTION;
				}

				// If no. of cards to be enrolled are more than max user allowed card
				if(inputDataPtr->cardCnt > MAX_USER_CARD)
				{
					SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
					return VERIFY_SELECTION;
				}

				break;
			}

            case ENROLL_MOBILE_DEVICE:
            	break;

			case ENROLL_FACE_ONLY:
			{
				if(inputDataPtr->faceCnt > SysAdvPara.maxFacePerUser)
				{
					EPRINT(SYS_LOG, "Not Valid Face Enroll Count Received: [%d] it should be same or less than: [%d]",inputDataPtr->faceCnt, SysAdvPara.maxFacePerUser);
					return CRED_LIMIT_EXCEEDS;
				}

				if(NO == ChkAndDeriveFaceCapturedFrame(inputDataPtr))
				{
					SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
					return VERIFY_SELECTION;
				}
			}
			break;
			
			default:// not expected
			{
				EPRINT(SYS_LOG, "Default VERIFY_SELECTION with Type: [%d]", inputDataPtr->enrollmentType);
				SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
				return VERIFY_SELECTION;
			}
		} // end of switch
	} // end of if user is to be enroll
	else if(inputDataPtr->category == ENROLL_CATEGORY_SP_FN)
	{
		// If card reader is not present at Entry on door
		if(IsReaderPresent(CARD_READER, INTERNAL_PORT, ANY_READER_MODE, NULL) == NO)
		{
			SetAudioVisual(USER_RESP_VERIFY_READER, INTERNAL_PORT, NULL);
			return VERIFY_READER;
		}

		// If no. of cards to be enrolled are more than max special function allowed card
		if(inputDataPtr->cardCnt > MAX_SP_FUNC_CARD)
		{
			SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
			return VERIFY_SELECTION;
		}
	}
	else
	{
		SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
		return VERIFY_SELECTION;
	}

	// Enrollment possble on this door
	return ENROLLMENT_PSBL;
}

//*****************************************************************************
//	StartEnrollment()
//	Param:
//		IN : none
//		OUT: none
//	Returns:
//		enrollment status
//	Description:
//		It validates inputs which depends on user or spl fn.
//		If inputs are valid, it starts enrollment process on door.
// 	NOTE:
//		'enrollInputs[]' should be filled up before calling this function.
//
//*****************************************************************************
static ENROLLMENT_STATUS_e StartEnrollment(void)
{
	// Get finger print reader id if mode is related to finger print enrollment
	if( (enrollInputs.enrollmentType == ENROLL_BIOMETRIC_ONLY) || (enrollInputs.enrollmentType == ENROLL_DURESS_FP) ||
		(enrollInputs.enrollmentType == ENROLL_BIOMETRIC_THEN_CARD) )
	{
		IsReaderPresent(FP_READER, INTERNAL_PORT, ANY_READER_MODE, &enrollActivity.readerId);
	}

	// First lets check if we have proper resources on door to start enrollment.
	if(enrollInputs.category == ENROLL_CATEGORY_USER)
	{
		// If user id is not present in user configuration database,
		// do not start enrollment
		if( (enrollInputs.source == ENROLL_SOURCE_ACMS) ||
			( (enrollInputs.source == SOURCE_SPECIAL_FUNC) && (SysAdvPara.enrollIdType == REF_USER_ID) ) )
		{
			if(IsUserValid(enrollInputs.userId, &(enrollActivity.userIdx) ) == NO)
			{
				SetAudioVisual(USER_RESP_ENROLL_USER_NOT_FOUND, INTERNAL_PORT, NULL);
				return USER_NOT_FOUND;
			}
		}
		else
		{
			// If user id is not present in user configuration database, do not start enrollment
			if(IsAlphaNumIdValid(enrollInputs.alphaNumId, &(enrollActivity.userIdx)) == NO)
			{
				SetAudioVisual(USER_RESP_ENROLL_USER_NOT_FOUND, INTERNAL_PORT, NULL);
				return USER_NOT_FOUND;
			}

			enrollInputs.userId = GetUserId(enrollActivity.userIdx);
		}

		switch(enrollInputs.enrollmentType)
		{
			case ENROLL_READ_ONLY_CARD:
			{
				// If no. of cards to be enrolled are not available for this user
				if(enrollInputs.cardCnt > GetAvailUserCard(enrollActivity.userIdx))
				{
					SetAudioVisual(USER_RESP_CRED_LIMIT_EXCEEDS, INTERNAL_PORT, NULL);
					return CRED_LIMIT_EXCEEDS;
				}

				// All validations are done, let's start enrollment
				// Update display enrollment mode + show card
				SetAudioVisual(USER_RESP_SHOW_CARD, INTERNAL_PORT, NULL);
				// Set enrollment state
				enrollActivity.state = ENROLL_USER_PROX_CARD_STATE;
				break;
			}

			case ENROLL_SMART_CARD:
			{
				if(!(((IsCustomSmartCardConfigEnabled(HID_2K2) == YES) && IsGivenFieldForSmartCardConfigured(HID_2K2, CARD_FIELD_CARD_ID)) ||
						((IsCustomSmartCardConfigEnabled(HID_16K2) == YES) && IsGivenFieldForSmartCardConfigured(HID_16K2, CARD_FIELD_CARD_ID)) ||
						((IsCustomSmartCardConfigEnabled(HID_16K16) == YES) && IsGivenFieldForSmartCardConfigured(HID_16K16, CARD_FIELD_CARD_ID)) ||
						((IsCustomSmartCardConfigEnabled(MIFARE_1K) == YES) && IsGivenFieldForSmartCardConfigured(MIFARE_1K, CARD_FIELD_CARD_ID)) ||
						((IsCustomSmartCardConfigEnabled(MIFARE_4K) == YES) && IsGivenFieldForSmartCardConfigured(MIFARE_4K, CARD_FIELD_CARD_ID))) )
				{
					// If no. of cards to be enrolled are not available for this user
					if(enrollInputs.cardCnt > GetAvailUserCard(enrollActivity.userIdx))
					{
						SetAudioVisual(USER_RESP_CRED_LIMIT_EXCEEDS, INTERNAL_PORT, NULL);
						return CRED_LIMIT_EXCEEDS;
					}
				}
				// If Data to be written to card
				if(IsCardWriteRequired(CARD_FIELD_ANY, &(enrollInputs.cardWriteFields)) == YES)
				{
					// Check all Enroll finger is duress then do not start enrollment    
					if(enrollInputs.cardWriteFields.noOfTemplates >(GetEnrolledUserFp(enrollActivity.userIdx) - (MAX_USER_DURESS_FP - GetAvailDuressUserFp(enrollActivity.userIdx))))
					{
						SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
						return VERIFY_SELECTION;
					  
					}
					
					// If required no. of finger prints are not enrolled, do not start enrollment
					if(enrollInputs.cardWriteFields.noOfTemplates > GetEnrolledUserFp(enrollActivity.userIdx) )
					{
						SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
						return VERIFY_SELECTION;
					}
				}

				// All validations are done, let's start enrollment
				// Update display enrollment mode + show card
				SetAudioVisual(USER_RESP_SHOW_CARD, INTERNAL_PORT, NULL);
				// Set enrollment state
				enrollActivity.state = ENROLL_USER_SMARTCARD_STATE;
				enrollActivity.cardState = PROC_CARD_DETECTED_EV;

				break;
			}
			case ENROLL_DURESS_FP:
			case ENROLL_BIOMETRIC_ONLY:
			{
				if(enrollInputs.enrollmentType == ENROLL_DURESS_FP)
				{
				    if(YES == isUserIsVisitor(enrollActivity.userIdx))
 				    {
 				        SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
 				        return VISITOR_NOT_ENROLL_IN_DURESS;
 				    }
				    
				    if(enrollInputs.fpCnt >  GetAvailDuressUserFp(enrollActivity.userIdx) || (enrollInputs.fpCnt > GetAvailUserFp(enrollActivity.userIdx)))
				    {
					SetAudioVisual(USER_RESP_CRED_LIMIT_EXCEEDS, INTERNAL_PORT, NULL);
					return CRED_LIMIT_EXCEEDS;
				     }
				}
			          
				// If no. of finger prints to be enrolled are not available for this user
				if(enrollInputs.fpCnt > GetAvailUserFp(enrollActivity.userIdx))
				{
					SetAudioVisual(USER_RESP_CRED_LIMIT_EXCEEDS, INTERNAL_PORT, NULL);
					return CRED_LIMIT_EXCEEDS;
				}

				// All validations are done, let's start enrollment
				// Send message to DC to get user's enrolled template count
				SendMsgGetUserTemplateCnt(enrollActivity.userIdx);
				// Update display enrollment mode + place finger
				SetAudioVisual(USER_RESP_PLACE_FINGER, INTERNAL_PORT, NULL);
				enrollActivity.scannedFpCnt = 0;
				enrollActivity.enrolledTemplateIdx = 0;
				// Set enrollment state
				enrollActivity.state = ENROLL_USER_FP_STATE;
				enrollActivity.fpEnrollmentStartF = FALSE;
				break;
			}

			case ENROLL_BIOMETRIC_THEN_CARD:
			{
				if(!(((IsCustomSmartCardConfigEnabled(HID_2K2) == YES) && IsGivenFieldForSmartCardConfigured(HID_2K2, CARD_FIELD_CARD_ID)) ||
						((IsCustomSmartCardConfigEnabled(HID_16K2) == YES) && IsGivenFieldForSmartCardConfigured(HID_16K2, CARD_FIELD_CARD_ID)) ||
						((IsCustomSmartCardConfigEnabled(HID_16K16) == YES) && IsGivenFieldForSmartCardConfigured(HID_16K16, CARD_FIELD_CARD_ID)) ||
						((IsCustomSmartCardConfigEnabled(MIFARE_1K) == YES) && IsGivenFieldForSmartCardConfigured(MIFARE_1K, CARD_FIELD_CARD_ID)) ||
						((IsCustomSmartCardConfigEnabled(MIFARE_4K) == YES) && IsGivenFieldForSmartCardConfigured(MIFARE_4K, CARD_FIELD_CARD_ID))) )
				{
					// If no. of cards to be enrolled are not available for this user
					if(enrollInputs.cardCnt > GetAvailUserCard(enrollActivity.userIdx))
					{
						SetAudioVisual(USER_RESP_CRED_LIMIT_EXCEEDS, INTERNAL_PORT, NULL);
						return CRED_LIMIT_EXCEEDS;
					}
				}
				// If no. of finger prints to be enrolled are not available for this user
				if(enrollInputs.fpCnt > GetAvailUserFp(enrollActivity.userIdx) )
				{
					SetAudioVisual(USER_RESP_CRED_LIMIT_EXCEEDS, INTERNAL_PORT, NULL);
					return CRED_LIMIT_EXCEEDS;
				}

				// If Data to be written to card
				if(IsCardWriteRequired(CARD_FIELD_ANY, &(enrollInputs.cardWriteFields)) == YES)
				{
					// If required no. of finger print templates won't be available to write them into card,
					// do not start enrollment
					if(enrollInputs.cardWriteFields.noOfTemplates >
						(enrollInputs.fpCnt + GetEnrolledUserFp(enrollActivity.userIdx) ) )
					{
						SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
						return VERIFY_SELECTION;
					}
				}

				// All validations are done, let's start enrollment
				// Send message to DC to get user's enrolled template count
				SendMsgGetUserTemplateCnt(enrollActivity.userIdx);
				// Update display enrollment mode + place finger
				SetAudioVisual(USER_RESP_PLACE_FINGER, INTERNAL_PORT, NULL);
				enrollActivity.scannedFpCnt = 0;
				enrollActivity.enrolledTemplateIdx = 0;
				// Set enrollment state
				enrollActivity.state = ENROLL_USER_FP_STATE;
				enrollActivity.fpEnrollmentStartF = FALSE;
				break;
			}

			case ENROLL_MOBILE_DEVICE:
			{
				SetAudioVisual(USER_RESP_ACS_ENROLLMENT_START_ON_DEVICE, INTERNAL_PORT, NULL);
				// Start enrollment timer
				if(SetMobileEnrollmentTimer() == FAIL)
				{
					EPRINT(DC_LOG, "Unable to set BLE Enrollment Timer");
					SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
					return TIMER_FAILED;
				}

				snprintf(enrollActivity.accessId, ACCESS_ID_SIZE,"%s", enrollInputs.accessId);
				enrollActivity.accessCardSelect = enrollInputs.accessCardSelect;
				enrollActivity.sendFc = enrollInputs.cardWriteFields.facCodeF;
				enrollActivity.state = ENROLL_USER_MOBILE_STATE;
				// Change DC slave FSM state to ENROLLMENT
				SwitchDcToEnrollmentState();

				return ENROLLMENT_STARTED;
			}

	    		case ENROLL_FACE_ONLY:
			{
				if(FRServerConfig.enableFrServer != TRUE)
				{
					EPRINT(DC_LOG, "Fr server Not Enabled");
					char FaliStr[QT_MSG_SIZE] = {'\0'};
					strcpy(FaliStr, GetDisplayMessage(FALSE,
								NULL_CHAR, MSG_FR_NOT_ENABLED_1069));
					SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
					// Exit enrollment mode
					ExitEnrollmentMode();
					return FAIL;
				}

				if(FAIL == StartFRStreamingForFaceEnrlOnDeviceScreen())
                {
					// Exit enrollment mode
					ExitEnrollmentMode();
					return FAILED_TO_CONNECT;
				}
                SendMessageToQt(&FRStreamDisplayF, GUI_MSG_FR_STREAM_STATUS);
				// Set enrollment state
				enrollActivity.state = ENROLL_USER_FACE_STATE;

				enrollActivity.fpEnrollmentStartF = FALSE;
                enrollActivity.faceEnrollActivity.curstate = ENROLL_FACE_STATE_WAIT;
                enrollActivity.faceEnrollActivity.prevstate = ENROLL_FACE_STATE_WAIT;
//              ChangeFaceEnrollState(ENROLL_FACE_STATE_WAIT);
                SetFaceEnrollStopGlobleWaitTimer();
                LoadFaceEnrollWaitCountDownTimer();
				DPRINT(SYS_LOG, "Face enroll is in Wait State");
			}
			break;

			default:// not expected
			{
				SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
				return VERIFY_SELECTION;
			}
		} // end of switch
	} // end of if user is to be enroll
	else if(enrollInputs.category == ENROLL_CATEGORY_SP_FN)
	{
		// If special function id is invalid, do not start enrollment
		if(enrollInputs.splFnIdx >= MAX_SPECIAL_FUNC)
		{
			SetAudioVisual(USER_RESP_SPL_FN_INVALID, INTERNAL_PORT, NULL);
			return SPL_FN_NOT_FOUND;
		}

		if(!(((IsCustomSmartCardConfigEnabled(HID_2K2) == YES) && IsGivenFieldForSmartCardConfigured(HID_2K2, CARD_FIELD_CARD_ID)) ||
				((IsCustomSmartCardConfigEnabled(HID_16K2) == YES) && IsGivenFieldForSmartCardConfigured(HID_16K2, CARD_FIELD_CARD_ID)) ||
				((IsCustomSmartCardConfigEnabled(HID_16K16) == YES) && IsGivenFieldForSmartCardConfigured(HID_16K16, CARD_FIELD_CARD_ID)) ||
				((IsCustomSmartCardConfigEnabled(MIFARE_1K) == YES) && IsGivenFieldForSmartCardConfigured(MIFARE_1K, CARD_FIELD_CARD_ID)) ||
				((IsCustomSmartCardConfigEnabled(MIFARE_4K) == YES) && IsGivenFieldForSmartCardConfigured(MIFARE_4K, CARD_FIELD_CARD_ID))) )
		{
			// If no. of cards to be enrolled are not available for this special function
			if(enrollInputs.cardCnt > GetAvailSplFnCard(enrollInputs.splFnIdx))
			{
				SetAudioVisual(USER_RESP_CRED_LIMIT_EXCEEDS, INTERNAL_PORT, NULL);
				return CRED_LIMIT_EXCEEDS;
			}
		}

		// All validations are done, let's start enrollment
		enrollActivity.state = ENROLL_SPL_FN_CARD_STATE;
		// Update display enrollment mode + show card
		SetAudioVisual(USER_RESP_SHOW_CARD, INTERNAL_PORT, NULL);
	}
	else
	{
		SetAudioVisual(USER_RESP_VERIFY_SELECTION, INTERNAL_PORT, NULL);
		return VERIFY_SELECTION;
	}

	// All validations are done, now trigger enrollment process on door.

	// Initialize enrollment activity related variables
	enrollActivity.fpCnt = 0;
	enrollActivity.cardCnt = 0;
	enrollActivity.faceCnt = 0;

	if(enrollInputs.enrollmentType != ENROLL_FACE_ONLY)
	{
		// Start enrollment timer
		if(SetEnrollmentTimer() == FAIL)
		{
			EPRINT(DC_LOG, "Unable to set Enrollment Timer");
		}
	}

	// Change DC slave FSM state to ENROLLMENT
	SwitchDcToEnrollmentState();

	return ENROLLMENT_STARTED;
}

static bool LoadCameraImageWaitCountDownTimer()
{
	TIMER_INFO_t timer;

	if(CameraImageWaitTimerHandler == INVALID_TIMER_HANDLE)
	{
		timer.count   = 5;
		timer.data 	  = 0;
		timer.funcPtr = &CameraImageWaitTimeout;

		return(StartTimer(timer, &CameraImageWaitTimerHandler, __FUNCTION__) );
	}
	else
	{
		return(ReloadTimer(CameraImageWaitTimerHandler, ((MAX_FR_RESP_WAIT_TIME+1)*10) ) );
	}
}

static void CameraImageWaitTimeout(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&CameraImageWaitTimerHandler);
	FACE_ENROLL_REQ_t tObj;
	tObj.requestType = USR_ENR_ENROLL_IMAGE;
	tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
	tObj.numberOfEnrollFace = enrollInputs.faceCnt;
    tObj.UserId = enrollInputs.userId;
	ProcessEnrFaceRequest(&tObj);
}

static bool LoadFaceEnrollWaitCountDownTimer()
{
	TIMER_INFO_t timer;

	if(FaceEnrollWaitTimerHandler == INVALID_TIMER_HANDLE)
	{
        timer.count   = 3;
		timer.data 	  = 0;
		timer.funcPtr = &FaceEnrollCountDownStart;

		return(StartTimer(timer, &FaceEnrollWaitTimerHandler, __FUNCTION__) );
	}

}

static void FaceEnrollCountDownStart(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&FaceEnrollWaitTimerHandler);
	SetAudioVisual(USER_RESP_FACE_ENROLL_WAITING, INTERNAL_PORT, NULL);
}

//*****************************************************************************
//	AskForEnrollmentInput()
//	Param:
//		IN : none
//		OUT: none
//	Returns:
//		none
//	Description:
//		It asks for user input (spl fn id or user id).
//
//*****************************************************************************
static void AskForEnrollmentInput(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&enrollActivity.waitNewEnrollmentTmrHandle);

	// Ask for user input
	SetAudioVisual(USER_RESP_GET_USER_INPUT, INTERNAL_PORT, NULL);

	// Start timer to get input within enrollment time
	if(SetGetEnrollInputTimer(ENROLLMENT_TIMER) == FAIL)
	{
		EPRINT(DC_LOG, "Unable to set 'get inputs for enrollment' timer");
	}

	// Set enrollment state
	enrollActivity.state = ENROLL_GET_INPUT_STATE;
}

//*****************************************************************************
//	EnrollmentTimeout
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			This function handles timeout for enrollment mode exit.
//
//*****************************************************************************
static void EnrollmentTimeout(UNUSED_PARA UINT32 dummy)
{
	// Send message to DC to display enrollment is timeout
    SetAudioVisual(USER_RESP_ENROLL_TIMEOUT, INTERNAL_PORT, NULL);
	ExitEnrollmentMode();
}

//*****************************************************************************
//	MobileEnrollmentTimeout
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			This function handles timeout for BLE enrollment mode exit.
//
//*****************************************************************************
static void MobileEnrollmentTimeout(UNUSED_PARA UINT32 dummy)
{
    ProcMsg(MSG_MOBILE_ENROLLMENT_TIMEOUT_EVENT, NULL, 0);
}

//*****************************************************************************
//	GetEnrollInputTimeout
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			This function handles timeout for getting enrollment input.
//
//*****************************************************************************
static void GetEnrollInputTimeout(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&enrollActivity.getEnrollInputTmrHandle);
	// Send message to DC to display enrollment is timeout
	SetAudioVisual(USER_RESP_ENROLL_TIMEOUT, INTERNAL_PORT, NULL);
	// Change DC slave FSM state to IDLE state
	SwitchDcToIdleState();
}

//*****************************************************************************
//	IsNextCredToBeEnroll
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	YES if next credntial is to be enroll otherwise /NO
//	Description:
//				Checks if next credential is to be enroll.
//
//*****************************************************************************
static BOOL IsNextCredToBeEnroll(void)
{
	if(enrollInputs.category == ENROLL_CATEGORY_USER)
	{
		switch(enrollInputs.enrollmentType)
		{
			case ENROLL_READ_ONLY_CARD:
			case ENROLL_SMART_CARD:
			{
				// Check next card to be enroll
				if(enrollActivity.cardCnt < enrollInputs.cardCnt)
					return YES;
				break;
			}

			case ENROLL_BIOMETRIC_THEN_CARD:
			{
				// Check next finger to be enroll
				if(enrollActivity.fpCnt < enrollInputs.fpCnt)
					return YES;
				// Check next card to be enroll
				else if(enrollActivity.cardCnt < enrollInputs.cardCnt)
					return YES;
				break;
			}
			case ENROLL_DURESS_FP:
			case ENROLL_BIOMETRIC_ONLY:
			{
				// Check next finger to be enroll
				if(enrollActivity.fpCnt < enrollInputs.fpCnt)
					return YES;
				break;
			}
			
			case ENROLL_FACE_ONLY: //TODO Jigar : logic pending : may be not needed
			{
				// Check next face to be enroll
				if(enrollActivity.faceCnt < enrollInputs.faceFrameCnt)
					return YES;
				break;
			}

			default:
			{
				break;
			}
		}
	}
	else if(enrollInputs.category == ENROLL_CATEGORY_SP_FN)
	{
		// Check next card to be enroll
		if(enrollActivity.cardCnt < enrollInputs.cardCnt)
			return YES;
	}

	return NO;
}

//*****************************************************************************
//	EnrollNextCredential
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			This function asks for next credential enrollment.

//			It should be called after 1st credential is enrolled successfully
//			and more credentials are to be enrolled.
//
//*****************************************************************************
static void EnrollNextCredential(UNUSED_PARA UINT32 dummy)
{
	//TODO Jigar : logic pending : may be not needed
	//Delete timer
	DeleteTimer(&enrollActivity.waitNextCredEnrollTmrHandle);
	
	if(enrollInputs.enrollmentType != ENROLL_FACE_ONLY)
	{
		// Start enrollment timer
		if(SetEnrollmentTimer() == FAIL)
		{
			EPRINT(DC_LOG, "Unable to set Enrollment Timer");
		}
	}

	if(enrollInputs.category == ENROLL_CATEGORY_USER)
	{
		switch(enrollInputs.enrollmentType)
		{
			case ENROLL_READ_ONLY_CARD:
			{
				//Send message to DC to display "NEXT CARD"
				SetAudioVisual(USER_RESP_NEXT_CARD, INTERNAL_PORT, NULL);

				// Set enrollment state
				enrollActivity.state = ENROLL_USER_PROX_CARD_STATE;
				break;
			}

			case ENROLL_SMART_CARD:
			{
				//Send message to DC to display "NEXT CARD"
				SetAudioVisual(USER_RESP_NEXT_CARD, INTERNAL_PORT, NULL);

				//Change enrollment state to ENROLL_USER_SMARTCARD
				enrollActivity.state = ENROLL_USER_SMARTCARD_STATE;
				enrollActivity.cardState = PROC_CARD_DETECTED_EV;
				break;
			}

			case ENROLL_BIOMETRIC_THEN_CARD:
			{
				// Check next finger to be enroll
				if(enrollActivity.fpCnt < enrollInputs.fpCnt)
				{
					// Send message to DC to get user's enrolled template count
					SendMsgGetUserTemplateCnt(enrollActivity.userIdx);
					//Send message to DC to display "NEXT FINGER"
					SetAudioVisual(USER_RESP_NEXT_FINGER, INTERNAL_PORT, NULL);
					enrollActivity.scannedFpCnt = 0;
					enrollActivity.enrolledTemplateIdx = 0;
					enrollActivity.fpEnrollmentStartF = FALSE;

					// Set enrollment state
					enrollActivity.state = ENROLL_USER_FP_STATE;
				}
				// Check next card to be enroll
				else if( (enrollActivity.cardCnt < enrollInputs.cardCnt) &&
							(enrollActivity.cardCnt < MAX_USER_CARD) )
				{
					//Send message to DC to display next credential
					if(enrollActivity.cardCnt == 0)
					{
						SetAudioVisual(USER_RESP_SHOW_CARD, INTERNAL_PORT, NULL);
					}
					else
					{
						SetAudioVisual(USER_RESP_NEXT_CARD, INTERNAL_PORT, NULL);
					}

					//Change enrollment state to ENROLL_USER_SMARTCARD
					enrollActivity.state = ENROLL_USER_SMARTCARD_STATE;
					enrollActivity.cardState = PROC_CARD_DETECTED_EV;
				}
				break;
			}
			
			case ENROLL_DURESS_FP:
			case ENROLL_BIOMETRIC_ONLY:
			{
				// Send message to DC to get user's enrolled template count
				SendMsgGetUserTemplateCnt(enrollActivity.userIdx);
				//Send message to DC to display "NEXT FINGER"
				SetAudioVisual(USER_RESP_NEXT_FINGER, INTERNAL_PORT, NULL);
				enrollActivity.scannedFpCnt = 0;
				enrollActivity.enrolledTemplateIdx = 0;

				// Set enrollment state
				enrollActivity.state = ENROLL_USER_FP_STATE;
				enrollActivity.fpEnrollmentStartF = FALSE;
				break;
			}
			case ENROLL_FACE_ONLY:
			{
				if(enrollActivity.faceCnt < enrollInputs.faceCnt)
				{
					//TODO: add logic to send progress bar info and etc to QT
				}
				//Change enrollment state to ENROLL_USER_FACE_STATE
				enrollActivity.scannedFpCnt = 0;
				enrollActivity.enrolledTemplateIdx = 0;
				enrollActivity.fpEnrollmentStartF = FALSE;
				enrollActivity.state = ENROLL_USER_FACE_STATE;
				break;
			}

			default:
				break;
		}
	}
	else if(enrollInputs.category == ENROLL_CATEGORY_SP_FN)
	{
		// Send message to DC to display "NEXT CARD"
		SetAudioVisual(USER_RESP_NEXT_CARD, INTERNAL_PORT, NULL);

		// Set enrollment state
		enrollActivity.state = ENROLL_SPL_FN_CARD_STATE;
	}
}

//*****************************************************************************
//	SetEnrollmentTimer
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function starts or reloads timer to wait for exiting
//			enrollment for given door id
//
//*****************************************************************************
static BOOL SetEnrollmentTimer(void)
{
	if(enrollActivity.exitTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = (ENROLLMENT_TIMER * 10 * TIMER_100MS);
		tInfo.data = 0;
		tInfo.funcPtr = &EnrollmentTimeout;
		return (StartTimer(tInfo, &enrollActivity.exitTmrHandle, __FUNCTION__));
	}
	else //reload timer
	{
		return (ReloadTimer(enrollActivity.exitTmrHandle, (ENROLLMENT_TIMER * 10 * TIMER_100MS)));
	}
}

//*****************************************************************************
//	SetMobileEnrollmentTimer
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function starts or reloads timer to wait for exiting
//			BLE enrollment for given door id
//
//*****************************************************************************
static BOOL SetMobileEnrollmentTimer(void)
{
    if(enrollActivity.exitMobileTmrHandle == INVALID_TIMER_HANDLE)
    {
        TIMER_INFO_t tInfo;

        tInfo.count = (BLE_ENROLLMENT_TIMER * 10 * TIMER_100MS);
        tInfo.data = 0;
        tInfo.funcPtr = &MobileEnrollmentTimeout;
        return (StartTimer(tInfo, &enrollActivity.exitMobileTmrHandle, __FUNCTION__));
    }
    else //reload timer
    {
        return (ReloadTimer(enrollActivity.exitMobileTmrHandle, (BLE_ENROLLMENT_TIMER * 10 * TIMER_100MS)));
    }
}
//*****************************************************************************
//	SetWaitNewEnrollmentTimer
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function starts or reloads timer for new enrollment.
//
//*****************************************************************************
static BOOL SetWaitNewEnrollmentTimer(void)
{
	if(enrollActivity.waitNewEnrollmentTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = (ENROLLMENT_USER_RESP_TIMER * 10 * TIMER_100MS);
		tInfo.data = 0;
		tInfo.funcPtr = &AskForEnrollmentInput;
		return (StartTimer(tInfo, &enrollActivity.waitNewEnrollmentTmrHandle, __FUNCTION__));
	}
	else //reload timer
	{
		return (ReloadTimer(enrollActivity.waitNewEnrollmentTmrHandle, (ENROLLMENT_USER_RESP_TIMER * 10 * TIMER_100MS)));
	}
}

//*****************************************************************************
//	SetWaitNextCredEnrollTimer
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function starts or reloads timer to wait for enrolling next
//			credential.
//
//*****************************************************************************
static BOOL SetWaitNextCredEnrollTimer(void)
{
	if(enrollActivity.waitNextCredEnrollTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = (ENROLLMENT_USER_RESP_TIMER * 10 * TIMER_100MS);
		tInfo.data = 0;
		tInfo.funcPtr = &EnrollNextCredential;
		return (StartTimer(tInfo, &enrollActivity.waitNextCredEnrollTmrHandle, __FUNCTION__));
	}
	else //reload timer
	{
		return (ReloadTimer(enrollActivity.waitNextCredEnrollTmrHandle, (ENROLLMENT_USER_RESP_TIMER * 10 * TIMER_100MS)));
	}
}

//*****************************************************************************
//	SetGetEnrollInputTimer
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function starts or reloads timer to get inputs (user id or
//			spl fn id)for enrollment.
//
//*****************************************************************************
static BOOL SetGetEnrollInputTimer(UINT16 timerValue)
{
	if(enrollActivity.getEnrollInputTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = (timerValue * 10 * TIMER_100MS);
		tInfo.data = 0;
		tInfo.funcPtr = &GetEnrollInputTimeout;
		return (StartTimer(tInfo, &enrollActivity.getEnrollInputTmrHandle, __FUNCTION__));
	}
	else //reload timer
	{
		return (ReloadTimer(enrollActivity.getEnrollInputTmrHandle, (timerValue * 10 * TIMER_100MS)));
	}
}

static BOOL SetFaceEnrollAbortWaitTimer(void)
{
	if(enrollActivity.faceEnrollActivity.abortwaitTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = (FACE_ENROLL_ABORT_STOP_USRINPUT_WAIT_TMR * 10 * TIMER_100MS);
		tInfo.data = 0;
		tInfo.funcPtr = &EnrollmentTimeout;
		return (StartTimer(tInfo, &enrollActivity.faceEnrollActivity.abortwaitTmrHandle, __FUNCTION__));
	}
	else //reload timer
	{
		return (ReloadTimer(enrollActivity.faceEnrollActivity.abortwaitTmrHandle, (FACE_ENROLL_ABORT_STOP_USRINPUT_WAIT_TMR * 10 * TIMER_100MS)));
	}
}

static BOOL SetFaceEnrollStopWaitTimer(void)
{
	if(enrollActivity.faceEnrollActivity.stopwaitTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = (FACE_ENROLL_ABORT_STOP_USRINPUT_WAIT_TMR * 10 * TIMER_100MS);
		tInfo.data = 0;
		tInfo.funcPtr = &EnrollmentTimeout;
		return (StartTimer(tInfo, &enrollActivity.faceEnrollActivity.stopwaitTmrHandle, __FUNCTION__));
	}
	else //reload timer
	{
		return (ReloadTimer(enrollActivity.faceEnrollActivity.stopwaitTmrHandle, (FACE_ENROLL_ABORT_STOP_USRINPUT_WAIT_TMR * 10 * TIMER_100MS)));
	}
}

static BOOL SetFaceEnrollStopGlobleWaitTimer(void)
{
	if(enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = (FACE_ENROLL_GLOBLE_WAIT_TMR * 10 * TIMER_100MS);
		tInfo.data = 0;
		tInfo.funcPtr = &EnrollmentTimeout;
		return (StartTimer(tInfo, &enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle, __FUNCTION__));
	}
}

//*****************************************************************************
//	EnrollFpStateFn
//	Param:
//		IN :	msgId 	- Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length 	- Length of the message
//		OUT:	none
//	Returns:	TRUE if message processed, Else FALSE
//	Description:
//			It handles finger print enrollment process.
//
//*****************************************************************************
static BOOL EnrollFpStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UNUSED_PARA UINT16 length)
{
	if(msgId == MSG_USER_TEMPLATE_CNT_RESP)
	{
		USER_TEMPLATE_CNT_RESP_DATA_t * userFpCntRespPtr = dataPtr;

		if(userFpCntRespPtr -> status == READER_RESP_STATUS_SUCCESS)
		{
			// Store no. of enrolled template as index to point out next template
			// enrollment position
			enrollActivity.enrolledTemplateIdx = userFpCntRespPtr -> templateCnt;

			// Send message to DC to "enroll finger by scan"
			SendMsgEnrollByScan(enrollActivity.userIdx);
		}

		return TRUE;// message is processed
	}
	else if(msgId == MSG_READ_TEMPLATE_RESP)
	{
		READ_TEMPLATE_RESP_DATA_t * readFpRespPtr = dataPtr;
		UINT8 		fpIdx;
		EV_FIELD2	evField2;
		EV_FIELD3	evField3;
		EV_FIELD4	evField4;
		BOOL		fpWrittenF = FALSE;
		READER_ID_e	readerId;

		DPRINT(DC_LOG, "Rcvd MSG_READ_TEMPLATE_RESP, status = %s", ReaderRespStatusStr[readFpRespPtr -> status]);

		if(readFpRespPtr -> status == READER_RESP_STATUS_SUCCESS)
		{
			// Store received finger template
			for(fpIdx = 0; (fpIdx < SysAdvPara.maxFingerPerUser); fpIdx++)
			{
				// If file not present for this user & finger index, write it
				if(IsUserFpPresent(enrollActivity.userIdx, fpIdx) == NO)
				{
					if(WriteUserFpIntoFile(enrollActivity.userIdx, fpIdx, readFpRespPtr -> template) == FILE_WRITE_SUCCESS)
					{
						fpWrittenF = TRUE;
					}
					break;
				}
			}

			if(fpWrittenF == FALSE)
			{
				EPRINT(DC_LOG, "Failed to write finger print into file for userIdx:%d ,fpIdx:%d ", (enrollActivity.userIdx+1), fpIdx);

				//Send message to DC to display "ENROLLMENT FAILED"
				SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);

				ExitEnrollmentMode();

				return TRUE;
			}

			//Send message to DC to display enrollment is successful
			SetAudioVisual(USER_RESP_FP_ENROLLED, INTERNAL_PORT, &enrollActivity.imageQualityScore);

			// Log event- user finger index xx is enrolled @ location xx
			// NOTE: filed 3 is BCD. First digit is 'fpIdx' and 2nd is 'location'.
			evField3 = (fpIdx * 10) + enrollActivity.enrolledTemplateIdx;
			if(SysBasicPara.fprTemplateFormat == ISO_19794_FORMAT)
			{
			            if(enrollInputs.enrollmentType ==  ENROLL_DURESS_FP)
				  evField2 = USER_DURESS_FP_ISO_FORMAT;
			            else
				evField2 = USER_FP_ISO_FORMAT;
			}
			else
			{
			            if(enrollInputs.enrollmentType ==  ENROLL_DURESS_FP)
				  evField2 = USER_DURESS_FP;
			            else
				evField2 = USER_FP;
			}
			evField4 = enrollActivity.imageQualityScore;

			WriteEventLog(ENROLLMENT, GetUserId(enrollActivity.userIdx), evField2, evField3, evField4, UNUSED_FIELD);
			enrollActivity.fpCnt++;

			if(ServerModeCfg.serverMode == COSEC_THIRD_PARTY && EnrollmentCmdRunningF == TRUE)
			{
				AddCredentialInThirdPartyBuffer((UINT8)PUSH_API_FINGER_CRED, (UINT64)INVALID_CARD_ID, &(readFpRespPtr -> template[0]) );
			}
			//Delete enrollment timer
			DeleteTimer(&enrollActivity.exitTmrHandle);

			// Enroll user finger print on other finger print readers
			for(readerId = 0; readerId < MAX_READERS; readerId++)
			{
				if(readerId != enrollActivity.readerId)
				{
					if(SysBasicPara.reader[readerId].type == SUP_FP_3030_READER)
					{
						SendMsgEnrollTemplate(readerId, enrollActivity.userIdx, fpIdx, ENROLL_METHOD_ADD_NEW, enrollInputs.source);
					}
				}
			}

			// At this stage finger enrollment is done so reset the progress Flag
			enrollActivity.fpEnrollmentStartF = FALSE;

			if(IsNextCredToBeEnroll() == YES)
			{
				// Set time to trigger next credential enrollment
				if(SetWaitNextCredEnrollTimer() == FAIL)
				{
					EPRINT(DC_LOG, "Unable to set Next Enrollment Timer");
				}
			}
			else
			{
				ExitEnrollmentMode();
			}
		}
		else // status received is fail
		{
			EPRINT(DC_LOG, "Can't read template from FPR");

			//Send message to DC to display "ENROLLMENT FAILED"
			SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);

			if(SysAdvPara.saveFpImageF == ENABLED)
			{
				INT32 fpImgfd=0;
				CHAR filepath[SAVE_FP_IMAGE_FILE_PATH_SIZE];
				sprintf(filepath, "%s%u_%d", SYS_USER_FP_IMG_FILE_PATH, enrollActivity.userIdx,CurEnrlInfo.fpIdx);
				fpImgfd	= open(filepath,O_RDONLY | O_SYNC , S_IRWXU | S_IRWXG | S_IRWXO);
				if(fpImgfd< 0)
				{
					BOOL retVal;
					SAVE_FP_IMAGE_ERROR_CODE_e  saveFpimageErrorCode;
					retVal = SearchEntryFromRecordFile(enrollActivity.userIdx,CurEnrlInfo.fpIdx, &saveFpimageErrorCode);
					if(YES == retVal)
					{
						DeleteEntryFromRecordFile(enrollActivity.userIdx,CurEnrlInfo.fpIdx);
					}
				}
				else
				{
					EPRINT(SYS_LOG, "Can't read template from module so deleting fpImage file");
					DeleteSingleUserFingerImage(enrollActivity.userIdx,CurEnrlInfo.fpIdx);
				}
			}
			ExitEnrollmentMode();
		}
		return TRUE;// message is processed
	}
	else if(msgId == MSG_READ_SFPR_IMAGE_CMD)
	{
			SendMsgReadUserTemplate();
			return TRUE;
	}
	else if(msgId == MSG_ENROLL_BY_SCAN_RESP)
	{
		ENROLL_BY_SCAN_RESP_DATA_t * enrollFpRespPtr = dataPtr;

		DPRINT(DC_LOG, "Rcvd MSG_ENROLL_BY_SCAN_RESP, status = %s", ReaderRespStatusStr[enrollFpRespPtr -> status]);
		switch(enrollFpRespPtr -> status)
		{
			case READER_RESP_STATUS_SCAN_SUCCESS:
			{
				// Finger Enrollment started
				enrollActivity.fpEnrollmentStartF = TRUE;
				// Send message to DC to display "REMOVE FINGER"
				SetAudioVisual(USER_RESP_REMOVE_FINGER, INTERNAL_PORT, NULL);

				// Reload exit enrollment timer
				if(SetEnrollmentTimer() == FAIL)
				{
					EPRINT(DC_LOG, "Unable to set Enrollment Timer");
				}
				break;
			}

			case READER_RESP_STATUS_SUCCESS:
			{
				// Reload exit enrollment timer
				if(SetEnrollmentTimer() == FAIL)
				{
					EPRINT(DC_LOG, "Unable to set Enrollment Timer");
				}

				if( (SysAdvPara.templatesPerFinger == TWO_TEMPLATES) && (enrollActivity.scannedFpCnt == 0) )
				{
					enrollActivity.scannedFpCnt = 1;
					// Send message to DC to display "PLACE FINGER AGAIN"
					SetAudioVisual(USER_RESP_PLACE_FINGER_AGAIN, INTERNAL_PORT, NULL);
					// Store image quality that is to be displayed after FP template is received from DC
					enrollActivity.imageQualityScore = enrollFpRespPtr -> imageQualityScore;
				}
				else
				{
					if(SysAdvPara.templatesPerFinger == ONE_TEMPLATE)
					{
						enrollActivity.imageQualityScore = enrollFpRespPtr -> imageQualityScore;
					}
					// If second scan is also done and image quality is better
					// than  first scan then increment location of template to
					// point out best quality template
					else if( (enrollActivity.scannedFpCnt == 1) && (enrollFpRespPtr -> imageQualityScore > enrollActivity.imageQualityScore) )
					{
						enrollActivity.enrolledTemplateIdx++;
						enrollActivity.imageQualityScore = enrollFpRespPtr -> imageQualityScore;
					}

					enrollActivity.scannedFpCnt = 0;
					
					UINT8 fpIdx=0;
					for(; (fpIdx < SysAdvPara.maxFingerPerUser); fpIdx++)
					{
						// Find fpIdx for image name creation
						if(IsUserFpPresent(enrollActivity.userIdx, fpIdx) == NO)
						{
							fpIdx++;
							break;
						}
					}
					CurEnrlInfo.fpIdx = fpIdx;
					CurEnrlInfo.userId = enrollInputs.userId;
					isCmdEnrollDuressThenStoreFpIdx(enrollActivity.userIdx,((CurEnrlInfo.fpIdx) - 1));
					
					if(SysAdvPara.saveFpImageF == ENABLED)
					{
						SendReadImageCmd(enrollActivity.readerId);
					}
					else
					{
						SendMsgReadUserTemplate();
					}
				}

				break;
			}

			case READER_RESP_STATUS_MEM_FULL:
			{
				DcInfo.internalFpMemFullF = TRUE;
				// Send message to DC to display "FP CAPACITY FULL"
				SetAudioVisual(USER_RESP_FP_MEM_FULL, INTERNAL_PORT, NULL);
				ExitEnrollmentMode();
				break;
			}

			case READER_RESP_STATUS_EXIST_FINGER:
			{
				DPRINT(DC_LOG, "Enrollment failed, reason: %s", EnrollmentStatusStr[DUPL_CREDENTIAL]);
				// Send message to DC to display "DUPLICATE FINGER"
				SetAudioVisual(USER_RESP_DUPL_USER_FINGER, INTERNAL_PORT, NULL);
				ExitEnrollmentMode();
				break;
			}

			case READER_RESP_STATUS_TRY_AGAIN:
			{
				DeleteTimer(&enrollActivity.exitTmrHandle);
				SetAudioVisual(USER_RESP_ENROLL_TRY_AGAIN_FINGER, INTERNAL_PORT, NULL);
				SetWaitNextCredEnrollTimer();
				break;
			}

			default:
			{
				// Send message to DC to display "ENROLLMENT FAILED"
				SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
				ExitEnrollmentMode();
				break;
			}
		}
		return TRUE;// message is processed
	}
	else if(msgId == GUI_BUTTON_HOME)
	{
		if(enrollActivity.fpEnrollmentStartF)
			return TRUE;
	}

	return FALSE;// message is not to be processed here
}

//*****************************************************************************
//	EnrollReadOnlyCardStateFn
//	Param:
//		IN :	msgId 	- Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length 	- Length of the message
//		OUT:	none
//	Returns:	TRUE if message processed, Else FALSE
//	Description:
//			It handles read only card enrollment process.
//
//*****************************************************************************
static BOOL EnrollReadOnlyCardStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UNUSED_PARA UINT16 length)
{
	BOOL 		EnrollCardF = CLEAR;	// Initialize flag indicating that card has not been enrolled

	switch(msgId)
	{
		case MSG_CARD_DETECTED_EVENT:
		{
			CARD_DETECTED_EV_DATA_t * 	cardDetectedPtr = dataPtr;

			// If message is from external reader, do not process it for enrollment
			if( (GetReaderRespPort(cardDetectedPtr -> readerId) == EXTERNAL_PORT) &&
				(SysBasicPara.reader[cardDetectedPtr -> readerId].type != UHF_W_READER) )
			{
				// If reader mode is exit then process it for access on exit, else ignore
				if(GetReaderEntryExitMode(cardDetectedPtr -> readerId) == EXIT_READER)
				{
					ProcessUserOnExitOnExternal(msgId, dataPtr);
				}
				return TRUE;
			}

			if(cardDetectedPtr ->cardId == 0)
			{
				//Send message to DC to display "ENROLLMENT FAILED"
				SetAudioVisual(USER_RESP_VERIFY_CARD_TYPE, INTERNAL_PORT, NULL);
				ExitEnrollmentMode();
				return TRUE;
			}
			//Check for duplicate card
			if(IsDuplicateCard(cardDetectedPtr -> cardId) == TRUE)
			{
				DPRINT(DC_LOG, "Enrollment failed, reason: %s", EnrollmentStatusStr[DUPL_CREDENTIAL]);
				ExitEnrollmentMode();
				return TRUE;
			}

			if(IsSmartCard(cardDetectedPtr -> cardType) == YES)
			{
				if((IsCustomSmartCardConfigEnabled(cardDetectedPtr -> cardType) == YES) &&
						(IsGivenFieldForSmartCardConfigured(cardDetectedPtr -> cardType, CARD_FIELD_CARD_ID) == YES))
				{
					SetAudioVisual(USER_RESP_VERIFY_CARD_TYPE, INTERNAL_PORT, NULL);
					DPRINT(DC_LOG, "Enrollment failed, reason: %s", EnrollmentStatusStr[DUPL_CREDENTIAL]);
					ExitEnrollmentMode();
					return TRUE;
				}
			}
			enrollActivity.cardId = cardDetectedPtr -> cardId;
			enrollActivity.cardType = cardDetectedPtr -> cardType;
			enrollActivity.readerId = cardDetectedPtr -> readerId;

			EnrollCardF = SET;
		}
		break;

		default:
		{
			return FALSE;
		}
		break;
	}

	if(EnrollCardF == SET)
	{
		UINT8 			cardIdx;

		//Store card id into user database
		for(cardIdx = 0; cardIdx < MAX_USER_CARD; cardIdx++)
		{
			if(IsUserCardPresent(enrollActivity.userIdx, cardIdx) == NO)
			{
				SetUserCardId(enrollActivity.userIdx, cardIdx, enrollActivity.cardId);
				break;
			}
		}
		//Send message to DC to display enrollment is successful
		SetAudioVisual(USER_RESP_CARD_ENROLLED, INTERNAL_PORT, NULL);
		if(ServerModeCfg.serverMode == COSEC_THIRD_PARTY && EnrollmentCmdRunningF == TRUE)
		{
			AddCredentialInThirdPartyBuffer(PUSH_API_CARD_CRED, enrollActivity.cardId, NULL);
		}

		WriteSingleUserConfig(enrollActivity.userIdx);

		//Log event- user card is enrolled
		WriteEventLog(ENROLLMENT, GetUserId(enrollActivity.userIdx), USER_CARD, (cardIdx+1), UNUSED_FIELD, UNUSED_FIELD);

		enrollActivity.cardCnt++;

		//Delete enrollment timer
		DeleteTimer(&enrollActivity.exitTmrHandle);

		if(IsNextCredToBeEnroll() == YES)
		{
			// Set time to trigger next credential enrollment
			if(SetWaitNextCredEnrollTimer() == FAIL)
			{
				EPRINT(DC_LOG, "Unable to set Next Enrollment Timer");
			}
		}
		else
		{
			ExitEnrollmentMode();
		}
	}
	return TRUE;
}

//*****************************************************************************
//	EnrollGetInputStateFn
//	Param:
//		IN :	msgId 	- Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length 	- Length of the message
//		OUT:	none
//	Returns:	TRUE 	- If message to be processed in this function
//				FALSE	- If message not to be processed in this function
//	Description:
//				It gets user id or spl fn id that is to be enrolled.
//
//*****************************************************************************
static BOOL EnrollGetInputStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UNUSED_PARA UINT16 length)
{
	UINT8 	getAvailCnt;
	BOOL	status;

	switch(msgId)
	{
		default:
			break;

		case MSG_ENROL_SEARCH_USER_NAME_FROM_ID:
		{
			UINT32 refUserId;

			if(SysAdvPara.enrollIdType == REF_USER_ID)
			{
				refUserId = (UINT32)atol( ( (USER_ALPHANUMID_t *)dataPtr) -> userAlphanumId);
				SendUserNameFromRefId( refUserId, GUI_MSG_ENROLL_GET_NAME_FROM_ID);
			}
			else
			{
				SendUserNameFromId( ((USER_ALPHANUMID_t *)dataPtr) -> userAlphanumId, GUI_MSG_ENROLL_GET_NAME_FROM_ID);
			}
			break;
		}

		case MSG_ENROL_SEARCH_USER_LIST_EVENT:
		{
			SendMsgSearchedUserList(((USER_NAME_SEARCH_t *)dataPtr) -> userName,
									GUI_MSG_SEARCHED_ENROLL_USER_LIST);
			break;
		}

		case MSG_ENROL_USER_VALIDATION_EVENT:
		{
			DPRINT(SYS_LOG, "Inside MSG_ENROL_USER_VALIDATION_EVENT");
			BOOL	validUserIdF = FALSE;

			sprintf(enrollInputs.alphaNumId, "%s", ((ENROLL_USER_t *)dataPtr) -> alphaNumId);
			enrollInputs.cardCnt =  ((ENROLL_USER_t *)dataPtr) -> cardCount;
			enrollInputs.fpCnt =  ((ENROLL_USER_t *)dataPtr) -> fpCount;
			enrollInputs.enrollmentType  = ((ENROLL_USER_t *)dataPtr) -> credType;
			enrollInputs.faceCnt  = ((ENROLL_USER_t *)dataPtr) -> maxFaces;
			DPRINT(SYS_LOG, "enrollInputs.faceCnt = %d",enrollInputs.faceCnt);
			memset(&EnrollmentTempInputs, 0, sizeof(EnrollmentTempInputs));
			//Store Selected data in temporary for repeating process of Enrollment
			EnrollmentTempInputs.enrollmentType  = enrollInputs.enrollmentType;
			EnrollmentTempInputs.cardCnt         = enrollInputs.cardCnt;
			EnrollmentTempInputs.fpCnt           = enrollInputs.fpCnt;
			EnrollmentTempInputs.faceCnt         = enrollInputs.faceCnt;
			EnrollmentTempInputs.enrollRepeatF   = TRUE;
			EnrollmentTempInputs.faceFrameCnt    = 0;
			
			ChkUpdateEnrollType(&enrollInputs,ENROLLMENT_SOURCE_QT_GUI);
			
			enrollInputs.faceFrameCnt = 0;
			
			if( (enrollInputs.enrollmentType == ENROLL_FACE_ONLY) && (NO == ChkAndDeriveFaceCapturedFrame(&enrollInputs)) )
			{
				//TODO Jigar : logic pending
				EPRINT(DC_LOG, "Invalid Face Enroll Count Received: [%d], Enrollment Not possible",enrollInputs.faceCnt);
                EnrollmentTempInputs.enrollRepeatF = 0;
                   EnrollmentTempInputs.faceCnt=1;
				TerminateEnrollmentActivity();
				AskForEnrollmentInput(0);
				return FAIL;
			}

			if(SysAdvPara.enrollIdType == ALPHANUM_ID)
			{
				DPRINT(DC_LOG, "enrollId Type is ALPHANUM_ID - %s", enrollInputs.alphaNumId);
				if(IsAlphaNumIdValid(enrollInputs.alphaNumId, &(enrollActivity.userIdx)) == YES)
				{
					validUserIdF = TRUE;
				}
			}
			else
			{
				enrollInputs.userId = (UINT32)atol(enrollInputs.alphaNumId);
				DPRINT(DC_LOG, "enrollId Type is USER_ID - %d", enrollInputs.userId);
				if(IsUserValid(enrollInputs.userId, &(enrollActivity.userIdx)) == YES)
				{
					validUserIdF = TRUE;
				}
			}

			if(validUserIdF == TRUE)
			{
				// In case of enrollment through special function we won't deny user if given
				// no. of cards/fingers can not be enrolled. Instead we will enroll either given
				// no. of cards/fingers or available no. of cards/fingers whichever is less.
				getAvailCnt = GetAvailUserCard(enrollActivity.userIdx);
				if( (getAvailCnt > 0) && (getAvailCnt < enrollInputs.cardCnt) )
 				{
					enrollInputs.cardCnt = getAvailCnt;
				}

				getAvailCnt = GetAvailUserFp(enrollActivity.userIdx);
				if( (getAvailCnt > 0) && (getAvailCnt < enrollInputs.fpCnt) )
				{
					enrollInputs.fpCnt = getAvailCnt;
				}
			}
			else //if(enrollInputs.source == SOURCE_SPECIAL_FUNC)
			{
				if(strlen(enrollInputs.alphaNumId) <= 0)
				{
					break;
				}

				UINT8 processingTimeoutValue = CURL_SERVER_API_TIMEOUT+3;
				enrollActivity.state = ENROLL_ADD_OR_ASSIGNUSER_STATE;
				SetAudioVisual(USER_RESP_WAIT_FOR_SIGNAL, INTERNAL_PORT, &processingTimeoutValue);
				strncpy(EnrollmentTempInputs.alphaNumId, enrollInputs.alphaNumId, ALPHANUM_ID_SIZE);
				if(pthread_create(&CheckUserExistOnServerThreadId, NULL, CheckUserExistOnServerThreadFn, NULL) == 0)
				{
					if(SetGetEnrollInputTimer(ENROLLMENT_TIMER) == FAIL)
					{
						EPRINT(DC_LOG, "Unable to set 'get inputs for enrollment' timer");
					}
					pthread_detach(CheckUserExistOnServerThreadId);
					return SUCCESS;
				}
				else
				{
					EPRINT(DC_LOG, "Unable to create CheckUserExistOnServerThread");
					SetAudioVisual(USER_RESP_BUZ_AUDIO_IDLE, INTERNAL_PORT, NULL);
					TerminateEnrollmentActivity();
					AskForEnrollmentInput(0);
					return FAIL;
				}
			}
			break;
		}

		case MSG_ENROL_SPL_FN_VALIDATION_EVENT:
		{
			enrollInputs.splFnIdx = ((ENROLL_SPL_FUNC_t *)dataPtr) -> fnId;
			enrollInputs.cardCnt = ((ENROLL_SPL_FUNC_t *)dataPtr) -> noOfCards;

			// In case of enrollment through special function we won't deny user if given
			// no. of cards/fingers can not be enrolled. Instead we will enroll either given
			// no. of cards/fingers or available no. of cards/fingers whichever is less.
			if(enrollInputs.splFnIdx < MAX_SPECIAL_FUNC)
			{
				getAvailCnt = GetAvailSplFnCard(enrollInputs.splFnIdx);
				if(getAvailCnt < enrollInputs.cardCnt)
				{
					enrollInputs.cardCnt = getAvailCnt;
				}
			}
			break;
		}
	}

	if( (msgId == MSG_ENROL_USER_VALIDATION_EVENT) ||
		(msgId == MSG_ENROL_SPL_FN_VALIDATION_EVENT) )
	{
		// Delete timer used to get inputs for enrolment
		DeleteTimer(&(enrollActivity.getEnrollInputTmrHandle));
		if( (status = StartEnrollment()) != ENROLLMENT_STARTED)
		{
			DPRINT(DC_LOG, "Enrollment not started: reason: %s", EnrollmentStatusStr[status]);
			// Set timer for how long to display user message before starting enrollment again
			if(SetWaitNewEnrollmentTimer() == FAIL)
			{
				EPRINT(DC_LOG, "Unable to start 'wait new enrollment timer'");
			}
		}
		return TRUE;// message is processed
	}

	// message is not to be processed here
	return FALSE;
}

//*****************************************************************************
//	EnrollmentAddOrAssignUserFn
//	Param:
//		IN :	msgId 	- Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length 	- Length of the message
//		OUT:	none
//	Returns:	TRUE if message processed, Else FALSE
//	Description:
//			It handles Enrollment Add Or AssignUserFn.
//
//*****************************************************************************
static BOOL EnrollmentAddOrAssignUserFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UNUSED_PARA UINT16 length)
{
	switch(msgId)
	{
		default:break;

		case GUI_ALNUM_KEEP_ALIVE:
		case GUI_BUTTON_KEEP_ALIVE:
		case GUI_BUTTON_FIELD_CLEAR:
		{
			// Reload display idle timer
			SetLcdIdleTimer(ENROLLMENT_TIMER);

			// Start timer to get input within enrollment time
			if(SetGetEnrollInputTimer(ENROLLMENT_TIMER) == FAIL)
			{
				EPRINT(DC_LOG, "Unable to set 'get inputs for enrollment' timer");
			}

			break;
		}

		case GUI_MSG_ASSIGN_USER_ON_DEVICE_START:
		{
			UINT8 processingTimeoutValue = CURL_SERVER_API_TIMEOUT+3;
			SetAudioVisual(USER_RESP_WAIT_FOR_SIGNAL, INTERNAL_PORT, &processingTimeoutValue);
			if(pthread_create(&AssignUserOnDeviceThreadId, NULL, AssignUserOnDeviceThreadFn, NULL) == 0)
			{
				pthread_detach(AssignUserOnDeviceThreadId);
				return SUCCESS;
			}
			else
			{
				return FAIL;
			}
		}break;

		case GUI_MSG_ASSIGN_USER_ON_DEVICE_CANCEL:
		{
			memset(&EnrollmentTempInputs, 0, sizeof(EnrollmentTempInputs));
			EnrollmentTempInputs.enrollRepeatF   = FALSE;
            EnrollmentTempInputs.faceCnt=1;
			SetAudioVisual(USER_RESP_BUZ_AUDIO_IDLE, INTERNAL_PORT, NULL);
			DeleteTimer(&DcInfo.lcdIdleTmrHandle);
			TerminateEnrollmentActivity();
			AskForEnrollmentInput(0);

		}break;

		case GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE_START:
		{
			SetBuzLedResp(USER_RESP_IDLE,INTERNAL_PORT);
			SendMessageToQt(" ", GUI_MSG_ENTER_USER_NAME);
		}break;

		case MSG_ENTER_USER_NAME_VALIDATION_EVENT:
		{
			SetAudioVisual(USER_RESP_WAIT_FOR_SIGNAL, INTERNAL_PORT, NULL);
			snprintf(EnrollmentTempInputs.name, NAME_SIZE_UTF8, "%s", ( (USER_NAME_SEARCH_t *)dataPtr) -> userName);
			if(pthread_create(&AddUserOnServerThreadFnId, NULL, AddUserOnServerThreadFn, NULL) == 0)
			{
				pthread_detach(AddUserOnServerThreadFnId);
				return SUCCESS;
			}
			else
			{
				return FAIL;
			}
		}break;

		case GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE_CANCEL:
		{
			memset(&EnrollmentTempInputs, 0, sizeof(EnrollmentTempInputs));
			EnrollmentTempInputs.enrollRepeatF   = FALSE;
             EnrollmentTempInputs.faceCnt=1;
			SetAudioVisual(USER_RESP_BUZ_AUDIO_IDLE, INTERNAL_PORT, NULL);
			TerminateEnrollmentActivity();
			AskForEnrollmentInput(0);
		}break;
	}

	return TRUE;
}
//*****************************************************************************
//	EnrollSplFnCardStateFn
//	Param:
//		IN :	msgId 	- Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length 	- Length of the message
//		OUT:	none
//	Returns:	TRUE if message processed, Else FALSE
//	Description:
//			It handles card enrollment process for special function.
//
//*****************************************************************************
static BOOL EnrollSplFnCardStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UNUSED_PARA UINT16 length)
{
	BOOL 		EnrollCardF = CLEAR;	// Initialize flag indicating that card has not been enrolled

	switch(msgId)
	{
		case MSG_CARD_DETECTED_EVENT:
		{
			CARD_DETECTED_EV_DATA_t * 	cardDetectedPtr = dataPtr;

			// If message is from external reader, do not process it for enrollment
			if(GetReaderRespPort(cardDetectedPtr -> readerId) == EXTERNAL_PORT)
			{
				// If reader mode is exit then process it for access on exit, else ignore
				if(GetReaderEntryExitMode(cardDetectedPtr -> readerId) == EXIT_READER)
				{
					ProcessUserOnExitOnExternal(msgId, dataPtr);
				}
				return TRUE;
			}

			if((IsSmartCard(cardDetectedPtr -> cardType) == NO) && (cardDetectedPtr -> cardId == 0))
			{
				EPRINT(DC_LOG,"card type read only and card id 0 received");
				SetAudioVisual(USER_RESP_VERIFY_CARD_TYPE, INTERNAL_PORT, NULL);
				ExitEnrollmentMode();
				return TRUE;
			}

			if((IsSmartCard(cardDetectedPtr -> cardType) == YES) &&
					(IsCustomSmartCardConfigEnabled(cardDetectedPtr -> cardType) == YES) && (IsGivenFieldForSmartCardConfigured(cardDetectedPtr -> cardType, CARD_FIELD_CARD_ID) == YES))
			{
				if(enrollInputs.enrollmentType == ENROLL_SMART_CARD)
				{
					enrollActivity.cardId = cardDetectedPtr -> cardId;
					enrollActivity.cardType = cardDetectedPtr -> cardType;
					enrollActivity.readerId = cardDetectedPtr -> readerId;

					if( (SpecialFunctions[enrollInputs.splFnIdx].cardId[0] == 0) &&
						(SpecialFunctions[enrollInputs.splFnIdx].cardId[1] == 0) &&
						(SpecialFunctions[enrollInputs.splFnIdx].cardId[2] == 0) &&
						(SpecialFunctions[enrollInputs.splFnIdx].cardId[3] == 0) )
					{
						EPRINT(DC_LOG,"special function card id not available to write");
						SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
						ExitEnrollmentMode();
						return TRUE;
					}

					SetAudioVisual(USER_RESP_WRITING_CARD, INTERNAL_PORT, NULL);
					SendMsgChangeSmartCardKey(enrollActivity.readerId, enrollActivity.cardType);
					if(SetEnrollmentTimer() == FAIL)
					{
						EPRINT(DC_LOG, "Unable to set Enrollment Timer");
					}
					return TRUE;
				}
				else
				{
					EPRINT(DC_LOG,"read only card selected card id can not be write");
					SetAudioVisual(USER_RESP_VERIFY_CARD_TYPE, INTERNAL_PORT, NULL);
					ExitEnrollmentMode();
					return TRUE;
				}
			}
			else
			{
				//Check for duplicate card
				if(IsDuplicateCard(cardDetectedPtr -> cardId) == TRUE)
				{
					DPRINT(DC_LOG, "Enrollment failed, reason: %s", EnrollmentStatusStr[DUPL_CREDENTIAL]);
					ExitEnrollmentMode();
					return TRUE;
				}

				if(GetAvailSplFnCard(enrollInputs.splFnIdx) == 0)
				{
					SetAudioVisual(USER_RESP_CRED_LIMIT_EXCEEDS, INTERNAL_PORT, NULL);
					ExitEnrollmentMode();
					return TRUE;
				}
				enrollActivity.cardId = cardDetectedPtr -> cardId;
				enrollActivity.cardType = cardDetectedPtr -> cardType;
				enrollActivity.readerId = cardDetectedPtr -> readerId;

				EnrollCardF = SET;
			}
		}
		break;

		case MSG_CHANGE_SMART_CARD_KEY_RESP:
		{
			SMART_CARD_KEY_UPDATE_RESP_DATA_t	*chngKeyDataPtr = (SMART_CARD_KEY_UPDATE_RESP_DATA_t *)dataPtr;

			DPRINT(DC_LOG, "Rcvd MSG_CHANGE_SMART_CARD_KEY_RESP, status = %s", ReaderRespStatusStr[chngKeyDataPtr -> status]);
			if(chngKeyDataPtr->status == READER_RESP_STATUS_SUCCESS)
			{
				SetAudioVisual(USER_RESP_WRITING_CARD, INTERNAL_PORT, NULL);
				// Erase all information from smart card
				SendMsgEraseSmartCard(enrollActivity.readerId, enrollActivity.cardType);
				if(SetEnrollmentTimer() == FAIL)
				{
					EPRINT(DC_LOG, "Unable to set Enrollment Timer");
				}
				return TRUE;
			}
			else
			{
				//Send message to DC to display "ENROLLMENT FAILED"
				SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
				ExitEnrollmentMode();
				return TRUE;
			}
		}
		break;

		case MSG_ERASE_SMART_CARD_RESP:
		{
			SMART_CARD_ERASE_RESP_DATA_t	*eraseCardDataPtr = (SMART_CARD_ERASE_RESP_DATA_t *)dataPtr;

			DPRINT(DC_LOG, "Rcvd MSG_ERASE_SMART_CARD_RESP, status = %s", ReaderRespStatusStr[eraseCardDataPtr -> status]);
			if(eraseCardDataPtr->status == READER_RESP_STATUS_SUCCESS)
			{
				SetAudioVisual(USER_RESP_WRITING_CARD, INTERNAL_PORT, NULL);
				// Erase all information from smart card

				SendMsgWriteSpclFunCardId(enrollInputs.splFnIdx);
				if(SetEnrollmentTimer() == FAIL)
				{
					EPRINT(DC_LOG, "Unable to set Enrollment Timer");
				}
				return TRUE;
			}
			else
			{
				//Send message to DC to display "ENROLLMENT FAILED"
				SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
				ExitEnrollmentMode();
				return TRUE;
			}
		}
		break;

		case MSG_WRITE_CARD_ID_RESP:
		{
			WRITE_CARD_ID_RESP_DATA_t * writeCardIdRespPtr = dataPtr;


			DPRINT(DC_LOG, "Rcvd MSG_WRITE_CARD_ID_RESP, status = %s", ReaderRespStatusStr[writeCardIdRespPtr -> status]);
			if(writeCardIdRespPtr -> status == READER_RESP_STATUS_INVALID_INPUT)
			{
				DPRINT(DC_LOG, "Enrollment failed, reason: %s", EnrollmentStatusStr[VERIFY_CARD_TYPE]);
				SetAudioVisual(USER_RESP_VERIFY_CARD_TYPE, INTERNAL_PORT, NULL);
				ExitEnrollmentMode();
				return TRUE;
			}
			else if(writeCardIdRespPtr -> status != READER_RESP_STATUS_SUCCESS)
			{
				//Send message to DC to display "ENROLLMENT FAILED"
				SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
				ExitEnrollmentMode();
				return TRUE;
			}

			EnrollCardF = SET;
		}
		break;

		default:
		{
			return FALSE;
		}
		break;
	}

	if(EnrollCardF == SET)
	{
		UINT8 			cardIdx, idx;

		if((IsSmartCard(enrollActivity.cardType) == YES) &&
				(IsCustomSmartCardConfigEnabled(enrollActivity.cardType) == YES) && (IsGivenFieldForSmartCardConfigured(enrollActivity.cardType, CARD_FIELD_CARD_ID) == YES))
		{
			if(SpecialFunctions[enrollInputs.splFnIdx].cardId[enrollActivity.cardCnt] != 0)
			{
				cardIdx = enrollActivity.cardCnt;
			}
			else
			{
				for(idx = 0; idx < MAX_SP_FUNC_CARD; idx++)
				{
					if(SpecialFunctions[enrollInputs.splFnIdx].cardId[idx] != 0)
					{
						cardIdx = idx;
						break;
					}
				}
			}
			//Send message to DC to display enrollment is successful
			// Log event- special function card is enrolled
			WriteEventLog(ENROLLMENT, enrollInputs.splFnIdx + 1, SPL_FN_CARD, (cardIdx+1), UNUSED_FIELD, UNUSED_FIELD);
		}
		else
		{
			//Store card id into special function database
			for(cardIdx = 0; cardIdx < MAX_SP_FUNC_CARD; cardIdx++)
			{
				if(IsSplFnCardPresent(enrollInputs.splFnIdx, cardIdx) == NO)
				{
					SpecialFunctions[enrollInputs.splFnIdx].cardId[cardIdx] = enrollActivity.cardId;
					break;
				}
			}
			WriteSingleSpecialFunctionsConfig(enrollInputs.splFnIdx);

			//Send message to DC to display enrollment is successful
			SetAudioVisual(USER_RESP_CARD_ENROLLED, INTERNAL_PORT, NULL);

			// Log event- special function card is enrolled
			WriteEventLog(ENROLLMENT, enrollInputs.splFnIdx + 1, SPL_FN_CARD, (cardIdx+1), UNUSED_FIELD, UNUSED_FIELD);
		}

		enrollActivity.cardCnt++;

		//Delete enrollment timer
		DeleteTimer(&enrollActivity.exitTmrHandle);

		if(IsNextCredToBeEnroll() == YES)
		{
			// Set time to trigger next credential enrollment
			if(SetWaitNextCredEnrollTimer() == FAIL)
			{
				EPRINT(DC_LOG, "Unable to set Next Enrollment Timer");
			}
		}
		else
		{
			ExitEnrollmentMode();
		}
	}
	return TRUE;
}

//*****************************************************************************
//	EnrollSmartCardStateFn
//	Param:
//		IN :	msgId 	- Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length 	- Length of the message
//		OUT:	none
//	Returns:	TRUE if message processed, Else FALSE
//	Description:
//			It handles smart card enrollment process.
//
//*****************************************************************************
static BOOL EnrollSmartCardStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UNUSED_PARA UINT16 length)
{
	BOOL 		EnrollCardF = CLEAR;	// Initialize flag indicating that card has not been enrolled
	UINT8 		cardIdx;
	static 		CARD_WRITE_FIELDS_t		currCardWriteFields;

	switch(msgId)
	{
		case MSG_CARD_DETECTED_EVENT:
		{
			CARD_DETECTED_EV_DATA_t * 	cardDetectedPtr = dataPtr;

			// If message is from external reader, do not process it for enrollment
			if(GetReaderRespPort(cardDetectedPtr -> readerId) == EXTERNAL_PORT)
			{
				// If reader mode is exit then process it for access on exit, else ignore
				if(GetReaderEntryExitMode(cardDetectedPtr -> readerId) == EXIT_READER)
				{
					ProcessUserOnExitOnExternal(msgId, dataPtr);
				}
				return TRUE;
			}

			if(enrollActivity.cardState == PROC_CARD_DETECTED_EV)
			{
				// Maintain copy of CARD_WRITE_FIELDS_t
				currCardWriteFields = enrollInputs.cardWriteFields;

				if( (IsSmartCard(cardDetectedPtr->cardType) == YES) && (IsCustomSmartCardConfigEnabled(cardDetectedPtr->cardType) == YES)
						&& (IsGivenFieldForSmartCardConfigured(cardDetectedPtr->cardType, CARD_FIELD_CARD_ID) == YES))
				{
					enrollInputs.cardWriteFields.cardIdF = SET;
					currCardWriteFields.cardIdF = SET;
				}
				else
				{
					// If no. of cards to be enrolled are not available for this user
					if(GetAvailUserCard(enrollActivity.userIdx) == 0)
					{
						SetAudioVisual(USER_RESP_CRED_LIMIT_EXCEEDS, INTERNAL_PORT, NULL);
						ExitEnrollmentMode();
						return TRUE;
					}
				}

				// check if any data to be written into smart card
				if(IsCardWriteRequired(CARD_FIELD_ANY, &(enrollInputs.cardWriteFields)) == YES)
				{
					// check if selected data can be written into card
					if(IsCardWritePsbl(cardDetectedPtr -> cardType) == NO)
					{
						if(IsCustomSmartCardConfigEnabled(cardDetectedPtr -> cardType) == YES)
						{
							DPRINT(DC_LOG, "Enrollment failed, reason: %s", EnrollmentStatusStr[VERIFY_SELECTION]);
							SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
						}
						else
						{
							DPRINT(DC_LOG, "Enrollment failed, reason: %s", EnrollmentStatusStr[VERIFY_CARD_TYPE]);
							SetAudioVisual(USER_RESP_VERIFY_CARD_TYPE, INTERNAL_PORT, NULL);
						}
						ExitEnrollmentMode();
						return TRUE;
					}
				}

				if( (IsSmartCard(cardDetectedPtr->cardType) == NO) || (IsCustomSmartCardConfigEnabled(cardDetectedPtr->cardType) == NO)
						|| (IsGivenFieldForSmartCardConfigured(cardDetectedPtr->cardType, CARD_FIELD_CARD_ID) == NO))
				{
					//Check for duplicate card
					if(IsDuplicateCard(cardDetectedPtr -> cardId) == TRUE)
					{
						DPRINT(DC_LOG, "Enrollment failed, reason: %s", EnrollmentStatusStr[DUPL_CREDENTIAL]);
						ExitEnrollmentMode();
						return TRUE;
					}
				}

				//Delete enrollment timer
				DeleteTimer(&enrollActivity.exitTmrHandle);

				// Store card id, card type and reader id into enrollment activity
				enrollActivity.cardId = cardDetectedPtr -> cardId;
				enrollActivity.cardType = cardDetectedPtr -> cardType;
				enrollActivity.readerId = cardDetectedPtr -> readerId;

				// check if it smart card
				if(IsSmartCard(cardDetectedPtr -> cardType) == YES)
				{
					SetAudioVisual(USER_RESP_WRITING_CARD, INTERNAL_PORT, NULL);

					if(IsDesfireCard(cardDetectedPtr -> cardType) == YES)
					{
						// Erase all information from smart card
						SendMsgEraseSmartCard(enrollActivity.readerId, enrollActivity.cardType);
						enrollActivity.cardState =  PROC_ERASE_SMART_CARD_RESP;
					}
					else
					{
						SendMsgChangeSmartCardKey(enrollActivity.readerId, enrollActivity.cardType);
						enrollActivity.cardState =  PROC_CHANGE_SMART_CARD_KEY_RESP;
					}

					if(SetEnrollmentTimer() == FAIL)
					{
						EPRINT(DC_LOG, "Unable to set Enrollment Timer");
					}
					return TRUE;
				}
			} // if card state is PROC_CARD_DETECTED_EV
			else // this message can not be handled in this state
			{
				return FALSE;
			}
		} // end of case MSG_CARD_DETECTED_EVENT
		break;

		case MSG_CHANGE_SMART_CARD_KEY_RESP:
		{
			SMART_CARD_KEY_UPDATE_RESP_DATA_t	*chngKeyDataPtr = (SMART_CARD_KEY_UPDATE_RESP_DATA_t *)dataPtr;

			DPRINT(DC_LOG, "Rcvd MSG_CHANGE_SMART_CARD_KEY_RESP, status = %s", ReaderRespStatusStr[chngKeyDataPtr -> status]);
			if(enrollActivity.cardState == PROC_CHANGE_SMART_CARD_KEY_RESP)
			{
				if(chngKeyDataPtr->status == READER_RESP_STATUS_SUCCESS)
				{
					if(IsDesfireCard(enrollActivity.cardType) == NO)
					{
						SetAudioVisual(USER_RESP_WRITING_CARD, INTERNAL_PORT, NULL);
						// Erase all information from smart card
						SendMsgEraseSmartCard(enrollActivity.readerId, enrollActivity.cardType);
						enrollActivity.cardState =  PROC_ERASE_SMART_CARD_RESP;
						if(SetEnrollmentTimer() == FAIL)
						{
							EPRINT(DC_LOG, "Unable to set Enrollment Timer");
						}
						return TRUE;
					}
				}
				else
				{
					//Send message to DC to display "ENROLLMENT FAILED"
					SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
					ExitEnrollmentMode();
					return TRUE;
				}
			}
			else
			{
				return FALSE;
			}
		}
		break;

		case MSG_ERASE_SMART_CARD_RESP:
		{
			SMART_CARD_ERASE_RESP_DATA_t	*eraseCardDataPtr = (SMART_CARD_ERASE_RESP_DATA_t *)dataPtr;

			DPRINT(DC_LOG, "Rcvd MSG_ERASE_SMART_CARD_RESP, status = %s", ReaderRespStatusStr[eraseCardDataPtr -> status]);
			if(enrollActivity.cardState == PROC_ERASE_SMART_CARD_RESP)
			{
				if(eraseCardDataPtr->status != READER_RESP_STATUS_SUCCESS)
				{
					//Send message to DC to display "ENROLLMENT FAILED"
					SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
					ExitEnrollmentMode();
					return TRUE;
				}

				if((eraseCardDataPtr->status == READER_RESP_STATUS_SUCCESS) && (IsDesfireCard(enrollActivity.cardType) == YES))
				{
					SendMsgChangeSmartCardKey(enrollActivity.readerId, enrollActivity.cardType);
					enrollActivity.cardState =  PROC_CHANGE_SMART_CARD_KEY_RESP;
					if(SetEnrollmentTimer() == FAIL)
					{
						EPRINT(DC_LOG, "Unable to set Enrollment Timer");
					}
					return TRUE;
				}
			}
			else
			{
				return FALSE;
			}
		}
		break;

		case MSG_WRITE_CARD_ID_RESP:
		{
			WRITE_CARD_ID_RESP_DATA_t * writeCardIdRespPtr = dataPtr;


			DPRINT(DC_LOG, "Rcvd MSG_WRITE_CARD_ID_RESP, status = %s", ReaderRespStatusStr[writeCardIdRespPtr -> status]);
			if(enrollActivity.cardState == PROC_WRITE_CARD_ID_RESP)
			{
				enrollInputs.cardWriteFields.cardIdF = CLEAR;
				currCardWriteFields.cardIdF = CLEAR;

				if(writeCardIdRespPtr -> status == READER_RESP_STATUS_INVALID_INPUT)
				{
					DPRINT(DC_LOG, "Enrollment failed, reason: %s", EnrollmentStatusStr[VERIFY_CARD_TYPE]);
					SetAudioVisual(USER_RESP_VERIFY_CARD_TYPE, INTERNAL_PORT, NULL);
					ExitEnrollmentMode();
					return TRUE;
				}
				else if(writeCardIdRespPtr -> status != READER_RESP_STATUS_SUCCESS)
				{
					//Send message to DC to display "ENROLLMENT FAILED"
					SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
					ExitEnrollmentMode();
					return TRUE;
				}
			}
			else
			{
				return FALSE;
			}
		}
		break;

		case MSG_WRITE_SEC_INFO_RESP:
		{
			WRITE_SEC_INFO_RESP_DATA_t * writeSecInfoRespPtr = dataPtr;

			DPRINT(DC_LOG, "Rcvd MSG_WRITE_SEC_INFO_RESP, status = %s", ReaderRespStatusStr[writeSecInfoRespPtr -> status]);
			if(enrollActivity.cardState == PROC_WRITE_SEC_INFO_RESP)
			{
				if(writeSecInfoRespPtr -> status == READER_RESP_STATUS_INVALID_INPUT)
				{
					DPRINT(DC_LOG, "Enrollment failed, reason: %s", EnrollmentStatusStr[VERIFY_CARD_TYPE]);
					SetAudioVisual(USER_RESP_VERIFY_CARD_TYPE, INTERNAL_PORT, NULL);
					ExitEnrollmentMode();
					return TRUE;
				}
				else if(writeSecInfoRespPtr -> status != READER_RESP_STATUS_SUCCESS)
				{
					//Send message to DC to display "ENROLLMENT FAILED"
					SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
					ExitEnrollmentMode();
					return TRUE;
				}
			}
			else // this message can not be handled in this state
			{
				return FALSE;
			}
		} // end of case MSG_RW_DATA_INTO_CARD_RESP
		break;

		case MSG_WRITE_PERS_INFO_RESP:
		{
			WRITE_PERS_INFO_RESP_DATA_t * writePersInfoRespPtr = dataPtr;

			DPRINT(DC_LOG, "Rcvd MSG_WRITE_PERS_INFO_RESP, status = %s", ReaderRespStatusStr[writePersInfoRespPtr -> status]);

			if(enrollActivity.cardState == PROC_WRITE_PERS_INFO_RESP)
			{
				if(writePersInfoRespPtr -> status == READER_RESP_STATUS_INVALID_INPUT)
				{
					DPRINT(DC_LOG, "Enrollment failed, reason: %s", EnrollmentStatusStr[VERIFY_CARD_TYPE]);
					SetAudioVisual(USER_RESP_VERIFY_CARD_TYPE, INTERNAL_PORT, NULL);
					ExitEnrollmentMode();
					return TRUE;
				}
				else if(writePersInfoRespPtr -> status != READER_RESP_STATUS_SUCCESS)
				{
					//Send message to DC to display "ENROLLMENT FAILED"
					SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
					ExitEnrollmentMode();
					return TRUE;
				}
			}
			else // this message can not be handled in this state
			{
				return FALSE;
			}
		} // end of case MSG_RW_DATA_INTO_CARD_RESP
		break;

		case MSG_WRITE_FP_RESP:
		{
			WRITE_FP_RESP_DATA_t * writeFpRespPtr = dataPtr;

			DPRINT(DC_LOG, "Rcvd MSG_WRITE_FP_RESP, status = %s", ReaderRespStatusStr[writeFpRespPtr -> status]);
			if(enrollActivity.cardState == PROC_WRITE_FP_RESP)
			{
				if(writeFpRespPtr -> status == READER_RESP_STATUS_INVALID_INPUT)
				{
					DPRINT(DC_LOG, "Enrollment failed, reason: %s", EnrollmentStatusStr[VERIFY_CARD_TYPE]);
					SetAudioVisual(USER_RESP_VERIFY_CARD_TYPE, INTERNAL_PORT, NULL);
					ExitEnrollmentMode();
					return TRUE;
				}
				else if(writeFpRespPtr -> status != READER_RESP_STATUS_SUCCESS)
				{
					//Send message to DC to display "ENROLLMENT FAILED"
					SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
					ExitEnrollmentMode();
					return TRUE;
				}
			}
			else // this message can not be handled in this state
			{
				return FALSE;
			}
		} // end of case MSG_RW_FP_INTO_CARD_RESP
		break;

		default:
		{
			// Message is not to be processed here
			return FALSE;
		}
	}

	if(IsCardWriteRequired(CARD_FIELD_ID, &(enrollInputs.cardWriteFields)) == YES)
	{
		if(SendMsgWriteDataIntoCard(MSG_WRITE_CARD_ID_CMD) == FAIL)
		{
			//Send message to DC to display "ENROLLMENT FAILED"
			SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
			ExitEnrollmentMode();
			return TRUE;
		}
		else
		{
			enrollActivity.cardState = PROC_WRITE_CARD_ID_RESP;
			// Send message to DC to display "WRITING SMART CARD"
			SetAudioVisual(USER_RESP_WRITING_CARD, INTERNAL_PORT, NULL);
			enrollInputs.cardWriteFields.cardIdF = CLEAR;
			if(SetEnrollmentTimer() == FAIL)
			{
				EPRINT(DC_LOG, "Unable to set Enrollment Timer");
			}
		}
	}
	else if(IsCardWriteRequired(CARD_FIELD_SEC_INFO, &(enrollInputs.cardWriteFields)) == YES)
	{
		// Send command to DC to write security info into card
		if(SendMsgWriteDataIntoCard(MSG_WRITE_SEC_INFO_CMD) == FAIL)
		{
			//Send message to DC to display "ENROLLMENT FAILED"
			SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
			ExitEnrollmentMode();
			return TRUE;
		}
		else
		{
			enrollActivity.cardState =  PROC_WRITE_SEC_INFO_RESP;
			// Send message to DC to display "WRITING SMART CARD"
			SetAudioVisual(USER_RESP_WRITING_CARD, INTERNAL_PORT, NULL);
			enrollInputs.cardWriteFields.addlSecCodeF = CLEAR;
			enrollInputs.cardWriteFields.facCodeF = CLEAR;
			enrollInputs.cardWriteFields.userIdF = CLEAR;
			if(SetEnrollmentTimer() == FAIL)
			{
				EPRINT(DC_LOG, "Unable to set Enrollment Timer");
			}
		}
	}
	else if(IsCardWriteRequired(CARD_FIELD_PERS_INFO, &(enrollInputs.cardWriteFields)) == YES)
	{
		// Send command to DC to write personalization info into card
		if(SendMsgWriteDataIntoCard(MSG_WRITE_PERS_INFO_CMD) == FAIL)
		{
			//Send message to DC to display "ENROLLMENT FAILED"
			SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
			ExitEnrollmentMode();
			return TRUE;
		}
		else
		{
			enrollActivity.cardState =  PROC_WRITE_PERS_INFO_RESP;
			// Send message to DC to display "WRITING SMART CARD"
			SetAudioVisual(USER_RESP_WRITING_CARD, INTERNAL_PORT, NULL);
			enrollInputs.cardWriteFields.userNameF = CLEAR;
			enrollInputs.cardWriteFields.designationF = CLEAR;
			enrollInputs.cardWriteFields.branchF = CLEAR;
			enrollInputs.cardWriteFields.departmentF = CLEAR;
			enrollInputs.cardWriteFields.bloodGrpF = CLEAR;
			enrollInputs.cardWriteFields.emrgncyContactF = CLEAR;
			enrollInputs.cardWriteFields.medicalHistoryF = CLEAR;
			if(SetEnrollmentTimer() == FAIL)
			{
				EPRINT(DC_LOG, "Unable to set Enrollment Timer");
			}
		}
	}
	else if(IsCardWriteRequired(CARD_FIELD_FP, &(enrollInputs.cardWriteFields)) == YES)
	{
		// Send command to DC to write user template into card
		if(SendMsgWriteDataIntoCard(MSG_WRITE_FP_CMD) == FAIL)
		{
			//Send message to DC to display "ENROLLMENT FAILED"
			SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
			ExitEnrollmentMode();
			return TRUE;
		}
		else
		{
			enrollActivity.cardState =  PROC_WRITE_FP_RESP;
			// Send message to DC to display "WRITING SMART CARD"
			SetAudioVisual(USER_RESP_WRITING_CARD, INTERNAL_PORT, NULL);
			enrollInputs.cardWriteFields.noOfTemplates = 0;
			if(SetEnrollmentTimer() == FAIL)
			{
				EPRINT(DC_LOG, "Unable to set Enrollment Timer");
			}
		}
	}
	else // Enrollment is done
	{
		EnrollCardF = SET;
	}

	if(EnrollCardF == SET)
	{
		if((IsSmartCard(enrollActivity.cardType) == YES) && (IsCustomSmartCardConfigEnabled(enrollActivity.cardType) == YES) &&
				(IsGivenFieldForSmartCardConfigured(enrollActivity.cardType, CARD_FIELD_CARD_ID) ==YES))
		{
			if(enrollActivity.cardCnt == 0)
			{
				if(GetUserCardId(enrollActivity.userIdx, 0) != 0)
				{
					cardIdx = 0;
				}
				else if(GetUserCardId(enrollActivity.userIdx, 1) != 0)
				{
					cardIdx = 1;
				}
			}
			else if(enrollActivity.cardCnt == 1)
			{
				if(GetUserCardId(enrollActivity.userIdx, 1) != 0)
				{
					cardIdx = 1;
				}
				else if(GetUserCardId(enrollActivity.userIdx, 0) != 0)
				{
					cardIdx = 0;
				}
			}
		}
		else
		{
			//Store card id into user database
			for(cardIdx = 0; cardIdx < MAX_USER_CARD; cardIdx++)
			{
				if(IsUserCardPresent(enrollActivity.userIdx, cardIdx) == NO)
				{
					SetUserCardId(enrollActivity.userIdx, cardIdx, enrollActivity.cardId);
					break;
				}
			}
			WriteSingleUserConfig(enrollActivity.userIdx);
		}
		// Send message to DC to display enrollment is successful
		SetAudioVisual(USER_RESP_CARD_ENROLLED, INTERNAL_PORT, NULL);
		if(ServerModeCfg.serverMode == COSEC_THIRD_PARTY && EnrollmentCmdRunningF == TRUE)
		{
			AddCredentialInThirdPartyBuffer(PUSH_API_CARD_CRED, enrollActivity.cardId, NULL);
		}
		// Log event- user card is enrolled
		WriteEventLog(ENROLLMENT, GetUserId(enrollActivity.userIdx), USER_CARD, (cardIdx+1),
																		UNUSED_FIELD, UNUSED_FIELD);

		enrollActivity.cardCnt++;

		// Delete enrollment timer
		DeleteTimer(&enrollActivity.exitTmrHandle);

		if(IsNextCredToBeEnroll() == YES)
		{
			// Copy Card write field flags for the enrollment of next card.
			enrollInputs.cardWriteFields = currCardWriteFields;

			// Set time to trigger next credential enrollment
			if(SetWaitNextCredEnrollTimer() == FAIL)
			{
				EPRINT(DC_LOG, "Unable to set Next Enrollment Timer");
			}
		}
		else
		{
			ExitEnrollmentMode();
		}
	}
	return TRUE;
}

//*****************************************************************************
//	EnrollMobileStateFn
//	Param:
//		IN :	msgId 	- Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length 	- Length of the message
//		OUT:	none
//	Returns:	TRUE if message processed, Else FALSE
//	Description:
//			It handles smart card enrollment process.
//
//*****************************************************************************
static BOOL EnrollMobileStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UNUSED_PARA UINT16 length)
{
    BOOL retVal;
    switch(msgId)
    {
        default:
        {
            DPRINT(SYS_LOG, "Mobile enroll msgid %x not handled", msgId);
            retVal = FALSE;
        }
        break;

        case MSG_GET_ACCESS_ID_EVENT:
        {
            if (dataPtr == NULL)
            {
                retVal = FALSE;
                EPRINT(SYS_LOG, "Mobile enroll event get access Id with null dataptr");
                break;
            }
            DPRINT(SYS_LOG, "Mobile enroll get access id evt");
            ACS_ENROLL_RESP_t acsEnrollResp;
            memset(acsEnrollResp.accessId,'\0',ACCESS_ID_SIZE);
            memcpy(&(acsEnrollResp.readerId),dataPtr,sizeof(UINT8));
            acsEnrollResp.status = (UINT8)GET_ACCESS_ID_SUCCESS;
            memcpy(acsEnrollResp.accessId, enrollActivity.accessId,  strlen(enrollActivity.accessId));
            acsEnrollResp.sendFacilityCode = enrollActivity.sendFc;
            acsEnrollResp.facilityCode = SysAdvPara.facilityCode;
            SendBLEEventTask(BLE_READ_ACCESS_ID_REQ_RESP, &acsEnrollResp, sizeof(ACS_ENROLL_RESP_t));
            retVal = TRUE;
        }
        break;

        case MSG_MOBILE_ENROLLMENT_EVENT_STATUS:
        {
            BOOL enrlStatus;
            if (dataPtr == NULL)
            {
                retVal = FALSE;
                EPRINT(SYS_LOG, "Mobile enroll event rcvd with null dataptr");
            }
            else
            {
                enrlStatus = *(BOOL *)dataPtr;
                DPRINT(SYS_LOG, "Mobile enroll event rcvd. Enrollment status:%s", (enrlStatus==FAIL)?"FAIL":"SUCCESS");
                if((enrlStatus == SUCCESS)&&(WriteEventLog(ENROLLMENT, GetUserId(enrollActivity.userIdx), MOBILE_DEVICE_EV, enrollActivity.accessCardSelect + 1, UNUSED_FIELD, UNUSED_FIELD)))
                {
                    SetAudioVisual(USER_RESP_ACS_ENROLLED, INTERNAL_PORT, NULL);
                }
                else
                {
                    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
                }
            }

            // At this stage Mobile enrollment is done so reset Flag
            EnrollmentCmdRunningF = FALSE;
            DeleteTimer(&enrollActivity.exitMobileTmrHandle);
            ExitEnrollmentMode();
        }
        break;

        case MSG_MOBILE_ENROLLMENT_TIMEOUT_EVENT:
        {
            // Send message to DC to display enrollment is timeout
            SetAudioVisual(USER_RESP_ENROLL_TIMEOUT, INTERNAL_PORT, NULL);
            EnrollmentCmdRunningF = FALSE;
            DeleteTimer(&enrollActivity.exitMobileTmrHandle);
            SwitchDcToIdleState();
            DPRINT(SYS_LOG, "Mobile enroll timeout");
            retVal = FALSE;
        }
        break;
    }
    return retVal;
}

/**
 * @brief ...
 * 
 * @param void ...
 * @return UINT32
 */
UINT32 GetCurrentEnrollmentState()
{
	return enrollInputs.enrollmentType;
}
/**
 * @brief ...
 * 
 * @param msgId ...
 * @param dataPtr ...
 * @param length ...
 * @return BOOL
 */
static BOOL EnrollFaceEnrollStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	//Face Enrollment state base funtion calling 
	//TODO:condition to ignore request other then face enroll If required
    if(enrollActivity.faceEnrollActivity.curstate >= ENROLL_FACE_STATE_END)
    {
        EPRINT(SYS_LOG, "Unhandeled face enroll state:%d, returning",enrollActivity.faceEnrollActivity.curstate);
        return FALSE;
    }

	if( (*faceenrollmentFsmFuncPtr[enrollActivity.faceEnrollActivity.curstate]) (msgId, dataPtr, length) == TRUE )
		return TRUE;
	else
		return FALSE;
}
/**
 * @brief ...
 * 
 * @param msgId ...
 * @param dataPtr ...
 * @param length ...
 * @return BOOL
 */
static BOOL FaceEnrollmentWait(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	BOOL retVal = FALSE;
           DPRINT(SYS_LOG, "Inside FaceEnrollmentWait");
    switch(msgId)
    {
        default:
        {
            DPRINT(SYS_LOG, "Face enroll msgid %x not handled", msgId);
            retVal = FALSE;
		}
		break;
		case MSG_FACE_ENROLL_START_CMD_SEND_EVENT:
		{
			//Send IDS Server to start enrollment
			DPRINT(SYS_LOG, "MSG_FACE_ENROLL_START_CMD_SEND_EVENT");
			FACE_ENROLL_REQ_t tObj;
			tObj.UserId = enrollInputs.userId; 
			tObj.requestType = USR_ENR_START;
			tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
			tObj.numberOfEnrollFace = enrollInputs.faceCnt;
			ChangeFaceEnrollState(ENROLL_FACE_STATE_START);
			ProcessEnrFaceRequest(&tObj);
			SetFaceEnrollStopGlobleWaitTimer();
			retVal = TRUE;
		}
		break;

		case GUI_BUTTON_FACE_ENRL_TERMINATE:
		case GUI_BUTTON_BACK:
		{
			DPRINT(SYS_LOG, "GUI_BUTTON_BACK");
			BackEnrollmentScreen();
			retVal = TRUE;
		}
		break;
    }
	return retVal;
}

void FaceEnrHandleLostConnectivity()
{
    if( GetCurrentFaceEnrollmentState()>= ENROLL_FACE_STATE_END)
    {
        EPRINT(SYS_LOG, "Unhandeled face enroll state:%d, returning",enrollActivity.faceEnrollActivity.curstate);
        return;
    }
	if(EnrollmentTempInputs.faceFrameCnt != 0)
	{
		EnrollFaceHandlingF = TRUE;
		DPRINT(SYS_LOG, "some frames are allready processed, Sending Stop process");
		DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
		if(enrollActivity.faceEnrollActivity.curstate != ENROLL_FACE_STATE_STOP)
		{
			FACE_ENROLL_REQ_t tObj;
			tObj.requestType = USR_ENR_ENROLL_STOP;
			tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
			tObj.numberOfEnrollFace = enrollInputs.faceCnt;
			tObj.UserId = enrollInputs.userId; 
			ChangeFaceEnrollState(ENROLL_FACE_STATE_STOP);
			ProcessEnrFaceRequest(&tObj);
		}
		ENROLL_FACE_MSG_INFO_TO_QT_t tmpObj;
		tmpObj.timeOutSec = 5;
		tmpObj.popupType = ENROLL_FACE_QT_POPUP_TYPE_USER_OTHER;
		tmpObj.warningIconShow = FALSE;
		snprintf(tmpObj.AlertString, sizeof(tmpObj.AlertString),
				GetDisplayMessage(FALSE,
					NULL_CHAR, MSG_CAMERA_CONNECTIVITY_LOST_1019));
		strcat(tmpObj.AlertString, "   \n");
		strcat(tmpObj.AlertString, GetDisplayMessage(FALSE,
                                        NULL_CHAR, MSG_DO_YOU_WANT_TO_ENROLL_ALREADY_CAPTURED_FACES_1066));
		snprintf(tmpObj.CountDownString, sizeof(tmpObj.CountDownString),
				GetDisplayMessage(FALSE,
					NULL_CHAR, MSG_AUTOMATICALLY_ABORT_IN_1017));
		SendMessageToQt(&tmpObj, GUI_MSG_FACE_ENROLL_USER_POPUP_SCREEN);
		SetFaceEnrollStopGlobleWaitTimer(); 
	}
	else
	{
		EnrollFaceHandlingF = TRUE;
		//No frames are processed
		char FaliStr[QT_MSG_SIZE] = {'\0'};
		DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
		DPRINT(SYS_LOG, "No frames are processed so terminating enroll process");	

		ChangeFaceEnrollState(ENROLL_FACE_STATE_ABORT);

		strcpy(FaliStr, GetDisplayMessage(FALSE,
					NULL_CHAR, MSG_CAMERA_CONNECTIVITY_LOST_1019));
		SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);

        EnrollmentTempInputs.faceFrameCnt = 0;
		// Exit enrollment mode
		ExitEnrollmentMode();
	}
}

/**
 * @brief ...
 * 
 * @param msgId ...
 * @param dataPtr ...
 * @param length ...
 * @return BOOL
 */
static BOOL FaceEnrollmentStart(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	BOOL retVal = FALSE;
    switch(msgId)
    {
	    default:
	    {
		    DPRINT(SYS_LOG, "Face enroll msgid %x not handled", msgId);
		    retVal = FALSE;
	    }
	    break;

	    case NO_MSG:
	    {
		    if( (NULL == dataPtr) || (length != sizeof(IDTResponseQueue_t)) )
		    {
			    DPRINT(SYS_LOG, "Invalid data received msgid %x", msgId);
			    retVal = FALSE;
			    break;
		    }
		    switch(((IDTResponseQueue_t*)dataPtr)->rplErlType)
		    {
			    case RPL_ERL_FROM_DVC_ERR :
			    {
				    char FaliStr[QT_MSG_SIZE] = {'\0'};
				    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);

				    if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_MAX_FACE_ENROLLED)
				    {
					    DPRINT(SYS_LOG, "Error : Max Faces Enrolled , Enrollment not started");
					    strcpy(FaliStr, GetDisplayMessage(FALSE,
								    NULL_CHAR, MSG_MAX_FACES_ENROLLED_1054));
					    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
				    }
				    else if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_INVALID_REQUEST)
				    {
					    DPRINT(SYS_LOG, "Error : Invalid Request Sent , Enrollment not started");
					    strcpy(FaliStr, GetDisplayMessage(FALSE,
								    NULL_CHAR, MSG_INVALID_REQUEST_SENT_1056));
					    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
				    }
				    else if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_USR_NOT_FOUND)
				    {
					    DPRINT(SYS_LOG, "Error : Invalid USER ID Sent , Enrollment not started");
					    strcpy(FaliStr, GetDisplayMessage(FALSE,
								    NULL_CHAR, MSG_USER_NOT_FOUND_575));
					    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
				    }
				    else if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_ENROLLMENT_ALREADY_RUNNING)
				    {
					    DPRINT(SYS_LOG, "Error : Enrollment allready in running state at IDS/EBS");
					    strcpy(FaliStr, GetDisplayMessage(FALSE,
								    NULL_CHAR, MSG_ENROLLMENT_ALREADY_RUNNING_1072));
					    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
				    }
				    else if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_ENROLLMENT_ALREADY_RUNNING_FOR_THIS_USER)
				    {
					    DPRINT(SYS_LOG, "Error : Enrollment allready in running for this user at IDS");
					    strcpy(FaliStr, GetDisplayMessage(FALSE,
								    NULL_CHAR, MSG_ENROLLMENT_PROCESS_ALREADY_RUNNING_FOR_THIS_USER_1073));
					    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
				    }
                    else if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_PARAMETER_INVALID)
				    {
					    DPRINT(SYS_LOG, "Error : Invalid Parameter , Enrollment not started");
					    strcpy(FaliStr, GetDisplayMessage(FALSE,
								    NULL_CHAR, MSG_INVALID_PARAMETER_GIVEN_FOR_ENROLLMENT_1055));
					    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
				    }
                    else
                    {
                        DPRINT(SYS_LOG, "Error : Unkown error , Enrollment not started");
                        strcpy(FaliStr, GetDisplayMessage(FALSE,
                                    NULL_CHAR, MSG_ERROR_PROCESS_FAILED_1076));
                        SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
                    }
				    // Exit enrollment mode
				    ExitEnrollmentMode();
				    // User response must be set idle to overcome incomplete process' response
				    retVal = FALSE;
			    }
			    break;
			    case RPL_ERL_FROM_DVC_START:
			    {
				    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
				    DPRINT(SYS_LOG, "Success : Enrollment Started");
				    FACE_ENROLL_REQ_t tObj;
				    tObj.requestType = USR_ENR_ENROLL_IMAGE;
				    tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
				    tObj.numberOfEnrollFace = enrollInputs.faceCnt;
				    tObj.UserId = enrollInputs.userId; 
				    ChangeFaceEnrollState(ENROLL_FACE_STATE_PROGRESS);
				    UINT32 ResponseTime = ((IDTResponseQueue_t*)dataPtr)->timeStamp;
				    struct 	stat FileInfo;
				    if(ElapsedTick(ResponseTime) >= 200 && (stat(FR_DST_IMAGE, &FileInfo) == 0))
				    { 
				    	DPRINT(SYS_LOG, "RESPONSE ARRIVED > 200 %d", ResponseTime);
				    	ProcessEnrFaceRequest(&tObj);
				    }
				    else
				    {
					   //TODO: CALL API HERE 
				    	DPRINT(SYS_LOG, "RESPONSE ARRIVED < 200 %d", ResponseTime);
			    		LoadCameraImageWaitCountDownTimer();
				    }
				    SetFaceEnrollStopGlobleWaitTimer();
				    retVal = TRUE;
			    }
			    break;
			    default:
			    {
				    DPRINT(SYS_LOG, "Invalid Enrollment reply Receive: %u", ((IDTResponseQueue_t*)dataPtr)->rplErlType);
				    retVal = FALSE;
			    }
				    break;
		    }
	    }
	    break;
	    case GUI_BUTTON_FACE_ENRL_TERMINATE:
	    case GUI_BUTTON_BACK:
	    {
		    DPRINT(SYS_LOG, "GUI_BUTTON_BACK");
            BackEnrollmentScreen();
		    retVal = TRUE;
	    }
	    break;

    }
	return retVal;
}
/**
 * @brief ...
 * 
 * @param msgId ...
 * @param dataPtr ...
 * @param length ...
 * @return BOOL
 */
static BOOL FaceEnrollmentInprogress(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	BOOL retVal = FALSE;
    switch(msgId)
    {
	    default:
	    {
		    DPRINT(SYS_LOG, "Face enroll msgid %x not handled", msgId);
		    retVal = FALSE;
	    }
	    break;

	    case NO_MSG:
	    {
		    if( (NULL == dataPtr) || (length != sizeof(IDTResponseQueue_t)) )
		    {
			    DPRINT(SYS_LOG, "Invalid data received msgid %x", msgId);
			    retVal = FALSE;
			    break;
		    }
		    switch(((IDTResponseQueue_t*)dataPtr)->rplErlType)
		    {
			    case RPL_ERL_FROM_DVC_ERR :
			    {
				    if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_INVALID_REQUEST)
				    {
					    char FaliStr[QT_MSG_SIZE] = {'\0'};
					    strcpy(FaliStr, GetDisplayMessage(FALSE,
								    NULL_CHAR, MSG_INVALID_REQUEST_SENT_1056));
					    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
                                            EPRINT(SYS_LOG, "Invalid request Sent");
					    // Exit enrollment mode
					    ExitEnrollmentMode();
					    retVal = FALSE;
				    }
				    if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_ENROLLMENT_FAILD)
				    {
					    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, NULL);
                                            EPRINT(SYS_LOG, "Invalid request Sent");
					    // Exit enrollment mode
					    ExitEnrollmentMode();
					    retVal = FALSE;
				    }
				    else if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_PARAMETER_INVALID || 
						    ((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_IMG_NOT_AVLBL)
				    {
					    char FaliStr[QT_MSG_SIZE] = {'\0'};
					    strcpy(FaliStr, GetDisplayMessage(FALSE,
								    NULL_CHAR, MSG_IMAGE_DATA_NOT_PROPER_1057));
					    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
                                            EPRINT(SYS_LOG, "Image data is not proper");
					    // Exit enrollment mode
					    ExitEnrollmentMode();
					    // User response must be set idle to overcome incomplete process' response
					    retVal = FALSE;
				    }
				    else
				    {
				//	    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
					    char ProgressBarData[QT_MSG_SIZE] = {'\0'};
					    if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_FACE_IS_NOT_DETECTED_TEMPLETE_EXTRACT_FAIL)
					    {
						    EPRINT(SYS_LOG,"FACE NOT DETECTED");
						    sprintf(ProgressBarData, "%d%c%d%c%s", enrollInputs.faceFrameCnt,
								    UI_MSG_FSP, EnrollmentTempInputs.faceFrameCnt, UI_MSG_FSP,
								    GetDisplayMessage(FALSE,
									    NULL_CHAR, MSG_FACE_NOT_DETECTED_1058));
					    }
					    else if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_MASK_DETECTED)
					    {
						    //TODO: may need to update for IDS error code for this condition
						    EPRINT(SYS_LOG,"MASK DETECTED");
						    sprintf(ProgressBarData, "%d%c%d%c%s", enrollInputs.faceFrameCnt,
								    UI_MSG_FSP, EnrollmentTempInputs.faceFrameCnt, UI_MSG_FSP,
								    GetDisplayMessage(FALSE,
									    NULL_CHAR, MSG_MASK_DETECTED_1059));
					    }
					    else if(((IDTResponseQueue_t*)dataPtr)->errorCode >= ERR_LIVENESS_FACE_NOT_STRAIGHT &&
							    ((IDTResponseQueue_t*)dataPtr)->errorCode <= ERR_LIVENESS_NOT_DETECTED)
					    {
						    EPRINT(SYS_LOG,"FACE IS NOT LIVE");
						    sprintf(ProgressBarData, "%d%c%d%c%s", enrollInputs.faceFrameCnt,
								    UI_MSG_FSP, EnrollmentTempInputs.faceFrameCnt, UI_MSG_FSP,
								    GetDisplayMessage(FALSE,
									    NULL_CHAR, MSG_FACE_IS_NOT_LIVE_1060));
					    }
					    else if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_FACE_IS_NOT_AS_EXPECTED)
					    {
						    EPRINT(SYS_LOG,"FACE ANGLE IS NOT AS EXPECTED");
						    sprintf(ProgressBarData, "%d%c%d%c%s", enrollInputs.faceFrameCnt,
								    UI_MSG_FSP, EnrollmentTempInputs.faceFrameCnt, UI_MSG_FSP,
								    GetDisplayMessage(FALSE,
									    NULL_CHAR, MSG_KEEP_YOUR_FACE_STRAIGHT_1061));
					    }
                        else if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_FULL_FACE_NOT_DETECTED)
                        {
                            EPRINT(SYS_LOG,"FULL FACE NOT VISIBLE");
                            sprintf(ProgressBarData, "%d%c%d%c%s", enrollInputs.faceFrameCnt,
                                    UI_MSG_FSP, EnrollmentTempInputs.faceFrameCnt, UI_MSG_FSP,
                                    GetDisplayMessage(FALSE,
                                        NULL_CHAR, MSG_FACE_NOT_VISIBLE_1075));
                        }

					    SendMessageToQt(&ProgressBarData[0], GUI_MSG_FACE_ENROLL_PROCESSING_SCREEN);

					    //Send Next Frame
					    FACE_ENROLL_REQ_t tObj;
					    tObj.requestType = USR_ENR_ENROLL_IMAGE;
					    tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
					    tObj.numberOfEnrollFace = enrollInputs.faceCnt;
					    tObj.UserId = enrollInputs.userId; 
					    UINT32 ResponseTime = ((IDTResponseQueue_t*)dataPtr)->timeStamp;
					    struct 	stat FileInfo;
					    if(ElapsedTick(ResponseTime) >= 200 && (stat(FR_DST_IMAGE, &FileInfo) == 0))
					    { 
                            DPRINT(SYS_LOG, "RESPONSE ARRIVED > 200 %d", ResponseTime);
						    ProcessEnrFaceRequest(&tObj);
					    }
					    else
					    {
						    //TODO: CALL API HERE 
						    DPRINT(SYS_LOG, "RESPONSE ARRIVED < 200 %d", ResponseTime);
						    LoadCameraImageWaitCountDownTimer();
					    }
//					    SetFaceEnrollStopGlobleWaitTimer();
					    retVal = TRUE;
				    }
			    }
			    break;
			    case RPL_ERL_FROM_DVC_NXT_FRAME:
			    {
				    char ProgressBarData[QT_MSG_SIZE] = {'\0'};
				    EnrollmentTempInputs.faceFrameCnt++;
				    sprintf(ProgressBarData, "%d%c%d%c%s", enrollInputs.faceFrameCnt,
						    UI_MSG_FSP, EnrollmentTempInputs.faceFrameCnt, UI_MSG_FSP, "");
				    SendMessageToQt(&ProgressBarData[0], GUI_MSG_FACE_ENROLL_PROCESSING_SCREEN);
				    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
				    //sent frame count is not equel to expected frame count so send next Image frame
				    if(EnrollmentTempInputs.faceFrameCnt < enrollInputs.faceFrameCnt)
				    {
					    DPRINT(SYS_LOG, "Requested Next Frame");
					    FACE_ENROLL_REQ_t tObj;
					    tObj.requestType = USR_ENR_ENROLL_IMAGE;
					    tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
					    tObj.numberOfEnrollFace = enrollInputs.faceCnt;
					    tObj.UserId = enrollInputs.userId; 
					    UINT32 ResponseTime = ((IDTResponseQueue_t*)dataPtr)->timeStamp;
					    struct 	stat FileInfo;
					    if(ElapsedTick(ResponseTime) >= 200 && (stat(FR_DST_IMAGE, &FileInfo) == 0))
					    { 
                            DPRINT(SYS_LOG, "RESPONSE ARRIVED > 200 %d", ResponseTime);
						    ProcessEnrFaceRequest(&tObj);
					    }
					    else
					    {
						    //TODO: CALL API HERE 
						    DPRINT(SYS_LOG, "RESPONSE ARRIVED < 200 %d", ResponseTime);
						    LoadCameraImageWaitCountDownTimer();
					    }
				    }
				    else
				    {
					    //sent frame count is equel to expected frame count send enroll face request
					    DPRINT(SYS_LOG, "Expected Frames sent : sending Enroll Face Request");
					    FACE_ENROLL_REQ_t tObj;
					    tObj.requestType = USR_ENR_ENROLL_FACES;
					    tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
					    tObj.numberOfEnrollFace = enrollInputs.faceCnt;
					    tObj.UserId = enrollInputs.userId; 
					    ProcessEnrFaceRequest(&tObj);
				    }
				    SetFaceEnrollStopGlobleWaitTimer();
				    retVal = TRUE;	
			    }
			    break;
			    case RPL_ERL_FROM_DVC_SUCCESS:
			    {
				    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
				    DPRINT(SYS_LOG, "Success : Enrollment done success");	
				    SetAudioVisual(USER_RESP_FACE_ENROLLED, INTERNAL_PORT, NULL);

				    EnrollmentTempInputs.faceFrameCnt = 0;
				    UINT16 userIdx = 0;
				    if(IsUserValid(((IDTResponseQueue_t*)dataPtr)->userId, &userIdx))
				    {
					    UINT8 tCnt = 0;
					    for(tCnt = 0; tCnt < (((IDTResponseQueue_t*)dataPtr)->enrolledFaceCnt); tCnt++)
					    {
						    DPRINT(DC_LOG,"Server Event Sent for : Type:[%d],Tid:[%d] Uid:[%d], Fid:[%d]",
								    ((IDTResponseQueue_t*)dataPtr)->rplErlType,
								    ((IDTResponseQueue_t*)dataPtr)->transactionId,
								    ((IDTResponseQueue_t*)dataPtr)->userId,
								    ((IDTResponseQueue_t*)dataPtr)->faceIdx[tCnt]);
						    WriteEventLog(ENROLLMENT, ((IDTResponseQueue_t*)dataPtr)->userId, FACE_ERL_EV,
								    ((IDTResponseQueue_t*)dataPtr)->faceIdx[tCnt], UNUSED_FIELD,
								    ((IDTResponseQueue_t*)dataPtr)->userId);
					    }
                                    }
				    // Exit enrollment mode
				    ExitEnrollmentMode();
				    retVal = TRUE;
			    }
			    break;
			    case RPL_ERL_FROM_DVC_USER_CONFLICT:
			    {
				    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
				    EPRINT(SYS_LOG, "Error : User conflicted with other enrolled user");
				    EnrollmentTempInputs.ConflictType = RPL_ERL_FROM_DVC_USER_CONFLICT;
				    ChangeFaceEnrollState(ENROLL_FACE_STATE_ABORT);
				    ENROLL_FACE_MSG_INFO_TO_QT_t tmpObj;
				    tmpObj.timeOutSec = 5;
				    tmpObj.popupType = ENROLL_FACE_QT_POPUP_TYPE_USER_CONFLICT;
				    tmpObj.warningIconShow = FALSE;
				    snprintf(tmpObj.AlertString, sizeof(tmpObj.AlertString),
						    GetDisplayMessage(FALSE,
//                                NULL_CHAR, MSG_DIFFERENT_USER_DETECTED_ENROLLMENT_ABORTED_1063));
                                  //Face is Already Enrolled Against Different User. Enrollment Aborted
//                                  NULL_CHAR, MSG_FACE_IS_ALREADY_ENROLLED_AGAINST_DIFFERENT_USER_NENROLLMENT_ABORTED_1063));
                                  NULL_CHAR, MSG_ENROLLMENT_ABORTED_NFACE_IS_ALREADY_ENROLLED_AGAINST_DIFFERENT_USER_1063));
				    snprintf(tmpObj.CountDownString, sizeof(tmpObj.CountDownString),
						    GetDisplayMessage(FALSE,
							    NULL_CHAR, MSG_AUTOMATICALLY_ABORT_IN_1017));
				    SendMessageToQt(&tmpObj, GUI_MSG_FACE_ENROLL_USER_POPUP_SCREEN);
				    SetFaceEnrollStopGlobleWaitTimer();
				    retVal = TRUE;
			    }
			    break;
			    case RPL_ERL_FROM_DVC_CAPTURED_CONFLICT:
			    {
				    EPRINT(SYS_LOG, "Error : User conflicted with previously captured faces");
				    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
				    ChangeFaceEnrollState(ENROLL_FACE_STATE_CONFLICT);
				    EnrollmentTempInputs.ConflictType = RPL_ERL_FROM_DVC_CAPTURED_CONFLICT;
				    ENROLL_FACE_MSG_INFO_TO_QT_t tmpObj;
				    tmpObj.timeOutSec = 5;
				    tmpObj.popupType = ENROLL_FACE_QT_POPUP_TYPE_USER_OTHER;
				    tmpObj.warningIconShow = FALSE;
				    snprintf(tmpObj.AlertString, sizeof(tmpObj.AlertString),
						    GetDisplayMessage(FALSE,
//							    NULL_CHAR, MSG_DIFFERENT_FACE_DETECTED_CONTINUE_1064));
                                //Face Conflict with Capture Faces. Do you want to Continue?
                                NULL_CHAR, MSG_FACE_CONFLICT_WITH_CAPTURE_FACES_NDO_YOU_WANT_TO_CONTINUE_1064));
				    snprintf(tmpObj.CountDownString, sizeof(tmpObj.CountDownString),
						    GetDisplayMessage(FALSE,
							    NULL_CHAR, MSG_AUTOMATICALLY_STOP_IN_1065));
				    SendMessageToQt(&tmpObj, GUI_MSG_FACE_ENROLL_USER_POPUP_SCREEN);
				    SetFaceEnrollStopGlobleWaitTimer();  
				    retVal = TRUE;
			    }
			    break;
			    case RPL_ERL_FROM_DVC_ENR_CONFLICT:
			    {
				    EPRINT(SYS_LOG, "Error : User conflicted with it's allready enrolled faces");
				    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
				    ChangeFaceEnrollState(ENROLL_FACE_STATE_CONFLICT);
				    EnrollmentTempInputs.ConflictType = RPL_ERL_FROM_DVC_ENR_CONFLICT;
				    ENROLL_FACE_MSG_INFO_TO_QT_t tmpObj;
				    tmpObj.timeOutSec = 5;
				    tmpObj.popupType = ENROLL_FACE_QT_POPUP_TYPE_USER_OTHER;
				    tmpObj.warningIconShow = FALSE;
				    snprintf(tmpObj.AlertString, sizeof(tmpObj.AlertString),
						    GetDisplayMessage(FALSE,
//							    NULL_CHAR, MSG_DIFFERENT_FACE_DETECTED_CONTINUE_1064));
                                //1075 Face Conflict with Enrolled Faces. Do you want to Continue?
                                NULL_CHAR, MSG_FACE_CONFLICT_WITH_ENROLLED_FACES_NDO_YOU_WANT_TO_CONTINUE_1074));
				    snprintf(tmpObj.CountDownString, sizeof(tmpObj.CountDownString),
						    GetDisplayMessage(FALSE,
							    NULL_CHAR, MSG_AUTOMATICALLY_STOP_IN_1065));
				    SendMessageToQt(&tmpObj, GUI_MSG_FACE_ENROLL_USER_POPUP_SCREEN);
				    SetFaceEnrollStopGlobleWaitTimer();  
				    retVal = TRUE;

			    }break;
			    default:
			    {
				    DPRINT(SYS_LOG, "Invalid Enrollment reply Receive: %u", ((IDTResponseQueue_t*)dataPtr)->rplErlType);
				    retVal = FALSE;
				    break;
			    }
		    }
	    }
	    break;

	    case GUI_BUTTON_FACE_ENRL_STOP:
	    {
		    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
		    FACE_ENROLL_REQ_t tObj;
		    tObj.requestType = USR_ENR_ENROLL_STOP;
		    tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
		    tObj.numberOfEnrollFace = enrollInputs.faceCnt;
		    tObj.UserId = enrollInputs.userId; 
		    ChangeFaceEnrollState(ENROLL_FACE_STATE_STOP);
		    ProcessEnrFaceRequest(&tObj);

		    ENROLL_FACE_MSG_INFO_TO_QT_t tmpObj;
		    tmpObj.timeOutSec = 5;
		    tmpObj.popupType = ENROLL_FACE_QT_POPUP_TYPE_USER_OTHER;
		    tmpObj.warningIconShow = FALSE;
		    snprintf(tmpObj.AlertString, sizeof(tmpObj.AlertString),
				    GetDisplayMessage(FALSE,
                        NULL_CHAR, MSG_DO_YOU_WANT_TO_ENROLL_ALREADY_CAPTURED_FACES_1066));
		    snprintf(tmpObj.CountDownString, sizeof(tmpObj.CountDownString),
				    GetDisplayMessage(FALSE,
					    NULL_CHAR, MSG_AUTOMATICALLY_ABORT_IN_1017));
		    SendMessageToQt(&tmpObj, GUI_MSG_FACE_ENROLL_USER_POPUP_SCREEN);
		    retVal = TRUE;
		    SetFaceEnrollStopGlobleWaitTimer();
	    }
	    break;

	    case GUI_BUTTON_FACE_ENRL_TERMINATE:
	    {
		    if(EnrollmentTempInputs.faceFrameCnt != 0)
		    {
			    EPRINT(SYS_LOG, "Teminating Enroll Process, abort clicked");
			    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
			    ChangeFaceEnrollState(ENROLL_FACE_STATE_ABORT);
			    ENROLL_FACE_MSG_INFO_TO_QT_t tmpObj;
			    tmpObj.timeOutSec = 5;
			    tmpObj.popupType = ENROLL_FACE_QT_POPUP_TYPE_USER_OTHER;
			    tmpObj.warningIconShow = FALSE;
			    snprintf(tmpObj.AlertString, sizeof(tmpObj.AlertString),
					    GetDisplayMessage(FALSE,
						    NULL_CHAR,MSG_THIS_WILL_TERMINATE_ENROLLMENT_PROCESS_DO_YOU_WANT_TO_CONTINUE_1052));
			    snprintf(tmpObj.CountDownString, sizeof(tmpObj.CountDownString),
					    GetDisplayMessage(FALSE,
						    NULL_CHAR, MSG_AUTOMATICALLY_TERMINATE_IN_1053));
			    SendMessageToQt(&tmpObj, GUI_MSG_FACE_ENROLL_USER_POPUP_SCREEN);
			    SetFaceEnrollStopGlobleWaitTimer();
			    retVal = TRUE;
		    }
		    else
		    {
			    DPRINT(SYS_LOG, "GUI_BUTTON_BACK");
                BackEnrollmentScreen();
			    retVal = TRUE;
		    }
	    }
	    break;

    }
	return retVal;
}

static BOOL FaceEnrollmentConflict(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	BOOL retVal = FALSE;
    switch(msgId)
    {
	    default:
	    {
		    DPRINT(SYS_LOG, "Face enroll msgid %x not handled", msgId);
		    retVal = FALSE;
	    }
	    break;

	    case NO_MSG:
	    {
		    if( (NULL == dataPtr) || (length != sizeof(IDTResponseQueue_t)) )
		    {
			    DPRINT(SYS_LOG, "Invalid data received msgid %x", msgId);
			    retVal = FALSE;
			    break;
		    }
		    switch(((IDTResponseQueue_t*)dataPtr)->rplErlType)
		    {
			    default:
			    {
				    DPRINT(SYS_LOG, "Invalid Enrollment reply Receive: %u", ((IDTResponseQueue_t*)dataPtr)->rplErlType);
				    retVal = FALSE;
			    }
				    break;
		    }
	    }
	    break;

	    case GUI_BUTTON_FACE_ENRL_CONTINUE:
	    {
		    DPRINT(SYS_LOG, "Conflict Accepted so sending responce");
		    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
		    FACE_ENROLL_REQ_t tObj;
		    if(EnrollmentTempInputs.ConflictType == RPL_ERL_FROM_DVC_CAPTURED_CONFLICT)
		    {
			    tObj.requestType = USR_ENR_CAPTURE_CONFLICT_CONFIRM;
			    SetFRStreamOverlayInfo(FALSE, MSG_MAX_LANG_STRING, FALSE, FOOTER_ABORT_STOP_ENABLE, FALSE);
		    }
		    else
		    {
			    tObj.requestType = USR_ENR_ENROLL_CONFLICT_CONFIRM;
			    SetFRStreamOverlayInfo(FALSE, MSG_MAX_LANG_STRING, FALSE, FOOTER_ABORT_STOP_DISABLE, FALSE);
		    }

		    tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
		    tObj.numberOfEnrollFace = enrollInputs.faceCnt;
		    tObj.UserId = enrollInputs.userId; 
//		    ChangeFaceEnrollState(ENROLL_FACE_STATE_PROGRESS);
		    ProcessEnrFaceRequest(&tObj);
		    retVal = TRUE;
            ShowFRStreamOnDeviceDisplay(ACCESS_WAIT_RESP);// TODO Bhavin ACCESS_WAIT_RESP - ACCESS_IDLE_RESP
            ChangeFaceEnrollState(ENROLL_FACE_STATE_PROGRESS);
            SetFaceEnrollStopGlobleWaitTimer();

	    }
	    break;

	    case GUI_BUTTON_FACE_ENRL_CANCEL:
	    case GUI_BUTTON_POPUP_SCREEN_TIMEOUT:
	    {
		    //if some frames are allready processed
		    DPRINT(SYS_LOG, "Conflict Not accepted so sending responce");
		    if(EnrollmentTempInputs.faceFrameCnt != 0)
		    {
			    DPRINT(SYS_LOG, "some frames are allready processed, Sending Stop process");
			    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
			    FACE_ENROLL_REQ_t tObj;
			    tObj.requestType = USR_ENR_ENROLL_STOP;
			    tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
			    tObj.numberOfEnrollFace = enrollInputs.faceCnt;
			    tObj.UserId = enrollInputs.userId; 
			    ChangeFaceEnrollState(ENROLL_FACE_STATE_STOP);
			    ProcessEnrFaceRequest(&tObj);
			    ENROLL_FACE_MSG_INFO_TO_QT_t tmpObj;
			    tmpObj.timeOutSec = 5;
			    tmpObj.popupType = ENROLL_FACE_QT_POPUP_TYPE_USER_OTHER;
			    tmpObj.warningIconShow = FALSE;
			    snprintf(tmpObj.AlertString, sizeof(tmpObj.AlertString),
					    GetDisplayMessage(FALSE,
                            NULL_CHAR, MSG_DO_YOU_WANT_TO_ENROLL_ALREADY_CAPTURED_FACES_1066));
			    snprintf(tmpObj.CountDownString, sizeof(tmpObj.CountDownString),
					    GetDisplayMessage(FALSE,
						    NULL_CHAR, MSG_AUTOMATICALLY_ABORT_IN_1017));
			    SendMessageToQt(&tmpObj, GUI_MSG_FACE_ENROLL_USER_POPUP_SCREEN);
			    retVal = TRUE;
			    SetFaceEnrollStopGlobleWaitTimer(); 
		    }
		    else
		    {
			    //No frames are processed
			    char FaliStr[QT_MSG_SIZE] = {'\0'};
			    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
			    DPRINT(SYS_LOG, "No frames are processed so terminating enroll process");	

			    ChangeFaceEnrollState(ENROLL_FACE_STATE_ABORT);

			    strcpy(FaliStr, GetDisplayMessage(FALSE,
						    NULL_CHAR, MSG_DEVICE_ABORTED_1068));
			    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);

                            EnrollmentTempInputs.faceFrameCnt = 0;
			    // Exit enrollment mode
			    ExitEnrollmentMode();
			    retVal = TRUE;
		    }
	    
	    }
	    break;

    }
	return retVal;

}
static BOOL FaceEnrollmentAbort(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	BOOL retVal = FALSE;
    switch(msgId)
    {
	    default:
	    {
		    DPRINT(SYS_LOG, "Face enroll msgid %x not handled", msgId);
		    retVal = FALSE;
	    }
	    break;

	    case NO_MSG:
	    {
		    if( (NULL == dataPtr) || (length != sizeof(IDTResponseQueue_t)) )
		    {
			    EPRINT(SYS_LOG, "Invalid data received msgid %x", msgId);
			    retVal = FALSE;
			    break;
		    }
		    switch(((IDTResponseQueue_t*)dataPtr)->rplErlType)
		    {
			    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
                char FaliStr[QT_MSG_SIZE] = {'\0'};
			    case RPL_ERL_FROM_DVC_ERR :
			    {
				    if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_INVALID_REQUEST)
				    {
					    EPRINT(SYS_LOG, "Error Received Invalid request");
					    char FaliStr[QT_MSG_SIZE] = {'\0'};
					    strcpy(FaliStr, GetDisplayMessage(FALSE,
								    NULL_CHAR, MSG_INVALID_REQUEST_SENT_1056));
					    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
					    // Exit enrollment mode
					    ExitEnrollmentMode();
					    retVal = FALSE;
				    }
			    }
			    break;
			    case RPL_ERL_FROM_DVC_ABORT_SUCCESS:
			    {

				    DPRINT(SYS_LOG, "IDS aboarted Successfully , aborting device");
				    retVal = TRUE;
			    }
			    break;
			    default:
			    {
				    DPRINT(SYS_LOG, "Invalid Enrollment reply Receive: %u", ((IDTResponseQueue_t*)dataPtr)->rplErlType);
				    retVal = FALSE;
			    }
				    break;
		    }
	    }
	    break;

	    case GUI_BUTTON_FACE_ENRL_CONTINUE:
	    case GUI_BUTTON_POPUP_SCREEN_TIMEOUT:
	    {
		    char FaliStr[QT_MSG_SIZE] = {'\0'};
		    DPRINT(SYS_LOG, "Abort selected or timout ouccured so sending responce");
		    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
		    if(EnrollmentTempInputs.ConflictType != RPL_ERL_FROM_DVC_USER_CONFLICT)
		    {

			    ChangeFaceEnrollState(ENROLL_FACE_STATE_ABORT);

		    }

		    strcpy(FaliStr, GetDisplayMessage(FALSE,
					    NULL_CHAR, MSG_DEVICE_ABORTED_1068));
		    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);

                    EnrollmentTempInputs.faceFrameCnt = 0;
		    // Exit enrollment mode
		    ExitEnrollmentMode();
		    retVal = TRUE;
	    }
	    break;

	    case GUI_BUTTON_FACE_ENRL_CANCEL:
	    {
		    /*Close dialog and
		    Handle request based on previous state*/
		    DPRINT(SYS_LOG, "Enrollment aboart cancled so sending responce");
		    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
		    FACE_ENROLL_REQ_t tObj;
		    tObj.requestType = USR_ENR_ENROLL_IMAGE;
		    tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
		    tObj.numberOfEnrollFace = enrollInputs.faceCnt;
		    tObj.UserId = enrollInputs.userId; 
//		    ChangeFaceEnrollState(ENROLL_FACE_STATE_PROGRESS);
		    SetFRStreamOverlayInfo(FALSE, MSG_MAX_LANG_STRING, FALSE, FOOTER_ABORT_STOP_ENABLE, FALSE);
		    struct 	stat FileInfo;
		    if((stat(FR_DST_IMAGE, &FileInfo) == 0))
		    { 
                DPRINT(SYS_LOG, "FR_DST_IMAGE present");
			    ProcessEnrFaceRequest(&tObj);
		    }
		    else
		    {
			    //TODO: CALL API HERE 
			    DPRINT(SYS_LOG, "FR_DST_IMAGE Not present");
			    LoadCameraImageWaitCountDownTimer();
		    }
		    SetFaceEnrollStopGlobleWaitTimer();
            ShowFRStreamOnDeviceDisplay(ACCESS_WAIT_RESP);
            ChangeFaceEnrollState(ENROLL_FACE_STATE_PROGRESS);
//            ShowFRStreamOnDeviceDisplayTimer(ACCESS_WAIT_RESP); //TODO Bhavin ACCESS_WAIT_RESP - ACCESS_WAIT_RESP
		    retVal = TRUE;
	    }
	    break;

	    //CONTINUE AFTER USER CONFLICT WHEN USER CLICK ON ABORT BUTTON ON USER CONFLICT SCREEN
	    case GUI_BUTTON_FACE_ENRL_CONTINUE_ABORT:
	    {
		    char FaliStr[QT_MSG_SIZE] = {'\0'};
            strcpy(FaliStr, GetDisplayMessage(FALSE,
                                    NULL_CHAR, MSG_DEVICE_ABORTED_1068));
		    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
		    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
		    DPRINT(SYS_LOG, "Device Aboart selected");
		    // Change source of enrollment to make sure it exits enrollment state
		    // Exit enrollment mode
		    ExitEnrollmentMode();
		    // User response must be set idle to overcome incomplete process' response
		    retVal = TRUE;
	    }
	    break;
    }
	return retVal;
	
}

static BOOL FaceEnrollmentStop(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	BOOL retVal = FALSE;
    switch(msgId)
    {
	    default:
	    {
		    DPRINT(SYS_LOG, "Face enroll msgid %x not handled", msgId);
		    retVal = FALSE;
	    }
	    break;

	    case NO_MSG:
	    {
		    if( (NULL == dataPtr) || (length != sizeof(IDTResponseQueue_t)) )
		    {
			    DPRINT(SYS_LOG, "Invalid data received msgid %x", msgId);
			    retVal = FALSE;
			    break;
		    }
		    switch(((IDTResponseQueue_t*)dataPtr)->rplErlType)
		    {
			    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
			    case RPL_ERL_FROM_DVC_ERR :
			    {
				    if(((IDTResponseQueue_t*)dataPtr)->errorCode == ERR_INVALID_REQUEST)
				    {
					    EPRINT(SYS_LOG, "Error Received Invalid request");
					    char FaliStr[QT_MSG_SIZE] = {'\0'};
					    strcpy(FaliStr, GetDisplayMessage(FALSE,
                                                                    NULL_CHAR, MSG_INVALID_REQUEST_SENT_1056));
					    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);
					    // Exit enrollment mode
					    ExitEnrollmentMode();
				    	    retVal = FALSE;
				    }
			    }
			    break;
			    case RPL_ERL_FROM_DVC_STOP_SUCCESS:
			    {

                    EPRINT(SYS_LOG, "Enrolled Process stopped success");
                    retVal = TRUE;
			    }
			    break;
			    default:
			    {
				    DPRINT(SYS_LOG, "Invalid Enrollment reply Receive: %u", ((IDTResponseQueue_t*)dataPtr)->rplErlType);
				    retVal = FALSE;
			    }
				    break;
		    }
	    }
	    break;

	    case GUI_BUTTON_FACE_ENRL_CONTINUE:
	    {
            DPRINT(SYS_LOG, "Enrollface selected so sending responce");
		    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);
		    FACE_ENROLL_REQ_t tObj;
		    tObj.UserId = enrollInputs.userId; 
		    tObj.requestType = USR_ENR_ENROLL_FACES;
		    tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
		    tObj.numberOfEnrollFace = enrollInputs.faceCnt;
		    ChangeFaceEnrollState(ENROLL_FACE_STATE_PROGRESS);
		    ProcessEnrFaceRequest(&tObj);
		    ENROLL_FACE_MSG_INFO_TO_QT_t tmpObj;
		    tmpObj.timeOutSec = 0;
		    tmpObj.popupType = ENROLL_FACE_QT_POPUP_TYPE_PROCESSING;
		    tmpObj.warningIconShow = FALSE;
		    snprintf(tmpObj.AlertString, sizeof(tmpObj.AlertString),
				    GetDisplayMessage(FALSE,
					    NULL_CHAR,MSG_PROCESSING_467));
		    snprintf(tmpObj.CountDownString, 512, "%s","");
		    SendMessageToQt(&tmpObj, GUI_MSG_FACE_ENROLL_USER_POPUP_SCREEN);
		    retVal = TRUE;
		    SetFaceEnrollStopGlobleWaitTimer();
	    }
	    break;

	    case GUI_BUTTON_FACE_ENRL_CANCEL:
	    case GUI_BUTTON_POPUP_SCREEN_TIMEOUT:
	    {
		    char FaliStr[QT_MSG_SIZE] = {'\0'};
		    DPRINT(SYS_LOG, "Enroll Face rejected so sending responce");
		    DeleteTimer(&enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle);

		    ChangeFaceEnrollState(ENROLL_FACE_STATE_ABORT);

		    strcpy(FaliStr, GetDisplayMessage(FALSE,
					    NULL_CHAR, MSG_DEVICE_ABORTED_1068));
		    SetAudioVisual(USER_RESP_ENROLL_FAIL, INTERNAL_PORT, FaliStr);

                    EnrollmentTempInputs.faceFrameCnt = 0;
		    // Exit enrollment mode
		    ExitEnrollmentMode();
		    retVal = TRUE;
	    }
	    break;

    }
	return retVal;
	
}

//*****************************************************************************
//	IsCardWritePsbl
//	Param:
//		IN :	cardType	- card type
//		OUT:	none
//	Returns:	YES/NO
//	Description:
//			It checks if given card has enough memory to store required data.
//
//*****************************************************************************
static BOOL IsCardWritePsbl(CARD_TYPE_e cardType)
{
	if((IsSmartCard(cardType)) && (IsCustomSmartCardConfigEnabled(cardType) == YES))
	{
		return(IsCustomCardWritePsbl(cardType, &(enrollInputs.cardWriteFields)));
	}

	switch(cardType)
	{
		// Nothing can be written into read only/unknown/mifare ultra light card
		default:
		{
			return NO;
		}

		// All info can be written into HID 16K2, 16K16, MIFARE 4K
		case MIFARE_4K:
		case HID_16K2:
		case HID_16K16:
		case MIFARE_DESFIRE_2K:
		case MIFARE_DESFIRE_4K:
		case MIFARE_DESFIRE_8K:
		{
			break;
		}

		// Security, personalization info and 1 FP can be written into MIFARE 1K
		case MIFARE_1K:
		{
			if(enrollInputs.cardWriteFields.noOfTemplates > 1)
			{
				return NO;
			}
			break;
		}

		// Only security info can be written into HID 2K2
		case HID_2K2:
		{
			if(IsCardWriteRequired(CARD_FIELD_PERS_INFO, &(enrollInputs.cardWriteFields)) == YES)
			{
				return NO;
			}
			if(enrollInputs.cardWriteFields.noOfTemplates)
			{
				return NO;
			}
			break;
		}
	}

	return YES;
}

//*****************************************************************************
//	DcEnrollmentStateFn
//	Param:
//		IN :	msgId 	- Message which needs to be processed
//				dataPtr - Pointer to data of the message
//				length 	- Length of the message
//		OUT:	none
//	Returns:	none
//	Description:
//				It handles FSM for enrollment process of user or special
//				functions.
//
//*****************************************************************************
void DcEnrollmentStateFn(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	// If next credential wait timer or new enrollment timer is active, it means
	// message to user is being displayed.
	// Do not process any event for enrollment during this time so display won't be updated.
	if( (enrollActivity.waitNewEnrollmentTmrHandle == INVALID_TIMER_HANDLE) &&
		(enrollActivity.waitNextCredEnrollTmrHandle == INVALID_TIMER_HANDLE) )
	{
		// If message is processed in enrollment FSM then no point in going ahead
		if( (*enrollmentFsmFuncPtr[enrollActivity.state]) (msgId, dataPtr, length) == TRUE)
		{
			return;
		}
	}

	// Message was not related to Enrollment, check for escape or
	// pass it to default message handler
	switch(msgId)
	{
		default:
		{
			DcDefaultMsgHandler(msgId, dataPtr, length);
			break;
		}

		case GUI_ALNUM_KEEP_ALIVE:
		case GUI_BUTTON_KEEP_ALIVE:
		case GUI_BUTTON_FIELD_CLEAR:
		{
			// Reload display idle timer
			SetLcdIdleTimer(ENROLLMENT_TIMER);

			// Reload enrollment timer used  to get user input
			if(SetGetEnrollInputTimer(ENROLLMENT_TIMER) == FAIL)
			{
				EPRINT(DC_LOG, "Unable to set 'get inputs for enrollment' timer");
			}
			break;
		}

		// 'escape' key handling
		case GUI_BUTTON_CANCEL:
		case GUI_BUTTON_HOME:
		{
			// Change source of enrollment to make sure it exits enrollment state
			enrollInputs.source = ENROLL_SOURCE_ACMS;
			// Exit enrollment mode
			ExitEnrollmentMode();

			// User response must be set idle to overcome incomplete process' response
			SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
			break;
		}

		case GUI_BUTTON_BACK:
		{
			BackEnrollmentScreen();
			break;
		}
	}
}

//*****************************************************************************
//	SendMsgEnrollByScan
//	Param:
//		IN :	userIdx	- user index
//		OUT:	none
//	Returns:	none
//	Description:
//			Sends message to DC to "enroll finger by scan".
//
//*****************************************************************************
static void SendMsgEnrollByScan(UINT16 userIdx)
{
	// Send message to DC to "enroll finger by scan"
	ENROLL_BY_SCAN_CMD_DATA_t enrollCmdData;

	enrollCmdData.readerId = enrollActivity.readerId;
	enrollCmdData.enrollMethod = ENROLL_METHOD_CHECK_FINGER;
	enrollCmdData.fprUserId = GetUserId(userIdx);
	if(ProcessReaderCommand(MSG_ENROLL_BY_SCAN_CMD, &enrollCmdData, sizeof(ENROLL_BY_SCAN_CMD_DATA_t)) == SUCCESS)
	{
		DPRINT(DC_LOG,"Sent 'ENROLL FINGER BY SCAN' command for User [%s]", GetAlphaNumId(userIdx));
	}
	else
	{
		EPRINT(DC_LOG,"Unable to send 'ENROLL FINGER BY SCAN' command for \
													User [%s]", GetAlphaNumId(userIdx));
	}
}

//*****************************************************************************
//	SendMsgDeleteSingleUserFp
//	Param:
//		IN :	userIdx
//		OUT:	none
//	Returns:	SUCCESS	- if message has been sent successfully
//				FAIL	- if message sent has been failed
//	Description:
//				Sends message to reader to delete finger print template of a
//				given user.
//
//*****************************************************************************
static BOOL SendMsgDeleteSingleUserFp(READER_ID_e readerId, UINT16 userIdx)
{
	DELETE_USER_FP_CMD_DATA_t deleteUserFp;

	deleteUserFp.readerId = readerId;
	deleteUserFp.deleteMethod = DELETE_SINGLE_USER;
	deleteUserFp.fprUserId = GetUserId(userIdx);
	deleteUserFp.subId = 0;

	FpTransPara.usrId = deleteUserFp.fprUserId;

	if(ProcessReaderCommand(MSG_DELETE_FP_TEMPLATES_CMD, &deleteUserFp,
									sizeof(DELETE_USER_FP_CMD_DATA_t)) == SUCCESS)
	{
		DPRINT(DC_LOG,"Sent 'DELETE USER FINGER' command for User [%s]", GetAlphaNumId(userIdx));
		return SUCCESS;
	}
	else
	{
		EPRINT(DC_LOG,"Unable to send 'DELETE USER FINGER' command for User [%s]", GetAlphaNumId(userIdx));
		return FAIL;
	}
}

//*****************************************************************************
//	SendMsgEnrollTemplate
//	Param:
//		IN :	readerId	- reader index
//				userIdx 	- index of user to be enrolled on FPR
//				fpIdx		- finger index
//				enrollMethod- to check for duplicate finger or not
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//			Sends message to single or all DC to enroll user fp template.
//
//*****************************************************************************
static BOOL SendMsgEnrollTemplate(READER_ID_e readerId, UINT16 userIdx,
								UINT8 fpIdx, FP_ENROLL_METHOD_e enrollMethod, ENROLL_SOURCE_e enrollmentSource)
{
	ENROLL_BY_TEMPLATE_CMD_DATA_t enrollCmdData;

	if( (GetReaderRespPort(readerId) == INTERNAL_PORT) &&
			(DcInfo.internalFpMemFullF == TRUE) )
	{
		EPRINT(DC_LOG,"Unable to send 'ENROLL FINGER BY TEMPLATE' command for User [%s] on Internal Port", GetAlphaNumId(userIdx));
		return FAIL;
	}
	else if( (GetReaderRespPort(readerId) == EXTERNAL_PORT) &&
				(DcInfo.externalFpMemFullF == TRUE) )
	{
		EPRINT(DC_LOG,"Unable to send 'ENROLL FINGER BY TEMPLATE' command for userIdx [%s] on External Port", GetAlphaNumId(userIdx));
		return FAIL;
	}

	enrollCmdData.readerId = readerId;
	enrollCmdData.fprUserId = GetUserId(userIdx);
	enrollCmdData.noOfEnroll = SysAdvPara.templatesPerFinger + 1;
	enrollCmdData.enrollMethod = enrollMethod;
	enrollCmdData.fpIndex = fpIdx;
	enrollCmdData.enrollmentSource = enrollmentSource;

	if(ReadUserFpFromFile(userIdx, fpIdx, enrollCmdData.template) == FAIL)
	{
		EPRINT(DC_LOG,"Unable to read finger template for User [%s]", GetAlphaNumId(userIdx));
		return FAIL;
	}

	if(ProcessReaderCommand(MSG_ENROLL_BY_TEMPLATE_CMD, &enrollCmdData,
								sizeof(ENROLL_BY_TEMPLATE_CMD_DATA_t)) == SUCCESS)
	{
		DPRINT(DC_LOG,"Sent 'ENROLL FINGER BY TEMPLATE' command for User [%s], fpIdx[%d]", GetAlphaNumId(userIdx), fpIdx);
		return SUCCESS;
	}
	else
	{
		EPRINT(DC_LOG,"Unable to send 'ENROLL FINGER BY TEMPLATE' command for User [%s], fpIdx[%d]", GetAlphaNumId(userIdx), fpIdx);
		return FAIL;
	}
}

//*****************************************************************************
//	SendMsgEraseSmartCard
//	Param:
//		IN :	readerId	- reader index
//				cardType 	- card type
//		OUT:	none
//	Returns:	none
//	Description:
//			Sends message to DC to erase smart card info.
//
//*****************************************************************************
static void SendMsgEraseSmartCard(UINT8 readerId, CARD_TYPE_e cardType)
{
	//Send message to DC to erase smart card info
	ERASE_SMART_CARD_CMD_DATA_t cmdData;
	cmdData.readerId = readerId;
	cmdData.cardType = cardType;
	ProcessReaderCommand(MSG_ERASE_SMART_CARD_CMD, &cmdData, sizeof(ERASE_SMART_CARD_CMD_DATA_t));
}

//*****************************************************************************
//	SendMsgChangeSmartCardKey
//	Param:
//		IN :	readerId	- reader index
//				cardType 	- card type
//		OUT:	none
//	Returns:	none
//	Description:
//			Sends message to DC to change key of all known sector of smart card.
//*****************************************************************************
void SendMsgChangeSmartCardKey(UINT8 readerId, CARD_TYPE_e cardType)
{
	CHANGE_SMART_CARD_KEY_CMD_DATA_t cmdData;
	cmdData.readerId = readerId;
	cmdData.cardType = cardType;
	ProcessReaderCommand(MSG_CHANGE_SMART_CARD_KEY_CMD, &cmdData, sizeof(CHANGE_SMART_CARD_KEY_CMD_DATA_t));
}

//*****************************************************************************
//	SendMsgCancelEnrollment
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			Sends message to DC to cancel enrollment.
//
//*****************************************************************************
static void SendMsgCancelEnrollment(void)
{
	//Send message to DC to cancel enrollment
	ABORT_FINGER_SCAN_CMD_DATA_t cmdData;

	cmdData.readerId = enrollActivity.readerId;
	ProcessReaderCommand(MSG_ABORT_FINGER_SCAN_CMD, &cmdData,
									sizeof(ABORT_FINGER_SCAN_CMD_DATA_t ));
}

//*****************************************************************************
//	SendMsgWriteDataIntoCard
//	Param:
//		IN :	msgId	- message id
//		OUT:	none
//	Returns:	SUCCESS if message sent else FAIL
//	Description:
//				It sends specified message to door controller reader.
//
//*****************************************************************************
static BOOL SendMsgWriteDataIntoCard(SLAVE_MSG_ID_e msgId)
{
	switch(msgId)
	{
		case MSG_WRITE_FP_CMD:
		{
			WRITE_FP_CMD_DATA_t cmdData;
			UINT8 FpIdx1 =0,FpIdx2 =1;
			cmdData.readerId = enrollActivity.readerId;
			cmdData.cardType = enrollActivity.cardType;

			cmdData.template1F = SET;
			cmdData.template2F = CLEAR;
			
			FpIndexForWriteSmartCard(enrollActivity.userIdx,&FpIdx1,&FpIdx2);
			if(INVALID_FP_IDX == FpIdx1)
			{
			    return FAIL;
			}
			
			if(ReadUserFpFromFile(enrollActivity.userIdx, FpIdx1, cmdData.template) == FAIL)
			{
				return FAIL;
			}

			// If both the templates are to be written into card
			if(enrollInputs.cardWriteFields.noOfTemplates == 2)
			{
				cmdData.template2F = SET;

				if(INVALID_FP_IDX == FpIdx2)
				{
				    return FAIL;
				}
				if(ReadUserFpFromFile(enrollActivity.userIdx, FpIdx2,
								&cmdData.template[MAX_SIZE_SINGLE_SFM_TEMPLATE]) == FAIL)
				{
					return FAIL;
				}
			}

			ProcessReaderCommand(MSG_WRITE_FP_CMD, (UINT8PTR)&cmdData, sizeof(WRITE_FP_CMD_DATA_t));
			return SUCCESS;
		}
		break;

		case MSG_WRITE_SEC_INFO_CMD:
		{
			WRITE_SEC_INFO_CMD_DATA_t cmdData;

			cmdData.readerId = enrollActivity.readerId;
			cmdData.cardType = enrollActivity.cardType;
			cmdData.facCodeF = enrollInputs.cardWriteFields.facCodeF;
			cmdData.addlSecCodeF = enrollInputs.cardWriteFields.addlSecCodeF;
			cmdData.userIdF = enrollInputs.cardWriteFields.userIdF;

			cmdData.facCode = SysAdvPara.facilityCode;
			cmdData.addlSecCode = SysAdvPara.addlSecurity.code;
			cmdData.userId = enrollInputs.userId;

			ProcessReaderCommand(MSG_WRITE_SEC_INFO_CMD, (UINT8PTR)&cmdData, sizeof(WRITE_SEC_INFO_CMD_DATA_t));
			return SUCCESS;
		}
		break;

		case MSG_WRITE_PERS_INFO_CMD:
		{
			WRITE_PERS_INFO_CMD_DATA_t cmdData;

			cmdData.readerId = enrollActivity.readerId;
			cmdData.cardType = enrollActivity.cardType;
			cmdData.userNameF = enrollInputs.cardWriteFields.userNameF;
			cmdData.designationF = enrollInputs.cardWriteFields.designationF;
			cmdData.branchF = enrollInputs.cardWriteFields.branchF;
			cmdData.departmentF = enrollInputs.cardWriteFields.departmentF;
			cmdData.bloodGrpF = enrollInputs.cardWriteFields.bloodGrpF;
			cmdData.emrgncyContactF = enrollInputs.cardWriteFields.emrgncyContactF;
			cmdData.medicalHistoryF = enrollInputs.cardWriteFields.medicalHistoryF;

			memcpy(cmdData.userName, enrollInputs.cardPersonalInfo.userName, CARD_FIELD_LEN_USER_NAME);
			memcpy(&cmdData.designation, enrollInputs.cardPersonalInfo.designation, CARD_FIELD_LEN_DESIGNATION);
			memcpy(&cmdData.branch, enrollInputs.cardPersonalInfo.branch, CARD_FIELD_LEN_BRANCH);
			memcpy(&cmdData.department, enrollInputs.cardPersonalInfo.department, CARD_FIELD_LEN_DEPARTMENT);
			memcpy(&cmdData.bloodGrp, enrollInputs.cardPersonalInfo.bloodGrp, CARD_FIELD_LEN_BLOOD_GRP);
			memcpy(&cmdData.emrgncyContact, enrollInputs.cardPersonalInfo.emrgncyContact, CARD_FIELD_LEN_CONTCAT_NO);
			memcpy(&cmdData.medicalHistory, enrollInputs.cardPersonalInfo.medicalHistory, CARD_FIELD_LEN_MED_HISTORY);

			ProcessReaderCommand(MSG_WRITE_PERS_INFO_CMD, (UINT8PTR)&cmdData, sizeof(WRITE_PERS_INFO_CMD_DATA_t));
			return SUCCESS;
		}
		break;

		case MSG_WRITE_CARD_ID_CMD:
		{
			WRITE_CARD_ID_CMD_DATA_t cmdData;

			cmdData.readerId = enrollActivity.readerId;
			cmdData.cardType = enrollActivity.cardType;
			cmdData.cardIdF = enrollInputs.cardWriteFields.cardIdF;

			if(enrollActivity.cardCnt == 0)
			{
				if(GetUserCardId(enrollActivity.userIdx, 0) != 0)
				{
					cmdData.cardId = GetUserCardId(enrollActivity.userIdx, 0);
				}
				else if(GetUserCardId(enrollActivity.userIdx, 1) != 0)
				{
					cmdData.cardId = GetUserCardId(enrollActivity.userIdx, 1);
				}
				else
				{
					cmdData.cardId = 0;
				}
			}
			else if(enrollActivity.cardCnt == 1)
			{
				if(GetUserCardId(enrollActivity.userIdx, 1) != 0)
				{
					cmdData.cardId = GetUserCardId(enrollActivity.userIdx, 1);
				}
				else if(GetUserCardId(enrollActivity.userIdx, 0) != 0)
				{
					cmdData.cardId = GetUserCardId(enrollActivity.userIdx, 0);
				}
				else
				{
					cmdData.cardId = 0;
				}
			}

			ProcessReaderCommand(MSG_WRITE_CARD_ID_CMD, (UINT8PTR)&cmdData, sizeof(WRITE_CARD_ID_CMD_DATA_t));
			return SUCCESS;
		}
		break;

		default:
			return FAIL;
	}
}

//*****************************************************************************
//	SendMsgGetUserTemplateCnt()
//	Param:
//		IN :	userIdx - index of user
//		OUT:	none
//	Returns:	none
//	Description:
//				It sends message to finger print reader to read no. of enrolled
//				templates for given user.
//
//*****************************************************************************
static void SendMsgGetUserTemplateCnt(UINT16 userIdx)
{
	USER_TEMPLATE_CNT_CMD_DATA_t cmdData;
	cmdData.readerId = enrollActivity.readerId;
	cmdData.fprUserId = GetUserId(userIdx);
	ProcessReaderCommand(MSG_USER_TEMPLATE_CNT_CMD, (UINT8PTR)&cmdData,
								sizeof(USER_TEMPLATE_CNT_CMD_DATA_t));
}

//*****************************************************************************
//	SendMsgReadUserTemplate()
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//				It sends message to finger print reader to read user's template.
//
//*****************************************************************************
static void SendMsgReadUserTemplate(void)
{
	// Send message to read template from reader
	READ_TEMPLATE_CMD_DATA_t readFpData;

	readFpData.readerId = enrollActivity.readerId;

	// Read best of the two template enrolled
	if(enrollActivity.enrolledTemplateIdx)
	{
		readFpData.fprUserId = GetUserId(enrollActivity.userIdx);
	}
	else
	{
		readFpData.fprUserId = 0; // read latest fp;
	}
	readFpData.subId = enrollActivity.enrolledTemplateIdx;

	ProcessReaderCommand(MSG_READ_TEMPLATE_CMD, (UINT8PTR)&readFpData, sizeof(READ_TEMPLATE_CMD_DATA_t));
}

//*****************************************************************************
//	ExitEnrollmentMode
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//				If enrollment was triggered through ACMS/Webjeeves, it will
//				exit enrollment process and switch DC task FSM to IDLE state.
//				If enrollment was triggered through special function then it
//				will trigger new enrollment (enrollment for next user or spl fn).
//
//*****************************************************************************
static void ExitEnrollmentMode(void)
{
    ClearAllRequestNode(); //clear all idt request Node.
    TerminateEnrollmentActivity();

    enrollActivity.faceEnrollActivity.prevstate = ENROLL_FACE_STATE_END;
    ChangeFaceEnrollState(ENROLL_FACE_STATE_END);
	isCurrentEnrllmentSourceQT = FALSE;
	
	EnrollmentCmdRunningF = FALSE;

	if(enrollInputs.enrollmentType == ENROLL_FACE_ONLY)
	{
//		EnrollmentTempInputs.enrollRepeatF = 0;
		EnrollmentTempInputs.faceFrameCnt = 0;
        if(FRServerConfig.faceCapturing == FACE_SERVER_TAP_AND_GO)
        {
            StopFRStreamingOnDeviceIdleScreen(TRUE);
        }
        else
        {
            StopFRStreamingOnDeviceIdleScreen(FALSE);
        }
        EnrollFaceHandlingF = FALSE;

	}
	if( (enrollInputs.enrollmentType == ENROLL_BIOMETRIC_ONLY) ||
		(enrollInputs.enrollmentType == ENROLL_BIOMETRIC_THEN_CARD) 
		||(enrollInputs.enrollmentType == ENROLL_DURESS_FP) )
	{
		// Abort scanning if required
		SendMsgCancelEnrollment();
	}

	if(enrollInputs.source == SOURCE_SPECIAL_FUNC)
	{
		// Set timer for how long to display user message before starting enrollment again
		if(SetWaitNewEnrollmentTimer() == FAIL)
		{
			EPRINT(DC_LOG, "Unable to start 'wait before new enrollment'");
		}
	}
	else
	{
		//Change DC slave FSM state to IDLE state
		SwitchDcToIdleState();
	}
    unlink(FR_DST_IMAGE);
}

//*****************************************************************************
//	DeleteUserFp
//	Param:
//		IN :	userIdx	- user index
//		OUT:	none
//	Returns:	SUCCESS/FAIL/WAIT
//	Description:
//			It deletes user finger print form door readers and removes file
//			stored for given user.
//
//*****************************************************************************
UINT8 DeleteUserFp(UINT16 userIdx)
{
	READER_ID_e readerId;
	UINT8 		status = SUCCESS;

	if((DcInfo.curState == DC_ENROLLMENT_STATE) && (enrollActivity.state != ENROLL_ADD_OR_ASSIGNUSER_STATE))
	{
		status = WAIT;
	}
	else
	{
		// If none of the FPR is configured in a system, we return SUCCESS
		// If command can not sent to any active FPR, we return FAIL
		for(readerId = 0; readerId < MAX_READERS; readerId++)
		{
			if(SysBasicPara.reader[readerId].type == SUP_FP_3030_READER)
			{
				if(SendMsgDeleteSingleUserFp(readerId, userIdx) == FAIL)
				{
					status = FAIL;
				}
			}
		}
	}

	DeleteSingleUserFpFiles(userIdx, ALL_USER_FP);
	//Delete Duress FP Index from User Config
	DeleteUserDuressFp(userIdx);
	return status;
}

//*****************************************************************************
//	TerminateEnrollmentActivity
//	Param:
//		IN : none
//		OUT: none
//	Returns:
//		none
//	Description:
//		This function cleans up enrollment activity to abnormally exit enrollment
//		state.
//
//*****************************************************************************
void TerminateEnrollmentActivity(void)
{
	// Terminate all timers which may be running in this state
	DeleteTimer(&enrollActivity.waitNextCredEnrollTmrHandle);
	DeleteTimer(&enrollActivity.exitTmrHandle);
	DeleteTimer(&enrollActivity.waitNewEnrollmentTmrHandle);
	DeleteTimer(&enrollActivity.getEnrollInputTmrHandle);
    DeleteTimer(&(enrollActivity.exitMobileTmrHandle));
	
	//face enrollment handler timer delete
    DeleteTimer(&(enrollActivity.faceEnrollActivity.abortwaitTmrHandle));
    DeleteTimer(&(enrollActivity.faceEnrollActivity.gbloblewaitRespTmrHandle));
    DeleteTimer(&(enrollActivity.faceEnrollActivity.stopwaitTmrHandle));
    DeleteTimer(&(enrollActivity.faceEnrollActivity.terminateTmrHandle));

    if(DcInfo.curState == DC_ENROLLMENT_STATE && enrollInputs.enrollmentType == ENROLL_FACE_ONLY)
    {
        if(FaceEnrollWaitTimerHandler != INVALID_TIMER_HANDLE)
            DeleteTimer(&(FaceEnrollWaitTimerHandler));
        if(DcInfo.displayFRStreamTimer != INVALID_TIMER_HANDLE)
            DeleteTimer(&DcInfo.displayFRStreamTimer);
        StartEnrollAbrtReq();
    }

    enrollActivity.faceEnrollActivity.prevstate = ENROLL_FACE_STATE_END;
    ChangeFaceEnrollState(ENROLL_FACE_STATE_END);
	isCurrentEnrllmentSourceQT = FALSE;
	EnrollmentTempInputs.faceFrameCnt = 0;

//    if(FrMultiReqF == FALSE)
//    {
//        FrSocketCloseConnection(FALSE, FALSE);
//    }
    
}

//*****************************************************************************
//	GetUserIdxBeingEnrolled()
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	userIdx- index of user that is being enrolled now
//	Description:
//			It gives user index that is being enrolled now.
//
//*****************************************************************************
UINT32 GetUserIdxBeingEnrolled(void)
{
	return (enrollActivity.userIdx);
}

//*****************************************************************************
//	GetSplFnIdxBeingEnrolled()
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	splFnIdx- index of special function that is being enrolled now
//	Description:
//			It gives special function index that is being enrolled now.
//
//*****************************************************************************
SPECIAL_FUNCTIONS_e GetSplFnIdxBeingEnrolled(void)
{
	return (enrollInputs.splFnIdx);
}

//*****************************************************************************
//	GetEnrollmentCategory()
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	category- user or speacial function
//	Description:
//			It gives enrollment category for which enrollment is going on.
//
//*****************************************************************************
ENROLL_CATEGORY_e GetEnrollmentCategory(void)
{
	return (enrollInputs.category);
}

//*****************************************************************************
//	SwitchDcToEnrollmentState()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function switches DCTask FSM to Enrollment state and
//				initializes required parameters.
//
//*****************************************************************************
static void SwitchDcToEnrollmentState(void)
{
	CleanUpTempUserFP();
	// Change DC slave FSM state to ENROLLMENT state
	DcInfo.curState = DC_ENROLLMENT_STATE;
	DeleteTimer(&(DcInfo.interDigitTmrHandle[INTERNAL_PORT]));
}

//*****************************************************************************
//	IsDuplicateCard()
//	Param:
//		IN :	doorId
//				card id
//		OUT:	None
//	Returns:	TRUE	- if duplicate card
//				FALSE	- if not duplicate card
//	Description:
//				It check for duplicate card. It searches first in special
//				function database and then in user database.
//
//*****************************************************************************
static BOOL IsDuplicateCard(UINT64 cardId)
{
	UINT16				userIdx;
	SPECIAL_FUNCTIONS_e	splFnIdx;

	if(IsSplFnCard(cardId, &splFnIdx, NULL) == YES)
	{
		SetAudioVisual(USER_RESP_DUPL_USER_SPL_FN_CARD, INTERNAL_PORT, NULL);
		return TRUE;
	}

	if(IsCardValid(cardId, &userIdx, NULL) == YES)
	{
		SetAudioVisual(USER_RESP_DUPL_USER_CARD, INTERNAL_PORT, NULL);
		return TRUE;
	}
	return FALSE;
}

//*****************************************************************************
//	EnrollUserFpOnAllFpr
//	Param:
//		IN :	userIdx		- user index
//				fpIdx		- index of finger print to be enrolled (0 to max supported - 1)
//		OUT:	none
//	Returns:	none
//	Description:
//			It enrolls user finger print on all finger print readers.
//	NOTE:	It is prerequisite that user's finger template is already saved in
//			database.
//
//*****************************************************************************
UINT8 EnrollUserFpOnAllFpr(UINT16 userIdx, UINT8 fpIdx, ENROLL_SOURCE_e enrollmentSource)
{
	READER_ID_e readerId;
	BOOL 		status = SUCCESS;

	if(DcInfo.curState == DC_ENROLLMENT_STATE && (enrollActivity.state != ENROLL_ADD_OR_ASSIGNUSER_STATE))
	{
		return WAIT;
	}

	// If none of the FPR is configured in a system, we return SUCCESS
	// If command can not sent to any active FPR, we return FAIL
	for(readerId = 0; readerId < MAX_READERS; readerId++)
	{
		if(SysBasicPara.reader[readerId].type == SUP_FP_3030_READER)
		{
			if(SendMsgEnrollTemplate(readerId, userIdx, fpIdx, ENROLL_METHOD_ADD_NEW, enrollmentSource) == FAIL)
			{
				status = FAIL;
			}
		}
	}

	return status;
}

//*****************************************************************************
//	BackEnrollmentScreen()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function loads previous enrollment screen.
//*****************************************************************************
static void BackEnrollmentScreen(void)
{
	switch(currentEnrollScreen)
	{
		default:
			break;

		case ENROLL_USER_FACE_SCREEN:
		{
			if(enrollInputs.source == SOURCE_SPECIAL_FUNC)
			{
                //EnrollmentTempInputs.enrollRepeatF = FALSE;
				EnrollmentCmdRunningF = FALSE;
				EnrollFaceHandlingF = FALSE;
				SetAudioVisual(USER_RESP_BUZ_AUDIO_IDLE, INTERNAL_PORT, NULL);
				TerminateEnrollmentActivity();
                if(FRServerConfig.faceCapturing == FACE_SERVER_TAP_AND_GO)
                {
                    StopFRStreamingOnDeviceIdleScreen(TRUE);
                }
                else
                {
                    StopFRStreamingOnDeviceIdleScreen(FALSE);
                }
				AskForEnrollmentInput(0);
			}
			else
			{
				// Change source of enrollment to make sure it exits enrollment state
                enrollInputs.source = ENROLL_SOURCE_ACMS;
				// Exit enrollment mode
				ExitEnrollmentMode();
                if(FRServerConfig.faceCapturing == FACE_SERVER_FREE_SCAN)
                {
                    DPRINT(DC_LOG,"### DEBUG FRStreamDisplayF = TRUE");
                    FRStreamDisplayF = TRUE;
                }
				// User response must be set idle to overcome incomplete process' response
				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
			}
		}
		break;
		case ENROLL_USER_SCREEN:
		case ENROLL_SPL_FN_CARD_SCREEN:
		{
			if(DcInfo.splFnThroughCard == FALSE)
			{
				EnrollmentTempInputs.enrollRepeatF = FALSE;
                EnrollmentTempInputs.faceCnt=1;
				TerminateEnrollmentActivity();
				SwitchDcToMenuState();
				DisplayMenuScreen(MENU_ENROLLMENT_SCREEN);
			}
			else
			{
				// Change source of enrollment to make sure it exits enrollment state
				enrollInputs.source = ENROLL_SOURCE_ACMS;
				// Exit enrollment mode
				ExitEnrollmentMode();
				// User response must be set idle to overcome incomplete process' response
				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
			}
			break;
		}

		case GET_USER_CREDENTIAL_SCREEN:
		{
			if(enrollInputs.source == SOURCE_SPECIAL_FUNC)
			{
				SetAudioVisual(USER_RESP_BUZ_AUDIO_IDLE, INTERNAL_PORT, NULL);
				TerminateEnrollmentActivity();
				AskForEnrollmentInput(0);
			}
			else
			{
				// Change source of enrollment to make sure it exits enrollment state
				enrollInputs.source = ENROLL_SOURCE_ACMS;
				// Exit enrollment mode
				ExitEnrollmentMode();
				// User response must be set idle to overcome incomplete process' response
				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
			}
			break;
		}

		case GET_SPL_FN_CARD_SCREEN:
		{
			if(enrollInputs.source == SOURCE_SPECIAL_FUNC)
			{
				SetAudioVisual(USER_RESP_BUZ_AUDIO_IDLE, INTERNAL_PORT, NULL);
				TerminateEnrollmentActivity();
				AskForEnrollmentInput(0);
			}
			else
			{
				// Change source of enrollment to make sure it exits enrollment state
				enrollInputs.source = ENROLL_SOURCE_ACMS;
				// Exit enrollment mode
				ExitEnrollmentMode();
				// User response must be set idle to overcome incomplete process' response
				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
			}
			break;
		}
	}
}

//*****************************************************************************
//	IsCustomCardWritePsbl
//	Param:
//		IN : cardType		- detected card type
//			 cardFieldsPtr	- pointer where writable fields are stored
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if any data to be written into smart card and if so it
//		returns YES else FALSE.
//
//*****************************************************************************
static BOOL IsCustomCardWritePsbl(CARD_TYPE_e cardType, CARD_WRITE_FIELDS_t * cardFieldsPtr)
{
	if(cardFieldsPtr -> addlSecCodeF == SET)
	{
		if(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_ADDL_SEC_CODE) == NO)
		{
			return NO;
		}
	}

	if(cardFieldsPtr -> facCodeF == SET)
	{
		if(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_FAC_CODE) == NO)
		{
			return NO;
		}
	}

	if(cardFieldsPtr -> userIdF == SET)
	{
		if(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_USERID) == NO)
		{
			return NO;
		}
	}

	if(cardFieldsPtr -> noOfTemplates == 1)
	{
		if((IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_FP1) == NO) &&
				(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_FP2) == NO))
		{
			return FAIL;
		}
	}

	if(cardFieldsPtr -> noOfTemplates == 2)
	{
		if((IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_FP1) == NO) ||
				(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_FP2) == NO))
		{
			return FAIL;
		}
	}

	if(cardFieldsPtr -> userNameF == SET)
	{
		if(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_USER_NAME) == NO)
		{
			return FAIL;
		}
	}

	if(cardFieldsPtr -> designationF == SET)
	{
		if(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_DESIG) == NO)
		{
			return FAIL;
		}
	}

	if(cardFieldsPtr -> branchF == SET)
	{
		if(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_BRANCH) == NO)
		{
			return NO;
		}
	}

	if(cardFieldsPtr -> departmentF == SET)
	{
		if(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_DEPT) == NO)
		{
			return NO;
		}
	}

	if(cardFieldsPtr -> bloodGrpF == SET)
	{
		if(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_BL_GRP) == NO)
		{
			return NO;
		}
	}

	if(cardFieldsPtr -> emrgncyContactF == SET)
	{
		if(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_EMRGNCY_CONATCT) == NO)
		{
			return NO;
		}
	}

	if(cardFieldsPtr -> medicalHistoryF == SET)
	{
		if(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_MED_HISTORY) == NO)
		{
			return NO;
		}
	}

	//If cardId is Configured and No card is assigned to user deny
	if(IsGivenFieldForSmartCardConfigured(cardType, CARD_FIELD_CARD_ID) == YES)
	{
		if((GetUserCardId(enrollActivity.userIdx, 0) == 0) &&
				(GetUserCardId(enrollActivity.userIdx, 1) == 0))
		{
			return NO;
		}
	}

	return YES;
}

/*ONLY FOR TEST APPLICATION*/
void setEnrollActivityPera(void)
{
	enrollActivity.userIdx = 0;
}
void setEnrollInputPera(ENROLL_CATEGORY_e enrlCategory)
{
	enrollInputs.category=enrlCategory;
}

//*****************************************************************************
//	SendMsgWriteSpclFunCardId
//	Param:
//		IN : cardType		- detected card type
//			 cardFieldsPtr	- pointer where writable fields are stored
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if any data to be written into smart card and if so it
//		returns YES else FALSE.
//
//*****************************************************************************
static BOOL SendMsgWriteSpclFunCardId(UINT8 splFnIdx)
{
	WRITE_CARD_ID_CMD_DATA_t cmdData;
	UINT8 idx;

	cmdData.readerId = enrollActivity.readerId;
	cmdData.cardType = enrollActivity.cardType;
	cmdData.cardIdF = TRUE;
	cmdData.cardId = 0;

	if(SpecialFunctions[splFnIdx].cardId[enrollActivity.cardCnt] != 0)
	{
		cmdData.cardId = SpecialFunctions[splFnIdx].cardId[enrollActivity.cardCnt];
	}
	else
	{
		for(idx = 0; idx < MAX_SP_FUNC_CARD; idx++)
		{
			if(SpecialFunctions[splFnIdx].cardId[idx] != 0)
			{
				cmdData.cardId = SpecialFunctions[splFnIdx].cardId[idx];
				break;
			}
		}
	}

	ProcessReaderCommand(MSG_WRITE_CARD_ID_CMD, (UINT8PTR)&cmdData, sizeof(WRITE_CARD_ID_CMD_DATA_t));
	return SUCCESS;
}
//*****************************************************************************
//	GetUserDetailFromId
//	Param:
//		IN : cardType		- detected card type
//			 cardFieldsPtr	- pointer where writable fields are stored
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if any data to be written into smart card and if so it
//		returns YES else FALSE.
//
//*****************************************************************************
BOOL GetUserDetailFromId(CHARPTR alphaNumId)
{
	CHAR cmdBuf[2048];
	INT32 resp;
	CHAR encodedAlphanumId[ALPHANUM_ID_SIZE*3], encDirName[MAX_HTTP_SERVER_DIRECTORY_LEN*3];
	CHARPTR host = NULL;

	URLEncodeAcms(alphaNumId,encodedAlphanumId);
	URLEncodeAcms(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerDirectory,encDirName);
	
	if(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName[0] != 0)
	{
		host = SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName;
	}
	else
	{
		host = SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress;
	}

	if( SysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF == ACTIVE )
	{
		snprintf(cmdBuf, 2048, CURL_CMD_HEADER ENCRPTD_SERVER_API_IS_USER_EXIST ,
									InterfaceStr[SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface], CURL_SERVER_API_TIMEOUT,
									SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword,
									SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort,
									encDirName,encodedAlphanumId,host,SERVER_API_RESP_FILE_PATH);
	}
	else
	{
		snprintf(cmdBuf, 2048, CURL_CMD_HEADER SERVER_API_IS_USER_EXIST ,
									InterfaceStr[SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface], CURL_SERVER_API_TIMEOUT,
									SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword,
									SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort,
									encDirName,encodedAlphanumId,host,SERVER_API_RESP_FILE_PATH);
	}

	DPRINT(DC_LOG,"User Detail with Encrytion mode [%d]", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF);
	resp = system(cmdBuf);
	resp = resp>>8;
	if(resp == 0)
	{
		if( ParseResponseFromXml(SERVER_API_RESP_FILE_PATH) == SUCCESS )
		{
			return SUCCESS;
		}
		else
		{
			if ( ParseResponseFromText() == USER_RECORD_NOT_FOUND)
			{
				return SUCCESS;
			}
			else
			{
				TerminateEnrollmentActivity();
				SetAudioVisual(USER_RESP_ADD_USER_FAIL, INTERNAL_PORT, NULL);
				sleep(3);
				SwitchDcToIdleState();
				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
				return FAIL;
			}
		}
	}
	else
	{
		EPRINT(DC_LOG, "error in Query of SERVER_API_IS_USER_EXIST. error code %u.", resp);
		TerminateEnrollmentActivity();
		SetAudioVisual(USER_RESP_NETWORK_ERROR, INTERNAL_PORT, NULL);
		sleep(3);
		SwitchDcToIdleState();
		SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
	}

	return FAIL;
}
//*****************************************************************************
//	CheckUserExistOnServerThreadFn
//	Param:
//		IN : cardType		- detected card type
//			 cardFieldsPtr	- pointer where writable fields are stored
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if any data to be written into smart card and if so it
//		returns YES else FALSE.
//
//*****************************************************************************
static VOIDPTR CheckUserExistOnServerThreadFn(UNUSED_PARA VOIDPTR dataPtr)
{
	CHAR cmdBuf[2048];
	INT32 resp;
	CHAR encodedAlphanumId[ALPHANUM_ID_SIZE*3], encDirName[MAX_HTTP_SERVER_DIRECTORY_LEN*3];
	CHARPTR host = NULL;

	URLEncodeAcms(EnrollmentTempInputs.alphaNumId,encodedAlphanumId);
	URLEncodeAcms(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerDirectory,encDirName);

	if(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName[0] != 0)
	{
		host = SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName;
	}
	else
	{
		host = SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress;
	}

	if( SysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF == ACTIVE )
	{
		snprintf(cmdBuf, 2048, CURL_CMD_HEADER ENCRPTD_SERVER_API_IS_USER_EXIST ,
									InterfaceStr[SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface], CURL_SERVER_API_TIMEOUT,
									SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword,
									SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort,
									encDirName,encodedAlphanumId,host,SERVER_API_RESP_FILE_PATH);
	}
	else
	{
		snprintf(cmdBuf, 2048, CURL_CMD_HEADER SERVER_API_IS_USER_EXIST ,
									InterfaceStr[SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface], CURL_SERVER_API_TIMEOUT,
									SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword,
									SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort,
									encDirName,encodedAlphanumId,host,SERVER_API_RESP_FILE_PATH);
	}

	DPRINT(DC_LOG,"User Detail with Encrytion mode [%d]", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF);
	resp = system(cmdBuf);
	resp = resp>>8;
	if(resp == 0)
	{
		if( ParseResponseFromXml(SERVER_API_RESP_FILE_PATH) == SUCCESS )
		{
			SetAudioVisual(USER_RESP_DO_YOU_ASSIGN_USER_ON_DEVICE, INTERNAL_PORT, NULL);
		}
		else
		{
			if ( ParseResponseFromText() == USER_RECORD_NOT_FOUND)
			{
				SetAudioVisual(USER_RESP_DO_YOU_ADD_ASSIGN_USER_ON_DEVICE, INTERNAL_PORT, NULL);
			}
			else
			{
				TerminateEnrollmentActivity();
				SetAudioVisual(USER_RESP_ADD_USER_FAIL, INTERNAL_PORT, NULL);
				sleep(3);
				SwitchDcToIdleState();
				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
			}
		}
	}
	else
	{
		EPRINT(DC_LOG, "error in Query of SERVER_API_IS_USER_EXIST. error code %u.", resp);
		TerminateEnrollmentActivity();
		SetAudioVisual(USER_RESP_NETWORK_ERROR, INTERNAL_PORT, NULL);
		sleep(3);
		SwitchDcToIdleState();
		SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
	}
	pthread_exit(NULL);
}
//*****************************************************************************
//	 CheckUserExistOnServerThreadFn
//	Param:
//		IN : cardType		- detected card type
//			 cardFieldsPtr	- pointer where writable fields are stored
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if any data to be written into smart card and if so it
//		returns YES else FALSE.
//
//*****************************************************************************
static VOIDPTR AssignUserOnDeviceThreadFn(UNUSED_PARA VOIDPTR dataPtr)
{
	CHAR 	cmdBuf[2048];
	INT32 	resp;
	CHAR 	encodedAlphanumId[ALPHANUM_ID_SIZE*3], encDirName[MAX_HTTP_SERVER_DIRECTORY_LEN*3];
	UINT16 	userIdx;
	UINT32	userConfigSyncTimerTick, userFpSyncTimerTick;
	UINT32	userConfigSyncWaitTime,  userFpSyncWaitTime;
	UINT8   processingTimeoutValue = USER_CONFIG_SYNC_WAIT_TIME+5;
	CHARPTR host = NULL;

	URLEncodeAcms(EnrollmentTempInputs.alphaNumId,encodedAlphanumId);
	URLEncodeAcms(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerDirectory,encDirName);

	if(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName[0] != 0)
	{
		host = SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName;
	}
	else
	{
		host = SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress;
	}

	if( SysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF == ACTIVE )
	{
		snprintf(cmdBuf, 2048, CURL_CMD_HEADER ENCRPTD_SERVER_API_ASSIGN_USER ,
									InterfaceStr[SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface], CURL_SERVER_API_TIMEOUT,
									SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword,
									SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort,
									encDirName,SysBasicPara.devId, encodedAlphanumId,host, SERVER_API_RESP_FILE_PATH);
	}
	else
	{
		snprintf(cmdBuf, 2048, CURL_CMD_HEADER SERVER_API_ASSIGN_USER ,
									InterfaceStr[SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface], CURL_SERVER_API_TIMEOUT,
									SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword,
									SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort,
									encDirName,SysBasicPara.devId, encodedAlphanumId,host,SERVER_API_RESP_FILE_PATH);
	}

	DPRINT(DC_LOG,"Assign User with Encrytion mode [%d]", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF);
	resp = system(cmdBuf);
	resp = resp>>8;
	FpDeletedFromModuleF = FALSE;

	if(resp == 0)
	{
		if ( ParseResponseFromText() == SERVER_API_SUCCESS )
		{
			DPRINT(DC_LOG,"User-Id [%s]:[%s] Is Assigned on Device.",EnrollmentTempInputs.alphaNumId, EnrollmentTempInputs.name);

			if(enrollUserfpIdx > SysAdvPara.maxFingerPerUser)
			{
				enrollUserfpIdx = SysAdvPara.maxFingerPerUser;
			}

			userConfigSyncWaitTime 		= CONVERT_SEC_TO_MSEC(USER_CONFIG_SYNC_WAIT_TIME);
			userFpSyncWaitTime  		= CONVERT_SEC_TO_MSEC(USER_CONFIG_SYNC_WAIT_TIME);
			userConfigSyncTimerTick 	= GetSysTick();

			//Delete All Enrollment Timer
			TerminateEnrollmentActivity();
			SetAudioVisual(USER_RESP_WAIT_FOR_SIGNAL, INTERNAL_PORT, &processingTimeoutValue);

			//Wait for User Config Sync
			while(1)
			{
				sleep(1);
				if(IsUserValid(enrollUserReferenceId, &userIdx) == YES)
				{
					DPRINT(SYS_LOG,"User-Id [%s]:[%s] Config Sync done. Elapsed time[%u ms] ",EnrollmentTempInputs.alphaNumId, EnrollmentTempInputs.name, ElapsedTick(userConfigSyncTimerTick));
					break;
				}
				else
				{
					if(ElapsedTick(userConfigSyncTimerTick) >= userConfigSyncWaitTime)
					{
						TerminateEnrollmentActivity();
						SetAudioVisual(USER_RESP_ADD_USER_WAIT_TIME_OUT, INTERNAL_PORT, NULL);
						sleep(3);
						SwitchDcToIdleState();
						SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
						EPRINT(DC_LOG,"User-Id [%s]:[%s] Config Sync Wait Time Out.",EnrollmentTempInputs.alphaNumId, EnrollmentTempInputs.name);
						goto timeOutThread;
					}
				}
			}

			// Wait for Delete Fp Command Responce
			userFpSyncTimerTick = GetSysTick();
			SetAudioVisual(USER_RESP_WAIT_FOR_SIGNAL, INTERNAL_PORT, &processingTimeoutValue);
			while(1)
			{
				sleep(1);
				if ( FpDeletedFromModuleF == TRUE )
				{
					DPRINT(DC_LOG,"User-Id [%s]:[%s],UserIdx [%d], Delete Fp Command Done Elapsed time[%u ms] ",EnrollmentTempInputs.alphaNumId, EnrollmentTempInputs.name,userIdx, ElapsedTick(userFpSyncTimerTick));
					break;
				}
				else
				{
					if(ElapsedTick(userFpSyncTimerTick) >= userFpSyncWaitTime)
					{
						TerminateEnrollmentActivity();
						SetAudioVisual(USER_RESP_ADD_USER_WAIT_TIME_OUT, INTERNAL_PORT, NULL);
						sleep(3);
						SwitchDcToIdleState();
						SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
						EPRINT(DC_LOG,"User-Id [%s]:[%s] Finger Template Delete Command Wait Time Out time[%u ms]",EnrollmentTempInputs.alphaNumId, EnrollmentTempInputs.name, ElapsedTick(userFpSyncTimerTick));
						goto timeOutThread;
					}
				}
			}

			// Wait for Fp Templeate
			userFpSyncTimerTick = GetSysTick();
			UINT8 syncFpCnt = 0;
			UINT8 currentFp = 0;
			SetAudioVisual(USER_RESP_WAIT_FOR_SIGNAL, INTERNAL_PORT, &processingTimeoutValue);
			while(1)
			{
				sleep(1);
				currentFp = GetCurrentUserFp(userIdx);
				if( enrollUserfpIdx == currentFp)
				{
					DPRINT(SYS_LOG,"User-Id [%s]:[%s] Finger Template [%d] Sync Done Elapsed time[%u ms] ",
							EnrollmentTempInputs.alphaNumId, EnrollmentTempInputs.name,
								enrollUserfpIdx, ElapsedTick(userFpSyncTimerTick));

					SetAudioVisual(USER_RESP_BUZ_AUDIO_IDLE, INTERNAL_PORT, NULL);
					AskForEnrollmentInput(0);
					goto timeOutThread;
				}
				else
				{
					if(currentFp > syncFpCnt)
					{
						syncFpCnt = currentFp;
						SetAudioVisual(USER_RESP_WAIT_FOR_SIGNAL, INTERNAL_PORT, &processingTimeoutValue);
						DPRINT(DC_LOG,"User-Id [%s]:[%s],UserIdx [%d], Sync Finger Template [%d], Elapsed time[%u ms] ",
										EnrollmentTempInputs.alphaNumId, EnrollmentTempInputs.name, userIdx,
											currentFp, ElapsedTick(userFpSyncTimerTick));

						//Reload Timer
						userFpSyncTimerTick = GetSysTick();
					}

					if((ElapsedTick(userFpSyncTimerTick) >= userFpSyncWaitTime) || (currentFp == SysAdvPara.maxFingerPerUser))
					{
						TerminateEnrollmentActivity();
						SetAudioVisual(USER_RESP_ADD_USER_WAIT_TIME_OUT, INTERNAL_PORT, NULL);
						sleep(3);
						SwitchDcToIdleState();
						SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
						EPRINT(DC_LOG,"User-Id [%s]:[%s] Finger Template Sync Wait Time Out.Expected Fp [%d] ,Sync FP [%d]"
									,EnrollmentTempInputs.alphaNumId, EnrollmentTempInputs.name, enrollUserfpIdx , currentFp);
						goto timeOutThread;
					}
				}
			}
		}
		else
		{
			TerminateEnrollmentActivity();
			SetAudioVisual(USER_RESP_ADD_USER_FAIL, INTERNAL_PORT, NULL);
			sleep(3);
			SwitchDcToIdleState();
			SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
			goto timeOutThread;
		}
	}
	else
	{
		EPRINT(DC_LOG, "error in Query of SERVER_API_ASSIGN_USER. error code %u.", resp);
		TerminateEnrollmentActivity();
		SetAudioVisual(USER_RESP_NETWORK_ERROR, INTERNAL_PORT, NULL);
		sleep(3);
		SwitchDcToIdleState();
		SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
		goto timeOutThread;
	}

timeOutThread:
	FpDeletedFromModuleF = FALSE;
	pthread_exit(NULL);
}
//*****************************************************************************
//	AddUserOnServerThreadFn
//	Param:
//		IN : cardType		- detected card type
//			 cardFieldsPtr	- pointer where writable fields are stored
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if any data to be written into smart card and if so it
//		returns YES else FALSE.
//
//*****************************************************************************
static VOIDPTR AddUserOnServerThreadFn(UNUSED_PARA VOIDPTR dataPtr)
{
	CHAR cmdBuf[2048];
	INT32 resp;
	CHAR encodedAlphanumId[ALPHANUM_ID_SIZE*3], encDirName[MAX_HTTP_SERVER_DIRECTORY_LEN*3];;
	CHAR encodedUsrName[NAME_SIZE_UTF8*3];
	CHARPTR host = NULL;

	URLEncodeAcms(EnrollmentTempInputs.alphaNumId,encodedAlphanumId);
	URLEncodeAcms(EnrollmentTempInputs.name,encodedUsrName);
	URLEncodeAcms(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerDirectory,encDirName);

	if(SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName[0] != 0)
	{
		host = SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName;
	}
	else
	{
		host = SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress;
	}

	if( SysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF == ACTIVE )
	{
		snprintf(cmdBuf, 2048, CURL_CMD_HEADER ENCRPTD_SERVER_API_ADD_USER,
						InterfaceStr[SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface], CURL_SERVER_API_TIMEOUT,
						SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword,
						SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress,SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort,
						encDirName,encodedAlphanumId, encodedUsrName,host,SERVER_API_RESP_FILE_PATH);
	}
	else
	{
		snprintf(cmdBuf, 2048, CURL_CMD_HEADER SERVER_API_ADD_USER,
						InterfaceStr[SysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface], CURL_SERVER_API_TIMEOUT,
						SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId, SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword,
						SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress,SysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort,encDirName,
						encodedAlphanumId,encodedUsrName,host,SERVER_API_RESP_FILE_PATH);
	}

	DPRINT(DC_LOG,"Add User On Server with Encrytion mode [%d]", SysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF);
	resp = system(cmdBuf);
	resp = resp>>8;
	if(resp == 0)
	{
		if ( ParseResponseFromText() == SERVER_API_SUCCESS )
		{
			DPRINT(DC_LOG,"User-Id [%s]:[%s] Is Add on Server.",EnrollmentTempInputs.alphaNumId, EnrollmentTempInputs.name);
			if( GetUserDetailFromId( EnrollmentTempInputs.alphaNumId ) == SUCCESS )
			{
				UINT8 processingTimeoutValue = CURL_SERVER_API_TIMEOUT+3;
				SetAudioVisual(USER_RESP_WAIT_FOR_SIGNAL, INTERNAL_PORT, &processingTimeoutValue);
				AssignUserOnDeviceThreadFn(NULL);
			}
			else
			{
				TerminateEnrollmentActivity();
				SetAudioVisual(USER_RESP_ADD_USER_FAIL, INTERNAL_PORT, NULL);
				sleep(3);
				SwitchDcToIdleState();
				SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
			}
		}
		else
		{
			TerminateEnrollmentActivity();
			SetAudioVisual(USER_RESP_ADD_USER_FAIL, INTERNAL_PORT, NULL);
			sleep(3);
			SwitchDcToIdleState();
			SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
		}
	}
	else
	{

		EPRINT(DC_LOG, "error in Query of SERVER_API_ADD_USER. error code %u.", resp);
		TerminateEnrollmentActivity();
		SetAudioVisual(USER_RESP_NETWORK_ERROR, INTERNAL_PORT, NULL);
		sleep(3);
		SwitchDcToIdleState();
		SetAudioVisual(USER_RESP_IDLE, INTERNAL_PORT, NULL);
	}
	pthread_exit(NULL);
}
//*****************************************************************************
//	CheckIsRespInXML()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         NONE
//	Description:
//		This function reads response and parses for tag and return val
//*****************************************************************************
BOOL CheckIsRespInXML()
{
	FILE *  	fp = NULL;
	CHAR		tempBuf[256];

	fp = fopen(SERVER_API_RESP_FILE_PATH, "r");
	if(fp == NULL)			//File is not created
	{
		EPRINT(DC_LOG,"Cannot Open file[%s] ",SERVER_API_RESP_FILE_PATH);
		return FAIL;
	}

	// Write initial values of count and rollover count.
	if(fgets(tempBuf, sizeof(tempBuf), fp) == NULL)
	{
		EPRINT(DC_LOG,"Cannot Read file[%s] ",SERVER_API_RESP_FILE_PATH);
		fclose(fp);
		return FAIL;
	}

	fclose(fp);

	if(tempBuf[0] == '<')
	{
		return YES;
	}

	return NO;
}
//*****************************************************************************
//	ParseResponseFromXml()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         NONE
//	Description:
//		This function reads response and parses for tag and return val
//*****************************************************************************
SERVER_API_RESP_e ParseResponseFromText()
{
	CHAR		tempBuf[256];
	FILE *  	fp = NULL;

	fp = fopen(SERVER_API_RESP_FILE_PATH, "r");
	if(fp == NULL)			//File is not created
	{
		EPRINT(DC_LOG,"Cannot Open file[%s] ",SERVER_API_RESP_FILE_PATH);
		return FAIL_RESPONSE_FROM_SERVER;
	}

	// Write initial values of count and rollover count.
	if(fgets(tempBuf, sizeof(tempBuf), fp) == NULL)
	{
		EPRINT(DC_LOG,"Cannot Read file[%s] ",SERVER_API_RESP_FILE_PATH);
		fclose(fp);
		return FAIL_RESPONSE_FROM_SERVER;
	}
	fclose(fp);

	if(strncmp(tempBuf, "success", 6) == 0)
	{
		if(strstr(tempBuf, "success: 0070200001 : saved successfully") != NULL)
		{
			DPRINT(DC_LOG,"Success Add Response Is [%s]",tempBuf);
			return SERVER_API_SUCCESS;
		}
		else if(strstr(tempBuf, "success: 0080600002 : successful") != NULL)
		{
			DPRINT(DC_LOG,"Success Assign Response Is [%s]",tempBuf);
			return SERVER_API_SUCCESS;
		}
	}
	else if(strncmp(tempBuf, "failed", 6) == 0)
	{
		if(strstr(tempBuf, "failed: 0070102003 : Insufficient User rights or Inactive User(s) or ID(s) does not exist.") != NULL)
		{
			DPRINT(DC_LOG,"Fail Response Is [%s]",tempBuf);
			return USER_RECORD_NOT_FOUND;
		}

		if(strstr(tempBuf, "failed: 0070201003 : Incomplete Command : module=") != NULL)
		{
			SetAudioVisual(USER_RESP_ADD_USER_FAIL, INTERNAL_PORT, NULL);
			EPRINT(DC_LOG, "Fail Response Is [%s]",tempBuf);
			return FAIL_RESPONSE_FROM_SERVER;
		}
	}

	EPRINT(DC_LOG,"Fail Response Is [%s]",tempBuf);
	return FAIL_RESPONSE_FROM_SERVER;
}

//*****************************************************************************
//	ParseResponseFromXml()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         NONE
//	Description:
//		This function reads response and parses for tag and return val
//*****************************************************************************
UINT8 ParseResponseFromXml(CHARPTR filename)
{
	xmlDocPtr 	doc;
    xmlNodePtr 	cur, sub, subSub;

    xmlInitParser();
    doc = xmlParseFile(filename);

    if(doc == NULL )
    {
        EPRINT(DC_LOG,"Document parsing failed.");
    }

    cur = xmlDocGetRootElement(doc); //Gets the root element of the XML Doc

    if(cur == NULL)
    {
        xmlFreeDoc(doc);
    	xmlCleanupParser();
        EPRINT(DC_LOG,"Document is Empty!!!");
		return FAIL;
    }

	while(cur != NULL)
	{
		if(xmlStrcasecmp(cur->name, (const xmlChar *)"DocumentElement") == 0)
		{
			sub = cur->xmlChildrenNode;
			while(sub != NULL)
			{
				if(xmlStrcasecmp(sub->name, (const xmlChar *)"user") == 0)
				{
					subSub = sub->xmlChildrenNode;
					while(subSub != NULL)
					{
						if(xmlStrcasecmp(subSub->name, (const xmlChar *)"short-name") == 0)
						{
							if( (subSub->xmlChildrenNode != NULL) && (subSub->xmlChildrenNode->content != NULL) )
							{
								snprintf(EnrollmentTempInputs.name, NAME_SIZE_UTF8, "%s", subSub->xmlChildrenNode->content);
							}
						}

						if(xmlStrcasecmp(subSub->name, (const xmlChar *)"reference-code") == 0)
						{
							if( (subSub->xmlChildrenNode != NULL) && (subSub->xmlChildrenNode->content != NULL) )
							{
								enrollUserReferenceId = (UINT32)atoi((char *)(subSub->xmlChildrenNode->content));
							}
						}

						if(SysBasicPara.fprTemplateFormat == ISO_19794_FORMAT)
						{
							if(xmlStrcasecmp(subSub->name, (const xmlChar *)"suprema-iso-count") == 0)
							{
								if( (subSub->xmlChildrenNode != NULL) && (subSub->xmlChildrenNode->content != NULL) )
								{
									enrollUserfpIdx = (UINT8)atoi((char *)(subSub->xmlChildrenNode->content));
								}
							}
						}
						else
						{
							if(xmlStrcasecmp(subSub->name, (const xmlChar *)"suprema-proprietary-count") == 0)
							{
								if( (subSub->xmlChildrenNode != NULL) && (subSub->xmlChildrenNode->content != NULL) )
								{
									enrollUserfpIdx = (UINT8)atoi((char *)(subSub->xmlChildrenNode->content));
								}
							}
						}
						subSub = subSub->next;
					}
				}
				sub = sub->next;
			}
			DPRINT(DC_LOG, "Name:[%s], RefId=[%d], Enrolled Finger=[%d] Finger-Formate=[%d]", EnrollmentTempInputs.name, enrollUserReferenceId, enrollUserfpIdx,SysBasicPara.fprTemplateFormat);
		}
		else
		{
	        xmlFreeDoc(doc);
	    	xmlCleanupParser();
	        EPRINT(DC_LOG, "DocumentElement not found, not a valid response.");
			return FAIL;
		}
		cur = cur->next;
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	return SUCCESS;
}
//*****************************************************************************
//	IsMobileEnrollmentStarted()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         NONE
//	Description:
//		This function checks if mobile enrollment is started for ACS.
//*****************************************************************************
BOOL IsMobileEnrollmentStarted()
{
	if((DcInfo.curState == DC_ENROLLMENT_STATE) &&( enrollActivity.state == ENROLL_USER_MOBILE_STATE))
	{
		return TRUE;
	}

	return FALSE;

}
//*****************************************************************************
//	StartEnrollAbrtReq()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//	         NONE
//	Description:
//
//*****************************************************************************

static void StartEnrollAbrtReq(void)
{
    //DPRINT(SYS_LOG, "In Start Abourt Request");
    FACE_ENROLL_REQ_t tObj;
    tObj.requestType = USR_ENR_ABORT_ENROLL_PROCESS;
    tObj.numberOfCaptureFace = enrollInputs.faceFrameCnt;
    tObj.numberOfEnrollFace = enrollInputs.faceCnt;
    tObj.UserId = enrollInputs.userId;
    ProcessEnrFaceRequest(&tObj);

}

//*****************************************************************************
//	GetAvailDuressUserFp
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
UINT8 GetAvailDuressUserFp(UINT32 userID)
{
            UINT8 fpCnt = 0,Idx;
             USER_CONFIG_t tUserConfig;
            
            GetUserConfig(userID, &tUserConfig);
            
            for(Idx = 0;Idx < MAX_DURESS_DUAL_FP_TEMPLATE;Idx++)
            {
	if(tUserConfig.DuressFp[Idx] == DURESS_FP_IDX_NONE)
	    fpCnt++;
            }
            if((SysAdvPara.templatesPerFinger == ONE_TEMPLATE) && fpCnt == MAX_USER_DURESS_FP)
	    fpCnt = 0;
            else
                   fpCnt = fpCnt/MAX_USER_DURESS_FP;
            return fpCnt;
}
//*****************************************************************************
//	isCmdEnrollDuressThenStoreFpIdx
//	Param:
//		IN : 	user Index
//			 FP Index
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if Duress Detection Flag is enable 
//		Then update the Duress FP Index
//
//*****************************************************************************
BOOL isCmdEnrollDuressThenStoreFpIdx(UINT32 UserId,UINT8 FpIdx)
{
            UINT8 status = FAIL, Idx,dualTempIdx;
            USER_CONFIG_t tUserConfig;
            
            GetUserConfig(UserId, &tUserConfig);
            
            if(enrollInputs.enrollmentType != ENROLL_DURESS_FP)
            {
	DPRINT(DC_LOG,"EnrollmentType[%d]",enrollInputs.enrollmentType);
	status =  SUCCESS;
            }
            else if(enrollInputs.enrollmentType == ENROLL_DURESS_FP && (0 != GetAvailDuressUserFp(UserId)))
            {
              
	for(Idx = 0; Idx < MAX_DURESS_DUAL_FP_TEMPLATE; Idx++)
	{
	      if(tUserConfig.DuressFp[Idx] == DURESS_FP_IDX_NONE)
	      {
	            if(SysAdvPara.templatesPerFinger == TWO_TEMPLATES)
	            {
		dualTempIdx = (FpIdx * MAX_USER_DURESS_FP);
		tUserConfig.DuressFp[Idx] = dualTempIdx;
		tUserConfig.DuressFp[Idx+1] = dualTempIdx+1;
	            }
	            else
	            {
		tUserConfig.DuressFp[Idx] = FpIdx;
	            }
	            SetUserConfig(UserId, tUserConfig);
	            WriteSingleUserConfig(UserId);
	            status =  SUCCESS;
	            break;
	      }
	}
            }
            else
            {
	EPRINT(DC_LOG,"Maximum Duress Fingure Enrolled already");
	status =  FAIL;
            }
        
        return status;
}


//*****************************************************************************
//	isCmdEnrollDuress
//	Param:
//		IN : 	user Index
//			 FP Index
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if Duress Detection Flag is enable 
//		Then update the Duress FP Index
//
//*****************************************************************************
BOOL isCmdEnrollDuress(void)
{
   BOOL status = NO;
   
   if(enrollInputs.enrollmentType == ENROLL_DURESS_FP)
      status =YES;
   
   return status;
}

//*****************************************************************************
//	FpIndexForWriteSmartCard
//	Param:
//		IN : 	user Index
//			 FP Index
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if index is enrolled as duress or not 
//		
//
//*****************************************************************************
void FpIndexForWriteSmartCard(UINT32 userIdx,UINT8 *FpIdx1,UINT8 *FpIdx2)
{
	UINT8 Idx, DuressIdx,dualTempIdx;
	USER_CONFIG_t			tempUserConfig;
	
	if(GetDoorApplType() == DIRECT_DOOR_APPL)
	{
	    if(userIdx > MAX_USERS)
	    {
		    *FpIdx1 = INVALID_FP_IDX;
		    *FpIdx2 = INVALID_FP_IDX;
		    EPRINT(DC_LOG, "userIdx [%u] is larger than MAX_USERS so FpIdx1[INVALID_FP_IDX] FpIdx2[INVALID_FP_IDX]", userIdx);
		    return;
	    }
	}
	
	GetUserConfig(userIdx, &tempUserConfig);
	
	if(SysAdvPara.templatesPerFinger == ONE_TEMPLATE)
	{
	        *FpIdx1 = INVALID_FP_IDX;
	        *FpIdx2 = INVALID_FP_IDX;
	        for(Idx = 0;Idx < MAX_USER_FP;Idx++)
	        {
	            for(DuressIdx = 0;DuressIdx < MAX_DURESS_DUAL_FP_TEMPLATE;DuressIdx++)
	            {
		    if(tempUserConfig.DuressFp[DuressIdx] == Idx)
		    {
		        break;
		    }
	            }
	            
	            if(DuressIdx == MAX_DURESS_DUAL_FP_TEMPLATE)
	            {
		    if((*FpIdx1) == INVALID_FP_IDX)
		    {
		        (*FpIdx1) = Idx;
		    }
		    else if((*FpIdx2) == INVALID_FP_IDX)
		    {
		        (*FpIdx2) = Idx;
		    }
	            }
	            if(((*FpIdx1) != INVALID_FP_IDX) && ((*FpIdx2) != INVALID_FP_IDX))
	            {
		break;
	            }
	        }
	}
	else
	{
	        *FpIdx1 = INVALID_FP_IDX;
	        *FpIdx2 = INVALID_FP_IDX;
	        
	        for(Idx = 0;Idx < MAX_USER_FP;Idx++)
	        {
	            dualTempIdx = (Idx * MAX_USER_DURESS_FP);
	            for(DuressIdx = 0;DuressIdx < MAX_DURESS_DUAL_FP_TEMPLATE;DuressIdx++)
	            {
		    if((tempUserConfig.DuressFp[DuressIdx] == dualTempIdx) && (tempUserConfig.DuressFp[DuressIdx+1] == dualTempIdx+1))
		    {
		        break;
		    }
	            }
	            
	            if(DuressIdx == MAX_DURESS_DUAL_FP_TEMPLATE)
	            {
		    if((*FpIdx1) == INVALID_FP_IDX)
		    {
		        (*FpIdx1) = Idx;
		    }
		    else if((*FpIdx2) == INVALID_FP_IDX)
		    {
		        (*FpIdx2) = Idx;
		    }
	            }
	            if(((*FpIdx1) != INVALID_FP_IDX) && ((*FpIdx2) != INVALID_FP_IDX))
	            {
		break;
	            }
	        }
	}
}

//*****************************************************************************
//	DeleteUserDuressFp
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
void DeleteUserDuressFp(UINT32 userID)
{	
	USER_CONFIG_t tUserConfig;
            
	GetUserConfig(userID, &tUserConfig);
	memcpy((VOIDPTR)tUserConfig.DuressFp,(VOIDPTR)DfltUserConfig.DuressFp,sizeof(DfltUserConfig.DuressFp));
	SetUserConfig(userID, tUserConfig);
	WriteSingleUserConfig(userID);
	DPRINT(DC_LOG,"Delete Duress FP config");
}

//*****************************************************************************
//	isUserIsVisitor
//	Param:
//		IN : 	user Index
//		OUT: none
//	Returns:
//		YES/NO
//	Description:
//		It checks if User is Visitor or not 
//		
//
//*****************************************************************************
BOOL isUserIsVisitor(UINT32 userIdx)
{	
	USER_CONFIG_t tUserConfig;
	BOOL status = NO;
	
	GetUserConfig(userIdx, &tUserConfig);
	
	if(tUserConfig.visitorIdx != INVALID_VISITOR_IDX)
	{
	    status = YES;
	}
	return status;
}
