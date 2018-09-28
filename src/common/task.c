#include "os_define.h"
#include "os_buffer.h"
#include "format.h"
#include "register.h"
#include "task.h"
#include "log.h"
#include "list.h"
#include "decoder.h"

static TaskContext sTaskContext[] = {
	{0, TASK_IDLE, TASK_WORK_NONE, NULL, NULL, NULL},
	{1, TASK_IDLE, TASK_WORK_NONE, NULL, NULL, NULL},
	{2, TASK_IDLE, TASK_WORK_NONE, NULL, NULL, NULL},
	{3, TASK_IDLE, TASK_WORK_NONE, NULL, NULL, NULL}
};

int task_create(int taskID, TaskWorkType type, TaskWorkParam *param)
{
	if (TASK_IDLE != sTaskContext[taskID].task_status) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskID(%d) have create\n", taskID);
		return TASK_FAILURE;
	}

	if (TASK_WORK_DECODER == type) {
		DecoderClass   *cls = NULL;
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

		sTaskContext[taskID].priv_name    = cls->name;
		sTaskContext[taskID].priv_class   = cls;
		sTaskContext[taskID].priv_context = ctx;
	} else {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskType(%d) error\n", type);
		return TASK_FAILURE;
	}

	sTaskContext[taskID].work_type    = type;
	sTaskContext[taskID].task_status  = TASK_WORK;

	return TASK_SUCCESS;
}

void task_destory(int taskID)
{
	TaskWorkType   type = sTaskContext[taskID].work_type;
	DecoderClass   *cls = sTaskContext[taskID].priv_class;
	DecoderContext *ctx = sTaskContext[taskID].priv_context;

	if (TASK_IDLE == sTaskContext[taskID].task_status) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskID(%d) have destory\n", taskID);
		return;
	}

	if (TASK_WORK_DECODER == type) {
		if (cls->free)
			cls->free(ctx);

		sTaskContext[taskID].priv_name    = NULL;
		sTaskContext[taskID].priv_class   = NULL;
		sTaskContext[taskID].priv_context = NULL;
	} else {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskType(%d) error\n", type);
		return;
	}

	sTaskContext[taskID].work_type    = TASK_WORK_NONE;
	sTaskContext[taskID].task_status  = TASK_IDLE;

	return;
}

TaskWorkStatus task_process(int taskID)
{
	TaskWorkStatus status = TASK_WORK_ERROR;
	TaskWorkType   type = sTaskContext[taskID].work_type;

	if (TASK_IDLE == sTaskContext[taskID].task_status) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskID(%d) have destory\n", taskID);
		return TASK_WORK_ERROR;
	}

	if (TASK_WORK_DECODER == type) {
		DecoderClass   *cls = sTaskContext[taskID].priv_class;
		DecoderContext *ctx = sTaskContext[taskID].priv_context;
		DecoderStatus  decStatus = DECODEC_ERROR;

		if (cls->decode)
			decStatus = cls->decode(ctx);

		if (DECODEC_FINISH == decStatus)
			status = TASK_WORK_FINISH;
		else if (DECODEC_NEED_DATA == decStatus)
			status = TASK_WORK_NEED_DATA;
		else if (DECODEC_OVER == decStatus)
			status = TASK_WORK_OVER;
		else
			status = TASK_WORK_ERROR;
	}

	return status;
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

void task_printf(void)
{
	int taskNum = sizeof(sTaskContext) / sizeof(TaskContext);
	int i = 0;
	const char *taskStatString[] = {"idle", "work"};
	const char *workTypeString[] = {"none", "decoder", "encoder", "decrypt", "encrypt", "filter"};

	log_printf(COMMON_MODULE, LEVEL_INFO, "=============================\n");
	for (i = 0; i < taskNum; i++) {
		log_printf(COMMON_MODULE, LEVEL_INFO, "Task ID:     %d\n", sTaskContext[i].task_id);
		log_printf(COMMON_MODULE, LEVEL_INFO, "Task Status: %s\n", taskStatString[sTaskContext[i].task_status]);
		log_printf(COMMON_MODULE, LEVEL_INFO, "Work Type:   %s\n", workTypeString[sTaskContext[i].work_type]);
		log_printf(COMMON_MODULE, LEVEL_INFO, "Codec Name:  %s\n", sTaskContext[i].priv_name);
		log_printf(COMMON_MODULE, LEVEL_INFO, "------------------------\n");
	}
	log_printf(COMMON_MODULE, LEVEL_INFO, "=============================\n");
}
