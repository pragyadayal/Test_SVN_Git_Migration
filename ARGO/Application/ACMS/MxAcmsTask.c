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
//   Created By   : Kunjal Garach
//   File         : MxAcmsTask.c
//   Description  : ACMS Task provides communication channel between ACS and
// 					ACMS software running on host PC. This will manage login
//					message and event related task as a client using push
//					message method.
// For login Message:
//		1. For login message- wait for relogin timer for message sent successfully.
//		2. If message sent sucessfully, timer will be reloaded with relogin timer to get ACK/reply.
//		3. If message sending failed, wait till relogin timeout before sending next login request.
// For all other messages
//		1. If any message sent sucessfully- wait for poll duration for reply/ACK.
//		2. If poll is to be sent again, wait duration of poll interval will apply beore sending next poll message.
//
/////////////////////////////////////////////////////////////////////////////

// macro; to make use of recursive mutex
#define _GNU_SOURCE

//system Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

//matrix Includes.
#include "MxTypedef.h"
#include "MxAcmsTask.h"
#include "MxAcmsSocket.h"
#include "MxAcmsCommon.h"
#include "MxAcmsMsgHandler.h"
#include "MxAcmsFileTransfer.h"
#include "MxAcmsConfig.h"
#include "MxAcmsCommand.h"
#include "MxConfig.h"
#include "MxConfigFileIo.h"
#include "MxLogs.h"
#include "MxEventLog.h"
#include "MxEthernetPort.h"
#include "MxSysTimer.h"
#include "MxGpio.h"
#include "MxDcTask.h"
#include "MxSysUtils.h"
#include "MxAvResp.h"
#include "MxSysCommonDef.h"
#include "MxNetworkUtils.h"
#include "MxMultiLanFileHandling.h"
#include "MxWebJvsBackupAndUpdate.h"
#include "MxLicenseDongle.h"
#include "MxFaceRecognition.h"
#include "MxFrLinkedLists.h"
#include "MxFaceRecognitionConfig.h"
#include "MxCafeteria.h"
#include "MxUserConfig.h"
#include "Common_Req_Resp_Data.h"
#include "MxDcTaskCommonDef.h"
#include "MxMobileBroadband.h"

//******** Extern Variables **********
extern SYS_BASIC_PARA_t		SysBasicPara;
extern ETHERNET_PARA_t		EthernetParameters;
extern SERVER_PARA_t		ServerParameters;
extern DOOR_ACTIVITY_INFO_t	DcInfo;
extern FTC_INFO_t			ftcInfo;
extern BOOL					ResolveAcmsDomainName;
extern TIMER_HANDLE			sysRebootTmrHandle;
extern FP_TRANS_PARA_t		FpTransPara;
extern COMM_PORT_e			acmsCurActiveInterface;
extern SERVER_PARA_t		TempServerParaVyom;
extern BOOL					ServerConnectionCheckFlags[MAX_PORT][SEVERVER_CONNECTION_MAX_CHECKS];
extern SERVER_MODE_CONFIG_t		ServerModeCfg;

extern BOOL					cafePolicyRespReceivedF;

extern UINT32					seqNoForEnrlFaceImg;
extern FR_SERVER_CONFIG_t				FRServerConfig;
extern ACMS_IP_RESOLVE_STATUS_e AcmsIpResolveStsState;
extern BOOL SuccessfullyGetImageTmpltFtcDoneF;
extern COMM_PORT_e						lastActiveComInterface;
extern CAFE_MENU_NAVIGATION_t	cafeMenuNavigation;
extern BOOL WifiAcmsResetF;
extern FrServerSupportedFeatures_t frServerSupportedFeatures;
//******** Defines and Data Types ****
#define	ACMS_RELOGIN_TIME			10		// 10 Second
#define MAX_POLL_MIS_COUNT			3		// Max 3 Poll request miss count
#define DEFAULT_POLL_DURATION		15		// 15 Second
#define DEFAULT_POLL_INTERVAL		2		// 2 Second
#define	SET_TMP_ACK_DELAY			6		// 3 Second
#define ACMS_FTC_RETRY_COUNT		3
#define ACMS_USB_VYOM_RESET_TIME	10 //10 min

// ACMS message task states
typedef enum
{
	ACMS_SEND_REQ_LOGIN_STATE,
	ACMS_RECV_ACK_LOGIN_STATE,
	ACMS_SEND_REQ_POLL_STATE,
	ACMS_RECV_ACK_POLL_STATE,
	ACMS_SEND_REQ_MSG_STATE,
	ACMS_RECV_MSG_STATE,
	ACMS_MSG_REPLY_WAIT_STATE,
	ACMS_MSG_REPLY_STATE,
	ACMS_SEND_MSG_WAIT_STATE,
	ACMS_SEND_RECV_MSG_WAIT_STATE,
	ACMS_SEND_RECV_MSG_HOLD,
	MAX_ACMS_SEND_RECV_MSG_STATE
}ACMS_MSG_STATE_e;

// ACMS event task states
typedef enum
{
	ACMS_SEND_EVT_IDLE_STATE = 0,
	ACMS_SEND_EVT_STATE,
	ACMS_RECV_EVT_ACK_STATE,
	ACMS_SEND_EVT_OVER_STATE,
	ACMS_SEND_EVT_WAIT_STATE,
	ACMS_SEND_RECV_EVT_WAIT_STATE,
	MAX_ACMS_SEND_RECV_EVT_STATE
}ACMS_EVT_STATE_e;

typedef enum
{
	ACMS_FTC_IDLE_STATE = 0,			// File transfer idle state
	ACMS_FTC_READY_STATE,				// File transfer ready state
	ACMS_FTC_RECV_CMD_STATE,			// File transfer receive command state
	ACMS_FTC_REPLY_CMD_STATE,			// File transfer reply command state
	ACMS_FTC_WAIT_CLOSE_SOCKET_STATE,	// File transfer wait close socket state
	ACMS_FTC_REPLY_WAIT_STATE,			// File transfer wait state for FR idt server response.
	MAX_ACMS_FTC_STATE
}ACMS_FTC_STATE_e;

#define GEN_ACMS_RCV_HDR_ENUM(RCV_MSG_HDR_ENUM, RCV_MSG_HDR_NAME,	RCV_MSG_FSP_CNT)	RCV_MSG_HDR_ENUM,
#define GEN_ACMS_RCV_HDR_NAME(RCV_MSG_HDR_ENUM, RCV_MSG_HDR_NAME,	RCV_MSG_FSP_CNT)	RCV_MSG_HDR_NAME,
#define GEN_ACMS_RCV_FSP_CNT( RCV_MSG_HDR_ENUM, RCV_MSG_HDR_NAME,	RCV_MSG_FSP_CNT)	RCV_MSG_FSP_CNT,

#define ACMS_RCV_MSG_HDR(API)\
		API(ACK_LOG,	"ACK_LOG",	4)\
		API(ACK_POL,	"ACK_POL",	1)\
		API(ACK_EVT,	"ACK_EVT",	2)\
		API(NAK_EVT,	"NAK_EVT",	2)\
		API(SET_CFG,	"SET_CFG",	2)\
		API(SET_CMD,	"SET_CMD",	2)\
		API(SET_SOR,	"SET_SOR",	2)\
		API(SET_TMP,	"SET_TMP",	2)\
		API(GET_EVT,	"GET_EVT",	3)\
		API(GET_CRD,	"GET_CRD",	2)\
		API(GET_TMP,	"GET_TMP",	2)\
		API(PRC_ENR,	"PRC_ENR",	2)\
		API(END_MSG,	"END_MSG",	0)\
		API(STA_FTC,	"STA_FTC",	2)\
		API(GET_DTL,	"GET_DTL",	2)\
		API(GET_PIN,    "GET_PIN",  2)\
		API(RDY_FRC,	"RDY_FRC",	6)\
		API(RDY_EFR,	"RDY_EFR",	4)\
		API(GET_UFI,	"GET_UFI",	3)
typedef enum
{
	ACMS_RCV_MSG_HDR(GEN_ACMS_RCV_HDR_ENUM)
	MAX_RCV_MSG_HDR,
}RCV_MSG_HDR_e;

#define ACMS_SND_MSG_HDR(API)\
		API(REQ_LOG,	"REQ_LOG")\
		API(REQ_POL,	"REQ_POL")\
		API(REQ_MSG,	"REQ_MSG")\
		API(RPL_EVT,	"RPL_EVT")\
		API(RPL_CRD,	"RPL_CRD")\
		API(RPL_TMP,	"RPL_TMP")\
		API(ERR_EVT,	"ERR_EVT")\
		API(ERR_CRD,	"ERR_CRD")\
		API(ERR_TMP,	"ERR_TMP")\
		API(END_EVT,	"END_EVT")\
		API(ACK_CFG,	"ACK_CFG")\
		API(ACK_CMD,	"ACK_CMD")\
		API(ACK_SOR,	"ACK_SOR")\
		API(ACK_TMP,	"ACK_TMP")\
		API(ACK_ENR,	"ACK_ENR")\
		API(STA_EVT,	"STA_EVT")\
		API(ACK_FTC,	"ACK_FTC")\
		API(RPL_DTL,	"RPL_DTL")\
		API(RPL_PIN,    "RPL_PIN")\
		API(ERR_PIN,	"ERR_PIN")\
		API(ACK_FRC,	"ACK_FRC")\
		API(ACK_EFR,	"ACK_EFR")\
		API(ACK_UFI,	"ACK_UFI")\
		API(ERR_UFI,	"ERR_UFI")\

typedef enum
{
	ACMS_SND_MSG_HDR(GEN_ACMS_ENUM)
	MAX_SND_MSG_HDR,
}SND_MSG_HDR_e;


#define ACMS_RCV_FTC_HDR(API)\
		API(RCV_FTC_SET_FIL,	"SET_FIL",	2)\
		API(RCV_FTC_GET_FIL,	"GET_FIL",	2)\
		API(RCV_FTC_ACK_FIL,	"ACK_FIL",	2)\
		API(RCV_FTC_END_FIL,	"END_FIL",	2)\

typedef enum
{
	ACMS_RCV_FTC_HDR(GEN_ACMS_RCV_HDR_ENUM)
	MAX_RCV_FTC_HDR,
}RCV_FTC_HDR_e;

#define ACMS_SND_FTC_HDR(API)\
		API(SND_FTC_RDY,		"FTC_RDY")\
		API(SND_FTC_ACK_FIL,	"ACK_FIL")\
		API(SND_FTC_RPL_FIL,	"RPL_FIL")\
		API(SND_FTC_ERR_FIL,	"ERR_FIL")\
		API(SND_FTC_END_FIL,	"END_FIL")\

typedef enum
{
	ACMS_SND_FTC_HDR(GEN_ACMS_ENUM)
	MAX_SND_FTC_HDR,
}SND_FTC_HDR_e;

//******** Function Prototypes *******
static void RunAcmsMessageTask(void);
static void RunAcmsEventTask(void);
static void RunAcmsFtcTask(void);
static BOOL LoadFtcProgTimer(void);
static void CloseFtcProcess(UINT32 forceClose);
static VOIDPTR AcmsFwUpgradeThread(VOIDPTR arg);
static void HandleFtcCloseProcess();
//******** Global Variables **********
UINT16					AcmsPollDuration;
CHAR					TenentIdStr[MAX_LENGTH_TEN_ID_STR] = "";
SERVER_CMD_MSG_INFO_t	cmdProcInfo;
CAFE_POLICY_CHECK_CMD_MSG_INFO_t cafePolicyCheckResp;
pthread_mutex_t 		resourceMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

CHARPTR sendCmdHdr[MAX_SND_CMD_HDR] =
{
	ACMS_SND_CMD_HDR(GEN_ACMS_NAME)
};

CHARPTR recvCmdHdr[MAX_RCV_CMD_HDR] =
{
	ACMS_RCV_CMD_HDR(GEN_ACMS_NAME)
};

//******** Static Variables **********
//ACS Receive string.
static CHARPTR acmsRecvMsgHdr[MAX_RCV_MSG_HDR] =
{
	ACMS_RCV_MSG_HDR(GEN_ACMS_RCV_HDR_NAME)
};

//ACS Send string.
static CHARPTR acmsSendMsgHdr[MAX_SND_MSG_HDR] =
{
	ACMS_SND_MSG_HDR(GEN_ACMS_NAME)
};

static CHARPTR acmsRecvFtcHdr[MAX_RCV_FTC_HDR] =
{
	ACMS_RCV_FTC_HDR(GEN_ACMS_RCV_HDR_NAME)
};

static CHARPTR acmsSendFtcHdr[MAX_SND_FTC_HDR] =
{
	ACMS_SND_FTC_HDR(GEN_ACMS_NAME)
};

static const UINT8 acmsRecvMsgFspCnt[MAX_RCV_MSG_HDR] =
{
	ACMS_RCV_MSG_HDR(GEN_ACMS_RCV_FSP_CNT)
};

static const UINT8 acmsRecvFtcFspCnt[MAX_RCV_FTC_HDR] =
{
	ACMS_RCV_FTC_HDR(GEN_ACMS_RCV_FSP_CNT)
};

static	ACMS_MSG_STATE_e	acmsMsgState;
static	ACMS_MSG_STATE_e	acmsNxtMsgState;
static	ACMS_MSG_STATE_e	acmsNxtMsgSuccessState;

static	UINT32				acmsMsgTimerTick;
static	UINT32				acmsMsgWaitTime;

static	ACMS_EVT_STATE_e	acmsEvtState;
static	ACMS_EVT_STATE_e	acmsNxtEvtState;
static	ACMS_EVT_STATE_e	acmsNxtEvtSuccessState;

static	UINT32				acmsEvtTimerTick;
static	UINT32				acmsEvtWaitTime;

static	UINT32				acmsSrvrCmdTimerTick;
static	UINT32				acmsSrvrCmdWaitTime;

static 	UINT16				acmsPollInterval;

static 	ACMS_FTC_STATE_e	acmsFtcState;

static 	ACMS_COMM_STATUS_e	acmsLoginStatus;		// communication status
static	BOOL				acmsEvtTrxStatus;
static	BOOL				acmsFtcTrxStatus;

static	UINT16				acmsRollOverCnt;
static	UINT32				acmsEventCnt;

static	UINT16				acmsDeviceNo = 0;

static 	TIMER_HANDLE		acmsFtcProgTmrHandle = INVALID_TIMER_HANDLE;
static 	CHAR				acmsFwUpgradeFilePath[100];
static 	pthread_t			fwUpgradeThreadData = 0;
static 	THREAD_STATUS_e		fwUpgradeThreadStatus = THREAD_INACTIVE;

static	UINT8				ftcRetryCnt;
static	RCV_FTC_HDR_e		lastFtcRecvHdr;
static	SND_FTC_HDR_e		lastFtcSndHdr;

//firmware upgrade status once firmware file is received
static	BOOL				fwUpgradeStatus = INACTIVE;

static TIMER_HANDLE		acmsVyomResetTmrHandler = INVALID_TIMER_HANDLE;
static COMM_PORT_e			acmsPrevConnectedActiveInterface;
static BOOL                 acmsReqLoginF = FALSE;
static	UINT32				acmsVyomUsbDongleResetTimerTick;
//******** Function Definations ******

//*****************************************************************************
//	InitAcmsTask()
//	Param:
//		IN	:	NONE
//		OUT	:	NONE
//	Returns	:	NONE
//	Description:
//	    This function creates Server port and listens to any connection request
// 		from ACMS. It initializes all this module related data structures.
//		This function must be called once before entering super-loop..
//
//*****************************************************************************
void InitAcmsTask(void)
{
	InitSocket();
	InitAcmsFtc();
	InitLicenseDongleState();

	acmsDeviceNo = SysBasicPara.devId;
	acmsMsgState = ACMS_SEND_REQ_LOGIN_STATE;
	acmsNxtMsgState = ACMS_SEND_REQ_LOGIN_STATE;
	acmsNxtMsgSuccessState = ACMS_SEND_REQ_LOGIN_STATE;

	acmsEvtState = ACMS_SEND_EVT_IDLE_STATE;
	acmsNxtEvtState = ACMS_SEND_EVT_IDLE_STATE;
	acmsNxtEvtSuccessState = ACMS_SEND_EVT_IDLE_STATE;

	AcmsPollDuration = DEFAULT_POLL_DURATION;
	acmsPollInterval = DEFAULT_POLL_INTERVAL;

	acmsFtcState	=	ACMS_FTC_IDLE_STATE;

	acmsLoginStatus = ACMS_OFFLINE;
	acmsEvtTrxStatus = OFF;
	acmsFtcTrxStatus = OFF;

	acmsRollOverCnt = 0;
	acmsEventCnt = 0;

	cmdProcInfo.cmdProcState = SERVER_CMD_MSG_IDLE_STATE;
	cmdProcInfo.cmdTransactionId = 0;
	cmdProcInfo.data = 0;
	cmdProcInfo.statusF = FALSE;

	// NOTE: Must be called after ACMS status is changed to OFFLINE
	SetNwStatusCad(acmsCurActiveInterface, INVALID_LED_STATUS);
	memset(ServerConnectionCheckFlags, 0xFF, sizeof(ServerConnectionCheckFlags));
}

//*****************************************************************************
//	RunAcmsTask()
//	Param:
//		IN:NONE
//		OUT:NONE
//	Returns:
//		NONE
//	Description:
//	     	This function will send login request and receive login ACK from ACMS.
// 			If ACK is receive within time then it poll for any message. If ACK
//			is not receive within time it try to login after define time. Ones
//			login is done it try to get commands, messages form ACMS. It also
//			sends events to ACMS.
//*****************************************************************************
void RunAcmsTask(void)
{
	if(WifiAcmsResetF == TRUE)
	{
		ChangeNetworkInterfacePara(WIFI_PARA);
		WifiAcmsResetF = FALSE;
	}
	// Run ACMS message Task
	RunAcmsMessageTask();
	// Run ACMS Event Task
	RunAcmsEventTask();
	// Run ACMS Ftc Task
	RunAcmsFtcTask();
	//Run License Dongle Task
	if(ServerModeCfg.serverMode == COSEC_CENTRA)
	{
		RunLicenseDongleTask();
	}
}

//*****************************************************************************
//	RunAcmsMessageTask()
//	Param:
//		IN:		None
//		OUT: 	None
//	Returns:
//				None
//	Description:
//			This function runs ACMS message task
//
//*****************************************************************************
static void RunAcmsMessageTask(void)
{
	BOOL			rxTxStatus;
	CHAR			acmsMsgSendBuf[MAX_ACMS_PACKET_SZ];
	CHAR			interfaceIpAddr[MAX_IP_ADDRESS];
	CHARPTR			acmsRecvPtr;
	UINT64			parseField[6];
	UINT16			tempAcmsRollOverCnt, length;
	UINT32			tempAcmsEventCnt;
	UINT64			tempAcmsReqCnt, tempAcmsCurrentCnt;
	RCV_MSG_HDR_e	acmsRecvMsg;
	BOOL			parseFieldSuccessF = TRUE;

	static	CHAR			acmsMsgProcBuf[MAX_ACMS_PACKET_SZ];
	static	UINT16			procMsgLength;
	static 	BOOL			pollResponseStatus = FALSE;
	static	UINT8			pollMisCnt = 0;
	static	UINT64			tid;
	static 	SND_MSG_HDR_e	sendMsgHdr;
	static 	GET_UFI_STATUS_e 	errUfiStatus = FAIL_UNKNOWN_REASON;

	// Send Message FSM
	switch(acmsMsgState)
	{
	default:
	case ACMS_SEND_REQ_LOGIN_STATE:
	{
		ResolveAcmsDomainName = TRUE;
		AcmsIpResolveStsState = ACMS_IP_RESOLVE_IN_PROGRESS;
		acmsMsgState = ACMS_MSG_REPLY_WAIT_STATE;
		// For Reset USB Dongle Modem if server is not connected after 10 min
		if(ServerModeCfg.serverMode == COSEC_VYOM)
		{
		    if(acmsReqLoginF == FALSE)
		    {
			    DPRINT(ACMS_LOG,"Dongle GetSysTick For ResetModem");
			    acmsVyomUsbDongleResetTimerTick = GetSysTick();
			    acmsReqLoginF = TRUE;
			    DPRINT(ACMS_LOG,"Dongle acmsReqLoginF[%d] acmsPrevConnectedActiveInterface [%d] ServerConnectionCheckFlags[PPP_PORT][SEVERVER_CONNECTION_CHECK_ACK_LOG][%d] acmsVyomUsbDongleResetTimerTick[%d]",acmsReqLoginF,acmsPrevConnectedActiveInterface, ServerConnectionCheckFlags[PPP_PORT][SEVERVER_CONNECTION_CHECK_ACK_LOG], acmsVyomUsbDongleResetTimerTick);
		    }
		    else
		    {	
			    DPRINT(ACMS_LOG,"Dongle acmsReqLoginF[%d] acmsPrevConnectedActiveInterface [%d] ServerConnectionCheckFlags[PPP_PORT][SEVERVER_CONNECTION_CHECK_ACK_LOG][%d] acmsVyomUsbDongleResetTimerTick[%d]",acmsReqLoginF,acmsPrevConnectedActiveInterface, ServerConnectionCheckFlags[PPP_PORT][SEVERVER_CONNECTION_CHECK_ACK_LOG], acmsVyomUsbDongleResetTimerTick);
		    }
		}
		
	}break;

	case ACMS_RECV_ACK_LOGIN_STATE:
	{
		rxTxStatus = RecvAcmsData(ACMS_MSG_SOCKET, &acmsRecvPtr, &length);
		if(rxTxStatus == SUCCESS)
		{
			if(GetRecvMsgHeader(acmsRecvMsgHdr, MAX_RCV_MSG_HDR, &acmsRecvPtr) == ACK_LOG)
			{
				if(GetDataParseField(&acmsRecvPtr, parseField, 1) == SUCCESS)
				{
					if((BOOL)parseField[0] == SUCCESS)
					{
						if(GetDataParseField(&acmsRecvPtr, parseField, 3) == SUCCESS)
						{
							ServerConnectionCheckFlags[acmsCurActiveInterface][SEVERVER_CONNECTION_CHECK_ACK_LOG] = TRUE;
							acmsLoginStatus = ACMS_ONLINE;
							acmsPrevConnectedActiveInterface   = acmsCurActiveInterface;
							acmsReqLoginF = FALSE;
							// Update server status on LCD status bar
							UpdateDisplayStatusField();
							SetLed(ACCESS_ONLINE_RESP, INT_EXT_PORT);

							if(TRUE == getDeviceInactiveStatus())
							{
							      if( TRUE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER1) )
							      {
								      SetLed(ACCESS_INACTIVE_DEVICE_RESP, INTERNAL_PORT);
							      }

							      if( TRUE == IsVerifiedEntryExitReaderModeForAccessSchedule(READER3) )
							      {
								      SetAudioVisual(USER_RESP_DEVICE_INACTIVE, EXTERNAL_PORT, (VOIDPTR)" ");
							      }

							}

							//Send Connectivity status to External Reader
							SendDoorConnectivityStatusToExternalReader();

							acmsDeviceNo = (UINT16)parseField[0];
							AcmsPollDuration = (UINT16)parseField[1];
							acmsPollInterval = (UINT16)parseField[2];

							pollMisCnt = (MAX_POLL_MIS_COUNT + 1);
							acmsMsgState = ACMS_SEND_REQ_POLL_STATE;

							DPRINT(ACMS_LOG, "Login Successful PD[%d] PI[%d] MID[%d]", AcmsPollDuration, acmsPollInterval, acmsDeviceNo);
						}
					}
					else if(parseField[0] == 9)
					{
						if(GetDataParseField(&acmsRecvPtr, parseField, 1) == SUCCESS)
						{
							if( (parseField[0] == 1) || (parseField[0] == 0) )
							{
								TempServerParaVyom.ethServerConnection = SERVER_CONN_HOST_NAME;
							}
							else
							{
								parseFieldSuccessF = FALSE;
							}
						}
						else
						{
							parseFieldSuccessF = FALSE;
						}

						if(parseFieldSuccessF != FALSE)
						{
							if(ParseString(&acmsRecvPtr, TempServerParaVyom.ethServerHostName, MAX_SERVER_ADDR_254) == FAIL)
							{
								parseFieldSuccessF = FALSE;
							}
							//strcpy(TempVyomServerPara.serverIpAddressVyom, DfltVyomServerPara.serverIpAddressVyom);
							strcpy(TempServerParaVyom.ethServerIpAddress, "");
						}

						if(GetDataParseField(&acmsRecvPtr, parseField, 1) == SUCCESS)
						{
							TempServerParaVyom.ethServerPort = (UINT16)parseField[0];
						}
						else
						{
							parseFieldSuccessF = FALSE;
						}

						if(parseFieldSuccessF == FALSE)
						{
							TempServerParaVyom = ServerParameters;
						}

						DPRINT(ACMS_LOG, "Login ACK redirect response");
						//parses and copy data
						//TempVyomServerPara

						acmsMsgWaitTime = CONVERT_SEC_TO_MSEC(ACMS_RELOGIN_TIME);
						acmsMsgTimerTick = GetSysTick();
						acmsNxtMsgState = ACMS_SEND_REQ_LOGIN_STATE;
						acmsMsgState = ACMS_SEND_RECV_MSG_WAIT_STATE;
					}
					else
					{
						DPRINT(ACMS_LOG, "Login ACK Fail");

						acmsMsgWaitTime = CONVERT_SEC_TO_MSEC(ACMS_RELOGIN_TIME);
						acmsMsgTimerTick = GetSysTick();
						acmsNxtMsgState = ACMS_SEND_REQ_LOGIN_STATE;
						acmsMsgState = ACMS_SEND_RECV_MSG_WAIT_STATE;
					}
					CloseClientSocket(ACMS_MSG_SOCKET);
				}
			}
		}
		else if(ElapsedTick(acmsMsgTimerTick) >= acmsMsgWaitTime)
		{
			if(ServerConnectionCheckFlags[acmsCurActiveInterface][SEVERVER_CONNECTION_CHECK_SSL_CONNECT] == TRUE)
			{
				ServerConnectionCheckFlags[acmsCurActiveInterface][SEVERVER_CONNECTION_CHECK_ACK_LOG] = FALSE;
			}
			DPRINT(ACMS_LOG, "Login ACK Time out");
			acmsMsgState = ACMS_SEND_REQ_LOGIN_STATE;
			CloseClientSocket(ACMS_MSG_SOCKET);
		}
		break;
	}

	case ACMS_SEND_REQ_POLL_STATE:
	{
		if(pollResponseStatus == TRUE)
		{
			pollResponseStatus = FALSE;

			if(pollMisCnt < MAX_POLL_MIS_COUNT)
			{
				pollMisCnt++;
			}
		}
		else
		{
			pollMisCnt--;

			if(pollMisCnt == 0)
			{
				LogoutAcms(ACMS_COMM_LOGOUT, DO_NOT_CLOSE_ALL_SERVER_SOCKETS);
				break;
			}
		}

		sprintf(acmsMsgSendBuf, "%c%s%c%s%d%c%d%c%c", SOT, acmsSendMsgHdr[REQ_POL], FSP, TenentIdStr,GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP, EOT);

		// Send ACMS Message
		SendAcmsData(ACMS_MSG_SOCKET, acmsMsgSendBuf, strlen(acmsMsgSendBuf));

		acmsMsgWaitTime	 		= CONVERT_SEC_TO_MSEC(AcmsPollDuration);
		acmsNxtMsgState 		= ACMS_SEND_REQ_POLL_STATE;
		acmsNxtMsgSuccessState 	= ACMS_RECV_ACK_POLL_STATE;
		acmsMsgState 			= ACMS_SEND_MSG_WAIT_STATE;

		DPRINT(ACMS_LOG, "Send Poll Message --> %s", acmsMsgSendBuf);
		break;
	}

	case ACMS_RECV_ACK_POLL_STATE:
	{
		rxTxStatus = RecvAcmsData(ACMS_MSG_SOCKET, &acmsRecvPtr, &length);

		if(rxTxStatus == SUCCESS)
		{
			if(GetRecvMsgHeader(acmsRecvMsgHdr, MAX_RCV_MSG_HDR, &acmsRecvPtr) == ACK_POL)
			{
				if(GetDataParseField(&acmsRecvPtr, parseField, acmsRecvMsgFspCnt[ACK_POL]) == SUCCESS)
				{
					rxTxStatus = (BOOL)parseField[0];

					if(rxTxStatus == TRUE)
					{
						acmsMsgState = ACMS_SEND_REQ_MSG_STATE;
						DPRINT(ACMS_LOG, "Receive Poll ACK Message To Get");
					}
					else
					{
						// Wait till poll interval before sending next poll message
						acmsMsgWaitTime 	= CONVERT_SEC_TO_MSEC(acmsPollInterval);
						acmsMsgTimerTick 	= GetSysTick();
						acmsNxtMsgState 	= ACMS_SEND_REQ_POLL_STATE;
						acmsMsgState 		= ACMS_SEND_RECV_MSG_WAIT_STATE;
						DPRINT(ACMS_LOG, "Receive Poll ACK No Message To Get");
					}
					pollResponseStatus = TRUE;
					CloseClientSocket(ACMS_MSG_SOCKET);
				}
			}
		}
		else if (ElapsedTick(acmsMsgTimerTick) >= acmsMsgWaitTime)
		{
			DPRINT(ACMS_LOG, "Poll ACK Time out");

			// Wait till poll interval before sending next poll message
			acmsMsgWaitTime = CONVERT_SEC_TO_MSEC(acmsPollInterval);
			acmsMsgTimerTick = GetSysTick();
			acmsNxtMsgState = ACMS_SEND_REQ_POLL_STATE;
			acmsMsgState = ACMS_SEND_RECV_MSG_WAIT_STATE;
			CloseClientSocket(ACMS_MSG_SOCKET);
		}
		break;
	}

	case ACMS_SEND_REQ_MSG_STATE:
	{
		sprintf(acmsMsgSendBuf, "%c%s%c%s%d%c%d%c%c", SOT, acmsSendMsgHdr[REQ_MSG], FSP, TenentIdStr, GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP, EOT);

		// Send ACMS Message
		SendAcmsData(ACMS_MSG_SOCKET, acmsMsgSendBuf, strlen(acmsMsgSendBuf));

		acmsMsgWaitTime = CONVERT_SEC_TO_MSEC(AcmsPollDuration);
		acmsNxtMsgState = ACMS_SEND_REQ_POLL_STATE;
		acmsNxtMsgSuccessState = ACMS_RECV_MSG_STATE;
		acmsMsgState = ACMS_SEND_MSG_WAIT_STATE;
		DPRINT(ACMS_LOG, "Send Get Message --> %s", acmsMsgSendBuf);
		break;
	}

	case ACMS_RECV_MSG_STATE:
	{
		rxTxStatus = RecvAcmsData(ACMS_MSG_SOCKET, &acmsRecvPtr, &length);

		if(rxTxStatus == SUCCESS)
		{
		  	acmsRecvMsg = GetRecvMsgHeader(acmsRecvMsgHdr, MAX_RCV_MSG_HDR, &acmsRecvPtr);

			if(acmsRecvMsg < MAX_RCV_MSG_HDR)
			{
				if(GetDataParseField(&acmsRecvPtr, parseField, acmsRecvMsgFspCnt[acmsRecvMsg]) == SUCCESS)
				{
					tid = parseField[1];

					DPRINT(ACMS_LOG, "Recv Msg [%s]", acmsRecvMsgHdr[acmsRecvMsg]);

					acmsMsgState = ACMS_MSG_REPLY_STATE;

					switch(acmsRecvMsg)
					{
					default:
					case END_MSG:
						// Wait till poll interval before sending next poll message
						acmsMsgWaitTime = CONVERT_SEC_TO_MSEC(acmsPollInterval);
						acmsMsgTimerTick = GetSysTick();
						acmsNxtMsgState = ACMS_SEND_REQ_POLL_STATE;
						acmsMsgState = ACMS_SEND_RECV_MSG_WAIT_STATE;

						CloseClientSocket(ACMS_MSG_SOCKET);
						break;

					case RDY_EFR:
					{
						GetFaceReqQueueData_t* data = NULL;
						ExceptionalFaceReqData_t* excDataPtr = NULL;
						UINT16 imgId = parseField[3];

						tid = parseField[2];
						if(IsMaxExcGetFaceReqRecvd())
						{
							DPRINT(ACMS_LOG,"Total Pending requests exceed for RDY_EFR.Tid:[%d] IID:[%d]",tid,imgId);
							WriteEventLog(EXCEPTIONAL_FACE_AVAILABLE_EVT, UNUSED_FIELD, FAILURE_FACE_IMG, imgId, UNUSED_FIELD, OTHER_FAIL_REASON_EXC_FACE);
						}
						data = (GetFaceReqQueueData_t*)calloc(1,sizeof(GetFaceReqQueueData_t));
						if(data == NULL)
						{
							EPRINT(ACMS_LOG,"Failed to allocate memory for RDY_EFR Data.Tid:[%d] IID:[%d]",tid,imgId);
							WriteEventLog(EXCEPTIONAL_FACE_AVAILABLE_EVT, UNUSED_FIELD, FAILURE_FACE_IMG, imgId, UNUSED_FIELD, OTHER_FAIL_REASON_EXC_FACE);
						}
						else if((excDataPtr = (ExceptionalFaceReqData_t*)calloc(1,sizeof(ExceptionalFaceReqData_t))) == NULL)
						{
							free(data);
							EPRINT(ACMS_LOG,"Failed to allocate memory for Exceptional Face Data.Tid:[%d] IID:[%d]",tid,imgId);
							WriteEventLog(EXCEPTIONAL_FACE_AVAILABLE_EVT, UNUSED_FIELD, FAILURE_FACE_IMG, imgId, UNUSED_FIELD, OTHER_FAIL_REASON_EXC_FACE);
						}
						else
						{
							InitGetFaceData(data);
							excDataPtr->imageId = imgId;
							data->dataPtr = excDataPtr;
							data->getFaceReqType = EXCEPTIONAL_GET_FACE_REQ;
							data->faceReqType = TYPE_IMG;

							if(InsertGetFaceRequestInQueue(data, INSERT_NODE_ON_END) == FALSE)
							{
								EPRINT(ACMS_LOG,"Failed to insert data in Queue: RDY_EFR. Tid:[%d] IID:[%d]",tid,imgId);
								WriteEventLog(EXCEPTIONAL_FACE_AVAILABLE_EVT, UNUSED_FIELD, FAILURE_FACE_IMG, imgId, UNUSED_FIELD, OTHER_FAIL_REASON_EXC_FACE);
								free(data->dataPtr);
								free(data);
							}
						}
						sendMsgHdr = ACK_EFR;
						procMsgLength = 0;
					}break;

					case RDY_FRC:
					{
						tid = parseField[2];
						GetFaceReqQueueData_t* data = NULL;
						AdaptiveReqData_t* adaptiveReqDataPtr = NULL;
						if(IsMaxAdptvGetFaceReqRecvd())
						{
							DPRINT(ACMS_LOG,"Max Get Face Queue Count Reached");
							WriteEventLog(FACE_IMG_TMPLT_AVAILABLE, UNUSED_FIELD, FAILURE_FACE_IMG, UNUSED_FIELD, OTHER_FAIL_REASON, UNUSED_FIELD);
						}
						else
						{
							UINT16 userIdx = 0;
							if(IsUserValid(parseField[3], &userIdx))
							{
								data = (GetFaceReqQueueData_t*)calloc(1,sizeof(GetFaceReqQueueData_t));
								if(data == NULL)
								{
									EPRINT(ACMS_LOG,"Failed TO allocate Memory: RDY_FRC");
									WriteEventLog(FACE_IMG_TMPLT_AVAILABLE, UNUSED_FIELD, FAILURE_FACE_IMG, UNUSED_FIELD, OTHER_FAIL_REASON, UNUSED_FIELD);
								}
								else if((adaptiveReqDataPtr = (AdaptiveReqData_t*)calloc(1,sizeof(AdaptiveReqData_t))) == NULL)
								{
									EPRINT(ACMS_LOG,"Failed TO allocate Memory for Adpt Data: RDY_FRC");
									free(data);
									WriteEventLog(FACE_IMG_TMPLT_AVAILABLE, UNUSED_FIELD, FAILURE_FACE_IMG, UNUSED_FIELD, OTHER_FAIL_REASON, UNUSED_FIELD);
								}
								else
								{
									InitGetFaceData(data);
									adaptiveReqDataPtr->userId = parseField[3];
									adaptiveReqDataPtr->faceIdx = parseField[4];
									data->dataPtr = adaptiveReqDataPtr;
									data->faceReqType = parseField[5];
									data->getFaceReqType = ADAPTIVE_GET_FACE_REQ;

									if(InsertGetFaceRequestInQueue(data, INSERT_NODE_ON_END) == FALSE)
									{
										EPRINT(ACMS_LOG,"Failed to insert data in Queue: RDY_FRC");
										free(data->dataPtr);
										free(data);
										WriteEventLog(FACE_IMG_TMPLT_AVAILABLE, UNUSED_FIELD, FAILURE_FACE_IMG, UNUSED_FIELD, OTHER_FAIL_REASON, UNUSED_FIELD);
									}
								}
							}
							else
							{
								EPRINT(ACMS_LOG,"Invalid User ID[%llu] for RDY_FRC",parseField[3]);
								WriteEventLog(FACE_IMG_TMPLT_AVAILABLE, UNUSED_FIELD, FAILURE_FACE_IMG, UNUSED_FIELD, FAIL_REASON_USERID_INVALID, UNUSED_FIELD);
							}
						}
						sendMsgHdr = ACK_FRC;
						procMsgLength = 0;
					}break;

					case GET_UFI:
					{
						tid = parseField[2];
						UINT16 userIdx = 0;
						if(FRServerConfig.enableFrServer == TRUE && FRServerConfig.frMode == FR_MODE_LOCAL)
						{
							if(frServerSupportedFeatures.isXmlCreationSupported == ENABLED)
							{
								errUfiStatus = AcmsMsgHandlerGetUficmd(&acmsRecvPtr, acmsMsgProcBuf, &procMsgLength);
								if(errUfiStatus == GET_UFI_SUCCESS)
								{
									sendMsgHdr = ACK_UFI;
									procMsgLength = 0;
								}
								else
								{
									EPRINT(DC_LOG,"sending ERR_UFI = [%d]",errUfiStatus);
									sendMsgHdr = ERR_UFI;
								}
							}
							else
							{
								errUfiStatus = FR_SERVER_ERROR;
								EPRINT(DC_LOG,"GET_UFI Command Not Processed : FR Server Is Having Older Version Or Disconnected Sending response = [%d]",errUfiStatus);
								sendMsgHdr = ERR_UFI;
							}
						}
						else
						{
							sendMsgHdr = ERR_UFI;
							EPRINT(ACMS_LOG,"GET_UFI Command Not Processed : FR Server is Disable/Assited Mode");
							errUfiStatus = FAIL_UNKNOWN_REASON;
						}
					
					}break;

					case SET_CFG:
						acmsDeviceNo = (UINT16)parseField[0];
						AcmsMsgHandlerSetConfig(&acmsRecvPtr, acmsMsgProcBuf, &procMsgLength);
						sendMsgHdr = ACK_CFG;
						break;

					case SET_CMD:
						if(AcmsMsgHandlerSetCmd(&acmsRecvPtr, acmsMsgProcBuf, &procMsgLength) == TRUE)
						{
							acmsMsgWaitTime = CONVERT_SEC_TO_MSEC(SET_TMP_ACK_DELAY);
							acmsMsgTimerTick = GetSysTick();
							acmsMsgState = ACMS_MSG_REPLY_WAIT_STATE;
						}
						sendMsgHdr = ACK_CMD;
						break;

					case SET_TMP:
						if(AcmsMsgHandlerSetTemplate(&acmsRecvPtr, acmsMsgProcBuf, &procMsgLength) == TRUE)
						{
							acmsMsgWaitTime = CONVERT_SEC_TO_MSEC(SET_TMP_ACK_DELAY);
							acmsMsgTimerTick = GetSysTick();
							acmsMsgState = ACMS_MSG_REPLY_WAIT_STATE;
						}
						sendMsgHdr = ACK_TMP;
						break;

					case GET_EVT:
						acmsRollOverCnt = parseField[1];
						acmsEventCnt = parseField[2];

						tempAcmsEventCnt = GetCurEventCount();
						tempAcmsRollOverCnt = GetCurEventRollOverCount();

						DPRINT(ACMS_LOG,"ACMS Rollover [%d] ACMS Event Count [%d]", acmsRollOverCnt, acmsEventCnt);
						DPRINT(ACMS_LOG,"ACS Rollover [%d] ACS Event Count [%d]", tempAcmsRollOverCnt, tempAcmsEventCnt);

						tempAcmsReqCnt = (acmsRollOverCnt * MAX_EVENT_COUNT) + acmsEventCnt;
						tempAcmsCurrentCnt = (tempAcmsRollOverCnt * MAX_EVENT_COUNT) + tempAcmsEventCnt;

						// Requested count can be next one that we don't have yet
						// or earlier upto MAX_EVENT_COUNT
						if( (tempAcmsReqCnt > (tempAcmsCurrentCnt + 1) ) ||
							( (tempAcmsReqCnt + MAX_EVENT_COUNT) <= tempAcmsCurrentCnt) )
						{
							acmsEvtTrxStatus = OFF;
							sendMsgHdr = ERR_EVT;
						}
						else // valid requested count
						{
							acmsEvtTrxStatus = ON;
							sendMsgHdr = STA_EVT;
						}
						break;

					case GET_CRD:
						if(AcmsMsgHandlerGetCardData(&acmsRecvPtr, acmsMsgProcBuf, &procMsgLength) == SUCCESS)
						{
							sendMsgHdr = RPL_CRD;
						}
						else
						{
							sendMsgHdr = ERR_CRD;
						}
						break;

					case GET_TMP:
						if(AcmsMsgHandlerGetTemplate(&acmsRecvPtr, acmsMsgProcBuf, &procMsgLength) == SUCCESS)
						{
							sendMsgHdr = RPL_TMP;
						}
						else
						{
							sendMsgHdr = ERR_TMP;
						}
						break;

					case GET_PIN:
						if(AcmsMsgHandlerGetPINData(&acmsRecvPtr, acmsMsgProcBuf, &procMsgLength) == SUCCESS)
						{
							sendMsgHdr = RPL_PIN;
						}
						else
						{
							sendMsgHdr = ERR_PIN;
						}
						break;

					case PRC_ENR:
						AcmsMsgHandlerProcessEnrollment(&acmsRecvPtr, acmsMsgProcBuf, &procMsgLength);
						sendMsgHdr = ACK_ENR;
						break;

					case STA_FTC:
						// send ack of file transfer start
						sendMsgHdr = ACK_FTC;
						break;

					case GET_DTL:
						AcmsMsgHandlerGetDeviceDetail(acmsMsgProcBuf, &procMsgLength);
						sendMsgHdr = RPL_DTL;
						break;
					}
				}
				else
				{
					EPRINT(ACMS_LOG, "Data Parsign fail ACMS_RECV_MSG_STATE state");
				}
			}
			else
			{
				EPRINT(ACMS_LOG, "Wrong Msg Receive ACMS_RECV_MSG_STATE state");
			}
		}
		else if(ElapsedTick(acmsMsgTimerTick) >= acmsMsgWaitTime)
		{
			DPRINT(ACMS_LOG, "Recv Message Timeout");

			// Wait till poll interval before sending next poll message
			acmsMsgWaitTime = CONVERT_SEC_TO_MSEC(acmsPollInterval);
			acmsMsgTimerTick = GetSysTick();
			acmsNxtMsgState = ACMS_SEND_REQ_POLL_STATE;
			acmsMsgState = ACMS_SEND_RECV_MSG_WAIT_STATE;

			CloseClientSocket(ACMS_MSG_SOCKET);
		}
		break;
	}

	case ACMS_MSG_REPLY_WAIT_STATE:
	{
		NonIDTRequest_t tmpNonIDTRequestData;
		GetFRNonIDTRequestData(&tmpNonIDTRequestData, NON_IDT_TYPE_CMD);

		if(AcmsIpResolveStsState != ACMS_IP_RESOLVE_INACTIVE)//wait for IP to resolve.
		{
			if(AcmsIpResolveStsState == ACMS_IP_RESOLVE_DONE)
			{
				AcmsIpResolveStsState = ACMS_IP_RESOLVE_INACTIVE;
				// Get Interface ip address
				GetServerInterfaceIpAddr(interfaceIpAddr);

				sprintf(acmsMsgSendBuf, "%c%s%c%s%d%c%s%c%d%c%s%cV%02dR%02d%c%s%c%d%c%d%c%c",
						SOT, acmsSendMsgHdr[REQ_LOG], FSP, TenentIdStr,GetCosecDeviceCode(), FSP,
						EthernetParameters.macAddress, FSP, LOGIN_CODE, FSP,
						interfaceIpAddr, FSP, SW_VER, SW_REV, FSP,
						((GetServerIpAddressPtr(acmsCurActiveInterface))), FSP,
						(*(GetServerPortNoPtr(acmsCurActiveInterface))), FSP, SDK_VERSION, FSP, EOT);

				// Send ACMS Message
				SendAcmsData(ACMS_MSG_SOCKET, acmsMsgSendBuf, strlen(acmsMsgSendBuf));

				acmsMsgWaitTime = CONVERT_SEC_TO_MSEC(AcmsPollDuration);
				acmsNxtMsgState = ACMS_SEND_REQ_LOGIN_STATE;
				acmsNxtMsgSuccessState = ACMS_RECV_ACK_LOGIN_STATE;
				acmsMsgState = ACMS_SEND_MSG_WAIT_STATE;

				DPRINT(ACMS_LOG, "Send Login Message --> %s", acmsMsgSendBuf);
			}
		}
		else if(ReadFpTransStatus() != FP_TRANS_STATUS_IDLE)
		{
			if( (ReadFpTransStatus() == FP_TRANS_STATUS_COMPLETE) ||
				(ElapsedTick(acmsMsgTimerTick) >= acmsMsgWaitTime) )
			{
				DeleteFpAckWaitTimer(0);
				FpTransPara.fpTransStatus = FP_TRANS_STATUS_IDLE;
				acmsMsgState = ACMS_MSG_REPLY_STATE;
			}
		}
		else if(tmpNonIDTRequestData.frProcessRequestStatus != INQUEUE_STATUS_RESET)
		{
			if((IsFaceDeleteTaskComplete() == YES) || (ElapsedTick(acmsMsgTimerTick) >= acmsMsgWaitTime))
			{
				memset(acmsMsgProcBuf, 0, sizeof(acmsMsgProcBuf));
				sprintf(acmsMsgProcBuf, "%d%c", tmpNonIDTRequestData.processDeteleFaceTemplateF, FSP);
				procMsgLength = strlen(acmsMsgProcBuf);
				if(tmpNonIDTRequestData.processDeteleFaceTemplateF == FR_DELETE_SUCCESS)
				{
					if(tmpNonIDTRequestData.frProcessRequestId == CMD_USR_DELFACE)
					{
						deleteFaceTemplateForOneUserFromDevice();
					}
					else if(tmpNonIDTRequestData.frProcessRequestId == CMD_USR_CLEAR_ALL_FACE)
					{
						deleteAllFaceTemplateFromDevice();
					}
				}
				tmpNonIDTRequestData.isNonIdtPending = FAIL;
				SetFRNonIDTRequestData(tmpNonIDTRequestData, NON_IDT_TYPE_CMD);
				EPRINT(DC_LOG,"EXITING FROM DELETION PROCESS");
				InitFRNonIDTRequestData(NON_IDT_TYPE_CMD);
				DeleteInvalidNodeWithTxnId(tmpNonIDTRequestData.transactionId);

				acmsMsgState = ACMS_MSG_REPLY_STATE;
			}
		}
		else
		{
			acmsMsgState = ACMS_MSG_REPLY_STATE;
		}
		break;
	}

	case ACMS_MSG_REPLY_STATE:
	{
		if(sendMsgHdr == STA_EVT)
		{
			sprintf(acmsMsgSendBuf, "%c%s%c%s%d%c%d%c%c", SOT, acmsSendMsgHdr[sendMsgHdr], FSP, TenentIdStr, GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP, EOT);
			procMsgLength = strlen(acmsMsgSendBuf);
		}
		else if(sendMsgHdr == ERR_EVT)
		{
			tempAcmsEventCnt = GetCurEventCount();
			tempAcmsRollOverCnt = GetCurEventRollOverCount();

			sprintf(acmsMsgSendBuf, "%c%s%c%s%d%c%d%c%d%c%d%c%c", SOT, acmsSendMsgHdr[sendMsgHdr], FSP, TenentIdStr, GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP, tempAcmsRollOverCnt, FSP, tempAcmsEventCnt, FSP, EOT);
			procMsgLength = strlen(acmsMsgSendBuf);
		}
		else if (sendMsgHdr == ACK_FTC)
		{
			sprintf(acmsMsgSendBuf, "%c%s%c%s%u%c%u%c%u%c%u%c%c", SOT, acmsSendMsgHdr[sendMsgHdr], FSP, TenentIdStr, GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP, SUCCESS, FSP, ACMS_FIRMWARE_FILE_VER, FSP, EOT);
			procMsgLength = strlen(acmsMsgSendBuf);
			// Close any file transfer activity if already running
			if(acmsFtcTrxStatus == ON ||
					acmsFtcState != ACMS_FTC_IDLE_STATE)
			{
				EPRINT(ACMS_LOG,"STA_FTC Encountered between a running FTC transaction ---");
			}
			CloseFtcProcess(TRUE);
			// start file transfer
			acmsFtcTrxStatus = ON;
		}
		else if(sendMsgHdr == ERR_UFI)
		{
			snprintf(acmsMsgSendBuf, MAX_ACMS_PACKET_SZ, "%c%s%c%s%d%c%d%c%llu%c%d%c%c", SOT, acmsSendMsgHdr[sendMsgHdr], FSP, TenentIdStr, GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP, tid, FSP, errUfiStatus, FSP, EOT);
			procMsgLength = strlen(acmsMsgSendBuf);
			errUfiStatus = FAIL_UNKNOWN_REASON;
		}
		else
		{
			switch(sendMsgHdr)
			{
			case RPL_DTL:
			case ACK_CFG:
			case ACK_CMD:
			case ACK_TMP:
			case RPL_TMP:
			case ERR_TMP:
			case RPL_CRD:
			case ERR_CRD:
			case ACK_ENR:
			case ACK_SOR:
			case RPL_PIN:
			case ERR_PIN:
			case ACK_FRC:
			case ACK_EFR:
			case ACK_UFI:
				{
					sprintf(acmsMsgSendBuf, "%c%s%c%s%d%c%d%c%llu%c", SOT, acmsSendMsgHdr[sendMsgHdr], FSP, TenentIdStr, GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP, tid, FSP);
					break;
				}
//				{
//					sprintf(acmsMsgSendBuf, "%c%s%c%s%c%d%c%d%c%llu%c", SOT, acmsSendMsgHdr[sendMsgHdr], FSP, TenentIdStr,FSP, GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP, tid, FSP);
//					break;
//				}
			default:
				sprintf(acmsMsgSendBuf, "%c%s%c%d%c%d%c%llu%c", SOT, acmsSendMsgHdr[sendMsgHdr], FSP, GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP, tid, FSP);
				break;
			}
			length = strlen(acmsMsgSendBuf);

			memcpy((UINT8PTR)&acmsMsgSendBuf[length], (UINT8PTR)acmsMsgProcBuf, procMsgLength);
			procMsgLength += length;
			acmsMsgSendBuf[procMsgLength++] = EOT;
		}

		// Send ACMS Message
		SendAcmsData(ACMS_MSG_SOCKET, acmsMsgSendBuf, procMsgLength);
		DPRINT(ACMS_LOG, "Send Message [%s] Mid[%d] Tid[%llu]", acmsSendMsgHdr[sendMsgHdr], acmsDeviceNo, tid);

		acmsMsgWaitTime = CONVERT_SEC_TO_MSEC(AcmsPollDuration);
		acmsNxtMsgState = ACMS_SEND_REQ_POLL_STATE;
		acmsNxtMsgSuccessState = ACMS_RECV_MSG_STATE;
		acmsMsgState = ACMS_SEND_MSG_WAIT_STATE;
		break;
	}

	case ACMS_SEND_MSG_WAIT_STATE:
	{
		rxTxStatus = GetAcmsTxStatus(ACMS_MSG_SOCKET);

		if(rxTxStatus == SUCCESS)
		{
			acmsMsgWaitTime = CONVERT_SEC_TO_MSEC(AcmsPollDuration);
			acmsMsgTimerTick = GetSysTick();
			acmsMsgState = acmsNxtMsgSuccessState;
		}
		else if(rxTxStatus == FAIL)
		{
			acmsMsgState = ACMS_SEND_RECV_MSG_WAIT_STATE;
			acmsMsgTimerTick = GetSysTick();
		}
		break;
	}

	case ACMS_SEND_RECV_MSG_WAIT_STATE:
	{
		if(ElapsedTick(acmsMsgTimerTick) >= acmsMsgWaitTime)
		{
			acmsMsgState = acmsNxtMsgState;
		}
		break;
	}

	case ACMS_SEND_RECV_MSG_HOLD:
		// Dont do anythig in this state.
		break;
	}

	if(ServerModeCfg.serverMode == COSEC_VYOM)
	{
	    if((GetModemStatus() == CONNECTED) && (acmsReqLoginF == TRUE) && (acmsPrevConnectedActiveInterface == PPP_PORT) && (CONVERT_MSEC_TO_MIN(ElapsedTick(acmsVyomUsbDongleResetTimerTick)) >= ACMS_USB_VYOM_RESET_TIME) )
	    {
		  DPRINT(ACMS_LOG,"Dongle USB MODEM Resetting because of not connected server after Elapsed 10 min");
		  ResetUsbForVyomRst();
		  acmsReqLoginF = FALSE;
	    }
	}
  
}

//*****************************************************************************
//	RunAcmsEventTask()
//	Param:
//		IN:		None
//		OUT: 	None
//	Returns:
//				None
//	Description:
//			This function runs ACMS event send task
//
//*****************************************************************************
static void RunAcmsEventTask(void)
{
	BOOL			rxTxStatus;
	CHAR			acmsEvtSendBuf[MAX_ACMS_PACKET_SZ];
	CHARPTR			acmsRecvPtr;
	UINT16			tempAcmsRollOverCnt;
	UINT32			tempAcmsEventCnt;
	UINT64			tempAcmsReqCnt, tempAcmsCurrentCnt;
	RCV_MSG_HDR_e	acmsRecvMsg;
	UINT16			evtMsgLength, length;
	UINT64			parseField[2];

	static	BOOL	evtTxSuccess = NO;

 	switch(acmsEvtState)
 	{
 	default:
 	case ACMS_SEND_EVT_IDLE_STATE:
 		if(acmsEvtTrxStatus == ON)
 		{
			DPRINT(ACMS_LOG, "Event Transmission Start");
 			acmsEvtState = ACMS_SEND_EVT_STATE;
 		}
 		break;

 	case ACMS_SEND_EVT_STATE:
		if(acmsEvtTrxStatus == OFF)
		{
			acmsEvtState = ACMS_SEND_EVT_IDLE_STATE;
			CloseClientSocket(ACMS_EVENT_SOCKET);
		}
		else
		{
			tempAcmsEventCnt = GetCurEventCount();
			tempAcmsRollOverCnt = GetCurEventRollOverCount();

			tempAcmsReqCnt = (acmsRollOverCnt * MAX_EVENT_COUNT) + acmsEventCnt;
			tempAcmsCurrentCnt = (tempAcmsRollOverCnt * MAX_EVENT_COUNT) + tempAcmsEventCnt;
			if(tempAcmsReqCnt <= tempAcmsCurrentCnt)
			{
				if(ReadAcmsEvent(acmsRollOverCnt, acmsEventCnt, acmsEvtSendBuf, &evtMsgLength) == TRUE)
				{
					SendAcmsData(ACMS_EVENT_SOCKET, acmsEvtSendBuf, evtMsgLength);

					DPRINT(ACMS_LOG, "Send Event --> %s", acmsEvtSendBuf);

					acmsEvtWaitTime = CONVERT_SEC_TO_MSEC(AcmsPollDuration);
					acmsNxtEvtState = ACMS_SEND_EVT_STATE;
					acmsNxtEvtSuccessState = ACMS_RECV_EVT_ACK_STATE;
					acmsEvtState = ACMS_SEND_EVT_WAIT_STATE;
				}
				else
				{
					EPRINT(ACMS_LOG, "Get Event Data Fails");
					acmsEvtWaitTime = CONVERT_SEC_TO_MSEC(acmsPollInterval);
					acmsEvtTimerTick = GetSysTick();
					acmsNxtEvtState = ACMS_SEND_EVT_STATE;
					acmsEvtState = ACMS_SEND_RECV_EVT_WAIT_STATE;
				}
			}
			else if(evtTxSuccess == YES)
			{
				evtTxSuccess = NO;

				sprintf(acmsEvtSendBuf, "%c%s%c%s%d%c%d%c%c", SOT, acmsSendMsgHdr[END_EVT], FSP, TenentIdStr, GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP, EOT);
				evtMsgLength = strlen(acmsEvtSendBuf);

				SendAcmsData(ACMS_EVENT_SOCKET, acmsEvtSendBuf, evtMsgLength);
				DPRINT(ACMS_LOG, "Send End Event --> %s", acmsEvtSendBuf);

				acmsEvtWaitTime = CONVERT_SEC_TO_MSEC(AcmsPollDuration);
				acmsNxtEvtState = ACMS_SEND_EVT_STATE;
				acmsNxtEvtSuccessState = ACMS_SEND_EVT_OVER_STATE;
				acmsEvtState = ACMS_SEND_EVT_WAIT_STATE;
			}
			// else	we need to stay into this state to send out any new generated event as server
			// will not request for new events now
		}
		break;

	case ACMS_RECV_EVT_ACK_STATE:
		rxTxStatus = RecvAcmsData(ACMS_EVENT_SOCKET, &acmsRecvPtr, &length);

		if(rxTxStatus == SUCCESS)
		{
			acmsRecvMsg = GetRecvMsgHeader(acmsRecvMsgHdr, MAX_RCV_MSG_HDR, &acmsRecvPtr);
			if(acmsRecvMsg == ACK_EVT)
			{
				if(GetDataParseField(&acmsRecvPtr, parseField, acmsRecvMsgFspCnt[ACK_EVT]) == SUCCESS)
				{
					// Process only if ack received for sent event with same roll over and event count
					if( (acmsRollOverCnt == (EV_ROLL_CNT)parseField[0]) && (acmsEventCnt == (EV_CNT)parseField[1]) )
					{
						DPRINT(ACMS_LOG, "Receive Event ACK");

						acmsEventCnt++;
						if(acmsEventCnt > MAX_EVENT_COUNT)
						{
							//one rollover increase and start from count zero
							acmsEventCnt = 1;
							acmsRollOverCnt++;
						}
						evtTxSuccess = YES;
						acmsEvtState = ACMS_SEND_EVT_STATE;
						break;
					}
				}
				acmsEvtTimerTick = GetSysTick();
			}
			else if(acmsRecvMsg == NAK_EVT)
			{
				DPRINT(ACMS_LOG, "Receive Event NACK, Event Tx off");

				acmsEvtTrxStatus = OFF;
				acmsEvtState = ACMS_SEND_EVT_STATE;
			}
			else
			{
				DPRINT(ACMS_LOG, "Receive Event Unknown");
			}
		}

		if(ElapsedTick(acmsEvtTimerTick) >= acmsEvtWaitTime)
		{
			DPRINT(ACMS_LOG, "Receive Event ACK Timeout");

			acmsEvtWaitTime = CONVERT_SEC_TO_MSEC(acmsPollInterval);
			acmsEvtTimerTick = GetSysTick();
			acmsNxtEvtState = ACMS_SEND_EVT_STATE;
			acmsEvtState = ACMS_SEND_RECV_EVT_WAIT_STATE;
			CloseClientSocket(ACMS_EVENT_SOCKET);
		}
		break;

	case ACMS_SEND_EVT_OVER_STATE:
		acmsEvtWaitTime = CONVERT_SEC_TO_MSEC(acmsPollInterval);
		acmsEvtTimerTick = GetSysTick();
		acmsNxtEvtState = ACMS_SEND_EVT_STATE;
		acmsEvtState = ACMS_SEND_RECV_EVT_WAIT_STATE;
		CloseClientSocket(ACMS_EVENT_SOCKET);
		break;

	case ACMS_SEND_EVT_WAIT_STATE:
		rxTxStatus = GetAcmsTxStatus(ACMS_EVENT_SOCKET);

		if (rxTxStatus == SUCCESS)
		{
			acmsEvtWaitTime = CONVERT_SEC_TO_MSEC(AcmsPollDuration);
			acmsEvtTimerTick = GetSysTick();
			acmsEvtState = acmsNxtEvtSuccessState;
		}
		else if (rxTxStatus == FAIL)
		{
			acmsEvtTimerTick = GetSysTick();
			acmsEvtState = ACMS_SEND_RECV_EVT_WAIT_STATE;
		}
		break;

	case ACMS_SEND_RECV_EVT_WAIT_STATE:
		if(ElapsedTick(acmsEvtTimerTick) >= acmsEvtWaitTime)
		{
			acmsEvtState = acmsNxtEvtState;
		}
		break;
	}
}

//*****************************************************************************
//	RunAcmsFtcTask()
//	Param:
//		IN:		None
//		OUT: 	None
//	Returns:
//				None
//	Description:
//			This function runs ACMS File Transfer task
//
//*****************************************************************************
static void RunAcmsFtcTask(void)
{
	BOOL				rxTxStatus;
	CHAR				acmsFtcSendBuf[MAX_ACMS_PACKET_SZ];
	CHARPTR				acmsRecvPtr;
	UINT64				parseField[4];
	UINT16				length;
	UINT8				txStatus;
	FTC_ACK_STAT_e		ftcAckStat;

	static	CHAR			acmsMsgProcBuf[MAX_ACMS_PACKET_SZ];
	static	UINT16			procMsgLength;
	static	RCV_FTC_HDR_e	recvFtcHdr;
	static 	SND_FTC_HDR_e	sendFtcHdr;

	switch(acmsFtcState)
	{
		case ACMS_FTC_IDLE_STATE:

			if(acmsFtcTrxStatus == ON)
			{
				// clear last received and send header
				ftcRetryCnt 	= 0;
				lastFtcRecvHdr 	= RCV_FTC_SET_FIL;
				lastFtcSndHdr	= MAX_SND_FTC_HDR;

				acmsFtcState = ACMS_FTC_READY_STATE;
				DPRINT(ACMS_LOG, "Start acms file transfer");
			}

			break;

		case ACMS_FTC_READY_STATE:

			if(strlen(ftcInfo.ftcIdentifier1) == 0)
			{
				sprintf(ftcInfo.ftcIdentifier1, "%s", "0");
			}
			// Send File transfer ready
			sprintf(acmsFtcSendBuf, "%c%s%c%s%u%c%u%c%u%c%s%c%d%c%c", SOT, acmsSendFtcHdr[SND_FTC_RDY], FSP, TenentIdStr, GetCosecDeviceCode(), FSP,
						acmsDeviceNo, FSP, ftcInfo.ftcIdentifierType, FSP,
						ftcInfo.ftcIdentifier1, FSP, ftcInfo.ftcCurrentSeqNo, FSP, EOT);
			procMsgLength = strlen(acmsFtcSendBuf);
			SendAcmsData(ACMS_FTC_SOCKET, acmsFtcSendBuf, procMsgLength);
			// Load timer for file transfer
			LoadFtcProgTimer();
			acmsFtcState = ACMS_FTC_RECV_CMD_STATE;
			DPRINT(ACMS_LOG, "File transfer ready");
			break;

		case ACMS_FTC_REPLY_WAIT_STATE:
		{
			BOOL imgFtcstatus = FTC_STATUS_FAIL;

			NonIDTRequest_t tmpNonIDTRequestData;
			GetFRNonIDTRequestData(&tmpNonIDTRequestData, NON_IDT_TYPE_FTC);

			if(IsFaceEnrollTaskComplete() == YES)
			{
				if(tmpNonIDTRequestData.faceEnrollImgSendSuccessF == SUCCESS)	//decide from face img idt server.
				{
					// move current temporary file
					MoveEnrolledImageToSuccessfullFolder(ftcInfo.ftcIdentifier1, ftcInfo.ftcFilePath, ftcInfo.ftcIdentifier2);
					imgFtcstatus = FTC_STATUS_SUCCESS;
					tmpNonIDTRequestData.faceEnrollImgSendSuccessF = FTC_STATUS_UNKNOWN_ERROR;
					tmpNonIDTRequestData.isNonIdtPending = FAIL;
					SetFRNonIDTRequestData(tmpNonIDTRequestData, NON_IDT_TYPE_FTC);
				}
				else
				{
					imgFtcstatus = FTC_STATUS_FAIL;		//same as ftcstatus flag.

					CHAR cmdBuff[strlen(ftcInfo.ftcFilePath)+7];
					sprintf(cmdBuff,"rm -f %s",ftcInfo.ftcFilePath);
					system(cmdBuff);
				}
				ftcInfo.ftcState = FTC_STATE_COMPLETE;
				// Reset the file transfer information
				ResetFtcActivity();

				sprintf(acmsFtcSendBuf, "%c%s%c%s%u%c%u%c", SOT, acmsSendFtcHdr[SND_FTC_ACK_FIL], FSP, TenentIdStr, GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP);
				length = strlen(acmsFtcSendBuf);

				// set file command reply
				sprintf(acmsMsgProcBuf, "%u%c%u%c",seqNoForEnrlFaceImg, FSP, imgFtcstatus, FSP);

				memcpy(	(UINT8PTR)&acmsFtcSendBuf[length], (UINT8PTR)acmsMsgProcBuf, procMsgLength);
				procMsgLength += length;
				acmsFtcSendBuf[procMsgLength++] = EOT;
				SendAcmsData(ACMS_FTC_SOCKET, acmsFtcSendBuf, procMsgLength);

				InitFRNonIDTRequestData(NON_IDT_TYPE_FTC);
				//after sending ack check for END_FIL in recv_cmd_state.
				acmsFtcState = ACMS_FTC_RECV_CMD_STATE;
			}
		}break;

		case ACMS_FTC_RECV_CMD_STATE:
			// Receive file transfer commands and forward it to file transfer handler
			rxTxStatus = RecvAcmsData(ACMS_FTC_SOCKET, &acmsRecvPtr, &length);
			if(rxTxStatus == SUCCESS)
			{
				recvFtcHdr = GetRecvMsgHeader(acmsRecvFtcHdr, MAX_RCV_FTC_HDR, &acmsRecvPtr);
				// If valid header is available
				if(recvFtcHdr < MAX_RCV_FTC_HDR)
				{
					// check for controller type and mid
					if(GetDataParseField(&acmsRecvPtr, parseField, acmsRecvFtcFspCnt[recvFtcHdr]) == SUCCESS)
					{
						if( (parseField[0] == GetCosecDeviceCode() ) && (parseField[1] == acmsDeviceNo) )
						{
							lastFtcRecvHdr	= recvFtcHdr;

							switch(recvFtcHdr)
							{
								case RCV_FTC_SET_FIL:
								{

									NonIDTRequest_t tmpNonIDTRequestData;
									// Load timer for file transfer
									LoadFtcProgTimer();
									ftcRetryCnt = 0;

									AcmsFtcHandlerSetFile(&acmsRecvPtr, acmsMsgProcBuf, &procMsgLength);
									GetFRNonIDTRequestData(&tmpNonIDTRequestData, NON_IDT_TYPE_FTC);
									if(tmpNonIDTRequestData.faceImgRcvdF == TRUE)
									{
										tmpNonIDTRequestData.faceImgRcvdF = FALSE;
										SetFRNonIDTRequestData(tmpNonIDTRequestData, NON_IDT_TYPE_FTC);
										acmsFtcState = ACMS_FTC_REPLY_WAIT_STATE;
									}
									else	// do not send ack on enroll face image received. Wait for acknowledgment(in ACMS_FTC_REPLY_WAIT_STATE) from idt server.
									{
										sprintf(acmsFtcSendBuf, "%c%s%c%s%u%c%u%c", SOT, acmsSendFtcHdr[SND_FTC_ACK_FIL], FSP, TenentIdStr, GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP);
										length = strlen(acmsFtcSendBuf);
										memcpy(	(UINT8PTR)&acmsFtcSendBuf[length], (UINT8PTR)acmsMsgProcBuf, procMsgLength);
										procMsgLength += length;
										acmsFtcSendBuf[procMsgLength++] = EOT;
										SendAcmsData(ACMS_FTC_SOCKET, acmsFtcSendBuf, procMsgLength);
									}
									break;
								}

								case RCV_FTC_GET_FIL:
								{
									LoadFtcProgTimer();

									ftcRetryCnt = 0;

									if(AcmsFtcHandlerGetFile(&acmsRecvPtr, acmsMsgProcBuf, &procMsgLength) == FAIL)
									{
										sendFtcHdr = SND_FTC_ERR_FIL;
									}
									else
									{
										if(AcmsFtcHandlerRplFile(acmsMsgProcBuf,&procMsgLength) == FAIL)
										{
											sendFtcHdr = SND_FTC_ERR_FIL;
										}
										else
										{
											sendFtcHdr = SND_FTC_RPL_FIL;
										}
									}

									acmsFtcState = ACMS_FTC_REPLY_CMD_STATE;

									break;
								}

								case RCV_FTC_END_FIL:
								{
									// System reboot timer is running then hold all communication.
									if(sysRebootTmrHandle != INVALID_TIMER_HANDLE)
									{
										LogoutAcms(ACMS_COMM_HOLD, DO_NOT_CLOSE_ALL_SERVER_SOCKETS);
									}

									CloseFtcProcess(0);
									break;
								}

								case RCV_FTC_ACK_FIL:
								{
									LoadFtcProgTimer();

									ftcRetryCnt = 0;

									ftcAckStat = AcmsFtcHandlerAckFile(&acmsRecvPtr, acmsMsgProcBuf,&procMsgLength);
									if(ftcAckStat != FTC_ACK_DUPLICATE)
									{
										if(ftcInfo.ftcState == FTC_STATE_COMPLETE)
										{
											sendFtcHdr = SND_FTC_END_FIL;
										}
										else if(ftcAckStat == FTC_ACK_FAIL ||
												AcmsFtcHandlerRplFile(acmsMsgProcBuf,&procMsgLength) == FAIL)
										{
											sendFtcHdr = SND_FTC_ERR_FIL;
										}
										else
										{
											sendFtcHdr = SND_FTC_RPL_FIL;
										}
										acmsFtcState = ACMS_FTC_REPLY_CMD_STATE;
									}

									break;
								}

								default:
									// Discard messages as it is invalid for
									// this controller
									lastFtcRecvHdr = MAX_RCV_FTC_HDR;
									break;
							}
						}
					}
				}
			}
			break;

		case ACMS_FTC_REPLY_CMD_STATE:
		{
			lastFtcSndHdr = sendFtcHdr;

			if( sendFtcHdr == SND_FTC_ERR_FIL )
			{
				sprintf(acmsFtcSendBuf, "%c%s%c%s%u%c%u%c", SOT,
							acmsSendFtcHdr[SND_FTC_ERR_FIL], FSP, TenentIdStr,
							GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP);
				length = strlen(acmsFtcSendBuf);
				memcpy(	(UINT8PTR)&acmsFtcSendBuf[length],
									(UINT8PTR)acmsMsgProcBuf, procMsgLength);
				length += procMsgLength;
				acmsFtcSendBuf[length++] = EOT;
				SendAcmsData(ACMS_FTC_SOCKET, acmsFtcSendBuf, length);

				acmsFtcState		= ACMS_FTC_WAIT_CLOSE_SOCKET_STATE;
			}
			else if( sendFtcHdr == SND_FTC_RPL_FIL)
			{
				sprintf(acmsFtcSendBuf, "%c%s%c%s%u%c%u%c", SOT,
							acmsSendFtcHdr[SND_FTC_RPL_FIL], FSP, TenentIdStr,
							GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP);
				length = strlen(acmsFtcSendBuf);
				memcpy(	(UINT8PTR)&acmsFtcSendBuf[length],
									(UINT8PTR)acmsMsgProcBuf, procMsgLength);
				length += procMsgLength;
				acmsFtcSendBuf[length++] = EOT;

				SendAcmsData(ACMS_FTC_SOCKET, acmsFtcSendBuf, length);

				acmsFtcState	= ACMS_FTC_RECV_CMD_STATE;
			}
			else if( sendFtcHdr == SND_FTC_END_FIL)
			{
				AcmsFtcEndFilHandler();
				sprintf(acmsFtcSendBuf, "%c%s%c%s%u%c%u%c", SOT,
							acmsSendFtcHdr[SND_FTC_END_FIL], FSP, TenentIdStr,
							GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP);
				length = strlen(acmsFtcSendBuf);

				acmsFtcSendBuf[length++] = EOT;
				SendAcmsData(ACMS_FTC_SOCKET, acmsFtcSendBuf, length);

				acmsFtcState	= ACMS_FTC_WAIT_CLOSE_SOCKET_STATE;
			}
			break;
		}

		case ACMS_FTC_WAIT_CLOSE_SOCKET_STATE:
		{
			txStatus = GetAcmsTxStatus(ACMS_FTC_SOCKET);

			if(txStatus != WAIT)
			{
				CloseFtcProcess(0);
			}

			break;
		}

		default:
			CloseFtcProcess(0);
			break;
	}
}

//*****************************************************************************
//	GetAcmsStatus()
//	Param:
//		IN:	NONE
//		OUT:NONE
//	Returns:
//		status-status of ACMS.
//	Description:
//			It returns communication status of ACMS module (Login or Logout).
//
//*****************************************************************************
ACMS_COMM_STATUS_e GetAcmsStatus(void)
{
 	return acmsLoginStatus;
}

//*****************************************************************************
//	LogoutAcms()
//	Param:
//		IN:	NONE
//		OUT:NONE
//	Returns:
//		None
//	Description:
//			This function Logout ACMS and Start Login again
//
//*****************************************************************************
void LogoutAcms(ACMS_COMM_LOGOUT_MODE_e logoutMode, BOOL commPortParaChangeF)
{
	// lock the resource to avoid simultaneous access
	pthread_mutex_lock(&resourceMutex);

	memset(ServerConnectionCheckFlags, 0xFF, sizeof(ServerConnectionCheckFlags));

	// To help device come online easily next time
	// Load default poll duration only if last set poll duration is less than the default value
	if(AcmsPollDuration < DEFAULT_POLL_DURATION)
		AcmsPollDuration = DEFAULT_POLL_DURATION;
	// Load default poll interval only if last set poll interval is less than the default value
	if(acmsPollInterval < DEFAULT_POLL_INTERVAL)
		acmsPollInterval = DEFAULT_POLL_INTERVAL;

	acmsLoginStatus = ACMS_OFFLINE;
	// Update server status on LCD status bar
	UpdateDisplayStatusField();
	// NOTE: Must be called after ACMS status is changed to OFFLINE
	// Pass invalid led status to display led status as per current interface status
	SetNwStatusCad(acmsCurActiveInterface, INVALID_LED_STATUS);
	acmsEvtTrxStatus = OFF;
	CloseClientSocket(ACMS_MSG_SOCKET);
	
	//Send Connectivity status to External Reader
	SendDoorConnectivityStatusToExternalReader();

	if(commPortParaChangeF == CLOSE_ALL_SERVER_SOCKETS)
	{
		CloseClientSocket(ACMS_EVENT_SOCKET);
		CloseClientSocket(ACMS_FTC_SOCKET);
	}

	// On Logout restart Interface Priority Selection
	RestartInterfacePriority();

	if(logoutMode  == ACMS_COMM_LOGOUT)
	{
		/** Note: On LogoutAcms clear server ip only for last active interface.
		 * 	Changes done for FIT-4942(JIRA Task Id ACTASWSP1-1251)
		 * 	Scenario: If server hostname is given instead of server ip address and device's both interfaces i.e Wifi(With connection to server)
		 * 	and Ethernet(Without connection to server) are Connected. Along with that server hostname resolutions takes little longer e.g 2-3 minutes.
		 * 	Then on first instance of server hostname resolution through wifi and server connectivity check success through wifi, device's preferred 
		 * 	interface gets updated to wifi as connectivity with server is possible through wifi. Hence acms logout on Lan happens. 
		 * 	Issue: On older implementation server ip for all interfaces gets cleared on logout.
		 * 	which led to connectivity check failure on all interfaces(As hots name resolution took time and server ip cleared for all interfaces) for 
		 * 	next iteration of connectivity check in thread InterfacePrioritySelect. So lan is set as preferred interface and acms logout from wlan
		 * 	(Again clearing of server ips) happens. This kept repeating and connection with server never happens. 
		 * 	Solution: So we have only cleared server ip address for last active interface on acms logout instead of all interfaces so on next inetration of 
		 * 	connectivity check device connectivity with wifi remains intact.
		*/
		switch(lastActiveComInterface)
		{
			default:
			case ETHERNET_PORT:
			{
				snprintf(ServerParameters.ethServerIpAddress, MAX_IP_ADDRESS, "%s", "");
				break;
			}
			case WIFI_PORT:
			{
				snprintf(ServerParameters.wifiServerIpAddress, MAX_IP_ADDRESS, "%s", "");
				break;
			}
			case PPP_PORT:
			{
				snprintf(ServerParameters.modemServerIpAddress, MAX_IP_ADDRESS, "%s", "");
				break;
			}
		}

		TempServerParaVyom = ServerParameters;
		acmsMsgState = ACMS_SEND_REQ_LOGIN_STATE;
		DPRINT(ACMS_LOG, "ACMS Communication Logout ........ \n");
	}
	else
	{
		acmsMsgState = ACMS_SEND_RECV_MSG_HOLD;
		DPRINT(ACMS_LOG, "ACMS Communication Hold........ \n");
	}

	// unlock the resource
	pthread_mutex_unlock(&resourceMutex);
}

//*****************************************************************************
//	ReadAcmsEvent()
//	Param:
//		IN:	rolloverCount, Eventcount
//		OUT:Event read Buffer, event length
//	Returns:
//		None
//	Description:
//			This function reads event and write in buf
//
//*****************************************************************************
BOOL ReadAcmsEvent(UINT16 rollCnt, UINT32 evCnt, CHARPTR txData, UINT16PTR lenPtr)
{
	UINT16	msgLength;

	sprintf(txData, "%c%s%c%s%d%c%d%c", SOT, acmsSendMsgHdr[RPL_EVT], FSP, TenentIdStr, GetCosecDeviceCode(), FSP, acmsDeviceNo, FSP);
	*lenPtr = strlen(txData);

	if(AcmsMsgHandlerGetEvent(rollCnt, evCnt, (txData + *lenPtr), &msgLength) == TRUE)
	{
		*lenPtr += msgLength;
		txData[*lenPtr] = EOT;
		*lenPtr += 1;
		txData[*lenPtr] = '\0';
		return SUCCESS;
	}
	return FAIL;
}

//*****************************************************************************
//	WriteAcmsConfig()
//	Param:
//		IN:	confDataPtr
//		OUT:NONE
//	Returns:
//		TRUE/FALSE (True --> Wait for 3 Second) False --> dont wait
//	Description:
//			This function writes data as configuration, commands.
//
//*****************************************************************************
BOOL WriteAcmsConfig(CHARPTR confDataPtr)
{
	BOOL			retVal = FALSE;
	CHAR			msgBuf[255];
	UINT16			mLength;
	UINT64			parseField[4];
	RCV_MSG_HDR_e	acmsRecvMsg;

	acmsRecvMsg = GetRecvMsgHeader(acmsRecvMsgHdr, MAX_RCV_MSG_HDR, &confDataPtr);

	if(acmsRecvMsg < MAX_RCV_MSG_HDR)
	{
		if(GetDataParseField(&confDataPtr, parseField, acmsRecvMsgFspCnt[acmsRecvMsg]) == SUCCESS)
		{
			switch(acmsRecvMsg)
			{
				default:
					break;

				case SET_CFG:
					AcmsMsgHandlerSetConfig(&confDataPtr, msgBuf, &mLength);
					break;

				case SET_CMD:
					if(AcmsMsgHandlerSetCmd(&confDataPtr, msgBuf, &mLength) == SUCCESS)
					{
						retVal = TRUE;
					}
					break;

				case SET_TMP:
					if(AcmsMsgHandlerSetTemplate(&confDataPtr, msgBuf, &mLength) == SUCCESS)
					{
						retVal = TRUE;
					}
					break;
			}
		}
	}
	return retVal;
}

//*****************************************************************************
//	LoadFtcProgTimer
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function will Start timer for file transfer is in progress.
//
//****************************************************************************
static BOOL LoadFtcProgTimer(void)
{
	TIMER_INFO_t timer;

	acmsMsgTimerTick = GetSysTick(); //Reload Msg Socket Timer as for consecutive FTC, after 1st ACK_FTC, server will not send any command till ftc transaction completes.
	//This may take more than Poll Duration. So, update tick on every ftc message received too so that timeout donot occur on recv_wait_state on message socket.
	if(acmsFtcProgTmrHandle == INVALID_TIMER_HANDLE)
	{
		timer.count   = (AcmsPollDuration * 10);
		timer.data 	  = 0;
		timer.funcPtr = &CloseFtcProcess;

		return(StartTimer(timer, &acmsFtcProgTmrHandle,__FUNCTION__) );
	}
	else
	{
		return(ReloadTimer(acmsFtcProgTmrHandle, (AcmsPollDuration * 10) ) );
	}
}

//*****************************************************************************
//	CloseFtcProcess()
//	Param:
//		IN:		UINT32 forceClose: To force close ftc. If true do not retry.
//		OUT:	None
//	Returns:
//		None
//	Description:
//		This function will close file trasnfer activity.
// NOTE: it also closes socket. As CloseFtcProcess is also a timeout function
// forceClose is UINT32
//*****************************************************************************
static void CloseFtcProcess(UINT32 forceClose)
{
	BOOL closeFtcProcess = TRUE;
	BOOL ftcRetryApply = FALSE;
	BACKUP_UPDATE_TYPE_e buType = BU_TYPE_FIRMWARE;

	if(forceClose == FALSE)
	{
		if(acmsFtcState == ACMS_FTC_RECV_CMD_STATE)
		{
			if(lastFtcRecvHdr == RCV_FTC_SET_FIL)
			{
				ftcRetryApply = TRUE;
			}
			else if(lastFtcSndHdr == SND_FTC_RPL_FIL)
			{
				ftcRetryApply = TRUE;
				acmsFtcState = ACMS_FTC_REPLY_CMD_STATE;
			}

			if(ftcRetryApply == TRUE)
			{
				ftcRetryCnt++;

				if(ftcRetryCnt <= ACMS_FTC_RETRY_COUNT)
				{
					DPRINT(ACMS_LOG, "ACMS FILE TRANSFER RETRY CNT [%d]", ftcRetryCnt);
					closeFtcProcess = FALSE;
					LoadFtcProgTimer();
				}
				else
				{
					DPRINT(ACMS_LOG, "ACMS FILE TRANSFER RETRY REACHED MAXIMUM COUNT");
				}
			}
		}
	}

	if(closeFtcProcess == TRUE || forceClose == TRUE)
	{
		HandleFtcCloseProcess();
		DPRINT(SYS_LOG,"File Transfer close for [%d]",ftcInfo.ftcIdentifierType);
		// delete timer of file transfer
		DeleteTimer(&acmsFtcProgTmrHandle);

		if(acmsFtcTrxStatus == ON ||
							acmsFtcState != ACMS_FTC_IDLE_STATE)
		{
			if(SuccessfullyGetImageTmpltFtcDoneF != true)
			{
				if(ftcInfo.ftcIdentifierType == FTC_IDENTIFIER_TYPE_GET_FACE_TMPLT)
				{
					if(GetGetFaceReqTmpltState() == GET_FACE_PROCESSING_FTC)
					{
						SetGetFaceReqTmpltState(GET_FACE_DONE_FR);
						DPRINT(ACMS_LOG,"Failed Get Template FTC ForceClose:[%d]",forceClose);
					}
				}
				else if(ftcInfo.ftcIdentifierType == FTC_IDENTIFIER_TYPE_GET_USER_FACE_IMAGE ||
						ftcInfo.ftcIdentifierType == FTC_IDENTIFIER_TYPE_GET_EXCEP_FACE_IMG)
				{
					if(GetGetFaceReqImageState() == GET_FACE_PROCESSING_FTC)
					{
						SetGetFaceReqImageState(GET_FACE_DONE_FR);
						DPRINT(SYS_LOG,"Failed Get Image FTC ForceClose:[%d]",forceClose);
					}
				}
			}
		}

		if( (ftcInfo.ftcCurrentSeqNo == ftcInfo.ftcEndSeqNo) || (ftcInfo.ftcCmd == FTC_CMD_GET_FIL) ||
				(ftcInfo.ftcIdentifierType != FTC_IDENTIFIER_TYPE_FIRMWARE))
		{
			// Reset ftc ongoing activity
			ResetFtcActivity();
		}
		// Close connection
		CloseClientSocket(ACMS_FTC_SOCKET);
		// Set acms file transfer mode to idle
		acmsFtcState = ACMS_FTC_IDLE_STATE;
		ftcInfo.ftcState = FTC_STATE_IDLE;
		// switch off to acms file transfer
		acmsFtcTrxStatus = OFF;

		SuccessfullyGetImageTmpltFtcDoneF = false;
		// Switch to idle task
		if( (DcInfo.curState == DC_ACMS_FIRMWARE_UPLOAD_STATE) && (fwUpgradeStatus == INACTIVE) )
		{
			SetAudioVisual(USER_RESP_BACKUP_UPDATE_XFER_FAILED, INTERNAL_PORT, &buType);
			SwitchDcToIdleState();
		}

		fwUpgradeStatus = INACTIVE;
		DPRINT(ACMS_LOG,"File transfer end");
	}
}

//*****************************************************************************
//	AcmsFwUpgradeThread
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function will Start timer to wait for upgrading firmware for
//			few seconds.
//****************************************************************************
static VOIDPTR AcmsFwUpgradeThread(UNUSED_PARA VOIDPTR arg)
{
	BACKUP_UPDATE_TYPE_e			buType = BU_TYPE_FIRMWARE;
	FIRMWARE_UPGRADE_STATUS_e		fwStatus;

	fwStatus = StartSystemFwUpgrade((VOIDPTR)acmsFwUpgradeFilePath, ONLY_BM_CHECK);

	if(fwStatus == FIRMWARE_UPGRADE_SUCCESS)
	{
		SetAudioVisual(USER_RESP_BACKUP_UPDATE_XFER_SUCCESS, INTERNAL_PORT, &buType);
	}
	else if(fwStatus == FIRMWARE_UPGRADE_CRC_FAIL)
	{
		SetAudioVisual(USER_RESP_BACKUP_UPDATE_CHECKSUM_FAIL, INTERNAL_PORT, &buType);
	}
	else if(fwStatus == FIRMWARE_UPGRADE_DOWNGRADE_CNTRL)
	{
		SetAudioVisual(USER_RESP_BACKUP_UPDATE_DOWNGRAGE_CNTRL, INTERNAL_PORT, &buType);
	}
	else if(fwStatus == FIRMWARE_UPGRADE_BENCHMARK_FOUND)
	{
		SetAudioVisual(USER_RESP_BENCHMARK_FW_FOUND, INTERNAL_PORT, &buType);
	}
	else
	{
		SetAudioVisual(USER_RESP_BACKUP_UPDATE_XFER_FAILED, INTERNAL_PORT, &buType);
	}

	SwitchDcToIdleState();
	fwUpgradeThreadStatus = THREAD_DONE;
	pthread_exit(NULL);
}

//*****************************************************************************
//	AcmsStartFwUpgradeThread
//	Param:
//		IN :	fileStr - file name with absolute path detail
//		OUT:	NONE
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function will Start thread to upgrade firmware in background
//****************************************************************************
BOOL AcmsStartFwUpgradeThread(CHARPTR fileStr)
{
	BOOL status = FAIL;

	// If thread is not active already then only activate thread
	//if previously thread is not created sucessfully or previously
	//thread is completed or it is first time then only activate
	if( (fwUpgradeThreadStatus == THREAD_INACTIVE) || (fwUpgradeThreadStatus == THREAD_DONE) )
	{
		fwUpgradeThreadStatus = THREAD_INACTIVE;
		strcpy(acmsFwUpgradeFilePath, fileStr);

		if(pthread_create(&fwUpgradeThreadData, NULL, AcmsFwUpgradeThread, NULL) == 0)
		{
			// In case thread is completed just before call reaches here (which is observed),
			// do not change thread status to active
			if(fwUpgradeThreadStatus == THREAD_INACTIVE)
			{
				fwUpgradeThreadStatus = THREAD_ACTIVE;
			}
			// Thread created successfully now detach thread
			pthread_detach(fwUpgradeThreadData);
			fwUpgradeStatus = ACTIVE;
			status = SUCCESS;
		}
		else
		{
			EPRINT(ACMS_LOG, "Failed to create thread for firmware upgrade");
		}
	}
	else
	{
		EPRINT(ACMS_LOG, "Thread for firmware upgrade is already running");
	}
	return status;
}

//*****************************************************************************
//	GetDeviceMID
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	device MID
//	Description:
//			This function will return MID of device
//****************************************************************************
UINT16 GetDeviceMID(void)
{
	return acmsDeviceNo;
}

//*****************************************************************************
//	RunServerCmdRespRcvTask()
//	Param:
//	IN: 	NONE
//			NONE
//	OUT: 	NONE
//	Returns: NONE
//
//	Description:
//			This function wait for command response data received from server.
//		Response contains user balance is sufficient or not for current transaction.
//*****************************************************************************
void RunServerCmdRespRcvTask(void)
{
	switch(cmdProcInfo.cmdProcState)
	{
		default:
			break;

		case SERVER_CMD_MSG_IDLE_STATE:
			break;

		case SERVER_CMD_MSG_SEND_STATE:
		{
			acmsSrvrCmdWaitTime 	= CONVERT_SEC_TO_MSEC(AcmsPollDuration);
			acmsSrvrCmdTimerTick 	= GetSysTick();
			cmdProcInfo.cmdProcState = SERVER_CMD_MSG_SEND_WAIT_STATE;
		}
		break;

		case SERVER_CMD_MSG_SEND_WAIT_STATE:
		{
			UINT8	sentStat;

			sentStat = GetAcmsTxStatus(SERVER_CMD_SOCKET);
			if(sentStat == SUCCESS)
			{
				acmsSrvrCmdWaitTime 	= CONVERT_SEC_TO_MSEC(AcmsPollDuration);
				acmsSrvrCmdTimerTick 	= GetSysTick();
				DPRINT(ACMS_LOG, "send SERVER command to server SUCCESS");
				cmdProcInfo.cmdProcState = SERVER_CMD_MSG_RESP_STATE;
			}
			else if(sentStat == FAIL)
			{
				DPRINT(ACMS_LOG, "send server command to server FAIL");
				cmdProcInfo.statusF = FAIL;
				ProcMsg(MSG_ACMS_SERVER_CMD_ACK_RECEIVED_EVENT, NULL, 0);
				AbortServerCmd();
			}
			else if(ElapsedTick(acmsSrvrCmdTimerTick) >= acmsSrvrCmdWaitTime)
			{
				cmdProcInfo.statusF = FAIL;
				ProcMsg(MSG_ACMS_SERVER_CMD_ACK_RECEIVED_EVENT, NULL, 0);
				AbortServerCmd();
			}
			break;
		}

		case SERVER_CMD_MSG_RESP_STATE:
		{
			CHARPTR				rcvCmdPtr;
			UINT16				rcvCmdlen;
			RCV_CMD_HDR_e		rcvCmdHdr;
			UINT64				field;
			BOOL				respValidF = FALSE;
			UINT8 				ItemIdx;
			CHAR				strDataBuf[15];

			//If complete packet received then we check its header(deviceid, mid, tid etc..), if valid than
			//take appropriate action and close the socket otherwise wait for valid packet
			if(RecvAcmsData(SERVER_CMD_SOCKET, &rcvCmdPtr, &rcvCmdlen) == SUCCESS)
			{
				rcvCmdHdr = GetRecvMsgHeader((CHARPTR *)recvCmdHdr, MAX_RCV_MSG_HDR, &rcvCmdPtr);
				switch(rcvCmdHdr)
				{
					case ACK_BAL:
					case ACK_PIN:
						if( (ParseField(&rcvCmdPtr, &field) == SUCCESS) && (field == GetCosecDeviceCode()) )
						{
							//check for mid
							if ( (ParseField(&rcvCmdPtr, &field) == SUCCESS) && (field == SysBasicPara.devId) )
							{
								//check for TID
								if ( (ParseField(&rcvCmdPtr, &field) == SUCCESS) && (field == cmdProcInfo.cmdTransactionId) )
								{
									if( ( ParseField(&rcvCmdPtr, &field) == SUCCESS) && (field <= SUCCESS) )
									{
										respValidF = TRUE;
										cmdProcInfo.data = field;
									}
								}
							}
						}
					break;

					case ACK_CUP:
					{
						// Type
						if( (ParseField(&rcvCmdPtr, &field) != SUCCESS) || (field != GetCosecDeviceCode()) )
							break;

						// MID
						if ( (ParseField(&rcvCmdPtr, &field) != SUCCESS) || (field != SysBasicPara.devId) )
							break;

						// TID
						if ( (ParseField(&rcvCmdPtr, &field) != SUCCESS) || (field != cmdProcInfo.cmdTransactionId) )
							break;

						// Reserved1
						if( ( ParseField(&rcvCmdPtr, &field) != SUCCESS) )
							break;

						// Reserved2
						if( ( ParseField(&rcvCmdPtr, &field) != SUCCESS) )
							break;

						// Reserved3
						if( ( ParseField(&rcvCmdPtr, &field) != SUCCESS) )
							break;

						// User Cafe Balance
						cafeMenuNavigation.userCafeBalanceReceivedF = NO;
						if(ParseString(&rcvCmdPtr, strDataBuf, sizeof(strDataBuf)) != SUCCESS)
							break;

						if('\0' != strDataBuf[0])
						{
							DPRINT(DC_LOG,"Cafe balance value received = [%s]",strDataBuf);
							FLOAT64 tempBalance = atof(strDataBuf);
							if(tempBalance >= (MAX_CAFE_USER_BALANCE_NEGATIVE) && tempBalance <= (MAX_CAFE_USER_BALANCE_POSITIVE))
							{
								cafeMenuNavigation.userCafeBalanceReceivedF = YES;
								cafeMenuNavigation.userCafeBalance = FloatStr32ToInt32(strDataBuf);
							}
							else
							{
								EPRINT(ACMS_LOG,"Received User Cafe Balance Value is Out Of Range");
							}
						}

						// Weekly Pending Limit
						cafeMenuNavigation.weeklyPendingLimitReceivedF = NO;
						if(ParseString(&rcvCmdPtr, strDataBuf, sizeof(strDataBuf)) != SUCCESS)
							break;

						if('\0' != strDataBuf[0])
						{
							cafeMenuNavigation.weeklyPendingLimitReceivedF = YES;
							cafeMenuNavigation.weeklyPendingLimit = Float32ToUint32(atof(strDataBuf));
						}

						// Response Code
						if( ( ParseField(&rcvCmdPtr, &field) != SUCCESS) )
							break;

						cafePolicyCheckResp.responseCode = (UINT8)field;
						if(cafePolicyCheckResp.responseCode < MAX_CAFE_POLICY)
						{
							DPRINT(DC_LOG,"responseCode = [%d]",cafePolicyCheckResp.responseCode);
							respValidF = TRUE;
							cafePolicyRespReceivedF = TRUE;

							for(ItemIdx=0; ItemIdx<MAX_CAFE_MENU_ITEMS; ItemIdx++)
							{
								if(ParseString(&rcvCmdPtr, strDataBuf, sizeof(strDataBuf)) == SUCCESS)
								{
									UINT8 itemID, itemRespCode;

									if(ParseItemIDAndResp( strDataBuf, &itemID, &itemRespCode) == SUCCESS)
									{
										cafePolicyCheckResp.cafeItemIdx[ItemIdx] = itemID;
										cafePolicyCheckResp.cafeItemRespCode[ItemIdx] = itemRespCode;
									}
									else
									{
										break;
									}
								}
								else
								{
									break;
								}
							}
							cafePolicyCheckResp.totalItems = ItemIdx;
						}
					}
					break;

					default:
						EPRINT(ACMS_LOG, "Unknown message received from server");
						return;
				}
				DPRINT(ACMS_LOG, "Received SERVER command ack from server SUCCESS, time[%u ms] respValidF=[%u]", ElapsedTick(acmsSrvrCmdTimerTick),respValidF);
				if(respValidF == TRUE)
				{
					cmdProcInfo.statusF = SUCCESS;
					AbortServerCmd();
					ProcMsg(MSG_ACMS_SERVER_CMD_ACK_RECEIVED_EVENT, NULL, 0);
				}
				else if(ElapsedTick(acmsSrvrCmdTimerTick) >= acmsSrvrCmdWaitTime)
				{
					cmdProcInfo.statusF = FAIL;
					ProcMsg(MSG_ACMS_SERVER_CMD_ACK_RECEIVED_EVENT, NULL, 0);
					AbortServerCmd();
				}
			}
			else if(ElapsedTick(acmsSrvrCmdTimerTick) >= acmsSrvrCmdWaitTime)
			{
				cmdProcInfo.statusF = FAIL;
				ProcMsg(MSG_ACMS_SERVER_CMD_ACK_RECEIVED_EVENT, NULL, 0);
				AbortServerCmd();
			}
			break;
		}
	}
}

//*****************************************************************************
//	AbortServerCmd()
//	Param:
//		IN :	NONE
//		OUT:	None
//	Returns:	None
//	Description:
//*****************************************************************************
void AbortServerCmd(void)
{
	CloseClientSocket(SERVER_CMD_SOCKET);
	cmdProcInfo.cmdProcState = SERVER_CMD_MSG_IDLE_STATE;
}

//*****************************************************************************
//	HandleFtcCloseProcess()
//	Param:
//		IN :	NONE
//		OUT:	None
//	Returns:	None
//	Description: Handles processes need to be done just before closing the ftc process.
//*****************************************************************************
static void HandleFtcCloseProcess()
{
	switch(ftcInfo.ftcIdentifierType)
	{
		case FTC_IDENTIFIER_TYPE_SET_USER_FACE_TMPL:
		{
			if(acmsFtcState == ACMS_FTC_REPLY_WAIT_STATE)
			{
				NonIDTRequest_t	tmpNonIDTRequestData;
				GetFRNonIDTRequestData(&tmpNonIDTRequestData, NON_IDT_TYPE_FTC);
				FindAndDeleteNonIdtReq(tmpNonIDTRequestData.transactionId);
			}
		}break;
		default:
			break;
	}
}

//*****************************************************************************
//	GetMaxTimeRequiredForAcmsTransaction()
//	Param:
//		IN :	NONE
//		OUT:	None
//	Returns:	UINT16
//	Description:This function return Max time for complete acms transaction
//*****************************************************************************
UINT16 GetMaxTimeRequiredForAcmsTransaction(VOID)
{
	return (AcmsPollDuration*MAX_TRANSACTION_WITH_ACMS);
}
