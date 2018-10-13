#include "os_define.h"
#include "register.h"
#include "log_printf.h"

static GeneralReg *sReg  = NULL;
static IsrFunc     sFunc = NULL;

#ifndef CONFIG_DSP32
static os_pthread_mutex_t regMutex;
static os_pthread_t       regTid;
#endif

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

#define REG_SET_VAL(reg, value) do {                                 \
	(*(volatile unsigned int *)reg) = value;                         \
} while(0)

#define REG_GET_VAL(reg)                                             \
	(*(volatile unsigned int *)reg)

#define REG_SET_FIELD(reg, val, c, offset) do {                      \
	unsigned int Reg = *(volatile unsigned int *)reg;                \
	Reg &= ~((sMask[c])<<(offset));                                  \
	Reg |= ((val)&(sMask[c]))<<(offset);                             \
	(*(volatile unsigned int *)reg) = Reg;                           \
} while(0)

#define REG_GET_FIELD(reg, c, offset)                                \
	(((*(volatile unsigned int *)reg)>>(offset))&(sMask[c]))

#define REG_SET_FIELD_MASK(reg, val, c, offset, c1, offset1) do {    \
	unsigned int Reg = *(volatile unsigned int *)reg;                \
	Reg &= ~(((sMask[c])<<(offset))|((sMask[c1])<<(offset1)));       \
	Reg |= ((val)&(sMask[c]))<<(offset);                             \
	(*(volatile unsigned int *)reg) = Reg;                           \
} while(0)

void register_init(GeneralReg *reg, IsrFunc func)
{
	if (NULL != sReg) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: sReg(%p)\n", __FUNCTION__, __LINE__, sReg);
		return;
	}

#ifndef CONFIG_DSP32
	os_pthread_mutex_init(&regMutex, NULL);
#endif
	sReg  = reg;
	sFunc = func;
	return;
}

void register_destroy(void)
{
#ifndef CONFIG_DSP32
	os_pthread_mutex_destroy(&regMutex);
#endif
	sReg  = NULL;
	sFunc = NULL;
	return;
}

void reg_enable_run(void)
{
#ifdef CONFIG_DSP32
	REG_SET_FIELD(&(sReg->RUN_ENABLE), 1, 1, 0);
#else
	extern int start_up(void);
	os_pthread_create(&regTid, NULL, (void*)start_up, NULL);
#endif
}

void reg_disable_run(void)
{
#ifdef CONFIG_DSP32
	REG_SET_FIELD_MASK(&(sReg->RUN_CTRL), 1, 1, 24, 1, 31);
#else
	REG_SET_FIELD(&(sReg->RUN_CTRL), 1, 1, 24);
	os_pthread_join(regTid, NULL);
#endif
}

unsigned int reg_disable_get(void)
{
	return REG_GET_FIELD(&(sReg->RUN_CTRL), 1, 24);
}

void reg_enable_task(void)
{
#ifdef CONFIG_DSP32
	REG_SET_FIELD(&(sReg->RUN_CTRL), 1, 1, 31);
#else
	unsigned int val0 = 0;
	unsigned int val1 = 0;

	os_pthread_mutex_lock(&regMutex);
	val0 = REG_GET_FIELD(&(sReg->TASK_QUEQUE), 4, 0);
	val1 = REG_GET_FIELD(&(sReg->TASK_QUEQUE), 4, 4);

	if ((val0 != 0) && (val1 != 0)) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: val0(%d) val1(%d)\n",
				__FUNCTION__, __LINE__, val0, val1);
	} else {
		unsigned int val = REG_GET_FIELD(&(sReg->RUN_CTRL), 4, 0);

		 if ((val0 == 0) && (val1 != 0)) {
			log_printf(COMMON_MODULE, LEVEL_WARN,
					"%s %d: val0(%d) val1(%d)\n",
					__FUNCTION__, __LINE__, val0, val1);
			REG_SET_FIELD(&(sReg->TASK_QUEQUE), val1, 4, 0);
			REG_SET_FIELD(&(sReg->TASK_QUEQUE), val,  4, 4);
		 } else if ((val0 != 0) && (val1 == 0)) {
			REG_SET_FIELD(&(sReg->TASK_QUEQUE), val,  4, 4);
		 } else if ((val0 == 0) && (val1 == 0)) {
			REG_SET_FIELD(&(sReg->TASK_QUEQUE), val,  4, 0);
		 }
	}
	os_pthread_mutex_unlock(&regMutex);
#endif
}

void reg_clear_task(TaskQueue queue)
{
#ifdef CONFIG_DSP32
	if (queue & TASK_QUEQUE_0)
		REG_SET_FIELD(&(sReg->TASK_QUEQUE), 0xf, 4, 0);

	if (queue & TASK_QUEQUE_1)
		REG_SET_FIELD(&(sReg->TASK_QUEQUE), 0xf, 4, 4);
#else
	unsigned int val0 = 0;
	unsigned int val1 = 0;

	os_pthread_mutex_lock(&regMutex);
	val0 = REG_GET_FIELD(&(sReg->TASK_QUEQUE), 4, 0);
	val1 = REG_GET_FIELD(&(sReg->TASK_QUEQUE), 4, 4);

	if ((val0 == 0) && (val1 == 0)) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: val0(%d) val1(%d)\n",
				__FUNCTION__, __LINE__, val0, val1);
	} else if ((val0 == 0) && (val1 != 0)) {
		log_printf(COMMON_MODULE, LEVEL_WARN,
				"%s %d: val0(%d) val1(%d)\n",
				__FUNCTION__, __LINE__, val0, val1);
		REG_SET_FIELD(&(sReg->TASK_QUEQUE), 0, 4, 4);
	} else if ((val0 != 0) && (val1 == 0)) {
		REG_SET_FIELD(&(sReg->TASK_QUEQUE), 0, 4, 0);
	} else if ((val0 != 0) && (val1 != 0)) {
		REG_SET_FIELD(&(sReg->TASK_QUEQUE), val1, 4, 0);
		REG_SET_FIELD(&(sReg->TASK_QUEQUE), 0,    4, 4);
	}
	os_pthread_mutex_unlock(&regMutex);
#endif
}

/***************************************************************/
/************************ ISR  Option **************************/
/***************************************************************/
void reg_set_task_isr(IsrType isr)
{
#ifdef CONFIG_DSP32
#else
	unsigned int isrEn  = REG_GET_VAL(&(sReg->INTEN));
	unsigned int oldIsr = REG_GET_VAL(&(sReg->INT));
	unsigned int newIsr = (isr | oldIsr);

	if (isr & oldIsr) {
		log_printf(COMMON_MODULE, LEVEL_ERRO,
				"%s %d: isr(%x) oldIsr(%x)\n",
				__FUNCTION__, __LINE__, isr, oldIsr);
	}

	REG_SET_VAL(&(sReg->INT), newIsr);

	if (isrEn & newIsr) {
		if (sFunc) sFunc();
	}
#endif
}

void reg_clr_task_isr(IsrType isr)
{
#ifdef CONFIG_DSP32
	REG_SET_VAL(&(sReg->INT), isr);
#else
	unsigned int oldIsr = REG_GET_VAL(&(sReg->INT));
	unsigned int newIsr = (oldIsr & (~isr));

	REG_SET_VAL(&(sReg->INT), newIsr);
#endif
}

void reg_clr_all_task_isr(void)
{
#ifdef CONFIG_DSP32
	REG_SET_VAL(&(sReg->INT), 0xffffffff);
#else
	REG_SET_VAL(&(sReg->INT), 0x0);
#endif
}

unsigned int reg_get_task_isr(void)
{
	return REG_GET_VAL(&(sReg->INT));
}

void reg_set_task_isr_en(IsrType isr)
{
	unsigned int isrEn = REG_GET_VAL(&(sReg->INTEN));

	REG_SET_VAL(&(sReg->INTEN), (isr | isrEn));
}

void reg_clr_task_isr_en(IsrType isr)
{
	unsigned int isrEn = REG_GET_VAL(&(sReg->INTEN));

	REG_SET_VAL(&(sReg->INTEN), ((~isr) & isrEn));
}

void reg_clr_all_task_isr_en(void)
{
	REG_SET_VAL(&(sReg->INTEN), 0x0);
}

unsigned int reg_get_task_isr_en(void)
{
	return REG_GET_VAL(&(sReg->INTEN));
}

/***************************************************************/
/*************** General Register Set Option *******************/
/***************************************************************/
unsigned int reg_get_run_frequency(void)
{
	return REG_GET_VAL(&(sReg->RUN_FREQUENCY));
}

unsigned char* reg_get_run_space_addr(void)
{
	unsigned int baseAddr = REG_GET_VAL(&(sReg->RUN_SPACE_ADDR)) & 0xfffffffe;

#ifdef CONFIG_DSP32
	return (unsigned char*)(baseAddr);
#else
	unsigned int highBaseAddr = REG_GET_VAL(&(sReg->EXT0));
	return (unsigned char*)(((unsigned long long)highBaseAddr << 32) | baseAddr);
#endif
}

unsigned int reg_get_run_space_size(void)
{
	return REG_GET_VAL(&(sReg->RUN_SPACE_SIZE));
}

TaskNo reg_get_task_no(void)
{
	unsigned int val0 = 0;
	unsigned int val1 = 0;

#ifdef CONFIG_DSP32
	val0 = REG_GET_FIELD(&(sReg->TASK_QUEQUE), 4, 0);
	val1 = REG_GET_FIELD(&(sReg->TASK_QUEQUE), 4, 4);
#else
	os_pthread_mutex_lock(&regMutex);
	val0 = REG_GET_FIELD(&(sReg->TASK_QUEQUE), 4, 0);
	val1 = REG_GET_FIELD(&(sReg->TASK_QUEQUE), 4, 4);
	os_pthread_mutex_unlock(&regMutex);
#endif

	if ((val0 == 0) && (val1 != 0)) {
		log_printf(COMMON_MODULE, LEVEL_WARN,
				"%s %d: val0(%d) val1(%d)\n",
				__FUNCTION__, __LINE__, val0, val1);
		return (val1 & 0x3);
	} else
		return (val0 & 0x3);
}

TaskAction reg_get_task_action(void)
{
	unsigned int val0 = 0;
	unsigned int val1 = 0;

#ifdef CONFIG_DSP32
	val0 = REG_GET_FIELD(&(sReg->TASK_QUEQUE), 4, 0);
	val1 = REG_GET_FIELD(&(sReg->TASK_QUEQUE), 4, 4);
#else
	os_pthread_mutex_lock(&regMutex);
	val0 = REG_GET_FIELD(&(sReg->TASK_QUEQUE), 4, 0);
	val1 = REG_GET_FIELD(&(sReg->TASK_QUEQUE), 4, 4);
	os_pthread_mutex_unlock(&regMutex);
#endif

	if ((val0 == 0) && (val1 != 0)) {
		log_printf(COMMON_MODULE, LEVEL_WARN,
				"%s %d: val0(%d) val1(%d)\n",
				__FUNCTION__, __LINE__, val0, val1);
		return (val1 & 0xc);
	} else
		return (val0 & 0xc);
}

unsigned int reg_get_task_id(TaskNo no)
{
	if (no == TASK_NO_0)
		return REG_GET_FIELD(&(sReg->RUN_CTRL), 4, 8);
	else if (no == TASK_NO_1)
		return REG_GET_FIELD(&(sReg->RUN_CTRL), 4, 12);
	else
		return 0;
}

TaskDo reg_get_task_do(TaskNo no)
{
	if (no == TASK_NO_0)
		return REG_GET_FIELD(&(sReg->RUN_CTRL), 4, 16);
	else if (no == TASK_NO_1)
		return REG_GET_FIELD(&(sReg->RUN_CTRL), 4, 20);
	else
		return 0;
}

/***************************************************************/
/*************** General Register Get Option *******************/
/***************************************************************/
void reg_set_run_frequency (unsigned int freq)
{
	REG_SET_VAL(&(sReg->RUN_FREQUENCY), freq);
}

void reg_set_run_space_addr(unsigned char *addr)
{
#ifdef CONFIG_DSP32
	unsigned int baseAddr = (unsigned int)addr;
	REG_SET_VAL(&(sReg->RUN_SPACE_ADDR), baseAddr);
#else
	unsigned int highBaseAddr = (unsigned int)(((unsigned long long)addr) >> 32);
	unsigned int baseAddr     = (unsigned int)(((unsigned long long)addr) & 0xfffffffff);
	REG_SET_VAL(&(sReg->EXT0), highBaseAddr);
	REG_SET_VAL(&(sReg->RUN_SPACE_ADDR), baseAddr);
#endif
}

void reg_set_run_space_size(unsigned int size)
{
	REG_SET_VAL(&(sReg->RUN_SPACE_SIZE), size);
}

/* ##################################################################
 * #################  Ext Register Option  ########################
 * ##################################################################*/
ExtReadReg *reg_get_ext_read_reg(TaskNo no)
{
	unsigned int readAddr = 0;

	if (TASK_NO_0 == no)
		readAddr = REG_GET_VAL(&(sReg->EXT_R_TO_TASK0));
	else if (TASK_NO_1 == no)
		readAddr = REG_GET_VAL(&(sReg->EXT_R_TO_TASK1));

#ifdef CONFIG_DSP32
	unsigned int baseAddr = REG_GET_VAL(&(sReg->RUN_SPACE_ADDR)) & 0xfffffffe;

	return (ExtReadReg *)((readAddr - baseAddr)|CONFIG_NO_CACHE_ADDR);
#else
	unsigned int highBaseAddr = 0;

	if (TASK_NO_0 == no)
		highBaseAddr = REG_GET_VAL(&(sReg->EXT1));
	else if (TASK_NO_1 == no)
		highBaseAddr = REG_GET_VAL(&(sReg->EXT2));

	return (ExtReadReg *)(((unsigned long long)highBaseAddr << 32) | readAddr);
#endif
}

ExtWriteReg *reg_get_ext_write_reg(TaskNo no)
{
	unsigned int writeAddr = 0;

	if (TASK_NO_0 == no)
		writeAddr = REG_GET_VAL(&(sReg->EXT_W_TO_TASK0));
	else if (TASK_NO_1 == no)
		writeAddr = REG_GET_VAL(&(sReg->EXT_W_TO_TASK1));

#ifdef CONFIG_DSP32
	unsigned int baseAddr = REG_GET_VAL(&(sReg->RUN_SPACE_ADDR)) & 0xfffffffe;
	return (ExtWriteReg *)((writeAddr - baseAddr)|CONFIG_NO_CACHE_ADDR);
#else
	unsigned int highBaseAddr = 0;

	if (TASK_NO_0 == no)
		highBaseAddr = REG_GET_VAL(&(sReg->EXT1));
	else if (TASK_NO_1 == no)
		highBaseAddr = REG_GET_VAL(&(sReg->EXT2));
	return (ExtWriteReg *)(((unsigned long long)highBaseAddr << 32) | writeAddr);
#endif
}

ExtDebugReg *reg_get_ext_debug_reg(TaskNo no)
{
	unsigned int debugAddr = 0;

	if (TASK_NO_0 == no)
		debugAddr = REG_GET_VAL(&(sReg->EXT_D_TO_TASK0));
	else if (TASK_NO_1 == no)
		debugAddr = REG_GET_VAL(&(sReg->EXT_D_TO_TASK1));

#ifdef CONFIG_DSP32
	unsigned int baseAddr = REG_GET_VAL(&(sReg->RUN_SPACE_ADDR)) & 0xfffffffe;

	return (ExtDebugReg *)((debugAddr - baseAddr)|CONFIG_NO_CACHE_ADDR);
#else
	unsigned int highBaseAddr = 0;

	if (TASK_NO_0 == no)
		highBaseAddr = REG_GET_VAL(&(sReg->EXT1));
	else if (TASK_NO_1 == no)
		highBaseAddr = REG_GET_VAL(&(sReg->EXT2));

	return (ExtDebugReg *)(((unsigned long long)highBaseAddr << 32) | debugAddr);
#endif
}

ExtBufferReg *reg_get_ext_buffer_reg(TaskNo no)
{
	unsigned int bufferAddr = 0;

	if (TASK_NO_0 == no)
		bufferAddr = REG_GET_VAL(&(sReg->EXT_B_TO_TASK0));
	else if (TASK_NO_1 == no)
		bufferAddr = REG_GET_VAL(&(sReg->EXT_B_TO_TASK1));

#ifdef CONFIG_DSP32
	unsigned int baseAddr = REG_GET_VAL(&(sReg->RUN_SPACE_ADDR)) & 0xfffffffe;

	return (ExtBufferReg *)((bufferAddr - baseAddr)|CONFIG_NO_CACHE_ADDR);
#else
	unsigned int highBaseAddr = 0;

	if (TASK_NO_0 == no)
		highBaseAddr = REG_GET_VAL(&(sReg->EXT1));
	else if (TASK_NO_1 == no)
		highBaseAddr = REG_GET_VAL(&(sReg->EXT2));

	return (ExtBufferReg *)(((unsigned long long)highBaseAddr << 32) | bufferAddr);
#endif
}

unsigned char* ext_reg_get_buf_addr(ExtBufferReg *reg, BufferIO io)
{
	unsigned int baseAddr = 0;

	if (BUFFER_I == io)
		baseAddr = REG_GET_VAL(&(reg->I_S_ADDR));
	else if (BUFFER_O == io)
		baseAddr = REG_GET_VAL(&(reg->O_S_ADDR));

#ifdef CONFIG_DSP32
	return (unsigned char*)(baseAddr);
#else
	unsigned int highBaseAddr = 0;

	if (BUFFER_I == io)
		highBaseAddr = REG_GET_VAL(&(reg->I_EXT));
	else if (BUFFER_O == io)
		highBaseAddr = REG_GET_VAL(&(reg->O_EXT));

	return (unsigned char*)(((unsigned long long)highBaseAddr << 32) | baseAddr);
#endif
}

unsigned int ext_reg_get_buf_size(ExtBufferReg *reg, BufferIO io)
{
	if (BUFFER_I == io)
		return REG_GET_VAL(&(reg->I_SIZE));
	else if (BUFFER_O == io)
		return REG_GET_VAL(&(reg->O_SIZE));

	return 0;
}

unsigned int ext_reg_get_buf_r_addr(ExtBufferReg *reg, BufferIO io)
{
	if (BUFFER_I == io)
		return REG_GET_VAL(&(reg->I_R_ADDR));
	else if (BUFFER_O == io)
		return REG_GET_VAL(&(reg->O_R_ADDR));

	return 0;
}

unsigned int ext_reg_get_buf_w_addr(ExtBufferReg *reg, BufferIO io)
{
	if (BUFFER_I == io)
		return REG_GET_VAL(&(reg->I_W_ADDR));
	else if (BUFFER_O == io)
		return REG_GET_VAL(&(reg->O_W_ADDR));

	return 0;
}

unsigned int ext_reg_get_buf_channel(ExtBufferReg *reg, BufferIO io)
{
	if (BUFFER_I == io)
		return REG_GET_VAL(&(reg->I_CHANNEL));
	else if (BUFFER_O == io)
		return REG_GET_VAL(&(reg->O_CHANNEL));

	return 0;
}

void ext_reg_set_buf_r_addr(ExtBufferReg *reg, BufferIO io, unsigned int addr)
{
	if (BUFFER_I == io)
		REG_SET_VAL(&(reg->I_R_ADDR), addr);
	else if (BUFFER_O == io)
		REG_SET_VAL(&(reg->O_R_ADDR), addr);
}

void ext_reg_set_buf_w_addr(ExtBufferReg *reg, BufferIO io, unsigned int addr)
{
	if (BUFFER_I == io)
		REG_SET_VAL(&(reg->I_W_ADDR), addr);
	else if (BUFFER_O == io)
		REG_SET_VAL(&(reg->O_W_ADDR), addr);
}

void ext_reg_set(unsigned int *reg, unsigned int value)
{
	REG_SET_VAL(reg, value);
}

unsigned int ext_reg_get(unsigned int *reg)
{
	return REG_GET_VAL(reg);
}
