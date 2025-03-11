 
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif


define msg
	@echo -e "\033[91m"
	@echo "========================================================================================================================================"
	@echo "   						 $(1)"
	@echo "========================================================================================================================================"
	@echo -e "\033[0m"
endef

all: source extract patch build install

source:
	$(call msg, Start Exporting ble packages Source )
	$(shell if [ ! -d "$(PLATFORM_BLE_BUILD_PATH)" ]; then mkdir -p $(PLATFORM_BLE_BUILD_PATH); fi ;)
	svn $(SVN_CREDENTIALS) export --force $(BLE_PACKAGE_SOURCE) $(PLATFORM_BLE_BUILD_PATH)
	$(call msg, Export complete )
	
extract:
	$(call msg,  Extracting package )
	
	tar -xvf $(PLATFORM_BLE_BUILD_PATH)/$(AM3352BLUETOPIA_PACKAGE) -C $(PLATFORM_BLE_BUILD_PATH)
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	rsync -a --exclude=".svn"  $(PLATFORM_BLE_PATCH_PATH)/ $(PLATFORM_BLE_BUILD_PATH)/$(BLUETOPIA_VERSION)
	$(call msg,    patch Applied )

build:
	$(call msg,  building ble )
	cd $(PLATFORM_BLE_BUILD_PATH)/$(BLUETOPIA_VERSION)/build && source configure.sh && $(MAKE) clean && $(MAKE) all BOARD_TYPE=$(ROOTFS_BOARD_TYPE)
	$(call msg,   ble build is done )
	
install:
	cp $(PLATFORM_BLE_BUILD_PATH)/$(BLUETOPIA_VERSION)/BluetopiaPM/bin/SS1BTPM $(PLATFORM_ROOTFS_PATCH_PATH)/$(ROOTFS_BOARD_TYPE)/matrix/appl
