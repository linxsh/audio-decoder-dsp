-include ../.config

#------------------------ platform ---------------------#
ifeq ($(CONFIG_X86), y)
-include ../scripts/x86-compiler.mak
endif

ifeq ($(CONFIG_DSP32), y)
-include ../scripts/dsp32-compiler.mak
endif
#-------------------------------------------------------#

#------------------------ decoder ----------------------#
ifeq ($(CONFIG_VORBIS_DECODER), y)
DECODER += vorbis
endif

ifeq ($(CONFIG_OPUS_DECODER), y)
DECODER += opus
endif
#-------------------------------------------------------#

#-------------------------------------------------------#
obj-y := $(shell find ./common/ -name "*.c")

obj-$(CONFIG_X86)   += $(shell find ./demo/x86/       -name "*.c")
obj-$(CONFIG_X86)   += $(shell find ./platform/x86/   -name "*.c")
obj-$(CONFIG_X86)   += $(shell find ./register/x86/   -name "*.c")
obj-$(CONFIG_DSP32) += $(shell find ./demo/dsp32/     -name "*.c")
obj-$(CONFIG_DSP32) += $(shell find ./platform/dsp32/ -name "*.c")
obj-$(CONFIG_DSP32) += $(shell find ./register/dsp32/ -name "*.c")

obj-$(CONFIG_VORBIS_DECODER) += $(shell find ./decoder/opus/   -name "*.c")
obj-$(CONFIG_VORBIS_DECODER) += $(shell find ./libsrc/opus/    -name "*.c")
obj-$(CONFIG_OPUS_DECODER)   += $(shell find ./decoder/vorbis/ -name "*.c")
obj-$(CONFIG_OPUS_DECODER)   += $(shell find ./libsrc/vorbis/  -name "*.c")
#------------------------------------------------------#

#------------------------------------------------------#
INCLUDE_DIR := ./include 

INCLUDE_FLAGS := $(addprefix -I , $(INCLUDE_DIR))
CCFLAGS := $(INCLUDE_FLAGS) -O2 -pipe -Wall -c
ARFLAGS := -rcs

SOBJECTS := $(subst .c,.o,$(obj-y))
#------------------------------------------------------#

.PHONY: prepare

all: prepare $(TARGETS)

prepare:
	@echo "--- Version: "$(CONFIG_VERSION)
	@echo "--- Platform: "$(PLATFORM)
	@echo "--- Decoder: "$(DECODER)

x86.elf: $(SOBJECTS)
	@echo "x86.elf compile complete .."

dsp32.bin:
	@echo "dsp32.bin compile complete .."

%.o:%.c
	@echo compiling $@
	@$(CC) $(CCFLAGS) -c $< -o $@

clean:
	@find ./ -name "*.o"|xargs   $(RM)
	@find ./ -name "*.d"|xargs   $(RM)
	@find ./ -name "*.a"|xargs   $(RM)
