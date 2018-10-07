#include "os_printf.h"
#include "os_timer.h"
#include "register.h"
#include "log.h"

int startup(void)
{
	ExtraReadReg  *rCtrl  = reg_get_extra_read_addr ();
	ExtraWriteReg *wCtrl  = reg_get_extra_write_addr();
	ExtraDebugReg *dCtrl  = reg_get_extra_debug_addr();
	unsigned int runFreq = reg_get_run_frequency();

	os_printf_init(runFreq);
	os_timer_init (runFreq);

	log_printf(COMMON_MODULE, LEVEL_INFO, "version:\n");
	log_printf(COMMON_MODULE, LEVEL_INFO, "date: %s %s\n", __DATE__, __TIME__);
	log_printf(COMMON_MODULE, LEVEL_INFO, "run addr: 0x%p\n", reg_get_run_space_addr());
	log_printf(COMMON_MODULE, LEVEL_INFO, "run size: %d\n", reg_get_run_space_size());
	log_printf(COMMON_MODULE, LEVEL_INFO, "run freq: 0x%x\n", runFreq);

	reg_set_task_work(1);
	reg_set_task_queue(REG_TASK_QUEUE0|REG_TASK_QUEUE1, 0xf);
	reg_set_interrupt_type(REG_RESET_OK);

#ifdef CONFIG_DSP32
	void *mallocAddress = CONFIG_MALLOC_BASE_ADDR;
#else
	void *mallocAddress = reg_get_run_space_addr();
#endif

	os_malloc_init(mallocAddress);
	log_printf(COMMON_MODULE, LEVEL_INFO, "malloc addr: 0x%p\n", mallocAddress);
#ifdef CONFIG_DSP32
	log_printf(COMMON_MODULE, LEVEL_INFO, "temp beg addr: 0x%x\n", ((unsigned int)(&temp_beg)));
	log_printf(COMMON_MODULE, LEVEL_INFO, "temp end addr: 0x%x\n", ((unsigned int)(&temp_end)));
	log_printf(COMMON_MODULE, LEVEL_INFO, "data beg addr: 0x%x\n", ((unsigned int)(&data_beg)));
	log_printf(COMMON_MODULE, LEVEL_INFO, "data end addr: 0x%x\n", ((unsigned int)(&data_end)));
	if ((((int)(&temp_end)) - ((int)(&temp_beg))) < (((int)(&data_end))-((int)(&data_beg)))) {
		while (1) {
			unsigned int delay = 50000;
			log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: not match\n", __FUNCTION__, __LINE__);
			while (delay > 0) delay--;
		}
	}
#endif

	return 0;
}
