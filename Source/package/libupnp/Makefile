
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

LIBUPNP_PACKAGE_NAME := libupnp-1.6.20
LIBUPNP_PACKAGE_NAME_WITH_EXT= $(LIBUPNP_PACKAGE_NAME).tar.bz2
LIBUPNP_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/libupnp/$(LIBUPNP_PACKAGE_NAME_WITH_EXT)
#LIBUPNP_SOURCE_PACKAGE_PATH=/opt/dl
LIBUPNP_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(LIBUPNP_PACKAGE_NAME)
LIBUPNP_INSTALL_DIR := $(LIBUPNP_SOURCE_DIR)/install

all: source extract patch build install

source:
	if [ ! -d $(LIBUPNP_SOURCE_DIR) ]; then \
		svn export --force $(LIBUPNP_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH); \
	fi;
extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(LIBUPNP_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH);
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	$(call msg,    patch Applied )
	
build:
	$(call msg,  building  )
	cd $(LIBUPNP_SOURCE_DIR) && ./configure  CC=$(C_COMPILER) --prefix=$(LIBUPNP_INSTALL_DIR) STRIP=$(STRIP) --host=arm-linux --enable-ipv6;
	make -C $(LIBUPNP_SOURCE_DIR) all install
	$(call msg, build is done )	
	
install:

	cp -Rf $(LIBUPNP_INSTALL_DIR)/lib/libixml.so.2.0.8 			$(MAIN_FILESYSTEM_PATH)/usr/lib/
	cp -Rf $(LIBUPNP_INSTALL_DIR)/lib/libthreadutil.so.6.0.4 	$(MAIN_FILESYSTEM_PATH)/usr/lib/
	cp -Rf $(LIBUPNP_INSTALL_DIR)/lib/libupnp.so.6.3.4 			$(MAIN_FILESYSTEM_PATH)/usr/lib/
	
 
