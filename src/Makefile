#
# Copyright(C) 2018, Juniper Networks, Inc.
# All rights reserved
#
# This SOFTWARE is licensed under the license provided in the LICENSE.txt
# file.  By downloading, installing, copying, or otherwise using the
# SOFTWARE, you agree to be bound by the terms of that license.  This
# SOFTWARE is not an official Juniper product.
#
# Third-Party Code: This SOFTWARE may depend on other components under
# separate copyright notice and license terms.  Your use of the source
# code for those components is subject to the term and conditions of
# the respective license as noted in the Third-Party source code.
#

ROOT_PATTERN	= /HoneyProcs/
DEPOT_ROOT		= $(word 1,$(subst $(ROOT_PATTERN), ,$(CURDIR)))
ROOT_PATH		= $(DEPOT_ROOT)$(ROOT_PATTERN)

include $(ROOT_PATH)/Makefile.globals

VPATH			= $(CURDIR)
LINK_LIBS		=
#LINK_LIBS		+= yara32
CL_FLAGS		+= -MD -EHsc -nologo
LINK_ARGS		=
INCLUDES		+= -I$(HONEYPROCS_INC_DIR)

MYTARGET		= scanner.exe

ALL_TARGETS		= chrome.exe \
				firefox.exe \
				explorer.exe \
				scanner.exe

SOURCES			= util-log.c

ifeq ($(MYTARGET), chrome.exe)
	SOURCES		+= honeyproc.c scan-engine.c
	CL_FLAGS	+= /DWINDOWS /DMIMIC_CHROME
else ifeq ($(MYTARGET), firefox.exe)
	SOURCES		+= honeyproc.c scan-engine.c
	CL_FLAGS	+= /DWINDOWS /DMIMIC_FIREFOX
else ifeq ($(MYTARGET), explorer.exe)
	SOURCES		+= honeyproc.c scan-engine.c
	CL_FLAGS	+= /DWINDOWS /DMIMIC_EXPLORER
else ifeq ($(MYTARGET), scanner.exe)
	SOURCES		+= scanner.c avl.c mmap.c scan-engine.c
endif

EXECUTABLE		= $(BUILD_BIN_DIR)/$(MYTARGET)
EXTRA_LD_FLAGS	+=
CL_FLAGS		+= /DWINDOWS

mytarget: setup $(EXECUTABLE)

default :
	@for target in $(ALL_TARGETS) ; do \
		echo ==== Building $$target ; \
		make --no-print-directory MYTARGET=$$target mytarget || exit 1; \
	done

default: setup $(EXECUTABLE)

clean::
	rm -rf $(BUILD_LIB_DIR)/*
	rm -rf $(BUILD_BIN_DIR)/*
	make rmtargets

$(DESTDIR)/usr/bin :
	mkdir -p $@

include $(ROOT_PATH)/Makefile.build
