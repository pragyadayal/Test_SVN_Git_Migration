###########################################################################################################
# AM335x sdk8 Defintions - this section should be updated based on SDK installation path
###########################################################################################################
ARCH_BIT=$(shell getconf LONG_BIT)
KERNELDIR=$(KERNEL_PATH)/
TOOLCHAIN_PATH_NATIVE=$(TOOLCHAIN_PATH)
TOOLCHAIN_PATH_TARGET=$(TOOLCHAIN_PATH)
ifeq ($(ARCH_BIT), 32)
TOOLCHAIN_PREFIX=arm-buildroot-linux-gnueabihf
else
TOOLCHAIN_PREFIX=arm-linux-gnueabihf
endif
ARCH=arm

###########################################################################################################
# Following section should be filled for non TI SDKs only
###########################################################################################################
# Follwoing are defs for AM335x
CFLAGS+=-march=armv7-a -marm -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a8 --sysroot=$(SVN_HOME_DIR)/Briefcase/SWD_Briefcase/GenericSoftwareModules/Platform/SWD/Trunk/Source/build/$(BOARD_TYPE)/package/buildroot/buildroot-2016.08/output/host/usr/arm-buildroot-linux-gnueabihf/sysroot/ -I/opt/gcc-linaro-5.3-2016.02-x86_64_arm-linux-gnueabihf/lib/gcc/arm-linux-gnueabihf/5.3.1/include/ -L$(SVN_HOME_DIR)/Briefcase/SWD_Briefcase/GenericSoftwareModules/Platform/SWD/Trunk/Source/build/$(BOARD_TYPE)/package/buildroot/buildroot-2016.08/output/host/usr/arm-buildroot-linux-gnueabihf/sysroot/usr/lib/ -lcrypto

LDFLAGS+=-lpthread -lm -lrt -lcrypto --sysroot=$(SVN_HOME_DIR)/Briefcase/SWD_Briefcase/GenericSoftwareModules/Platform/SWD/Trunk/Source/build/$(BOARD_TYPE)/package/buildroot/buildroot-2016.08/output/host/usr/arm-buildroot-linux-gnueabihf/sysroot/


###########################################################################################################
# Following section should not be updated
###########################################################################################################
CC=$(TOOLCHAIN_PREFIX)-gcc
AS=$(TOOLCHAIN_PREFIX)-as
LD=$(TOOLCHAIN_PREFIX)-ld
GDB=$(TOOLCHAIN_PREFIX)-gdb
STRIP=$(TOOLCHAIN_PREFIX)-strip
RANLIB=$(TOOLCHAIN_PREFIX)-ranlib
OBJCOPY=$(TOOLCHAIN_PREFIX)-objcopy
OBJDUMP=$(TOOLCHAIN_PREFIX)-objdump
AR=$(TOOLCHAIN_PREFIX)-ar
NM=$(TOOLCHAIN_PREFIX)-nm

# The following should apply for gstreamer plugin
#CONFIGURE_FLAGS=--target=arm-linux-gnueabi --host=arm-linux-gnueabi --build=i686-linux --with-libtool-sysroot=$(TOOLCHAIN_PATH_TARGET)

PKG_CONFIG_SYSROOT_DIR=$(TOOLCHAIN_PATH_TARGET)
PKG_CONFIG_PATH=$(TOOLCHAIN_PATH_TARGET)/usr/lib/pkgconfig
PKG_CONFIG_ALLOW_SYSTEM_LIBS=1


GLOBCFLAG=S$(CFLAGS)
GLOBLDFLAGS=$(LDFLAGS)

CROSS_COMPILE=$(TOOLCHAIN_PREFIX)-

PATH:=$(TOOLCHAIN_PATH_NATIVE):$(PATH)
