#if !defined GPIO_DRV_H_
#define GPIO_DRV_H_
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
//   Project      : NVR-X (HiSilicon Based NVR-X)
//   Created By   : Darshan Patel
//   File         : MxGpioDrv.h
//   Description  : This file implements GPIO driver releted defines and API.
//
/////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#include "MxTypedef.h"

//******** Defines and Data Types ****
#define GPIO_DEVICE_NAME		"/dev/MxGpioDrv"

enum
{
	NVRX_IDLE_RESP,
	NVRX_APPL_INIT_RESP,
	NVRX_NET_CONN_RESP,
	NVRX_NET_DISCONN_RESP,
	NVRX_STORAGE_INCOMPLETE_RESP,
	NVRX_STORAGE_FULL_ALT_RESP,
	NVRX_STORAGE_FULL_RESP,
	NVRX_FORMAT_DISK_RESP,
	NVRX_FW_UPG_INPROCESS_RESP,
	NVRX_FW_UPG_FAILED_RESP,
	NVRX_TRG_EVT_RESP,
	NVRX_BKUP_FULL_RESP,
	NVRX_NO_BKUP_RESP,
	NVRX_NORMAL_STATE,
	NVRX_RECORDING_FAIL,
	NVRX_RECORDING_FAIL_FOR_ALL,
	NVRX_CPUTEMP_HIGH,

	NVRX_RESP_MAX,
	NVRX_RESP_ALL,
	NVRX_RESP_PRESENT,
	NVRX_RESP_NONE
};
typedef UINT8	NVRX_RESPONSE_e;

typedef enum
{
	BUZZER,
	RED_LED,
	GREEN_LED,
	BUZZER_LED_MAX
}BUZZER_LED_e;

typedef enum
{
	BUZZER_CYCLES,
	RED_LED_CYCLES,
	GREEN_LED_CYCLES,
	BUZZER_LED_CYCLES_MAX
}BUZZER_LED_CYCLES_e;

typedef enum
{
	IPDEF_INPUT,
	ALMIN1_INPUT,
	ALMIN2_INPUT,

	MAX_INPUT_MAP
}INPUT_MAP_e;

typedef enum
{
	PIN_LOW,
	PIN_HIGH,
	PIN_UNKNOWN,
}PIN_STATUS_e;


// Input status type
enum
{
	INPUT_INACTIVE,
	INPUT_ACTIVE,
	INPUT_SHORT,
	INPUT_OPEN,
	INPUT_UNKNOWN,

	MAX_INPUT_STATUS,
};
typedef UINT8	INPUT_STATUS_e;

//Output status type
enum
{
	OUTPUT_INACTIVE,
	OUTPUT_ACTIVE,
	OUTPUT_UNKNOWN,

	MAX_OUTPUT_STATUS,
};
typedef UINT8	OUTPUT_STATUS_e;

typedef struct
{
	PIN_STATUS_e		status;				// Physical Input Status (HIGH or LOW)
	UINT16				sampleVal;			// value of each sample (stored in a bit)
}INPUT_PIN_ACTIVITY_t;

enum{
	IPDEF_PRESSED,
	IPDEF_RELEASED,
};

enum{
	ALMIN1_SET,
	ALMIN1_CLEAR,
	ALMIN2_SET,
	ALMIN2_CLEAR,
};
enum{
	BUZZER_SET,
	BUZZER_CLEAR,
};

enum{
	ALMOUT_SET,
	ALMOUT_CLEAR,
};

enum{
	FAN1_ON,
	FAN2_ON,
	ALLFAN_ON,
	FAN1_OFF,
	FAN2_OFF,
	ALLFAN_OFF,
};

enum{
	USB3POWER_ON,  	/* unused */
	USB2POWER1_ON,
	USB2POWER2_ON, 	/* unused */
	ALLUSBPOWER_ON, /* unused */
	USB3POWER_OFF,  /* unused */
	USB2POWER1_OFF,
	USB2POWER2_OFF,  /* unused */
	USBALLPOWER_OFF, /* unused */
	USBPOWER_LAST
};

enum{
	REDLED_ON,
	REDLED_OFF,
	GREENLED_ON,
	GREENLED_OFF,
	ORANGELED_ON,
	ORANGELED_OFF,
};

typedef struct
{
	unsigned int 	InputPinStatus[MAX_INPUT_MAP];
}INPUT_STATUS_t;

typedef struct
{
	UINT16						buzLedCad;
	UINT8						noOfCycles;
	UINT8						cadPosition;	// current bit position from where to play cadence accordingly	
	UINT8						cadBits;	
	NVRX_RESPONSE_e				accessResp;
}BUZZER_LED_CAD_t;

typedef struct
{
	UINT16				LedCadResp;	
	UINT16				BuzCadResp;
}BUZZER_LED_RESP_t;


#define MXGPIO_IOC_MAGIC  'G'
typedef enum
{
	MXGPIO_SET_BUZZER			= 	_IOW(MXGPIO_IOC_MAGIC,  1,unsigned int),
	MXGPIO_SET_ALMOUT			= 	_IOW(MXGPIO_IOC_MAGIC,  2,unsigned int),
	MXGPIO_SET_FANCTL			= 	_IOW(MXGPIO_IOC_MAGIC,  3,unsigned int),
	MXGPIO_SET_USBPOWER			=	_IOW(MXGPIO_IOC_MAGIC,  4,unsigned int),
	MXGPIO_SET_STSLED			=	_IOW(MXGPIO_IOC_MAGIC,  5,unsigned int),
	MXGPIO_SET_CADENCE			=	_IOW(MXGPIO_IOC_MAGIC,  6,unsigned int),
	MXGPIO_CLR_CADENCE			=	_IOW(MXGPIO_IOC_MAGIC,  7,BUZZER_LED_RESP_t),
	MXGPIO_GET_ALL_INPUT		=	_IOR(MXGPIO_IOC_MAGIC,  8,INPUT_STATUS_t),
	MXGPIO_GET_CORETEMPE		=	_IOR(MXGPIO_IOC_MAGIC,  9,unsigned int),
	MXGPIO_GET_CAD_STS			=	_IOR(MXGPIO_IOC_MAGIC,  10,BUZZER_LED_CAD_t),
	MXGPIO_GET_BUZ_STS			=	_IOR(MXGPIO_IOC_MAGIC,  11,unsigned int),
	MXGPIO_GET_CARD_TYPE		=	_IOR(MXGPIO_IOC_MAGIC,  12,unsigned int),
	MXGPIO_GET_BOARD_VERSION	=	_IOR(MXGPIO_IOC_MAGIC,  13,unsigned int),
	MXGPIO_SET_CLR_PHY	        =	_IOR(MXGPIO_IOC_MAGIC,  14,unsigned int),
	MXGPIO_SET_CLR_PHY2	        =	_IOR(MXGPIO_IOC_MAGIC,  15,unsigned int),
	MXGPIO_SET_CLR_ASM_RST	        =	_IOR(MXGPIO_IOC_MAGIC,  16,unsigned int),
	MXGPIO_IOC_MAXNR
}GPIO_IOCTL_e;
//******** Function Prototypes *******

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // GPIO_DRV_H_
