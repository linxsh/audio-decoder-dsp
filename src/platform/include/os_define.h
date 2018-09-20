#ifndef __OS_DEFINE_H__
#define __OS_DEFINE_H__

#include <stddef.h>

#ifdef CONFIG_X86
#include <stdlib.h>
#include <string.h>

#define os_malloc  malloc
#define os_free    free
#define os_memset  memset
#endif

#endif
