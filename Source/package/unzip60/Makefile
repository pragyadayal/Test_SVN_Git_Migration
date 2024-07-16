
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

UNZIP60_PACKAGE_FOLDER_NAME=Unzip60
UNZIP60_PACKAGE_NAME := unzip60
UNZIP60_PACKAGE_NAME_WITH_EXT=$(UNZIP60_PACKAGE_NAME).tar.gz
UNZIP60_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/$(UNZIP60_PACKAGE_NAME)/
UNZIP60_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(UNZIP60_PACKAGE_FOLDER_NAME)
UNZIP60_INSTALL_DIR := $(UNZIP60_SOURCE_DIR)/install
UNZIP60_TARGET_BIN_NAME = unzip
UNZIP60_SOURCE_PATCH_PATH=$(PLATFORM_UNZIP60_PACKAGE_PATH)/patch_work

all: source extract patch build install

source:
	$(call msg, Start Exporting $(UNZIP60_PACKAGE_NAME) packages Source )
	if [ ! -d $(UNZIP60_SOURCE_DIR) ]; then \
		svn export --force $(MATRIX_EMBEDDED_SOFTWARE_MODULE)/$(UNZIP60_PACKAGE_FOLDER_NAME) $(UNZIP60_SOURCE_DIR);\
	fi;
		
	$(call msg, Export $(UNZIP60_PACKAGE_NAME) complete )

	
	
extract:
	$(call msg,  Extracting $(UNZIP60_PACKAGE_NAME) package )
	tar -xvf $(UNZIP60_SOURCE_DIR)/$(UNZIP60_PACKAGE_NAME_WITH_EXT) -C $(UNZIP60_SOURCE_DIR)/;
	$(call msg,  Extract $(UNZIP60_PACKAGE_NAME) complete )

patch:
	$(call msg,   Applying patch to $(UNZIP60_PACKAGE_NAME))
	cp -rf $(UNZIP60_SOURCE_PATCH_PATH)/* $(UNZIP60_SOURCE_DIR)/$(UNZIP60_PACKAGE_NAME)
	$(call msg,   patch is applied to $(UNZIP60_PACKAGE_NAME))

build:
	$(call msg,  $(UNZIP60_PACKAGE_NAME) building  )
	
	cd $(UNZIP60_SOURCE_DIR)/$(UNZIP60_PACKAGE_NAME) && $(MAKE) CC=$(C_COMPILER) -f unix/Makefile clean
	cd $(UNZIP60_SOURCE_DIR)/$(UNZIP60_PACKAGE_NAME) && $(MAKE) CC=$(C_COMPILER) -f unix/Makefile generic
	
	$(call msg, $(UNZIP60_PACKAGE_NAME) build is done )
	
install:
	cp -Rf $(UNZIP60_SOURCE_DIR)/$(UNZIP60_PACKAGE_NAME)/$(UNZIP60_TARGET_BIN_NAME) $(MAIN_FILESYSTEM_PATH)/usr/bin/
	
