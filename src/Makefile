include ../.config
include ../scripts/config.mak

#-------------------------------------------------------#
#---------------------- platform -----------------------#
#-------------------------------------------------------#
obj-y := ./main.c
obj-y += ./list.c
obj-y += ./task.c
obj-y += ./platform/register.c
obj-y += ./platform/log_printf.c
obj-y += ./platform/os_buffer.c
obj-y += ./platform/os_malloc.c
obj-y += ./platform/os_timer.c
obj-$(CONFIG_X86)   += $(shell find ./platform/x86/   -name "*.c")
obj-$(CONFIG_DSP32) += $(shell find ./platform/dsp32/ -name "*.c")

OBJECTS := $(subst .c,.o,$(obj-y))
DEPENDS := $(subst .c,.d,$(obj-y))
#-------------------------------------------------------#

#-------------------------------------------------------#
#----------------------- libapi.a ----------------------#
#-------------------------------------------------------#
obj-$(CONFIG_VORBIS_DECODER) := $(shell find ./libapi/decoder/vorbis/   -name "*.c")
obj-$(CONFIG_OPUS_DECODER)   += $(shell find ./libapi/decoder/opus/     -name "*.c")

API_OBJECTS := $(subst .c,.o,$(obj-y))
DEPENDS     += $(subst .c,.d,$(obj-y))
#-------------------------------------------------------#

#-------------------------------------------------------#
#---------------------- libopus.a ----------------------#
#-------------------------------------------------------#
obj-$(CONFIG_OPUS_DECODER) := $(shell find ./libsrc/opus/     -name "*.c")
OPUS_OBJECTS := $(subst .c,.o,$(obj-y))
DEPENDS      += $(subst .c,.d,$(obj-y))
#-------------------------------------------------------#

#-------------------------------------------------------#
#---------------------- config -------------------------#
#-------------------------------------------------------#
INCLUDE_DIR := ./include 
INCLUDE_DIR += ./register/include
INCLUDE_DIR += ./platform/include
INCLUDE_DIR += ./libapi/include

INCLUDE_DIR += ./libsrc/opus/include
INCLUDE_DIR += ./libsrc/opus/celt
INCLUDE_DIR += ./libsrc/opus/silk

INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIR))
CCFLAGS := $(INCLUDE_FLAGS) $(CONFIG_FLAGS) -O2 -pipe -Wall -c
ARFLAGS := -rcs
#-------------------------------------------------------#

.PHONY: prepare

all: prepare libapi.a $(CONFIG_LIBS) $(TARGETS)

prepare:
	@echo "\033[31;31m--- Version:  "$(CONFIG_VERSION)"\033[0m"
	@echo "\033[31;31m--- Platform: "$(PLATFORM)"\033[0m"
	@echo "\033[31;31m--- Decoder:  "$(DECODER)"\033[0m"

libopus.a: $(OPUS_OBJECTS)
	@echo "\033[31;36mlibopus.a compile complete.. \033[0m"

libvorbis.a: $(VORBIS_OBJECTS)
	@echo "\033[31;36mlibvorbis.a compile complete.. \033[0m"

libapi.a: $(API_OBJECTS)
	@echo "\033[31;36mlibapi.a compile complete.. \033[0m"

x86.elf: $(OBJECTS)
	@echo "\033[31;36mx86.elf compile complete.. \033[0m"

dsp32.bin:
	@echo "\033[31;36mdsp32.bin compile complete.. \033[0m"

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPENDS)
endif

%.d:%.c
	@$(CC) $(CCFLAGS) -MM $< > $@.$$$$;\
	sed 's,\(.*\)\.o[ :]*,$(shell dirname $@)/\1.o $@ : ,g' <$@.$$$$ > $@;\
	rm -f $@.$$$$

%.o:%.c
	@echo $@
	@$(CC) $(CCFLAGS) -c $< -o $@

clean:
	@find ./ -name "*.o"|xargs rm -f
	@find ./ -name "*.d"|xargs rm -f
	@find ./ -name "*.a"|xargs rm -f
