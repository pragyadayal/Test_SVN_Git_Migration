##################################################################################################################
# NOTE: If kernel is being compiled first time then may need to do 'make clean' it separately
# NOTE: If rootfs is being compiled first time then compile 'buildroot' and then 'rootfs'
##################################################################################################################
SHELL:=/bin/bash
YELLOW = \033[93m
GREEN = \033[92m
COLOR = \033[0m
YELLOW_BOLD = \033[1;93m
RED =\033[91m

include SystemAppsPath.mak

###############################################################################
# SVN CREDENTIALS
###############################################################################

# SVN Command with Credentials
SVN_CREDENTIALS = --username $(SVN_USERNAME) --password '$(SVN_PASSWORD)'

svnup: 
	svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(COMMON_INCLUDE_DIR)) $(COMMON_INCLUDE_DIR) || (svn cleanup $(COMMON_INCLUDE_DIR) && svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(COMMON_INCLUDE_DIR)) $(COMMON_INCLUDE_DIR))

	svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(GOAHEAD_PACKAGE_DIR)) $(GOAHEAD_PACKAGE_DIR) || (svn cleanup $(GOAHEAD_PACKAGE_DIR) && svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(GOAHEAD_PACKAGE_DIR)) $(GOAHEAD_PACKAGE_DIR))

	svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(PLATFORM_SRC_DIR)) $(PLATFORM_SRC_DIR) || (svn cleanup $(PLATFORM_SRC_DIR) && svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(PLATFORM_SRC_DIR)) $(PLATFORM_SRC_DIR))

	svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(PLATFORM_COMMON_SRC_DIR)) $(PLATFORM_COMMON_SRC_DIR) || (svn cleanup $(PLATFORM_COMMON_SRC_DIR) && svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(PLATFORM_COMMON_SRC_DIR)) $(PLATFORM_COMMON_SRC_DIR))

	svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(PLATFORM_RELEASE_PATH)) $(PLATFORM_RELEASE_PATH) || (svn cleanup $(PLATFORM_RELEASE_PATH) && svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(PLATFORM_RELEASE_PATH)) $(PLATFORM_RELEASE_PATH))

	svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(AESCRYPT_BIN_SOURCE_PATH)) $(AESCRYPT_BIN_SOURCE_PATH) || (svn cleanup $(AESCRYPT_BIN_SOURCE_PATH) && svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(AESCRYPT_BIN_SOURCE_PATH)) $(AESCRYPT_BIN_SOURCE_PATH))

	svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(AESCRYPT_LIB_PATH)) $(AESCRYPT_LIB_PATH) || (svn cleanup $(AESCRYPT_LIB_PATH) && svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(AESCRYPT_LIB_PATH)) $(AESCRYPT_LIB_PATH))

	svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(PRODUCT_PACKAGE_RELEASE_PATH)) $(PRODUCT_PACKAGE_RELEASE_PATH) || (svn cleanup $(PRODUCT_PACKAGE_RELEASE_PATH) && svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(PRODUCT_PACKAGE_RELEASE_PATH)) $(PRODUCT_PACKAGE_RELEASE_PATH))

	svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(NXP_LIB_SOURCE_PACKAGE_DIR)) $(NXP_LIB_SOURCE_PACKAGE_DIR) || (svn cleanup $(NXP_LIB_SOURCE_PACKAGE_DIR) && svn $(SVN_CREDENTIALS) co --force --quiet $(patsubst $(SVN_HOME_DIR)%,$(SVN_URL)%,$(NXP_LIB_SOURCE_PACKAGE_DIR)) $(NXP_LIB_SOURCE_PACKAGE_DIR))

	svn $(SVN_CREDENTIALS) revert --quiet $(MULTI_LANG_SCRIPT_DIR)/QtGui/dispstr.h
	svn $(SVN_CREDENTIALS) revert --quiet -R $(PRODUCT_PACKAGE_RELEASE_PATH)

# Make options
all	: evt_detect DemoControl web appl gui

package: all package_zip

nxplib:
	@rm -rf $(NXP_LIB_SOURCE_DIR)
	@cp -Rf $(LINUX_PACKAGE_DIR)/$(NXP_LIB_SOURCE_NAME) $(MAIN_SOURCE_DIR)/Build/
	@cd $(NXP_LIB_SOURCE_DIR)/
	@cp -Rf $(NXP_LIB_SOURCE_DIR)/Patch/* $(NXP_LIB_SOURCE_DIR)/
	@mkdir $(NXP_LIB_BUILD_DIR)
	@mkdir $(NXP_LIB_DIR)
	@cd $(NXP_LIB_BUILD_DIR)/ && cmake ../ -DTOOLCHAIN_PREFIX:STRING="$(CC_ARM_LINUX)" && make
	@find $(NXP_LIB_BUILD_DIR)/ -name *.a -exec cp {} $(NXP_LIB_DIR)/ \;

genMuliLang:
	@echo "=================================================================="
	@echo " Generating Multilangage Enums files "
	@echo "=================================================================="
	cd $(MULTI_LANG_SCRIPT_DIR) && ./xlstoc.sh ./xls/$(MULTI_LANG_FILE_NAME) ./xls/$(NON_MULTI_LANG_FILE_NAME) ./
	@echo "=================================================================="
	@echo " Multilangage Enums files Generated Successfully"
	@echo "=================================================================="	
	
aescrypt_util:
	@echo "Compile AES utilty" $(PARSE_STR)
	$(MAKE) -C $(AESCRYPT_BIN_SOURCE_PATH) CROSS_COMPILE= KEY_FLAG='$(KEYING)' cleanall encrypt_bin
	$(MAKE) -C $(AESCRYPT_BIN_SOURCE_PATH) CROSS_COMPILE=$(CC_ARM_LINUX) ARCH=$(MX_ARCH) KEY_FLAG='$(KEYING)' clean_decrypt decrypt_bin
	@echo "========================================================================"
	@echo " aescrypt utility compiled successfully"
	@echo "========================================================================"


package_zip: 
	rm -rf $(TEMP_RELEASE_DIR)
	rm -rf $(BUILD_DIR)/*.zip

	if [ -e $(PLATFORM_UPGRADE_ZIP_PATH) ]; then \
	mkdir -p $(TEMP_RELEASE_DIR) && cd $(TEMP_RELEASE_DIR) && unzip -o $(PLATFORM_UPGRADE_ZIP_PATH); \
	fi

	#make necessary directories
	mkdir -p $(TEMP_RELEASE_DIR)/matrix/appl
	mkdir -p $(TEMP_RELEASE_DIR)/matrix/web/bin
	mkdir -p $(TEMP_RELEASE_DIR)/matrix/web/html_pages
	mkdir -p $(TEMP_RELEASE_DIR)/matrix/fsFiles
	mkdir -p $(TEMP_RELEASE_DIR)/usr/lib
	
	#Copy necessary files for Firmware Zip
	cp -f $(DEMO_APP_PATH)/bin/ReadFile 					$(TEMP_RELEASE_DIR)/usr/lib/
	cp $(EVT_SOURCE_DIR)/bin/evtDetect 					$(TEMP_RELEASE_DIR)/matrix/appl/
	cp $(APP_SOURCE_DIR)/bin/$(MODULE_BIN_NAME) 				$(TEMP_RELEASE_DIR)/matrix/appl/
	cp $(QT_APPL_SOURCE_DIR)/$(GUI_APP_NAME)				$(TEMP_RELEASE_DIR)/matrix/appl/
	cp -rf $(QT_APPL_SOURCE_DIR)/qrc_ARGOResource.cpp 			$(TEMP_RELEASE_DIR)/matrix/appl/
	cp -Rf $(QT_APPL_SOURCE_DIR)/image 					$(TEMP_RELEASE_DIR)/matrix/appl/
	cp $(GOAHEAD_WEB_SOURCE_DIR)/build/linux-arm-default/bin/goahead 	$(TEMP_RELEASE_DIR)/matrix/web/bin/
	cp $(GOAHEAD_WEB_SOURCE_DIR)/build/linux-arm-default/bin/gopass 	$(TEMP_RELEASE_DIR)/matrix/web/bin/
	cp -rf $(WEB_SOURCE_DIR)/web/HtmlPages/*				$(TEMP_RELEASE_DIR)/matrix/web/html_pages/
	cp -rf $(WEB_COMMON_SOURCE_DIR)/web/HtmlPages/*				$(TEMP_RELEASE_DIR)/matrix/web/html_pages/
	cp $(GOAHEAD_WEB_SOURCE_DIR)/build/linux-arm-default/bin/libgo.so 	$(TEMP_RELEASE_DIR)/usr/lib
	cp $(QT_APPL_SOURCE_DIR)/startup_logo.png 				$(TEMP_RELEASE_DIR)/matrix/fsFiles/
	cp $(THIRD_PARTY_DIR)/SS1ToolBin/SS1Tool  				$(TEMP_RELEASE_DIR)/matrix/appl/
	
	#Copy string files
	cd $(MAIN_SOURCE_DIR)
	cp -f $(MULI_LANG_STRING_DIR)/Device_Language_PanelDoor.xls $(TEMP_RELEASE_DIR)/matrix/web/html_pages/MxSave_Default_String_Change.xls
	cp -f $(MULI_LANG_STRING_DIR)/Device_Language_PanelDoor.csv $(TEMP_RELEASE_DIR)/matrix/web/html_pages/MxSave_Default_String_Change.csv

	
	touch $(TEMP_RELEASE_DIR)/matrix/Door_Appl_DD
	echo $(VER_REV_STRING) >> $(TEMP_RELEASE_DIR)/matrix/Door_Appl_DD
	
	#creating upgrading firmware signature
	touch $(TEMP_RELEASE_DIR)/matrix/UpgradingFirmware
	echo  $(VER_REV_STRING) >> $(TEMP_RELEASE_DIR)/matrix/UpgradingFirmware
	
	# Copy upgrade.sh script which is common for all Doors
	cp -rf $(COMMON_FS_PATH)/matrix/scripts				$(TEMP_RELEASE_DIR)/matrix/
	# copy firmware decrypt binary
	cp $(AESCRYPT_BIN_SOURCE_PATH)/fwdecrypt 			$(TEMP_RELEASE_DIR)/matrix/appl/

	echo $(BM_1) >>  $(TEMP_RELEASE_DIR)/matrix/argo_verSign_$(shell printf '%02d' $(SW_VER))_$(shell printf '%02d' $(SW_REV))_$(shell printf '%02d' $(SW_MINOR_REV))
	
	chmod 775 -R $(TEMP_RELEASE_DIR)/matrix/*
	chmod 775 -R $(TEMP_RELEASE_DIR)/usr/*

	# Creating md5sum of each file
	cd $(TEMP_RELEASE_DIR) && find * -type f -exec md5sum {} + > ./md5sum.txt && find * -type l -exec md5sum {} + >> ./md5sum.txt
	chmod 755 $(TEMP_RELEASE_DIR)/md5sum.txt
	
	mkdir -p $(PACKAGE_REVISION_PATH)
	mkdir -p $(PACKAGE_PATH)
	
# Creating ZIP File
	cd $(TEMP_RELEASE_DIR)/ && zip -9 -r -y -q $(UNENCRYPTED_FIRMWARE_ZIP) etc matrix usr lib sbin bin md5sum.txt
	$(AESCRYPT_BIN_SOURCE_PATH)/fwencrypt $(TEMP_RELEASE_DIR)/$(UNENCRYPTED_FIRMWARE_ZIP) $(BUILD_DIR)/$(ENCRYPTED_FIRMWARE_ZIP)
	cp -f $(BUILD_DIR)/$(ENCRYPTED_FIRMWARE_ZIP) $(PACKAGE_PATH)/
	
	@echo "================================================================="
	@echo " Firmware ZIP is generated										"
	@echo "================================================================="

DemoControl:
	@echo "Building DemoControlAppl..."
	cd $(DEMO_APP_PATH) && $(MAKE) CROSS_COMPILE=$(CC_ARM_LINUX) clean
	cd $(DEMO_APP_PATH) && $(MAKE) CROSS_COMPILE=$(CC_ARM_LINUX) all
	cp $(DEMO_APP_PATH)/bin/ReadFile $(DEVICE_FS_CHANGES)/usr/lib/
	
evt_detect:
	cd $(EVT_SOURCE_DIR) && $(MAKE) CROSS_COMPILE=$(CC_ARM_LINUX) clean all
	cp $(EVT_SOURCE_DIR)/bin/evtDetect $(FS_MAIN_APPL_BIN_DIR)/

appl: nxplib genMuliLang aescrypt_util
	$(MAKE) -C $(APP_SOURCE_DIR) CROSS_COMPILE=$(CC_ARM_LINUX) APP_C_FLAGS=$(SW_VER_FLAGS) clean DEBUG=on all
	cp $(APP_SOURCE_DIR)/bin/$(MODULE_BIN_NAME) $(FS_MAIN_APPL_BIN_DIR)/

gui:
	cd $(QT_APPL_SOURCE_DIR) && $(BUILDROOT_OPT_DIR)/host/usr/bin/qmake -spec $(BUILDROOT_OPT_DIR)/host/usr/mkspecs/qws/linux-arm-g++
	cd $(QT_APPL_SOURCE_DIR) && $(MAKE) clean
	cd $(QT_APPL_SOURCE_DIR) && $(MAKE) all
	cp $(QT_APPL_SOURCE_DIR)/$(GUI_APP_NAME) $(FS_MAIN_APPL_BIN_DIR)/
	rm -rf $(FS_MAIN_APPL_BIN_DIR)/qrc_ARGOResource.cpp
	cp -rf $(QT_APPL_SOURCE_DIR)/qrc_ARGOResource.cpp $(FS_MAIN_APPL_BIN_DIR)/
	# Copy GUI related images into file system
	rm -rf $(FS_MAIN_APPL_BIN_DIR)/image
	cp -Rf $(QT_APPL_SOURCE_DIR)/image $(FS_MAIN_APPL_BIN_DIR)/

web:
	if [ ! -d $(GOAHEAD_WEB_SOURCE_DIR) ]; then \
		cd $(BUILD_DIR) && tar -xvf $(GOAHEAD_PACKAGE_DIR)/$(GOAHEAD_WEBSERVER_PACKAGE_NAME_WITH_EXT); \
	fi

	cp -rf $(GOAHEAD_WEB_PATCH_DIR)/* $(GOAHEAD_WEB_SOURCE_DIR)/
	$(MAKE) -C $(GOAHEAD_WEB_SOURCE_DIR) ARCH=arm OS=linux AR=$(CC_ARM_LINUX)ar CC=$(CC_ARM_LINUX)gcc LD=$(CC_ARM_LINUX)ld clean
	$(MAKE) -C $(GOAHEAD_WEB_SOURCE_DIR) DEBUG=debug ARCH=arm OS=linux AR=$(CC_ARM_LINUX)ar CC=$(CC_ARM_LINUX)gcc LD=$(CC_ARM_LINUX)ld  PROFILE=default SHOW=1 ME_COM_OPENSSL_INCLUDE=$(OPENSSL_INSTALL_DIR)/include ME_COM_OPENSSL_PATH=$(OPENSSL_INSTALL_DIR)/lib LDFLAGS="$(OPENSSL_INSTALL_DIR)/lib/libssl.so $(OPENSSL_INSTALL_DIR)/lib/libcrypto.so" IFLAGS=-I"./build/linux-arm-default/inc" IFLAGS+=-I"$(OPENSSL_INSTALL_DIR)/include" all

	cp $(GOAHEAD_WEB_SOURCE_DIR)/build/linux-arm-default/bin/goahead $(FS_WEB_BIN_DIR)/
	cp $(GOAHEAD_WEB_SOURCE_DIR)/build/linux-arm-default/bin/gopass $(FS_WEB_BIN_DIR)/

	cp -rf $(WEB_SOURCE_DIR)/web/HtmlPages/* $(FS_HTML_PAGES_DIR)/
	cp -rf $(WEB_COMMON_SOURCE_DIR)/web/HtmlPages/* $(FS_HTML_PAGES_DIR)/

platformBuildVariables:
	export KEY_FLAG='$(KEYING)';
	@echo "FIRMWARE_ZIP=$(PACKAGE_SVN_PATH)/$(ENCRYPTED_FIRMWARE_ZIP) RELEASE_VER_REV_FOLDER_PATH=$(PACKAGE_REVISION_PATH) SW_VER=$(SW_VER) SW_REV=$(SW_REV) SW_MINOR_REV=$(SW_MINOR_REV) BIN_FILES_FOLDER=$(BIN_FILES_FOLDER) PLATFORM_BUILD_DIR=$(PLATFORM_BUILD_DIR)"
##################################################################################################################
# End of makefile
##################################################################################################################
