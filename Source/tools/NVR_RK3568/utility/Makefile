 
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif


BOOT_SCRIT_SOURCE_FILE=$(BOARD_NAME)_boot.txt
USB_BOOT_SCRIPT_IMG_FILE=$(BOARD_NAME)_usbboot.scr
ETH_BOOT_SCRIPT_IMG_FILE=$(BOARD_NAME)_ethboot.scr
UPGRADE_SCRIT_SOURCE_FILE=$(BOARD_NAME)_upgrade.txt
USB_UPGRADE_SCRIPT_IMG_FILE=$(BOARD_NAME)_usbupgrade.scr
ETH_UPGRADE_SCRIPT_IMG_FILE=$(BOARD_NAME)_ethupgrade.scr

NVRX_LOGO_JPEG=nvrx_logo.bmp
BL_MSG_JPEG=BLUpdSuc.bmp
KNL_MSG_JPEG=KnlUpdSuc.bmp
FS_MSG_JPEG=FSUpdSuc.bmp
UPDFAIL_MSG_JPEG=UpgradeFailed.bmp


UBOOT_SPL_IMAGE=$(BOARD_NAME)_spl_$(PLATFORM_SW_VER).bin
UBOOT_IMAGE=$(BOARD_NAME)_uboot_$(PLATFORM_SW_VER).bin
KERNEL_IMAGE=$(BOARD_NAME)_uImage_V$(PRODUCT_VERSION)_R$(PRODUCT_REVISION)
ROOTFS_UBIFS_NAME=$(BOARD_NAME)_rootfs
#TODO validate below file is require or not
JMB572_IMAGE_NAME=JMB572_fw.bin

NVRX_NAND_PRODUCTION_IMAGE=$(BOARD_NAME)_NAND_$(APPLICATION_VER_REV_STR).bin
NAND_IMAGE_DIR=$(UTILITY_DIR_PATH)/nand_production_linux


#NVRX_PRODUCTION_ZIP=NVR0801X_1601X_kernel_fs_$(APPLICATION_VER_REV_STR).zip




all: ethbootscript usbbootscript generate_zip genprodbin



ethbootscript:
	@echo "nand erase 0x160000" > $(BOOT_SCRIT_SOURCE_FILE)
	@echo "nand erase 0x160000" > $(UPGRADE_SCRIT_SOURCE_FILE)

	@echo "echo ==== Programming Kernel Start =====" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "mw.b 0x42000000 0xff 0x500000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "tftp 0x42000000 "$(BOARD_NAME)/$(KERNEL_IMAGE)" || dispimg "$(BOARD_NAME)/$(UPDFAIL_MSG_JPEG)" 1" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand erase 0xC40000 0x500000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand write 0x42000000 0xC40000 0x500000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "dispimg "$(BOARD_NAME)/$(KNL_MSG_JPEG) | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "echo ###### Programming Kernel Complete ######" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null

	@echo "echo ==== Programming JMB576 FW Start =====" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "mw.b 0x42000000 0xff 0x20000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "tftp 0x42000000 "$(BOARD_NAME)/$(JMB572_IMAGE_NAME)" || dispimg "$(BOARD_NAME)/$(UPDFAIL_MSG_JPEG)" 1" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand erase 0x1240000 0x20000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand write 0x42000000 0x1240000 0x20000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "echo ###### Programming JMB576 FW Complete ######" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null

	@echo "echo ==== Programming Rootfs Start =====" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "mw.b 0x42000000 0xff 0xED00000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "tftp 0x42000000 "$(BOARD_NAME)/$(ROOTFS_UBIFS_NAME)" || dispimg "$(BOARD_NAME)/$(UPDFAIL_MSG_JPEG)" 1" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand erase 0x1280000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand write 0x42000000 0x1280000 "`ls -al $(BUILD_PATH)/$(BOARD_NAME)/$(ROOTFS_UBIFS_NAME) | awk {'printf("0x%x",$$5)'}`" || nand erase 0x1280000 "`ls -al $(BUILD_PATH)/$(BOARD_NAME)/$(ROOTFS_UBIFS_NAME) | awk {'printf("0x%x",$$5)'}` | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "dispimg "$(BOARD_NAME)/$(FS_MSG_JPEG) | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "echo ###### Programming Rootfs Complete ######" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null

	@echo "echo ==== Programming Logo start =====" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "mw.b 0x61000000 0xff 0x20000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "tftp 0x61000000 "$(BOARD_NAME)/$(NVRX_LOGO_JPEG)" || dispimg "$(BOARD_NAME)/$(UPDFAIL_MSG_JPEG)" 1" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand erase 0x100000 0x20000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand write 0x61000000 0x100000 0x20000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "echo ###### Programming Logo Complete ######" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	
	@echo "echo ==== Programming SPL start =====" >> $(BOOT_SCRIT_SOURCE_FILE)	
	@echo "mw.b 0x42000000 0xff 0x80000" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "fatload usb 0x0 0x42000000 "$(BOARD_NAME)/$(UBOOT_SPL_IMAGE)" || dispimg "$(BOARD_NAME)/$(UPDFAIL_MSG_JPEG)" 1" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "nand erase 0x0 0x80000" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "nand write 0x42000000 0x0 0x80000" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "dispimg "$(BOARD_NAME)/$(BL_MSG_JPEG) >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "echo ###### Programming SPL Complete ######" >> $(BOOT_SCRIT_SOURCE_FILE)

	@echo "echo ==== Programming Uboot start =====" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "mw.b 0x42000000 0xff 0x80000" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "tftp 0x42000000 "$(BOARD_NAME)/$(UBOOT_IMAGE)" || dispimg "$(BOARD_NAME)/$(UPDFAIL_MSG_JPEG)" 1" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "nand erase 0x0 0x80000" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "nand write 0x42000000 0x0 0x80000" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "dispimg "$(BOARD_NAME)/$(BL_MSG_JPEG) >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "echo ###### Programming Uboot Complete ######" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "delay 3000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE)
	@echo "reset" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE)

	mkimage -A arm -O U-Boot -C none -T script -d $(BOOT_SCRIT_SOURCE_FILE) $(ETH_BOOT_SCRIPT_IMG_FILE)
	mkimage -A arm -O U-Boot -C none -T script -d $(UPGRADE_SCRIT_SOURCE_FILE) $(ETH_UPGRADE_SCRIPT_IMG_FILE)

	rm -rf $(BOOT_SCRIT_SOURCE_FILE)
	rm -rf $(UPGRADE_SCRIT_SOURCE_FILE)
	mv $(ETH_BOOT_SCRIPT_IMG_FILE) $(IMAGE_DIR_PATH)/
	mv $(ETH_UPGRADE_SCRIPT_IMG_FILE) $(IMAGE_DIR_PATH)/
	#######################################################################
	# nvrx_ethupgrade.scr and nvrx_ethboot.scr generated in Build Dir!!
	#######################################################################

usbbootscript:
	@echo "nand erase 0x160000" > $(BOOT_SCRIT_SOURCE_FILE)
	@echo "nand erase 0x160000" > $(UPGRADE_SCRIT_SOURCE_FILE)
	
	@echo "echo ==== Programming Kernel Start =====" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "mw.b 0x42000000 0xff 0x500000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "fatload usb 0x0 0x42000000 "$(BOARD_NAME)/$(KERNEL_IMAGE)" || dispimg "$(BOARD_NAME)/$(UPDFAIL_MSG_JPEG)" 1" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand erase 0xC40000 0x500000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand write 0x42000000 0xC40000 0x500000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "dispimg "$(BOARD_NAME)/$(KNL_MSG_JPEG) | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "echo ###### Programming Kernel Complete ######" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null

	@echo "echo ==== Programming JMB576 FW Start =====" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "mw.b 0x42000000 0xff 0x20000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "fatload usb 0x0 0x42000000 "$(BOARD_NAME)/$(JMB572_IMAGE_NAME)" || dispimg "$(BOARD_NAME)/$(UPDFAIL_MSG_JPEG)" 1" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand erase 0x1240000 0x20000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand write 0x42000000 0x1240000 0x20000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "echo ###### Programming JMB576 FW Complete ######" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null

	@echo "echo ==== Programming Rootfs Start =====" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "mw.b 0x42000000 0xff 0xED00000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "fatload usb 0x0 0x42000000 "$(BOARD_NAME)/$(ROOTFS_UBIFS_NAME)" || dispimg "$(BOARD_NAME)/$(UPDFAIL_MSG_JPEG)" 1" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand erase 0x1280000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand write 0x42000000 0x1280000 "`ls -al $(BUILD_PATH)/$(BOARD_NAME)/$(ROOTFS_UBIFS_NAME) | awk {'printf("0x%x",$$5)'}`" || nand erase 0x1280000 "`ls -al $(BUILD_PATH)/$(BOARD_NAME)/$(ROOTFS_UBIFS_NAME) | awk {'printf("0x%x",$$5)'}` | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "dispimg "$(BOARD_NAME)/$(FS_MSG_JPEG) | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "echo ###### Programming Rootfs Complete ######" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null

	@echo "echo ==== Programming Logo start =====" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "mw.b 0x61000000 0xff 0x20000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "fatload usb 0x0 0x61000000 "$(BOARD_NAME)/$(NVRX_LOGO_JPEG)" || dispimg "$(BOARD_NAME)/$(UPDFAIL_MSG_JPEG)" 1" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand erase 0x100000 0x20000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "nand write 0x61000000 0x100000 0x20000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "echo ###### Programming Logo Complete ######" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null

	@echo "echo ==== Programming SPL start =====" >> $(BOOT_SCRIT_SOURCE_FILE)	
	@echo "mw.b 0x42000000 0xff 0x80000" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "fatload usb 0x0 0x42000000 "$(BOARD_NAME)/$(UBOOT_SPL_IMAGE)" || dispimg "$(BOARD_NAME)/$(UPDFAIL_MSG_JPEG)" 1" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "nand erase 0x0 0x80000" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "nand write 0x42000000 0x0 0x80000" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "dispimg "$(BOARD_NAME)/$(BL_MSG_JPEG) >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "echo ###### Programming SPL Complete ######" >> $(BOOT_SCRIT_SOURCE_FILE)
	
	
	@echo "echo ==== Programming Uboot start =====" >> $(BOOT_SCRIT_SOURCE_FILE)	
	@echo "mw.b 0x42000000 0xff 0x80000" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "fatload usb 0x0 0x42000000 "$(BOARD_NAME)/$(UBOOT_IMAGE)" || dispimg "$(BOARD_NAME)/$(UPDFAIL_MSG_JPEG)" 1" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "nand erase 0x0 0x80000" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "nand write 0x42000000 0x0 0x80000" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "dispimg "$(BOARD_NAME)/$(BL_MSG_JPEG) >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "echo ###### Programming Uboot Complete ######" >> $(BOOT_SCRIT_SOURCE_FILE)
	@echo "delay 3000" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "saveenv" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null
	@echo "reset" | tee -a $(BOOT_SCRIT_SOURCE_FILE) $(UPGRADE_SCRIT_SOURCE_FILE) > /dev/null

	mkimage -A arm -O U-Boot -C none -T script -d $(BOOT_SCRIT_SOURCE_FILE) $(USB_BOOT_SCRIPT_IMG_FILE)
	mkimage -A arm -O U-Boot -C none -T script -d $(UPGRADE_SCRIT_SOURCE_FILE) $(USB_UPGRADE_SCRIPT_IMG_FILE)

	rm -rf $(BOOT_SCRIT_SOURCE_FILE)
	rm -rf $(UPGRADE_SCRIT_SOURCE_FILE)
	mv $(USB_UPGRADE_SCRIPT_IMG_FILE) $(IMAGE_DIR_PATH)/
	mv $(USB_BOOT_SCRIPT_IMG_FILE) $(IMAGE_DIR_PATH)/
	#######################################################################
	# nvrx_usbupgrade.scr and nvrx_usbboot.scr generated in Build Dir!!
	#######################################################################
	
genprodbin:


	@echo " ***** Production image generation started *****"

	@echo " *****Image generated successfully ***** "

generate_zip:
	$(call CHECKFILE,$(UBOOT_SPL_IMAGE),524288)#TODO change size
	$(call CHECKFILE,$(UBOOT_IMAGE),524288)	#512K
	$(call CHECKFILE,$(KERNEL_IMAGE),5242880)	#5M
	$(call CHECKFILE,$(ROOTFS_UBIFS_NAME),246415360)	#235M
	cp $(TOOLS_UTILITY_FOLDER_PATH)/BootScreenLogo/* $(IMAGE_DIR_PATH)/
	#cp $(BUILD_PATH)/$(BOARD_NAME)/$(USB_UPGRADE_SCRIPT_IMG_FILE) $(BUILD_PATH)/HI3536_NVR
	#cp $(PRODUCT_SOURCE_PATH)/Package/JMB576_Firmware/* $(IMAGE_DIR_PATH)/ #TODO validate this file is require or not

	mkdir -p $(USBUPGRADE_OUTPUT_PATH)
	mkdir -p $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_NAME)
	
	echo -n `date` > $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_NAME)/NVR_signature.txt
	
	cp -rf $(PLATFROM_BUILD_OUTPUT_PATH)/kernel/ $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_NAME)
	cp -rf $(PLATFROM_BUILD_OUTPUT_PATH)/uboot/ $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_NAME)
	
	cd $(USBUPGRADE_OUTPUT_PATH)/ && zip -ruy $(BOARD_NAME)/$(NVRX_PRODUCTION_ZIP) $(BOARD_NAME)/
	cp $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_NAME)/$(NVRX_PRODUCTION_ZIP) $(USBUPGRADE_OUTPUT_PATH)/
	rm -rf $(USBUPGRADE_OUTPUT_PATH)/$(BOARD_NAME)
	
#================================================================================================================================================================	
	
# This is used to check whether bootscripting binary is valid or not
# $(call CHECKFILE,<File Name>,<Max Limit Of File Size to be Tolerable>)
CHECKFILE = \
    @if [ ! -f "$(IMAGE_DIR_PATH)/$(1)" ]; then \
        echo -e "File Doesnt Exist -> $(RED_BOLD) $(IMAGE_DIR_PATH)/$(1) $(COLOR)" ; \
        exit 1 ; \
    fi; \
    FSIZE=$$(du -b "$(IMAGE_DIR_PATH)/$(1)" | cut -f 1) ; \
    if [ $$FSIZE -le "$(2)" ]; then \
	echo -e "File $(IMAGE_DIR_PATH)/$(1) =$(GREEN_BOLD) $$FSIZE Bytes $(COLOR)" ; \
    else \
	echo -e "Binary $(IMAGE_DIR_PATH)/$(1) =$(RED_BOLD) $$FSIZE Bytes $(COLOR) exceeds BAD BLOCK margin!!!!" ; \
	exit 1; \
    fi ;
