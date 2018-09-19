#------------------------ platform ---------------------#
ifeq ($(CONFIG_X86), y)
CROSS_COMPILER= 
CROSS_COMPILER_PATH=
PLATFORM = x86
TARGETS  = x86.elf
CONFIG_FLAGS := -DCONFIG_X86
endif

ifeq ($(CONFIG_DSP32), y)
CROSS_COMPILER= 
CROSS_COMPILER_PATH=
PLATFORM = dsp32
TARGETS  = dsp32.bin
CONFIG_FLAGS := -DCONFIG_DSP32
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
CONFIG_FLAGS += -DCONFIG_VORBIS_DECODER
CONFIG_LIBS  += libvorbis.a
endif

ifeq ($(CONFIG_OPUS_DECODER), y)
DECODER += opus
CONFIG_FLAGS += -DCONFIG_OPUS_DECODER
CONFIG_LIBS  += libopus.a
endif
#-------------------------------------------------------#

#-------------------------------------------------------#
#-------------------------------------------------------#
