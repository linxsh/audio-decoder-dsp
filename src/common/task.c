#include "os_define.h"
#include "format.h"
#include "buf_handle.h"
#include "register.h"
#include "task.h"
#include "log.h"
#include "list.h"
#include "decoder.h"

static TaskContext sTaskContext[] = {
	{0, TASK_IDLE, TASK_WORK_IDLE, NULL, NULL},
	{1, TASK_IDLE, TASK_WORK_IDLE, NULL, NULL},
	{2, TASK_IDLE, TASK_WORK_IDLE, NULL, NULL},
	{3, TASK_IDLE, TASK_WORK_IDLE, NULL, NULL}
};

int task_create(int taskID, TaskWorkType type, TaskWorkParam *param)
{
	if (TASK_WORK_DECODER == type) {
		DecoderClass   *cls = find_register_decoder(param->codec_id);
		DecoderContext *ctx = NULL;

		if (NULL == cls) {
			log_printf(COMMON_MODULE, LEVEL_ERRO, "(%d) decoder find fail\n", param->codec_id);
			return TASK_FAILURE;
		}

		ctx = cls->init ? cls->init(param->in, param->out, param->rCtrl, param->wCtrl) : NULL;
		if (NULL == ctx) {
			log_printf(COMMON_MODULE, LEVEL_ERRO, "(%s) decoder init fail\n", cls->name);
			return TASK_FAILURE;
		}

		sTaskContext[taskID].priv_class   = cls;
		sTaskContext[taskID].priv_context = ctx;
	} else {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskType(%d) not exist\n", type);
		return TASK_FAILURE;
	}

	sTaskContext[taskID].work_type   = type;
	sTaskContext[taskID].task_status = TASK_WORK;

	return TASK_SUCCESS;
}

void task_destory(int taskID)
{
	TaskWorkType   type = sTaskContext[taskID].work_type;
	DecoderClass   *cls = sTaskContext[taskID].priv_class;
	DecoderContext *ctx = sTaskContext[taskID].priv_context;

	if (TASK_WORK_IDLE == type) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskID(%d) have destory\n", taskID);
		return;
	}

	if ((NULL == cls) || (NULL == ctx)) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "cls(%s) have destory\n", cls->name);
		return;
	}

	if (cls->free)
		cls->free(ctx);

	sTaskContext[taskID].work_type    = TASK_WORK_IDLE;
	sTaskContext[taskID].task_status  = TASK_IDLE;
	sTaskContext[taskID].priv_class   = NULL;
	sTaskContext[taskID].priv_context = NULL;

	return;
}

TaskWorkStatus task_process(int taskID)
{
	return TASK_WORK_FINISH;
}

TaskStatus task_status(int taskID)
{
	int taskNum = sizeof(sTaskContext) / sizeof(TaskContext);

	if ((taskID < 0) || (taskID >= taskNum)) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskId(%d) not exist\n", taskID);
		return TASK_NOT_EXIST;
	}

	return sTaskContext[taskID].task_status;
}
