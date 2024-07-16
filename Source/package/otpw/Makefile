ifeq ($(BOARD_TYPE),RASPBERRY_PI_4)
SHELL:=/bin/bash
C_COMPILER=gcc
SVN_SERVER=svn://192.168.100.5
MATRIX_EMBEDDED_SOFTWARE_MODULE=$(SVN_SERVER)/Briefcase/SWD_Briefcase/EmbeddedSoftwareModules
LOCAL_PACKAGES_BUILD_PATH=/root/BUILD
PLATFORM_PACKAGE_PATH=/root
else
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif
endif

OTPW_PACKAGE_NAME := otpw-1.5
OTPW_PACKAGE_NAME_WITH_EXT=$(OTPW_PACKAGE_NAME).tar.gz
OTPW_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/Secure_Remote_Login/$(OTPW_PACKAGE_NAME_WITH_EXT)
OTPW_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(OTPW_PACKAGE_NAME)
OTPW_INSTALL_DIR := $(OTPW_SOURCE_DIR)/install
OTPW_PATCH_DIR := $(PLATFORM_PACKAGE_PATH)/otpw/otpwPatch

AESCRYPT_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/AESCrypt
AESCRYPT_SOURCE_DIR :=$(LOCAL_PACKAGES_BUILD_PATH)/AESCrypt

PAM_PACKAGE_NAME := Linux-PAM-1.3.0
PAM_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(PAM_PACKAGE_NAME)
PAM_INSTALL_DIR := $(PAM_SOURCE_DIR)/install

ifeq ($(BOARD_TYPE),RASPBERRY_PI_4)
_LDFLAGS_="-L$(AESCRYPT_SOURCE_DIR) -lmaescrypt /lib/arm-linux-gnueabihf/libpam.so.0 /lib/arm-linux-gnueabihf/libpam_misc.so.0"
_CFLAGS_="-O -ggdb -W -Wall -Wno-unused-result -fPIC -I$(LOCAL_PACKAGES_BUILD_PATH)/Linux-PAM-1.3.1/libpam/include/ -I$(AESCRYPT_SOURCE_DIR)"
else
_LDFLAGS_="-L$(AESCRYPT_SOURCE_DIR) -lmaescrypt -L$(PAM_INSTALL_DIR)/lib/ -lpam -lpam_misc"
_CFLAGS_="-O -ggdb -fPIC -I$(PAM_INSTALL_DIR)/usr/include/ -I$(AESCRYPT_SOURCE_DIR)"
endif

all: source extract patch build install

source:
	$(call msg, Start Exporting otpw packages Source )
	if [ ! -d $(OTPW_SOURCE_DIR) ]; then \
		svn export --force $(OTPW_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH); \
		svn export --force $(AESCRYPT_SOURCE_PACKAGE_PATH)	$(AESCRYPT_SOURCE_DIR);\
		svn export --force $(MATRIX_EMBEDDED_SOFTWARE_MODULE)/Secure_Remote_Login/Linux-PAM-1.3.1.tar.xz $(LOCAL_PACKAGES_BUILD_PATH); \
	fi
	$(call msg, Export complete )

extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(OTPW_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH);
	@if [ $(BOARD_TYPE) == RASPBERRY_PI_4 ]; then \
		tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/Linux-PAM-1.3.1.tar.xz -C $(LOCAL_PACKAGES_BUILD_PATH); \
	fi
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	rsync -a --exclude=".svn" $(OTPW_PATCH_DIR)/ $(OTPW_SOURCE_DIR)/
	
	@if [ $(BOARD_TYPE) == RASPBERRY_PI_4 ]; then \
		rm -f $(OTPW_SOURCE_DIR)/pam_otpw.c; \
		mv $(OTPW_SOURCE_DIR)/pam_otpw_RPi4.c $(OTPW_SOURCE_DIR)/pam_otpw.c; \
	else \
		rm -f $(OTPW_SOURCE_DIR)/pam_otpw_RPi4.c; \
	fi
	
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	cd $(AESCRYPT_SOURCE_DIR) && make CC=$(C_COMPILER) clean all; \
	cd $(OTPW_SOURCE_DIR) && make clean && make CC=$(C_COMPILER) CFLAGS=$(_CFLAGS_) LDFLAGS=$(_LDFLAGS_) PAMLIB=$(PAM_INSTALL_DIR)/lib/
	$(call msg, build is done )
	
install:
	mkdir -p $(MAIN_FILESYSTEM_PATH)/sbin/
	mkdir -p $(MAIN_FILESYSTEM_PATH)/lib/security/
	cp -rf  $(OTPW_SOURCE_DIR)/otpw-gen 		$(MAIN_FILESYSTEM_PATH)/sbin/
	cp -rf $(OTPW_SOURCE_DIR)/pam_otpw.so 		$(MAIN_FILESYSTEM_PATH)/lib/security/
