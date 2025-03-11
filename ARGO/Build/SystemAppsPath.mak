###############################################################################
###                         Common Paths for System                         ###
###############################################################################
SHELL:=/bin/bash
ARCH_BIT=$(shell getconf LONG_BIT)

#----------------------------------------------------------------------#
# Device firmware version revision string
#----------------------------------------------------------------------#
# To pass minor rev to c code we have keep value of MINOR_VER_SUPPORTED=1
# To not to pass minor ver then keep MINOR_VER_SUPPORTED=0
MINOR_VER_SUPPORTED=1
# Device firmware version revision
SW_VER=1

SW_REV=42
# Minor Revision
SW_MINOR_REV=1
# First SW_REV with minor ver support which can handle encrypted firmware upgrade but itself is not encrypted
SW_REV_WITH_MINOR_VER_SUPPORT=19
VER_REV_STRING=V$(shell printf '%02d' $(SW_VER))R$(shell printf '%02d' $(SW_REV))

# APP_BUILD_TYPE : QA_RELEASE or PRODUCTION_RELEASE
APP_BUILD_TYPE ?= QA_RELEASE
ifeq ($(APP_BUILD_TYPE), QA_RELEASE)
TRK_BRANCH_NAME=$(shell cd ../../.. && basename `pwd`)
LAST_CHANGE_SVN_REV=$(shell svn info --show-item last-changed-revision $(shell cd ../.. && pwd))
QA_RELEASE="$(TRK_BRANCH_NAME)_rev$(LAST_CHANGE_SVN_REV)"
endif

#Include benchmark files here with same format as below
#BM_1:="BM=V01R11.00"
BM_1:=""

FINAL_FIRMWARE=1
SW_VER_FLAGS := "-DSW_VER=$(SW_VER) -DSW_REV=$(SW_REV) -DSW_MINOR_REV=$(SW_MINOR_REV)"

COSEC_VER_REV_STR=V16R6
#----------------------------------------------------------------------#

#----------------------------------------------------------------------#
# Compiler options
#----------------------------------------------------------------------#
MX_ARCH=arm
#----------------------------------------------------------------------#
# Base Parameters
#----------------------------------------------------------------------#
SVN_URL=svn://192.168.100.5
PRODUCT_FOLDER_NAME = ARGO
OPT_FOLDER_NAME = am335x_CosecARGO
PRODUCT_REL_FOLDER_NAME = ARGO
PRODUCT_ZIP_NAME=ARGO_DOOR
MODULE_BIN_NAME = argo
QT_GUI_DIR_NAME=QtGui
GUI_APP_NAME=ARGOGui
ACCESS_CONTROL_MAIN_DIR = $(SVN_HOME_DIR)/Products/AccessControlSystem
DEVICE_MAIN_DIR = $(ACCESS_CONTROL_MAIN_DIR)/SDT/Device
#----------------------------------------------------------------------#

#----------------------------------------------------------------------#
# OPT BUILDROOT Path
#----------------------------------------------------------------------#
BUILDROOT_OPT_DIR=/opt/$(OPT_FOLDER_NAME)
BUILDROOT_TARGET_DIR=$(BUILDROOT_OPT_DIR)/target
BUILDROOT_DL_DIR=/opt/dl

ifeq ($(ARCH_BIT), 64)
CC_ARM_LINUX=$(BUILDROOT_OPT_DIR)/host/usr/bin/arm-linux-gnueabihf-
HOST_NAME=$(BUILDROOT_OPT_DIR)/host/usr/bin/arm-linux-gnueabihf
else
CC_ARM_LINUX=$(BUILDROOT_OPT_DIR)/host/usr/bin/arm-buildroot-linux-gnueabihf-
HOST_NAME=$(BUILDROOT_OPT_DIR)/host/usr/bin/arm-buildroot-linux-gnueabihf
endif
#----------------------------------------------------------------------#

#----------------------------------------------------------------------#
# Application Base Path
#----------------------------------------------------------------------#
MAIN_SOURCE_DIR=$(patsubst %/,%,$(dir $(shell pwd)))
DEVICE_SOURCE_DIR=$(MAIN_SOURCE_DIR)
COMMON_SOURCE_DIR=$(MAIN_SOURCE_DIR)/../Common_Source/Linux_Devices_Common_Source
#----------------------------------------------------------------------#

#----------------------------------------------------------------------#
# Other Applications Path
#----------------------------------------------------------------------#
COMMON_INCLUDE_DIR=$(DEVICE_MAIN_DIR)/Common_Include/Branches/Releases/V20R9.1
EVT_SOURCE_DIR=$(MAIN_SOURCE_DIR)/EvtDetect
DEMO_APP_PATH=$(MAIN_SOURCE_DIR)/DemoControlAppl
AESCRYPT_BIN_SOURCE_PATH=$(DEVICE_MAIN_DIR)/Utility/FirmwareEncryption
AESCRYPT_LIB_PATH=$(SVN_HOME_DIR)/Briefcase/SWD_Briefcase/EmbeddedSoftwareModules/AESCrypt

LINUX_PACKAGE_DIR=$(SVN_HOME_DIR)/Briefcase/SWD_Briefcase/EmbeddedSoftwareModules
NXP_LIB_SOURCE_NAME := AM335xNxpNfcRdLib_Linux_v05.22.00_Extended
NXP_LIB_SOURCE_DIR := $(MAIN_SOURCE_DIR)/Build/$(NXP_LIB_SOURCE_NAME)
NXP_LIB_BUILD_DIR := $(NXP_LIB_SOURCE_DIR)/build
NXP_LIB_DIR := $(NXP_LIB_SOURCE_DIR)/lib
NXP_LIB_SOURCE_PACKAGE_DIR=$(LINUX_PACKAGE_DIR)/$(NXP_LIB_SOURCE_NAME)
THIRD_PARTY_DIR=$(COMMON_SOURCE_DIR)/Thirdparty

#----------------------------------------------------------------------#

#----------------------------------------------------------------------#
# Application Source Path
#----------------------------------------------------------------------#
APP_SOURCE_DIR=$(DEVICE_SOURCE_DIR)/Application
APP_COMMON_SOURCE_DIR=$(COMMON_SOURCE_DIR)/Application
WEB_SOURCE_DIR=$(MAIN_SOURCE_DIR)/WebServer
WEB_COMMON_SOURCE_DIR=$(COMMON_SOURCE_DIR)/WebServer
CGI_BIN_SOURCE_DIR=$(WEB_COMMON_SOURCE_DIR)/web/cgi-bin
QT_APPL_SOURCE_DIR=$(MAIN_SOURCE_DIR)/$(QT_GUI_DIR_NAME)
#----------------------------------------------------------------------#

#----------------------------------------------------------------------#
# Untar directory for packages
#----------------------------------------------------------------------#
GOAHEAD_WEBSERVER_NAME=goahead-3.6.5
GOAHEAD_WEBSERVER_PACKAGE_NAME_WITH_EXT=$(GOAHEAD_WEBSERVER_NAME)-src.tgz
#----------------------------------------------------------------------#

#----------------------------------------------------------------------#
# Source folder directory for compilation
#----------------------------------------------------------------------#
GOAHEAD_PACKAGE_DIR=$(SVN_HOME_DIR)/Briefcase/SWD_Briefcase/EmbeddedSoftwareModules/goahead
#----------------------------------------------------------------------#

#----------------------------------------------------------------------#
# Multilanguage string file directory
#----------------------------------------------------------------------#
MULTI_LANG_FILE_NAME=Device_Language_PanelDoor.xls
NON_MULTI_LANG_FILE_NAME=Non_MultiLanguage_String_Changes.xls
MULTI_LANG_SCRIPT_DIR=$(COMMON_INCLUDE_DIR)/CommonMultiLanguage
COMMON_MULTI_LANG_DIR=$(COMMON_INCLUDE_DIR)/CommonMultiLanguage/Application
MULI_LANG_STRING_DIR=$(MULTI_LANG_SCRIPT_DIR)/xls
#----------------------------------------------------------------------#
# Platform Source path
#----------------------------------------------------------------------#
PLATFORM_RELEASE_VERSION="2.0.1"
#----------------------------------------------------------------------#
# Platform Source path
#----------------------------------------------------------------------#
PLATFORM_SRC_MAIN_DIR=$(DEVICE_MAIN_DIR)/Platform/Branches/Releases/$(PLATFORM_RELEASE_VERSION)/Source
PLATFORM_SRC_DIR=$(PLATFORM_SRC_MAIN_DIR)/$(PRODUCT_FOLDER_NAME)
PLATFORM_BUILD_DIR=$(PLATFORM_SRC_DIR)/Build
DEVICE_PATCH_DIR=$(PLATFORM_SRC_DIR)/Config
PLATFORM_COMMON_SRC_DIR=$(PLATFORM_SRC_MAIN_DIR)/Common
COMMON_PATCH_DIR=$(PLATFORM_COMMON_SRC_DIR)/Config
COMMON_FS_PATH=$(COMMON_PATCH_DIR)/file_system
FS_PATH=$(DEVICE_PATCH_DIR)/file_system
FS_MAIN_APPL_BIN_DIR=$(FS_PATH)/matrix/appl
FS_WEB_BIN_DIR=$(FS_PATH)/matrix/web/bin
FS_HTML_PAGES_DIR=$(FS_PATH)/matrix/web/html_pages
#----------------------------------------------------------------------#

#----------------------------------------------------------------------#
# Platform Release path
#----------------------------------------------------------------------#
PLATFORM_RELEASE_PATH=$(DEVICE_MAIN_DIR)/Platform/Software_Release/$(PRODUCT_FOLDER_NAME)/$(PLATFORM_RELEASE_VERSION)
DRIVER_PATH=$(PLATFORM_RELEASE_PATH)/Drivers
BLUETOOTH_INCLUDE_PATH=$(PLATFORM_RELEASE_PATH)/BLE/AM335xBluetopiaLinuxProduction-4.2.1.0.1.0/BluetopiaPM
DEVICE_FS_CHANGES=$(DEVICE_PATCH_DIR)/file_system
GOAHEAD_WEB_PATCH_DIR=$(COMMON_PATCH_DIR)/$(GOAHEAD_WEBSERVER_NAME)
FS_LIB_DIR=$(DEVICE_FS_CHANGES)/usr/lib
HOST_LIB_DIR=$(BUILDROOT_OPT_DIR)/host/usr
LIB_XML_INCLUDE_DIR=$(HOST_LIB_DIR)/include/libxml2
KERNEL_PATH=$(PLATFORM_RELEASE_PATH)/Kernel
MLO_PATH=$(PLATFORM_RELEASE_PATH)/MLO
UBOOT_PATH=$(PLATFORM_RELEASE_PATH)/U-boot
OPENSSL_INSTALL_DIR=$(PLATFORM_RELEASE_PATH)/Openssl/openssl-1.1.0g/install
CURL_INSTALL_DIR=$(PLATFORM_RELEASE_PATH)/Curl/curl-7.50.1/install
ZLIB_INSTALL_DIR=$(PLATFORM_RELEASE_PATH)/Zlib/zlib-1.2.11/install
PLATFORM_UPGRADE_ZIP_DIR_NAME=Platform_upgradezip
PLATFORM_UPGRADE_ZIP_NAME=COSEC_$(PRODUCT_ZIP_NAME)_platform.zip
PLATFORM_UPGRADE_ZIP_PATH=$(PLATFORM_RELEASE_PATH)/$(PLATFORM_UPGRADE_ZIP_DIR_NAME)/$(PLATFORM_UPGRADE_ZIP_NAME)
#----------------------------------------------------------------------#

#----------------------------------------------------------------------#
# Build related path 
#----------------------------------------------------------------------#
BUILD_DIR=$(DEVICE_SOURCE_DIR)/Build
ifeq ($(APP_BUILD_TYPE),QA_RELEASE)
RELEASE_FOLDER_NAME=QA_Release
BIN_FILES_FOLDER=/$(TRK_BRANCH_NAME)/Prod_Bin_Files
else
RELEASE_FOLDER_NAME=Production_Release
BIN_FILES_FOLDER=/Prod_Bin_Files
endif
PACKAGE_RELEASE_PATH=$(DEVICE_MAIN_DIR)/Software_Release/DoorController/$(RELEASE_FOLDER_NAME)
PRODUCT_PACKAGE_RELEASE_PATH=$(PACKAGE_RELEASE_PATH)/$(PRODUCT_REL_FOLDER_NAME)
ifeq ($(SW_MINOR_REV),0)
PACKAGE_REVISION_PATH=$(PACKAGE_RELEASE_PATH)/$(PRODUCT_REL_FOLDER_NAME)/V$(SW_VER)/R$(SW_REV)
PACKAGE_REVISION_SVN_PATH=$(SVN_URL)/Products/AccessControlSystem/SDT/Device/Software_Release/DoorController/$(RELEASE_FOLDER_NAME)/$(PRODUCT_REL_FOLDER_NAME)/V$(SW_VER)/R$(SW_REV)
else
PACKAGE_REVISION_PATH=$(PACKAGE_RELEASE_PATH)/$(PRODUCT_REL_FOLDER_NAME)/V$(SW_VER)/R$(SW_REV).$(SW_MINOR_REV)
PACKAGE_REVISION_SVN_PATH=$(SVN_URL)/Products/AccessControlSystem/SDT/Device/Software_Release/DoorController/$(RELEASE_FOLDER_NAME)/$(PRODUCT_REL_FOLDER_NAME)/V$(SW_VER)/R$(SW_REV).$(SW_MINOR_REV)
endif
PACKAGE_PATH=$(PACKAGE_REVISION_PATH)$(BIN_FILES_FOLDER)
PACKAGE_SVN_PATH=$(PACKAGE_REVISION_SVN_PATH)$(BIN_FILES_FOLDER)
GOAHEAD_WEB_SOURCE_DIR=$(BUILD_DIR)/$(GOAHEAD_WEBSERVER_NAME)
TEMP_RELEASE_DIR=$(BUILD_DIR)/temp_fs
#----------------------------------------------------------------------#

#----------------------------------------------------------------------#
#V1R6.1
UNENCRYPTED_FIRMWARE_ZIP=UNENCRYPT_COSEC_$(PRODUCT_ZIP_NAME)_$(VER_REV_STRING).$(shell printf '%02d' $(SW_MINOR_REV)).zip
#V1R6.1
ENCRYPTED_FIRMWARE_ZIP=COSEC_$(PRODUCT_ZIP_NAME)_$(VER_REV_STRING).$(shell printf '%02d' $(SW_MINOR_REV)).zip
#V1R6
FIRMWARE_ZIP=COSEC_$(PRODUCT_ZIP_NAME)_$(VER_REV_STRING).zip
#----------------------------------------------------------------------#

#----------------------------------------------------------------------#
#key derivation for firmware encyrption and decryption
#----------------------------------------------------------------------#
PARSE_STR:=`grep "FIRMWARE_ENCRYPTION_KEY" $(APP_SOURCE_DIR)/MxDeviceSpecific.h | awk '{print $$3}'`
KEYING:='-DKEY=$(PARSE_STR)'
#----------------------------------------------------------------------#
