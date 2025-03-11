
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

OPEN_ISCSI_PACKAGE_NAME := open-iscsi-2.1.4
OPEN_ISCSI_PACKAGE_NAME_WITH_EXT=$(OPEN_ISCSI_PACKAGE_NAME).zip
OPEN_ISCSI_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/Openiscsi/$(OPEN_ISCSI_PACKAGE_NAME_WITH_EXT)
OPEN_ISCSI_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(OPEN_ISCSI_PACKAGE_NAME)
OPEN_ISCSI_INSTALL_DIR := $(OPEN_ISCSI_SOURCE_DIR)/install
OPEN_ISCSI_PATCH_DIR := $(PLATFORM_PACKAGE_PATH)/open-iscsi/patch-2.1.4

#depended package build
#OPEN_ISNS_PACKAGE_NAME=open-isns-0.101
OPEN_ISNS_PACKAGE_NAME_WITH_EXT=$(OPEN_ISNS_PACKAGE_NAME).tar.gz
OPEN_ISNS_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/Openiscsi/$(OPEN_ISNS_PACKAGE_NAME_WITH_EXT)
OPEN_ISNS_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(OPEN_ISNS_PACKAGE_NAME)
OPEN_ISNS_INSTALL_DIR := $(OPEN_ISNS_SOURCE_DIR)/install


all: source extract patch build install

source:
	$(call msg, Start Exporting packages Source )
	if [ ! -d $(OPEN_ISCSI_SOURCE_DIR) ]; then \
		mkdir -p $(LOCAL_PACKAGES_BUILD_PATH)/$(OPEN_ISCSI_PACKAGE_NAME) ; \
		cd $(LOCAL_PACKAGES_BUILD_PATH)/$(OPEN_ISCSI_PACKAGE_NAME)/ && svn $(SVN_CREDENTIALS) export --force $(OPEN_ISCSI_SOURCE_PACKAGE_PATH) ./ ;\
		svn $(SVN_CREDENTIALS) export --force $(OPEN_ISNS_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH);\
	fi
	$(call msg, Export complete )

	
	
extract:
	$(call msg,  Extracting package )
	cd $(LOCAL_PACKAGES_BUILD_PATH)/ && unzip $(LOCAL_PACKAGES_BUILD_PATH)/$(OPEN_ISCSI_PACKAGE_NAME)/$(OPEN_ISCSI_PACKAGE_NAME_WITH_EXT); \
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(OPEN_ISNS_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH); \
	
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	
	rsync -a --exclude=".svn" $(OPEN_ISCSI_PATCH_DIR)/ $(OPEN_ISCSI_SOURCE_DIR)/
	
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	export PATH=$(PATH):$(LOCAL_PACKAGES_BUILD_PATH)/
	cd $(OPEN_ISNS_SOURCE_DIR) && ./configure --target=aarch64-buildroot-linux-gnu --host=arm --build=x86_64-pc-linux-gnu --prefix=$(OPEN_ISNS_INSTALL_DIR)
	cd $(OPEN_ISNS_SOURCE_DIR) && make CC=$(CC)
	cd $(OPEN_ISNS_SOURCE_DIR) && make install_hdrs
	cd $(OPEN_ISNS_SOURCE_DIR) && make install_lib
	
	cd $(OPEN_ISCSI_SOURCE_DIR) && make CC=$(CC) NO_SYSTEMD=1 all; \
	make CC=$(CC) NO_SYSTEMD=1 all; \
	make install DESTDIR=$(OPEN_ISCSI_INSTALL_DIR)
	$(call msg, build is done )
	
install:
	$(call msg,  installing  )
	mkdir -p $(MAIN_FILESYSTEM_PATH)/usr/bin/
	cp -rf $(OPEN_ISCSI_INSTALL_DIR)/etc/iscsi $(MAIN_FILESYSTEM_PATH)/etc/
	cp -rf $(OPEN_ISCSI_INSTALL_DIR)/sbin/* $(MAIN_FILESYSTEM_PATH)/sbin/
	cp -rf $(OPEN_ISCSI_INSTALL_DIR)/usr/lib64/* $(MAIN_FILESYSTEM_PATH)/usr/lib64/
	$(call msg,  install done  )
	
