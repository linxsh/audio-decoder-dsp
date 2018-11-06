#ifndef __OS_BUFFER_H__
#define __OS_BUFFER_H__

#include "register.h"

typedef enum {
	OS_BUFFER_R_ATTR = (0x1 << 0),
	OS_BUFFER_W_ATTR = (0x1 << 1)
} OsBufferAttr;

typedef enum {
	OS_BUFFER_ERROR     = -1,
	OS_BUFFER_OK        = 0,
	OS_BUFFER_DATA_LESS = 1,
	OS_BUFFER_DATA_FULL = 2,
	OS_BUFFER_DATA_EOF  = 3
} OsBufferState;

/***************************************************
 **********  +1 表示即将操作的起始位置   ***********
 ***************** IN BUFFER ***********************
 ************** cpu ********* dsp ******************
 ********** (r + 1) -------->     ******************
 **********     (w) -------->     ******************
 **********         <-------- (r) ******************
 ***************** OUT BUFFER **********************
 ************** cpu ********* dsp ******************
 ********** (w + 1) -------->     ******************
 **********         <-------- (w) ******************
 ***************************************************/
typedef struct {
	BufferIO       io;
	ExtBufferReg  *reg;
	unsigned char *addr;
	unsigned int   channel;   //buffer个数, 至少一个channel
	unsigned int   size;     //每个channel buffer的长度
	unsigned int   r_addr;   //相对地址,记录第一个channel buffer的读指针
	unsigned int   w_addr;   //相对地址,记录第一个channel buffer的写指针
	unsigned int   loop;
	unsigned int   eof;
} OsBufferHandle;

extern OsBufferHandle *os_buffer_open(BufferIO io, ExtBufferReg *reg);

extern void os_buffer_close(OsBufferHandle *handle);

extern int os_buffer_check(OsBufferHandle *handle,
		unsigned int length,
		unsigned int *freeFrameBytes,
		unsigned int *fillFrameBytes);

extern int os_buffer_read(OsBufferHandle *handle,
		unsigned int index,
		unsigned int update,
		unsigned char *buffer,
		unsigned int length);

extern int os_buffer_write(OsBufferHandle *handle,
		unsigned int index,
		unsigned int update,
		unsigned char *buffer,
		unsigned int length);

extern int os_buffer_update_to  (OsBufferHandle *handle, OsBufferAttr rw);

extern int os_buffer_update_from(OsBufferHandle *handle, OsBufferAttr rw);

extern unsigned int os_buffer_get_channel(OsBufferHandle *handle);

#endif
