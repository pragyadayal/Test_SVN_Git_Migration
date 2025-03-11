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
//   File         : MxWatchDog.c
//   Description  : This file declares system utility API.
//
/////////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>

#include "MxTypedef.h"	
#include "MxLogs.h"
#include "MxSysTimer.h"
#include "MxWatchDog.h"

//******** Extern Variables **********

//******** Defines and Data Types ****
#define WATCHDOG_FILE					"/dev/watchdog"
#define	WATCH_DOG_TIMEOUT				30			// in second
#define WATCH_DOG_KICK_TMR_CNT			10000		//in mili second ( (WATCH_DOG_TIMEOUT * 1000)/3 )

//******** Function Prototypes *******

//******** Global Variables **********

//******** Static Variables **********
static INT32			wdtFd = -1;
static UINT32			wdtTick = 0;
static TIMER_HANDLE		watchDogKickTmrHandle = INVALID_TIMER_HANDLE;

//******** Function Definations ******

//***************************************************************************
//	StartWatchDog()
//	Param:
//		IN :	kickTime
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//			This function start watch dog timer and start kicking of watch dog.
//
//***************************************************************************
void StartWatchDog(void)
{
	TIMER_INFO_t 	tInfo;
	int 			timeout;

    wdtFd = open(WATCHDOG_FILE, O_WRONLY);
	if(wdtFd < 0)
	{
		DPRINT(SYS_LOG, "Fail to start watch dog timer");
		return;
	}

	timeout = WATCH_DOG_TIMEOUT;
	ioctl(wdtFd, WDIOC_SETTIMEOUT, &timeout);
	DPRINT(SYS_LOG, "watch dog time out is set to:%d seconds", timeout);

	tInfo.count = (WATCH_DOG_TIMEOUT * 10)/3;
	tInfo.data = 1;
	tInfo.funcPtr = &KickWatchDog;

	if(StartTimer(tInfo, &watchDogKickTmrHandle, __FUNCTION__) == FAIL)
	{
		EPRINT(GENERAL_LOG,"Unable to start watchdog kick timer");
	}

	wdtTick = GetSysTick();
}

//***************************************************************************
//	KickWatchDog()
//	Param:
//		IN :	dummy
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//			This function start kick watch dog timer.
//
//***************************************************************************
void KickWatchDog(UINT32 dummy)
{
	// (WATCH_DOG_TIMEOUT * 1000)/3
	if( (ElapsedTick(wdtTick) > WATCH_DOG_KICK_TMR_CNT) || (dummy == 1) )
	{
		wdtTick = GetSysTick();
		ioctl(wdtFd, WDIOC_KEEPALIVE, &dummy);
	}
}
