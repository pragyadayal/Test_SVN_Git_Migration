
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

PASSWORD_UPDATE_BUILD_DIR=$(LOCAL_PACKAGES_BUILD_PATH)

all: source build install

source:
	$(call msg, Start Exporting password_update packages Source )
	cp -rf $(PLATFORM_PASWORDUPDATE_PACKAGE_PATH)  $(PASSWORD_UPDATE_BUILD_DIR)
	$(call msg, Export complete )

	
	
extract:
	$(call msg,  Extracting package )
	
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	cd $(PASSWORD_UPDATE_BUILD_DIR)/passwordupdate && $(C_COMPILER) passwordupdate.c -o passwordupdate
	$(call msg, build is done )
	
install:
	mkdir -p $(MAIN_FILESYSTEM_PATH)/usr/sbin
	cp -rf $(PASSWORD_UPDATE_BUILD_DIR)/passwordupdate/passwordupdate 		$(MAIN_FILESYSTEM_PATH)/usr/sbin/
	
	
