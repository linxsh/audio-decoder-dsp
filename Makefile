RTCONFIG = ${GXSRC_PATH}/config
HDRCONFIG = ${GXSRC_PATH}/include/config.h
VERCONFIG = ${GXSRC_PATH}/include/version.h

include ${RTCONFIG}

############### common #################
mod-y := $(wildcard ./src/common/*.c)
mod-y := $(wildcard ./src/register/*.c)=

############### platform #################
mod-$(CONFIG_PLATFORM_X86) := $(wildcard ./src/demo/x86/*.c)
mod-$(CONFIG_PLATFORM_X86) := $(wildcard ./src/platform/x86/*.c)
mod-$(CONFIG_PLATFORM_X86) := $(wildcard ./src/driver/*.c)
mod-$(CONFIG_PLATFORM_DSP32) := $(wildcard ./src/demo/dsp32/*.c)
mod-$(CONFIG_PLATFORM_DSP32) := $(wildcard ./src/platform/dsp32/*.c)

############### decoder #################
mod-$(CONFIG_DECODER_OPUS) := $(wildcard ./src/decoder/opus/*.c)
mod-$(CONFIG_DECODER_OPUS) := $(wildcard ./src/libsrc/opus/*.c)

obj-y = $(patsubst %.c, %.o, $(mod-y))

#########################################
CFLAGS += -Winline -Wall
CFLAGS += -I$(GXSRC_PATH)/include
CFLAGS += -I$(GXSRC_PATH)/src/include

-include ./scripts/inc.Makefile

all: $(RTCONFIG) $(HDRCONFIG) $(VERCONFIG) $(obj-y) $(ELF)

$(HDRCONFIG): $(RTCONFIG) Makefile
	@echo "/*" > $@
