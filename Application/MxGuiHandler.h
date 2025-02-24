#if !defined MXGUIHANDLER_H
#define MXGUIHANDLER_H
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
//   File         : MxGuiHandler.h
//   Description  :
///////////////////////////////////////////////////////////////////////////

//********Place all include files ****
//system includes
#include <stdio.h>
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
//matrix includes
#include "MxTypedef.h"
#include "MxConfig.h"
#include "MxSlaveCommonMsgList.h"
#include "MxGpioDrv.h"
#include "MxFaceRecognition.h"
#include "MxSlaveCommon.h"

//******** Defines and Data Types ****
#define	SOM						'{'
#define EOM						'}'
#define SOT						'['
#define EOT						']'
#define SOI						'('
#define EOI						')'
#define FSP						'&'
#define FVS						'='

#define UI_MSG_FSP				0x05	// Field separator
#define	EOT_FSP					0x02
#define VALUE_FSP 				0x04
#define TIME_FSP				':'		// Field Separator For Time Field
#define DATE_FSP				'-' 	// Field Separator For Date Field
#define DESH_FSP				0x03 	// Field Separator For Date Field

#define QT_QUEUE_SIZE 			15
#define MAX_MESG_BYTE			(4  + NULL_CHAR_SIZE)
#define	MAX_TCP_PORT_SIZE		(5  + NULL_CHAR_SIZE)
#define	QT_RX_BUF_SIZE			2048		// Receive Buffer Maximum Length

#define	MAX_JOB_IN_PAGE			25
#define MAX_TEMPERATURE_LENGTH	8

typedef enum
{
	IMAGE_NONE,
	IMAGE_PARTIAL_APPROACH,
	IMAGE_TOTAL_APPROACH,
}DYNAMIC_IMAGE_USER_INFO_SCREEN_e;

typedef enum
{
	PD_POPUP_USER_ALLOWED,
	PD_POPUP_USER_DENIED,
	PD_POPUP_READING_CARD,
	PD_POPUP_PLACE_YOUR_FINGER,
	PD_POPUP_INVALID_CARD,
	PD_POPUP_CARD_READ_FAILED,
	PD_POPUP_INVALID_PASSWORD,
	PD_POPUP_MENU_TIMEOUT,
	PD_POPUP_SYSTEM_DEFAULTING,
	PD_POPUP_SYSTEM_DEFAULTED,
	PD_POPUP_FIRMWARE_UPLOAD_SUCCESS,
	PD_POPUP_FIRMWARE_UPLOAD_FAIL,
	PD_POPUP_COMMUNICATION_ERROR,
	PD_POPUP_FIRMWARE_XFER_SUCCESS,
	PD_POPUP_FIRMWARE_XFER_FAIL,
	PD_POPUP_SEARCHING_FOR_WIFI,
	PD_POPUP_PROCESSING,
	PD_POPUP_NO_ACESSPOINTS_FOUND,
	PD_POPUP_NETWORK_ESTABLISHED,
	PD_POPUP_DEVICE_BUSY,
	MAX_PD_POPUP,
}PD_DEGRADE_POPUP_IMAGE_e;

typedef enum
{
	VALIDATION_RESP_NONE,						//0
	VALIDATION_RESP_INVALID_PASWORD,			//1
	VALIDATION_RESP_INVALID_IP,					//2
	VALIDATION_RESP_INVALID_SUBNET,				//3
	VALIDATION_RESP_INVALID_GATEWAY,			//4
	VALIDATION_RESP_INVALID_PDNS,				//5
	VALIDATION_RESP_INVALID_ADNS,				//6
	VALIDATION_RESP_INVALID_PORT,				//7
	VALIDATION_RESP_INVALID_DNS,				//8
	VALIDATION_RESP_WRONG_CURR_PSWD,			//9
	VALIDATION_RESP_PSWD_MISSMATCH,				//10
	VALIDATION_RESP_PSWD_BLANK,					//11
    VALIDATION_RESP_INVALID_SERVER_IP_ETH,      //12
    VALIDATION_RESP_INVALID_SERVER_IP_WIFI,     //13
    VALIDATION_RESP_INVALID_SERVER_IP_MODEM,    //14
    VALIDATION_RESP_INVALID_SERVER_PORT_ETH,	//15
    VALIDATION_RESP_INVALID_SERVER_PORT_WIFI,	//16
    VALIDATION_RESP_INVALID_SERVER_PORT_MODEM,	//17
	VALIDATION_RESP_CHARS_NOT_SUPPORTED,		//18
	VALIDATION_RESP_BLANK_SERVER_URL,			//19
	VALIDATION_RESP_BLANK_SERVER_PORT,			//20
	VALIDATION_RESP_INVALID_SERVER_PORT,		//21
	VALIDATION_RESP_BLANK_SERVER_TENANT_ID,		//22
	VALIDATION_RESP_INVALID_SERVER_TENANT_ID,	//23
	MAX_VALIDATION_RESP,
}DYNAMIC_VALIDATION_RESP_e;

typedef enum
{
	CAFE_ALLOW_SCREEN_IMAGE_USER_ALLOWED,
	CAFE_ALLOW_SCREEN_IMAGE_VIEW_BALANCE_SUCCESS,
}CAFE_ALLOW_SCREEN_IMAGE_e;

typedef enum
{
	GRIT_IMAGE_NONE,
	GRIT_IMAGE_1,
	GRIT_IMAGE_2,
	GRIT_IMAGE_3,
	GRIT_IMAGE_4,
	GRIT_IMAGE_5,
	GRIT_IMAGE_6,
	GRIT_IMAGE_7,
	GRIT_IMAGE_8,
	GRIT_IMAGE_9,
	MAX_GRIT_IMAGE,
}GRIT_IMAGE_e;

/**************************************************************************************************
						Index Id will be used when choosing from Multiple items
**************************************************************************************************/
typedef struct
{
	int index;
}INDEX_QT_t;
  
/**************************************************************************************************
						User Information to be displayed
**************************************************************************************************/
typedef struct
{
	USER_QT_t 			user;
	MSG_GEN_INFO_QT_t	genInfo;
	BOOL				cafeSepF;
}CAFE_USER_ALLOWED_QT_t;

/*Ref Message Id:GUI_MSG_USER_DENIED*/
typedef struct __attribute__((packed))
{
	BOOL 				ifUserInfo;
	USER_QT_t 			user;
	MSG_GEN_INFO2_QT_t	genInfo;
}CAFE_POLICY_USER_DENIED_QT_t;

/**************************************************************************************************
						QT All function Inforamation status
**************************************************************************************************/
typedef struct
{
	UINT8   		respMsgIdx;
}DYNAMIC_VALIDATION_RESPONSE_t;

typedef struct
{
	CHAR            titleText[QT_MSG_SIZE]; 	// Title
	CHAR			message1[QT_MSG_SIZE]; 		// Line1
	CHAR			message2[QT_MSG_SIZE];  	// Line2
	CHAR			message3[QT_MSG_SIZE];		// Line3
}SYSTEM_TEST_INFO_t;

/**************************************************************************************************
						Idle screen with company info to be displayed
**************************************************************************************************/
/*
//Ref Message Id:GUI_MSG_PAGE_VIEW_USERPROFILE
typedef struct
{
	USER_QT_t	user;
	CHAR		designation[QT_NAME_SIZE];		//designation
	CHAR		branch[QT_NAME_SIZE];			//branch name
	CHAR		department[QT_NAME_SIZE];		//department
	CHAR		bGroup[5];						//bloodGroup
	CHAR		eContact[QT_NAME_SIZE];			//emergency Contact
}SHOW_USERPROFILE_QT_t;*/

/**************************************************************************************************
						Screen to be shwon in response to LAN Settings if Enable
**************************************************************************************************/
//Ref Message Id:MSG_WIFI_CFG_VALIDATION_EVENT
typedef struct
{
	CHAR				wifissid[MAX_SSID_NAME];
	CHAR				wifiSecType;
	CHAR				wifiEncrypt;
	CHAR				wifiKey[MAX_PASSPHRESH_SIZE];
	CHAR				wifiNwType;
	CHAR				wifiIp[MAX_IP_ADDRESS];
	CHAR				wifiSn[MAX_SUBNET_MASK];
	CHAR				wifiDNS1[MAX_DNS_ADDRESS];
	CHAR				wifiDNS2[MAX_DNS_ADDRESS];
	CHAR				wifiGw[MAX_GATEWAY];
	WIFI_KEY_MGMT_e		wifiEncryptionTyp;
	WIFI_AUTH_TYPE_e	wifiAuthTyp;
	CHAR				wifiAnonymousId[MAX_WIFI_IDENTITY_SIZE];
	CHAR				wifiUserName[MAX_WIFI_IDENTITY_SIZE];
	PEAP_VER_e			wifiPeapVerTyp;
	INNER_AUTH_e		wifiInnerAuthTyp;
	UINT8				wifiConnType;
	CHAR				wifiPin[MAX_WPS_PIN_SIZE];
	CHAR				wifiBssid[MAX_MAC_ADDRESS];
	WIFI_BAND_FREQ_TYPE_e	bandFreqType;
}WIFI_SETTINGS_PARA_t;

typedef struct
{
	WPS_TRIGER_TYP_e	wpsTrigerTyp;
	CHAR				wpsPin[MAX_WPS_PIN_SIZE];
}WPS_PROGRESS_t;
//Ref Message Id:MSG_ETHERNET_CFG_VALIDATION_EVENT
//				:GUI_MSG_SET_EHTERNET_CFG
typedef struct
{
	CHAR	lanIp[MAX_IP_ADDRESS];		//IP Address
	CHAR	lanSm[MAX_SUBNET_MASK];		//Subnet Mask
	CHAR	lanGw[MAX_GATEWAY];		//Gateway
	CHAR	lanPrefDns[MAX_DNS_ADDRESS];	//Preffered DNS
	CHAR    lanAltDns[MAX_DNS_ADDRESS];	//Alternate DNS
	LAN_IP_MODE_e	nwType;
}LAN_ADDRESS_t;

typedef struct
{
	SERVER_MODE_e	serverMode;
	CHAR        	serverURL[MAX_SERVER_ADDR_254];		// Ethernet Server Hostname
	UINT16			serverPortNumber;							// Ethernet Server port to listen ACMS message
	CHAR			serverTenantId[MAX_TENANT_ID];
}SERVER_ADDRESS_t;

typedef struct
{
	UINT8	profileNum;							//Profile No. Selected
	BOOL	isSelected;							//ifActivate
	CHAR	name[MODEM_PROVIDER_NAME_SIZE];		//profile name
	CHAR	dialNo[MODEM_DIAL_NUM_SIZE];		//dial Number
	CHAR	usrName[MODEM_USER_NAME_SIZE];		//user Name
	CHAR	passwd[MODEM_PASSWORD_SIZE];		//password
	MODEM_TYPE_e	serviceType;				//GSM/CDMA
	CHAR	apn[MX_USER_NAME_SIZE];						//Access Point Name
}MODEM_PROFILE_t;

typedef struct
{
	UINT8	profileIdx;
}MODEM_SET_t;

//typedef struct __attribute__((packed))
//{
//	UINT16 	sleepTimer;			//Credentials to be enrolled
//	PWM_DUTY_e 	setBrightness;			//Credentials to be enrolled
//
//}BRIGHTNESS_DETAIL_t;

/**************************************************************************************************
						Screen to be shwon in response to DateTime Settings 
**************************************************************************************************/
//Ref Message Id:GUI_MSG_SETTINGS_DATETIME
typedef struct
{
	CHAR	time[MAX_DATE_TIME_STR_SIZE];		//Time : HH:MM:SS
	CHAR	date[MAX_DATE_TIME_STR_SIZE];		//Date : DD:MM:YYYY
}DATE_TIME_t;

/**************************************************************************************************
						Structure For Status Bar Information
**************************************************************************************************/
//Ref Message Id:GUI_MSG_STATUSBAR_SCREEN;
typedef struct
{
	UINT8	ethStatus;		//is Ethernet Present
	UINT8	wifiStatus;		//is wifi Present
	UINT8	modemStatus;	//is Modem Present
	UINT8	alarmStatus;	//is Alarm Active
 	UINT8	doorLockStatus;	//Door :Lock,Unlock,Normal
 	UINT8	serverStatus;	//Server Online/Offline
 	UINT8	usbStatus;		//is USB Present
 	UINT8	lateInEarlyOutStatus;

 	UINT8	nwFaultF;
 	UINT8	degradeModeF;
 	UINT8	dndF;
 	UINT8	deadManZoneF;
 	UINT8	downloadingF;
 	UINT8   goingInEnableF;
 	UINT8   goingOutEnableF;
 	UINT8	bluetoothStatus;
 	UINT8	temperatureSensorStatusF;
 	FLOAT32	temperature;
 	BOOL	tempUnit;
	UINT8 	FsmSyncStatus;
	UINT8	degradeModeType;
    BOOL    statusBarExpandedF;
}STATUS_BAR_INFO_t;

/**************************************************************************************************
						Structure For Memory Card File Path
**************************************************************************************************/
//Ref Message Id:GUI_MSG_SET_MMC_DEV_PATH;
typedef struct
{
    CHAR	userPicsFile[MAX_PICS_PATH_SIZE];
}MEDIA_FILE_PATH_t;

/**************************************************************************************************
						Structure For System Parameter Information For Status Page
**************************************************************************************************/
//Ref Message Id:GUI_MSG_SYS_STATUS_SCREEN;
typedef struct
{
	CHAR	ipAdd[MAX_IP_ADDRESS];		//IP
	CHAR	snAdd[MAX_SUBNET_MASK];		//Subnet
	CHAR	gwAdd[MAX_GATEWAY];		//GateWay
	CHAR	dnsAdd[MAX_DNS_ADDRESS];		//DNS
	CHAR	serverIp[MAX_IP_ADDRESS];
	UINT16	serverPort;
	CHAR	mac[MAX_MAC_ADDRESS];
	CHAR	verRev[QT_MSG_SIZE];
	CHAR	interface[QT_MSG_SIZE];	//Wireless/Ethernet
	CHAR	acmsStatus[QT_MSG_SIZE];
	CHAR	doorStaus[QT_MSG_SIZE];
}SYSTEM_STATUS_t;

/**************************************************************************************************
						Structure For Backup And Update Activity
**************************************************************************************************/
typedef struct
{
    CHAR				splFunc[QT_MSG_SIZE]; 	// string=special function name
	MSG_GEN_INFO_QT_t 	genInfo;
    UINT8				prog;
}BACKUP_UPDATE_PROGRESS_QT_t;

/**************************************************************************************************
						Structure For Firmware Update
**************************************************************************************************/
typedef struct
{
	CHAR				splFunc[QT_MSG_SIZE]; 	// string=special function name
	MSG_GEN_INFO_QT_t 	genInfo;
}FIRMWARE_UPLOAD_PROGRESS_QT_t;

/**************************************************************************************************
						Structure For Background Image Setting
**************************************************************************************************/
typedef struct
{
	UINT8	imageIdx;
}BACKGROUND_SET_t;

/**************************************************************************************************
						Structure For Clock Setting
**************************************************************************************************/
typedef struct
{
	UINT8	clockIdx;
}CLOCK_SET_t;

/**************************************************************************************************
						Structure For Keypad Setting
**************************************************************************************************/
typedef struct
{
	UINT8	keypadIdx;
}KEYPAD_SET_t;

//Ref Message Id:GUI_MSG_SETTINGS_ADMINPW_CHANGE
/**************************************************************************************************
						Screen to be shown in response to Change Admin Password 
**************************************************************************************************/
typedef struct
{
	CHAR	currPw[MAX_PASSWORD_SIZE];	//Old Password
	CHAR	newPw[MAX_PASSWORD_SIZE];		//New Password
	CHAR	cnfrmPw[MAX_PASSWORD_SIZE];	//Confirm New Password
}CHANGE_PASSWORD_t;

/**************************************************************************************************
						User job code selection screen
**************************************************************************************************/
typedef struct
{
	UINT8	currFirstSubPageNo;
	UINT8	currSubPageCnt;
	UINT8	prevFirstSubPageNo;
	UINT8	prevSubPageCnt;
	UINT8	nextFirstSubPageNo;
	UINT8	nextSubPageCnt;
}JOB_CODE_PAGE_BTN_t;

typedef struct
{
	JOB_CODE_PAGE_BTN_t	pageBtnInfo;
	UINT8				jobCodeCnt;
	CHAR				jobCode[MAX_JOB_IN_PAGE][JOB_CODE_SIZE];
}JOB_CODE_MSG_QT_t;

/**************************************************************************************************
						Enums defining  message id for displaying respective GUI Screen
						//defined as per cosec NGT Menu Navaigation.doc
**************************************************************************************************/
#define GUI_MSG_ID_e SLAVE_MSG_ID_e
  
/**************************************************************************************************
						structure represnting a queue implementation
**************************************************************************************************/

typedef struct  		
{
	GUI_MSG_ID_e	qtMsgId;
	UINT32			qtMsgLen;
	CHAR			msg[QT_RX_BUF_SIZE];
}QT_MESSAGE_STRUCT_t;

typedef struct
 {
        UINT8 				inPtr;						//represents writing head
        UINT8 				outPtr;						//represents reading head
        UINT8				length;						//total length of queue
	QT_MESSAGE_STRUCT_t 	pBuf[QT_QUEUE_SIZE];		//pointer to a queue buffer
}QUEUE_MESSAGE_t;

typedef enum
{
    GUI_LOG_SEVERITY_NONE = 0,
    GUI_LOG_SEVERITY_FATAL,
    GUI_LOG_SEVERITY_ERROR,
    GUI_LOG_SEVERITY_WARNING,
    GUI_LOG_SEVERITY_INFO,
    GUI_LOG_SEVERITY_DEBUG,
    GUI_LOG_SEVERITY_VERBOSE,

    MAX_GUI_LOG_SEVERITY
}GUI_LOG_SEVERITY_e;

typedef struct
{
    GUI_LOG_SEVERITY_e 	guiLogSeverity;
    BOOL				sysLogEnableF;
    CHAR				sysLogServerAddr[MAX_IP_ADDRESS];
    UINT16				sysLogServerPort;
    CHAR				deviceMacAddr[MAX_MAC_ADDRESS];
}QT_LOG_INFO_t;

//******** Function Prototypes *******

void InitGuiHandler(void);
void GuiMsgHandler(void);
CHARPTR GetStrBeforeFsp(CHARPTR * srcStr, CHAR delim);
BOOL SendMessageToQt(VOIDPTR msgPtr, GUI_MSG_ID_e msgId);
VOID ProcessQtMessage();
void SetPopupImagesOnSlaveDegraed(UINT8 *ImgIdx);
void SetSplFnEnrollImagesOnPanelDoor(UINT8 *SplFnImgIdx);
void UpdateGuiLogInfo(void);
#endif //MXGUIHANDLER_H
