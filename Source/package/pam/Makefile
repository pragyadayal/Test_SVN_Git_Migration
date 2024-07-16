
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

PAM_PACKAGE_NAME := Linux-PAM-1.3.0
PAM_PACKAGE_NAME_WITH_EXT=$(PAM_PACKAGE_NAME).tar.bz2
PAM_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/Secure_Remote_Login/$(PAM_PACKAGE_NAME_WITH_EXT)
PAM_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(PAM_PACKAGE_NAME)
PAM_INSTALL_DIR := $(PAM_SOURCE_DIR)/install

all: source extract patch build install

source:
	$(call msg, Start Exporting libpam packages Source )
		if [ ! -d $(PAM_SOURCE_DIR) ]; then \
		svn export --force $(PAM_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH);\
	fi
	$(call msg, Export complete )

extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(PAM_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH)
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	cd $(PAM_SOURCE_DIR) && ./configure CC=$(C_COMPILER) LD=$(LINKER) --disable-nis --host=arm-linux-gnueabihf; \
		make && make install DESTDIR=$(PAM_INSTALL_DIR)
	$(call msg, build is done )
	
install:
	mkdir -p $(MAIN_FILESYSTEM_PATH)/etc
	mkdir -p $(MAIN_FILESYSTEM_PATH)/lib
	mkdir -p $(MAIN_FILESYSTEM_PATH)/usr
	cp -rf $(PAM_INSTALL_DIR)/etc/security 		$(MAIN_FILESYSTEM_PATH)/etc/
	cp -rf $(PAM_INSTALL_DIR)/lib/* 			$(MAIN_FILESYSTEM_PATH)/lib/
	cp -rf $(PAM_INSTALL_DIR)/usr/share/locale 	$(MAIN_FILESYSTEM_PATH)/usr/share/
	rm -rf $(MAIN_FILESYSTEM_PATH)/lib/security/*.la
	rm -rf $(MAIN_FILESYSTEM_PATH)/lib/*.la 
	rm -rf $(PAM_INSTALL_DIR)/lib/security/*.la
	rm -rf $(PAM_INSTALL_DIR)/lib/*.la 
	echo "========================================================================="
	echo "========================================================================="
	echo "========================================================================="
	
 
