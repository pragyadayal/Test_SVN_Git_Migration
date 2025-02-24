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
//   File         : MxFpCalib.c
//   Description  : This file declares system utility API.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include <fcntl.h>
#include <unistd.h>

#include "MxTypedef.h"
#include "MxLogs.h"
#include "MxFpCalib.h"

//******** Extern Variables **********

//******** Defines and Data Types ****
#define CALIBRATE_SFPM_SIGN_FILE		"/matrix/applData/MxCalibSfpm"

//******** Function Prototypes *******

//******** Global Variables **********

//******** Static Variables **********

//******** Function Definations ******

//*****************************************************************************
//	WriteCalibrateSfprSign
//	Param:
//		IN :	calibSensorSign
//		OUT:	none
//	Returns:	none
//	Description:
//			It saves calibration signature into file.
//
//*****************************************************************************
BOOL WriteCalibrateSfprSign(UINT16 calibSensorSign)
{
	INT32 	fd;

	fd = open(CALIBRATE_SFPM_SIGN_FILE, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		EPRINT(SYS_LOG, "Failed to open calibrate sfpr sign file");
		return FAIL;
	}

	if(sizeof(UINT16) != write(fd, &calibSensorSign, sizeof(UINT16)) )
	{
		EPRINT(SYS_LOG, "Failed to write calibrate sfpr sign file");
		close(fd);
		return FAIL;
	}

	close(fd);
	return SUCCESS;
}

//*****************************************************************************
//	ReadCalibrateSfprSign
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It reads calibration signature from file.
//
//*****************************************************************************
BOOL ReadCalibrateSfprSign(UINT16 * calibSensorSign)
{
	INT32 	fd;

	fd = open(CALIBRATE_SFPM_SIGN_FILE, O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0)
	{
		return FAIL;
	}

	if(sizeof(UINT16) != read(fd, calibSensorSign, sizeof(UINT16)) )
	{
		EPRINT(SYS_LOG, "Failed to read calibrate sfpr sign file");
		close(fd);
		return FAIL;
	}

	close(fd);
	return SUCCESS;
}


