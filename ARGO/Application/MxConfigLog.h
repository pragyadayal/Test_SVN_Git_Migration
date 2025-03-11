#if !defined MXCONFIGLOG_H
#define MXCONFIGLOG_H
///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Comsec Pvt. Ltd., Baroda, India.
//   Project      : Access Control System
//   Created By   : Manish Chandra
//   File         : MxConfigLog.h
//   Description  : This file consists of configuration related defines and
//					functions.
//
/////////////////////////////////////////////////////////////////////////////

//******** Include files *******
#include "MxTypedef.h"
#include "MxSysCommonDef.h"
#include "MxAttendanceDetailConfig.h"


//******** Function Prototypes *******
void IoLinkConfigDebug(UINT8 index);
void SystemVariableConfigDebug(void);
void SystemAdvanceParaConfigDebug(void);
void SysBasicParaConfigDebug(void);
void BlockedUserConfigDebug(UINT8 pg, UINT8 start, UINT8 length);
void DayLightSavingConfigDebug(void);
void FirstInUserGroupConfigDebug(UINT8 index);
void HolidayScheduleConfigDebug(UINT8 index);
void DayParameter(CHARPTR preStrPtr,DATE_DDMMYY_t *Detail);
void EthernetParameterConfigDebug(void);
void ServerParameterConfigDebug(void);
void WifiParameterConfigDebug(void);
void ModemProfileConfigDebug(UINT8 index);
void CommandControlConfigDebug(void);
void PasswordConfigDebug(void);
void TwoPersonGroupConfigDebug(UINT8 index);
void TimeParameter(CHARPTR preStrPtr, TIME_HHMM_t *TimeDetail);
void VisitorConfigDebug(UINT32 index);
void SpecialFunctionsConfigDebug(UINT8 index);
void TimeTrigerFuncConfigDebug(UINT8 index);
void FingerPrintConfigDebug(void);
void ReadersParaConfigDebug(void);
void IoLinkExtentionDevConfigDebug(UINT8 index);
void IoLinkExtDevMappingDebug(UINT8 index);
void CafeItemConfigDebug(UINT8 index);
void CafeMenuConfigDebug(UINT16 index);
void CafeMenuSchConfigDebug(UINT8 index);
void PrinterConfigDebug(void);
void DvrEvtActionConfigDebug(UINT8 index);
void SmartRouteAccessConfigDebug(UINT8 index);
void FirstInInfoConfigDebug(void);
void WebAPIEventsConfigDebug(void);
void CardFormatConfigDebug(void);
void VamConfigDebug(UINT32 index);
void JcmConfigDebug(UINT16 index);
void DoorModeSchedulingConfigDebug(UINT8 index);
void ServerModeConfigDebug(void);
void VyomServerConfigDebug(void);
void BackUpAndUpdateConfigDebug(void);
void V11R3ServerParameterConfigDebug(void);
void ConnectToPanelConfigDebug(void);
void LicenseDongleConfigDebug(void);
void UserAttndnsDetailConfigDebug(USER_ATTNDNS_DETAIL_t *singleUserAttndnsDetailConfig);
void IpCameraConfigDebug (void);
void DisplayBrightnessConfigDebug(void);
VOID SplFnScheduleConfigDebug(VOID);
#ifdef PUSH_API_SUPPORTED
VOID PushAPIConfigDebug(VOID);
void PushApiEventCount(void);
#endif //PUSH_API_SUPPORTED
void AccessScheduleConfigDebug();

#endif	//MXCONFIGLOG_H



















