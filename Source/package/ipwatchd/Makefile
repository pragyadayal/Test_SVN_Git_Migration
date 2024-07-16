
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

IPWATCHD_PACKAGE_NAME := ipwatchd-1.2.1
IPWATCHD_PACKAGE_NAME_WITH_EXT=$(IPWATCHD_PACKAGE_NAME).tar.gz
IPWATCHD_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/ipwatcher/$(IPWATCHD_PACKAGE_NAME_WITH_EXT)
IPWATCHD_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(IPWATCHD_PACKAGE_NAME)
IPWATCHD_INSTALL_DIR := $(IPWATCHD_SOURCE_DIR)/install
IPWATCHD_TARGET_BIN_NAME = ipwatchd
IPWATCHD_TARGET_CONFIG_FILE_NAME=ipwatchd.conf

WATCHD_MAKE_OPT = CC="$(C_COMPILER)" CFLAGS="-O3 -I$(BUILDROOT_INCLUDE_PATH)/usr/include" LIBS="-L$(BUILDROOT_LIB_PATH)/lib -L$(BUILDROOT_LIB_PATH)/usr/lib -lpcap -lnet -lnl-3 -lnl-genl-3"

all: source extract patch build install

source:
	$(call msg, Start Exporting $(IPWATCHD_PACKAGE_NAME) packages Source )
	if [ ! -d $(IPWATCHD_SOURCE_DIR) ]; then \
		svn export --force $(IPWATCHD_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH)/;\
	fi;
		
	$(call msg, Export $(IPWATCHD_PACKAGE_NAME) complete )

	
	
extract:
	$(call msg,  Extracting $(IPWATCHD_PACKAGE_NAME) package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(IPWATCHD_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH)/;
	$(call msg,  Extract $(IPWATCHD_PACKAGE_NAME) complete )

patch:
	$(call msg,   Applying patch to $(IPWATCHD_PACKAGE_NAME))
	$(call msg,   patch is applied to $(IPWATCHD_PACKAGE_NAME))

build:
	$(call msg,  $(IPWATCHD_PACKAGE_NAME) building  )
	
	cd $(IPWATCHD_SOURCE_DIR) && $(MAKE) $(WATCHD_MAKE_OPT) -C src -f Makefile
	
	$(call msg, $(IPWATCHD_PACKAGE_NAME) build is done )
	
install:
	cp -Rf $(IPWATCHD_SOURCE_DIR)/src/$(IPWATCHD_TARGET_BIN_NAME) $(ROOTFS_OUTPUT_TARGET_PATH)/usr/bin/
	cp -f $(IPWATCHD_SOURCE_DIR)/src/$(IPWATCHD_TARGET_CONFIG_FILE_NAME) $(ROOTFS_OUTPUT_TARGET_PATH)/etc/
	
