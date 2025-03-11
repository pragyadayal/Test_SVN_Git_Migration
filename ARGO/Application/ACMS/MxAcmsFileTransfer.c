//////////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix ComSec Pvt. Ltd., Baroda, India.
//   Project      : ACS (Access Control System)
//   Created By   : Viral Chavda
//   File         : MxAcmsFileTransfer.c
//   Description  : This file will provide File Upload and Backup related APIs.
//
/////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "MxTypedef.h"
#include "MxAcmsCommon.h"
#include "MxAcmsTask.h"
#include "MxAcmsFileTransfer.h"
#include "MxAvResp.h"
#include "MxConfig.h"
#include "MxDcTask.h"
#include "MxLogs.h"
#include "MxSysUtils.h"
#include "MxSysCommonDef.h"
#include "MxBlockDevice.h"
#include "MxCafeteria.h"
#include "MxConfigFileIo.h"
#include "MxMultiLanFileHandling.h"
#include "MxAcmsMultiLangFileXfer.h"
#include "MxFaceRecognition.h"
#include "MxSupremaReader.h"
#include "MxSysCommonUtils.h"
#include "MxFrLinkedLists.h"
#include "MxReaderCommonUtils.h"
#include "MxUserConfig.h"

//******** Extern Variables **********
extern DOOR_ACTIVITY_INFO_t				DcInfo;
extern MMC_INFO_t 						MmcInfo;
extern BOOL								isMulLanFileProcessF;

//******** Defines and Data Types ****
#define FTC_SEQNO_OFFSET	1
#define FTC_FIRST_SEQNO		1



typedef enum
{
	FTC_SET_SEQNO = 0,
	FTC_SET_IDENTIFIER_TYPE,
	FTC_SET_IDENTIFIER_1,
	FTC_SET_IDENTIFIER_2,
	FTC_SET_FILE_LENGTH,
	FTC_SET_DATA_LENGTH,
	MAX_FTC_SET
}FTC_SET_e;

typedef enum
{
	FTC_GET_IDENTIFIER_TYPE,
	FTC_GET_IDENTIFIER_1,
	FTC_GET_IDENTIFIER_2,
	MAX_FTC_GET
}FTC_GET_e;

typedef enum
{
	FTC_ACK_SEQNO,
	FTC_ACK_STATUS,
	MAX_FTC_ACK
}FTC_ACK_e;

//******** Function Prototypes *******
static BOOL WriteAcmsFile(VOIDPTR data, UINT32 length);
static BOOL ReadAcmsFile(VOIDPTR data, UINT32 length );

static FTC_STATUS_e AcmsFtcHandlerSetFileMsgHandler(CHARPTR * rxData, CHARPTR Identfier1, CHARPTR Identfier2);
static FTC_STATUS_e AcmsFtcHandlerSetFileDataFirstSeq(void);
static FTC_STATUS_e AcmsFtcHandlerSetFileDataProcess( CHARPTR * rxData, CHARPTR Identfier1, CHARPTR Identfier2);

static FTC_STATUS_e AcmsFtcHandlerGetFileInfo(void);

static void SetAcmspFirmwareUploadState(void);
static void DeleteFtcFile(void);

//******** Global Variables **********
FTC_INFO_t	ftcInfo;
UINT32		seqNoForEnrlFaceImg = 0;
BOOL SuccessfullyGetImageTmpltFtcDoneF = false;
//******** Static Variables **********
const  FTC_INFO_t	defltFtcInfo =
					{
						.ftcState = FTC_STATE_IDLE,	// state
						.fd = -1,					// File descriptor
						.ftcFilePath[0] = 0,		// File Path
						.ftcCmd = MAX_FTC_CMD,		// current command
						.ftcCurrentSeqNo = 0,		// current sequence numner
						.ftcEndSeqNo = 0,			// end sequence number
						.ftcIdentifierType = FTC_IDENTIFIER_TYPE_FIRMWARE,	// identifier type
						.ftcIdentifier1[0] = 0,	// identifier 1 information
						.ftcIdentifier2[0] = 0,	// identifier 2 information
						.ftcCurrentFileSize = 0,	// current file size
						.ftcEndFileSize = 0,		// end of file size
						.ftcRecvDataLength = 0		// recevied data length
					};

//******** Function Definations ******

//*****************************************************************************
//	InitAcmsFtc()
//	Param:
//		IN:		None
//		OUT:	None
//	Returns:
//		None
//	Description:
//		This function will Initialize the file transfer information.
//*****************************************************************************
void InitAcmsFtc(void)
{
	CHAR sysBuf[FTC_SYS_CMD_LEN];

	sprintf(sysBuf, "rm -rf %s*", ZIP_PATH);
	system(sysBuf);
	// Load Default value in ftc information
	ftcInfo = defltFtcInfo;
}

//*****************************************************************************
//	ResetFtcActivity()
//	Param:
//		IN:		None
//		OUT:	None
//	Returns:
//		None
//	Description:
//		This function will initialize file transfer activity.
// NOTE: it will abort file transfer progress if it is active.
//*****************************************************************************
void ResetFtcActivity(void)
{
	// file transfer is ongoing and process abort due to time out or
	// end of file command
	if(ftcInfo.fd !=  -1)
	{
		DPRINT(ACMS_LOG, "Aborting FTC activity");
		close(ftcInfo.fd);
	}
	// Load Default value in ftc information
	ftcInfo = defltFtcInfo;
}

//*****************************************************************************
//	DeleteFtcFile()
//	Param:
//		IN:		None
//		OUT:	None
//	Returns:
//		None
//	Description:
//		This function will delete file which is specified in ftcFilepath.
//*****************************************************************************
static void DeleteFtcFile(void)
{
	char sysBuf[FTC_SYS_CMD_LEN];

	sprintf(sysBuf, "rm -rf %s", ftcInfo.ftcFilePath);
	system(sysBuf);
}

//*****************************************************************************
//	AcmsFtcHandlerSetFile()
//	Param:
//		IN:		seqNo	- received sequence number
//				rxData	- received command data
//		OUT:	txData	- data to be transmitted
//				txData	- length of data to be transmitted
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This function will set file which is asked to save into the controller.
//*****************************************************************************
BOOL AcmsFtcHandlerSetFile(CHARPTR * rxData, CHARPTR txData, UINT16PTR txDataLen)
{
	BOOL 					retFlag = FAIL;
	FTC_INFO_t				tempFtcInfo;
	CHAR 					tempStr[FTC_IDENTIFIER_LEN];
	FTC_STATUS_e			ftcStatus = FTC_STATUS_FAIL;
	FTC_SET_e				ftcSetCnt;
	NonIDTRequest_t 		tmpNonIDTRequestData;

	tempFtcInfo = defltFtcInfo;

	// parse received set file transfer information
	for(ftcSetCnt = FTC_SET_SEQNO; ftcSetCnt < MAX_FTC_SET; ftcSetCnt++)
	{
		retFlag = ParseString(rxData, tempStr, sizeof(tempStr) );
		if(retFlag == SUCCESS)
		{
			switch(ftcSetCnt)
			{
				case FTC_SET_SEQNO:
					tempFtcInfo.ftcCurrentSeqNo	= (UINT32)atol(tempStr);
					break;

				case FTC_SET_IDENTIFIER_TYPE:
					tempFtcInfo.ftcIdentifierType = (FTC_IDENTIFIER_TYPE_e)atol(tempStr);
					break;

				case FTC_SET_IDENTIFIER_1:
					strcpy(tempFtcInfo.ftcIdentifier1, tempStr);
					break;

				case FTC_SET_IDENTIFIER_2:
					strcpy(tempFtcInfo.ftcIdentifier2, tempStr);
					break;

				case FTC_SET_FILE_LENGTH:
					tempFtcInfo.ftcEndFileSize = (UINT32)atol(tempStr);
					break;

				case FTC_SET_DATA_LENGTH:
					tempFtcInfo.ftcRecvDataLength = (UINT32)atol(tempStr);
					break;

				default:
					break;
			}
		}
		else
		{
			break;
		}
	}

	if(retFlag == SUCCESS)
	{
		// check process in idle and received sequence number is first then
		// we will set file transfer information.
		if(ftcInfo.ftcState == FTC_STATE_IDLE)
		{
			if(tempFtcInfo.ftcIdentifierType == FTC_IDENTIFIER_TYPE_FIRMWARE)
			{
				if(IsDoorBusy() == YES)
				{
					ftcStatus	= FTC_STATUS_FAIL;
					retFlag = FAIL;
				}
				else
				{
					SetAcmspFirmwareUploadState();
				}
			}

			if(tempFtcInfo.ftcIdentifierType == FTC_IDENTIFIER_TYPE_SET_MULTILANGUAGE_FILE)
			{
				DPRINT(SYS_LOG,"[%d]Check for MultiLanguage Process Is Running? Current Status Is[%d]",__LINE__,isMulLanFileProcessF);
				if(isMulLanFileProcessF == BUSY)
				{
					ftcStatus	= FTC_STATUS_FAIL;
					retFlag = FAIL;
					EPRINT(ACMS_LOG, "Multi language file process is already working try after some time.");
				}
				else
				{
					isMulLanFileProcessF = BUSY;
					DPRINT(SYS_LOG,"[%d]Change to BUSY MultiLanguage Process Flag. New Status Is[%d]",__LINE__,isMulLanFileProcessF);
				}
			}

			if(retFlag == SUCCESS)
			{
				if(tempFtcInfo.ftcCurrentSeqNo  == FTC_FIRST_SEQNO)
				{
					ResetFtcActivity();
					if(ftcInfo.ftcCurrentSeqNo != ftcInfo.ftcEndSeqNo)
					{
						DeleteFtcFile();
					}

					ftcInfo.ftcCmd				= FTC_CMD_SET_FIL;
					ftcInfo.ftcIdentifierType	= tempFtcInfo.ftcIdentifierType;
					strcpy(ftcInfo.ftcIdentifier1, tempFtcInfo.ftcIdentifier1);
					strcpy(ftcInfo.ftcIdentifier2, tempFtcInfo.ftcIdentifier2);
					ftcInfo.ftcEndFileSize		= tempFtcInfo.ftcEndFileSize;
					ftcInfo.ftcRecvDataLength	= tempFtcInfo.ftcRecvDataLength;
					DPRINT(ACMS_LOG,"Start ftc for type: [%d]",ftcInfo.ftcIdentifierType);
				}
				ftcInfo.ftcState = FTC_STATE_PROGRESS;
			}
		}

		// if file transfer is in progress
		if(ftcInfo.ftcState == FTC_STATE_PROGRESS)
		{
			// previous command state must not mismatched with current state
			if(ftcInfo.ftcCmd != FTC_CMD_SET_FIL)
			{
				ftcStatus 	= FTC_STATUS_WRONG_ATTEMPT;
				retFlag 	= FAIL;
			}
			// previous file transfer identifier must not mismatched with current identifier
			// or with in spevified limit
			else if( (ftcInfo.ftcIdentifierType != tempFtcInfo.ftcIdentifierType)
					||(ftcInfo.ftcIdentifierType >=  MAX_FTC_IDENTIFIER_TYPE) )
			{
				ftcStatus 	= FTC_STATUS_IDENTIFIER_TYPE_MISMATCHED;
				retFlag 	= FAIL;
			}
			// previous end file size must not mismatched with current end file size
			else if(ftcInfo.ftcEndFileSize != tempFtcInfo.ftcEndFileSize)
			{
				EPRINT(ACMS_LOG,"Previous End File Size [%u] mismatch with current end file size [%u]"
						,ftcInfo.ftcEndFileSize, tempFtcInfo.ftcEndFileSize);
				ftcStatus 	= FTC_STATUS_INVALID_FILE_LENGTH;
				retFlag 	= FAIL;
			}
			// same sequence asked don't process just send proces successful
			else if(tempFtcInfo.ftcCurrentSeqNo == ftcInfo.ftcCurrentSeqNo)
			{
				ftcStatus 	= FTC_STATUS_SUCCESS;
			}
			// if current sequence is next sequence then previous sequence then
			// process the current received data.
			else if(tempFtcInfo.ftcCurrentSeqNo == (ftcInfo.ftcCurrentSeqNo + FTC_SEQNO_OFFSET) )
			{
				ftcInfo.ftcCurrentSeqNo 	= tempFtcInfo.ftcCurrentSeqNo;
				ftcInfo.ftcRecvDataLength	= tempFtcInfo.ftcRecvDataLength;

				// if current file size is greater then end file size
				if( (ftcInfo.ftcCurrentFileSize + ftcInfo.ftcRecvDataLength) > ftcInfo.ftcEndFileSize)
				{
					ftcStatus 	= FTC_STATUS_INVALID_DATA_LENGTH;
					retFlag 	= FAIL;
				}
				else
				{
					ftcStatus = AcmsFtcHandlerSetFileMsgHandler( rxData, tempFtcInfo.ftcIdentifier1, tempFtcInfo.ftcIdentifier2);
					if(ftcStatus != FTC_STATUS_SUCCESS)
					{
						retFlag 	= FAIL;
					}
				}
			}
			else
			{
				ftcStatus 	= FTC_STATUS_SEQ_NO_MISMATCHED;
				retFlag 	= FAIL;
			}
		}

		// file transfer complete state is not valid for set file
		if( ftcInfo.ftcState == FTC_STATE_COMPLETE )
		{
			ftcStatus 	= FTC_STATUS_FAIL;
			retFlag 	= FAIL;
		}
	}

	GetFRNonIDTRequestData(&tmpNonIDTRequestData, NON_IDT_TYPE_FTC);
	if(tmpNonIDTRequestData.faceImgRcvdF == TRUE)
	{
		seqNoForEnrlFaceImg = tempFtcInfo.ftcCurrentSeqNo;	//used in wait state.
	}
	// set file command reply
	sprintf(txData, "%u%c%u%c", tempFtcInfo.ftcCurrentSeqNo, FSP, ftcStatus, FSP);
	*txDataLen = strlen(txData);
	return retFlag;
}

//*****************************************************************************
//	AcmsFtcHandlerSetFileMsgHandler()
//	Param:
//		IN:		seqNo	- received sequence number
//				rxData	- received command data
//		OUT:	txData	- data to be transmitted
//				txData	- length of data to be transmitted
//				status 	- firmware upgrade status
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This function will set file which is asked to save into the controller.
//
//*****************************************************************************
static FTC_STATUS_e AcmsFtcHandlerSetFileMsgHandler(CHARPTR * rxData, CHARPTR Identfier1, CHARPTR Identfier2)
{
	FTC_STATUS_e 	status = FTC_STATUS_SUCCESS;
	static UINT8	percentageCnt = 0;
	UINT8			tmpPercentCnt;
	BACKUP_UPDATE_TYPE_e buType = BU_TYPE_FIRMWARE;
	CHAR	sysCmdBuf[FTC_SYS_CMD_LEN];
	UINT16	userIdx;
	UINT32	userId;
	UINT8	faceTmplIdx;	

	// if current sequence number is first then set file transfer information accordingly
	if(ftcInfo.ftcCurrentSeqNo == FTC_FIRST_SEQNO)
	{
		status =  AcmsFtcHandlerSetFileDataFirstSeq();
		if(ftcInfo.ftcIdentifierType == FTC_IDENTIFIER_TYPE_FIRMWARE && status == FTC_STATUS_SUCCESS)
		{
			percentageCnt = 0;
			SendBuProgAvResp(BU_TYPE_FIRMWARE, percentageCnt);
		}
	}

	if(status == FTC_STATUS_SUCCESS)
	{
		status =  AcmsFtcHandlerSetFileDataProcess(rxData, Identfier1, Identfier2);

		if(status == FTC_STATUS_SUCCESS)
		{
			ftcInfo.ftcCurrentFileSize += ftcInfo.ftcRecvDataLength;
			if(ftcInfo.ftcIdentifierType == FTC_IDENTIFIER_TYPE_FIRMWARE)
			{
				// Update percentage
				tmpPercentCnt = (UINT8)( (ftcInfo.ftcCurrentSeqNo * 100)/ ftcInfo.ftcEndSeqNo );
				if(tmpPercentCnt != percentageCnt)
				{
					percentageCnt = tmpPercentCnt;
					SendBuProgAvResp(BU_TYPE_FIRMWARE, percentageCnt);
				}
			}
			// if file trnasfer complete then process set file
			if( ftcInfo.ftcCurrentFileSize == ftcInfo.ftcEndFileSize )
			{
				switch(ftcInfo.ftcIdentifierType)
				{
					case FTC_IDENTIFIER_TYPE_FIRMWARE:
						// file transfer completed
						ftcInfo.ftcState = FTC_STATE_COMPLETE;
						// start firmware upgrade
						if(AcmsStartFwUpgradeThread(ftcInfo.ftcIdentifier1) == FAIL)
						{
							EPRINT(ACMS_LOG, "Failed to start thread for firmware upgrade");
							SetAudioVisual(USER_RESP_BACKUP_UPDATE_XFER_FAILED, INTERNAL_PORT, &buType);
							SwitchDcToIdleState();
						}
						// reset the file transfer information
						percentageCnt = 0;
						ResetFtcActivity();
						break;

					case FTC_IDENTIFIER_TYPE_USER_PHOTO:
						// file transfer completed
						ftcInfo.ftcState = FTC_STATE_COMPLETE;

						DeleteSingleUserPhoto(atoi(ftcInfo.ftcIdentifier1));

						// move current temporary file
						sprintf(sysCmdBuf, "mv %s %s/%s/ &", ftcInfo.ftcFilePath, MmcInfo.devPath, USER_PICS_FOLDER);
						system(sysCmdBuf);

						// Reset the file transfer information
						ResetFtcActivity();
						break;

					case FTC_IDENTIFIER_TYPE_SET_MULTILANGUAGE_FILE:/*MODIFY?*///
						// file transfer completed
						ftcInfo.ftcState = FTC_STATE_COMPLETE;
						if(AcmsStartMultiLangFileParsingThread(ftcInfo.ftcIdentifier1) == FAIL)
						{
							EPRINT(ACMS_LOG, "Failed to start thread for File Parse");
						}

						// reset the file transfer information
						ResetFtcActivity();
						break;

					case FTC_IDENTIFIER_TYPE_SET_USER_FACE_TMPL:
						{
							userId = (UINT32)atoll(ftcInfo.ftcIdentifier1);
							faceTmplIdx = (UINT32)atoll(ftcInfo.ftcIdentifier2);

							NonIDTRequest_t tmpNonIDTRequestData;
							GetFRNonIDTRequestData(&tmpNonIDTRequestData,NON_IDT_TYPE_FTC);

							if(IsUserValid(userId, &userIdx) == NO)
							{
								DPRINT(DC_LOG,"User not valid [%d]\n",userId);
								status	= FTC_STATUS_IDENTIFIER_INVALID;
								// file transfer completed
								ftcInfo.ftcState = FTC_STATE_COMPLETE;
								// Reset the file transfer information
								ResetFtcActivity();
							}
							else if(tmpNonIDTRequestData.isLocallyProcessing == TRUE 
								|| tmpNonIDTRequestData.frProcessRequestStatus == INQUEUE_STATUS_SET)
							{
								EPRINT(DC_LOG, "FR NonIDT operation is going on..FR is busy");
								status = FTC_STATUS_SAVE_FP_IMAGE_ERROR_ACS_BUSY; //This is used in both FR and FP Image feature
								// file transfer completed
								ftcInfo.ftcState = FTC_STATE_COMPLETE;
								// Reset the file transfer information
								ResetFtcActivity();
							}
							else
							{
								NonIDTRequest_t tmpNonIDTRequestData;

								GetFRNonIDTRequestData(&tmpNonIDTRequestData, NON_IDT_TYPE_FTC);
								tmpNonIDTRequestData.faceEnrollImgSendSuccessF = FTC_STATUS_UNKNOWN_ERROR;
								tmpNonIDTRequestData.faceImgRcvdF = TRUE;
								tmpNonIDTRequestData.userRefId = userId;
								tmpNonIDTRequestData.faceIdx = faceTmplIdx;
								tmpNonIDTRequestData.frProcessRequestId = CMD_USR_ERL;
								tmpNonIDTRequestData.isNonIdtPending = false;
								SetFRNonIDTRequestData(tmpNonIDTRequestData, NON_IDT_TYPE_FTC);

								if(ProcessNonIDTRequest(ftcInfo.ftcFilePath, NULL) == SUCCESS)
								{
									return SUCCESS;
								}
								else
								{
									EPRINT(SYS_LOG,"Failed to start the thread for Enrollment of Face Template");

									InitFRNonIDTRequestData(NON_IDT_TYPE_FTC);
									// Delete the received file
									sprintf(sysCmdBuf,"rm -rf %s",ftcInfo.ftcFilePath);
									system(sysCmdBuf);

									status	= FTC_STATUS_FAIL;
									// file transfer completed
									ftcInfo.ftcState = FTC_STATE_COMPLETE;
									// Reset the file transfer information
									ResetFtcActivity();
									return FAIL;
								}
							}
						}
						break;
					default:
						break;
				}
			}
		}
	}
	return status;
}

//*****************************************************************************
//	AcmsFtcHandlerSetFileDataFirstSeq()
//	Param:
//		IN:		Identifier1	- identifier 1 information
//				Identifier2	- identifier 2 information
//		OUT:	status		- error status
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This function will verify the data in first sequence.
//
//*****************************************************************************
static FTC_STATUS_e AcmsFtcHandlerSetFileDataFirstSeq(void)
{
	FTC_STATUS_e status = FTC_STATUS_SUCCESS;
	UINT16			userIdx;
	UINT32			userId;

	switch(ftcInfo.ftcIdentifierType)
	{
		case FTC_IDENTIFIER_TYPE_FIRMWARE:

			// verify identifier 1 and create file path
			if(strstr(ftcInfo.ftcIdentifier1, DIRECT_DOORFIRMWARE_FILE) == NULL)
			{
				status	= FTC_STATUS_IDENTIFIER_INVALID;
			}
			else if(strstr(ftcInfo.ftcIdentifier1, ZIP_FILE_EXT) == NULL)
			{
				status	= FTC_STATUS_IDENTIFIER_INVALID;
			}
			else
			{
				sprintf(ftcInfo.ftcFilePath,"%s%s",ZIP_PATH,ftcInfo.ftcIdentifier1);
			}
			break;

		case FTC_IDENTIFIER_TYPE_USER_PHOTO:
			// verify identifier 1 and create file path
			if(IsUserValid(atoi(&ftcInfo.ftcIdentifier1[0]), NULL) == NO)
			{
				status	= FTC_STATUS_IDENTIFIER_INVALID;
				ResetFtcActivity();
			}
			else
			{
				if(strlen(ftcInfo.ftcIdentifier2) > 1)
				{
					sprintf(ftcInfo.ftcFilePath, "%s%s%u.%s", TEMP_DIR, USER_PICS_PREFIX, atoi(ftcInfo.ftcIdentifier1), ftcInfo.ftcIdentifier2);
				}
				else
				{
					sprintf(ftcInfo.ftcFilePath, "%s%s%u%s", TEMP_DIR, USER_PICS_PREFIX, atoi(ftcInfo.ftcIdentifier1), DEFAULT_PICS_FILE_EXT );
				}

				if( ftcInfo.ftcEndFileSize >  MAX_PHOTO_SIZE)
				{
					status	= FTC_STATUS_INVALID_FILE_LENGTH;
					ResetFtcActivity();
				}
			}
			break;

		case FTC_IDENTIFIER_TYPE_SET_MULTILANGUAGE_FILE:
			 sprintf(ftcInfo.ftcFilePath, "%s",ftcInfo.ftcIdentifier1);
			break;

		case FTC_IDENTIFIER_TYPE_SET_USER_FACE_TMPL:
			// verify identifier 1 and create file path
			userId = (UINT32)atoll(ftcInfo.ftcIdentifier1);
			if(IsUserValid(userId, &userIdx) == NO)
			{
				status	= FTC_STATUS_IDENTIFIER_INVALID;
				ResetFtcActivity(); //On Failure, for next FTC, server will send seqNo w.r.t. seqNo sent in FTC_RDY.
				//This is to resume a half received file. But on success, it will resume new file with seqNO. = FIRST_SEQ_NO (1).
				//On success for face template, server will send us next template from seq No = 1.
				//But for Face template fail, server will send us next seq No for new file based on FTC_RDY seqNo.
				//So, to overcome this we ReserFtcActivity() here, so that we will send 0 in FTC_RDY and proper comm could be established. ACTASWSP1-1345
			}
			else
			{
				sprintf(ftcInfo.ftcFilePath, "%sUserFaceTmpl%u_%u.jpg", TEMP_DIR, atoi(ftcInfo.ftcIdentifier1), atoi(ftcInfo.ftcIdentifier2) );
				//the below condition is commented as we don't want to limit the file size for image.
//				if( ftcInfo.ftcEndFileSize >  MAX_PHOTO_SIZE)
//				{
//					status	= FTC_STATUS_INVALID_FILE_LENGTH;
//					ResetFtcActivity(); //On Failure, for next FTC, server will send seqNo w.r.t. seqNo sent in FTC_RDY.
//				//This is to resume a half received file. But on success, it will resume new file with seqNO. = FIRST_SEQ_NO (1).
//				//On success for face template, server will send us next template from seq No = 1.
//				//But for Face template fail, server will send us next seq No for new file based on FTC_RDY seqNo.
//				//So, to overcome this we ReserFtcActivity() here, so that we will send 0 in FTC_RDY and proper comm could be established. ACTASWSP1-1345
//				}
			}
			break;

		default:
			status 	= FTC_STATUS_IDENTIFIER_TYPE_MISMATCHED;
			break;
	}

	if(status == FTC_STATUS_SUCCESS)
	{
		// opne file in write mode and find end sequence number
        ftcInfo.fd = open(ftcInfo.ftcFilePath, O_WRONLY | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
		if(ftcInfo.fd  < 0)
		{
			status	= FTC_STATUS_FAIL;
			EPRINT(ACMS_LOG,"fail to open ftc file %s", ftcInfo.ftcFilePath);
		}
		else
		{
			ftcInfo.ftcEndSeqNo =  (ftcInfo.ftcEndFileSize / MAX_SIZE_FILE_DATA_CHUNK);
			if( (ftcInfo.ftcEndFileSize % MAX_SIZE_FILE_DATA_CHUNK) != 0)
			{
				ftcInfo.ftcEndSeqNo++;
			}
		}
	}

	return status;
}

//*****************************************************************************
//	AcmsFtcHandlerSetFileDataProcess()
//	Param:
//		IN:		rxData		- received data
//				Identifier1	- identifier 1 information
//				Identifier2	- identifier 2 information
//		OUT:	status		- error status
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This function will process set file data and write it to appropriate
//		file.
//
//*****************************************************************************
static FTC_STATUS_e AcmsFtcHandlerSetFileDataProcess( CHARPTR * rxData, CHARPTR Identfier1, CHARPTR Identfier2)
{
	FTC_STATUS_e status = FTC_STATUS_SUCCESS;

	// if previous and current identifier 1 mismatched
	if( strcmp(ftcInfo.ftcIdentifier1,Identfier1) != 0 )
	{
		status	= FTC_STATUS_IDENTIFIER_INVALID;
	}
	// if previous and current identfier 2 mismatched
	else if( strcmp(ftcInfo.ftcIdentifier2, Identfier2) != 0 )
	{
		status	= FTC_STATUS_IDENTIFIER_INVALID;
	}
	// if received chunk is greater than our buffer
	else if( ftcInfo.ftcRecvDataLength > MAX_SIZE_FILE_DATA_CHUNK)
	{
		status	= FTC_STATUS_INVALID_DATA_LENGTH;
	}
	// if current sequence number exceed from end sequence number
	else if( ftcInfo.ftcCurrentSeqNo > ftcInfo.ftcEndSeqNo )
	{
		status	= FTC_STATUS_SEQ_NO_MISMATCHED;
	}
	// write row data to file
	else
	{
		switch(ftcInfo.ftcIdentifierType)
		{
			case FTC_IDENTIFIER_TYPE_FIRMWARE:
			case FTC_IDENTIFIER_TYPE_USER_PHOTO:
			case FTC_IDENTIFIER_TYPE_SET_MULTILANGUAGE_FILE:
			case FTC_IDENTIFIER_TYPE_SET_USER_FACE_TMPL:
			{
				if(WriteAcmsFile( (VOIDPTR)(*rxData), ftcInfo.ftcRecvDataLength) == FAIL)
				{
					status	= FTC_STATUS_FAIL;
				}
				break;
			}
			default:
			{
				status = FTC_STATUS_IDENTIFIER_INVALID;
				break;
			}
		}
	}

	return status;
}

//*****************************************************************************
//	AcmsFtcHandlerGetFile()
//	Param:
//		IN:		rxData		- received command data
//		OUT:	txData		- data to be transmitted
//				txDataLen	- length of data to be transmitted
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This function will get file which is asked.
//
//*****************************************************************************
BOOL AcmsFtcHandlerGetFile(CHARPTR * rxData, CHARPTR txData, UINT16PTR txDataLen)
{
	BOOL 					retFlag = FAIL;
	FTC_INFO_t				tempFtcInfo;
	CHAR 					tempStr[FTC_IDENTIFIER_LEN];
	FTC_STATUS_e			ftcStatus = FTC_STATUS_FAIL;
	FTC_GET_e				ftcSetCnt;

	tempFtcInfo = defltFtcInfo;

	// parse get file information
	for( ftcSetCnt = FTC_GET_IDENTIFIER_TYPE; ftcSetCnt < MAX_FTC_GET; ftcSetCnt++)
	{
		retFlag = ParseString(rxData, tempStr, sizeof(tempStr));

		if( retFlag == SUCCESS)
		{
			switch(ftcSetCnt)
			{
				case FTC_GET_IDENTIFIER_TYPE:
					tempFtcInfo.ftcIdentifierType	=
										(FTC_IDENTIFIER_TYPE_e)atol(tempStr);
					break;

				case FTC_GET_IDENTIFIER_1:
					strcpy(tempFtcInfo.ftcIdentifier1, tempStr);
					break;

				case FTC_GET_IDENTIFIER_2:
					strcpy( tempFtcInfo.ftcIdentifier2, tempStr);
					break;

				default:
					break;
			}
		}
		else
		{
			break;
		}
	}

	if( retFlag == SUCCESS)
	{
		// if file transfer in idle mode then get file information
		if( ftcInfo.ftcState == FTC_STATE_IDLE )
		{
			ftcInfo.ftcState			= FTC_STATE_PROGRESS;
			ftcInfo.ftcCmd				= FTC_CMD_GET_FIL;
			ftcInfo.ftcIdentifierType	= tempFtcInfo.ftcIdentifierType;
			strcpy( ftcInfo.ftcIdentifier1, tempFtcInfo.ftcIdentifier1);
			strcpy( ftcInfo.ftcIdentifier2, tempFtcInfo.ftcIdentifier2);

			ftcStatus = AcmsFtcHandlerGetFileInfo();

			if( ftcStatus != FTC_STATUS_SUCCESS)
			{
				DPRINT(ACMS_LOG, "Get file transfer information failed. Status:[%d]",ftcStatus);
				retFlag = FAIL;
			}
		}
		// if file transfer not in idle then discard the process
		else
		{
			ftcStatus 	= FTC_STATUS_WRONG_ATTEMPT;
			retFlag 	= FAIL;
		}
	}
	// fill error information if fail to get file information
	if( retFlag == FAIL)
	{
		sprintf(txData, "%u%c", ftcStatus, FSP);
		*txDataLen = strlen(txData);
	}

	return retFlag;
}

//*****************************************************************************
//	AcmsFtcHandlerGetFile()
//	Param:
//		IN:		none
//		OUT:	none
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This function will get file information.
//
//*****************************************************************************
static FTC_STATUS_e AcmsFtcHandlerGetFileInfo(void)
{
	FTC_STATUS_e 	status = FTC_STATUS_SUCCESS;
	struct stat 	fileInfo;
	UINT16			userIdx = 0, imgId = 0;
	UINT32			userId = 0;
	UINT8 			faceIdx = 0;
	UINT16		fpIndex;

	// get file path
	switch( ftcInfo.ftcIdentifierType)
	{
		case FTC_IDENTIFIER_TYPE_EVENT_PHOTO:
			sprintf(ftcInfo.ftcFilePath, "%s/%s/%u_%u%s", MmcInfo.devPath, EVENT_PICS_FOLDER,
						atoi(ftcInfo.ftcIdentifier1), atoi(ftcInfo.ftcIdentifier2), EVENT_PICS_FILE_EXT);
			break;
		case FTC_IDENTIFIER_TYPE_GET_FACE_TMPLT:
			{
				DPRINT(ACMS_LOG,"Get Face Template ftc");
				userId = (UINT32)atoll(ftcInfo.ftcIdentifier1);
				faceIdx = (UINT8)atoll(ftcInfo.ftcIdentifier2);
				if(IsGetFaceReqQueuePending() == FALSE)
				{
					EPRINT(ACMS_LOG,"Wrong Attempt for Get Template: No req pending");
					status = FTC_STATUS_WRONG_ATTEMPT;
				}
				else if(IsUserAndFaceValidForGetFaceReq(userId, faceIdx))
				{
					sprintf(ftcInfo.ftcFilePath, "%s/%u/%u_%u", LOCAL_ERL_FILE_PATH,
								atoi(ftcInfo.ftcIdentifier1),atoi(ftcInfo.ftcIdentifier1), atoi(ftcInfo.ftcIdentifier2) );
					SetGetFaceReqTmpltState(GET_FACE_PROCESSING_FTC);
				}
				else
				{
					//If user in GetFaceReq is different from what we received from GET_FIL,
					//we consider this as a wrong attempt.
					//Remove this node and process next as for the same user, this could stuck on
					//same request response infinitly.
					EPRINT(ACMS_LOG,"Wrong Attempt for Get Template: User Id Mismatch:[%u]",userId);
					status = FTC_STATUS_WRONG_ATTEMPT;
					SetGetFaceReqTmpltState(GET_FACE_SUCCESS_FTC); //set as success, so that we process next request from MonitorGetFaceReq();
					SuccessfullyGetImageTmpltFtcDoneF = TRUE;
				}
			}
			break;

		case FTC_IDENTIFIER_TYPE_GET_USER_FACE_IMAGE:
			{
				DPRINT(ACMS_LOG,"Get Face Image Received");
				userId = (UINT32)atoll(ftcInfo.ftcIdentifier1);
				faceIdx = (UINT8)atoll(ftcInfo.ftcIdentifier2);
				if(IsGetFaceReqQueuePending() == FALSE)
				{
					EPRINT(ACMS_LOG,"Wrong Attempt for Get Image: No req Pending");
					status = FTC_STATUS_WRONG_ATTEMPT;
				}
				else if(IsUserAndFaceValidForGetFaceReq(userId, faceIdx))
				{
					sprintf(ftcInfo.ftcFilePath, "%s/%u/%u_%u.jpg", LOCAL_ERL_FILE_PATH,
								atoi(ftcInfo.ftcIdentifier1),atoi(ftcInfo.ftcIdentifier1), atoi(ftcInfo.ftcIdentifier2) );
					SetGetFaceReqImageState(GET_FACE_PROCESSING_FTC);
				}
				else
				{
					EPRINT(ACMS_LOG,"Wrong Attempt for Get Image: User Id Mismatch:[%u]",userId);
					status = FTC_STATUS_WRONG_ATTEMPT;
					SetGetFaceReqImageState(GET_FACE_SUCCESS_FTC);
					SuccessfullyGetImageTmpltFtcDoneF = TRUE;
				}
			}
			break;
		case FTC_IDENTIFIER_TYPE_GET_EXCEP_FACE_IMG:
		{
			imgId = (UINT16)atoll(ftcInfo.ftcIdentifier1);
			if(IsGetFaceReqQueuePending() == FALSE)
			{
				EPRINT(ACMS_LOG,"Wrong Attempt for Get Exceptional Image: No req Pending");
				status = FTC_STATUS_WRONG_ATTEMPT;
			}
			else if(IsExcepGetFaceReqValid(imgId))
			{
				sprintf(ftcInfo.ftcFilePath, "%s/%u.jpg", EXCEPTIONAL_FACE_FILE_PATH,
							atoi(ftcInfo.ftcIdentifier1));
				SetGetFaceReqImageState(GET_FACE_PROCESSING_FTC);
			}
			else
			{
				EPRINT(ACMS_LOG,"Wrong Attempt for Get Image: Img Id Mismatch:[%u]",imgId);
				status = FTC_STATUS_WRONG_ATTEMPT;
				SetGetFaceReqImageState(GET_FACE_SUCCESS_FTC);
				SuccessfullyGetImageTmpltFtcDoneF = TRUE;
			}
		}break;
		case FTC_IDENTIFIER_TYPE_GET_FINGERPRINT_IMAGE:
		{
			userId = (UINT32)atoll(ftcInfo.ftcIdentifier1);
			UINT8 fpIdx = ((UINT8)atoi(ftcInfo.ftcIdentifier2) / 10);
			if(IsUserValid(userId, &userIdx) == YES)
			{
				sprintf(ftcInfo.ftcFilePath, "%s%u_%u", SYS_USER_FP_IMG_FILE_PATH, userId, fpIdx+1);
			}
			else
			{
				status = FTC_STATUS_IDENTIFIER_INVALID;
			}
		}
		break;

		default:
			status = FTC_STATUS_IDENTIFIER_INVALID;
			break;
	}

	if( status == FTC_STATUS_SUCCESS)
	{
		// open file and calculate end sequence
		ftcInfo.fd	= open(ftcInfo.ftcFilePath,O_RDONLY | O_SYNC ,
                  S_IRWXU | S_IRWXG | S_IRWXO);
		if(ftcInfo.fd < 0)
		{
			if(ftcInfo.ftcIdentifierType == FTC_IDENTIFIER_TYPE_GET_FINGERPRINT_IMAGE)
			{
				BOOL retVal;
				SAVE_FP_IMAGE_ERROR_CODE_e  saveFpimageErrorCode;
				userId = (UINT32)atoll(ftcInfo.ftcIdentifier1);
				fpIndex = ((UINT16)atoi(ftcInfo.ftcIdentifier2) / 10) + 1;
				
				retVal = SearchEntryFromRecordFile(userId, fpIndex, &saveFpimageErrorCode);
				if(YES == retVal)
				{
					DeleteEntryFromRecordFile(userId, fpIndex);
					status = saveFpimageErrorCode;
				}
				else
				{
					status = FTC_STATUS_SAVE_FP_IMAGE_ERROR_WRONG_LOCATION;
				}
			}
			else
			{
				DPRINT(ACMS_LOG,"Failed to Open File for FTC:[%s]",ftcInfo.ftcFilePath);
				status	= FTC_STATUS_FAIL;
			}
		}
		else
		{
			// get file size information through stat command
			if(stat(ftcInfo.ftcFilePath, &fileInfo) < 0)
			{
				EPRINT(ACMS_LOG,"Failed to get size for File:[%s]",ftcInfo.ftcFilePath);
				status	= FTC_STATUS_FAIL;
			}
			else
			{
				ftcInfo.ftcCurrentSeqNo		= FTC_FIRST_SEQNO;
				ftcInfo.ftcEndFileSize 		= fileInfo.st_size;
				ftcInfo.ftcEndSeqNo =  (ftcInfo.ftcEndFileSize / MAX_SIZE_FILE_DATA_CHUNK);
				if( (ftcInfo.ftcEndFileSize % MAX_SIZE_FILE_DATA_CHUNK) != 0)
				{
					ftcInfo.ftcEndSeqNo++;
				}
			}
		}
	}

	return status;
}

//*****************************************************************************
//	AcmsFtcHandlerRplFile()
//	Param:
//		IN:		none
//		OUT:	txData		- data to be sent
//				txDataLen	- data length to be sent
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This function will reply file information.
//
//*****************************************************************************
BOOL AcmsFtcHandlerRplFile(CHARPTR txData, UINT16PTR txDataLen)
{
	BOOL 	retFlag = FAIL;
	CHAR	tempBuff[MAX_SIZE_FILE_DATA_CHUNK];

	// for reply file only progress
	if( ftcInfo.ftcState == FTC_STATE_PROGRESS )
	{
		if(ftcInfo.ftcCurrentSeqNo <= ftcInfo.ftcEndSeqNo)
		{
			// check how many bytes are available to send ?
			ftcInfo.ftcRecvDataLength = (ftcInfo.ftcEndFileSize - ftcInfo.ftcCurrentFileSize);

			if(ftcInfo.ftcRecvDataLength >=  MAX_SIZE_FILE_DATA_CHUNK)
			{
				ftcInfo.ftcRecvDataLength = MAX_SIZE_FILE_DATA_CHUNK;
			}

			// read data from the file
			retFlag = ReadAcmsFile(tempBuff, ftcInfo.ftcRecvDataLength);

			if( retFlag == SUCCESS)
			{
				// create chunk of file to send
				sprintf(txData,"%u%c%u%c%s%c%s%c%u%c%u%c", ftcInfo.ftcCurrentSeqNo, FSP,
						ftcInfo.ftcIdentifierType, FSP, ftcInfo.ftcIdentifier1, FSP,
						ftcInfo.ftcIdentifier2, FSP, ftcInfo.ftcEndFileSize, FSP,
						ftcInfo.ftcRecvDataLength, FSP);
				*txDataLen = strlen(txData);
				memcpy(&txData[*txDataLen],tempBuff,ftcInfo.ftcRecvDataLength);
				*txDataLen += MAX_SIZE_FILE_DATA_CHUNK;
			}
			else
			{
				sprintf(txData,"%u%c", FTC_STATUS_SAVE_FP_IMAGE_ERROR_OTHER_REASON, FSP);
				*txDataLen = strlen(txData);
		}
	}
		else
		{
			sprintf(txData,"%u%c", FTC_STATUS_SEQ_NO_MISMATCHED, FSP);
			*txDataLen = strlen(txData);
		}
	}
	else
	{
		sprintf(txData,"%u%c", FTC_STATUS_WRONG_ATTEMPT, FSP);
		*txDataLen = strlen(txData);
	}

	return retFlag;
}

//*****************************************************************************
//	AcmsFtcHandlerAckFile()
//	Param:
//		IN:		rxData		- acknowledgement received
//		OUT:	none
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This function will verify acknowledgement.
//		DO NOT RESET FTC INFO (ResetFtcActivity()) HERE. As ftcInfo.ftcState is used
//		to determine completion of FTC.
//*****************************************************************************
FTC_ACK_STAT_e AcmsFtcHandlerAckFile(CHARPTR * rxData, CHARPTR txData, UINT16PTR txDataLen)
{
	FTC_ACK_STAT_e	retFlag = FTC_ACK_FAIL;
	CHAR 			tempStr[FTC_IDENTIFIER_LEN];
	FTC_STATUS_e	ftcStatus = FTC_STATUS_FAIL;
	FTC_ACK_e		ftcAckCnt;
	UINT32			seqNo;

	// valid state for acknowledgement is progress only
	if(ftcInfo.ftcState == FTC_STATE_PROGRESS)
	{
		for( ftcAckCnt = FTC_ACK_SEQNO; ftcAckCnt < MAX_FTC_ACK; ftcAckCnt++)
		{
			if(ParseString(rxData, tempStr, sizeof(tempStr)) == SUCCESS)
			{
				retFlag = FTC_ACK_SUCCESS;

				switch(ftcAckCnt)
				{
					case FTC_ACK_SEQNO:
						seqNo	= (UINT32)atol(tempStr);
						break;

					case FTC_ACK_STATUS:
						ftcStatus = (FTC_STATUS_e) atol(tempStr);
						break;

					default:
						break;
				}
			}
			else
			{
				retFlag = FTC_ACK_FAIL;
				break;
			}
		}

		// parsing successful but sequence number not matched or status not successful
		if(retFlag == FTC_ACK_SUCCESS)
		{
			if( (seqNo > ftcInfo.ftcCurrentSeqNo) || (ftcStatus != FTC_STATUS_SUCCESS) )
			{
				EPRINT(ACMS_LOG,"ACK_FIL Sequence Number received:[%d]. Expected:[%d]",seqNo,ftcInfo.ftcCurrentSeqNo);
				retFlag = FTC_ACK_FAIL;
				sprintf(txData,"%u%c", FTC_STATUS_SEQ_NO_MISMATCHED, FSP);
				*txDataLen = strlen(txData);
			}
			else if(ftcStatus != FTC_STATUS_SUCCESS)
			{
				DPRINT(ACMS_LOG,"ACK_FIL failed. Received status:[%d].",ftcStatus);
				retFlag = FTC_ACK_FAIL;
				sprintf(txData,"%u%c", FTC_STATUS_WRONG_ATTEMPT, FSP);
				*txDataLen = strlen(txData);
			}
			else if(seqNo < ftcInfo.ftcCurrentSeqNo)
			{
				retFlag = FTC_ACK_DUPLICATE;
			}
			else
			{
				// increment sequence number for send reply command
				ftcInfo.ftcCurrentSeqNo++;
				ftcInfo.ftcCurrentFileSize+= ftcInfo.ftcRecvDataLength;
				
				if(ftcInfo.ftcCurrentFileSize == ftcInfo.ftcEndFileSize)
				{
					DPRINT(ACMS_LOG, "FILE TRANSFER DONE");
					switch(ftcInfo.ftcIdentifierType)
					{
						case FTC_IDENTIFIER_TYPE_GET_FINGERPRINT_IMAGE:
						{
							UINT32 userId = (UINT32)atoll(ftcInfo.ftcIdentifier1);
							UINT16 userIdx, fpIdx = ((UINT16)atoi(ftcInfo.ftcIdentifier2)/10);
							
							// file transfer completed
							ftcInfo.ftcState = FTC_STATE_COMPLETE;
							
							if(IsUserValid(userId, &userIdx) == NO)
							{
								DPRINT(DC_LOG,"User not valid [%d]\n",userId);
							}
							else
							{
								DeleteSingleUserFingerImage(userId, fpIdx+1);
							}
							
							// Reset the file transfer information
//							ResetFtcActivity();
						}break;
						
						default:
							break;
					}
				}

				if( ftcInfo.ftcCurrentSeqNo > ftcInfo.ftcEndSeqNo )
				{
					ftcInfo.ftcState = FTC_STATE_COMPLETE;
					retFlag = FTC_ACK_FAIL;
				}
			}
		}
		else
		{
			DPRINT(ACMS_LOG,"ACK_FIL failed.");
			retFlag = FTC_ACK_FAIL;
			sprintf(txData,"%u%c", FTC_STATUS_FAIL, FSP);
			*txDataLen = strlen(txData);
		}
	}
	else
	{
			DPRINT(ACMS_LOG,"ACK_FIL failed. State not idle");
			retFlag = FTC_ACK_FAIL;
			sprintf(txData,"%u%c", FTC_STATUS_WRONG_ATTEMPT, FSP);
			*txDataLen = strlen(txData);
	}

	return retFlag;
}

//*****************************************************************************
//	WriteAcmsFile()
//	Param:
//		IN:		data	- data to be write
//				length	- length of data to be write
//		OUT:	None
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This function writes given data into file which is already opeened in
//		write mode.
//*****************************************************************************
static BOOL WriteAcmsFile(VOIDPTR data, UINT32 length)
{
	BOOL retFlag = FAIL;

	if((UINT32)write(ftcInfo.fd, data,length) == length)
	{
		retFlag = SUCCESS;
	}
	return retFlag;
}

//*****************************************************************************
//	ReadAcmsFile()
//	Param:
//		IN:		currentSeq	- received sequence number
//				endSeqNo	- end sequence number
//				data		- recevied command data
//				length		- received data length
//		OUT:	None
//	Returns:
//		FAIL/SUCCESS
//	Description:
//		This function will write the file.
//
//*****************************************************************************
static BOOL ReadAcmsFile(VOIDPTR data, UINT32 length )
{
	BOOL	retFlag = FAIL;

	if ((UINT32)read(ftcInfo.fd, data, length) == length)
	{
		retFlag = SUCCESS;
	}
	else
	{
			EPRINT(ACMS_LOG,"Failed to Read File for FTC");
	}

	return retFlag;
}

//*****************************************************************************
//	SetAcmspFirmwareUploadState()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This API will set state of the http firmware upload.
//*****************************************************************************
static void SetAcmspFirmwareUploadState(void)
{
	CleanUpTempUserFP();
	DcInfo.curState = DC_ACMS_FIRMWARE_UPLOAD_STATE;
}

VOID AcmsFtcEndFilHandler()
{

	DPRINT(ACMS_LOG,"FTC END Done: Id:[%d]",ftcInfo.ftcIdentifierType);
	switch(ftcInfo.ftcIdentifierType)
	{
		case FTC_IDENTIFIER_TYPE_GET_FACE_TMPLT:
		{
			CHAR fileBuff[512];
			SuccessfullyGetImageTmpltFtcDoneF = true;
			snprintf(fileBuff,sizeof(fileBuff), "%s/%s/%s_%s", LOCAL_ERL_FILE_PATH,
					ftcInfo.ftcIdentifier1,ftcInfo.ftcIdentifier1, ftcInfo.ftcIdentifier2 );
			unlink(fileBuff);
			SetGetFaceReqTmpltState(GET_FACE_SUCCESS_FTC);
		}break;

		case FTC_IDENTIFIER_TYPE_GET_USER_FACE_IMAGE:
		{
			CHAR fileBuff[512];
			DPRINT(ACMS_LOG,"Face Image FTC Done");
			SuccessfullyGetImageTmpltFtcDoneF = true;
			snprintf(fileBuff,sizeof(fileBuff), "%s/%s/%s_%s.jpg", LOCAL_ERL_FILE_PATH,
						ftcInfo.ftcIdentifier1,ftcInfo.ftcIdentifier1, ftcInfo.ftcIdentifier2 );
				unlink(fileBuff);
			SetGetFaceReqImageState(GET_FACE_SUCCESS_FTC);
		}break;

		case FTC_IDENTIFIER_TYPE_GET_EXCEP_FACE_IMG:
		{
			CHAR fileBuff[512];
			DPRINT(ACMS_LOG,"Face Image FTC Done for Exceptional Face");
			SuccessfullyGetImageTmpltFtcDoneF = true;
			snprintf(fileBuff,sizeof(fileBuff), "%s/%s.jpg", EXCEPTIONAL_FACE_FILE_PATH,
						ftcInfo.ftcIdentifier1);
				unlink(fileBuff);
			SetGetFaceReqImageState(GET_FACE_SUCCESS_FTC);
		}break;
		default:
			break;
	}

}
