
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

DRIVER_NAME = watchdog

define msg
	@echo -e "\033[91m"
	@echo "========================================================================================================================================"
	@echo "   						 $(1)"
	@echo "========================================================================================================================================"
	@echo -e "\033[0m"
endef


obj-m := $(DRIVER_NAME).o

all: source extract config patch build install 

source:

extract:	
		
config:
		
patch:

build:
	$(call msg,   Start Building  $(DRIVER_NAME) drvier )
		
	make -C $(LOCAL_KERNEL_BUILD_PATH)/$(KERNEL_VERSION) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PLATFORM_DRIVER_WATCHDOG_PATH) modules 
	
	$(call msg,   $(DRIVER_NAME) Build complite )
	
install:
	cp -rf $(DRIVER_NAME).ko $(DRIVER_INSTALL_DIR)

clean:
	$(call msg,   clean $(DRIVER_NAME) drvier )
	
	@rm -f *.o modules.* *.symvers *.mod.c *.ko
	
	$(call msg,   clean $(DRIVER_NAME) complite )
	