
#Add space seprated board here
PLATFORM_SUPPORTED_BOARD_LIST= NETRA_IPC_8MP PANEL_LITE_V2 ATOM ARGO_FR RASPBERRY_PI_4 SSTAR_IPC_5MP_8MP SSTAR_IPC_2MP RV1126_ARGO_FR ESSENTIAL_IPC_8MP NVR_RK3568 NVRX_RK3588

PLATFORM_SUPPORTED_BOARD_VARIANT= L H

#checking input arguments 
ifeq ($(BOARD_TYPE),)
$(info Supported Board are $(PLATFORM_SUPPORTED_BOARD_LIST))
$(error Please pass a valid board e.g. BOARD_TYPE=NETRA_IPC_8MP)
endif

ifeq ($(BOARD_TYPE),PANEL_LITE_V2)
ifeq ($(FIRMWARE_ZIP),)
$(error "Please pass FIRMWARE_ZIP parameter, e.g FIRMWARE_ZIP=<svn server path>")
endif
endif

#validate input arguments
ifeq ($(filter $(PLATFORM_SUPPORTED_BOARD_LIST),$(BOARD_TYPE)),)
$(info Supported Board are $(PLATFORM_SUPPORTED_BOARD_LIST))
$(error Please pass a valid board type)
endif

ifeq ($(filter NETRA_IPC_8MP ARGO_FR RASPBERRY_PI_4 ESSENTIAL_IPC_8MP RV1126_ARGO_FR,$(BOARD_TYPE)),$(BOARD_TYPE))
ifeq ($(FIRMWARE_RELEASE_DIR_PATH),)
$(error "Please pass FIRMWARE_RELEASE_DIR_PATH parameter, e.g FIRMWARE_RELEASE_DIR_PATH=<svn server path>")
endif
endif

ifeq ($(filter SSTAR_IPC_5MP_8MP SSTAR_IPC_2MP NVR_RK3568 NVRX_RK3588,$(BOARD_TYPE)),$(BOARD_TYPE))
ifeq ($(PRODUCT_VERSION),)
$(error "Please pass PRODUCT_VERSION parameter, e.g PRODUCT_VERSION=V01")
endif
ifeq ($(PRODUCT_REVISION),)
$(error "Please pass PRODUCT_REVISION parameter, e.g PRODUCT_REVISION=R01")
endif
endif

ARCH_BIT=$(shell getconf LONG_BIT)

#including the constant path varibles
# PLATFORM_PATH_VARIABLE_FILE=$(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_TYPE)/Pathvariables.mk
PLATFORM_PATH_VARIABLE_FILE=$(CURDIR)/board/$(BOARD_TYPE)/Pathvariables.mk
include $(PLATFORM_PATH_VARIABLE_FILE)

#checking wether sdk check out or not
#ifeq (($(BOARD_TYPE),NETRA_IPC_8MP) || ($(BOARD_TYPE),ARGO_FR))
ifeq ($(BOARD_TYPE), $(filter $(BOARD_TYPE), NETRA_IPC_8MP ARGO_FR ESSENTIAL_IPC_8MP RV1126_ARGO_FR))
CHECK_SDK_CHECKOUT=$(realpath $(SDK_SOURCE_PATH))
ifeq ($(CHECK_SDK_CHECKOUT),)
$(error "Error to find SDK, please checkout at $(SDK_SOURCE_PATH)")
endif
endif

#validate input arguments
ifeq ($(BOARD_TYPE), $(filter $(BOARD_TYPE), NVR_RK3568))
ifeq ($(filter $(PLATFORM_SUPPORTED_BOARD_VARIANT),$(BOARD_VARIANT)),)
$(info Supported Board variant are $(PLATFORM_SUPPORTED_BOARD_VARIANT))
$(error Please pass a valid board variant)
endif
endif

export BOARD_VARIANT := $(BOARD_VARIANT)

all: $(SUPPORTED_BUILD_LIST)

release: packages drivers rootfs

install-toolchain:
ifeq ($(ARCH_BIT), 64)
	$(call msg,  installing toolchain )
	if [ $(BOARD_TYPE) == PANEL_LITE_V2  -o  $(BOARD_TYPE) == ATOM ]; then \
	if [ ! -d /opt/$(TOOLCHAIN_DIR_NAME) ]; then\
		sudo chmod 777 /opt/; \
		cd /opt && tar -xvf $(TOOLCHAIN_PACKAGE_DIR)/$(TOOLCHAIN_PACKAGE_NAME_WITH_EXT); \
	fi \
	fi
	$(call msg,  Toolchain installation done )
else
	echo "System is 32 Bit !!!!!"
	exit 1
endif
	
install-sdk:
	make -C $(SDK_INSTALLATION_MAKEFILE_PATH) all PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE) 
	
rootfs:
	@if [ $(BOARD_TYPE) == PANEL_LITE_V2 ]; then \
	make -C $(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_TYPE)/package/boot all PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE) ROOTFS_BOARD_TYPE=$(BOARD_TYPE); \
	elif [ $(BOARD_TYPE) == SSTAR_IPC_5MP_8MP ] || [ $(BOARD_TYPE) == SSTAR_IPC_2MP ] || [ $(BOARD_TYPE) == NVRX_RK3588 ]; then \
		make -C $(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_TYPE)/package rootfs_all PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE); \
	else \
		make -C $(PLATFORM_BUILDROOT_PATH) matrixfs PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE)  ROOTFS_BOARD_TYPE=$(BOARD_TYPE); \
		cp -rf $(BUILDROOT_OUTPUT_PATH)/images/$(BUILDROOT_ROOTFS_NAME) $(PLATFROM_BUILD_OUTPUT_PATH)/$(ROOTFS_IMAGE_NAME); \
	fi

uboot:
	make -C $(PLATFORM_UBOOT_SOURCE_PATH) all PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE) UBOOT_BOARD_TYPE=$(BOARD_TYPE)

kernel:
	make -C $(PLATFORM_KERNEL_SOURCE_PATH) all PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE)
	
drivers:
	make -C $(PLATFORM_DRIVER_SOURCE_PATH) all PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE)
	
packages:
	make -C $(PLATFORM_PACKAGE_PATH) all PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE)  PACKAGE_BOARD_TYPE=$(BOARD_TYPE)

production_release:
	@if [ $(BOARD_TYPE) == PANEL_LITE_V2 ]; then \
	make -C $(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_TYPE)/package/boot all PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE) ROOTFS_BOARD_TYPE=$(BOARD_TYPE); \
	make -C $(MISC_FILE_PATH) PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) all PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE) ROOTFS_BOARD_TYPE=$(BOARD_TYPE); \
	fi
	
	@if [ $(BOARD_TYPE) == ESSENTIAL_IPC_8MP ] || [ $(BOARD_TYPE) == SSTAR_IPC_2MP ] || [ $(BOARD_TYPE) == SSTAR_IPC_5MP_8MP ] || [ $(BOARD_TYPE) == RV1126_ARGO_FR ] || \
	[ $(BOARD_TYPE) == NVR_RK3568 ] || [ $(BOARD_TYPE) == NVRX_RK3588 ]; then \
		make -C $(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_TYPE)/package/production_release all PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE) ROOTFS_BOARD_TYPE=$(BOARD_TYPE); \
	fi
	
qa_release:
	@if [ $(BOARD_TYPE) == PANEL_LITE_V2 ]; then \
	make -C $(PLATFORM_SUPPORTED_BOARD_PATH)/$(BOARD_TYPE)/package/boot all PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE) ROOTFS_BOARD_TYPE=$(BOARD_TYPE); \
	make -C $(MISC_FILE_PATH) PATH_VARIABLE_FILE=$(PATH_VARIABLE_FILE) sdkzip PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE) ROOTFS_BOARD_TYPE=$(BOARD_TYPE); \
	fi
	
clean-all: clean-rootfs clean-uboot clean-kernel clean-drivers

clean-sdk_extract:
	rm -rf $(SDK_SOURCE_PATH)/$(HI3519A_COMMON_SOURCE_SUBDIR_NAME)
	
clean-rootfs:
	make -C $(PLATFORM_ROOTFS_PATH) clean PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE)

clean-uboot:
	make -C $(PLATFORM_UBOOT_SOURCE_PATH) clean PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE)
	
clean-kernel:
	make -C $(PLATFORM_KERNEL_SOURCE_PATH) clean PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE) 
	
clean-drivers:
	make -C $(PLATFORM_DRIVER_SOURCE_PATH) clean PATH_VARIABLE_FILE=$(PLATFORM_PATH_VARIABLE_FILE)
	
