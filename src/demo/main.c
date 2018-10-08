#include "os_printf.h"
#include "os_timer.h"
#include "os_buffer.h"
#include "os_malloc.h"
#include "register.h"
#include "log.h"
#include "format.h"
#include "task.h"

int startup(void)
{
	unsigned int runFreq = reg_get_run_frequency();

	os_printf_init(runFreq);
	os_timer_init (runFreq);

	log_printf(COMMON_MODULE, LEVEL_INFO, "version:\n");
	log_printf(COMMON_MODULE, LEVEL_INFO, "date: %s %s\n", __DATE__, __TIME__);
	log_printf(COMMON_MODULE, LEVEL_INFO, "run addr: 0x%p\n", reg_get_run_space_addr());
	log_printf(COMMON_MODULE, LEVEL_INFO, "run size: %d\n", reg_get_run_space_size());
	log_printf(COMMON_MODULE, LEVEL_INFO, "run freq: 0x%x\n", runFreq);

	reg_set_task_enable(1);
	reg_set_task_queue(REG_TASK_QUEUE0|REG_TASK_QUEUE1, 0xf);
	reg_set_interrupt_type(REG_INT_INIT_OK);

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

	while (1) {
		if (reg_get_request_reset()) {
			reg_set_interrupt_type(REG_INT_RESET_OK);
			while(1);
		}

		if (reg_get_task_enable()) {
			int    taskID = reg_get_task_id();
			TaskDo taskDo = reg_get_task_do();
			TaskStatus taskStatus = task_status(taskID);

			if (TASK_NOT_EXIST == taskStatus) {
				task_printf();
				continue;
			}

			if (TASK_DO_STOP == taskDo) {
				task_destory(taskID);
				continue;
			} else {
				TaskDoResult result = TASK_DO_ERROR;

				if (taskStatus == TASK_IDLE)
					task_create(taskID, taskDo);

				result = task_process(taskID);
				switch (result) {
				case TASK_DO_ERROR:
					break;
				case TASK_DO_FINISH:
					break;
				case TASK_DO_OVER:
					break;
				case TASK_DO_NEED_DATA:
					break;
				default:
					break;
				}
			}
			reg_set_task_enable(1);
		} else {
		}
	}

	return 0;
}
