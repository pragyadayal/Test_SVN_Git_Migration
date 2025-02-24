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
//   Created By   : Yashavadan Dalsaniya
//   File         : MxSelfEnrollment.c
//   Description  : 
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
#include "MxLogs.h"
#include "MxDcTask.h"
#include "MxSysCommonDef.h"
#include "MxSelfEnrollment.h"
#include "MxConfigFileIo.h"
#include "MxUserConfig.h"

//******** Extern Variables **********
extern DOOR_ACTIVITY_INFO_t			DcInfo;				//Door Controller FSM info
extern SYS_ADVANCE_PARA_t			SysAdvPara;
extern const CHARPTR				EnrollmentStatusStr[MAX_ENROLLMENT_STATUS + 1];

//******** Defines and Data Types ****

//******** Function Prototypes *******
static void SelfEnrolInputWaitTimeout(UINT32 dummy);
static BOOL LoadSelfEnrollInputTimer(void);

//******** Global Variables **********

//******** Static Variables **********


//******** Function Definitions ******
//*****************************************************************************
//	DcWaitForSelfEnrlInputState()
//	Param:
//		IN: msgId 	- Message which needs to be processed
//			dataPtr - Pointer to data of the message
//			length 	- Length of the message
//		OUT: None
//	Returns:
//		NONE
//	Description:
//*****************************************************************************
void DcWaitForSelfEnrlInputState(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	// Based on message ID, process it
	switch(msgId)
	{
		case GUI_BUTTON_SELF_ENROLLMENT_START:
		{
			ENROLLMENT_INPUTS_t		enrollment;
			ENROLLMENT_STATUS_e		enrollmentStatus;

			enrollment.source = ENROLL_SOURCE_SELF_ENROLL;
			enrollment.category = ENROLL_CATEGORY_USER;
			enrollment.enrollmentType = ENROLL_BIOMETRIC_ONLY;
			strcpy(enrollment.alphaNumId, GetAlphaNumId(DcInfo.userIdx));
			enrollment.userId = GetUserId(DcInfo.userIdx);
			enrollment.fpCnt = SysAdvPara.fpCount;
			memset(&enrollment.cardWriteFields, CLEAR, sizeof(CARD_WRITE_FIELDS_t) );
			SwitchDcToIdleState();
			//start enrollment proccess
			enrollmentStatus = RequestEnrollment(&enrollment);
			DPRINT(DC_LOG,"Enrollment status [%s]", EnrollmentStatusStr[enrollmentStatus]);
			break;
		}

		case GUI_BUTTON_SELF_ENROLLMENT_CANCEL:
		{
			UINT8 tSelfEnrlSkippedCnt = 0;
			DeleteTimer(&DcInfo.selfEnrlInputTimerHandle);

			if((tSelfEnrlSkippedCnt = GetUserSelfEnrlSkippedCnt(DcInfo.userIdx)) < SysAdvPara.selfEnrlRetryCnt)
			{
				DcInfo.bypassBioCred = TRUE;
				SetUserSelfEnrlSkippedCnt(DcInfo.userIdx, tSelfEnrlSkippedCnt + 1);
				WriteSingleUserConfig(DcInfo.userIdx);
			}
			else if(IsFingerBypass(DcInfo.userIdx) != ENABLED)
			{
				DcInfo.authReason = AUTH_NO_ACCESS;
				DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				return;
			}

			// 	All other users have to show proper credential as per ZAM policy
			if(IsCredentialValid() == NO)
			{
				// set userIdx as ZAM user for updating display and log an event
				DcInfo.zamUserIdx = DcInfo.userIdx;
				DcInfo.authReason = AUTH_INVALID_INPUT;
				// Store current credential as invalid
				StoreRcvdCred(INVALID_CREDENTIAL, CARD_NUMBER_NOT_APPLICABLE);
				DenyUser(DcInfo.zamUserIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
				return;
			}

			if(CheckSmartRouteFeature() == PROCESS_DONE)
			{
				return;
			}

			// If more than one credentials are required to access this door,
			// we switch state where we wait for additional credential of the same user.
			if((GetCredentialCnt()) > DcInfo.credCnt)
			{
				// will check for palm verification, will ask for next credential
				CheckNextCredToProcess();
				// set userIdx as ZAM user so that we can verify him/her later
				DcInfo.zamUserIdx = DcInfo.userIdx;
				// Change DC FSM state to wait for additional credential state
				SwitchDcToZAMState();
				return;
			}

			if(CheckTwoPersonFeature() == PROCESS_DONE)
			{
				return;
			}

				// We have gone thru all the checks and finally we can let the user in.
				CheckFinalPolicyAndAllow();
			//switch to process user state.
			break;
		}

		default:
			// Events that are not handled in this state are passed to default message handler .
			DcDefaultMsgHandler(msgId, dataPtr, length);
			break;
	}
}

//*****************************************************************************
//	SwitchToSelfEnrlInputState()
//	Param:
//		IN: NONE
//		OUT: None
//	Returns:
//		NONE
//	Description:
//*****************************************************************************
void SwitchToSelfEnrlInputState(void)
{
	LoadSelfEnrollInputTimer();
	SetAudioVisual(USER_RESP_SELF_ENROLL_REQ_INPUT, INTERNAL_PORT, NULL);
	CleanUpTempUserFP();
	DcInfo.curState = DC_WAIT_FOR_SELF_ENROLL_REQ_STATE;
}

//*****************************************************************************
//	IsBioCredSelfEnrollReq()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	YES/NO
//	Description:
//				This function checks whether bio-matric credential needs to be
//		enrolled or not through self enrollment feature.
//*****************************************************************************
BOOL IsBioCredSelfEnrollReq(void)
{
	ACCESS_MODE_e		accessMode;

	if(DcInfo.respPort == INTERNAL_PORT)
	{
		accessMode = GetAccessModeFromRespPort(DcInfo.userType, DcInfo.respPort);
		switch(accessMode)
		{
			case FINGER_PRINT_ONLY:
			case FINGER_PRINT_PIN:  // Palm  and PIN are required
			case CARD_FINGER_PRINT:		// Card and Palm are required
			case FINGER_PRINT_THEN_CARD:
			case CARD_FINGER_PRINT_PIN:		// Card + Palm + PIN required
			case ACCESS_MODE_FACE_FINGER_PRINT:
			case ACCESS_MODE_CARD_THEN_BIOMETRIC:
			{
				if( (SysAdvPara.selfEnrlEnbl == ENABLED) && (IsUserSelfEnrlEnable(DcInfo.userIdx) == ENABLED) )
				{
					if(GetEnrolledUserFp(DcInfo.userIdx) == 0)
					{
						return YES;
					}
				}
				break;
			}

			default:
			{
				break;
			}
		}
	}
	return NO;
}

//*****************************************************************************
//	LoadSelfEnrollInputTimer()
//	Param:
//		IN : timerValue	- timer value in seconds
//		OUT: NONE
//	Returns:
//			 SUCCESS/FAIL
//	Description:
//*****************************************************************************
static BOOL LoadSelfEnrollInputTimer(void)
{
 	if(DcInfo.selfEnrlInputTimerHandle == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = (MENU_TIMER * 10);
		tInfo.funcPtr = &SelfEnrolInputWaitTimeout;
		tInfo.data = 0;

		return(StartTimer(tInfo, &(DcInfo.selfEnrlInputTimerHandle), __FUNCTION__));
	}
	else
	{
		return(ReloadTimer(DcInfo.selfEnrlInputTimerHandle, (MENU_TIMER * 10)));
	}
}

//*****************************************************************************
//	SelfEnrolInputWaitTimeout()
//	Param:
//		IN : NONE
//		OUT: NONE
//	Returns:
//			 NONE
//	Description:
//
//*****************************************************************************
static void SelfEnrolInputWaitTimeout(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&DcInfo.selfEnrlInputTimerHandle);
	DcInfo.authReason = AUTH_TIMEOUT;
	DenyUser(DcInfo.userIdx, DcInfo.readerMode, DcInfo.respPort, DC_STATE_SWITCH_TO_IDLE);
}
