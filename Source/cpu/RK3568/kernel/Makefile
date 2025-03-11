
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

RK_KERNEL_DEFCONFIG_FRAGMENT=rockchip_linux_bifrost.config
RK_KERNEL_DEFCONFIG=rockchip_linux_defconfig

JMB527_SATA_MULTIPLIER_FIRMWARE=JMB572_fw.bin
JMB527_SATA_MULTIPLIER_FIRMWARE_FILE_H=JMB572_fw.h

all:source extract patch config build install  #clean distclean

source:
	$(shell if [ ! -d "$(LOCAL_KERNEL_BUILD_PATH)" ]; then mkdir -p $(LOCAL_KERNEL_BUILD_PATH); fi ;)
	
	$(call msg, Start Source Linux Kernel  )

	@if [ ! -f $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)/.tmp ]; then \
	cp -rf  $(KERNEL_PACKAGE_PATH) $(LOCAL_KERNEL_BUILD_PATH)/; \
	touch $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)/.tmp; \
	fi
	
	$(call msg, Source complete )

extract:	

	$(call msg,  Start Extracting Linux Kernel  )
	
	$(call msg,  Linux Kernel Extract complete )
		
config:

	$(call msg,   Appling Configuration )
	
	cp -rf $(KERNEL_CONFIG) $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)/arch/arm64/configs/$(RK_KERNEL_DEFCONFIG)
	#make -C $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) $(RK_KERNEL_DEFCONFIG) $(RK_KERNEL_DEFCONFIG_FRAGMENT)
	make -C $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) $(RK_KERNEL_DEFCONFIG)
	
	$(call msg,    Configuration Applied )
		
patch:
	$(call msg,   Appling patch )
	
	#read the jmb572 firmware file 
	cd $(BOARD_ROOTFS_PATCH_PATH)/lib/firmware/ && xxd -i $(JMB527_SATA_MULTIPLIER_FIRMWARE) > $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)/drivers/ata/$(JMB527_SATA_MULTIPLIER_FIRMWARE_FILE_H)
	
	rsync -a --exclude=".svn"  $(PLATFORM_KERNEL_SOURCE_PATH)/$(KERNEL_VERSION)/  $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)/
	
	cp -f $(SDK_UPDATE_IMAGES_PATH)/* $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)/
	
	$(call msg,    patch Applied )

distclean:
	$(call msg,   Start distclean  )
	
	make -C $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) distclean
	
	$(call msg,   distclean complete )
	
build:
	$(call msg,   Start Building  )
	
	#RK_KERNEL_DEFCONFIG_FRAGMENT option for kernel build is not required becuase its already added in kernel.config file
	make -C $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) $(RK_KERNEL_DTS).img -j12
	make -C $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules -j12
	
	
	#ln -rfs $(PLATFORM_BUILD_DEVICE_PATH)/$(RK_TARGET_PRODUCT)/BoardConfig-rk3568-nvr.mk $(PLATFORM_BUILD_DEVICE_PATH)/.BoardConfig.mk	
	
	#@if [ -f "$(PLATFORM_BUILD_DEVICE_PATH)/$(RK_TARGET_PRODUCT)/$(RK_KERNEL_FIT_ITS)" ]; then \
		#$(PLATFORM_BUILD_DEVICE_PATH)/common/mk-fitimage.sh $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)/$(RK_BOOT_IMG) \
		#$(PLATFORM_BUILD_DEVICE_PATH)/$(RK_TARGET_PRODUCT)/$(RK_KERNEL_FIT_ITS); \
	#fi
	$(call msg,   Kernel Build complete )
	
install:
	$(shell if [ ! -d $(PLATFROM_BUILD_OUTPUT_PATH) ]; then mkdir $(PLATFROM_BUILD_OUTPUT_PATH); fi ;)
	@mkdir -p $(KERNEL_BUILD_OUTPUT_PATH)
	@mkdir -p $(KERNEL_MODULE_BUILD_OUTPUT_PATH)
	$(INSTALL) $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)/$(RK_BOOT_IMG) $(PLATFROM_BUILD_OUTPUT_PATH)/kernel/$(KERNEL_IMAGE_NAME)
	#$(INSTALL) $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)/drivers/ata/*.ko $(KERNEL_MODULE_BUILD_OUTPUT_PATH)/

clean:
	$(call msg,   Clean kernel  )
	
	rm -rf $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)
	
	$(call msg,   Clean kernel complete)

