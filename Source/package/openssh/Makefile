
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

ZLIB_PACKAGE_NAME := zlib-1.2.11
ZLIB_PACKAGE_NAME_WITH_EXT=$(ZLIB_PACKAGE_NAME).tar.gz
ZLIB_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/zlib/$(ZLIB_PACKAGE_NAME_WITH_EXT)
ZLIB_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(ZLIB_PACKAGE_NAME)
ZLIB_INSTALL_DIR := $(ZLIB_SOURCE_DIR)/install/

OPENSSL_PACKAGE_NAME ?= openssl-1.1.0g
OPENSSL_PACKAGE_NAME_WITH_EXT=$(OPENSSL_PACKAGE_NAME).tar.gz
OPENSSL_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/OpenSSL/$(OPENSSL_PACKAGE_NAME_WITH_EXT)
OPENSSL_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(OPENSSL_PACKAGE_NAME)
OPENSSL_INSTALL_DIR := $(OPENSSL_SOURCE_DIR)/install

OPENSSH_PACKAGE_NAME ?= openssh-8.0p1
OPENSSH_PACKAGE_NAME_WITH_EXT=$(OPENSSH_PACKAGE_NAME).tar.gz
OPENSSH_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/Secure_Remote_Login/$(OPENSSH_PACKAGE_NAME_WITH_EXT)
OPENSSH_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(OPENSSH_PACKAGE_NAME)
OPENSSH_INSTALL_DIR := $(OPENSSH_SOURCE_DIR)/install

PAM_PACKAGE_NAME := Linux-PAM-1.3.0
PAM_SOURCE_DIR := $(LOCAL_PACKAGES_BUILD_PATH)/$(PAM_PACKAGE_NAME)
PAM_INSTALL_DIR := $(PAM_SOURCE_DIR)/install

all: source extract patch build install

source:
	$(call msg, Start Exporting openssh packages Source )
		if [ ! -d $(OPENSSH_SOURCE_DIR) ]; then \
		svn $(SVN_CREDENTIALS) export --force $(OPENSSH_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH);\
	fi
	$(call msg, Export complete )

	
	
extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(OPENSSH_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH); \
	
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	cd $(OPENSSH_SOURCE_DIR) && ./configure CC=$(C_COMPILER) --host=arm-linux-gnueabihf --prefix=$(OPENSSH_INSTALL_DIR) \
		--with-zlib=$(ZLIB_INSTALL_DIR) --with-ssl-dir=$(OPENSSL_INSTALL_DIR) --disable-strip --with-privsep-user=nobody \
		--with-pam CFLAGS=-I$(PAM_INSTALL_DIR)/usr/include/ LDFLAGS="-L$(PAM_INSTALL_DIR)/lib/" LIBS="-Llpam"; \
		make -s -C $(OPENSSH_SOURCE_DIR) all install-nokeys; \

	$(call msg, build is done )
	
install:
	mkdir -p $(MAIN_FILESYSTEM_PATH)/bin
	mkdir -p $(MAIN_FILESYSTEM_PATH)/sbin
	mkdir -p $(MAIN_FILESYSTEM_PATH)/usr/libexec

	$(STRIP) --strip-debug --strip-unneeded $(OPENSSH_INSTALL_DIR)/bin/ssh-keygen
	cp -Rf $(OPENSSH_INSTALL_DIR)/bin/ssh-keygen $(MAIN_FILESYSTEM_PATH)/bin/

	if [ -d $(OPENSSH_INSTALL_DIR)/libexec ]; then \
		$(STRIP) --strip-debug --strip-unneeded $(OPENSSH_INSTALL_DIR)/libexec/sftp-server ; \
		cp -Rf $(OPENSSH_INSTALL_DIR)/libexec/sftp-server $(MAIN_FILESYSTEM_PATH)/usr/libexec/; \
	else \
		$(STRIP) --strip-debug --strip-unneeded $(OPENSSH_INSTALL_DIR)/lib/sftp-server ; \
		cp -Rf $(OPENSSH_INSTALL_DIR)/lib/sftp-server $(MAIN_FILESYSTEM_PATH)/usr/libexec/; \
	fi; \
	$(STRIP) --strip-debug --strip-unneeded $(OPENSSH_INSTALL_DIR)/sbin/sshd
	$(STRIP) --strip-debug --strip-unneeded $(OPENSSH_INSTALL_DIR)/bin/scp
	cp -Rf $(OPENSSH_INSTALL_DIR)/sbin/sshd $(MAIN_FILESYSTEM_PATH)/sbin/
	cp -Rf $(OPENSSH_INSTALL_DIR)/bin/scp 	$(MAIN_FILESYSTEM_PATH)/bin/
	
