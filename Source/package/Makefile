 
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif


define msg
	@echo -e "\033[91m"
	@echo "========================================================================================================================================"
	@echo "   						 $(1)"
	@echo "========================================================================================================================================"
	@echo -e "\033[0m"
endef


start_msg:
	$(call msg,   Start package Building  )
	
end_msg:
	$(call msg,   End package Building  )	


all: start_msg $(SUPPORTED_PACKAGE_LIST) end_msg


BUILDROOT:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_BUILDROOT_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) ROOTFS_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )

BLE:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_BLE_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) ROOTFS_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )
	
CATDOC:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_CATDOC_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) 
	$(call msg, compiling $@ completed)
	
ZLIB:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_ZLIB_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )
OPENSSL:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_OPENSSL_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )

CURL:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_CURL_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )

PAM:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_PAM_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )
	
OTPW:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_OTPW_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) 
	$(call msg, compiling $@ completed )
	
PASSWORD_UPDATE:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_PASWORDUPDATE_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) 
	$(call msg, compiling $@ completed )
	
OPENSSH:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_OPENSSH_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )

CSV2XLS:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_CSV2XLS_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )

CRASH_HANDLER:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_CRASHHANDLER_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )

SNMP:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_SNMP_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )

IPWATCHD:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_IPWATCHD_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )
	
UNZIP60:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_UNZIP60_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )

ZIP:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_ZIP_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )
	
LIBICON: 
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_LIBICON_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )
	
SAMBA: LIBICON
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_SAMBA_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )

FDISK:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_FDISK_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )
	
REG_TOOL:
	$(call msg, compiling $@ Source )
	make -C $(SDK_REG_TOOL_PATH) CROSS=$(HOST_ARM)
	cp -rf $(SDK_REG_TOOL_PATH)/bin $(ROOTFS_OUTPUT_TARGET_PATH)
	$(call msg, compiling $@ completed )

	
RGB_LCD:
	$(call msg, compiling $@ Source )
	rm -rf $(SDK_MPP_SOURCE_PATH)/sample/rgb_lcd
	cp -rf $(PLATFORM_RGB_LCD_PACKAGE_PATH) $(SDK_MPP_SOURCE_PATH)/sample/
	cd $(SDK_MPP_SOURCE_PATH)/sample/rgb_lcd && make clean && make all
	cp $(SDK_MPP_SOURCE_PATH)/sample/rgb_lcd/lcd_configure $(ROOTFS_OUTPUT_TARGET_PATH)/usr/bin/
	$(call msg, compiling $@ completed )
	
RASPBIAN:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_RASPBIAN_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )
INADYN:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_INADYN_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )

LIBUPNP:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_LIBUPNP_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )
	
BLUEZ:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_BLUEZ_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )	

OPEN-ISCSI:
	$(call msg, compiling $@ Source )
	make -C $(PLATFORM_OPEN-ISCSI_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) PACKAGE_BOARD_TYPE=$(BOARD_TYPE)
	$(call msg, compiling $@ completed )		

#------------------------------------------------------------------------------------------------------------------------------------
