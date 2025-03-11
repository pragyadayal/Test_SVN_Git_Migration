ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid path variable file as PATH_VARIABLE_FILE=<path variable file>)
else
include $(PATH_VARIABLE_FILE)
endif

ARCH=arm

all:clean source extract patch src_clean config build install

secureboot: all build_secureboot install_secureboot

source:
	$(shell if [ ! -d "$(LOCAL_UBOOT_BUILD_PATH)" ]; then mkdir -p $(LOCAL_UBOOT_BUILD_PATH); fi ;)
	
	$(call msg, Start Package Source )
	
	cp -rf $(UBOOT_PACKAGE_PATH)/$(UBOOT_VERSION) $(LOCAL_UBOOT_BUILD_PATH)
	
	$(call msg, Source complete )
	
extract:

patch:
	$(call msg,   Appling patch )
	
	cp -rf $(PLATFORM_UBOOT_SOURCE_PATH)/$(UBOOT_VERSION)/* $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)
	cp -rf $(PLATFORM_UBOOT_SOURCE_PATH)/$(UBOOT_VERSION)/ddrbin_param.txt $(LOCAL_SDK_PATH)/rkbin/tools
	#cp -rf $(LOCAL_SDK_PATH)/rkbin/bin/rk35/rk3568_ddr_1560MHz_v1.09_dbg.bin $(LOCAL_SDK_PATH)/rkbin/bin/rk35/$(RK3568_DDR_BIN_NAME)
	$(LOCAL_SDK_PATH)/rkbin/tools/ddrbin_tool $(LOCAL_SDK_PATH)/rkbin/tools/ddrbin_param.txt $(LOCAL_SDK_PATH)/rkbin/bin/rk35/$(RK3568_DDR_BIN_NAME)
		
	$(call msg,    patch Applied )

src_clean:
	make -C $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) distclean

config:
	$(call msg,   Appling Configuration )
	
	cp -rf $(UBOOT_CONFIG) $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/.config	
	
	$(call msg,    Configuration Applied )
	
build:
	$(call msg,   Start Building  )
		
	cd $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION) && ./make.sh
	cd $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION) && make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) envtools
	cd $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION) && ./tools/mkimage -n rk3568 -T rksd -d $(LOCAL_SDK_PATH)/rkbin/bin/rk35/$(RK3568_DDR_BIN_NAME):$(LOCAL_SDK_PATH)/rkbin/bin/rk35/$(RK3568_SPL_BIN_NAME) $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/$(IDB_LOADER_NAME)
	$(call msg,   Uboot Build Success )
		
install:
	mkdir -p $(PLATFROM_BUILD_OUTPUT_PATH)
	cp $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/$(UBOOT_SPL_IMAGE_BUILD_NAME) $(PLATFROM_BUILD_OUTPUT_PATH)/$(UBOOT_SPL_IMAGE_NAME)
	cp $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/$(UBOOT_IMAGE_BUILD_NAME) $(PLATFROM_BUILD_OUTPUT_PATH)/$(UBOOT_IMAGE_NAME)
	cp $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/$(IDB_LOADER_NAME) $(PLATFROM_BUILD_OUTPUT_PATH)/$(IDBLOADER_IMAGE_NAME)

build_secureboot:
	$(call msg,   Start Building secure Uboot )
	
	#first create keys folder and generate private and public if not present
	#generate public key and privite key
	#generated key name is privateKey.pem and publicKey.pem
	#Uboot requirement is privateKey.pem as dev.key
	#Uboot requirement is publicKey.pem as dev.pubkey
	#generate Self certificate from generated keys
	
	@if [ ! -f $(PLATFORM_UBOOT_SOURCE_PATH)/$(UBOOT_VERSION)/keys/dev.key ]; then \
		echo "Generating Public and private keys"; \
		mkdir -p $(PLATFORM_UBOOT_SOURCE_PATH)/$(UBOOT_VERSION)/keys; \
		$(LOCAL_PACKAGES_BUILD_PATH)/rkbin/tools/rk_sign_tool KK --bits 2048 --out $(PLATFORM_UBOOT_SOURCE_PATH)/$(UBOOT_VERSION)/keys/; \
		mv -f $(PLATFORM_UBOOT_SOURCE_PATH)/$(UBOOT_VERSION)/keys/privateKey.pem $(PLATFORM_UBOOT_SOURCE_PATH)/$(UBOOT_VERSION)/keys/dev.key; \
		mv -f $(PLATFORM_UBOOT_SOURCE_PATH)/$(UBOOT_VERSION)/keys/publicKey.pem $(PLATFORM_UBOOT_SOURCE_PATH)/$(UBOOT_VERSION)/keys/dev.pubkey; \
		cd $(PLATFORM_UBOOT_SOURCE_PATH)/$(UBOOT_VERSION) && openssl req -batch -new -x509 -key keys/dev.key -out keys/dev.crt; \
	else \
		echo "Public and private keys available no need to generate"; \
	fi
	
	cp -rf $(PLATFORM_UBOOT_SOURCE_PATH)/$(UBOOT_VERSION)/keys  $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/
	
	echo "CONFIG_FIT_SIGNATURE=y"     >> $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/.config
	echo "# CONFIG_FIT_ROLLBACK_PROTECT is not set" >> $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/.config
	echo "CONFIG_SPL_FIT_SIGNATURE=y" >> $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/.config
	echo "# CONFIG_SPL_FIT_ROLLBACK_PROTECT is not set" >> $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/.config
	echo "CONFIG_FIT_ENABLE_RSASSA_PSS_SUPPORT=y" >> $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/.config
	
	cd $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION) && ./make.sh --spl-new --boot_img $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)/$(RK_BOOT_IMG) --chip 3568 --burn-key-hash 
	# Sign idbloader for emmc production hex
	cd $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION) && $(LOCAL_PACKAGES_BUILD_PATH)/rkbin/tools/rk_sign_tool SR --key ./keys/dev.key --pubkey ./keys/dev.pubkey --idb $(IDB_LOADER_NAME) 
	$(call msg,   Secure Uboot Build Success )
	
	
install_secureboot:
	$(call msg,   Start secureboot installation  )
	$(shell if [ ! -d $(PLATFROM_BUILD_OUTPUT_PATH) ]; then mkdir $(PLATFROM_BUILD_OUTPUT_PATH); fi ;)
	cp $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/$(UBOOT_SPL_IMAGE_BUILD_NAME)  $(PLATFROM_BUILD_OUTPUT_PATH)/$(UBOOT_SPL_IMAGE_NAME)
	cp $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/$(UBOOT_IMAGE_BUILD_NAME) $(PLATFROM_BUILD_OUTPUT_PATH)/$(UBOOT_IMAGE_NAME)
	cp $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/$(RK_BOOT_IMG) $(KERNEL_BUILD_OUTPUT_PATH)/$(KERNEL_IMAGE_NAME)
	cp $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)/$(IDB_LOADER_NAME) $(PLATFROM_BUILD_OUTPUT_PATH)/$(IDBLOADER_IMAGE_NAME)
	$(call msg,   secureboot installation  complete)
	
	
clean:
	$(call msg,   clean Uboot  )
	
	rm -rf $(LOCAL_UBOOT_BUILD_PATH)/$(UBOOT_VERSION)
	
	$(call msg,   clean Uboot  Success)

