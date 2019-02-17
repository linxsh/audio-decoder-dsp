include ../.config
include ../scripts/resolver.mak
include ../scripts/compiler.mak

LIBS-y    :=
INC_FLAGS := $(addprefix -I,../platform/include)
CCFLAGS   := $(INC_FLAGS) $(CONFIG_FLAGS) -O2 -pipe -Wall -c
ARFLAGS   := -rcs
###########################################################################
LIBS_OPUS = n
ifeq ($(CONFIG_OPUS_DECODER), y)
LIBS_OPUS = y
endif
ifeq ($(CONFIG_OPUS_ENCODER), y)
LIBS_OPUS = y
endif
ifeq ($(LIBS_OPUS), y)
LIBS-y += libopus.prep
LIBS-y += libopus.a
LIBS-y += libopus.inst
obj-y  := $(shell find ./opus/ -name "*.c")
endif
OPUS_OBJECTS   := $(subst .c,.o,$(obj-y))
OPUS_DEPENDS   := $(subst .c,.d,$(obj-y))
OPUS_INC_FLAGS := $(addprefix -I,./opus/include)
OPUS_INC_FLAGS += $(addprefix -I,./opus/celt)
OPUS_INC_FLAGS += $(addprefix -I,./opus/silk)
###########################################################################
.PHONY:

all: $(LIBS-y)
###########################################################################
libopus.prep:
	@echo "\033[31;36mlibopus.a compiling..\033[0m"

libopus.a: $(OPUS_OBJECTS)
	@$(AR) $(ARFLAGS) $@ $^

libopus.inst:
	@mv libopus.a      ../install/lib

ifneq ($(MAKECMDGOALS), clean)
-include $(OPUS_DEPENDS)
endif

$(OPUS_DEPENDS):%.d:%.c
	@$(CC) $(OPUS_INC_FLAGS) $(CCFLAGS) -MM $< > $@.$$$$;\
	sed 's,\(.*\)\.o[ :]*,$(shell dirname $@)/\1.o $@ : ,g' <$@.$$$$ > $@;\
	rm -f $@.$$$$

$(OPUS_OBJECTS):%.o:%.c
	@echo $@
	@$(CC) $(OPUS_INC_FLAGS) $(CCFLAGS) -c $< -o $@
###########################################################################

clean:
	@find ./ -name "*.o"|xargs rm -f
	@find ./ -name "*.d"|xargs rm -f
