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
//   File         : MxSysDefault.h
//   Description  : This file declares system utility API.
//
/////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "MxTypedef.h"
#include "MxLogs.h"
#include "MxSysTimer.h"
#include "MxSysDefault.h"
#include "MxAvResp.h"
#include "MxConfig.h"
#include "MxConfigFileIo.h"
#include "MxDcTaskFeatures.h"
#include "MxReaderUtils.h"
#include "MxSysTime.h"
#include "MxMenu.h"

#ifdef OEM_SUPPORTED
#include "OEM.h"
#endif //#ifdef OEM_SUPPORTED

//******** Extern Variables **********

//******** Defines and Data Types ****
#define SYS_DEFAULT_FLAG_FILE			"/matrix/applData/MxSysDfltFlag"

//******** Function Prototypes *******
static void PreDefaultProc(SYS_DFLT_TYPE_e sysDfltType);

//******** Global Variables **********

//******** Static Variables **********
static 	SYS_DFLT_TYPE_e	sysDfltType = SYS_DFLT_FALSE;

//******** Function Definations ******

//*****************************************************************************
//	DefaultSystem
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It defaults entire system including event log.
//			It only excludes network parameters and password.
//				
//*****************************************************************************
void DefaultSystem(SYS_DFLT_TYPE_e sysDfltType)
{
	switch(sysDfltType)
	{
	case SYS_DFLT_BASIC_WITH_DOOR_MODE_CHNG:
		// This will default all configuaration data, all Palm tamplates.
		// It only excludes network parameters and password.
		LoadDfltSysConfig(SYS_DFLT_BASIC_WITH_DOOR_MODE_CHNG);
		DfltConfForDoorModeChng();
		break;

	case SYS_DFLT_BASIC:
		// This will default all configuaration data, all Palm tamplates.
		// It only excludes network parameters and password.
		LoadDfltSysConfig(SYS_DFLT_BASIC);
		break;

	case SYS_DFLT_ALL:
		//This will default all the configuration available in system.
        ResetDispPwdAttemptData(MAX_USER_TYPE);
		DfltAllConfig();
		break;

	default:
		return;
	}
	// Remove Event Log File
	DeleteAllEventLogs();
	// Clear Absentee record
	DeleteAbsenteeRecord();
	// Now Init Absentee Rule
	InitAbsenteeRule();
}

//*****************************************************************************
//	CheckSysDefltReq
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It checks if system default required. if so, it defaults system 
//			configuration.
// NOTE: It doesn't default N/W and password configuration.
//*****************************************************************************
void CheckSysDefltReq(void)
{
	SYSTEM_DEFAULT_STATUS_e systemDefaultStatus = SYSTEM_DEFAULT_STATUS_MAX;
	
	// Check if entire system needs to be defaulted
	if(ReadDfltSysFlag(&sysDfltType) == FAIL)
	{
		sysDfltType = SYS_DFLT_FALSE;
	}
	if( (sysDfltType == SYS_DFLT_BASIC) || (sysDfltType == SYS_DFLT_BASIC_WITH_DOOR_MODE_CHNG) || (sysDfltType == SYS_DFLT_ALL) )
	{
        	//Below two apis are called to update device firmware type and door connectivity status at QT side which is required for system default
        	SendMessageToQt(" ", GUI_MSG_STARTUP_INFO);
        	UpdateDisplayStatusField();

		systemDefaultStatus = SYSTEM_DEFAULT_START;
		DPRINT(SYS_LOG,"STARTED process to default System Configuration");
		SetAudioVisual(USER_RESP_SYSTEM_DEFAULT, INTERNAL_PORT, &systemDefaultStatus);

		// Play cadance on status LED.
		//	SetLed(ACCESS_PROCESS_RESP, INTERNAL_PORT);

		//if finger reader is allready configured then delete fingerprint from reader.
		//DefltAllFpr function must be called before Defaultsystem function because to default we need to check
		// if previously finger reader is configured or not.
		DefltAllFpr();
		// Default system configuration including user finger templates and event log.
		// NOTE: It doesn't default network and password configuration.
		DefaultSystem(sysDfltType);
		WriteEventLog(SYSTEM_DEFAULTED, UNUSED_FIELD, UNUSED_FIELD, SOURCE_ACMS_EV, UNUSED_FIELD, UNUSED_FIELD);
		

		systemDefaultStatus = SYSTEM_DEFAULT_COMPLETE;
		DPRINT(SYS_LOG,"Master System Configuration Default SUCCESS");
		SetAudioVisual(USER_RESP_SYSTEM_DEFAULT, INTERNAL_PORT, &systemDefaultStatus);

		//Update time-date in case time zone configuation may have been changed
		UpdateSysDateTime();

		// Play cadance on BUZZER
		SetBuzzer(ACCESS_CONT_ON_RESP, INTERNAL_PORT);
		//Sleep for 1 mSec
		usleep(1000000);
		// Stop cadance on status LED and make it ON
		// SetLed(ACCESS_CONT_ON_RESP, INTERNAL_PORT);
		// Stop cadance on BUZZER
		SetBuzzer(ACCESS_CONT_OFF_RESP, INTERNAL_PORT);
	}
}

//*****************************************************************************
//	WriteDfltSysFlag
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It Sets flag used to default system.
//
//*****************************************************************************
BOOL WriteDfltSysFlag(SYS_DFLT_TYPE_e sysDfltType)
{
	INT32 	fd;

	fd = open(SYS_DEFAULT_FLAG_FILE, O_RDWR | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open System default flag file");
		return FAIL;
	}

	if(sizeof(sysDfltType) != write(fd, &sysDfltType, sizeof(sysDfltType)) )
	{
		EPRINT(SYS_LOG, "Failed to write System default flag file");
		close(fd);
		return FAIL;
	}

	PreDefaultProc(sysDfltType);
	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	ReadDfltSysFlag
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It Sets flag used to default system.
//
//*****************************************************************************
BOOL ReadDfltSysFlag(SYS_DFLT_TYPE_e * sysDfltTypePtr)
{
	INT32 	fd;

	fd = open(SYS_DEFAULT_FLAG_FILE, O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		return FAIL;
	}

	if(sizeof(SYS_DFLT_TYPE_e) != read(fd, sysDfltTypePtr, sizeof(SYS_DFLT_TYPE_e) ) )
	{
		EPRINT(SYS_LOG, "Failed to read System default flag file");
		close(fd);
		return FAIL;
	}

	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	PreDefaultProc
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			This function does pre default procedure required according to default
//		request type.
//*****************************************************************************
static void PreDefaultProc(SYS_DFLT_TYPE_e sysDfltType)
{
	switch(sysDfltType)
	{
	case SYS_DFLT_BASIC_WITH_DOOR_MODE_CHNG:
	case SYS_DFLT_BASIC:
	case SYS_DFLT_ALL:
		unlink(WEB_SRV_ROUTE_FILE);
		break;

	default:
		return;
	}
}

//*****************************************************************************
//	GetSysDefaultType
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			This function dreturn System Default Type
//		request type.
//*****************************************************************************
SYS_DFLT_TYPE_e GetSysDefaultType(VOID)
{
	return sysDfltType;
}
