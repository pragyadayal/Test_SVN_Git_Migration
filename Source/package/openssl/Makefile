
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

OPENSSL_PACKAGE_NAME ?= openssl-1.1.0g
OPENSSL_PACKAGE_NAME_WITH_EXT=$(OPENSSL_PACKAGE_NAME).tar.gz
OPENSSL_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/OpenSSL/$(OPENSSL_PACKAGE_NAME_WITH_EXT)
OPENSSL_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(OPENSSL_PACKAGE_NAME)
OPENSSL_INSTALL_DIR := $(OPENSSL_SOURCE_DIR)/install
OPENSSL_PATCH_DIR := $(PLATFORM_PACKAGE_PATH)/openssl/opensslPatch

all: source extract patch build install

source:
	$(call msg, Start Exporting openssl packages Source )
	if [ ! -d $(OPENSSL_SOURCE_DIR) ]; then \
		svn $(SVN_CREDENTIALS) export --force $(OPENSSL_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH);\
	fi
	$(call msg, Export complete )

	
	
extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(OPENSSL_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH); \
	
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	rsync -a --exclude=".svn" $(OPENSSL_PATCH_DIR)/ $(OPENSSL_SOURCE_DIR)/

	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	cd $(OPENSSL_SOURCE_DIR) && ./Configure linux-generic32 --prefix=$(OPENSSL_INSTALL_DIR) enable-deprecated no-async; \
	make -s -C $(OPENSSL_SOURCE_DIR) CC=$(C_COMPILER) OPEN_SSL_PATH=$(OPENSSL_SOURCE_DIR) all install; \
	$(STRIP) --strip-debug --strip-unneeded  		$(OPENSSL_INSTALL_DIR)/bin/openssl
	$(call msg, build is done )
	
install:
	mkdir -p $(MAIN_FILESYSTEM_PATH)/usr/bin/
	cp -Rf $(OPENSSL_INSTALL_DIR)/lib/libcrypto.so.1.1 	$(MAIN_FILESYSTEM_PATH)/usr/lib/
	cp -Rf $(OPENSSL_INSTALL_DIR)/lib/libssl.so.1.1 	$(MAIN_FILESYSTEM_PATH)/usr/lib/
	cp -Rf $(OPENSSL_INSTALL_DIR)/bin/openssl 			$(MAIN_FILESYSTEM_PATH)/usr/bin/
	
