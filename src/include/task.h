#ifndef __TASK_H__
#define __TASK_H__

#define TASK_SUCCESS  (0)
#define TASK_FAILURE  (-1)

typedef enum {
	TASK_WORK      = 0,
	TASK_IDLE      = 1,
	TASK_NOT_EXIST = 2
} TaskStatus;

typedef enum {
	TASK_DO_STOP    = 0,
	TASK_DO_DECODER = 1,
	TASK_DO_ENCODER = 2,
	TASK_DO_DECRYPT = 3,
	TASK_DO_ENCRYPT = 4,
	TASK_DO_FILTER  = 5
} TaskDo;

typedef enum {
	TASK_DO_ERROR     = -1,
	TASK_DO_FINISH    = 0,
	TASK_DO_NEED_DATA = 1,
	TASK_DO_OVER      = 2,
} TaskDoResult;

typedef struct {
	CodecID          codec_id;
	OsBufferHandle   *in_buf;
	OsBufferHandle   *out_buf;
	ExtraReadReg     *r_ctrl;
	ExtraWriteReg    *w_ctrl;
	ExtraDebugReg    *d_ctrl;
} TaskDoParam;

typedef struct {
	int          task_id;
	TaskStatus   task_status;
	TaskDo       task_do;
	TaskDoParam  task_do_param;
	const char   *priv_name;
	void         *priv_class;
	void         *priv_context;
} TaskContext;

extern int            task_create (int taskID, TaskDo taskDo);
extern void           task_destory(int taskID);
extern TaskDoResult   task_process(int taskID);
extern TaskStatus     task_status (int taskID);
extern void           task_printf (void);
#endif
