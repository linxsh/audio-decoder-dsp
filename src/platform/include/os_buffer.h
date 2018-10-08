#ifndef __OS_BUFFER_H__
#define __OS_BUFFER_H__

typedef enum {
	OS_BUFFER_INPUT  = 0,
	OS_BUFFER_OUTPUT = 1,
} OsBufferType;

typedef enum {
	OS_BUFFER_R_ATTR = (0x1 << 0),
	OS_BUFFER_W_ATTR = (0x1 << 1)
} OsBufferAttr;

typedef enum {
	OS_BUFFER_ERROR = -1,
	OS_BUFFER_NEED  = -2,
	OS_BUFFER_EMPTY = -3
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
	OsBufferType   type;
	unsigned char *addr;
	unsigned int   channel;   //buffer个数, 至少一个channel
	unsigned int   size;     //每个channel buffer的长度
	unsigned int   r_addr;   //相对地址,记录第一个channel buffer的读指针
	unsigned int   w_addr;   //相对地址,记录第一个channel buffer的写指针
} OsBufferHandle;

extern OsBufferHandle *os_buffer_open(OsBufferType type);

extern void os_buffer_close(OsBufferHandle *handle);

extern int os_buffer_check(OsBufferHandle *handle, unsigned int length);

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
#endif
