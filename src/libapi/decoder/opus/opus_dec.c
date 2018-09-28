#include "decoder.h"
#include "os_define.h"
#include "os_malloc.h"
#include "log.h"
#include "../../libsrc/opus/include/opus_types.h"
#include "../../libsrc/opus/include/opus_multistream.h"

static DecoderContext* opus_dec_init(OsBufferHandle *in,
		OsBufferHandle   *out,
		DecoderReadCtrl  *rCtrl,
		DecoderWriteCtrl *wCtrl)
{
	DecoderContext *opusDec = os_malloc(sizeof(DecoderContext));

	if (opusDec == NULL) {
		log_printf(OPUS_DEC_MODULE, LEVEL_ERRO, "malloc error\n");
		return NULL;
	}

	os_memset(opusDec, 0, sizeof(DecoderContext));

	return opusDec;
}

static void opus_dec_free(DecoderContext *opusDec)
{
	os_free(opusDec);
	return;
}

static DecoderStatus opus_dec_decode(DecoderContext *opusDec)
{
	return DECODEC_FINISH;
}

DecoderClass opusDecClass = {
	.name      = "opus",
	.long_name = "opus decoder",
	.codec_id  = CODEC_ID_OPUS,
	.init      = opus_dec_init,
	.free      = opus_dec_free,
	.decode    = opus_dec_decode,
};
