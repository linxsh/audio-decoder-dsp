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
.PHONY: prepare compile link

build: .config prepare compile link

.config:
	@make menuconfig

prepare:
	@make  -C ./scripts prepare
	@mkdir -p ./install/lib
	@mkdir -p ./install/include
	@mkdir -p ./install/bin

compile:
	@make -C ./platform
	@make -C ./libsrc
	@make -C ./api
	#@make -C ./driver
	#@make -C ./app

link:
	@make -C ./scripts link

clean:
	@make -C ./platform clean
	@make -C ./libsrc   clean
	@make -C ./api      clean
	#@make -C ./driver   clean
	#@make -C ./app      clean
	@rm ./install/lib     -rf
	@rm ./install/include -rf
	@rm ./install/bin     -rf

distclean:
	@find ./ -name *.o | xargs rm -f
	@find ./ -name *.d | xargs rm -f
	@rm ./install/lib     -rf
	@rm ./install/include -rf
	@rm ./install/bin     -rf
	@rm .config -f
	@rm .config.old -f
	@rm scripts/kconfig/mconf -f

help:
	@echo 'Cleaning:'
	@echo '  clean                  - delete all files created by build'
	@echo '  distclean              - delete all non-source files (including .config)'
	@echo
	@echo 'Configuration:'
	@echo '  menuconfig             - interactive curses-based configurator'
