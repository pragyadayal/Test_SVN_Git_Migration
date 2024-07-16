
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

SAMBA_CONF_ENV = \
	samba_cv_HAVE_GETTIMEOFDAY_TZ=yes \
	samba_cv_USE_SETREUID=yes \
	samba_cv_HAVE_KERNEL_OPLOCKS_LINUX=yes \
	samba_cv_HAVE_IFACE_IFCONF=yes \
	samba_cv_HAVE_MMAP=yes \
	samba_cv_HAVE_FCNTL_LOCK=yes \
	samba_cv_HAVE_SECURE_MKSTEMP=yes \
	samba_cv_CC_NEGATIVE_ENUM_VALUES=yes \
	samba_cv_fpie=no \
	libreplace_cv_HAVE_IPV6=$(if $(BR2_INET_IPV6),yes,no) \
	$(if $(BR2_PACKAGE_SAMBA_AVAHI),AVAHI_LIBS=-pthread)
	
SAMBA_CONF_OPT = \
	--localstatedir=/var \
	--with-piddir=/var/run \
	--with-lockdir=/var/lock \
	--with-logfilebase=/var/log \
	--with-configdir=/etc/samba \
	--with-privatedir=/etc/samba \
	\
	--disable-cups \
	--disable-static \
	--enable-shared \
	--enable-shared-libs \
	--disable-pie \
	--disable-relro \
	--disable-dnssd \
	\
	$(if $(BR2_PACKAGE_SAMBA_AVAHI),--enable-avahi,--disable-avahi) \
	$(if $(BR2_PACKAGE_SAMBA_GAMIN),--enable-fam,--disable-fam) \
	$(if $(BR2_PACKAGE_SAMBA_SWAT),--enable-swat,--disable-swat) \
	\
	--without-cluster-support \
	--without-cifsupcall \
	--without-ads \
	--without-ldap \
	--with-included-popt \
	--with-included-iniparser \
	--with-libiconv=$(ROOTFS_OUTPUT_TARGET_PATH) \
	\
	$(if $(BR2_PACKAGE_SAMBA_CIFS),--with-cifsmount,--without-cifsmount) \
	$(if $(BR2_PACKAGE_SAMBA_RPCCLIENT),--with-readline=$(ROOTFS_OUTPUT_TARGET_PATH)) \
	$(if $(BR2_PACKAGE_SAMBA_SMBCLIENT),--with-readline=$(ROOTFS_OUTPUT_TARGET_PATH)) \
	$(if $(BR2_PACKAGE_SAMBA_WINBINDD),--with-winbind,--without-winbind)


	
SAMBA_PACKAGE_NAME=samba-3.3.14
SAMBA_PACKAGE_NAME_WITH_EXT=$(SAMBA_PACKAGE_NAME).tar.gz
SAMBA_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/SambaClient/$(SAMBA_PACKAGE_NAME)/
SAMB_BUILD_PATH=$(LOCAL_PACKAGES_BUILD_PATH)/samba
SAMBA_SOURCE_DIR := $(SAMB_BUILD_PATH)/$(SAMBA_PACKAGE_NAME)
SAMBA_INSTALL_DIR := $(IPWATCHD_SOURCE_DIR)/install


PATCH = samba-add-check-for-__use_bsd.patch \
	samba-CVE-2011-0719.patch \
	samba-do-not-check-glibc-version.patch \
	samba-fix-client-mtab.patch \
	samba-fix-mount.cifs.patch \
	samba-getgrouplist.patch \
	samba-remove-legacy-index.patch

CONFIGURE := ./configure --prefix=$(ROOTFS_OUTPUT_TARGET_PATH)/usr/ --exec-prefix=$(ROOTFS_OUTPUT_TARGET_PATH)/usr/ --target=arm-linux --host=$(HOST_ARM) CC=$(C_COMPILER)\
	$(SAMBA_CONF_ENV) \
	$(SAMBA_CONF_OPT) \
	CFLAGS=-I$(ROOTFS_OUTPUT_TARGET_PATH)/usr/include/   LDFLAGS=-L$(ROOTFS_OUTPUT_TARGET_PATH)/usr/lib \
	-with-libiconv=$(ROOTFS_OUTPUT_TARGET_PATH)
	
default: all

all: source extract patch configure build install

source:
	$(call msg, Start Exporting $(SAMBA_PACKAGE_NAME) packages Source )
	if [ ! -d $(SAMBA_SOURCE_DIR) ]; then \
		svn export --force $(SAMBA_SOURCE_PACKAGE_PATH) $(SAMB_BUILD_PATH)/;\
	fi;
		
	$(call msg, Export $(SAMBA_PACKAGE_NAME) complete )
	
extract:
	$(call msg,  Extracting $(SAMBA_PACKAGE_NAME) package )
	tar -xvf $(SAMB_BUILD_PATH)/$(SAMBA_PACKAGE_NAME_WITH_EXT) -C $(SAMB_BUILD_PATH)/;
	$(call msg,  Extract $(SAMBA_PACKAGE_NAME) complete )
	
patch: 
	$(call msg,   Applying patch to $(SAMBA_PACKAGE_NAME))
	cd $(SAMBA_SOURCE_DIR)/ && patch -p1 < $(SAMB_BUILD_PATH)/samba-add-check-for-__use_bsd.patch
	
	cd $(SAMBA_SOURCE_DIR)/ && patch -p1 < $(SAMB_BUILD_PATH)/samba-CVE-2011-0719.patch

	cd $(SAMBA_SOURCE_DIR)/ && patch -p1 < $(SAMB_BUILD_PATH)/samba-do-not-check-glibc-version.patch
	
	cd $(SAMBA_SOURCE_DIR)/ && patch -p1 < $(SAMB_BUILD_PATH)/samba-fix-client-mtab.patch

	cd $(SAMBA_SOURCE_DIR)/ && patch -p1 < $(SAMB_BUILD_PATH)/samba-fix-mount.cifs.patch

	cd $(SAMBA_SOURCE_DIR)/ && patch -p1 < $(SAMB_BUILD_PATH)/samba-getgrouplist.patch

	cd $(SAMBA_SOURCE_DIR)/ && patch -p1 < $(SAMB_BUILD_PATH)/samba-remove-legacy-index.patch
	
	$(call msg,   patch is applied to $(SAMBA_PACKAGE_NAME))
	
# 	$(foreach FILE, $(PATCH), $(call exec, /home/rahulpatel/svn/Products/Matrix_IPCamera_Netra5000/SDT/V2/Platform/5MP/Trunk/Source/Rootfs/Third_Party_Modules/samba/$(FILE)))\

configure:
	$(call msg,   configuring the $(SAMBA_PACKAGE_NAME))
	
	cd $(SAMBA_SOURCE_DIR)/source && $(CONFIGURE)
	
	$(call msg,   configured $(SAMBA_PACKAGE_NAME))
	
build:
	$(call msg,  $(SAMBA_PACKAGE_NAME) building  )
	
	make -C $(SAMBA_SOURCE_DIR)/source
	
	$(call msg, $(SAMBA_PACKAGE_NAME) build is done )
	
install:
	$(STRIP) $(SAMBA_SOURCE_DIR)/source/bin/nmbd
	$(STRIP) $(SAMBA_SOURCE_DIR)/source/bin/smbd
	$(STRIP) $(SAMBA_SOURCE_DIR)/source/bin/libtalloc.so.1
	$(STRIP) $(SAMBA_SOURCE_DIR)/source/bin/libtdb.so.1
	cp -f $(SAMBA_SOURCE_DIR)/source/bin/nmbd $(ROOTFS_OUTPUT_TARGET_PATH)/usr/sbin/
	cp -f $(SAMBA_SOURCE_DIR)/source/bin/smbd $(ROOTFS_OUTPUT_TARGET_PATH)/usr/sbin/
	cp -f $(SAMBA_SOURCE_DIR)/source/bin/libtalloc.so.1 $(ROOTFS_OUTPUT_TARGET_PATH)/usr/lib/
	cd  $(ROOTFS_OUTPUT_TARGET_PATH)/usr/lib/ && ln -sf libtalloc.so.1 libtalloc.so
	cp -f $(SAMBA_SOURCE_DIR)/source/bin/libtdb.so.1 $(ROOTFS_OUTPUT_TARGET_PATH)/usr/lib/
	cd $(ROOTFS_OUTPUT_TARGET_PATH)/usr/lib/ && ln -sf libtdb.so.1 libtdb.so
clean:
	rm -rf $(SAMBA_SOURCE_DIR)
