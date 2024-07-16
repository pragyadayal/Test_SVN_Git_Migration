#!/bin/bash

ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

################################ Production and firmware release path and variable ##################################### 
#define Production bin generated path
TARGET_DIR_NAME=$(PLATFORM_SW_VER)/target/$(BOARD_NAME)$(BOARD_VARIANT)

HEX_FILE_NAME=RK3568_NVRL_eMMC_$(PLATFORM_SW_VER).bin

PRODUCTION_RELEASE_PATH=$(MATRIX_GENERIC_SOFTWARE_MODULE)/Platform/Software_Releases/$(BOARD_NAME)
MINILOADER_IMAGE_PATH=$(PLATFROM_BUILD_OUTPUT_PATH)/$(UBOOT_SPL_IMAGE_NAME)
IDBLOADER_IMAGE_PATH=$(PLATFROM_BUILD_OUTPUT_PATH)/$(IDBLOADER_IMAGE_NAME)
U_BOOT_IMAGE_PATH=$(PLATFROM_BUILD_OUTPUT_PATH)/$(UBOOT_IMAGE_NAME)
KERNEL_IMAGE_PATH=$(PLATFROM_BUILD_OUTPUT_PATH)/kernel/$(KERNEL_IMAGE_NAME)
ROOTFS_IMAGE_PATH=$(PLATFROM_BUILD_OUTPUT_PATH)/$(ROOTFS_IMAGE_NAME)
PRODUCTION_HEX_PATH=$(PLATFROM_BUILD_OUTPUT_PATH)/$(HEX_FILE_NAME)
USBUPGRADE_OUTPUT_PATH=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/usbupgrade
GPT_PARAMETER_NAME=$(BOARD_NAME)_gpt_part.img
RESOURCE_IMG_NAME=$(BOARD_NAME)_Resource_img_$(PLATFORM_SW_VER).img

MKIMAGE_TOOL_PATH=$(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/tools

#GPT size is captured from IDB loader image size
GPT_PART_SIZE=`du -b $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(GPT_PARAMETER_NAME) | cut -f1`
#uboot size is captured from uboot image size
IDB_LOADER_SIZE=`du -b $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(IDBLOADER_IMAGE_NAME) | cut -f1`
#uboot size is captured from uboot image size
UBOOT_SIZE=`du -b $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(UBOOT_IMAGE_NAME) | cut -f1`
#Resource part size is captured from Resource image size
RESOURCE_SIZE=`du -b $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(RESOURCE_IMG_NAME) | cut -f1`
#Kernel size is captured from kernel image size
KERNEL_SIZE=`du -b $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(KERNEL_IMAGE_NAME) | cut -f1`
#Rootfs size is captured from rootfs image size
ROOTFS_SIZE=`du -b $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(ROOTFS_IMAGE_NAME) | cut -f1`

SIZE=$(shell expr $(GPT_PART_SIZE) + $(IDB_LOADER_SIZE) + $(UBOOT_SIZE) + $(RESOURCE_SIZE) + $(KERNEL_SIZE) + $(ROOTFS_SIZE))

USB_SCRIT_SOURCE_FILE=$(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)/$(BOARD_TYPE)_usbupgrade.txt
USB_SCRIPT_IMG_FILE=$(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)/$(BOARD_TYPE)_usbupgrade.img
##############################################################################################################

all: matrix_fs copy_content resourceImg usbbootscript generate_zip genProdBin

matrix_fs:extra_lib matrix_patch build_fs install platform_upgrade_zip

matrix_patch:

	#creating kernel version file with current version
	echo $(KERNEL_UPGRADE_VER) > $(PLATFORM_SW_VER_FILE_PATH)/$(KERNEL_UPGRADE_VER_FILE_NAME)
	
	# Generate platform sw version file
	echo $(PLATFORM_SW_VER) > $(PLATFORM_SW_VER_FILE_PATH)/$(PLATFORM_SW_VER_FILE_NAME)
	
	#Copy application data
	rm -rf $(PLATFORM_BUILD_PATH)/app_firmware/*
	mkdir -p $(PLATFORM_BUILD_PATH)/app_firmware/temp/
	
	@echo $(FIRMWARE_RELEASE_DIR_PATH)
	svn export --force $(FIRMWARE_RELEASE_DIR_PATH) $(PLATFORM_BUILD_PATH)/app_firmware/
	mv $(PLATFORM_BUILD_PATH)/app_firmware/*_firmware_*.zip $(PLATFORM_BUILD_PATH)/app_firmware/final_firmware.zip
	
#	-----------------------------------------------------------------------------------	
	# package increpted  then add logic of decryption
 	#-cd $(TOOLS_PATH)/$(BOARD_NAME) && \
 	#./fwdecrypt $(PLATFORM_BUILD_PATH)/app_firmware/final_firmware.zip $(PLATFORM_BUILD_PATH)/app_firmware/dec_final_firmware.zip
 	#if [ -e $(PLATFORM_BUILD_PATH)/app_firmware/dec_final_firmware.zip ]; then\
 	#	unzip $(PLATFORM_BUILD_PATH)/app_firmware/dec_final_firmware.zip -d $(PLATFORM_BUILD_PATH)/app_firmware/temp;\
 	#else \
 	#	unzip $(PLATFORM_BUILD_PATH)/app_firmware/final_firmware.zip -d $(PLATFORM_BUILD_PATH)/app_firmware/temp;\
 	#fi
	unzip $(PLATFORM_BUILD_PATH)/app_firmware/final_firmware.zip -d $(PLATFORM_BUILD_PATH)/app_firmware/temp/

#	----------------------------OR-----------------------------------------------------

	# Field package extraction (application + platform package)
#	make -C $(APPLICATION_DIR_PATH) production BUILDROOT_TARGET_PATH=$(BUILDROOT_TARGET_PATH) BOARD_TYPE=$(BOARD_TYPE)
	
	chmod -R 755 $(PLATFORM_BUILD_PATH)/app_firmware/
	cp -rf $(PLATFORM_BUILD_PATH)/app_firmware/temp/* $(ROOTFS_OUTPUT_TARGET_PATH)
	
	$(call msg, application and platform files merge successfully )
	
extra_lib:
	$(call msg,   Start copying library from toolchain )

	cd $(EXTRA_TOOL_CHAIN_LIBRARY_PATH)/ && cp -drf $(EXTRA_TOOL_CHAIN_LIBRARY_NAME) $(ROOTFS_OUTPUT_TARGET_PATH)/usr/lib/
	
	#copy from RKMPI_Release
	cd $(EXTRA_TOOL_CHAIN_LIBRARY_PATH)/ && cp -drf $(EXTRA_APPL_DEP_LIBRARY_NAME) $(ROOTFS_OUTPUT_TARGET_PATH)/usr/lib/
	
	cd $(EXTRA_APPL_BINRARY_PATH)/ && cp -drf $(EXTRA_APPL_BINRARY_NAME) $(ROOTFS_OUTPUT_TARGET_PATH)/bin/
	
	$(call msg,   libray copy done )

build_secureboot:
	
	@if [ "$(SECUREBOOT_EN)" = "ENABLE" ]; then \
		make -C $(PLATFORM_UBOOT_SOURCE_PATH) secureboot; \
	fi
	
build_fs:
	$(call msg,   Start Building Rootfs )
	
	$(MAKE) -C $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION) all
	
	$(call msg,   Rootfs Build Success )
	
install:
	cp -rf $(BUILDROOT_OUTPUT_PATH)/images/$(BUILDROOT_ROOTFS_NAME) $(PLATFROM_BUILD_OUTPUT_PATH)/$(ROOTFS_IMAGE_NAME);
	
platform_upgrade_zip:

	$(call msg, Generate platform_upgrade.zip)

	rm -rf $(LOCAL_PLATFORM_UPGRADE_DIR_PATH)/$(PLATFORM_UPGRADE_FILE)

	# create required directories
	mkdir -p $(LOCAL_PLATFORM_UPGRADE_DIR_PATH)
	mkdir -p $(LOCAL_PLATFORM_UPGRADE_DIR_PATH)/bin
	mkdir -p $(LOCAL_PLATFORM_UPGRADE_DIR_PATH)/etc
	mkdir -p $(LOCAL_PLATFORM_UPGRADE_DIR_PATH)/matrix
	mkdir -p $(LOCAL_PLATFORM_UPGRADE_DIR_PATH)/usr
	mkdir -p $(LOCAL_PLATFORM_UPGRADE_DIR_PATH)/sbin

	# copying files
	cp -rf $(ROOTFS_OUTPUT_TARGET_PATH)/matrix/kernel $(LOCAL_PLATFORM_UPGRADE_DIR_PATH)/matrix/kernel
	cp -rf $(ROOTFS_OUTPUT_TARGET_PATH)/matrix/ko $(LOCAL_PLATFORM_UPGRADE_DIR_PATH)/matrix/ko

	# deleting empty directories
	find $(LOCAL_PLATFORM_UPGRADE_DIR_PATH) -type d -empty -delete
	
	cd $(LOCAL_PLATFORM_UPGRADE_DIR_PATH)/ && zip -r -y -q -9 $(PLATFORM_UPGRADE_FILE) bin etc matrix usr sbin
	mv $(LOCAL_PLATFORM_UPGRADE_DIR_PATH)/$(PLATFORM_UPGRADE_FILE) $(PLATFROM_BUILD_OUTPUT_PATH)
	rm -rf $(LOCAL_PLATFORM_UPGRADE_DIR_PATH)
	
	$(call msg,platform_upgrade_zip ZIP is generated)

genProdBin:

	$(call msg, Generate hex for production)
	rm -rf $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(HEX_FILE_NAME)
	
	#generate GPT partition image from parameter file
	$(SDK_UPGRADE_TOOL_PATH)/upgrade_tool GPT $(PARTITION_TXT_FILE_PATH) $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(GPT_PARAMETER_NAME)
	
	 
	objcopy --gap-fill=0xff --pad-to=$(SIZE)  -I binary -O binary $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(IDBLOADER_IMAGE_NAME) $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(HEX_FILE_NAME)
	
	
	# Add gpt partition to flash image
	@echo "==============Adding Parameter file to flash image============="
	@dd if=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(GPT_PARAMETER_NAME) of=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(HEX_FILE_NAME) seek=0 conv=sync
	
	
	# Add idbloader to flash image
	@echo "==============Adding idbloader to flash image============="
	@dd if=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(IDBLOADER_IMAGE_NAME) of=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(HEX_FILE_NAME) seek=64 conv=sync
	
	
	# Add u-boot to flash image
	@echo "==============Adding U-boot to flash image============="
	@dd if=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(UBOOT_IMAGE_NAME) of=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(HEX_FILE_NAME) seek=16384 conv=sync
	
	# Add resource part to flash image
	@echo "==============Adding resource part to flash image=============="
	@dd if=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(RESOURCE_IMG_NAME) of=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(HEX_FILE_NAME) seek=26624 conv=sync
		
	# Add Kernel  to flash image
	@echo "==============Adding Kernel to flash image=============="
	@dd if=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(KERNEL_IMAGE_NAME) of=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(HEX_FILE_NAME) seek=38912 conv=sync
	
	# Add rootfs to flash image
	@echo "==============Adding FileSystem to flash image============"
	@dd if=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(ROOTFS_IMAGE_NAME) of=$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(HEX_FILE_NAME) seek=104448 conv=sync
	
	
	@echo  "board variant $(BOARD_VARIANT)"
	
	$(call msg, Generate hex for $(BOARD_VARIANT) production done)
	
	$(call msg, Please commit Production hex bin at PATH:\n    $(PRODUCTION_HEX_PATH))
	
copy_content:

	$(call msg,  Production Release start )
	
	rm -rf $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)
	mkdir -p $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)
	#copy to software release path for production release
	
	cp -rf $(IDBLOADER_IMAGE_PATH) 	$(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/
	cp -rf $(MINILOADER_IMAGE_PATH) $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/
	cp -rf $(U_BOOT_IMAGE_PATH) $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/
	cp -rf $(KERNEL_IMAGE_PATH) $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/
	cp -rf $(ROOTFS_IMAGE_PATH) $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/
	cp -rf $(BUILDROOT_OUTPUT_PATH)/images/rootfs.tar  $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/
	
	$(call msg,  Production release done )
	
	$(call msg, Please commit all binary for Production release at PATH:\n    $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME))

usbbootscript:

	rm -rf   $(USBUPGRADE_OUTPUT_PATH)
	mkdir -p $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)
	@echo "echo ==== Programming Uboot start =====" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "fatload usb 0:1 0x09400000 "$(BOARD_TYPE)/$(UBOOT_IMAGE_NAME) "|| rockchip_show_bmp B_U_F.bmp" "&& exit" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "mmc erase  0x00004000 0x1fff" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "mmc write  0x09400000 0x4000 0x1fff" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "rockchip_show_bmp B_U_S.bmp" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "echo ###### Programming Uboot Complete ######" >> $(USB_SCRIT_SOURCE_FILE)
	
	@echo "echo ==== Programming Resource Img Start =====" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "fatload usb 0:1 0x09400000 "$(BOARD_TYPE)/$(RESOURCE_IMG_NAME)  "|| rockchip_show_bmp  K_U_F.bmp" "&& exit" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "mmc erase 0x6800 0x2FFF" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "mmc write  0x09400000  0x6800 0x2FFF" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "echo ###### Programming Resourec Complete ######" >> $(USB_SCRIT_SOURCE_FILE)
	
	@echo "echo ==== Programming Kernel Start =====" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "fatload usb 0:1 0x09400000 "$(BOARD_TYPE)/$(KERNEL_IMAGE_NAME)  "|| rockchip_show_bmp  K_U_F.bmp" "&& exit " >> $(USB_SCRIT_SOURCE_FILE)
	@echo "mmc erase 0x9800 0xFFFF" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "mmc write  0x09400000  0x9800 0xFFFF" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "rockchip_show_bmp  K_U_S.bmp" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "echo ###### Programming Kernel Complete ######" >> $(USB_SCRIT_SOURCE_FILE)

	@echo "echo ==== Programming Rootfs Start =====" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "fatload usb 0:1 0x09400000 "$(BOARD_TYPE)/$(ROOTFS_IMAGE_NAME) "|| rockchip_show_bmp F_S_U_F.bmp" "&& exit" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "mmc erase  0x19800 0x1FFFFF" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "mmc write 0x09400000  0x00019800 0x1FFFFF" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "rockchip_show_bmp F_S_U_S.bmp" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "saveenv" >> $(USB_SCRIT_SOURCE_FILE)
	@echo "delay 5000" >> $(USB_SCRIT_SOURCE_FILE) #delay in MS
	@echo "reset" >> $(USB_SCRIT_SOURCE_FILE)
	
	#mkimage -T script -C none -n 'flashing' -d $(USB_SCRIT_SOURCE_FILE) $(USB_SCRIPT_IMG_FILE)
	$(MKIMAGE_TOOL_PATH)/mkimage -A arm -O U-Boot -C none -T script -d $(USB_SCRIT_SOURCE_FILE) $(USB_SCRIPT_IMG_FILE)
	
	$(call msg, USB SCRIPT generated successfully)
	
	rm -rf $(USB_SCRIT_SOURCE_FILE)
	#rm -rf $(USB_SCRIPT_IMG_FILE)
	
	$(call msg, $(BOARD_TYPE)_usbupgrade.img generated in target Dir!!)
	
generate_zip:
	
	$(call msg, Generating  USB Upgrade zip started)
	
	#cp $(SDK_UPDATE_IMAGES_PATH)/* $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)/
	
	echo -n `date` > $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)/$(BOARD_TYPE)_signature.txt
	
	#cp -rf $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)/$(BOARD_TYPE)_signature.txt $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)/$(BOARD_TYPE)_signature.txt
	#cp -rf $(USB_SCRIPT_IMG_FILE) $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)/$(BOARD_TYPE)_usbupgrade.img
	echo -n "$(BOARD_TYPE)/$(BOARD_TYPE)_signature.txt;$(BOARD_TYPE)/$(BOARD_TYPE)_usbupgrade.img;" > $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)/package
	
	cp -rf $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(UBOOT_IMAGE_NAME)  $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)/
	cp -rf $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(RESOURCE_IMG_NAME) $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)/
	cp -rf $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(KERNEL_IMAGE_NAME) $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)/
	cp -rf $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(ROOTFS_IMAGE_NAME) $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_TYPE)/
	
	cd $(USBUPGRADE_OUTPUT_PATH) && zip -ruy $(PRODUCTION_ZIP_NAME) $(BOARD_TYPE)
	mv $(USBUPGRADE_OUTPUT_PATH)/$(PRODUCTION_ZIP_NAME) $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/
	rm -rf $(USBUPGRADE_OUTPUT_PATH)
	
	$(call msg, Generating  USB Upgrade zip successfully)

resourceImg:
	
	$(call msg, Generating Resource image)
	
	#rm -rf $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/tools/images/*
	#cp $(SDK_UPDATE_IMAGES_PATH)/* $(MKIMAGE_TOOL_PATH)/images/
	
	#cd $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION) && ./scripts/pack_resource.sh $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)/resource.img
	
	cp -rf  $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)/resource.img  $(PRODUCTION_RELEASE_PATH)/$(TARGET_DIR_NAME)/$(RESOURCE_IMG_NAME)
	$(call msg, Generating Resource successfully)
	
