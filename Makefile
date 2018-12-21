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

#
# 1-27-2011: the $(word 1, $(subst....))) stuff takes the CURDIR
# and strips off everything after "/cyphort-endpoint". If the path
# doesn't have '/cyphort-endpoint', DEPOT_ROOT will be the curr. dir.
#
ROOT_PATTERN	= /HoneyProcs
DEPOT_ROOT		= $(word 1,$(subst $(ROOT_PATTERN), ,$(CURDIR)))
ROOT_PATH		= $(DEPOT_ROOT)$(ROOT_PATTERN)

include $(ROOT_PATH)/Makefile.globals

SUB_DIRS		= 3rdparty src

default:
	for dir in $(SUB_DIRS) ; do  \
		make -C $$dir default || exit 1; \
	done

clean :
	for dir in $(SUB_DIRS) ; do  \
		make -C $$dir clean || exit 1; \
	done

all : default

allclean: clean
	rm -rf $(HONEYPROCS_BUILD_ROOT)

help :
	@echo "default                : builds local libs and executables"
	@echo "clean                  : removes local libs and executables"
	@echo "all                    : builds 3rdparty and local stuff"
	@echo "allclean               : remove local stuff and 3rdparty"
	@echo "3rdparty-libs          : builds 3rd party libraries"
	@echo "HONEYPROCS_BUILD_ROOT  : env var specifying build results dir: $(HONEYPROCS_BUILD_ROOT)"
	@echo "VENDOR_LIB_DIR         : 'make' var for 3rd party libs: $(VENDOR_LIB_DIR)"
	@echo "VENDOR_INC_DIR         : 'make' var for 3rd party libs: $(VENDOR_INC_DIR)"

