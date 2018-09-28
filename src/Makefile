include ../.config
include ../scripts/config.mak

#-------------------------------------------------------#
obj-y := $(shell find ./common/          -name "*.c")
obj-y += $(shell find ./platform/common/ -name "*.c")

obj-$(CONFIG_X86)   += $(shell find ./demo/x86/       -name "*.c")
obj-$(CONFIG_X86)   += $(shell find ./platform/x86/   -name "*.c")
obj-$(CONFIG_X86)   += $(shell find ./register/x86/   -name "*.c")
obj-$(CONFIG_DSP32) += $(shell find ./demo/dsp32/     -name "*.c")
obj-$(CONFIG_DSP32) += $(shell find ./platform/dsp32/ -name "*.c")
obj-$(CONFIG_DSP32) += $(shell find ./register/dsp32/ -name "*.c")
#------------------------------------------------------#

#------------------------------------------------------#
INCLUDE_DIR := ./include 
INCLUDE_DIR += ./register/include
INCLUDE_DIR += ./platform/include
INCLUDE_DIR += ./libapi/include

INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIR))
CCFLAGS := $(INCLUDE_FLAGS) $(CONFIG_FLAGS) -O2 -pipe -Wall -c
ARFLAGS := -rcs

OBJECTS := $(subst .c,.o,$(obj-y))
DEPENDS := $(subst .c,.d,$(obj-y))
#------------------------------------------------------#

.PHONY: prepare

all: prepare $(LIBSRCS) $(TARGETS)

prepare:
	@echo "--- Version: "$(CONFIG_VERSION)
	@echo "--- Platform: "$(PLATFORM)
	@echo "--- Decoder: "$(DECODER)

x86.elf: $(OBJECTS)
	@make -C libapi
	@make -C libsrc
	@echo "x86.elf compile complete .."

dsp32.bin:
	@make -C libapi
	@make -C libsrc
	@echo "dsp32.bin compile complete .."

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPENDS)
endif

%.d:%.c
	@$(CC) $(CCFLAGS) -MM $< > $@.$$$$;\
	sed 's,\(.*\)\.o[ :]*,$(shell dirname $@)/\1.o $@ : ,g' <$@.$$$$ > $@;\
	rm -f $@.$$$$

%.o:%.c
	@echo compiling $@
	@$(CC) $(CCFLAGS) -c $< -o $@

clean:
	@make -C libapi clean
	@make -C libsrc clean
	@find ./ -name "*.o"|xargs rm -f
	@find ./ -name "*.d"|xargs rm -f
	@find ./ -name "*.a"|xargs rm -f
