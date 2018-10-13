#include "os_define.h"
#include "register.h"
#include "os_buffer.h"
#include "format.h"
#include "task.h"
#include "log_printf.h"
#include "list.h"
#include "decoder.h"

static TaskContext sTaskContext[] = {
	{0, TASK_IDLE, TASK_NONE, {}, NULL, NULL, NULL},
	{1, TASK_IDLE, TASK_NONE, {}, NULL, NULL, NULL},
	{2, TASK_IDLE, TASK_NONE, {}, NULL, NULL, NULL},
	{3, TASK_IDLE, TASK_NONE, {}, NULL, NULL, NULL}
};

int task_create(int taskID, TaskDo taskDo, TaskNo taskNo)
{
	ExtReadReg   *rCtrl = reg_get_ext_read_reg  (taskNo);
	ExtWriteReg  *wCtrl = reg_get_ext_write_reg (taskNo);
	ExtDebugReg  *dCtrl = reg_get_ext_debug_reg (taskNo);
	ExtBufferReg *bCtrl = reg_get_ext_buffer_reg(taskNo);

	OsBufferHandle *inBuf  = os_buffer_open(BUFFER_I, bCtrl);
	OsBufferHandle *outBuf = os_buffer_open(BUFFER_O, bCtrl);

	if (TASK_IDLE != sTaskContext[taskID].task_status) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskID(%d) have create\n", taskID);
		return TASK_FAILURE;
	}

	if (TASK_DECODER == taskDo) {
		DecoderClass   *cls = NULL;
		DecoderContext *ctx = NULL;
		CodecID codecID = ext_reg_get(&(rCtrl->DECODER.CODEC_ID));

		if (NULL == (cls = find_register_decoder(codecID))) {
			log_printf(COMMON_MODULE, LEVEL_ERRO, "(%d) decoder find fail\n", codecID);
			return TASK_FAILURE;
		}

		ctx = cls->init ? cls->init(inBuf, outBuf, &rCtrl->DECODER, &wCtrl->DECODER) : NULL;
		if (NULL == ctx) {
			log_printf(COMMON_MODULE, LEVEL_ERRO, "(%s) decoder init fail\n", cls->name);
			return TASK_FAILURE;
		}

		sTaskContext[taskID].priv_name    = cls->name;
		sTaskContext[taskID].priv_class   = cls;
		sTaskContext[taskID].priv_context = ctx;
	} else {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskDo(%d) error\n", taskDo);
		return TASK_FAILURE;
	}

	sTaskContext[taskID].task_do_param.r_ctrl   = rCtrl;
	sTaskContext[taskID].task_do_param.w_ctrl   = wCtrl;
	sTaskContext[taskID].task_do_param.d_ctrl   = dCtrl;
	sTaskContext[taskID].task_do_param.b_ctrl   = bCtrl;
	sTaskContext[taskID].task_do_param.in_buf   = inBuf;
	sTaskContext[taskID].task_do_param.out_buf  = outBuf;
	sTaskContext[taskID].task_do     = taskDo;
	sTaskContext[taskID].task_status = TASK_WORK;

	return TASK_SUCCESS;
}

void task_destory(int taskID)
{
	TaskDo         type = sTaskContext[taskID].task_do;
	DecoderClass   *cls = sTaskContext[taskID].priv_class;
	DecoderContext *ctx = sTaskContext[taskID].priv_context;

	if (TASK_IDLE == sTaskContext[taskID].task_status) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskID(%d) have destory\n", taskID);
		return;
	}

	if (TASK_DECODER == type) {
		if (cls->free)
			cls->free(ctx);
		sTaskContext[taskID].priv_name    = NULL;
		sTaskContext[taskID].priv_class   = NULL;
		sTaskContext[taskID].priv_context = NULL;
	} else {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskType(%d) error\n", type);
		return;
	}

	if (sTaskContext[taskID].task_do_param.in_buf) {
		os_buffer_close(sTaskContext[taskID].task_do_param.in_buf);
		sTaskContext[taskID].task_do_param.in_buf = NULL;
	}
	if (sTaskContext[taskID].task_do_param.out_buf) {
		os_buffer_close(sTaskContext[taskID].task_do_param.out_buf);
		sTaskContext[taskID].task_do_param.out_buf = NULL;
	}
	sTaskContext[taskID].task_do_param.r_ctrl  = NULL;
	sTaskContext[taskID].task_do_param.w_ctrl  = NULL;
	sTaskContext[taskID].task_do_param.d_ctrl  = NULL;
	sTaskContext[taskID].task_do_param.b_ctrl  = NULL;
	sTaskContext[taskID].task_do      = TASK_NONE;
	sTaskContext[taskID].task_status  = TASK_IDLE;

	return;
}

TaskDoResult task_process(int taskID)
{
	TaskDoResult taskDoResult = TASK_DO_ERROR;

	if (TASK_IDLE == sTaskContext[taskID].task_status) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "taskID(%d) have destory\n", taskID);
		return TASK_DO_ERROR;
	}

	os_buffer_update_from(sTaskContext[taskID].task_do_param.in_buf,  OS_BUFFER_W_ATTR);
	os_buffer_update_from(sTaskContext[taskID].task_do_param.out_buf, OS_BUFFER_R_ATTR);
	if (TASK_DECODER == sTaskContext[taskID].task_do) {
		DecoderClass   *cls = sTaskContext[taskID].priv_class;
		DecoderContext *ctx = sTaskContext[taskID].priv_context;
		DecoderStatus  decStatus = DECODEC_ERROR;

		if (cls->decode)
			decStatus = cls->decode(ctx);

		if (DECODEC_FINISH == decStatus)
			taskDoResult = TASK_DO_FINISH;
		else if (DECODEC_NEED_DATA == decStatus)
			taskDoResult = TASK_DO_NEED_DATA;
		else if (DECODEC_OVER == decStatus)
			taskDoResult = TASK_DO_OVER;
		else
			taskDoResult = TASK_DO_ERROR;
	}
	os_buffer_update_to(sTaskContext[taskID].task_do_param.in_buf,  OS_BUFFER_R_ATTR);
	os_buffer_update_to(sTaskContext[taskID].task_do_param.out_buf, OS_BUFFER_W_ATTR);

	return taskDoResult;
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
	const char *taskStString[] = {"idle", "work"};
	const char *taskDoString[] = {"none", "decoder", "encoder", "decrypt", "encrypt", "filter"};

	log_printf(COMMON_MODULE, LEVEL_INFO, "=============================\n");
	for (i = 0; i < taskNum; i++) {
		log_printf(COMMON_MODULE, LEVEL_INFO, "Task ID:     %d\n", sTaskContext[i].task_id);
		log_printf(COMMON_MODULE, LEVEL_INFO, "Task Status: %s\n", taskStString[sTaskContext[i].task_status]);
		log_printf(COMMON_MODULE, LEVEL_INFO, "Work Type:   %s\n", taskDoString[sTaskContext[i].task_do]);
		log_printf(COMMON_MODULE, LEVEL_INFO, "Codec Name:  %s\n", sTaskContext[i].priv_name);
		log_printf(COMMON_MODULE, LEVEL_INFO, "------------------------\n");
	}
	log_printf(COMMON_MODULE, LEVEL_INFO, "=============================\n");
}
