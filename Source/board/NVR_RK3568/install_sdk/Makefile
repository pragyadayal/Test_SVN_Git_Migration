
ifeq ($(PATH_VARIABLE_FILE),)
$(error Please pass a valid config file as PATH_VARIABLE_FILE=variable)
else
include $(PATH_VARIABLE_FILE)
endif

all: source extract patch install

source:
	$(call msg,  Checking Out SDK )
	@if [ ! -f $(SDK_SOURCE_PATH)/.stamp_source ]; then \
	rm -rf $(SDK_SOURCE_PATH)/; \
	svn --force export $(SVN_RK3568_SDK_PATH)/$(SDK_VERSION) $(SDK_SOURCE_PATH)/; \
	touch $(SDK_SOURCE_PATH)/.stamp_source ; \
	fi
	
	$(call msg,  Checking Out SDK complete)
	
extract:

	$(call msg,  SDK Extract start )
	@if [ ! -f $(SDK_SOURCE_PATH)/.stamp_extract ]; then \
	tar -xvf $(SDK_SOURCE_PATH)/$(SDK_PACKAGE_NAME) --exclude="*/.repo" -C $(SDK_SOURCE_PATH)/ ; \
	cd $(LOCAL_SDK_PATH) && chmod 777 ./build_emmc.sh && ./build_emmc.sh env ; \
	touch $(SDK_SOURCE_PATH)/.stamp_extract ; \
	fi
	$(call msg,  SDK Extract complete )

	#tar -xvf $(SDK_SOURCE_PATH)/$(SDK_FULL_PACKAGE_NAME) --exclude="*/.repo/*" -C $(SDK_SOURCE_PATH)/ ; \

patch:
	$(call msg,  SDK patch start )
	
	@if [ ! -f $(SDK_SOURCE_PATH)/.stamp_patch ]; then \
	cp -drf $(SDK_MATRIX_PATCH_PATH)/* $(LOCAL_SDK_PATH)/; \
	# RKMPI release patch \
	cd $(SDK_SOURCE_PATH)/patch && tar -xvf $(SDK_RKMPI_RELEASE_PACKAGE_NAME); \
	cd $(EXTRA_APPL_DEP_LIBRARY_PATH)/ && cp -rf include 							$(EXTRA_TOOL_CHAIN_LIBRARY_PATH)/../include/rockit; \
	cd $(EXTRA_APPL_DEP_LIBRARY_PATH)/ && cp -drf lib64/librockit.so				$(EXTRA_TOOL_CHAIN_LIBRARY_PATH); \
	cd $(EXTRA_APPL_DEP_LIBRARY_PATH)/ && cp -drf deps/lib/libasound.so.2			$(EXTRA_TOOL_CHAIN_LIBRARY_PATH)/libasound.so.2.0.0; \
	cd $(EXTRA_APPL_DEP_LIBRARY_PATH)/ && cp -drf deps/lib/libavcodec.so.58			$(EXTRA_TOOL_CHAIN_LIBRARY_PATH); \
	cd $(EXTRA_APPL_DEP_LIBRARY_PATH)/ && cp -drf deps/lib/libavformat.so.58		$(EXTRA_TOOL_CHAIN_LIBRARY_PATH); \
	cd $(EXTRA_APPL_DEP_LIBRARY_PATH)/ && cp -drf deps/lib/libavutil.so.56			$(EXTRA_TOOL_CHAIN_LIBRARY_PATH); \
	cd $(EXTRA_APPL_DEP_LIBRARY_PATH)/ && cp -drf deps/lib/libdrm.so.2				$(EXTRA_TOOL_CHAIN_LIBRARY_PATH)/libdrm.so.2.4.0; \
	cd $(EXTRA_APPL_DEP_LIBRARY_PATH)/ && cp -drf deps/lib/libgraphic_lsf.so		$(EXTRA_TOOL_CHAIN_LIBRARY_PATH); \
	cd $(EXTRA_APPL_DEP_LIBRARY_PATH)/ && cp -drf deps/lib/librga.so.2				$(EXTRA_TOOL_CHAIN_LIBRARY_PATH)/librga.so.2.1.0; \
	cd $(EXTRA_APPL_DEP_LIBRARY_PATH)/ && cp -drf deps/lib/librockchip_mpp.so.1		$(EXTRA_TOOL_CHAIN_LIBRARY_PATH)/librockchip_mpp.so.0; \
	cd $(EXTRA_APPL_DEP_LIBRARY_PATH)/ && cp -drf deps/lib/libswresample.so.3		$(EXTRA_TOOL_CHAIN_LIBRARY_PATH); \
	# libmali.so copied from $(SDK_MATRIX_PATCH_PATH) \
	touch $(SDK_SOURCE_PATH)/.stamp_patch ; \
	fi
	
	$(call msg,  SDK patch complete )

install:
	$(call msg,  Toolchain Installation Start )

	@if [ ! -d $(TOOLCHAIN_INSTALL_PATH) ]; then \
	mkdir -p $(TOOLCHAIN_INSTALL_PATH); \
	cp -rf $(LOCAL_SDK_PATH)/build/toolchain $(TOOLCHAIN_INSTALL_PATH)/; \
	touch $(SDK_SOURCE_PATH)/.stamp_patch ; \
	fi

	$(call msg,  Toolchain Installation Complete )

clean-sdk:
	rm -rf $(SDK_SOURCE_PATH)
	$(call msg,  SDK removed )

