#if !defined MXSLAVEMSG_H
#define MXSLAVEMSG_H
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
//   Created By   : Priti Purohit
//   File         : MxSlaveMsg.h
//   Description  : This file defines all Master-slave message data structures.
//
/////////////////////////////////////////////////////////////////////////////

#include "MxTypedef.h"
#include "MxSlaveCommonMsgList.h"
#include "MxConfig.h"
#include "MxCardReaderCommon.h"
#include "MxReaderTask.h"
#include "MxGpio.h"
#include "MxEventLog.h"
#include "MxPDEventLog.h"
#include "MxSysCommonDef.h"
#include "MxBlockDevice.h"
#include "MxSmartCardFormatConfig.h"
#include "MxWiegandOutConfig.h"
#include "MxConfigDef.h"
#include "MxWebAPI.h"
#include "MxSlaveMsgCommon.h"
#include "MxSysCommonUtils.h"
#include "MxSlaveCommon.h"

//******** Defines and Data Types ****

//###################### // structure defined for commands ######################

//###################### // structure defined for responses ######################

// Update 'door status' response
typedef struct __attribute__((packed))
{
	IO_STATUS_t				ioStatus;
	READER_STATUS_e	 		readerStatus[MAX_READERS];		// status for reader
	RTC_STATUS_e			rtcStatus;							
	CALIB_STATUS_e			fprSensorCalibStatus[MAX_FPR_PORT];	
	UINT8					readerType[MAX_READERS];
	IO_STATUS_e				readerTamperStatus;
}DC_STATUS_RESP_DATA_t;

//###################### structures defined for events ######################

// Update 'MMC status' event
typedef struct __attribute__((packed))
{
	MMC_DEVICE_STATUS_e		status;	// current status of MMC device
}MMC_STATUS_EV_DATA_t;

// Firmware upgrade command
typedef struct __attribute__((packed))
{
	READER_ID_e 			readerId;
	UINT32					size;
}SFPR_FW_UPGRADE_START_CMD_DATA_t;

// Firmware upgrade response
typedef struct __attribute__((packed))
{
	READER_ID_e 			readerId;
	UINT8					status;
}SFPR_FW_UPGRADE_START_RESP_DATA_t;

#define	MAX_STR_SIZE			(15 + NULL_CHAR_SIZE)

typedef struct __attribute__((packed))
{
	READER_ID_e 			readerId;
	UINT8 					noOfTemplates;	// no. of templates provided
	UINT8					responseTime;
	UINT8 					template[MAX_SIZE_DOUBLE_SFM_TEMPLATES];	// template(s) data
}PD_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t;

// Update 'verify host template by scan' command
typedef struct __attribute__((packed))
{
	READER_ID_e 			readerId;
	UINT8 					noOfTemplates;	// no. of templates provided
	UINT8					responseTime;
	UINT8					sequenceNumber;
	BOOL					lastMsgF;
	UINT8 					template[MAX_SIZE_DOUBLE_SFM_TEMPLATES];	// template(s) data
}PIC32_PD_VERIFY_HOST_TEMPLATE_BY_SCAN_CMD_DATA_t;

// Update 'verify by template' command
typedef struct __attribute__((packed))
{
	READER_ID_e 			readerId;
	UINT32 					fprUserId;		// NOTE: Refer macro defined in DcTask.h to exchange user id info with finger print reader
	UINT8 					noOfTemplates;	// no. of templates provided
	UINT8 					template[MAX_SIZE_DOUBLE_SFM_TEMPLATES];	// template(s) data
}PD_VERIFY_BY_TEMPLATE_CMD_DATA_t;

// Used for panel and panel Door communication when door is ONLINE
typedef struct __attribute__((packed))
{
	UINT16 		sleepTimer;
	PWM_DUTY_e 	setBrightness;
}BRIGHTNESS_DETAIL_CMD_t;

//******** Function Prototypes *******

#endif  // MXSLAVEMSG_H

