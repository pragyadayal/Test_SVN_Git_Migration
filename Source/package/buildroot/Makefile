
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

POST_ROOTFS_BUILD_SCRIPT_NAME=utilities/post_rootfs_build.sh
CUSTOM_ROOTFS_BUILD_SCRIPT_NAME=utilities/custom_rootfs_build.sh

all: source extract patch config build

matrixfs:prepare_rootfs build  

source:
	
	$(call msg, Start Exporting Buildroot and Packages Source )
	
	$(shell if [ ! -d "$(PLATFORM_ROOTFS_BUILD_PATH)" ]; then mkdir -p $(PLATFORM_ROOTFS_BUILD_PATH); fi ;)
	
	@if [ -f $(BUILDROOT_PACKAGE_MAKEFILE_PATH)/Makefile ]; then \
	make -C $(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_TYPE)/package/buildroot source; \
	else \
		if [ $(ROOTFS_BOARD_TYPE) == NVR_RK3568 ]; then \
			if [ ! -f $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/.stamp_source ]; then \
				mkdir -p $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION); \
				$(COPY) $(BUILDROOT_PACKAGE_SOURCE)/* $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/; \
				svn export --force $(BUILDROOT_DL_SOURCE) $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/dl; \
				touch $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/.stamp_source; \
			fi ;\
			for i in $(BUILDROOT_PACKAGE_SOURDCE_DEPENDED_PACKAGE_LIST); \
			do \
				cp -rf $$i $(PLATFORM_ROOTFS_BUILD_PATH) ; \
			done ;\
		else \
			if [ ! -f $(PLATFORM_ROOTFS_BUILD_PATH)/.tmp ]; then \
				svn export --force $(BUILDROOT_PACKAGE_SOURCE) $(PLATFORM_ROOTFS_BUILD_PATH); \
				svn export --force $(BUILDROOT_DL_SOURCE) $(PLATFORM_ROOTFS_BUILD_PATH)/dl; \
			fi ;\
		fi ;\
	fi
	
	$(call msg, Export complete )

extract:

	$(call msg,  Extracting package )
	
	@if [ -f $(BUILDROOT_PACKAGE_MAKEFILE_PATH)/Makefile ]; then \
		make -C $(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_TYPE)/package/buildroot extract; \
	else \
		if [ $(ROOTFS_BOARD_TYPE) != NVR_RK3568 ]; then \
			if [ ! -f $(PLATFORM_ROOTFS_BUILD_PATH)/.tmp ]; then \
			tar -xvf $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_PACKAGE) -C $(PLATFORM_ROOTFS_BUILD_PATH); \
			mv  $(PLATFORM_ROOTFS_BUILD_PATH)/dl $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/dl; \
			touch $(PLATFORM_ROOTFS_BUILD_PATH)/.tmp; \
			fi ;\
		fi ;\
	fi
	
	$(call msg,  Extract complete )
		
config:

	$(call msg,   Applying Configuration )

	cp $(BUILDROOT_CONFIG) $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/.config
	cp $(BUSYBOX_CONFIG)   $(BUSYBOX_SOURCE_PATH)/busybox.config 

	$(call msg,    Configuration Applied )
		
patch:

	$(call msg,   Applying patch )
	
	@if [ $(ROOTFS_BOARD_TYPE) == NETRA_IPC_8MP ]; then \
	rsync -a --exclude=".svn"  $(PLATFORM_ROOTFS_PATCH_PATH)/etc/  $(BUILROOT_ROOTFS_SKELETON)/etc/ ;\
	fi
	
	@if [ -f $(BUILDROOT_PACKAGE_MAKEFILE_PATH)/Makefile ]; then \
	make -C $(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_TYPE)/package/buildroot patch; \
	else \
		rsync -a --exclude=".svn"  $(PLATFORM_BUILDROOT_PATH)/$(BUILDROOT_VERSION)/  $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/; \
	fi
	
	@if [ $(ROOTFS_BOARD_TYPE) == ARGO_FR ]; then \
	chmod 777 $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "#!/bin/sh" > $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "#Auto Generate file Do not Edit" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "ROOTFS_OUTPUT_TARGET_PATH=$(ROOTFS_OUTPUT_TARGET_PATH)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "BOARD_ROOTFS_PATCH_PATH=$(BOARD_ROOTFS_PATCH_PATH)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "PLATFORM_ROOTFS_PATCH_PATH=$(PLATFORM_ROOTFS_PATCH_PATH)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "chmod 777 $(CUSTOM_ROOTFS_BUILD_SCRIPT_NAME)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "./$(CUSTOM_ROOTFS_BUILD_SCRIPT_NAME) $(ROOTFS_OUTPUT_TARGET_PATH) $(PLATFORM_ROOTFS_PATCH_PATH) $(BOARD_ROOTFS_PATCH_PATH) $(PRODUCT_VER_REV_PATCH)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	rsync -a --exclude=".svn"  $(BUILDROOT_UTILITY)/  $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/; \
	fi
	
	@if [ $(ROOTFS_BOARD_TYPE) == ESSENTIAL_IPC_8MP ]; then \
	echo "#!/bin/sh" > $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "#Auto Generate file Do not Edit" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "ROOTFS_OUTPUT_TARGET_PATH=$(ROOTFS_OUTPUT_TARGET_PATH)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "BOARD_ROOTFS_PATCH_PATH=$(BOARD_ROOTFS_PATCH_PATH)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "PLATFORM_ROOTFS_PATCH_PATH=$(PLATFORM_ROOTFS_PATCH_PATH)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "chmod 777 $(CUSTOM_ROOTFS_BUILD_SCRIPT_NAME)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	echo "./$(CUSTOM_ROOTFS_BUILD_SCRIPT_NAME) $(ROOTFS_OUTPUT_TARGET_PATH) $(PLATFORM_ROOTFS_PATCH_PATH) $(BOARD_ROOTFS_PATCH_PATH)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	chmod 777 $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME); \
	rsync -a --exclude=".svn"  $(BUILDROOT_UTILITY)/  $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/; \
	fi

	@if [ $(ROOTFS_BOARD_TYPE) == NVR_RK3568 ]; then \
		if [  -f $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/.stamp_source ]; then \
			echo "#!/bin/sh" > $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME) ;\
			echo "#Auto Generate file Do not Edit" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME) ;\
			echo "ROOTFS_OUTPUT_TARGET_PATH=$(ROOTFS_OUTPUT_TARGET_PATH)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME) ;\
			echo "BOARD_ROOTFS_PATCH_PATH=$(BOARD_ROOTFS_PATCH_PATH)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME) ;\
			echo "PLATFORM_ROOTFS_PATCH_PATH=$(PLATFORM_ROOTFS_PATCH_PATH)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME) ;\
			echo "chmod 777 $(CUSTOM_ROOTFS_BUILD_SCRIPT_NAME)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME) ;\
			echo "./$(CUSTOM_ROOTFS_BUILD_SCRIPT_NAME) $(ROOTFS_OUTPUT_TARGET_PATH) $(PLATFORM_ROOTFS_PATCH_PATH) $(BOARD_ROOTFS_PATCH_PATH) $(PRODUCT_VER_REV_PATCH)" >> $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME) ;\
			chmod 777 $(BUILDROOT_UTILITY)/$(POST_ROOTFS_BUILD_SCRIPT_NAME) ;\
			rsync -a --exclude=".svn"  $(BUILDROOT_UTILITY)/  $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/ ;\
		fi ; \
		# remove the patch file from the package of xfs ; \
		rm -rf $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION)/package/xfsprogs/000*	; \
	fi
	
	$(call msg,    patch Applied )

build:

	$(call msg,   Start Building Rootfs )
	
	@if [ -f $(BUILDROOT_PACKAGE_MAKEFILE_PATH)/Makefile ]; then \
	make -C $(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_TYPE)/package/buildroot build; \
	else \
		if [ $(BOARD_TYPE) == SSTAR_IPC_5MP_8MP ] || [ $(BOARD_TYPE) == SSTAR_IPC_2MP ]; then \
			$(MAKE) -C $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION) all BOARD_TYPE=$(BOARD_TYPE); \
		else \
			$(MAKE) -C $(PLATFORM_ROOTFS_BUILD_PATH)/$(BUILDROOT_VERSION) all; \
		fi \
	fi
	
	@if [ $(ROOTFS_BOARD_TYPE) == NVR_RK3568 ]; then \
		echo $(PRODUCT_VER_REV_PATCH) >  $(ROOTFS_OUTPUT_TARGET_PATH)/etc/PlatformVersionRevision ; \
	fi

	#copy the u-boot env binary to rootfs
	cp -rf $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/tools/env/fw_printenv $(ROOTFS_OUTPUT_TARGET_PATH)/usr/sbin/fw_printenv
	cp -rf $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/tools/env/fw_printenv $(ROOTFS_OUTPUT_TARGET_PATH)/usr/sbin/fw_setenv
	
	$(call msg,   Rootfs Build Success )
	

prepare_rootfs:

	$(call msg,   Start Building final Rootfs )
	
	#copy rsa public key
	#cp -f $(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_NAME)/package/uboot/$(SECURE_BOOT_PACKAGE_NAME)/rsa2048pem/rsa_pub_2048.pem $(ROOTFS_OUTPUT_TARGET_PATH)/etc/
	
	#copy app frimware data
	rm -rf $(PLATFORM_BUILD_PATH)/app_firmware
	mkdir -p $(PLATFORM_BUILD_PATH)/app_firmware	
	
	@if [ $(ROOTFS_BOARD_TYPE) == ARGO_FR ]; then \
	svn export --force $(FIRMWARE_RELEASE_DIR_PATH)/$(BOARD_NAME)_$(PRODUCT_VER_REV).zip $(PLATFORM_BUILD_PATH)/app_firmware/app_firmware_new.zip.enc; \
	cp -f $(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_NAME)/package/uboot/$(SECURE_BOOT_PACKAGE_NAME)/rsa2048pem/enc_dec.key $(PLATFORM_BUILD_PATH)/app_firmware/; \
	cd $(PLATFORM_BUILD_PATH)/app_firmware && openssl enc -aes-256-cbc -salt -d -in app_firmware_new.zip.enc -out app_firmware_new.zip -pass file:enc_dec.key; \
	unzip $(PLATFORM_BUILD_PATH)/app_firmware/app_firmware_new.zip -d $(PLATFORM_BUILD_PATH)/app_firmware/temp; \
	unzip $(PLATFORM_BUILD_PATH)/app_firmware/temp/*.zip -d $(PLATFORM_BUILD_PATH)/app_firmware/temp/temp; \
	cp -rf $(PLATFORM_BUILD_PATH)/app_firmware/temp/temp/* $(ROOTFS_OUTPUT_TARGET_PATH); \
	fi
	
	@if [ $(ROOTFS_BOARD_TYPE) == ESSENTIAL_IPC_8MP ]; then \
	mkdir -p $(ROOTFS_OUTPUT_TARGET_PATH)/matrix/kernel; \
	rm -rf $(PLATFORM_BUILD_PATH)/app_firmware; \
	mkdir -p $(PLATFORM_BUILD_PATH)/app_firmware; \
	mkdir -p $(PLATFORM_BUILD_PATH)/app_firmware/mibs; \
	svn export --force svn://192.168.100.5/Products/Matrix_IPCamera_Netra5000/SDT/V2/Built_packages/Third_Party_Modules/net-snmp/net-snmp-5.8/1.0/install/R2MP_R5MP_E8MP  $(PLATFORM_BUILD_PATH)/app_firmware/; \
	svn export --force svn://192.168.100.5/Products/Matrix_IPCamera_Netra5000/SDT/V2/Built_packages/Third_Party_Modules/net-snmp/net-snmp-5.8/1.0/mibs $(PLATFORM_BUILD_PATH)/app_firmware/mibs; \
	rm -rf $(PLATFORM_BUILD_PATH)/app_firmware/bin; \
	rm -rf $(PLATFORM_BUILD_PATH)/app_firmware/include; \
	rm -rf $(PLATFORM_BUILD_PATH)/app_firmware/share; \
	svn export --force $(FIRMWARE_RELEASE_DIR_PATH) $(PLATFORM_BUILD_PATH)/app_firmware/; \
	cp -rf $(PLATFORM_BUILD_PATH)/app_firmware/* $(ROOTFS_OUTPUT_TARGET_PATH); \
	echo $(PRODUCT_VER_REV_PATCH) >  $(ROOTFS_OUTPUT_TARGET_PATH)/etc/PlatformVersionRevision; \
	echo $(KERNEL_VER_SIGNATURE) > $(ROOTFS_OUTPUT_TARGET_PATH)/matrix/kernel/kernel_ver; \
	fi
	
	$(call msg,   Rootfs Build Success )

clean:

	$(call msg,   clean roofs )

	rm -rf $(PLATFORM_ROOTFS_BUILD_PATH)
	
	$(call msg,   clean roofs Success)
