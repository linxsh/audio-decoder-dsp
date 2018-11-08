include ../.config
include ../scripts/resolver.mak
include ../scripts/compiler.mak

obj-y := log_printf.c
obj-y += os_buffer.c
obj-y += os_malloc.c
obj-y += os_timer.c
obj-y += register.c

obj-$(CONFIG_X86) += x86/x86_buffer.c
obj-$(CONFIG_X86) += x86/x86_timer.c

obj-$(CONFIG_DSP32) += dsp32/dsp32_buffer.c
obj-$(CONFIG_DSP32) += dsp32/dsp32_printf.c
obj-$(CONFIG_DSP32) += dsp32/dsp32_timer.c

LIBS-y :=
LIBS-$(CONFIG_X86)   +=libx86.a
LIBS-$(CONFIG_DSP32) +=libdsp32.a

OBJECTS := $(subst .c,.o,$(obj-y))
DEPENDS := $(subst .c,.d,$(obj-y))

INCLUDE_FLAGS := $(addprefix -I,include)
CCFLAGS := $(INCLUDE_FLAGS) $(CONFIG_FLAGS) -O2 -pipe -Wall -c
ARFLAGS := -rcs

.PHONY:

all: prepare $(LIBS-y) install

prepare:
	@echo "\033[31;36m$(LIBS-y) compiling..\033[0m"

libx86.a: $(OBJECTS)
	@$(AR) $(ARFLAGS) $@ $^

libdsp32.a: $(OBJECTS)
	@$(AR) $(ARFLAGS) $@ $^

install:
	@mv *.a ../install/lib

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
