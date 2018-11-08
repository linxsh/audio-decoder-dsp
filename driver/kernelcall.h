#ifndef __KERNEL_CALL_H__
#define __KERNEL_CALL_H__

#include <stddef.h>
#include <string.h>

#ifdef CONFIG_X86
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define gx_malloc(size)               malloc(size)
#define gx_free(p)                    free(p)
#define gx_page_malloc(size)          malloc(size)
#define gx_page_free(p, size)         free(p)
#define gx_virt_to_phys(p)            (p)
#define gx_dcache_clean_range(p0, p1) ((void)0)

#define gx_printf(fmt, args...)       printf(fmt, ##args)
#define gx_vprintf(fmt, args...)      vprintf(fmt, args)

#define gx_mdelay(ms)                 usleep((ms) * 1000)
#endif



#ifdef CONFIG_DSP32
#include <linux/slab.h>

#define gx_malloc(size)           kmalloc(size, GFP_KERNEL | __GFP_REPEAT)
#define gx_free(p)                kfree(p)

#define gx_printf(fmt, args...)   printk(fmt, ##args)
#define gx_vprintf(fmt, args...)  vprintk(fmt, args)

extern unsigned char *gx_page_malloc (unsigned long size);
extern void           gx_page_free   (unsigned char *p, unsigned long size);
extern unsigned char *gx_virt_to_phys(unsigned char *p);
#endif



#define gx_memset memset

extern void *gx_mallocz(unsigned int size);

#endif
