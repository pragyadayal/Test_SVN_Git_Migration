#ifndef _ETNEV3_WATCHDOG_C_
#define _ETNEV3_WATCHDOG_C_

// ///////////////////////////////////////////////////////////////////////////
//
//   MMM     MMM       AAA       TTTTTTTTTT  RRRRRR    IIIIIIII  XX   XX
//   MMMM   MMMM      AA AA          TT      RR   RR      II      XX XX
//   MM MM MM MM     AA   AA         TT      RR    RR     II       XXX
//   MM  MM   MM    AAAAAAAAA        TT      RRRRRRR      II       XXX
//   MM       MM   AA       AA       TT      RR  RR       II      XX XX
//   MM       MM  AA         AA      TT      RR   RR   IIIIIIII  XX   XX
//
//   Company      : Matrix comsec Pvt. Ltd., Baroda, India.
//   Project      : ARGO FR
//   Created By   : MATRIX PLATFORM TEAM
//   File         : watchdog.c
//   Dated        : 02-03-2021
//
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////


#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include "watchdog.h"

/****************************************************************
 * @defines
 ****************************************************************/
#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

#define FALSE	0
#define TRUE	1

#define LOW     0
#define HIGH    1

/****************************************************************
 * Pins Configuration defines
 ****************************************************************/
#define	WDT_EN_PIN				GPIO_TO_PIN(2, 1) //GPIO2_A1_u
#define WDT_KICK_PIN    			GPIO_TO_PIN(2, 2) //GPIO2_A2_d
 
#define	WATCHDOG_INTERVAL_MS	(HZ/10)		// 100 ms 
#define WATCHDOG_TIMEOUT	 600			// 100 * 600 = 60 second.

/****************************************************************
 * @global varibales declearation
 ****************************************************************/
struct timer_list	wdtTimer;
static unsigned char isSystemRunning = TRUE;
static unsigned char wdtPinStatus;
static unsigned int  wdtTimeOutCount = 0;
/****************************************************************
 * @function decleration
 ****************************************************************/
static void wdt_Tasklet(struct timer_list *timer);
static void wdtInit(void);
static long watchdog_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int watchdog_open(struct inode *inode, struct file *filp);
int wdt_close(struct inode *inode, struct file *filp);

/****************************************************************
 * @structure decleration
 ****************************************************************/
static struct file_operations watchdog_fops = 
{
	open:				watchdog_open,
	unlocked_ioctl:		watchdog_ioctl,
	release:			wdt_close,
};


/**
 * @fn       static void wdt_Tasklet(unsigned long param
 * @param    unsigned long
 * @returns  NONE
 * @brief
 * @note
 */
static void wdt_Tasklet(struct timer_list *timer)
{	
	if(isSystemRunning)
	{
		// Toggle WDT pin
		if(wdtPinStatus == 0)
		{
  		  wdtPinStatus = 1;
		  gpio_set_value(WDT_KICK_PIN, HIGH);
		}	
		else
		{
  		  wdtPinStatus = 0;
		  gpio_set_value(WDT_KICK_PIN, LOW);
		} 
		
		//Reload timer
		mod_timer(&wdtTimer, jiffies + WATCHDOG_INTERVAL_MS);
	}
	if(wdtTimeOutCount)
	{
		wdtTimeOutCount--;
		if (wdtTimeOutCount == 0)
		{	
			isSystemRunning = FALSE;
			wdtTimeOutCount = WATCHDOG_TIMEOUT;
			printk(KERN_INFO"Drv_WDT :Info: Time %d\n",wdtTimeOutCount);
		}	
	}
}
/**
 * @fn       static void wdtInit(void)
 * @param    NONE
 * @returns  NONE
 * @brief
 * @note
 */
static void wdtInit(void)
{
	isSystemRunning = 1;
	wdtTimeOutCount = WATCHDOG_TIMEOUT;
	
	//init_timer(&wdtTimer);
	
	timer_setup(&wdtTimer,wdt_Tasklet,0);
	mod_timer(&wdtTimer,jiffies + WATCHDOG_INTERVAL_MS);

	
	gpio_set_value(WDT_EN_PIN, LOW);
	
	wdtPinStatus = 1;
	gpio_set_value(WDT_KICK_PIN, HIGH);
}

/**
 * @fn       static long watchdog_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
 * @param    
 * @returns  int
 * @brief	 Function used to do ioctl operation.
 * @note
 */
static long watchdog_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) 
	{
		
	/*******************************
	* Toggling watchdog pin.
	 *******************************/
		case IOCTL_WATCHDOG_TOGGLE:
		  wdtTimeOutCount = WATCHDOG_TIMEOUT;
		  break;
		  
		default:
			printk(KERN_ERR "Drv_WDT :Error: Invalid IOCTL received: %u \n",cmd); 
			return (FALSE);
		break;
	}
	return (TRUE);
}

/**
 * @fn       int watchdog_open(struct inode *inode, struct file *filp)
 * @param    file's inode , file's fd.
 * @returns  int
 * @brief
 * @note
 */
int watchdog_open(struct inode *inode, struct file *filp)
{
	printk("Drv_WDT :Info: driver open \n");
	
	wdtInit();
	return (0);
}

/**
 * @fn       int wdt_close(struct inode *inode, struct file *filp)
 * @param    file's inode , file's fd.
 * @returns  int
 * @brief
 * @note
 */
int wdt_close(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "Drv_WDT: Info: wdt_close\n");
 	return (0);
}

/**
 * @fn       static int __init watchdog_init(void)
 * @param    NONE
 * @returns  int
 * @brief	@time of imserting module init function will be called.
 * @note
 */
static int __init watchdog_init(void)
{
	int result;
	printk(KERN_INFO "wdt_drv: Info: Date: %s Time: %s \n",__DATE__, __TIME__);
	
	result = register_chrdev(0, WATCHDOG_DEVICE_NAME, &watchdog_fops);
	if(result < 0)
	{  
		printk(KERN_ERR "wdt_drv: Error: Not able to register WATCHDOG driver\n");
		return (result);
	}
	
	//Configure WDT_EN Pin as an GPIO
	gpio_request(WDT_EN_PIN, "WDT_EN");
	//Set WDT_EN PIN  as an Output
	gpio_direction_output(WDT_EN_PIN, HIGH);
	
	//Configure WDT_SYS Pin as an GPIO
	gpio_request(WDT_KICK_PIN, "WDT_KICK");
	//Configure WDT_SYS Pin as an output
	gpio_direction_output(WDT_KICK_PIN, HIGH);
	
	return (0);
}
/**
 * @fn       static void __exit watchdog_exit(void)
 * @param    NONE
 * @returns  NONE
 * @brief
 * @note
 */
static void __exit watchdog_exit(void)
{
  	printk(KERN_INFO "Drv_WDT :Info: watchdog_exit:\n");
	if(wdtTimer.function != NULL)
	{
	  del_timer(&wdtTimer);
	}
  	
	/************************************
	 * @time of exit,
	 * removing GPIO watchdog pins resources.
	 * **********************************/
	gpio_free(WDT_EN_PIN);
	gpio_free(WDT_KICK_PIN);
	unregister_chrdev(0, WATCHDOG_DEVICE_NAME);
}

module_init(watchdog_init);
module_exit(watchdog_exit);
MODULE_AUTHOR("Platform Team <platform.team@MatrixComSec.com>");
MODULE_DESCRIPTION("ACTA watchdog driver module");
MODULE_LICENSE("GPL");

// -----------------------------------------
// ///////////////////////////////////////////////////////////////////////////
// End of File "watchDog.c"
// ///////////////////////////////////////////////////////////////////////////
#endif 
