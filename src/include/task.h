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
	TASK_DO_ERROR     = -1,
	TASK_DO_FINISH    = 0,
	TASK_DO_NEED_DATA = 1,
	TASK_DO_OVER      = 2,
} TaskDoResult;

typedef struct {
	OsBufferHandle *in_buf;
	OsBufferHandle *out_buf;
	ExtReadReg     *r_ctrl;
	ExtWriteReg    *w_ctrl;
	ExtDebugReg    *d_ctrl;
	ExtBufferReg   *b_ctrl;
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

extern int            task_create (int taskID, TaskDo taskDo, TaskNo taskNo);
extern void           task_destory(int taskID);
extern TaskDoResult   task_process(int taskID);
extern TaskStatus     task_status (int taskID);
extern void           task_printf (void);
#endif
