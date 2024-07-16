
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

ZLIB_PACKAGE_NAME := zlib-1.2.11
ZLIB_PACKAGE_NAME_WITH_EXT=$(ZLIB_PACKAGE_NAME).tar.xz
ZLIB_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/zlib/$(ZLIB_PACKAGE_NAME_WITH_EXT)
ZLIB_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(ZLIB_PACKAGE_NAME)
ZLIB_INSTALL_DIR := $(ZLIB_SOURCE_DIR)/install/

OPENSSL_PACKAGE_NAME ?= openssl-1.1.0g
OPENSSL_PACKAGE_NAME_WITH_EXT=$(OPENSSL_PACKAGE_NAME).tar.gz
OPENSSL_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/openssl/$(OPENSSL_PACKAGE_NAME_WITH_EXT)
OPENSSL_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(OPENSSL_PACKAGE_NAME)
OPENSSL_INSTALL_DIR := $(OPENSSL_SOURCE_DIR)/install

CURL_PACKAGE_NAME := curl-7.50.1
CURL_PACKAGE_NAME_WITH_EXT=$(CURL_PACKAGE_NAME).tar.bz2
CURL_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/Curl/$(CURL_PACKAGE_NAME_WITH_EXT)
CURL_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(CURL_PACKAGE_NAME)
CURL_INSTALL_DIR := $(CURL_SOURCE_DIR)/install

all: source extract patch build install

source:
	$(call msg, Start Exporting curl packages Source )
	if [ ! -d $(CURL_SOURCE_DIR) ]; then \
		svn $(SVN_CREDENTIALS) export --force $(CURL_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH);\
	fi
		
	$(call msg, Export complete )

	
	
extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(CURL_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH)
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	cd $(CURL_SOURCE_DIR) && ./configure --target=arm-linux-gnueabihf --host=arm-linux-gnueabihf CC=$(C_COMPILER) --prefix=$(CURL_INSTALL_DIR) --enable-shared=yes --enable-static=yes --sysconfdir=/etc --localstatedir=/var --program-prefix= --disable-gtk-doc --disable-gtk-doc-html --disable-doc --disable-docs --disable-documentation --disable-dependency-tracking --disable-telnet --enable-smtp --enable-pop3 --enable-imap --enable-ftp --disable-tftp --enable-ipv6 --enable-static --enable-shared --disable-verbose --disable-manual --disable-ntlm-wb --enable-hidden-symbols --with-random=/dev/urandom --disable-curldebug --with-ssl=$(OPENSSL_INSTALL_DIR) --with-ca-path=$(OPENSSL_INSTALL_DIR)/ssl/certs --disable-ares --without-libssh2 --with-zlib=$(ZLIB_INSTALL_DIR) --with-ssl=$(OPENSSL_INSTALL_DIR)
	make -s -C $(CURL_SOURCE_DIR) all install
	$(STRIP) --strip-debug --strip-unneeded  $(CURL_INSTALL_DIR)/bin/curl
	$(call msg, build is done )
	
install:
	cp -Rf $(CURL_INSTALL_DIR)/bin/curl 		$(MAIN_FILESYSTEM_PATH)/usr/bin/
	cp -Rf $(CURL_INSTALL_DIR)/lib/libcurl.so 	$(MAIN_FILESYSTEM_PATH)/usr/lib/
	cp -Rf $(CURL_INSTALL_DIR)/lib/libcurl.so.4 	$(MAIN_FILESYSTEM_PATH)/usr/lib/
	cp -Rf $(CURL_INSTALL_DIR)/lib/libcurl.so.4.4.0 $(MAIN_FILESYSTEM_PATH)/usr/lib/
	
