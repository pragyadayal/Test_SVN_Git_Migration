
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif


#CATDOC_PACKAGE_NAME=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/catdoc
CATDOC_PACKAGE_NAME=catdoc-0.95
CATDOC_PACKAGE_NAME_WITH_EXT=catdoc-0.95.tar.gz
CATDOC_SOURCE_PACKAGE_PATH=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/catdoc/$(CATDOC_PACKAGE_NAME_WITH_EXT)
COMMON_SVN_CATDOC_CHANGES=$(COMMON_SVN_CONFIG_CHANGES)/$(SVN_CATDOC_PACKAGE_DIR_NAME)
CATDOC_SOURCE_DIR=$(LOCAL_PACKAGES_BUILD_PATH)/$(CATDOC_PACKAGE_NAME)




all: source extract patch build install

source:
	$(call msg, Start Exporting curl packages Source )
		if [ ! -d $(CATDOC_SOURCE_DIR) ]; then \
		svn export --force $(CATDOC_SOURCE_PACKAGE_PATH) $(LOCAL_PACKAGES_BUILD_PATH);\
	fi
	$(call msg, Export complete )

	
	
extract:
	$(call msg,  Extracting package )
	tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(CATDOC_PACKAGE_NAME_WITH_EXT) -C $(LOCAL_PACKAGES_BUILD_PATH);
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )	
	#	NOTE:- To compile catdoc-0.95.tar.gz no need to save old configure script
	# Changes for Date system for xls2csv
	#cp -rf $(COMMON_SVN_CATDOC_CHANGES)/* $(CATDOC_SOURCE_DIR)/
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	cd $(CATDOC_SOURCE_DIR) && CC="$(C_COMPILER)" ./configure --host --disable-langinfo --with-input=8859-1 --with-output=8859-1 --disable-wordview --prefix=/usr --with-install-root=$(CATDOC_SOURCE_DIR)
	cd $(CATDOC_SOURCE_DIR) && make clean
	cd $(CATDOC_SOURCE_DIR) && make CC=$(C_COMPILER)
	cd $(CATDOC_SOURCE_DIR) && make CC=$(C_COMPILER) install
	
	$(call msg, build is done )
	
install:

	cp $(CATDOC_SOURCE_DIR)/usr/bin/xls2csv				$(MAIN_FILESYSTEM_PATH)/usr/bin/
	cp -rf $(CATDOC_SOURCE_DIR)/usr/share/catdoc		$(MAIN_FILESYSTEM_PATH)/usr/share/
	
	
