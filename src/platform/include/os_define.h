#ifndef __OS_DEFINE_H__
#define __OS_DEFINE_H__

#include "config.h"

#ifdef CONFIG_X86
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#define uart_printf    printf
#define uart_va_list   va_list
#define uart_va_start  va_start
#define uart_va_end    va_end
#define uart_vprintf   vprintf
#endif

#endif
