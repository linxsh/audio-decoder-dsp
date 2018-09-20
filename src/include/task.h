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
	TASK_WORK_ERROR     = -1,
	TASK_WORK_FINISH    = 0,
	TASK_WORK_NEED_DATA = 1,
	TASK_WORK_OVER      = 2,
} TaskWorkStatus;

typedef enum {
	TASK_WORK_IDLE    = 0,
	TASK_WORK_DECODER = 1,
	TASK_WORK_ENCODER = 2,
	TASK_WORK_DECRYPT = 3,
	TASK_WORK_ENCRYPT = 4,
	TASK_WORK_FILTER  = 5
} TaskWorkType;

typedef struct {
	CodecID          codec_id;
	BufHandle        *in;
	BufHandle        *out;
	DecoderReadCtrl  *rCtrl;
	DecoderWriteCtrl *wCtrl;
} TaskWorkParam;

typedef struct {
	int          task_id;
	TaskStatus   task_status;
	TaskWorkType work_type;
	void         *priv_class;
	void         *priv_context;
} TaskContext;

extern int            task_create (int taskID, TaskWorkType type, TaskWorkParam *param);
extern void           task_destory(int taskID);
extern TaskWorkStatus task_process(int taskID);
extern TaskStatus     task_status (int taskID);
#endif
