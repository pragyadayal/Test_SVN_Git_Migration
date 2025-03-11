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
//   Project      : NVR-X
//   Created By   : Darshan Patel
//   File         : MxGpioCtl.c
//   Description  : This file provides APIs to set/clear GPIOs.
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include "../MxTypedef.h"
#include "../MxGpioDrv.h"
#include "../../watchdog/watchdog.h"

//******** Global Variables **********
UINT8 eventname[NVRX_RESP_MAX][30]=
{
	"NVRX_IDLE_RESP              ",
	"NVRX_APPL_INIT_RESP         ",
	"NVRX_NET_CONN_RESP          ",
	"NVRX_NET_DISCONN_RESP       ",
	"NVRX_STORAGE_INCOMPLETE_RESP",
	"NVRX_STORAGE_FULL_ALT_RESP  ",
	"NVRX_STORAGE_FULL_RESP      ",
	"NVRX_FORMAT_DISK_RESP       ",
	"NVRX_FW_UPG_INPROCESS_RESP  ",
	"NVRX_FW_UPG_FAILED_RESP     ",
	"NVRX_TRG_EVT_RESP           ",
	"NVRX_BKUP_FULL_RESP         ",
	"NVRX_NO_BKUP_RESP           ",
	"NVRX_NORMAL_STATE			 ",
	"NVRX_RECORDING_FAIL         ",
	"NVRX_RECORDING_FAIL_FOR_ALL ",
	"NVRX_CPUTEMP_HIGH"
};

UINT8 DeviceName[BUZZER_LED_MAX][11]=
{
	"BUZZER    ",
	"RED_LED   ",
	"GREEN_LED ",
};
//******** Function Prototypes *******

//******** Global Variables **********

//******** Static Variables **********

//******** Function Definations ******
static int atoul(char* str, unsigned int* pulValue);
static int atoulx(char* str, unsigned int* pulValue);
int StrToNumber(char* str , unsigned int* pulValue);

//********* Main *********************
int main (int argc, char **argv)
{
	unsigned int	value = -1;
	unsigned int	cmd = 15;
	INT32	MxGpioDrvFd = -1;
	INT32   MxWatchdogFd = -1;
	INPUT_STATUS_t inputstatus;
	BUZZER_LED_CAD_t CadSts[BUZZER_LED_MAX];
	BUZZER_LED_RESP_t __buzLedResp;
	UINT8 index = 0;
	unsigned int cnt = 0;
	
	if(argc > 3 || argc <= 1 )
	{
		printf("\nUSAGE:\n\t./MxGpioCtl <CMD> <VALUE>\n");
		printf("\n\t<CMD> : \n\t0-MXGPIO_SET_BUZZER[0,1] \
				    \n\t1-MXGPIO_SET_ALMOUT[0,1] \
				    \n\t2-MXGPIO_SET_FANCTL(0-5) \
				    \n\t3-MXGPIO_SET_USBPOWER(0-7) \
				    \n\t4-MXGPIO_SET_STSLED(0,5) \
				    \n\t5-MXGPIO_SET_CADENCE(0-16) \
				    \n\t6-MXGPIO_CLR_CADENCE(LED)(0-16) \
				    \n\t7-MXGPIO_CLR_CADENCE(Buzer)(0-16) \
				    \n\t8-MXGPIO_GET_ALL_INPUT \
				    \n\t9-MXGPIO_GET_CORETEMPE \
				    \n\t10-MXGPIO_GET_CAD_STS \
				    \n\t11-MXGPIO_GET_CARD_TYPE \
				    \n\t12-MXGPIO_GET_BUZ_STS \
				    \n\t13-MXGPIO_CLR_CADENCE \
				    \n\t14-MXGPIO_GET_BOARD_VERSION \
				    \n\t15-MXGPIO_SET_CLR_PHY[0-1] \
				    \n\t16-MXGPIO_SET_CLR_PHY2[0-1] \
				    \n\t17-MXGPIO_SET_CLR_ASM_RST[0-1] \
				    \n\t18-IOCTL_WATCHDOG_TOGGLE\n");
		return 0;
	}
	else if(argc == 2)
	{
		StrToNumber(argv[1], (unsigned int *)&cmd);
	}
	else
	{	
		StrToNumber(argv[1], (unsigned int *)&cmd);
		StrToNumber(argv[2], (unsigned int *)&value);	
	}	
  	MxGpioDrvFd = open(GPIO_DEVICE_NAME, O_RDWR);
	if(MxGpioDrvFd < 0)
	{
		// Fail to open Driver
		printf("MxGpioCtl Application Failed to Open GpioDriver\n");
		return 0;
	}
	
	switch(cmd)
	{
		case 0: if(ioctl(MxGpioDrvFd, MXGPIO_SET_BUZZER, &value) < 0)
				{
					printf("ioctl MXGPIO_SET_BUZZER failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("ioctl MXGPIO_SET_BUZZER %d set successfully\n",value);
				break;
		case 1: if(ioctl(MxGpioDrvFd, MXGPIO_SET_ALMOUT, &value) < 0)
				{
					printf("ioctl MXGPIO_SET_ALMOUT failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("ioctl MXGPIO_SET_ALMOUT %d set successfully\n",value);
				break;
		case 2: if(ioctl(MxGpioDrvFd, MXGPIO_SET_FANCTL, &value) < 0)
				{
					printf("ioctl MXGPIO_SET_FANCTL failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("ioctl MXGPIO_SET_FANCTL %d set successfully\n",value);
				break;
		case 3: if(ioctl(MxGpioDrvFd, MXGPIO_SET_USBPOWER, &value) < 0)
				{
					printf("ioctl MXGPIO_SET_USBPOWER failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("ioctl MXGPIO_SET_USBPOWER %d set successfully\n",value);
				break;
		case 4: if(ioctl(MxGpioDrvFd, MXGPIO_SET_STSLED, &value) < 0)
				{
					printf("ioctl MXGPIO_SET_STSLED failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("ioctl MXGPIO_SET_STSLED %d set successfully\n",value);
				break;
		case 5:if(ioctl(MxGpioDrvFd, MXGPIO_SET_CADENCE, &value) < 0)
				{
					printf("ioctl MXGPIO_SET_CADENCE failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("ioctl MXGPIO_SET_CADENCE %d set successfully\n",value);
				break;
		case 6:
				__buzLedResp.LedCadResp = value;
				__buzLedResp.BuzCadResp = NVRX_RESP_NONE;
				if(ioctl(MxGpioDrvFd, MXGPIO_CLR_CADENCE, &__buzLedResp) < 0)
				{
					printf("ioctl MXGPIO_CLR_CADENCE failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("ioctl MXGPIO_CLR_CADENCE %d set successfully\n",value);
				break;
		case 7: __buzLedResp.LedCadResp = NVRX_RESP_NONE;
				__buzLedResp.BuzCadResp = value;
				if(ioctl(MxGpioDrvFd, MXGPIO_CLR_CADENCE, &__buzLedResp) < 0)
				{
					printf("ioctl MXGPIO_CLR_CADENCE failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("ioctl MXGPIO_CLR_CADENCE %d set successfully\n",value);
				break;
	
		case 8: if(ioctl(MxGpioDrvFd, MXGPIO_GET_ALL_INPUT, &inputstatus) < 0)
				{
					printf("ioctl MXGPIO_GET_ALL_INPUT failed\n");
					close(MxGpioDrvFd);
					return 0;
				}

				for(cnt = 0; cnt < MAX_INPUT_MAP; cnt++)
					printf("input%d : %d\n",cnt,inputstatus.InputPinStatus[cnt]);
				break;
		case 9: if(ioctl(MxGpioDrvFd, MXGPIO_GET_CORETEMPE, &value) < 0)
				{
					printf("ioctl MXGPIO_GET_CORETEMPE failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("core temperature : %u c\n",value);
				break;
		case 10: if(ioctl(MxGpioDrvFd, MXGPIO_GET_CAD_STS, &CadSts) < 0)
				{
					printf("ioctl MXGPIO_GET_CAD_STS failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("\nDevice     \t   accessResp                  \tbuzLedCad\tnoOfCycles\tcadPosition\tcadBits\n");
				for(index = 0; index < BUZZER_LED_MAX ; index++ )
				{
					printf("%s\t",DeviceName[index]);
					printf("%s\t",eventname[CadSts[index].accessResp]);
					printf("0x%08x\t",CadSts[index].buzLedCad);
					printf("%d\t\t",CadSts[index].noOfCycles);
					printf("%d\t\t",CadSts[index].cadPosition);
					printf("%d\t",CadSts[index].cadBits);
					printf("\n");
				}
				break;
		case 11: if(ioctl(MxGpioDrvFd, MXGPIO_GET_CARD_TYPE, &value) < 0)
				{
					printf("ioctl MXGPIO_GET_CARD_TYPE failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("Card Type : %u\n",value);
				break;
		case 12: if(ioctl(MxGpioDrvFd, MXGPIO_GET_BUZ_STS, &value) < 0)
				{
					printf("ioctl MXGPIO_GET_CORETEMPE failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("Buzzer is %s.\n",value==1?"ON":"OFF");
				break;
		
		case 13: 	__buzLedResp.LedCadResp = NVRX_RESP_PRESENT;
				__buzLedResp.BuzCadResp = NVRX_RESP_PRESENT;
				if(ioctl(MxGpioDrvFd, MXGPIO_CLR_CADENCE, &__buzLedResp) < 0)
				{
					printf("ioctl MXGPIO_CLR_CADENCE failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("ioctl MXGPIO_CLR_CADENCE set successfully\n");
				break;
					
		case 14: if(ioctl(MxGpioDrvFd, MXGPIO_GET_BOARD_VERSION, &value) < 0)
				{
					printf("ioctl MXGPIO_GET_BOARD_VERSION failed\n");
					close(MxGpioDrvFd);
					return 0;
				}
				printf("Board version : %u\n",value);
				break;
                
		case 15: 
				printf("ioctl MXGPIO_SET_CLR_PHY0 value : %d\n", value);
				if(ioctl(MxGpioDrvFd, MXGPIO_SET_CLR_PHY, &value) < 0)
				{
				    printf("ioctl MXGPIO_SET_CLR_PHY failed\n");
				    close(MxGpioDrvFd);
				    return 0;
				}break;
				
		case 16: 
				printf("ioctl MXGPIO_SET_CLR_PHY2 value : %d\n", value);
				if(ioctl(MxGpioDrvFd, MXGPIO_SET_CLR_PHY2, &value) < 0)
				{
				    printf("ioctl MXGPIO_SET_CLR_PHY2 failed\n");
				    close(MxGpioDrvFd);
				    return 0;
				}break;
				
		case 17: 
				printf("ioctl MXGPIO_SET_CLR_ASM_RST value : %d\n", value);
				if(ioctl(MxGpioDrvFd, MXGPIO_SET_CLR_ASM_RST, &value) < 0)
				{
				    printf("ioctl MXGPIO_SET_CLR_ASM_RST failed\n");
				    close(MxGpioDrvFd);
				    return 0;
				}break;
				
		case 18:

				MxWatchdogFd = open(WATCHDOG_DEVICE_NAME, O_RDWR);
				if(MxWatchdogFd < 0)
				{
					// Fail to open Driver
					printf("MxGpioCtl Application Failed to Open watchdogDriver\n");
					return 0;
				}
				while(1)
				{
				  printf("start IOCTL_WATCHDOG_TOGGLE testing....\n");
				  if(ioctl(MxWatchdogFd, IOCTL_WATCHDOG_TOGGLE,0) < 0)
				  {
				      printf("ioctl IOCTL_WATCHDOG_TOGGLE failed\n");
				      close(MxWatchdogFd);
				      return 0;
				  }
				  sleep(5);
				}
				
				break;
	  
				
		default:
			printf("\nUSAGE:\n\t./MxGpioCtl <CMD> <VALUE>\n");
			printf("\n\t<CMD> : \n\t0-MXGPIO_SET_BUZZER[0,1] \n\t1-MXGPIO_SET_ALMOUT[0,1]\n\t2-MXGPIO_SET_FANCTL(0-5)\n\t3-MXGPIO_SET_USBPOWER(0-7)\n\t4-MXGPIO_SET_STSLED(0,3)\n\t5-MXGPIO_SET_CADENCE(0-12)\n\t6-MXGPIO_CLR_CADENCE(LED)(0-12)\n\t7-MXGPIO_CLR_CADENCE(Buzer)(0-12)\n\t8-MXGPIO_GET_ALL_INPUT\n\t9-MXGPIO_GET_CORETEMPE\n\t10-MXGPIO_GET_CAD_STS\n\t11-MXGPIO_GET_CARD_TYPE\n\t12-MXGPIO_GET_BUZ_STS\n\t13-MXGPIO_CLR_CADENCE\n");
			break;
	}

	close(MxGpioDrvFd);
	return 0;
}

int StrToNumber(char* str , unsigned int* pulValue)
{
    if ( *str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X') )
    {
        if (*(str + 2) == '\0')
        {
            return -1;
        }
        else
        {
            return atoulx(str + 2, pulValue);
        }
    }
    else
    {
        return atoul(str, pulValue);
    }
}

static int atoul(char* str, unsigned int* pulValue)
{
    unsigned int ulResult = 0;

    while (*str)
    {
        unsigned char ch = toupper(*str);

        if (isdigit(ch))
        {
            /*最大支持到0xFFFFFFFF(4294967295),
               X * 10 + (*str)-48 <= 4294967295
               所以， X = 429496729 */
            if ((ulResult < 429496729) || ((ulResult == 429496729) && (*str < '6')))
            {
                ulResult = ulResult * 10 + (*str) - 48;
            }
            else
            {
                *pulValue = ulResult;
                return -1;
            }
        }
        else
        {
            *pulValue = ulResult;
            return -1;
        }
        str++;
    }
    *pulValue = ulResult;
    return 0;
}

#define ASC2NUM(ch) (ch - '0')
#define HEXASC2NUM(ch) (ch - 'A' + 10)

int  atoulx(char* str, unsigned int* pulValue)
{
    unsigned int   ulResult = 0;
    unsigned char ch;

    while (*str)
    {
        ch = toupper(*str);
        if (isdigit(ch) || ((ch >= 'A') && (ch <= 'F' )))
        {
            if (ulResult < 0x10000000)
            {
                ulResult = (ulResult << 4) + ((ch <= '9') ? (ASC2NUM(ch)) : (HEXASC2NUM(ch)));
            }
            else
            {
                *pulValue = ulResult;
                return -1;
            }
        }
        else
        {
            *pulValue = ulResult;
            return -1;
        }
        str++;
    }

    *pulValue = ulResult;
    return 0;
}
