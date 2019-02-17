include ../.config
include ../scripts/resolver.mak
include ../scripts/compiler.mak

###########################################################################
obj-y := ./dev_list.c
obj-y += ./dev_core.c
obj-y += ./kernelcall.c
obj-y += ./log_format.c

LIBS-y += libdriver.prep
LIBS-y += libdriver.a
LIBS-y += libdriver.inst
DRIVER_OBJECTS := $(subst .c,.o,$(obj-y))
DRIVER_DEPENDS := $(subst .c,.d,$(obj-y))
###########################################################################

INC_FLAGS := $(addprefix -I,./include)
INC_FLAGS += $(addprefix -I,../platform/include)
CCFLAGS   := $(INC_FLAGS) $(CONFIG_FLAGS) -O2 -pipe -Wall -c
ARFLAGS   := -rcs
###########################################################################
.PHONY:

all: $(LIBS-y)
###########################################################################
libdriver.prep:
	@echo "\033[31;36mlibdriver.a compiling..\033[0m"

libdriver.a: $(DRIVER_OBJECTS)
	@$(AR) $(ARFLAGS) $@ $^

libdriver.inst:
	@mv libdriver.a ../install/lib
###########################################################################
ifneq ($(MAKECMDGOALS), clean)
-include $(DRIVER_DEPENDS)
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
