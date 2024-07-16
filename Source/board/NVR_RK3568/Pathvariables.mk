 
##################################  toolchain enviroment  ########################################################## 
TOOL_CHAIN_PATH =	$(LOCAL_SDK_PATH)/build/toolchain
CROSS_COMPILE	=	$(TOOL_CHAIN_PATH)/bin/aarch64-linux-
C_COMPILER		=	$(CROSS_COMPILE)gcc
CPP_COMPILER	=	$(CROSS_COMPILE)g++
LINKER			=	$(CROSS_COMPILE)ld
ARCHIVER		=	$(CROSS_COMPILE)ar
STRIP			=	$(CROSS_COMPILE)strip
RANLIB			=	$(CROSS_COMPILE)ranlib
ARCH			=	arm64
CC			=/opt/rk3568_nvr/host/bin/aarch64-linux-gcc

SHELL	=	/bin/bash 
INSTALL =	cp

#board specific source in board/board_name directory
BOARD_NAME		= NVR_RK3568
PRODUCT_VERSION		:= 8
PRODUCT_REVISION	:= 5
PRODUCT_PATCH		:= 0
PLATFORM_SW_VER=$(PRODUCT_VERSION).$(PRODUCT_REVISION).$(PRODUCT_PATCH)
APP_VERSION=V08R05
KERNEL_UPGRADE_VER = 3


#Secureboot enable or disable configurations (value = ENABLE or DISABLE)
SECUREBOOT_EN		?= DISABLE

SVN_SERVER=svn://192.168.100.5

################################################################################################

TRUNK			= Trunk
BRANCHES		= Branches
#----------------------------------Release type---------------------------------
#DEVELOPMENT =  use this tag for development purpose
#QA = use this tag for QA Release 
#MANU - use this tag for producation release 
#----------------------------------Release type---------------------------------
SUPPORTED_RELEASE_TYPE = DEVELOPMENT QA MANU

#change this tag as per requirement
#varialbe should be pass from command line (Jenkins)
RELEASE_TYPE		= DEVELOPMENT


DEVELOPMENT_BRANCH	= $(BRANCHES)/Development/$(BOARD_NAME)
QA_RELEASE_BRANCH	= $(BRANCHES)/Development
PRODUCTION_BRANCH	= $(TRUNK)

ifeq ($(filter $(SUPPORTED_RELEASE_TYPE),$(RELEASE_TYPE)),)

$(info Supported release are $(SUPPORTED_RELEASE_TYPE))
$(error Please pass a valid release type)
endif

ifeq ($(RELEASE_TYPE),DEVELOPMENT)
TAG		= $(DEVELOPMENT_BRANCH)
endif

ifeq ($(RELEASE_TYPE),QA)
TAG		= $(QA_RELEASE_BRANCH)
endif
ifeq ($(RELEASE_TYPE),MANU)
TAG		= $(PRODUCTION_BRANCH)
endif


################################################################################################


#directory of all Matrix Briefcase 
MATRIX_SWD_BRIEFCASE=$(SVN_HOME_DIR)/Briefcase/SWD_Briefcase

#directory of all Matrix Generic modules is present
MATRIX_GENERIC_SOFTWARE_MODULE=$(MATRIX_SWD_BRIEFCASE)/GenericSoftwareModules

MATRIX_EMBEDDED_SOFTWARE_MODULE=$(SVN_SERVER)/Briefcase/SWD_Briefcase/EmbeddedSoftwareModules

#directory of all Matrix Briefcase 
MATRIX_SECURITY_DOMAIN_SOFTWARE_MODULE=$(MATRIX_SWD_BRIEFCASE)/Security

# directory of generic platfrom source
GENERIC_PLATFORM_PATH=$(MATRIX_GENERIC_SOFTWARE_MODULE)/Platform/SWD/Branches/Releases/NVR_RK3568/8.5.0

# platform source directory structure
PLATFORM_SOURCE_PATH=$(GENERIC_PLATFORM_PATH)/Source

# platform cpu directory structure
PLATFORM_SUPPORTED_CPU_PATH=$(PLATFORM_SOURCE_PATH)/cpu/RK3568

#Directory to store specific changes
PLATFORM_SUPPORTED_BOARD_PATH=$(PLATFORM_SOURCE_PATH)/board

#Directory to store packages rootfs source
PLATFORM_PACKAGE_PATH=$(PLATFORM_SOURCE_PATH)/package

##################################  SDK   ########################################################## 

#SOC_VENDOR name
SOC_VENDOR_NAME=Rockchip

#SOC name
SOC_NAME=RK3568

#version information 
SDK_VERSION=rk356x_linux_release_v1.3.0_20220122

#NOTE: buildroot package is used from the rk356x_nvr_linux_full.tar.gz. reason to avoid the package vesion conflit 
#sdk package source name 
SDK_PACKAGE_NAME=rk356x_nvr_lite_1.3.0_20220122.tar.gz

#rockchip SVN SDK path
SVN_RK3568_SDK_PATH=$(SVN_SERVER)/Briefcase/SWD_Briefcase/Security/RDK/$(SOC_VENDOR_NAME)/$(SOC_NAME)

# directory rockchip package of all sources
SDK_SOURCE_PATH=$(MATRIX_SECURITY_DOMAIN_SOFTWARE_MODULE)/RDK/$(SOC_VENDOR_NAME)/$(SOC_NAME)/$(SDK_VERSION)
LOCAL_SDK_PATH=$(MATRIX_SECURITY_DOMAIN_SOFTWARE_MODULE)/RDK/$(SOC_VENDOR_NAME)/$(SOC_NAME)/$(SDK_VERSION)/rk356x_nvr_lite_1.3.0_20220122

SDK_RKMPI_RELEASE_PACKAGE=RKMPI_Release_V1.5.1_220514
SDK_RKMPI_RELEASE_PACKAGE_NAME=$(SDK_RKMPI_RELEASE_PACKAGE).tar.gz

SDK_RKMPI_RELEASE_PATCH_PATH=$(SDK_SOURCE_PATH)/patch/$(SDK_RKMPI_RELEASE_PACKAGE)

SDK_INSTALLATION_MAKEFILE_PATH=$(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_NAME)/install_sdk
SDK_MATRIX_PATCH_PATH=$(SDK_INSTALLATION_MAKEFILE_PATH)/patch/$(SDK_VERSION)/rk356x_nvr_lite_1.3.0_20220122

################################## Build Part  ########################################################## 
#platfrom run time build directory
PLATFORM_BUILD_PATH=$(PLATFORM_SOURCE_PATH)/build/$(BOARD_NAME)

#Platfom output directory
PLATFROM_BUILD_OUTPUT_PATH=$(PLATFORM_BUILD_PATH)/output

##################################  packages  ########################################################## 

# directory where packages to be build
LOCAL_PACKAGES_BUILD_PATH=$(PLATFORM_BUILD_PATH)/package

##################################  rootfs  ########################################################## 

ROOTFS_IMAGE_NAME=$(BOARD_NAME)$(BOARD_VARIANT)_rootfs_$(PLATFORM_SW_VER).ext4

BUILDROOT_ROOTFS_NAME=rootfs.ext2

#version information 
BUILDROOT_VERSION=buildroot-2018.02-rc3
BUSYBOX_VERSION=busybox-1.27.2

#package source
BUILDROOT_PACKAGE=$(BUILDROOT_VERSION).tar.bz2

#buildroot and buildroot download packages source
#BUILDROOT_PACKAGE_SOURCE=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/buildroot/$(BUILDROOT_VERSION)/$(BUILDROOT_PACKAGE)
BUILDROOT_DL_SOURCE=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/buildroot/$(BUILDROOT_VERSION)/dl

BUILDROOT_PACKAGE_SOURCE=$(SDK_SOURCE_PATH)/rk356x_nvr_linux_full/buildroot

BUILDROOT_PACKAGE_SOURDCE_DEPENDED_PACKAGE_LIST =$(LOCAL_SDK_PATH)/rkbin

#Directory to where rootfs to be build
PLATFORM_ROOTFS_BUILD_PATH=$(LOCAL_PACKAGES_BUILD_PATH)

#Directory to store platform rootfs source
PLATFORM_BUILDROOT_PATH=$(PLATFORM_PACKAGE_PATH)/buildroot

BUILDROOT_INCLUDE_PATH=$(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/output/host/arm-buildroot-linux-gnueabihf/sysroot/
BUILDROOT_LIB_PATH=$(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/output/target

#Directory to where buildroot output to be generated
BUILDROOT_OUTPUT_PATH=$(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/output

#Directory to where buildroot target to be generated
ROOTFS_OUTPUT_TARGET_PATH=$(BUILDROOT_OUTPUT_PATH)/target

MAIN_FILESYSTEM_PATH=$(ROOTFS_OUTPUT_TARGET_PATH)

# LOCAL_PLATFORM_UPGRADE_DIR_PATH=$(PLATFROM_BUILD_OUTPUT_PATH)/platform_upgrade_zip
# PLATFORM_UPGRADE_FILE=platform_upgrade.zip
# FIRMWARE_ZIP_FILE=NVR1602X_32X_64X_firmware_V07R10.zip


##################################  BUILDROOT  ########################################################## 

#buildroot configurations files
BUILDROOT_CONFIG=$(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_NAME)/config/$(BUILDROOT_VERSION).config

#buildroot utility folder path
BUILDROOT_UTILITY=$(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_NAME)/package/buildroot/

#busybox configurations files
BUSYBOX_CONFIG=$(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_NAME)/config/$(BUSYBOX_VERSION).config

#busybox source directory
BUSYBOX_SOURCE_PATH=$(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/package/busybox

#Directory to store platform rootfs patch
PLATFORM_ROOTFS_PATCH_PATH=$(PLATFORM_PACKAGE_PATH)/buildroot/rootfs

#Board specific rootfs patch
BOARD_ROOTFS_PATCH_PATH=$(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_NAME)/package/rootfs

SDK_PROVIDED_PACKAGE_PATH=$(LOCAL_SDK_PATH)/

PLATFORM_SW_VER_FILE_NAME=platform_sw_ver.txt

PLATFORM_SW_VER_FILE_PATH=$(ROOTFS_OUTPUT_TARGET_PATH)/etc

KERNEL_UPGRADE_VER_FILE_NAME=kernel_ver

###########################  Application firmware path ##############################################

#TODO update path and version revision
#FIRMWARE_RELEASE_DIR_PATH ?=$(SVN_SERVER)/Products/SATATYA_DEVICES/SDT/Software_Releases/VideoRecorder/Production/V07R10/HI3536_NVRH/web/
#APPLICATION_VER_REV ?=V07R10
#PPLICATION_FIRMWARE_NAME ?=NVR1602X_32X_64X_firmware_$(APPLICATION_VER_REV)
#APPLICATION_FIRMWARE_ZIP ?=$(APPLICATION_FIRMWARE_NAME).zip

##################################  uboot  ########################################################## 

UBOOT_SPL_IMAGE_NAME=$(BOARD_NAME)_spl_$(PLATFORM_SW_VER).bin

UBOOT_IMAGE_NAME=$(BOARD_NAME)_uboot_$(PLATFORM_SW_VER).img

UBOOT_CONFIG=$(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_NAME)/config/u-boot.config

#package source
UBOOT_VERSION=u-boot

# uboot packages source
UBOOT_PACKAGE_PATH=$(SDK_PROVIDED_PACKAGE_PATH)

# directory where uboot to be build
LOCAL_UBOOT_BUILD_PATH=$(PLATFORM_BUILD_PATH)/package

#Directory to store platform uboot source
PLATFORM_UBOOT_SOURCE_PATH=$(PLATFORM_SUPPORTED_CPU_PATH)/uboot

UBOOT_SPL_IMAGE_BUILD_NAME=rk356x_spl_loader_v1.13.111.bin

UBOOT_IMAGE_BUILD_NAME=uboot.img

IDB_LOADER_NAME=idbloader.img

MINILOADER_IMAGE_NAME=$(BOARD_NAME)_spl_loader_$(PLATFORM_SW_VER).bin
IDBLOADER_IMAGE_NAME=$(BOARD_NAME)_idb_loader_$(PLATFORM_SW_VER).bin

RK3568_DDR_BIN_NAME=rk3568_ddr_1560MHz_v1.13.bin
RK3568_SPL_BIN_NAME=rk356x_spl_v1.11.bin
##################################  KERNEL  ########################################################## 

#version information 
KERNEL_VERSION=kernel

#kernel img name 
KERNEL_IMAGE_NAME=$(BOARD_NAME)_uImage_$(PLATFORM_SW_VER)

#package source
KERNEL_PACKAGE=kernel

#Kernel Package download path
KERNEL_PACKAGE_PATH=$(SDK_PROVIDED_PACKAGE_PATH)/$(KERNEL_PACKAGE)

#Directory to store platform kernel source
PLATFORM_KERNEL_SOURCE_PATH=$(PLATFORM_SUPPORTED_CPU_PATH)/$(KERNEL_PACKAGE)

# directory where kernel to be build
LOCAL_KERNEL_BUILD_PATH=$(PLATFORM_BUILD_PATH)/package

#Kernel config file soource
KERNEL_CONFIG=$(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_NAME)/config/kernel.config

KERNEL_ROOT=$(LOCAL_KERNEL_BUILD_PATH)/kernel

KERNEL_IMAGE_BUILD_NAME=


KERNEL_BUILD_OUTPUT_PATH=$(PLATFROM_BUILD_OUTPUT_PATH)/kernel/
KERNEL_MODULE_BUILD_OUTPUT_PATH=$(KERNEL_BUILD_OUTPUT_PATH)/modules


##################################  PACKAGE   ########################################################## 
#iscsi package
PLATFORM_OPEN-ISCSI_PACKAGE_PATH = $(PLATFORM_PACKAGE_PATH)/open-iscsi
#isns packag3
PLATFORM_OPEN-ISNS_PACKAGE_PATH = $(PLATFORM_PACKAGE_PATH)/open-isns

OPEN_ISNS_PACKAGE_NAME=open-isns-0.101

LOCAL_OPEN_ISNS_PACKAGE_PATH=$(LOCAL_PACKAGES_BUILD_PATH)/$(OPEN_ISNS_PACKAGE_NAME)

################################## Drivers  ########################################################## 

PLATFORM_DRIVER_SOURCE_PATH=$(PLATFORM_SUPPORTED_CPU_PATH)/drivers

# directory where GPIO Driver source is present 
PLATFORM_DRIVER_GPIO_PATH=$(PLATFORM_DRIVER_SOURCE_PATH)/MxGpio

# directory where WATCHDOG Driver source is present 
PLATFORM_DRIVER_WATCHDOG_PATH=$(PLATFORM_DRIVER_SOURCE_PATH)/watchdog

SUPPORTED_BUILD_LIST=install-sdk uboot kernel packages drivers rootfs

SUPPORTED_DRIVER_LIST= GPIO WATCHDOG

#NOTE: Name in the list should not match with any directory/file name. If Name match then particular target will not work.
SUPPORTED_PACKAGE_LIST= BUILDROOT OPEN-ISCSI

################################## Copy Extra library from tool chain ##################################################
TOOLCHAIN_INSTALL_PATH=/opt/rk3568_nvr/host/

EXTRA_TOOL_CHAIN_LIBRARY_PATH=$(TOOL_CHAIN_PATH)/aarch64-buildroot-linux-gnu/sysroot/usr/lib
EXTRA_TOOL_CHAIN_LIBRARY_NAME= libgbm.so* librockchip_vpu.so* libOpenCL.so* libkms.so* libGLESv2.so* libGLESv1_CM.so* libEGL.so* libdrm_rockchip.so*  librkaiq.so

################################## Copy Extra library from application dependent ##################################################
EXTRA_APPL_DEP_LIBRARY_PATH=$(SDK_RKMPI_RELEASE_PATCH_PATH)/sdk
EXTRA_APPL_DEP_LIBRARY_NAME=librockit.so libgraphic_lsf.so* libswresample.so* libavutil.so* libavformat.so* libavcodec.so* librockchip_mpp.so* libmali.so* librga.so* libdrm.so*

EXTRA_APPL_BINRARY_PATH=$(SDK_RKMPI_RELEASE_PATCH_PATH)/bin
EXTRA_APPL_BINRARY_NAME=dumpsys

################################## Install Path ##################################################
#define Install to MATRIX directory

TARGET_MATRIX_INSTALL_DIR=$(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/output/target/matrix
DRIVER_INSTALL_DIR=$(TARGET_MATRIX_INSTALL_DIR)/ko/extdrv
LIB_INSTALL_PATH=$(TARGET_MATRIX_INSTALL_DIR)/lib

################################## tools folder path ##################################################

TOOLS_FOLDER_PATH=$(PLATFORM_SOURCE_PATH)/tools
TOOLS_UTILITY_FOLDER_PATH=$(TOOLS_FOLDER_PATH)/utility
USBUPGRADE_OUTPUT_PATH=$(PLATFROM_BUILD_OUTPUT_PATH)/usbupgrade

################################## USB upgrade Path##################################################
TOOLS_PATH=$(PLATFORM_SOURCE_PATH)/tools
SDK_UPDATE_IMAGES_PATH=$(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_NAME)/package/production_release/sdk_update/image
PRODUCTION_ZIP_NAME=NVR_8X_16X_P2_kernel_fs_$(APP_VERSION).zip

################################## SDK Tool path and GPT creation path ##################################################
PARTITION_TXT_FILE_PATH=$(TOOLS_PATH)/$(BOARD_NAME)/partition_table/matrix_parameter-buildroot-fit.txt
SDK_UPGRADE_TOOL_PATH=$(LOCAL_SDK_PATH)/tools/linux/Linux_Upgrade_Tool/Linux_Upgrade_Tool

################################## Common Work  ########################################################## 

PLATFORM_BUILD_DEVICE_PATH=$(LOCAL_PACKAGES_BUILD_PATH)/device/rockchip

export RK_TARGET_PRODUCT=rk356x
# kernel image format type: fit(flattened image tree)
export RK_KERNEL_FIT_ITS=boot.its
# Kernel dts
export RK_KERNEL_DTS=rk3568-nvr-demo-v10-linux
# boot image type
export RK_BOOT_IMG=boot.img

################################## copy command  ##########################################################
COPY=cp -rf 

define msg
	@echo -e "\033[91m"
	@echo "========================================================================================================================================"
	@echo "   						 $(1)"
	@echo "========================================================================================================================================"
	@echo -e "\033[0m"
endef

FIRMWARE_APP_RELEASE_PATH=$(SVN_SERVER)


