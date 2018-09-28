#include "os_define.h"
#include "register.h"
#include "log.h"

static GeneralReg *sReg = NULL;

void register_init(GeneralReg *reg)
{
	if (NULL != sReg) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: sReg(%p)\n", __FUNCTION__, __LINE__, sReg);
		return;
	}

	sReg = reg;
	return;
}

void register_destory(void)
{
	sReg = NULL;
	return;
}

unsigned int reg_get_uart_frequency(void)
{
	return sReg->UART_FREQUENCY;
}

unsigned char* reg_get_run_space_addr(void)
{
	unsigned int baseAddr = sReg->RUN_SAPCE_S_ADDR;

#ifdef CONFIG_X86
	return (unsigned char*)(((unsigned long long)sReg->EXTEND_H_ADDR << 32) | baseAddr);
#else
	return (unsigned char*)(baseAddr);
#endif
}

unsigned int reg_get_run_space_size(void)
{
	return sReg->RUN_SAPCE_SIZE;
}

unsigned char* reg_get_buffer_addr(RegBufType type)
{
	unsigned int baseAddr = 0;

	if (type == REG_BUF_INPUT)
		baseAddr = sReg->IN_BUFFER_S_ADDR;
	else if (type == REG_BUF_OUTPUT)
		baseAddr = sReg->OUT_BUFFER_S_ADDR;

#ifdef CONFIG_X86
	return (unsigned char*)(((unsigned long long)sReg->EXTEND_H_ADDR << 32) | baseAddr);
#else
	return (unsigned char*)(baseAddr);
#endif
}

unsigned int reg_get_buffer_size(RegBufType type)
{
	if (type == REG_BUF_INPUT)
		return sReg->IN_BUFFER_SIZE;
	else if (type == REG_BUF_OUTPUT)
		return sReg->OUT_BUFFER_SIZE;

	return 0;
}

unsigned int reg_get_buffer_r_addr(RegBufType type)
{
	if (type == REG_BUF_INPUT)
		return sReg->IN_BUFFER_R_ADDR;
	else if (type == REG_BUF_OUTPUT)
		return sReg->OUT_BUFFER_R_ADDR;

	return 0;
}

unsigned int reg_get_buffer_w_addr(RegBufType type)
{
	if (type == REG_BUF_INPUT)
		return sReg->IN_BUFFER_W_ADDR;
	else if (type == REG_BUF_OUTPUT)
		return sReg->OUT_BUFFER_W_ADDR;

	return 0;
}

unsigned int reg_get_buffer_count(RegBufType type)
{
	return 0;
}

unsigned int reg_get_codec_type(void)
{
	return sReg->CODEC_TYPE;
}

unsigned int reg_set_interrupt_type(RegIntType type)
{
	return 0;
}

unsigned int reg_set_buffer_r_addr(RegBufType type, unsigned int addr)
{
	if (type == REG_BUF_INPUT)
		sReg->IN_BUFFER_R_ADDR = addr;
	else if (type == REG_BUF_OUTPUT)
		sReg->OUT_BUFFER_R_ADDR = addr;

	return 0;
}

unsigned int reg_set_buffer_w_addr(RegBufType type, unsigned int addr)
{
	if (type == REG_BUF_INPUT)
		sReg->IN_BUFFER_W_ADDR = addr;
	else if (type == REG_BUF_OUTPUT)
		sReg->OUT_BUFFER_W_ADDR = addr;

	return 0;
}

