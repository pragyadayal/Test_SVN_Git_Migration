/*****< btpsvend.c >***********************************************************/
/*      Copyright 2013 - 2014 Stonestreet One.                                */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  BTPSVEND - Vendor specific functions/definitions used to define a set of  */
/*             vendor specific functions supported by the Bluetopia Protocol  */
/*             Stack.  These functions may be unique to a given hardware      */
/*             platform.                                                      */
/*                                                                            */
/*  Author:  Damon Lange                                                      */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   07/31/09  D. Lange       Initial creation.                               */
/*   09/06/13  S. Hishmeh     Created for use with AM335x platforms.          */
/******************************************************************************/
#include "SS1BTPS.h"          /* Bluetopia API Prototypes/Constants.          */
#include "HCIDRV.h"           /* Main HCI Driver Prototypes/Constants.        */
#include "BTPSKRNL.h"         /* BTPS Kernel Prototypes/Constants.            */
#include "BTPSVEND.h"         /* Bluetooth Vend. Spec. Prototypes/Constants.  */
#ifndef SHARED_TRANSPORT
#include "SS1TIBTS.h"         /* TI BTS Script Parser Prototypes/Constants.   */
#include <stdlib.h>           /* For system() call.                           */
#endif

#include <stdio.h>
#include <fcntl.h>
#include <string.h>

   /* Miscellaneous Type Declarations.                                  */
#define BTPSVEND_DEBUG 1

#ifdef SHARED_TRANSPORT
   /* Internal Variables to this Module (Remember that all variables    */
   /* declared static are initialized to 0 automatically by the         */
   /* compiler as part of standard C/C++).                              */

   /* The following variable is used to track whether or not the Vendor */
   /* Specific Commands (and Patch RAM commands) have already been      */
   /* issued to the device.  This is done so that we do not issue them  */
   /* more than once for a single device (there is no need).  They could*/
   /* be potentially issued because the HCI Reset hooks (defined below) */
   /* are called for every HCI_Reset() that is issued.                  */
static Boolean_t VendorCommandsIssued;

   /* Internal Function Prototypes.                                     */
#else

	/* The following definition specifies path for the dts file         */
	/* parameters in the tibt section.                                  */
#define TIBT_SYSFS_BASE		     "/proc/device-tree/tibt"
	/* The following definition specifies path for the UART
	 * configuration file to override the DTS content, then the
	 * configurations can be changed dynamically without boot again.
	 */
#define TIBT_SYSFS_BASE_DYNAMIC  "/home/root/tibt/config"

	/* The following definitions select the UART protocol.  It is        */
	/* currently set to HCILL+RTS/CTS because commands to enable HCILL   */
	/* are embedded in the BTS Initialization Script and HCILL is        */
	/* therefore enabled when the BTS Script is executed.                */
#define BLUETOOTH_HCI_UART_PROTOCOL               cpHCILL_RTS_CTS

	/* The following definition specifies the UART COMM port number used */
	/* by the Bluetooth radio. Not in use, just for init correctly       */
#define BLUETOOTH_HCI_DEV_TTY_COM_PORT_NUMBER     1

   /* The following definitions specifies the default baud rate at which*/
   /* the Bluetooth radio will use to communicate after it is reset.    */
#define BLUETOOTH_CONTROLLER_POWER_ON_BAUD_RATE   115200

   /* The following definition specifies the delay in milliseconds      */
   /* required to wait while the Bluetooth radio power cycles.  The     */
   /* large delay of 2 seconds was selected to work around an issue with*/
   /* the WL8 and the AM335x RTS/CTS mux configuration when the WLAN is */
   /* initialized before Bluetooth: if the WLAN is turned on before     */
   /* Bluetooth, due to a long handshake between Bluetooth and the WLAN,*/
   /* it takes more time for the controller to de-assert RTS, if during */
   /* this time the host is sending commands then the data is lost and  */
   /* Bluetooth initialization will fail.                               */
   /* * NOTE * This issue is found in the Linux kernel provided with TI */
   /*          SDK 6.0 and has been resolved in SDK 7.0.  If you are    */
   /*          using SDK 7.0 this delay can be safely lowered to 75     */
   /*          milliseconds.                                            */
#define BLUETOOTH_CONTROLLER_POWER_ON_DELAY       2000

   /* The following definition specifies the delay in milliseconds to   */
   /* wait while the Bluetooth controller powers off.  This delay is    */
   /* used for safety to ensure that the radio is not power cycled off  */
   /* and on too quickly which could cause Bluetooth initialization to  */
   /* fail.                                                             */
#define BLUETOOTH_CONTROLLER_POWER_OFF_DELAY      75

   /* The following definitions specifies the HCI Command Op Code that  */
   /* is used to change the Controller's Baud Rate.                     */
#define UPDATE_UART_HCI_BAUDRATE_COMMAND_OPCODE   ((Word_t)(0xFF36))

/* Value from dts file tibt section to store at HCI driver update       */
static int nShutDownGpio;

   /* The following variable determines if the Bluetooth Controller's   */
   /* Initialization Script has be loaded on to the device.             */
static Boolean_t InitializationScriptLoaded;

   /* The following variable stores the HCI driver information and is   */
   /* used to reconfigure the driver.                                   */
static HCI_DriverInformation_t TargetHCIDriverInformation;

   /* Internal Function Prototypes.                                     */
static void Enable_Bluetooth_Controller(void);
static void Disable_Bluetooth_Controller(void);
static int ExecuteScriptFile(unsigned int BluetoothStackID, TIBTS_Script_Version_Info_t *ScriptVersionInfo);
static int ProcessSendHCICommandAction(unsigned int BluetoothStackID, TIBTS_Send_HCI_Command_Action_Data_t *SendHCICommandActionData);
static int ConfigureHCIBaudRate(unsigned int BluetoothStackID, DWord_t BaudRate);
static int MapSendRawResults(int Result, Byte_t Status, Byte_t LengthResult, Byte_t *ReturnData);

static int FirstTime = 1;

int Execute(char *cmd_pattern)
{
	char cmd[60];

	sprintf(cmd, cmd_pattern,  nShutDownGpio);
#if BTPSVEND_DEBUG
	printf("%s\n", cmd);
#endif
	return system(cmd);
}

   /* The following function enables the Bluetooth Controller.          */
static int SetBTEnable(int state)
{
	int ret_val;
   /* Set the Bluetooth Enable line high.                               */

	if(FirstTime)
	{
		FirstTime = 0;
		Execute("echo %d > /sys/class/gpio/export");
		Execute("echo out > /sys/class/gpio/gpio%d/direction");
	}

	if(state)
	{
		ret_val = Execute("echo 1 > /sys/class/gpio/gpio%d/value");
        /* The following Power off and Power on is a workaround for an issue*/
        /* with the WL8 and the AM335x RTS/CTS mux configuration when the   */
        /* WLAN is initialized before the Bluetooth: If the WLAN is turned  */
        /* on before the Bluetooth, and the Bluetooth tries to initialize   */
        /* after the Bluetooth was already on due to previous run the       */
        /* won't initialize correctly, the RTS doesn't triggered so the     */
        /* procedure will return an error. when using power off and power on*/
        /* again the RTS is triggered and the Bluetooth will initialize     */
        /* correctly.                                                       */
        ret_val = Execute("echo 0 > /sys/class/gpio/gpio%d/value");
        ret_val = Execute("echo 1 > /sys/class/gpio/gpio%d/value");
	}
	else
		ret_val = Execute("echo 0 > /sys/class/gpio/gpio%d/value");

	return ret_val;
}

static void Enable_Bluetooth_Controller()
{
   /* Set the Bluetooth Enable line high.                               */

	if(!SetBTEnable(1))
    {
      /* Delay while we wait for the Bluetooth Controller to boot.      */
      BTPS_Delay(BLUETOOTH_CONTROLLER_POWER_ON_DELAY);
    }

}

   /* The following function disables the Bluetooth Controller.         */
static void Disable_Bluetooth_Controller(void)
{
	if(!SetBTEnable(0))
    {
      /* Delay for safety in case the line is immediately brought back  */
      /* high, which may or may not cause problems with the Bluetooth   */
      /* Controller.                                                    */
      BTPS_Delay(BLUETOOTH_CONTROLLER_POWER_OFF_DELAY);
   }

   /* The Bluetooth Controller has been disabled, flag that the script  */
   /* is no longer loaded so that the script will be loaded when the    */
   /* controller is re-enabled.                                         */
   InitializationScriptLoaded = FALSE;
}

/* The following function reads string with '=' and return the       */
/* parameter after the '=' as integer.                               */
static int GetParamFromStr(char* str)
{
	char *equalChar = "=";
	char *paramStr;

	paramStr = strstr(str, equalChar);
    /* Increment by one to point the first char after the '=' */
    paramStr++;
    return  atoi(paramStr);
}

   /* The following function is a utility function that is used to map  */
   /* the return results that we returned from Send Raw (ONLY for       */
   /* commands that return command complete event) to a negative error  */
   /* code.                                                             */
   /* * NOTE * As an internal function no check is done on the return   */
   /*          parameters.                                              */
static int MapSendRawResults(int Result, Byte_t Status, Byte_t LengthResult, Byte_t *ReturnData)
{
   int ret_val;

   /* Check to see if the API returned an error.                        */
   if((!Result) && (!Status) && (LengthResult >= 1))
   {
      /* Check to see if the chip returned an error.                    */
      if(ReturnData[0] == HCI_ERROR_CODE_NO_ERROR)
         ret_val = 0;
      else
         ret_val = BTPS_ERROR_CODE_HCI_STATUS_BASE - ReturnData[0];
   }
   else
      ret_val = BTPS_ERROR_DEVICE_HCI_ERROR;

   return(ret_val);
}

   /* The following function executes the vendor specific command which */
   /* is used to change the Bluetooth UART Baud Rate for the Local      */
   /* Bluetooth Device.  The first parameter is the Bluetooth Stack ID  */
   /* and the second parameter is the baud rate which the HCI interface */
   /* will be configured to.  This change encompasses both changing the */
   /* speed of the Bluetooth chip (by issuing the correct commands) and */
   /* then, if successful, informing the HCI Driver of the change (so   */
   /* the driver can communicate with the Bluetooth device at the new   */
   /* baud rate).  This function returns zero if successful or a        */
   /* negative return error code otherwise.                             */
static int ConfigureHCIBaudRate(unsigned int BluetoothStackID, DWord_t BaudRate)
{
   int                                 ret_val;
   Byte_t                              Length;
   Byte_t                              Status;
   Byte_t                              OGF;
   Word_t                              OCF;
   unsigned long                       LBaudRate;
   NonAlignedDWord_t                   _BaudRate;
   HCI_Driver_Reconfigure_Data_t       DriverReconfigureData;

   union
   {
      Byte_t                           Buffer[16];
      HCI_COMMReconfigureInformation_t COMMReconfigureInformation;
   } Data;

   /* Before continuing, make sure the input parameters appear to be    */
   /* semi-valid.                                                       */
   if((BluetoothStackID) && (BaudRate))
   {
      /* Write the Baud Rate.                                           */
      ASSIGN_HOST_DWORD_TO_LITTLE_ENDIAN_UNALIGNED_DWORD(&_BaudRate, BaudRate);

      LBaudRate = (unsigned long)BaudRate;

      /* Next, write the command to the device.                         */
      Length  = sizeof(Data.Buffer);
      OGF     = TIBTS_COMMAND_OPCODE_TO_OGF(UPDATE_UART_HCI_BAUDRATE_COMMAND_OPCODE);
      OCF     = TIBTS_COMMAND_OPCODE_TO_OCF(UPDATE_UART_HCI_BAUDRATE_COMMAND_OPCODE);

      ret_val = HCI_Send_Raw_Command(BluetoothStackID, OGF, OCF, sizeof(NonAlignedDWord_t), (Byte_t *)&_BaudRate, &Status, &Length, Data.Buffer, TRUE);

      /* Map the return results into an error code.                     */
      if((!ret_val) && (!(ret_val = MapSendRawResults(ret_val, Status, Length, Data.Buffer))))
      {
         /* Note that after this command is issued the Bluetooth        */
         /* controller will set its RTS output high while it            */
         /* re-configures its UART.  There is an issue in v6.00 of the  */
         /* AM335x SDK where UART disregards its CTS input.  It is      */
         /* therefore possible for us to start sending while the        */
         /* Bluetooth controller has its RTS output high and is not     */
         /* ready to receive data.  To workaround this issue we add a   */
         /* delay here while we wait for the Bluetooth controller to    */
         /* de-assert its RTS output.                                   */
         /* * NOTE * This issue will be resolved in the next release of */
         /*          the AM335x SDK and this delay will be removed then */
         /*          when the updated SDK is released.                  */
         /* * NOTE * The delay from the time the radio sent a response  */
         /*          to the baud-rate change command to the time when   */
         /*          RTS was set low was measured to be 0.58            */
         /*          milliseconds using a WL1835, this value was        */
         /*          measured with a baud rate of 921,600 and 3,000,000 */
         /*          bps and was the same for both baud rates.  In the  */
         /*          line of code below we delay longer than the minimum*/
         /*          of 0.58 milliseconds for additional safety.        */
         BTPS_Delay(5);

         /* We were successful, now we need to change the baud rate of  */
         /* the driver.                                                 */
         BTPS_MemInitialize(&(Data.COMMReconfigureInformation), 0, sizeof(Data.COMMReconfigureInformation));

         Data.COMMReconfigureInformation.BaudRate         = LBaudRate;
         Data.COMMReconfigureInformation.ReconfigureFlags = HCI_COMM_RECONFIGURE_INFORMATION_RECONFIGURE_FLAGS_CHANGE_BAUDRATE;

         DriverReconfigureData.ReconfigureCommand         = HCI_COMM_DRIVER_RECONFIGURE_DATA_COMMAND_CHANGE_COMM_PARAMETERS;
         DriverReconfigureData.ReconfigureData            = (void *)&Data.COMMReconfigureInformation;

         ret_val = HCI_Reconfigure_Driver(BluetoothStackID, FALSE, &DriverReconfigureData);
      }
   }
   else
      ret_val = BTPS_ERROR_INVALID_PARAMETER;

   /* Return the result the caller.                                     */
   return(ret_val);
}

   /* The following function is responsible for processing and sending  */
   /* an HCI Command.  This function returns 0 on success and a negative*/
   /* error code otherwise.                                             */
static int ProcessSendHCICommandAction(unsigned int BluetoothStackID, TIBTS_Send_HCI_Command_Action_Data_t *SendHCICommandActionData)
{
   int    ret_val;
   Byte_t Length;
   Byte_t Status;
   Byte_t Buffer[32];

   /* Check if this command to change the Controller's UART Baud Rate.  */
   if(TIBTS_OGF_OCF_TO_COMMAND_OPCODE(SendHCICommandActionData->OGF, SendHCICommandActionData->OCF) != UPDATE_UART_HCI_BAUDRATE_COMMAND_OPCODE)
   {
      /* This is not a command to change the Controller's UART Baud     */
      /* Rate, next, write the command to the device.                   */
      Length  = sizeof(Buffer);

      ret_val = HCI_Send_Raw_Command(BluetoothStackID, SendHCICommandActionData->OGF, SendHCICommandActionData->OCF, SendHCICommandActionData->CommandDataLength, SendHCICommandActionData->CommandData, &Status, &Length, Buffer, TRUE);

      /* Map the return results into an error code.                     */
      if(!ret_val)
         ret_val = MapSendRawResults(ret_val, Status, Length, Buffer);
   }
   else
   {
      /* Skip the baud rate change command so that the Host and         */
      /* Controller Baud Rates do not get out of sync.                  */
      ret_val = 0;
   }

   return(ret_val);
}

   /* The following function executes a BTS Script from a file in the   */
   /* file system.  This function returns 0 on success and a negative   */
   /* error code otherwise.                                             */
static int ExecuteScriptFile(unsigned int BluetoothStackID, TIBTS_Script_Version_Info_t *ScriptVersionInfo)
{
   int                            ret_val;
   int                            Status;
   char                           ScriptPath[sizeof("/lib/firmware/") + sizeof("TIInit_xxx.xxx.xxx.bts")];
   TIBTS_Script_Open_Parameters_t ScriptOpenParameters;
   TIBTS_Script_Handle_t          ScriptHandle;
   TIBTS_Script_Action_t          ScriptAction;

   /* Using the Script's Version Information create the expected BTS    */
   /* Script's filename.                                                */
   if((ret_val = BTPS_SprintF(ScriptPath, "/lib/firmware/TIInit_%u.%u.%u.bts", ScriptVersionInfo->ProjectVersion, ScriptVersionInfo->MajorVersion, ScriptVersionInfo->MinorVersion)) > 0)
   {
      printf("Status: Executing BTS Script %s.\r\n", ScriptPath);

      /* Configure the Script Open Parameters for the TIBTS library.    */
      BTPS_MemInitialize(&ScriptOpenParameters, 0x00, sizeof(TIBTS_Script_Open_Parameters_t));

      ScriptOpenParameters.ScriptFlags = 0;
      ScriptOpenParameters.ScriptPath  = ScriptPath;

      /* Attempt to open the script.                                    */
      if((ScriptHandle = TIBTS_Open_Script(&ScriptOpenParameters)))
      {
         ret_val = 0;

         /* Loop through all of the HCI Commands in the script.         */
         while((!ret_val) && (!(Status = TIBTS_Get_Next_Action(ScriptHandle, &ScriptAction))))
         {
            switch(ScriptAction.Type)
            {
               case atSendHCICommand:
                  ret_val = ProcessSendHCICommandAction(BluetoothStackID, &ScriptAction.ActionData.SendHCICommandActionData);
                  break;

               case atDelay:
                  BTPS_Delay((unsigned long)(ScriptAction.ActionData.DelayActionData.Delay));
                  break;

               default:
                  printf("Error: Unexpected action type %d returned from TIBTS_Get_Next_Action().\r\n", ScriptAction.Type);
                  ret_val = TIBTS_ERROR_INVALID_SCRIPT_FORMAT;
                  break;
            }
         }

         /* Check if any error occurred while processing the script.    */
         if(!ret_val)
         {
            /* No error occurred, check if the status returned from     */
            /* getting the last command was an end of file error.       */
            if(Status != TIBTS_ERROR_END_OF_SCRIPT)
            {
               printf("Error: TIBTS_Get_Next_Action() returned unexpected result (%d).\r\n", Status);

               /* The status of the last function call to get the next  */
               /* HCI Command was not an end of file error as it should */
               /* have been, flag the error to the caller.              */
               ret_val = TIBTS_ERROR_INVALID_SCRIPT_FORMAT;
            }
         }

         /* Close the script.                                           */
         TIBTS_Close_Script(ScriptHandle);
      }
      else
      {
         printf("Error: Could not open the script file %s.\r\n", ScriptPath);
         ret_val = BTPS_ERROR_INVALID_PARAMETER;
      }
   }
   else
   {
      printf("Error: Could not create the filename. BTPS_SprintF() Result: %d.\r\n", ret_val);
      ret_val = BTPS_ERROR_INVALID_PARAMETER;
   }

   return(ret_val);
}

#endif


   /* The following function prototype represents the vendor specific   */
   /* function which is used to implement any needed Bluetooth device   */
   /* vendor specific functionality that needs to be performed before   */
   /* the HCI Communications layer is opened.  This function is called  */
   /* immediately prior to calling the initialization of the HCI        */
   /* Communications layer.  This function should return a BOOLEAN TRUE */
   /* indicating successful completion or should return FALSE to        */
   /* indicate unsuccessful completion.  If an error is returned the    */
   /* stack will fail the initialization process.                       */
   /* * NOTE * The parameter passed to this function is the exact       */
   /*          same parameter that was passed to BSC_Initialize() for   */
   /*          stack initialization.  If this function changes any      */
   /*          members that this pointer points to, it will change the  */
   /*          structure that was originally passed.                    */
   /* * NOTE * No HCI communication calls are possible to be used in    */
   /*          this function because the driver has not been initialized*/
   /*          at the time this function is called.                     */
Boolean_t BTPSAPI HCI_VS_InitializeBeforeHCIOpen(HCI_DriverInformation_t *HCI_DriverInformation)
{
#ifdef SHARED_TRANSPORT
   /* Flag that we have not issued the first Vendor Specific Commands   */
   /* before the first reset.                                           */
   VendorCommandsIssued = FALSE;

   /* Initialize the COMM information structure.                        */
   HCI_DRIVER_SET_COMM_INFORMATION(HCI_DriverInformation, 0, 0, cpHCILL_RTS_CTS);
   
   /* Specify the COM device's name.                                    */
   HCI_DriverInformation->DriverInformation.COMMDriverInformation.COMDeviceName = "/dev/hci_tty";   
   
   /* Specify the COM Port Number to be -1 to let the HCI driver know   */
   /* that we have specified the COMM device name and that it does not  */
   /* need to append a number to the default device name prefix.        */
   HCI_DriverInformation->DriverInformation.COMMDriverInformation.COMPortNumber = -1;
#else


   static char install_sysfs_entry[48];
   static char dev_name_sysfs[48];
   static char baud_rate_sysfs[48];
   static char flow_cntrl_sysfs[48];
   static char nshutdown_gpio_sysfs[48];
   char *temp_path;
   unsigned char buf[32];
   char *name_string;
   int  name_string_len, len, fd, flow_type, com_num;
   /* For text file parameters reading */
   FILE *fp = NULL;
   InitializationScriptLoaded = FALSE;

   fp = fopen(TIBT_SYSFS_BASE_DYNAMIC, "r");
   /* Check if the dynamic configuration file exist */
   /* The configuration structure:                  */
   /*	nshutdown_gpio=117
	*	dev_name="/dev/ttyS1"
	*	flow_cntrl=1
	*	baud_rate=3000000
    */
   if(fp) {
	   char line[256];
	   char *paramStr;
	   char *equalChar = "=";

	   /* Read the first parameter- The nShutDown GPIO */
	   if(fgets(line, sizeof(line), fp) != NULL) {
		   nShutDownGpio = GetParamFromStr(line);
	   } else {
		   printf("Error: Can't read parameters from file");
	   }

	   /* Read the second parameter- The device name */
	   if(fgets(line, sizeof(line), fp) != NULL) {
		   name_string_len = 0;
		   paramStr = strstr((char *)line, equalChar);
		   /* Increment by one to point the first char after the '=' */
		   paramStr++;
		   while(paramStr[name_string_len] != 0) {
			   name_string_len++;
		   }
		   /* Clear the '/n' at the end of the line */
		   paramStr[name_string_len-1] = 0;
		   name_string = BTPS_AllocateMemory(name_string_len);
		   strcpy(name_string, paramStr);
	   } else {
		   printf("Error: Can't read parameters from file");
	   }

	   /* Read the third parameter- The flow control type */
	   if(fgets(line, sizeof(line), fp) != NULL) {
		   flow_type = GetParamFromStr(line);
	   } else {
		   printf("Error: Can't read parameters from file");
	   }

	   /* Read the forth parameter- The application baud-rate */
	   if(fgets(line, sizeof(line), fp) != NULL) {
		   HCI_DriverInformation->DriverInformation.COMMDriverInformation.BaudRate = GetParamFromStr(line);
	   } else {
		   printf("Error: Can't read parameters from file");
	   }
       fclose (fp);
   } else {
	   /* For DTS file structure reading */
	   temp_path = TIBT_SYSFS_BASE;
	   strcpy(install_sysfs_entry, temp_path);
	   strcpy(dev_name_sysfs, temp_path);
	   strcpy(baud_rate_sysfs, temp_path);
	   strcpy(flow_cntrl_sysfs, temp_path);
	   strcpy(nshutdown_gpio_sysfs, temp_path);

	   strcat(install_sysfs_entry, "/install");
	   strcat(dev_name_sysfs, "/dev_name");
	   strcat(baud_rate_sysfs, "/baud_rate");
	   strcat(flow_cntrl_sysfs, "/flow_cntrl");
	   strcat(nshutdown_gpio_sysfs, "/nshutdown_gpio");

	   /* Read the device name-  */
// 	   memset(buf, 0, 32);
// 	   fd = open(dev_name_sysfs, O_RDONLY);
// 	   if (fd < 0) {
// 		   printf("Error: Can't open %s", dev_name_sysfs);
// 	   }
//        else
//        {
//     	   len = read(fd, buf, 32);
//     	   if(len > 0) {
    		name_string_len = 10;
    		name_string = BTPS_AllocateMemory(name_string_len);
			strcpy(name_string, "/dev/ttyO5");
//     	   } else {
//     		   printf("Error: read length = %d \r\n", len);
//     	   }
//            close (fd);
       }
   	   /* Read the target baud rate-  */
/*   	   memset(buf, 0, 32);
	   fd = open(baud_rate_sysfs, O_RDONLY);
	   if (fd < 0) {
		   printf("Error: Can't open %s", baud_rate_sysfs);
	   }
       else
       {
    	   len = read(fd, buf, 32);
     	   if(len > 3) {*/
    		   HCI_DriverInformation->DriverInformation.COMMDriverInformation.BaudRate = 115200;
//     				   (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
//     	   } else {
//     		   printf("Error: read length = %d \r\n", len);
//     	   }
//            close (fd);            
//        }
	   /* Read the device UART flow control-  */
// 	   memset(buf, 0, 32);
// 	   fd = open(flow_cntrl_sysfs, O_RDONLY);
// 	   if (fd < 0) {
// 		   printf("Error: Can't open %s", flow_cntrl_sysfs);
// 	   }
//        else
//        {
//     	   len = read(fd, buf, 32);
//     	   if(len > 3) {
//     		   /* The flow type is enum 0-11 in HCICommT.h file- HCI_COMM_Protocol_t */
//     		   /* Use the define BLUETOOTH_HCI_UART_PROTOCOL for now */
    		   flow_type = 1;
//     	   } else {
//     		   printf("Error: read length = %d \r\n", len);
//     	   }
//            close (fd);
//        }
	   /* Read the device nShutDown GPIO-  */
// 	   memset(buf, 0, 32);
// 	   fd = open(nshutdown_gpio_sysfs, O_RDONLY);
// 	   if (fd < 0) {
// 		   printf("Error: Can't open %s", nshutdown_gpio_sysfs);
// 	   }
//        else
//        {
//     	   len = read(fd, buf, 32);
//     	   if(len > 3) {
    		   nShutDownGpio = 102;
//     	   } else {
//     		   printf("Error: read length = %d \r\n", len);
//     	   }
//            close (fd);
//        }
//    }

   HCI_DRIVER_SET_COMM_INFORMATION(HCI_DriverInformation, BLUETOOTH_HCI_DEV_TTY_COM_PORT_NUMBER,
		   HCI_DriverInformation->DriverInformation.COMMDriverInformation.BaudRate, BLUETOOTH_HCI_UART_PROTOCOL);
   HCI_DriverInformation->DriverInformation.COMMDriverInformation.COMDeviceName = BTPS_AllocateMemory(name_string_len);
   strcpy(HCI_DriverInformation->DriverInformation.COMMDriverInformation.COMDeviceName, name_string);
   BTPS_FreeMemory(name_string);
   /* Save the target HCI driver configuration, we will use it when the */
   /* patch is applied to re-configure the driver.                      */
   BTPS_MemCopy(&TargetHCIDriverInformation, HCI_DriverInformation, sizeof(TargetHCIDriverInformation));

   /* Set the baud rate to the default rate that chip boots to.         */
   HCI_DriverInformation->DriverInformation.COMMDriverInformation.BaudRate = BLUETOOTH_CONTROLLER_POWER_ON_BAUD_RATE;

   /* Reset the Bluetooth Controller by disabling it off if it's not    */
   /* already disabled, and then re-enable it.                          */
   Disable_Bluetooth_Controller();
   Enable_Bluetooth_Controller();

#endif
   return(TRUE);
}

   /* The following function prototype represents the vendor specific   */
   /* function which is used to implement any needed Bluetooth device   */
   /* vendor specific functionality after the HCI Communications layer  */
   /* is initialized (the driver only).  This function is called        */
   /* immediately after returning from the initialization of the HCI    */
   /* Communications layer (HCI Driver).  This function should return a */
   /* BOOLEAN TRUE indicating successful completion or should return    */
   /* FALSE to indicate unsuccessful completion.  If an error is        */
   /* returned the stack will fail the initialization process.          */
   /* * NOTE * No HCI layer function calls are possible to be used in   */
   /*          this function because the actual stack has not been      */
   /*          initialized at this point.  The only initialization that */
   /*          has occurred is with the HCI Driver (hence the HCI       */
   /*          Driver ID that is passed to this function).              */
Boolean_t BTPSAPI HCI_VS_InitializeAfterHCIOpen(unsigned int HCIDriverID)
{
   return(TRUE);
}

   /* The following function prototype represents the vendor specific   */
   /* function which is used to implement any needed Bluetooth device   */
   /* vendor specific functions after the HCI Communications layer AND  */
   /* the HCI Stack layer has been initialized.  This function is called*/
   /* after all HCI functionality is established, but before the initial*/
   /* HCI Reset is sent to the stack.  The function should return a     */
   /* BOOLEAN TRUE to indicate successful completion or should return   */
   /* FALSE to indicate unsuccessful completion.  If an error is        */
   /* returned the stack will fail the initialization process.          */
   /* * NOTE * At the time this function is called HCI Driver and HCI   */
   /*          layer functions can be called, however no other stack    */
   /*          layer functions are able to be called at this time       */
   /*          (hence the HCI Driver ID and the Bluetooth Stack ID      */
   /*          passed to this function).                                */
Boolean_t BTPSAPI HCI_VS_InitializeBeforeHCIReset(unsigned int HCIDriverID, unsigned int BluetoothStackID)
{
#ifdef SHARED_TRANSPORT
   /* If we haven't issued the Vendor Specific Commands yet, then go    */
   /* ahead and issue them.  If we have, then there isn't anything to   */
   /* do.                                                               */
   if(!VendorCommandsIssued)
   {
      /* Flag that we have issued the Vendor Specific Commands (so we   */
      /* don't do it again if someone issues an HCI_Reset().            */
      VendorCommandsIssued = TRUE;
   }
#endif
   return(TRUE);
}

   /* The following function prototype represents the vendor specific   */
   /* function which is used to implement any needed Bluetooth device   */
   /* vendor specific functionality after the HCI layer has issued any  */
   /* HCI Reset as part of the initialization.  This function is called */
   /* after all HCI functionality is established, just after the initial*/
   /* HCI Reset is sent to the stack.  The function should return a     */
   /* BOOLEAN TRUE to indicate successful completion or should return   */
   /* FALSE to indicate unsuccessful completion.  If an error is        */
   /* returned the stack will fail the initialization process.          */
   /* * NOTE * At the time this function is called HCI Driver and HCI   */
   /*          layer functions can be called, however no other stack    */
   /*          layer functions are able to be called at this time (hence*/
   /*          the HCI Driver ID and the Bluetooth Stack ID passed to   */
   /*          this function).                                          */
Boolean_t BTPSAPI HCI_VS_InitializeAfterHCIReset(unsigned int HCIDriverID, unsigned int BluetoothStackID)
{
#ifdef SHARED_TRANSPORT
   return(TRUE);
#else   
   int                         ret_val = -1;
   TIBTS_Script_Version_Info_t ScriptVersionInfo;

   /* Verify that the parameters that were passed in appear valid.      */
   if((HCIDriverID) && (BluetoothStackID))
   {
      if(!InitializationScriptLoaded)
      {
         /* Check to see if we need to change the Baud Rate.            */
         if(TargetHCIDriverInformation.DriverInformation.COMMDriverInformation.BaudRate != BLUETOOTH_CONTROLLER_POWER_ON_BAUD_RATE)
         {
            printf("Changing HCI baud rate to %lu\r\n", TargetHCIDriverInformation.DriverInformation.COMMDriverInformation.BaudRate);

            if((ret_val = ConfigureHCIBaudRate(BluetoothStackID, TargetHCIDriverInformation.DriverInformation.COMMDriverInformation.BaudRate)) != 0)
               printf("Error: Could not change Bluetooth Chip's Baud Rate to %lu. BTS_Configure_HCI_Baud_Rate() Result: %d.\r\n", TargetHCIDriverInformation.DriverInformation.COMMDriverInformation.BaudRate, ret_val);
         }
         else
            ret_val = 0;

         /* Check if an error occurred.                                 */
         if(!ret_val)
         {
            /* Get the Script Version Information from the Controller.  */
            if(!(ret_val = TIBTS_Get_Script_Version(BluetoothStackID, &ScriptVersionInfo)))
            {
               /* An error did not occur, attempt to execute the BTS    */
               /* Initialization Script.                                */
               if(!(ret_val = ExecuteScriptFile(BluetoothStackID, &ScriptVersionInfo)))
               {
                  printf("Status: BTS Script successfully executed.\r\n");

                  /* An error did not occur, flag that the              */
                  /* initialization script has been loaded.             */
                  InitializationScriptLoaded = TRUE;
               }
            }
            else
            {
               printf("Error: Could not get the script version from the controller. TIBTS_Get_Script_Version() Result: %d.\r\n", ret_val);
            }
         }
      }
   }

   /* Check if an error occurred in the processing above.               */
   if(ret_val)
   {
      /* An error occurred, disable the Bluetooth Controller.           */
      Disable_Bluetooth_Controller();
   }

   return((Boolean_t)(!ret_val));
#endif
}

   /* The following function prototype represents the vendor specific   */
   /* function which would is used to implement any needed Bluetooth    */
   /* device vendor specific functionality before the HCI layer is      */
   /* closed.  This function is called at the start of the HCI_Cleanup()*/
   /* function (before the HCI layer is closed), at which time all HCI  */
   /* functions are still operational.  The caller is NOT able to call  */
   /* any other stack functions other than the HCI layer and HCI Driver */
   /* layer functions because the stack is being shutdown (i.e.         */
   /* something has called BSC_Shutdown()).  The caller is free to      */
   /* return either success (TRUE) or failure (FALSE), however, it will */
   /* not circumvent the closing down of the stack or of the HCI layer  */
   /* or HCI Driver (i.e. the stack ignores the return value from this  */
   /* function).                                                        */
   /* * NOTE * At the time this function is called HCI Driver and HCI   */
   /*          layer functions can be called, however no other stack    */
   /*          layer functions are able to be called at this time (hence*/
   /*          the HCI Driver ID and the Bluetooth Stack ID passed to   */
   /*          this function).                                          */
Boolean_t BTPSAPI HCI_VS_InitializeBeforeHCIClose(unsigned int HCIDriverID, unsigned int BluetoothStackID)
{
   return(TRUE);
}

   /* The following function prototype represents the vendor specific   */
   /* function which is used to implement any needed Bluetooth device   */
   /* vendor specific functionality after the entire Bluetooth Stack is */
   /* closed.  This function is called during the HCI_Cleanup()         */
   /* function, after the HCI Driver has been closed.  The caller is    */
   /* free return either success (TRUE) or failure (FALSE), however, it */
   /* will not circumvent the closing down of the stack as all layers   */
   /* have already been closed.                                         */
   /* * NOTE * No Stack calls are possible in this function because the */
   /*          entire stack has been closed down at the time this       */
   /*          function is called.                                      */
Boolean_t BTPSAPI HCI_VS_InitializeAfterHCIClose(void)
{
#ifndef SHARED_TRANSPORT
   Disable_Bluetooth_Controller();
#endif
   return(TRUE);
}

   /* The following function prototype represents the vendor specific   */
   /* function which is used to enable a specific vendor specific       */
   /* feature.  This can be used to reconfigure the chip for a specific */
   /* feature (i.e. if a special configuration/patch needs to be        */
   /* dynamically loaded it can be done in this function).  This        */
   /* function returns TRUE if the feature was able to be enabled       */
   /* successfully, or FALSE if the feature was unable to be enabled.   */
   /* * NOTE * This functionality is not normally supported by default  */
   /*          (i.e. a custom stack build is required to enable this    */
   /*          functionality).                                          */
Boolean_t BTPSAPI HCI_VS_EnableFeature(unsigned int BluetoothStackID, unsigned long Feature)
{
   return(FALSE);
}

   /* The following function prototype represents the vendor specific   */
   /* function which is used to enable a specific vendor specific       */
   /* feature.  This can be used to reconfigure the chip for a specific */
   /* feature (i.e. if a special configuration/patch needs to be        */
   /* dynamically loaded it can be done in this function).  This        */
   /* function returns TRUE if the feature was able to be disabled      */
   /* successfully, or FALSE if the feature was unable to be disabled.  */
   /* * NOTE * This functionality is not normally supported by default  */
   /*          (i.e. a custom stack build is required to enable this    */
   /*          functionality).                                          */
Boolean_t BTPSAPI HCI_VS_DisableFeature(unsigned int BluetoothStackID, unsigned long Feature)
{
   return(FALSE);
}

