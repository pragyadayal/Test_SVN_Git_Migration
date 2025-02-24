/////////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix Telecom Pvt. Ltd., Baroda, India.
//   Project      : ACS (Access Control System) - Master Controller
//   Created By   : Kunjal Garach
//   File         : MxGpio.c
//   Description  : This file implements GPIO related driver functions.
//
/////////////////////////////////////////////////////////////////////////////

//*** Place all include files here ***
//System Included files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

//Matrix included file
#include "MxTypedef.h"
#include "MxGpio.h"
#include "MxSysTimer.h"
#include "MxConfig.h"
#include "MxEventLog.h"
#include "MxLogs.h"
#include "MxDcTask.h"
#include "MxCerTask.h"
#include "MxSlaveMsg.h"
#include "MxReaderUtils.h"
#include "MxGpioDrv.h"
#include "MxReaderCommonUtils.h"

//******** Extern Variables **********
extern SYS_ADVANCE_PARA_t	SysAdvPara;
extern SYS_BASIC_PARA_t		SysBasicPara;
extern DOOR_ACTIVITY_INFO_t	DcInfo;
extern DOOR_MONITOR_t		DcMonitorControl;

extern BUZZ_LED_CAD_CONFIG_t	BuzzLedCadPara[];
extern IO_STATUS_e		CerTamperStatus;

//****** Defines and Data Types ******
#define	GPIO_SCAN_TMR			1	//in 100 ms resolution

#define AUX_IN_STATUS_FILE			"/sys/bus/iio/devices/iio\:device0/in_voltage1_raw"
#define DOOR_SENSE_STATUS_FILE		"/sys/bus/iio/devices/iio\:device0/in_voltage0_raw"
#define	BACKLIGHT_BRIGHNESS_FILE 	"/sys/class/backlight/pwm-backlight/brightness"
#define ADC_BUFFER_SIZE				5

// This is Direct 12 bit ADC Count that is 0 to 4096 in integer.
// External reference voltage is 1.8V, so resolution is 1.8V/4096 = 0.44mv
// Max Detected Voltage is 1.58V

//Received Values :
// SHORT 	: 8
// INACTIVE : 1680 (5k Load)
// ACTIVE   : 2150 (10k Load)
// OPEN		: 2970

// TOD0:- Set Proper Limits
#define SHORT_MAX				100			// 930
#define CLOSE_MAX				1825		// 2147
#define OPEN_MAX				2565		// 3070

//******** Function Prototypes *******
static void GetIOStatus(void);
static void RunGpioTask(UINT32 dummy);
static BOOL GetCurrOutputStatus(OUTPUT_STATUS_t *outStatus);
static void StartOutputWaitTimer(UINT32 timerCnt);
static void OutputWaitTimeout(UINT32 data);

//******** Global Variables **********
INPUT_STATUS_t	SysInputStatus;
OUTPUT_STATUS_t	SysOutputStatus;
GPIO_STATUS_t	GpioStatus;
IO_STATUS_e	WiegandTamperStatus;

//******** Static Variables **********
static	INT32			gpioDrvFd = -1;
static	INT32			backlightFd = -1;
static  TIMER_HANDLE	gpioScanTmrHandle = INVALID_TIMER_HANDLE;
static  TIMER_HANDLE	outputWaitTmrHandle = INVALID_TIMER_HANDLE;
// Aux input status without considering debounce time
// (last saved status derived after applying suprevision and sense type configuration)
static IO_STATUS_e		auxCurIoStatus = MAX_IO_STATUS;
static IO_STATUS_e		doorSenseCurIoStatus = MAX_IO_STATUS;

CHARPTR IoStatus[]=
{
	"INACTIVE",
	"ACTIVE",
	"SHORT",
	"OPEN",
	"UNKONWN",
};

// access reaponse conversion table for CER
static UINT8  acRespConvForCer[ACCESS_CONTROL_RESP_MAX] =
{
	CER_ACCESS_IDLE_RESP,			//ACCESS_IDLE_RESP,
	CER_ACCESS_PROCESS_RESP,		//ACCESS_PROCESS_RESP,
	CER_ACCESS_WAIT_RESP,			//ACCESS_WAIT_RESP,
	CER_ACCESS_SUCCESS_RESP,		//ACCESS_ALLOWED_RESP,
	CER_ACCESS_FAIL_RESP,			//ACCESS_DENIED_RESP,
	CER_ACCESS_FAIL_RESP,			//ACCESS_INVALID_RESP,
	CER_ACCESS_ALARM_MINOR_RESP,	//ACCESS_MINOR_ALARM_RESP,
	CER_ACCESS_ALARM_MAJOR_RESP,	//ACCESS_MAJOR_ALARM_RESP,
	CER_ACCESS_ALARM_CRITICAL_RESP,	//ACCESS_CRITICAL_ALARM_RESP,
	CER_ACCESS_ALARM_CLEAR_RESP,	//ACCESS_CLEAR_ALARM_RESP,
	CER_ACCESS_OFFLINE_RESP,		//ACCESS_OFFLINE_RESP,
	CER_ACCESS_ONLINE_RESP,			//ACCESS_ONLINE_RESP,
	CER_ACCESS_DEGRADE_RESP,		//ACCESS_DEGRADE_RESP,
	CER_ACCESS_SUCCESS_RESP,		//ACCESS_ENROLL_SUCCESS_RESP,
	CER_ACCESS_FAIL_RESP,			//ACCESS_ENROLL_FAIL_RESP,
	CER_ACCESS_FAIL_RESP,			//ACCESS_TIMEOUT_RESP,
	CER_ACCESS_NO_KEEP_ALIVE_RESP,	//ACCESS_FP_SYNC_RESP,
	CER_ACCESS_TEST_BUZLED_RESP,	//ACCESS_TEST_BUZLEDS_RESP,
	CER_ACCESS_IDLE_RESP,			//ACCESS_CONT_OFF_RESP,
	CER_ACCESS_IDLE_RESP,			//ACCESS_CONT_ON_RESP,
	CER_ACCESS_SUCCESS_RESP,		//ACCESS_SUCCESS_RESP,
	CER_ACCESS_FAIL_RESP,			//ACCESS_FAIL_RESP,
};

//******** Function Definations ******

//***************************************************************************
//	InitGpioDriver()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function initialises GPIO module by opening GPIO Driver
//				and initialise its parameter.
//
//***************************************************************************
void InitGpioDriver(void)
{
    gpioDrvFd = open(GPIO_DEVICE_NAME, O_RDWR);
	if(gpioDrvFd < 0)
	{
		// Fail to open Driver
		EPRINT(SYS_LOG,"Application Failed to Open GpioDriver");
	}

	// Initialize all inputs to unknown state
	SysInputStatus.sysTamper = INPUT_UNKNOWN;
	SysInputStatus.rdrTamper = INPUT_UNKNOWN;
	SysInputStatus.lockTamper = INPUT_UNKNOWN;
	SysInputStatus.doorSense = INPUT_UNKNOWN;
	SysInputStatus.auxIn = INPUT_UNKNOWN;
	SysInputStatus.exitSw = INPUT_UNKNOWN;
	CerTamperStatus = IO_INACTIVE;
	WiegandTamperStatus = IO_INACTIVE;

	// Play Buzzer & led
	SetBuzzer(ACCESS_CONT_ON_RESP, INT_EXT_PORT);
	SetLed(ACCESS_CONT_ON_RESP, INT_EXT_PORT);
	//Sleep for 1 second
	sleep(1);
	//OFF buzzers
	SetBuzzer(ACCESS_CONT_OFF_RESP, INT_EXT_PORT);
	// Off internal and external LEDs
	SetLed(ACCESS_CONT_OFF_RESP, INT_EXT_PORT);
}

//***************************************************************************
//	ActivateAuxRelay()
//	Param:
//		IN :	timerCnt 			- Time in Seconds
//				OP_PORT_LATCH_TIME	- for latching
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				It activates AUX relay.
//
//***************************************************************************
void ActivateAuxRelay(UNUSED_PARA UINT16 timerCnt)
{
	DPRINT(SYS_LOG,"Aux Relay is Not Supported in ARGO DOOR");
}


//***************************************************************************
//	DeactivateAuxRelay()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				It deactivates AUX relay.
//
//***************************************************************************
void DeactivateAuxRelay(void)
{
	DPRINT(SYS_LOG,"Aux Relay is Not Supported in ARGO DOOR");
}

//***************************************************************************
//	ActivateDoorRelay()
//	Param:
//		IN :	timerCnt 			- Time in Seconds
//				OP_PORT_LATCH_TIME	- for latching
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				It activates Door Lock relay.
//
//***************************************************************************
BOOL ActivateDoorRelay(FLOAT32 timerCnt)
{
	DOOR_RELAY_t	doorRelay;

	doorRelay.status = ACTIVE;
	doorRelay.timerCnt = ((UINT32)(timerCnt*1000));

	if(ioctl(gpioDrvFd, SET_DOOR_RELAY, &doorRelay) == -1)
	{
		EPRINT(SYS_LOG,"Failed to execute SET_DOOR_RELAY");
		return FAIL;
	}
	return SUCCESS;
}

//***************************************************************************
//	DeactivateDoorRelay()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				It deactivates Door Lock relay.
//
//***************************************************************************
BOOL DeactivateDoorRelay(void)
{
	DOOR_RELAY_t	doorRelay;

	doorRelay.status = INACTIVE;
	doorRelay.timerCnt = 0; // N.A.

	if(ioctl(gpioDrvFd, SET_DOOR_RELAY, &doorRelay) == -1)
	{
		EPRINT(SYS_LOG,"Failed to execute SET_DOOR_RELAY");
		return FAIL;
	}
	return SUCCESS;
}

//*****************************************************************************
//	SetBuzzer()
//	Param:
//		IN :	ACCESS_CONTROL_RESPONSE_e
//				RESPONSE_PORT_e
//		OUT:	None
//	Returns:	None
//	Description:
//				It sets cadence for buzzer.
//*****************************************************************************
void SetBuzzer(ACCESS_CONTROL_RESPONSE_e resp, RESPONSE_PORT_e respPort)
{
	switch(respPort)
	{
		default:
		{
			break;
		}

		case INTERNAL_PORT:
		{
			if(ioctl(gpioDrvFd, SET_INT_BUZZER, resp) == -1)
			{
				EPRINT(SYS_LOG,"Failed to execute SET_INT_BUZZER");
			}
			break;
		}

		case EXTERNAL_PORT:
		{
			if(ioctl(gpioDrvFd, SET_EXT_BUZZER, resp) == -1)
			{
				EPRINT(SYS_LOG,"Failed to execute SET_EXT_BUZZER");
			}
			break;
		}

		case INT_EXT_PORT:
		{
			if(ioctl(gpioDrvFd, SET_INT_EXT_BUZZER, resp) == -1)
			{
				EPRINT(SYS_LOG,"Failed to execute SET_INT_BUZZER");
			}
			break;
		}
	}

	if( (respPort >= EXTERNAL_PORT) && (YES == CheckComboRdrPresent(COMBO_READER_ANY)) && (IsCerFwInProgress() == NO) )
	{
		CER_AV_RESP_t cerAVResp;
		cerAVResp.acResp = acRespConvForCer[resp];
		cerAVResp.avComb = BUZ_ONLY;

		#ifdef ATOM_READER_SUPPORTED
		if ((cerAVResp.avComb != LED_ONLY) || (NO == CheckComboRdrPresent(COMBO_READER_ATOM_RD300)))
		#endif
		{
			InsertMsgToCERTxQ(CER_TX_AVRESP, sizeof(CER_AV_RESP_t), (UINT8PTR)&cerAVResp);
		}
	}
}

//*****************************************************************************
//	SetLed()
//	Param:
//		IN :	ACCESS_CONTROL_RESPONSE_e
//				RESPONSE_PORT_e
//		OUT:	None
//	Returns:	None
//	Description:
//				It sets cadence for all LEDs.
//*****************************************************************************
void SetLed(ACCESS_CONTROL_RESPONSE_e resp, RESPONSE_PORT_e respPort)
{
	switch(respPort)
	{
		default:
		case INTERNAL_PORT:
			break;

		case INT_EXT_PORT:
		case EXTERNAL_PORT:
		{
			if(ioctl(gpioDrvFd, SET_EXT_LED, resp) == -1)
			{
				EPRINT(SYS_LOG,"Failed to execute SET_EXT_LED");
			}
			break;
		}
	}

	if( (respPort >= EXTERNAL_PORT) && (YES == CheckComboRdrPresent(COMBO_READER_ANY)))
	{
		CER_AV_RESP_t cerAVResp;
		cerAVResp.acResp = acRespConvForCer[resp];
		cerAVResp.avComb = LED_ONLY;

		#ifdef ATOM_READER_SUPPORTED
		if ((cerAVResp.avComb != LED_ONLY) || (NO == CheckComboRdrPresent(COMBO_READER_ATOM_RD300)))
		#endif
		{
			InsertMsgToCERTxQ(CER_TX_AVRESP, sizeof(CER_AV_RESP_t), (UINT8PTR)&cerAVResp);
		}
	}

}

//***************************************************************************
//	SetReader1Reset()
//	Param:
//		IN :	HIGH
//				LOW
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				It changes physical status of Reader1 RESET pin as per given.
//***************************************************************************
void SetReader1Reset(BOOL status)
{
	BOOL	tempStatus;

	tempStatus = ( (status == HIGH) ? LOW : HIGH);

	// set reverse pulse condition for one mili second
	if(ioctl(gpioDrvFd, SET_RDR1_RESET, tempStatus) == -1)
	{
		EPRINT(SYS_LOG,"Failed to execute SET_RDR1_RESET");
	}
	DelayMiliSecs(1);
	if(ioctl(gpioDrvFd, SET_RDR1_RESET, status) == -1)
	{
		EPRINT(SYS_LOG,"Failed to execute SET_RDR1_RESET");
	}
}

//***************************************************************************
//	SetReader3Hold()
//	Param:
//		IN :	HIGH
//				LOW
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				It changes physical status of Reader3 HOLD pin as per given.
//***************************************************************************
void SetReader3Hold(UNUSED_PARA BOOL status)
{
}

//***************************************************************************
//	WifiReset()
//	Param:
//		IN :	None
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				Application uses this API to reset wifi module.
//***************************************************************************
void WifiReset(void)
{
}

//***************************************************************************
//	SetUsbPower()
//	Param:
//		IN :	status	-ON or OFF USB power
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				Application uses this API to set USB power ON or OFF.
//
//***************************************************************************
void SetUsbPower(BOOL status)
{
	if(ioctl(gpioDrvFd, CHNG_USB_CTRL, status) == -1)
	{
		EPRINT(SYS_LOG,"Failed to execute SET_USB2_POWER");
	}
}

//***************************************************************************
//	SetUsbFPPower()
//	Param:
//		IN :	status	-ON or OFF USB power
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				Application uses this API to set USB power ON or OFF.
//
//***************************************************************************
void SetUsbFPPower(BOOL status)
{
	if(ioctl(gpioDrvFd, CHNG_USB_FP_CTRL, status) == -1)
	{
		EPRINT(SYS_LOG,"Failed to execute SET_USB_FP_POWER");
	}
}

//***************************************************************************
//	RunGpioTask()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				It scans input pins .
//***************************************************************************
static void RunGpioTask(UNUSED_PARA UINT32 dummy)
{
	GetIOStatus();
}

//***************************************************************************
//	GetIOStatus()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				It scans input pins .
//***************************************************************************
static void GetIOStatus(void)
{
	GPIO_STATUS_t	curInputStatus;

	// Read input pin status
	if(ioctl(gpioDrvFd, GET_INPUT_STATUS, &curInputStatus) == -1)
	{
		EPRINT(SYS_LOG,"Failed to execute GET_INPUT_STATUS");
	}
	else
	{

		if(SysInputStatus.exitSw != curInputStatus.exitSw)
		{
			SysInputStatus.exitSw = curInputStatus.exitSw;
			ProcMsg(MSG_REX_IP_CHANGE_EVENT, &SysInputStatus.exitSw, sizeof(SysInputStatus.exitSw));
			DPRINT(GENERAL_LOG,"input change: REX: %s", IoStatus[curInputStatus.exitSw]);
		}
		if(SysBasicPara.doorSense.type == NORMAL_OPEN)
		{
			if(curInputStatus.doorSts == INPUT_INACTIVE)
			{
				curInputStatus.doorSts = INPUT_ACTIVE;
			}
			else if(curInputStatus.doorSts == INPUT_ACTIVE)
			{
				curInputStatus.doorSts = INPUT_INACTIVE;
			}
		}
		if(SysInputStatus.doorSense != curInputStatus.doorSts)
		{
			SysInputStatus.doorSense = curInputStatus.doorSts;
			ProcMsg(MSG_DOOR_SENSE_CHANGE_EVENT, &SysInputStatus.doorSense, sizeof(SysInputStatus.doorSense) );
			DPRINT(GENERAL_LOG,"input change: DOOR SENSE: %s", IoStatus[curInputStatus.doorSts]);
		}
		
// 		if(SysInputStatus.rdrTamper != curInputStatus.rdrTamper)
// 		{
// 			BOOL status;
// 			SysInputStatus.rdrTamper = curInputStatus.rdrTamper;
// 			status = (SysInputStatus.rdrTamper || CerTamperStatus );
// 			WiegandTamperStatus = SysInputStatus.rdrTamper;
// 			ProcMsg(MSG_READER_TAMPER_CHANGE_EVENT, &SysInputStatus.rdrTamper, sizeof(SysInputStatus.rdrTamper) );
// 			DPRINT(GENERAL_LOG,"input change: READER TAMPER: %s", IoStatus[curInputStatus.rdrTamper]);
// 		}

		if(SysInputStatus.sysTamper != curInputStatus.doorTamper)
		{
			SysInputStatus.sysTamper = curInputStatus.doorTamper;
			ProcMsg(MSG_TAMPER_IP_CHANGE_EVENT, &SysInputStatus.sysTamper, sizeof(SysInputStatus.sysTamper));
			DPRINT(GENERAL_LOG,"input change: SYSTEM TAMPER: %s", IoStatus[curInputStatus.doorTamper]);
		}
	}
	// Store Output status.
	GetCurrOutputStatus(&SysOutputStatus);
}

//*****************************************************************************
//	StartInputScanTimer()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will start GPIO Input Scan cyclic timer.
//				Inputs are read periodically and its logical status are updated.
//
//*****************************************************************************
void StartInputScanTimer(void)
{
	TIMER_INFO_t tInfo;

	tInfo.count = GPIO_SCAN_TMR;
	tInfo.data = 0;
	tInfo.funcPtr = &RunGpioTask;

	StartTimer(tInfo, &gpioScanTmrHandle, __FUNCTION__);
}

//***************************************************************************
//	ClearAuxInStatus()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				It clears aux input status to its initial status.
//***************************************************************************
void ClearAuxInStatus(void)
{
	SysInputStatus.auxIn = MAX_IO_STATUS;
	auxCurIoStatus = MAX_IO_STATUS;
}

//***************************************************************************
//	ClearDoorSenseStatus()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				It clears door sense status to its initial status.
//***************************************************************************
void ClearDoorSenseStatus(void)
{
	SysInputStatus.doorSense = MAX_IO_STATUS;
	doorSenseCurIoStatus = MAX_IO_STATUS;
}

//***************************************************************************
//	GetCurrInputStatus()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				It clears door sense status to its initial status.
//***************************************************************************
BOOL GetCurrInputStatus(GPIO_STATUS_t *inutStatus)
{
	if(ioctl(gpioDrvFd, GET_INPUT_STATUS, inutStatus) == -1)
	{
		EPRINT(SYS_LOG,"Failed to execute GET_INPUT_STATUS");
		return FAIL;
	}
	return SUCCESS;
}

//***************************************************************************
//	SetBuzMuteFlagStatus()
//	Param:
//		IN :	status - ON/OFF
//
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				It enables/disables the Buzzer Mute Feature.
//
//***************************************************************************
void SetBuzMuteFlagStatus(BOOL status)
{
	if(ioctl(gpioDrvFd, SET_BUZ_MUTE, status) == -1)
	{
		EPRINT(SYS_LOG,"Failed to execute SET_BUZ_MUTE");
	}
}

//***************************************************************************
//	SetBackLightBrightness()
//	Param:
//		IN :	brighness
//
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//
//***************************************************************************
void SetBackLightBrightness(UINT8 brightness)
{
	CHAR	brightnessBuff[4];

	sprintf(brightnessBuff,"%d", brightness);

	backlightFd = open(BACKLIGHT_BRIGHNESS_FILE, O_WRONLY);
	if(backlightFd < 0)
	{
		EPRINT(SYS_LOG,"Application Failed to Backlight Brightness File");
	}
	else
	{
		write(backlightFd, brightnessBuff, sizeof(brightnessBuff));
	}
	close(backlightFd);
}

//*****************************************************************************
//	GetInputOutputStatus()
//	Param:
//		IN :	None
//		OUT:	IO_STATUS_t
//	Returns:	None
//	Description:
//				This Function gives Input and Output Status to Application.
//				IO_STATUS_t struct is output.
//*****************************************************************************
void GetInputOutputStatus(IO_STATUS_t * ioStatus)
{
	OUTPUT_STATUS_t				opStatus;

	// get status of door controller’s peripheral
	GetCurrOutputStatus(&opStatus);

	ioStatus->doorSenseStatus = SysInputStatus.doorSense;
	ioStatus->auxInputStatus = SysInputStatus.auxIn;
	ioStatus->exitSwStatus = SysInputStatus.exitSw;
	ioStatus->doorTamperSwStatus = SysInputStatus.sysTamper;
	ioStatus->emTamperSwStatus = SysInputStatus.lockTamper;
	ioStatus->readerTamperSwStatus = SysInputStatus.rdrTamper;
	ioStatus->doorLockStatus = opStatus.doorLockStatus;
	ioStatus->auxOutputStatus =opStatus.auxOutStatus;
}

//***************************************************************************
//	GetCurrOutputStatus()
//	Param:
//		IN :	NONE
//		OUT:	Output status
//	Returns:	SUCCESS or FAIL
//
//	Description:
//				It gives the status of output aux relay and door relay.
//***************************************************************************
static BOOL GetCurrOutputStatus(OUTPUT_STATUS_t *outStatus)
{
	if(ioctl(gpioDrvFd, GET_OUTPUT_STATUS, outStatus) == -1)
	{
		EPRINT(SYS_LOG,"Failed to execute GET_OUTPUT_STATUS");
		return FAIL;
	}
	return SUCCESS;
}

//*****************************************************************************
//	StartOutputWaitTimer()
//	Param:
//		IN :	timerCnt- Upon timeout, relay should be activated as per this value
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function will start Output Wait Timer.
//				This timer is started when Alarm input is detected and debounce
//				timeout takes place.and if outputwaittime is configured.
//
//*****************************************************************************
static void StartOutputWaitTimer(UINT32 timerCnt)
{
	TIMER_INFO_t tInfo;

	if(outputWaitTmrHandle == INVALID_TIMER_HANDLE)
	{
		tInfo.count = (SysAdvPara.outputWaitTime *10);
		tInfo.data = timerCnt;
		tInfo.funcPtr = &OutputWaitTimeout;

		StartTimer(tInfo, &outputWaitTmrHandle, __FUNCTION__);
	}
	else
	{
		ReloadTimer(outputWaitTmrHandle, (SysAdvPara.outputWaitTime));
	}
}

//*****************************************************************************
//	OutputWaitTimeout()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function is call back function when Output Wait timeout.
//				This timer was started aux out was activated and outputwaittimer
//				is configured.
//*****************************************************************************
static void OutputWaitTimeout(UINT32 timerCnt)
{
	DeleteTimer(&outputWaitTmrHandle);
	ActivateAuxRelay(timerCnt);
}

//*****************************************************************************
//	ActivateAuxRelayWithOutputWaitTImer()
//	Param:
//		IN :	timercount-     for the time aux out should be active
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function is call back function when Output Wait timeout.
//				This timer was started aux out was activated and outputwaittimer
//				is configured.
//*****************************************************************************
void ActivateAuxRelayWithWaitTimer(UINT8 timerCnt)
{
	if(!SysAdvPara.outputWaitTime)
	{
		ActivateAuxRelay(timerCnt);
		return;
	}
	StartOutputWaitTimer(timerCnt);
}

//*****************************************************************************
//	DeleteOutputWaitTimer()
//	Param:
//		IN :	NONE
//		OUT:	NONE
//	Returns:	NONE
//	Description:
//				This function delete the output wait timer.so aux out will not
//				change if iolink is reset before output wait timeout.
//*****************************************************************************
void DeleteOutputWaitTimer(void)
{
	DeleteTimer(&outputWaitTmrHandle);
}

//*****************************************************************************
//	SetBuzzerLedCadDuration()
//	Param:
//		IN :	cadanceIndex
//				led buzzer cadance config stucture
//		OUT:	NONE
//	Returns:	NONE
//	Description: This function fills buzledCad structure and writes it in
//			in MxGpio file
//*****************************************************************************
void SetBuzzerLedCadDuration(UINT8 idx)
{
	BUZZER_LED_CAD_t	buzLedCad; //this structure will be sent to
	INT32				retVal=FAIL;

	//to calculate cad bits
	buzLedCad.cadBits =  BuzzLedCadPara[idx].cadBits;

	//to assign no of cycles
	if(BuzzLedCadPara[idx].cycles == 0)
	{
		buzLedCad.noOfCycles = 255;
	}
	else
	{
		buzLedCad.noOfCycles = BuzzLedCadPara[idx].cycles;
	}

	switch(idx)
	{
		case LED_AND_BUZ_CAD_SUCCESS:
		{
			buzLedCad.accessResp = ACCESS_SUCCESS_RESP;
			break;
		}
		case LED_AND_BUZ_CAD_DENIED:
		{
			buzLedCad.accessResp = ACCESS_FAIL_RESP;
			break;
		}
		default:
		{
			buzLedCad.accessResp = ACCESS_CONTROL_RESP_MAX;
			break;
		}
	}

	retVal = ioctl(gpioDrvFd, SET_BUZ_LED_CAD_DUR, &buzLedCad);
	if( retVal < 0)
	{
		EPRINT(SYS_LOG,"Failed to execute SET_BUZ_LED_CAD_DUR");
	}
}
//*****************************************************************************
//	SetPWMStatus()
//	Param:
//		IN :	None
//		OUT:	NONE
//	Returns:	NONE
//	Description: This function Use For Set PWM Status Value.
//*****************************************************************************
BOOL SetPWMStatus(PWM_DUTY_e pwmStatus)
{
	if(ioctl(gpioDrvFd, SET_PWM_DUTY, pwmStatus) == -1)
	{
		EPRINT(SYS_LOG,"Failed to execute SET_PWM_DUTY");
		return FAIL;
	}
	return SUCCESS;
}

//*****************************************************************************
//	GetReaderTamperStatus()
//	Param:
//		IN :	None
//		OUT:	DC_STATUS_RESP_DATA_t
//	Returns:	None
//	Description:
//				This Function gives reader tamper status Status to Application.
//				DC_STATUS_RESP_DATA_t struct is output.
//*****************************************************************************
void GetReaderTamperStatus(IO_STATUS_e * rdrTamperStatus)
{
	rdrTamperStatus = ( WiegandTamperStatus | CerTamperStatus );
	rdrTamperStatus = ( WiegandTamperStatus | CerTamperStatus );
}

///////////////////////////////////////////////////////////////////////////////
// End of File "MxGpio.c"
///////////////////////////////////////////////////////////////////////////////
