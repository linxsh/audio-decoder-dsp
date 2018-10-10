#ifndef __CHIP_H__
#define __CHIP_H__

#define CONFIG_MALLOC_BASE_ADDR ((((unsigned int)&bss_end) & (~0xf)) + 16)

#define CONFIG_REG_BASE_ADDR   0x34600100
#define CONFIG_TIMER_BASE_ADDR 0x340fc000

#define CONFIG_NO_CACHE_ADDR   0x20000000
#define CONFIG_QMEM_BASE_ADDR  0x1c000000

#endif
