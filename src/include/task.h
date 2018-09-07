#ifndef __TASK_H__
#define __TASK_H__

typedef enum {
	DECODER_TASK = 0,
	ENCODER_TASK = 1,
	DECRYPT_TASK = 2,
	ENCRYPT_TASK = 3,
	FILTER_TASK  = 4
} TaskID;

#endif
