
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif



#Exporting here the KERNEL_ROOT as our local kernel build dir which alowe take this kernel as a root while building the internal  sdk drivers instead intenal sdk kernel root path
export KERNEL_ROOT=$(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION)
export KCFLAGS:=-Wno-date-time

all: extract source patch config build_$(BOARD_TYPE)

source:

extract:
	$(call msg,   Extracting SDK )
	
	$(call msg,    Extracting Done )

config:

		
patch:
	$(call msg,   Appling patch )
	
	mkdir -p $(ROOTFS_OUTPUT_TARGET_PATH)/matrix/ko/extdrv
	
	$(call msg,    patch Applied )
	
build_$(BOARD_TYPE): start_msg $(SUPPORTED_DRIVER_LIST) end_msg


start_msg:
	$(call msg,   Start Driver Building  )

GPIO:
	@if [ ! -z "$(PLATFORM_DRIVER_GPIO_PATH)" ]; then make -C $(PLATFORM_DRIVER_GPIO_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) ;fi
	
WATCHDOG:
	@if [ ! -z "$(PLATFORM_DRIVER_WATCHDOG_PATH)" ]; then make -C $(PLATFORM_DRIVER_WATCHDOG_PATH) all PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) ;fi


end_msg:
	$(call msg,   Driver Build complite  )
	
install:
	$(call msg,   Installing SDK )
		
	$(call msg,   Install SDK Done )

clean:
	$(call msg,   clean drivers  )
		
	@if [ ! -z "$(PLATFORM_DRIVER_GPIO_PATH)" ]; then make -C $(PLATFORM_DRIVER_GPIO_PATH) clean PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) ;fi
	
	@if [ ! -z "$(PLATFORM_DRIVER_WATCHDOG_PATH)" ]; then make -C $(PLATFORM_DRIVER_WATCHDOG_PATH) clean PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) ;fi
	
	$(call msg,  driver Clean complite  )	
