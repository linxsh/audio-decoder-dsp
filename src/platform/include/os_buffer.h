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
	unsigned int   count;   //buffer个数, 至少一个buffer
	unsigned int   size;     //每个buffer的长度
	unsigned int   r_addr;   //相对地址,记录第一个buffer的读指针
	unsigned int   w_addr;   //相对地址,记录第一个buffer的写指针
} OsBufferHandle;

extern OsBufferHandle *os_buffer_open(OsBufferType type);

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

#endif
