#ifndef __DECODER_H__
#define __DECODER_H__

#include "reg.h"

typedef enum {
} DecoderStatus;

typedef struct {
} DecoderContext;

typedef struct {
	const char      *name;
	const char      *long_name;
	CodecID         codec_id;
	DecoderContext* (*init)  (FILE *in, FILE *out, DecoderReadCtrl *rCtrl, DecoderWriteCtrl *wCtrl);
	void            (*free)  (DecoderContext *avctx);
	DecoderStatus   (*decode)(DecoderContext *avctx);
} DecoderClass;

#endif
