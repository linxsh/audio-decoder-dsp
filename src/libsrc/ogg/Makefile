include ../../../.config
include ../../../scripts/config.mak

obj-y := $(shell find ./ -name "*.c")

INCLUDE_DIR := ./include
INCLUDE_DIR += ./../../platform/include

INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIR))
CCFLAGS := $(INCLUDE_FLAGS) $(CONFIG_FLAGS) -O2 -pipe -Wall -c
ARFLAGS := -rcs

OBJECTS := $(subst .c,.o,$(obj-y))
DEPENDS := $(subst .c,.d,$(obj-y))

libogg.a: $(OBJECTS)
	@echo "libogg.a compile complete .."

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

