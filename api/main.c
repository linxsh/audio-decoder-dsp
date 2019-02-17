#include "os_printf.h"
#include "os_timer.h"
#include "register.h"
#include "os_buffer.h"
#include "os_malloc.h"
#include "log_printf.h"
#include "task.h"

int start_up(void)
{
	unsigned int runFreq = 0;
	void *mallocAddress  = NULL;

#ifdef CONFIG_DSP32
	register_init(CONFIG_REG_BASE_ADDR, NULL);
#endif

	runFreq = reg_get_run_frequency();
	os_printf_init(runFreq);
	os_timer_init (runFreq);

	log_printf(COMMON_MODULE, LEVEL_INFO, "version: %s\n", COMPILE_VERS);
	log_printf(COMMON_MODULE, LEVEL_INFO, "date:    %s\n", COMPILE_DATE);
	log_printf(COMMON_MODULE, LEVEL_INFO, "run addr: 0x%p\n", reg_get_run_space_addr());
	log_printf(COMMON_MODULE, LEVEL_INFO, "run size: %d\n",   reg_get_run_space_size());
	log_printf(COMMON_MODULE, LEVEL_INFO, "run freq: %d(HZ)\n", runFreq);

#ifdef CONFIG_DSP32
	reg_enable_task ();
	reg_clear_task  (REG_TASK_QUEUE0|REG_TASK_QUEUE1);
#endif
	reg_set_task_isr(ISR_ENABLE_OK);

#ifdef CONFIG_DSP32
	mallocAddress = (void *)CONFIG_MALLOC_BASE_ADDR;
#else
	mallocAddress = (void *)reg_get_run_space_addr();
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
		TaskNo       taskNo = 0;
		TaskAction   taskAction = 0;
		unsigned int taskID = 0;
		TaskDo       taskDo = 0;
		TaskStatus   taskStatus = 0;

		if (reg_disable_get()) { //exit all task
			reg_set_task_isr(ISR_DISABLE_OK);
			break;
		}

		taskNo     = reg_get_task_no();
		taskAction = reg_get_task_action();
		if ((TASK_NO_0 != taskNo) && (TASK_NO_1 != taskNo))
			continue;
		if ((TASK_START != taskAction) && (TASK_STOP != taskAction))
			continue;

		taskID     = reg_get_task_id(taskNo);
		taskDo     = reg_get_task_do(taskNo);
		taskStatus = task_status(taskID);

		if (TASK_NOT_EXIST == taskStatus) {
			task_printf();
			continue;
		}

		if (TASK_STOP == taskAction) {
			task_destory(taskID);

			if (TASK_NO_0 == taskNo)
				reg_set_task_isr(ISR_TASK_NO_0_STOP);
			if (TASK_NO_1 == taskNo)
				reg_set_task_isr(ISR_TASK_NO_1_STOP);

			continue;
		} else {
			TaskDoResult result = TASK_DO_ERROR;

			if (taskStatus == TASK_IDLE)
				task_create(taskID, taskDo, taskNo);

			result = task_process(taskID);
			switch (result) {
			case TASK_DO_ERROR:
				break;
			case TASK_DO_FINISH:
				break;
			case TASK_DO_DATA_LESS:
				break;
			case TASK_DO_DATA_EOF:
				break;
			default:
				break;
			}
		}
	}

	return 0;
}
