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
//   File         : MxMsgHandler.c
//   Description  : This file parse command coming from master to related task.
//
/////////////////////////////////////////////////////////////////////////////

// macro; to make use of recursive mutex
#define _GNU_SOURCE

//*** Place all include files here ***
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#include "MxDeviceSpecific.h"
#include "MxTypedef.h"
#include "MxSlaveMsg.h"
#include "MxMsgHandler.h"
#include "MxPDDcTask.h"
#include "MxCommTask.h"
#include "MxReaderTask.h"
#include "MxSupremaReader.h"
#include "MxGpio.h"
#include "MxSysTime.h"
#include "MxEventLog.h"
#include "MxConfig.h"
#include "MxConfigFileIo.h"
#include "MxLogs.h"
#include "MxSysUtils.h"
#include "MxGuiHandler.h"
#include "MxCerTask.h"
#include "MxWifiPort.h"
#include "MxMenu.h"
#include "MxNetworkUtils.h"
#include "MxDisplayMessage.h"
#include "MxReaderUtils.h"
#include "MxSmartCardFormatConfig.h"
#include "MxConfigUpdateHandler.h"
#include "MxWiegandOutConfig.h"
#include "MxWiegandOutput.h"
#include "MxDcTask.h"
#include "MxMultiLangConfig.h"
#include "MxAcmsFileTransfer.h"
#include "MxSysCommonUtils.h"
#include "MxReaderCommonUtils.h"
#include "MxBluetoothConfig.h"
#include "MxSlaveMsgCommon.h"
#include "MxWebJvsWifiSetting.h"
#include "MxCbBluetooth.h"
#include "MxReaderBluetoothConfig.h"
#include "MxCommonBLE.h"
#include "MxSlaveCommon.h"
#include "MxReaderGuiMessage.h"
#include "MxUserConfig.h"
#include "MxUdpPing.h"
#include "MxTempUserTask.h"
#include "MxCommonConfig.h"

//******** Extern Variables **********
extern	SYS_BASIC_PARA_t			SysBasicPara;
extern	SYS_ADVANCE_PARA_t			SysAdvPara;
extern	UINT8 						LogicSlaveId;
extern 	TIME_t						CurrTime;
extern 	FPR_CONFIG_t				FprConfig;
extern 	READER_ACTIVITY_INFO_t		ReaderData[];
extern	COMM_MODE_e					CommMode;
extern  READERS_PARAMETER_t			ReadersParaConfig;
extern	PASSWORD_t			  		Password;
extern 	STATUS_BAR_INFO_t			CurrStatusbarData;
extern  INPUT_STATUS_t				SysInputStatus;
extern	READER_STATUS_e 			readerStatus[];			// holds reader status (active/inactive/unknown)
extern 	CALIB_STATUS_e				SensorCalibStatus[];
extern	BOOL						WifiWpsTrigF;
extern	WIFI_PARA_t					WifiParameters;
extern CARD_FORMAT_CONFIG_t			CardFormatConf[MAX_CARD_FORMAT_PER_DOOR];
extern SMART_CARD_FORMAT_t 			SmartCardFormat[MAX_ACMS_CARD_TYPE];
extern SMART_CARD_CONFIG_t			SmartCardCustomConfig[MAX_ACMS_CARD_TYPE][MAX_SMART_CARD_CUSTOM_FIELDS];
extern WIEGAND_OUT_CUSTOM_FORMAT_t	WiegnadOutCustomFormat[MAX_COSTUM_FORMAT_WIEGAND_OUT_IDX];
extern DOOR_ACTIVITY_INFO_t			DcInfo;
extern ETHERNET_PARA_t		  		EthernetParameters;
extern SERVER_PARA_t		  		ServerParameters;
extern WIFI_PARA_t					WifiParameters;;
extern MMC_INFO_t 					MmcInfo;
extern MULTI_LANG_DISP_STR_CONFIG_t	MultiLangDispStrConfig;
extern BLUETOOTH_CONFIG_t			BluetoothPara;
extern pthread_mutex_t 				MutexLockAutoSwitchWiFi;
extern BOOL							ManualWifiConnectF;
extern READER_BLUETOOTH_CONFIG_t	ReaderBluetoothPara;
extern SYSTEM_INFORMATION_t             systemInformation;
extern TEMP_USER_ADD_INFO_t 		TempUserAddActivity;
//****** Defines and Data Types ******
#define SIZE_EVENT_RESP_BUFF		1024
#define LOC_EV_CNT					0
#define LOC_EV_PENDING_F			1

//******** Function Prototypes *******
static void ProcessMsg(SLAVE_MSG_ID_e msgId, UINT8PTR * dataPtr, UINT16 length);

//******** Global Variables **********
ACCESS_CONTROL_RESPONSE_e	NetworkStatusResp = ACCESS_CONTROL_RESP_MAX;
pthread_t					fwUpgradeFromPanelThreadData = 0;
THREAD_STATUS_e				fwUpgradeFromPanelThreadStatus = THREAD_INACTIVE;

//******** Static Variables **********

//******** Function Definations ******
//*****************************************************************************
//	MsgHandler()
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//				It checks if there is any pending command in comm task queue.
//				If found then, it will get command information and pass it to
//				appropriate task in DC.
//				This function must be called periodically from super loop.
//
//*****************************************************************************
void MsgHandler(void)
{
	SLAVE_MSG_ID_e 		msgId				__attribute__((aligned(4)));
	UINT16				length				__attribute__((aligned(4)));
	VOIDPTR				dataPtr				__attribute__((aligned(4)));
	UINT8PTR			currPtr				__attribute__((aligned(4)));
	MSG_RECV_STATUS_e	msgStatus;
	UINT8 				noOfCmd;
	UINT8 				cnt;

	// Check if there is any message pending in comm. task
	// Use Comm buffer to avoid copy. We will later free up the buffer.
	msgStatus = GetMsgNoCopy((SLAVE_MSG_ID_e *)&msgId, &length, &dataPtr);

	// make working copy of dataPtr since we need dataPtr intact to free it later
	currPtr = (UINT8PTR)dataPtr;

	if(msgStatus == MSG_RECIEVED) // Found pending message
	{
		if(msgId == MSG_MULTI_CMD)	// if multiple commands in a single messages
		{
			// First byte should give us number of commands within a message
			noOfCmd = *currPtr++;

			// Now loop through each commands and process it
			for (cnt = 0; cnt < noOfCmd; cnt++)
			{
				// Get message id and data pointer for command
				memcpy(&msgId, currPtr, sizeof(UINT16) );
				currPtr += sizeof(UINT16);
				DPRINT(COMM_LOG,"UDP msg received via MULTI_CMD [%x]",msgId);
				// Process individual commands. Data pointer is incremented
				// within this function to get next command’s data pointer.
				ProcessMsg(msgId, &currPtr, length);
			}
		}
		else // Only one command within a message
		{
			ProcessMsg(msgId, &currPtr, length);
		}

		// Done processing commands. Now free comm. buffer
		FreeCommBuffer(dataPtr);
	}
}

//*****************************************************************************
//	ProcessMsg()
//	Param:
//		IN :	None
//		OUT:	msgId		- message Id of next message in Q
//				dataPtr		- points to message payload
//				length		- length of message payload in Q
//	Returns:	none
//	Description:
//
//*****************************************************************************
static void ProcessMsg(SLAVE_MSG_ID_e msgId, UINT8PTR * dataPtr, UINT16 length)
{
	switch(msgId)
	{
		case MSG_DOOR_REBOOT_CMD:
		{
			SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
		}break;
		case MSG_RTC_CMD:
		{
			// create instance of required structure and copy data
			RTC_CMD_DATA_t	dateTime;
			DATE_DDMMYYYY_t	dateDDMMYY;

			if(length < sizeof(RTC_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(RTC_CMD_DATA_t) );
				break;
			}

			memcpy(&dateTime, *dataPtr, sizeof(dateTime) );

			dateDDMMYY.day = dateTime.date.day;
			dateDDMMYY.month = dateTime.date.month;
			dateDDMMYY.year = dateTime.date.year + YEAR_OFFSET_2000;

			// updates date, time in RTC hardware and software
			ChangeDateTime(dateDDMMYY, dateTime.time, LOCAL_TIME);

			//To update DST related Info.
			SendMessageToQt(" ", GUI_MSG_SEND_CONFIG_INFO); //For Sending  Time Format

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(RTC_CMD_DATA_t);
		}
		break;

		case MSG_GET_EVENT_LOG_CMD:
		{
			GET_EV_LOG_RESP_DATA_t	evLogRespData;
			DEG_EV_LOG_DATA_t		evLogData;
			GET_EV_STATUS_e			evStatus;
			UINT8 					cnt;
			UINT8					data[SIZE_EVENT_RESP_BUFF];
			UINT8 					maxEvCnt;
			UINT8PTR 				tempPtr;

			if(length < sizeof(GET_EV_LOG_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(GET_EV_LOG_CMD_DATA_t) );
				break;
			}

			tempPtr = data;

			// increment data pointer for the fields which will be updated later
			tempPtr += sizeof(evLogRespData.noOfEvCnt) + sizeof(evLogRespData.pendingEvF);

			// Initialize no. of event cnt and next event pending flag
			evLogRespData.noOfEvCnt = 0;
			evLogRespData.pendingEvF = CLEAR;

			maxEvCnt = SIZE_EVENT_RESP_BUFF/sizeof(GET_EV_LOG_RESP_DATA_t);

			// get event(s)
			for(cnt = 0; cnt < **dataPtr && cnt < maxEvCnt; cnt++)
			{
				evStatus = ReadPDEventLog(&evLogData);

				if(evStatus == NO_PENDING_EV)
				{
					evLogRespData.pendingEvF = CLEAR;
					break;
				}
				else
				{
					evLogRespData.noOfEvCnt++;
					//Fill data in data field of event log data
					memcpy(tempPtr, &evLogData, sizeof(DEG_EV_LOG_DATA_t));
					//Increment count to point to the next empty data field
					tempPtr += sizeof(DEG_EV_LOG_DATA_t);

					// if next event is pending set flag else clear
					if(evStatus == NEXT_EV_IS_PENDING)
					{
						evLogRespData.pendingEvF = SET;
					}
					else
					{
						evLogRespData.pendingEvF = CLEAR;
						break;
					}
				}
			}

			data[LOC_EV_CNT] = evLogRespData.noOfEvCnt;
			data[LOC_EV_PENDING_F] = evLogRespData.pendingEvF;

			// send out message
			SENDMSG(MSG_GET_EVENT_LOG_RESP, (VOIDPTR)data, (UINT16)(tempPtr - data));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(GET_EV_LOG_CMD_DATA_t);
		}
		break;

        case GUI_SCREEN_TOUCH_EVENT:
        {
            EPRINT(GUI_LOG, "SendMessageToQt(), GUI_SCREEN_TOUCH_EVENT");
            SetLCDDimTimer();
         }
	 break;

		case MSG_GET_DC_STATUS_CMD:
		{

			SendStatusResponse();
			// Input data is NONE, so no need to update dataPtr for next message
		}
		break;

		case MSG_CLOSE_DOOR_CMD:
		{
			DeactivateDoorRelay();
			// Input data is NONE, so no need to update dataPtr for next message
		}
		break;

		case MSG_OPEN_DOOR_CMD:
		{
			// copy data pointer
			OPEN_DOOR_CMD_DATA_FLOAT_t * tempPtr = (VOIDPTR)(*dataPtr);

			if(length < sizeof(OPEN_DOOR_CMD_DATA_FLOAT_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(OPEN_DOOR_CMD_DATA_FLOAT_t) );
				break;
			}

			ActivateDoorRelay(tempPtr->pulseTime);
			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(OPEN_DOOR_CMD_DATA_FLOAT_t);
		}
		break;

		case MSG_BUZZER_CMD:
		{
			// copy data pointer
			BUZZER_CMD_DATA_t * tempPtr = (VOIDPTR)(*dataPtr);

			if(length < sizeof(BUZZER_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(BUZZER_CMD_DATA_t) );
				break;
			}

			SetBuzzer(tempPtr->cadence, tempPtr->respPort);
			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(BUZZER_CMD_DATA_t);
		}
		break;

		case MSG_LED_CMD:
		{
			// copy data pointer
			LED_CMD_DATA_t * tempPtr = (VOIDPTR)(*dataPtr);

			if(length < sizeof(LED_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(LED_CMD_DATA_t) );
				break;
			}
			SetLed(tempPtr->cadence, tempPtr->respPort);
			if( (tempPtr->cadence == ACCESS_OFFLINE_RESP) ||
				(tempPtr->cadence == ACCESS_ONLINE_RESP) ||
				(tempPtr->cadence == ACCESS_DEGRADE_RESP) )
			{
				NetworkStatusResp = tempPtr->cadence;
			}
			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(LED_CMD_DATA_t);
		}
		break;

		case MSG_AUX_OUTPUT_CMD:
		{
			// copy data pointer
			AUX_OP_CMD_DATA_t * tempPtr = (VOIDPTR)(*dataPtr);

			if(length < sizeof(AUX_OP_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(AUX_OP_CMD_DATA_t) );
				break;
			}

			if(tempPtr->setType == SET)
			{
				ActivateAuxRelay(tempPtr->pulseTime);
			}
			else
			{
				DeactivateAuxRelay();
			}

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(AUX_OP_CMD_DATA_t);
		}
		break;

		case MSG_ENROLL_BY_SCAN_CMD:
		{
			if(length < sizeof(ENROLL_BY_SCAN_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(ENROLL_BY_SCAN_CMD_DATA_t) );
				break;
			}
			// process reader command to enroll user by scanning
			ProcessReaderCommand(msgId, *dataPtr, sizeof(ENROLL_BY_SCAN_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(ENROLL_BY_SCAN_CMD_DATA_t);
		}
		break;

		case MSG_ENROLL_BY_TEMPLATE_CMD:
		{
			// copy data pointer
			ENROLL_BY_TEMPLATE_CMD_DATA_t * tempPtr = (ENROLL_BY_TEMPLATE_CMD_DATA_t*)(*dataPtr);
			READER_ID_e idx;
			FILE_WRITE_STATUS_e	fileWriteStutus;

			if(length < sizeof(ENROLL_BY_TEMPLATE_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(ENROLL_BY_TEMPLATE_CMD_DATA_t) );
				break;
			}
			fileWriteStutus = WriteUserFpIntoFile(tempPtr->fprUserId, tempPtr->fpIndex, tempPtr->template);
			
			//Here userIdx is UserId
			if(fileWriteStutus != FILE_WRITE_SUCCESS)
			{
				ENROLL_BY_TEMPLATE_RESP_DATA_t	enrollByTemplateResp;

				enrollByTemplateResp.readerId = tempPtr->readerId;
				enrollByTemplateResp.enrollmentSource = tempPtr->enrollmentSource;
				enrollByTemplateResp.fpIndex = tempPtr->fpIndex;
				enrollByTemplateResp.fprUserId = tempPtr->fprUserId;

				if(fileWriteStutus == FILE_WRITE_FAIL_NO_MEMORY)
				{
					enrollByTemplateResp.status = READER_RESP_STATUS_MEM_FULL;
				}
				else
				{
					enrollByTemplateResp.status = READER_RESP_STATUS_INVALID_RESP;
				}

				ProcMsg(MSG_ENROLL_BY_TEMPLATE_RESP, &enrollByTemplateResp, sizeof(ENROLL_BY_TEMPLATE_RESP_DATA_t));
				return;
			}
			// check if message is for all finger print readers
			if(tempPtr->readerId == ALL_FP_READERS)
			{
				for(idx=0; idx<MAX_READERS; idx++)
				{
					if(SysBasicPara.reader[idx].type == SUP_FP_3030_READER)
					{
						// Ignore if it is not for this reader
						if( (LogicSlaveId == tempPtr->excDoorId) && (idx == tempPtr->excReaderId) )
							continue;

						tempPtr->readerId = idx;
						// process reader command to enroll user by scanning
						ProcessReaderCommand(msgId, *dataPtr, sizeof(ENROLL_BY_TEMPLATE_CMD_DATA_t));
					}
				}
			}
			else // message is for specific finger print reader
			{
				// process reader command to enroll user by scanning
				ProcessReaderCommand(msgId, *dataPtr, sizeof(ENROLL_BY_TEMPLATE_CMD_DATA_t));
			}

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(ENROLL_BY_TEMPLATE_CMD_DATA_t);
		}
		break;

		case MSG_VERIFY_BY_SCAN_CMD:
		{
			if(length < sizeof(VERIFY_BY_SCAN_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(VERIFY_BY_SCAN_CMD_DATA_t) );
				break;
			}
			// process reader command to enroll user by scanning
			ProcessReaderCommand(msgId, *dataPtr, sizeof(VERIFY_BY_SCAN_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(VERIFY_BY_SCAN_CMD_DATA_t);
		}
		break;

		case MSG_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD:
		{
			VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t* cmdPtr = (VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t*) (*dataPtr);
			VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t	verifyHostTemplateByScan;

			if( (length != sizeof(PD_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t) ) && (length != sizeof(VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t) ) )

			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(PD_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t) );
				break;
			}

			verifyHostTemplateByScan.readerId = cmdPtr -> readerId;
			verifyHostTemplateByScan.noOfTemplates = cmdPtr -> noOfTemplates;
			verifyHostTemplateByScan.responseTime = cmdPtr -> responseTime;
			memcpy(verifyHostTemplateByScan.template, cmdPtr -> template, (MAX_SIZE_SINGLE_SFM_TEMPLATE*verifyHostTemplateByScan.noOfTemplates));

			// process reader command to enroll user by scanning
			SendVerifyHostTemplateByScanCmd(verifyHostTemplateByScan);

			// Update data pointer for next messages
			*dataPtr = *dataPtr + length;
		}
		break;

		case MSG_LINUX_PD_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD:
		{
			UINT8												idx;
			VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t				verifyHostTemplateByScan;
			LINUX_PD_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t*	cmdPtr = (LINUX_PD_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t*) (*dataPtr);
			BOOL status;

			if(length < sizeof(LINUX_PD_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t))
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(LINUX_PD_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t) );
				break;
			}


			memset(&verifyHostTemplateByScan, 0, sizeof(verifyHostTemplateByScan));
			verifyHostTemplateByScan.readerId = cmdPtr -> readerId;
			verifyHostTemplateByScan.noOfTemplates = cmdPtr -> noOfTemplates;
			verifyHostTemplateByScan.responseTime = cmdPtr -> responseTime;

			for(idx = 0; idx < (cmdPtr -> noOfTemplates); idx++)
			{
				status = ReadUserFpFromFile(cmdPtr->fprUserId, idx, &verifyHostTemplateByScan.template[idx*MAX_SIZE_SINGLE_SFM_TEMPLATE]);
				if(status == FAIL)
				{
					verifyHostTemplateByScan.noOfTemplates = idx;
					break;
				}
				DPRINT(DC_LOG, "ReadUserFpFromFile MSG_LINUX_PD_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD status %u\n",status);
			}

			SendVerifyHostTemplateByScanCmd(verifyHostTemplateByScan);

			// Update data pointer for next messages
			*dataPtr = *dataPtr + length;
		}
		break;

		case MSG_VERIFY_BY_TEMPLATE_CMD:
		{
			if( (length != sizeof(VERIFY_BY_TEMPLATE_CMD_DATA_t) ) && (length != sizeof(PD_VERIFY_BY_TEMPLATE_CMD_DATA_t) ) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(PD_VERIFY_BY_TEMPLATE_CMD_DATA_t) );
				break;
			}
			// process reader command to enroll user by scanning
			ProcessReaderCommand(msgId, *dataPtr, length);

			// Update data pointer for next messages
			*dataPtr = *dataPtr + length;
		}
		break;

		case MSG_READ_TEMPLATE_CMD:
		{
			if(length < sizeof(READ_TEMPLATE_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(READ_TEMPLATE_CMD_DATA_t) );
				break;
			}
			// process reader command to enroll user by scanning
			ProcessReaderCommand(msgId, *dataPtr, sizeof(READ_TEMPLATE_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(READ_TEMPLATE_CMD_DATA_t);
		}
		break;
		
		case MSG_READ_SFPR_IMAGE_CMD:
		{
			ProcessReaderCommand(msgId, *dataPtr, sizeof(READER_ID_e));
			*dataPtr = *dataPtr + sizeof(READER_ID_e);
		}
		break;
		
		case MSG_UPDATE_USER_IMAGE_CMD:
		{
			if(length < sizeof(UPDATE_IMAGE_SYNC_CMD_DATA_t))
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(UPDATE_IMAGE_SYNC_CMD_DATA_t) );
				break;
			}

			UPDATE_IMAGE_SYNC_CMD_DATA_t		imageUpdateCmdData;
			PHOTO_SYNC_TYPE_e updateProfileType = MAX_PHOTO_SYNC_TYPE;

			memcpy(&imageUpdateCmdData, *dataPtr, sizeof(imageUpdateCmdData));

			if(imageUpdateCmdData.profilePhotoUpdateCnt > 0)
			{
				updateProfileType = PROFILE_PHOTO_TYPE;
				HandlePhotoSyncCmd(updateProfileType, imageUpdateCmdData.userIdx);
				EPRINT(DC_LOG,"GET_UFI Command Success For Profile");
			}
			
			if(imageUpdateCmdData.profilePhotoDeleteF == TRUE)
			{
				HandleDeleteUserPhotoCmd(imageUpdateCmdData.userIdx);
			}

			SendMsg(MSG_UPDATE_USER_IMAGE_RESP, &imageUpdateCmdData, sizeof(UPDATE_IMAGE_SYNC_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(UPDATE_IMAGE_SYNC_CMD_DATA_t);
		}
		break;

		case MSG_DELETE_FP_TEMPLATES_CMD:
		{
			// copy data pointer
			DELETE_USER_FP_CMD_DATA_t * tempPtr = (VOIDPTR)(*dataPtr);
			READER_ID_e idx;
			if(length < sizeof(DELETE_USER_FP_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(DELETE_USER_FP_CMD_DATA_t) );
				break;
			}

			if(tempPtr->deleteMethod == DELETE_ALL_TEMPLATES)
			{
				DeleteAllUsersFpFiles();
			}


			if(tempPtr->deleteMethod == DELETE_SINGLE_USER)
			{
				DeleteSingleUserFpFiles(tempPtr->fprUserId, ALL_USER_FP);
			}

			if(tempPtr->fprUserId == TEMP_USER_ID)
			{
				ResetTempUserAddInfo();
			}
			
			// check if message is for all finger print readers
			if(tempPtr->readerId == ALL_FP_READERS)
			{
				for(idx=0; idx<MAX_READERS; idx++)
				{
					if(SysBasicPara.reader[idx].type == SUP_FP_3030_READER)
					{
						tempPtr->readerId = idx;
						// process reader command to delete user
						ProcessReaderCommand(msgId, *dataPtr, sizeof(DELETE_USER_FP_CMD_DATA_t));
					}
				}
			}
			else // message is for specific finger print reader
			{
				// process reader command to delete user
				ProcessReaderCommand(msgId, *dataPtr, sizeof(DELETE_USER_FP_CMD_DATA_t));
			}

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(DELETE_USER_FP_CMD_DATA_t);
		}
		break;

		case MSG_FPR_AVAIL_TEMPLATE_CNT_CMD:
		{
			if(length < sizeof(FPR_AVAIL_TEMPLATE_CNT_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(FPR_AVAIL_TEMPLATE_CNT_CMD_DATA_t) );
				break;
			}
			// process reader command to enroll user by scanning
			ProcessReaderCommand(msgId, *dataPtr, sizeof(FPR_AVAIL_TEMPLATE_CNT_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(FPR_AVAIL_TEMPLATE_CNT_CMD_DATA_t);
		}
		break;

		case MSG_FPR_ENROLLED_TEMPLATE_CNT_CMD:
		{
			if(length < sizeof(FPR_ENROLLED_TEMPLATE_CNT_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(FPR_ENROLLED_TEMPLATE_CNT_CMD_DATA_t) );
				break;
			}
			// process reader command to enroll user by scanning
			ProcessReaderCommand(msgId, *dataPtr, sizeof(FPR_ENROLLED_TEMPLATE_CNT_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(FPR_ENROLLED_TEMPLATE_CNT_CMD_DATA_t);
		}
		break;

		case MSG_USER_TEMPLATE_CNT_CMD:
		{
			if(length < sizeof(USER_TEMPLATE_CNT_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(USER_TEMPLATE_CNT_CMD_DATA_t) );
				break;
			}
			// process reader command to enroll user by scanning
			ProcessReaderCommand(msgId, *dataPtr, sizeof(USER_TEMPLATE_CNT_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(USER_TEMPLATE_CNT_CMD_DATA_t);
		}
		break;

		case MSG_ABORT_FINGER_SCAN_CMD:
		{
			if(length < sizeof(ABORT_FINGER_SCAN_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(ABORT_FINGER_SCAN_CMD_DATA_t) );
				break;
			}
			// process reader command to enroll user by scanning
			ProcessReaderCommand(msgId, *dataPtr, sizeof(ABORT_FINGER_SCAN_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(ABORT_FINGER_SCAN_CMD_DATA_t);
		}
		break;

		case MSG_SET_FP_SCAN_WAIT_TIME_CMD:
		{
			if(length < sizeof(SET_FP_SCAN_WAIT_TIME_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(SET_FP_SCAN_WAIT_TIME_CMD_DATA_t) );
				break;
			}
			// process reader command to enroll user by scanning
			ProcessReaderCommand(msgId, *dataPtr, sizeof(SET_FP_SCAN_WAIT_TIME_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(SET_FP_SCAN_WAIT_TIME_CMD_DATA_t);
		}
		break;

		case MSG_WRITE_FP_CMD:
		{
			if(length < sizeof(WRITE_FP_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(WRITE_FP_CMD_DATA_t) );
				break;
			}
			// process reader command to write user finger print template into card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(WRITE_FP_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(WRITE_FP_CMD_DATA_t);
		}
		break;

		case MSG_WRITE_SEC_INFO_CMD:
		{
			if(length < sizeof(WRITE_SEC_INFO_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(WRITE_SEC_INFO_CMD_DATA_t) );
				break;
			}
			// process reader command to write user security information into card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(WRITE_SEC_INFO_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(WRITE_SEC_INFO_CMD_DATA_t);
		}
		break;

		case MSG_WRITE_PERS_INFO_CMD:
		{
			if(length < sizeof(WRITE_PERS_INFO_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(WRITE_PERS_INFO_CMD_DATA_t) );
				break;
			}
			// process reader command to write user personalization information into card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(WRITE_PERS_INFO_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(WRITE_PERS_INFO_CMD_DATA_t);
		}
		break;

		case MSG_WRITE_SMART_ROUTE_ACCESS_INFO_CMD:
		{
			if(length < sizeof(WRITE_SMART_ROUTE_ACCESS_INFO_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(WRITE_SMART_ROUTE_ACCESS_INFO_CMD_DATA_t) );
				break;
			}
			// process reader command to write smart route access information into card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(WRITE_SMART_ROUTE_ACCESS_INFO_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(WRITE_SMART_ROUTE_ACCESS_INFO_CMD_DATA_t);
		}
		break;

		case MSG_WRITE_CARD_ID_CMD:
		{
			if(length < sizeof(WRITE_CARD_ID_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(WRITE_CARD_ID_CMD_DATA_t) );
				break;
			}
			// process reader command to write user security information into card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(WRITE_CARD_ID_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(WRITE_CARD_ID_CMD_DATA_t);
		}
		break;

		case MSG_WRITE_SSA_INFO_CMD:
		{
			if(length < sizeof(WRITE_SSA_INFO_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(WRITE_SSA_INFO_CMD_DATA_t) );
				break;
			}
			// process reader command to write user security information into card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(WRITE_SSA_INFO_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(WRITE_SSA_INFO_CMD_DATA_t);
		}
		break;

		case MSG_READ_FP_CMD:
		{
			if(length < sizeof(READ_FP_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(READ_FP_CMD_DATA_t) );
				break;
			}
			// process reader command to read user finger print template from card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(READ_FP_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(READ_FP_CMD_DATA_t);
		}
		break;

		case MSG_READ_SEC_INFO_CMD:
		{
			if(length < sizeof(READ_SEC_INFO_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(READ_SEC_INFO_CMD_DATA_t) );
				break;
			}
			// process reader command to read user security information from card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(READ_SEC_INFO_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(READ_SEC_INFO_CMD_DATA_t);
		}
		break;

		case MSG_READ_PERS_INFO_CMD:
		{
			if(length < sizeof(READ_PERS_INFO_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(READ_PERS_INFO_CMD_DATA_t) );
				break;
			}
			// process reader command to read user personalization information from card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(READ_PERS_INFO_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(READ_PERS_INFO_CMD_DATA_t);
		}
		break;

		case MSG_READ_SMART_ROUTE_ACCESS_INFO_CMD:
		{
			if(length < sizeof(READ_SMART_ROUTE_ACCESS_INFO_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(READ_SMART_ROUTE_ACCESS_INFO_CMD_DATA_t) );
				break;
			}
			// process reader command to read smart route information from card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(READ_SMART_ROUTE_ACCESS_INFO_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(READ_SMART_ROUTE_ACCESS_INFO_CMD_DATA_t);
		}
		break;

		case MSG_ERASE_SMART_CARD_CMD:
		{
			if(length < sizeof(ERASE_SMART_CARD_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(ERASE_SMART_CARD_CMD_DATA_t) );
				break;
			}
			// process reader command to read user personalization information from card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(ERASE_SMART_CARD_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(ERASE_SMART_CARD_CMD_DATA_t);
		}
		break;

		case MSG_READ_CARD_ID_CMD:
		{
			if(length < sizeof(READ_CARD_ID_RESP_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(READ_CARD_ID_RESP_DATA_t) );
				break;
			}
			// process reader command to read user security information from card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(READ_CARD_ID_RESP_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(READ_CARD_ID_RESP_DATA_t);
		}
		break;

		case MSG_CHANGE_SMART_CARD_KEY_CMD:
		{
			if(length < sizeof(CHANGE_SMART_CARD_KEY_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(CHANGE_SMART_CARD_KEY_CMD_DATA_t) );
				break;
			}
			// process reader command to read user personalization information from card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(CHANGE_SMART_CARD_KEY_CMD_DATA_t) );

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(CHANGE_SMART_CARD_KEY_CMD_DATA_t);
		}
		break;

		case MSG_ENABLE_FREE_SCAN_CMD:
		{
			if(length < sizeof(ENABLE_FREE_SCAN_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(ENABLE_FREE_SCAN_CMD_DATA_t) );
				break;
			}
			// process reader command to read user personalization information from card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(ENABLE_FREE_SCAN_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(ENABLE_FREE_SCAN_CMD_DATA_t);
		}
		break;

		case MSG_DISABLE_FREE_SCAN_CMD:
		{
			if(length < sizeof(DISABLE_FREE_SCAN_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(DISABLE_FREE_SCAN_CMD_DATA_t) );
				break;
			}
			// process reader command to read user personalization information from card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(DISABLE_FREE_SCAN_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(DISABLE_FREE_SCAN_CMD_DATA_t);
		}
		break;

		case MSG_SET_FPR_PARAM_CMD:
		{
			// copy data pointer
			SET_FPR_PARAM_CMD_DATA_t * 	tempPtr = (VOIDPTR)(*dataPtr);
			BOOL 						configChangedF = FALSE;

			if(length < sizeof(SET_FPR_PARAM_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(SET_FPR_PARAM_CMD_DATA_t) );
				break;
			}

			if( (tempPtr->secLevel < MAX_SECURITY_LEVEL) &&
				(FprConfig.secLevel != tempPtr->secLevel) )
			{
				FprConfig.secLevel = tempPtr->secLevel;
				configChangedF = TRUE;
			}

			if( (tempPtr->imageQuality < MAX_IMAGE_QUALITY) &&
				(FprConfig.imageQuality != tempPtr->imageQuality) )
			{
				FprConfig.imageQuality = tempPtr->imageQuality;
				configChangedF = TRUE;
			}

			if( (tempPtr->sensitivityLevel < MAX_SENSITIVITY_LEVEL) &&
				(FprConfig.sensitivityLevel != tempPtr->sensitivityLevel) )
			{
				FprConfig.sensitivityLevel = tempPtr->sensitivityLevel;
				configChangedF = TRUE;
			}

			if( (tempPtr->lightCondition < MAX_LIGHT_CONDITION) &&
				(FprConfig.lightCondition != tempPtr->lightCondition) )
			{
				FprConfig.lightCondition = tempPtr->lightCondition;
				configChangedF = TRUE;
			}

			if( (tempPtr->fastMode < MAX_FAST_MODES) &&
				(FprConfig.fastMode != tempPtr->fastMode) )
			{
				FprConfig.fastMode = tempPtr->fastMode;
				configChangedF = TRUE;
			}

			// If any of the parameter changed then only save it in FPR configuration
			// As door will always receive this parameters whenever it comes onlne
			if(configChangedF == TRUE)
			{
				WriteFprConfig();
				SetFprPara();
			}

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(SET_FPR_PARAM_CMD_DATA_t);
		}
		break;

		case MSG_SM125_SET_RADF_CMD:
		{
			// copy data pointer
			SET_SM125_PARA_CMD_DATA_t * 	tempPtr = (VOIDPTR)(*dataPtr);
			UINT8							readerId;

			if(length < sizeof(SET_SM125_PARA_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(SET_SM125_PARA_CMD_DATA_t) );
				break;
			}
			readerId = (UINT8)tempPtr->readerId;
			if( (tempPtr->value <= MAX_ACMS_RADF_VALUE) &&
							(ReadersParaConfig.sm125Radf[readerId] != tempPtr->value) )
			{
				ReadersParaConfig.sm125Radf[readerId] = tempPtr->value;
				WriteReadersParaConfig();
				SetRadfValue(readerId);
			}

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(SET_SM125_PARA_CMD_DATA_t);
		}
		break;

		case MSG_SET_AUDIO_VISUAL_CMD:
		{
			// copy data pointer
			SET_AUDIO_VISUAL_CMD_DATA_t * tempPtr = (VOIDPTR)(*dataPtr);

			if(length < sizeof(SET_AUDIO_VISUAL_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(SET_AUDIO_VISUAL_CMD_DATA_t) );
				break;
			}

			SetBuzzer(tempPtr->acsResponse, tempPtr->respPort);
            SetLed(tempPtr->acsResponse, tempPtr->respPort);

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(SET_AUDIO_VISUAL_CMD_DATA_t);
		}
		break;

		case MSG_SET_CUSTOM_CARD_FORMAT_CMD:
		{
			CARD_FORMAT_CONFIG_MSG_t	customCardFmtMsG, *customCardmsgPtr;
			UINT8						tmpIdx;

			for(tmpIdx = INTERNAL_PORT; tmpIdx < INT_EXT_PORT; tmpIdx++)
			{
				customCardFmtMsG.FCIncardIdF[tmpIdx] = CardFormatConf[0].FCIncardIdF[tmpIdx];
				customCardFmtMsG.cardIdOrder[tmpIdx] = CardFormatConf[0].cardIdOrder[tmpIdx];
				customCardFmtMsG.maxCardBit[tmpIdx] = CardFormatConf[0].maxCardBit[tmpIdx];
				customCardFmtMsG.cardStructBit[tmpIdx] = CardFormatConf[0].cardStructBit[tmpIdx];
				memcpy(customCardFmtMsG.cardStruct[tmpIdx], CardFormatConf[0].cardStruct[tmpIdx], sizeof(customCardFmtMsG.cardStruct[tmpIdx]) );
				customCardFmtMsG.operationSeq[tmpIdx] = CardFormatConf[0].operationSeq[tmpIdx];
			}
			if(memcmp(&customCardFmtMsG, *dataPtr, sizeof(CARD_FORMAT_CONFIG_MSG_t) ) != 0)
			{
				customCardmsgPtr = (CARD_FORMAT_CONFIG_MSG_t *)*dataPtr;
				for(tmpIdx = INTERNAL_PORT; tmpIdx < INT_EXT_PORT; tmpIdx++)
				{
					CardFormatConf[0].FCIncardIdF[tmpIdx] = customCardmsgPtr->FCIncardIdF[tmpIdx];
					CardFormatConf[0].cardIdOrder[tmpIdx] = customCardmsgPtr->cardIdOrder[tmpIdx];
					CardFormatConf[0].maxCardBit[tmpIdx] = customCardmsgPtr->maxCardBit[tmpIdx];
					CardFormatConf[0].cardStructBit[tmpIdx] = customCardmsgPtr->cardStructBit[tmpIdx];
					memcpy(CardFormatConf[0].cardStruct[tmpIdx], customCardmsgPtr->cardStruct[tmpIdx], sizeof(customCardmsgPtr->cardStruct[tmpIdx]) );
					CardFormatConf[0].operationSeq[tmpIdx] = customCardmsgPtr->operationSeq[tmpIdx];
				}
				WriteCardFormatConfig();
			}
			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(CARD_FORMAT_CONFIG_MSG_t);
			break;
		}

		case MSG_SET_MULTIPLE_CARD_FORMAT_CMD:
		{
			MULTIPLE_CARD_FORMAT_MSG_t	multCardFmtMsg, *multCardFmtMsgPtr;
			UINT8						tmpIdx;
			UINT8						cardFmtIdx;

			multCardFmtMsgPtr = (MULTIPLE_CARD_FORMAT_MSG_t *)*dataPtr;
			cardFmtIdx = multCardFmtMsgPtr->cardFmtIdx;

			for(tmpIdx = INTERNAL_PORT; tmpIdx < INT_EXT_PORT; tmpIdx++)
			{
				multCardFmtMsg.cardFmtIdx = cardFmtIdx;
				multCardFmtMsg.FCIncardIdF[tmpIdx] = CardFormatConf[cardFmtIdx].FCIncardIdF[tmpIdx];
				multCardFmtMsg.cardIdOrder[tmpIdx] = CardFormatConf[cardFmtIdx].cardIdOrder[tmpIdx];
				multCardFmtMsg.maxCardBit[tmpIdx] = CardFormatConf[cardFmtIdx].maxCardBit[tmpIdx];
				multCardFmtMsg.cardStructBit[tmpIdx] = CardFormatConf[cardFmtIdx].cardStructBit[tmpIdx];
				memcpy(multCardFmtMsg.cardStruct[tmpIdx], CardFormatConf[cardFmtIdx].cardStruct[tmpIdx], sizeof(multCardFmtMsg.cardStruct[tmpIdx]) );
				multCardFmtMsg.operationSeq[tmpIdx] = CardFormatConf[cardFmtIdx].operationSeq[tmpIdx];
			}
			if(memcmp(&multCardFmtMsg, *dataPtr, sizeof(MULTIPLE_CARD_FORMAT_MSG_t) ) != 0)
			{
				for(tmpIdx = INTERNAL_PORT; tmpIdx < INT_EXT_PORT; tmpIdx++)
				{
					CardFormatConf[cardFmtIdx].FCIncardIdF[tmpIdx] = multCardFmtMsgPtr->FCIncardIdF[tmpIdx];
					CardFormatConf[cardFmtIdx].cardIdOrder[tmpIdx] = multCardFmtMsgPtr->cardIdOrder[tmpIdx];
					CardFormatConf[cardFmtIdx].maxCardBit[tmpIdx] = multCardFmtMsgPtr->maxCardBit[tmpIdx];
					CardFormatConf[cardFmtIdx].cardStructBit[tmpIdx] = multCardFmtMsgPtr->cardStructBit[tmpIdx];
					memcpy(CardFormatConf[cardFmtIdx].cardStruct[tmpIdx], multCardFmtMsgPtr->cardStruct[tmpIdx], sizeof(multCardFmtMsgPtr->cardStruct[tmpIdx]) );
					CardFormatConf[cardFmtIdx].operationSeq[tmpIdx] = multCardFmtMsgPtr->operationSeq[tmpIdx];
				}
				WriteCardFormatConfig();
			}
			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(MULTIPLE_CARD_FORMAT_MSG_t);
			break;
		}

		case MSG_SET_DC_CONFIGURATION_CMD:
		{
			UINT8						readerId;
			DOOR_CONTROLLER_CONFIG_ARGO_VEGA_t *  doorConfPtr;
			SYS_BASIC_PARA_t			tempSysBasicPara;
			SYS_ADVANCE_PARA_t			tempSysAdvPara;
			READER_TYPE_e				tempRdrType = NO_READER_PRESENT;
			BOOL						isHttpUserIdPswdChng = FALSE, httpParaChangeF = FALSE;
			SERVER_PARA_t				tempServerParameter = ServerParameters;

			if(length < sizeof(DOOR_CONTROLLER_CONFIG_ARGO_VEGA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(DOOR_CONTROLLER_CONFIG_ARGO_VEGA_t) );
				break;
			}
			doorConfPtr = (DOOR_CONTROLLER_CONFIG_ARGO_VEGA_t *)*dataPtr;
			tempSysBasicPara = SysBasicPara;
			tempSysAdvPara = SysAdvPara;

			// Disable free scan on FPR if it has been deselected
			for(readerId = 0; readerId < MAX_READERS; readerId++)
			{
				tempRdrType = ConvertPanelRdrTypToDD(doorConfPtr->reader[readerId].type);
				if((NO == IsVariant_With_Internal_FP_Rdr()) && (READER2 == readerId) && (SUP_FP_3030_READER == tempRdrType))
				{
					DPRINT(CONFIG_LOG," Reader 2 type[%d] not supported by ARGO CAE/CAM/CAI",tempRdrType);
					continue;
				}
				else
				{
					tempSysBasicPara.reader[readerId].type = tempRdrType;
					tempSysBasicPara.reader[readerId].mode = doorConfPtr->reader[readerId].mode;
				}
			}

			if(doorConfPtr->cerPresentF == TRUE)
			{
				tempSysBasicPara.reader[READER3].type = COMBO_EXIT_READER;
			}
			else if(doorConfPtr->cbrPresentF == TRUE)
			{
				tempSysBasicPara.reader[READER3].type = CB_U_READER;
			}
			else if(doorConfPtr->atomRD100PresentF == TRUE)
			{
				tempSysBasicPara.reader[READER3].type = ATOM_RD100_READER;
			}
			else if(doorConfPtr->atomRD200PresentF == TRUE)
			{
				tempSysBasicPara.reader[READER3].type = ATOM_RD200_READER;
			}
			else if(doorConfPtr->atomRD300PresentF == TRUE)
			{
				tempSysBasicPara.reader[READER3].type = ATOM_RD300_READER;
			}

			tempSysBasicPara.doorPulseTime = doorConfPtr -> doorPulseTime;
			tempSysBasicPara.doorPulseTimeFloat = doorConfPtr -> doorPulseTimeFloat;
			tempSysBasicPara.doorSense.enable = doorConfPtr -> doorSenseEnable;
			tempSysBasicPara.doorSense.type = doorConfPtr -> doorSenseType;
			tempSysBasicPara.doorSense.supervise = doorConfPtr -> doorSenseMonitor;
			tempSysBasicPara.tamperAlarm = doorConfPtr -> tamperSenseEnable;
			tempSysBasicPara.requestExitSw = doorConfPtr -> requestExitSwEnable;

			tempSysAdvPara.auxInput.enable = doorConfPtr -> auxInEnable;
			tempSysAdvPara.auxInput.sense = doorConfPtr -> auxSenseType;
			tempSysAdvPara.auxInput.supervise = doorConfPtr -> auxInMonitor;
			tempSysAdvPara.auxInput.debounceTime = doorConfPtr -> auxInDebounce;

			tempSysBasicPara.timeFormat = doorConfPtr -> timeFormat;
			tempSysAdvPara.addlSecurity.code = doorConfPtr -> addlSecCode;
			tempSysAdvPara.facilityCode = doorConfPtr -> facilityCode;

			tempSysAdvPara.addlSecurity.enable = doorConfPtr -> addlSecCodeEnable;
			tempSysBasicPara.application = doorConfPtr -> application;
			tempSysAdvPara.genExitSwEvent = doorConfPtr -> genExitSwEvent;
			tempSysAdvPara.genInvalidUserEvent = doorConfPtr -> genInvalidInputEvent;

			tempSysBasicPara.buzMuteF = doorConfPtr -> buzMuteF;
			tempSysBasicPara.tagRedetectDelay = doorConfPtr -> tagRedetectDelay;
			tempSysAdvPara.smartRouteAccessEnbl = doorConfPtr -> smartRouteAccessEnbl;
			tempSysAdvPara.mifareCustomKeyEnbl = doorConfPtr -> mifareCustomKeyEnbl;
			memcpy(tempSysAdvPara.mifareCustomKey, doorConfPtr -> mifareCustomKey, MIFARE_KEY_SIZE);
			tempSysAdvPara.hidIclasCustomKeyEnbl = doorConfPtr -> hidIclasCustomKeyEnbl;

			memcpy(tempSysAdvPara.hidIclasCustomKey, doorConfPtr -> hidIclasCustomKey, HID_ICLASS_KEY_SIZE);
			tempSysAdvPara.customKeyAutoUpdateF = doorConfPtr -> customKeyAutoUpdateF;

			tempSysBasicPara.cdcExitReaderF = doorConfPtr -> cdcExitReaderF_dummy;
			tempSysBasicPara.degradeModeEnable = doorConfPtr -> degradeModeEnable;
			tempSysBasicPara.degradeWaitTimer = doorConfPtr -> degradeWaitTimer;
			tempSysAdvPara.lockType = doorConfPtr -> lockType;
			tempSysAdvPara.templatesPerFinger = doorConfPtr->templatesPerFinger;
			tempSysAdvPara.SSAConfig.accessMode = doorConfPtr->accessMode;
			strcpy(tempSysBasicPara.name, doorConfPtr->name);
			tempSysBasicPara.fprTemplateFormat = doorConfPtr->fprTemplateFormat;
			tempSysAdvPara.multiAccessTimer = doorConfPtr->multiAccessTimer;

			tempSysBasicPara.accessAllowCadDuration	= doorConfPtr->accessAllowCadDuration;
			tempSysBasicPara.accessDenyCadDuration = doorConfPtr->accessDenyCadDuration;

			//API security key needed for panel door , for BLE access
			sprintf(tempSysBasicPara.apiSecurityKey, "%s", doorConfPtr->apiSecurityKey);

			tempSysAdvPara.autoHideMenuBarF = doorConfPtr->autoHideMenuBarF;
			
			tempSysAdvPara.saveFpImageF = doorConfPtr->saveFpImageF;

			tempSysBasicPara.abnormalAlarm = doorConfPtr->abnormalAlarmEnable;
			tempSysBasicPara.forceOpenAlarm = doorConfPtr->forceOpenAlarmEnable;
			tempSysBasicPara.faultAlarm = doorConfPtr->faultAlarmEnable;
			tempSysBasicPara.panicAlarm = doorConfPtr->panicAlarmEnable;

			tempSysAdvPara.interDigitTimer = doorConfPtr->interDigitTimer;
			tempSysAdvPara.facilityCodeCheckEnableF = doorConfPtr->facilityCodeCheckEnableF;
	if(strcmp(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId, doorConfPtr->httpWebServerConfig.httpSyncConfig.httpServerUserId) != 0)
            {
                isHttpUserIdPswdChng = TRUE;
            }

            snprintf(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId, sizeof(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerUserId), "%s",doorConfPtr->httpWebServerConfig.httpSyncConfig.httpServerUserId);
            if(strcmp(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword, doorConfPtr->httpWebServerConfig.httpSyncConfig.httpServerPassword) != 0)
            {
                isHttpUserIdPswdChng = TRUE;
            }
    snprintf(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword, sizeof(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPassword), "%s",doorConfPtr->httpWebServerConfig.httpSyncConfig.httpServerPassword);

            if((tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF != doorConfPtr->httpWebServerConfig.httpSyncConfig.httpEncryptionModeF) ||
                    (tempSysBasicPara.httpWebServerConfig.httpSyncConfig.interfaceSelctionF != doorConfPtr->httpWebServerConfig.httpSyncConfig.interfaceSelctionF) ||
                    (tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface != doorConfPtr->httpWebServerConfig.httpSyncConfig.httpInterface) ||
                    (tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort != doorConfPtr->httpWebServerConfig.httpSyncConfig.httpServerPort) ||
                    (strcmp(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName, doorConfPtr->httpWebServerConfig.httpSyncConfig.httpServerHostName) != 0) ||
                    (strcmp(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerDirectory, doorConfPtr->httpWebServerConfig.httpSyncConfig.httpServerDirectory) != 0) ||
                    (strcmp(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress, doorConfPtr->httpWebServerConfig.httpSyncConfig.httpServerIpAddress) != 0))
            {
                httpParaChangeF = TRUE;
            }

            tempSysBasicPara.httpWebServerConfig.httpSyncConfig.interfaceSelctionF = doorConfPtr->httpWebServerConfig.httpSyncConfig.interfaceSelctionF;
            snprintf(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerDirectory, sizeof(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerDirectory), "%s",doorConfPtr->httpWebServerConfig.httpSyncConfig.httpServerDirectory);
            tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpInterface = doorConfPtr->httpWebServerConfig.httpSyncConfig.httpInterface;
            tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpEncryptionModeF =  doorConfPtr->httpWebServerConfig.httpSyncConfig.httpEncryptionModeF;
            tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerPort =  doorConfPtr->httpWebServerConfig.httpSyncConfig.httpServerPort;
            tempSysBasicPara.httpWebServerConfig.httpSyncRequestRetryTimer =  doorConfPtr->httpWebServerConfig.httpSyncRequestRetryTimer;
            snprintf(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress, sizeof(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerIpAddress), "%s",doorConfPtr->httpWebServerConfig.httpSyncConfig.httpServerIpAddress);
            snprintf(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName, sizeof(tempSysBasicPara.httpWebServerConfig.httpSyncConfig.httpServerHostName), "%s",doorConfPtr->httpWebServerConfig.httpSyncConfig.httpServerHostName);

		tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.interfaceSelctionF = doorConfPtr->httpWebServerConfig.profilePicHttpSyncConfig.interfaceSelctionF;
            tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpInterface = doorConfPtr->httpWebServerConfig.profilePicHttpSyncConfig.httpInterface;
            tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpEncryptionModeF = doorConfPtr->httpWebServerConfig.profilePicHttpSyncConfig.httpEncryptionModeF;
            snprintf(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerHostName, sizeof(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerHostName), "%s", doorConfPtr->httpWebServerConfig.profilePicHttpSyncConfig.httpServerHostName);
            snprintf(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerIpAddress, sizeof(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerIpAddress), "%s", doorConfPtr->httpWebServerConfig.profilePicHttpSyncConfig.httpServerIpAddress);
            tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPort = doorConfPtr->httpWebServerConfig.profilePicHttpSyncConfig.httpServerPort;
            snprintf(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerUserId, sizeof(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerUserId), "%s", doorConfPtr->httpWebServerConfig.profilePicHttpSyncConfig.httpServerUserId);
            snprintf(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPassword, sizeof(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerPassword), "%s", doorConfPtr->httpWebServerConfig.profilePicHttpSyncConfig.httpServerPassword);
            snprintf(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerDirectory, sizeof(tempSysBasicPara.httpWebServerConfig.profilePicHttpSyncConfig.httpServerDirectory), "%s", doorConfPtr->httpWebServerConfig.profilePicHttpSyncConfig.httpServerDirectory);  
			
			tempSysBasicPara.degradeModeType = doorConfPtr -> degradeModeType;
			tempSysBasicPara.apiAccessAllowEnable = doorConfPtr -> mobileAccessAllowEnable;
			tempSysBasicPara.apiEntryAccessMode = doorConfPtr -> mobileEntryAccessMode;
			tempSysBasicPara.apiExitAccessMode = doorConfPtr -> mobileExitAccessMode;
			tempSysBasicPara.doorAccessMode = doorConfPtr -> doorAccessMode;
			tempSysBasicPara.extRdrAccessMode = doorConfPtr -> extRdrAccessMode;
			tempSysBasicPara.visitorDoorAccessMode = doorConfPtr -> visitorDoorAccessMode;
			tempSysBasicPara.visitorExtRdrAccessMode = doorConfPtr -> visitorExtRdrAccessMode;
			
			tempSysAdvPara.verifyAccessPolicyOnExitF = doorConfPtr -> verifyAccessPolicyOnExitF;
			tempSysBasicPara.accessAllowScreenDuration = doorConfPtr->accessAllowScreenDuration;
			tempSysBasicPara.accessDenyScreenDuration = doorConfPtr->accessDenyScreenDuration;
			
			tempSysAdvPara.temporaryUserGenerateInfo.autoAddTempUser = doorConfPtr->temporaryUserGenerateInfo.autoAddTempUser;
			tempSysAdvPara.temporaryUserGenerateInfo.confirmReqF = doorConfPtr->temporaryUserGenerateInfo.confirmReqF;
			tempSysAdvPara.temporaryUserGenerateInfo.addNameViaDeviceF = doorConfPtr->temporaryUserGenerateInfo.addNameViaDeviceF;

			if(ConfigUpdateHandler(GEN_SYS_BASIC_TABLE_ID, 0, (VOIDPTR)(&tempSysBasicPara) ) == FAIL)
			{
				EPRINT(CONFIG_LOG, "Basic config update fail");
			}
			if(ConfigUpdateHandler(GEN_SYS_ADV_TABLE_ID, 0, (VOIDPTR)(&tempSysAdvPara) ) == FAIL)
			{
				EPRINT(CONFIG_LOG, "Advance config update fail");
			}
			// Here when door configuration changes we are sending IO current
			// status to master controller like door sense and Aux input sense
			// because when door monitoring or Aux input monitoring is enabled
			// disabled in configuration master need to take nacessary action.
			// get status of door controller’s peripheral
			SendStatusResponse();
		}
		break;

		case CMD_DOWNLOAD_START:
		{
			DPRINT(BU_LOG, "CMD_DOWNLOAD_START");
			StartPDFirmwareUpgrade((FIRMWARE_PKT_INFO_t *)*dataPtr);
		}
		break;

		case CMD_WRITE_ROW_DATA:
		{
			FIRMWARE_PKT_WRITE_DATA_t* 		pktData;
			UPGRADE_RESPONSE_DATA_t 		response;

			pktData = (FIRMWARE_PKT_WRITE_DATA_t *)*dataPtr;

			if(in_cksum((UINT16PTR) (pktData->data), NULL, sizeof(pktData->data),0) != (UINT16) pktData->checksum)
			{
				response.status = FAIL;
				EPRINT(BU_LOG, "Checksum mismatch for packet[%d]",pktData->pktNo);
			}
			else if((WriteZipFile(pktData->pktNo,pktData->data))!= SUCCESS)
			{
				response.status = FAIL;
				EPRINT(BU_LOG, "Cannot write packet[%d]  into file",pktData->pktNo);
			}
			else
			{
				response.status = SUCCESS;
			}
			SendMsg(MSG_WRITE_ROW_DATA,&response,sizeof(UPGRADE_RESPONSE_DATA_t));
		}
		break;

		case CMD_DOWNLOAD_OVER:
		{
			CloseDownloadZip();

			/* Register signal that will be sent to thread when timeout occurs */
			signal(SIGKILL, SigHandlerForFirmwareUpgradeFromPanelThread);

			if( (fwUpgradeFromPanelThreadStatus == THREAD_INACTIVE) || (fwUpgradeFromPanelThreadStatus == THREAD_DONE) )
			{
				fwUpgradeFromPanelThreadStatus = THREAD_INACTIVE;
				if(pthread_create(&fwUpgradeFromPanelThreadData, NULL, FirmwareUpgradeFromPanelThread, NULL) == 0)
				{
					// In case thread is completed just before call reaches here then no need to change thread status to active
					if(fwUpgradeFromPanelThreadStatus == THREAD_INACTIVE)
					{
						fwUpgradeFromPanelThreadStatus = THREAD_ACTIVE;
					}
					// Thread created successfully, now detach thread
					pthread_detach(fwUpgradeFromPanelThreadData);
				}
				else
				{
					EPRINT(ACMS_LOG, "Failed to create thread for firmware upgrade from panel");
				}
			}
		}
		break;

		case MSG_READ_SSA_INFO_CMD:
		{
			if(length < sizeof(READ_SSA_INFO_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(READ_SSA_INFO_CMD_DATA_t) );
				break;
			}

			// process reader command to read user personalization information from card
			ProcessReaderCommand(msgId, *dataPtr, sizeof(READ_SSA_INFO_CMD_DATA_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(READ_SSA_INFO_CMD_DATA_t);
		}
		break;

		case MSG_CALIBRATE_SENSOR_CMD:
		{
			CalibrateFpSensor(NULL);
		}
		break;

		case MSG_GET_BUILD_NO_CMD:
		{
			DOOR_BUILD_INFO_t	doorBuildNo;

			doorBuildNo.swVersion = DOOR_COMM_SW_VER;
			doorBuildNo.hwVersion = HW_VER;
			doorBuildNo.buildNo = DOOR_BUILD_NO;

			// send out message
			SENDMSG(MSG_GET_BUILD_NO_RESP, &doorBuildNo, sizeof(DOOR_BUILD_INFO_t));
		}
		break;

		case MSG_SET_PSWD_CONFIGURATION_CMD:
		{
			PD_PASSWORD_t 	tempPassword;
			UINT8			index;

            memset(&tempPassword, 0, sizeof(tempPassword));
			if(length < sizeof(PD_PASSWORD_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%lu]", msgId, length, sizeof(PD_PASSWORD_t) );
				break;
			}

            memcpy(&tempPassword, *dataPtr, sizeof(PD_PASSWORD_t));
            for(index = 0; index < MAX_USER_TYPE; index++)
            {
                ChangeDisplayPswd(index,tempPassword.user[index]);
            }

			// Save all Data
			WritePswdConfig();
		}
		break;

		case MSG_SET_MODE_DIRECT_DOOR_CMD:
		{
			SetDoorApplType(DIRECT_DOOR_APPL);
			break;
		}

		case MSG_SET_DC_SMART_CARD_FORMAT_CMD:
		{
			SET_DC_SMART_CARD_FORMAT_CMD_DATA_t tempSmartCardFormat[VER2_MAX_ACMS_CARD_TYPE];
			UINT8 idx;

			memcpy(tempSmartCardFormat, *dataPtr, sizeof(tempSmartCardFormat));

			for(idx = 0; idx < VER2_MAX_ACMS_CARD_TYPE; idx++)
			{
				SmartCardFormat[idx].customMode = tempSmartCardFormat[idx].customMode;
				SmartCardFormat[idx].useCSNasCardIdF = tempSmartCardFormat[idx].useCSNasCardIdF;
				SmartCardFormat[idx].cardNo = tempSmartCardFormat[idx].cardNo;
			}

			WriteSmartCardFormat();
			*dataPtr = *dataPtr + sizeof(tempSmartCardFormat);
			break;
		}

		case MSG_SET_DC_CUSTOM_SMART_CARD_FORMAT_CMD:
		{
			CUSTOM_SMART_CARD_FORMAT_CMD_DATA_t *tempSmartCardFormat = (CUSTOM_SMART_CARD_FORMAT_CMD_DATA_t *) *dataPtr;
			UINT8 cardType, idx;

			cardType = tempSmartCardFormat->cardType;

			for(idx = 0; idx < MAX_SMART_CARD_CUSTOM_FIELDS; idx++)
			{
				SmartCardCustomConfig[cardType][idx].fieldType = tempSmartCardFormat->customSmartCardField[idx].fieldType;
				SmartCardCustomConfig[cardType][idx].dateFormat = tempSmartCardFormat->customSmartCardField[idx].dateFormat;
				SmartCardCustomConfig[cardType][idx].saparator = tempSmartCardFormat->customSmartCardField[idx].saparator;
				SmartCardCustomConfig[cardType][idx].dateFieldType = tempSmartCardFormat->customSmartCardField[idx].dateFieldType;
				SmartCardCustomConfig[cardType][idx].startBlock = tempSmartCardFormat->customSmartCardField[idx].startBlock;
				SmartCardCustomConfig[cardType][idx].startByte = tempSmartCardFormat->customSmartCardField[idx].startByte;
				SmartCardCustomConfig[cardType][idx].length = tempSmartCardFormat->customSmartCardField[idx].length;
				SmartCardCustomConfig[cardType][idx].startSectorOrPage = tempSmartCardFormat->customSmartCardField[idx].startSectorOrPage;
			}
			WriteSingleCustomSmartCardConfig(cardType);

			*dataPtr = *dataPtr + sizeof(CUSTOM_SMART_CARD_FORMAT_CMD_DATA_t);
			break;
		}


		case GUI_MSG_MENU_SCREEN:
		{
			SendMessageToQt(" ", msgId);
			break;
		}

		case GUI_MSG_GOING_IN_SCREEN:
		{
			if(length < sizeof(QT_TA_SCREEN_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(QT_TA_SCREEN_t) );
				break;
			}
			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(QT_TA_SCREEN_t);
			break;
		}

		case GUI_MSG_GOING_OUT_SCREEN:
		{
			if(length < sizeof(QT_TA_SCREEN_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(QT_TA_SCREEN_t) );
				break;
			}
			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(QT_TA_SCREEN_t);
			break;
		}

		case GUI_MSG_TEMP_USER_CREATE_CONFIRM_SCREEN:
		{
			
			SendMessageToQt(" ", msgId);
			break;
		}
		case GUI_MSG_TEMP_USER_ENTER_NAME_SCREEN:
		{
			
			SendMessageToQt(" ", msgId);
			break;
		}
		case GUI_MSG_TEMP_USER_POPUP_SCREEN:
		{
			if(length < sizeof(TEMP_USER_POPUP_INFO_QT_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(TEMP_USER_POPUP_INFO_QT_t) );
				break;
			}
			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(TEMP_USER_POPUP_INFO_QT_t);
			break;
		}
		case GUI_MSG_TEMP_USER_REGISTERED_SUCCESSFULLY:
		{
			if(length < sizeof(TEMP_USER_COMMON_QT_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(TEMP_USER_COMMON_QT_t) );
				break;
			}
			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(TEMP_USER_COMMON_QT_t);
			break;
		}
		case GUI_MSG_DISPLAY_IDLE_SCREEN:
		{
			if(length < sizeof(RESPONSE_PORT_e) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(RESPONSE_PORT_e) );
				break;
			}

			if(**dataPtr == EXTERNAL_PORT)
			{
				SendGuiMessageToReader(MAX_USER_RESP, *dataPtr, msgId);
			}
			else
			{
				SendMessageToQt(" ", msgId);
			}

			*dataPtr += sizeof(RESPONSE_PORT_e);
			break;
		}

		case GUI_MSG_DOOR_MODE_SELECTION_SCREEN:
		{
			SendMessageToQt(" ", msgId);
			break;
		}

		case GUI_MSG_PIN_ENTER_SCREEN:
		{
			if(length < sizeof(RESPONSE_PORT_e) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(RESPONSE_PORT_e) );
				break;
			}

			if(**dataPtr == EXTERNAL_PORT)
			{
				SendGuiMessageToReader(MAX_USER_RESP, *dataPtr, msgId);
			}
			else
			{
				SendMessageToQt(" ", msgId);
			}

			*dataPtr += sizeof(RESPONSE_PORT_e);
			break;
		}

		case GUI_MSG_MENU_LOGIN_SCREEN:
		{
            UINT16 sourceMsg = MAX_SLAVE_MSG; // = (VOIDPTR) *dataPtr;
            memcpy(&sourceMsg, *dataPtr, sizeof(sourceMsg) );
            CONFIG_USER_TYPE_e userType = MAX_USER_TYPE;
            if(sourceMsg == GUI_MSG_HRD_MENU_SCREEN)
            {
                userType = USER_HRD;
            }
            else if(sourceMsg == GUI_MSG_ADMIN_MENU_SCREEN)
            {
                userType = USER_ADMIN;
            }

            if((sourceMsg == MAX_SLAVE_MSG) || Password.DispPwdLoginRestrictTimer[userType] == INVALID_TIMER_HANDLE)
            {
                SendMessageToQt(" ", msgId);
            }
			break;
		}

		case GUI_MSG_ADMIN_MENU_SCREEN:
		{
			if(length < sizeof(QT_ADMIN_HRD_SCREEN_t) )
			{
                EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%lu]", msgId, length, sizeof(QT_ADMIN_HRD_SCREEN_t) );
				break;
			}
            ResetDispPwdAttemptData(USER_ADMIN);
			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(QT_ADMIN_HRD_SCREEN_t);
			break;
		}

		case GUI_MSG_HRD_MENU_SCREEN:
		{
			if(length < sizeof(QT_ADMIN_HRD_SCREEN_t) )
			{
                EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%lu]", msgId, length, sizeof(QT_ADMIN_HRD_SCREEN_t) );
				break;
			}
            ResetDispPwdAttemptData(USER_HRD);
			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(QT_ADMIN_HRD_SCREEN_t);
			break;
		}

		case GUI_MSG_ADMIN_SETTING_SCREEN:
		{
			CHAR	wifistatus[20];

			sprintf(wifistatus,"%d%c",GetWiFiStatus(), UI_MSG_FSP);
			SendMessageToQt(wifistatus, msgId);
			break;
		}

		case GUI_MSG_DISPLAY_MENU_SCREEN:
		{
			SendMessageToQt(" ", msgId);
			break;
		}

		case GUI_MSG_BACKGROUND_SELECTION_SCREEN:
		{
			SendMessageToQt(" ", msgId);
			break;
		}

		case GUI_MSG_BRIGHTNESS_SELECTION_SCREEN:
		{
			SendMessageToQt(" ", msgId);
			break;
		}

		case GUI_MSG_CLOCK_SELECTION_SCREEN:
		{
			SendMessageToQt(" ", msgId);
			break;
		}

		case GUI_MSG_KEYPAD_SELECTION_SCREEN:
		{
			SendMessageToQt(" ", msgId);
			break;
		}

		case  GUI_MSG_ASSIGN_USER_ON_DEVICE:
		case GUI_MSG_SELF_ENROLLMENT_SCREEN:
		{
			SendMessageToQt(" ", msgId);
			break;
		}

		case GUI_MSG_STATUSBAR_SCREEN:
		{
			if(length < sizeof(QT_STATUSBAR_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(QT_STATUSBAR_t) );
				break;
			}

			CurrStatusbarData.dndF = ((QT_STATUSBAR_t *) *dataPtr)->dndF;
			CurrStatusbarData.deadManZoneF = ((QT_STATUSBAR_t *) *dataPtr)->deadManZoneF;
			CurrStatusbarData.alarmStatus = ((QT_STATUSBAR_t *) *dataPtr)->alarmConditionF;
			CurrStatusbarData.doorLockStatus = ((QT_STATUSBAR_t *) *dataPtr)->doorLockUnlock;
			CurrStatusbarData.lateInEarlyOutStatus = ((QT_STATUSBAR_t *) *dataPtr)->lateInEarlyOut;
			CurrStatusbarData.downloadingF = ((QT_STATUSBAR_t *) *dataPtr)->downloadingF;
			UpdateDisplayStatusField();
			
			//Thsi is use for External Redaer i.e TOM RD200/300
			SendDoorLockStatusToExternalReader(CurrStatusbarData.doorLockStatus);
			
			*dataPtr += sizeof(QT_STATUSBAR_t);
			break;
		}

		case GUI_MSG_SYS_STATUS_SCREEN:
		{
            ResetDispPwdAttemptData(USER_ADMIN);
			SendMessageToQt(" ", msgId);
			break;
		}

		case GUI_MSG_ABOUT_DEVICE_ALARMS_SCREEN:
		{
			if(length < sizeof(QT_ABOUT_DEVICE_ALARMS_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(QT_ABOUT_DEVICE_ALARMS_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(QT_ABOUT_DEVICE_ALARMS_t);
			break;
		}

		case GUI_MSG_ABOUT_DEVICE_CONNECTIVITY_SCREEN:
		{
			if(length < sizeof(QT_ABOUT_DEVICE_CONNECTIVITY_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(QT_ABOUT_DEVICE_CONNECTIVITY_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(QT_ABOUT_DEVICE_CONNECTIVITY_t);
			break;
		}

		case GUI_MSG_ABOUT_DEVICE_READERS_INTERFACES_SCREEN:
		{
			SendMessageToQt(" ", msgId);
			break;
		}

		case GUI_MSG_ENROLLMENT_SCREEN:
		{
			if(length < sizeof(QT_ENROLL_MENU_SCREEN_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(QT_ENROLL_MENU_SCREEN_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(QT_ENROLL_MENU_SCREEN_t);
			break;
		}

		case GUI_MSG_ZONE_CFG_SCREEN:
		{
			if(length < sizeof(QT_ZONE_SETTINGS_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(QT_ZONE_SETTINGS_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(QT_ZONE_SETTINGS_t);
			break;
		}

		case GUI_MSG_EHTERNET_CFG_SCREEN:
		{
			SendMessageToQt(" ", msgId);
			break;
		}

		case GUI_MSG_WIFI_CFG_SCREEN:
		{
			SendMessageToQt(" ", msgId);
			break;
		}

		case GUI_MSG_SEARCHING_WIFI_SCREEN:
		{
			if(length < sizeof(DYNAMIC_POPUP_INFO_QT_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(DYNAMIC_POPUP_INFO_QT_t) );
				break;
			}
			//NOTE:Received message and message sent to Qt is different
			SendMessageToQt(*dataPtr, GUI_MSG_DYNAMIC_POP_UP_SCREEN);
			*dataPtr += sizeof(DYNAMIC_POPUP_INFO_QT_t);
			SearchWirelessNw();
			break;
		}

		case GUI_MSG_WPS_PROGRESS_SCREEN:
		{
			WPS_PROGRESS_t			wpsProgData;

			pthread_mutex_lock(&MutexLockAutoSwitchWiFi);
			WifiParameters.wpsTrigerTyp = WPS_TRIGER_PUSH_BUTTON;

			if(WriteWifiParaConfig() == SUCCESS)
			{
				WifiWpsTrigF = TRUE;
				ChangeNetworkInterfacePara(WIFI_PARA);
				DPRINT(WEBJVS_LOG,"Wifi Parameter Config set successfully");
			}

			pthread_mutex_unlock(&MutexLockAutoSwitchWiFi);
			wpsProgData.wpsTrigerTyp = WifiParameters.wpsTrigerTyp;
			SendMessageToQt(&wpsProgData, msgId);

			break;
		}

		case GUI_MSG_WPS_PROCESSING_SCREEN:
		{
			DYNAMIC_POPUP_INFO_QT_t		qtDynamicInfo;

			pthread_mutex_lock(&MutexLockAutoSwitchWiFi);

			strcpy(WifiParameters.wpsPin, "");
			WifiParameters.wpsTrigerTyp = WPS_TRIGER_PIN_FROM_DEVICE;

			if(WriteWifiParaConfig() == SUCCESS)
			{
				WifiWpsTrigF = TRUE;
				ChangeNetworkInterfacePara(WIFI_PARA);
				DPRINT(WEBJVS_LOG,"Wifi Parameter Config set successfully");
			}

			pthread_mutex_unlock(&MutexLockAutoSwitchWiFi);
			strcpy(qtDynamicInfo.message1, GetDisplayMessage(FALSE,NULL_CHAR,MSG_PLEASE_WAIT_444));
			qtDynamicInfo.imageIdx = POPUP_PROCESSING;
			strcpy(qtDynamicInfo.titleText, "");
			strcpy(qtDynamicInfo.message2, "");

			SendMessageToQt(&qtDynamicInfo, GUI_MSG_DYNAMIC_POP_UP_SCREEN);

			StartWaitForWpsPinThread();
			break;
		}

		case GUI_MSG_ALARM_OPTIONS_SCREEN:
		{
			if(length < sizeof(QT_ALARM_SETTINGS_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(QT_ALARM_SETTINGS_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(QT_ALARM_SETTINGS_t);
			break;
		}

		case GUI_MSG_ZONE_CONTROL_SCREEN:
		{
			if(length < sizeof(QT_ZONE_MENU_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(QT_ZONE_MENU_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(QT_ZONE_MENU_t);
			break;
		}

		case GUI_MSG_ADMIN_DOOR_CONTROL_SCREEN:
		{
			if(length < sizeof(QT_ZONE_MENU_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(QT_ZONE_MENU_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(QT_ZONE_MENU_t);
			break;
		}

		case GUI_MSG_DEL_CRED_SCREEN:
		{
			if(length < sizeof(DELETE_USER_CREDEN_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(DELETE_USER_CREDEN_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(DELETE_USER_CREDEN_t);
			break;
		}

		case GUI_MSG_ENROLL_USER_SCREEN:
		{
			if(length < sizeof(ENROLL_USER_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(ENROLL_USER_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(ENROLL_USER_t);
			break;
		}

		case GUI_MSG_ENROLL_SPL_FN_SCREEN:
		{
			if(length < sizeof(ENROLL_SPL_FUNC_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(ENROLL_SPL_FUNC_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(ENROLL_SPL_FUNC_t);
			break;
		}

		case GUI_MSG_NOTICE_TEXT:
		{
			if(length < sizeof(MSG_GEN_INFO_QT_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(MSG_GEN_INFO_QT_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(MSG_GEN_INFO_QT_t);
			break;
		}

		case GUI_MSG_SPL_FUNC_STATUS:
		{
			if(length < sizeof(FUNC_STATUS_QT_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(FUNC_STATUS_QT_t) );
				break;
			}

			DPRINT(COMM_LOG, "Msg Id[0x%x] Rcvd for respPort: %u", msgId, ((FUNC_STATUS_QT_t*)*dataPtr)->respPort);
			if( ((FUNC_STATUS_QT_t*)*dataPtr)->respPort == EXTERNAL_PORT)
			{
				SendGuiMessageToReader(MAX_USER_RESP, *dataPtr, msgId);
			}
			else
			{
				SendMessageToQt(*dataPtr, msgId);
			}

			*dataPtr += sizeof(FUNC_STATUS_QT_t);
			break;
		}

		case GUI_MSG_SPL_FUNC_DYNAMIC_SCREEN:
		{
			if(length < sizeof(DYNAMIC_SPL_FUNC_INFO_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(DYNAMIC_SPL_FUNC_INFO_t) );
				break;
			}

			DPRINT(COMM_LOG, "Msg Id[0x%x] Rcvd for respPort: %u", msgId, ((DYNAMIC_SPL_FUNC_INFO_t*)*dataPtr)->respPort);
			if( ((DYNAMIC_SPL_FUNC_INFO_t*)*dataPtr)->respPort == EXTERNAL_PORT)
			{
				SendGuiMessageToReader(MAX_USER_RESP, *dataPtr, msgId);
			}
			else
			{
				SendMessageToQt(*dataPtr, msgId);
			}

			*dataPtr += sizeof(DYNAMIC_SPL_FUNC_INFO_t);
			break;
		}

		case GUI_MSG_DYNAMIC_POP_UP_SCREEN:
		{
			if(length < sizeof(DYNAMIC_POPUP_INFO_QT_t) )
			{
                EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%lu]", msgId, length, sizeof(DYNAMIC_POPUP_INFO_QT_t) );
				break;
			}

            DPRINT(COMM_LOG, "Msg Id[0x%x] Rcvd for respPort: %u", msgId, ((DYNAMIC_POPUP_INFO_QT_t*)*dataPtr)->respPort);

			if( ((DYNAMIC_POPUP_INFO_QT_t*)*dataPtr)->respPort == EXTERNAL_PORT)
			{
				SendGuiMessageToReader(MAX_USER_RESP, *dataPtr, msgId);
			}
			else
			{
                UINT16 tempSourceMsg = ((DYNAMIC_POPUP_INFO_QT_t*)*dataPtr)->sourceMsg;
                CONFIG_USER_TYPE_e userType = MAX_USER_TYPE;

                if(tempSourceMsg == GUI_MSG_ADMIN_MENU_SCREEN || tempSourceMsg == GUI_MSG_HRD_MENU_SCREEN || tempSourceMsg == GUI_MSG_SYS_STATUS_SCREEN)
                {
                    userType = (tempSourceMsg == GUI_MSG_HRD_MENU_SCREEN) ? USER_HRD : USER_ADMIN;

                    SetDispPwdAttemptCount(userType, Password.DispPwdAttemptCount[userType] - 1);
                    SetAudioVisual(USER_RESP_MENU_LOGIN_RESTRICTED, INTERNAL_PORT, (VOIDPTR)&userType);
                    if(Password.DispPwdLoginRestrictTimer[userType] != INVALID_TIMER_HANDLE)
                    {
						SendMsg(GUI_MSG_USER_RESTRICT_TIMER, &tempSourceMsg, sizeof(tempSourceMsg));
                    }
                }
                else
                {
                    SendMessageToQt(*dataPtr, msgId);
                }
            }

			*dataPtr += sizeof(DYNAMIC_POPUP_INFO_QT_t);
			break;
		}

		case GUI_MSG_USER_DENIED:
		{
			if(length < sizeof(USER_DENIED_QT_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(USER_DENIED_QT_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(USER_DENIED_QT_t);
			break;
		}

		case GUI_MSG_USER_ALLOWED:
		{
			if(length < sizeof(USER_ALLOWED_QT_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(USER_ALLOWED_QT_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(USER_ALLOWED_QT_t);
			break;
		}

		case GUI_MSG_USER_ALLOWED_COMMON_SCREEN:
		{
			if(length < sizeof(USER_COMMON_QT_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(USER_COMMON_QT_t) );
				break;
			}

			DPRINT(COMM_LOG, "Msg Id[0x%x] Rcvd for respPort: %u", msgId, ((USER_COMMON_QT_t*)*dataPtr)->respPort);
			if( ((USER_COMMON_QT_t*)*dataPtr)->respPort == EXTERNAL_PORT)
			{
				SendGuiMessageToReader(MAX_USER_RESP, *dataPtr, msgId);
			}
			else
			{
				SendMessageToQt(*dataPtr, msgId);
			}

			*dataPtr += sizeof(USER_COMMON_QT_t);
			break;
		}

		case GUI_MSG_USER_DENIED_COMMON_SCREEN:
		{
			if(length < sizeof(USER_COMMON_QT_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(USER_COMMON_QT_t) );
				break;
			}

			DPRINT(COMM_LOG, "Msg Id[0x%x] Rcvd for respPort: %u", msgId, ((USER_COMMON_QT_t*)*dataPtr)->respPort);
			if( ((USER_COMMON_QT_t*)*dataPtr)->respPort == EXTERNAL_PORT)
			{
				SendGuiMessageToReader(MAX_USER_RESP, *dataPtr, msgId);
			}
			else
			{
				SendMessageToQt(*dataPtr, msgId);
			}

			*dataPtr += sizeof(USER_COMMON_QT_t);
			break;
		}

		case GUI_MSG_SEARCHED_DEL_CRED_USER_LIST:
		case GUI_MSG_SEARCHED_ENROLL_USER_LIST:
		case GUI_MSG_ENROLL_GET_NAME_FROM_ID:
		case GUI_MSG_DEL_CRED_GET_NAME_FROM_ID:
			SendMessageToQt(*dataPtr, msgId);
			break;

		case GUI_MSG_VIEW_USER_PROFILE_SCREEN:
		{
			if(length < sizeof(USER_VIEW_PROFILE_QT_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(USER_VIEW_PROFILE_QT_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(USER_VIEW_PROFILE_QT_t);
			break;
		}

		case MSG_SET_DC_WGND_OUT_CSTM_FORMAT_CMD:
		{
			SET_WIEGAND_OUT_CUSTOM_FORMAT_CMD_t *tmpWgndOutCstmFrmt = (SET_WIEGAND_OUT_CUSTOM_FORMAT_CMD_t *)(*dataPtr);
			UINT8	idx;

			idx = tmpWgndOutCstmFrmt -> idx;
			memcpy(&WiegnadOutCustomFormat[idx], &(tmpWgndOutCstmFrmt -> wgndOutCstmFrmtCnfg), sizeof(WIEGAND_OUT_CUSTOM_FORMAT_t) );
			WriteWiegnadOutCstmFrmtConfig();
			*dataPtr = *dataPtr + sizeof(SET_WIEGAND_OUT_CUSTOM_FORMAT_CMD_t);
			break;
		}

		case MSG_SET_DC_WIEGAND_OUT_CONFIG_CMD:
		{
			WIEGADN_OUT_CONFIG_CMD_DATA_t *tmpWgndOutConfigPtr = (WIEGADN_OUT_CONFIG_CMD_DATA_t *)(*dataPtr);
			SYS_BASIC_PARA_t			tempSysBasicPara;

			tempSysBasicPara = SysBasicPara;

			tempSysBasicPara.extWiegandRdrMode = tmpWgndOutConfigPtr-> extWiegandRdrMode;
			tempSysBasicPara.accessFeedbackWaitTime = tmpWgndOutConfigPtr-> accessFeedbackWaitTime;
			tempSysBasicPara.waitForAccessFeedback = tmpWgndOutConfigPtr-> waitForAccessFeedback;
			tempSysBasicPara.waitForUsrVerification = tmpWgndOutConfigPtr-> waitForUsrVerification;
			tempSysBasicPara.wiegandOutDir = tmpWgndOutConfigPtr-> wiegandOutDir;
			tempSysBasicPara.wiegandOutFormat = tmpWgndOutConfigPtr-> wiegandOutFormat;
			memcpy(tempSysBasicPara.wgndOutCstmFrmtNo, tmpWgndOutConfigPtr-> wgndOutCstmFrmtNo, sizeof(tempSysBasicPara.wgndOutCstmFrmtNo) );
			memcpy(tempSysBasicPara.wgndOutEventSpclCode, tmpWgndOutConfigPtr-> wgndOutEventSpclCode, sizeof(tempSysBasicPara.wgndOutEventSpclCode) );
			SetComboRdrCfgType(READER3,&tempSysBasicPara,&tempSysBasicPara);

			if(ConfigUpdateHandler(GEN_SYS_BASIC_TABLE_ID, 0, (VOIDPTR)(&tempSysBasicPara) ) == FAIL)
			{
				EPRINT(CONFIG_LOG, "Basic config update fail for wiegand out");
			}
			*dataPtr = *dataPtr + sizeof(WIEGADN_OUT_CONFIG_CMD_DATA_t);
			break;
		}

		case MSG_SEND_DATA_ON_WIEGAND_CMD:
		{
			SEND_DATA_ON_WGND_CMD_DATA_t *tmpSendDataOnWgndCmdData = (SEND_DATA_ON_WGND_CMD_DATA_t *)(*dataPtr);
			DcInfo.fCInCSN = tmpSendDataOnWgndCmdData ->fCInCSN;
			DcInfo.fCInCardMem = tmpSendDataOnWgndCmdData ->fCInCardMem;
			DcInfo.cardCredntlDetectedF = tmpSendDataOnWgndCmdData ->cardCredntlDetectedF;
			SendUserInfoOnExtWgndRdrPort(UNKNOWN_USER_IDX, tmpSendDataOnWgndCmdData ->eventType, tmpSendDataOnWgndCmdData ->cardId);
			*dataPtr = *dataPtr + sizeof(SEND_DATA_ON_WGND_CMD_DATA_t);
			break;
		}

		case MSG_SET_MULTILANG_CFG_CMD:
		{
			MULTI_LANG_SUPPORT_t *tempMultiLangConfigPtr = (MULTI_LANG_SUPPORT_t*) (*dataPtr);
			if(ConfigUpdateHandler(MULTI_LANGUAGE_CONFIG_TABLE_ID, 0, (VOIDPTR)(tempMultiLangConfigPtr)) != SUCCESS)
			{
				EPRINT(SYS_LOG, "multi lang. cfg set failed");
			}

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(MULTI_LANG_SUPPORT_t);
			break;
		}

		case MSG_READ_SFPR_PARA_CMD:
		{
			SET_FPR_PARA_CMD_t *tmpSetFprParaCmdData = (SET_FPR_PARA_CMD_t *)(*dataPtr);
			// process reader command to enroll user by scanning
			ProcessReaderCommand(msgId, tmpSetFprParaCmdData, sizeof(SET_FPR_PARA_CMD_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(SET_FPR_PARA_CMD_t);
		}
		break;

		case MSG_BT_ACCESS_REQ_RESP:
		{
			BT_ACCESS_REQ_RESPONSE_t *tmpRespBtData = (BT_ACCESS_REQ_RESPONSE_t *)(*dataPtr);

			//after getting response from panel
			//send notification to mobile
			SendNotification(tmpRespBtData->respStatus, tmpRespBtData->respReason);

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(BT_ACCESS_REQ_RESPONSE_t);
		}
		break;

		case MSG_SET_BLE_CNFG_CMD:
		{
			BLUETOOTH_PD_CNFG_t *tmpRespBtData = (BLUETOOTH_PD_CNFG_t *)(*dataPtr);

			BLUETOOTH_CONFIG_t	tempBluetoothPara;

			//get copy of current configuration
			tempBluetoothPara = BluetoothPara;

			//after getting response from panel
			tempBluetoothPara.btEnable = tmpRespBtData->btEnable;
			memcpy(tempBluetoothPara.btName, tmpRespBtData->btName , sizeof(tmpRespBtData->btName) );
			tempBluetoothPara.btRange = tmpRespBtData->btRange;

				INT16 tempBtCustomeRangeStrength = (INT16)tmpRespBtData->btCustomeRangeStrength;
				if((tempBtCustomeRangeStrength >= RANGE_MIN_DBM) && (tempBtCustomeRangeStrength <= RANGE_MAX_DBM))
				{
					tempBluetoothPara.btCustomeRangeStrength = tempBtCustomeRangeStrength;
				}

			if(ConfigUpdateHandler(BLE_CONFIG_ID, UNUSED_FIELD, (VOIDPTR)&tempBluetoothPara) == SUCCESS)
			{
				DPRINT(COMM_LOG,"BLE set successfully");
			}

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(BLUETOOTH_PD_CNFG_t);
		}
		break;

		case GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE:
		{
			if(length < sizeof(ENROLL_USER_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(ENROLL_USER_t) );
				break;
			}

			SendMessageToQt(*dataPtr, msgId);
			*dataPtr += sizeof(ENROLL_USER_t);
			break;
		}

		case GUI_MSG_ENTER_USER_NAME:
		{
			SendMessageToQt(" ", msgId);
			break;
		}

		case GUI_MSG_WIFI_ENABLE_DISABLE_FLAG:
		{
			UINT8PTR dataReceived = *(dataPtr);
			BOOL wifiEnableF;

			wifiEnableF = (BOOL)(*dataReceived);

			if(wifiEnableF != WifiParameters.enableWifi )
			{
				pthread_mutex_lock(&MutexLockAutoSwitchWiFi);
				WifiParameters.enableWifi = wifiEnableF;

				if(WifiParameters.enableWifi == ENABLED)
				{
					ManualWifiConnectF = TRUE;
				}

				StartWifiEnableDisableThreadFunc(WifiParameters.enableWifi);
				SaveWifiParaToSavedWifiList();

				if(WriteWifiParaConfig() == SUCCESS)
				{
					DPRINT(GUI_LOG,"Wifi Parameter Config set successfully");
				}
				pthread_mutex_unlock(&MutexLockAutoSwitchWiFi);
			}

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(wifiEnableF);
			break;
		}
		case MSG_CB_BLE_ACCESS_RESP_CMD:
		{
			if(length < sizeof(CB_READER_ACCESS_REQ_RESPONSE_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(CB_READER_ACCESS_REQ_RESPONSE_t) );
				break;
			}
			ProcessReaderCommand(msgId, *dataPtr, sizeof(CB_READER_ACCESS_REQ_RESPONSE_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(CB_READER_ACCESS_REQ_RESPONSE_t);
		}
		break;

		case MSG_BLE_ENROLLMENT_RESP:
		{
			if(length < sizeof(ACS_ENROLL_RESP_t))
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(ACS_ENROLL_RESP_t) );
				break;
			}
			SendBLEEventTask(BLE_READ_ACCESS_ID_REQ_RESP, *dataPtr, sizeof(ACS_ENROLL_RESP_t));

			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(ACS_ENROLL_RESP_t);
		}
		break;

		case MSG_ACCESS_ID_RESP:
		{
            if(length < sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t))
			{
                EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t) );
				break;
			}
            SendBLEEventTask(BLE_ACCESS_REQ_RESP, *dataPtr, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			// Update data pointer for next messages
            *dataPtr = *dataPtr + sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t);
		}break;

		case MSG_MOBILE_PIN_REQUEST_RESP:
		{
			if(length < sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t))
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t) );
				break;
			}
			SendBLEEventTask(INT_BLE_PIN_ACCESS_RESP, *dataPtr, sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t));
			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(ACS_BLE_ACCESS_REQ_RESPONSE_t);
		}break;

		case MSG_SET_READER_BLE_CNFG_CMD:
		{
			/** Validate data packet length */
			if(length < sizeof(READER_BLUETOOTH_PD_CONFIG_t))
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(READER_BLUETOOTH_PD_CONFIG_t));
				break;
			}

			/** Get pointer to PD Reader Ble config */
			READER_BLUETOOTH_PD_CONFIG_t* tmpPdReaderBleCfg = (READER_BLUETOOTH_PD_CONFIG_t *)(*dataPtr);
			READER_BLUETOOTH_CONFIG_t		tempReaderBleConfig;

			/** Get copy of current Reader Ble config */
			tempReaderBleConfig = ReaderBluetoothPara;

			/** Check Reader Ble config received from panel flag status */
			if(TRUE == tmpPdReaderBleCfg->RdrGrp2ConfigBleFromPanel)
			{
				/** Note : Device other than Arc Dc 200, only Reader Group1 config is applicable */
				/** Update Reader Ble config parameters received from the panel : Reader Group1 */
				tempReaderBleConfig.btEnableReader = tmpPdReaderBleCfg->RdrGrp2btEnable;
				memcpy((VOIDPTR)tempReaderBleConfig.btNameReader,(VOIDPTR)tmpPdReaderBleCfg->RdrGrp2btName,sizeof(tempReaderBleConfig.btNameReader));
				tempReaderBleConfig.btRangeReader = tmpPdReaderBleCfg->RdrGrp2btRange;

				/** Update Config */
				if(ConfigUpdateHandler(READER_BLE_CONFIG_TABLE_ID, UNUSED_FIELD, (VOIDPTR)&tempReaderBleConfig) == SUCCESS)
				{
					DPRINT(COMM_LOG,"Reader BLE config set successfully");
				}
			}
			else
			{	/** Ignored configurations if flag is not set */
				EPRINT(COMM_LOG,"Reader Ble config from panel ignored! RdrGrp2ConfigBleFromPanel[%d]",tmpPdReaderBleCfg->RdrGrp2ConfigBleFromPanel);
				break;
			}

			/** Update data pointer for next messages */
			*dataPtr = *dataPtr + sizeof(READER_BLUETOOTH_PD_CONFIG_t);
		}break;

		case MSG_USER_CONFIG_CMD:
		{
			USER_CONFIG_CMD_DATA_t	* tempUserConfigPtr;
			UINT16			userIdx = INVALID_USER_ID;
			UPDATE_USER_CONFIG_RESP_DATA_t	updateRespData;
			USER_CONFIG_t			tempUserConfig;
			BOOL				blockedUserF = NO;

			if(length < sizeof(USER_CONFIG_CMD_DATA_t) )
			{
				EPRINT(COMM_LOG, "Msg Id[0x%x] Incomplete packet Rcvd [%d] Expec [%d]", msgId, length, sizeof(USER_CONFIG_CMD_DATA_t) );
				break;
			}
			tempUserConfigPtr = (USER_CONFIG_CMD_DATA_t *)(*dataPtr);

			userIdx = tempUserConfigPtr -> userIdx;
			//sending response to panel.
			updateRespData.userId = tempUserConfigPtr -> userId;
			//get copy of current configuration
			if(SUCCESS == GetUserConfig(userIdx, &tempUserConfig))
			{				
				//Copy User Config to Send Structure
				tempUserConfig.enable = tempUserConfigPtr -> enable;
				tempUserConfig.userId = tempUserConfigPtr -> userId;
				snprintf(tempUserConfig.alphaNumId, ALPHANUM_ID_SIZE, "%s", tempUserConfigPtr -> alphaNumId);
				snprintf(tempUserConfig.name, NAME_SIZE_UTF8, "%s", tempUserConfigPtr -> name);
				snprintf(tempUserConfig.pinCode, MAX_PIN_SIZE, "%s", tempUserConfigPtr -> pinCode);
				tempUserConfig.cardId[0] = tempUserConfigPtr -> cardId[0];
				tempUserConfig.cardId[1] = tempUserConfigPtr -> cardId[1];
				tempUserConfig.vip = tempUserConfigPtr -> vip;
				tempUserConfig.visitorIdx = tempUserConfigPtr -> visitorIdx;
				tempUserConfig.userGrpId = tempUserConfigPtr -> userGrpId;
				tempUserConfig.restrictAccess = tempUserConfigPtr -> restrictAccess;
				tempUserConfig.bypassFinger = tempUserConfigPtr -> bypassBiometric;
				tempUserConfig.terminationEnable = tempUserConfigPtr -> terminationEnable;
				memcpy(&tempUserConfig.terminationDate, &tempUserConfigPtr->terminationDate, sizeof(tempUserConfig.terminationDate));
				tempUserConfig.panelDoorAssignF = tempUserConfigPtr -> panelDoorAssignF;
				blockedUserF = tempUserConfigPtr -> blockedUserF;
				if(YES == blockedUserF)
				{
					AddBlockedUser(userIdx, REASON_UNKNOWN);
				}
				else
				{
					RestoreBlockedUser(userIdx);
				}
				
				if(ConfigUpdateHandler(USER_CONFIG_TABLE_ID, userIdx, (VOIDPTR)(&tempUserConfig)) == FAIL)
				{
					updateRespData.status = FAIL;
					DPRINT(COMM_LOG, "userIdx[%d] config save success",userIdx);
				}
				else
				{
					updateRespData.status = SUCCESS;
					DPRINT(COMM_LOG, "userIdx[%d] config save success",userIdx);
				}
			}
			else
			{
				updateRespData.status = FAIL;
				DPRINT(COMM_LOG, "userIdx[%d] config save fail",userIdx);
			}

			SendMsg(MSG_USER_CONFIG_RESP, &updateRespData, sizeof(updateRespData) );
			// Update data pointer for next messages
			*dataPtr = *dataPtr + sizeof(USER_CONFIG_CMD_DATA_t);
		}
		break;
		
		case MSG_DFLT_USER_CNFG_CMD:
		{
			DPRINT(COMM_LOG, "Default user Cnfg CMD Received");
			DefaultPanelDoorUsersConfig();
			DeleteAllUserPhoto();
		}break;

		case MSG_GET_TEMP_USER_FP_CMD:
		{
			SendMsg(MSG_GET_TEMP_USER_FP_RESP, (VOIDPTR)TempUserAddActivity.template, sizeof(TempUserAddActivity.template) );
		}break;

		default:
			break;
	}
}

void SendStatusResponse(void)
{
	DC_STATUS_RESP_DATA_t	dcStatus;
	UINT8					readerId;
	UINT8					fprPort;

	// get status of door controller’s peripheral
	GetInputOutputStatus(&dcStatus.ioStatus);

	for(readerId = 0; readerId < MAX_READERS; readerId++)
	{
		dcStatus.readerStatus[readerId] = GetReaderStatus(readerId);
		dcStatus.readerType[readerId] = ConvertDDRdrTypToPanel(SysBasicPara.reader[readerId].type);
	}

	for(fprPort = 0; fprPort < MAX_FPR_PORT; fprPort++)
	{
		dcStatus.fprSensorCalibStatus[fprPort] = SensorCalibStatus[fprPort];
	}

	dcStatus.rtcStatus = GetRtcStatus();
	GetReaderTamperStatus(&dcStatus.readerTamperStatus);

	// send out message
	SENDMSG(MSG_GET_DC_STATUS_RESP, &dcStatus, sizeof(DC_STATUS_RESP_DATA_t));
}
