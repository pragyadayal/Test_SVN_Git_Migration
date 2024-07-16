#ifndef _ETNEV3_WATCHDOG_H_
#define _ETNEV3_WATCHDOG_H_

// ///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Telecom Pvt. Ltd., Baroda, India.
//   Project      : Eternity - NEV3
//   Created By   : santosh rai.
//   File         : watchdog.h
//   Dated        : 17-07-2015
//
//   Copyright (C) 2014 Matrix ComSec Pvt. Ltd., India.
// 
// ///////////////////////////////////////////////////////////////////////////

/****************************************************************
 * DRIVER TUNING PARAMETER    
 ****************************************************************/

#define WATCHDOG_DEVICE_NAME             "/dev/watchdog"

/****************************************************************
 * IOCTLs
 ****************************************************************/
/* NOTE: current timeout set in driver is 60 seconds so call ioctl before 60 sec */ 

#define WATCHDOG_IOCTL_MAGIC	'W'

#define IOCTL_WATCHDOG_TOGGLE			_IO(WATCHDOG_IOCTL_MAGIC, 1)

#endif

