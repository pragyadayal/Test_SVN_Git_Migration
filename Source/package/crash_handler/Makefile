
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

CRASHHANDLER_PACKAGE_NAME := crash_handler-0.6
CRASHHANDLER_PACKAGE_NAME_WITH_EXT= Crash_handler-0.6.tgz
CRASHHANDLER_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/crash_handler/$(CRASHHANDLER_PACKAGE_NAME_WITH_EXT)
CRASHHANDLER_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(CRASHHANDLER_PACKAGE_NAME)
CRASHHANDLER_INSTALL_DIR := $(CRASHHANDLER_SOURCE_DIR)/install
CRASHHANDLER_PATCH_PATH :=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/crash_handler/patch
CRASHHANDLER_TARGET_BIN_NAME =crash_handler

all: source extract patch build install

source:
	$(call msg, Start Exporting crash_handler package Source )
	if [ ! -d $(CRASHHANDLER_SOURCE_DIR) ]; then \
		svn $(SVN_CREDENTIALS) export --force $(CRASHHANDLER_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH);\
		tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(CRASHHANDLER_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH);  \
	fi;
		
	$(call msg, Export complete )

	
	
extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(CRASHHANDLER_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH);
	svn $(SVN_CREDENTIALS) export --force $(CRASHHANDLER_PATCH_PATH) $(CRASHHANDLER_SOURCE_DIR)
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	cd $(CRASHHANDLER_SOURCE_DIR) && $(MAKE) CROSS_COMPILE=$(CROSS_COMPILE);
	$(STRIP) --strip-debug --strip-unneeded $(CRASHHANDLER_SOURCE_DIR)/$(CRASHHANDLER_TARGET_BIN_NAME);
	mkdir -p $(CRASHHANDLER_INSTALL_DIR) && cp -f $(CRASHHANDLER_SOURCE_DIR)/$(CRASHHANDLER_TARGET_BIN_NAME) $(CRASHHANDLER_INSTALL_DIR)
	$(call msg, build is done )
	
install:
	cp -Rf $(CRASHHANDLER_INSTALL_DIR)/$(CRASHHANDLER_TARGET_BIN_NAME) $(MAIN_FILESYSTEM_PATH)/usr/bin/
	
