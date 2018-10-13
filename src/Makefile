include ../.config
include ../scripts/config.mak

#-------------------------------------------------------------------------#
#----------------------------- platform ----------------------------------#
#-------------------------------------------------------------------------#
obj-y :=
obj-y += ./tools/main.c
obj-y += ./tools/list.c
obj-y += ./tools/task.c
obj-y += ./platform/register.c
obj-y += ./platform/log_printf.c
obj-y += ./platform/os_buffer.c
obj-y += ./platform/os_malloc.c
obj-y += ./platform/os_timer.c
obj-$(CONFIG_X86)   += $(shell find ./platform/x86/   -name "*.c")
obj-$(CONFIG_DSP32) += $(shell find ./platform/dsp32/ -name "*.c")
OBJECTS := $(subst .c,.o,$(obj-y))
DEPENDS := $(subst .c,.d,$(obj-y))

#-------------------------------------------------------------------------#
#----------------------------- libavcodec.a ------------------------------#
#-------------------------------------------------------------------------#
obj-y :=
obj-$(CONFIG_VORBIS_DECODER) += ./avcodec/vorbis_dec.c
obj-$(CONFIG_VORBIS_ENCODER) += ./avcodec/vorbis_enc.c
obj-$(CONFIG_OPUS_ENCODER)   += ./avcodec/opus_dec.c
obj-$(CONFIG_OPUS_DECODER)   += ./avcodec/opus_enc.c
AVCODEC_OBJECTS := $(subst .c,.o,$(obj-y))
DEPENDS         += $(subst .c,.d,$(obj-y))
CONFIG_LIBS     += libavcodec.a

#-------------------------------------------------------------------------#
#----------------------------- libavutils.a ------------------------------#
#-------------------------------------------------------------------------#
obj-y :=
obj-y += ./avutils/framing.c
obj-y += ./avutils/opus_header.c
AVUTILS_OBJECTS := $(subst .c,.o,$(obj-y))
DEPENDS         += $(subst .c,.d,$(obj-y))
CONFIG_LIBS     += libavutils.a

#-------------------------------------------------------------------------#
#------------------------------- libopus.a -------------------------------#
#-------------------------------------------------------------------------#
obj-y :=
obj-$(CONFIG_OPUS_DECODER) += $(shell find ./libsrc/opus/ -name "*.c")
obj-$(CONFIG_OPUS_ENCODER) += $(shell find ./libsrc/opus/ -name "*.c")
OPUS_OBJECTS := $(subst .c,.o,$(obj-y))
DEPENDS      += $(subst .c,.d,$(obj-y))

#-------------------------------------------------------------------------#
#------------------------------ libdriver.a ------------------------------#
#-------------------------------------------------------------------------#
obj-y :=
obj-$(CONFIG_X86) := $(shell find ./driver -name "*.c")
DRIVER_OBJECTS := $(subst .c,.o,$(obj-y))
DEPENDS        += $(subst .c,.d,$(obj-y))

#-------------------------------------------------------------------------#
#------------------------------ config -----------------------------------#
#-------------------------------------------------------------------------#
INCLUDE_DIR := ./include 
INCLUDE_DIR += ./register/include
INCLUDE_DIR += ./platform/include
INCLUDE_DIR += ./libsrc/opus/include
INCLUDE_DIR += ./libsrc/opus/celt
INCLUDE_DIR += ./libsrc/opus/silk
INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIR))
CCFLAGS := $(INCLUDE_FLAGS) $(CONFIG_FLAGS) -O2 -pipe -Wall -c
ARFLAGS := -rcs


.PHONY: prepare

all: prepare $(CONFIG_LIBS) $(TARGETS)

prepare:
	@echo "\033[31;31m--- Version:  "$(CONFIG_VERSION)"\033[0m"
	@echo "\033[31;31m--- Platform: "$(PLATFORM)"\033[0m"
	@echo "\033[31;31m--- Decoder:  "$(DECODER)"\033[0m"
	@mkdir -p ../lib
	@mkdir -p ../bin

libavcodec.a: $(AVCODEC_OBJECTS)
	@$(AR) $(ARFLAGS) $@ $^
	@mv libavcodec.a ../lib
	@echo "\033[31;36mlibavcodec.a compile complete.. \033[0m"

libavutils.a: $(AVUTILS_OBJECTS)
	@$(AR) $(ARFLAGS) $@ $^
	@mv libavutils.a ../lib
	@echo "\033[31;36mlibavutils.a compile complete.. \033[0m"

libopus.a: $(OPUS_OBJECTS)
	@$(AR) $(ARFLAGS) $@ $^
	@mv libopus.a ../lib
	@echo "\033[31;36mlibopus.a compile complete.. \033[0m"

libvorbis.a: $(VORBIS_OBJECTS)
	@$(AR) $(ARFLAGS) $@ $^
	@mv libvorbis.a ../lib
	@echo "\033[31;36mlibvorbis.a compile complete.. \033[0m"

libdriver.a: $(DRIVER_OBJECTS)
	@$(AR) $(ARFLAGS) $@ $^
	@mv libdriver.a ../lib
	@echo "\033[31;36mlibdriver.a compile complete.. \033[0m"

x86.elf: libdriver.a $(OBJECTS)
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
