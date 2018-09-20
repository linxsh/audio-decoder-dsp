include ../../.config
include ../../scripts/config.mak

obj-$(CONFIG_VORBIS_DECODER) += $(shell find ./decoder/vorbis/   -name "*.c")
obj-$(CONFIG_OPUS_DECODER)   += $(shell find ./decoder/opus/     -name "*.c")

INCLUDE_DIR := ./include
INCLUDE_DIR += ../include
INCLUDE_DIR += ../register/include
INCLUDE_DIR += ../platform/include

INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIR))
CCFLAGS := $(INCLUDE_FLAGS) $(CONFIG_FLAGS) -O2 -pipe -Wall -c
ARFLAGS := -rcs

OBJECTS := $(subst .c,.o,$(obj-y))
DEPENDS := $(subst .c,.d,$(obj-y))

libapi.a: $(OBJECTS)
	@echo "libapi.a compile complete .."

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
	@find ./ -name "*.o"|xargs rm -f
	@find ./ -name "*.d"|xargs rm -f
	@find ./ -name "*.a"|xargs rm -f
