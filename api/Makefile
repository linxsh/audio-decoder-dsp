include ../.config
include ../scripts/resolver.mak
include ../scripts/compiler.mak

LIBS_OPUS = n
ifeq ($(CONFIG_OPUS_ENCODER), y)
LIBS_OPUS = y
endif
ifeq ($(CONFIG_OPUS_DECODER), y)
LIBS_OPUS = y
endif
###########################################################################
LIBS-y    :=
INC_FLAGS := $(addprefix -I,./include)
INC_FLAGS += $(addprefix -I,../platform/include)
ifeq ($(LIBS_OPUS), y)
INC_FLAGS += $(addprefix -I,../libsrc/opus/include)
endif

CCFLAGS   := $(INC_FLAGS) $(COMPILER_FLAGS) $(CONFIG_FLAGS) -O2 -pipe -Wall -c
ARFLAGS   := -rcs
###########################################################################
obj-y :=
obj-y += ./main.c
obj-y += ./list.c
obj-y += ./task.c

LIBS-y += libapi.prep
LIBS-y += libapi.a
LIBS-y += libapi.inst
API_OBJECTS := $(subst .c,.o,$(obj-y))
API_DEPENDS := $(subst .c,.d,$(obj-y))
###########################################################################
obj-y :=
obj-$(CONFIG_VORBIS_DECODER) += ./avcodec/vorbis_dec.c
obj-$(CONFIG_VORBIS_ENCODER) += ./avcodec/vorbis_enc.c
obj-$(CONFIG_OPUS_ENCODER)   += ./avcodec/opus_dec.c
obj-$(CONFIG_OPUS_DECODER)   += ./avcodec/opus_enc.c

LIBS-y += libavcodec.prep
LIBS-y += libavcodec.a
LIBS-y += libavcodec.inst
AVCODEC_OBJECTS := $(subst .c,.o,$(obj-y))
AVCODEC_DEPENDS := $(subst .c,.d,$(obj-y))
###########################################################################
obj-y :=
obj-y += ./avutils/framing.c
obj-y += ./avutils/opus_header.c

LIBS-y += libavutils.prep
LIBS-y += libavutils.a
LIBS-y += libavutils.inst
AVUTILS_OBJECTS := $(subst .c,.o,$(obj-y))
AVUTILS_DEPENDS := $(subst .c,.d,$(obj-y))
###########################################################################
.PHONY:

all: $(LIBS-y)
###########################################################################
libapi.prep:
	@echo "\033[31;36mlibapi.a compiling..\033[0m"

libapi.a: $(API_OBJECTS)
	@$(AR) $(ARFLAGS) $@ $^

libapi.inst:
	@mv libapi.a ../install/lib
###########################################################################
libavcodec.prep:
	@echo "\033[31;36mlibavcodec.a compiling..\033[0m"

libavcodec.a: $(AVCODEC_OBJECTS)
	@$(AR) $(ARFLAGS) $@ $^

libavcodec.inst:
	@mv libavcodec.a ../install/lib
###########################################################################
libavutils.prep:
	@echo "\033[31;36mlibavutils.a compiling..\033[0m"

libavutils.a: $(AVUTILS_OBJECTS)
	@$(AR) $(ARFLAGS) $@ $^

libavutils.inst:
	@mv libavutils.a ../install/lib
###########################################################################
ifneq ($(MAKECMDGOALS), clean)
-include $(API_DEPENDS)
-include $(AVCODEC_DEPENDS)
-include $(AVUTILS_DEPENDS)
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
