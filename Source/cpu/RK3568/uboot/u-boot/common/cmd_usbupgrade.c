/*
 * cmd_usbupdate.c
 *
 *	Created on: Mar 23, 2017
 *		Author: Darshan Patel
 *  DESC : This cmd is used update uboot,kernel,rootfs,userfs from USB.
 */

/*
 * Update from USB Support
 */
#include <common.h>
#include <command.h>
#include <fat.h>
#include <part.h>
#include <environment.h>
#include <malloc.h>
#include <image.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <usb.h>
#include <exports.h>

#define BOARD_NAME		"NVR_RK3568/"
#define SIGNATURE_FILE_NAME	BOARD_NAME "NVR_RK3568_signature.txt"
#define SCRIPTNAME		BOARD_NAME "NVR_RK3568_usbupgrade.img"
#define PACKAGE_FILE_NAME	BOARD_NAME "package"
//package file format 
// "NVR_RK3568/NVR_RK3568_signature.txt;NVR_RK3568/NVR_RK3568_usbupgrade.img;

#ifdef CONFIG_CMD_USBUPGRADE
unsigned char usb_upgrade_F = 0;
/*******************************************************************/
/* update - check for nvrx_usbboot.scr/nvr_usbupgrade.scr and run script */
/*******************************************************************/
int	do_usbupgrade(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int				dev = 0, part = 1;
	char 				fileBuf[200], srcBuf[3072];
	char				signfilename[100] = SIGNATURE_FILE_NAME, *signEnvPtr = NULL;
	char 				imagefilename[100] = SCRIPTNAME;
	char				*signFilePtr = NULL;
	char 				*imgFilePtr = NULL;

	long int			ret = -1;
	struct blk_desc	*stor_dev=NULL;

	ret = usb_init();
	if (ret != 0)
		goto exit;

	if (ret < 0) {
		debug("usb_init failed!\r\n");
		goto exit;
	}

	/*
	 * check whether a storage device is attached (assume that it's
	 * a USB memory stick, since nothing else should be attached).
	 */
	ret = usb_stor_scan(0);
	if (-1 == ret)
	{
		printf("No USB device found. Not initialized!\r\n");
		goto exit;
	}

	stor_dev = blk_get_dev("usb", dev);
	if (NULL == stor_dev)
	{
		debug("Unknow device type!\r\n");
		ret = -1;
		goto exit;
	}

	ret = fat_register_device(stor_dev,part);
	if (ret != 0)
	{
		debug ("\n** Unable to use %d:%d for fatls **\r\n",dev,part);
		goto exit;
	}

	ret = file_fat_detectfs();
	if (ret != 0)
	{
		debug("file_fat_detectfs failed\r\n");
		goto exit;
	}

	memset(fileBuf, 0, sizeof(fileBuf));
	
	ret = file_fat_read(PACKAGE_FILE_NAME, fileBuf, (sizeof(fileBuf) - 1) );
	if (ret > 0)
	{
		//extrace the signature and img file name from the fileBuf
		//package file format 
		// "NVR_RK3568/NVR_RK3568_signature.txt;NVR_RK3568/NVR_RK3568_usbupgrade.img;
		signFilePtr = strtok(fileBuf, ";");
		imgFilePtr = strtok(NULL, ";");
		
		if(signFilePtr != NULL)
		{
			strncpy(signfilename,signFilePtr,(sizeof(signfilename) - 1));
		}
			
		if(imgFilePtr != NULL)
		{
			strncpy(imagefilename,imgFilePtr,(sizeof(imagefilename) - 1));
		}
	}
	
	memset(fileBuf, 0, sizeof(fileBuf));
	
	signEnvPtr = env_get("signature");
	
	ret = file_fat_read (signfilename, fileBuf, (sizeof(fileBuf) - 1) );

	printf("env  (%s)\nfile (%s)\n",signEnvPtr,fileBuf);
	if( ((signEnvPtr == NULL) || (strcmp(fileBuf, signEnvPtr) != 0)) && ret != -1 )
	{
		printf("Updating ...\n");
		env_set("signature", fileBuf);
		memset(srcBuf, 0, sizeof(srcBuf) );
		ret = file_fat_read (imagefilename, srcBuf, (sizeof(srcBuf) - 1) );
		if(ret == -1)
		{
			debug("\n** Unable to read  **\n");
			goto exit;
		}
		debug ("\n%ld bytes read\n", ret);
		usb_upgrade_F = 1;
		ret = source((ulong)srcBuf, NULL);
		if(ret != 0)
		{
			printf("** script exit ret = %ld **\r\n",ret);
			usb_stop();
			return -1;
		}
		usb_stop();
		usb_upgrade_F = 0;
		return 0;
	}

exit:
	usb_upgrade_F = 0;
	usb_stop();
	return 0;
}

#if 0 
int	do_displayimage(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int ret = -1;
	char *const memwrite[4]={"mw.b","0x61000000","0xff","0x40000"};
	char *tftpload[3]={"tftp","0x61000000","\0"};
	char *fatload[5]={"fatload","usb","0x0","0x61000000","\0"};
	char *displayload[6]={"bmp","display","0x61000000","0x0","0x0","\0"};

	if(argc < 2)
	{
		 printf ("Usage:\n%s\n",cmdtp->usage);
		 return -1;
	}

	do_mem_mw(0,0,4,memwrite);

	if(usb_upgrade_F == 1)
	{
		fatload[4] = argv[1];
		ret = do_fat_fsload(0,0,5,fatload);
		if(ret != 0)
		{
			printf("\n** Unable to read %s from usb **\r\n",argv[1]);
			return -2;
		}
	}
	else
	{
		tftpload[2] = argv[1];
		ret = do_tftpb(0,0,3,tftpload);
		if(ret != 0)
		{
			printf("\n** Unable to read %s from tftp **\r\n",tftpload[2]);
			return -2;
		}
	}
	
	ret = do_bmp_display(0,0,6,displayload);
	if(ret == 1)
	{
	  printf("\n** Unable to display image **");
	}

	ret  = (unsigned int)simple_strtoul(argv[2], NULL, 10);
	if(ret == 1)
		return -2;
	else
		return 0;
}
#endif
int	do_delay(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	unsigned int count;

	if (argc < 2)
	{
		printf("Insufficient parameter!\n");
		printf ("Usage:\n%s\n", cmdtp->usage);
		return -1;
	}

	count = (unsigned int)simple_strtoul(argv[1], NULL, 10);
	udelay(count*1000);
	return 0;
}

U_BOOT_CMD(
	usbupgrade, 1, 1, do_usbupgrade,
	"read NVR_RK3568_usbupgrade.img from USB storage device and execute it",
	"usbupgrade"
);
#if 0 
U_BOOT_CMD(
	dispimg, 3, 1, do_displayimage,
	"display image i.e. dispimg <image path> <0/1>",
	""
);
#endif
U_BOOT_CMD(
	delay, 2, 1, do_delay,
	"delay in milliseconds i.e. delay <count>",
	""
);
#endif
