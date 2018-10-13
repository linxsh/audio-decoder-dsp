#
# Audio Dsp Preprocess
# Author linxsh
#
# Makefile: Audio Dsp Top Makefile
#
#----------------------------------------------------------------------------------
# Just run 'make menuconfig', configure stuff, then run 'make'.
# You shouldn't need to mess with anything beyond this point...
#----------------------------------------------------------------------------------

###################################################################################
#
# You should probably leave this stuff alone unless you know
# what you are doing.
#
###################################################################################

.PHONY: menuconfig

all: build

# absolute path
CURDIR          := $(shell pwd)
obj             := $(CURDIR)/scripts/kconfig
SUBDIR          := $(obj)
CONFIG_KCONFIG  := Kconfig

menuconfig : $(obj)/mconf $(obj)/conf
	@$< $(CONFIG_KCONFIG)

$(obj)/mconf:
	@make -C $(SUBDIR) prepare
	@make -C $(SUBDIR)

$(obj)/conf:
	@make -C $(SUBDIR) prepare
	@make -C $(SUBDIR)

###################################################################################
.PHONY: process result

build: .config process result

.config:
	@make menuconfig

process:
	@make -C src

result:
	@ls -l bin

clean:
	@make -C ./src clean
	@rm -rf bin

distclean: clean
	@make -C $(SUBDIR) clean
	@find ./ -name *.o | xargs rm -f
	@find ./ -name *.d | xargs rm -f
	@find ./ -name .config | xargs rm -f

help:
	@echo 'Cleaning:'
	@echo '  clean                  - delete all files created by build'
	@echo '  distclean              - delete all non-source files (including .config)'
	@echo
	@echo 'Configuration:'
	@echo '  menuconfig             - interactive curses-based configurator'
