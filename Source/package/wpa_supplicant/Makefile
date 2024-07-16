WPA_SUPPLICANT_PACKAGE_NAME := wpa_supplicant-2.9
WPA_SUPPLICANT_PACKAGE_NAME_WITH_EXT=$(WPA_SUPPLICANT_PACKAGE_NAME).tar.gz

all: source extract matrix_patch build install

source:
	svn update $(WPA_SUPPLICANT_PACKAGE_NAME_WITH_EXT)

extract:
	$(call msg,  Extracting package )
	rm -rf $(WPA_SUPPLICANT_PACKAGE_NAME)
	tar -xvf $(WPA_SUPPLICANT_PACKAGE_NAME_WITH_EXT)
	$(call msg,  Extract complete )

matrix_patch:
	$(call msg,   Applying patch )
	cp -rf patch/* $(WPA_SUPPLICANT_PACKAGE_NAME)
	$(call msg,    patch Applied )

build:
	$(call msg,  building  )
	make -C $(WPA_SUPPLICANT_PACKAGE_NAME)/wpa_supplicant clean
	cp $(WPA_SUPPLICANT_PACKAGE_NAME)/wpa_supplicant/defconfig $(WPA_SUPPLICANT_PACKAGE_NAME)/wpa_supplicant/.config
	make -C $(WPA_SUPPLICANT_PACKAGE_NAME)/wpa_supplicant CC=$(SVN_HOME_DIR)/Briefcase/SWD_Briefcase/GenericSoftwareModules/Platform/SWD/Trunk/Source/build/PANEL_LITE_V2/package/buildroot/buildroot-2016.08/output/host/usr/bin/arm-linux-gnueabihf-gcc
	$(call msg, build is done )
	
install:
	mkdir -p output
	cp -f $(WPA_SUPPLICANT_PACKAGE_NAME)/wpa_supplicant/wpa_supplicant output
	cp -f $(WPA_SUPPLICANT_PACKAGE_NAME)/wpa_supplicant/wpa_cli output
	cp -f $(WPA_SUPPLICANT_PACKAGE_NAME)/wpa_supplicant/wpa_passphrase output
	
