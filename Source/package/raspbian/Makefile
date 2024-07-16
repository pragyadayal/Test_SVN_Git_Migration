
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

all: rootfs

rootfs: source extract mount patch config build unmount


source:
	
	$(call msg, Start Exporting Packages Source )
	
	$(shell if [ ! -d "$(PLATFORM_ROOTFS_BUILD_PATH)" ]; then mkdir -p $(PLATFORM_ROOTFS_BUILD_PATH); fi ;)
	
	@if [ ! -f $(PLATFORM_ROOTFS_BUILD_PATH)/.source ]; then \
		svn $(SVN_CREDENTIALS) export --force $(OS_IMAGE_SOURCE_PATH) $(PLATFORM_ROOTFS_BUILD_PATH); \
		touch $(PLATFORM_ROOTFS_BUILD_PATH)/.source; \
	fi
	
	$(call msg, Export complete )

extract:

	$(call msg,  Extracting package )
	
	@if [ ! -f $(PLATFORM_ROOTFS_BUILD_PATH)/.extract ]; then \
		cd $(PLATFORM_ROOTFS_BUILD_PATH) && unzip $(OS_IMAGE_FILE); \
		touch $(PLATFORM_ROOTFS_BUILD_PATH)/.extract; \
	fi
	
	$(call msg,  Extract complete )

mount:
	$(call msg,   Rasbian Os Image mounting is in progress...)
	
	@if [ -f $(LOCAL_OS_IMAGE_MOUNT_PATH)/.mount ]; then \
		$(PLATFORM_TOOL_SCRIPT_PATH)/$(UNMOUNT_SCRIPT) $(LOCAL_OS_IMAGE_BOOT_PART_PATH); \
		$(PLATFORM_TOOL_SCRIPT_PATH)/$(UNMOUNT_SCRIPT) $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH); \
		rm -rf $(LOCAL_OS_IMAGE_MOUNT_PATH)/.mount; \
	fi
	
	@rm -rf $(LOCAL_OS_IMAGE_BOOT_PART_PATH)
	@rm -rf $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH)
	
	@mkdir -p $(LOCAL_OS_IMAGE_BOOT_PART_PATH)
	@mkdir -p $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH)
	
	#TODO return value validation is pending
	$(PLATFORM_TOOL_SCRIPT_PATH)/$(MOUNT_SCRIPT) $(PLATFORM_ROOTFS_BUILD_PATH)/$(OS_IMAGE_NAME) $(OS_IMAGE_BOOT_PART_NUM) $(LOCAL_OS_IMAGE_BOOT_PART_PATH)
	$(PLATFORM_TOOL_SCRIPT_PATH)/$(MOUNT_SCRIPT) $(PLATFORM_ROOTFS_BUILD_PATH)/$(OS_IMAGE_NAME) $(OS_IMAGE_ROOTFS_PART_NUM) $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH)
	@touch $(LOCAL_OS_IMAGE_MOUNT_PATH)/.mount; 

	$(call msg,   Rasbian OS Image is mounted)
	
unmount:
	@if [ -f $(LOCAL_OS_IMAGE_MOUNT_PATH)/.mount ]; then \
		$(PLATFORM_TOOL_SCRIPT_PATH)/$(UNMOUNT_SCRIPT) $(LOCAL_OS_IMAGE_BOOT_PART_PATH); \
		$(PLATFORM_TOOL_SCRIPT_PATH)/$(UNMOUNT_SCRIPT) $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH); \
		rm -rf $(LOCAL_OS_IMAGE_MOUNT_PATH)/.mount; \
	fi
	
	mkdir -p $(PLATFORM_BUILD_OUTPUT_PATH)
	mv $(PLATFORM_ROOTFS_BUILD_PATH)/$(OS_IMAGE_NAME) $(PLATFORM_BUILD_OUTPUT_PATH)/$(OS_IMAGE_RELEASE_NAME)

config:

	$(call msg,   Applying Configuration )

	$(call msg,    Configuration Applied )
	
patch:
	$(call msg,   Applying patch )

	sudo mkdir -p $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH)/opt/fr/data
	sudo mkdir -p $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH)/opt/fr/upgrade
	sudo mkdir -p $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH)/opt/fr/info
	
	sudo cp -rf $(PLATFORM_ROOTFS_PATH)/buster_os/boot/* $(LOCAL_OS_IMAGE_BOOT_PART_PATH)
	sudo cp -rf $(PLATFORM_ROOTFS_PATH)/buster_os/file_system/* $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH)
	sudo cp -rf $(PLATFORM_PACKAGE_PATH)/buildroot/rootfs/etc/pam.d $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH)/etc/
	
	sudo chmod -R 755 $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH)/etc/fr/*.sh

	#Copy application data
	mkdir -p $(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware
	rm -rf $(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/*
	
	svn $(SVN_CREDENTIALS) export --force $(FIRMWARE_RELEASE_DIR_PATH)/ $(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/	
	
	svn $(SVN_CREDENTIALS) export --force $(AES_CRYPT_SOURCE_PATH) $(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/tools/AESCrypt
	svn $(SVN_CREDENTIALS) export --force $(FIRMWARE_ENCRYPT_DECRYPT_PATH) $(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/tools/FirmwareEncryption
	cd $(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/tools/FirmwareEncryption && make all KEY_FLAG='-DKEY="pfr54mhws8a3dms235g3"' AESCRYPT_LIB_SOURCE_PATH=$(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/tools/AESCrypt
	$(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/tools/FirmwareEncryption/fwdecrypt $(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/COSEC_COGNIFACE_buster_V*R*.zip $(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/app_firmware_decrypted.zip
	
	unzip $(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/app_firmware_decrypted.zip -d $(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/temp 
	sudo cp -rf $(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/temp/* $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH)/opt/fr
	
	sudo cp -rf $(LOCAL_OS_IMAGE_MOUNT_PATH)/firmware/versionInfo.txt $(LOCAL_OS_IMAGE_ROOTFS_PART_PATH)/opt/fr/info/
	
	
	$(call msg,    patch Applied )
build: 
	$(call msg,   Start Building rootfs )

	$(call msg,   Rootfs Build Success )
	
	
##################################################################################################################
# End of makefile
##################################################################################################################

