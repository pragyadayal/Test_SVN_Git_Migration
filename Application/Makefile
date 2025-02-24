#************************************************************************
# Filename		: Makefile
# Author		: Kunjal Garach
# Description	: Makefile to compile NGT Master Application
# Date			: 12th July 2010
# NOTE			: This make file uses moderately advanced feature of -
#		  	  	  gnu make and should be edited with proper care.
#************************************************************************
#
#************************************************************************
SHELL = /bin/sh

#Include header file
include ../Build/SystemAppsPath.mak

#############################################
# Bluetooth Flag Value
#############################################
BLUEZ_STACK_SUPPORTED=0

##############################################
# Check options passed to make file
##############################################
MODULE_BIN_DIR := bin
MODULE_OBJ_DIR := obj

CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar
LINK := $(CROSS_COMPILE)gcc

ifeq ($(DEBUG), on)
DEBUG_FLAG := -g
else
DEBUG_FLAG :=
endif

ifeq ($(VERBOSE), on)
_@_ :=
else
_@_ := @
endif

##############################################
# COMPILER SELECTION + PROJECT EXTENSIONS
##############################################

DEPEND_EXTENSION = .d
OBJ_EXTENSION = .o
NODEPEND_TARGETS := clean

# GENERIC_BUILD_DIR=../AM335xBluetopiaLinuxProduction-4.0.3.0.1.0/build/core
# PROJECT_BUILD_DIR=../AM335xBluetopiaLinuxProduction-4.0.3.0.1.0/build/proj
# DEPDIR=../AM335xBluetopiaLinuxProduction-4.0.3.0.1.0/build/dep
# 
# include ${PROJECT_BUILD_DIR}/shell-defs.mak
# include ${PROJECT_BUILD_DIR}/toolchain-defs.mak
# include ${GENERIC_BUILD_DIR}/bluetopia.mak
# include ${GENERIC_BUILD_DIR}/bluetopiapm.mak
# include ${PROJECT_BUILD_DIR}/platform.mak
# include ${PROJECT_BUILD_DIR}/plat-rules.mak
# include ${GENERIC_BUILD_DIR}/bluetopia-rules.mak
# include ${GENERIC_BUILD_DIR}/bluetopiapm-rules.mak
# include ${GENERIC_BUILD_DIR}/dist.mak
# include $(wildcard $(DEPDIR)/*.d)
##############################################
# SOURCE DIRECTORY PATH
##############################################
MODULE_SOURCE_DIR := .

ACCESS_POLICIES_SOURCE_DIR := ./AccessPolicies
ACMS_SOURCE_DIR := ./ACMS
CONFIGURATION_SOURCE_DIR := ./Configuration
BLUETOOTH_SOURCE_DIR := ./Bluetooth
DC_TASK_SOURCE_DIR := ./DCTask
DOOR_COMM_TASK_SOURCE_DIR := ./DoorCommTask
ENROLLMENT_SOURCE_DIR := ./Enrollment
EVENT_LOG_SOURCE_DIR := ./EventLog
FEATURES_SOURCE_DIR := ./Features
INTERFACES_SOURCE_DIR := ./Interfaces
READER_SOURCE_DIR := ./Reader
TEMP_SOURCE_DIR := ./TemperatureSensorReader
SYS_TEST_SOURCE_DIR := ./SysTest
SYS_TIME_SOURCE_DIR := ./SysTime
SYS_TIMER_SOURCE_DIR := ./SysTimer
WEB_SERVER_SOURCE_DIR := ./WebServer
WEB_API_SOURCE_DIR := ./WebAPI

GPIO_DRIVER_SOURCE_DIR := $(DRIVER_PATH)/GPIO
WIEGAND_DRIVER_SOURCE_DIR := $(DRIVER_PATH)/WIEGAND

OPENSSL_INCLUDE_PATH=$(OPENSSL_INSTALL_DIR)/include
CURL_INCLUDE_PATH=$(CURL_INSTALL_DIR)/include
ZLIB_INCLUDE_PATH=$(ZLIB_INSTALL_DIR)/include

COMMON_ACCESS_POLICIES_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/AccessPolicies
COMMON_ACMS_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/ACMS
COMMON_CONFIGURATION_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/Configuration
COMMON_EVENT_LOG_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/EventLog
COMMON_FEATURES_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/Features
COMMON_INTERFACES_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/Interfaces
COMMON_READER_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/Reader
COMMON_TEMP_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/TemperatureSensorReader
COMMON_SYS_TIME_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/SysTime
COMMON_SYS_TIMER_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/SysTimer
COMMON_WEB_SERVER_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/WebServer
COMMON_SESSION_MANAGEMENT_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/WebServer/SessionManagement
COMMON_WEB_API_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/WebAPI
COMMON_DC_TASK_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/DCTask
COMMON_DOOR_COMM_TASK_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/DoorCommTask
COMMON_PUSH_API_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/PushApi
COMMON_SYSLOG_DIR := $(APP_COMMON_SOURCE_DIR)/Syslog
COMMON_HTTP_FTC_DIR := $(APP_COMMON_SOURCE_DIR)/HttpFtc
COMMON_FR_HANDLER_DIR := $(APP_COMMON_SOURCE_DIR)/FrHandler
BLUETOPIA_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/Bluetooth/BlueTopia_Stack
ifeq ($(BLUEZ_STACK_SUPPORTED), 1)
BLUETOOTH_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/Bluetooth
BLUEZ_SOURCE_DIR := $(APP_COMMON_SOURCE_DIR)/Bluetooth/BlueZ_Stack
endif

BT_ALL_LIBS := $(BLUETOOTH_INCLUDE_PATH)/lib/*.a
BT_ALL_LIBS += $(BLUETOOTH_INCLUDE_PATH)/Bluetopia/lib/*.a

NXP_LIB_SOURCE_NAME := AM335xNxpNfcRdLib_Linux_v05.22.00_Extended
NXP_LIB_SOURCE_DIR := $(MODULE_SOURCE_DIR)/../Build/$(NXP_LIB_SOURCE_NAME)
NXP_LIB_DIR := $(NXP_LIB_SOURCE_DIR)/lib

NXP_LIB := $(NXP_LIB_DIR)/libMxCLRC663NXPLibWrapper.a
NXP_LIB += $(NXP_LIB_DIR)/libphOsal.a 
NXP_LIB += $(NXP_LIB_DIR)/libDAL.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_acDiscLoop.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_halHw.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_KeyStore.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_phTools.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palFelica.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI18092mPI.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4a.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palMifare.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alFelica.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alMful.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alMfdf.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alT1T.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alTop.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alMfc.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palSli15693.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI18000p3m3.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alICode.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alI18000p3m3.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alICode.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palEpcUid.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_lnLlcp.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4mC.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI18092mT.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_ceT4T.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palEpcUid.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_phNfcLib.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_acDiscLoop.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alICode.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palSli15693.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alI18000p3m3.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI18000p3m3.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p3a.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p3b.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_halHw.a 
NXP_LIB += $(NXP_LIB_DIR)/libphOsal.a
NXP_LIB += $(NXP_LIB_DIR)/libDAL.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI18092mPI.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4a.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alMfdf.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alT1T.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alTop.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alFelica.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palFelica.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alMful.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palMifare.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_CryptoSym.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_CryptoRng.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alMfc.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_KeyStore.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palEpcUid.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_lnLlcp.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_phTools.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI18092mT.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_ceT4T.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4mC.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_npSnep.a 
NXP_LIB += $(NXP_LIB_DIR)/*.a

NXP_LIB_SOURCE_NAME := AM335xNxpNfcRdLib_Linux_v05.22.00_Extended
NXP_LIB_SOURCE_DIR := $(MODULE_SOURCE_DIR)/../Build/$(NXP_LIB_SOURCE_NAME)
NXP_LIB_DIR := $(NXP_LIB_SOURCE_DIR)/lib

NXP_LIB := $(NXP_LIB_DIR)/libMxCLRC663NXPLibWrapper.a
NXP_LIB += $(NXP_LIB_DIR)/libphOsal.a 
NXP_LIB += $(NXP_LIB_DIR)/libDAL.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_acDiscLoop.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_halHw.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_KeyStore.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_phTools.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palFelica.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI18092mPI.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4a.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palMifare.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alFelica.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alMful.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alMfdf.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alT1T.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alTop.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alMfc.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palSli15693.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI18000p3m3.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alICode.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alI18000p3m3.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alICode.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palEpcUid.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_lnLlcp.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4mC.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI18092mT.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_ceT4T.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palEpcUid.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_phNfcLib.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_acDiscLoop.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alICode.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palSli15693.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alI18000p3m3.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI18000p3m3.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p3a.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p3b.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_halHw.a 
NXP_LIB += $(NXP_LIB_DIR)/libphOsal.a
NXP_LIB += $(NXP_LIB_DIR)/libDAL.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI18092mPI.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4a.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alMfdf.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alT1T.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alTop.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alFelica.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palFelica.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alMful.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palMifare.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_CryptoSym.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_CryptoRng.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_alMfc.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_KeyStore.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palEpcUid.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_lnLlcp.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_phTools.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI18092mT.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_ceT4T.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_palI14443p4mC.a 
NXP_LIB += $(NXP_LIB_DIR)/libNxpRdLib_npSnep.a 
NXP_LIB += $(NXP_LIB_DIR)/*.a
##############################################
# INCLUDE DIRECTORY PATH (SOURCE + LIBRARY)
##############################################
INCLUDE_DIRS := $(MODULE_SOURCE_DIR) 
INCLUDE_DIRS += $(ACCESS_POLICIES_SOURCE_DIR)
INCLUDE_DIRS += $(ACMS_SOURCE_DIR)
INCLUDE_DIRS += $(CONFIGURATION_SOURCE_DIR)
INCLUDE_DIRS += $(BLUETOOTH_SOURCE_DIR)
INCLUDE_DIRS += $(DC_TASK_SOURCE_DIR)
INCLUDE_DIRS += $(DOOR_COMM_TASK_SOURCE_DIR)
INCLUDE_DIRS += $(ENROLLMENT_SOURCE_DIR)
INCLUDE_DIRS += $(EVENT_LOG_SOURCE_DIR)
INCLUDE_DIRS += $(FEATURES_SOURCE_DIR)
INCLUDE_DIRS += $(INTERFACES_SOURCE_DIR)
INCLUDE_DIRS += $(READER_SOURCE_DIR)
INCLUDE_DIRS += $(TEMP_SOURCE_DIR)
INCLUDE_DIRS += $(SYS_TEST_SOURCE_DIR)
INCLUDE_DIRS += $(SYS_TIME_SOURCE_DIR)
INCLUDE_DIRS += $(SYS_TIMER_SOURCE_DIR)
INCLUDE_DIRS += $(WEB_SERVER_SOURCE_DIR)
INCLUDE_DIRS += $(WEB_API_SOURCE_DIR)
INCLUDE_DIRS += $(AESCRYPT_LIB_PATH)

INCLUDE_DIRS += $(APP_COMMON_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_ACCESS_POLICIES_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_ACMS_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_CONFIGURATION_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_EVENT_LOG_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_FEATURES_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_INTERFACES_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_READER_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_TEMP_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_SYS_TIME_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_SYS_TIMER_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_WEB_SERVER_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_SESSION_MANAGEMENT_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_WEB_API_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_DC_TASK_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_DOOR_COMM_TASK_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_PUSH_API_SOURCE_DIR)
INCLUDE_DIRS += $(COMMON_SYSLOG_DIR)
INCLUDE_DIRS += $(COMMON_HTTP_FTC_DIR)
INCLUDE_DIRS += $(COMMON_FR_HANDLER_DIR)
INCLUDE_DIRS += $(BLUEZ_SOURCE_DIR)
INCLUDE_DIRS += $(BLUETOPIA_SOURCE_DIR)

INCLUDE_DIRS += $(GPIO_DRIVER_SOURCE_DIR)
INCLUDE_DIRS += $(WIEGAND_DRIVER_SOURCE_DIR)

INCLUDE_DIRS += $(LIB_XML_INCLUDE_DIR)

INCLUDE_DIRS += $(OPENSSL_INCLUDE_PATH)
INCLUDE_DIRS += $(CURL_INCLUDE_PATH)
INCLUDE_DIRS += $(ZLIB_INCLUDE_PATH)

INCLUDE_DIRS += $(COMMON_INCLUDE_DIR)
INCLUDE_DIRS += $(COMMON_MULTI_LANG_DIR)

INCLUDE_DIRS += $(BLUETOOTH_INCLUDE_PATH)/include
INCLUDE_DIRS += $(BLUETOOTH_INCLUDE_PATH)/Bluetopia/include
INCLUDE_DIRS += $(BLUETOOTH_INCLUDE_PATH)/Bluetopia/btpskrnl/threaded
INCLUDE_DIRS += $(BLUETOOTH_INCLUDE_PATH)/Bluetopia/btpscfg
INCLUDE_DIRS += $(BLUETOOTH_INCLUDE_PATH)/btpmerr
INCLUDE_DIRS += $(BLUETOOTH_INCLUDE_PATH)/include/client
INCLUDE_DIRS += $(BLUETOOTH_INCLUDE_PATH)/include/server
INCLUDE_DIRS += $(BLUETOOTH_INCLUDE_PATH)/btpmvs/wl18xx
INCLUDE_DIRS += $(NXP_LIB_SOURCE_DIR)/Wrapper_Files/src/
export INCLUDE_DIRS

##############################################
# LIBRARY DIRECTORY PATH
##############################################
LIBRARY_DIRS := $(HOST_LIB_DIR)
LIBRARY_DIRS += $(FS_LIB_DIR)

##############################################
# Libraries needed by application
##############################################
APPL_LIBS_NEEDED := -lgobject-2.0 -lpthread -lrt  -lxml2 -lSecureDongle -ljpeg -ldl -lm
APPL_LIBS_NEEDED += $(BT_ALL_LIBS)
APPL_LIBS_NEEDED += $(FS_LIB_DIR)/libPasswd.a
APPL_LIBS_NEEDED += $(AESCRYPT_LIB_PATH)/libmaescrypt.a
APPL_LIBS_NEEDED += $(NXP_LIB)
##############################################
# COMPILER & LINKER RELATED VARIABLES
##############################################
COMPILE_ONLY_FLAG := -c
INCLUDE_FLAG := -I
LIB_DIR_FLAG := -L
CC_OBJ_OUTPUT = -o $@
CFLAGS := -include MxDeviceSpecific.h -Wall -pthread -Wunreachable-code -W -Wextra $(DEBUG_FLAG)
CFLAGS += $(INCLUDE_DIRS:%=$(INCLUDE_FLAG)%) $(APP_C_FLAGS)
CFLAGS += -DBLUETOPIA_STACK_SUPPORTED
ifeq ($(BLUEZ_STACK_SUPPORTED), 1)
CFLAGS += -DBLUEZ_STACK_SUPPORTED=$(BLUEZ_STACK_SUPPORTED)
endif
# ifeq ($(ARMLINUX), on)
CFLAGS += -DARMLINUX -DOPENSSL_USE_DEPRECATED -DCLRC_SUPPORT_ENABLE
# endif

LDFLAGS += -o $(MODULE_BIN_DIR)/$(MODULE_BIN_NAME)
LDFLAGS += -pthread -lupnp -ljpeg
LDFLAGS += $(CURL_INSTALL_DIR)/lib/libcurl.so
LDFLAGS += $(OPENSSL_INSTALL_DIR)/lib/libssl.so
LDFLAGS += $(OPENSSL_INSTALL_DIR)/lib/libcrypto.so
ifeq ($(DEBUG), on)
CFLAGS += -DGDB_DEBUG_ON
endif

ifdef QA_RELEASE
CFLAGS += '-DQA_RELEASE=$(QA_RELEASE)'
endif

##############################################
# Source / Object lists
##############################################
# Create object list for both CPP and C files
PROGRAM_C_SRCS := $(wildcard $(MODULE_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(ACCESS_POLICIES_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(ACMS_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(CONFIGURATION_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(BLUETOOTH_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(DC_TASK_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(DOOR_COMM_TASK_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(ENROLLMENT_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(EVENT_LOG_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(FEATURES_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(INTERFACES_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(READER_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(TEMP_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(SYS_TEST_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(SYS_TIME_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(SYS_TIMER_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(WEB_SERVER_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(WEB_API_SOURCE_DIR)/*.c)

PROGRAM_C_SRCS += $(wildcard $(APP_COMMON_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_ACCESS_POLICIES_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_ACMS_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_CONFIGURATION_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_EVENT_LOG_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_FEATURES_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_INTERFACES_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_READER_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_TEMP_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_SYS_TIME_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_SYS_TIMER_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_WEB_SERVER_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_SESSION_MANAGEMENT_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_WEB_API_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_DC_TASK_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_DOOR_COMM_TASK_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_PUSH_API_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_HTTP_FTC_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_INCLUDE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_MULTI_LANG_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(COMMON_SYSLOG_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(BLUEZ_SOURCE_DIR)/*.c)
PROGRAM_C_SRCS += $(wildcard $(BLUETOPIA_SOURCE_DIR)/*.c)

MODULE_C_OBJ_LIST := $(foreach src, $(PROGRAM_C_SRCS),$(MODULE_OBJ_DIR)/$(notdir $(src:.c=$(OBJ_EXTENSION))))
MODULE_OBJ_LIST := $(MODULE_CPP_OBJ_LIST) $(MODULE_C_OBJ_LIST)

##############################################
# GENERATING DEPENDENCIES
##############################################
MAKEDEPEND = set -e; $(CC) -P -M -c $(CFLAGS) $< | \
		sed '1s=^=$@ $(PROJ_OBJ_DIR)/$(*D)/=' > $@; \
		[ -s $@ ] || rm -f $@

##############################################
# CREATING DEPENDENCIES FOR CPP / C FILES
##############################################
$(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION) : $(PROGRAM_C_SRCS)
	$(_@_) [ -d $(@D) ] || mkdir -p $(@D)
	$(_@_) $(MAKEDEPEND)

##############################################
# COMPILING CPP / C FILES
##############################################
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(MODULE_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(ACCESS_POLICIES_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(ACMS_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(CONFIGURATION_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(BLUETOOTH_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)	
		
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(DC_TASK_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(DOOR_COMM_TASK_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(ENROLLMENT_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(EVENT_LOG_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(FEATURES_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(INTERFACES_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(READER_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(TEMP_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(SYS_TEST_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(SYS_TIME_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(SYS_TIMER_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(WEB_SERVER_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(WEB_API_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(APP_COMMON_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_ACCESS_POLICIES_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_ACMS_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_CONFIGURATION_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_EVENT_LOG_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_FEATURES_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_INTERFACES_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_READER_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_TEMP_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_SYS_TIME_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_SYS_TIMER_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_WEB_SERVER_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_SESSION_MANAGEMENT_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_WEB_API_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_DC_TASK_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_DOOR_COMM_TASK_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_PUSH_API_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)	


$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_HTTP_FTC_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)	
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_INCLUDE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)	
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_MULTI_LANG_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)	

$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(COMMON_SYSLOG_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)	
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(BLUEZ_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)
	
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(BLUETOPIA_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)	

##############################################
# TARGETS
##############################################
# if no goal (target) was specified then assume "all"
.PHONY: default
default: all


.PHONY: clean
clean:
	$(_@_) echo Cleaning all object and binary files
	$(_@_) rm -fR $(MODULE_OBJ_DIR)/*
	$(_@_) rm -fR $(MODULE_BIN_DIR)/*


all :$(MODULE_OBJ_LIST)
	$(_@_) mkdir -m777 -p $(MODULE_BIN_DIR)
	$(_@_) $(LINK) $(LDFLAGS) $(MODULE_OBJ_LIST) $(LIBRARY_DIRS:%=$(LIB_DIR_FLAG)%) $(APPL_LIBS_NEEDED)
	$(_@_) echo -e "\033[1m\E[33;42mApplication \"$(MODULE_BIN_DIR)/$(MODULE_BIN_NAME)\" created successfully!  \033[0m"
	$(_@_) echo -e "\033[1m\E[33;44m`ls -go $(MODULE_BIN_DIR)/$(MODULE_BIN_NAME)`\033[0m"
