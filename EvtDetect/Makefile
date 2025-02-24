#************************************************************************
# Filename		: Makefile
# Author		: Kunjal Garach
# Description	: Makefile to compile Wireless door Application
# Date			: 11th Nov 2010
# NOTE			: This make file uses moderately advanced feature of -
#		  	  	  gnu make and should be edited with proper care.
#************************************************************************
#
#************************************************************************
SHELL = /bin/sh

##############################################
# Check options passed to make file
##############################################
MODULE_BIN_DIR := bin
MODULE_OBJ_DIR := obj

CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar
LINK := $(CROSS_COMPILE)gcc

ifeq ($(DEBUG), on)
DEBUG_FLAG := -g
else
DEBUG_FLAG :=
endif

ifeq ($(VERBOSE), on)
_@_ :=
else
_@_ := @
endif

##############################################
# COMPILER SELECTION + PROJECT EXTENSIONS
##############################################

DEPEND_EXTENSION = .d
OBJ_EXTENSION = .o
NODEPEND_TARGETS := clean

##############################################
# SOURCE DIRECTORY PATH
##############################################
MODULE_SOURCE_DIR := .
MODULE_BIN_NAME := evtDetect

##############################################
# INCLUDE DIRECTORY PATH (SOURCE + LIBRARY)
##############################################
INCLUDE_DIRS := 

##############################################
# LIBRARY DIRECTORY PATH
##############################################
LIBRARY_DIRS := 

##############################################
# Libraries needed by application
##############################################
APPL_LIBS_NEEDED := 

##############################################
# COMPILER & LINKER RELATED VARIABLES
##############################################
COMPILE_ONLY_FLAG := -c
INCLUDE_FLAG := -I
LIB_DIR_FLAG := -L
CC_OBJ_OUTPUT = -o $@
CFLAGS := -Wall -pthread -Wunreachable-code -W -Wextra $(DEBUG_FLAG)
CFLAGS += $(INCLUDE_DIRS:%=$(INCLUDE_FLAG)%)
CFLAGS += -DARMLINUX

LDFLAGS += -o $(MODULE_BIN_DIR)/$(MODULE_BIN_NAME)
LDFLAGS += -pthread


##############################################
# Source / Object lists
##############################################
# Create object list for both CPP and C files
MODULE_C_OBJ_LIST := $(foreach src,$(wildcard $(MODULE_SOURCE_DIR)/*.c),$(MODULE_OBJ_DIR)/$(notdir $(src:.c=$(OBJ_EXTENSION))))
MODULE_OBJ_LIST := $(MODULE_CPP_OBJ_LIST) $(MODULE_C_OBJ_LIST)

##############################################
# GENERATING DEPENDENCIES
##############################################
MAKEDEPEND = set -e; $(CC) -P -M -c $(CFLAGS) $< | \
		sed '1s=^=$@ $(PROJ_OBJ_DIR)/$(*D)/=' > $@; \
		[ -s $@ ] || rm -f $@

##############################################
# CREATING DEPENDENCIES FOR CPP / C FILES
##############################################
$(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION): $(MODULE_SOURCE_DIR)/%.c
	$(_@_) [ -d $(@D) ] || mkdir -p $(@D)
	$(_@_) $(MAKEDEPEND)

##############################################
# COMPILING CPP / C FILES
##############################################
$(MODULE_OBJ_DIR)/%$(OBJ_EXTENSION): $(MODULE_SOURCE_DIR)/%.c $(MODULE_OBJ_DIR)/%$(DEPEND_EXTENSION)
	@echo Compiling C file $< $(ECHO_STDOUT)
	$(_@_) $(CC) $(COMPILE_ONLY_FLAG) $(CFLAGS) $< $(CC_OBJ_OUTPUT)

##############################################
# TARGETS
##############################################
# if no goal (target) was specified then assume "all"
.PHONY: default
default: all


.PHONY: clean
clean:
	$(_@_) echo Cleaning all object and binary files
	$(_@_) rm -fR $(MODULE_OBJ_DIR)/*
	$(_@_) rm -fR $(MODULE_BIN_DIR)/*


all :$(MODULE_OBJ_LIST)
	$(_@_) mkdir -m777 -p $(MODULE_BIN_DIR)
	$(_@_) $(LINK) $(LDFLAGS) $(MODULE_OBJ_LIST) $(LIBRARY_DIRS:%=$(LIB_DIR_FLAG)%) $(APPL_LIBS_NEEDED)
	$(_@_) echo -e "\033[1m\E[33;42mApplication \"$(MODULE_BIN_DIR)/$(MODULE_BIN_NAME)\" created successfully!  \033[0m"
	$(_@_) echo -e "\033[1m\E[33;44m`ls -go $(MODULE_BIN_DIR)/$(MODULE_BIN_NAME)`\033[0m"
