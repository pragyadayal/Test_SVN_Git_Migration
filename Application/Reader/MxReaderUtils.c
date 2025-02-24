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
//   File         : MxReaderUtils.c
//   Description  : This file implements API layer of reader task to communicate
//					with diffrent reader drivers
//
/////////////////////////////////////////////////////////////////////////////

//******** Include files **********

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "MxTypedef.h"
#include "MxSysTimer.h"
#include "MxConfig.h"
#include "MxSlaveCommonMsgList.h"
#include "MxReaderRxTx.h"
#include "MxSupremaReader.h"
#include "MxReaderUtils.h"
#include "MxReaderTask.h"
#include "MxDelay.h"
#include "MxSlaveMsg.h"
#include "MxDcTask.h"
#include "MxSysUtils.h"
#include "MxCerTask.h"
#include "MxLogs.h"
#include "MxFpCalib.h"
#include "MxReaderCommonUtils.h"
#include "MxCbBluetooth.h"

//******** Extern Variables **********
extern FPR_FW_UPGRADE_STATUS_t			FprFwUpgrade;
extern READERS_PARAMETER_t				ReadersParaConfig;
extern FPR_SENSOR_TYPE_e				SfprSensorType[];
extern SYS_BASIC_PARA_t 	 			SysBasicPara;

//****** Defines and Data Types ******
// Signature to calibrate sensor
#define CALIBRATE_SENSOR_SIGN_VALUE 0x2222

//******** Function Prototypes *******
static void SfprFwUpgradeTimeout(void);

//******** Global Variables **********

//******** Static Variables **********
static 	TIMER_HANDLE			sfprFwTmrHandle = INVALID_TIMER_HANDLE;

//******** Function Definations ******

//*****************************************************************************
//	DefltAllFpr()
//	Param:
//		IN :	none
//		OUT:	none
//	Returns:	none
//	Description:
//			It deletes all templates from finger print module and disables free
//			scan mode.
//
//*****************************************************************************
void DefltAllFpr(void)
{
	READER_ID_e readerId;

	// This API only can be called in default process during initilization
	InitReaderTask();

	// Now search through reader configuration
	for(readerId = 0; readerId < MAX_READERS; readerId++)
	{
		// If Suprema finger print reader
		if(GetReaderTypeCfg(readerId) == SUP_FP_3030_READER)
		{
			// Send comand to default finger print templates from reader
			DefltSuprema3030(readerId);
			// Send comand to disable free scan on finger print reader
			SendSuprema3030DisFreeScanMsg(readerId);
		}
	}
}

//*****************************************************************************
//	CheckAndDisableFreeScanOnFpr()
//	Param:
//		IN :	readerId - on which free scan is to be disabled
//		OUT:	none
//	Returns:	none
//	Description:
//			It disables free scan running on finger print reader.
//
//*****************************************************************************
void CheckAndDisableFreeScanOnFpr(UINT8 readerId)
{
	// If Suprema finger print reader
	if(GetReaderTypeCfg(readerId) == SUP_FP_3030_READER)
	{
		SendSuprema3030DisFreeScanMsg(readerId);
	}
}

//*****************************************************************************
//	SetFprPara()
//	Param:
//		IN : 	none
//		OUT: 	none
//	Returns:	none
//	Description:
//				It inserts given message to set given parameter on all finger
//				print readers.
//*****************************************************************************
void SetFprPara(void)
{
	SetSupremaFprPara();
}

//*****************************************************************************
//	CalibrateFpSensor()
//	Param:
//		IN :	dummy
//		OUT:	None
//	Returns:	SUCCESS
//	Description:
//		It writes 'calibrate sensor signature' and then reboots the system.
// 		After power on, flag will be checked and sensor will be calibrated
//*****************************************************************************
UINT8 CalibrateFpSensor(UNUSED_PARA CHARPTR * dummy)
{
	UINT16 calibrateSign = CALIBRATE_SENSOR_SIGN_VALUE;

	WriteCalibrateSfprSign(calibrateSign);
	// Reboot now
	SetSysRebootTimer(SYS_REBOOT_TMR_CNT);
	return SUCCESS;
}

//*****************************************************************************
//	IsSensorCalibrationRequire()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	YES/NO
//	Description:
//		It reads and checks if 'calibrate sensor signature' is written or not
//*****************************************************************************
BOOL IsSensorCalibrationRequire(void)
{
	UINT16 calibrateSign;

	ReadCalibrateSfprSign(&calibrateSign);
	if(calibrateSign == CALIBRATE_SENSOR_SIGN_VALUE)
	{
		calibrateSign = 0xFFFF;
		WriteCalibrateSfprSign(calibrateSign);
		return TRUE;
	}
	return FALSE;
}

//*****************************************************************************
//	StartFprFwUpgradeCmd()
//	Param:
//		IN :	readerId
//		OUT:	None
//	Returns:	None
//	Description:
//		It sends command to EM prox reader to set RADF value.
//		Parameter value will be used from readers parameter table.
//*****************************************************************************
void StartFprFwUpgradeCmd(READER_ID_e readerId)
{
	SFPR_FW_UPGRADE_START_CMD_DATA_t	cmdData;
	struct stat							buf;

	if(SfprSensorType[readerId] == OPTICAL_SENSOR)
	{
		sprintf(FprFwUpgrade.fileName, "%s%s%s", ZIP_PATH, SFPR3030_BIN_FILE, SFPR_BIN_FILE_EXT);
	}
	else if(SfprSensorType[readerId] == CAPACITIVE_SENSOR)
	{
		sprintf(FprFwUpgrade.fileName, "%s%s%s", ZIP_PATH, SFPR3050_BIN_FILE, SFPR_BIN_FILE_EXT);
	}
	else
	{
		return;
	}

	if(stat(FprFwUpgrade.fileName, &buf) == 0)
	{
		FprFwUpgrade.firmwareSize = (UINT32)buf.st_size;
	}
	else
	{
		return;
	}

	FprFwUpgrade.noOfPacket = 1 + (FprFwUpgrade.firmwareSize / SFPR_FW_DATA_PACKET_SIZE);;
	cmdData.readerId = readerId;
	cmdData.size	= FprFwUpgrade.firmwareSize;
	ProcessReaderCommand(MSG_SFPR_FW_UPGRADE_START_CMD, &cmdData, sizeof(SFPR_FW_UPGRADE_START_CMD_DATA_t) );
	SetSfprFwUpgradeTimer(FPR_FW_UPGRADE_TIMEOUT);
}

//*****************************************************************************
//	SetSfprFwUpgradeTimer
//	Param:
//		IN :	timerValue	- timer value in multiple of 100 msec
//		OUT:	none
//	Returns:	SUCCESS/FAIL
//	Description:
//				This function starts Suprema firmware upgrade timer
//
//*****************************************************************************
BOOL SetSfprFwUpgradeTimer(UINT16 timerValue)
{
	if(sfprFwTmrHandle== INVALID_TIMER_HANDLE)
	{
		TIMER_INFO_t tInfo;

		tInfo.count = timerValue;
		tInfo.data = 0;
		tInfo.funcPtr = (VOIDPTR)&SfprFwUpgradeTimeout;
		return (StartTimer(tInfo, &sfprFwTmrHandle, __FUNCTION__));
	}
	else //reload timer
	{
		return (ReloadTimer(sfprFwTmrHandle, timerValue));
	}
}

//*****************************************************************************
//	SfprFwUpgradeTimeout()
//	Param:
//		IN :	readerId- reader id for which response timeout occured
//		OUT:	none
//	Returns:	none
//	Description:
//				Suprema finger reader firmware upgrade timeout
//
//*****************************************************************************
static void SfprFwUpgradeTimeout(void)
{
	// Delete timer used for reader response
	DeleteTimer(&sfprFwTmrHandle);
	SwitchDcToIdleState();
}

//*****************************************************************************
//	ReadSfprPara()
//	Param:
//		IN : paraId - Parameter Id
//		OUT: NONE
//	Returns: NONE
//	Description:
//			It Reads Suprema Finger readers Parameter value
//*****************************************************************************
void ReadSfprPara(UINT8 paraId)
{
	SET_FPR_PARA_CMD_t	readParaCmd;
	READER_ID_e			readerId;

	readParaCmd.paraId = paraId;

	for(readerId = 0; readerId < MAX_READERS; readerId++)
	{
		if(GetReaderTypeCfg(readerId) == SUP_FP_3030_READER)
		{
			readParaCmd.readerId = readerId;
			ProcessReaderCommand(MSG_READ_SFPR_PARA_CMD, &readParaCmd, sizeof(SET_FPR_PARA_CMD_t) );
		}
	}
}

//*****************************************************************************
//	ChangeFingerPrintReaderSettings()
//	Param:
//		IN :	commState
//		OUT:	none
//	Returns:	none
//	Description:
//				If state is OFFLINE, it will abort finger scanning on entry
//				finger print reader.
//				If state is DEGRADE, it changes finger print reader scanning time
//				as per degarde mode requirement.
//				If state is ONLINE, it changes finger print reader scanning time
//				as per nomral mode requirement.
//
//*****************************************************************************
void ChangeFingerPrintReaderSettings(COMM_STATE_e commState)
{
	UINT8 								readerId;
	SET_FP_SCAN_WAIT_TIME_CMD_DATA_t	setScanTime;
	ABORT_FINGER_SCAN_CMD_DATA_t 		abortFingerScan;
	ENABLE_FREE_SCAN_CMD_DATA_t 		enableFreeScan;

	for(readerId=0; readerId < MAX_READERS; readerId++)
	{
		if(GetReaderTypeCfg(readerId) == SUP_FP_3030_READER)
		{
			switch(commState)
			{
				case SLAVE_ONLINE:
				{
					//Send message to set scanning timeout for finger print reader
					setScanTime.readerId = readerId;
					setScanTime.timeoutValue = IDLE_FP_SCAN_TIMER;
					ProcessReaderCommand(MSG_SET_FP_SCAN_WAIT_TIME_CMD,
								(UINT8PTR)&setScanTime,
									sizeof(SET_FP_SCAN_WAIT_TIME_CMD_DATA_t));
					break;
				}

				case SLAVE_OFFLINE:
				{
					//Send message to cancel scanning on finger print reader
					abortFingerScan.readerId = readerId;
					ProcessReaderCommand(MSG_ABORT_FINGER_SCAN_CMD,
								(UINT8PTR)&abortFingerScan,
									sizeof(ABORT_FINGER_SCAN_CMD_DATA_t));

					//Send message to enable free scanning on finger print reader
					enableFreeScan.readerId = readerId;
					ProcessReaderCommand(MSG_ENABLE_FREE_SCAN_CMD,
								(UINT8PTR)&enableFreeScan,
									sizeof(ENABLE_FREE_SCAN_CMD_DATA_t));
					break;
				}

				case SLAVE_DEGRADE:
				{
					//Send message to set scanning timeout for finger print reader
					//For Advance Degrade Mode set as online door
					if(ADVANCE_DEGRADE_MODE_TYPE == SysBasicPara.degradeModeType)
					{
						DPRINT(READER_TASK_LOG,"FP Scan Wait Timer Set Infinite");
						setScanTime.readerId = readerId;
						setScanTime.timeoutValue = IDLE_FP_SCAN_TIMER;
						ProcessReaderCommand(MSG_SET_FP_SCAN_WAIT_TIME_CMD,
									(UINT8PTR)&setScanTime,
										sizeof(SET_FP_SCAN_WAIT_TIME_CMD_DATA_t));
					}
					else if(BASIC_DEGRADE_MODE_TYPE == SysBasicPara.degradeModeType)
					{
						DPRINT(READER_TASK_LOG,"FP Scan Wait Timer Set for Degrade Basic");
						setScanTime.readerId = readerId;
						setScanTime.timeoutValue = DEGRADE_FP_SCAN_TIMER;
						ProcessReaderCommand(MSG_SET_FP_SCAN_WAIT_TIME_CMD,
									(UINT8PTR)&setScanTime,
										sizeof(SET_FP_SCAN_WAIT_TIME_CMD_DATA_t));
					}	
					break;
				}

				default:
				{
					break;
				}
			} // end of switch case - comm state
		}	// end of 'if reader is finger print'
	}	// loop for no. of readers
}

//*****************************************************************************
//	SetExternalReaderBlePara()
//	Param:
//		IN : 	none
//		OUT: 	none
//	Returns:	none
//	Description:
//				It sets and then sends the config for Bluetooth for Cb reader
//
//*****************************************************************************
void SetCbBlePara()
{
	SetExtRdrBluetoothPara(READER4);
}
