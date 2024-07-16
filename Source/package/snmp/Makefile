
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

SNMP_PACKAGE_NAME := net-snmp-5.7.2
SNMP_PACKAGE_NAME_WITH_EXT=$(SNMP_PACKAGE_NAME).tar.gz
SNMP_SOURCE_PACKAGE_PATH=$(SVN_SERVER_101)/Third_Party_Modules/SNMP/$(SNMP_PACKAGE_NAME_WITH_EXT)
SNMP_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(SNMP_PACKAGE_NAME)
SNMP_INSTALL_DIR := $(SNMP_SOURCE_DIR)/install

all: source extract patch build install

source:
	$(call msg, Start Exporting curl packages Source )
	if [ ! -d $(CRASHHANDLER_SOURCE_DIR) ]; then \
		svn $(SVN_CREDENTIALS) export --force $(CRASHHANDLER_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH);\
		tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(CRASHHANDLER_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH);  \
	fi;
		
	$(call msg, Export complete )

	
	
extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(CRASHHANDLER_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH);
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	cd $(CRASHHANDLER_PATCH_PATH) && cp $(CRASHHANDLER_PATCH_PATH)/*.c $(CRASHHANDLER_SOURCE_DIR)
	cd $(CRASHHANDLER_SOURCE_DIR) && $(MAKE) CROSS_COMPILE=$(CROSS_COMPILE);
	$(STRIP) --strip-debug --strip-unneeded $(CRASHHANDLER_SOURCE_DIR)/$(CRASHHANDLER_TARGET_BIN_NAME);
	mkdir -p $(CRASHHANDLER_INSTALL_DIR) && cp -f $(CRASHHANDLER_SOURCE_DIR)/$(CRASHHANDLER_TARGET_BIN_NAME) $(CRASHHANDLER_INSTALL_DIR)
	$(call msg, build is done )
	
install:
	cp -Rf $(CRASHHANDLER_INSTALL_DIR)/$(CRASHHANDLER_TARGET_BIN_NAME) $(MAIN_FILESYSTEM_PATH)/usr/bin/
	
