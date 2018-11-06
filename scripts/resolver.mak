#------------------------ platform ---------------------#
ifeq ($(CONFIG_X86), y)
PLATFORM = x86
TARGETS  = x86.elf
CONFIG_FLAGS += -DCONFIG_X86
endif

ifeq ($(CONFIG_DSP32), y)
PLATFORM = dsp32
TARGETS  = dsp32.bin
CONFIG_FLAGS += -DCONFIG_DSP32
endif

#-------------------------------------------------------#

#------------------------ decoder ----------------------#
ifeq ($(CONFIG_VORBIS_DECODER), y)
DECODER += vorbis
CONFIG_FLAGS += -DCONFIG_VORBIS_DECODER
endif

ifeq ($(CONFIG_OPUS_DECODER), y)
DECODER += opus
CONFIG_FLAGS += -DCONFIG_OPUS_DECODER
endif
#-------------------------------------------------------#

#------------------------ encoder ----------------------#
ifeq ($(CONFIG_VORBIS_ENCODER), y)
ENCODER += vorbis
CONFIG_FLAGS += -DCONFIG_VORBIS_ENCODER
endif

ifeq ($(CONFIG_OPUS_ENCODER), y)
ENCODER += opus
CONFIG_FLAGS += -DCONFIG_OPUS_ENCODER
endif
#-------------------------------------------------------#
