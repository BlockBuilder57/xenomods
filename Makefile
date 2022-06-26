#TODO(Khangaroo) : Make this process a lot less hacky(no, export did not work)
#See MakefileNSO

.PHONY: all clean skyline send

#annoying
#PYTHON : = python3
#ifeq(, $(shell which python3))
# #if no python3 alias, fall back to `python` and hope it's py3
PYTHON := python
#endif

NAME := $(shell basename $(CURDIR))
NAME_LOWER := $(shell echo $(NAME) | tr A-Z a-z)

# if no ip specified then just assume this one
ifeq (,$(IP))
IP := 192.168.1.58
endif

TID := 0100E95004038000
CODE_NAME := bf2
#TID := 0100FF500E34A000
#CODE_NAME := bfsw
SUBSDK_NAME := subsdk1
SCRIPTS_DIR := scripts
BUILD_DIR := build
SEND_PATCH := $(SCRIPTS_DIR)/send_over_ftp.py
MAKE_NSO := nso.mk

all : patch-$(CODE_NAME)/ $(CODE_NAME).npdm skyline

patch-$(CODE_NAME)/:
	mkdir -pv $@

skyline:
	/mingw64/bin/cmake -P $(SCRIPTS_DIR)/gittag.cmake

	$(MAKE) all -f $(MAKE_NSO) MAKE_NSO=$(MAKE_NSO) BUILD=$(BUILD_DIR) TARGET=$(NAME)
	
# this automatically packages up into a patch-$(CODENAME) dir
	cp $(NAME).nso patch-$(CODE_NAME)/$(SUBSDK_NAME)
	cp $(CODE_NAME).npdm patch-$(CODE_NAME)/main.npdm

$(CODE_NAME).npdm:
	/opt/devkitpro/tools/bin/npdmtool $(CODE_NAME).json $(CODE_NAME).npdm

send : all
	$(PYTHON) $(SEND_PATCH) $(IP) $(TID) $(CODE_NAME) $(SUBSDK_NAME)

clean: 
	@-rm -f $(CODE_NAME).npdm
	@-rm -rf patch-$(CODE_NAME)
	$(MAKE) clean -f $(MAKE_NSO)
