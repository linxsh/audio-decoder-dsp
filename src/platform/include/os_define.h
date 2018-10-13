#ifndef __OS_DEFINE_H__
#define __OS_DEFINE_H__

#include <stddef.h>
#include <string.h>

#define os_memset   memset
#define os_memcpy   memcpy
#define os_memmove  memmove
#define os_memcmp   memcmp

#ifdef CONFIG_X86
#include <pthread.h>

typedef pthread_mutex_t    os_pthread_mutex_t;
typedef pthread_t          os_pthread_t;

#define os_pthread_mutex_init      pthread_mutex_init
#define os_pthread_mutex_destroy   pthread_mutex_destroy
#define os_pthread_mutex_lock      pthread_mutex_lock
#define os_pthread_mutex_unlock    pthread_mutex_unlock
#define os_pthread_create          pthread_create
#define os_pthread_join            pthread_join

#endif

#endif
