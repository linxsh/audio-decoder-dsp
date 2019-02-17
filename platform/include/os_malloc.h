#ifndef __OS_MALLOC_H__
#define __OS_MALLOC_H__

extern void  os_malloc_init(void *address);
extern void *os_malloc     (unsigned int size);
extern void *os_realloc    (void *address, unsigned int size);
extern void  os_free       (void *address);

#endif
