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
//   File         : MxSysReboot.c
//   Description  : This file declares system utility API.
//
/////////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "MxTypedef.h"	
#include "MxLogs.h"
#include "MxSysTimer.h"
#include "MxSysReboot.h"
#include "MxAcmsTask.h"

//******** Extern Variables **********

//******** Defines and Data Types ****

//******** Function Prototypes *******

//******** Global Variables **********
TIMER_HANDLE			sysRebootTmrHandle = INVALID_TIMER_HANDLE;

//******** Static Variables **********
static BOOL				enableSysRebootF = TRUE;

//******** Function Definations ******

//*****************************************************************************
//	RebootSystem
//	Param: 
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It reboots the system.
//				
//*****************************************************************************
void RebootSystem(UNUSED_PARA UINT32 dummy)
{
	DeleteTimer(&sysRebootTmrHandle);
	if(enableSysRebootF == TRUE)
	{
		DPRINT(SYS_LOG, "System is rebooting now...");
		//Flush file system caches to disk		
		system("/bin/sync");
		DPRINT(SYS_LOG, "Sync Completed Successfully.");
		sleep(2);
		//Now safely Rebooting system
		system("/sbin/reboot");
		LogoutAcms(ACMS_COMM_HOLD, DO_NOT_CLOSE_ALL_SERVER_SOCKETS);
	}
}

//*****************************************************************************
//	EnableSystemReboot
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It enable/disable system reboot functionality.
//
//*****************************************************************************
void EnableSystemReboot(BOOL enable)
{
	enableSysRebootF = enable;
}

//*****************************************************************************
//	SetSysRebootTimer
//	Param: 
//		IN :	NONE
//		OUT:	NONE
//	Returns:	SUCCESS/FAIL
//	Description:
//			This function will Start timer to reboot system.
//				
//****************************************************************************
BOOL SetSysRebootTimer(UINT32 timeCount)
{
	TIMER_INFO_t timer;
    
	if(sysRebootTmrHandle == INVALID_TIMER_HANDLE)
	{
		timer.count   = timeCount;
		timer.data 	  = 0; 
		timer.funcPtr = &RebootSystem;    
        
		return(StartTimer(timer, &sysRebootTmrHandle, __FUNCTION__) );
	}	
	else
	{
		return(FAIL);
	}
}


