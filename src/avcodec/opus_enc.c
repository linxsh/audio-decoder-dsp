/*****************************************************************************
 * Author : linxsh
 * Type : Source
 * File Name : opus_dec.c
 * Describle : api for libopus decoder
 * Release History:
 * VERSION Date        AUTHOR    Description
 * 1.2-1   2018.09.24  linxsh    creation
 * ***************************************************************************/
#include "os_define.h"
#include "os_malloc.h"
#include "encoder.h"
#include "log_printf.h"
#include "../libsrc/opus/include/opus_types.h"
#include "../libsrc/opus/include/opus_multistream.h"
#include "../avutils/ogg.h"
#include "../avutils/opus_header.h"

#define OPUS_ENC_SAMPLE_RATE (48000)

typedef struct {
	OpusMSEncoder    *st;
	OpusHeader        header;
	unsigned int      frame_size;
} OpusEncContext;

static EncoderContext* opus_enc_init(OsBufferHandle *in,
		OsBufferHandle   *out,
		EncoderReadCtrl  *rCtrl,
		EncoderWriteCtrl *wCtrl)
{
	OpusEncContext *opusEnc    = os_malloc(sizeof(OpusEncContext));
	OpusHeader     *opusHeader = NULL;
	unsigned int   frameMS;
	unsigned int   application;
	int ret = 0;

	if (opusEnc == NULL) {
		log_printf(OPUS_ENC_MODULE, LEVEL_ERRO, "malloc error\n");
		return NULL;
	}

	frameMS     = ext_reg_get(&(rCtrl->FRAME_MS));
	application = (frameMS < 10) ? OPUS_APPLICATION_RESTRICTED_LOWDELAY:OPUS_APPLICATION_AUDIO;
	opusHeader = &opusEnc->header;
	opusHeader->input_sample_rate = ext_reg_get(&(rCtrl->SAMPLE_RATE));
	opusHeader->channels          = ext_reg_get(&(rCtrl->CHANNELS));
	opusHeader->channel_mapping   = (opusHeader->channels > 8) ? 255 : (opusHeader->channels > 2);

	opusEnc->st = opus_multistream_surround_encoder_create(opusHeader->input_sample_rate,
			opusHeader->channels, opusHeader->channel_mapping,
			&opusHeader->nb_streams, &opusHeader->nb_coupled,
			opusHeader->stream_map, application, &ret);

	opusEnc->frame_size = frameMS * opusHeader->input_sample_rate / 1000;
	if (ret != OPUS_OK) {
		log_printf(OPUS_ENC_MODULE, LEVEL_ERRO, "opus encoder error\n");
		os_free(opusEnc);
		return NULL;
	}

	return opusEnc;
}

static EncoderStatus opus_enc_encode(EncoderContext *enc)
{
	return ENCODEC_FINISH;
}

static void opus_enc_free(EncoderContext *enc)
{
}

EncoderClass opusEncClass = {
	.name      = "opus",
	.long_name = "opus encoder",
	.codec_id  = CODEC_ID_OPUS,
	.init      = opus_enc_init,
	.free      = opus_enc_free,
	.encode    = opus_enc_encode,
};
