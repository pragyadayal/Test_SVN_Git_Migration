#if !defined MXGPIO_H
#define MXGPIO_H
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
//   Project      : ACS (Access Control System) -Master Controller
//   Created By   : Kunjal Garach
//   File         : MxGpio.h
//   Description  : This file implements GPIO related defines and API.
//
/////////////////////////////////////////////////////////////////////////////

//********Place all include files ****
#include "MxTypedef.h"
#include "MxGpioDrv.h"
#include "MxSysCommonDef.h"

//******** Defines and Data Types ****

enum
{
	IO_INACTIVE,
	IO_ACTIVE,
	IO_SHORT,
	IO_OPEN,
	MAX_IO_STATUS
};
typedef UINT8	IO_STATUS_e;

// Input port status
typedef struct
{
	INPUT_STATUS_e	sysTamper;
	INPUT_STATUS_e	rdrTamper;
	INPUT_STATUS_e	lockTamper;
	INPUT_STATUS_e	doorSense;
	INPUT_STATUS_e  auxIn;
	INPUT_STATUS_e	exitSw;
}INPUT_STATUS_t;

typedef struct __attribute__((packed))
{
	DOOR_SENSE_STATUS_e		doorSenseStatus;		// From Door Sense
	AUX_INPUT_STATUS_e		auxInputStatus;			// From Aux Input
	IO_STATUS_e				exitSwStatus;			// From Exit Switch
	IO_STATUS_e				doorTamperSwStatus;		// From Door Tamper Switch
	IO_STATUS_e				emTamperSwStatus;		// From EM Tamper Switch
	IO_STATUS_e				readerTamperSwStatus;	// From Reader Tamper Switch
	IO_STATUS_e				doorLockStatus;			// Door Open Relay Status
	IO_STATUS_e				auxOutputStatus;		// Aux Output Relay Status
}IO_STATUS_t;

//******** Function Prototypes *******
void InitGpioDriver(void);
void ActivateAuxRelay(UINT16 timer);
void ActivateAuxRelayWithWaitTimer(UINT8 timerCnt);
void DeactivateAuxRelay(void);
BOOL ActivateDoorRelay(FLOAT32 timer);
BOOL DeactivateDoorRelay(void);
void SetReader1Reset(BOOL status);
void SetReader3Hold(BOOL status);
void SetBuzzer(ACCESS_CONTROL_RESPONSE_e acResp, RESPONSE_PORT_e respPort);
void SetLed(ACCESS_CONTROL_RESPONSE_e acResp, RESPONSE_PORT_e respPort);
void WifiReset(void);
void SetUsbPower(BOOL status);
void SetUsbFPPower(BOOL status);
void StartInputScanTimer(void);
void ClearAuxInStatus(void);
void ClearDoorSenseStatus(void);
BOOL GetCurrInputStatus(GPIO_STATUS_t *inutStatus);
void SetBuzMuteFlagStatus(BOOL status);
void SetBackLightBrightness(UINT8 brightness);
void GetInputOutputStatus(IO_STATUS_t * ioStatus);
void DeleteOutputWaitTimer(void);
void SetBuzzerLedCadDuration(UINT8 idx);
BOOL SetPWMStatus(PWM_DUTY_e pwmStatus);
void GetReaderTamperStatus(IO_STATUS_e * rdrTamperStatus);

#endif  // MXGPIO_H
