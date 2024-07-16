
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

BLUEZ_PACKAGE_NAME := bluez-5.58
BLUEZ_PACKAGE_NAME_WITH_EXT=$(BLUEZ_PACKAGE_NAME).tar.xz
BLUEZ_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/Bluez/$(BLUEZ_PACKAGE_NAME_WITH_EXT)
BLUEZ_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(BLUEZ_PACKAGE_NAME)
BLUEZ_INSTALL_DIR := $(BLUEZ_SOURCE_DIR)/install



all: source extract patch build install
source:
	$(call msg, Start Exporting bluez packages Source )
	if [ ! -d $(BLUEZ_SOURCE_DIR) ]; then \
		svn $(SVN_CREDENTIALS) export --force $(BLUEZ_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH);\
	fi
		
	$(call msg, Export complete )

extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(BLUEZ_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH)
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	
	cp -Rf $(SVN_BLUEZ_CONFIG_CHANGES)/$(BLUEZ_PACKAGE_NAME)/* $(BLUEZ_SOURCE_DIR)/
	
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	
	cd $(BLUEZ_SOURCE_DIR) && $(HOST_USR_DIR_PATH)/bin/libtoolize --force 
	cd $(BLUEZ_SOURCE_DIR) && $(HOST_USR_DIR_PATH)/bin/aclocal 
	cd $(BLUEZ_SOURCE_DIR) && $(HOST_USR_DIR_PATH)/bin/autoheader  
	cd $(BLUEZ_SOURCE_DIR) && $(HOST_USR_DIR_PATH)/bin/automake --force-missing --add-missing 
	cd $(BLUEZ_SOURCE_DIR) && $(HOST_USR_DIR_PATH)/bin/autoconf 
	cd $(BLUEZ_SOURCE_DIR) &&  ./configure --target=$(HOST_ARM) --host=$(HOST_ARM) --build=x86_64-pc-linux-gnu CC=$(C_COMPILER) PKG_CONFIG="$(HOST_USR_DIR_PATH)/bin/pkg-config" --prefix=$(BLUEZ_INSTALL_DIR)/usr --exec-prefix=$(BLUEZ_INSTALL_DIR)/usr --sysconfdir=$(BLUEZ_INSTALL_DIR)/etc/ --localstatedir=$(BLUEZ_INSTALL_DIR)/var --with-dbusconfdir=$(BLUEZ_INSTALL_DIR)/etc/  --with-udevdir=$(BLUEZ_INSTALL_DIR)/lib/ --program-prefix="" --enable-experimental  --disable-dependency-tracking  --disable-static --enable-shared  --enable-tools --enable-library  --disable-cups   --enable-udev --disable-systemd	--enable-deprecated --enable-sixaxis
	make  -C $(BLUEZ_SOURCE_DIR) 
	make  -C $(BLUEZ_SOURCE_DIR) install
	
	$(call msg, build is done )
	
install:
	$(call msg,  INSTALL )
	cp -Rf $(BLUEZ_INSTALL_DIR)/usr/bin/* $(MAIN_FILESYSTEM_PATH)/usr/sbin/
	cp -Rdf $(BLUEZ_INSTALL_DIR)/usr/lib/libbluetooth.so* $(MAIN_FILESYSTEM_PATH)/usr/lib/
	mkdir -p $(MAIN_FILESYSTEM_PATH)/usr/lib/bluetooth/plugins
	cp -Rf $(BLUEZ_INSTALL_DIR)/usr/lib/bluetooth/plugins/sixaxis.so $(MAIN_FILESYSTEM_PATH)/usr/lib/bluetooth/plugins
	cp -Rf $(BLUEZ_INSTALL_DIR)/etc/* $(MAIN_FILESYSTEM_PATH)/etc/
	cp -Rf $(BLUEZ_INSTALL_DIR)/usr/libexec/bluetooth/bluetoothd $(MAIN_FILESYSTEM_PATH)/usr/sbin/
	cp -Rf $(BLUEZ_INSTALL_DIR)/../src/org.bluez.service  $(BUILDROOT_TARGET_DIR)/usr/share/dbus-1/system-services/
	mkdir -p $(MAIN_FILESYSTEM_PATH)/lib/firmware/qca
	cp -Rf $(SVN_BLUEZ_CONFIG_CHANGES)/firmware/* $(MAIN_FILESYSTEM_PATH)/lib/firmware/qca/
	$(call msg,  INSTALL DONE  )
	
