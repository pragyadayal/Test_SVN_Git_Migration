#if !defined MXDCTASKFEATURES_H
#define MXDCTASKFEATURES_H
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
//   Project      : ACS (Access Control System)
//   Created By   : Ronak Tanna
//   File         : MxDcTaskFeatures.h
//   Description  : This function contains DC Features related Data types and
//					function prototypes.
//
/////////////////////////////////////////////////////////////////////////////

    
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "MxTypedef.h"
#include "MxSlaveCommonMsgList.h"
#include "MxDcTaskCommonDef.h"
#include "MxGpio.h"
#include "MxSysCommonDef.h"
#include "MxConfig.h"
#include "MxAbsenteeRule.h"
#include "MxAntiPassBack.h"
#include "MxFirstInRule.h"
#include "MxOccupancyRule.h"
#include "MxTwoPersonRule.h"
#include "MxUseCountRule.h"

//******** Defines and Data Types ****
// Door related IO status
typedef struct
{
	DOOR_SENSE_STATUS_e		curDoorSenseStatus;
	AUX_INPUT_STATUS_e		curAuxInputStatus;
	IO_STATUS_e				curAuxOutputStatus;
	IO_STATUS_e				curDoorTamperStatus;
	IO_STATUS_e				curExitSwitchStatus;
}DOOR_IO_STATUS_t;

// For door monitoring
typedef struct
{
	DOOR_LOCK_STATE_e	doorLock;			// Current door lock status
	UINT8				doorSenseDsbl;		// Door sense disable flag
	UINT8				auxInDsbl;			// Aux in disable flag
	UINT8				auxOutDsbl;			// Aux out disable flag
	DOOR_IO_STATUS_t	DcIoStatus;
}DOOR_MONITOR_t;

// Door status to show on jeeves
typedef struct
{
	DOOR_LOCK_STATE_e			doorLock;
	IO_STATUS_e					tamper;
	MONITORED_DOOR_STATUS_e		doorSense;
	MONITORED_DOOR_STATUS_e		auxIn;
	MONITORED_DOOR_STATUS_e		auxOut;
	MONITORED_DOOR_STATUS_e		exitSwitch;
}MONITORED_DOOR_STATUS_t;

//******** Function Prototypes *******

BOOL LoadMultiAccessTimer(UINT16 timerValue);
BOOL IsCredentialValid(void);
UINT8 GetCredentialCnt(void);
void CheckNextCredToProcess(void);
BOOL IsStartFrStreamForFaceCredFailed(VOID);


BOOL SetDoorLockState(DOOR_LOCK_STATE_e lockState);
void UpdateDoorStatus(DOOR_SENSE_STATUS_e doorSense);
UINT8 ChangeDoorSoftStatus(DOOR_SOFT_CONTROL_e softControlPara, UINT8 status);
void GetDoorStatus(MONITORED_DOOR_STATUS_t * monitoredStatus);


UINT8 OpenDoor(void);
BOOL LoadAuxInDebounceTimer(UINT32 count);
BOOL ChangeAuxOutputStatus(BOOL status, UINT16 timer);
ACCESS_MODE_e GetAccessModeFromRespPort(USER_TYPE_e userType, RESPONSE_PORT_e respPort);

BOOL IsAuthAllowedReason(AUTH_REASON_e authReason);


BOOL CheckTimeBasedAccess(void);
BOOL IsCurrTimeInGivenWindow(TIME_HHMM_t startTime, TIME_HHMM_t endTime);
BOOL IsDateInGivenWindow(struct tm * checkDate, DATE_DDMMYY_t startDate, DATE_DDMMYY_t endDate);
BOOL IsCurrDayInGivenWindow(struct tm * currDate, BOOL * repeatDays);
UINT16 ConvertDateInDayCnt(UINT8 date, UINT8 month, UINT8 year);
UINT8 CalculateWeekCnt(struct tm * checkDay);
BOOL IsGivenDateHoliday(struct tm * checkDate, UINT8 holidaySchId);
void InitDoorLockState(void);


BOOL CheckVisitorPolicy(VISITOR_MANAGEMENT_t currentVisitor, struct tm * checkDate);

#endif  // MXDCTASKFEATURES_H

