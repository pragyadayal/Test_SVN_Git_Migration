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
//   File         : MxReaderTask.c
//   Description  : This file implements API layer of reader task to communicate
//					with diffrent reader drivers
//
/////////////////////////////////////////////////////////////////////////////

//******** Include files **********

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "MxTypedef.h"
#include "MxSysTimer.h"
#include "MxReaderTask.h"
#include "MxReaderUtils.h"
#include "MxReaderRxTx.h"
#include "MxMifareReader.h"
#include "MxSM130Task.h"
#include "MxSM125Task.h"
#include "MxPN532Task.h"
#include "MxCLR663Task.h"
#include "MxCerTask.h"
#include "MxSupremaReader.h"
#include "MxOEM75Task.h"
#include "MxSlaveMsg.h"
#include "MxDcTask.h"
#include "MxSysUtils.h"
#include "MxDelay.h"
#include "MxConfig.h"
#include "MxLogs.h"
#include "MxConfigFileIo.h"
#include "MxSysTest.h"
#include "MxUhfReader.h"
#include "MxHIDProx.h"
#include "MxMsgHandler.h"
#include "MxSE32X0Task.h"
#include "MxIclassReaderTask.h"
#include "MxReaderCommonUtils.h"
#include "MxCbBluetooth.h"
#include "MxDeviceSpecific.h"
#include "MxAtomReaderBluetooth.h"

//******** Extern Variables **********
extern SYS_BASIC_PARA_t 	 			SysBasicPara;
extern const CHARPTR 					ReaderStatusStr[MAX_READER_STATUS];
extern const CHARPTR 					CardTypeStr[];
extern READER_INIT_PROC_INFO_t			ReaderInitInfo;
extern READER_AUTO_DETECT_PROC_INFO_t	ReaderAutoDetectInfo;
extern CALIB_STATUS_e					SensorCalibStatus[];
extern SYSTEM_TEST_STATUS_e				ImageProgressF;
extern BOOL								ScanTempCmdToSend[MAX_READERS];
extern BIO_IDENTIFY_SERVER_INFO_t		BioIdentifyServer;
extern BOOL								FpDeletedFromModuleF;

//****** Defines and Data Types ******
#define SIZE_READER_Q 				20		// Depth of each reader queue
#define NUM_READER_S_BUFF			20		// Number of small buffers
#define NUM_READER_M_BUFF			10		// Number of medium buffer
#define NUM_READER_L_BUFF			2		// Number of Large buffer

// Conditions which requires MEDIUM buffers are as per follow
// 1. templates to be read from card or templates to be write into card
// 2. templates read from card to be passed to FP reader for verification
// 3. templates to be enrolled on finger print reader (on entry and exit FPR)
// 4. templates to be retrieved from finger print reader (from entry FPR only)

// Conditions which requires LARGE buffers are as per follow
// 1. templates to be read from card or templates to be write into card
// 2. templates read from card to be passed to FP reader for verification

// NOTES:
// 1. Conditions 3 may require 2 medium buffers, if it is to be enrolled on entry
// and exit FPR.
// 2. Condition 1 and 2 may occur simultenously.

#define READER_EV_BUFF_SIZE			25		// event buffer size

//******** Function Prototypes *******

static BOOL AllocateReaderQ(UINT8 readerId, UINT16 buffSize, READER_Q_INFO_t ** qPtr);
static void ReaderRespTimeout(UINT8 readerId);
static void FreeReaderQ(UINT8 readerId);
static READER_CMD_PROC_STATUS_e 	SendCmdToReader(UINT8 readerId, BOOL retryFlag);
static READER_RESP_PROC_STATUS_e 	GetRespFromReader(UINT8 readerId, READER_MSG_INFO_t * readerMsgInfoPtr);
static READER_EV_PROC_STATUS_e 		GetEventFromReader(UINT8 readerId, READER_MSG_INFO_t * readerMsgInfoPtr);
static READER_EV_PROC_STATUS_e 		GetEvFromWiegandReader(UINT8 readerId, READER_MSG_INFO_t * readerMsgInfoPtr);
static void ProcFailedResp(READER_ID_e readerId, SLAVE_MSG_ID_e cmdMsgId);
static void ReaderWaitCmdTimeout(UINT8 readerId);
static void InitReader(READER_ID_e readerId);
static void FreeAllReaderBuff(void);
static void AbortFingerScan(UINT8 readerId, VOIDPTR dataPtr, UINT16 length);
static void CompleteReaderBgInitProcess(UINT8 readerId);
static void ScanTempWaitTimeout(UINT8 readerId);
static void RdrTaskMsgProcDone(UINT8 readerId);

//******** Global Variables **********
READER_RESP_STATUS_e						ReaderRespFailReason[MAX_READERS];
READER_ACTIVITY_INFO_t						ReaderData[MAX_READERS];
READ_SEC_INFO_RESP_DATA_t 					SecInfoRespData[MAX_READERS];
READ_SMART_ROUTE_ACCESS_INFO_RESP_DATA_t	SmartRouteInfoData[MAX_READERS];
TEMP_RW_FIELD_FLAG_t						tempRwField[MAX_READERS];
BOOL										CalibrateFpSensorF;
READER_STATUS_e 							readerStatus[MAX_READERS];			// holds reader status (active/inactive/unknown)
READER_MEDIUM_BUFF_t						readerMBuff[NUM_READER_M_BUFF];		// reader buffer of medium size
FPR_FW_UPGRADE_STATUS_t						FprFwUpgrade;
BOOL										FprUninitReqdF[MAX_READERS];
READ_CARD_ID_RESP_DATA_t					ReadCardIdRespData[MAX_READERS];
BOOL 										gIsVariant_With_Internal_FP_Rdr = FALSE; //variant has Internal FP reader or not (default: FALSE)

//******** Static Variables **********

static 	READER_Q_INFO_t 		readerQ[MAX_READERS][SIZE_READER_Q];// reader queues
static 	READER_SMALL_BUFF_t 	readerSBuff[NUM_READER_S_BUFF];		// reader buffer of small size
static 	READER_LARGE_BUFF_t		readerLBuff[NUM_READER_L_BUFF];		// reader buffer of large size
static 	UINT8 					evDataBuffer[MAX_READERS][READER_EV_BUFF_SIZE];
																	// holds latest event from readers.
// NOTE: size of event bugffer should be such that it can accomodate card or finger print detect event messages
static 	UINT8					readerQInPtr[MAX_READERS];			// points to entry in queue where next command can be inserted.
static 	UINT8 					readerQOutPtr[MAX_READERS];			// points to entry in queue which next command needs to be processed.
static 	UINT8 					rdhState[MAX_READERS];				// holds current state of reader distribution handler
static	TIMER_HANDLE			readerRespTmrHandle[MAX_READERS];	// holds timer id to get response of sent command before its timeout
static	UINT8 					readerRetryCnt[MAX_READERS];		// holds retry count for command
static	TIMER_HANDLE			readerWaitCmdTmrHandle[MAX_READERS];// holds timer id for wait to send command again
static 	TIMER_HANDLE			scanTemplateWaitTmrHandle[MAX_READERS];	// holds timer id for wait to send scan temp cmd

//******** Function Definations ******
void SetRDHState(READER_ID_e iReaderId, RDH_STATES_e iState)
{
	if(iReaderId >= MAX_READERS || iState >= RDH_STATE_MAX)
	{
		DPRINT(READER_TASK_LOG, "Invalid input Reader = %d, State = %d", (int)iReaderId, (int)iState);
		return;
	}
	
	if (rdhState[iReaderId] != iState)
	{
		rdhState[iReaderId] = (UINT8)iState;
		DPRINT(READER_TASK_LOG, "RDH State[%d]=%d", (int)iReaderId, (int)iState);
	}
}

RDH_STATES_e GetRDHState(READER_ID_e iReaderId)
{
	if(iReaderId >= MAX_READERS)
	{
		return RDH_IDLE_STATE;
	}
	return ((RDH_STATES_e) rdhState[iReaderId]);
}


//*****************************************************************************
//	ProcessReaderCommand()
//	Param:
//		IN :	msgId	- message Id defined in system
//				dataPtr - points where command data is stored
//				length	- length of payload
//		OUT:	none
//	Returns:	SUCCESS	- if command successfully queued in reader Q.
//				FAIL	- if command couldn't be queued in reader Q.
//	Description:
//				This function will place command in reader Q to be processed
//				later.
//				It will determine which reader to queue for based on readerId
//				in data.
//				If queue and buffer both are available, it copies data into
//				buffer and update queue and buffer status as pending.
//				It will update queue status, message ID, payload pointer and
//				length in queue and data in buffer.
//
//*****************************************************************************
BOOL ProcessReaderCommand(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	UINT8 readerId;
	READER_Q_INFO_t * tempQPtr;

	// Get reader id using data pointer
	// readerID is always first byte in reader related messages
	readerId = *((UINT8PTR)dataPtr);

	// If reader id is invalid, ignore this command
	if(readerId >= MAX_READERS)
	{
		return FAIL;
	}

	// If reader is not configured, ignore this command
	if(GetReaderTypeCfg(readerId) == NO_READER_PRESENT)
	{
		return FAIL;
	}

	// If reader is INACTIVE, ignore this command but say it's done
	if(readerStatus[readerId] == READER_IS_INACTIVE)
	{
		return SUCCESS;
	}

	// Handle 'abort scan' command differently
	if(msgId == MSG_ABORT_FINGER_SCAN_CMD)
	{
		// It stops processing current command and replaces command info
		// in RDH Q with abort command to process it immediately.
		AbortFingerScan(readerId, dataPtr, length);
		ScanTempCmdToSend[readerId] = TRUE;
		return SUCCESS;
	}
#ifdef BIO_IDT_SRVR_SUPPORT
	// Abort scan template command if it is being processed right now, otherwise newly inserted command
	// will not get processed
	else if( (ReaderData[readerId].currMsgId == MSG_SCAN_TEMPLATE_CMD) && (BioIdentifyServer.enblBioServer == ENABLED) )
	{
		// Add exceptions- if finger already scanned, do not abort command
		if(ReaderData[readerId].fpBeingProcessedF != TRUE)
		{
			//If scan template cmd is running abort
			AbortFingerScan(readerId, dataPtr, length);
			ScanTempCmdToSend[readerId] = TRUE;
		}
	}
#endif

	// Check if reader queue is available for readerId with required buff length
	if(AllocateReaderQ(readerId, length, &tempQPtr) == SUCCESS)
	{
		// Copy command data into buffer of allocated Q
		memcpy((VOIDPTR)((tempQPtr->cmdBuffPtr)->data), dataPtr, length);

		// Set length field in Q to size of command data
		tempQPtr->length = length;

		// Copy MsgId into allocated Q
		tempQPtr->msgId = msgId;

		// Now mark Q status as pending so that command can be processed
		tempQPtr->status= Q_STATUS_PENDING;

		return SUCCESS;
	}
	else // Q not available
	{
		if(msgId == MSG_ENABLE_FREE_SCAN_CMD)
		{
			ReaderData[readerId].freeScanRequireF = TRUE;
		}
		else if(msgId == MSG_SCAN_TEMPLATE_CMD)
		{
			if(BioIdentifyServer.enblBioServer == ENABLED)
			{
				ScanTempCmdToSend[readerId] = TRUE;
				LoadScanTemplateTimer(readerId, SCAN_TEMP_COUNT);
				DPRINT(READER_TASK_LOG,"loading scan template timer");
			}
		}
		ReaderRespFailReason[readerId] = READER_RESP_STATUS_BUFF_UNAVAIL;
		ProcFailedResp(readerId, msgId);
		return FAIL;
	}
}

//*****************************************************************************
//	ReaderDistributionHandler()
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//				This function must be called periodically to process any
//				pending	command, event or response related to reader.
//				Pending commands are passed to appropriate reader driver.
//				Response received from reader is dispatched to degraded mode or
//				comm task if required.
//				It also manages how many times retry should be done to process
//				any failed command.
//				RDH maintains two states for each reader.
//				In IDLE state, it checks for new event from reader or any
//				pending commands.
//				In WAIT_FOR_RESP state, it waits for response from reader for
//				previous commands.
//
//*****************************************************************************
void ReaderDistributionHandler(void)
{
	READER_MSG_INFO_t		readerMsgInfo;		
	READER_EV_PROC_STATUS_e evProcStatus;	// status of event processing
	UINT8 					readerId;
	UINT8 					msgStatus;		// holds status of message 

	//Loop up to MAX_READERS
	for(readerId = READER1; readerId<MAX_READERS; readerId++)
	{
		if( ( (readerId == READER3) || (readerId == READER4) ) && (IsCerFwInProgress() == YES) )
		{
			return;
		}
		// Reader Background initialization state
		switch(GetRDHState(readerId))
		{
			case RDH_RDR_BG_INIT_INFO:
				// Only initialize Cer related informateion if reader 3 is changed
				if( (readerId == READER3) && (YES == CheckComboRdrPresent(COMBO_READER_ANY)))
				{
					InitCerReaderRelatedInfo();
					SendCerBuzzerStatus();
					SendCerCadDuration();

					//Get Systick to Avoid Reconnection Reader (ACDVCSP1-1911)
					SetLastAckRcvdTick();
				}
				InitReaderRelatedInfo(readerId);

				if(GetReaderTypeCfg(readerId) == HID_ICLASS_U_READER)
				{
					CheckNextHidIclassReader(readerId, TRUE);
				}
				// For Mifare reader reset mifare reader type (SM130/PN532) to SM130
				if(GetReaderTypeCfg(readerId) == MIFARE_U_READER)
				{
					CheckNextMifareReader(readerId, TRUE);
				}
				SetRDHState(readerId, RDH_RDR_BG_INIT_PROCESS);
				break;

			case RDH_RDR_BG_INIT_PROCESS:

				InitReader(readerId);
				if(readerStatus[readerId] != READER_STATUS_PROGRESS)
				{
					CompleteReaderBgInitProcess(readerId);
					SendMsgScanTemplate(readerId);
				}
				break;

			default:
				break;
		}

		// If this reader is not configured or reader is inactive, check for next reader
		if( (GetReaderTypeCfg(readerId) == NO_READER_PRESENT) ||
							(readerStatus[readerId] == READER_IS_INACTIVE) )
		{
			continue;
		}

		//Based on current RDH state, do following:
		switch(GetRDHState(readerId))
		{
			// In Idle state, we start processing new command or handle new 
			// reader event.
			case RDH_IDLE_STATE:
			{
				// Check if new event is detected from reader driver
				// If reader processing event, do not process pending command
				evProcStatus = GetEventFromReader(readerId, &readerMsgInfo);
				if(ReaderData[readerId].freeScanRequireF == TRUE)
				{
					ENABLE_FREE_SCAN_CMD_DATA_t enableFreeScan;

					ReaderData[readerId].freeScanRequireF = FALSE;
					enableFreeScan.readerId = readerId;
					ProcessReaderCommand(MSG_ENABLE_FREE_SCAN_CMD, &enableFreeScan, sizeof(ENABLE_FREE_SCAN_CMD_DATA_t));
				}
				else if(evProcStatus == READER_EVENT_DATA)
				{
					// Send it to comm. task or degrade mode handler
					ProcMsg(readerMsgInfo.msgId, readerMsgInfo.dataPtr, readerMsgInfo.length);
				}
				else if(evProcStatus == READER_EVENT_PROCESSING)
				{
					// do nothing
				}
				// Check if there is any pending command for readers
				else if(readerQ[readerId][readerQOutPtr[readerId]].status == Q_STATUS_PENDING)
				{				
					// Do not send command if wait timer is running
					if(readerWaitCmdTmrHandle[readerId] != INVALID_TIMER_HANDLE)
					{
						break;
					}	
					
					//delete scan timer so other command can be processed
					DeleteTimer(&scanTemplateWaitTmrHandle[readerId]);

					// Check if reader is free. 
					if(IsReaderFree(readerId)== YES)
					{
						// Send pending command to reader with retry status
						if(readerRetryCnt[readerId])
						{
							msgStatus = SendCmdToReader(readerId, READER_CMD_RETRY);
						}
						else
						{
							ReaderData[readerId].currMsgId = readerQ[readerId][readerQOutPtr[readerId]].msgId;
							msgStatus = SendCmdToReader(readerId, READER_CMD_NOT_RETRY);
						}

						// If command is sent successfully
						if(msgStatus == READER_CMD_SUCCESS)
						{
							// Change reader queue status to PROCESSING 
							readerQ[readerId][readerQOutPtr[readerId]].status = Q_STATUS_PROCESSING;
							
							// Change RDH state to get response of sent command
							SetRDHState(readerId, RDH_WAIT_FOR_RESP_STATE);
							break;
						}
						// If command can't be sent
						else if(msgStatus == READER_CMD_FAILED)
						{
							// Construct response failed message and send out
							ProcFailedResp(readerId, ReaderData[readerId].currMsgId);
							RdrTaskMsgProcDone(readerId);
						}
					}// end of if reader is free
				} // end of if pending command found
				break;
			}
			
			case RDH_WAIT_FOR_RESP_STATE:
			{
				// Get status of response from the reader.
				msgStatus  = GetRespFromReader(readerId, &readerMsgInfo);
				if(msgStatus == READER_RESP_FAILED)
				{
					DPRINT(READER_TASK_LOG, "RESP FAILED: Reader = %d, msgId = %x, status = %d", (readerId+1), ReaderData[readerId].currMsgId, ReaderRespFailReason[readerId]);

					// Try until max retry 
					if(++readerRetryCnt[readerId] < MAX_READER_CMD_RETRY_COUNT)
					{						
						//Change reader queue status to PENDING
						readerQ[readerId][readerQOutPtr[readerId]].status = Q_STATUS_PENDING;
						//Delete Reader Response Timer
						DeleteTimer(&readerRespTmrHandle[readerId]);
						// Lets go back to idle state. So that we can process new
						// command/event or retry.
						SetRDHState(readerId, RDH_IDLE_STATE);
					}
					else // Tried maximum times. Give up
					{
						// Construct failed response and send it to comm task
						// or degraded mode
						ProcFailedResp(readerId, ReaderData[readerId].currMsgId);
						
						DeleteTimer(&readerWaitCmdTmrHandle[readerId]);
						RdrTaskMsgProcDone(readerId);
					}

				}
				else if( (msgStatus == READER_RESP_NO_DATA) || (msgStatus == READER_RESP_WITH_DATA) )
				{
					if(msgStatus == READER_RESP_WITH_DATA)
					{
						// Command has been successfully processed by reader.  
						// Send response to comm. task or degrade mode handler
						ProcMsg(readerMsgInfo.msgId, readerMsgInfo.dataPtr, readerMsgInfo.length);
					}
					
					// Delete Reader Response Timer
					DeleteTimer(&readerRespTmrHandle[readerId]);
				
					// Clear Reader Retry counter	
					readerRetryCnt[readerId] = 0;
					
					// Set reader status as ACTIVE as valid response received
					// NOTE: Do not update status run time now
					// readerStatus[readerId] = READER_IS_ACTIVE;
					RdrTaskMsgProcDone(readerId);
				}	

				break;
			}
		}// end of switch loop for RDH state
	} // end of for loop-used to process each reader driver
}

//*****************************************************************************
//	ProcFailedResp()
//	Param: 
//		IN :	readerId
//		OUT:	none
//	Returns:	none
//	Description:
//				If reader can't get response successfully for given command,
//				this function will check if fail response is to be send out.
//				If response needs to be send out, this function will constructs
//				it with fail reason	and sends it out.
//*****************************************************************************
static void ProcFailedResp(READER_ID_e readerId, SLAVE_MSG_ID_e cmdMsgId)
{
	SLAVE_MSG_ID_e		respMsgId;
	UINT16				length;
	// This size should be suffiecient for any response, although care should be
	// taken while adding any new response
	UINT8				respData[1024];

	switch(cmdMsgId)
	{
		case MSG_ENROLL_BY_SCAN_CMD:
			((ENROLL_BY_SCAN_RESP_DATA_t *)respData)->fprUserId = 0;
			length = sizeof(ENROLL_BY_SCAN_RESP_DATA_t);
			respMsgId = MSG_ENROLL_BY_SCAN_RESP;
		break;
		
		case MSG_ENROLL_BY_TEMPLATE_CMD:
			((ENROLL_BY_TEMPLATE_RESP_DATA_t *)respData)->fprUserId = ReaderData[readerId].userId;
			((ENROLL_BY_TEMPLATE_RESP_DATA_t *)respData)->fpIndex = ReaderData[readerId].fpIdx;
			((ENROLL_BY_TEMPLATE_RESP_DATA_t *)respData)->enrollmentSource = ReaderData[readerId].fpEnrollSource;
			length = sizeof(ENROLL_BY_TEMPLATE_RESP_DATA_t);
			respMsgId = MSG_ENROLL_BY_TEMPLATE_RESP;
		break;

		case MSG_VERIFY_BY_SCAN_CMD:
			((VERIFY_BY_SCAN_RESP_DATA_t *)respData)->fprUserId = ReaderData[readerId].userId;
			((VERIFY_BY_SCAN_RESP_DATA_t *)respData)->subId = 0;
			length = sizeof(VERIFY_BY_SCAN_RESP_DATA_t);
			respMsgId = MSG_VERIFY_BY_SCAN_RESP;
		break;

		case MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD:
			length = sizeof(VERIFY_HOST_TEMPLATE_BY_SCAN_RESP_DATA_t);
			respMsgId = MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_RESP;
		break;

		case MSG_VERIFY_BY_TEMPLATE_CMD:
			((VERIFY_BY_TEMPLATE_RESP_DATA_t *)respData)->fprUserId = ReaderData[readerId].userId;
			((VERIFY_BY_TEMPLATE_RESP_DATA_t *)respData)->subId = 0;
			length = sizeof(VERIFY_BY_TEMPLATE_RESP_DATA_t);
			respMsgId = MSG_VERIFY_BY_TEMPLATE_RESP;
		break;

		case MSG_READ_TEMPLATE_CMD:
			length = sizeof(READ_TEMPLATE_RESP_DATA_t);
			respMsgId = MSG_READ_TEMPLATE_RESP;
		break;

		case MSG_FPR_AVAIL_TEMPLATE_CNT_CMD:
			((FPR_AVAIL_TEMPLATE_CNT_RESP_DATA_t *)respData)->templateCnt = 0;
			length = sizeof(FPR_AVAIL_TEMPLATE_CNT_RESP_DATA_t);
			respMsgId = MSG_FPR_AVAIL_TEMPLATE_CNT_RESP;
		break;

		case MSG_FPR_ENROLLED_TEMPLATE_CNT_CMD:
			((FPR_ENROLLED_TEMPLATE_CNT_RESP_DATA_t *)respData)->templateCnt = 0;
			length = sizeof(FPR_ENROLLED_TEMPLATE_CNT_RESP_DATA_t);
			respMsgId = MSG_FPR_ENROLLED_TEMPLATE_CNT_RESP;
		break;

		case MSG_USER_TEMPLATE_CNT_CMD:
			((USER_TEMPLATE_CNT_RESP_DATA_t *)respData)->templateCnt = 0;
			((USER_TEMPLATE_CNT_RESP_DATA_t *)respData)->cmdSourceType = ReaderData[readerId].fpEnrollSource;
			length = sizeof(USER_TEMPLATE_CNT_RESP_DATA_t);
			respMsgId = MSG_USER_TEMPLATE_CNT_RESP;
		break;

		case MSG_WRITE_FP_CMD:
			length = sizeof(WRITE_FP_RESP_DATA_t);
			respMsgId = MSG_WRITE_FP_RESP;
		break;

		case MSG_WRITE_SEC_INFO_CMD:
			length = sizeof(WRITE_SEC_INFO_RESP_DATA_t);
			respMsgId = MSG_WRITE_SEC_INFO_RESP;
		break;

		case MSG_WRITE_PERS_INFO_CMD:
			length = sizeof(WRITE_PERS_INFO_RESP_DATA_t);
			respMsgId = MSG_WRITE_PERS_INFO_RESP;
		break;

		case MSG_WRITE_SMART_ROUTE_ACCESS_INFO_CMD:
			length = sizeof(WRITE_SMART_ROUTE_ACCESS_INFO_RESP_DATA_t);
			respMsgId = MSG_WRITE_SMART_ROUTE_ACCESS_INFO_RESP;
		break;

		case MSG_READ_FP_CMD:
			length = sizeof(READ_FP_RESP_DATA_t);
			respMsgId = MSG_READ_FP_RESP;
		break;

		case MSG_READ_SEC_INFO_CMD:
			length = sizeof(READ_SEC_INFO_RESP_DATA_t);
			respMsgId = MSG_READ_SEC_INFO_RESP;
		break;

		case MSG_READ_PERS_INFO_CMD:
			length = sizeof(READ_PERS_INFO_RESP_DATA_t);
			respMsgId = MSG_READ_PERS_INFO_RESP;
		break;

		case MSG_READ_SSA_INFO_CMD:
			length = sizeof(READ_SSA_INFO_RESP_DATA_t);
			respMsgId = MSG_READ_SSA_INFO_RESP;
		break;

		case MSG_DELETE_FP_TEMPLATES_CMD:
			((DELETE_FP_TEMPLATES_RESP_t *)respData)->fprUserId = ReaderData[readerId].userId;
			((DELETE_FP_TEMPLATES_RESP_t *)respData)->deleteMethod = ReaderData[readerId].deleteMethod;
			length = sizeof(DELETE_FP_TEMPLATES_RESP_t);
			respMsgId = MSG_DELETE_FP_TEMPLATES_RESP;
			FpDeletedFromModuleF = FALSE;
		break;

#ifdef CAFETERIA_FEATURE_SUPPORTED
		case MSG_WRITE_VALUE_INFO_CMD:
			length = sizeof(WRITE_VALUE_RESP_DATA_t);
			respMsgId = MSG_WRITE_VALUE_INFO_RESP;
		break;

		case MSG_READ_VALUE_INFO_CMD:
			length = sizeof(READ_VALUE_RESP_DATA_t);
			respMsgId = MSG_READ_VALUE_INFO_RESP;
		break;
#endif

		case MSG_READ_SMART_ROUTE_ACCESS_INFO_CMD:
			length = sizeof(READ_SMART_ROUTE_ACCESS_INFO_RESP_DATA_t);
			respMsgId = MSG_READ_SMART_ROUTE_ACCESS_INFO_RESP;
		break;

		case MSG_CHANGE_SMART_CARD_KEY_CMD:
			length = sizeof(SMART_CARD_KEY_UPDATE_RESP_DATA_t);
			respMsgId = MSG_CHANGE_SMART_CARD_KEY_RESP;
		break;

		case MSG_ERASE_SMART_CARD_CMD:
			length = sizeof(SMART_CARD_ERASE_RESP_DATA_t);
			respMsgId = MSG_ERASE_SMART_CARD_RESP;
		break;

		case MSG_SCAN_TEMPLATE_CMD:
			length = sizeof(ENROLL_BY_SCAN_RESP_DATA_t);
			if(BioIdentifyServer.enblBioServer == ENABLED)
			{
				ScanTempCmdToSend[readerId] = TRUE;
			}
			printf("failed resp scan template FailReason:%d SendCmdFlg:%d \n", ReaderRespFailReason[readerId],ScanTempCmdToSend[readerId]);
			respMsgId = MSG_SCAN_TEMPLATE_RESP;
			break;

		case MSG_READ_CARD_ID_CMD:
			length = sizeof(READ_CARD_ID_RESP_DATA_t);
			respMsgId = MSG_READ_CARD_ID_RESP;
			break;

		case MSG_WRITE_CARD_ID_CMD:
			length = sizeof(WRITE_CARD_ID_RESP_DATA_t);
			respMsgId = MSG_WRITE_CARD_ID_RESP;
			break;

		case MSG_WRITE_SSA_INFO_CMD:
			length = sizeof(WRITE_SSA_INFO_RESP_DATA_t);
			respMsgId = MSG_WRITE_SSA_INFO_RESP;
			break;

		case MSG_READ_SFPR_IMAGE_CMD:
			//NOTE: This is done to complete web jvs task status
			ImageProgressF = SYSTEM_TEST_STATUS_COMPLETE;
			respMsgId = MSG_READ_SFPR_IMAGE_RESP;
			//Do not put break here.
		default:
			return;
		break;
	}

	// This common handling added: considering all response structure 
	// have first two member 'readerId' and 'status' 
	((ENROLL_BY_SCAN_RESP_DATA_t *)respData)->readerId = readerId;
	((ENROLL_BY_SCAN_RESP_DATA_t *)respData)->status = ReaderRespFailReason[readerId];
	ProcMsg(respMsgId, respData, length);
}

//*****************************************************************************
//	AbortFingerScan()
//	Param: 
//		IN :	readerId- reader id on which scanning is to be aborted
//				dataPtr - points where command data is stored
//				length	- length of payload
//		OUT:	none
//	Returns:	none
//	Description:
//				This function aborts command being processed currently.
//				It replaces current command (Q and buffer paramters) with abort
//	   		    command data so it can be processed by RDH immediately.
//*****************************************************************************
static void AbortFingerScan(UINT8 readerId, VOIDPTR dataPtr, UINT16 length)
{
	UINT8 count;
	// If waiting for response for sent command and processing scan related
	// command, abort scan
	if(GetRDHState(readerId) == RDH_WAIT_FOR_RESP_STATE)
	{
		switch(ReaderData[readerId].currMsgId)
		{
			default:
				break;
	
			case MSG_ENROLL_BY_SCAN_CMD:
			case MSG_VERIFY_BY_SCAN_CMD:
			case MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD:
			case MSG_SCAN_TEMPLATE_CMD:
			{
				// ..... abort current command being processed ..... //
		
				// Delete timer used for reader response
				DeleteTimer(&readerRespTmrHandle[readerId]);
				// Clear reader retry counter
				readerRetryCnt[readerId] = 0;
				// Initialize reader RX driver for default receive settings
				InitReaderReceiver(readerId);
		
				// ..... Assign q and buffer to 'abort command' ..... //
		
				READER_Q_INFO_t * qPtr;
				// Set pointer to current out Q
				qPtr = &readerQ[readerId][readerQOutPtr[readerId]];
		
				// Copy command data into buffer of allocated Q
				memcpy((VOIDPTR)((qPtr->cmdBuffPtr)->data), dataPtr, length);
				// Set length field in Q to size of command data
				qPtr->length = length;
				// Copy MsgId into allocated Q
				qPtr->msgId = MSG_ABORT_FINGER_SCAN_CMD;
				// Now mark Q status as pending so that command can be processed
				qPtr->status= Q_STATUS_PENDING;
				// Free up response buffer used for previous command
				if((qPtr->respBuffPtr) != NULL)
				{
				   	 (qPtr->respBuffPtr)->status = READER_BUFF_FREE;
					 (qPtr->respBuffPtr) = NULL;
				}
				
				// ..... change RDH state to IDLE to process command ..... //
				SetRDHState(readerId, RDH_IDLE_STATE);

				break;		
			}
		}
	}

	count = readerQOutPtr[readerId];
	while(count != readerQInPtr[readerId])
	{
		READER_Q_INFO_t * qPtr;
		// Set pointer to current out Q
		qPtr = &readerQ[readerId][count];
		switch(qPtr->msgId)
		{
		case MSG_ENROLL_BY_SCAN_CMD:
		case MSG_VERIFY_BY_SCAN_CMD:
		case MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD:
			// Change buffer status as FREE if buffer is allocated
			if(readerQ[readerId][count].cmdBuffPtr != NULL)
			{
		    	 readerQ[readerId][count].cmdBuffPtr->status = READER_BUFF_FREE;
				 readerQ[readerId][count].cmdBuffPtr = NULL;
			}
			if(readerQ[readerId][count].respBuffPtr != NULL)
			{
			   	 readerQ[readerId][count].respBuffPtr->status = READER_BUFF_FREE;
				 readerQ[readerId][count].respBuffPtr = NULL;
			}
			// Change queue status as FREE
			readerQ[readerId][count].status = Q_STATUS_FREE;
			break;
		default:
			break;
		}

		count++;
		if(count == SIZE_READER_Q)
		{
			count = 0;
		}
	}

	while(readerQOutPtr[readerId] != readerQInPtr[readerId])
	{
		if(readerQ[readerId][readerQOutPtr[readerId]].status == Q_STATUS_FREE)
		{
			++readerQOutPtr[readerId];
			if(readerQOutPtr[readerId] == SIZE_READER_Q)
			{
				readerQOutPtr[readerId] = 0;
			}
		}
		else
		{
			break;
		}
	}
}

//*****************************************************************************
//	AllocateReaderBuff()
//	Param: 
//		IN :	length	- what size of buffer required 	
//		OUT:	buffPtr	- Pointer to buffer if allocated. Else NULL	
//	Returns:	SUCCESS	- if buffer allocation is success.
//				FAIL	- if buffer allocation is failed.
//	Description:
//				It checks for reader buffer availability of required length.
//				If available then it will update given pointer for avalialble 
//				buffer.
//*****************************************************************************
BOOL AllocateReaderBuff(UINT16 length, READER_SMALL_BUFF_t ** buffPtr)
{
	UINT8 buffIdx;
	
	// Look through small buffers
	if(length <= MAX_LEN_READER_S_BUFF)
	{
		for(buffIdx = 0; buffIdx < NUM_READER_S_BUFF; buffIdx++)
		{
			// If found any free buffer, allocate it
			if(readerSBuff[buffIdx].status == READER_BUFF_FREE)
			{
				readerSBuff[buffIdx].status = READER_BUFF_ALLOCATED;
				*buffPtr = &readerSBuff[buffIdx];
				return SUCCESS;
			}
		}
	}
	
	// Look through medium buffers
	if(length <= MAX_LEN_READER_M_BUFF)
	{
		for(buffIdx = 0; buffIdx < NUM_READER_M_BUFF; buffIdx++)
		{	
			// If found any free buffer, allocate it
			if(readerMBuff[buffIdx].status == READER_BUFF_FREE)
			{
				readerMBuff[buffIdx].status = READER_BUFF_ALLOCATED;
				// NOTE: type casted to small buffer to avoid warning
				*buffPtr= (READER_SMALL_BUFF_t *)&readerMBuff[buffIdx];
				return SUCCESS;
			}
		}
	}
	
	// Look through large buffers
	if(length <= MAX_LEN_READER_L_BUFF)
	{
		for(buffIdx = 0; buffIdx < NUM_READER_L_BUFF; buffIdx++)
		{	
			// If found any free buffer, allocate it			
			if(readerLBuff[buffIdx].status == READER_BUFF_FREE)
			{
				readerLBuff[buffIdx].status = READER_BUFF_ALLOCATED;
				// NOTE: type casted to small buffer to avoid warning
				*buffPtr= (READER_SMALL_BUFF_t *)&readerLBuff[buffIdx];
				return SUCCESS;
			}
		}
	}

	// Buffer of requested size is not available
	return FAIL;
}

//*****************************************************************************
//	AllocateReaderQ ()
//	Param: 
//		IN :	Reader id - for which reader queue is needed
//				Buff Size - Size of buffer needed to associate with Q	
//		OUT:	qPtr 	- Pointer to free Q entry
//	Returns:	SUCCESS	- if queue allocation is success.
//				FAIL	- if queue allocation is failed.
//	Description:
//				It checks for reader queue availability.
//				If available then it will update given pointer for avalialble 
//				queue.
//*****************************************************************************
static BOOL AllocateReaderQ(UINT8 readerId, UINT16 buffSize, READER_Q_INFO_t ** qPtr)
{
	READER_SMALL_BUFF_t * buffPtr;
	
	// status of In pointer must be free
	if(readerQ[readerId][readerQInPtr[readerId]].status == Q_STATUS_FREE)
	{
		// Check required size of buffer is available 
		if(AllocateReaderBuff(buffSize, &buffPtr) == SUCCESS)
		{
			readerQ[readerId][readerQInPtr[readerId]].cmdBuffPtr = buffPtr;
			*qPtr = &readerQ[readerId][readerQInPtr[readerId]];
			(*qPtr)->status = Q_STATUS_ALLOCATED;	
			// Increment In pointer, consider rollover
			if(++readerQInPtr[readerId] >= SIZE_READER_Q)
				readerQInPtr[readerId] = 0;
			return SUCCESS;
		}
	}
	
	EPRINT(READER_TASK_LOG, "Failed to allocate reader Q");
	// Q or required size of buffer is not available
	return FAIL;	
}

//*****************************************************************************
//	ReaderRespTimeout()
//	Param: 
//		IN :	readerId- reader id for which response timeout occured 
//		OUT:	none
//	Returns:	none
//	Description:
//				When reader driver sends command to reader, it starts timer to 
//				get response within it.	At that time it passes address of this 
//				function as a call back function to be called up on timeout.
//				It frees up current queue and buffer for given reader id.
//				It resets reader receiver settings.
//				It changes RDH state to IDLE so next command or new event can 
//				be processed.
//
//*****************************************************************************
static void ReaderRespTimeout(UINT8 readerId)
{
	// If Reader Retry Counter is less than MAX_READER_CMD_RETRY_COUNT
	if(++readerRetryCnt[readerId] < MAX_READER_CMD_RETRY_COUNT)
	{						
		// Change reader queue status to PENDING
		readerQ[readerId][readerQOutPtr[readerId]].status = Q_STATUS_PENDING;
		// Delete timer used for reader response
		DeleteTimer(&readerRespTmrHandle[readerId]);
		// Change RDH state to IDLE to process event or next pending command
		SetRDHState(readerId, RDH_IDLE_STATE);
	}
	else // Tried maximum times. Give up
	{
		// Construct failed response and send it to comm task or degraded mode
		ReaderRespFailReason[readerId] = READER_RESP_STATUS_TIME_OUT;
		ProcFailedResp(readerId, ReaderData[readerId].currMsgId);
		RdrTaskMsgProcDone(readerId);
	}

	// Initialize reader RX driver for default receive settings
	InitReaderReceiver(readerId);
}

//*****************************************************************************
//	RdrTaskMsgProcDone()
//	Param: 
//		IN :	readerId
//		OUT:	none
//	Returns:	none
//	Description:
//				It removes message from queue and clears other information that
//				was set for current being processed command and will intializes
//				other information to process for next commands
//*****************************************************************************
static void RdrTaskMsgProcDone(UINT8 readerId)
{
	if(ScanTempCmdToSend[readerId] == TRUE)
	{
		LoadScanTemplateTimer(readerId, SCAN_TEMP_COUNT);
		DPRINT(READER_TASK_LOG,"loading scan template timer");
	}

	// Delete timer used for reader response
	DeleteTimer(&readerRespTmrHandle[readerId]);
	// Clear reader retry counter 
	readerRetryCnt[readerId] = 0;					
	// Free q and buffer that was being processed
	FreeReaderQ(readerId);

	// TODO: //ReaderData[readerId].currMsgId = NO_MSG;

	// Change RDH state to IDLE to process event or next pending command
	SetRDHState(readerId, RDH_IDLE_STATE);
}

//*****************************************************************************
//	FreeReaderQ()
//	Param: 
//		IN :	readerId - which reader's current queue need to free
//		OUT:	none
//	Returns:	none
//	Description:
//				It frees current queue being processed.
//				
//*****************************************************************************
static void FreeReaderQ(UINT8 readerId)
{
	// Change buffer status as FREE if buffer is allocated
	if(readerQ[readerId][readerQOutPtr[readerId]].cmdBuffPtr != NULL)
	{
    	 readerQ[readerId][readerQOutPtr[readerId]].cmdBuffPtr->status = READER_BUFF_FREE;
		 readerQ[readerId][readerQOutPtr[readerId]].cmdBuffPtr = NULL;
	}
	if(readerQ[readerId][readerQOutPtr[readerId]].respBuffPtr != NULL)
	{
	   	 readerQ[readerId][readerQOutPtr[readerId]].respBuffPtr->status = READER_BUFF_FREE;
		 readerQ[readerId][readerQOutPtr[readerId]].respBuffPtr = NULL;
	}
	// Change queue status as FREE
	readerQ[readerId][readerQOutPtr[readerId]].status = Q_STATUS_FREE;
	
	// Increment Q Out pointer
	// Consider rollover

	while(readerQOutPtr[readerId] != readerQInPtr[readerId])
	{
		if(readerQ[readerId][readerQOutPtr[readerId]].status == Q_STATUS_FREE)
		{
			if(++readerQOutPtr[readerId] >= SIZE_READER_Q)
				readerQOutPtr[readerId] = 0;
		}
		else
		{
			break;
		}
	}
}

//*****************************************************************************
//	GetReaderRespBuffPtr()
//	Param: 
//		IN :	readerId
//		OUT:	none
//	Returns:	response buffer pointer
//	Description:
//				It provides data buffer address where response should be stored.
//				
//*****************************************************************************
READER_SMALL_BUFF_t * GetReaderRespBuffPtr(UINT8 readerId)
{
	return (readerQ[readerId][readerQOutPtr[readerId]].respBuffPtr);
}

//*****************************************************************************
//	SetReaderRespBuffPtr()
//	Param: 
//		IN :	readerId - for which reader id response buffer is to be set
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//				It sets given address as response buffer in reader Q info.
//				
//*****************************************************************************
BOOL SetReaderRespBuffPtr(UINT8 readerId, READER_SMALL_BUFF_t * respBuffPtr)
{
	readerQ[readerId][readerQOutPtr[readerId]].respBuffPtr = respBuffPtr;
	return SUCCESS;
}

//*****************************************************************************
//	GetReaderOutMsg()
//	Param: 
//		IN :	readerId - for which reader id message info is required
//		OUT:	none
//	Returns:	message information
//	Description:
//				It gives reader message information like message id, 
//				command/resp buffer ptr, message length
//				
//*****************************************************************************
READER_Q_INFO_t GetReaderOutMsg(UINT8 readerId)
{
	return readerQ[readerId][readerQOutPtr[readerId]];
}

//*****************************************************************************
//	GetReaderEvBuffPtr()
//	Param: 
//		IN :	readerId - for which reader event buffer pointer is required
//		OUT:	none
//	Returns:	event buffer pointer
//	Description:
//				It provides buffer pointer where event data should be stored.
//
//*****************************************************************************
VOIDPTR GetReaderEvBuffPtr(UINT8 readerId)
{
	return (&evDataBuffer[readerId][0]);
}

//*****************************************************************************
//	SendCmdToReader()
//	Param: 
//		IN :	readerId	- to which reader command to be send
//				retryFlag	- flag indicating whether this is retry command
//		OUT:	none
//	Returns:	command status received from reader driver
//	Description:
//				Based on reader type configured, it will pass command to 
//				appropriate reader driver.
//				
//*****************************************************************************
static READER_CMD_PROC_STATUS_e SendCmdToReader(UINT8 readerId, BOOL retryFlag)
{
	UINT8 cmdStatus;

	//Switch based on reader type configured
	switch(GetReaderTypeCfg(readerId))
	{
		case HID_ICLASS_U_READER:
			cmdStatus = SendHidIclassUartCmd(readerId);
		break;
		
		case SM_PROX_READER:
			 cmdStatus = SendSM125UartCmd(readerId, retryFlag);
		break;
		
		case MIFARE_U_READER:
			cmdStatus = SendMifareUartCmd(readerId, retryFlag);
		break;
		
		case SUP_FP_3030_READER:
			cmdStatus = SendSuprema3030UartCmd(readerId, retryFlag);
		break;

		case CB_U_READER:
		case ATOM_RD100_READER:
			cmdStatus = SendExternalBleCmd(readerId, retryFlag);
		break;

		case ATOM_RD200_READER:
		case ATOM_RD300_READER:
			cmdStatus = SendAtomReaderBleCmd(readerId, retryFlag);
		break;

		default: 	// command can't be send to this reader
			ReaderRespFailReason[readerId] = READER_RESP_STATUS_INVALID_INPUT;
			cmdStatus = READER_CMD_FAILED;
		break;
	} // End of switch- readerType

	return cmdStatus;

}

//*****************************************************************************
//	GetRespFromReader()
//	Param: 
//		IN :	readerId			- to which reader command to be send
//		OUT:	readerMsgInfoPtr	- where response info should get copied
//	Returns:	response status received from reader driver
//	Description:
//				Based on reader type configured, it will get response status 
//				from appropriate reader driver.
//				
//*****************************************************************************
static READER_RESP_PROC_STATUS_e GetRespFromReader(UINT8 readerId, READER_MSG_INFO_t * readerMsgInfoPtr)
{
	UINT8 respStatus;
	//Switch based on reader type configured
	switch(GetReaderTypeCfg(readerId))
	{
		case HID_ICLASS_U_READER:
			respStatus = GetHidIclassUartResp(readerId, readerMsgInfoPtr);
		break;
		
		case SM_PROX_READER:
			 respStatus = GetSM125UartResp(readerId, readerMsgInfoPtr);
		break;
		
		case MIFARE_U_READER:
			respStatus = GetMifareUartResp(readerId, readerMsgInfoPtr);
		break;
		
		case SUP_FP_3030_READER:
			respStatus = GetSuprema3030UartResp(readerId, readerMsgInfoPtr);
		break;

		case CB_U_READER:
		case ATOM_RD100_READER:
			respStatus = GetExternalBleResp(readerId, readerMsgInfoPtr);
			break;

		case ATOM_RD200_READER:
		case ATOM_RD300_READER:
			respStatus = GetAtomReaderBleResp(readerId, readerMsgInfoPtr);
			break;

		default: 	// command can't be send to this reader
			ReaderRespFailReason[readerId] = READER_RESP_STATUS_INVALID_INPUT;
			respStatus = READER_RESP_FAILED;
		break;
	} // End of switch- readerType

	return respStatus;
}

//*****************************************************************************
//	GetEventFromReader()
//	Param: 
//		IN :	readerId			- readerID of command to be send
//		OUT:	readerMsgInfoPtr	- where event info should get copied
//				
//	Returns:	event status received from reader driver
//	Description:
//				Based on reader type configured, it will get event status from
//				appropriate reader driver.
//				
//*****************************************************************************
static READER_EV_PROC_STATUS_e GetEventFromReader(UINT8 readerId, READER_MSG_INFO_t * readerMsgInfoPtr)
{
	READER_EV_PROC_STATUS_e evStatus;
	
	//Switch based on reader type configured
	switch(GetReaderTypeCfg(readerId))
	{
		case MIFARE_W_READER:
		case HID_ICLASS_W_READER:
		case HID_PROX_READER:
		case PIN_W_READER:
		case UHF_W_READER:
		case CB_W_READER:
			evStatus = GetEvFromWiegandReader(readerId, readerMsgInfoPtr);
		break;

		case HID_ICLASS_U_READER:
			if(IsProcessingHidSEReader(readerId) == TRUE)
			{
				evStatus = GetHidIclassSeUartEv(readerId, readerMsgInfoPtr);
			}
			else
			{
				evStatus = GetHidIclassUartEv(readerId, readerMsgInfoPtr);
			}
		break;
		
		case SM_PROX_READER:
			evStatus = GetSM125UartEv(readerId, readerMsgInfoPtr);	
		break;
		
		case MIFARE_U_READER:
			if(IsProcessingCLR663MifareReader(readerId) == TRUE)
			{
				evStatus = GetCLR663UartEv(readerId, readerMsgInfoPtr);
			}
			else if(IsProcessingPN532MifareReader(readerId) == TRUE)
			{
				evStatus = GetPN532UartEv(readerId, readerMsgInfoPtr);
			}
			else
			{
				evStatus = GetSM130UartEv(readerId, readerMsgInfoPtr);
			}
		break;
		
		case SUP_FP_3030_READER:
			evStatus = GetSuprema3030UartEv(readerId, readerMsgInfoPtr);
		break;

		case CB_U_READER:
		case ATOM_RD100_READER:
			evStatus = GetExternalBleEv(readerId, readerMsgInfoPtr);
			break;

		case ATOM_RD200_READER:
		case ATOM_RD300_READER:
			evStatus = GetAtomReaderBleEv(readerId, readerMsgInfoPtr);
			break;

		default:
			evStatus = NO_READER_EVENT;	
		break;
	} // End of switch- readerType

	return evStatus;
}

//*****************************************************************************
//	GetEvFromWiegandReader()
//	Param:
//		IN :	readerId		-readerID of command to be send
//		OUT:	readerMsgInfoPtr-points to where event info should get copied
//	Returns:	event status received from reader driver
//	Description:
//				It checks for received data on wiegand port.
//				If data is received, it validates card format and parity.
//				If valid, it construct event and send it to RDH.
//
//	Note:		In CSN bytes LSB is at lower address.
//
//*****************************************************************************
static READER_EV_PROC_STATUS_e GetEvFromWiegandReader(UINT8 readerId, READER_MSG_INFO_t * readerMsgInfoPtr)
{
	READER_RX_STATUS_e	rxStatus;	// status of rx port

	// Get status of Rx port
	rxStatus = GetReaderPortStatus(readerId, READER_RX_PORT);
	// If receive is done
	if(rxStatus == READER_RX_RCV_DONE)
	{
		UINT8 		rcvdDataCnt;	// to hold received data count on wiegand port
		UINT8PTR 	rcvdDataPtr;	// points out to received data on wiegand port
		UINT8		cardFormat = CARD_FORMAT_UNKNOWN;
		UINT8		loc;
		UINT8 		cardId[MAX_CARD_ID_LEN] __attribute__((aligned(MAX_CARD_ID_LEN)));
		UINT8		cardIdByteLen, cardIdBitCnt, tmpBitIdx, bitMask;

		CARD_DETECTED_EV_DATA_t * cardDetEvPtr;	// to points out event buffer
		WGND_PIN_DATA_t			* wgndPinDetEvPtr; // to points out event buffer

		// Get receive data pointer where card id is stored
		rcvdDataPtr = GetReaderRcvDataPtr(readerId);

		// Get receive data count
		rcvdDataCnt = (UINT8)GetReaderRcvDataCnt(readerId);

		if( (readerId == READER3) && (GetReaderTypeCfg(readerId) == PIN_W_READER) )
		{
			wgndPinDetEvPtr = GetReaderEvBuffPtr(readerId);
			// Initialize reader RX driver for default receive settings
			InitReaderReceiver(readerId);
			if( (rcvdDataCnt == 4) || (rcvdDataCnt == 8) )
			{
				wgndPinDetEvPtr -> rcvdData = (UINT32)(rcvdDataPtr[0] & 0x0F);
				wgndPinDetEvPtr -> rcvdDataBitLen = 4;
			}
			else if(rcvdDataCnt == 26)
			{
				wgndPinDetEvPtr -> rcvdData = (UINT32)( ( (*( (UINT32PTR)rcvdDataPtr) ) & (0x01FFFE) ) >> 1);
				wgndPinDetEvPtr -> rcvdDataBitLen = 16;
			}
			else
			{
				return NO_READER_EVENT;	// ignored event data
			}
			wgndPinDetEvPtr -> readerId = READER3;
			// Update structure- readerMsgInfo parameters
			readerMsgInfoPtr->msgId = MSG_PIN_DETECTED_ON_WGND_EVENT;
			readerMsgInfoPtr->dataPtr = (UINT8PTR)wgndPinDetEvPtr;
			readerMsgInfoPtr->length = sizeof(WGND_PIN_DATA_t);
			return	READER_EVENT_DATA;
		}
		else
		{
			// Get event data pointer
			cardDetEvPtr = GetReaderEvBuffPtr(readerId);

			// Ignore card event, if not received enough data
			if(rcvdDataCnt < 10)
			{
				InitReaderReceiver(readerId);
				return NO_READER_EVENT;	// ignored event data
			}

			// Check receive data counter
			if(rcvdDataCnt == 26)
			{
				cardFormat = CARD_FORMAT_26BITS;
				memcpy(cardId, rcvdDataPtr, MAX_CARD_ID_LEN);
			}
			else if(rcvdDataCnt == 35)
			{
				cardFormat = CARD_FORMAT_HID_35BITS;
				memcpy(cardId, rcvdDataPtr, MAX_CARD_ID_LEN);
			}
			else if(rcvdDataCnt == 37)
			{
				cardFormat = CARD_FORMAT_HID_37BITS;
				memcpy(cardId, rcvdDataPtr, MAX_CARD_ID_LEN);
			}
			else if(rcvdDataCnt == 32)
			{
				cardDetEvPtr->fCInCSN = 0;
				for(loc=0;loc<4;loc++)
				{
					cardId[3-loc] = rcvdDataPtr[loc];
				}
			}
			else if(rcvdDataCnt == 56)	// Mifare 1K/4K or UL detected
			{
				cardDetEvPtr->fCInCSN = 0;
				for(loc=0;loc<7;loc++)
				{
					cardId[6-loc] = rcvdDataPtr[loc];
				}
			}
			// If received data is 34 bit and Card Reader Type is Mifare Wiegand reader
			else if( (rcvdDataCnt == 34) && (GetReaderTypeCfg(readerId) == MIFARE_W_READER) )
			{
				cardFormat = CARD_FORMAT_MIFARE_34BITS;
				memcpy(cardId, rcvdDataPtr, MAX_CARD_ID_LEN);
			}
			else // card with unknown format detected
			{
				cardDetEvPtr->fCInCSN = 0;
				memcpy(cardId, rcvdDataPtr, MAX_CARD_ID_LEN);
			}

			//setting other bit to zero
			cardIdByteLen = (rcvdDataCnt / 8);
			cardIdBitCnt = (rcvdDataCnt % 8);
			if(cardIdBitCnt > 0)
			{
				cardIdByteLen += 1;
			}
			memset(&cardId[cardIdByteLen], 0, (MAX_CARD_ID_LEN - cardIdByteLen) );
			if(cardIdBitCnt > 0)
			{
				bitMask = 0;
				for(tmpBitIdx = 0; tmpBitIdx < cardIdBitCnt; tmpBitIdx++)
				{
					bitMask |= (1 << tmpBitIdx);
				}
				cardId[cardIdByteLen - 1] = (cardId[cardIdByteLen - 1] & bitMask);
			}
			cardDetEvPtr->cardId = 0;
			GetFinalCardId(cardFormat, readerId, cardDetEvPtr, cardId, rcvdDataCnt);

			// Construct message frame as per system design @ evDataBuffer
			cardDetEvPtr->readerId = readerId;
			cardDetEvPtr->cardType = READ_ONLY_CARD;
			// Following info is already filled
			//cardDetEvPtr->facilityCode
			//cardDetEvPtr->cardId
			cardDetEvPtr->addlSecCode = 0;
			cardDetEvPtr->doorLevel = 0;
			cardDetEvPtr->doorMode = 0;
			cardDetEvPtr->doorLevelF = CLEAR;
			cardDetEvPtr->doorModeF = CLEAR;
			cardDetEvPtr->addlSecCodeF = CLEAR;
			cardDetEvPtr->fCInCardMem = 0;

			// Update structure- readerMsgInfo parameters
			readerMsgInfoPtr->msgId = MSG_CARD_DETECTED_EVENT;
			readerMsgInfoPtr->dataPtr = (UINT8PTR)cardDetEvPtr;
			readerMsgInfoPtr->length = sizeof(CARD_DETECTED_EV_DATA_t);

			DPRINT(READER_TASK_LOG, "Card Type %s, Card ID: %llu", CardTypeStr[cardDetEvPtr -> cardType], cardDetEvPtr -> cardId);

			// Initialize reader RX driver for default receive settings
			InitReaderReceiver(readerId);

			if(GetReaderTypeCfg(readerId) == UHF_W_READER)
			{
				if(IsUhfCardValid(cardDetEvPtr->cardId) == TRUE)
				{
					return	READER_EVENT_DATA;
				}
				else
				{
					return	NO_READER_EVENT;
				}
			}
			else
			{
				return	READER_EVENT_DATA;
			}
		}
	}
	else
	{
		return NO_READER_EVENT;	// event not received
	}
}

//*****************************************************************************
//	InitReaderTask()
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//				It initializes and sets reader task related variables.
//				It must be called in init routine after configuration of the 
//				system has been read.
//				
//*****************************************************************************
void InitReaderTask(void)
{
	READER_ID_e readerId;
	UINT8		fprPort;

	InitReaderRxTxDriver();
	FreeAllReaderBuff();
	CalibrateFpSensorF = IsSensorCalibrationRequire();

	if(YES == CheckComboRdrPresent(COMBO_READER_ANY))
	{
		SetReaderTypeCfg(READER3, GetAutoDetectedReaderType(READER3));
	}

	for(readerId = 0; readerId < MAX_READERS; readerId++)
	{
		DeleteTimer(&readerRespTmrHandle[readerId]);
		DeleteTimer(&readerWaitCmdTmrHandle[readerId]);
		InitReaderRelatedInfo(readerId);
		ReaderInitInfo.state[readerId] = READER_INIT_IDLE;
		CheckNextHidIclassReader(readerId, TRUE);
		CheckNextMifareReader(readerId, TRUE);
		readerStatus[readerId] = READER_STATUS_PROGRESS;

		if(GetReaderTypeCfg(readerId) == NO_READER_PRESENT)
		{
			FprUninitReqdF[readerId] = TRUE;
		}
		else
		{
			FprUninitReqdF[readerId] = FALSE;
		}
	}

	for(fprPort = 0; fprPort < MAX_FPR_PORT; fprPort++)
	{
		SensorCalibStatus[fprPort] = CALIB_NOT_REQUIRE;
	}

	while(1)
	{
		for(readerId = 0; readerId < MAX_READERS; readerId++)
		{
			if(readerStatus[readerId] != READER_STATUS_PROGRESS)
			{
				continue;
			}

			InitReader(readerId);

			if(readerStatus[readerId] != READER_STATUS_PROGRESS)
			{
				DPRINT(READER_TASK_LOG, "reader%d status = %s type: %d", readerId+1, ReaderStatusStr[readerStatus[readerId]], GetReaderTypeCfg(readerId));
				// reader mifare loading and handling
				ReaderInitInfo.state[readerId] = MAX_READER_INIT;
				if(readerStatus[readerId] != READER_IS_ACTIVE)
				{
					if(GetReaderTypeCfg(readerId) == HID_ICLASS_U_READER)
					{
						if(CheckNextHidIclassReader(readerId, FALSE) == TRUE)
						{
							ReaderInitInfo.state[readerId] = READER_INIT_IDLE;
							readerStatus[readerId] = READER_STATUS_PROGRESS;
						}
					}
					if(GetReaderTypeCfg(readerId) == MIFARE_U_READER)
					{
						// Check for next type ogf mifare reader and if it is
						// valid type initialize it
						if(CheckNextMifareReader(readerId, FALSE) == TRUE)
						{
							ReaderInitInfo.state[readerId] = READER_INIT_IDLE;
							readerStatus[readerId] = READER_STATUS_PROGRESS;
						}
					}
				}

				SendMsgScanTemplate(readerId);
			}
		}

		for(readerId = 0; readerId< MAX_READERS; readerId++)
		{
			if(readerStatus[readerId] == READER_STATUS_PROGRESS)
			{
				break;
			}
		}

		if(readerId ==  MAX_READERS)
		{
			CalibrateFpSensorF = FALSE;
			break;
		}
	}
	for(readerId = 0; readerId< MAX_READERS; readerId++)
	{
		//continuous retry background init if mifare reader not active
		if( (readerStatus[readerId] != READER_IS_ACTIVE) && (GetReaderTypeCfg(readerId) == MIFARE_U_READER) )
		{
			StartReaderInitProcess(readerId);
		}

		/** Send Message to External reader */
		if (readerId == READER3)
		{
			SendReaderMsgOnInitComplete();
		}
	}
}

//*****************************************************************************
//	InitReaderRelatedInfo()
//	Param:
//		IN :	readerId
//		OUT:	none
//	Returns:	none
//	Description:
//				It initializes reader related common information like processing
//				state and queue and buffer.
//*****************************************************************************
void InitReaderRelatedInfo(READER_ID_e readerId)
{
	UINT8 cnt;

	// Start with empty Q
	readerQInPtr[readerId] = 0;
	readerQOutPtr[readerId] = 0;

	// Initialize each Q entry
	for(cnt=0; cnt < SIZE_READER_Q; ++cnt)
	{
		readerQ[readerId][cnt].status = Q_STATUS_FREE;
		readerQ[readerId][cnt].cmdBuffPtr = NULL;
		readerQ[readerId][cnt].respBuffPtr = NULL;
	}

	DelayMiliSecs(10);
	SetRDHState(readerId, RDH_IDLE_STATE);

	readerRetryCnt[readerId] = 0;
	DeleteTimer(&readerRespTmrHandle[readerId]);
	readerStatus[readerId] = READER_IS_INACTIVE;
	DeleteTimer(&readerWaitCmdTmrHandle[readerId]);

	// Disable reader's communication port
	DisableReaderWiegand(readerId);
	ResetReaderUartProcess(readerId);

	if( (readerId < READER3) || (NO == CheckComboRdrPresent(COMBO_READER_ALL)))
	{
		DisableReaderUart(readerId);
	}
}

//*****************************************************************************
//	InitReader()
//	Param: 
//		IN :	readerId
//		OUT:	none
//	Returns:	none
//	Description:
//				It initializes reader.
//*****************************************************************************
static void InitReader(READER_ID_e readerId)
{
	// Switch based on reader type
	switch(GetReaderTypeCfg(readerId))
	{
		case MIFARE_W_READER:
		case HID_ICLASS_W_READER:
		case CB_W_READER:
			readerStatus[readerId] = InitHidIclassWiegand(readerId);
		break;

		case PIN_W_READER:
		{
			EnableReaderWiegand(readerId);
			readerStatus[readerId] = READER_STATUS_UNKNOWN;
		}
		break;

		case HID_PROX_READER:
			readerStatus[readerId] = InitHidProxPoint(readerId);
		break;

		case HID_ICLASS_U_READER:
			if(IsProcessingHidSEReader(readerId) == YES)
			{
				readerStatus[readerId] = InitHidSeReaderUart(readerId);
			}
			else
			{
				readerStatus[readerId] = InitHidIclassUart(readerId);
			}
		break;

		case SM_PROX_READER:
			readerStatus[readerId] = InitSM125Uart(readerId);
		break;

		case MIFARE_U_READER:

			if(IsProcessingCLR663MifareReader(readerId) == YES)
			{
				readerStatus[readerId] = InitCLR663Uart(readerId);
			}
			else if(IsProcessingPN532MifareReader(readerId) == YES)
			{
				readerStatus[readerId] = InitPN532Uart(readerId);
			}
			else
			{
				readerStatus[readerId] = InitSM130Uart(readerId);
			}
		break;

		case SUP_FP_3030_READER:
			readerStatus[readerId] = InitSuprema3030(readerId);
		break;

		case UHF_W_READER:
			readerStatus[readerId] = InitUhfWiegandReader(readerId);
		break;

		case CB_U_READER:
		case ATOM_RD100_READER:
			readerStatus[readerId] = InitExternalBleReader(readerId);
		break;

		case ATOM_RD200_READER:
		case ATOM_RD300_READER:
			readerStatus[readerId] = InitAtomBleReader(readerId);
		break;

		default:
			//check if FP reader configured previously then only uninit
			if(FprUninitReqdF[readerId] == TRUE )
			{
				readerStatus[readerId] = UninitSuprema3030(readerId);
				if(readerStatus[readerId] != READER_STATUS_PROGRESS)
				{
					FprUninitReqdF[readerId] = FALSE;//uninit finished
				}
			}
			else
			{
				readerStatus[readerId] = READER_NOT_CONFIGURED;
			}
			break;
	} // End of switch- readerType

	//DPRINT(SYS_LOG, "reader%d status = %s type: %d", readerId+1, ReaderStatusStr[readerStatus[readerId]], READER_TYPE(readerId));
}

//*****************************************************************************
//	SetReaderRespTimer
//	Param: 
//		IN :	readerId	- for which reader timer is to be start or reload
//				timerValue	- timer value in multiple of 100 msec
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function starts or reloads timer for given reader
//				
//*****************************************************************************
BOOL SetReaderRespTimer(UINT8 readerId, UINT16 timerValue)
{
	if(timerValue <= 0)
	{
		// Delete Reader Response Timer
		DeleteTimer(&readerRespTmrHandle[readerId]);
		return SUCCESS;
	}
	if(readerRespTmrHandle[readerId] == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = timerValue;
		tInfo.data = readerId;
		tInfo.funcPtr = (VOIDPTR)&ReaderRespTimeout;
		return (StartTimer(tInfo, &readerRespTmrHandle[readerId], __FUNCTION__));
	}
	else //reload timer
	{
		return (ReloadTimer(readerRespTmrHandle[readerId], timerValue));
	}
}

//*****************************************************************************
//	SetReaderWaitCmdTimer
//	Param: 
//		IN :	readerId	- for which reader timer is to be start or reload
//				timerValue	- timer value in multiple of 100 msec
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function starts or reloads timer for given reader.
//				
//*****************************************************************************
BOOL SetReaderWaitCmdTimer(UINT8 readerId, UINT16 timerValue)
{
	if(readerWaitCmdTmrHandle[readerId] == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = timerValue;
		tInfo.data = readerId;
		tInfo.funcPtr = (VOIDPTR)&ReaderWaitCmdTimeout;
		return (StartTimer(tInfo, &readerWaitCmdTmrHandle[readerId], __FUNCTION__));
	}
	else //reload timer
	{
		return (ReloadTimer(readerWaitCmdTmrHandle[readerId], timerValue));
	}
}

//*****************************************************************************
//	ReaderWaitCmdTimeout
//	Param: 
//		IN :	readerId	- for which reader timer is to be start or reload
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function initialize timer handle used for 'wait to send 
//				command to reader'.
//				
//*****************************************************************************
static void ReaderWaitCmdTimeout(UINT8 readerId)
{
	DeleteTimer(&readerWaitCmdTmrHandle[readerId]);
}

//*****************************************************************************
//	FreeAllReaderBuff()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				It will free all reader buffers
//
//*****************************************************************************
static void FreeAllReaderBuff(void)
{
	UINT8 buffIdx;

	for(buffIdx = 0; buffIdx < NUM_READER_S_BUFF; buffIdx++)
	{
		readerSBuff[buffIdx].status = READER_BUFF_FREE;
	}

	for(buffIdx = 0; buffIdx < NUM_READER_M_BUFF; buffIdx++)
	{
		readerMBuff[buffIdx].status = READER_BUFF_FREE;
	}

	for(buffIdx = 0; buffIdx < NUM_READER_L_BUFF; buffIdx++)
	{
		readerLBuff[buffIdx].status = READER_BUFF_FREE;
	}
}

//*****************************************************************************
//	ReadSfprBinFile()
//	Param:
//		IN : Address - Address from where data reading start
//			 *data	- data buffer address
//		OUT: NONE
//	Returns: SUCCESS/FAIL
//	Description:
//			Reads Suprema Finger readers firmware file from given address.
//*****************************************************************************
BOOL ReadSfprBinFile(UINT32 address, UINT8PTR data, UINT16 size)
{
	BOOL	retVal = FALSE;
	FILE	*fd;
	struct 	stat	buf;

    if(stat(FprFwUpgrade.fileName, &buf) == 0)
	{
		if((UINT32)buf.st_size == FprFwUpgrade.firmwareSize)
		{
			fd = fopen(FprFwUpgrade.fileName, "r");
			if(fd == NULL)
			{
				EPRINT(READER_TASK_LOG,"Failed to open Sfpr file");
			}
			else
			{
				fseek(fd, (address * sizeof(UINT8)), SEEK_SET);
				if(fread(data, sizeof(UINT8), size, fd) > 0)
				{
					//DPRINT(READER_TASK_LOG, "Read packet at address [%d] Successfully", address);
					retVal = SUCCESS;
				}
				else
				{
					EPRINT(READER_TASK_LOG,"Failed to Read Sfpr file");
				}
				fclose(fd);
			}
		}
		else
		{
			EPRINT(READER_TASK_LOG,"Wrong Suprema Firmware file ");
		}
	}
	return retVal;
}

//*****************************************************************************
//	StartReaderInitProcess()
//	Param:
//		IN :	rxData
//		OUT:	none
//	Returns:	none
//	Description:
//				It will switch RDH to initialize reader in background
//				This is called whenever reader type is changed
//*****************************************************************************
void StartReaderInitProcess(UINT8 readerId)
{
	if( (readerId == READER3) && (YES == CheckComboRdrPresent(COMBO_READER_ANY)))
	{
		SetReaderTypeCfg(readerId, ReaderAutoDetectInfo.readerType[readerId]);
	}
	FreeReaderQ(readerId);
	//check current reader status & if not active then uninit not required
	if( (readerStatus[readerId] != READER_IS_ACTIVE) || (GetReaderTypeCfg(readerId) != NO_READER_PRESENT) )
	{
		FprUninitReqdF[readerId] = FALSE;
	}

	else if(scanTemplateWaitTmrHandle[readerId] != INVALID_TIMER_HANDLE)
	{
		DeleteTimer(&scanTemplateWaitTmrHandle[readerId]);
	}

	ReaderInitInfo.state[readerId] = READER_INIT_IDLE;
	SetRDHState(readerId, RDH_RDR_BG_INIT_INFO);
	readerStatus[readerId] = READER_STATUS_PROGRESS;
	DPRINT(READER_TASK_LOG,"Reader Type[%d]", GetReaderTypeCfg(readerId));
}

//*****************************************************************************
//	CompleteReaderBgInitProcess()
//	Param:
//		IN :	rxData
//		OUT:	none
//	Returns:	none
//	Description:
//				It will stop reader background task and swtich RDH to IDLE state
//				to perform normal operation
//				Exception: For mifare reader type, PN532/SM130 type is switched for
//				for apropriate reader identification and initialization.
//*****************************************************************************
static void CompleteReaderBgInitProcess(UINT8 readerId)
{
	UINT8	rdrIdx = 0;

	SetRDHState(readerId, RDH_IDLE_STATE);
	ReaderInitInfo.state[readerId] = MAX_READER_INIT;

	DPRINT(READER_TASK_LOG, "reader%d status = %s type: %d", readerId+1, ReaderStatusStr[readerStatus[readerId]], GetReaderTypeCfg(readerId));

	if( (readerStatus[readerId] != READER_IS_ACTIVE) &&
			(GetReaderTypeCfg(readerId) == HID_ICLASS_U_READER) )
	{
		if(CheckNextHidIclassReader(readerId, FALSE) == TRUE)
		{
			readerStatus[readerId] = READER_STATUS_PROGRESS;
			ReaderInitInfo.state[readerId] = READER_INIT_IDLE;
			SetRDHState(readerId, RDH_RDR_BG_INIT_PROCESS);
		}
	}

	if( (readerStatus[readerId] != READER_IS_ACTIVE) &&
			(GetReaderTypeCfg(readerId) == MIFARE_U_READER) )
	{
		if(CheckNextMifareReader(readerId, FALSE) == TRUE)
		{
			readerStatus[readerId] = READER_STATUS_PROGRESS;
			ReaderInitInfo.state[readerId] = READER_INIT_IDLE;
			SetRDHState(readerId, RDH_RDR_BG_INIT_PROCESS);
		}
	}

	for(rdrIdx = 0; rdrIdx< MAX_READERS; rdrIdx++)
	{
		if(readerStatus[rdrIdx] == READER_STATUS_PROGRESS)
		{
			break;
		}
		//continuous retry background init if mifare reader not active
		if( (readerStatus[rdrIdx] != READER_IS_ACTIVE) && (GetReaderTypeCfg(rdrIdx) == MIFARE_U_READER) )
		{
			StartReaderInitProcess(rdrIdx);
		}
	}

	if (readerId == READER3)
	{
		SendReaderMsgOnInitComplete();
	}

	if(rdrIdx == MAX_READERS)
	{
		if(GetDoorApplType() == PANEL_DOOR_APPL)
		{
			SendStatusResponse();
		}
		// TODO:Update UI message
	}
}

//*****************************************************************************
//	LoadScanTemplateTimer
//	Param:
//		IN :	readerId	- for which reader timer is to be start or reload
//				timerValue	- timer value in multiple of 100 msec
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function starts or reloads scan template cmd timer for given reader
//
//*****************************************************************************
BOOL LoadScanTemplateTimer(UINT8 readerId, UINT16 timerValue)
{
	if(scanTemplateWaitTmrHandle[readerId] == INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = timerValue;
		tInfo.data = readerId;
		tInfo.funcPtr = (VOIDPTR)&ScanTempWaitTimeout;
		return (StartTimer(tInfo, &scanTemplateWaitTmrHandle[readerId], __FUNCTION__));
	}
	else //reload timer
	{
		return (ReloadTimer(scanTemplateWaitTmrHandle[readerId], timerValue));
	}
}

//*****************************************************************************
//	ScanTempWaitTimeout
//	Param:
//		IN :  	UINT32 readerId
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function gets called on timeout
//
//*****************************************************************************
static void ScanTempWaitTimeout(UINT8 readerId)
{
	DeleteTimer(&scanTemplateWaitTmrHandle[readerId]);
	SendMsgScanTemplate(readerId);
	ScanTempCmdToSend[readerId] = FALSE;
}

//*****************************************************************************
//	CheckAnyFpReaderCfg
//	Param:
//		IN :  	respPort
//		OUT:	readerId
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function checks configured FP reader on given response port
//
//*****************************************************************************
BOOL CheckAnyFpReaderCfg(RESPONSE_PORT_e respPort, READER_ID_e * readerIdPtr)
{
	UINT8	readerId;
	BOOL	readerCfgStatus = FAIL;

	if(readerIdPtr != NULL)
	{
		*readerIdPtr = INVALID_READER;

		for(readerId = 0; readerId < MAX_READERS; readerId++)
		{
			if(respPort == GetReaderRespPort(readerId))
			{
				if(GetReaderTypeCfg(readerId) == SUP_FP_3030_READER)
				{
					readerCfgStatus = SUCCESS;
					*readerIdPtr = readerId;
					break;
				}
			}
		}
	}

	return readerCfgStatus;
}

//*****************************************************************************
//	InitReaderRelatedVars
//	Param:
//		IN :  	none
//		OUT:	none
//	Returns:	none
//	Description:
//				This function inits reader related variables
//*****************************************************************************
void InitReaderRelatedVars(void)
{
	UINT8 readerId;
	for(readerId = 0; readerId < MAX_READERS; readerId++)
	{
		readerRespTmrHandle[readerId] = INVALID_TIMER_HANDLE;
		readerWaitCmdTmrHandle[readerId] = INVALID_TIMER_HANDLE;
		scanTemplateWaitTmrHandle[readerId] = INVALID_TIMER_HANDLE;
		ReaderData[readerId].seekTagRetryCnt = 0;
		ReaderData[readerId].ignoreDetectWaitF = FALSE;
		ReaderData[readerId].freeScanRequireF = FALSE;
	}
}

#ifdef BIO_IDT_SRVR_SUPPORT
//*****************************************************************************
//	SendMsgScanTemplate()
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//				It sends message to finger print reader to read user's template.
//
//*****************************************************************************
void SendMsgScanTemplate(UINT8 readerId)
{
	SCAN_TEMPLATE_CMD_DATA_t		scanTemplate;

#ifdef DEVICE_IS_FMX
	if( (GetReaderTypeCfg(readerId) != FINGER_PRINT_READER) || (readerStatus[readerId] != READER_IS_ACTIVE) )
#else
	if( (GetReaderTypeCfg(readerId) != SUP_FP_3030_READER) || (readerStatus[readerId] != READER_IS_ACTIVE) )
#endif
	{
		return;
	}

	if(BioIdentifyServer.enblBioServer == ENABLED)
	{
		scanTemplate.readerId = readerId;
		ProcessReaderCommand(MSG_SCAN_TEMPLATE_CMD, (UINT8PTR)&scanTemplate, sizeof(SCAN_TEMPLATE_CMD_DATA_t));
	}
}
#endif

//*****************************************************************************
//	Init_Internal_FP_Rdr_UsbNode_Detection()
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	BOOL
//	Description:
//				On startup, it check internal FP reader USB node is detected or not
//
//*****************************************************************************
BOOL Init_Internal_FP_Rdr_UsbNode_Detection()
{
	// NOTE: Please re-visit when PALM reader is supported on READER2
	// internal FP reader dev file READER2_UART_DEV_FILE "/dev/ttyACM0" is detected or not in ARGO
	INT32 fd = open(READER2_UART_DEV_FILE, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	if(fd < 0)
	{
		DPRINT(SYS_LOG, "USB DEV of READER2=%s not detected", READER2_UART_DEV_FILE);
		// There is possiblity of first time boot with UART interface
		return FALSE;
	}
	close(fd);
	
	// "/sys/bus/usb/devices/1-1/idVendor" SUPREMA_6030_FP_RDR_VENDOR_ID_FILE --> SUPREMA_6030_FP_RDR_VENDOR_ID 16d1
	// "/sys/bus/usb/devices/1-1/idProduct" SUPREMA_6030_FP_RDR_PRODUCT_ID_FILE --> SUPREMA_6030_FP_RDR_PRODUCT_ID a4a7
	
	/*
	UCHAR tStrBuf[100];
	FILE *pFile = NULL;
	BOOL tIsDetailMatched = FALSE;
	pFile = fopen(SUPREMA_6030_FP_RDR_VENDOR_ID_FILE, "r");
	if (pFile == NULL)
	{
		DPRINT(SYS_LOG, "%s file open failed", SUPREMA_6030_FP_RDR_VENDOR_ID_FILE);
		return FALSE;
	}
	while (!feof(pFile))
	{
		memset (tStrBuf, 0, sizeof(tStrBuf));
		if(NULL == fgets(tStrBuf, sizeof(tStrBuf)-1,pFile))
		{
			break;
		}
		if(0 == strcasecmp(tStrBuf,SUPREMA_6030_FP_RDR_VENDOR_ID))
		{
			tIsDetailMatched = TRUE;
			break;
		}
	}
	fclose(pFile);
	if(FALSE == tIsDetailMatched)
	{
		DPRINT(SYS_LOG, "vendor-id [%s] does not found in %s", SUPREMA_6030_FP_RDR_VENDOR_ID, SUPREMA_6030_FP_RDR_VENDOR_ID_FILE);
		return FALSE;
	}
	
	pFile = NULL;
	tIsDetailMatched = FALSE;
	pFile = fopen(SUPREMA_6030_FP_RDR_PRODUCT_ID_FILE, "r");
	if (pFile == NULL)
	{
		DPRINT(SYS_LOG, "%s file open failed", SUPREMA_6030_FP_RDR_PRODUCT_ID_FILE);
		return FALSE;
	}
	while (!feof(pFile))
	{
		memset (tStrBuf, 0, sizeof(tStrBuf));
		if(NULL == fgets(tStrBuf, sizeof(tStrBuf)-1,pFile))
		{
			break;
		}
		if(0 == strcasecmp(tStrBuf,SUPREMA_6030_FP_RDR_PRODUCT_ID))
		{
			tIsDetailMatched = TRUE;
			break;
		}
	}
	fclose(pFile);
	if(FALSE == tIsDetailMatched)
	{
		DPRINT(SYS_LOG, "product-id [%s] does not found in %s", SUPREMA_6030_FP_RDR_PRODUCT_ID, SUPREMA_6030_FP_RDR_PRODUCT_ID_FILE);
		return FALSE;
	}
	*/
	
	DPRINT(SYS_LOG, "USB DEV of READER2=%s detected", READER2_UART_DEV_FILE);
	// Variant has internal FP reader module
	SetIsVariant_With_Internal_FP_Rdr(TRUE);
	return TRUE;
}

//*****************************************************************************
//	IsVariant_With_Internal_FP_Rdr()
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	BOOL
//	Description:
//				Function returns boolean value whether variant has Internal 
//				finger print reader or not
//*****************************************************************************
BOOL IsVariant_With_Internal_FP_Rdr()
{
	return gIsVariant_With_Internal_FP_Rdr;
}
//*****************************************************************************
//	SetIsVariant_With_Internal_FP_Rdr()
//	Param:
//		IN :	BOOL Val
//		OUT:	none
//	Returns:	void
//	Description:
//				Sets global variable used for variant has Internal finger print
//				reader or not
//*****************************************************************************
void SetIsVariant_With_Internal_FP_Rdr(BOOL Val)
{
	// check validation
	if( (Val != TRUE) && (Val != FALSE) )
		return;
	
	// Debug log
	if(Val != gIsVariant_With_Internal_FP_Rdr)
	{
		DPRINT(SYS_LOG, "OldValue=%d NewValue=%d", gIsVariant_With_Internal_FP_Rdr, Val);
	}
	
	gIsVariant_With_Internal_FP_Rdr = Val;
}
