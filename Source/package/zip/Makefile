
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

ZIP_PACKAGE_FOLDER_NAME=
ZIP_PACKAGE_NAME := ZIP-2.32
ZIP_PACKAGE_NAME_WITH_EXT=$(ZIP_PACKAGE_NAME).tar.gz
ZIP_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/$(ZIP_PACKAGE_NAME)/
ZIP_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(ZIP_PACKAGE_NAME)
ZIP_INSTALL_DIR := $(ZIP_SOURCE_DIR)/install
ZIP_TARGET_BIN_NAME = zip
ZIP_SOURCE_PATCH_PATH=$(PLATFORM_ZIP_PACKAGE_PATH)/patch_work

all: source extract patch build install

source:
	$(call msg, Start Exporting $(ZIP_PACKAGE_NAME) packages Source )
	if [ ! -d $(ZIP_SOURCE_DIR) ]; then \
		svn $(SVN_CREDENTIALS) export --force $(ZIP_SOURCE_PACKAGE_PATH)/ $(ZIP_SOURCE_DIR);\
	fi;
		
	$(call msg, Export $(ZIP_PACKAGE_NAME) complete )

	
	
extract:
	$(call msg,  Extracting $(ZIP_PACKAGE_NAME) package )
	tar -xvf $(ZIP_SOURCE_DIR)/$(ZIP_PACKAGE_NAME_WITH_EXT) -C $(ZIP_SOURCE_DIR)/;
	$(call msg,  Extract $(ZIP_PACKAGE_NAME) complete )

patch:
	$(call msg,   Applying patch to $(ZIP_PACKAGE_NAME))
	cp -rf $(ZIP_SOURCE_PATCH_PATH)/* $(ZIP_SOURCE_DIR)/$(ZIP_PACKAGE_NAME)
	$(call msg,   patch is applied to $(ZIP_PACKAGE_NAME))

build:
	$(call msg,  $(ZIP_PACKAGE_NAME) building  )
	
	cd $(ZIP_SOURCE_DIR)/$(ZIP_PACKAGE_NAME) && $(MAKE) CC=$(C_COMPILER) -f unix/Makefile clean
	cd $(ZIP_SOURCE_DIR)/$(ZIP_PACKAGE_NAME) && $(MAKE) CC=$(C_COMPILER) -f unix/Makefile generic
	
	$(call msg, $(ZIP_PACKAGE_NAME) build is done )
	
install:
	cp -Rf $(ZIP_SOURCE_DIR)/$(ZIP_PACKAGE_NAME)/$(ZIP_TARGET_BIN_NAME) $(MAIN_FILESYSTEM_PATH)/usr/bin/
	
