/////////////////////////////////////////////////////////////////////////////
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
//   Created By   : Kunjal Garach
//   File         : MxCommTask.c
//   Description  : This file implements API layer of communication task
//					in Panel door.
//
/////////////////////////////////////////////////////////////////////////////

//*** Place all include files here ***
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

#include "MxTypedef.h"
#include "MxCommTask.h"
#include "MxSysTimer.h"
#include "MxReaderTask.h"
#include "MxSlaveMsg.h"
#include "MxConfig.h"
#include "MxGpio.h"
#include "MxLogs.h"
#include "MxUdpPing.h"
#include "MxPDDcTask.h"
#include "MxDcTask.h"
#include "MxReaderUtils.h"
#include "MxMsgHandler.h"
#include "MxDcFileSyncTask.h"
#include "MxCerTask.h"
#include "AesCbcCrypto.h"
#include "MxTempUserTask.h"

//******** Extern Variables **********
extern SYS_BASIC_PARA_t	  			SysBasicPara;
extern BOOL							ipUpdateReqF;			// Need to Update IP related Flag
extern DOOR_ACTIVITY_INFO_t			DcInfo;
extern CHARPTR						InterfaceStr[MAX_PORT];
extern SERVER_PARA_t 				ServerParameters;
extern ETHERNET_PARA_t		  		EthernetParameters;
extern WIFI_PARA_t					WifiParameters;
extern const unsigned char sAesCbcCnfgKey256[AES_256_KEY_SIZE];
extern SYSTEM_INFORMATION_t         systemInformation;

//****** Defines and Data Types ******

#define MAX_COM_Q_SIZE				15
#define MAX_COM_S_BUF				8
#define SIZE_OF_COM_S_BUF			64
#define MAX_COM_M_BUF				4
#define SIZE_OF_COM_M_BUF			512
#define MAX_COM_L_BUF				1
#define SIZE_OF_COM_L_BUF			4096

#define	HB_TMR_CNT					10		//1000 ms
#define	MAX_HB_CNT					3		//3 Counts

#define WANDER_TMR_CNT				10	    // 1000ms
#define MAX_WANDER_CNT				7		// 7 Secs
#define	WAIT_ACK_TMR_CNT			5		// 500 mSec
#define	MAX_RETRY_CNT				3

#define	LOCAL_UDP_PORT				2000	// Slave's UDP listen port
#define	REMOTE_UDP_PORT				2002	// Master's UDP listen port

#define	LOCAL_SECURE_UDP_PORT				2004	// Slave's UDP listen port
#define	REMOTE_SECURE_UDP_PORT				2006	// Master's UDP listen port

#define SYS_HW_VER_SIZE		       	1
#define SYS_SW_VER_SIZE	           	2
#define DOOR_TYPE_SIZE		       	2

#define UDP_SOCKET_REOPEN_WAIT_COUNT 3

typedef struct
{
	OG_MSG_DESC_t		ogMsgDesc[MAX_COM_Q_SIZE];
	UINT8				size;			// Size of message Q
	UINT8				iptr;			// In pointer of message Q
	UINT8				optr; 			// Out pointer of message Q
}OG_MSG_QUEUE_t;

typedef struct
{
	IC_MSG_DESC_t		icMsgDesc[MAX_COM_Q_SIZE];
	UINT8				size;			// Size of message Q
	UINT8				iptr;			// In pointer of message Q
	UINT8				optr; 			// Out pointer of message Q
}IC_MSG_QUEUE_t;

typedef struct
{
	UINT32	status;
	UINT8	buf[SIZE_OF_COM_S_BUF];
}COM_SMALL_BUF_t;

typedef struct
{
	UINT32	status;
	UINT8	buf[SIZE_OF_COM_M_BUF];
}COM_MEDIUM_BUF_t;

typedef struct
{
	UINT32	status;
	UINT8	buf[SIZE_OF_COM_L_BUF];
}COM_LARGE_BUF_t;

//******** Function Prototypes *******

BOOL UpdateCommBufStatus(VOIDPTR dataPtr, BOOL status);

static void InitCommHB(void);
static void StartHbTimer(void);
static void HbTimeout(UINT32 data);
static void StartDegradeTimer(void);
static void DegradeTimeout(UINT32 data);

static void RunEthernetTask(void);
static void InitCommHS(void);
static void WanderTimeout(UINT32 data);
static void StartWanderTimer(void);
static void SendAckNackMsg(SLAVE_MSG_ID_e msgId, MATRIX_HDR_t * matHdr);
static void WaitAckTimeout(UINT32 data);
static void UpdateToSendNewMsg(void);
static void StartWaitAckTimer(void);
static void CloseUdpSocket(void);
static void CloseUdpSecureSocket(void);

//******** Global Variables **********
BOOL			HbRcvdF;
COMM_MODE_e		CommMode;
COMM_STATE_e	CommState;
UINT8			LogicSlaveId;
PANEL_INFO_t    	panelInfo;
//******** Static Variables **********
static UINT8			hbCnt;
static UINT8			msgRetryCnt;			// Current status of retry count
static UINT8			msgSeqNum;				// Last sequence number used (0-63)
static BOOL				waitAckF;				// Flag indicating waiting for ACK of last message
static UINT8			wanderCnt[MAX_SEQ_NUM];	// Wandering counts
static UINT8			broadcastMsgSeqNum;		// Broadcast Message sequence number used (0-63)
static UINT8			broadcastWanderCnt[MAX_SEQ_NUM];	// Broadcast message wandering counts

static TIMER_HANDLE		hbTmrHandle = INVALID_TIMER_HANDLE;	//Heartbeat timer handle
static TIMER_HANDLE		wnTmrHandle = INVALID_TIMER_HANDLE;	//Wandering timer handle
static TIMER_HANDLE		waTmrHandle = INVALID_TIMER_HANDLE;	//Wait Ack timer handle

static COM_SMALL_BUF_t	comSmallBuf[MAX_COM_S_BUF];
static COM_MEDIUM_BUF_t	comMediumBuf[MAX_COM_M_BUF];
static COM_LARGE_BUF_t	comLargeBuf[MAX_COM_L_BUF];

static OG_MSG_QUEUE_t	ogMsgQueue;
static IC_MSG_QUEUE_t	icMsgQueue;

static UDP_SOCKET		commUdpSocket = -1;
static UDP_SOCKET		commSecureUdpSocket = -1;
static BOOL 			bPowerStatusF;

static FIRMWARE_UPGRATE_STATE_e		firmwareUpgradeStat = FIRMWARE_UPGRADE_IDLE;

//******** Function Definations ******

//*****************************************************************************
//	SendMsg()
//	Param:
//		IN :	msgId		- message Id of message to be sent
//				length		- length of data payload
//				*dataPtr	- points to data payload to be sent
//		OUT:	NONE
//	Returns:	SUCCESS/FAIL - SUCCESS if successfully queued message to be sent
//	Description:
//				Application uses this API to send event or response to master.
//
//*****************************************************************************
BOOL SendMsg(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length)
{
	BOOL			status;
	UINT8PTR 		tempPtr = NULL;
	OG_MSG_DESC_t	ogMsg;

	if(length > 0)
	{
		// Return pointer of a buffer which stores send message data.
		tempPtr = GetFreeBuf(length);

		if(tempPtr == NULL)
			return(FAIL);
	}

	ogMsg.msgId = msgId;
	ogMsg.payLength = length;
	ogMsg.payLoadPtr = (VOIDPTR)tempPtr;

	status = PutCommOutQ(ogMsg);

	if( (status == SUCCESS) && (tempPtr != NULL) )
	{
		if(dataPtr != NULL)
		{
			// We copy data into buffer and mark buffer is in use
			memcpy((VOIDPTR)tempPtr, dataPtr, length);
	 		// update buffer status
			UpdateCommBufStatus(tempPtr, BUSY);
		}
	}
	return(status);
}

//*****************************************************************************
//	GetMsgNoCopy()
//	Param:
//		IN :	NONE
//		OUT:	*msgId		- Message Id of next message in Q
//				*length		- length of message payload in Q
//	Returns:	VOIDPTR		- Pointer of Comm Buffer where message payload is stored
//	Description:
//				Application uses this API to get next message from remote.
//				The message data will remain in communication task buffer and
//				only the pointer to that buffer will be returned.
//
//	NOTE:		APPLICATION MUST FREE PROVIDED COMM BUFFER USING FreeCommBuffer( )
//*****************************************************************************
MSG_RECV_STATUS_e GetMsgNoCopy(SLAVE_MSG_ID_e * msgId, UINT16PTR length, VOIDPTR * dataPtr)
{
	IC_MSG_DESC_t	icMsg;

	if(GetCommInQ(&icMsg) == SUCCESS)
	{
		// Output Message ID get from Q.
		*msgId = icMsg.msgId;
		// Output Length of message
		*length = icMsg.payLength;
		// Output Data pointer
		*dataPtr = icMsg.payLoadPtr;
		// Update IC Queue Optr.
		UpdateCommInQ(FALSE);
		// Return payload pointer.
		return(MSG_RECIEVED);
	}
	else
	// No message is there in Q to read then return 0.
	{
		return(NO_MSG_IN_Q);
	}
}

//*****************************************************************************
//	GetFreeBuf()
//	Param:
//		IN :	bufSize  - Required buffer size
//		OUT:	NONE
//	Returns:	UINT8PTR - Pointer to allocated buffer.
//						   Null if buffer not available
//	Description:
//				This function is used to allocated free buffer according to
//				length pass. If we cannot allocate buffer of size specified,
//				we will try bigger size	buffer.
//*****************************************************************************
UINT8PTR GetFreeBuf(UINT16 bufSize)
{
	UINT8 cnt;

	// Compare buffer size with small buffer size.
	if(bufSize <= SIZE_OF_COM_S_BUF)
	{
		// Search for free buffer up to maximum number.
		for(cnt = 0; cnt < MAX_COM_S_BUF; cnt++)
		{
			// Check if buffer is free.
			if(comSmallBuf[cnt].status == FREE)
			{
				// First location is status and if it is free return that pointer.
				return(&comSmallBuf[cnt].buf[0]);
			}
		}
	}
	// Compare buffer size with medium buffer size.
	if(bufSize <= SIZE_OF_COM_M_BUF)
	{
		// Search for free buffer up to maximum number.
		for(cnt = 0; cnt < MAX_COM_M_BUF; cnt++)
		{
			// Check if buffer is free.
			// First location is status and if it is free return that pointer.
			if(comMediumBuf[cnt].status == FREE)
			{
				return(&comMediumBuf[cnt].buf[0]);
			}
		}
	}
	// Compare buffer size with small buffer size.
	if(bufSize <= SIZE_OF_COM_L_BUF)
	{
		// Check if buffer is free.
		// First location is status and if it is free return that pointer.
		for(cnt = 0; cnt < MAX_COM_L_BUF; cnt++)
		{
			if(comLargeBuf[cnt].status == FREE)
			{
				return(&comLargeBuf[cnt].buf[0]);
			}
		}
	}
	// Cannot find any buffer free then return NULL.
	return(NULL);
}

//*****************************************************************************
//	UpdateCommBufStatus()
//	Param:
//		IN :	VOIDPTR	// Pointer of Comm buffer to free
//		OUT:	NONE
//	Returns:	(SUCCESS/FAIL)
//	Description:
//				This Function is use to change status of buffer.
//
//*****************************************************************************
BOOL UpdateCommBufStatus(VOIDPTR dataPtr, BOOL status)
{
	UINT8 cnt;

	// Search for buffer up to maximum number.
	for(cnt = 0; cnt < MAX_COM_S_BUF; cnt++)
	{
		// Find buffer address matches with buffer address pass.
		if((VOIDPTR)&comSmallBuf[cnt].buf == dataPtr)
		{
			// Make status FREE
			comSmallBuf[cnt].status = status;
			return(SUCCESS);
		}
	}

	// Search for buffer up to maximum number.
	for(cnt = 0; cnt < MAX_COM_M_BUF; cnt++)
	{
		// Find buffer address matches with buffer address pass.
		if((VOIDPTR)&comMediumBuf[cnt].buf == dataPtr)
		{
			// Make status FREE
			comMediumBuf[cnt].status = status;
			return(SUCCESS);
		}
	}

	// Search for buffer up to maximum number.
	for(cnt = 0; cnt < MAX_COM_L_BUF; cnt++)
	{
		// Find buffer address matches with buffer address pass.
		if((VOIDPTR)&comLargeBuf[cnt].buf == dataPtr)
		{
			// Make status FREE
			comLargeBuf[cnt].status = status;
			return(SUCCESS);
		}
	}
	return(FAIL);
}

//*****************************************************************************
//	FreeCommBuffer()
//	Param:
//		IN :	VOIDPTR	// Pointer of Comm buffer to free
//		OUT:	NONE
//	Returns:	(SUCCESS/FAIL)
//	Description:
//				This Function is use to change status of buffer.
//
//*****************************************************************************
BOOL FreeCommBuffer(VOIDPTR dataPtr)
{
	return(UpdateCommBufStatus(dataPtr, FREE));
}

//*****************************************************************************
//	InitCommTask()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will initialize Communication task.
//				All queue and buffers freed. Heartbeat and Handshaking logic
//				is intialised
//	NOTE:		This function must be called before using any comm task API
//*****************************************************************************
void InitCommTask(void)
{
	UINT8 cnt;

	// NOTE Sync with V2 Paneldoor
	bPowerStatusF = TRUE;

	// Make free all small buffers.
	for(cnt = 0; cnt < MAX_COM_S_BUF; cnt++)
	{
		// First location is status so make it FREE
		comSmallBuf[cnt].status = FREE;
	}
	// Make free all medium buffers.
	for(cnt = 0; cnt < MAX_COM_M_BUF; cnt++)
	{
		// First location is status so make it FREE
		comMediumBuf[cnt].status = FREE;
	}
	// Make free all large buffers.
	for(cnt = 0; cnt < MAX_COM_L_BUF; cnt++)
	{
		// First location is status so make it FREE
		comLargeBuf[cnt].status = FREE;
	}

	// Make InPointer and OutPointer 0.
	ogMsgQueue.iptr = 0;
	ogMsgQueue.optr = 0;
	ogMsgQueue.size = MAX_COM_Q_SIZE;

	// Make InPointer and OutPointer 0.
	icMsgQueue.iptr = 0;
	icMsgQueue.optr = 0;
	icMsgQueue.size = MAX_COM_Q_SIZE;

	// Initialize Heart beat and Handshaking layers
	InitCommHB();
	InitCommHS();
}

//*****************************************************************************
//	PutCommOutQ()
//	Param:
//		IN :	ogMsg	- msg description to be placed in Out Q.
//		OUT:	NONE
//	Returns:	(SUCCESS/FAIL)
//	Description:
//				This function is used to put message in Out message queue.
//				Data payload is not copied to message buffer in this function.
//*****************************************************************************
BOOL PutCommOutQ(OG_MSG_DESC_t ogMsg)
{
	UINT8	tmpIptr;

	// Take one temp pointer and take iptr + 1 value
	tmpIptr = ogMsgQueue.iptr + 1;

	// Compare with size of queue and if it is greater or equal to size then make tmpIptr to 0
	if(tmpIptr >= ogMsgQueue.size)
		tmpIptr = 0;

	// compare tmpIptr and optr and if it is equal then queue is full so return with FALSE.
	if(tmpIptr == ogMsgQueue.optr)
		return(FAIL);

	// Copy data.
	memcpy((VOIDPTR)&ogMsgQueue.ogMsgDesc[tmpIptr], (VOIDPTR)&ogMsg, sizeof(OG_MSG_DESC_t));

	// Save tmpIptr to actual iptr.
	ogMsgQueue.iptr = tmpIptr;

	return(SUCCESS);
}

//*****************************************************************************
//	GetCommOutQ()
//	Param:
//		IN :	NONE
//		OUT:	ogMsg	-  pointer to msg description
//	Returns:	(SUCCESS/FAIL)
//	Description:
//				This function is used to get message from out message queue.
//				This function does not increment out pointer so caller needs
//				to call 'updateCommOutQ' function after processing message.
//*****************************************************************************
BOOL GetCommOutQ(OG_MSG_DESC_t *ogMsg)
{
	UINT8	tmpOptr;

	// Queue is empty so return.
	if(ogMsgQueue.optr == ogMsgQueue.iptr)
		return(FAIL);

	// Take optr + 1 value in one temp pointer.
	tmpOptr = ogMsgQueue.optr + 1;

	// Compare with size of queue and if it is greater or equal to size then make tmpIptr to 0
	if(tmpOptr >= ogMsgQueue.size)
		tmpOptr = 0;

	// Copy data.
	memcpy((VOIDPTR)ogMsg, (VOIDPTR)&ogMsgQueue.ogMsgDesc[tmpOptr], sizeof(OG_MSG_DESC_t));

	return(SUCCESS);
}

//*****************************************************************************
//	UpdateCommOutQ()
//	Param:
//		IN :	bufFreeF	- Flag indicating whether to free buffer
//		OUT:	NONE
//	Returns:	(SUCCESS/FAIL)
//	Description:
//				This function updates Outpointer. Caller must read message
//				before calling this function otherwise message will be lost.
//				BufFreeFlag is use to free buffer status or not.
//*****************************************************************************
BOOL UpdateCommOutQ(BOOL bufFreeF)
{
	UINT8	tmpOptr;

	// Queue is empty so return.
	if(ogMsgQueue.optr == ogMsgQueue.iptr)
		return(FAIL);

	// Take optr + 1 value in one temp pointer.
	tmpOptr = ogMsgQueue.optr + 1;

	// Compare with size of queue and if it is greater or equal to size then make tmpIptr to 0
	if(tmpOptr >= ogMsgQueue.size)
		tmpOptr = 0;

	if(bufFreeF == TRUE)
	{
		// Mark buffer free
		UpdateCommBufStatus(ogMsgQueue.ogMsgDesc[tmpOptr].payLoadPtr, FREE);
	}

	// Save tmpOptr to message queue Optr.
	ogMsgQueue.optr = tmpOptr;

	return(SUCCESS);
}

//*****************************************************************************
//	ClearCommOutQ()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function clears Queue by marking inpointer and outpointer 0.
//				It will free up all allocated buffer for Out Q before clearing Q.
//*****************************************************************************
void ClearCommOutQ(void)
{
	UINT8	tmpOptr, cnt = 0;

	do
	{
		// Queue is empty so return.
		if(ogMsgQueue.optr == ogMsgQueue.iptr)
			break;

		// Take optr + 1 value in one temp pointer.
		tmpOptr = ogMsgQueue.optr + 1;

		// Compare with size of queue and if it is greater or equal to size then make
		// tmpIptr to 0
		if(tmpOptr >= ogMsgQueue.size)
			tmpOptr = 0;

		// Mark buffer free
		UpdateCommBufStatus(ogMsgQueue.ogMsgDesc[tmpOptr].payLoadPtr, FREE);

		// Save tmpOptr to message queue Optr.
		ogMsgQueue.optr = tmpOptr;

		cnt++;

	}while(cnt <= MAX_COM_Q_SIZE);

	// Make InPointer and OutPointer 0.
	ogMsgQueue.iptr = 0;
	ogMsgQueue.optr = 0;
	// Size of queue is maximum number of message in buffer.
	ogMsgQueue.size = MAX_COM_Q_SIZE;
}

//*****************************************************************************
//	PutCommInQ()
//	Param:
//		IN :	icMsg	- msg description to be placed in In Q.
//		OUT:	NONE
//	Returns:	(SUCCESS/FAIL)
//	Description:
//				This function is used to put message in In message queue.
//
//*****************************************************************************
BOOL PutCommInQ(IC_MSG_DESC_t icMsg)
{
	UINT8	tmpIptr;

	// Take one temp pointer and take iptr + 1 value
	tmpIptr = icMsgQueue.iptr + 1;

	// Compare with size of queue and if it is greater or equal to size then make tmpIptr to 0
	if(tmpIptr >= icMsgQueue.size)
		tmpIptr = 0;

	// compare tmpIptr and optr and if it is equal then queue is full so return with FALSE.
	if(tmpIptr == icMsgQueue.optr)
		return(FAIL);

	// Copy data.
	memcpy((VOIDPTR)&icMsgQueue.icMsgDesc[tmpIptr], (VOIDPTR)&icMsg, sizeof(IC_MSG_DESC_t));

	// Save tmpIptr to actual iptr.
	icMsgQueue.iptr = tmpIptr;

	return(SUCCESS);
}

//*****************************************************************************
//	GetCommInQ()
//	Param:
//		IN :	NONE
//		OUT:	icMsg	- pointer to msg description
//	Returns:	(SUCCESS/FAIL)
//	Description:
//				This function is used to get message from In message queue.
//				This function does not increment out pointer so caller needs
//				to call 'updateCommInQ' function to read next message.
//*****************************************************************************
BOOL GetCommInQ(IC_MSG_DESC_t * icMsg)
{
	UINT8	tmpOptr;

	// Queue is empty so return.
	if(icMsgQueue.optr == icMsgQueue.iptr)
		return(FAIL);

	// Take optr + 1 value in one temp pointer.
	tmpOptr = icMsgQueue.optr + 1;

	// Compare with size of queue and if it is greater or equal to size then make tmpIptr to 0
	if(tmpOptr >= icMsgQueue.size)
		tmpOptr = 0;

	// Copy data.
	memcpy((VOIDPTR)icMsg, (VOIDPTR)&icMsgQueue.icMsgDesc[tmpOptr], sizeof(IC_MSG_DESC_t));

	return(SUCCESS);
}

//*****************************************************************************
//	UpdateCommInQ()
//	Param:
//		IN :	bufFreeF	- Flag indicating whether to free buffer
//		OUT:	NONE
//	Returns:	(SUCCESS/FAIL)
//	Description:
//				This function is used to update Outpointer. Application must read
//				message before calling this function otherwise message will be lost.
//*****************************************************************************
BOOL UpdateCommInQ(BOOL bufFreeF)
{
	UINT8	tmpOptr;

	// Queue is empty so return.
	if(icMsgQueue.optr == icMsgQueue.iptr)
		return(FAIL);

	// Take optr + 1 value in one temp pointer.
	tmpOptr = icMsgQueue.optr + 1;

	// Compare with size of queue and if it is greater or equal to size then make tmpIptr to 0
	if(tmpOptr >= icMsgQueue.size)
		tmpOptr = 0;

	if(bufFreeF == TRUE)
	{
		// Mark buffer free
		UpdateCommBufStatus(icMsgQueue.icMsgDesc[tmpOptr].payLoadPtr, FREE);
	}

	// Save tmpOptr to message queue Optr.
	icMsgQueue.optr = tmpOptr;

	return(SUCCESS);
}

//*****************************************************************************
//	ClearCommInQ()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function is used to clear InQueue so inpointer and
//				outpointer are set to 0. It will also free all allocated
//				buffers for In Q before clearing Q
//*****************************************************************************
void ClearCommInQ(void)
{
	UINT8	tmpOptr, cnt = 0;

	do
	{
		// Queue is empty so return.
		if(icMsgQueue.optr == icMsgQueue.iptr)
			break;

		// Take optr + 1 value in one temp pointer.
		tmpOptr = icMsgQueue.optr + 1;

		// Compare with size of queue and if it is greater or equal to size then make
		// tmpIptr to 0
		if(tmpOptr >= icMsgQueue.size)
			tmpOptr = 0;

		// Mark buffer free
		UpdateCommBufStatus(icMsgQueue.icMsgDesc[tmpOptr].payLoadPtr, FREE);

		// Save tmpOptr to message queue Optr.
		icMsgQueue.optr = tmpOptr;

		cnt++;

	}while(cnt <= MAX_COM_Q_SIZE);

	// Make InPointer and OutPointer 0.
	icMsgQueue.iptr = 0;
	icMsgQueue.optr = 0;
	// Size of queue is maximum number of message in buffer.
	icMsgQueue.size = MAX_COM_Q_SIZE;
}

//--------- Heartbeats --------------

//*****************************************************************************
//	InitCommHB()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will initialise all parameter related to
//				heartbeat logic
//*****************************************************************************
static void InitCommHB(void)
{
	HbRcvdF = FALSE;
	hbCnt = 0;

	// We start with OFFLINE
	CommState = SLAVE_OFFLINE;
	// Logical Slave Id
	LogicSlaveId = 0xFF;
	
	if(GetDoorApplType() == PANEL_DOOR_APPL)
	{
		// Start HB timer here since we are using it for Ethernet
		StartHbTimer();
		// Start Degrade timer
		StartDegradeTimer();
		// Set Idle Response
		UpdateDisplayStatusField();
		SetLed(ACCESS_OFFLINE_RESP , INT_EXT_PORT);
	}
	// Open UDP Ping Socket
	OpenUdpPingSocket();
}

//*****************************************************************************
//	StartHbTimer()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will start heartbeat cyclic timer for application
//*****************************************************************************
static void StartHbTimer(void)
{
	TIMER_INFO_t tInfo;

	tInfo.count = HB_TMR_CNT;
	tInfo.data = 0;
	tInfo.funcPtr = &HbTimeout;
	StartTimer(tInfo, &hbTmrHandle, __FUNCTION__);
}

//*****************************************************************************
//	HbTimeout()
//	Param:
//		IN :	data - Unused
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function is callback function of HB timer. HB timer is
//				cyclic timer and on expiry of this timer, this function is called.
//				This function will check if any heartbeat was received since last
//				check and update hbCnt and CommState.
//*****************************************************************************
static void HbTimeout(UNUSED_PARA UINT32 data)
{
	static UINT8		udpSocketReopenCnt = 0;

	// Check any heart beat received since last timeout
	if( (HbRcvdF == TRUE) && (IsCerFwInProgress() == NO) )
	{
		HbRcvdF = FALSE;

		if(hbCnt < MAX_HB_CNT)
		{
			DPRINT(COMM_LOG, "Keep Alive Received, hbCnt[%d]", hbCnt);
			hbCnt++;
		}

		if( (hbCnt >= MAX_HB_CNT) && (CommState != SLAVE_ONLINE) )
		{
			ChangeCommState(SLAVE_ONLINE, TRUE);
		}

	}
	else
	{
		if(hbCnt > 0)
		{
			DPRINT(COMM_LOG, "Keep Alive Not Received, hbCnt[%d]", hbCnt);
			hbCnt--;
		}
		if(hbCnt == 0)
		{
			if(CommState == SLAVE_ONLINE)
			{
				ChangeCommState(SLAVE_OFFLINE, TRUE);
			}

			udpSocketReopenCnt++;
			if(udpSocketReopenCnt > UDP_SOCKET_REOPEN_WAIT_COUNT)
			{
				udpSocketReopenCnt = 0;
                CloseUdpPingSocket(TRUE);
				OpenUdpPingSocket();
			}
		}
	}
}

//*****************************************************************************
//	ChangeCommState()
//	Param:
//		IN :	COMM_STATE_e
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will stop Degrade timer if running
//				NOTE : Added one more parameter to keep sync with other application.
//*****************************************************************************
void ChangeCommState(COMM_STATE_e commState, UNUSED_PARA BOOL switchIdlF)
{
	COMM_STATE_e prevCommState;
	CHAR * DeviceInterfaceMac = NULL;

	if( (firmwareUpgradeStat == FIRMWARE_UPGRADE_PROGRESS) || (CommState == commState) )
	{
		return;
	}

	prevCommState = CommState;
	CommState = commState;
	// Clear In and Out queue.
	ClearCommOutQ();
	ClearCommInQ();

	// Set Idle Response for Buzzer and LED
	SetLed(ACCESS_IDLE_RESP, INT_EXT_PORT);
	SetBuzzer(ACCESS_IDLE_RESP, INT_EXT_PORT);

	ChangeFingerPrintReaderSettings(CommState);

	UpdateDisplayStatusField();
	ResetFileReceiveServer();
	//Send Connectivity status to External Reader
	SendDoorConnectivityStatusToExternalReader();
	
	if(CommState == SLAVE_ONLINE)
	{
		DPRINT(COMM_LOG,"State changed to Online");

        if(systemInformation.UdpEncryptValidationData.isPreviousPingEncrypted == TRUE)
        {
            DPRINT(COMM_LOG,"Encryption enabled so starting encryption key creation");
            systemInformation.UdpEncryptValidationData.EncryptionF = TRUE;

            //close normal udp ping/command socket
            CloseUdpSocket();
            CloseUdpPingSocket(FALSE);

            //open Secure ping and command socket
            OpenUdpSecurePingSocket();
            OpenUdpSecureSocket();

            int iRet = GetDeviceEncryptedKey(systemInformation.UdpEncryptValidationData.panelMacAddress, sAesCbcCnfgKey256, systemInformation.UdpEncryptValidationData.DeviceEncryptionKey);
            if(iRet == FAIL)
            {
                systemInformation.UdpEncryptValidationData.EncryptionF = FALSE;
            }
        }


		//Stop Degrade Timer
		DeleteTimer(&DcInfo.dgTmrHandle);
		
		//In Door Webpage In LAN Setting When We press Submit IDEL Screen is not shows so that 
		//first we do the ideal screen then delete the timer 
		if(DcInfo.lcdIdleTmrHandle != INVALID_TIMER_HANDLE)
		{
		    DisplayIdleScreen(INVALID_TIMER_HANDLE);
		}
		
		ExitDegradeMode();
		// Clear Any Alarm and fault
		SetLed(ACCESS_ONLINE_RESP, INT_EXT_PORT);
		SetBuzzer(ACCESS_CLEAR_ALARM_RESP, INT_EXT_PORT);
		// Slave Online so Open UDP port
        if(systemInformation.UdpEncryptValidationData.EncryptionF == FALSE)
            OpenUdpSocket();
	}
	else if(CommState == SLAVE_OFFLINE)
	{
		DPRINT(COMM_LOG,"State changed to Offline");

        ResetUDPEncryptValidationData();
        //close secure udp ping/command socket
        CloseUdpSecureSocket();
        CloseUdpSecurePingSocket();

		//if door goes offline than it should accept ping from other panel also.
		ipUpdateReqF = TRUE;
		// Start Degrade Timer
		StartDegradeTimer();
		// Close Door
		DeactivateDoorRelay();
		// Clear Aux Relay Port
		DeactivateAuxRelay();

		// Set Resp Clear Response
		SetLed(ACCESS_OFFLINE_RESP, INT_EXT_PORT);
		SetBuzzer(ACCESS_CLEAR_ALARM_RESP, INT_EXT_PORT);

		// Update display mode as Idle only if device is in online state.
		if(prevCommState == SLAVE_ONLINE)
		{
			SetAudioVisual(USER_RESP_PD_COMMUNICATION_ERROR, INTERNAL_PORT, NULL);
		}
		SendMsgDeleteSingleTempUserFp();
		ResetTempUserAddInfo();
		// Slave Offline so Close UDP port
		CloseUdpSocket();

		// as door is offline now.
		// Make CommMode Unknown here and it is set when first Poll or ping receive.
		CommMode = COMM_MODE_UNKNOWN;
		SendDoorLockStatusToExternalReader(DOOR_LOCK_NORMAL);
	}
}


//*****************************************************************************
//	StartDegradeTimer()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will start Degrade timer. This function should
//				be called when DC goes offline.
//*****************************************************************************
static void StartDegradeTimer(void)
{
	TIMER_INFO_t tInfo;

	// We only need to start timer if Degrade Mode feature is enabled.
	if(SysBasicPara.degradeModeEnable == ENABLED)
	{
		if(DcInfo.dgTmrHandle == INVALID_TIMER_HANDLE)
		{
			tInfo.count = (SysBasicPara.degradeWaitTimer * 10);
			tInfo.data = 0;
			tInfo.funcPtr = &DegradeTimeout;
			StartTimer(tInfo, &DcInfo.dgTmrHandle, __FUNCTION__);
		}
		else
		{
			ReloadTimer(DcInfo.dgTmrHandle, (SysBasicPara.degradeWaitTimer * 10) );
		}
	}
}

//*****************************************************************************
//	DegradeTimeout()
//	Param:
//		IN :	data - Unused
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function is callback function of Degraded mode timer.
//				This timer was started when the slave goes Offline. If the
//				slave does not go back Online before expiry of this timer
//				slave is considered to be degraded mode.
//				This function declares slave to be in degraded mode.
//*****************************************************************************
void DegradeTimeout(UNUSED_PARA UINT32 data)
{
	DeleteTimer(&DcInfo.dgTmrHandle);		//Degrade timer handle
	// Declare that slave is now in Degraded mode
	CommState = SLAVE_DEGRADE;
	ChangeFingerPrintReaderSettings(SLAVE_DEGRADE);
	// Inidicate that we are now working in degraded mode.
	SetLed(ACCESS_DEGRADE_RESP , INT_EXT_PORT);
	SendDoorConnectivityStatusToExternalReader();
	UpdateDisplayStatusField();
}


//--------- Handshaking --------------

//*****************************************************************************
//	InitCommHS()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will initialise all parameter related to message
//				delivery handshaking logic.
//*****************************************************************************
static void InitCommHS(void)
{
	UINT8	cnt;

	msgRetryCnt = 0;
	msgSeqNum = 0;
	waitAckF = CLEAR;

	for(cnt = 0; cnt < MAX_SEQ_NUM; cnt++)
	{
		wanderCnt[cnt] = 0;
	}

	broadcastMsgSeqNum = 0;

	for(cnt = 0; cnt < MAX_SEQ_NUM; cnt++)
	{
		broadcastWanderCnt[cnt] = 0;
	}

	// Start Wander Timer
	StartWanderTimer();
}

//*****************************************************************************
//	OpenUdpSocket()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will open UDP socket and bind it to remote
//*****************************************************************************
void OpenUdpSocket(void)
{
	INT32				optVal  = 1;
	struct sockaddr_in	sockConf;	// Socket configuration
	BOOL				sockSuccessF = FALSE;
	struct ifreq 		if_idx;

	if(commUdpSocket != -1)
	{
		EPRINT(COMM_LOG, "UDP Socket already open");
		return;
	}

	// Open the UDP socket
	commUdpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(commUdpSocket < 0)
	{
		EPRINT(COMM_LOG, "Cannot Open UDP Socket");
		return;
	}

	// Must clear socket configuration structure first before sending
	memset((VOIDPTR)&sockConf, 0, sizeof(sockConf));

	sockConf.sin_family = AF_INET;
	sockConf.sin_port = htons(LOCAL_UDP_PORT);
	sockConf.sin_addr.s_addr = htonl(INADDR_ANY);

	memset(&if_idx, 0, sizeof(struct ifreq));
	//make sure that interface name size is less than 15 character(IFNAMSIZ).
	sprintf(if_idx.ifr_name, "%s", InterfaceStr[ServerParameters.interface] );

	if(ioctl(commUdpSocket, SIOCGIFINDEX, &if_idx) >= 0)
	{
		if( (setsockopt(commUdpSocket, SOL_SOCKET, SO_BINDTODEVICE, (void *)&if_idx, sizeof(if_idx) ) == 0) &&
				(setsockopt(commUdpSocket, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal) ) == 0) &&
				(setsockopt(commUdpSocket, SOL_SOCKET, SO_BROADCAST, &optVal, sizeof(optVal) ) == 0) )
		{
			sockSuccessF = TRUE;
		}

		fcntl(commUdpSocket, F_SETFD, FD_CLOEXEC);
	}

	if(sockSuccessF != TRUE)
	{
		EPRINT(COMM_LOG, "Cannot change Socket Properly");
		CloseUdpSocket();
	}
	else
	{
		if(bind(commUdpSocket, (struct sockaddr *)&sockConf, sizeof(struct sockaddr_in)) < 0)
		{
			EPRINT(COMM_LOG, "Cannot Bind UDP Socket");
			CloseUdpSocket();
		}
		else
		{
			DPRINT(SYS_LOG, "UDP Socket Open Successfully");
		}
	}
}

//*****************************************************************************
//	OpenUdpSecureSocket()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will open UDP socket and bind it to remote
//*****************************************************************************
void OpenUdpSecureSocket(void)
{
    INT32				optVal  = 1;
    struct sockaddr_in	sockConf;	// Socket configuration
    BOOL				sockSuccessF = FALSE;
    struct ifreq 		if_idx;

    if(commSecureUdpSocket != -1)
    {
        EPRINT(COMM_LOG, "UDP Socket already open");
        return;
    }

    // Open the UDP socket
    commSecureUdpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(commSecureUdpSocket < 0)
    {
        EPRINT(COMM_LOG, "Cannot Open UDP Socket");
        return;
    }

    // Must clear socket configuration structure first before sending
    memset((VOIDPTR)&sockConf, 0, sizeof(sockConf));

    sockConf.sin_family = AF_INET;
    sockConf.sin_port = htons(LOCAL_SECURE_UDP_PORT);
    sockConf.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(&if_idx, 0, sizeof(struct ifreq));
    //make sure that interface name size is less than 15 character(IFNAMSIZ).
    sprintf(if_idx.ifr_name, "%s", InterfaceStr[ServerParameters.interface] );

    if(ioctl(commSecureUdpSocket, SIOCGIFINDEX, &if_idx) >= 0)
    {
        if( (setsockopt(commSecureUdpSocket, SOL_SOCKET, SO_BINDTODEVICE, (void *)&if_idx, sizeof(if_idx) ) == 0) &&
                (setsockopt(commSecureUdpSocket, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal) ) == 0) &&
                (setsockopt(commSecureUdpSocket, SOL_SOCKET, SO_BROADCAST, &optVal, sizeof(optVal) ) == 0) )
        {
            sockSuccessF = TRUE;
        }

        fcntl(commSecureUdpSocket, F_SETFD, FD_CLOEXEC);
    }

    if(sockSuccessF != TRUE)
    {
        EPRINT(COMM_LOG, "Cannot change Socket Properly");
        CloseUdpSecureSocket();
    }
    else
    {
        if(bind(commSecureUdpSocket, (struct sockaddr *)&sockConf, sizeof(struct sockaddr_in)) < 0)
        {
            EPRINT(COMM_LOG, "Cannot Bind UDP Socket");
            CloseUdpSecureSocket();
        }
        else
        {
            DPRINT(SYS_LOG, "Secure UDP Socket Open Successfully");
        }
    }
}

//*****************************************************************************
//	CloseUdpSocket()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will close udp socket settings
//
//*****************************************************************************
static void CloseUdpSocket(void)
{
	if(commUdpSocket != -1)
	{
		close(commUdpSocket);
		commUdpSocket = -1;
	}
	DPRINT(COMM_LOG,"UDP Socket Closed");
}


//*****************************************************************************
//	CloseUdpSecureSocket()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will close udp socket settings
//
//*****************************************************************************
static void CloseUdpSecureSocket(void)
{
    if(commSecureUdpSocket != -1)
    {
        close(commSecureUdpSocket);
        commSecureUdpSocket = -1;
    }
    DPRINT(COMM_LOG,"UDP Secure Socket Closed");
}

//*****************************************************************************
//	WanderTimeout()
//	Param:
//		IN :	data  - Unused
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function is callback function of Wandering timer.
//				This function will decrement wander count is not zero
//
//*****************************************************************************
static void WanderTimeout(UNUSED_PARA UINT32 data)
{
	UINT8	cnt;

	for(cnt = 0; cnt < MAX_SEQ_NUM; cnt++)
	{
		if(wanderCnt[cnt] > 0)
		{
			wanderCnt[cnt]--;
		}

		if(broadcastWanderCnt[cnt] > 0)
		{
			broadcastWanderCnt[cnt]--;
		}
	}
}

//*****************************************************************************
//	StartWanderTimer()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will start Cyclic Wandering timer.
//				This timer will be started when commMode is set to Ethernet.
//				For each message received and acknowledged, a wandering count is set.
//				This count is decremented each time wandering timer expires unless it is clear (0).
//				If a message with same sequence number is received before this count is cleared,
//				message is ignored as it must be a duplicate message which could have wander
//				around in the network.
//*****************************************************************************
static void StartWanderTimer(void)
{
	TIMER_INFO_t tInfo;

	tInfo.count = WANDER_TMR_CNT;
	tInfo.data = 0;
	tInfo.funcPtr = &WanderTimeout;
	StartTimer(tInfo, &wnTmrHandle, __FUNCTION__);
}

//*****************************************************************************
//	RunCommTask()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function must be called from superloop to queue any
//				incoming message or send any pending outgoing message. Based
//				on the type of network it is connected, it will call
//				appropriate functions.
//*****************************************************************************
void RunCommTask(void)
{
	RunPingTask();
	RunEthernetTask();
}

//*****************************************************************************
//	RunEthernetTask()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function must will queue any incoming message or send any
//				pending outgoing message, if slave is connected on Ethernet.
//				It also implements handshaking functionality of MC-Slave
//				communication.
//				This function must be called periodically from super loop.
//				Each time, it will process maximum of one incoming and one
//				outgoing message. However, we only send an outgoing message
//				if we are not waiting for ACK of previously sent message.
//*****************************************************************************
static void RunEthernetTask(void)
{
	socklen_t			fromlen;
	struct sockaddr_in 	from, to;
	BOOL				tmpFlag;
	UINT8PTR 			tempPtr = NULL;
	INT16				msgLength = 0;
	UINT16  			countSend = 0;
	INT32				sendCnt;
	MATRIX_HDR_t		matrixHdr;
	IC_MSG_DESC_t		icMsg;
	OG_MSG_DESC_t		ogMsg;
	POWER_STATUS_t		powerStatus;
	UINT8				udpRecvBuff[MAX_UDP_MSG_LEN];
	UINT8				udpSendBuff[MAX_UDP_MSG_LEN];
    unsigned char * EncryptedData;
    CHAR				udpEncryptedBuf[MAX_UDP_MSG_LEN];
    CHAR						udpEncryptedRecvBuff[MAX_UDP_MSG_LEN];
    size_t decryptedLen = 0;
    unsigned char * DecriptedData;

	if(CommState != SLAVE_ONLINE)
	{
		return;
	}

	memset(udpRecvBuff, 0 , MAX_UDP_MSG_LEN);

	// First lets look at incoming message
	// Check any UDP packet available and get its size
	fromlen = sizeof(struct sockaddr_in);
    if(commUdpSocket != -1)
    {
        msgLength = recvfrom(commUdpSocket,udpRecvBuff, MAX_UDP_MSG_LEN,
                                 MSG_DONTWAIT, (struct sockaddr *)&from, &fromlen);
	 if(msgLength >= (INT16)sizeof(MATRIX_HDR_t))
	         DPRINT(COMM_LOG, "Normal DATA Communication on-going / DATA RECEIVED OF SIZE %d", msgLength);
    }
    else if((commSecureUdpSocket != -1) && (systemInformation.UdpEncryptValidationData.EncryptionF == TRUE))
    {
        msgLength = recvfrom(commSecureUdpSocket,udpEncryptedRecvBuff, MAX_UDP_MSG_LEN,
                                 MSG_DONTWAIT, (struct sockaddr *)&from, &fromlen);
        if(msgLength >= (INT16)sizeof(MATRIX_HDR_t))
        {
            DPRINT(COMM_LOG, "Encrypted DATA Communication on-going / DATA RECEIVED OF SIZE %d", msgLength);
            DecriptedData = (unsigned char *)&udpRecvBuff[0];
            decryptedLen = AesCbcDecrypt256(udpEncryptedRecvBuff, &DecriptedData, msgLength, TRUE, systemInformation.UdpEncryptValidationData.DeviceEncryptionKey);
            if(decryptedLen == FAIL)
            {
                EPRINT(COMM_LOG, "Could not DEcrypt DATA");
            }
            else
            {
                DPRINT(COMM_LOG, "DEcripted Len %ld", decryptedLen);
                msgLength=(int)decryptedLen;
            }
        }
    }
    if(msgLength != -1)
	{
		//Check if UDP packet is from Master IP
		if(strcmp(systemInformation.PanelIP , inet_ntoa(from.sin_addr)) == 0)
		{
			//Minimum matrix header need to be received before we can proceed.
            if(msgLength >= (INT16)sizeof(MATRIX_HDR_t))
			{

				// Get matrix header and subtract its length from overall msg size
				msgLength -= sizeof(MATRIX_HDR_t);
				memcpy((VOIDPTR)&matrixHdr, (VOIDPTR)udpRecvBuff, sizeof(MATRIX_HDR_t));

 				DPRINT(COMM_LOG,"UDP msg received [0x%x]", matrixHdr.msgId);
                msgLength = (INT16)matrixHdr.msgLength;
				// We only process message, if payload length match what is
				// specified in the header.
				// Check seq Number too it must be less then MAX_SEQ_NUM
				if( (msgLength == matrixHdr.msgLength) && (matrixHdr.seqId.seqNum < MAX_SEQ_NUM) )
				{
					// Check slave Id take message if it is our id or broadcast id
					if(matrixHdr.seqId.slaveId == LogicSlaveId)
					{
						// First we check if msg is ACK or NACK of previously sent message.
						if(matrixHdr.msgId == MSG_ACK)
						{
							if( (matrixHdr.seqId.seqNum == msgSeqNum) && (waitAckF == SET) )
							{
								// Clear wait ack flag to send new message
								waitAckF = CLEAR;
								// Stop Wait Ack Timer
								DeleteTimer(&waTmrHandle);
								//Update out queue to send new message.
								UpdateToSendNewMsg();
							}
						}
						else if(matrixHdr.msgId == MSG_NACK)
						{
							if( (matrixHdr.seqId.seqNum == msgSeqNum) && (waitAckF == SET) )
							{
								// Clear wait ack flag to send new message
								waitAckF = CLEAR;
								// Time out so increament retry count
								msgRetryCnt++;
								// Stop Wait Ack Timer
								DeleteTimer(&waTmrHandle);
								// Retry up to max count and then discard that message.
								if(msgRetryCnt >= MAX_RETRY_CNT)
								{
									UpdateToSendNewMsg();
								}
							}
						}
						//Send Power Status.
						else if( (matrixHdr.msgId == MSG_POWER_STATUS)
									&& (wanderCnt[matrixHdr.seqId.seqNum] == 0) )
						{
                            
							if( (0 < matrixHdr.msgLength) && (SW_REV >= 33) )
							{
								memcpy(&panelInfo, (VOIDPTR)&udpRecvBuff[sizeof(MATRIX_HDR_t)], sizeof(panelInfo) );
								DPRINT(COMM_LOG,"Panel Appl Mode value [%d] Panel ver [%s]",panelInfo.panelApplMode,panelInfo.panelVersion);
							}

							if(waitAckF == SET)
							{
								// Clear wait ack flag to send new message
								waitAckF = CLEAR;
								// Stop Wait Ack Timer
								DeleteTimer(&waTmrHandle);
							}

							// if Retry Count is not zero.
							if(msgRetryCnt > 0)
							{
								UpdateToSendNewMsg();
							}

							// First clear Out Queue
							ClearCommOutQ();

							powerStatus.status = bPowerStatusF;
							powerStatus.swVersion = DOOR_COMM_SW_VER;
							powerStatus.swRevision = SW_REV;
							powerStatus.swMinorRevision = SW_MINOR_REV;
							powerStatus.swType[0] = SYS_TYPE[0];
							powerStatus.swType[1] = SYS_TYPE[1];
							powerStatus.hwVersion = HW_VER;
							bPowerStatusF = FALSE;
							SendMsg(MSG_POWER_STATUS, &powerStatus, sizeof(POWER_STATUS_t));
							// Clear Wander timer
							wanderCnt[matrixHdr.seqId.seqNum] = MAX_WANDER_CNT;
							// Send ACK
							SendAckNackMsg(MSG_ACK, &matrixHdr);
						}
						// Not ACK or NACK. Could be a new Command from Master Controller
						// We only consider it as valid message if corresponding wander count is clear
						else if(wanderCnt[matrixHdr.seqId.seqNum] == 0)
						{
							tmpFlag = FALSE;

							// Return pointer of a buffer which stores receive message data.
							if(msgLength > 0)
							{
								tempPtr = GetFreeBuf(msgLength);
								if(tempPtr == NULL)
								{
									tmpFlag = TRUE;
								}
								else
								{
									// Copy message data to the buffer
									memcpy((VOIDPTR)tempPtr, (VOIDPTR)&udpRecvBuff[sizeof(MATRIX_HDR_t)], msgLength);
								}
							}

							if(tmpFlag == FALSE)
							{
								icMsg.msgId = matrixHdr.msgId;
								icMsg.payLength = msgLength;
								icMsg.payLoadPtr = tempPtr;

								if(SUCCESS == PutCommInQ(icMsg))
								{
									if(tempPtr != NULL)
									{
										// update buffer status
										UpdateCommBufStatus(tempPtr, BUSY);
									}

									// Clear Wander timer
									wanderCnt[matrixHdr.seqId.seqNum] = MAX_WANDER_CNT;
									// Send ACK
									SendAckNackMsg(MSG_ACK, &matrixHdr);
								}
							}
						} // End of Wandercount == CLEAR
						else // Since we have already queued msg before, we simply send ACK
						{
							// Send ACK
							SendAckNackMsg(MSG_ACK, &matrixHdr);
						}
					}
					else if(matrixHdr.seqId.slaveId == BROADCAST_SLAVE_ID)
					{
						switch(matrixHdr.msgId)
						{
						case MSG_ACK:				//Dont proceed this message
						case MSG_NACK:				//Dont proceed this message
						case MSG_POWER_STATUS:		//Dont proceed this message
							break;

						default:
							// Check wander count for receive seq Number
							if(broadcastWanderCnt[matrixHdr.seqId.seqNum] == 0)
							{
								tmpFlag = FALSE;

								// Return pointer of a buffer which stores receive message data.
								if(msgLength > 0)
								{
									tempPtr = GetFreeBuf(msgLength);

									if(tempPtr == NULL)
									{
										tmpFlag = TRUE;
									}
									else
									{
										// Copy message data to the buffer
										memcpy((VOIDPTR)tempPtr,
											(VOIDPTR)&udpRecvBuff[sizeof(MATRIX_HDR_t)], msgLength);
									}
								}

								if(tmpFlag == FALSE)
								{
									icMsg.msgId = matrixHdr.msgId;
									icMsg.payLength = msgLength;
									icMsg.payLoadPtr = tempPtr;

									if(SUCCESS == PutCommInQ(icMsg))
									{
										if(tempPtr != NULL)
										{
											// update buffer status
											UpdateCommBufStatus(tempPtr, BUSY);
										}

										// Clear Wander timer
										broadcastWanderCnt[matrixHdr.seqId.seqNum] = MAX_WANDER_CNT;
									}
								}
							}
							break;
						}// End of Message Id Check
					}// End of Slave ID Check
				}// End of length and seq Num check
				else
				{
					// Send NACK
					SendAckNackMsg(MSG_NACK, &matrixHdr);
				}
			} // End of header received
		}
	}
	// Now check if any outgoing message pending
	// We only send a pending message, if not waiting for an ack of previously sent msg.
	if(waitAckF == CLEAR)
	{
		if(GetCommOutQ(&ogMsg) == SUCCESS)
		{
			// Check if we have transmit buffer for the size we are trying to send.
			matrixHdr.msgId = ogMsg.msgId;
			// Prepare SeqId to send.
			matrixHdr.seqId.seqNum = msgSeqNum;
			matrixHdr.seqId.slaveId = LogicSlaveId;
			// Prepare matrix header
			matrixHdr.msgLength = ogMsg.payLength;
			// Send msg to Master
			// Put Matrix Header Structure
			memcpy((VOIDPTR)&udpSendBuff[0], (VOIDPTR)&matrixHdr, sizeof(MATRIX_HDR_t));
			countSend += sizeof(MATRIX_HDR_t);

			if(ogMsg.payLength > 0)
			{
				// Put Actual message payload.
				memcpy((VOIDPTR)&udpSendBuff[sizeof(MATRIX_HDR_t)],
						(VOIDPTR)ogMsg.payLoadPtr, matrixHdr.msgLength);
				countSend += matrixHdr.msgLength;
			}

			// Must clear socket configuration structure first before sending
			memset((VOIDPTR)&to, 0, sizeof(to));

			if(inet_aton((CHARPTR)&(systemInformation.PanelIP), &to.sin_addr) == 0)
			{
				EPRINT(COMM_LOG,"Remote Address for master is wrong : %s", systemInformation.PanelIP);
			}
			else
			{
				to.sin_family = AF_INET;

                if((commSecureUdpSocket != -1) && (systemInformation.UdpEncryptValidationData.EncryptionF == TRUE))
                {
                    EncryptedData = &udpEncryptedBuf[0];
                    size_t encryptedLen = AesCbcEncrypt256(udpSendBuff, &EncryptedData, countSend, TRUE, systemInformation.UdpEncryptValidationData.DeviceEncryptionKey);
                        if(encryptedLen == FAIL)
                        {
                            EPRINT(COMM_LOG, "Could not Encrypt DATA");
                            return;
                        }
                        else
                        {
                            DPRINT(COMM_LOG, "Encrypted Len %ld", encryptedLen);
                            countSend = encryptedLen;
                        }
                        to.sin_port = htons(REMOTE_SECURE_UDP_PORT);
                        sendCnt = sendto(commSecureUdpSocket, udpEncryptedBuf, countSend, MSG_DONTWAIT,
                                         (struct sockaddr *)&to, sizeof(struct sockaddr_in));
                }
                else
                {
                    to.sin_port = htons(REMOTE_UDP_PORT);
                    sendCnt = sendto(commUdpSocket, udpSendBuff, countSend, MSG_DONTWAIT,
                                                     (struct sockaddr *)&to, sizeof(struct sockaddr_in));

                }


				if (sendCnt < 0)
				{
					EPRINT(COMM_LOG, "Error sending UDP Message to Master");
				}
				else if (sendCnt != countSend)
				{
					EPRINT(COMM_LOG,"UDP message count sent %d and actual %d expected", sendCnt, countSend);
				}
				else
				{
 					DPRINT(COMM_LOG,"UDP msg sent [%x]",matrixHdr.msgId);
				}

				//Set Wait Ack Flag
				waitAckF = SET;
				// Start Wait ACK Timer
				StartWaitAckTimer();
			}
		}
	}
}

//*****************************************************************************
//	SendAckNackMsg()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function is send ACK or NACK message to master
//*****************************************************************************
static void SendAckNackMsg(SLAVE_MSG_ID_e msgId, MATRIX_HDR_t * matHdr)
{
	struct sockaddr_in 	to;
	UINT16 sentCnt;
    unsigned char * EncryptedData;
    CHAR				udpEncryptedBuf[MAX_UDP_MSG_LEN];
    int countSend = 0;

	// Must clear socket configuration structure first before sending
	memset((VOIDPTR)&to, 0, sizeof(to));

	matHdr->msgId = msgId;
	matHdr->msgLength = 0;
	//Seqid field send which is already receive.
	// Put Matrix Header Structure
	if(inet_aton((CHARPTR)&(systemInformation.PanelIP), &to.sin_addr) == 0)
	{
		EPRINT(COMM_LOG,"Remote Address for master is wrong : %s", systemInformation.PanelIP);
		return;
	}
	
	to.sin_family = AF_INET;

    if((commSecureUdpSocket != -1) && (systemInformation.UdpEncryptValidationData.EncryptionF == TRUE))
    {
        EncryptedData = &udpEncryptedBuf[0];
        size_t encryptedLen = AesCbcEncrypt256((unsigned char *)matHdr, &EncryptedData, sizeof(MATRIX_HDR_t), TRUE, systemInformation.UdpEncryptValidationData.DeviceEncryptionKey);
            if(encryptedLen == FAIL)
            {
                EPRINT(COMM_LOG, "Could not Encrypt DATA");
                return;
            }
            else
            {
                DPRINT(COMM_LOG, "Encrypted Len %ld", encryptedLen);
                countSend = encryptedLen;
            }
            to.sin_port = htons(REMOTE_SECURE_UDP_PORT);
            sentCnt = sendto(commSecureUdpSocket, udpEncryptedBuf, countSend, MSG_DONTWAIT,
                  (struct sockaddr *)&to, sizeof(struct sockaddr_in));
    }
    else
    {
        to.sin_port = htons(REMOTE_UDP_PORT);
        sentCnt = sendto(commUdpSocket, matHdr, sizeof(MATRIX_HDR_t), MSG_DONTWAIT,
                         (struct sockaddr *)&to, sizeof(struct sockaddr_in));
    }

    //

	DPRINT(COMM_LOG,"UDP msg of [%u] bytes sent [%x]", sentCnt, matHdr->msgId);
}

//*****************************************************************************
//	WaitAckTimeout()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function is callback function of Wait Ack timer.
//*****************************************************************************
static void WaitAckTimeout(UNUSED_PARA UINT32 data)
{
	// Stop Wait Ack Timer
	DeleteTimer(&waTmrHandle);
	// Clear wait ack flag to send new message
	waitAckF = CLEAR;
	// Time out so increament retry count
	msgRetryCnt++;
	// Retry up to max count and then discard that message.
	if(msgRetryCnt >= MAX_RETRY_CNT)
	{
		UpdateToSendNewMsg();
	}
}

//*****************************************************************************
//	UpdateToSendNewMsg()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function update Out queue to next pointer including freeing
//				buffer. It also resets retry count and increments seqNum to be
//				used with next message.
//*****************************************************************************
static void UpdateToSendNewMsg(void)
{
	// Make REtry Count to 0.
	msgRetryCnt = 0;
	//Ignore message from OutQueue
	//Update OutQueue pointer to next entry and free up buffer
	UpdateCommOutQ(TRUE);
	// Increament Seq Id for new message.
	msgSeqNum++;
	// Rolover seqId if it reach max value.
	if(msgSeqNum >= MAX_SEQ_NUM)
	{
		msgSeqNum = 0;
	}
}

//*****************************************************************************
//	StartWaitAckTimer()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function starts Wait Ack Timer. This timer is started
//				after sending a message so that we can limit how long we
//				wait for ACK from remote.
//*****************************************************************************
static void StartWaitAckTimer(void)
{
	TIMER_INFO_t tInfo;

	tInfo.count = WAIT_ACK_TMR_CNT;
	tInfo.data = 0;
	tInfo.funcPtr = &WaitAckTimeout;
	StartTimer(tInfo, &waTmrHandle, __FUNCTION__);
}

//*****************************************************************************
//	StopCommTask()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function stops all Communication with Master panel
//*****************************************************************************
void StopCommTask(void)
{
    CloseUdpPingSocket(TRUE);
	ChangeCommState(SLAVE_OFFLINE, TRUE);
 	hbCnt = 0;
}

//*****************************************************************************
//	ChangeFirmwareUpgradeState()
//	Param:
//		IN :	FIRMWARE_UPGRATE_STATE_e fwStat
//		OUT:	NONE
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function change firmware upgrade state of door.
//*****************************************************************************
BOOL ChangeFirmwareUpgradeState(FIRMWARE_UPGRATE_STATE_e fwStat)
{
	firmwareUpgradeStat = fwStat;
	return SUCCESS;
}

//*****************************************************************************
//	GetCurrentHbcnt()
//	Param:
//		IN :	
//		OUT:	
//	Returns:	hbcnt
//	Description:
//				This function will return current hbcnt.
//*****************************************************************************
UINT8 GetCurrentHbcnt()
{
	return hbCnt;
}

//*****************************************************************************
//	SetCurrentHbcnt()
//	Param:
//		IN :	hbcnt
//		OUT:	
//	Returns:	
//	Description:
//				This function will Set current hbcnt.
//*****************************************************************************
void SetCurrentHbcnt(UINT8 cnt)
{
	hbCnt = cnt;
}

/*****************************************************************************
 * EOF
 *****************************************************************************/
