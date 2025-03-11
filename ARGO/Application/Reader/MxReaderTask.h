#if !defined MXREADERTASK_H
#define MXREADERTASK_H
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
//   File         : MxReaderTask.h
//   Description  : This file declares reader task related defines and API.
//
/////////////////////////////////////////////////////////////////////////////


//******** Include files **********
#include "MxTypedef.h"
#include "MxSlaveCommonMsgList.h"
#include "MxCardReaderCommon.h"
#include "MxDelay.h"
#include "MxSysCommonDef.h"
#include "MxEnrollment.h"
#include "MxSysTimer.h"
#include "MxSlaveCommon.h"

//******** Defines and Data Types ****

#define MAX_LEN_READER_S_BUFF					30		// length of each small buffer
#define MAX_LEN_READER_M_BUFF					410		// length of each medium buffer
#define MAX_LEN_READER_L_BUFF					4000	// length of each Large buffer

#define MAX_READER_CMD_RETRY_COUNT				3		// number of total retry for a command to reader
#define MAX_READER_CMD_RETRY_CNT_DURING_INIT	2		// number of total retry for a command to reader
#define MAX_READER_CMD_RETRY_CNT_DURING_INIT_HID_SE_READER	4	// number of total retry for a command to reader
#define MAX_SM130_CMD_RETRY_CNT_DURING_INIT		5
#define MAX_READER_AUTO_DETECT_RETRY_CNT_DURING_INIT		2
#define MAX_CMD_RETRY_CNT_DURING_EV				0		// number of retry for a command once failure is detected
#define HID_MAX_CMD_RETRY_CNT_DURING_EV			1		// number of retry for a command once failure is detected
#define HID_SE_MAX_CMD_RETRY_CNT_DURING_EV		0		// number of retry for a command once failure is detected
#define HID_RESUME_AUTONOMOUS_MODE_RETRY_CNT	5


#define IDLE_FP_SCAN_TIMER						0x30		// finger scan timeout value for idle mode - infinite
#define DEGRADE_FP_SCAN_TIMER					0x34		// finger scan timeout value for degrade mode - 4 seconds
#define ENROLLMENT_TIMER						30			// in seconds
#define BLE_ENROLLMENT_TIMER					60			// in seconds

#define WAIT_READER_RESET_TIME					100
#define WAIT_READER_RESP_IN_INIT_TIME			700

#define FPR_FW_UPGRADE_TIMEOUT					(TIMER_100MS * 30) // 3 seconds
#define SFPR_FW_DATA_PACKET_SIZE				3900
#define MAX_AUTO_DETECT_READER_TYPE				11

#define VALUE_VER2								2

#define GetReaderStatus(readerId)				(readerStatus[readerId])
#define SCAN_TEMP_COUNT							(TIMER_100MS * 10)	//in 100 ms

#define DESFIRE_CARD_VER		1
#define	DESFIRE_APPL_ID_SIZE	3
#define	DESFIRE_OFFSET_SIZE		3
#define	DESFIRE_LENGTH_SIZE		3

#define	SUPREMA_6030_FP_RDR_PRODUCT_ID			"a4a7"
#define	SUPREMA_6030_FP_RDR_VENDOR_ID			"16d1"

#define	SUPREMA_6030_FP_RDR_PRODUCT_ID_FILE		"/sys/bus/usb/devices/1-1/idProduct"
#define	SUPREMA_6030_FP_RDR_VENDOR_ID_FILE		"/sys/bus/usb/devices/1-1/idVendor"


typedef enum
{
	READER_CMD_FAILED,			//invalid command or parameter
	READER_CMD_SUCCESS			//an acknowledge that reader driver has started command processing
}READER_CMD_PROC_STATUS_e;

typedef enum
{
	NO_READER_RESP,				//response has not detected yet
	READER_RESP_FAILED,			//invalid response
	READER_RESP_NO_DATA,		//response received but should not be send to MC
	READER_RESP_WITH_DATA		//response received, need to send to MC
}READER_RESP_PROC_STATUS_e;

typedef enum
{
	NO_READER_EVENT,			//event has not been detected yet or invalid event was received and ignored
	READER_EVENT_DATA,			//event received
	READER_EVENT_PROCESSING		//event received and it's communicating with reader to get more info
}READER_EV_PROC_STATUS_e;

typedef enum
{
	READER_CMD_NOT_RETRY,		// command is being send to reader first time
	READER_CMD_RETRY			// resending command to reader
}READER_CMD_RETRY_STATUS_e;

enum
{
	READER_IS_INACTIVE,			// reader is not communicating with DC
	READER_IS_ACTIVE,			// reader is communicating OK with DC
	READER_STATUS_UNKNOWN,		// not known
	READER_NOT_CONFIGURED,		// reader configuration NONE
	READER_STATUS_PROGRESS,		// reader initialization process is under progress
	MAX_READER_STATUS,
};
typedef UINT8	READER_STATUS_e;

typedef enum
{
	READER_BUFF_FREE,			// buffer is free
	READER_BUFF_ALLOCATED		// buffer is alloacted
}READER_BUFF_STATUS_e;

typedef enum
{
	MIFARE_U_READER_SM130,
	MIFARE_U_READER_PN532,
	MIFARE_U_READER_CLR663,
	MAX_MIFARE_U_READER_TYPE,
}MIFARE_U_READER_TYPE_e;

typedef enum
{
	HID_READER_OEM_150,
	HID_READER_SE_32X0,
	MAX_HID_READER_TYPE,
}HID_ICLASS_READER_TYPE_e;

//Structure describing message information
typedef struct
{
	UINT8PTR	dataPtr; 		//points where data is stored
	UINT16		msgId; 			//message Id defined in system
	UINT16		length;	 		//length of data
}READER_MSG_INFO_t;

//Structure describing reader buffer of small size, used for payload information
typedef struct
{
	READER_BUFF_STATUS_e	status;
	UINT8	data[MAX_LEN_READER_S_BUFF];//data space for buffer
}READER_SMALL_BUFF_t;

//Structure describing reader buffer of medium size, used for payload information
typedef struct
{
	READER_BUFF_STATUS_e	status;
	UINT8	data[MAX_LEN_READER_M_BUFF];//data space for buffer
}READER_MEDIUM_BUFF_t;

//Structure describing reader buffer of large size, used for payload information
typedef struct
{
	READER_BUFF_STATUS_e	status;
	UINT8	data[MAX_LEN_READER_L_BUFF];//data space for buffer
}READER_LARGE_BUFF_t;

//Structure describing reader queue information
typedef struct
{
	READER_SMALL_BUFF_t	* 	cmdBuffPtr;		// points to command data
	READER_SMALL_BUFF_t	*	respBuffPtr;	// points to response data
	UINT8					status;			// queue status (free, pending, processing)
	UINT16					msgId;			// message Id of command/response/event
	UINT16					length;			// length of command/response data
}READER_Q_INFO_t;

// Related to reader event activity
typedef struct
{
	UINT32				markTime;
	UINT32				delayTime;
	UINT8				delayF;
	UINT8				msgRetryCnt;
}READER_EV_ACTIVITY_t;

// Structure describing reader activity information
typedef struct
{
	READER_EV_ACTIVITY_t	ev;
	VOIDPTR 				cmdDataPtr;
	VOIDPTR 				respDataPtr;
	SLAVE_MSG_ID_e			currMsgId;

	VALUE_FIELD_OPER_STATE_e	vFOperState;

	CARD_TYPE_e				cardType;
	UINT8					pgNo;				//page no. of card to process
	UINT8					currBlkNo;			//current block no. that is being read/write
	UINT8					currRWLen;			//length of data that is to be read/write for current block
	UINT16					remainRWLen;		//remaining length of data that is to be read/write from/into card
	UINT8					currKeyLoc;			//location in reader EEPROM for current key
	VOIDPTR					dataAddr;			//points to data which is to be writtn into card 
	
	BOOL					readSecInfoF;
	BOOL					readPersInfoF;
	BOOL					readFpInfoF;
	BOOL					readSsaInfoF;


	// card field info-  finger template related flags
	BOOL 					template1F;
	BOOL 					template2F;

	// card field info-  value related flags
	BOOL					valueF;
	BOOL					backupValueF;

	// card field info-  security parameters related flags
	BOOL 					facCodeF;
	BOOL 					addlSecCodeF;
	BOOL 					userIdF;

	// card field info-  user personal info related flags
	BOOL 					userNameF;
	BOOL 					designationF;
	BOOL 					branchF;
	BOOL 					departmentF;
	BOOL					bloodGrpF;
	BOOL					emrgncyContactF;
	BOOL			 		medicalHistoryF;
	
	// card field info- smart secure info related flags
	BOOL					alphaNumIdF;
	BOOL					accessLevelF;
	BOOL					bypassFingerF;
	BOOL					vipF;
	BOOL					validityF;
	BOOL					pinF;
	BOOL					accessRouteF;
	BOOL					smartAccessRouteLevelF;

	BOOL			 		fieldWrStatusF;
	BOOL					getFieldInfoF;

	BOOL					readingSSADebugFlag;
	BOOL 					bypassPalmF;
	
	//hid reader key
	UINT8					hidRdrKey[HID_ICLASS_KEY_SIZE];

	// card field smart card based route access related flag
	BOOL					doorLevelF;
	BOOL					doorModeF;

	UINT8 					fpIdx;
	UINT32 					userId;

	UINT64					cardId;
	UINT8					cardIdLength;
	ENROLL_SOURCE_e			fpEnrollSource;
	FP_DELETE_METHOD_e 		deleteMethod;
	UINT8					dataBlock[MIFARE_SMART_CARD_BLOCK_SIZE];
	BOOL					fpBeingProcessedF;
	UINT8					currStartByte;				//start byte for custom smart card field
	CARD_FIELD_e			currCardFieldIdx;				//currtently read write opertion for given field is processing
	BOOL					cardIdF;
	UINT8					seekTagRetryCnt;
	BOOL					ignoreDetectWaitF;
	BOOL					cardWriteInProgressF;
	BOOL					freeScanRequireF;
	UINT8					desFireCurrApplId[DESFIRE_APPL_ID_SIZE];
	UINT8					desFireCurrLength[DESFIRE_LENGTH_SIZE];
	UINT8					desFireCurrOffset[DESFIRE_OFFSET_SIZE];
	UINT8					desFireCurrFileNo;
	UINT8					desFireCurrFileSize[DESFIRE_LENGTH_SIZE];
	BOOL 					palmTemplateF;
}READER_ACTIVITY_INFO_t; 

typedef struct __attribute__((aligned(4), packed))
{
	// card field info-  security parameters related flags
	BOOL 					facCodeF;
	BOOL 					addlSecCodeF;
	BOOL 					userIdF;

	// card field info-  user personal info related flags
	BOOL 					userNameF;
	BOOL 					designationF;
	BOOL 					branchF;
	BOOL 					departmentF;
	BOOL					bloodGrpF;
	BOOL					emrgncyContactF;
	BOOL			 		medicalHistoryF;
	
	// card field info- smart secure inf related flags
	BOOL					alphaNumIdF;
	BOOL					accessLevelF;
	BOOL					bypassFingerF;	
	BOOL					vipF;
	BOOL					validityF;
	BOOL					pinF;
	BOOL					accessRouteF;
	BOOL					smartAccessRouteLevelF;
	BOOL					bypassPalmF;
}TEMP_RW_FIELD_FLAG_t;

typedef enum
{
	MATRIX_SMART_CARD_KEY,
	CUSTOM_SMART_CARD_KEY,
	MAX_SMART_CARD_KEY_TYPE,
}SMART_CARD_KEY_TYPE_e;

typedef struct
{
	UINT8					oldKey[8];
	SMART_CARD_KEY_TYPE_e	oldKeyType;
	SMART_CARD_KEY_TYPE_e	newKeyType;
	BOOL					smartCardKeyChangeF;
	BOOL					reqRecFromAPI;
	TIMER_HANDLE			keyChngWaitTmrHandle;
	UINT8					keyChngResp;
	UINT64					cardId;
	CARD_TYPE_e				cardType;
}SMART_CARD_KEY_UPDATE_t;

// Reader Auto Detection status
typedef enum
{
	RDR_DETECT_SUCCESS,			// reader is not communicating with DC
	RDR_DETECT_FAIL,		// not known
	RDR_DETECT_PROGRESS,			// reader is communicating OK with DC
	MAX_RDR_DETECT
}RDR_DETECT_STATUS_e;

// Reader initialization status (it also used during auto detection process)
typedef enum
{
	READER_INIT_IDLE,
	READER_INIT_WAIT1,
	READER_INIT_WAIT2,
	READER_INIT_CMD,
	READER_INIT_RESP,
	READER_INIT_COMPLETE,
	READER_CANCEL_PREV_CMD,
	READER_CANCEL_CMD_RESP,
	READER_PROTOCOL_MODE,
	READER_PROTOCOL_MODE_RESP,
	MAX_READER_INIT,
}READER_INIT_STATE_e;

typedef struct
{
	UINT32						timeTick[MAX_READERS];
	READER_INIT_STATE_e			state[MAX_READERS];
	UINT8						retryCnt[MAX_READERS];
	UINT32						waitTime[MAX_READERS];
}READER_INIT_PROC_INFO_t;

typedef struct
{
	BOOL						autoDetectF;
	BOOL						comboCardDetectF[COMBO_READER_MAX];
	BOOL						comboBlDetctedF[COMBO_READER_MAX];
	RDR_DETECT_STATUS_e			rdrStatus[MAX_READERS];
	UINT8						rdrTypeIdx[MAX_READERS];
	READER_TYPE_e				readerType[MAX_READERS];
}READER_AUTO_DETECT_PROC_INFO_t;

typedef enum
{
	RDH_IDLE_STATE,				// to process event and pending commands
	RDH_WAIT_FOR_RESP_STATE,		// to process response
	RDH_RDR_BG_INIT_INFO,
	RDH_RDR_BG_INIT_PROCESS,
	RDH_STATE_MAX,
}RDH_STATES_e;

//******** Function Prototypes *******
void SetRDHState(READER_ID_e iReaderId, RDH_STATES_e iState);
RDH_STATES_e GetRDHState(READER_ID_e iReaderId);
BOOL ProcessReaderCommand(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
void ReaderDistributionHandler(void);
void InitReaderTask(void);
BOOL SetReaderRespTimer(UINT8 readerId, UINT16 timerValue);
BOOL AllocateReaderBuff(UINT16 length, READER_SMALL_BUFF_t ** buffPtr);
READER_SMALL_BUFF_t * GetReaderRespBuffPtr(UINT8 readerId);
BOOL SetReaderRespBuffPtr(UINT8 readerId, READER_SMALL_BUFF_t * respBuffPtr);
VOIDPTR GetReaderEvBuffPtr(UINT8 readerId);
READER_Q_INFO_t GetReaderOutMsg(UINT8 readerId);
BOOL SetReaderWaitCmdTimer(UINT8 readerId, UINT16 timerValue);
BOOL ReadSfprBinFile(UINT32 address, UINT8PTR data, UINT16 size);
BOOL FillAutoDetectReaderData(CHARPTR rxData);
void AutoReaderDetectionTask(void);
void SupremaFingerReaderCommProtocolCheck(void);
void StartReaderInitProcess(UINT8 readerId);
READER_TYPE_e GetAutoDetectedReaderType(UINT8	readerId);
void InitReaderRelatedInfo(READER_ID_e readerId);
BOOL CheckNextMifareReader(UINT8 readerId, BOOL resetF);
BOOL CheckNextHidIclassReader(UINT8 readerId, BOOL resetF);
BOOL LoadScanTemplateTimer(UINT8 readerId, UINT16 timerValue);
BOOL CheckAnyFpReaderCfg(RESPONSE_PORT_e respPort, READER_ID_e * readerIdPtr);
void InitReaderRelatedVars(void);
// On startup, it check internal FP reader USB node is detected or not
BOOL Init_Internal_FP_Rdr_UsbNode_Detection();
// to differentiate whether variant has Internal finger print reader or not
BOOL IsVariant_With_Internal_FP_Rdr();
void SetIsVariant_With_Internal_FP_Rdr(BOOL Val);
READER_TYPE_e GetAutoDetectReader(UINT8 auto_detect_reader_type, READER_ID_e reader);

#endif  // MXREADERTASK_H

