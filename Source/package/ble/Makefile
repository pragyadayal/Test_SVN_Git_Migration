SHELL:=/bin/bash

ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif


BLE_PACKAGE_NAME := AM335xBluetopiaLinuxProduction-4.2.1.0.1.0
BLE_PACKAGE_NAME_WITH_EXT=AM335xBluetopiaLinuxProduction-4.2.1.0.1.0.tar.xz
BLE_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/AM335xBluetopia/$(BLE_PACKAGE_NAME_WITH_EXT)
BLE_SOURCE_DIR   := $(LOCAL_PACKAGES_BUILD_PATH)/$(BLE_PACKAGE_NAME)
BLE_INSTALL_DIR := $(BLE_SOURCE_DIR)/BluetopiaPM/bin

all: source extract patch build install

source:
	$(call msg, Start Exporting zlib packages Source )
	if [ ! -d $(BLE_SOURCE_DIR) ]; then \
		svn $(SVN_CREDENTIALS) export --force $(BLE_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH);\
	fi
	$(call msg, Export complete )

extract:
	$(call msg,  Extracting package )
		if [ ! -d $(BLE_SOURCE_DIR) ]; then \
			tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(BLE_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH); \
		fi
	$(call msg,  Extract complete )


patch:
	$(call msg,   Applying patch )
		cp -rf $(SVN_BLUETOPIA_CONFIG_CHANGES)/* $(BLE_SOURCE_DIR)/
	$(call msg,    patch Applied )
	
build:
	$(call msg,  building  )
	cd $(BLE_SOURCE_DIR)/build && source configure.sh && make clean all
	$(call msg, build is done )
	
install:

	cp $(BLE_INSTALL_DIR)/SS1BTPM $(MAIN_FILESYSTEM_PATH)/matrix/appl
	cp $(BLE_INSTALL_DIR)/SS1Tool $(MAIN_FILESYSTEM_PATH)/matrix/appl
