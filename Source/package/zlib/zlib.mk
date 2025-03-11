ZLIB_PACKAGE_NAME := zlib-1.2.11
ZLIB_PACKAGE_NAME_WITH_EXT=$(ZLIB_PACKAGE_NAME).tar.gz
ZLIB_SOURCE_PACKAGE_PATH=$(SVN_SERVER_101)/Third_Party_Modules/zlib/$(ZLIB_PACKAGE_NAME_WITH_EXT)
ZLIB_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(ZLIB_PACKAGE_NAME)
ZLIB_INSTALL_DIR := $(ZLIB_SOURCE_DIR)/install/

all: source extract patch build install

source:
	$(call msg, Start Exporting ble packages Source )
	if [ ! -d $(ZLIB_SOURCE_DIR) ]; then \
		svn export --force $(ZLIB_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH);\
	fi
	$(call msg, Export complete )

	
	
extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(ZLIB_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH); \
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	
	cd $(ZLIB_SOURCE_DIR) && CHOST=$(HOST_TAG) ./configure --prefix=$(ZLIB_INSTALL_DIR); \
	cd $(ZLIB_SOURCE_DIR) && make all; \
	cd $(ZLIB_SOURCE_DIR) && make install; \

	$(call msg, build is done )
	
install:
	cp -Rf $(ZLIB_INSTALL_DIR)/lib/libz.so 		$(MAIN_FILESYSTEM_PATH)/usr/lib/
	cp -Rf $(ZLIB_INSTALL_DIR)/lib/libz.so.1 	$(MAIN_FILESYSTEM_PATH)/usr/lib/
	cp -Rf $(ZLIB_INSTALL_DIR)/lib/libz.so.1.2.11 	$(MAIN_FILESYSTEM_PATH)/usr/lib/
