#include "os_define.h"
#include "os_buffer.h"
#include "os_malloc.h"
#include "register.h"
#include "log.h"

#ifdef CONFIG_X86
#include "../x86/x86_buffer.h"
#endif

#ifdef CONFIG_DSP32
#include "../dsp32/dsp32_buffer.h"
#endif

static RegBufType switch_buffer_type(OsBufferType type)
{
	RegBufType regBufType = REG_BUF_INPUT;

	if (OS_BUFFER_INPUT == type)
		regBufType = REG_BUF_INPUT;
	else if (OS_BUFFER_OUTPUT == type)
		regBufType = REG_BUF_OUTPUT;

	return regBufType;
}

static void check_buffer(OsBufferHandle *handle)
{
	RegBufType regBufType = switch_buffer_type(handle->type);

	if (handle->addr != reg_get_buffer_addr(regBufType)) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: addr(%p, %p)\n",
				__FUNCTION__, __LINE__, handle->addr, reg_get_buffer_addr(regBufType));
	}

	if (handle->size != reg_get_buffer_size(regBufType)) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: size(%x, %x)\n",
				__FUNCTION__, __LINE__, handle->size, reg_get_buffer_size(regBufType));
	}

	if (handle->channel != reg_get_buffer_count(regBufType)) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: channel(%d, %d)\n",
				__FUNCTION__, __LINE__, handle->channel, reg_get_buffer_count(regBufType));
	}

	return;
}

OsBufferHandle *os_buffer_open(OsBufferType type)
{
	OsBufferHandle *handle = os_malloc(sizeof(OsBufferHandle));
	RegBufType regBufType = switch_buffer_type(type);

	if (NULL == handle) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: malloc error\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	os_memset(handle, 0, sizeof(OsBufferHandle));
	handle->type    = type;
	handle->addr    = reg_get_buffer_addr (regBufType);
	handle->size    = reg_get_buffer_size (regBufType);
	handle->channel = reg_get_buffer_count(regBufType);

	return handle;
}

void os_buffer_close(OsBufferHandle *handle)
{
	os_free(handle);

	return;
}

int os_buffer_update_from(OsBufferHandle *handle, OsBufferAttr rw)
{
	RegBufType regBufType = switch_buffer_type(handle->type);

	if (rw & OS_BUFFER_R_ATTR)
		handle->r_addr = reg_get_buffer_r_addr(regBufType);

	if (rw & OS_BUFFER_W_ATTR)
		handle->w_addr = reg_get_buffer_w_addr(regBufType);

	return 0;
}

int os_buffer_update_to(OsBufferHandle *handle, OsBufferAttr rw)
{
	unsigned rAddr = (handle->r_addr == 0) ? (handle->size - 1) : (handle->r_addr - 1);
	unsigned wAddr = (handle->w_addr == 0) ? (handle->size - 1) : (handle->w_addr - 1);
	RegBufType regBufType = switch_buffer_type(handle->type);

	if (rw & OS_BUFFER_R_ATTR)
		reg_set_buffer_r_addr(regBufType, rAddr);

	if (rw & OS_BUFFER_W_ATTR)
		reg_set_buffer_w_addr(regBufType, wAddr);

	return 0;
}

int os_buffer_check(OsBufferHandle *handle, unsigned int length)
{
	check_buffer(handle);

	if (length > handle->size) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: length(%d) size(%d)\n",
				__FUNCTION__, __LINE__, length, handle->size);
		return -1;
	}

	return 0;
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
	}

	return 0;
}
