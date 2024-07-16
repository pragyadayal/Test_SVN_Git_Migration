
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PLATFORM_PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif


UTILS_PACKAGE_DIR=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/COSEC_DEVICES/Utils
COMMON_SVN_XLSLIB_CHANGES=$(PLATFORM_CSV2XLS_PACKAGE_PATH)/patch_work
LIBCSV_PACKAGE_DIR=$(MATRIX_EMBEDDED_SOFTWARE_MODULE)/libcsv

XLSLIB_PACKAGE_NAME_WITH_EXT=xlslib-package-2.5.0.zip
LIBCSV_PACKAGE_NAME_WITH_EXT=libcsv-master.zip

SVN_CSV2XLS_PACKAGE_DIR_NAME=csv2xls-0.3.5
CSV2XLS_PACKAGE_NAME_WITH_EXT=csv2xls-0.3.5.tar.gz
SVN_LIBCSV_DIR_NAME=libcsv-master
SVN_XLSLIB_DIR_NAME=xlslib

XLSLIB_SOURCE_DIR=$(LOCAL_PACKAGES_BUILD_PATH)/$(SVN_XLSLIB_DIR_NAME)/$(SVN_XLSLIB_DIR_NAME)
LIBCSV_SOURCE_DIR=$(LOCAL_PACKAGES_BUILD_PATH)/$(SVN_LIBCSV_DIR_NAME)
CSV2XLS_SOURCE_DIR=$(LOCAL_PACKAGES_BUILD_PATH)/$(SVN_CSV2XLS_PACKAGE_DIR_NAME)
	
all: source extract patch build install

source:
	$(call msg, Start Exporting curl packages Source )
	
	# Libraries for csv2xls utility
	# (1) xlslib
	if [ ! -d $(XLSLIB_SOURCE_DIR) ]; then \
		svn export --force $(UTILS_PACKAGE_DIR)/$(XLSLIB_PACKAGE_NAME_WITH_EXT) $(LOCAL_PACKAGES_BUILD_PATH);\
	fi
	
	# (2) libcsv
	
	if [ ! -d $(LIBCSV_SOURCE_DIR) ]; then \
		svn export --force $(LIBCSV_PACKAGE_DIR) $(LOCAL_PACKAGES_BUILD_PATH);\
	fi
	
	# (3) csv2xls
	if [ ! -d $(CSV2XLS_SOURCE_DIR) ]; then \
		svn export --force $(UTILS_PACKAGE_DIR)/$(CSV2XLS_PACKAGE_NAME_WITH_EXT) $(LOCAL_PACKAGES_BUILD_PATH);\
	fi
	$(call msg, Export complete )

	
	
extract:
	$(call msg,  Extracting package )
	cd $(LOCAL_PACKAGES_BUILD_PATH) && unzip -o $(LOCAL_PACKAGES_BUILD_PATH)/$(XLSLIB_PACKAGE_NAME_WITH_EXT)
	cp -rf $(COMMON_SVN_XLSLIB_CHANGES)/* $(XLSLIB_SOURCE_DIR)/
	cd $(LOCAL_PACKAGES_BUILD_PATH) && unzip -o $(LOCAL_PACKAGES_BUILD_PATH)/$(LIBCSV_PACKAGE_NAME_WITH_EXT)
	cd $(LOCAL_PACKAGES_BUILD_PATH) && tar -xvf $(LOCAL_PACKAGES_BUILD_PATH)/$(CSV2XLS_PACKAGE_NAME_WITH_EXT)
	$(call msg,  Extract complete )

patch:
	$(call msg,   Applying patch )
	cp -rf $(COMMON_SVN_XLSLIB_CHANGES)/* $(XLSLIB_SOURCE_DIR)/;
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	chmod 777 $(XLSLIB_SOURCE_DIR)/bootstrap
	cd $(XLSLIB_SOURCE_DIR) && ./bootstrap
	cd $(XLSLIB_SOURCE_DIR) && aclocal
	cd $(XLSLIB_SOURCE_DIR) && CC=$(C_COMPILER) CXX=$(CPP_COMPILER) ./configure --prefix=$(MAIN_FILESYSTEM_PATH)/usr/ --host=$(HOST_ARM)
	cd $(XLSLIB_SOURCE_DIR) && make clean
	cd $(XLSLIB_SOURCE_DIR) && make CC=$(CPP_COMPILER) CXX=$(CPP_COMPILER) 
	#cd $(XLSLIB_SOURCE_DIR) && make CC=$(CPP_COMPILER) CXX=$(CPP_COMPILER) install
	
	@echo "========================================================================"
	@echo " xlslib library compiled successfully."
	@echo "========================================================================"
	
	chmod 777 $(LIBCSV_SOURCE_DIR)/autogen.sh
	cd $(LIBCSV_SOURCE_DIR) && aclocal
	cd $(LIBCSV_SOURCE_DIR) && ./autogen.sh
	cd $(LIBCSV_SOURCE_DIR) && CC=$(C_COMPILER) ./configure --prefix=$(MAIN_FILESYSTEM_PATH)/usr/ --host=$(HOST_ARM)
	cd $(LIBCSV_SOURCE_DIR) && make clean
	cd $(LIBCSV_SOURCE_DIR) && make CC=$(C_COMPILER)
	cd $(LIBCSV_SOURCE_DIR) && make CC=$(C_COMPILER) install
	
	@echo "========================================================================"
	@echo " libcsv library compiled successfully."
	@echo "========================================================================"
	
	chmod 777 $(CSV2XLS_SOURCE_DIR)/bootstrap.sh
	cd $(CSV2XLS_SOURCE_DIR) && ./bootstrap.sh
	cd $(CSV2XLS_SOURCE_DIR) && CC=$(C_COMPILER) CXX=$(CPP_COMPILER) ./configure --prefix=$(MAIN_FILESYSTEM_PATH)/usr/ --host=$(HOST_ARM)
	cd $(CSV2XLS_SOURCE_DIR) && make clean
	cd $(CSV2XLS_SOURCE_DIR) && make CC=$(C_COMPILER) CXX=$(CPP_COMPILER)
	#cd $(CSV2XLS_SOURCE_DIR) && make CC=$(C_COMPILER) install

	@echo "========================================================================"
	@echo " CSV2XLS library compiled successfully."
	@echo "========================================================================"
	
	$(call msg, build is done )
	
install:
	# Move required file into file system folder
	cp $(XLSLIB_SOURCE_DIR)/src/.libs/libxls.so.3.0.0 	$(MAIN_FILESYSTEM_PATH)/usr/lib/
	
	# Move required file into file system folder
	cp $(LIBCSV_SOURCE_DIR)/.libs/libcsv.so.3.0.2 	$(MAIN_FILESYSTEM_PATH)/usr/lib/
	
	# Move required file into file system folder
	cp $(CSV2XLS_SOURCE_DIR)/src/csv2xls		$(MAIN_FILESYSTEM_PATH)/usr/bin/
	
	
	
