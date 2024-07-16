
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

LIBICONV_PACKAGE_NAME := libiconv-1.12
LIBICONV_PACKAGE_NAME_WITH_EXT= $(LIBICONV_PACKAGE_NAME).tar.gz
LIBICONV_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/libiconv/$(LIBICONV_PACKAGE_NAME)/$(LIBICONV_PACKAGE_NAME_WITH_EXT)
LIBICONV_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(LIBICONV_PACKAGE_NAME)
LIBICONV_INSTALL_DIR := $(LIBICONV_SOURCE_DIR)/install

all: source extract patch configure build install

source:
	if [ ! -d $(LIBICONV_SOURCE_DIR) ]; then \
		svn $(SVN_CREDENTIALS) export --force $(LIBICONV_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH); \
	fi;
extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(LIBICONV_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH);
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch to $(LIBICONV_PACKAGE_NAME))
	$(call msg,   patch is applied to $(LIBICONV_PACKAGE_NAME))
	
configure:
	$(call msg,   configuring $(LIBICONV_PACKAGE_NAME))
	cd $(LIBICONV_SOURCE_DIR) && ./configure --target=$(HOST_ARM) --host=$(HOST_ARM) CC=$(C_COMPILER) --prefix=$(LIBICONV_INSTALL_DIR) --exec-prefix=$(LIBICONV_INSTALL_DIR) --enable-extra-encodings
	$(call msg,   configured $(LIBICONV_PACKAGE_NAME))

build:
	$(call msg,  $(LIBICONV_PACKAGE_NAME) building  )
	make -C $(LIBICONV_SOURCE_DIR) all
	$(call msg, $(LIBICONV_PACKAGE_NAME) build is done )
	
install:
	mkdir -p $(LIBICONV_INSTALL_DIR)
	make -C $(LIBICONV_SOURCE_DIR) install
	cp -rf $(LIBICONV_INSTALL_DIR)/lib $(ROOTFS_OUTPUT_TARGET_PATH)/usr/lib
	cp -rf $(LIBICONV_INSTALL_DIR)/bin $(ROOTFS_OUTPUT_TARGET_PATH)/usr/bin
	rm -rf $(ROOTFS_OUTPUT_TARGET_PATH)/usr/lib/preloadable_libiconv.so

