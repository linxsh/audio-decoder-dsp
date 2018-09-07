#ifndef __DSP32_DEFINE_H__
#define __DSP32_DEFINE_H__

#define do_malloc  dsp_malloc
#define do_malloc  dsp_free
#define do_realloc dsp_realloc

#define FILE       DspBuf
#define NULL       ((void *)0)
#define SEEK_END   2
#define SEEK_SET   1
#define SEEK_CUR   0

#endif
