#ifndef __DECODER_H__
#define __DECODER_H__

#include "format.h"
#include "register.h"
#include "buf_handle.h"

typedef enum {
	DECODEC_FINISH    = 0,
	DECODEC_NEED_DATA = 1,
	DECODEC_OVER      = 2
} DecoderStatus;

typedef struct {
} DecoderContext;

typedef struct {
	const char      *name;
	const char      *long_name;
	CodecID         codec_id;
	DecoderContext* (*init)  (BufHandle *in,  BufHandle *out, DecoderReadCtrl *rCtrl, DecoderWriteCtrl *wCtrl);
	void            (*free)  (DecoderContext *avctx);
	DecoderStatus   (*decode)(DecoderContext *avctx);
} DecoderClass;

#endif
