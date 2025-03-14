/* SPDX-License-Identifier:     GPL-2.0+ */
/*
 * (C) Copyright 2020 Rockchip Electronics Co., Ltd
 *
 */

#ifndef __CONFIG_RK3568_COMMON_H
#define __CONFIG_RK3568_COMMON_H

#include "rockchip-common.h"


/* #define CONFIG_CMD_USBUPGRADE */

#define MATRIX_PATCH 1
#define CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_TEXT_BASE		0x00000000
#define CONFIG_SPL_MAX_SIZE		0x00040000
#define CONFIG_SPL_BSS_START_ADDR	0x03fe0000
#define CONFIG_SPL_BSS_MAX_SIZE		0x00010000
#define CONFIG_SPL_STACK		0x03fe0000

#define CONFIG_SYS_MALLOC_LEN		(32 << 20)
#define CONFIG_SYS_CBSIZE		1024
#define CONFIG_SKIP_LOWLEVEL_INIT

#ifdef CONFIG_SUPPORT_USBPLUG
#define CONFIG_SYS_TEXT_BASE		0x00000000
#else
#define CONFIG_SYS_TEXT_BASE		0x00a00000
#endif

#define CONFIG_SYS_INIT_SP_ADDR		0x00c00000
#define CONFIG_SYS_LOAD_ADDR		0x00c00800
#define CONFIG_SYS_BOOTM_LEN		(64 << 20)	/* 64M */
#define COUNTER_FREQUENCY		24000000

#define GICD_BASE			0xfd400000
#define GICR_BASE			0xfd460000
#define GICC_BASE			0xfd800000

/* secure otp */
#define OTP_UBOOT_ROLLBACK_OFFSET	0xe0
#define OTP_UBOOT_ROLLBACK_WORDS	2	/* 64 bits, 2 words */
#define OTP_ALL_ONES_NUM_BITS		32
#define OTP_SECURE_BOOT_ENABLE_ADDR	0x80
#define OTP_SECURE_BOOT_ENABLE_SIZE	2
#define OTP_RSA_HASH_ADDR		0x90
#define OTP_RSA_HASH_SIZE		32

/* MMC/SD IP block */
#define CONFIG_BOUNCE_BUFFER
#ifdef CONFIG_AHCI
#define CONFIG_SYS_SCSI_MAX_SCSI_ID	1
#define CONFIG_SYS_SCSI_MAX_LUN		1
/*#define CONFIG_SCSI_AHCI_PLAT */
#define CONFIG_SYS_SCSI_MAX_DEVICE	(CONFIG_SYS_SCSI_MAX_SCSI_ID * \
					 CONFIG_SYS_SCSI_MAX_LUN)
#endif
/* Nand */
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_SYS_NAND_PAGE_SIZE	2048
#define CONFIG_SYS_NAND_PAGE_COUNT	64
#define CONFIG_SYS_NAND_SIZE		(256 * 1024 * 1024)

#define CONFIG_SYS_SDRAM_BASE		0
#define SDRAM_MAX_SIZE			0xf0000000
#define CONFIG_PREBOOT			"setenv ethact ethernet@fe010000; setenv ethrotate no; automac; usbupgrade;"

#define CONFIG_SYS_NONCACHED_MEMORY	(1 << 20)	/* 1 MiB */

#ifndef CONFIG_SPL_BUILD
/* usb mass storage */
#define CONFIG_USB_FUNCTION_MASS_STORAGE
#define CONFIG_ROCKUSB_G_DNL_PID	0x350a

#define ENV_MEM_LAYOUT_SETTINGS \
	"scriptaddr=0x00c00000\0" \
	"pxefile_addr_r=0x00e00000\0" \
	"fdt_addr_r=0x0a100000\0" \
	"kernel_addr_no_low_bl32_r=0x00280000\0" \
	"kernel_addr_r=0x00a80000\0" \
	"kernel_addr_c=0x04080000\0" \
	"ramdisk_addr_r=0x0a200000\0"

#include <config_distro_bootcmd.h>

#define CONFIG_EXTRA_ENV_SETTINGS \
	ENV_MEM_LAYOUT_SETTINGS \
	"partitions=" PARTS_RKIMG \
	ROCKCHIP_DEVICE_SETTINGS \
	RKIMG_DET_BOOTDEV \
	BOOTENV

/* multiplex change for watchdog pin */	
#undef RKIMG_BOOTCOMMAND
#define RKIMG_BOOTCOMMAND		\
	"mw 0xFDC60020 0xFFFF2001;"	\
	"boot_fit;"			\
	"boot_android ${devtype} ${devnum};" \
	"run distro_bootcmd;"
#endif

/* rockchip ohci host driver */
#define CONFIG_USB_OHCI_NEW
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	1

#define CONFIG_LIB_HW_RAND

#define CONFIG_ENV_OVERWRITE

#endif
