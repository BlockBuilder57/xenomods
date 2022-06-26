.PHONY: all send

PYTHON := python

# If no ip specified, then just assume this one
ifeq (,$(IP))
IP := 192.168.1.58
endif

TID := 0100E95004038000
CODE_NAME := bf2
#TID := 0100FF500E34A000
#CODE_NAME := bfsw

SUBSDK_NAME := subsdk1
SCRIPTS_DIR := scripts
SEND_PATCH := $(SCRIPTS_DIR)/send_over_ftp.py
MAKE_NSO := nso.mk

all:
	@echo This makefile doesnt actually build anything, use cmake
	@echo It only exists for automating the FTP transfer

send: all
	$(PYTHON) $(SEND_PATCH) $(IP) $(TID) $(CODE_NAME) $(SUBSDK_NAME)
