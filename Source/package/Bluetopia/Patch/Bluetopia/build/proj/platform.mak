#########################################################################
##### Platform Sepcific Definitions                                 #####
#####   Targets (sample apps), specific PATH, libraries and objects ##### 
#####   Also includes env specific compilation/linking flags        #####
#########################################################################

#####################################################################
# First, define platform specific files that to be added to the 
# internal compilation and not exported to clients. This will include
# modules that are reimplemented for certain platform, but are not
# part of the notmal porting. 
#####################################################################
BTPS_OBJS += $(OBJDIR)/HCITRANS.o $(OBJDIR)/LOCKFILE.o


#####################################################################
# The following defines platform files that are 
# available for the customers for porting on specific platform
#####################################################################
ifeq ($(ISPP),y)
	BTPS_PLA_OBJS +=  $(OBJDIR)/IACPTRAN.o 
endif

#####################################################################
# Now, Update Targets (usually sample apps and optionally PM Server),
# and their SOURCES (VPATH) and INCLUDE paths.
#####################################################################
RELPATH = 
RELPATH_C = 
RELPATH_S = 



ifeq ($(AUD),y)
#	LDFLAGS += -lasound 

ifeq ($(BUILD_TYPE),FULL)
ifeq ($(GSTREAMER),y)
        ### the following defintions are required for adding gstreamer plugin ###
        GSTREAMER_TARGETS += libgstss1bluetooth.so
        CFLAGS += -fPIC
endif
endif

endif


ifeq ($(PAN),y)
	VNET_TARGETS = SS1VNETM.ko
	RELPATH += VNET/driver/SS1VNETM/
ifdef PROJ_PATH
	VNETPATH = $(PROJ_PATH)/VNET/driver/SS1VNETM/Kernel_2_6/
else
	VNETPATH = $(PWD)/$(CORE_PATH)/VNET/driver/SS1VNETM/Kernel_2_6/
endif
endif

ifeq ($(ISPP),y)
	CFLAGS += -DISPP_SUPPORT=TRUE
else
        CFLAGS += -DISPP_SUPPORT=FALSE
endif

ifeq ($(PLATFORM_MANAGER),y)
	PM_SAMPLES =  LinuxDEVM LinuxHDPM LinuxHDPM_11073  
	PM_SAMPLES += LinuxHIDM LinuxHRPM LinuxOPPM LinuxPBAM LinuxSPPLE_PM 
ifeq ($(CSCP),y)
        PM_SAMPLES += LinuxCSCM
endif
	PM_SAMPLES += LinuxIBeacon 
ifeq ($(ISPP),y)
        PM_SAMPLES += LinuxISPPM
endif
	PM_SAMPLES += LinuxBASM LinuxHOGM LinuxPANM LinuxSPPM SS1Tool
	PM_PROFILE_SAMPLES += LinuxFTPM_SRV LinuxFTPM_CLT 
	PM_PROFILE_SAMPLES += LinuxFMPM_TAR 
	PM_PROFILE_SAMPLES += LinuxGATM_SRV LinuxGATM_CLT LinuxGAT_MAT BTClient TestApp
	PM_PROFILE_SAMPLES += LinuxHFRM_AG LinuxHFRM_HF LinuxHDSM_AG  LinuxHDSM_HS
	PM_PROFILE_SAMPLES += LinuxMAPM_MCE LinuxMAPM_MSE
	PM_PROFILE_SAMPLES += LinuxPXPM_MON 
	RELPATH_C += $(PM_SAMPLES:%=sample/%) 
	RELPATH_C += sample/LinuxFTPM sample/LinuxFMPM sample/LinuxGATM sample/LinuxHDSM  sample/LinuxHFRM sample/LinuxGAT_MAT
	RELPATH_C +=  sample/LinuxMAPM sample/LinuxPANM sample/LinuxPXPM
	_PM_TARGETS += SS1BTPM $(PM_SAMPLES) $(PM_PROFILE_SAMPLES)
endif

ifdef PROJ_PATH
VPATH += $(RELPATH:%=$(PROJ_PATH)/%) 
VPATH += $(RELPATH_C:%=$(PM_PROJ_PATH)/%) 
VPATH += $(RELPATH_S:%=$(PM_PROJ_PATH)/%) 
else
VPATH += $(RELPATH:%=$(CORE_PATH)/%) 
VPATH += $(RELPATH_C:%=$(PM_CORE_PATH)/%) 
VPATH += $(RELPATH_S:%=$(PM_CORE_PATH)/%) 
endif

ifeq ($(GSTREAMER),y)
	TARGETS = $(GSTREAMER_TARGETS:%=$(PMLIBDIR)/%)
endif
TARGETS += $(VNET_TARGETS:%=$(VNETDIR)/%)
PM_TARGETS += $(_PM_TARGETS:%=$(PMBINDIR)/%)
