#if !defined MXACMSCONFIG_H
#define MXACMSCONFIG_H
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
//   Created By   : Bharat Gohil
//   File         : MxACMSConfig.h
//   Description  : This is Header File for MxACMSConfig.c
//
/////////////////////////////////////////////////////////////////////////////

//******** Place all include files ***
#include "MxTypedef.h"
#include "MxSysCommonDef.h"

//******** Defines and Data Types ****

//******** Function Prototypes *******
UINT16 AcmsParseConfigIdx(CHARPTR * rxData);
BOOL GetTimeFromString(CHARPTR timeStr, TIME_HHMM_t * time);

BOOL SetAcmsUnknownConfigFields(CHARPTR * rxData);
BOOL SetAcmsUserConfigFields(CHARPTR * rxData);
BOOL SetAcmsHolidaySchedule1ConfigFields(CHARPTR * rxData);
BOOL SetAcmsHolidaySchedule2ConfigFields(CHARPTR * rxData);
BOOL SetAcmsHolidaySchedule3ConfigFields(CHARPTR * rxData);
BOOL SetAcmsHolidaySchedule4ConfigFields(CHARPTR * rxData);
BOOL SetAcmsTwoPersonConfigFields(CHARPTR * rxData);
BOOL SetAcmsIoLinkConfigFields(CHARPTR * rxData);
BOOL SetAcmsFirstInUserConfigFields(CHARPTR * rxData);
BOOL SetAcmsGenSysParaBasicConfigFields(CHARPTR * rxData);
BOOL SetAcmsGenSysParaAdvConfigFields(CHARPTR * rxData);
BOOL SetAcmsNetworkParaConfigFields(CHARPTR * rxData);
BOOL SetAcmsDstConfigFields(CHARPTR * rxData);
BOOL SetAcmsSplFunctionConfigFields(CHARPTR * rxData);
BOOL SetAcmsVisitorMgmtConfigFields(CHARPTR * rxData);
BOOL SetAcmsTimeTriggerConfigFields(CHARPTR * rxData);
BOOL SetAcmsCafeItemConfigFields(CHARPTR * rxData);
BOOL SetAcmsCafeMenuConfigFields(CHARPTR * rxData);
BOOL SetAcmsCafeMenuSchConfigFields(CHARPTR * rxData);
BOOL SetAcmsPrinterConfigFields(CHARPTR * rxData);
BOOL SetAcmsTimeStampConfigFields(CHARPTR * rxData);
BOOL SetAcmsGreetingMsgConfigFields(CHARPTR * rxData);
BOOL SetAcmsFprConfigFields(CHARPTR * rxData);
BOOL SetAcmsDvrEvtActnConfigFields(CHARPTR * rxData);
BOOL SetAcmsSmartRouteAccessConfigFields(CHARPTR * rxData);
BOOL SetAcmsIoLinkExtDeviceConfigFields(CHARPTR * rxData);
BOOL SetAcmsIoLinkExtDeviceMappingFields(CHARPTR * rxData);
BOOL SetAcmsExtCardFormatConfigFields(CHARPTR * rxData);
BOOL SetAcmsIntCardFormatConfigFields(CHARPTR * rxData);
BOOL SetAcmsVamConfigFields(CHARPTR * rxData);
BOOL SetAcmsJcmConfigFields(CHARPTR * rxData);
BOOL SetAcmsMultiLanConfigFields(CHARPTR * rxData);
BOOL SetAcmsDoorModeSchConfigFields(CHARPTR * rxData);
BOOL SetAcmsBleConfig(CHARPTR * rxData);
BOOL SetAcmsIpCameraConfig(CHARPTR * rxData);
BOOL SetAcmsIpCameraAdvanceConfig(CHARPTR * rxData);
BOOL SetAcmsBackupUpdateConfigFields(CHARPTR * rxData);
BOOL SetAcmsFaceRecogntionConfigFields(CHARPTR * rxData);
BOOL SetAcmsReaderBleConfig(CHARPTR * rxData);
BOOL ParseHttpUrlData(const CHARPTR const urlStrPtr, CHARPTR const ipOrHostName, CHARPTR const directoryName,UINT16PTR port);
BOOL SetAcmsAccessScheduleConfigFields(CHARPTR * rxData);
#endif  // MXACMSCONFIG_H
