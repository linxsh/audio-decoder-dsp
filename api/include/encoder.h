#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "register.h"
#include "os_buffer.h"

typedef enum {
	ENCODEC_ERROR     = -1,
	ENCODEC_FINISH    =  0,
	ENCODEC_DATA_LESS =  1,
	ENCODEC_DATA_EOF =  2
} EncoderStatus;

typedef void EncoderContext;

typedef struct {
	const char      *name;
	const char      *long_name;
	CodecID         codec_id;
	EncoderContext* (*init)  (OsBufferHandle *in, OsBufferHandle *out, EncoderReadCtrl *rCtrl, EncoderWriteCtrl *wCtrl);
	void            (*free)  (EncoderContext *avctx);
	EncoderStatus   (*encode)(EncoderContext *avctx);
} EncoderClass;

#endif
