#include "os_define.h"
#include "register.h"
#include "log.h"

static GeneralReg *sReg = NULL;

static unsigned int sMask[33] = {
	0x00000000,
	0x00000001, 0x00000003, 0x00000007, 0x0000000f,
	0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff,
	0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,
	0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,
	0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff,
	0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
	0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff,
	0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
};

#define REG_SET_VAL(reg, value) do {                                \
	(*(volatile unsigned int *)reg) = value;                        \
} while(0)

#define REG_GET_VAL(reg)                                            \
	(*(volatile unsigned int *)reg)

#define REG_SET_FIELD(reg, val, c, offset) do {                     \
	unsigned int Reg = *(volatile unsigned int *)reg;               \
	Reg &= ~((sMask[c])<<(offset));                                  \
	Reg |= ((val)&(sMask[c]))<<(offset);                             \
	(*(volatile unsigned int *)reg) = Reg;                          \
} while(0)

#define REG_GET_FIELD(reg, c, offset)                               \
	(((*(volatile unsigned int *)reg)>>(offset))&(sMask[c]))

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

/***************************************************************/
/*************** General Register Get Option *******************/
/***************************************************************/
unsigned int reg_get_codec_type(void)
{
	return REG_GET_VAL(&(sReg->CODEC_TYPE));
}

unsigned int reg_get_run_frequency(void)
{
	return REG_GET_VAL(&(sReg->RUN_FREQUENCY));
}

unsigned char* reg_get_run_space_addr(void)
{
	unsigned int baseAddr = REG_GET_VAL(&(sReg->RUN_SAPCE_S_ADDR)) & 0xfffffffe;

#ifdef CONFIG_DSP32
	return (unsigned char*)(baseAddr);
#else
	unsigned int highBaseAddr = REG_GET_VAL(&(sReg->EXT0));
	return (unsigned char*)(((unsigned long long)highBaseAddr << 32) | baseAddr);
#endif
}

unsigned int reg_get_run_space_size(void)
{
	return REG_GET_VAL(&(sReg->RUN_SAPCE_SIZE));
}

unsigned char* reg_get_buffer_addr(RegBufType type)
{
	unsigned int baseAddr = 0;

	if (type == REG_BUF_INPUT)
		baseAddr = REG_GET_VAL(&(sReg->IN_BUFFER_S_ADDR));
	else if (type == REG_BUF_OUTPUT)
		baseAddr = REG_GET_VAL(&(sReg->OUT_BUFFER_S_ADDR));

#ifdef CONFIG_DSP32
	return (unsigned char*)(baseAddr);
#else
	unsigned int highBaseAddr = 0;
	if (type == REG_BUF_INPUT)
		highBaseAddr = REG_GET_VAL(&(sReg->EXT1));
	else if (type == REG_BUF_OUTPUT)
		highBaseAddr = REG_GET_VAL(&(sReg->EXT2));
	return (unsigned char*)(((unsigned long long)highBaseAddr << 32) | baseAddr);
#endif
}

unsigned int reg_get_buffer_size(RegBufType type)
{
	if (type == REG_BUF_INPUT)
		return REG_GET_VAL(&(sReg->IN_BUFFER_SIZE));
	else if (type == REG_BUF_OUTPUT)
		return REG_GET_VAL(&(sReg->OUT_BUFFER_SIZE));

	return 0;
}

unsigned int reg_get_buffer_r_addr(RegBufType type)
{
	if (type == REG_BUF_INPUT)
		return REG_GET_VAL(&(sReg->IN_BUFFER_R_ADDR));
	else if (type == REG_BUF_OUTPUT)
		return REG_GET_VAL(&(sReg->OUT_BUFFER_R_ADDR));

	return 0;
}

unsigned int reg_get_buffer_w_addr(RegBufType type)
{
	if (type == REG_BUF_INPUT)
		return REG_GET_VAL(&(sReg->IN_BUFFER_W_ADDR));
	else if (type == REG_BUF_OUTPUT)
		return REG_GET_VAL(&(sReg->OUT_BUFFER_W_ADDR));

	return 0;
}

unsigned int reg_get_buffer_count(RegBufType type)
{
	return 0;
}

/***************************************************************/
/*************** General Register Set Option *******************/
/***************************************************************/
void reg_set_buffer_r_addr(RegBufType type, unsigned int addr)
{
	if (type == REG_BUF_INPUT)
		REG_SET_VAL(&(sReg->IN_BUFFER_R_ADDR),  addr);
	else if (type == REG_BUF_OUTPUT)
		REG_SET_VAL(&(sReg->OUT_BUFFER_R_ADDR), addr);
}

void reg_set_buffer_w_addr(RegBufType type, unsigned int addr)
{
	if (type == REG_BUF_INPUT)
		REG_SET_VAL(&(sReg->IN_BUFFER_W_ADDR),  addr);
	else if (type == REG_BUF_OUTPUT)
		REG_SET_VAL(&(sReg->OUT_BUFFER_W_ADDR), addr);
}

void reg_set_interrupt_type(RegIntType type)
{
	unsigned int intEn = REG_GET_VAL(&(sReg->INTEN));

	REG_SET_VAL(&(sReg->INT), type);

	if (intEn & type) {
		//do some things
	}
}

void reg_set_task_work(unsigned int enable)
{
	REG_SET_FIELD(&(sReg->RUN_CTRL), enable, 1, 31);
}

void reg_set_task_queue(RegTaskQueue queue, unsigned int value)
{
	if (queue & REG_TASK_QUEUE0)
		REG_SET_FIELD(&(sReg->TASK_QUEQUE), value, 4, 0);

	if (queue & REG_TASK_QUEUE1)
		REG_SET_FIELD(&(sReg->TASK_QUEQUE), value, 4, 4);
}

/* ##################################################################
 * #################  Extra Register Option  ########################
 * ##################################################################*/
ExtraReadReg *reg_get_extra_read_addr(void)
{
	unsigned int readAddr = REG_GET_VAL(&(sReg->EXTRA_R_TO_SDRAM));

#ifdef CONFIG_DSP32
	unsigned int baseAddr = REG_GET_VAL(&(sReg->RUN_SAPCE_S_ADDR)) & 0xfffffffe;
	return (ExtraReadReg *)((readAddr - baseAddr)|CONFIG_NO_CACHE_ADDR);
#else
	unsigned int highBaseAddr = REG_GET_VAL(&(sReg->EXT0));
	return (ExtraReadReg *)(((unsigned long long)highBaseAddr << 32) | readAddr);
#endif
}

ExtraWriteReg *reg_get_extra_write_addr(void)
{
	unsigned int writeAddr = REG_GET_VAL(&(sReg->EXTRA_W_TO_SDRAM));

#ifdef CONFIG_DSP32
	unsigned int baseAddr = REG_GET_VAL(&(sReg->RUN_SAPCE_S_ADDR)) & 0xfffffffe;
	return (ExtraWriteReg *)((writeAddr - baseAddr)|CONFIG_NO_CACHE_ADDR);
#else
	unsigned int highBaseAddr = REG_GET_VAL(&(sReg->EXT0));
	return (ExtraWriteReg *)(((unsigned long long)highBaseAddr << 32) | writeAddr);
#endif
}

ExtraDebugReg *reg_get_extra_debug_addr(void)
{
	unsigned int debugAddr = REG_GET_VAL(&(sReg->EXTRA_D_TO_SDRAM));

#ifdef CONFIG_DSP32
	unsigned int baseAddr = REG_GET_VAL(&(sReg->RUN_SAPCE_S_ADDR)) & 0xfffffffe;
	return (ExtraDebugReg *)((debugAddr - baseAddr)|CONFIG_NO_CACHE_ADDR);
#else
	unsigned int highBaseAddr = REG_GET_VAL(&(sReg->EXT0));
	return (ExtraDebugReg *)(((unsigned long long)highBaseAddr << 32) | debugAddr);
#endif
}
