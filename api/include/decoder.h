#ifndef __DECODER_H__
#define __DECODER_H__

#include "register.h"
#include "os_buffer.h"

typedef enum {
	DECODEC_ERROR     = -1,
	DECODEC_FINISH    =  0,
	DECODEC_DATA_LESS =  1,
	DECODEC_DATA_EOF  =  2
} DecoderStatus;

typedef void DecoderContext;

typedef struct {
	const char      *name;
	const char      *long_name;
	CodecID         codec_id;
	DecoderContext* (*init)  (OsBufferHandle *in, OsBufferHandle *out, DecoderReadCtrl *rCtrl, DecoderWriteCtrl *wCtrl);
	void            (*free)  (DecoderContext *avctx);
	DecoderStatus   (*decode)(DecoderContext *avctx);
} DecoderClass;

#endif
