#ifndef __OS_PRINTF_H__
#define __OS_PRINTF_H__

#ifdef CONFIG_X86
#include <stdio.h>
#include <stdarg.h>

#define os_printf    printf
#define os_va_list   va_list
#define os_va_start  va_start
#define os_va_end    va_end
#define os_vprintf   vprintf
#endif

#ifdef CONFIG_DSP32
#endif

#endif
