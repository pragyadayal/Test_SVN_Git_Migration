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
//   Project      : NVR-X (Rockchip Based NVR)
//   Created By   : Darshan Patel
//   File         : MxGpioDrv.c
//   Description  : This file implements GPIO driver related functions.
//
/////////////////////////////////////////////////////////////////////////////

//*** Place all include files here ***

#include<linux/kernel.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/sysfs.h>
#include <linux/workqueue.h>
#include "MxGpioDrv.h"
#include <linux/seq_file.h>
#include<linux/jiffies.h>

//******** Extern Variables **********

//****** Defines and Data Types ******
#define GPIO_DRV_NAME		"gpios"
#define MXGPIO_DEVICE_NAME	"MxGpioDrv"
#define GPIO_NAME		"gpio%d"

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))


#define ASM_RST_PIN		GPIO_TO_PIN(0, 22) //GPIO0_C6_d
#define PHY0_RST_PIN		GPIO_TO_PIN(2, 27) //GPIO2_D3_d
#define PHY1_RST_PIN		GPIO_TO_PIN(2, 25) //GPIO2_D1_d
#define BUZZER_GPIO_PIN		GPIO_TO_PIN(0, 5) //GPIO0_A5_d
#define IP_DEFUALT_PIN		GPIO_TO_PIN(0, 11) //GPIO0_B3_u
#define ALM_RLY_CNTL_PIN	GPIO_TO_PIN(0, 15) //GPIO0_B7_d
#define FAN_CTL1_PIN		GPIO_TO_PIN(1, 29) //GPIO1_D5_u
#define FAN_CTL2_PIN		GPIO_TO_PIN(1, 30) //GPIO1_D6_u
#define ALM_IN1_PIN		GPIO_TO_PIN(3, 18) //GPIO3_C2_d
#define ALM_IN2_PIN		GPIO_TO_PIN(3, 19) //GPIO3_C3_d
#define USB2_PWREN0_PIN		GPIO_TO_PIN(3, 16) //GPIO3_C0_d
#define REDLED_GPIO_PIN		GPIO_TO_PIN(0, 23) //GPIO0_C7_d
#define GREENLED_GPIO_PIN	GPIO_TO_PIN(0, 16) //GPIO0_C0_d

#define CARD_TYPE_0_PIN		GPIO_TO_PIN(3, 0) //GPIO3_A0_d
#define CARD_TYPE_1_PIN		GPIO_TO_PIN(3, 1) //GPIO3_A1_d
#define CARD_TYPE_2_PIN		GPIO_TO_PIN(3, 2) //GPIO3_A2_d
#define CARD_TYPE_3_PIN		GPIO_TO_PIN(3, 3) //GPIO3_A3_d

#define BOARD_VER_0_PIN		GPIO_TO_PIN(3, 4) //GPIO3_A4_d
#define BOARD_VER_1_PIN		GPIO_TO_PIN(3, 5) //GPIO3_A5_d
#define BOARD_VER_2_PIN		GPIO_TO_PIN(3, 6) //GPIO3_A6_d
#define BOARD_VER_3_PIN		GPIO_TO_PIN(3, 7) //GPIO3_A7_d


#define SetBuzzerPin(status)	(status == ACTIVE) ? gpio_set_value(BUZZER_GPIO_PIN, HIGH) : gpio_set_value(BUZZER_GPIO_PIN, LOW)
#define SetGreenLedPin(status)  (status == ACTIVE) ? gpio_set_value(GREENLED_GPIO_PIN, HIGH) : gpio_set_value(GREENLED_GPIO_PIN, LOW)
#define SetRedLedPin(status)	(status == ACTIVE) ? gpio_set_value(REDLED_GPIO_PIN, HIGH) : gpio_set_value(REDLED_GPIO_PIN, LOW)
#define SetUsbCtrlPin(status)	(status == ACTIVE) ? gpio_set_value(USB2_PWREN0_PIN, LOW) : gpio_set_value(USB2_PWREN0_PIN, HIGH)
#define SetAlmRelayCtrlPin(status)	(status == ACTIVE) ? gpio_set_value(ALM_RLY_CNTL_PIN, HIGH) : gpio_set_value(ALM_RLY_CNTL_PIN, LOW)
#define SetFan1CtrlPin(status)	(status == ACTIVE) ? gpio_set_value(FAN_CTL1_PIN, LOW) : gpio_set_value(FAN_CTL1_PIN, HIGH) 
#define SetFan2CtrlPin(status)	(status == ACTIVE) ? gpio_set_value(FAN_CTL2_PIN, LOW) : gpio_set_value(FAN_CTL2_PIN, HIGH) 


#define	GPIO_POLL_CNT			30	//@10mSec resolution --> 30mSec
#define	BUZ_LED_TMR_CNT			500	//@10mSec resolution --> 500mSec
#define BUZ_LED_SYNC_TMR_CNT		10	//@10msec resolution --> 10msec

#define TSENSOR_EN			(1 << 30)
#define TSENSOR_MON_EN			(1 << 29)
#define TSENSOR_MON_PERIOD(n)		(n << 16)
#define GET_TCODE0(n)			(n & 0x3FF)
#define GET_TCODE1(n)			( (n >> 16) & 0x3FF )

#define	ALL_SAMPLES_HIGH		0x1F	// value calculated for 5 samples
#define	INPUT_SAMPLE_MASK_VAL		ALL_SAMPLES_HIGH
#define	ALL_SAMPLES_LOW			0

#define MAX_PRIORITY			8
#define INFINITE_CYCLE				0xFF

// Maximum 16 bits are used for cadence, MSB is is for don't care.
// So, 15 bits can be used for other cadence. So, max duration possible for
// cadence is 15 bits x 500 msec resolution = 7500 msec.
#define	CAD_DONT_CARE				0x8000

#define CAD_OFF						0x0000
#define CONT_OFF_CAD_BITS			15
#define CAD_OFF_CAD_BITS			1

#define CAD_ON						0x7fff
#define CONT_ON_CAD_BITS			15

#define IDLE_CAD					0x0000
#define IDLE_CAD_BITS				1

//500 - ON, 500 - OFF
#define SMALL_PULSE_CAD				0x0001
#define SMALL_PULSE_CAD_BITS		2

//1000 - ON, 1000 - OFF
#define SHORT_PULSE_CAD				0x0003
#define SHORT_PULSE_CAD_BITS		4

//1000 - ON, 1000 - OFF, 1000 - ON, 1000 - OFF
#define DOUBLE_PULSE_CAD			0x0033
#define DOUBLE_PULSE_CAD_BITS		8

//3000 - ON and 1000 – OFF
#define LONG_ON_CAD					0x003F
#define LONG_ON_CAD_BITS			8

//500 - ON, 5000 - OFF
#define SHORT_LONG_CAD				0x0001
#define SHORT_LONG_CAD_BITS			11

//Red 1000 - ON and 5000 – OFF
#define LONG_OFF_CAD				0x0003
#define LONG_OFF_CAD_BITS			12

//Red 2000 - ON and 2000 – OFF
#define LONG_PULSE_CAD				0x000F
#define LONG_PULSE_CAD_BITS			8

//Red 1000 - ON and 3000 – OFF
#define SHORT_ON_CAD				0x0003
#define SHORT_ON_CAD_BITS			8

//Red 1000 - ON and 2000 – OFF
#define SHORT_OFF_CAD				0x0003
#define SHORT_OFF_CAD_BITS			6

// Table defining Cadence mapped to particular Access Request Response
static const UINT16	accessRespCadMap[NVRX_RESP_MAX][BUZZER_LED_MAX] = 
{	//Buzzer 			Red Led 			Green Led
	{IDLE_CAD,			IDLE_CAD,			IDLE_CAD		},	//NVRX_IDLE_RESP	
	{LONG_ON_CAD,			DOUBLE_PULSE_CAD,		CAD_OFF			},	//NVRX_APPL_INIT_RESP
	{CAD_OFF,			CAD_OFF,			LONG_ON_CAD		},	//NVRX_NET_CONN_RESP
	{CAD_OFF,			LONG_ON_CAD,			CAD_OFF			},	//NVRX_NET_DISCONN_RESP
	{LONG_OFF_CAD,			LONG_OFF_CAD,			CAD_OFF			},	//NVRX_STORAGE_INCOMPLETE_RESP
	{CAD_OFF,			LONG_PULSE_CAD,			CAD_OFF			},	//NVRX_STORAGE_FULL_ALT_RESP
	{CAD_OFF,			SHORT_ON_CAD,			CAD_OFF			},	//NVRX_STORAGE_FULL_RESP
	{CAD_OFF,			SHORT_PULSE_CAD,		CAD_OFF			},	//NVRX_FORMAT_DISK_RESP
	{CAD_OFF,			CAD_ON,				CAD_ON			},	//NVRX_FW_UPG_INPROCESS_RESP
	{SHORT_PULSE_CAD,		SHORT_PULSE_CAD,		CAD_OFF			},	//NVRX_FW_UPG_FAILED_RESP
	{LONG_ON_CAD,			CAD_OFF,			SMALL_PULSE_CAD		},	//NVRX_TRG_EVT_RESP
	{SHORT_OFF_CAD,			CAD_OFF,			SMALL_PULSE_CAD		},	//NVRX_BKUP_FULL_RESP
	{SHORT_ON_CAD,			CAD_OFF,			SMALL_PULSE_CAD		},	//NVRX_NO_BKUP_RESP
	{CAD_OFF,			CAD_OFF,			SMALL_PULSE_CAD 	},	//NVRX_NORMAL_STATE
	{CAD_OFF,			SMALL_PULSE_CAD,    		SMALL_PULSE_CAD 	},  	//NVRX_RECORDING_FAIL
	{CAD_OFF,			CAD_ON,				CAD_OFF			},  	//NVRX_RECORDING_FAIL_FOR_ALL
	{SHORT_LONG_CAD,		SMALL_PULSE_CAD,    		CAD_OFF         	}   	//NVRX_CPUTEMP_HIGH
};

// Table defining cadence cycles for each access response
static	UINT8	accessRespCadCycles[NVRX_RESP_MAX][BUZZER_LED_CYCLES_MAX] =
{	//Buzzer			Red Led				Green Led
	{INFINITE_CYCLE,	INFINITE_CYCLE,		INFINITE_CYCLE },  //NVRX_IDLE_RESP
	{0x1,				0x1,				0x1			   },  //NVRX_APPL_INIT_RESP
	{INFINITE_CYCLE,	INFINITE_CYCLE,		INFINITE_CYCLE },  //NVRX_NET_CONN_RESP
	{INFINITE_CYCLE,	INFINITE_CYCLE,		INFINITE_CYCLE },  //NVRX_NET_DISCONN_RESP
	{0xA,				0xA,				0xA			   },  //NVRX_STORAGE_INCOMPLETE_RESP
	{0xF,				0xF,				0xF			   },  //NVRX_STORAGE_FULL_ALT_RESP
	{0xF,				0xF,				0xF			   },  //NVRX_STORAGE_FULL_RESP
	{INFINITE_CYCLE,	INFINITE_CYCLE,		INFINITE_CYCLE },  //NVRX_FORMAT_DISK_RESP
	{INFINITE_CYCLE,	INFINITE_CYCLE,		INFINITE_CYCLE },  //NVRX_FW_UPG_INPROCESS_RESP
	{0x1E,				0x1E,				0x1E		   },  //NVRX_FW_UPG_FAILED_RESP
	{0xF,				0x3C,				0x3C		   },  //NVRX_TRG_EVT_RESP
	{0x14,				0x3C,				0x3C           },  //NVRX_BKUP_FULL_RESP
	{0xF,				0x3C,				0x3C           },  //NVRX_NO_BKUP_RESP
	{INFINITE_CYCLE,	INFINITE_CYCLE,		INFINITE_CYCLE },  //NVRX_NORMAL_STATE
	{INFINITE_CYCLE,	INFINITE_CYCLE,		INFINITE_CYCLE },  //NVRX_RECORDING_FAIL
	{INFINITE_CYCLE,	INFINITE_CYCLE,		INFINITE_CYCLE },  //NVRX_RECORDING_FAIL_FOR_ALL
	{INFINITE_CYCLE,	INFINITE_CYCLE,		INFINITE_CYCLE }   //NVRX_CPUTEMP_HIGH
};

// Table defining cadence bits for each cadence
static	UINT8	accessRespCadBits[NVRX_RESP_MAX][BUZZER_LED_MAX] = 
{		//Buzzer 				Red Led 				Green Led
	{	IDLE_CAD_BITS,			IDLE_CAD_BITS,			IDLE_CAD_BITS		},	//NVRX_IDLE_RESP
	{	LONG_ON_CAD_BITS,		DOUBLE_PULSE_CAD_BITS,	CAD_OFF_CAD_BITS	},	//NVRX_APPL_INIT_RESP
	{	CAD_OFF_CAD_BITS,		CAD_OFF_CAD_BITS,		LONG_ON_CAD_BITS	},	//NVRX_NET_CONN_RESP
	{	CAD_OFF_CAD_BITS,		LONG_ON_CAD_BITS,		CAD_OFF_CAD_BITS	},	//NVRX_NET_DISCONN_RESP
	{	LONG_OFF_CAD_BITS,		LONG_OFF_CAD_BITS,		CAD_OFF_CAD_BITS	},	//NVRX_STORAGE_INCOMPLETE_RESP
	{	CAD_OFF_CAD_BITS,		LONG_PULSE_CAD_BITS,	CAD_OFF_CAD_BITS	},	//NVRX_STORAGE_FULL_ALT_RESP
	{	CAD_OFF_CAD_BITS,		SHORT_ON_CAD_BITS,		CAD_OFF_CAD_BITS	},	//NVRX_STORAGE_FULL_RESP
	{	CAD_OFF_CAD_BITS,		SHORT_PULSE_CAD_BITS,	CAD_OFF_CAD_BITS	},	//NVRX_FORMAT_DISK_RESP
	{	CAD_OFF_CAD_BITS,		CONT_ON_CAD_BITS,		CONT_ON_CAD_BITS	},	//NVRX_FW_UPG_INPROCESS_RESP
	{	SHORT_PULSE_CAD_BITS,	SHORT_PULSE_CAD_BITS,	CAD_OFF_CAD_BITS	},	//NVRX_FW_UPG_FAILED_RESP
	{	LONG_ON_CAD_BITS,		CAD_OFF_CAD_BITS,		SMALL_PULSE_CAD_BITS},	//NVRX_TRG_EVT_RESP
	{	SHORT_OFF_CAD_BITS,		CAD_OFF_CAD_BITS,		SMALL_PULSE_CAD_BITS},	//NVRX_BKUP_FULL_RESP
	{	SHORT_ON_CAD_BITS,		CAD_OFF_CAD_BITS,		SMALL_PULSE_CAD_BITS},	//NVRX_NO_BKUP_RESP
	{	CAD_OFF_CAD_BITS,		CAD_OFF_CAD_BITS,		SMALL_PULSE_CAD_BITS},	//NVRX_NORMAL_STATE
	{	CAD_OFF_CAD_BITS,		SMALL_PULSE_CAD_BITS,	SMALL_PULSE_CAD_BITS},	//NVRX_RECORDING_FAIL
	{	CAD_OFF_CAD_BITS,       CONT_ON_CAD_BITS,		CAD_OFF_CAD_BITS	},	//NVRX_RECORDING_FAIL_FOR_ALL
	{	SHORT_LONG_CAD_BITS,	SMALL_PULSE_CAD_BITS,	CAD_OFF_CAD_BITS	}	//NVRX_CPUTEMP_HIGH
};

static	UINT8	accessRespCadPriority[NVRX_RESP_MAX] = 
{
	8,	//NVRX_IDLE_RESP	
	0,	//NVRX_APPL_INIT_RESP
	6,	//NVRX_NET_CONN_RESP
	6,	//NVRX_NET_DISCONN_RESP
	4,	//NVRX_STORAGE_INCOMPLETE_RESP
	4,	//NVRX_STORAGE_FULL_ALT_RESP
	4,	//NVRX_STORAGE_FULL_RESP
	3,	//NVRX_FORMAT_DISK_RESP
	0,	//NVRX_FW_UPG_INPROCESS_RESP
	0,	//NVRX_FW_UPG_FAILED_RESP
	1,	//NVRX_TRG_EVT_RESP
	5,	//NVRX_BKUP_FULL_RESP
	5,	//NVRX_NO_BKUP_RESP
	6,  //NVRX_NORMAL_STATE
	2,  //NVRX_RECORDING_FAIL
	2,	//NVRX_RECORDING_FAIL_FOR_ALL
	0	//NVRX_CPUTEMP_HIGH
};

//******** Function Prototypes *******
static struct proc_dir_entry *entry;
static INT32 GpioOpen(struct inode *inode, struct file *file);
static INT32 GpioClose(struct inode * inode, struct file * file);
static long GpioIoctl(struct file *file, unsigned int cmd, unsigned long arg);
static int SetBuzzerCad(NVRX_RESPONSE_e accessResp);
static int SetBuzzer(UINT8 state);
static int SetAlarmOut(UINT32 data);
static int SetFanOut(UINT32 data);
static int SetUSBPower(UINT32 data);
static int SetLedCad(NVRX_RESPONSE_e accessResp);
static int SetLed(UINT8 state);
static void CheckInputPinStatus(struct timer_list *timer);
static void ProcBuzLedCad(struct timer_list *timer);
static void SyncBuzLedCad(struct timer_list *timer);
static void ChangeInputStatus(INPUT_MAP_e inputMap);
static void UpdateCadPriority(void);
static void FillStructOfLedBuz(NVRX_RESPONSE_e accessResp, BUZZER_LED_e output);
static void SetBuzLedPinStatus(BUZZER_LED_e buzLedPortIdx, BOOL status);
static int phy_power_on_off(int state);
static int phy2_power_on_off(int state);
static int asm_power_on_off(int state);
//******** Global Variables **********
static DEFINE_MUTEX(phyrst_lock);
static DEFINE_MUTEX(phyrst2_lock);
static DEFINE_MUTEX(asmrst_lock);
static DEFINE_MUTEX(buzzer_lock);
static DEFINE_MUTEX(ipdef_lock);
static DEFINE_MUTEX(almout_lock);
static DEFINE_MUTEX(fanctl_lock);
static DEFINE_MUTEX(almin_lock);
static DEFINE_MUTEX(usbpower_lock);
static DEFINE_MUTEX(redled_lock);
static DEFINE_MUTEX(greenled_lock);
static DEFINE_MUTEX(cadsynclock);
static DEFINE_MUTEX(usb_power_lock);
//******** Global Variables **********

//******** Static Variables **********
static dev_t 	 				gpioDevNo;
static struct cdev 				*gpioCdev;
static struct class 			*gpioDevClass;
static struct device 			*MxdevGpio;
static struct timer_list		gpioPollTimerInfo;
static struct timer_list		buzLedCadTimerInfo;
static struct timer_list		buzLedSyncTimerInfo;
static INPUT_STATUS_t			inputStatus;
static INPUT_PIN_ACTIVITY_t		inputPinActivity[MAX_INPUT_MAP];
static BOOL				IsBuzLedFillInProg = FALSE;
static UINT8				CurrentPriority = MAX_PRIORITY;

BUZZER_LED_CAD_t buzLedCadence[MAX_PRIORITY + 1][BUZZER_LED_MAX];
// static BOOL							buzMuteF = FALSE;

//----------- Phy control ---------------
static ssize_t phyrst_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int value = 0;
	mutex_lock(&phyrst_lock);
	sscanf(buf, "%u", &value);
	phy_power_on_off(value);
	mutex_unlock(&phyrst_lock);
	return size;
}

static ssize_t phyrst2_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int value = 0;
	mutex_lock(&phyrst2_lock);
	sscanf(buf, "%u", &value);
	phy2_power_on_off(value);
	mutex_unlock(&phyrst2_lock);
	return size;
}
static const DEVICE_ATTR(phyrst, 0664, NULL, phyrst_value_store);
static const DEVICE_ATTR(phyrst2, 0664, NULL, phyrst2_value_store);

static const struct attribute *phyrst_gpio_attrs[] = {
	&dev_attr_phyrst.attr,
	&dev_attr_phyrst2.attr,
	NULL,
};

static const struct attribute_group phyrst_attr_group = {
	.attrs = (struct attribute **) phyrst_gpio_attrs,
};


//ASM reset control
//----------- ASM control ---------------
static ssize_t asmrst_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int value = 0;
	mutex_lock(&asmrst_lock);
	sscanf(buf, "%u", &value);
	asm_power_on_off(value);
	mutex_unlock(&asmrst_lock);
	return size;
}

static const DEVICE_ATTR(asmrst, 0664, NULL, asmrst_value_store);

static const struct attribute *asmrst_gpio_attrs[] = {
	&dev_attr_asmrst.attr,
	NULL,
};

static const struct attribute_group asmrst_attr_group = {
	.attrs = (struct attribute **) asmrst_gpio_attrs,
};

//-----------BUZZER---------------
static ssize_t buzzer_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int value = 0;
	mutex_lock(&buzzer_lock);
	sscanf(buf, "%u", &value);
	SetBuzzerPin(value);
	mutex_unlock(&buzzer_lock);
	return size;
}

static const DEVICE_ATTR(buzzer, 0664, NULL, buzzer_value_store);

static const struct attribute *buzzer_gpio_attrs[] = {
	&dev_attr_buzzer.attr,
	NULL,
};

static const struct attribute_group buzzer_attr_group = {
	.attrs = (struct attribute **) buzzer_gpio_attrs,
};

//-----------IPDEF---------------
static ssize_t ipdef_value_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	UINT8	value=0;
	int		status = -EINVAL;
	mutex_lock(&ipdef_lock);
	value = gpio_get_value(IP_DEFUALT_PIN);
	status = sprintf(buf, "%d\n", value);
	mutex_unlock(&ipdef_lock);
	return status;
}

static const DEVICE_ATTR(ipdef, 0664,ipdef_value_show, NULL);

static const struct attribute *ipdef_gpio_attrs[] = {
	&dev_attr_ipdef.attr,
	NULL,
};

static const struct attribute_group ipdef_attr_group = {
	.attrs = (struct attribute **) ipdef_gpio_attrs,
};

//-----------ALMOUT---------------
static ssize_t almout_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int value = 0;
	mutex_lock(&almout_lock);
	sscanf(buf, "%u", &value);
	gpio_set_value(ALM_RLY_CNTL_PIN,value);
	mutex_unlock(&almout_lock);
	return size;
}

static const DEVICE_ATTR(almout, 0664, NULL, almout_value_store);

static const struct attribute *almout_gpio_attrs[] = {
	&dev_attr_almout.attr,
	NULL,
};

static const struct attribute_group almout_attr_group = {
	.attrs = (struct attribute **) almout_gpio_attrs,
};

//-----------FANCTL---------------
static ssize_t fanctl1_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int value = 0;
	mutex_lock(&fanctl_lock);
	sscanf(buf, "%u", &value);
	SetFan1CtrlPin(value);
	mutex_unlock(&fanctl_lock);
	return size;
}

static ssize_t fanctl2_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int value = 0;
	mutex_lock(&fanctl_lock);
	sscanf(buf, "%u", &value);
	SetFan2CtrlPin(value);
	mutex_unlock(&fanctl_lock);
	return size;
}

static const DEVICE_ATTR(fanctl1, 0664, NULL, fanctl1_value_store);
static const DEVICE_ATTR(fanctl2, 0664, NULL, fanctl2_value_store);

static const struct attribute *fanctl_gpio_attrs[] = {
	&dev_attr_fanctl1.attr,
	&dev_attr_fanctl2.attr,
	NULL,
};

static const struct attribute_group fanctl_attr_group = {
	.attrs = (struct attribute **) fanctl_gpio_attrs,
};

//-----------ALMIN---------------
static ssize_t almin1_value_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	UINT8	value;
	int		status = -EINVAL;
	mutex_lock(&almin_lock);
	value = gpio_get_value(ALM_IN1_PIN);
	status = sprintf(buf, "%d\n", value);
	mutex_unlock(&almin_lock);
	return status;
}

//-----------ALMIN---------------
static ssize_t almin2_value_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	UINT8	value;
	int		status = -EINVAL;
	mutex_lock(&almin_lock);
 	value = gpio_get_value(ALM_IN2_PIN);
	status = sprintf(buf, "%d\n", value);
	mutex_unlock(&almin_lock);
	return status;
}

static const DEVICE_ATTR(almin1, 0664,almin1_value_show, NULL);
static const DEVICE_ATTR(almin2, 0664,almin2_value_show, NULL);

static const struct attribute *almin_gpio_attrs[] = {
	&dev_attr_almin1.attr,
	&dev_attr_almin2.attr,
	NULL,
};

static const struct attribute_group almin_attr_group = {
	.attrs = (struct attribute **) almin_gpio_attrs,
};

//-----------USBPOWER---------------
/*static ssize_t usb3power_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int value = 0;
	mutex_lock(&usbpower_lock);
	sscanf(buf, "%u", &value);
	HISI_WRITE_REG(USB3_PWREN_DATA_BASE_ADDR, value << USB3_PWREN_NUMBER);
	mutex_unlock(&usbpower_lock);
	return size;
}
*/
static ssize_t usb2power1_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int value = 0;
	mutex_lock(&usbpower_lock);
	sscanf(buf, "%u", &value);
	SetUsbCtrlPin(value);
	mutex_unlock(&usbpower_lock);
	return size;
}
/*
static ssize_t usb2power2_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int value = 0;
	mutex_lock(&usbpower_lock);
	sscanf(buf, "%u", &value);
	HISI_WRITE_REG(USB2_PWREN2_DATA_BASE_ADDR, value << USB2_PWREN2_NUMBER);
	mutex_unlock(&usbpower_lock);
	return size;
}*/

//static const DEVICE_ATTR(usb3power, 0666, NULL, usb3power_value_store);
static const DEVICE_ATTR(usb2power1, 0664, NULL, usb2power1_value_store);
//static const DEVICE_ATTR(usb2power2, 0666, NULL, usb2power2_value_store);

static const struct attribute *usbpower_gpio_attrs[] = {
	//&dev_attr_usb3power.attr,
	&dev_attr_usb2power1.attr,
	//&dev_attr_usb2power2.attr,
	NULL,
};

static const struct attribute_group usbpower_attr_group = {
	.attrs = (struct attribute **) usbpower_gpio_attrs,
};

//-----------REDLED---------------
static ssize_t redled_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int value = 0;
	mutex_lock(&redled_lock);
	sscanf(buf, "%u", &value);
	SetRedLedPin(value);
	mutex_unlock(&redled_lock);
	return size;
}

static const DEVICE_ATTR(redled, 0664, NULL, redled_value_store);

static const struct attribute *redled_gpio_attrs[] = {
	&dev_attr_redled.attr,
	NULL,
};

static const struct attribute_group redled_attr_group = {
	.attrs = (struct attribute **) redled_gpio_attrs,
};

//-----------GREENLED---------------
static ssize_t greenled_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned int value = 0;
	mutex_lock(&greenled_lock);
	sscanf(buf, "%u", &value);
	SetGreenLedPin(value);
	mutex_unlock(&greenled_lock);
	return size;
}

static const DEVICE_ATTR(greenled, 0664, NULL, greenled_value_store);

static const struct attribute *greenled_gpio_attrs[] = {
	&dev_attr_greenled.attr,
	NULL,
};

static const struct attribute_group greenled_attr_group = {
	.attrs = (struct attribute **) greenled_gpio_attrs,
};
//******** Function Definations ******
/*static unsigned int ReadCoreTemp(void)
{
	UINT32 tempe=0, tcode[MAX_TCODE];
	unsigned char count = 0;
	
	tcode[0] = HISI_READ_REG(PERI_PMC70_BASE_ADDR);
	tcode[1] = GET_TCODE1(tcode[0]);
	tcode[0] = GET_TCODE0(tcode[0]);
	tcode[2] = HISI_READ_REG(PERI_PMC71_BASE_ADDR);
	tcode[3] = GET_TCODE1(tcode[2]);
	tcode[2] = GET_TCODE0(tcode[2]);
	tcode[4] = HISI_READ_REG(PERI_PMC72_BASE_ADDR);
	tcode[5] = GET_TCODE1(tcode[4]);
	tcode[4] = GET_TCODE0(tcode[4]);
	tcode[6] = HISI_READ_REG(PERI_PMC73_BASE_ADDR);
	tcode[7] = GET_TCODE1(tcode[6]);
	tcode[6] = GET_TCODE0(tcode[6]);

	for(count = 0; count < MAX_TCODE; count++)
		tempe += tcode[count];

	tempe = tempe / MAX_TCODE;

//	NOTE: T = [(tsensor_temp_code − 125)/806] x 165 − 40
	tempe = tempe - 125;
	tempe = tempe * 165;
	tempe = tempe / 806;
	tempe = tempe - 40;
	return tempe;
}*/

// #define USB_PWR_TMR_LOAD_VAL   		30
// 
// typedef enum {
// 	GPIO_USB2_PWREN1_BIT_POS 	= 4,
// 	GPIO_USB2_PWREN2_BIT_POS 	= 2,
// 	GPIO_USB3_PWREN_BIT_POS 	= 1,
// 	GPIO_USB_ALL_POWER_BIT_POS	= (GPIO_USB3_PWREN_BIT_POS | GPIO_USB2_PWREN2_BIT_POS | GPIO_USB2_PWREN1_BIT_POS),
// }USB_GPIO_BIT_POS;
// 
// typedef struct {
// 	struct timer_list	delay_timer;
// 	int bit_status;
// 	bool is_tmr_running;
// } usb_power_on_t;
// usb_power_on_t usb_power_on_info;
// 
// static void set_usb_power_bit (int pos )
// {
// 	mutex_lock(&usb_power_lock);
// 	usb_power_on_info.bit_status |= pos;
// 	mutex_unlock(&usb_power_lock);
// }
// 
// static void clear_usb_power_bit(int pos)
// {
// 	mutex_lock(&usb_power_lock);
// 	usb_power_on_info.bit_status &= (~pos);
// 	mutex_unlock(&usb_power_lock);
// }
// 
// void usb_pwr_tmr_control(void)
// {
// 	if (usb_power_on_info.bit_status && (!usb_power_on_info.is_tmr_running)) {
// 		/* reload timer because timer is not running and usb on cmd given*/
// 		mod_timer(&usb_power_on_info.delay_timer, (unsigned long)(jiffies + USB_PWR_TMR_LOAD_VAL) );
// 		usb_power_on_info.is_tmr_running = true;
// 	}
// }
// 
// void usb_power_set_gpio_tmr_callback (unsigned long data)
// {
// 	if ( usb_power_on_info.bit_status & GPIO_USB2_PWREN1_BIT_POS) {
// 		// set usb21 gpio
// 		HISI_WRITE_REG(USB2_PWREN1_DATA_BASE_ADDR, 0);
// 		clear_usb_power_bit(GPIO_USB2_PWREN1_BIT_POS);
// 		printk("USB2_PWREN1_DATA_BASE_ADDR \n");
// 	}
// 	else if ( usb_power_on_info.bit_status & GPIO_USB2_PWREN2_BIT_POS) {
// 		// set usb22 gpio
// 		HISI_WRITE_REG(USB2_PWREN2_DATA_BASE_ADDR, 0);
// 		clear_usb_power_bit(GPIO_USB2_PWREN2_BIT_POS);
// 		printk("USB2_PWREN2_DATA_BASE_ADDR \n");
// 	}
// 	else if ( usb_power_on_info.bit_status & GPIO_USB3_PWREN_BIT_POS) {
// 		// set usb3 gpio
// 		HISI_WRITE_REG(USB3_PWREN_DATA_BASE_ADDR, 0);
// 		clear_usb_power_bit(GPIO_USB3_PWREN_BIT_POS);
// 		printk("USB3_PWREN_DATA_BASE_ADDR \n");
// 	}
// 
// 	if ( usb_power_on_info.bit_status ) {
// 		/* Reload timer for pending */
// 		mod_timer(&usb_power_on_info.delay_timer, (unsigned long)(jiffies + USB_PWR_TMR_LOAD_VAL) );
// 	} else {
// 		/* no pending bits */
// 		usb_power_on_info.is_tmr_running = false;
// 	}
// 	printk("usb_power_on_info.bit_status: %x \n", usb_power_on_info.bit_status);
// }


//*****************************************************************************
//	SetBuzzerCad()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is to set and clear buzzer.
//*****************************************************************************
static int SetBuzzerCad(NVRX_RESPONSE_e accessResp)
{
	if(accessResp > NVRX_RESP_MAX)
	{
		return -1;
	}
	FillStructOfLedBuz(accessResp, BUZZER);
	return 0;
}

//*****************************************************************************
//	phy_power_on_off()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is to set and clear phy ic.
//*****************************************************************************
int phy_power_on_off(int state)
{
    static BOOL is_pin_init = false;
    if (false == is_pin_init) {
          
	gpio_set_value(PHY0_RST_PIN,LOW);
	mdelay(20);
	gpio_set_value(PHY0_RST_PIN,HIGH);
        is_pin_init = true;
    }
    return 0;
}


//*****************************************************************************
//	phy2_power_on_off()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is to set and clear phy ic.
//*****************************************************************************
int phy2_power_on_off(int state)
{
    static BOOL is_pin_init = false;
    if (false == is_pin_init) {
        
	gpio_set_value(PHY1_RST_PIN,LOW);
	mdelay(20);
	gpio_set_value(PHY1_RST_PIN,HIGH);
        is_pin_init = true;
    }
    return 0;
}

//*****************************************************************************
//	asm_power_on_off()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is to set and clear asm ic.
//*****************************************************************************
int asm_power_on_off(int state)
{
    static BOOL is_pin_init = false;
    if (false == is_pin_init) {
          
	gpio_set_value(ASM_RST_PIN ,LOW);
	mdelay(5);
	gpio_set_value(ASM_RST_PIN,HIGH);
	mdelay(5);
        is_pin_init = true;
    }
    return 0;
}
//*****************************************************************************
//	SetBuzzer()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is to set and clear buzzer.
//*****************************************************************************
static int SetBuzzer(UINT8 state)
{
	int ret = 0;
	switch(state)
	{
		case BUZZER_SET : SetBuzzerPin(ACTIVE); break;

		case BUZZER_CLEAR: SetBuzzerPin(INACTIVE); break;

		default: ret = -1;
				break;
	}
	return ret;
}

//*****************************************************************************
//	ClearCadence()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is to set and clear buzzer.
//*****************************************************************************
static int ClearCadence(BUZZER_LED_RESP_t __BuzLedResp)
{
	UINT8 AcPro = 0;
	BUZZER_LED_e output;
	
	IsBuzLedFillInProg = TRUE;
	if(__BuzLedResp.LedCadResp < NVRX_RESP_MAX)
	{
		for(output = RED_LED ; output < BUZZER_LED_MAX ; output++ )
		{
			AcPro = accessRespCadPriority[__BuzLedResp.LedCadResp];
			if(buzLedCadence[AcPro][output].accessResp == __BuzLedResp.LedCadResp)
			{
				buzLedCadence[AcPro][output].accessResp = __BuzLedResp.LedCadResp;
				buzLedCadence[AcPro][output].buzLedCad = CAD_OFF;
				buzLedCadence[AcPro][output].cadBits = CAD_OFF_CAD_BITS;
				buzLedCadence[AcPro][output].cadPosition = 0;
				if(buzLedCadence[AcPro][BUZZER].buzLedCad == CAD_OFF)
				{
					buzLedCadence[AcPro][output].noOfCycles = 1;
					buzLedCadence[AcPro][BUZZER].noOfCycles = 0;
				}
				else
				{
					buzLedCadence[AcPro][output].noOfCycles = 1;
				}
			}
		}
	}
	else if(__BuzLedResp.LedCadResp == NVRX_RESP_ALL)
	{
		for(AcPro = 0 ; AcPro < MAX_PRIORITY ; AcPro++ )
		{
			for(output = RED_LED ; output < BUZZER_LED_MAX ; output++ )
			{
				buzLedCadence[AcPro][output].buzLedCad = CAD_OFF;
				buzLedCadence[AcPro][output].cadBits = CAD_OFF_CAD_BITS;
				buzLedCadence[AcPro][output].cadPosition = 0;
				if(buzLedCadence[AcPro][BUZZER].buzLedCad == CAD_OFF)
				{
					buzLedCadence[AcPro][output].noOfCycles = 1;
					buzLedCadence[AcPro][BUZZER].noOfCycles = 0;
				}
				else
				{
					buzLedCadence[AcPro][output].noOfCycles = 1;
				}
			}
		}
	}
	else if(__BuzLedResp.LedCadResp == NVRX_RESP_PRESENT)
	{
		for(output = RED_LED ; output < BUZZER_LED_MAX ; output++ )
		{
		  buzLedCadence[CurrentPriority][output].buzLedCad = CAD_OFF;
		  buzLedCadence[CurrentPriority][output].cadBits = CAD_OFF_CAD_BITS;
		  buzLedCadence[CurrentPriority][output].cadPosition = 0;
		  buzLedCadence[CurrentPriority][output].noOfCycles = 0;
		}
	}
	
	if(__BuzLedResp.BuzCadResp < NVRX_RESP_MAX)
	{
		AcPro = accessRespCadPriority[__BuzLedResp.BuzCadResp];
		if(buzLedCadence[AcPro][BUZZER].accessResp == __BuzLedResp.BuzCadResp)
		{
			buzLedCadence[AcPro][BUZZER].accessResp = __BuzLedResp.BuzCadResp;
			buzLedCadence[AcPro][BUZZER].buzLedCad = CAD_OFF;
			buzLedCadence[AcPro][BUZZER].cadBits = CAD_OFF_CAD_BITS;
			buzLedCadence[AcPro][BUZZER].cadPosition = 0;
			if((buzLedCadence[AcPro][RED_LED].buzLedCad == CAD_OFF) && (buzLedCadence[AcPro][GREEN_LED].buzLedCad == CAD_OFF))
			{
			  buzLedCadence[AcPro][BUZZER].noOfCycles = 1;
			  buzLedCadence[AcPro][RED_LED].noOfCycles = 0;
			  buzLedCadence[AcPro][GREEN_LED].noOfCycles = 0;
			}
			else
			{
				buzLedCadence[AcPro][BUZZER].noOfCycles = 1;
			}
		}
	}
	else if(__BuzLedResp.BuzCadResp == NVRX_RESP_ALL)
	{
		for(AcPro = 0 ; AcPro < MAX_PRIORITY ; AcPro++ )
		{
			buzLedCadence[AcPro][BUZZER].buzLedCad = CAD_OFF;
			buzLedCadence[AcPro][BUZZER].cadBits = CAD_OFF_CAD_BITS;
			buzLedCadence[AcPro][BUZZER].cadPosition = 0;
			if((buzLedCadence[AcPro][RED_LED].buzLedCad == CAD_OFF) && (buzLedCadence[AcPro][GREEN_LED].buzLedCad == CAD_OFF))
			{
			  buzLedCadence[AcPro][BUZZER].noOfCycles = 1;
			  buzLedCadence[AcPro][RED_LED].noOfCycles = 0;
			  buzLedCadence[AcPro][GREEN_LED].noOfCycles = 0;
			}
			else
			{
				buzLedCadence[AcPro][BUZZER].noOfCycles = 1;
			}
		}
	}
	else if(__BuzLedResp.BuzCadResp == NVRX_RESP_PRESENT)
	{
		buzLedCadence[CurrentPriority][BUZZER].buzLedCad = CAD_OFF;
		buzLedCadence[CurrentPriority][BUZZER].cadBits = CAD_OFF_CAD_BITS;
		buzLedCadence[CurrentPriority][BUZZER].cadPosition = 0;
		buzLedCadence[CurrentPriority][BUZZER].noOfCycles = 0;
	}
	
	UpdateCadPriority();
	buzLedSyncTimerInfo.expires  = jiffies + msecs_to_jiffies(BUZ_LED_SYNC_TMR_CNT); //10 mSec
	mod_timer(&buzLedSyncTimerInfo, (unsigned long) buzLedSyncTimerInfo.expires);

	return 0;
}
//*****************************************************************************
//	SetAlarmOut()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is to set and clear alarm relay.
//*****************************************************************************
static int SetAlarmOut(UINT32 data)
{
	int ret = 0;
	switch(data)
	{
		case ALMOUT_SET: SetAlmRelayCtrlPin(ACTIVE);
				 break;

		case ALMOUT_CLEAR: SetAlmRelayCtrlPin(INACTIVE);
				   break;

		default: ret = -1;
				break;
	}
	return ret;
}

//*****************************************************************************
//	SetFanOut()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is to set and clear fan.
//*****************************************************************************
static int SetFanOut(UINT32 data)
{
	int ret = 0;
	switch(data)
	{
		case FAN1_ON: SetFan1CtrlPin(ACTIVE); 
			      break;

		case FAN2_ON: SetFan2CtrlPin(ACTIVE); 
			      break;

		case ALLFAN_ON:	SetFan1CtrlPin(ACTIVE);
				SetFan2CtrlPin(ACTIVE);
				break;

		case FAN1_OFF:	SetFan1CtrlPin(INACTIVE); 
				break;

		case FAN2_OFF: SetFan2CtrlPin(INACTIVE); 
				break;

		case ALLFAN_OFF:SetFan1CtrlPin(INACTIVE);
				SetFan2CtrlPin(INACTIVE);
				break;

		default: ret = -1;
				break;
	}
	return ret;
}

//*****************************************************************************
//	SetLedCad()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is to set and clear alarm led.
//*****************************************************************************
static int SetLedCad(NVRX_RESPONSE_e accessResp)
{
	if(accessResp >= NVRX_RESP_MAX)
	{
		return -1;
	}
	FillStructOfLedBuz(accessResp, RED_LED);
	FillStructOfLedBuz(accessResp, GREEN_LED);
	return 0;
}

//*****************************************************************************
//	SetLed()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is to set and clear alarm led.
//*****************************************************************************
static int SetLed(UINT8 state)
{
	int ret = 0;
	switch(state)
	{
		case REDLED_ON: SetRedLedPin(ACTIVE); break;
		
		case REDLED_OFF:SetRedLedPin(INACTIVE); break;
		
		case GREENLED_ON:SetGreenLedPin(ACTIVE); break;
		
		case GREENLED_OFF:SetGreenLedPin(INACTIVE); break;

		case ORANGELED_ON:SetRedLedPin(ACTIVE);
				  SetGreenLedPin(ACTIVE);
				  break;
		
		case ORANGELED_OFF:SetRedLedPin(INACTIVE);
				   SetGreenLedPin(INACTIVE);
				   break;

		default:ret = -1;
			break;
	}
	return ret;
}

//*****************************************************************************
//	SetUSBPower()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is to set and clear USB Power.
//*****************************************************************************
static int SetUSBPower(UINT32 data)
{
	int ret = 0;
	switch(data)
	{
		/*case USB3POWER_ON:		set_usb_power_bit(GPIO_USB3_PWREN_BIT_POS);

								break;
		
		*/
		case USB2POWER1_ON: SetUsbCtrlPin(ACTIVE);
				    break;

		/*case USB2POWER2_ON:		set_usb_power_bit(GPIO_USB2_PWREN2_BIT_POS);

								break;*/
		
		case ALLUSBPOWER_ON:
				SetUsbCtrlPin(ACTIVE);
                                break;

		/*case USB3POWER_OFF:
								clear_usb_power_bit(GPIO_USB3_PWREN_BIT_POS);
								HISI_WRITE_REG(USB3_PWREN_DATA_BASE_ADDR, 1 << USB3_PWREN_NUMBER);
								break;
								
		*/

		case USB2POWER1_OFF: SetUsbCtrlPin(INACTIVE);
				     break;

		/*case USB2POWER2_OFF:	clear_usb_power_bit(GPIO_USB2_PWREN2_BIT_POS);
                                HISI_WRITE_REG(USB2_PWREN2_DATA_BASE_ADDR, 1 << USB2_PWREN2_NUMBER);
								break;*/

		case USBALLPOWER_OFF:
				    SetUsbCtrlPin(INACTIVE);
				    break;
		

		default: ret = -1;
				break;
	}

	//usb_pwr_tmr_control();
	return ret;
}

//*****************************************************************************
//	GetHWInputStatus()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is to get hardware input status.
//*****************************************************************************
static UINT8 GetHWInputStatus(INPUT_MAP_e input)
{
	UINT8 val = 0;
	
	switch(input)
	{
		case IPDEF_INPUT: val = gpio_get_value(IP_DEFUALT_PIN); break;

		case ALMIN1_INPUT: val = gpio_get_value(ALM_IN1_PIN); break;

		case ALMIN2_INPUT: val = gpio_get_value(ALM_IN2_PIN); break;

		default:
			break;
	}
	return val;
}


//*****************************************************************************
//	UpdateCadPriority()
//	Param: 
//		IN :	
//				UINT8 buzLedPortIdx
//		OUT:	None
//	Returns:	None
//	Description:
//				It plays next highest priority cadence on given port. 
//*****************************************************************************
static void UpdateCadPriority(void)
{
	UINT8 JobPriority = 0;
	UINT8 RespIndex = 0;
	UINT8 exitF = FALSE;
	
	for(JobPriority = 0 ; JobPriority <= MAX_PRIORITY ; JobPriority++ )
	{
		for(RespIndex = 0; RespIndex < BUZZER_LED_MAX ; RespIndex++ )
		{
			if(buzLedCadence[JobPriority][RespIndex].noOfCycles > 0)
			{
				exitF = TRUE;
				CurrentPriority = JobPriority;
				break;
			}
		}
		
		if(exitF == TRUE)
			break;
	}
}

//*****************************************************************************
//	FillStructOfLedBuz()
//	Param: 
//		IN :	NVRX_RESPONSE_e
//				UINT8 output
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is common function use to set Buzzer and LED 
//				access response.
//*****************************************************************************
static void FillStructOfLedBuz(NVRX_RESPONSE_e accessResp, BUZZER_LED_e output)
{
	UINT8 AcPro = 0;
	
	if( (accessResp < NVRX_RESP_MAX) && (output < BUZZER_LED_MAX) )
	{
		IsBuzLedFillInProg = TRUE;	
		AcPro = accessRespCadPriority[accessResp];

		buzLedCadence[AcPro][output].accessResp = accessResp;
		buzLedCadence[AcPro][output].buzLedCad = accessRespCadMap[accessResp][output];
		if(output == BUZZER)
		{
			buzLedCadence[AcPro][output].noOfCycles = accessRespCadCycles[accessResp][BUZZER_CYCLES];
		}
		else
		{
			buzLedCadence[AcPro][output].noOfCycles = accessRespCadCycles[accessResp][output];
		}
		buzLedCadence[AcPro][output].cadBits = accessRespCadBits[accessResp][output];
		buzLedCadence[AcPro][output].cadPosition = 0;
		UpdateCadPriority();
		buzLedSyncTimerInfo.expires  = jiffies + msecs_to_jiffies(BUZ_LED_SYNC_TMR_CNT); //10 mSec
		mod_timer(&buzLedSyncTimerInfo, (unsigned long) buzLedSyncTimerInfo.expires);
	}
}

//*****************************************************************************
//	SyncBuzLedCad()
//	Param: 
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function is called at every 10 msec from kernel timer.
//*****************************************************************************
static void SyncBuzLedCad(struct timer_list *timer)
{
	IsBuzLedFillInProg = FALSE;
}

//*****************************************************************************
//	ProcBuzLedCad()
//	Param: 
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This function is called at every 500 msec from kernel timer.
//*****************************************************************************
static void ProcBuzLedCad(struct timer_list *timer)
{
	BUZZER_LED_e	buzLedPortIdx;
	BOOL		IsNextCad = TRUE;
	
	if(IsBuzLedFillInProg == FALSE)
	{
	for(buzLedPortIdx = 0; buzLedPortIdx < BUZZER_LED_MAX; buzLedPortIdx++)
	{
		if(buzLedCadence[CurrentPriority][buzLedPortIdx].noOfCycles > 0)
		{	
			
			if(buzLedCadence[CurrentPriority][buzLedPortIdx].cadPosition >= buzLedCadence[CurrentPriority][buzLedPortIdx].cadBits)
			{	
				buzLedCadence[CurrentPriority][buzLedPortIdx].cadPosition = 0;
				
				// For NET CONN/NET DISCONN cadence we need to play INFINITE times.
				if(buzLedCadence[CurrentPriority][buzLedPortIdx].noOfCycles != INFINITE_CYCLE)
				{
					buzLedCadence[CurrentPriority][buzLedPortIdx].noOfCycles--;
				}
			}
			
			// here, for perticular responce if buzzer is set to CAD_OFF then only led cadence needs to play.
			if(buzLedCadence[CurrentPriority][buzLedPortIdx].noOfCycles > 0)
			{	
				if(buzLedCadence[CurrentPriority][buzLedPortIdx].buzLedCad & (1 << buzLedCadence[CurrentPriority][buzLedPortIdx].cadPosition))	
				{
					SetBuzLedPinStatus(buzLedPortIdx, TRUE);
				}
				else
				{
					SetBuzLedPinStatus(buzLedPortIdx, FALSE);
				}
				
				buzLedCadence[CurrentPriority][buzLedPortIdx].cadPosition++;
			}
		}
	}

	for(buzLedPortIdx = 0; buzLedPortIdx < BUZZER_LED_MAX; buzLedPortIdx++)
	{
		// if Response cadence is not yet completed. do not play next cadence
		if(buzLedCadence[CurrentPriority][buzLedPortIdx].noOfCycles > 0)
		{
			IsNextCad = FALSE;
			break;
		}
		else{
			buzLedCadence[CurrentPriority][buzLedPortIdx].cadPosition = 0;
		}
	}
	
	// if Response is completed then play lower priority cadence
	if(IsNextCad == TRUE)
	{
		UpdateCadPriority();
	}
	// Re-start timer
	buzLedCadTimerInfo.expires = jiffies + msecs_to_jiffies(BUZ_LED_TMR_CNT);
	}
	else
	{
		buzLedCadTimerInfo.expires = jiffies + msecs_to_jiffies(BUZ_LED_SYNC_TMR_CNT);
	}
	mod_timer(&buzLedCadTimerInfo,(unsigned long) buzLedCadTimerInfo.expires);
}

//*****************************************************************************
//	SetBuzLedPinStatus()
//	Param: 
//		IN :	buzLedId- index of buzzer/led port
//		OUT:	status	- ACTIVE/INACTIVE
//	Returns:	None
//	Description:
//				This Function sets Led/buzzer pin status as per given
//*****************************************************************************
static void SetBuzLedPinStatus(BUZZER_LED_e buzLedPortIdx, BOOL status)
{
	switch(buzLedPortIdx)
	{
		default:
		{
			break;
		}
			
		case RED_LED:
		{
			SetRedLedPin(status);
			break;
		} 		
		case GREEN_LED:
		{
			SetGreenLedPin(status);
			break;
		}
		case BUZZER:
		{
			SetBuzzerPin(status);
			break;
		}  
	}
}
//*****************************************************************************
//	CheckInputPinStatus()
//	Param: 
//		IN :	data
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is call back function of GPIO poll timer. 
//				It is being called at every 40 mSec.
//*****************************************************************************
static void CheckInputPinStatus(struct timer_list *timer)
{
	INPUT_MAP_e	cnt;
	BOOL 		statusChangedF[MAX_INPUT_MAP];
	
	for(cnt = 0; cnt < MAX_INPUT_MAP; cnt++)
	{
		statusChangedF[cnt] = FALSE;
		
		// Store sample value @ last bit
		inputPinActivity[cnt].sampleVal = ((inputPinActivity[cnt].sampleVal << 1) 
								| (GetHWInputStatus(cnt) & 1));

		// If defined no. of consecutive samples are HIGH then only consider pin change to HIGH	
		if((inputPinActivity[cnt].sampleVal & INPUT_SAMPLE_MASK_VAL) == ALL_SAMPLES_HIGH)
		{
			if( (inputPinActivity[cnt].status == PIN_LOW) || (inputPinActivity[cnt].status == PIN_UNKNOWN) )
			{	
				inputPinActivity[cnt].status = PIN_HIGH;
				statusChangedF[cnt] = TRUE;
			}
		}
		// If defined no. of consecutive samples are LOW then only consider pin change to LOW
		else if((inputPinActivity[cnt].sampleVal & INPUT_SAMPLE_MASK_VAL) == ALL_SAMPLES_LOW)
		{	
			if( (inputPinActivity[cnt].status == PIN_HIGH) || (inputPinActivity[cnt].status == PIN_UNKNOWN) )
			{	
				inputPinActivity[cnt].status = PIN_LOW;
				statusChangedF[cnt] = TRUE;
			}
		}
	}	
	
	for(cnt = 0; cnt < MAX_INPUT_MAP; cnt++)
	{	
		if(statusChangedF[cnt] == TRUE)
		{		
			// Physical change on PIN confirmed, Now change logical status of the pin		
			ChangeInputStatus(cnt);
		}	
	}
	//Reload GPIO scan timer
	gpioPollTimerInfo.expires = jiffies + msecs_to_jiffies(GPIO_POLL_CNT);
	mod_timer(&gpioPollTimerInfo, (unsigned long) gpioPollTimerInfo.expires);
}

//*****************************************************************************
//	ChangeInputStatus()
//	Param: 
//		IN :	inputMap
//		OUT:	None
//	Returns:	None
//	Description:
//				This function updates physical status of particular pin to logical
//				status structure.
//*****************************************************************************
static void ChangeInputStatus(INPUT_MAP_e inputMap)
{
	if(inputPinActivity[inputMap].status == PIN_LOW)
	{
		inputStatus.InputPinStatus[inputMap] = INPUT_INACTIVE;
	}
	else
	{
		inputStatus.InputPinStatus[inputMap] = INPUT_ACTIVE;
	}
}

// File Operation Related Structure We Support open,ioctl,close
static struct file_operations GpioFops = 
{
	open:				GpioOpen,
	unlocked_ioctl:			GpioIoctl,
	release:			GpioClose
};
/*
static int tsensor_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "CoreTemp = %uc\n",ReadCoreTemp());
	return 0;
}

static int tsensor_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, tsensor_proc_show, NULL);
}

static const struct file_operations tsensor_proc_fops = {
	.open		= tsensor_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release
};*/

//*****************************************************************************
//	registerGPIOPin()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is called when GpioModInit function to Intialize Gpio.
//*****************************************************************************

static void registerGPIOPin(void)
{
	// Input pins: arguments: pin no, pullup 
	gpio_request(IP_DEFUALT_PIN, "ipdefault");
	gpio_direction_input(IP_DEFUALT_PIN);
	
	gpio_request(ALM_IN1_PIN, "alarmin1");
	gpio_direction_input(ALM_IN1_PIN);

	gpio_request(ALM_IN2_PIN, "alarmin2");
	gpio_direction_input(ALM_IN2_PIN);
	
	gpio_request(CARD_TYPE_0_PIN, "cardtype0");
	gpio_direction_input(CARD_TYPE_0_PIN);
	
	gpio_request(CARD_TYPE_1_PIN, "cardtype1");
	gpio_direction_input(CARD_TYPE_1_PIN);
	
	gpio_request(CARD_TYPE_2_PIN, "cardtype2");
	gpio_direction_input(CARD_TYPE_2_PIN);
	
	gpio_request(CARD_TYPE_3_PIN, "cardtype3");
	gpio_direction_input(CARD_TYPE_3_PIN);
	
	gpio_request(BOARD_VER_0_PIN, "boardver0");
	gpio_direction_input(BOARD_VER_0_PIN);
	
	gpio_request(BOARD_VER_1_PIN, "boardver1");
	gpio_direction_input(BOARD_VER_1_PIN);
	
	gpio_request(BOARD_VER_2_PIN, "boardver2");
	gpio_direction_input(BOARD_VER_2_PIN);
	
	gpio_request(BOARD_VER_3_PIN, "boardver3");
	gpio_direction_input(BOARD_VER_3_PIN);
	
	gpio_request(ALM_RLY_CNTL_PIN, "alarmout");
	gpio_direction_output(ALM_RLY_CNTL_PIN,0);
	
	gpio_request(USB2_PWREN0_PIN, "usbpwrctrl");
	gpio_direction_output(USB2_PWREN0_PIN,0);
	
	gpio_request(GREENLED_GPIO_PIN, "greenled");
	gpio_direction_output(GREENLED_GPIO_PIN,0);
	
	gpio_request(REDLED_GPIO_PIN, "redled");
	gpio_direction_output(REDLED_GPIO_PIN,0);
	
	gpio_request(BUZZER_GPIO_PIN, "buzzerled");
	gpio_direction_output(BUZZER_GPIO_PIN,0);
	
	gpio_request(ASM_RST_PIN, "asmrst");
	gpio_direction_output(ASM_RST_PIN,1);
	
	gpio_request(FAN_CTL1_PIN, "fanctrl1");
	gpio_direction_output(FAN_CTL1_PIN,1);
	
	gpio_request(FAN_CTL2_PIN, "fanctrl2");
	gpio_direction_output(FAN_CTL2_PIN,1);
	
	gpio_request(PHY0_RST_PIN, "phyrst0");
	gpio_direction_output(PHY0_RST_PIN,1);
	gpio_request(PHY1_RST_PIN, "phyrst1");
	gpio_direction_output(PHY1_RST_PIN,1);
}
//*****************************************************************************
//	GpioModInit()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is called when insmod command used. Intialize Gpio.
//*****************************************************************************
INT32 GpioModInit(void)
{
	INT32 ret;
	UINT8 JobPriority = 0;
	UINT8 RespIndex = 0;
	
	unsigned int pinNum = 0;
	
	printk("\n*******MxGPIO driver: last Modified On %s at %s\n", __DATE__, __TIME__);



   /* Comment due to timer callback not required in Rockchip based NVR */
   /* init_timer(&usb_power_on_info.delay_timer);
      usb_power_on_info.delay_timer.data = 0;
      usb_power_on_info.delay_timer.function = usb_power_set_gpio_tmr_callback;
      usb_power_on_info.delay_timer.expires = (jiffies + USB_PWR_TMR_LOAD_VAL);
    
    */
    
	ret = alloc_chrdev_region(&gpioDevNo, 0, 1, GPIO_DRV_NAME);
	if(ret < 0)
	{
		goto remove_chrdev_region;
	}
	//entry = proc_create("coretemp", 0, NULL, &tsensor_proc_fops);
	// Register char Device
	gpioCdev = cdev_alloc(); //Get allocated cdev structure

	gpioCdev->owner = THIS_MODULE;
	gpioCdev->ops = &GpioFops;

	// Adding into kernel
	ret = cdev_add(gpioCdev, gpioDevNo, 1);
	if(ret < 0)
	{
		printk("Failed to Register GPIO driver\n");
		goto remove_cdev;
	}

	gpioDevClass = class_create(THIS_MODULE, GPIO_DRV_NAME);
	
	MxdevGpio = device_create(gpioDevClass, NULL, gpioDevNo, NULL, MXGPIO_DEVICE_NAME);
	
	ret = sysfs_create_group(&MxdevGpio->kobj,&buzzer_attr_group);
	if(ret < 0)
	{
		printk("Failed to create sysfs buzzer GPIO\n");
		goto remove_sysfs_buzzer;
	}

	ret = sysfs_create_group(&MxdevGpio->kobj,&phyrst_attr_group);
	if(ret < 0)
	{
		printk("Failed to create sysfs phyrst GPIO\n");
		goto remove_sysfs_phyrst;
	}
	
	ret = sysfs_create_group(&MxdevGpio->kobj,&asmrst_attr_group);
	if(ret < 0)
	{
		printk("Failed to create sysfs asmrst GPIO\n");
		goto remove_sysfs_asmrst;
	}
	
	ret = sysfs_create_group(&MxdevGpio->kobj,&ipdef_attr_group);
	if(ret < 0)
	{
		printk("Failed to create sysfs ipdef GPIO\n");
		goto remove_sysfs_ipdef;
	}
	ret = sysfs_create_group(&MxdevGpio->kobj,&almout_attr_group);
	if(ret < 0)
	{
		printk("Failed to create sysfs almout GPIO\n");
		goto remove_sysfs_almout;
	}
	ret = sysfs_create_group(&MxdevGpio->kobj,&fanctl_attr_group);
	if(ret < 0)
	{
		printk("Failed to create sysfs fanctl GPIO\n");
		goto remove_sysfs_fanctl;
	}
	ret = sysfs_create_group(&MxdevGpio->kobj,&almin_attr_group);
	if(ret < 0)
	{
		printk("Failed to create sysfs almin GPIO\n");
		goto remove_sysfs_almin;
	}
	ret = sysfs_create_group(&MxdevGpio->kobj,&usbpower_attr_group);
	if(ret < 0)
	{
		printk("Failed to create sysfs usbpower GPIO\n");
		goto remove_sysfs_usbpower;
	}
	ret = sysfs_create_group(&MxdevGpio->kobj,&redled_attr_group);
	if(ret < 0)
	{
		printk("Failed to create sysfs redled GPIO\n");
		goto remove_sysfs_redled;
	}
	
	ret = sysfs_create_group(&MxdevGpio->kobj,&greenled_attr_group);
	if(ret < 0)
	{
		printk("Failed to create sysfs greenled GPIO\n");
		goto remove_sysfs_greenled;
	}
	// Intialize Temperature Sensor
	// HISI_WRITE_REG(PERI_PMC68_BASE_ADDR,TSENSOR_EN | TSENSOR_MON_EN | TSENSOR_MON_PERIOD(50));
	
	//Init All gpio pins
	  registerGPIOPin();
	  
	// Set All LED/BUZZER cadence off
	for(JobPriority = 0 ; JobPriority >= MAX_PRIORITY ; JobPriority++ )
	{
		for(RespIndex = 0; RespIndex < BUZZER_LED_MAX ; RespIndex++ )
		{
			buzLedCadence[JobPriority][RespIndex].buzLedCad = 0;
			buzLedCadence[JobPriority][RespIndex].noOfCycles = 0;
			buzLedCadence[JobPriority][RespIndex].cadPosition = 0;
			buzLedCadence[JobPriority][RespIndex].cadBits = CAD_OFF;
		}
	}
	
	// Init timer objects
	//init_timer(&gpioPollTimerInfo);
	timer_setup(&gpioPollTimerInfo,CheckInputPinStatus,0);
	mod_timer(&gpioPollTimerInfo,jiffies + msecs_to_jiffies(GPIO_POLL_CNT));
	
	
	// Start GPIO poll timer.
	//gpioPollTimerInfo.data = 0;
	//gpioPollTimerInfo.function = CheckInputPinStatus;
	//gpioPollTimerInfo.expires  = jiffies + GPIO_POLL_CNT;

	for(pinNum = 0; pinNum < MAX_INPUT_MAP ; pinNum++ )
	{
		inputStatus.InputPinStatus[pinNum] = INPUT_UNKNOWN;
		inputPinActivity[pinNum].status = PIN_UNKNOWN;
	}
	
	
	//buzLedCadTimerInfo.data = 0;
	//buzLedCadTimerInfo.function = ProcBuzLedCad;
	//buzLedCadTimerInfo.expires  = jiffies + BUZ_LED_TMR_CNT; //200 mSec

	//init_timer(&buzLedCadTimerInfo);
	timer_setup(&buzLedCadTimerInfo,ProcBuzLedCad,0);
	mod_timer(&buzLedCadTimerInfo,jiffies + msecs_to_jiffies(BUZ_LED_TMR_CNT));
	
	
	//buzLedSyncTimerInfo.data = 0;
	//buzLedSyncTimerInfo.function = SyncBuzLedCad;
	
	//init_timer(&buzLedSyncTimerInfo);
	timer_setup(&buzLedSyncTimerInfo,SyncBuzLedCad,0);
	
	SetLedCad(NVRX_IDLE_RESP);
	SetBuzzerCad(NVRX_IDLE_RESP);
	return ret;

remove_sysfs_greenled:
	sysfs_remove_group(&MxdevGpio->kobj, &greenled_attr_group);
	
remove_sysfs_redled:
	sysfs_remove_group(&MxdevGpio->kobj, &redled_attr_group);
	
remove_sysfs_usbpower:
	sysfs_remove_group(&MxdevGpio->kobj, &usbpower_attr_group);
	
remove_sysfs_almin:
	sysfs_remove_group(&MxdevGpio->kobj, &almin_attr_group);
	
remove_sysfs_fanctl:
	sysfs_remove_group(&MxdevGpio->kobj, &fanctl_attr_group);
	
remove_sysfs_almout:
	sysfs_remove_group(&MxdevGpio->kobj, &almout_attr_group);

remove_sysfs_ipdef:
	sysfs_remove_group(&MxdevGpio->kobj, &ipdef_attr_group);

remove_sysfs_phyrst:
	sysfs_remove_group(&MxdevGpio->kobj, &phyrst_attr_group);

remove_sysfs_asmrst:
	sysfs_remove_group(&MxdevGpio->kobj, &asmrst_attr_group);
    
remove_sysfs_buzzer:
	sysfs_remove_group(&MxdevGpio->kobj, &buzzer_attr_group);
	device_destroy(gpioDevClass, gpioDevNo);
	class_destroy(gpioDevClass);

remove_cdev:
	cdev_del(gpioCdev);
	proc_remove(entry);

remove_chrdev_region:
	unregister_chrdev_region(gpioDevNo, 1);
	
	return -1;
}

//*****************************************************************************
//	GpioModExit()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is called when rmmod command used. Relese Gpio
//              Module.
//*****************************************************************************
void GpioModExit(void)
{
	SetRedLedPin(0);
	SetGreenLedPin(0);
	SetBuzzerPin(0);
	
	gpio_free(IP_DEFUALT_PIN);
	gpio_free(ALM_IN1_PIN);
	gpio_free(ALM_IN2_PIN);
	gpio_free(CARD_TYPE_0_PIN);	
	gpio_free(CARD_TYPE_1_PIN);	
	gpio_free(CARD_TYPE_2_PIN);
	gpio_free(CARD_TYPE_3_PIN);
	gpio_free(BOARD_VER_0_PIN);	
	gpio_free(BOARD_VER_1_PIN);
	gpio_free(BOARD_VER_2_PIN);
	gpio_free(BOARD_VER_3_PIN);
	gpio_free(ALM_RLY_CNTL_PIN);
	gpio_free(USB2_PWREN0_PIN);	
	gpio_free(GREENLED_GPIO_PIN);
	gpio_free(REDLED_GPIO_PIN);
	gpio_free(BUZZER_GPIO_PIN);
	gpio_free(ASM_RST_PIN);
	gpio_free(FAN_CTL1_PIN);
	gpio_free(FAN_CTL2_PIN);
	gpio_free(PHY0_RST_PIN);
	
	del_timer(&gpioPollTimerInfo);
	del_timer(&buzLedCadTimerInfo);
	del_timer(&buzLedSyncTimerInfo);
	//del_timer(&usb_power_on_info.delay_timer);
	unregister_chrdev_region(gpioDevNo, 1);
	sysfs_remove_group(&MxdevGpio->kobj, &buzzer_attr_group);
	sysfs_remove_group(&MxdevGpio->kobj, &phyrst_attr_group);
	sysfs_remove_group(&MxdevGpio->kobj, &asmrst_attr_group);
	sysfs_remove_group(&MxdevGpio->kobj, &ipdef_attr_group);
	sysfs_remove_group(&MxdevGpio->kobj, &almout_attr_group);
	sysfs_remove_group(&MxdevGpio->kobj, &fanctl_attr_group);
	sysfs_remove_group(&MxdevGpio->kobj, &almin_attr_group);
	sysfs_remove_group(&MxdevGpio->kobj, &usbpower_attr_group);
	sysfs_remove_group(&MxdevGpio->kobj, &redled_attr_group);
	sysfs_remove_group(&MxdevGpio->kobj, &greenled_attr_group);
	device_destroy(gpioDevClass, gpioDevNo);
	class_destroy(gpioDevClass);
	cdev_del(gpioCdev);
	
// 	del_timer(&buzLedSyncTimerInfo);
}

//*****************************************************************************
//	GpioOpen()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is called when Driver Open Function called.
//
//*****************************************************************************
static INT32 GpioOpen(struct inode *inode, struct file *file)
{
// 	printk("GPIO Driver Opened Successfully \n");
	return 0;
}

//*****************************************************************************
//	GpioClose()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is called when Driver close Function called.
//
//*****************************************************************************
static INT32 GpioClose(struct inode * inode, struct file * file)
{
	// Release timer.
// 	del_timer(&gpioPollTimerInfo);
// 	printk("GPIO Driver Closed Successfully \n");
	return 0;
}

//*****************************************************************************
//	GpioIoctl()
//	Param:
//		IN :	None
//		OUT:	None
//	Returns:	None
//	Description:
//				This Function is used for Interface betn Driver & Application
//              using ioctl command.
//*****************************************************************************
static long GpioIoctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int	retValue = -1;
	UINT32	State = 0;

	switch (cmd)
	{
		case MXGPIO_SET_BUZZER:
		{
			if(copy_from_user((VOIDPTR)&State, (VOIDPTR)arg, sizeof(UINT32)) == 0)
			{
				retValue = SetBuzzer(State);
			}
			break;
		}

		case MXGPIO_SET_ALMOUT:
		{
			if(copy_from_user((VOIDPTR)&State, (VOIDPTR)arg, sizeof(UINT32)) == 0)
			{
// 				printk("Alarm Out State Configured is: %d\n\r", State);
				retValue = SetAlarmOut(State);
			}
			break;
		}

		case MXGPIO_SET_FANCTL:
		{
			if(copy_from_user((VOIDPTR)&State, (VOIDPTR)arg, sizeof(UINT32)) == 0)
			{
// 				printk("Fan Out State Configured is: %d\n\r", State);
				retValue = SetFanOut(State);
			}
			break;
		}

		case MXGPIO_SET_USBPOWER:
		{
			if(copy_from_user((VOIDPTR)&State, (VOIDPTR)arg, sizeof(UINT32)) == 0)
			{
// 				printk("USB power Out State Configured is: %d\n\r", State);
				retValue = SetUSBPower(State);
			}
			break;
		}

		case MXGPIO_SET_STSLED:	
		{
			if(copy_from_user((VOIDPTR)&State, (VOIDPTR)arg, sizeof(UINT32)) == 0)
			{
				retValue = SetLed(State);
			}
			break;
		}

		case MXGPIO_SET_CADENCE:
		{
			if(copy_from_user((VOIDPTR)&State, (VOIDPTR)arg, sizeof(UINT32)) == 0)
			{
				retValue = SetLedCad(State);
				if(retValue < 0)
					break;
				retValue = SetBuzzerCad(State);
				if(retValue < 0)
					break;
				
				retValue = 0;
			}
			break;
		}
		
		case MXGPIO_CLR_CADENCE:
		{
			BUZZER_LED_RESP_t BuzLedState;
			if(copy_from_user((VOIDPTR)&BuzLedState, (VOIDPTR)arg, sizeof(UINT32)) == 0)
			{
				retValue = ClearCadence(BuzLedState);
			}
			break;
		}
		
		case MXGPIO_GET_ALL_INPUT:
		{
			if(copy_to_user((VOIDPTR)arg, (VOIDPTR)&inputStatus, sizeof(INPUT_STATUS_t)) == 0)
			{
				retValue = 0;
			}
			break;
		}

		case MXGPIO_GET_CORETEMPE:
		{
			/*UINT32	coretempeV=0;
			coretempeV = ReadCoreTemp();
			if(copy_to_user((VOIDPTR)arg, (VOIDPTR)&coretempeV, sizeof(UINT32)) == 0)
			{
				retValue = 0;
			}*/
			break;
		}
		
		case MXGPIO_GET_CAD_STS:
		{
			BUZZER_LED_CAD_t CadSts[BUZZER_LED_MAX];
			UINT8 index = 0;

			for(index = 0; index < BUZZER_LED_MAX ; index++ )
			{
				CadSts[index].buzLedCad = buzLedCadence[CurrentPriority][index].buzLedCad;
				CadSts[index].cadBits = buzLedCadence[CurrentPriority][index].cadBits;
				CadSts[index].noOfCycles = buzLedCadence[CurrentPriority][index].noOfCycles;
				CadSts[index].cadPosition = buzLedCadence[CurrentPriority][index].cadPosition;
				CadSts[index].accessResp = buzLedCadence[CurrentPriority][index].accessResp;
			}
			if(copy_to_user((VOIDPTR)arg, (VOIDPTR)&CadSts, sizeof(BUZZER_LED_CAD_t)*BUZZER_LED_MAX) == 0)
			{
				retValue = 0;
			}
			break;
		}
		case MXGPIO_GET_BUZ_STS:
		{
			UINT8 index = 0;
			State = 0;
			for(index = 0; index < MAX_PRIORITY ; index++ )
			{
				if((buzLedCadence[index][BUZZER].noOfCycles > 0) &&
						(buzLedCadence[index][BUZZER].buzLedCad != CAD_OFF) &&
						(buzLedCadence[index][BUZZER].buzLedCad != IDLE_CAD) &&
						(buzLedCadence[index][BUZZER].cadPosition != 0) )
				{
					State = 1;
				}
				if(copy_to_user((VOIDPTR)arg, (VOIDPTR)&State, sizeof(UINT32)) == 0)
				{
					retValue = 0;
				}
				if(State == 1)
					break;
			}
			break;
		}
		case MXGPIO_GET_CARD_TYPE:
		{
			UINT32 CardType = gpio_get_value(CARD_TYPE_0_PIN) | gpio_get_value(CARD_TYPE_1_PIN)<<1 | gpio_get_value(CARD_TYPE_2_PIN)<<2 | gpio_get_value(CARD_TYPE_3_PIN)<<3;
			if(copy_to_user((VOIDPTR)arg, (VOIDPTR)&CardType, sizeof(UINT32)) == 0)
			{
				retValue = 0;
			}
			break;
		}
		
		case MXGPIO_GET_BOARD_VERSION:
		{
			UINT32 boardVer = 0; // Default board version
			boardVer = gpio_get_value(BOARD_VER_0_PIN) | gpio_get_value(BOARD_VER_1_PIN)<<1 | gpio_get_value(BOARD_VER_2_PIN)<<2 | gpio_get_value(BOARD_VER_3_PIN)<<3;
			
			
			if(copy_to_user((VOIDPTR)arg, (VOIDPTR)&boardVer, sizeof(UINT32))== 0)
			{
				retValue = 0;
			}
			
			break;
		}
		case MXGPIO_SET_CLR_PHY:
		{
			if(copy_from_user((VOIDPTR)&State, (VOIDPTR)arg, sizeof(UINT32)) == 0)
			{
				retValue = phy_power_on_off(State);
			}
			break;
		}
	
		case MXGPIO_SET_CLR_PHY2:
		{
			if(copy_from_user((VOIDPTR)&State, (VOIDPTR)arg, sizeof(UINT32)) == 0)
			{
				retValue = phy2_power_on_off(State);
			}
			break;
		}
		
		case MXGPIO_SET_CLR_ASM_RST:
		{
			if(copy_from_user((VOIDPTR)&State, (VOIDPTR)arg, sizeof(UINT32)) == 0)
			{
				retValue = asm_power_on_off(State);
			}
			break;
		}
		default:
			break;
	}
	return (retValue);
}

module_init(GpioModInit);
module_exit(GpioModExit);

// Module Descripation.
MODULE_AUTHOR("Platform Team <platform.team@matrixcomsec.com>");
MODULE_DESCRIPTION("GPIO driver module");
MODULE_LICENSE("GPL");

///////////////////////////////////////////////////////////////////////////////
// End of File "MxGpioDrv.c"
///////////////////////////////////////////////////////////////////////////////
