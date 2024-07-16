
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

FDISK_PACKAGE_NAME := busybox-1.18.5
FDISK_PACKAGE_NAME_WITH_EXT= $(FDISK_PACKAGE_NAME).tar.bz2
FDISK_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/busybox/$(FDISK_PACKAGE_NAME_WITH_EXT)
FDISK_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(FDISK_PACKAGE_NAME)
FDISK_INSTALL_DIR := $(FDISK_SOURCE_DIR)/install
FDISK_CONFIG_FILE :=fdisk.config


all: source extract patch configure build install

source:
	if [ ! -d $(FDISK_SOURCE_DIR) ]; then \
		svn $(SVN_CREDENTIALS) export --force $(FDISK_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH); \
	fi;
extract:
	$(call msg,  Extracting package $(FDISK_PACKAGE_NAME))
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(FDISK_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH);
	$(call msg,  Extract complete $(FDISK_PACKAGE_NAME))

patch:
	$(call msg,   Applying patch to $(FDISK_PACKAGE_NAME))
	
	$(call msg,   patch is applied to $(FDISK_PACKAGE_NAME))
	
configure:
	$(call msg,   configuring $(FDISK_PACKAGE_NAME))
	cp -rf $(FDISK_CONFIG_FILE) $(FDISK_SOURCE_DIR)/.config

	$(call msg,   configured $(FDISK_PACKAGE_NAME))

build:
	$(call msg,  $(FDISK_PACKAGE_NAME) building  )
	make -C $(FDISK_SOURCE_DIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) all

	$(call msg, $(FDISK_PACKAGE_NAME) build is done )
	
install:
	rm -rf $(ROOTFS_OUTPUT_TARGET_PATH)/sbin/fdisk
	cp -rf $(FDISK_SOURCE_DIR)/busybox $(ROOTFS_OUTPUT_TARGET_PATH)/sbin/fdisk

