#if !defined MXCOMMTASK_H
#define MXCOMMTASK_H
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
//   Created By   : Kunjal Garach
//   File         : MxCommTask.h
//   Description  : This file implements Comm task related defines and API.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"
#include "MxSlaveCommonMsgList.h"
#include "MxSlaveCommon.h"

//******** Defines and Data Types ****

#define MAX_SEQ_NUM					255
#define	UNKNOWN_SEQ_NUM				MAX_SEQ_NUM
#define	BROADCAST_SLAVE_ID			0xFF
#define UDP_SOCKET					INT32
#define	MAX_UDP_MSG_LEN				4100	// Maximum UDP message length

typedef enum
{
	MSG_RECIEVED = 0,
	NO_MSG_IN_Q,
	TOO_LONG_MSG,
	MAX_MSG_STATUS
}MSG_RECV_STATUS_e;

typedef enum
{
	COMM_MODE_UNKNOWN,
	COMM_MODE_ETHERNET,
	COMM_MODE_RS485,
	COMM_MODE_MAX
}COMM_MODE_e;

typedef enum
{
	SLAVE_OFFLINE,
	SLAVE_ONLINE,
	SLAVE_DEGRADE,
	COMM_STATE_MAX
}COMM_STATE_e;

typedef enum
{
	FIRMWARE_UPGRADE_IDLE,
	FIRMWARE_UPGRADE_PROGRESS,
	FIRMWARE_UPGRADE_COMPLETE,
}FIRMWARE_UPGRATE_STATE_e;

//******** Function Prototypes *******
BOOL SendMsg(SLAVE_MSG_ID_e msgId, VOIDPTR dataPtr, UINT16 length);
MSG_RECV_STATUS_e GetMsgNoCopy(SLAVE_MSG_ID_e * msgId, UINT16PTR length, VOIDPTR * dataPtr);
UINT8PTR GetFreeBuf(UINT16 bufSize);
BOOL FreeCommBuffer(VOIDPTR dataPtr);

BOOL PutCommOutQ(OG_MSG_DESC_t ogMsg);
BOOL GetCommOutQ(OG_MSG_DESC_t *ogMsg);
BOOL UpdateCommOutQ(BOOL bufFreeF);
void ClearCommOutQ(void);
BOOL PutCommInQ(IC_MSG_DESC_t icMsg);
BOOL GetCommInQ(IC_MSG_DESC_t *icMsg);
BOOL UpdateCommInQ(BOOL bufFreeF);
void ClearCommInQ(void);

void InitCommTask(void);
void RunCommTask(void);
void OpenUdpSocket(void);
void OpenUdpSecureSocket(void);
void StopCommTask(void);
void ChangeCommState(COMM_STATE_e commState, BOOL switchIdlF);
BOOL ChangeFirmwareUpgradeState(FIRMWARE_UPGRATE_STATE_e fwStat);
UINT8 GetCurrentHbcnt();
void SetCurrentHbcnt(UINT8 cnt);

#endif  // MXCOMMTASK_H
