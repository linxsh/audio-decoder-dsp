CONFIG_FLAGS := -DCOMPILE_VERS=\"$(CONFIG_VERSION)\"
CONFIG_FLAGS += -DCOMPILE_DATE=\"$(shell date +%F)\$(shell date +%H:%M:%S)\"

#------------------------ platform ---------------------#
ifeq ($(CONFIG_X86), y)
CROSS_COMPILER= 
CROSS_COMPILER_PATH=
PLATFORM = x86
TARGETS  = x86.elf
CONFIG_FLAGS += -DCONFIG_X86
CROSS_COMPILER_PATCH =
CROSS_COMPILER =
endif

ifeq ($(CONFIG_DSP32), y)
CROSS_COMPILER= 
CROSS_COMPILER_PATH=
PLATFORM = dsp32
TARGETS  = dsp32.bin
CONFIG_FLAGS += -DCONFIG_DSP32
CROSS_COMPILER_PATCH = ../toolchains/or32-uclinux-3.4.4/bin/
CROSS_COMPILER = or32-uclinux-
endif

CC      = $(CROSS_COMPILER_PATCH)$(CROSS_COMPILER)gcc
CPP     = $(CROSS_COMPILER_PATCH)$(CROSS_COMPILER)g++
LD      = $(CROSS_COMPILER_PATCH)$(CROSS_COMPILER)ld
AS      = $(CROSS_COMPILER_PATCH)$(CROSS_COMPILER)as
AR      = $(CROSS_COMPILER_PATCH)$(CROSS_COMPILER)ar
OBJCOPY = $(CROSS_COMPILER_PATCH)$(CROSS_COMPILER)objcopy
OBJDUMP = $(CROSS_COMPILER_PATCH)$(CROSS_COMPILER)objdump
NM      = $(CROSS_COMPILER_PATCH)$(CROSS_COMPILER)nm
STRIP   = $(CROSS_COMPILER_PATCH)$(CROSS_COMPILER)strip
READELF = $(CROSS_COMPILER_PATCH)$(CROSS_COMPILER)readelf -a
#-------------------------------------------------------#

#------------------------ decoder ----------------------#
ifeq ($(CONFIG_VORBIS_DECODER), y)
DECODER += vorbis
CONFIG_LIBS  += libvorbis.a
CONFIG_FLAGS += -DCONFIG_VORBIS_DECODER
endif

ifeq ($(CONFIG_OPUS_DECODER), y)
DECODER += opus
CONFIG_LIBS  += libopus.a
CONFIG_FLAGS += -DCONFIG_OPUS_DECODER
endif
#-------------------------------------------------------#

#------------------------ encoder ----------------------#
ifeq ($(CONFIG_VORBIS_ENCODER), y)
ENCODER += vorbis
CONFIG_LIBS  += libvorbis.a
CONFIG_FLAGS += -DCONFIG_VORBIS_ENCODER
endif

ifeq ($(CONFIG_OPUS_ENCODER), y)
ENCODER += opus
CONFIG_LIBS  += libopus.a
CONFIG_FLAGS += -DCONFIG_OPUS_ENCODER
endif
#-------------------------------------------------------#
