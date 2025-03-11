# PROJECT DEFINITION FILE - Linux (AM335x/AM437x)

MAKEDEPEND = $(CC) -M -MT $(OBJDIR)/$*.o $(CFLAGS) -o $(DEPDIR)/$*.d $<  
MAKEDEPEND_S = $(CC) -M -MT $(OBJDIR)/$*_S.o $(CFLAGS_S) -o $(DEPDIR)/$*_S.d $<  
MAKEDEPEND_C = $(CC) -M -MT $(OBJDIR)/$*_C.o $(CFLAGS_C) -o $(DEPDIR)/$*_C.d $<  
RM = rm -rf
CP = cp -rf
RSYNC = rsync -a
MV = mv
MKDIR = mkdir -p

