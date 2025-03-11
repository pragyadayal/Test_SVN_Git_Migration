#include <mpi_vo.h>
#include <hi_comm_vo.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include "hifb.h"
#include "hi_common.h"
#include "hi_buffer.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_vo.h"
#include "hi_defines.h"
#include "hi_comm_hdmi.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vo.h"

#ifdef HI_SUCCESS
#undef HI_SUCCESS
#endif
#define HI_SUCCESS 0

#ifdef HI_FAILURE
#undef HI_FAILURE
#endif
#define HI_FAILURE -1

#define GRAPHICS_LAYER_G0      0
#define GRAPHICS_LAYER_G1      1
#define GRAPHICS_LAYER_G3      2

#define IMAGE_WIDTH     320
#define IMAGE_HEIGHT    480

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	480

#define FB_DEVICE "/dev/fb1"

#define PIXEL_18BIT 1

#ifdef PIXEL_18BIT

	#define HIFB_RED_1555          0xFC00 //green 0x83E0 //red 0xFC00 //blue 0x801F
	#define HIFB_BLUE_1555         0x801F //green 0x83E0 //red 0xFC00 //blue 0x801F
	#define HIFB_GREEN_1555         0x83E0 //green 0x83E0 //red 0xFC00 //blue 0x801F

#else
	#define HIFB_RED_8888          0xFFFF0000
#endif

int frameBufFd = -1;


static void lcd_ili9488_cmd(int data)
{
	FILE *fp;
	fp = fopen("/tmp/sndData.txt", "w");
	
	if(fp)
	{
		data = data & 0x0ff;
		fwrite(&data, 2, 1, fp);
		fclose(fp);
	}
	else
	{
		printf("fail to open file\n");
	}
	
	system("spi-pipe -d /dev/spidev2.0 -b 2 < /tmp/sndData.txt");
}
static void lcd_ili9488_index(int data)
{
	FILE *fp;
	fp = fopen("/tmp/sndData.txt", "w");
	
	if(fp)
	{
		data = data | 0x100;
		fwrite(&data, 2, 1, fp);
		fclose(fp);
	}
	else
	{
		printf("fail to open file\n");
	}
	
	system("spi-pipe -d /dev/spidev2.0 -b 2 < /tmp/sndData.txt");
}

static void init_lcd(void)
{
	// VCI=2.8V 
	//************* Reset LCD Driver ****************// 
	//LCD_nRESET = 1; 
	//Delayms(1); // Delay 1ms 
	//LCD_nRESET = 0; 
	//Delayms(10); // Delay 10ms // This delay time is necessary 
	//LCD_nRESET = 1; 
	//Delayms(120); // Delay 120 ms 
	//************* Start Initial Sequence **********// 

	system("spi-config -d /dev/spidev2.0 -b9 -m 0 -s 100000");
	lcd_ili9488_cmd(0xE0);      //P-Gamma 
	lcd_ili9488_index(0x00); 
	lcd_ili9488_index(0x10); 
	lcd_ili9488_index(0x14); 
	lcd_ili9488_index(0x01); 
	lcd_ili9488_index(0x0E); 
	lcd_ili9488_index(0x04); 
	lcd_ili9488_index(0x33); 
	lcd_ili9488_index(0x56); 
	lcd_ili9488_index(0x48); 
	lcd_ili9488_index(0x03); 
	lcd_ili9488_index(0x0C); 
	lcd_ili9488_index(0x0B); 
	lcd_ili9488_index(0x2B); 
	lcd_ili9488_index(0x34); 
	lcd_ili9488_index(0x0F); 
	
	lcd_ili9488_cmd(0XE1);      //N-Gamma 
	lcd_ili9488_index(0x00); 
	lcd_ili9488_index(0x12); 
	lcd_ili9488_index(0x18); 
	lcd_ili9488_index(0x05); 
	lcd_ili9488_index(0x12); 
	lcd_ili9488_index(0x06); 
	lcd_ili9488_index(0x40); 
	lcd_ili9488_index(0x34); 
	lcd_ili9488_index(0x57); 
	lcd_ili9488_index(0x06); 
	lcd_ili9488_index(0x10); 
	lcd_ili9488_index(0x0C); 
	lcd_ili9488_index(0x3B); 
	lcd_ili9488_index(0x3F); 
	lcd_ili9488_index(0x0F); 
	
	lcd_ili9488_cmd(0XC0);      //Power Control 1 
	lcd_ili9488_index(0x0F);    //Vreg1out 
	lcd_ili9488_index(0x0C);    //Verg2out 
	
	lcd_ili9488_cmd(0xC1);      //Power Control 2 
	lcd_ili9488_index(0x41);    //VGH,VGL 
	
	lcd_ili9488_cmd(0xC5);      //Power Control 3 
	lcd_ili9488_index(0x00); 
	lcd_ili9488_index(0x25);    //Vcom 
	lcd_ili9488_index(0x80); 
	
	lcd_ili9488_cmd(0x36);      //Memory Access 
	lcd_ili9488_index(0x48); 
	
	lcd_ili9488_cmd(0x3A);      // Interface Pixel Format 
	lcd_ili9488_index(0x66);    //18bit 
	
	lcd_ili9488_cmd(0XB0);      // Interface Mode Control 
	lcd_ili9488_index(0x02);     
	
	lcd_ili9488_cmd(0xB1);      //Frame rate 
	lcd_ili9488_index(0xA0);    //60Hz 
	
	lcd_ili9488_cmd(0xB4);      //Display Inversion Control 
	lcd_ili9488_index(0x02);    //2-dot 
	
	lcd_ili9488_cmd(0XB6);      //RGB/MCU Interface Control 
	lcd_ili9488_index(0x72);    //MCU 
	lcd_ili9488_index(0x02);    //Source,Gate scan dieection 

	lcd_ili9488_cmd(0XB5);      //RGB/MCU Interface Control 
	lcd_ili9488_index(2);    //MCU 
	lcd_ili9488_index(4);    //Source,Gate scan dieection 
	lcd_ili9488_index(49);    //MCU 
	lcd_ili9488_index(54);    //Source,Gate scan dieection 
	
	
	lcd_ili9488_cmd(0XE9);      // Set Image Function   
	lcd_ili9488_index(0x00);    //disable 24 bit data input 
	
	lcd_ili9488_cmd(0xF7);     //   A d j u s t   C o n t r o l 
	lcd_ili9488_index(0xA9);     
	lcd_ili9488_index(0x51);     
	lcd_ili9488_index(0x2C);     
	lcd_ili9488_index(0x82);   //   D 7   s t r e a m ,   l o o s e  
	
	lcd_ili9488_cmd(0x21);      //Normal Black 
	
	lcd_ili9488_cmd(0x11);     //Sleep out 
	usleep(120000); 
	lcd_ili9488_cmd(0x29);    //Display on 

}

/******************************************************************************
* function : vb init & MPI system init
******************************************************************************/
static HI_S32 sys_init(void)
{

	HI_S32 s32Ret = HI_FAILURE;
	VB_CONFIG_S stVbConf;

	memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

	HI_MPI_SYS_Exit();
	HI_MPI_VB_Exit();


	s32Ret = HI_MPI_VB_SetConfig(&stVbConf);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_VB_SetConf failed!\n");
		return HI_FAILURE;
	}

	s32Ret = HI_MPI_VB_Init();
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_VB_Init failed!\n");
		return HI_FAILURE;
	}

	s32Ret = HI_MPI_SYS_Init();
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_SYS_Init failed!\n");
		HI_MPI_VB_Exit();
		return HI_FAILURE;
	}

	return HI_SUCCESS;
}


static HI_S32 startVoDev(void)
{
	VO_PUB_ATTR_S stPubAttr;
	HI_S32 s32Ret;
	VO_DEV VoDev = GRAPHICS_LAYER_G1;
	VO_USER_INTFSYNC_INFO_S stUserInfo = {0};
	HI_U32 u32Framerate = 60;
	VO_CSC_S csc_s={0};
	
	//18 bit LCD interface 
#ifdef PIXEL_18BIT
	stPubAttr.enIntfType = VO_INTF_LCD_18BIT;
#else
	stPubAttr.enIntfType = VO_INTF_LCD_24BIT;
#endif
	stPubAttr.enIntfSync = VO_OUTPUT_USER;
	stPubAttr.stSyncInfo.bSynm = 0; /* RW; sync mode(0:timing,as BT.656; 1:signal,as LCD) */
	stPubAttr.stSyncInfo.bIop = 1; /* RW; interlaced or progressive display(0:i; 1:p) */
	stPubAttr.stSyncInfo.u8Intfb = 0; /* RW; interlace bit width while output */

	stPubAttr.stSyncInfo.u16Vact = 480; /* RW; vertical active area */
	stPubAttr.stSyncInfo.u16Vbb = 4; /* RW; vertical back blank porch */
	stPubAttr.stSyncInfo.u16Vfb = 2; /* RW; vertical front blank porch */ 

	stPubAttr.stSyncInfo.u16Hact = 320; /* RW; horizontal active area */
	stPubAttr.stSyncInfo.u16Hbb = 54; /* RW; horizontal back blank porch */
	stPubAttr.stSyncInfo.u16Hfb = 49; /* RW; horizontal front blank porch */ 

	stPubAttr.stSyncInfo.u16Hmid = 1; /* RW; bottom horizontal active area */
	stPubAttr.stSyncInfo.u16Bvact = 1; /* RW; bottom vertical active area */
	stPubAttr.stSyncInfo.u16Bvbb = 1; /* RW; bottom vertical back blank porch */
	stPubAttr.stSyncInfo.u16Bvfb = 1; /* RW; bottom vertical front blank porch */

	stPubAttr.stSyncInfo.u16Hpw = 5; /* RW; horizontal pulse width */
	stPubAttr.stSyncInfo.u16Vpw = 2; /* RW; vertical pulse width */

	stPubAttr.stSyncInfo.bIdv = 0; /* RW; inverse data valid of output */ 
	stPubAttr.stSyncInfo.bIhs = 1; /* RW; inverse horizontal synch signal */
	stPubAttr.stSyncInfo.bIvs = 1; /* RW; inverse vertical synch signal */    
	
	
	//set the back ground color
	stPubAttr.u32BgColor = 0x00;
	
	s32Ret = HI_MPI_VO_SetPubAttr(VoDev, &stPubAttr);
	if (s32Ret != HI_SUCCESS)
	{
		printf("Set device attributes failed with errno %#x!\n", s32Ret);
		return HI_FAILURE;
	}
	
	HI_MPI_VO_SetDevFrameRate(VoDev, u32Framerate);

	stUserInfo.stUserIntfSyncAttr.enClkSource = VO_CLK_SOURCE_LCDMCLK;
	stUserInfo.stUserIntfSyncAttr.u32LcdMClkDiv = 0x159A8D; /* clk frequency 12.531 MHz */ //159A8D
	stUserInfo.u32PreDiv = 1;
	stUserInfo.u32DevDiv = 1;
	stUserInfo.bClkReverse = 0;

	s32Ret = HI_MPI_VO_SetUserIntfSyncInfo(VoDev, &stUserInfo);
	if (s32Ret != HI_SUCCESS)
	{
		printf ("Set user interface sync info failed with %x.\n", s32Ret);
		return HI_FAILURE;
	}
	
	printf("Enable VO device\n");
	//Enable video outout interface
	s32Ret = HI_MPI_VO_Enable(VoDev);
	if (s32Ret != HI_SUCCESS)
	{
		printf("VO Enable failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}
	
	printf("set the CSC VO device\n");
	memset(&csc_s,0,sizeof(VO_CSC_S));

	csc_s.enCscMatrix = VO_CSC_MATRIX_IDENTITY;
	s32Ret = HI_MPI_VO_SetGraphicLayerCSC(VoDev ,&csc_s);
	if (HI_SUCCESS != s32Ret)
	{
		printf("HI_MPI_VO_SetGraphicLayerCSC failed with 0x%x!\n", s32Ret);
		return s32Ret;
	}
	
	return HI_SUCCESS;
}


static HI_S32 getFbInfo(HI_U32 u32Width, HI_U32 u32Height)
{
	struct fb_var_screeninfo var;
	
#ifdef PIXEL_18BIT
	static struct fb_bitfield s_r16 = {10, 5, 0};
	static struct fb_bitfield s_g16 = {5, 5, 0};
	static struct fb_bitfield s_b16 = {0, 5, 0};
	static struct fb_bitfield s_a16 = {15, 1, 0};
#else
	
	static struct fb_bitfield s_a32 = {24, 8, 0};
	static struct fb_bitfield s_r32 = {16, 8, 0};
	static struct fb_bitfield s_g32 = {8,  8, 0};
	static struct fb_bitfield s_b32 = {0,  8, 0};
#endif
	
	HI_BOOL bShow;
	HIFB_CAPABILITY_S stCapability;
	HIFB_POINT_S stPoint;
	
	if (frameBufFd < 0)
	{
		frameBufFd = open(FB_DEVICE, O_RDWR | O_CLOEXEC);
		if (frameBufFd < 0)
		{
			perror("open hifb0");
			return HI_FAILURE;
		}
	} 
	else 
	{
		printf("already fd open [frameBufFd=%d]",frameBufFd);
	}
	
	bShow = HI_FALSE;
	if (ioctl(frameBufFd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
	{
		printf("FBIOPUT_SHOW_HIFB failed!\n");
		return HI_FAILURE;
	}
	
	
	memset(&stCapability, 0, sizeof(HIFB_CAPABILITY_S));
	if(ioctl(frameBufFd, FBIOGET_CAPABILITY_HIFB, &stCapability) < 0)
	{
		close(frameBufFd);
		frameBufFd = -1;
		printf("FBIOGET_CAPABILITY_HIFB failed\n");
		return HI_FAILURE;
	}
	printf("[bKeyRgb=%d] [bKeyAlpha=%d] [bGlobalAlpha=%d]\n",stCapability.bKeyRgb, stCapability.bKeyAlpha,stCapability.bGlobalAlpha);
	printf("[bCmap =%d] [bHasCmapReg =%d] [bVoScale =%d] [bLayerSupported =%d]\n",stCapability.bCmap, stCapability.bHasCmapReg,stCapability.bVoScale, stCapability.bLayerSupported);
	printf("[u32MaxWidth %u] [u32MaxHeight %u]\n",stCapability.u32MaxWidth, stCapability.u32MaxHeight);
	printf("[u32VDefLevel %u] [u32HDefLevel %u]\n",stCapability.u32VDefLevel, stCapability.u32HDefLevel);
	printf("[u32MinWidth %u] [u32MinHeight %u]\n",stCapability.u32MinWidth, stCapability.u32MinHeight);
	printf("[bDcmp %d] [bPreMul %d]\n",stCapability.bDcmp, stCapability.bPreMul);
	
	memset(&stPoint, 0, sizeof(HIFB_POINT_S));
	stPoint.s32XPos = 0;
	stPoint.s32YPos = 0;
	if (ioctl(frameBufFd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
	{
		close(frameBufFd);
		frameBufFd = -1;
		printf("set screen original show position failed!\n");
		return HI_FAILURE;
	}

	memset(&var, 0, sizeof(struct fb_var_screeninfo));
	if (ioctl(frameBufFd, FBIOGET_VSCREENINFO, &var) < 0)
	{
		close(frameBufFd);
		frameBufFd = -1;
		printf("Get variable screen info failed!\n");
		return HI_FAILURE;
	}
	
	var.xres = u32Width;
	var.yres = u32Height;
	var.xres_virtual = u32Width;
	var.yres_virtual = u32Height * 2;
	var.xoffset = 0;
	var.yoffset = 0;
#ifdef PIXEL_18BIT
	var.bits_per_pixel = 16;
	var.transp = s_a16;
	var.red = s_r16;
	var.green = s_g16;
	var.blue = s_b16;
	
#else	
	var.bits_per_pixel = 32;
	var.transp = s_a32;
	var.red    = s_r32;
	var.green  = s_g32;
	var.blue   = s_b32;
#endif
	var.activate = FB_ACTIVATE_NOW;
	
	if (ioctl(frameBufFd, FBIOPUT_VSCREENINFO, &var) < 0)
	{
		close(frameBufFd);
		frameBufFd = -1;
		printf("Put variable screen info failed!\n");
		return HI_FAILURE;
	}
	
	return HI_SUCCESS;

}

static HI_S32 display_image(void)
{
	HI_S32 i, x, y;
	HI_U8* pShowScreen=NULL;
	struct fb_fix_screeninfo fix;
	struct fb_var_screeninfo var;
	HI_VOID* ptemp = NULL;
	HI_BOOL bShow=HI_TRUE;
	
	if(frameBufFd <= 0)
	{
		return HI_FAILURE;
	}
	
	memset(&var, 0, sizeof(struct fb_var_screeninfo));
	if (ioctl(frameBufFd, FBIOGET_VSCREENINFO, &var) < 0)
	{
		close(frameBufFd);
		frameBufFd = -1;
		printf("Get variable screen info failed!\n");
		return HI_FAILURE;
	}
	printf("vscreen info start --------------\n");
	printf("bits_per_pixel [%d]\n",var.bits_per_pixel);
	printf("xres [%d]\n",var.xres);
	printf("yres [%d]\n",var.yres);
	printf("xres_virtual [%d]\n",var.xres_virtual);
	printf("yres_virtual [%d]\n",var.yres_virtual);
	printf("height [%d]\n",var.height);
	printf("width [%d]\n",var.width);
	printf("activate [%d]\n",var.activate);

	printf("vscreen info end --------------\n");
	
	memset(&fix, 0, sizeof(struct fb_fix_screeninfo));
	if (ioctl(frameBufFd, FBIOGET_FSCREENINFO, &fix) < 0)
	{
		close(frameBufFd);
		frameBufFd = -1;
		printf("Get fix screen info failed!\n");
		return HI_FAILURE;
	}
	
	printf("fix line_length %u\n", fix.line_length);
	printf("fix smem_len %u\n", fix.smem_len);

	pShowScreen = mmap(HI_NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, frameBufFd, 0);
	
	
	if (NULL == pShowScreen)
	{
		close(frameBufFd);
		frameBufFd = -1;
		printf("mmap framebuffer failed!\n");
		return HI_FAILURE;
	}
	
	memset(pShowScreen, 0x0, fix.smem_len);
	
	if (ioctl(frameBufFd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
	{
		munmap(pShowScreen, fix.smem_len);
		close(frameBufFd);
		frameBufFd = -1;
		printf("FBIOPUT_SHOW_HIFB failed!\n");
		return HI_FAILURE;
	}
	for (i = 0; i < 1; i++)
	{
		if (i % 2)
		{
			var.yoffset = var.yres;
		}
		else
		{
			var.yoffset = 0;
		}
		
		ptemp = (pShowScreen + var.yres * fix.line_length * (i % 2));
		for (y = 100; y < 200; y++)
		{
			for (x = 0; x < 320; x++)
			{
#ifdef PIXEL_18BIT
				*((HI_U16*)ptemp + y * var.xres + x) = HIFB_RED_1555;
#else
				*((HI_U32*)ptemp + y * var.xres + x) = HIFB_RED_8888;
#endif
			}
		}
		
		printf("expected: the red box will appear!\n");
	}
	
	return HI_SUCCESS;
}


int main()
{

	//spi int
	init_lcd();
	
	if(HI_FAILURE == sys_init())
	{
		printf("sys_init failed\n");
		return HI_FAILURE;
	}
	
	if(HI_FAILURE == startVoDev())
	{
		printf("startVoDev failed\n");
		return HI_FAILURE;
	}
	
	if(HI_FAILURE == getFbInfo(SCREEN_WIDTH,SCREEN_HEIGHT))
	{
		printf("getFbInfo failed\n");
		return HI_FAILURE;
	}
	
	if(HI_FAILURE == display_image())
	{
		printf("display_image failed\n");
		return HI_FAILURE;
	}
	
	HI_MPI_SYS_Exit();
	HI_MPI_VB_Exit();
	
	return HI_SUCCESS;
}
