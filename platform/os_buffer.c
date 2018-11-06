/*****************************************************************************
 * Author : linxsh
 * Type : Source
 * File Name : os_buffer.c
 * Describle : i/o buffer/fifo manager
 * Release History:
 * VERSION Date        AUTHOR    Description
 * 1.2-1   2018.09.24  linxsh    creation
 * ***************************************************************************/

#include "os_define.h"
#include "register.h"
#include "os_buffer.h"
#include "os_malloc.h"
#include "log_printf.h"

#ifdef CONFIG_X86
#include "../x86/x86_buffer.h"
#endif

#ifdef CONFIG_DSP32
#include "../dsp32/dsp32_buffer.h"
#endif

static void check_buffer(OsBufferHandle *handle)
{
	if (handle->addr != ext_reg_get_buf_addr(handle->reg, handle->io)) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: addr(%p, %p)\n",
				__FUNCTION__, __LINE__,
				handle->addr, ext_reg_get_buf_addr(handle->reg, handle->io));
	}

	if (handle->size != ext_reg_get_buf_size(handle->reg, handle->io)) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: size(%x, %x)\n",
				__FUNCTION__, __LINE__,
				handle->size, ext_reg_get_buf_size(handle->reg, handle->io));
	}

	if (handle->channel != ext_reg_get_buf_channel(handle->reg, handle->io)) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: channel(%d, %d)\n",
				__FUNCTION__, __LINE__,
				handle->channel, ext_reg_get_buf_channel(handle->reg, handle->io));
	}

	return;
}

OsBufferHandle *os_buffer_open(BufferIO io, ExtBufferReg *reg)
{
	OsBufferHandle *handle = os_malloc(sizeof(OsBufferHandle));

	if (NULL == handle) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: malloc error\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	os_memset(handle, 0, sizeof(OsBufferHandle));
	handle->io      = io;
	handle->reg     = reg;
	handle->addr    = ext_reg_get_buf_addr   (reg, io);
	handle->size    = ext_reg_get_buf_size   (reg, io);
	handle->channel = ext_reg_get_buf_channel(reg, io);

	return handle;
}

void os_buffer_close(OsBufferHandle *handle)
{
	os_free(handle);

	return;
}

int os_buffer_update_from(OsBufferHandle *handle, OsBufferAttr rw)
{
	check_buffer(handle);

	if (rw & OS_BUFFER_R_ATTR)
		handle->r_addr = ext_reg_get_buf_r_addr(handle->reg, handle->io);

	if (rw & OS_BUFFER_W_ATTR)
		handle->w_addr = ext_reg_get_buf_w_addr(handle->reg, handle->io);

	if ((rw & OS_BUFFER_R_ATTR) || (rw & OS_BUFFER_W_ATTR)) {
		handle->loop = ext_reg_get_buf_loop(handle->reg, handle->io);
		handle->eof  = ext_reg_get_buf_eof (handle->reg, handle->io);
	}

	return 0;
}

int os_buffer_update_to(OsBufferHandle *handle, OsBufferAttr rw)
{
	check_buffer(handle);

	if (rw & OS_BUFFER_R_ATTR)
		ext_reg_set_buf_r_addr(handle->reg, handle->io, handle->r_addr);

	if (rw & OS_BUFFER_W_ATTR)
		ext_reg_set_buf_w_addr(handle->reg, handle->io, handle->w_addr);

	if ((rw & OS_BUFFER_R_ATTR) || (rw & OS_BUFFER_W_ATTR)) {
		ext_reg_set_buf_loop(handle->reg, handle->io, handle->loop);
		ext_reg_set_buf_eof (handle->reg, handle->io, handle->eof);
	}

	return 0;
}

OsBufferState os_buffer_check(OsBufferHandle *handle,
		unsigned int length,
		unsigned int *freeLength,
		unsigned int *fillLength)
{
	unsigned int freeLen, fillLen;
	check_buffer(handle);

	if (length > handle->size) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: length(%d) size(%d)\n",
				__FUNCTION__, __LINE__, length, handle->size);
		return OS_BUFFER_ERROR;
	}

	if (handle->loop)
		fillLen = handle->size;
	else if (handle->w_addr >= handle->r_addr)
		fillLen = handle->w_addr - handle->r_addr;
	else
		fillLen = handle->size + handle->w_addr - handle->r_addr;

	freeLen = handle->size - fillLen;
	if (freeLength)
		*freeLength = freeLen;
	if (fillLength)
		*fillLength = fillLen;

	if (BUFFER_I == handle->io) {
		if (length > fillLen)
			return (handle->eof) ? OS_BUFFER_DATA_EOF : OS_BUFFER_DATA_LESS;
	} else if (BUFFER_O == handle->io) {
		if (length > freeLen)
			return OS_BUFFER_DATA_FULL;
	}

	return OS_BUFFER_OK;
}

int os_buffer_read(OsBufferHandle *handle,
		unsigned int index,
		unsigned int update,
		unsigned char *buffer,
		unsigned int length)
{
	unsigned int sAddr = 0, eAddr = 0, rAddr = 0;

	if (index >= handle->channel) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: index(%d) channel(%d)\n",
				__FUNCTION__, __LINE__, index, handle->channel);
		return -1;
	}

	sAddr = handle->size * index;
	eAddr = sAddr + handle->size;
	rAddr = sAddr + handle->r_addr;
#ifdef CONFIG_X86
	x86_buffer_read(buffer, handle->addr, sAddr, eAddr, rAddr, length);
#endif
#ifdef CONFIG_DSP32
	dsp32_buffer_read(buffer, handle->addr, sAddr, eAddr, rAddr, length);
#endif

	if (update) {
		unsigned int tmp = handle->r_addr + length;
		handle->r_addr = (tmp >= handle->size) ? (tmp - handle->size) : tmp;
		handle->loop   = (length > 0) ? 0 : handle->loop;
	}

	return 0;
}

int os_buffer_write(OsBufferHandle *handle,
		unsigned int index,
		unsigned int update,
		unsigned char *buffer,
		unsigned int length)
{
	unsigned int sAddr = 0, eAddr = 0, wAddr = 0;

	if (index >= handle->channel) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: index(%d) channel(%d)\n",
				__FUNCTION__, __LINE__, index, handle->channel);
		return -1;
	}

	sAddr = handle->size * index;
	eAddr = sAddr + handle->size;
	wAddr = sAddr + handle->w_addr;
#ifdef CONFIG_X86
	x86_buffer_write(buffer, handle->addr, sAddr, eAddr, wAddr, length);
#endif
#ifdef CONFIG_DSP32
	dsp32_buffer_write(buffer, handle->addr, sAddr, eAddr, wAddr, length);
#endif

	if (update) {
		unsigned int tmp = handle->w_addr + length;
		handle->w_addr = (tmp >= handle->size) ? (tmp - handle->size) : tmp;
		handle->loop   = ((handle->r_addr == handle->w_addr) && (length > 0)) ? 1 : handle->loop;
	}

	return 0;
}

unsigned int os_buffer_get_channel(OsBufferHandle *handle)
{
	return handle->channel;
}
