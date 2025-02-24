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
//   Project      : NGT (Next Generation Terminal)
//   Created By   : Kaival Shah
//   File         : MxGuiHandler.c
//   Description  : This file communicates with Qt Application
///////////////////////////////////////////////////////////////////////////

//********Place all include files ****
//system Includes
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

//matrix Includes.
#include "MxTypedef.h"
#include "MxDcTask.h"
#include "MxSysTimer.h"
#include "MxLogs.h"
#include "MxSlaveCommonMsgList.h"
#include "MxGuiHandler.h"
#include "MxMobileBroadband.h"
#include "MxWifiPort.h"
#include "MxEthernetPort.h"
#include "MxAcmsTask.h"
#include "MxDcSpecialFunc.h"
#include "MxSysUtils.h"
#include "MxAlarms.h"
#include "MxSysTest.h"
#include "MxNetworkUtils.h"
#include "MxConfigFileIo.h"
#include "MxUdpPing.h"
#include "MxReaderUtils.h"
#include "MxSysTime.h"
#include "MxMenu.h"
#include "MxCafeteria.h"
#include "MxMultiLanFileHandling.h"
#include "MxEthernetPort.h"
#include "MxAttendanceDetail.h"
#include "MxAttendanceDetailConfig.h"
#include "MxFaceRecognitionConfig.h"
#include "MxDisplayMessage.h"
#include "MxWebJvsWifiSetting.h"
#include "MxGpioDrv.h"
#include "MxFaceRecognition.h"
#include "MxPushApiConfig.h"
#include "MxTemperatureSensor.h"
#include "MxDcTaskCommon.h"
#include "MxDcTaskUtils.h"
#include "MxSlaveCommon.h"
#include "MxCerTask.h"
#include "MxConfigDef.h"
#include "MxEnrollment.h"
#include "MxAutoSwitchWifiConfig.h"
#include "MxCommonEnrollment.h"
#include "MxRegistryFile.h"
#include "MxAvResp.h"
#include "MxFileIO.h"

//******** Extern Variables **********
extern SYS_BASIC_PARA_t	  		SysBasicPara;
extern ETHERNET_PARA_t			EthernetParameters;
extern SERVER_PARA_t			ServerParameters;
extern WIFI_PARA_t				WifiParameters;
extern MODEM_CONFIG_t			ModemProfile[];
extern MODEM_NW_PARA_t			BroadBandNwPara;
extern MMC_INFO_t 				MmcInfo;
extern USB_INFO_t				UsbInfo;
extern struct tm				CurrTimeDate;
extern DOOR_ACTIVITY_INFO_t 	DcInfo;
extern DOOR_MONITOR_t			DcMonitorControl;
extern SYS_VAR_t				SysVar;
extern READER_STATUS_e			readerStatus[];
extern SPECIAL_FUNCTION_t 		SpecialFunctions[];
extern SYS_ADVANCE_PARA_t	  	SysAdvPara;
extern PRINTER_CONFIG_t			PrinterConfig;
extern ALARM_t 					Alarms[];
extern INPUT_STATUS_t			SysInputStatus;
extern PASSWORD_t			  	Password;
extern COMM_PORT_e				acmsCurActiveInterface;
extern OUTPUT_STATUS_t			SysOutputStatus;
extern COMM_STATE_e				CommState;
extern ETHERNET_PARA_t			EthernetParameters;
extern BOOL						WifiWpsTrigF;
extern BOOL						WifiNwParaChngF;
extern BOOL						WifiApParaChngF;
extern CHAR						WifiAccessPointBssid[MAX_MAC_ADDRESS];
extern READER_MODE_e			DoorEntryExitMode;
extern BOOL						DispDoorModeF;
extern MULTI_LANG_SUPPORT_t		MultiLangConfig;
extern SERVER_MODE_CONFIG_t		ServerModeCfg;
extern const WIFI_PARA_t		DfltWifiPara;
extern APPLY_LEAVE_DATA_t 		applyLeaveData;
extern const SERVER_PARA_t  	DfltServerParaCentra;
extern const SYS_BASIC_PARA_t  	DfltBasicSysPara;
extern FR_SERVER_CONFIG_t		FRServerConfig;
extern pthread_mutex_t 			MutexLockAutoSwitchWiFi;
extern BOOL						ManualWifiConnectF;
extern BRIGHTNESS_DETAIL_t		DisplayBrightness;
extern PUSH_API_t				PushAPIConfig;
extern ADAPTIVE_FR_GUI_QUERY_t	AdaptiveFrQuery;
extern TEMPERATURE_SENSOR_INFO_t tempSensorInfo;
extern const MSG_MULTI_LANGUAGE_STRING_e getSysReaderStatusStr[];
extern const CHARPTR sysReaderTypeStr[];
extern SAVED_WIFI_LIST_t		SavedWifiList[MAX_SAVED_WIFI];

//******** Defines and Data Types ****
#define 	MAX_CONCURRANT_REQ 				1
#define		DATA_FRAME_TIMEOUT				100			// Data Frame Time Out In MiliSecond
#define 	MAX_FRAME_WAIT_TIME				1			// In second
#define		DISPLAY_CLOCK_SCREEN_TMR_CNT	100			// Count in 100 msec resolution
#define         SKIP_PD_SPFN_ID						4//set special function Id for clear Alarm
typedef struct
{
#ifdef QT_DESKTOP
	struct sockaddr_in 		serverAddr;
	struct sockaddr_in 		clientAddr;
#else
	struct sockaddr_un 		serverAddr;
	struct sockaddr_un 		clientAddr;
#endif
	INT32 					serverSockFd;
	INT32 					clientSockFd;
	CHAR 					rcvBuf[QT_RX_BUF_SIZE + 1];
	INT32 					rcvMsgLen;
	INT32 					client_len;
	BOOL 					clientConnectionF;
} QT_SOCKET_INFO_t;

//******** Function Prototypes *******

static BOOL InitQtMessageQueue();
static BOOL WriteQtMessageIntoQueue(GUI_MSG_ID_e msgId, VOIDPTR msgStruct,
														UINT32 fMessageSize);
BOOL ReadQtMessageFromQueue(GUI_MSG_ID_e *readMsgId, VOIDPTR readMsg,
															UINT16PTR length);
BOOL RecvMessage(INT32 connFd, CHARPTR rcvMsg, UINT32PTR rcvLen, CHAR startOfData,
								CHAR endOfData, UINT32 maxData, UINT32 timeout);
static BOOL SetDisplayClockScreenTimer(void);
static CHARPTR HandleWifiStatusStr(MSG_MULTI_LANGUAGE_STRING_e status);
static CHARPTR HandleModemStatusStr(MSG_MULTI_LANGUAGE_STRING_e status);

//******** Global Variables **********

QUEUE_MESSAGE_t		 	Qt_Message_Queue;
QT_SOCKET_INFO_t		qtSocketInfo;
MODEM_SET_t 			modemData;
STATUS_BAR_INFO_t		CurrStatusbarData;

//******** Static Variables **********

static const MSG_MULTI_LANGUAGE_STRING_e getSysAcmsStatusStr[2]=
{
	MSG_OFFLINE_413,
	MSG_ONLINE_415
};

static const MSG_MULTI_LANGUAGE_STRING_e getSysDoorStatusStr[DOOR_LOCK_MAX]=
{
	MSG_NORMAL_398,
	MSG_LOCKED_327,
	MSG_UNLOCKED_620,
};

static const MSG_MULTI_LANGUAGE_STRING_e getSysDoorMonStatusStr[MDS_MAX]=
{
	MSG_NORMAL_398,
	MSG_ACTIVATED_29,
	MSG_SHORT_FAULT_1036,
	MSG_OPEN_FAULT_1037,
	MSG_DISABLED_1035,
	MSG_OVERRIDDEN_1038,
};

static const MSG_MULTI_LANGUAGE_STRING_e getSysModemStatusStr[3]=
{
	MSG_NOT_PRESENT_1030,
	MSG_NOT_CONNECTED_774,
	MSG_CONNECTED_99,
};

static const MSG_MULTI_LANGUAGE_STRING_e getSysWifiStatusStr[3]=
{
	MSG_NOT_PRESENT_1030,
	MSG_NOT_CONNECTED_774,
	MSG_CONNECTED_99,
};

static const MSG_MULTI_LANGUAGE_STRING_e getSysStorageDevStatusStr[3]=
{
	MSG_NOT_CONNECTED_774,
	MSG_CONNECTED_99,
	MSG_DEVICE_NOT_RECOGNIZED_1033,
};

static const MSG_MULTI_LANGUAGE_STRING_e getSysExitSwitchStr[MDS_MAX]=
{
	MSG_INACTIVE_272,
	MSG_ACTIVE_1031,
	MSG_UNKNOWN_1034,
	MSG_UNKNOWN_1034,
	MSG_DISABLED_1035,
	MSG_UNKNOWN_1034,
};

static const MSG_MULTI_LANGUAGE_STRING_e getSysAlarmStr[MAX_STATUS_ALARM]=
{
	MSG_INACTIVE_272,
	MSG_ACTIVE_1031,
	MSG_ACKNOWLEDGED_1032,
};

//******** Function Definitions ******

//*****************************************************************************
//	InitGuiHandler()
//	Param:
//		IN:NONE
//		OUT:NONE
//	Returns:
//		NONE
//	Description:
//*****************************************************************************
void InitGuiHandler(void)
{
	socklen_t				sin_size;
	CHAR                    		buf[SYSTEM_CMD_BUF_SIZE];

	qtSocketInfo.clientSockFd = -1;

	if (!InitQtMessageQueue())
	{
		EPRINT(GUI_LOG, "Message Queue space allocation");
		return;
	}
	// create server socket
#ifdef QT_DESKTOP
	if ((qtSocketInfo.serverSockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		EPRINT(GUI_LOG, "Failed to create server socket");
		return;
	}
#else
	if ((qtSocketInfo.serverSockFd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		EPRINT(GUI_LOG, "Failed to create server socket");
		return;
	}

	DeleteFileUsingUnlink(QT_SOCKET_FILE);

	if(IsDirExist(QT_SOCKET_DIRECTORY) == FALSE)
	{
		snprintf(buf, sizeof(buf), "mkdir -p %s", QT_SOCKET_DIRECTORY);
		ExeCmd(buf);
	}
#endif

	memset(&qtSocketInfo.serverAddr, 0, sizeof(qtSocketInfo.serverAddr));
	memset(&qtSocketInfo.rcvBuf,0,sizeof(qtSocketInfo.rcvBuf));
	fcntl(qtSocketInfo.serverSockFd, F_SETFL, O_NONBLOCK);
	fcntl(qtSocketInfo.serverSockFd, F_SETFD, FD_CLOEXEC);

#ifdef QT_DESKTOP
	qtSocketInfo.serverAddr.sin_family = AF_INET;
	qtSocketInfo.serverAddr.sin_addr.s_addr = ntohl(INADDR_ANY);
	qtSocketInfo.serverAddr.sin_port = htons(9734);
#else
	qtSocketInfo.serverAddr.sun_family = AF_UNIX;
	snprintf(qtSocketInfo.serverAddr.sun_path, strlen(QT_SOCKET_FILE)+1, "%s", QT_SOCKET_FILE);
#endif
	qtSocketInfo.clientConnectionF = FALSE;

	if(bind(qtSocketInfo.serverSockFd, (struct sockaddr*)&qtSocketInfo.serverAddr,
										sizeof(qtSocketInfo.serverAddr) ) < 0)
	{
		close(qtSocketInfo.serverSockFd);
		EPRINT(GUI_LOG, "Server socket failed : bind");
		return;
	}

	if(listen(qtSocketInfo.serverSockFd, MAX_CONCURRANT_REQ) < 0)
	{
		close(qtSocketInfo.serverSockFd);
		EPRINT(GUI_LOG, "Server socket failed : listen");
		return;
	}

	sin_size = sizeof(qtSocketInfo.clientAddr);

	while(qtSocketInfo.clientSockFd == -1)
	{
		qtSocketInfo.clientSockFd = accept(qtSocketInfo.serverSockFd, (struct sockaddr*)&qtSocketInfo.clientAddr, &sin_size);
	}

	qtSocketInfo.clientConnectionF = TRUE;
	fcntl(qtSocketInfo.clientSockFd , F_SETFL, O_NONBLOCK);

	DPRINT(GUI_LOG, "Connected with QT socket");
}

//*****************************************************************************
//	GuiMsgHandler()
//	Param:
//		IN:NONE
//		OUT:NONE
//	Returns:
//		NONE
//	Description:
//
//*****************************************************************************
void GuiMsgHandler(void)
{
	SLAVE_MSG_ID_e 			msgId;
	CHARPTR 				fieldPtr;
	CHARPTR					msgPtr;
	CHAR					msgBuf[QT_RX_BUF_SIZE];
	CHARPTR					rcvDataPtr;
    BOOL					processF;

	qtSocketInfo.rcvMsgLen = read(qtSocketInfo.clientSockFd, (VOIDPTR)&qtSocketInfo.rcvBuf[0], QT_RX_BUF_SIZE);
	qtSocketInfo.rcvBuf[qtSocketInfo.rcvMsgLen] = '\0';
	rcvDataPtr = qtSocketInfo.rcvBuf;

	if(qtSocketInfo.rcvMsgLen > 0)
	{
		DPRINT(GUI_LOG,"Received request from server[%s] length[%d] discriptor:%d",
											qtSocketInfo.rcvBuf, qtSocketInfo.rcvMsgLen, qtSocketInfo.clientSockFd);
		processF = TRUE;
        SetLCDDimTimer();

		while(processF)
		{
			msgPtr = GetStrBeforeFsp(&rcvDataPtr, EOT_FSP);

			if(msgPtr)
			{
				fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
				sprintf(msgBuf, "%s", fieldPtr);

				msgId = atoi(msgBuf);
				DPRINT(GUI_LOG, "Message Received: msgId = 0x%x", msgId);

				switch(msgId)
				{

                case GUI_SCREEN_TOUCH_EVENT:
                {
                    /* SetLCDDimTimer() function is called above So is not Called again here */

                }break;
                case GUI_STATUSBAR_EXPAND_STATUS:
                {
                    fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
                    UINT8 status = (UINT8)atoi(fieldPtr);

                    StatusBarExpanded(status);
                    if((GetDoorApplType() == PANEL_DOOR_APPL) && (CommState == SLAVE_ONLINE))
                    {
                        WriteQtMessageIntoQueue(msgId, (VOIDPTR)&status, sizeof(UINT8));
                    }
                }break;
				case GUI_BUTTON_ADAPTIVE_FR_RESP:
				{
					fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
					UINT8 status = atoi(fieldPtr);
					ProcessRplErlConfirmMsg(status);
				}break;
				case GUI_BUTTON_TEMP_USR_ACKNOWLEDGEMENT_RESP:
				{
					fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
					BOOL tempUserAckResp = atoi(fieldPtr);
					WriteQtMessageIntoQueue(msgId, (VOIDPTR)&tempUserAckResp, sizeof(tempUserAckResp));				  
					break;
				}
				case GUI_BUTTON_TEMP_USR_NAME_RESP:
				{
					USER_NAME_SEARCH_t	searchData;

					fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
					if(fieldPtr)
					{
						sprintf(searchData.userName, "%s", fieldPtr);
						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&searchData,
															sizeof(USER_NAME_SEARCH_t));
					}  
					break;
				}
					case MSG_PIN_VALIDATION_EVENT:
					{
						PIN_DATA_t pinData;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						// Ignore message if PIN has not been entered

						if(fieldPtr)
						{
							sprintf(pinData.pinData,"%s",fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR) &pinData, sizeof(PIN_DATA_t));
						}
						break;
					}

					case MSG_ETHERNET_CFG_VALIDATION_EVENT:
					{
						LAN_ADDRESS_t lanPara;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(lanPara.lanIp, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(lanPara.lanSm, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(lanPara.lanGw, "%s", fieldPtr);

						if(GetDoorApplType() == DIRECT_DOOR_APPL)
						{
							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							sprintf(lanPara.lanPrefDns, "%s", fieldPtr);

							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							sprintf(lanPara.lanAltDns, "%s", fieldPtr);

							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							lanPara.nwType = (LAN_IP_MODE_e)atoi(fieldPtr);
						}

						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&lanPara, sizeof(LAN_ADDRESS_t));
						break;
					}

					case MSG_SERVER_CFG_VALIDATION_EVENT:
					{
						SERVER_ADDRESS_t serverPara;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							if((UINT8)atoi(fieldPtr) == COSEC_THIRD_PARTY)
							{
								serverPara.serverMode = COSEC_THIRD_PARTY;
							}
							else
							{
								serverPara.serverMode = (UINT8)atoi(fieldPtr);
							}
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						snprintf(serverPara.serverURL, MAX_SERVER_ADDR_254 ,"%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							serverPara.serverPortNumber = (UINT16)atoi(fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						snprintf(serverPara.serverTenantId, MAX_TENANT_ID, "%s", fieldPtr);

						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&serverPara, sizeof(SERVER_ADDRESS_t));
						break;
					}

					case MSG_WIFI_CFG_VALIDATION_EVENT:
					{
						WIFI_SETTINGS_PARA_t wiFiPara;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(wiFiPara.wifissid, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						wiFiPara.bandFreqType = (UINT8)atoi(fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							wiFiPara.wifiConnType = (UINT8)atoi(fieldPtr);
						}

						if(wiFiPara.wifiConnType == 0)
						{
							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							if(fieldPtr)
							{
								wiFiPara.wifiSecType = (UINT8)atoi(fieldPtr);
							}

							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							sprintf(wiFiPara.wifiKey, "%s", fieldPtr);

							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							if(fieldPtr)
							{
								wiFiPara.wifiEncryptionTyp = (UINT8)atoi(fieldPtr);
							}

							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							if(fieldPtr)
							{
								wiFiPara.wifiAuthTyp = (UINT8)atoi(fieldPtr);
							}

							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							sprintf(wiFiPara.wifiAnonymousId, "%s", fieldPtr);

							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							sprintf(wiFiPara.wifiUserName, "%s", fieldPtr);

							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							if(fieldPtr)
							{
								wiFiPara.wifiPeapVerTyp = (UINT8)atoi(fieldPtr);
							}

							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							if(fieldPtr)
							{
								wiFiPara.wifiInnerAuthTyp = (UINT8)atoi(fieldPtr);
							}
						}
						else
						{
							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							sprintf(wiFiPara.wifiBssid, "%s", fieldPtr);

							fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
							sprintf(wiFiPara.wifiPin, "%s", fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							wiFiPara.wifiNwType = (UINT8)atoi(fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(wiFiPara.wifiIp, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(wiFiPara.wifiSn, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(wiFiPara.wifiGw, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(wiFiPara.wifiDNS1, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(wiFiPara.wifiDNS2, "%s", fieldPtr);

						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&wiFiPara, sizeof(WIFI_SETTINGS_PARA_t));
						break;
					}

					case MSG_MOBILE_CFG_VALIDATION_EVENT:
					{
						MODEM_PROFILE_t modemPara;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							modemPara.profileNum = (UINT8)atoi(fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							modemPara.isSelected = (UINT8)atoi(fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(modemPara.name, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(modemPara.dialNo, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(modemPara.usrName, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(modemPara.passwd, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							modemPara.serviceType = (UINT8)atoi(fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(modemPara.apn, "%s", fieldPtr);

						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&modemPara,
															sizeof(MODEM_CONFIG_t));
						break;
					}

					case MSG_ENTER_USER_NAME_VALIDATION_EVENT:
					case MSG_ENROL_SEARCH_USER_LIST_EVENT:
					case MSG_DEL_CRED_SEARCH_USER_LIST_EVENT:
					{
						USER_NAME_SEARCH_t	searchData;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							sprintf(searchData.userName, "%s", fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR)&searchData,
																sizeof(USER_NAME_SEARCH_t));
						}
						break;
					}

					case MSG_ENROL_SEARCH_USER_NAME_FROM_ID:
					case  MSG_DEL_CRED_SEARCH_USER_NAME_FROM_ID:
					{
						USER_ALPHANUMID_t	userAlphaNumData;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							sprintf(userAlphaNumData.userAlphanumId, "%s", fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR)&userAlphaNumData, sizeof(USER_ALPHANUMID_t));
						}
						break;
					}

					case MSG_ENROL_USER_VALIDATION_EVENT:
					{
						ENROLL_USER_t	enrollUserInput;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							enrollUserInput.credType = (UINT8)atoi(fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							enrollUserInput.fpCount = (UINT8)atoi(fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							enrollUserInput.cardCount = (UINT8)atoi(fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							sprintf(enrollUserInput.alphaNumId, "%s", fieldPtr );
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(enrollUserInput.name, "%s", fieldPtr);
						
						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							enrollUserInput.maxFaces = (UINT8)atoi(fieldPtr);
						}

						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&enrollUserInput, sizeof(ENROLL_USER_t));
						break;
					}

					case MSG_ENROL_SPL_FN_VALIDATION_EVENT:
					{
						ENROLL_SPL_FUNC_t	enrollSplFnInput;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							enrollSplFnInput.fnId = (UINT8)atoi(fieldPtr);
						}
						
						//For getting proper special function Id according to Panel door SPECIAL_FUNCTIONS_e
						SetSplFnEnrollImagesOnPanelDoor(&enrollSplFnInput.fnId);
						
						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							enrollSplFnInput.noOfCards = (UINT8)atoi(fieldPtr);
						}
						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&enrollSplFnInput, sizeof(ENROLL_SPL_FUNC_t));
						break;
					}

					case MSG_DEL_CRED_VALIDATION_EVENT:
					{
						DELETE_USER_CREDEN_t delUsrCredInput;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							delUsrCredInput.credType = atoi(fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							sprintf(delUsrCredInput.alphaNumId, "%s", fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(delUsrCredInput.name, "%s", fieldPtr);

						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&delUsrCredInput, sizeof(DELETE_USER_CREDEN_t));
						break;
					}

					case MSG_DATE_TIME_VALIDATION_EVENT:
					{
						DATE_TIME_t	dateTimeInput;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(dateTimeInput.time, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(dateTimeInput.date, "%s", fieldPtr);

						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&dateTimeInput, sizeof(DATE_TIME_t));
						break;
					}

					case MSG_ADMIN_PW_VALIDATION_EVENT:
					case MSG_HRD_PW_VALIDATION_EVENT:
					{
						CHANGE_PASSWORD_t pswdInput;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(pswdInput.currPw, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(pswdInput.newPw, "%s", fieldPtr);

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(pswdInput.cnfrmPw, "%s", fieldPtr);

						WriteQtMessageIntoQueue(msgId,(VOIDPTR) &pswdInput, sizeof(CHANGE_PASSWORD_t));
						break;
					}

					case MSG_LOGIN_MENU_VALIDATION_EVENT:
					{
						LOGIN_PASSWORD_t passwdData;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							sprintf(passwdData.loginPw, "%s", fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR) &passwdData, sizeof(LOGIN_PASSWORD_t));
						}
						break;
					}

					case MSG_SEND_MODEM_PROFILE_DATA:
					{
						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							modemData.profileIdx = atoi(fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR)&modemData, sizeof(MODEM_SET_t));
						}
						break;
					}

					case MSG_BACKGROUND_SET_EVENT:
					{
						BACKGROUND_SET_t backGroundData;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							backGroundData.imageIdx = atoi(fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR)&backGroundData, sizeof(BACKGROUND_SET_t));
						}
						break;
					}

					case MSG_CLOCK_SET_EVENT:
					{
						CLOCK_SET_t clockData;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							clockData.clockIdx = atoi(fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR)&clockData, sizeof(CLOCK_SET_t));
						}
						break;
					}

					case GUI_MSG_BRIGHTNEES_SET_FROM_SPIN_BOX:
					{
						BRIGHTNESS_DETAIL_t	tmpBrightnessDetail;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						tmpBrightnessDetail.sleepTimer = (UINT16)atoi(fieldPtr);
						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						tmpBrightnessDetail.setBrightness = (PWM_DUTY_e)atoi(fieldPtr);
						SetPWMStatus(tmpBrightnessDetail.setBrightness);
						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&tmpBrightnessDetail, sizeof(BRIGHTNESS_DETAIL_t));
						break;
					}

					case GUI_MSG_BRIGHTNESS_VALIDATION_EVENT:
					{
						BRIGHTNESS_DETAIL_t	tmpBrightnessDetail;

						if((fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP)))
						{
							tmpBrightnessDetail.sleepTimer = (UINT16)atoi(fieldPtr);// HERE
						}

						if((fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP)))
						{
							tmpBrightnessDetail.setBrightness = (PWM_DUTY_e)atoi(fieldPtr);
							SetPWMStatus(tmpBrightnessDetail.setBrightness);
						}
						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&tmpBrightnessDetail, sizeof(BRIGHTNESS_DETAIL_t));
						break;
					}

					case MSG_KEYPAD_SET_EVENT:
					{
						KEYPAD_SET_t keypadData;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							keypadData.keypadIdx = atoi(fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR)&keypadData, sizeof(KEYPAD_SET_t));
						}
						break;
					}

					case MSG_WIFI_DFLT_CFG_EVENT:
					{
						WriteQtMessageIntoQueue(msgId, NULL, 0);
						break;
					}

					case MSG_SEARCH_JOB_CODE_LIST_EVENT:
					{
						CHAR	searchQuery[JOB_CODE_SIZE];

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							sprintf(searchQuery, "%s", fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR)searchQuery, sizeof(searchQuery));
						}
						break;
					}

					case MSB_JOB_CODE_SELECTED_EVENT:
					{
						UINT8	selectedIdx;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							selectedIdx = atoi(fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR)&selectedIdx, sizeof(selectedIdx));
						}
						break;
					}

					case GUI_BUTTON_NEXT_CAFE_MENU_LIST:
					{
						LoadDisplayCafeMenuList(CAFE_MENU_NEXT_PAGE, GUI_MSG_CAFE_SOLDOUT_ITEM_SCREEN);
					}
					break;

					case GUI_BUTTON_PREVIOUS_CAFE_MENU_LIST:
					{
						LoadDisplayCafeMenuList(CAFE_MENU_PREV_PAGE, GUI_MSG_CAFE_SOLDOUT_ITEM_SCREEN);
					}
					break;

					case GUI_BUTTON_NEXT_MAIN_CAFE_MENU_SCREEN_LIST:
					{
						LoadDisplayCafeMenuList(CAFE_MENU_NEXT_PAGE, GUI_MSG_CAFE_ITEM_LIST_SCREEN);
					}
					break;

					case GUI_BUTTON_PREVIOUS_MAIN_CAFE_MENU_SCREEN_LIST:
					{
						LoadDisplayCafeMenuList(CAFE_MENU_PREV_PAGE, GUI_MSG_CAFE_ITEM_LIST_SCREEN);
					}
					break;

					case GUI_BUTTON_NEXT_CAFE_AVAIL_MENU_LIST:
						LoadDisplayCafeMenuList(CAFE_MENU_NEXT_PAGE, GUI_MSG_CAFE_AVAILABLE_ITEM_SCREEN);
						break;

					case GUI_BUTTON_PREVIOUS_CAFE_AVAIL_MENU_LIST:
						LoadDisplayCafeMenuList(CAFE_MENU_PREV_PAGE, GUI_MSG_CAFE_AVAILABLE_ITEM_SCREEN);
						break;

					case GUI_MSG_WIFI_ENABLE_DISABLE_FLAG:
					{
						BOOL	wifiEnableF;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							wifiEnableF = (BOOL)atoi(fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR)&wifiEnableF, sizeof(wifiEnableF));
						}
					}
					break;

					case GUI_BUTTON_REFRESH_ATTNDNS_DTL:
					{
						UINT16	userIdx;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							userIdx = (UINT16)atoi(fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR)&userIdx, sizeof(userIdx));
						}
					}
					break;

					case GUI_BUTTON_RELOAD_ATTNDNS_DETAIL_TIMER:
					{
						UINT8	lcdIdleTimer;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							lcdIdleTimer = (UINT16)atoi(fieldPtr);
							WriteQtMessageIntoQueue(msgId, (VOIDPTR)&lcdIdleTimer, sizeof(lcdIdleTimer));
						}
					}
					break;

					case GUI_BUTTON_APPLY_LEAVE:
					{
						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							applyLeaveData.userIdx = (UINT16)atoi(fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							sprintf(applyLeaveData.absentDate, "%s", fieldPtr);
						}

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						if(fieldPtr)
						{
							sprintf(applyLeaveData.leaveCode, "%s", fieldPtr);
						}

						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&applyLeaveData, sizeof(applyLeaveData));
					}
					break;

					case GUI_BUTTON_OK_ASSIGN_STATIC_DEVICE_IP:
					{
						LAN_ADDRESS_t lanPara;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(lanPara.lanIp, "%s", fieldPtr);

						lanPara.nwType = LAN_IP_STATIC;

						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&lanPara, sizeof(LAN_ADDRESS_t));
						break;
					}

					case GUI_BUTTON_OK_ASSIGN_SERVER_URL:
					{
						SERVER_ADDRESS_t serverPara;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(serverPara.serverURL, "%s", fieldPtr);

						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&serverPara, sizeof(SERVER_ADDRESS_t));
						break;
					}

					case GUI_BUTTON_OK_ASSIGN_TENANT_ID:
					{
						SERVER_ADDRESS_t serverPara;

						fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
						sprintf(serverPara.serverTenantId, "%s", fieldPtr);

						WriteQtMessageIntoQueue(msgId, (VOIDPTR)&serverPara, sizeof(SERVER_ADDRESS_t));
						break;
					}

                    case MSG_AUTH_STATUSBAR_MENU_ACCESS_EVENT:
                    {
                        AUTH_STATUSBAR_MENU_ACCESS_t MsgidPassword;

                        fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
                        MsgidPassword.msgId = (SLAVE_MSG_ID_e)atoi(fieldPtr);
                        fieldPtr = GetStrBeforeFsp(&msgPtr, UI_MSG_FSP);
                        sprintf(MsgidPassword.loginPw, "%s", fieldPtr);

                        WriteQtMessageIntoQueue(MSG_AUTH_STATUSBAR_MENU_ACCESS_EVENT, (VOIDPTR)&MsgidPassword, sizeof(AUTH_STATUSBAR_MENU_ACCESS_t));
                        break;
                    }

					default:
					{
						WriteQtMessageIntoQueue(msgId, msgPtr, strlen(msgPtr) );
						break;
					}
				}
			}
			else
			{
				processF = FALSE;
			}
		}
	}
}

//***************************************************************************
//	ProcessQtMessage()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This will be called Periodically by the main Application from
//				an infinite loop. And if a Message exists in Message Queue,
//				it will process the Message Parameters as per the Message Id.
//				The Message has been written in Queue by QtMessageReceived().
//***************************************************************************
VOID ProcessQtMessage()
{
    SLAVE_MSG_ID_e		msgId;
	UINT8				msgFormat[1024] = {0};
	UINT16				msgLen;
	CHAR				cmdBuf[128];

	if(ReadQtMessageFromQueue(&msgId, msgFormat, &msgLen) == TRUE)
	{
		switch(msgId)
		{
			case MSG_PIN_VALIDATION_EVENT:
			{
				PIN_DATA_t * pinValue = (PIN_DATA_t *)msgFormat;

				ProcMsg(MSG_PIN_VALIDATION_EVENT, (VOIDPTR)pinValue -> pinData, (UINT16)sizeof(PIN_DATA_t));
				break;
			}

			case MSG_ETHERNET_CFG_VALIDATION_EVENT:
			{
				LAN_ADDRESS_t 					* lanAddRcvd = (LAN_ADDRESS_t *)msgFormat;
				DYNAMIC_VALIDATION_RESPONSE_t 	qtDynamicValidationRespInfo;
				BOOL 							changeIpAddr = FALSE, validationF = FALSE;

				//TODO : Need to verify design
				if(ValidateNetworkAddr(lanAddRcvd -> lanIp, lanAddRcvd -> lanIp, CHECK_IP_ADDR) == FALSE)
				{
					qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_INVALID_IP;
				}
				else if(ValidateSubnetAddr(lanAddRcvd -> lanSm, lanAddRcvd -> lanSm) == FALSE)
				{
					qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_INVALID_SUBNET;
				}
				else if(ValidateNetworkAddr(lanAddRcvd -> lanGw, lanAddRcvd -> lanGw, CHECK_GATEWAY_ADDR) == FALSE)
				{
					qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_INVALID_GATEWAY;
				}
				else if( (GetDoorApplType() == DIRECT_DOOR_APPL ) &&
							( ValidateNetworkAddr(lanAddRcvd -> lanPrefDns, lanAddRcvd -> lanPrefDns, CHECK_DNS_ADDR) == FALSE) )
				{
					qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_INVALID_PDNS;
				}
				else if( (GetDoorApplType() == DIRECT_DOOR_APPL ) &&
							( ValidateNetworkAddr(lanAddRcvd -> lanAltDns, lanAddRcvd -> lanAltDns, CHECK_DNS_ADDR) == FALSE) )
				{
					qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_INVALID_ADNS;
				}
				else
				{
					validationF = TRUE;
					if(strcmp(EthernetParameters.ipAddress,lanAddRcvd->lanIp) !=0 )
					{
						memcpy(EthernetParameters.ipAddress, lanAddRcvd->lanIp, MAX_IP_ADDRESS);
						changeIpAddr = TRUE;
					}

					if(strcmp(EthernetParameters.subnet, lanAddRcvd->lanSm) !=0 )
					{
						memcpy(EthernetParameters.subnet, lanAddRcvd->lanSm, MAX_IP_ADDRESS);
						changeIpAddr = TRUE;
					}

					if(strcmp(EthernetParameters.gateway, lanAddRcvd->lanGw) !=0 )
					{
						memcpy(EthernetParameters.gateway, lanAddRcvd->lanGw, MAX_IP_ADDRESS);
						changeIpAddr = TRUE;
					}

					if( GetDoorApplType() == DIRECT_DOOR_APPL )
					{
						if(strcmp(EthernetParameters.dns1, lanAddRcvd -> lanPrefDns) != 0)
						{
							sprintf(EthernetParameters.dns1, "%s", lanAddRcvd -> lanPrefDns);
							changeIpAddr = TRUE;
						}

						if(strcmp(EthernetParameters.dns2, lanAddRcvd -> lanAltDns) != 0)
						{
							sprintf(EthernetParameters.dns2, "%s", lanAddRcvd -> lanAltDns);
							changeIpAddr = TRUE;
						}

						if( EthernetParameters.nwType != lanAddRcvd -> nwType )
						{
							EthernetParameters.nwType = lanAddRcvd -> nwType;
							changeIpAddr = TRUE;
						}
					}
					if(WriteEthParaConfig() == SUCCESS)
					{
						// Log event for configuration change
						DPRINT(GUI_LOG, "Ethernet Parameter Config set successfully");

						//Delete deviceIpSignature.txt on change of LAN Settings
						memset(cmdBuf,0,sizeof(cmdBuf));
						sprintf(cmdBuf,"rm %s",DEVICE_IP_SIGN_FILE);
						system(cmdBuf);

						// Ip address or sunebt is change reinit comtask
						if(changeIpAddr == TRUE)
						{
							ChangeNetworkInterfacePara(ETHERNET_PARA);

							if(EthernetParameters.nwType == LAN_IP_STATIC)
							{
								DPRINT(SYS_LOG, "Process For Selection On Static.");
								StopEthernetDhcpClient();
							}

							if(EthernetParameters.nwType == LAN_IP_DYNAMIC)
							{
								DPRINT(SYS_LOG, "Process For Selection On Dynamic.");
								StartEthernetDhcpClient();
							}
						}
					}
					else
					{
						EPRINT(GUI_LOG, "Ethernet Parameter Config set failed");
					}
				}
				if(validationF == SUCCESS)
				{
					ProcMsg(MSG_ETHERNET_CFG_VALIDATION_EVENT, NULL, 0);
				}
				else
				{
					SendMessageToQt(&qtDynamicValidationRespInfo, GUI_MSG_VALIDATION_RESPONSE);
					ProcMsg(GUI_BUTTON_KEEP_ALIVE, NULL, 0);
				}
				break;
			}

			case MSG_WIFI_CFG_VALIDATION_EVENT:
			{
				BOOL	validationF 						= FAIL;
				WifiNwParaChngF								= FALSE;
				WifiApParaChngF								= FALSE;
				WifiWpsTrigF 								= FALSE;
				WIFI_SETTINGS_PARA_t 			*wifiPara	= (WIFI_SETTINGS_PARA_t*)msgFormat;
				DYNAMIC_VALIDATION_RESPONSE_t 	qtDynamicValidationRespInfo;

				if((IP_MODE_e)wifiPara -> wifiNwType == IP_STATIC)
				{
					if(ValidateNetworkAddr(wifiPara -> wifiIp, wifiPara -> wifiIp, CHECK_IP_ADDR) == FALSE)
					{
						qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_INVALID_IP;
					}
					else if(ValidateSubnetAddr(wifiPara -> wifiSn, wifiPara -> wifiSn) == FALSE)
					{
						qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_INVALID_SUBNET;
					}
					else if(ValidateNetworkAddr(wifiPara -> wifiGw, wifiPara -> wifiGw, CHECK_GATEWAY_ADDR) == FALSE)
					{
						qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_INVALID_GATEWAY;
					}
					else if(ValidateNetworkAddr(wifiPara -> wifiDNS1, wifiPara -> wifiDNS1, CHECK_DNS_ADDR) == FALSE)
					{
						qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_INVALID_PDNS;
					}
					else if(ValidateNetworkAddr(wifiPara -> wifiDNS2, wifiPara -> wifiDNS2, CHECK_DNS_ADDR) == FALSE)
					{
						qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_INVALID_ADNS;
					}
					else
					{
						validationF = SUCCESS;
					}
				}
				else
				{
					validationF = SUCCESS;
				}

				if(validationF == SUCCESS)
				{
					pthread_mutex_lock(&MutexLockAutoSwitchWiFi);
					ManualWifiConnectF = TRUE;

					if( (WifiParameters.nwType != (IP_MODE_e)wifiPara -> wifiNwType) ||
						(strcmp(WifiParameters.ipAddress,  wifiPara -> wifiIp) != 0) ||
						(strcmp(WifiParameters.subnet,  wifiPara -> wifiSn) != 0) ||
						(strcmp(WifiParameters.gateway,  wifiPara -> wifiGw) != 0) ||
						((GetDoorApplType() == DIRECT_DOOR_APPL)&&(strcmp(WifiParameters.dns1,  wifiPara -> wifiDNS1) != 0))||
						((GetDoorApplType() == DIRECT_DOOR_APPL)&&(strcmp(WifiParameters.dns2,  wifiPara -> wifiDNS2) != 0)))
						{
							WifiParameters.nwType = (IP_MODE_e)wifiPara -> wifiNwType;
							sprintf(WifiParameters.ipAddress, "%s", wifiPara -> wifiIp);
							sprintf(WifiParameters.subnet, "%s", wifiPara -> wifiSn);
							sprintf(WifiParameters.gateway, "%s", wifiPara -> wifiGw);
							if(GetDoorApplType() == DIRECT_DOOR_APPL)
							{
								sprintf(WifiParameters.dns1, "%s", wifiPara -> wifiDNS1);
								sprintf(WifiParameters.dns2, "%s", wifiPara -> wifiDNS2);
							}

							WifiNwParaChngF = TRUE;
						}

					if(wifiPara->wifiConnType == 0)
					{
						WifiParameters.wpsTrigerTyp = WPS_TRIGER_NONE;
						if( (strcmp(WifiParameters.ssid, wifiPara -> wifissid) != 0)
							|| (WifiParameters.secType != (SEC_TYPE_e)wifiPara -> wifiSecType)
							|| (strcmp(WifiParameters.passphresh, wifiPara -> wifiKey) != 0)
							|| (WifiParameters.keymgmt != (WIFI_KEY_MGMT_e)wifiPara -> wifiEncryptionTyp)
							|| (WifiParameters.advAuthType != (WIFI_AUTH_TYPE_e)wifiPara -> wifiAuthTyp)
							|| (strcmp(WifiParameters.anonymousIdentity, wifiPara -> wifiAnonymousId) != 0)
							|| (strcmp(WifiParameters.userName, wifiPara -> wifiUserName) != 0)
							|| (WifiParameters.peapVer != (PEAP_VER_e)wifiPara -> wifiPeapVerTyp)
							|| (WifiParameters.innerAuth != (INNER_AUTH_e)wifiPara -> wifiInnerAuthTyp) )
						{
							sprintf(WifiParameters.ssid, "%s", wifiPara -> wifissid);
							WifiParameters.secType = (SEC_TYPE_e)wifiPara -> wifiSecType;
							sprintf(WifiParameters.passphresh, "%s", wifiPara -> wifiKey);
							WifiParameters.keymgmt = (WIFI_KEY_MGMT_e)wifiPara -> wifiEncryptionTyp;

							WifiParameters.advAuthType = (WIFI_AUTH_TYPE_e)wifiPara -> wifiAuthTyp;
							sprintf(WifiParameters.anonymousIdentity, "%s", wifiPara -> wifiAnonymousId);
							sprintf(WifiParameters.userName, "%s", wifiPara -> wifiUserName);
							WifiParameters.peapVer = (PEAP_VER_e)wifiPara -> wifiPeapVerTyp;
							WifiParameters.innerAuth = (INNER_AUTH_e)wifiPara -> wifiInnerAuthTyp;
							WifiParameters.bandFreqType = (WIFI_BAND_FREQ_TYPE_e)wifiPara -> bandFreqType;

							WifiApParaChngF = TRUE;
						}
					}
					else
					{
						WifiWpsTrigF = TRUE;
						WifiParameters.wpsTrigerTyp = WPS_TRIGER_PIN_FROM_ACCESS_POINT;
						sprintf(WifiAccessPointBssid, "%s", wifiPara -> wifiBssid);
						sprintf(WifiParameters.wpsPin, "%s", wifiPara -> wifiPin);
					}

					SaveWifiParaToSavedWifiList();
					if(WriteWifiParaConfig() )
					{
						// Log event for configuration change
						DPRINT(GUI_LOG, "WiFi Parameter Config set successfully");

						if( (WifiApParaChngF) || (WifiNwParaChngF) || (WifiWpsTrigF) )
						{
							ChangeNetworkInterfacePara(WIFI_PARA);
						}
					}
					else
					{
						EPRINT(GUI_LOG, "WiFi Parameter Config set failed");
					}

					pthread_mutex_unlock(&MutexLockAutoSwitchWiFi);
					ProcMsg(MSG_WIFI_CFG_VALIDATION_EVENT, NULL, 0);
				}
				else
				{
					SendMessageToQt(&qtDynamicValidationRespInfo, GUI_MSG_VALIDATION_RESPONSE);
					ProcMsg(GUI_BUTTON_KEEP_ALIVE, NULL, 0);
				}
				break;
			}

			case MSG_WIFI_DFLT_CFG_EVENT:
			{
				BOOL tmpEnblF = WifiParameters.enableWifi;
				CHAR forgotNW[MAX_SSID_NAME] = {'\0'};
				memcpy(forgotNW, WifiParameters.ssid, MAX_SSID_NAME);
				WifiApParaChngF = TRUE;
				WifiNwParaChngF = TRUE;
				WifiWpsTrigF	= FALSE;

				pthread_mutex_lock(&MutexLockAutoSwitchWiFi);

				WifiParameters = DfltWifiPara;
				WifiParameters.enableWifi = tmpEnblF;
                SetConnectedSsidSignalStrength(NO_WIFI_SIGNAL);

				if(GetWiFiStatus() != NO_WIFI)
				{
					GetWifiMacAddress();
				}

				if(DeleteWiFiNwFromSavedWifiList(forgotNW) == SUCCESS)
				{
					if(WriteSavedWifiParaConfig() == SUCCESS)
					{
						DPRINT(GUI_LOG, "WiFi SSID removed successfully");
					}
				}
				else
				{
					//                    memset(SavedWifiList, 0, sizeof(SavedWifiList));
					//                    memcpy(SavedWifiList, tempSavedWifiList, sizeof(tempSavedWifiList));
					//                    WriteSavedWifiParaConfig();
					EPRINT(GUI_LOG, "WiFi SSID removal failed, WiFi saved para defaulted");
				}

				if(WriteWifiParaConfig() == SUCCESS)
				{
					ChangeNetworkInterfacePara(WIFI_PARA);
				}

				pthread_mutex_unlock(&MutexLockAutoSwitchWiFi);
				ProcMsg(MSG_WIFI_DFLT_CFG_EVENT, NULL, 0);
				break;
			}

			case MSG_SERVER_CFG_VALIDATION_EVENT:
			{
				SERVER_ADDRESS_t  * servAddRcvd = (SERVER_ADDRESS_t *)msgFormat;

				ProcMsg(MSG_SERVER_CFG_VALIDATION_EVENT, (VOIDPTR)servAddRcvd, (UINT16)sizeof(SERVER_ADDRESS_t));
				break;
			}

			case MSG_MOBILE_CFG_VALIDATION_EVENT:
			{
				MODEM_PROFILE_t * modemParaRcvd = (MODEM_PROFILE_t *)msgFormat;

				ProcMsg(MSG_MOBILE_CFG_VALIDATION_EVENT, (VOIDPTR)modemParaRcvd, (UINT16)sizeof(modemParaRcvd));
				break;
			}

			case MSG_ENROL_USER_VALIDATION_EVENT:
			{
				ENROLL_USER_t * enrlUsr = (ENROLL_USER_t *)msgFormat;

				ProcMsg(MSG_ENROL_USER_VALIDATION_EVENT, (VOIDPTR)enrlUsr, (UINT16)sizeof(ENROLL_USER_t));
				break;
			}

			case MSG_DEL_CRED_VALIDATION_EVENT:
			{
				DELETE_USER_CREDEN_t * delCrden = (DELETE_USER_CREDEN_t *)msgFormat;

				ProcMsg(MSG_DEL_CRED_VALIDATION_EVENT, (VOIDPTR)delCrden, (UINT16)sizeof(DELETE_USER_CREDEN_t));
				break;
			}

			case MSG_ENROL_SPL_FN_VALIDATION_EVENT:
			{
				ENROLL_SPL_FUNC_t 	 * enrlSplFn = (ENROLL_SPL_FUNC_t *)msgFormat;

				ProcMsg(MSG_ENROL_SPL_FN_VALIDATION_EVENT, (VOIDPTR)enrlSplFn, (UINT16)sizeof(ENROLL_SPL_FUNC_t));
				break;
			}

			case MSG_DATE_TIME_VALIDATION_EVENT:
			{
				DATE_TIME_t * dateTime = (DATE_TIME_t*)msgFormat;

				ProcMsg(MSG_DATE_TIME_VALIDATION_EVENT, (VOIDPTR)dateTime, (UINT16)sizeof(DATE_TIME_t));
				break;
			}

			case MSG_ADMIN_PW_VALIDATION_EVENT:
			{
				CHANGE_PASSWORD_t * passwordData = (CHANGE_PASSWORD_t *)msgFormat;

				ProcMsg(MSG_ADMIN_PW_VALIDATION_EVENT, (VOIDPTR)passwordData, (UINT16)sizeof(CHANGE_PASSWORD_t));
				break;
			}

			case MSG_HRD_PW_VALIDATION_EVENT:
			{
				CHANGE_PASSWORD_t * passwordData = (CHANGE_PASSWORD_t *)msgFormat;

				ProcMsg(MSG_HRD_PW_VALIDATION_EVENT, (VOIDPTR)passwordData, (UINT16)sizeof(CHANGE_PASSWORD_t));
				break;
			}

			case MSG_LOGIN_MENU_VALIDATION_EVENT:
			{
				LOGIN_PASSWORD_t * menuPasswd = (LOGIN_PASSWORD_t *)msgFormat;

				ProcMsg(MSG_LOGIN_MENU_VALIDATION_EVENT, (VOIDPTR)menuPasswd, (UINT16)sizeof(LOGIN_PASSWORD_t));
				break;
			}

			case MSG_BACKGROUND_SET_EVENT:
			{
				BACKGROUND_SET_t * bgImage = (BACKGROUND_SET_t *)msgFormat;

				// TODO : Need to verify design
				if(GetDoorApplType() == PANEL_DOOR_APPL)
				{
					SysVar.backgroundImage = bgImage->imageIdx;
					WriteSysVars();
					ProcMsg(MSG_BACKGROUND_SET_EVENT, NULL, 0);
				}
				else
				{
					ProcMsg(MSG_BACKGROUND_SET_EVENT, (VOIDPTR)bgImage, (UINT16)sizeof(BACKGROUND_SET_t));
				}
				break;
			}

			case MSG_CLOCK_SET_EVENT:
			{
				CLOCK_SET_t * selectedClock = (CLOCK_SET_t *)msgFormat;

				// TODO : Need to verify design
				if(GetDoorApplType() == PANEL_DOOR_APPL)
				{
					SysVar.currentClock = selectedClock->clockIdx;
					WriteSysVars();
					ProcMsg(MSG_CLOCK_SET_EVENT, NULL, 0);
				}
				else
				{
					ProcMsg(MSG_CLOCK_SET_EVENT, (VOIDPTR)selectedClock, (UINT16)sizeof(CLOCK_SET_t));
				}
				break;
			}

			case GUI_MSG_BRIGHTNEES_SET_FROM_SPIN_BOX:
			{
				BRIGHTNESS_DETAIL_t * tmpBrightnessDetail = (BRIGHTNESS_DETAIL_t *)msgFormat;
				SetPWMStatus(tmpBrightnessDetail->setBrightness);
				break;
			}

			case GUI_MSG_BRIGHTNESS_VALIDATION_EVENT:
				{
					BRIGHTNESS_DETAIL_t * tmpBrightnessDetail = (BRIGHTNESS_DETAIL_t *)msgFormat;

					if(GetDoorApplType() == PANEL_DOOR_APPL)
					{
						SetPWMStatus(tmpBrightnessDetail->setBrightness);

						DisplayBrightness.sleepTimer = tmpBrightnessDetail -> sleepTimer;
						DisplayBrightness.setBrightness = tmpBrightnessDetail -> setBrightness;

						if(WriteDisplayBrightnessConfig() == FAIL)
						{
							DPRINT(SYS_LOG, "Display Brightness Configuration Write Fail\n");
						}

						if(CommState != SLAVE_ONLINE)
							ProcMsg(GUI_MSG_BRIGHTNESS_VALIDATION_EVENT, (VOIDPTR)tmpBrightnessDetail, (UINT16)sizeof(BRIGHTNESS_DETAIL_t));
						else
							ProcMsg(GUI_MSG_BRIGHTNESS_VALIDATION_EVENT, (VOIDPTR)tmpBrightnessDetail, (UINT16)sizeof(BRIGHTNESS_DETAIL_CMD_t));
					}
					else
					{
						ProcMsg(GUI_MSG_BRIGHTNESS_VALIDATION_EVENT, (VOIDPTR)tmpBrightnessDetail, (UINT16)sizeof(BRIGHTNESS_DETAIL_t));
					}

					break;
				}

			case MSG_KEYPAD_SET_EVENT:
			{
				KEYPAD_SET_t * selectedKeypad = (KEYPAD_SET_t *)msgFormat;

				// TODO : Need to verify design
				if(GetDoorApplType() == PANEL_DOOR_APPL)
				{
					SysVar.currentKeypad = selectedKeypad->keypadIdx;
					WriteSysVars();
					ProcMsg(MSG_KEYPAD_SET_EVENT, NULL, 0);
				}
				else
				{
					ProcMsg(MSG_KEYPAD_SET_EVENT, (VOIDPTR)selectedKeypad, (UINT16)sizeof(KEYPAD_SET_t));
				}
				break;
			}

			case MSG_SEND_MODEM_PROFILE_DATA:
			{
				MODEM_SET_t * profile = (MODEM_SET_t *)msgFormat;

				ProcMsg(MSG_SEND_MODEM_PROFILE_DATA, (VOIDPTR)profile, (UINT16)sizeof(MODEM_SET_t));
				break;
			}

			case MSG_ENROL_SEARCH_USER_NAME_FROM_ID:
			{
				USER_ALPHANUMID_t * idData = (USER_ALPHANUMID_t *)msgFormat;

				ProcMsg(MSG_ENROL_SEARCH_USER_NAME_FROM_ID, (VOIDPTR)idData, (UINT16)sizeof(USER_ALPHANUMID_t));
				break;
			}

			case MSG_ENROL_SEARCH_USER_LIST_EVENT:
			{
				USER_NAME_SEARCH_t * searchData = (USER_NAME_SEARCH_t *)msgFormat;

				ProcMsg(MSG_ENROL_SEARCH_USER_LIST_EVENT, (VOIDPTR)searchData, (UINT16)sizeof(USER_NAME_SEARCH_t));
				break;
			}

			case MSG_ENTER_USER_NAME_VALIDATION_EVENT:
			{
				USER_NAME_SEARCH_t * searchData = (USER_NAME_SEARCH_t *)msgFormat;

				ProcMsg(MSG_ENTER_USER_NAME_VALIDATION_EVENT, (VOIDPTR)searchData, (UINT16)sizeof(USER_NAME_SEARCH_t));
				break;
			}

			case MSG_DEL_CRED_SEARCH_USER_NAME_FROM_ID:
			{
				USER_ALPHANUMID_t * alphaNumData = (USER_ALPHANUMID_t *)msgFormat;

				ProcMsg(MSG_DEL_CRED_SEARCH_USER_NAME_FROM_ID, (VOIDPTR)alphaNumData, (UINT16)sizeof(USER_ALPHANUMID_t));
				break;
			}

			case MSG_DEL_CRED_SEARCH_USER_LIST_EVENT:
			{
				USER_NAME_SEARCH_t * searchData = (USER_NAME_SEARCH_t *)msgFormat;

				ProcMsg(MSG_DEL_CRED_SEARCH_USER_LIST_EVENT, (VOIDPTR)searchData, (UINT16)sizeof(USER_NAME_SEARCH_t));
				break;
			}

			case MSG_SEARCH_JOB_CODE_LIST_EVENT:
			{
				ProcMsg(MSG_SEARCH_JOB_CODE_LIST_EVENT, (VOIDPTR)msgFormat, (UINT16)sizeof(JOB_CODE_SIZE));
				break;
			}

			case MSB_JOB_CODE_SELECTED_EVENT:
			{
				ProcMsg(MSB_JOB_CODE_SELECTED_EVENT, (VOIDPTR)msgFormat, (UINT16)sizeof(UINT8));
				break;
			}

			case GUI_MSG_WIFI_ENABLE_DISABLE_FLAG:
			{
				ProcMsg(GUI_MSG_WIFI_ENABLE_DISABLE_FLAG, (VOIDPTR)msgFormat, (UINT16)sizeof(BOOL));
				break;
			}

			case GUI_BUTTON_REFRESH_ATTNDNS_DTL:
			{
				ProcMsg(GUI_BUTTON_REFRESH_ATTNDNS_DTL, (VOIDPTR)msgFormat, (UINT16)sizeof(UINT16));
				break;
			}

			case GUI_BUTTON_RELOAD_ATTNDNS_DETAIL_TIMER:
			{
				ProcMsg(GUI_BUTTON_RELOAD_ATTNDNS_DETAIL_TIMER, (VOIDPTR)msgFormat, (UINT16)sizeof(UINT8));
				break;
			}

			case GUI_BUTTON_OK_ASSIGN_STATIC_DEVICE_IP:
			{
				LAN_ADDRESS_t 	* lanAddRcvd = (LAN_ADDRESS_t *)msgFormat;
				DYNAMIC_VALIDATION_RESPONSE_t 	qtDynamicValidationRespInfo;

				if(ValidateNetworkAddr(lanAddRcvd -> lanIp, lanAddRcvd -> lanIp, CHECK_IP_ADDR) == FALSE)
				{
					qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_INVALID_IP;
					SendMessageToQt(&qtDynamicValidationRespInfo, GUI_MSG_VALIDATION_RESPONSE);
					ProcMsg(GUI_BUTTON_KEEP_ALIVE, NULL, 0);
				}
				else
				{
					memcpy(EthernetParameters.ipAddress, lanAddRcvd->lanIp, MAX_IP_ADDRESS);
					EthernetParameters.nwType = LAN_IP_STATIC;
					if(WriteEthParaConfig() == SUCCESS)
					{
						// Log event for configuration change
						DPRINT(SYS_LOG, "Installation State : Ethernet Parameter Config set successfully");

						//Delete deviceIpSignature.txt on change of LAN Settings
						memset(cmdBuf,0,sizeof(cmdBuf));
						sprintf(cmdBuf,"rm %s",DEVICE_IP_SIGN_FILE);
						system(cmdBuf);

						ChangeNetworkInterfacePara(ETHERNET_PARA);

						if(EthernetParameters.nwType == LAN_IP_STATIC)
						{
							DPRINT(SYS_LOG, "Process For Selection On Static.");
							StopEthernetDhcpClient();
						}

						if(EthernetParameters.nwType == LAN_IP_DYNAMIC)
						{
							DPRINT(SYS_LOG, "Process For Selection On Dynamic.");
							StartEthernetDhcpClient();
						}
					}
					else
					{
						EPRINT(SYS_LOG, "Installation State : Ethernet Parameter Config set failed");
					}
					ProcMsg(GUI_BUTTON_OK_ASSIGN_STATIC_DEVICE_IP, NULL, 0);
				}
				break;
			}

			case GUI_BUTTON_OK_ASSIGN_SERVER_URL:
			{
				SERVER_ADDRESS_t  * servAddRcvd = (SERVER_ADDRESS_t *)msgFormat;
				DYNAMIC_VALIDATION_RESPONSE_t 	qtDynamicValidationRespInfo;

				if(strcmp("\0", servAddRcvd -> serverURL) == 0)
				{
					qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_BLANK_SERVER_URL;
					SendMessageToQt(&qtDynamicValidationRespInfo, GUI_MSG_VALIDATION_RESPONSE);
					ProcMsg(GUI_BUTTON_KEEP_ALIVE, NULL, 0);
				}
				else
				{
					sprintf(ServerParameters.ethServerHostName, "%s", servAddRcvd -> serverURL);
					sprintf(ServerParameters.wifiServerHostName, "%s", servAddRcvd -> serverURL);
					sprintf(ServerParameters.modemServerHostName, "%s", servAddRcvd -> serverURL);

					ServerParameters.ethServerPort = DfltServerParaCentra.ethServerPort;	// default port 11000
					ServerParameters.wifiServerPort = DfltServerParaCentra.wifiServerPort;	// default port 11000
					ServerParameters.modemServerPort = DfltServerParaCentra.modemServerPort;	// default port 11000

					ServerModeCfg.serverMode = COSEC_CENTRA;
					if(WriteServerModeConfig() == SUCCESS)
					{
						CheckHttpServerConfig();
						if(WriteServerParaConfig() == SUCCESS)
						{
							DPRINT(SYS_LOG, "Installation State : CENTRA Server Parameter Config set successfully");
							//Delete serverIpSignature.txt on change of Server Settings
							memset(cmdBuf,0,sizeof(cmdBuf));
							sprintf(cmdBuf,"rm %s",SERVER_IP_SIGN_FILE);
							system(cmdBuf);
							ChangeNetworkInterfacePara(SERVER_PARA);
						}
						else
						{
							EPRINT(SYS_LOG, "Installation State : CENTRA Server Parameter Config set failed");
						}
					}
					ProcMsg(GUI_BUTTON_OK_ASSIGN_SERVER_URL, NULL, 0);
				}
				break;
			}

			case GUI_BUTTON_OK_ASSIGN_TENANT_ID:
			{
				SERVER_ADDRESS_t  * servAddRcvd = (SERVER_ADDRESS_t *)msgFormat;
				DYNAMIC_VALIDATION_RESPONSE_t 	qtDynamicValidationRespInfo;

				if( ( (IsServerModeVyom() == YES) || ((servAddRcvd -> serverMode) == COSEC_VYOM) )
					&& ( atoll(servAddRcvd ->serverTenantId) == 0) )
				{
					if(servAddRcvd ->serverTenantId[0] == 0)
					{
						qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_BLANK_SERVER_TENANT_ID;
					}
					else
					{
						qtDynamicValidationRespInfo.respMsgIdx = VALIDATION_RESP_INVALID_SERVER_TENANT_ID;
					}
					SendMessageToQt(&qtDynamicValidationRespInfo, GUI_MSG_VALIDATION_RESPONSE);
					ProcMsg(GUI_BUTTON_KEEP_ALIVE, NULL, 0);
				}
				else
				{
					sprintf(ServerParameters.TenantIdVyom, "%s", servAddRcvd -> serverTenantId);
					if(WriteServerParaConfig() == SUCCESS)
					{
						DPRINT(SYS_LOG, "Installation State : VYOM Server Parameter Config set successfully");
						//Delete serverIpSignature.txt on change of Server Settings
						memset(cmdBuf,0,sizeof(cmdBuf));
						sprintf(cmdBuf,"rm %s",SERVER_IP_SIGN_FILE);
						system(cmdBuf);
						ChangeNetworkInterfacePara(SERVER_PARA);
					}
					else
					{
						EPRINT(SYS_LOG, "Installation State : VYOM Server Parameter Config set failed");
					}
					ProcMsg(GUI_BUTTON_OK_ASSIGN_TENANT_ID, NULL, 0);
				}
				break;
			}

			case GUI_MSG_TEMPERATURE_REFRESH:
			{
				ProcMsg(GUI_MSG_TEMPERATURE_REFRESH, NULL, 0);
				break;
			}

			case GUI_MSG_AMBIENT_COUNTDOWN_COMPLETED:
			{
				ProcMsg(GUI_MSG_AMBIENT_COUNTDOWN_COMPLETED, NULL, 0);
				break;
			}
			
			case GUI_MSG_FACE_DETECTING_COUNTDOWN_COMPLETED:
			{
				ProcMsg(MSG_FACE_ENROLL_START_CMD_SEND_EVENT,NULL,0);
				break;
			}

			case GUI_BUTTON_PIN_ENTER_SCREEN:
			case GUI_ALNUM_KEEP_ALIVE:
			case GUI_BUTTON_FIELD_CLEAR:
			case GUI_BUTTON_KEEP_ALIVE:
			case GUI_BUTTON_CANCEL:
			case GUI_BUTTON_HOME:
			{
				RESPONSE_PORT_e respPort = INTERNAL_PORT;

				ProcMsg(msgId, &respPort, sizeof(respPort));
				break;
			}

			case GUI_BUTTON_FACE_ENRL_STOP:
			{
				ProcMsg(GUI_BUTTON_FACE_ENRL_STOP,NULL,0);
				break;
			}
			case GUI_BUTTON_FACE_ENRL_CONTINUE:
			{
				ProcMsg(GUI_BUTTON_FACE_ENRL_CONTINUE,NULL,0);
				break;
			}
			case GUI_BUTTON_FACE_ENRL_CONTINUE_ABORT:
			{
				ProcMsg(GUI_BUTTON_FACE_ENRL_CONTINUE_ABORT,NULL,0);
				break;	
			}
			case GUI_BUTTON_POPUP_SCREEN_TIMEOUT:
			{
				ProcMsg(GUI_BUTTON_POPUP_SCREEN_TIMEOUT,NULL,0);
				break;
			}
			case GUI_BUTTON_FACE_ENRL_CANCEL:
			{
				ProcMsg(GUI_BUTTON_FACE_ENRL_CANCEL,NULL,0);
				break;
			}

			case GUI_BUTTON_FACE_ENRL_TERMINATE:
			{
				ProcMsg(GUI_BUTTON_FACE_ENRL_TERMINATE,NULL,0);
				break;
			}

            case MSG_AUTH_STATUSBAR_MENU_ACCESS_EVENT:
            {
                ProcMsg(MSG_AUTH_STATUSBAR_MENU_ACCESS_EVENT, (VOIDPTR)msgFormat, (UINT16)sizeof(AUTH_STATUSBAR_MENU_ACCESS_t));
                break;
            }

			default:
				ProcMsg(msgId, (VOIDPTR)msgFormat, msgLen);
				break;
		}
	}
}
extern ENROLLMENT_TEMP_INPUTS_t 		EnrollmentTempInputs;
//*****************************************************************************
//	SendMessageToQt()
//	Param:
//		IN : Pointer to a Message
//		   : Message Id
//		OUT: None
//	Returns: SUCCESS/FAIL
//	Description:This function is a global function and will  be called by
//		     	Main application to send any message with the message id
//		     	to Qt application.
//
//*****************************************************************************
BOOL SendMessageToQt(VOIDPTR msgPtr, GUI_MSG_ID_e msgId)
{
	CHAR		msgBuf[QT_RX_BUF_SIZE];
	UINT32		sendBytes;
	UINT16		bufLen;

	if(GetDoorApplType() == DIRECT_DOOR_APPL)
	{
		switch(msgId)
		{
			default:
				EPRINT(GUI_LOG, "SendMessageToQt(), Message is Invalid, Message id = 0x%x", msgId);
				return FALSE;
				break;

			// TODO: confirm why 'line1' argument missing

			case GUI_MSG_FR_SERV_ENABLE:
				sprintf(msgBuf, "%d%c%d%c%c",msgId,UI_MSG_FSP,FRServerConfig.enableFrServer,UI_MSG_FSP,EOT_FSP);
				break;

			case GUI_MSG_NOTICE_TEXT:	//Any single interactive message text e.g TIMEOUT
			{
				MSG_GEN_INFO_QT_t * info = msgPtr;

				sprintf(msgBuf,"%d%c%s%c%s%c%d%c%d%c%d%c%c",msgId, UI_MSG_FSP, info -> line2,
								UI_MSG_FSP, info -> line3, UI_MSG_FSP, info -> imageIdx, UI_MSG_FSP,
								info -> ifPinEntryBtn, UI_MSG_FSP, info -> ifEscBtn , UI_MSG_FSP , EOT_FSP);
				break;
			}

			case GUI_MSG_SPL_FUNC_STATUS:		//Screen to be shown with id,name,photo
												//and submit when enrolling new user
			{
				FUNC_STATUS_QT_t * funcInfo = msgPtr;

				sprintf(msgBuf, "%d%c%s%c%d%c%s%c%s%c%d%c%s%c%s%c%d%c%d%c%d%c%u%c%s%c%d%c%d%c%d%c%c",
								msgId,UI_MSG_FSP,funcInfo -> splFunc, UI_MSG_FSP, funcInfo -> ifUserInfo, UI_MSG_FSP,
								funcInfo -> user.alphaNumId, UI_MSG_FSP, funcInfo -> user.name, UI_MSG_FSP,
								funcInfo -> user.photo, UI_MSG_FSP, funcInfo-> genInfo.line1,
								UI_MSG_FSP, funcInfo -> genInfo.line2, UI_MSG_FSP,
								funcInfo -> genInfo.imageIdx,
								UI_MSG_FSP, funcInfo -> genInfo.ifPinEntryBtn, UI_MSG_FSP,
								funcInfo -> genInfo.ifEscBtn, UI_MSG_FSP, funcInfo -> user.userId, UI_MSG_FSP,
								funcInfo -> temperature, UI_MSG_FSP, funcInfo -> temperatureUnit, UI_MSG_FSP, 
								funcInfo -> temperatureViolation, UI_MSG_FSP, funcInfo -> faceMaskDecision, UI_MSG_FSP,
								EOT_FSP);
				break;
			}

			case GUI_MSG_DYNAMIC_POP_UP_SCREEN:
			{
				DYNAMIC_POPUP_INFO_QT_t * popUpInfo = msgPtr;

				sprintf(msgBuf, "%d%c%s%c%d%c%s%c%s%c%d%c%c",
								msgId,UI_MSG_FSP,popUpInfo -> titleText, UI_MSG_FSP, popUpInfo -> imageIdx, UI_MSG_FSP,
								popUpInfo -> message1, UI_MSG_FSP, popUpInfo -> message2, UI_MSG_FSP, popUpInfo -> footerHideF, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_TEMP_USER_POPUP_SCREEN:
			{
				TEMP_USER_POPUP_INFO_QT_t * popUpInfo = msgPtr;

				sprintf(msgBuf, "%d%c%s%c%d%c%s%c%s%c%d%c%c",
								msgId,UI_MSG_FSP,popUpInfo -> titleText, UI_MSG_FSP, popUpInfo -> imageIdx, UI_MSG_FSP,
								popUpInfo -> message1, UI_MSG_FSP, popUpInfo -> message2, UI_MSG_FSP, popUpInfo -> footerHideF, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_TEMP_USER_REGISTERED_SUCCESSFULLY:
			{
				TEMP_USER_COMMON_QT_t * tempUser = msgPtr;
				sprintf(msgBuf,"%d%c%s%c%s%c%u%c%c",
						msgId, UI_MSG_FSP, tempUser -> user.alphaNumId,UI_MSG_FSP,
								   tempUser -> user.name, UI_MSG_FSP,
								   tempUser -> user.userId, UI_MSG_FSP,
						EOT_FSP);
				break;
			}
			case GUI_MSG_SPL_FUNC_DYNAMIC_SCREEN:
			{
				DYNAMIC_SPL_FUNC_INFO_t * msgInfo = msgPtr;

				sprintf(msgBuf,"%d%c%s%c%d%c%s%c%d%c%d%c%c", msgId , UI_MSG_FSP , msgInfo->splFunc , UI_MSG_FSP ,
								msgInfo->splFuncImageIdx , UI_MSG_FSP , msgInfo->noticeLine , UI_MSG_FSP ,
								msgInfo->ifEscBtn , UI_MSG_FSP , msgInfo->ifPinEntryBtn ,UI_MSG_FSP , EOT_FSP);
				break;

			}

			case GUI_MSG_NET_CONN_NOT_AVAILABLE_SCREEN:
			{
				sprintf(msgBuf, "%d%c%c", msgId, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_CAFE_USER_DENIED:
			case GUI_MSG_USER_DENIED:	  				//Display User Denied Screen
			{
				USER_DENIED_QT_t * deny = msgPtr;

				sprintf(msgBuf,"%d%c%d%c%s%c%s%c%d%c%s%c%s%c%s%c%d%c%u%c%s%c%c", msgId,UI_MSG_FSP,deny -> ifUserInfo,
						UI_MSG_FSP, deny -> user.alphaNumId, UI_MSG_FSP, deny -> user.name, UI_MSG_FSP, deny -> user.photo,
						UI_MSG_FSP, deny -> genInfo.line1, UI_MSG_FSP, deny -> genInfo.line2, UI_MSG_FSP,
						deny -> genInfo.line3, UI_MSG_FSP, deny -> genInfo.imageIdx,
						UI_MSG_FSP, deny -> user.userId, UI_MSG_FSP, deny -> user.designation, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_CAFE_USER_DENIED_COMMON_SCREEN:	  		//Display Cafe User Denied Screen
			case GUI_MSG_USER_DENIED_COMMON_SCREEN:				//Display User Denied Common Screen
			{
				USER_COMMON_QT_t * deny = msgPtr;
				GetCurrentTimeForUserAllowScreen(deny->accessTime);
				sprintf(msgBuf,"%d%c%d%c%s%c%s%c%d%c%s%c%s%c%s%c%d%c%u%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%d%c%d%c%s%c%d%c%d%c%d%c%c",
						msgId, UI_MSG_FSP, deny->ifUserInfo, UI_MSG_FSP, deny -> user.alphaNumId, UI_MSG_FSP,
						deny -> user.name, UI_MSG_FSP,	deny -> user.photo, UI_MSG_FSP,
						deny -> genInfo.line1, UI_MSG_FSP, deny -> genInfo.line2, UI_MSG_FSP,
						deny -> genInfo.line3, UI_MSG_FSP, deny -> genInfo.imageIdx, UI_MSG_FSP,
						deny -> user.userId, UI_MSG_FSP, deny -> user.designation, UI_MSG_FSP,
						deny -> accessTime, UI_MSG_FSP, deny->matchingScore, UI_MSG_FSP,
						deny -> greetingInfo.msg1, UI_MSG_FSP, deny -> greetingInfo.msg2, UI_MSG_FSP,
						deny -> genInfo.line4, UI_MSG_FSP, deny -> genInfo.line5, UI_MSG_FSP,
						deny -> cafeSepF, UI_MSG_FSP, SysBasicPara.accessAllowScreenDuration, UI_MSG_FSP,
						deny -> temperature, UI_MSG_FSP, deny -> temperatureUnit, UI_MSG_FSP,
						deny -> temperatureViolation, UI_MSG_FSP, deny -> faceMaskDecision, UI_MSG_FSP,
						// deny -> isJobCodeAllowScreen, UI_MSG_FSP,
						EOT_FSP);
				break;
			}

			case GUI_MSG_CAFE_POLICY_USER_DENIED:
			{
				CAFE_POLICY_USER_DENIED_QT_t * deny = msgPtr;

				sprintf(msgBuf,"%d%c%d%c%s%c%s%c%d%c%s%c%s%c%s%c%d%c%u%c%c", msgId,UI_MSG_FSP,deny -> ifUserInfo,
						UI_MSG_FSP, deny -> user.alphaNumId, UI_MSG_FSP, deny -> user.name, UI_MSG_FSP, deny -> user.photo,
						UI_MSG_FSP, deny -> genInfo.line1, UI_MSG_FSP, deny -> genInfo.line2, UI_MSG_FSP,
						deny -> genInfo.line3, UI_MSG_FSP, deny -> genInfo.imageIdx,
						UI_MSG_FSP, deny -> user.userId, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_USER_ALLOWED:				//Display User Allowed Screen
			{
				USER_ALLOWED_QT_t * allow = msgPtr;

				sprintf(msgBuf,"%d%c%s%c%s%c%d%c%s%c%s%c%s%c%d%c%u%c%s%c%s%c%s%c%s%c%s%c%c", msgId, UI_MSG_FSP, allow -> user.alphaNumId, UI_MSG_FSP,
						allow -> user.name, UI_MSG_FSP, allow -> user.photo, UI_MSG_FSP, allow -> genInfo.line1,
						UI_MSG_FSP, allow -> genInfo.line2, UI_MSG_FSP, allow -> genInfo.line3, UI_MSG_FSP,
						allow -> genInfo.imageIdx, UI_MSG_FSP, allow -> user.userId, UI_MSG_FSP,
						allow -> jobCodeInfo.jobCodeId, UI_MSG_FSP, allow -> jobCodeInfo.jobCodeName, UI_MSG_FSP, allow -> jobCodeInfo.jobActionStr, UI_MSG_FSP,
						allow -> allowTime, UI_MSG_FSP, allow -> user.designation, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_CAFE_USER_ALLOWED_COMMON_SCREEN:			//Display Cafe User Allowed Screen
			case GUI_MSG_USER_ALLOWED_COMMON_SCREEN:				//Display User Allowed Common Screen
			{
				USER_COMMON_QT_t * allow = msgPtr;
				allow->ifUserInfo = TRUE;
				GetCurrentTimeForUserAllowScreen(allow->accessTime);
				sprintf(msgBuf,"%d%c%d%c%s%c%s%c%d%c%s%c%s%c%s%c%d%c%u%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%d%c%u%c%s%c%d%c%d%c%d%c%s%c%d%c%c",
						msgId, UI_MSG_FSP, allow -> ifUserInfo, UI_MSG_FSP, allow -> user.alphaNumId, UI_MSG_FSP,
						allow -> user.name, UI_MSG_FSP,	allow -> user.photo, UI_MSG_FSP,
						allow -> genInfo.line1, UI_MSG_FSP, allow -> genInfo.line2, UI_MSG_FSP, allow -> genInfo.line3, UI_MSG_FSP,
						allow -> genInfo.imageIdx, UI_MSG_FSP, allow -> user.userId, UI_MSG_FSP,
						allow -> user.designation, UI_MSG_FSP, allow -> accessTime, UI_MSG_FSP,
						allow -> matchingScore, UI_MSG_FSP, allow -> greetingInfo.msg1, UI_MSG_FSP, allow -> greetingInfo.msg2, UI_MSG_FSP,
						allow -> genInfo.line4, UI_MSG_FSP, allow -> genInfo.line5, UI_MSG_FSP, 
						allow -> cafeSepF, UI_MSG_FSP, SysBasicPara.accessAllowScreenDuration, UI_MSG_FSP,
						allow -> temperature, UI_MSG_FSP, allow -> temperatureUnit, UI_MSG_FSP, allow -> temperatureViolation, UI_MSG_FSP,
						allow -> faceMaskDecision, UI_MSG_FSP,allow -> genInfo.line6, UI_MSG_FSP, GetUserAllowEntryExitModeFlag(), UI_MSG_FSP, /*allow -> isJobCodeAllowScreen, UI_MSG_FSP,*/
						EOT_FSP);
				break;
			}

			case GUI_MSG_CAFE_USER_ALLOWED:
			{
				CAFE_USER_ALLOWED_QT_t * cafeAllow = msgPtr;

				sprintf(msgBuf,"%d%c%s%c%s%c%d%c%s%c%s%c%s%c%s%c%s%c%d%c%u%c%d%c%c", msgId,UI_MSG_FSP,cafeAllow -> user.alphaNumId, UI_MSG_FSP,
									cafeAllow -> user.name, UI_MSG_FSP, cafeAllow -> user.photo, UI_MSG_FSP, cafeAllow -> genInfo.line1, UI_MSG_FSP,
									cafeAllow -> genInfo.line2, UI_MSG_FSP, cafeAllow -> genInfo.line3, UI_MSG_FSP,cafeAllow -> genInfo.line4, UI_MSG_FSP,
									cafeAllow -> genInfo.line5, UI_MSG_FSP, cafeAllow ->genInfo.imageIdx, UI_MSG_FSP,
									cafeAllow -> user.userId, UI_MSG_FSP, cafeAllow ->cafeSepF, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_EHTERNET_CFG_SCREEN:			//Ethernet Address
			{
				sprintf(msgBuf,"%d%c%s%c%s%c%s%c%s%c%s%c%d%c%c",msgId,UI_MSG_FSP, EthernetParameters.ipAddress, UI_MSG_FSP,
							EthernetParameters.subnet, UI_MSG_FSP, EthernetParameters.gateway, UI_MSG_FSP,
							EthernetParameters.dns1, UI_MSG_FSP, EthernetParameters.dns2, UI_MSG_FSP, EthernetParameters.nwType, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_ACMS_SERVER_CFG_SCREEN:
			{
				if(ServerModeCfg.serverMode == COSEC_THIRD_PARTY)
				{
					sprintf(msgBuf,"%d%c%d%c%s%c%d%c%s%c%c",msgId,UI_MSG_FSP,
									ServerModeCfg.serverMode, UI_MSG_FSP,
									PushAPIConfig.pushApiUrl, UI_MSG_FSP,
									PushAPIConfig.pushApiPort, UI_MSG_FSP,
									"", UI_MSG_FSP, EOT_FSP);
				}
				else if(strcmp(ServerParameters.TenantIdVyom, DFLT_TENANT_ID_VYOM) == 0)
				{
					sprintf(msgBuf,"%d%c%d%c%s%c%d%c%s%c%c",msgId,UI_MSG_FSP,
									ServerModeCfg.serverMode, UI_MSG_FSP,
									ServerParameters.ethServerHostName, UI_MSG_FSP,
									ServerParameters.ethServerPort, UI_MSG_FSP,
									"", UI_MSG_FSP, EOT_FSP);
				}
				else
				{
					sprintf(msgBuf,"%d%c%d%c%s%c%d%c%s%c%c",msgId,UI_MSG_FSP,
									ServerModeCfg.serverMode, UI_MSG_FSP,
									ServerParameters.ethServerHostName, UI_MSG_FSP,
									ServerParameters.ethServerPort, UI_MSG_FSP,
									ServerParameters.TenantIdVyom, UI_MSG_FSP, EOT_FSP);
				}
				break;
			}

			case GUI_MSG_WIFI_CFG_SCREEN:
			{
			    UINT8 length;

			    sprintf(msgBuf,"%d%c%s%c%d%c%s%c%d%c%s%c%s%c%s%c%s%c%s%c%d%c%d%c%d%c%s%c%s%c%d%c%d%c%d%c",msgId,UI_MSG_FSP,
				    WifiParameters.ssid, UI_MSG_FSP, WifiParameters.secType, UI_MSG_FSP,
				    WifiParameters.passphresh, UI_MSG_FSP, WifiParameters.nwType, UI_MSG_FSP,
				    WifiParameters.ipAddress, UI_MSG_FSP, WifiParameters.subnet, UI_MSG_FSP,
				    WifiParameters.gateway ,UI_MSG_FSP, WifiParameters.dns1 ,UI_MSG_FSP,
				    WifiParameters.dns2 ,UI_MSG_FSP, GetWiFiStatus(),UI_MSG_FSP,
				    WifiParameters.keymgmt, UI_MSG_FSP, WifiParameters.advAuthType, UI_MSG_FSP,
				    WifiParameters.anonymousIdentity, UI_MSG_FSP, WifiParameters.userName, UI_MSG_FSP,
				    WifiParameters.peapVer, UI_MSG_FSP, WifiParameters.innerAuth, UI_MSG_FSP,
				    WifiParameters.enableWifi, UI_MSG_FSP);

			    length = strlen(msgBuf);

			    if(GetWiFiStatus() == WIFI_CONNECTED)
			    {
				snprintf(&msgBuf[length], (QT_RX_BUF_SIZE-length),"%d%c%d%c%c", GetConnectedSsidSignalStrength(),UI_MSG_FSP,WifiParameters.bandFreqType, UI_MSG_FSP, EOT_FSP);
			    }
			    else
			    {
				snprintf(&msgBuf[length], (QT_RX_BUF_SIZE-length),"%d%c%c",WifiParameters.bandFreqType,UI_MSG_FSP, EOT_FSP);
			    }
			    GetPereodicWiFiSignalStrength();
			    break;
			}

			case GUI_MSG_WIFI_SIGNAL_STRENGTH:
			{
				WIFI_SIGNAL_QUALITY_e sigCheck = *(WIFI_SIGNAL_QUALITY_e *)msgPtr;
				snprintf(msgBuf,QT_RX_BUF_SIZE,"%d%c%d%c%s%c%d%c%d%c%d%c%c",msgId,UI_MSG_FSP,sigCheck,UI_MSG_FSP,WifiParameters.ssid,UI_MSG_FSP,WifiParameters.secType,UI_MSG_FSP,WifiParameters.enableWifi,UI_MSG_FSP,WifiParameters.bandFreqType,UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_MODEM_PROFILE_SCREEN:
			{
				UINT8 idx;
				UINT8 length;

				sprintf(msgBuf,"%d%c%d%c",msgId,UI_MSG_FSP,MAX_MODEM_PROFILE,UI_MSG_FSP);
				length = strlen(msgBuf);

				for(idx=0; idx < MAX_MODEM_PROFILE ; idx++)
				{
					sprintf(&msgBuf[length],"%d%c%s%c",ModemProfile[idx].enable,DESH_FSP,
							ModemProfile[idx].profileName,UI_MSG_FSP);
					length = strlen(msgBuf);
				}
				sprintf(&msgBuf[length],"%d%c%c", GetModemStatus(), UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_MODEM_PROFILE_DATA:
			{
				UINT8 length;

				sprintf(msgBuf,"%d%c%d%c%d%c%s%c%s%c%s%c%s%c%d%c%s%c%d%c",
								msgId,UI_MSG_FSP,modemData.profileIdx,UI_MSG_FSP,ModemProfile[modemData.profileIdx].enable,UI_MSG_FSP,
								ModemProfile[modemData.profileIdx].profileName, UI_MSG_FSP, ModemProfile[modemData.profileIdx].dialNo,
								UI_MSG_FSP, ModemProfile[modemData.profileIdx].userName,
								UI_MSG_FSP, ModemProfile[modemData.profileIdx].userPswd, UI_MSG_FSP,
								ModemProfile[modemData.profileIdx].modemType, UI_MSG_FSP, ModemProfile[modemData.profileIdx].apn,
								UI_MSG_FSP, BroadBandNwPara.modemStatus, UI_MSG_FSP);

				length = strlen(msgBuf);

				if(BroadBandNwPara.modemStatus == CONNECTED)
				{
					sprintf(&msgBuf[length],"%s%c%s%c%s%c%s%c%c",BroadBandNwPara.ipAddress[MODEM_NW_ADDR_TYP_DISP], UI_MSG_FSP, BroadBandNwPara.gateway[MODEM_NW_ADDR_TYP_DISP],
							UI_MSG_FSP, BroadBandNwPara.dns1[MODEM_NW_ADDR_TYP_DISP], UI_MSG_FSP, BroadBandNwPara.dns2[MODEM_NW_ADDR_TYP_DISP], UI_MSG_FSP ,EOT_FSP );
				}
				else
				{
					sprintf(&msgBuf[length],"%c",EOT_FSP);
				}
				break;
			}

			case GUI_MSG_MANAGE_CANTEEN_SCREEN:
			{
				BOOL printerEnable;

				printerEnable = ( (PrinterConfig.printerType != PRINTER_NONE) ? ENABLED : DISABLED);

				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%c", msgId, UI_MSG_FSP, printerEnable, UI_MSG_FSP,
								SpecialFunctions[ITEM_SOLD_OUT].enable, UI_MSG_FSP,
								SpecialFunctions[ITEM_AVAILABLE].enable, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_HRD_MENU_SCREEN:
			{
				MUL_USR_SPL_FUN_QT_t	mulUsrSplFn;

				GetMultiUserSplFnStatus(&mulUsrSplFn);
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%c",msgId,UI_MSG_FSP,	//Only Message Id Is Required
								mulUsrSplFn.lateInEnable, UI_MSG_FSP, mulUsrSplFn.lateInStartStop, UI_MSG_FSP,
								mulUsrSplFn.earlyOutEnable, UI_MSG_FSP, mulUsrSplFn.earlyOutStartStop, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_ADMIN_MENU_SCREEN:
			{
				MUL_USR_SPL_FUN_QT_t	mulUsrSplFn;
				BOOL temperatureIconEnable = FALSE;
                if((IsTempratureConfigApplicable() == TRUE))
				{
					if(SysAdvPara.temperatureLogCnfg.sensorType != SENSOR_FEVO) //To manage temperature icon
					{
						temperatureIconEnable = TRUE;
					}
				}
				GetMultiUserSplFnStatus(&mulUsrSplFn);
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%c",msgId, UI_MSG_FSP , UsbInfo.status , UI_MSG_FSP,
								mulUsrSplFn.lateInEnable, UI_MSG_FSP, mulUsrSplFn.lateInStartStop, UI_MSG_FSP,
								mulUsrSplFn.earlyOutEnable, UI_MSG_FSP, mulUsrSplFn.earlyOutStartStop , UI_MSG_FSP,
								SpecialFunctions[CLEAR_SYS_ALARM].enable , UI_MSG_FSP,
								temperatureIconEnable , UI_MSG_FSP , EOT_FSP);
				break;
			}

			case GUI_MSG_TOUCH_PANEL_TEST_SCREEN:
			case GUI_MSG_E_CANTEEN_MENU_SCREEN:
			case GUI_MSG_DISPLAY_MENU_SCREEN:
			case GUI_MSG_BACKGROUND_SELECTION_SCREEN:
			case GUI_MSG_CLOCK_SELECTION_SCREEN:
			case GUI_MSG_KEYPAD_SELECTION_SCREEN:
			case GUI_MSG_MENU_LOGIN_SCREEN:
			case GUI_MSG_DISPLAY_IDLE_SCREEN:
			case GUI_MSG_ADMIN_BACKUP_AND_UPDATE_SCREEN:
			case GUI_MSG_CAFE_NOT_SERVING_SCREEN:
			case GUI_MSG_START_GUI_APPLICATION:
			case GUI_MSG_REFRESH_WALLPAPER:
			case GUI_MSG_MENU_PASSWORD_SCREEN:
			case GUI_MSG_SELF_ENROLLMENT_SCREEN:
			case GUI_MSG_DOOR_MODE_SELECTION_SCREEN:
			case GUI_MSG_INIT_FIRMWARE_UPGRADE:
			case GUI_MSG_ENTER_USER_NAME:
			case GUI_MSG_INSTALLATION_SCREEN_DEVICE_IP:
			case GUI_MSG_INSTALLATION_SCREEN_SERVER_IP:
			case GUI_MSG_INSTALLATION_SCREEN_VYOM_TENANT_ID:
			case GUI_MSG_START_AUTO_SYSTEM_TEST_SCREEN:
			case GUI_MSG_AUTO_SYSTEM_TEST_RUNNING_SCREEN:
			case GUI_MSG_AMBIENT_TEMP_DETECTING_SCREEN:
			case GUI_MSG_FACE_DETECTING_WAIT_SCREEN:
			case GUI_MSG_TEMP_USER_CREATE_CONFIRM_SCREEN:
			case GUI_MSG_TEMP_USER_ENTER_NAME_SCREEN:
			{
				if(msgId == GUI_MSG_DISPLAY_IDLE_SCREEN && CheckForInfiniteFreeScanFailState() == YES )
				{
					return FALSE;
				}
				sprintf(msgBuf, "%d%c%c",msgId,UI_MSG_FSP,EOT_FSP);	//Only Message Id Is Required
				break;
			}
			case GUI_MSG_RELOAD_IMAGE:
			{
				UINT8 *Flag=msgPtr;
				sprintf(msgBuf, "%d%c%d%c%c",msgId,UI_MSG_FSP,*Flag,UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_FACE_ENROLL_PROCESSING_SCREEN:
			{
				CHARPTR *progressdata = (CHARPTR)msgPtr;
				sprintf(msgBuf, "%d%c%s%c%c",msgId,UI_MSG_FSP,progressdata,UI_MSG_FSP,EOT_FSP);
				break;
			}
			
			case GUI_MSG_FACE_ENROLL_USER_POPUP_SCREEN:
			{
				ENROLL_FACE_MSG_INFO_TO_QT_t *progressdata = msgPtr;
// 				snprintf(msgBuf,QT_RX_BUF_SIZE,"%d%c%d%c%d%c%s%c%s%c%c",msgId,UI_MSG_FSP,progressdata->popupType,UI_MSG_FSP,
// 																progressdata->warningIconShow,UI_MSG_FSP,
// 																progressdata->AlertString,UI_MSG_FSP,
// 																progressdata->CountDownString,UI_MSG_FSP,
// 																EOT_FSP);
				snprintf(msgBuf,QT_RX_BUF_SIZE,"%d%c%d%c%d%c%d%c%s%c%s%c%c",msgId,UI_MSG_FSP,progressdata->timeOutSec,UI_MSG_FSP,
												progressdata->popupType,UI_MSG_FSP,
												progressdata->warningIconShow,UI_MSG_FSP,
												progressdata->AlertString,UI_MSG_FSP,
												progressdata->CountDownString,UI_MSG_FSP,
												EOT_FSP);
				
				break;
			}
			case GUI_MSG_FR_CAMERA_CONNECTION_FAILED:
			case GUI_MSG_FR_CAMERA_CONNECTION_FAILED_INFINITE:
			{
				sprintf(msgBuf, "%d%c%s%c",msgId,UI_MSG_FSP, GetDisplayMessage(FALSE,NULL_CHAR,MSG_CAMERA_CONNECTIVITY_LOST_1019), EOT_FSP);
				break;
			}

			case GUI_MSG_FR_STREAM_STATUS:
			{
				UINT8 *Flag=msgPtr;
				sprintf(msgBuf, "%d%c%d%c%c",msgId,UI_MSG_FSP,*Flag,UI_MSG_FSP,EOT_FSP);
				break;
			}
			case GUI_MSG_BRIGHTNESS_SELECTION_SCREEN:
			{
				sprintf(msgBuf, "%d%c%d%c%d%c%c",msgId,UI_MSG_FSP,
						DisplayBrightness.sleepTimer, UI_MSG_FSP,
						DisplayBrightness.setBrightness, UI_MSG_FSP,
								EOT_FSP);
				break;
			}
			case GUI_MSG_ASSIGN_USER_ON_DEVICE:
			{
				ENROLL_USER_t * enrollInput = msgPtr;
				sprintf(msgBuf, "%d%c%s%c%s%c%c",msgId,UI_MSG_FSP,
								enrollInput -> alphaNumId, UI_MSG_FSP,
								enrollInput -> name, UI_MSG_FSP,
								EOT_FSP);
				break;
			}
			case GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE:
			{
				ENROLL_USER_t * enrollInput = msgPtr;
				sprintf(msgBuf, "%d%c%s%c%s%c%c",msgId,UI_MSG_FSP,
								enrollInput -> alphaNumId, UI_MSG_FSP,
								enrollInput -> name, UI_MSG_FSP,
								EOT_FSP);
				break;
			}

			case GUI_MSG_TRANSACTION_WITHOUT_DISCOUNT:
			{
				USER_QT_t	*userInfo = msgPtr;

				sprintf(msgBuf,"%d%c%s%c%s%c%d%c%u%c%c", msgId,UI_MSG_FSP,
						userInfo->alphaNumId, UI_MSG_FSP,
						userInfo->name, UI_MSG_FSP,
						userInfo->photo, UI_MSG_FSP,
						userInfo->userId, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_PIN_ENTER_SCREEN:
			{
				sprintf(msgBuf, "%d%c%d%c%c",msgId,UI_MSG_FSP,SysAdvPara.showPin, UI_MSG_FSP,EOT_FSP);	//Only Message Id Is Required
				break;
			}

			case GUI_MSG_ENROLL_SPL_FN_SCREEN:
			{
				sprintf(msgBuf,"%d%c%d%c%d%c%c",msgId, UI_MSG_FSP, ( (SysAdvPara.cardCount < MAX_SP_FUNC_CARD) ? SysAdvPara.cardCount : MAX_SP_FUNC_CARD ),
													UI_MSG_FSP, MAX_SP_FUNC_CARD, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_ADMIN_DOOR_CONTROL_SCREEN:
			{
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%c",msgId,UI_MSG_FSP,SpecialFunctions[LOCK_DOOR].enable,UI_MSG_FSP,
										SpecialFunctions[UNLOCK_DOOR].enable,UI_MSG_FSP,
										SpecialFunctions[NORMAL_DOOR].enable,UI_MSG_FSP,
										//Dummy Field for Panel door - Door ARM & DISARM
										DISABLED, UI_MSG_FSP, DISABLED, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_ENROLLMENT_SCREEN:
			{
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%c",msgId, UI_MSG_FSP,	//Only Message Id Is Required
												SpecialFunctions[ENROL_USER].enable, UI_MSG_FSP,
												SpecialFunctions[ENROL_SPECIAL_CARDS].enable, UI_MSG_FSP,
												SpecialFunctions[ENROL_MODE_DELETE_DATA].enable, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_ADMIN_SETTING_SCREEN:
			{
				sprintf(msgBuf, "%d%c%s%c",msgId,UI_MSG_FSP, (CHARPTR) msgPtr,EOT_FSP);
				break;
			}

			//Note:- Always Update SW_VER and SW_REV while releasing
			case GUI_MSG_SYS_STATUS_SCREEN:
			{
				CHAR	verRevBuff[10] = {0};

				snprintf(verRevBuff, sizeof(verRevBuff), "V%02dR%02d.%02d", SW_VER, SW_REV, SW_MINOR_REV);
				sprintf(msgBuf, "%d%c%s%c%s%c%c",
								msgId,UI_MSG_FSP,
								verRevBuff, UI_MSG_FSP,
								EthernetParameters.macAddress,UI_MSG_FSP,EOT_FSP);

				break;
			}

			case GUI_MSG_ABOUT_DEVICE_CONNECTIVITY_SCREEN:
			{
				bufLen = 0;
				snprintf(msgBuf,sizeof(msgBuf),"%d%c",msgId,UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",EthernetParameters.ipAddress, UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetServerIpAddressPtr(acmsCurActiveInterface), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAcmsStatusStr[GetAcmsStatus()]), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",HandleWifiStatusStr(getSysWifiStatusStr[GetWiFiStatus()]), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c%c",HandleModemStatusStr(getSysModemStatusStr[GetModemStatus()]), UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_ABOUT_DEVICE_READERS_INTERFACES_SCREEN:
			{
				CHAR	readerStatusBuff[25] = {0};
				CHAR	readerStatusBuff1[25] = {0};
				MONITORED_DOOR_STATUS_t doorStatus;
				UINT8 	length;

				if(readerStatus[READER1] == READER_IS_ACTIVE)
				{
					sprintf(readerStatusBuff, sysReaderTypeStr[SysBasicPara.reader[READER1].type]);
				}
				else
				{
					sprintf(readerStatusBuff,GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER1]]));
				}

				bufLen = 0;
				snprintf(msgBuf,sizeof(msgBuf),"%d%c",msgId, UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",(CHARPTR)readerStatusBuff,UI_MSG_FSP);
				bufLen = strlen(msgBuf);

				if(YES == IsVariant_With_Internal_FP_Rdr())
				{
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER2]]), UI_MSG_FSP);
				}

				length = strlen(msgBuf);

				if(SysBasicPara.cerPresentF == TRUE)
				{
					if((readerStatus[READER3] == READER_IS_ACTIVE))
					{
						sprintf(readerStatusBuff, sysReaderTypeStr[SysBasicPara.reader[READER3].type]);
					}
					else
					{
						sprintf(readerStatusBuff, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER3]]));
					}
					sprintf(readerStatusBuff1, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER4]]));
				}
				else if((SysBasicPara.cbrPresentF == TRUE) || (SysBasicPara.atomRD100PresentF == TRUE))
				{
					if((readerStatus[READER3] == READER_IS_ACTIVE))
					{
						sprintf(readerStatusBuff, sysReaderTypeStr[SysBasicPara.reader[READER3].type]);
					}
					else
					{
						sprintf(readerStatusBuff, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER3]]));
					}
					sprintf(readerStatusBuff1, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[READER_NOT_CONFIGURED]));
				}
				else if((SysBasicPara.atomRD200PresentF == TRUE) || (SysBasicPara.atomRD300PresentF == TRUE))
				{
					if((readerStatus[READER3] == READER_IS_ACTIVE))
					{
						sprintf(readerStatusBuff, sysReaderTypeStr[SysBasicPara.reader[READER3].type]);
					}
					else
					{
						sprintf(readerStatusBuff, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER3]]));
					}
					sprintf(readerStatusBuff1, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER5]]));
				}
				else
				{
					if(readerStatus[READER3] == READER_IS_ACTIVE)
					{
						if(SysBasicPara.reader[READER3].type != SUP_FP_3030_READER)
						{
							sprintf(readerStatusBuff, sysReaderTypeStr[SysBasicPara.reader[READER3].type]);
							sprintf(readerStatusBuff1, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER4]]));
						}
						else
						{
							sprintf(readerStatusBuff, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER4]]));
							sprintf(readerStatusBuff1, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER3]]));
						}
					}
					else
					{
						sprintf(readerStatusBuff, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER3]]));
						sprintf(readerStatusBuff1, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER3]]));
					}
				}

				sprintf(&msgBuf[length],"%s%c%s%c",(CHARPTR)readerStatusBuff,UI_MSG_FSP,(CHARPTR)readerStatusBuff1, UI_MSG_FSP);

				//getting current door status from system
				GetDoorStatus(&doorStatus);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysStorageDevStatusStr[MmcInfo.status]), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysExitSwitchStr[doorStatus.exitSwitch]), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysDoorStatusStr[doorStatus.doorLock]), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysDoorMonStatusStr[doorStatus.doorSense]), UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_ABOUT_DEVICE_ALARMS_SCREEN:
			{
				bufLen = 0;
				snprintf(msgBuf,sizeof(msgBuf),"%d%c",msgId, UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[Alarms[RTC_FAULT_ALARM].status]), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[Alarms[TAMPER_ALARM].status]), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[Alarms[DOOR_ABNORMAL_ALARM].status]), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[Alarms[DOOR_FORCE_OPEN_ALARM].status]), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[Alarms[DOOR_FAULT_ALARM].status]), UI_MSG_FSP, EOT_FSP);
				bufLen = strlen(msgBuf);
				break;
			}

			case GUI_MSG_BACKUP_UPDATE_PROG:
			{
				BACKUP_UPDATE_PROGRESS_QT_t * progress = msgPtr;

				sprintf(msgBuf, "%d%c%s%c%s%c%d%c%c", msgId,UI_MSG_FSP,progress -> splFunc, UI_MSG_FSP,
												progress -> genInfo.line1, UI_MSG_FSP,
												progress -> prog, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_FIRMWARE_UPLOAD_PROG:
			{
				FIRMWARE_UPLOAD_PROGRESS_QT_t * progress = msgPtr;

				sprintf(msgBuf, "%d%c%s%c%s%c%d%c%c", msgId,UI_MSG_FSP,progress -> splFunc, UI_MSG_FSP,
												progress -> genInfo.line1, UI_MSG_FSP,
												0, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_STARTUP_INFO:
			{
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%u%c%d%c%c",msgId,UI_MSG_FSP,SysBasicPara.timeFormat, UI_MSG_FSP,
				SysVar.backgroundImage, UI_MSG_FSP, SysVar.currentClock , UI_MSG_FSP, GetDiffBetGmtAndLocalTime(), UI_MSG_FSP,
			    SysVar.currentKeypad, UI_MSG_FSP, SysBasicPara.application, UI_MSG_FSP, GetDoorApplType(), UI_MSG_FSP, MultiLangConfig.multiLangEnblF, UI_MSG_FSP,
			    SysAdvPara.autoHideMenuBarF,UI_MSG_FSP,IsVariant_With_Internal_FP_Rdr(),UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_MULTILANG_FLAG:
			{
				sprintf(msgBuf, "%d%c%d%c%c",msgId,UI_MSG_FSP,MultiLangConfig.multiLangEnblF, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_MILTILANG_FONT_LOAD:
			{
				sprintf(msgBuf, "%d%c%s%c%c",msgId,UI_MSG_FSP, FONTFILE_PROCESS_FILE_PATH, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_SEND_CONFIG_INFO:
			{
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%u%c%c",msgId,UI_MSG_FSP,SysBasicPara.timeFormat, UI_MSG_FSP, GetDiffBetGmtAndLocalTime(), UI_MSG_FSP,
								DispDoorModeF, UI_MSG_FSP, DoorEntryExitMode, UI_MSG_FSP,
								SysBasicPara.entryModeWallpaperF, UI_MSG_FSP, SysBasicPara.exitModeWallpaperF, UI_MSG_FSP, SysAdvPara.autoHideMenuBarF, UI_MSG_FSP, EOT_FSP);
				
				if(getDeviceInactiveStatus() == TRUE)
				{
				    snprintf(msgBuf, QT_RX_BUF_SIZE,  "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%u%c%c",msgId,UI_MSG_FSP,SysBasicPara.timeFormat, UI_MSG_FSP, GetDiffBetGmtAndLocalTime(), UI_MSG_FSP,
								FALSE, UI_MSG_FSP, SysBasicPara.reader[READER1].mode, UI_MSG_FSP,
								DfltBasicSysPara.entryModeWallpaperF, UI_MSG_FSP, DfltBasicSysPara.exitModeWallpaperF, UI_MSG_FSP, SysAdvPara.autoHideMenuBarF, UI_MSG_FSP, EOT_FSP);
				}
				break;
			}

			case GUI_MSG_ENROLL_USER_SCREEN:
			{
				ENROLL_USER_t * enrollInput = msgPtr;
                sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%s%c%s%c%d%c%d%c%c",msgId,UI_MSG_FSP,
								enrollInput -> credType, UI_MSG_FSP,
								enrollInput -> fpCount, UI_MSG_FSP,
								enrollInput -> cardCount, UI_MSG_FSP,
								MAX_USER_CARD , UI_MSG_FSP ,
								SysAdvPara.maxFingerPerUser , UI_MSG_FSP ,
								SysAdvPara.enrollIdType , UI_MSG_FSP ,
								enrollInput -> alphaNumId, UI_MSG_FSP,
								enrollInput -> name, UI_MSG_FSP,
								SysAdvPara.maxFacePerUser , UI_MSG_FSP ,
                                EnrollmentTempInputs.faceCnt ,UI_MSG_FSP,
								EOT_FSP);
				break;
			}

			case GUI_MSG_DEL_CRED_SCREEN:
			{
				DELETE_USER_CREDEN_t *deleteInput = msgPtr;
				sprintf(msgBuf, "%d%c%d%c%d%c%c", msgId,UI_MSG_FSP,deleteInput -> credType, UI_MSG_FSP, SysAdvPara.enrollIdType, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_GOING_IN_SCREEN:
			{
				if(SetDisplayClockScreenTimer() == FAIL)
				{
					EPRINT(GUI_LOG,"Enable to start timer for Going in Screen");
				}
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%c", msgId,UI_MSG_FSP, SpecialFunctions[MARK_CLOCK_IN].enable, UI_MSG_FSP,
															SpecialFunctions[CLK_IN_ENTRY_OFFICIAL_WORK].enable, UI_MSG_FSP,
															SpecialFunctions[CLK_IN_MARKING_BREAK].enable, UI_MSG_FSP,
															SpecialFunctions[SHORT_LEAVE_MARKING_IN].enable, UI_MSG_FSP,
															SpecialFunctions[OVER_TIME_MARKING_IN].enable, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_GOING_OUT_SCREEN:
			{
				if(SetDisplayClockScreenTimer() == FAIL)
				{
					EPRINT(GUI_LOG,"Enable to start timer for Going out Screen");
				}
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%c", msgId,UI_MSG_FSP, SpecialFunctions[MARK_CLOCK_OUT].enable, UI_MSG_FSP,
															SpecialFunctions[CLK_OUT_EXIT_OFFICIAL_WORK].enable, UI_MSG_FSP,
															SpecialFunctions[CLK_OUT_MARKING_BREAK].enable, UI_MSG_FSP,
															SpecialFunctions[SHORT_LEAVE_MARKING_OUT].enable, UI_MSG_FSP,
															SpecialFunctions[OVER_TIME_MARKING_OUT].enable, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_MENU_SCREEN:
			{
				if(SetDisplayClockScreenTimer() == FAIL)
				{
					EPRINT(GUI_LOG,"Enable to start timer for Menu Screen");
				}
				sprintf(msgBuf, "%d%c%d%c%c", msgId , UI_MSG_FSP, SysBasicPara.application ,
											UI_MSG_FSP , EOT_FSP);
				break;
			}

			case GUI_MSG_SEARCH_WIFI_SCREEN:
			{
				sprintf(msgBuf, "%d%c%s%c", msgId,UI_MSG_FSP,(CHARPTR) msgPtr,EOT_FSP);	//msgBuf Already Filled From Menu
				break;
			}

			case GUI_MSG_DATE_TIME_SCREEN:				//DATETIME settings menu screen
			{
				sprintf(msgBuf, "%d%c%d%c%c",
						msgId,UI_MSG_FSP,SysBasicPara.timeUpdateMode, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_CHANGE_PW_SCREEN:
			{
				sprintf(msgBuf, "%d%c%s%c%s%c%c", msgId,UI_MSG_FSP,Password.displayPswd[USER_ADMIN],UI_MSG_FSP,Password.displayPswd[USER_HRD],UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_STATUSBAR_SCREEN:
			{
				STATUS_BAR_INFO_t * statusBar = msgPtr;
				CHAR TemperatureStr[10] = "";

				if (statusBar -> temperature != 0.0)
				{
					if(statusBar -> tempUnit == UNIT_FAHRENHEIT)
					{
						snprintf(TemperatureStr, sizeof(TemperatureStr), "%.1f˚F", statusBar -> temperature);
					}
					else if(statusBar -> tempUnit == UNIT_CELCIUS)
					{
						snprintf(TemperatureStr, sizeof(TemperatureStr), "%.1f˚C", statusBar -> temperature);
					}
				}

				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%s%c%d%c%d%c%c",msgId,UI_MSG_FSP,
						statusBar -> ethStatus, UI_MSG_FSP,
						statusBar -> wifiStatus, UI_MSG_FSP,
						statusBar -> modemStatus, UI_MSG_FSP,
						statusBar -> alarmStatus, UI_MSG_FSP,
						statusBar -> doorLockStatus,UI_MSG_FSP,
						statusBar -> serverStatus,UI_MSG_FSP,
						statusBar -> usbStatus,UI_MSG_FSP,
						statusBar -> lateInEarlyOutStatus , UI_MSG_FSP,
						statusBar -> goingInEnableF, UI_MSG_FSP,
						statusBar -> goingOutEnableF,UI_MSG_FSP,
						statusBar -> downloadingF,UI_MSG_FSP,
						statusBar -> bluetoothStatus,UI_MSG_FSP,
						statusBar -> temperatureSensorStatusF, UI_MSG_FSP,
						TemperatureStr, UI_MSG_FSP,
                        IsTempratureConfigApplicable(), UI_MSG_FSP,
                        			statusBar->FsmSyncStatus, UI_MSG_FSP,
						EOT_FSP);

				break;
			}

			case GUI_MSG_SET_MMC_DEV_PATH:
			{
				MEDIA_FILE_PATH_t filePath;

				if(strlen(MmcInfo.devPath))
				{
					sprintf(filePath.userPicsFile, "%s/%s/%s", MmcInfo.devPath, USER_PICS_FOLDER, USER_PICS_PREFIX);
				}
				else
				{
					sprintf(filePath.userPicsFile, "%s", "");
				}
				sprintf(msgBuf, "%d%c%s%c%c", msgId, UI_MSG_FSP, filePath.userPicsFile, UI_MSG_FSP, EOT_FSP);
				break;
			}

			// Common handling if 'msgBuf' is already Filled
			case GUI_MSG_ENROLL_GET_NAME_FROM_ID: 			//Name From Id
			case GUI_MSG_SEARCHED_ENROLL_USER_LIST: 		//Searched User name List
			case GUI_MSG_DEL_CRED_GET_NAME_FROM_ID: 		//Name From Id
			case GUI_MSG_SEARCHED_DEL_CRED_USER_LIST: 		//Searched User name List
			case GUI_MSG_CAFE_ITEM_LIST_SCREEN:
			case GUI_MSG_CAFE_SOLDOUT_ITEM_SCREEN:
			case GUI_MSG_CAFE_AVAILABLE_ITEM_SCREEN:
			case GUI_MSG_CAFE_ITEM_SELECTION:
			case GUI_MSG_CAFE_LIST_SCREEN_UPDATE:
			{
				sprintf(msgBuf, "%d%c%s%c", msgId, UI_MSG_FSP, (CHARPTR) msgPtr, EOT_FSP);
				break;
			}

			case GUI_MSG_VALIDATION_RESPONSE:
			{
				DYNAMIC_VALIDATION_RESPONSE_t * validationInfo = msgPtr;
				sprintf(msgBuf, "%d%c%d%c%c", msgId, UI_MSG_FSP, validationInfo -> respMsgIdx, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_SYSTEM_TEST_POPUP_SCREEN:
			{
				SYSTEM_TEST_INFO_t	*sysTestInfo = msgPtr;

				sprintf(msgBuf, "%d%c%s%c%s%c%s%c%s%c%c", msgId, UI_MSG_FSP, sysTestInfo->titleText, UI_MSG_FSP, sysTestInfo->message1,
											UI_MSG_FSP, sysTestInfo->message2, UI_MSG_FSP, sysTestInfo->message3, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_LCD_TEST_SCREEN:
			{
				sprintf(msgBuf, "%d%c%d%c%c",msgId, UI_MSG_FSP, SYS_LCD_TEST_TIMER, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_WPS_PROGRESS_SCREEN:
			{
				WPS_PROGRESS_t *wpsDataPtr = msgPtr;

				sprintf(msgBuf, "%d%c%d%c%s%c%c",msgId, UI_MSG_FSP, wpsDataPtr->wpsTrigerTyp, UI_MSG_FSP,
								wpsDataPtr->wpsPin, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_JOB_CODE_DISPLAY_SCREEN:
			case GUI_MSG_JOB_CODE_LIST:
			{
				UINT8				listIdx;
				UINT16				len;
				JOB_CODE_MSG_QT_t	*jobCodeMsg = (JOB_CODE_MSG_QT_t *)msgPtr;

				sprintf(msgBuf, "%d%c", msgId, UI_MSG_FSP);
				len = strlen(msgBuf);
				sprintf(&msgBuf[len], "%d%c%d%c%d%c%d%c%d%c%d%c", jobCodeMsg->pageBtnInfo.currFirstSubPageNo, UI_MSG_FSP,
																jobCodeMsg->pageBtnInfo.currSubPageCnt, UI_MSG_FSP,
																jobCodeMsg->pageBtnInfo.prevFirstSubPageNo, UI_MSG_FSP,
																jobCodeMsg->pageBtnInfo.prevSubPageCnt, UI_MSG_FSP,
																jobCodeMsg->pageBtnInfo.nextFirstSubPageNo, UI_MSG_FSP,
																jobCodeMsg->pageBtnInfo.nextSubPageCnt, UI_MSG_FSP);
				len = strlen(msgBuf);
				sprintf(&msgBuf[len], "%d%c", jobCodeMsg->jobCodeCnt, UI_MSG_FSP);//Job Code count
				len = strlen(msgBuf);
				for(listIdx = 0; listIdx < jobCodeMsg->jobCodeCnt; listIdx++)
				{
					sprintf(&msgBuf[len], "%s%c", &(jobCodeMsg->jobCode[listIdx][0]), UI_MSG_FSP);
					len = strlen(msgBuf);
				}
				sprintf(&msgBuf[len], "%c", EOT_FSP);
				break;
			}

			case GUI_MSG_INOUT_SPCL_FUNCTION_SELECTION:
			{
				IN_OUT_SP_FN_MSG_QT_t	*inOutSpMsg  = msgPtr;
				UINT8				listIdx;
				UINT16				len;

				sprintf(msgBuf, "%d%c", msgId, UI_MSG_FSP);
				len = strlen(msgBuf);
				for(listIdx = 0; listIdx < inOutSpMsg->spFnCnt; listIdx++)
				{
					sprintf(&msgBuf[len], "%d%c",inOutSpMsg->enableDisabledSpecionFunctionList[listIdx], UI_MSG_FSP);
					len = strlen(msgBuf);
				}
				sprintf(&msgBuf[len], "%c", EOT_FSP);
				break;
			}
			case GUI_MSG_SHOW_USER_ATTENDANCE_DETAIL:
			{
				USER_ATNDNC_DISPLAY_DATA_t	*singleUserAttndnsDisplayData = (USER_ATNDNC_DISPLAY_DATA_t *)msgPtr;
				CHAR						dayStatusStr[65], workHrs[7] = "", overtime[7] = "", lateIn[7] = "", earlyOut[7] = "";
				CHAR						leaveCodeStr[120] = "", leaveTypeStr[150] = "", leaveBalanceStr[260] = "";
				CHAR						absentDateStr[1024] = "", firstHalfStr[190] = "", secondHalfStr[190] = "";
				UINT8						date, idx;
				UINT16						strIdx = 0, firstHalfIdx, secondHalfIdx, ret;
				UINT32						absentYear;
				struct tm 					*lastUpdated;
				CHARPTR monthStr[12] = 		{"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

				SingleUserLeaveCodeCountSorting(&(singleUserAttndnsDisplayData->singleUserLeaveDetail));

				strIdx = 0;
				for(idx = 0; idx < MAX_LEAVE_CODES; idx++)
				{
					if(strlen(singleUserAttndnsDisplayData -> singleUserLeaveDetail.leaveCode[idx]) != 0)
					{
						strIdx += sprintf(&leaveCodeStr[strIdx], "%s,", singleUserAttndnsDisplayData -> singleUserLeaveDetail.leaveCode[idx]);
					}
				}
				leaveCodeStr[strIdx - 1] = '\0';

				strIdx = 0;
				for(idx = 0; idx < MAX_LEAVE_CODES; idx++)
				{
					if(strlen(singleUserAttndnsDisplayData -> singleUserLeaveDetail.leaveCode[idx]) != 0)
					{
						strIdx += sprintf(&leaveBalanceStr[strIdx], "%s,", singleUserAttndnsDisplayData -> singleUserLeaveDetail.leaveBalance[idx]);
					}
				}
				leaveBalanceStr[strIdx - 1] = '\0';

				strIdx = 0;
				for(idx = 0; idx < MAX_LEAVE_CODES; idx++)
				{
					if(strlen(singleUserAttndnsDisplayData -> singleUserLeaveDetail.leaveCode[idx]) != 0)
					{
						strIdx += sprintf(&leaveTypeStr[strIdx], "%u,", singleUserAttndnsDisplayData -> singleUserLeaveDetail.leaveType[idx]);
					}
				}
				leaveTypeStr[strIdx - 1] = '\0';

				strIdx = 0;
				for(date = 0; date < MAX_DAYS_IN_MONTH; date++)
				{
					UINT8 dayStatus = singleUserAttndnsDisplayData->singleUserAttendanceDetail.dayStatus[date];
					if( (dayStatus == 22) || (dayStatus == 21) || (dayStatus == 20) )
					{
						dayStatus = 2;
					}
					else if(dayStatus > 4)
					{
						dayStatus = 0;
					}

					strIdx += sprintf(&dayStatusStr[strIdx],"%u,", dayStatus);
				}
				dayStatusStr[strIdx - 1] = '\0';

				strIdx = 0;
				firstHalfIdx = 0;
				secondHalfIdx = 0;
				for(date = 0; date < MAX_DAYS_IN_MONTH; date++)
				{
					UINT8 dayStatus = singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.dayStatus[date];

					if(CurrTimeDate.tm_mon > 0)
					{
						absentYear = (CurrTimeDate.tm_year+LINUX_YEAR_OFFSET_1900);
						idx = CurrTimeDate.tm_mon - 1;
					}
					else
					{
						absentYear = (CurrTimeDate.tm_year+LINUX_YEAR_OFFSET_1900 - 1);
						idx = 11;
					}

					if(dayStatus == 22)
					{
						strIdx += sprintf(&absentDateStr[strIdx], "%u %s, %u.",(date+1), monthStr[singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.processedMonth], singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.processedYear);
						firstHalfIdx += sprintf(&firstHalfStr[firstHalfIdx], "%s,", "PR");
						secondHalfIdx += sprintf(&secondHalfStr[secondHalfIdx], "%s,", "AB");
					}
					if(dayStatus == 21)
					{
						strIdx += sprintf(&absentDateStr[strIdx], "%u %s, %u.",(date+1), monthStr[singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.processedMonth], singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.processedYear);
						firstHalfIdx += sprintf(&firstHalfStr[firstHalfIdx], "%s,", "AB");
						secondHalfIdx += sprintf(&secondHalfStr[secondHalfIdx], "%s,", "PR");
					}
					if(dayStatus == 20)
					{
						strIdx += sprintf(&absentDateStr[strIdx], "%u %s, %u.",(date+1), monthStr[singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.processedMonth], singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.processedYear);
						firstHalfIdx += sprintf(&firstHalfStr[firstHalfIdx], "%s,", "AB");
						secondHalfIdx += sprintf(&secondHalfStr[secondHalfIdx], "%s,", "AB");
					}
				}
				if( (singleUserAttndnsDisplayData->singleUserAttendanceDetail.processedMonth != singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.processedMonth) ||
					(singleUserAttndnsDisplayData->singleUserAttendanceDetail.processedYear != singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.processedYear) )
				{
					for(date = 0; date < MAX_DAYS_IN_MONTH; date++)
					{
						UINT8 dayStatus = singleUserAttndnsDisplayData->singleUserAttendanceDetail.dayStatus[date];

						idx = CurrTimeDate.tm_mon;
						absentYear = (CurrTimeDate.tm_year+LINUX_YEAR_OFFSET_1900);

						if(dayStatus == 22)
						{
							strIdx += sprintf(&absentDateStr[strIdx], "%u %s, %u.",(date+1), monthStr[idx], absentYear);
							firstHalfIdx += sprintf(&firstHalfStr[firstHalfIdx], "%s,", "PR");
							secondHalfIdx += sprintf(&secondHalfStr[secondHalfIdx], "%s,", "AB");
						}
						if(dayStatus == 21)
						{
							strIdx += sprintf(&absentDateStr[strIdx], "%u %s, %u.",(date+1), monthStr[idx], absentYear);
							firstHalfIdx += sprintf(&firstHalfStr[firstHalfIdx], "%s,", "AB");
							secondHalfIdx += sprintf(&secondHalfStr[secondHalfIdx], "%s,", "PR");
						}
						if(dayStatus == 20)
						{
							strIdx += sprintf(&absentDateStr[strIdx], "%u %s, %u.",(date+1), monthStr[idx], absentYear);
							firstHalfIdx += sprintf(&firstHalfStr[firstHalfIdx], "%s,", "AB");
							secondHalfIdx += sprintf(&secondHalfStr[secondHalfIdx], "%s,", "AB");
						}
					}
				}
				absentDateStr[strIdx - 1] = '\0';
				firstHalfStr[firstHalfIdx - 1] = '\0';
				secondHalfStr[secondHalfIdx - 1] = '\0';

				lastUpdated = gmtime(&singleUserAttndnsDisplayData->singleUserAttendanceDetail.lastUpdatedTimeDate);

				snprintf(workHrs, sizeof(workHrs), "%03d:%02d",(singleUserAttndnsDisplayData->singleUserAttendanceDetail.workHrs / 60),
											(singleUserAttndnsDisplayData->singleUserAttendanceDetail.workHrs % 60));
				snprintf(overtime, sizeof(overtime), "%03d:%02d",(singleUserAttndnsDisplayData->singleUserAttendanceDetail.overtime / 60),
											(singleUserAttndnsDisplayData->singleUserAttendanceDetail.overtime % 60));

				if(singleUserAttndnsDisplayData->singleUserAttendanceDetail.separatLateInEarlyOutF == TRUE) // separate
				{
					if(singleUserAttndnsDisplayData->singleUserAttendanceDetail.lateInEarlyOutCountFormatF == TRUE)
					{
						snprintf(lateIn, sizeof(lateIn), "%d",singleUserAttndnsDisplayData->singleUserAttendanceDetail.lateIn);
						snprintf(earlyOut, sizeof(earlyOut), "%d",singleUserAttndnsDisplayData->singleUserAttendanceDetail.earlyOut);
					}
					else
					{
						snprintf(lateIn, sizeof(lateIn), "%03d:%02d",(singleUserAttndnsDisplayData->singleUserAttendanceDetail.lateIn / 60),
												(singleUserAttndnsDisplayData->singleUserAttendanceDetail.lateIn % 60));
						snprintf(earlyOut, sizeof(earlyOut), "%03d:%02d",(singleUserAttndnsDisplayData->singleUserAttendanceDetail.earlyOut / 60),
												(singleUserAttndnsDisplayData->singleUserAttendanceDetail.earlyOut % 60));
					}
				}
				else // combine fields
				{
					if(singleUserAttndnsDisplayData->singleUserAttendanceDetail.lateInEarlyOutCountFormatF == 1)
					{
						snprintf(lateIn, sizeof(lateIn), "%d",singleUserAttndnsDisplayData->singleUserAttendanceDetail.lateIn);
					}
					else
					{
						snprintf(lateIn, sizeof(lateIn), "%03d:%02d",(singleUserAttndnsDisplayData->singleUserAttendanceDetail.lateIn / 60),
												(singleUserAttndnsDisplayData->singleUserAttendanceDetail.lateIn % 60));
					}
				}

				ret = sprintf(msgBuf, "%d%c%d%c%u%c%s%c%s%c%d%c%d%c%s%c%d%c%d%c%d%c%d%c%d%c%d%c%s%c%s%c%s%c%s%c", msgId, UI_MSG_FSP,
									singleUserAttndnsDisplayData->userIdx, UI_MSG_FSP,
									singleUserAttndnsDisplayData->userId, UI_MSG_FSP,
									singleUserAttndnsDisplayData->name, UI_MSG_FSP,
									singleUserAttndnsDisplayData->alphaNumId, UI_MSG_FSP,
									singleUserAttndnsDisplayData->singleUserAttendanceDetail.processedMonth, UI_MSG_FSP,
									singleUserAttndnsDisplayData->singleUserAttendanceDetail.processedYear, UI_MSG_FSP,
									dayStatusStr, UI_MSG_FSP,
									lastUpdated -> tm_mday,  UI_MSG_FSP,
									lastUpdated -> tm_mon,  UI_MSG_FSP,
									(lastUpdated -> tm_year + LINUX_YEAR_OFFSET_1900), UI_MSG_FSP,
									lastUpdated -> tm_hour,  UI_MSG_FSP,
									lastUpdated -> tm_min, UI_MSG_FSP,
									singleUserAttndnsDisplayData->singleUserAttendanceDetail.separatLateInEarlyOutF, UI_MSG_FSP,
									workHrs,UI_MSG_FSP,
									overtime,UI_MSG_FSP,
									lateIn,UI_MSG_FSP,
									earlyOut,UI_MSG_FSP);

				strIdx = 0;
				for(date = 0; date < MAX_DAYS_IN_MONTH; date++)
				{
					UINT8 dayStatus = singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.dayStatus[date];
					if( (dayStatus == 22) || (dayStatus == 21) || (dayStatus == 20) )
					{
						dayStatus = 2;
					}
					else if(dayStatus > 4)
					{
						dayStatus = 0;
					}
					strIdx += sprintf(&dayStatusStr[strIdx],"%u,", dayStatus);
				}
				dayStatusStr[strIdx - 1] = '\0';

				lastUpdated = gmtime(&singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.lastUpdatedTimeDate);

				snprintf(workHrs, sizeof(workHrs), "%03d:%02d",(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.workHrs / 60),
											(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.workHrs % 60));
				snprintf(overtime, sizeof(overtime), "%03d:%02d",(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.overtime / 60),
											(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.overtime % 60));

				if(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.separatLateInEarlyOutF == TRUE) // separate
				{
					if(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.lateInEarlyOutCountFormatF == TRUE)
					{
						snprintf(lateIn, sizeof(lateIn), "%d",singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.lateIn);
						snprintf(earlyOut, sizeof(earlyOut), "%d",singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.earlyOut);
					}
					else
					{
						snprintf(lateIn, sizeof(lateIn), "%03d:%02d",(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.lateIn / 60),
												(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.lateIn % 60));
						snprintf(earlyOut, sizeof(earlyOut), "%03d:%02d",(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.earlyOut / 60),
												(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.earlyOut % 60));
					}
				}
				else // combine fields
				{
					if(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.lateInEarlyOutCountFormatF == 1)
					{
						snprintf(lateIn, sizeof(lateIn), "%d",singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.lateIn);
					}
					else
					{
						snprintf(lateIn, sizeof(lateIn), "%03d:%02d",(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.lateIn / 60),
												(singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.lateIn % 60));
					}
				}

				sprintf(msgBuf+ret,	"%d%c%d%c%s%c%d%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%c",
									singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.processedMonth, UI_MSG_FSP,
									singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.processedYear, UI_MSG_FSP,
									dayStatusStr, UI_MSG_FSP,
									singleUserAttndnsDisplayData->singleUserAttendanceDetailPrevMonth.separatLateInEarlyOutF, UI_MSG_FSP,
									workHrs,UI_MSG_FSP,
									overtime,UI_MSG_FSP,
									lateIn,UI_MSG_FSP,
									earlyOut,UI_MSG_FSP,
									leaveCodeStr,UI_MSG_FSP,
									leaveBalanceStr,UI_MSG_FSP,
									leaveTypeStr,UI_MSG_FSP,
									absentDateStr,UI_MSG_FSP,
									firstHalfStr,UI_MSG_FSP,
									secondHalfStr,UI_MSG_FSP,
									EOT_FSP);

				DPRINT(DC_LOG, "leaveCodeStr = %s , leaveBalanceStr = %s, leaveTypeStr = %s\n ",leaveCodeStr,leaveBalanceStr,leaveTypeStr);

				break;
			}

			case GUI_MSG_APPLY_LEAVE_RESPONSE:
			{
				CHARPTR	response  = (CHARPTR)msgPtr;
				UINT8 	aplyLeavRspns;

				aplyLeavRspns = response[0] - '0';
				sprintf(msgBuf,"%d%c%d%c%s%c%c", msgId, UI_MSG_FSP, aplyLeavRspns, UI_MSG_FSP , &response[1], UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_FR_BUSY_SIGNAL:
			{
				UINT8 *Flag=msgPtr;
				sprintf(msgBuf, "%d%c%d%c%s%c%c",msgId,UI_MSG_FSP,*Flag,UI_MSG_FSP,GetDisplayMessage(FALSE,NULL_CHAR,MSG_DEVICE_IS_BUSY_977),UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_ADAPTIVE_FR_SCREEN:
			{
				ADAPTIVE_FR_GUI_QUERY_t *frQuery = (ADAPTIVE_FR_GUI_QUERY_t *)msgPtr;
				sprintf(msgBuf, "%d%c%d%c%s%c%s%c%c", msgId, UI_MSG_FSP, frQuery->adaptiveFrTime, UI_MSG_FSP,
						frQuery->usrId, UI_MSG_FSP, frQuery->usrName, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_APPL_MODE_INFO:
			{
				UINT8 *mode = msgPtr;
				sprintf(msgBuf, "%d%c%d%c%c", msgId, UI_MSG_FSP, *mode, UI_MSG_FSP, EOT_FSP);
				break;
			}
			case GUI_MSG_DYNAMIC_TEMPERATURE_SCREEN:
			{
				DYNAMIC_TEMP_SCREEN_INFO_QT_t * DynamicTempScreenInfo = msgPtr;

				sprintf(msgBuf, "%d%c%s%c%d%c%d%c%d%c%s%c%d%c%s%c%c",
					msgId,UI_MSG_FSP, DynamicTempScreenInfo -> titleText, UI_MSG_FSP, DynamicTempScreenInfo -> imageIdx, UI_MSG_FSP,
					DynamicTempScreenInfo -> footerHideF, UI_MSG_FSP, DynamicTempScreenInfo -> ImageHideF, UI_MSG_FSP,
					DynamicTempScreenInfo->user.alphaNumId,UI_MSG_FSP,DynamicTempScreenInfo->user.userId,UI_MSG_FSP,
					DynamicTempScreenInfo->user.name,UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_TEMPERATURE_MENU_SCREEN:
			{
				CHAR	lastUpdatedDateTimeStr[50] = {0};
				CHAR	ambientTemperature[10] = {0};
				CHAR	currentTemperature[10] = {0};
				CHAR	tempUnit = 'F';

				/* Update temperature unit */
				if (SysAdvPara.temperatureLogCnfg.tempUnit == UNIT_CELCIUS)
				{
					tempUnit = 'C';
				}

				if (tempSensorInfo.currentTemperature != 0.0)
				{
					snprintf(currentTemperature, sizeof(currentTemperature), "%.1f°%c",tempSensorInfo.currentTemperature, tempUnit);
				}

				if (tempSensorInfo.ambientTemperature != 0.0)
				{
					snprintf(ambientTemperature, sizeof(ambientTemperature), "%.1f°%c",tempSensorInfo.ambientTemperature, tempUnit);
				}

				snprintf(lastUpdatedDateTimeStr, sizeof(lastUpdatedDateTimeStr), "(%.2d/%.2d/%.2d) (%.2d:%.2d:%.2d)",
						tempSensorInfo.currTimeDate.tm_mday, tempSensorInfo.currTimeDate.tm_mon, tempSensorInfo.currTimeDate.tm_year,
						tempSensorInfo.currTimeDate.tm_hour, tempSensorInfo.currTimeDate.tm_min, tempSensorInfo.currTimeDate.tm_sec);

				snprintf(msgBuf, sizeof(msgBuf),"%d%c%s%c%s%c%s%c%c", msgId, UI_MSG_FSP, currentTemperature, UI_MSG_FSP ,
						ambientTemperature, UI_MSG_FSP, lastUpdatedDateTimeStr, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_DISPLAY_IDLE_SCREEN_FOR_FR_STREAMING:
			{
				DYNAMIC_IDLE_SCREEN_INFO_QT_t	qtDynamicIdleScreenInfo = *(DYNAMIC_IDLE_SCREEN_INFO_QT_t *)msgPtr;

				snprintf(msgBuf, sizeof(msgBuf),"%d%c%d%c%d%c%s%c%d%c%d%c%s%c%d%c%d%c%c", msgId, UI_MSG_FSP, qtDynamicIdleScreenInfo.showOverlay, UI_MSG_FSP,
						qtDynamicIdleScreenInfo.showOverlayText, UI_MSG_FSP, qtDynamicIdleScreenInfo.overlayText, UI_MSG_FSP,
						qtDynamicIdleScreenInfo.showUserInfo, UI_MSG_FSP, qtDynamicIdleScreenInfo.userId, UI_MSG_FSP,
						qtDynamicIdleScreenInfo.name, UI_MSG_FSP, qtDynamicIdleScreenInfo.footerIdx, UI_MSG_FSP,qtDynamicIdleScreenInfo.doNotApplyAutoHideF, UI_MSG_FSP, EOT_FSP);
				break;
			}

#if defined(FR_ADDITIONAL_FEATURE_SUPPORTED)
			case GUI_MSG_DYNAMIC_USER_INFO_SCREEN:
			{
				DYNAMIC_USER_INFO_SCREEN_QT_t	*dynamicUserInfo = msgPtr;

				snprintf(msgBuf, sizeof(msgBuf), "%d%c%s%c%d%c%s%c%d%c%c",
								msgId, UI_MSG_FSP,
								dynamicUserInfo->user.alphaNumId, UI_MSG_FSP,
								dynamicUserInfo->user.userId, UI_MSG_FSP,
								dynamicUserInfo->user.name, UI_MSG_FSP,
								dynamicUserInfo->imageIdx, UI_MSG_FSP, EOT_FSP);
				break;
			}
#endif
			case GUI_MSG_SET_OVERLAY_TEXT_ON_STREAM:
			{
				FR_OVERLAY_STREAM_INFO_t *frOverlayInfo = msgPtr;

				snprintf(msgBuf, sizeof(msgBuf), "%d%c%d%c%d%c%s%c%c", msgId, UI_MSG_FSP,
								frOverlayInfo->showOverlay, UI_MSG_FSP,
								frOverlayInfo->showOverlayText, UI_MSG_FSP,
								GetDisplayMessage(FALSE, NULL_CHAR, frOverlayInfo->overlayMultilangTextIdx), UI_MSG_FSP, EOT_FSP);
				break;								
			}

        case GUI_MSG_LOG_INFO:
        {
            QT_LOG_INFO_t *tGuiLogInfo = (QT_LOG_INFO_t *)msgPtr;
            snprintf(msgBuf, sizeof(msgBuf),"%d%c%d%c%u%c%s%c%u%c%s%c%d%c%c",
                     msgId, UI_MSG_FSP,tGuiLogInfo->guiLogSeverity, UI_MSG_FSP,
                     tGuiLogInfo->sysLogEnableF, UI_MSG_FSP,
                     tGuiLogInfo->sysLogServerAddr, UI_MSG_FSP,
                     tGuiLogInfo->sysLogServerPort, UI_MSG_FSP,
                     tGuiLogInfo->deviceMacAddr, UI_MSG_FSP,
                     GetDiffBetGmtAndLocalTime(), UI_MSG_FSP,
                     EOT_FSP);
            break;
        }

        case GUI_MSG_DEVICE_INACTIVE:
        {
            snprintf(msgBuf, QT_RX_BUF_SIZE, "%d%c%d%c%d%c%s%c%c", msgId, UI_MSG_FSP, getDeviceInactiveStatus(), UI_MSG_FSP, SysBasicPara.hideMenuBarForAccessScheduleF, UI_MSG_FSP,GetDisplayMessage(FALSE,NULL_CHAR, MSG_RESTRICTED_ACCESS_HOURS_1097),UI_MSG_FSP, EOT_FSP);
            break;
        }

		}
	}
	else// if(GetDoorApplType() == PANEL_DOOR_APPL)
	{
		switch(msgId)
		{
			default:
				EPRINT(GUI_LOG, "SendMessageToQt(), Message is Invalid, Message id = 0X%X", msgId);
				return FALSE;
				break;

			case GUI_MSG_SEND_CONFIG_INFO:
			{
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%u%c%c",msgId,UI_MSG_FSP,SysBasicPara.timeFormat, UI_MSG_FSP,
						GetDiffBetGmtAndLocalTime(), UI_MSG_FSP, DispDoorModeF, UI_MSG_FSP, DoorEntryExitMode, UI_MSG_FSP,
						DfltBasicSysPara.entryModeWallpaperF, UI_MSG_FSP, DfltBasicSysPara.exitModeWallpaperF, UI_MSG_FSP,
						SysAdvPara.autoHideMenuBarF, UI_MSG_FSP, EOT_FSP);
				break;
			}
			case GUI_MSG_SET_MMC_DEV_PATH:
			{
				MEDIA_FILE_PATH_t filePath;
				if(strlen(MmcInfo.devPath))
				{
					sprintf(filePath.userPicsFile, "%s/%s/%s", MmcInfo.devPath, USER_PICS_FOLDER, USER_PICS_PREFIX);
				}
				else
				{
					sprintf(filePath.userPicsFile, "%s", "");
				}
				sprintf(msgBuf, "%d%c%s%c%c", msgId, UI_MSG_FSP, filePath.userPicsFile, UI_MSG_FSP, EOT_FSP);
				break;
			}
			case GUI_MSG_START_GUI_APPLICATION:
			case GUI_MSG_MENU_LOGIN_SCREEN:
			case GUI_MSG_DISPLAY_IDLE_SCREEN:
			case GUI_MSG_DISPLAY_MENU_SCREEN:
			case GUI_MSG_BACKGROUND_SELECTION_SCREEN:
			case GUI_MSG_CLOCK_SELECTION_SCREEN:
			case GUI_MSG_KEYPAD_SELECTION_SCREEN:
			case GUI_MSG_SELF_ENROLLMENT_SCREEN:
			case GUI_MSG_DOOR_MODE_SELECTION_SCREEN:
			case GUI_MSG_ENTER_USER_NAME:
			case GUI_MSG_TEMP_USER_CREATE_CONFIRM_SCREEN:
			case GUI_MSG_TEMP_USER_ENTER_NAME_SCREEN:
			{
				sprintf(msgBuf, "%d%c%c",msgId,UI_MSG_FSP,EOT_FSP);	//Only Message Id Is Required
				break;
			}

			case GUI_MSG_TEMP_USER_POPUP_SCREEN:
			{
				TEMP_USER_POPUP_INFO_QT_t * popUpInfo = msgPtr;

				sprintf(msgBuf, "%d%c%s%c%d%c%s%c%s%c%d%c%c",
								msgId,UI_MSG_FSP,popUpInfo -> titleText, UI_MSG_FSP, popUpInfo -> imageIdx, UI_MSG_FSP,
								popUpInfo -> message1, UI_MSG_FSP, popUpInfo -> message2, UI_MSG_FSP, popUpInfo -> footerHideF, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_TEMP_USER_REGISTERED_SUCCESSFULLY:
			{
				TEMP_USER_COMMON_QT_t * tempUser = msgPtr;
				sprintf(msgBuf,"%d%c%s%c%s%c%u%c%c",
						msgId, UI_MSG_FSP, tempUser -> user.alphaNumId,UI_MSG_FSP,
								   tempUser -> user.name, UI_MSG_FSP,
								   tempUser -> user.userId, UI_MSG_FSP,
						EOT_FSP);
				break;
			}

			case GUI_MSG_BRIGHTNESS_SELECTION_SCREEN:
			{
				sprintf(msgBuf, "%d%c%d%c%d%c%c",msgId,UI_MSG_FSP,
										DisplayBrightness.sleepTimer, UI_MSG_FSP,
										DisplayBrightness.setBrightness, UI_MSG_FSP,
												EOT_FSP);
				break;
			}

			case GUI_MSG_PIN_ENTER_SCREEN:
			{
				SysAdvPara.showPin = 0 ;    //fpr panel door it will be disabled
				sprintf(msgBuf, "%d%c%d%c%c",msgId,UI_MSG_FSP,SysAdvPara.showPin, UI_MSG_FSP,EOT_FSP);	//Only Message Id Is Required
				break;
			}

			case GUI_MSG_ADMIN_SETTING_SCREEN:
			{
				sprintf(msgBuf, "%d%c%s%c",msgId,UI_MSG_FSP, (CHARPTR) msgPtr,EOT_FSP);
				break;
			}

			case GUI_MSG_STARTUP_INFO:
			{
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%u%c%d%c%c",msgId,UI_MSG_FSP, SysBasicPara.timeFormat, UI_MSG_FSP,
				SysVar.backgroundImage, UI_MSG_FSP, SysVar.currentClock , UI_MSG_FSP, 0, UI_MSG_FSP,
				SysVar.currentKeypad, UI_MSG_FSP, SysBasicPara.application, UI_MSG_FSP, DOOR_TYPE_PD, UI_MSG_FSP,
				MultiLangConfig.multiLangEnblF, UI_MSG_FSP, SysAdvPara.autoHideMenuBarF,UI_MSG_FSP,IsVariant_With_Internal_FP_Rdr(),UI_MSG_FSP,EOT_FSP);
				break;
			}
			case GUI_MSG_MULTILANG_FLAG:
			{
				sprintf(msgBuf, "%d%c%d%c%c",msgId,UI_MSG_FSP,MultiLangConfig.multiLangEnblF, UI_MSG_FSP,EOT_FSP);
				break;
			}
			case GUI_MSG_MILTILANG_FONT_LOAD:
			{
				sprintf(msgBuf, "%d%c%s%c%c",msgId,UI_MSG_FSP, FONTFILE_PROCESS_FILE_PATH, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_GOING_IN_SCREEN:
			{
				QT_TA_SCREEN_t * taDataPtr = msgPtr;
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%c", msgId,UI_MSG_FSP, taDataPtr->regularF, UI_MSG_FSP,
						taDataPtr->officialF, UI_MSG_FSP,
						taDataPtr->breakF, UI_MSG_FSP,
						taDataPtr->shortLeaveF, UI_MSG_FSP,
						taDataPtr->overTimeF, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_GOING_OUT_SCREEN:
			{
				QT_TA_SCREEN_t * taDataPtr = msgPtr;
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%c", msgId, UI_MSG_FSP, taDataPtr->regularF, UI_MSG_FSP,
						taDataPtr->officialF, UI_MSG_FSP,
						taDataPtr->breakF, UI_MSG_FSP,
						taDataPtr->shortLeaveF, UI_MSG_FSP,
						taDataPtr->overTimeF, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_MENU_SCREEN:
			{
				sprintf(msgBuf, "%d%c%d%c%c", msgId , UI_MSG_FSP, SysBasicPara.application,
						UI_MSG_FSP , EOT_FSP);
				break;
			}

			case GUI_MSG_HRD_MENU_SCREEN:
			case GUI_MSG_ADMIN_MENU_SCREEN:
			{
				QT_ADMIN_HRD_SCREEN_t	* adminHrdDataPtr = msgPtr;

				if(CommState == SLAVE_ONLINE)
				{
					sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%c",msgId,UI_MSG_FSP,
							adminHrdDataPtr->multiUser.lateInEnable, UI_MSG_FSP,
							adminHrdDataPtr->multiUser.lateInStartStop, UI_MSG_FSP,
							adminHrdDataPtr->multiUser.earlyOutEnable, UI_MSG_FSP,
							adminHrdDataPtr->multiUser.earlyOutStartStop, UI_MSG_FSP,
							adminHrdDataPtr->userProfileF, UI_MSG_FSP, EOT_FSP);
				}
				else
				{
					sprintf(msgBuf, "%d%c%c",msgId,UI_MSG_FSP,EOT_FSP);
				}
				break;
			}

			case GUI_MSG_STATUSBAR_SCREEN:
			{
				STATUS_BAR_INFO_t	* statusBarDataPtr = msgPtr;

				CHAR TemperatureStr[10] ="";
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%s%c%d%c%d%c%c", msgId,UI_MSG_FSP,
						statusBarDataPtr->ethStatus, UI_MSG_FSP,
						statusBarDataPtr->wifiStatus, UI_MSG_FSP,
						statusBarDataPtr->nwFaultF, UI_MSG_FSP,
						statusBarDataPtr->degradeModeF, UI_MSG_FSP,
						statusBarDataPtr->doorLockStatus, UI_MSG_FSP,
						statusBarDataPtr->alarmStatus, UI_MSG_FSP,
						statusBarDataPtr->lateInEarlyOutStatus, UI_MSG_FSP,
						statusBarDataPtr->dndF, UI_MSG_FSP,
						statusBarDataPtr->deadManZoneF, UI_MSG_FSP,
						statusBarDataPtr->downloadingF, UI_MSG_FSP,
						statusBarDataPtr->bluetoothStatus,UI_MSG_FSP,
						statusBarDataPtr->temperatureSensorStatusF, UI_MSG_FSP,
						TemperatureStr, UI_MSG_FSP,
                        IsTempratureConfigApplicable(), UI_MSG_FSP,
						statusBarDataPtr->degradeModeType, UI_MSG_FSP,
						EOT_FSP);

				break;
			}

			case GUI_MSG_ZONE_CFG_SCREEN:
			{
				QT_ZONE_SETTINGS_t * zoneCfg = msgPtr;

				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%c",msgId,UI_MSG_FSP,
						zoneCfg -> dndEnableF, UI_MSG_FSP,
						zoneCfg -> dndStatusF, UI_MSG_FSP,
						zoneCfg -> dmzEnableF, UI_MSG_FSP,
						zoneCfg -> dmzStatusF ,UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_ADMIN_DOOR_CONTROL_SCREEN:
			case GUI_MSG_ZONE_CONTROL_SCREEN:
			{
				QT_ZONE_MENU_t * zoneMenu = msgPtr;

				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%c",msgId,UI_MSG_FSP,
						zoneMenu -> lockF, UI_MSG_FSP,
						zoneMenu -> unlockF, UI_MSG_FSP,
						zoneMenu -> normalF ,UI_MSG_FSP,
						zoneMenu -> armF ,UI_MSG_FSP,
						zoneMenu -> disarmF ,UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_ALARM_OPTIONS_SCREEN:
			{
				QT_ALARM_SETTINGS_t * alarmOptions = msgPtr;

				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%c",msgId,UI_MSG_FSP,
						alarmOptions -> paincAlarmF, UI_MSG_FSP,
						alarmOptions -> muteDoorBuzzerF, UI_MSG_FSP,
						alarmOptions -> clrDoorAuxOpF, UI_MSG_FSP,
						alarmOptions -> muteMasterBuzzerF, UI_MSG_FSP,
						alarmOptions -> clrMasterOpF, UI_MSG_FSP,
						alarmOptions -> clrDCAlarmOpF, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_NOTICE_TEXT:	//Any single interactive message text e.g TIMEOUT
			{
				MSG_GEN_INFO_QT_t * info = msgPtr;

				sprintf(msgBuf,"%d%c%s%c%s%c%d%c%d%c%d%c%c",msgId, UI_MSG_FSP, info -> line2,
								UI_MSG_FSP, info -> line3, UI_MSG_FSP, info -> imageIdx, UI_MSG_FSP,
								info -> ifPinEntryBtn, UI_MSG_FSP, info -> ifEscBtn , UI_MSG_FSP , EOT_FSP);
				break;
			}

			case GUI_MSG_SPL_FUNC_STATUS:		//Screen to be shown with id,name,photo
												//and submit when enrolling new user
			{
				FUNC_STATUS_QT_t * funcInfo = msgPtr;

				sprintf(msgBuf, "%d%c%s%c%d%c%s%c%s%c%d%c%s%c%s%c%d%c%d%c%d%c%u%c%s%c%d%c%d%c%d%c%c",
								msgId,UI_MSG_FSP,funcInfo -> splFunc, UI_MSG_FSP, funcInfo -> ifUserInfo, UI_MSG_FSP,
								funcInfo -> user.alphaNumId, UI_MSG_FSP, funcInfo -> user.name, UI_MSG_FSP,
								funcInfo -> user.photo, UI_MSG_FSP, funcInfo-> genInfo.line1,
								UI_MSG_FSP, funcInfo -> genInfo.line2, UI_MSG_FSP,
								funcInfo -> genInfo.imageIdx,
								UI_MSG_FSP, funcInfo -> genInfo.ifPinEntryBtn, UI_MSG_FSP,
								funcInfo -> genInfo.ifEscBtn, UI_MSG_FSP, funcInfo -> user.userId, UI_MSG_FSP,
								funcInfo -> temperature, UI_MSG_FSP, funcInfo -> temperatureUnit, UI_MSG_FSP, 
								funcInfo -> temperatureViolation, UI_MSG_FSP, funcInfo -> faceMaskDecision, UI_MSG_FSP,
								EOT_FSP);
				break;
			}

			case GUI_MSG_DYNAMIC_POP_UP_SCREEN:
			{
				DYNAMIC_POPUP_INFO_QT_t * popUpInfo = msgPtr;
				SetPopupImagesOnSlaveDegraed(&(popUpInfo -> imageIdx));
				sprintf(msgBuf, "%d%c%s%c%d%c%s%c%s%c%d%c%c",
								msgId,UI_MSG_FSP,popUpInfo -> titleText, UI_MSG_FSP, popUpInfo -> imageIdx, UI_MSG_FSP,
								popUpInfo -> message1, UI_MSG_FSP, popUpInfo -> message2, UI_MSG_FSP, popUpInfo -> footerHideF, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_SPL_FUNC_DYNAMIC_SCREEN:
			{
				DYNAMIC_SPL_FUNC_INFO_t * msgInfo = msgPtr;

				sprintf(msgBuf,"%d%c%s%c%d%c%s%c%d%c%d%c%c", msgId , UI_MSG_FSP , msgInfo->splFunc , UI_MSG_FSP ,
								msgInfo->splFuncImageIdx , UI_MSG_FSP , msgInfo->noticeLine , UI_MSG_FSP ,
								msgInfo->ifEscBtn , UI_MSG_FSP , msgInfo->ifPinEntryBtn ,UI_MSG_FSP , EOT_FSP);
				break;

			}

			case GUI_MSG_USER_DENIED:	  				//Display User Denied Screen
			{
				USER_DENIED_QT_t * deny = msgPtr;

				sprintf(msgBuf,"%d%c%d%c%s%c%s%c%d%c%s%c%s%c%s%c%d%c%u%c%s%c%c", msgId,UI_MSG_FSP,deny -> ifUserInfo,
						UI_MSG_FSP, deny -> user.alphaNumId, UI_MSG_FSP, deny -> user.name, UI_MSG_FSP, deny -> user.photo,
						UI_MSG_FSP, deny -> genInfo.line1, UI_MSG_FSP, deny -> genInfo.line2, UI_MSG_FSP,
						deny -> genInfo.line3, UI_MSG_FSP, deny -> genInfo.imageIdx,
						UI_MSG_FSP, deny -> user.userId, UI_MSG_FSP, deny -> user.designation, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_CAFE_USER_DENIED_COMMON_SCREEN:	  		//Display Cafe User Denied Screen
			case GUI_MSG_USER_DENIED_COMMON_SCREEN:				//Display User Denied Common Screen
			{
				USER_COMMON_QT_t * deny = msgPtr;
				GetCurrentTimeForUserAllowScreen(deny->accessTime);
				sprintf(msgBuf,"%d%c%d%c%s%c%s%c%d%c%s%c%s%c%s%c%d%c%u%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%d%c%d%c%s%c%d%c%d%c%d%c%c",
						msgId, UI_MSG_FSP, deny->ifUserInfo, UI_MSG_FSP, deny -> user.alphaNumId, UI_MSG_FSP,
						deny -> user.name, UI_MSG_FSP,	deny -> user.photo, UI_MSG_FSP,
						deny -> genInfo.line1, UI_MSG_FSP, deny -> genInfo.line2, UI_MSG_FSP,
						deny -> genInfo.line3, UI_MSG_FSP, deny -> genInfo.imageIdx, UI_MSG_FSP,
						deny -> user.userId, UI_MSG_FSP, deny -> user.designation, UI_MSG_FSP,
						deny -> accessTime, UI_MSG_FSP, deny->matchingScore, UI_MSG_FSP,
						deny -> greetingInfo.msg1, UI_MSG_FSP, deny -> greetingInfo.msg2, UI_MSG_FSP,
						deny -> genInfo.line4, UI_MSG_FSP, deny -> genInfo.line5, UI_MSG_FSP,
						deny -> cafeSepF, UI_MSG_FSP, SysBasicPara.accessAllowScreenDuration, UI_MSG_FSP,
						deny -> temperature, UI_MSG_FSP, deny -> temperatureUnit, UI_MSG_FSP,
						deny -> temperatureViolation, UI_MSG_FSP, deny -> faceMaskDecision, UI_MSG_FSP,
						// deny -> isJobCodeAllowScreen, UI_MSG_FSP,
						EOT_FSP);
				break;
			}

			case GUI_MSG_USER_ALLOWED:				//Display User Allowed Screen
			{
				USER_ALLOWED_QT_t * allow = msgPtr;
				sprintf(msgBuf,"%d%c%s%c%s%c%d%c%s%c%s%c%s%c%d%c%u%c%s%c%s%c%s%c%s%c%s%c%c", msgId, UI_MSG_FSP, allow -> user.alphaNumId, UI_MSG_FSP,
						allow -> user.name, UI_MSG_FSP, allow -> user.photo, UI_MSG_FSP, allow -> genInfo.line1,
						UI_MSG_FSP, allow -> genInfo.line2, UI_MSG_FSP, allow -> genInfo.line3, UI_MSG_FSP,
						allow -> genInfo.imageIdx, UI_MSG_FSP, allow -> user.userId, UI_MSG_FSP,
						allow -> jobCodeInfo.jobCodeId, UI_MSG_FSP, allow -> jobCodeInfo.jobCodeName, UI_MSG_FSP, allow -> jobCodeInfo.jobActionStr, UI_MSG_FSP,
						allow -> allowTime, UI_MSG_FSP, allow -> user.designation, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_CAFE_USER_ALLOWED_COMMON_SCREEN:			//Display Cafe User Allowed Screen
			case GUI_MSG_USER_ALLOWED_COMMON_SCREEN:				//Display User Allowed Common Screen
			{
				USER_COMMON_QT_t * allow = msgPtr;
				allow->ifUserInfo = TRUE;
				GetCurrentTimeForUserAllowScreen(allow->accessTime);
				sprintf(msgBuf,"%d%c%d%c%s%c%s%c%d%c%s%c%s%c%s%c%d%c%u%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%d%c%u%c%s%c%d%c%d%c%d%c%s%c%d%c%c",
						msgId, UI_MSG_FSP, allow -> ifUserInfo, UI_MSG_FSP, allow -> user.alphaNumId, UI_MSG_FSP,
						allow -> user.name, UI_MSG_FSP,	allow -> user.photo, UI_MSG_FSP,
						allow -> genInfo.line1, UI_MSG_FSP, allow -> genInfo.line2, UI_MSG_FSP, allow -> genInfo.line3, UI_MSG_FSP,
						allow -> genInfo.imageIdx, UI_MSG_FSP, allow -> user.userId, UI_MSG_FSP,
						allow -> user.designation, UI_MSG_FSP, allow -> accessTime, UI_MSG_FSP,
						allow -> matchingScore, UI_MSG_FSP, allow -> greetingInfo.msg1, UI_MSG_FSP, allow -> greetingInfo.msg2, UI_MSG_FSP,
						allow -> genInfo.line4, UI_MSG_FSP, allow -> genInfo.line5, UI_MSG_FSP, 
						allow -> cafeSepF, UI_MSG_FSP, SysBasicPara.accessAllowScreenDuration, UI_MSG_FSP,
						allow -> temperature, UI_MSG_FSP, allow -> temperatureUnit, UI_MSG_FSP, allow -> temperatureViolation, UI_MSG_FSP,
						allow -> faceMaskDecision, UI_MSG_FSP,allow -> genInfo.line6, UI_MSG_FSP, GetUserAllowEntryExitModeFlag(), UI_MSG_FSP, /*allow -> isJobCodeAllowScreen, UI_MSG_FSP,*/
						EOT_FSP);
				break;
			}

			case GUI_MSG_EHTERNET_CFG_SCREEN:			//Ethernet Address
			{
				sprintf(msgBuf,"%d%c%s%c%s%c%s%c%c",msgId,UI_MSG_FSP, EthernetParameters.ipAddress, UI_MSG_FSP,
						EthernetParameters.subnet, UI_MSG_FSP, EthernetParameters.gateway, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_ENROLL_SPL_FN_SCREEN:
			{
				ENROLL_SPL_FUNC_t * enrollSplFuncDataPtr = msgPtr;

				sprintf(msgBuf,"%d%c%d%c%d%c%c",msgId, UI_MSG_FSP,
							enrollSplFuncDataPtr->noOfCards, UI_MSG_FSP,
							enrollSplFuncDataPtr->maxCards, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_ENROLLMENT_SCREEN:
			{
				QT_ENROLL_MENU_SCREEN_t * enrollMenuPtr = msgPtr;

				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%c",msgId, UI_MSG_FSP,
							enrollMenuPtr->enrollUserF, UI_MSG_FSP,
							enrollMenuPtr->enrollSplFuncF, UI_MSG_FSP,
							enrollMenuPtr->deleteCredF, UI_MSG_FSP,EOT_FSP);
				break;
			}

			//Note:- Always Update SW_VER and SW_REV while releasing
			case GUI_MSG_SYS_STATUS_SCREEN:
			{
				CHAR	verRevBuff[10] = {0};

				snprintf(verRevBuff, sizeof(verRevBuff), "V%02dR%02d.%02d", SW_VER, SW_REV, SW_MINOR_REV);
				sprintf(msgBuf, "%d%c%s%c%s%c%c", msgId,UI_MSG_FSP,
							verRevBuff, UI_MSG_FSP,
							EthernetParameters.macAddress,UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_ABOUT_DEVICE_CONNECTIVITY_SCREEN:
			{
				QT_ABOUT_DEVICE_CONNECTIVITY_t * abtDeviceConnectivityDataPtr = msgPtr;

				if(CommState == SLAVE_ONLINE)
				{
					bufLen = 0;
					snprintf(msgBuf,sizeof(msgBuf),"%d%c",msgId,UI_MSG_FSP);
					bufLen  = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",EthernetParameters.ipAddress, UI_MSG_FSP);
					bufLen  = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",abtDeviceConnectivityDataPtr->panelIpAddress, UI_MSG_FSP);
					bufLen  = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c%c",HandleWifiStatusStr(getSysWifiStatusStr[GetWiFiStatus()]), UI_MSG_FSP, EOT_FSP);
				}
				else
				{
					bufLen = 0;
					snprintf(msgBuf,sizeof(msgBuf),"%d%c",msgId,UI_MSG_FSP);
					bufLen  = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",EthernetParameters.ipAddress, UI_MSG_FSP);
					bufLen  = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c","", UI_MSG_FSP);
					bufLen  = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c%c",HandleWifiStatusStr(getSysWifiStatusStr[GetWiFiStatus()]), UI_MSG_FSP, EOT_FSP);
				}

				break;
			}

			case GUI_MSG_ABOUT_DEVICE_ALARMS_SCREEN:
			{
				QT_ABOUT_DEVICE_ALARMS_t * abtDeviceAlarmsDataPtr = msgPtr;

				if(CommState == SLAVE_ONLINE)
				{
					bufLen = 0;
					snprintf(msgBuf,sizeof(msgBuf),"%d%c",msgId, UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[abtDeviceAlarmsDataPtr->doorRtcFault]), UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[abtDeviceAlarmsDataPtr->doorTemper]), UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[abtDeviceAlarmsDataPtr->doorAbnormal]), UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[abtDeviceAlarmsDataPtr->doorForceOpen]), UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[abtDeviceAlarmsDataPtr->doorFault]), UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[abtDeviceAlarmsDataPtr->panicAlarm]), UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[abtDeviceAlarmsDataPtr->dmzTimerExpired]), UI_MSG_FSP, EOT_FSP);
				}
				else
				{
					bufLen = 0;
					snprintf(msgBuf,sizeof(msgBuf),"%d%c",msgId, UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[INACTIVE]), UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[INACTIVE]), UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[INACTIVE]), UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[INACTIVE]), UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[INACTIVE]), UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[INACTIVE]), UI_MSG_FSP);
					bufLen = strlen(msgBuf);
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysAlarmStr[INACTIVE]), UI_MSG_FSP, EOT_FSP);
				}

				break;
			}

			case GUI_MSG_ABOUT_DEVICE_READERS_INTERFACES_SCREEN:
			{
				CHAR	readerStatusBuff[25] = {0};
				CHAR	readerStatusBuff1[25] = {0};
				UINT8 	length;

				if(readerStatus[READER1] == READER_IS_ACTIVE)
				{
					sprintf(readerStatusBuff, sysReaderTypeStr[SysBasicPara.reader[READER1].type]);
				}
				else
				{
					sprintf(readerStatusBuff, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER1]]));
				}

				bufLen = 0;
				snprintf(msgBuf,sizeof(msgBuf),"%d%c",msgId, UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",(CHARPTR)readerStatusBuff,UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				if(YES == IsVariant_With_Internal_FP_Rdr())
				{
					snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER2]]), UI_MSG_FSP);
				}
				length = strlen(msgBuf);

				if(SysBasicPara.cerPresentF == TRUE)
				{
					if((readerStatus[READER3] == READER_IS_ACTIVE))
					{
						sprintf(readerStatusBuff, sysReaderTypeStr[SysBasicPara.reader[READER3].type]);
					}
					else
					{
						sprintf(readerStatusBuff, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER3]]));
					}
					sprintf(readerStatusBuff1, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER4]]));
				}
				else if((SysBasicPara.cbrPresentF == TRUE) || (SysBasicPara.atomRD100PresentF == TRUE))
				{
					if((readerStatus[READER3] == READER_IS_ACTIVE))
					{
						sprintf(readerStatusBuff, sysReaderTypeStr[SysBasicPara.reader[READER3].type]);
					}
					else
					{
						sprintf(readerStatusBuff, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER3]]));
					}
					sprintf(readerStatusBuff1, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[READER_NOT_CONFIGURED]));
				}
				else if((SysBasicPara.atomRD200PresentF == TRUE) || (SysBasicPara.atomRD300PresentF == TRUE))
				{
					if((readerStatus[READER3] == READER_IS_ACTIVE))
					{
						sprintf(readerStatusBuff, sysReaderTypeStr[SysBasicPara.reader[READER3].type]);
					}
					else
					{
						sprintf(readerStatusBuff, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER3]]));
					}
					sprintf(readerStatusBuff1, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER5]]));
				}
				else
				{
					if(readerStatus[READER3] == READER_IS_ACTIVE)
					{
						if(SysBasicPara.reader[READER3].type != SUP_FP_3030_READER)
						{
							sprintf(readerStatusBuff, sysReaderTypeStr[SysBasicPara.reader[READER3].type]);
							sprintf(readerStatusBuff1, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER4]]));
						}
						else
						{
							sprintf(readerStatusBuff, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER4]]));
							sprintf(readerStatusBuff1, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER3]]));
						}
					}
					else
					{
						sprintf(readerStatusBuff, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER3]]));
						sprintf(readerStatusBuff1, GetDisplayMessage(FALSE,NULL_CHAR,getSysReaderStatusStr[readerStatus[READER3]]));
					}
				}

				sprintf(&msgBuf[length],"%s%c%s%c",(CHARPTR)readerStatusBuff,UI_MSG_FSP,(CHARPTR)readerStatusBuff1, UI_MSG_FSP);
				length = strlen(msgBuf);

				bufLen = length;
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysStorageDevStatusStr[MmcInfo.status]), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysExitSwitchStr[SysInputStatus.exitSw]), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysDoorStatusStr[CurrStatusbarData.doorLockStatus]), UI_MSG_FSP);
				bufLen = strlen(msgBuf);
				snprintf(&msgBuf[bufLen],(sizeof(msgBuf)-bufLen),"%s%c%c",GetDisplayMessage(FALSE,NULL_CHAR,getSysDoorMonStatusStr[SysInputStatus.doorSense]), UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_FIRMWARE_UPLOAD_PROG:
			{
				FIRMWARE_UPLOAD_PROGRESS_QT_t * progress = msgPtr;

				sprintf(msgBuf, "%d%c%s%c%s%c%d%c%c", msgId,UI_MSG_FSP,progress -> splFunc, UI_MSG_FSP,
												progress -> genInfo.line1, UI_MSG_FSP,
												0, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_BACKUP_UPDATE_PROG:
			{
				BACKUP_UPDATE_PROGRESS_QT_t * progress = msgPtr;

				sprintf(msgBuf, "%d%c%s%c%s%c%d%c%c", msgId,UI_MSG_FSP,progress -> splFunc, UI_MSG_FSP,
						progress -> genInfo.line1, UI_MSG_FSP,
			   progress -> prog, UI_MSG_FSP,EOT_FSP);
				break;
			}

			case GUI_MSG_ENROLL_USER_SCREEN:
			{
				ENROLL_USER_t * enrollInput = msgPtr;
				sprintf(msgBuf, "%d%c%d%c%d%c%d%c%d%c%d%c%d%c%s%c%s%c%d%c%c",msgId,UI_MSG_FSP,
								enrollInput -> credType, UI_MSG_FSP,
								enrollInput -> fpCount, UI_MSG_FSP,
								enrollInput -> cardCount, UI_MSG_FSP,
								enrollInput -> maxCards , UI_MSG_FSP ,
								enrollInput -> maxFp , UI_MSG_FSP ,
								enrollInput -> enrollIdType , UI_MSG_FSP ,
								enrollInput -> alphaNumId, UI_MSG_FSP,
								enrollInput -> name, UI_MSG_FSP,
								enrollInput -> maxFaces , UI_MSG_FSP ,
								EOT_FSP);
				break;
			}

			case GUI_MSG_DEL_CRED_SCREEN:
			{
				DELETE_USER_CREDEN_t *deleteInput = msgPtr;
				sprintf(msgBuf, "%d%c%d%c%d%c%c", msgId,UI_MSG_FSP,deleteInput -> credType, UI_MSG_FSP, deleteInput -> enrollIdType , UI_MSG_FSP ,EOT_FSP);
				break;
			}

			// Common handling if 'msgBuf' is already Filled
			case GUI_MSG_ENROLL_GET_NAME_FROM_ID: 			//Name From Id
			case GUI_MSG_SEARCHED_ENROLL_USER_LIST: 		//Searched User name List
			case GUI_MSG_DEL_CRED_GET_NAME_FROM_ID: 		//Name From Id
			case GUI_MSG_SEARCHED_DEL_CRED_USER_LIST: 		//Searched User name List
			{
				EPRINT(DC_LOG,"msgPtr[%p]",msgPtr);
				if( (msgPtr == NULL) || ( ((CHARPTR)msgPtr)[0] == 0) )
				{
					sprintf(msgBuf, "%d%c%c", msgId, UI_MSG_FSP, EOT_FSP);
				}
				else
				{
					sprintf(msgBuf, "%d%c%s%c", msgId, UI_MSG_FSP, (CHARPTR) msgPtr, EOT_FSP);
				}
				break;
			}

			case GUI_MSG_VALIDATION_RESPONSE:
			{
				DYNAMIC_VALIDATION_RESPONSE_t * validationInfo = msgPtr;
				sprintf(msgBuf, "%d%c%d%c%c", msgId, UI_MSG_FSP, validationInfo -> respMsgIdx, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_VIEW_USER_PROFILE_SCREEN:
			{
				USER_VIEW_PROFILE_QT_t * userProfileInfo = msgPtr;
				sprintf(msgBuf, "%d%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%c", msgId, UI_MSG_FSP,
						userProfileInfo -> userId, UI_MSG_FSP,
						userProfileInfo -> designation, UI_MSG_FSP,
						userProfileInfo -> branch, UI_MSG_FSP,
						userProfileInfo -> department, UI_MSG_FSP,
						userProfileInfo -> bloodGroup, UI_MSG_FSP,
						userProfileInfo -> emergencyContact, UI_MSG_FSP,
						userProfileInfo -> medicalHistory, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_WIFI_CFG_SCREEN:
			{
			    UINT8 length;

			    sprintf(msgBuf,"%d%c%s%c%d%c%s%c%d%c%s%c%s%c%s%c%s%c%s%c%d%c%d%c%d%c%s%c%s%c%d%c%d%c%d%c",msgId,UI_MSG_FSP,
				    WifiParameters.ssid, UI_MSG_FSP, WifiParameters.secType, UI_MSG_FSP,
				    WifiParameters.passphresh, UI_MSG_FSP, WifiParameters.nwType, UI_MSG_FSP,
				    WifiParameters.ipAddress, UI_MSG_FSP, WifiParameters.subnet, UI_MSG_FSP,
				    WifiParameters.gateway ,UI_MSG_FSP, WifiParameters.dns1 ,UI_MSG_FSP,
				    WifiParameters.dns2 ,UI_MSG_FSP, GetWiFiStatus(),UI_MSG_FSP,
				    WifiParameters.keymgmt, UI_MSG_FSP, WifiParameters.advAuthType, UI_MSG_FSP,
				    WifiParameters.anonymousIdentity, UI_MSG_FSP, WifiParameters.userName, UI_MSG_FSP,
				    WifiParameters.peapVer, UI_MSG_FSP, WifiParameters.innerAuth, UI_MSG_FSP,
				    WifiParameters.enableWifi, UI_MSG_FSP);

			    length = strlen(msgBuf);

			    if(GetWiFiStatus() == WIFI_CONNECTED)
			    {
				snprintf(&msgBuf[length], (QT_RX_BUF_SIZE-length),"%d%c%d%c%c", GetConnectedSsidSignalStrength(), UI_MSG_FSP,WifiParameters.bandFreqType, UI_MSG_FSP, EOT_FSP);
			    }
			    else
			    {
				snprintf(&msgBuf[length], (QT_RX_BUF_SIZE-length),"%d%c%c",WifiParameters.bandFreqType, UI_MSG_FSP, EOT_FSP);
			    }
			    GetPereodicWiFiSignalStrength();
			    break;
			}

			case GUI_MSG_WIFI_SIGNAL_STRENGTH:
			{
			    WIFI_SIGNAL_QUALITY_e sigCheck = *(WIFI_SIGNAL_QUALITY_e *)msgPtr;
			    snprintf(msgBuf,QT_RX_BUF_SIZE,"%d%c%d%c%s%c%d%c%d%c%d%c%c",msgId,UI_MSG_FSP,sigCheck,UI_MSG_FSP,WifiParameters.ssid,UI_MSG_FSP,WifiParameters.secType,UI_MSG_FSP,WifiParameters.enableWifi,UI_MSG_FSP,WifiParameters.bandFreqType, UI_MSG_FSP,EOT_FSP);
			    break;
			}

			case GUI_MSG_SEARCH_WIFI_SCREEN:
			{
				sprintf(msgBuf, "%d%c%s%c", msgId,UI_MSG_FSP,(CHARPTR) msgPtr,EOT_FSP);	//msgBuf Already Filled From Menu
				break;
			}

			case GUI_MSG_WPS_PROGRESS_SCREEN:
			{
				WPS_PROGRESS_t *wpsDataPtr = msgPtr;

				sprintf(msgBuf, "%d%c%d%c%s%c%c",msgId, UI_MSG_FSP, wpsDataPtr->wpsTrigerTyp, UI_MSG_FSP,
								wpsDataPtr->wpsPin, UI_MSG_FSP, EOT_FSP);
				break;
			}

			case GUI_MSG_ADD_ASSIGN_USER_ON_DEVICE:
			{
				ENROLL_USER_t * enrollInput = msgPtr;
				sprintf(msgBuf, "%d%c%s%c%s%c%c",msgId,UI_MSG_FSP,
								enrollInput -> alphaNumId, UI_MSG_FSP,
								enrollInput -> name, UI_MSG_FSP,
								EOT_FSP);
				break;
			}

			case GUI_MSG_DYNAMIC_TEMPERATURE_SCREEN:
			{
				DYNAMIC_TEMP_SCREEN_INFO_QT_t * DynamicTempScreenInfo = msgPtr;

				sprintf(msgBuf, "%d%c%s%c%d%c%d%c%d%c%s%c%d%c%s%c%c",
								msgId,UI_MSG_FSP, DynamicTempScreenInfo -> titleText, UI_MSG_FSP, DynamicTempScreenInfo -> imageIdx, UI_MSG_FSP,
								DynamicTempScreenInfo -> footerHideF, UI_MSG_FSP, DynamicTempScreenInfo -> ImageHideF, UI_MSG_FSP,
								DynamicTempScreenInfo->user.alphaNumId,UI_MSG_FSP, DynamicTempScreenInfo->user.userId,UI_MSG_FSP,
								DynamicTempScreenInfo->user.name, UI_MSG_FSP,EOT_FSP);
				break;
			}

            case GUI_MSG_LOG_INFO:
            {
                QT_LOG_INFO_t *tGuiLogInfo = (QT_LOG_INFO_t *)msgPtr;
                snprintf(msgBuf, sizeof(msgBuf),"%d%c%d%c%u%c%s%c%u%c%s%c%d%c%c",
                         msgId, UI_MSG_FSP,tGuiLogInfo->guiLogSeverity, UI_MSG_FSP,
                         tGuiLogInfo->sysLogEnableF, UI_MSG_FSP,
                         tGuiLogInfo->sysLogServerAddr, UI_MSG_FSP,
                         tGuiLogInfo->sysLogServerPort, UI_MSG_FSP,
                         tGuiLogInfo->deviceMacAddr, UI_MSG_FSP,
                         GetDiffBetGmtAndLocalTime(), UI_MSG_FSP,
                         EOT_FSP);
                break;
            }
		}
	}

	if(qtSocketInfo.clientConnectionF == TRUE)
	{
		// Send Bytes
		sendBytes = write(qtSocketInfo.clientSockFd, (VOIDPTR)&msgBuf[0], strlen(msgBuf) );

		if( sendBytes != strlen(msgBuf))
		{
			EPRINT(GUI_LOG,"Failed to transmit data %d and error %s discriptor:%d", sendBytes, strerror(errno), qtSocketInfo.clientSockFd);
			return FAIL;
		}
		else
		{
			DPRINT(GUI_LOG, "Message sent to Qt, Message Id= 0x%x Data[%s]", msgId, msgBuf);
			return TRUE;
		}
	}
	else
	{
		DPRINT(GUI_LOG, "Can't send data due to no connection");
	}
	return FALSE;
}

//*****************************************************************************
//	InitQtMessageQueue()
//	Param:
//		IN : None
//		OUT: None
//	Returns:True/False
//	Description:
//				This function Will Initialize the Queue required for storing
//				Qt Messages.
//*****************************************************************************
static  BOOL InitQtMessageQueue(VOID)
{
	Qt_Message_Queue.inPtr = 0;
	Qt_Message_Queue.outPtr = 0;
	Qt_Message_Queue.length = QT_QUEUE_SIZE;

	DPRINT(GUI_LOG, "Qt Message Queue Initialized");
	return TRUE;
}

//*****************************************************************************
//	GetStrBeforeFsp()
//	Param:
//		IN : CHAR **	:Source String
//		OUT: CHAR		: Field Delimeter
//	Returns:CHARPTR		: Extracted String Before UI_MSG_FSP
//	Description:
//		This function Extracts String Before a Delimeter ,And  Jumps
//		to next string after a Delimeter. Extracted string will
//		be retuned as a Pointer to a First Char of the string.
//	NOTE: Every Time a srcStr will be the same. Because After Extracting a first
//		string and removing a Delimeter, pointer to the source string gets moved.
//*****************************************************************************
CHARPTR GetStrBeforeFsp(CHARPTR * srcStr, CHAR delim)
{
	CHARPTR tempPtr, sPtr;

	tempPtr = strchr(*srcStr, delim);

	if(tempPtr)
	{
		*tempPtr = 0;	//NULL
		sPtr = *srcStr;
		*srcStr = (tempPtr + 1);
		tempPtr = sPtr;
	}
	return tempPtr;
}

//*****************************************************************************
//	SetDisplayClockScreenTimer()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function loads timer to display analog clock screen.
//*****************************************************************************
static BOOL SetDisplayClockScreenTimer(void)
{
	return TRUE;
}

//*****************************************************************************
//	WriteQtMessageIntoQueue()
//	Param:
//		IN :
//		OUT:
//	Returns:
//	Description:
//
//*****************************************************************************
static BOOL WriteQtMessageIntoQueue(GUI_MSG_ID_e msgId, VOIDPTR msgStruct, UINT32 fMessageSize)
{
  DPRINT(GUI_LOG, "WriteQtMessageIntoQueue:msgId=0x%x",msgId);

  UINT8 	tmpInPtr;

  tmpInPtr= Qt_Message_Queue.inPtr + 1; //One Event to Write

  if(tmpInPtr >= Qt_Message_Queue.length)
	tmpInPtr=0;

  if(tmpInPtr == Qt_Message_Queue.outPtr)
  {
	  EPRINT(GUI_LOG, "Qt message queue is Full");
	  return FALSE;
  }

  Qt_Message_Queue.pBuf[tmpInPtr].qtMsgId = msgId;
  Qt_Message_Queue.pBuf[tmpInPtr].qtMsgLen = fMessageSize;

  if(msgStruct)
	  memcpy((VOIDPTR)&Qt_Message_Queue.pBuf[tmpInPtr].msg[0], (VOIDPTR)msgStruct, fMessageSize);

  Qt_Message_Queue.inPtr = tmpInPtr;
  return TRUE;
}

//*****************************************************************************
//	ReadQtMessageFromQueue()
//	Param:
//		IN :
//		OUT:
//	Returns:
//	Description:
//
//*****************************************************************************
BOOL ReadQtMessageFromQueue(GUI_MSG_ID_e *readMsgId, VOIDPTR readMsg, UINT16PTR length)
{
	UINT8 tmpOutPtr;

	if(Qt_Message_Queue.inPtr == Qt_Message_Queue.outPtr)	// Check if queue is empty ?
	  return(FALSE);

	tmpOutPtr = Qt_Message_Queue.outPtr+ 1;					//one Message to read
	if(tmpOutPtr >= Qt_Message_Queue.length)				//OverWrite
		tmpOutPtr = 0;

	*readMsgId = Qt_Message_Queue.pBuf[tmpOutPtr].qtMsgId;
	*length 	= Qt_Message_Queue.pBuf[tmpOutPtr].qtMsgLen;
	memcpy((VOIDPTR)readMsg, (VOIDPTR)&Qt_Message_Queue.pBuf[tmpOutPtr].msg[0], *length);
	Qt_Message_Queue.outPtr = tmpOutPtr;

	DPRINT(GUI_LOG, "ReadQtMessageFromQueue:msgId=0x%x", *readMsgId);
	return(TRUE);
}

//*****************************************************************************
//	HandleWifiStatusStr()
//	Param:
//		IN : Multi- language enums
//		OUT: None
//	Returns: multi-language string or ip address if connected
//	Description: This function will handle wifi status related multi-language string
//
//*****************************************************************************
static CHARPTR HandleWifiStatusStr(MSG_MULTI_LANGUAGE_STRING_e status)
{
	if(status == MSG_CONNECTED_99)
	{
		return WifiParameters.ipAddress;
	}
	else
	{
		return GetDisplayMessage(FALSE,NULL_CHAR,status);
	}
}

//*****************************************************************************
//	HandleModemStatusStr()
//	Param:
//		IN : Multi- language enums
//		OUT: None
//	Returns: multi-language string or ip address if connected
//	Description: This function will handle Modem status related multi-language string
//
//*****************************************************************************
static CHARPTR HandleModemStatusStr(MSG_MULTI_LANGUAGE_STRING_e status)
{
	if(status == MSG_CONNECTED_99)
	{
		return BroadBandNwPara.ipAddress[MODEM_NW_ADDR_TYP_DISP];
	}
	else
	{
		return GetDisplayMessage(FALSE,NULL_CHAR,status);
	}
}

/**************************************************************************************************
SetPopupImagesOnSlaveDegraed
**************************************************************************************************/
/**
@brief         	This function is set POPUP screen According to door online or offline/degraded
@param[in]		ImgIdx - POPUP Img index 
**************************************************************************************************/
void SetPopupImagesOnSlaveDegraed(UINT8 *ImgIdx)
{
  UINT8 msgId = (*ImgIdx);
  DOOR_CONNECTIVITY_STATUS_t doorConnectivityStatus;
  
  GetDoorConnectivityStatus(&doorConnectivityStatus);
  
  if((doorConnectivityStatus.doorApplType == PANEL_DOOR_APPL) && (doorConnectivityStatus.status != SLAVE_ONLINE))
  {
      switch(msgId)
      {
	default: 
	  break;
	  
	case POPUP_READING_CARD:
	{
	    (*ImgIdx) = PD_POPUP_READING_CARD;
	    break;
	}
	
	case POPUP_INAVALID_PASSWORD:
	{
	  (*ImgIdx) = PD_POPUP_INVALID_PASSWORD;
	    break;
	}
	
	case POPUP_TIMEOUT:
	{
	    (*ImgIdx) = PD_POPUP_MENU_TIMEOUT;
	    break;
	}
	
	case POPUP_SYSTEM_DEFAULTING:
	{
	    (*ImgIdx) = PD_POPUP_SYSTEM_DEFAULTING;
	    break;
	}
	
	case POPUP_SYSTEM_DEFAULTED:
	{
	    (*ImgIdx) = PD_POPUP_SYSTEM_DEFAULTED;
	    break;
	}
	
	case POPUP_SUCCESS:
	{
	    (*ImgIdx) = PD_POPUP_FIRMWARE_XFER_SUCCESS;
	    break;
	}
	
	case POPUP_FAILED:
	{
	    (*ImgIdx) = PD_POPUP_FIRMWARE_XFER_FAIL;
	      break;
	}
	
	case POPUP_FIRMWARE_TRANSFER_SUCCESS:
	{
	    (*ImgIdx) = PD_POPUP_FIRMWARE_UPLOAD_SUCCESS;
	    break;
	}
	
	case POPUP_FIRMWARE_UPLOAD_FAIL:
	{
	    (*ImgIdx) = PD_POPUP_FIRMWARE_UPLOAD_FAIL;
	    break;
	}
	
	case POPUP_SEARCHING_FOR_WIFI:
	{
	    (*ImgIdx) = PD_POPUP_SEARCHING_FOR_WIFI;
	      break;
	}
	  
	case POPUP_PROCESSING:
	{
	      (*ImgIdx) = PD_POPUP_PROCESSING;
	      break;
	}
	  
	case POPUP_NO_ACESSPOINTS_FOUND:
	{
	      (*ImgIdx) = PD_POPUP_NO_ACESSPOINTS_FOUND;
	      break;
	}

	case POPUP_DEVICE_BUSY:
	{
	      (*ImgIdx) = PD_POPUP_DEVICE_BUSY;
	      break;
	}
     
	
      }
      DPRINT(GUI_LOG, "Image Index Change From [%d] to [%d]", msgId, (*ImgIdx));
	}
}


/**************************************************************************************************
SetSplFnEnrollImagesOnPanelDoor
**************************************************************************************************/
/**
@brief         	This function is set the panel door special function id according to Qt's special function Index.
@param[in]		SplFnImgIdx - Special function ImagId/Position Id in Qt
**************************************************************************************************/
void SetSplFnEnrollImagesOnPanelDoor(UINT8 *SplFnImgIdx)
{
  UINT8 msgId;
  
  //For getting proper panel door special function id according to Enroll special fucntions Index
  //Beacause Enroll special fucntions Index in Qt is mismatching with panel door SPECIAL_FUNCTIONS_e
    msgId = (*SplFnImgIdx) + ( SKIP_PD_SPFN_ID + 1 );// Added plus 1 for prechecking special function ID
   
  if(GetDoorApplType() == PANEL_DOOR_APPL)
  {
      switch(msgId)
      {
	default: 
	  break;
	  
	case PD_CLEAR_SYS_ALARM:
	{
	    DPRINT(GUI_LOG, "\nSpecial function Index Change From  [%u] to\n",(*SplFnImgIdx));
	    
	    (*SplFnImgIdx)=(*SplFnImgIdx) + SKIP_PD_SPFN_ID ;
	     break;
	}
	
      }
      DPRINT(GUI_LOG, "\nSpecial function Index  [%u]\n",(*SplFnImgIdx));
  }
}

void UpdateGuiLogInfo(void)
{
    int16_t 		tGuiLogSeverity = GUI_LOG_SEVERITY_ERROR;
    SYSLOG_CNFG_t	tSysLogCnfg;
    QT_LOG_INFO_t	qtLogInfo;

    SyslogGetConfig(&tSysLogCnfg);
    if(tSysLogCnfg.fileLogFlag[GUI_LOG] == ENABLED)
    {
        // Get Gui Log Severity from Matrix Registry file
            GetRegistryValueInt(MX_GUI_LOG_SEVERITY, (int16_t)GUI_LOG_SEVERITY_VERBOSE, (int16_t)GUI_LOG_SEVERITY_NONE,(int16_t)GUI_LOG_SEVERITY_ERROR, &tGuiLogSeverity);
    }
    qtLogInfo.guiLogSeverity = tGuiLogSeverity;
    qtLogInfo.sysLogEnableF = tSysLogCnfg.enableSyslogFlag;
    snprintf(qtLogInfo.sysLogServerAddr, MAX_IP_ADDRESS, "%s", tSysLogCnfg.serverAddr);
    qtLogInfo.sysLogServerPort = tSysLogCnfg.serverPort;
    snprintf(qtLogInfo.deviceMacAddr, MAX_MAC_ADDRESS, "%s", EthernetParameters.macAddress);
    // Send message to Qt to update the logs info
    SendMessageToQt(&qtLogInfo, GUI_MSG_LOG_INFO);
}

