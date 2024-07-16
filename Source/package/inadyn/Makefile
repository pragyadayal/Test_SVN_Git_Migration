
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

INADYN_PACKAGE_NAME := inadyn-1.99.12
INADYN_PACKAGE_NAME_WITH_EXT= inadyn-1.99.12.tar.xz
INADYN_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/inadyn/$(INADYN_PACKAGE_NAME_WITH_EXT)
INADYN_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(INADYN_PACKAGE_NAME)
INADYN_INSTALL_DIR := $(INADYN_SOURCE_DIR)/install
INADYN_TARGET_BIN_NAME =inadyn

OPENSSL_PACKAGE_NAME ?= openssl-1.1.0g
OPENSSL_PACKAGE_NAME_WITH_EXT=$(OPENSSL_PACKAGE_NAME).tar.gz
OPENSSL_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/openssl/$(OPENSSL_PACKAGE_NAME_WITH_EXT)
OPENSSL_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(OPENSSL_PACKAGE_NAME)
OPENSSL_INSTALL_DIR := $(OPENSSL_SOURCE_DIR)/install

all: source extract patch build install

source:
	if [ ! -d $(INADYN_SOURCE_DIR) ]; then \
		svn export --force $(INADYN_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH); \
	fi;
	
extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(INADYN_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH);
	$(call msg,  Extract complete )
	
patch:
	$(call msg,   Applying patch )
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	cd $(INADYN_SOURCE_DIR) && ./configure CC=$(CC_ARM_LINUX)gcc --prefix=$(INADYN_INSTALL_DIR) --host --enable-openssl;
	cd $(INADYN_SOURCE_DIR) && make all install
	$(call msg, build is done )

	
	
install:
	cp -Rf $(INADYN_INSTALL_DIR)/sbin/$(INADYN_TARGET_BIN_NAME) $(MAIN_FILESYSTEM_PATH)/usr/bin/
	
 
