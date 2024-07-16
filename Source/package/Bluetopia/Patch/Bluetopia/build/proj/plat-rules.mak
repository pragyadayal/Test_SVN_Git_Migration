# PLATFORM SPECIFIC RULES DEFINITIONS
$(OBJDIR)/%.o: %.c
	@$(MAKEDEPEND)
	@echo CC $(<F) -o $(@F)
	@$(CC)  $(CFLAGS) -c $< -o $@

$(OBJDIR)/%_S.o: server/%.c
	@$(MAKEDEPEND_S)
	@echo CC_S $(<F) -o $(@F)
	@$(CC)  $(CFLAGS_S) -c $< -o $@

$(OBJDIR)/%_S.o: %.c
	@$(MAKEDEPEND_S)
	@echo CC-S $(<F) -o $(@F)
	@$(CC)  $(CFLAGS_S) -c $< -o $@

$(OBJDIR)/%_C.o: client/%.c
	@$(MAKEDEPEND_C)
	@echo CC_C $(<F) -o $(@F)
	@$(CC)  $(CFLAGS_C) -c $< -o $@

$(OBJDIR)/%_C.o: %.c
	@$(MAKEDEPEND_C)
	@echo CC-C $(<F) -o $(@F)
	@$(CC)  $(CFLAGS_C) -c $< -o $@

$(VNETDIR)/SS1VNETM.ko: Kernel_2_6/SS1VNETM.c  Kernel_2_6/SS1VNETM.h
	make -C $(KERNELDIR) M=$(VNETPATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules 
	cp $(VNETPATH)/SS1VNETM.ko $@
	
	
$(PMBINDIR)/LinuxDEVM:  $(OBJDIR)/LinuxDEVM_C.o  $(PMLIBDIR)/libBTPM_C.a  $(PMLIBDIR)/libBTPM_HAL_C.a  
	@echo LN $(^F) -o $(@F)
	@$(CC)  -L$(LIBDIR) $^ $(LDFLAGS) -o $@
	
$(PMBINDIR)/LinuxFTPM_SRV:  $(OBJDIR)/LinuxFTPM_SRV_C.o  $(PMLIBDIR)/libBTPM_C.a  $(PMLIBDIR)/libBTPM_HAL_C.a 
	@echo LN $(^F) -o $(@F)
	@$(CC)  -L$(LIBDIR) $^ $(LDFLAGS) -o $@
	
$(PMBINDIR)/LinuxFTPM_CLT:  $(OBJDIR)/LinuxFTPM_CLT_C.o  $(PMLIBDIR)/libBTPM_C.a   $(PMLIBDIR)/libBTPM_HAL_C.a
	@echo LN $(^F) -o $(@F)
	@$(CC)  -L$(LIBDIR) $^ $(LDFLAGS) -o $@
	
$(PMBINDIR)/LinuxHDSM_HS:  $(OBJDIR)/LinuxHDSM_HS_C.o  $(PMLIBDIR)/libBTPM_C.a   $(PMLIBDIR)/libBTPM_HAL_C.a
	@echo LN $(^F) -o $(@F)
	@$(CC)  -L$(LIBDIR) $^ $(LDFLAGS) -o $@
	
$(PMBINDIR)/LinuxHDSM_AG:  $(OBJDIR)/LinuxHDSM_AG_C.o  $(PMLIBDIR)/libBTPM_C.a   $(PMLIBDIR)/libBTPM_HAL_C.a
	@echo LN $(^F) -o $(@F)
	@$(CC)  -L$(LIBDIR) $^ $(LDFLAGS) -o $@
	
$(PMBINDIR)/LinuxHDPM_11073:  $(OBJDIR)/LinuxHDPM_11073_C.o  $(OBJDIR)/IEEE_11073_C.o $(PMLIBDIR)/libBTPM_C.a  $(PMLIBDIR)/libBTPM_HAL_C.a 
	@echo LN $(^F) -o $(@F)
	@$(CC)  -L$(LIBDIR) $^ $(LDFLAGS) -o $@
	
ifeq ($(CSCP),y)	
$(PMBINDIR)/LinuxCSCM:  $(OBJDIR)/LinuxCSCM_C.o  $(PMLIBDIR)/libBTPM_C.a   $(PMLIBDIR)/libBTPM_HAL_C.a
	@echo LN $(^F) -o $(@F)
	@$(CC)  -L$(LIBDIR) $^ $(LDFLAGS) -o $@
endif
	
$(PMBINDIR)/LinuxMAPM_%:  $(OBJDIR)/LinuxMAPM_%_C.o  $(OBJDIR)/MsgStorePM_C.o $(PMLIBDIR)/libBTPM_C.a   $(PMLIBDIR)/libBTPM_HAL_C.a
	@echo LN $(^F) -o $(@F)
	@$(CC)  -L$(LIBDIR) $^ $(LDFLAGS) -o $@
	
$(PMBINDIR)/%:  $(OBJDIR)/%_C.o  $(PMLIBDIR)/libBTPM_C.a   $(PMLIBDIR)/libBTPM_HAL_C.a
	@echo LN $(^F) -o $(@F)
	@$(CC)  -L$(LIBDIR) $^ $(LDFLAGS) -o $@

ifeq ($(ISPP),y)
$(PMBINDIR)/LinuxISPPM:  $(OBJDIR)/LinuxISPPM_C.o $(PMLIBDIR)/libBTPM_C.a $(PMLIBDIR)/libBTPM_HAL_C.a $(BT_ALL_LIBS)
	@echo LN $(^F) -o $(@F)
	@$(CC)  -L$(LIBDIR) $^ $(LDFLAGS) -o $@
endif

$(LIBDIR)/libBTPS_OSA.a: $(BTPS_OSA_OBJS)
	@echo AR $(^F) -o $(@F)
	@$(AR) r $@ $?
	
$(LIBDIR)/libBTPS_PLA.a: $(BTPS_PLA_OBJS) 
	@echo AR $(^F) -o $(@F)
	@$(AR) r $@ $?

$(LIBDIR)/libBTPS_VNET.a: $(OBJDIR)/SS1VNET.o $(OBJDIR)/SS1VNETI.o
	@echo AR $(^F) -o $(@F)
	@$(AR) r $@ $?	

$(PMLIBDIR)/libBTPM_HAL_C.a: $(BTPM_HAL_OBJS_C)  $(BTPM_VS_OBJS_C)
	@echo AR $(^F) -o $(@F)
	@$(AR) r $@ $?

$(PMLIBDIR)/libBTPM_HAL_S.a: $(BTPM_HAL_OBJS_S)  
	@echo AR $(^F) -o $(@F)
	@$(AR) r $@ $?

	
$(PMBINDIR)/SS1BTPM:   $(OBJDIR)/BTPMSRVR_S.o  $(PMLIBDIR)/libBTPM_S.a $(PMLIBDIR)/libBTPM_HAL_S.a  $(BT_ALL_LIBS)
	@echo LN $(^F) -o $(@F)
	@$(CC)  -L$(PMLIBDIR) $^ $(LDFLAGS) -o $@

ifeq ($(GSTREAMER),y)
$(PM_PROJ_PATH)/gstreamer/Makefile: $(PM_PROJ_PATH)/gstreamer/autogen.sh $(PM_PROJ_PATH)/gstreamer/configure.ac $(PM_PROJ_PATH)/gstreamer/Makefile.am
	@cd $(PM_PROJ_PATH)/gstreamer && ./autogen.sh
endif

.EXPORT_ALL_VARIABLES:

ifeq ($(GSTREAMER),y)
$(PMLIBDIR)/libgstss1bluetooth.so: $(PMLIBDIR)/libBTPM_C.a $(PMLIBDIR)/libBTPM_HAL_C.a $(PM_PROJ_PATH)/gstreamer/Makefile $(PM_PROJ_PATH)/gstreamer/src/*.c $(PM_PROJ_PATH)/gstreamer/src/*.h
	@make --directory $(PM_PROJ_PATH)/gstreamer
	@cp $(PM_PROJ_PATH)/gstreamer/src/.libs/libgstss1bluetooth.so $@
	@echo gstreamer plugin is done...
endif