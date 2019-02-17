/*****************************************************************************
 * Author : linxsh
 * Type : Source
 * File Name : opus_dec.c
 * Describle : api for libopus encoder
 * Release History:
 * VERSION Date        AUTHOR    Description
 * 1.2-1   2018.09.24  linxsh    creation
 * ***************************************************************************/

#include "os_define.h"
#include "os_malloc.h"
#include "encoder.h"
#include "log_printf.h"
#include "opus_types.h"
#include "opus_multistream.h"
#include "../avutils/ogg.h"
#include "../avutils/opus_header.h"

#define OPUS_ENC_SAMPLE_RATE (48000)
#define IMIN(a,b) ((a) < (b) ? (a) : (b))   /**< Minimum int value.   */
#define IMAX(a,b) ((a) > (b) ? (a) : (b))   /**< Maximum int value.   */

#define OPUS_ENC_I_FRAME_BYTES  (8 * 1024)
#define OPUS_ENC_O_FRAME_BYTES (16 * 1024)

typedef struct {
	OpusMSEncoder    *st;
	OpusHeader        header;
	OsBufferHandle   *in;
	OsBufferHandle   *out;
	EncoderReadCtrl  *r_ctrl;
	EncoderWriteCtrl *w_ctrl;
	unsigned int      bits;
	unsigned int      i_frame_size;
	unsigned int      i_frame_bytes;
	unsigned int      o_frame_bytes;
	unsigned int      cbr;  //constant bitrate
	unsigned int      cvbr; //constrained variable bitrate
	unsigned int      bitRate;
	unsigned int      complexity;
	unsigned int      expect_loss;
	unsigned char     header_data[276];
	unsigned int      header_len;
} OpusEncContext;

static EncoderContext* opus_enc_init(OsBufferHandle *in,
		OsBufferHandle   *out,
		EncoderReadCtrl  *rCtrl,
		EncoderWriteCtrl *wCtrl)
{
	OpusEncContext *opusEnc    = os_malloc(sizeof(OpusEncContext));
	unsigned int   frameMS;
	unsigned int   application;
	int ret = 0;

	if (opusEnc == NULL) {
		log_printf(OPUS_ENC_MODULE, LEVEL_ERRO, "malloc error\n");
		return NULL;
	}
	os_memset(opusEnc, 0, sizeof(OpusEncContext));

	frameMS          = ext_reg_get(&(rCtrl->FRAME_MS));
	opusEnc->bits    = ext_reg_get(&(rCtrl->BITS));
	opusEnc->cbr     = ext_reg_get(&(rCtrl->CBR));
	opusEnc->cvbr    = ext_reg_get(&(rCtrl->CVBR));
	opusEnc->bitRate = ext_reg_get(&(rCtrl->BIT_RATE));
	opusEnc->complexity  = ext_reg_get(&(rCtrl->COMPLEXITY));
	opusEnc->expect_loss = ext_reg_get(&(rCtrl->EXPECT_LOSS));
	application = (frameMS < 10) ? OPUS_APPLICATION_RESTRICTED_LOWDELAY:OPUS_APPLICATION_AUDIO;
	opusEnc->header.input_sample_rate = ext_reg_get(&(rCtrl->SAMPLE_RATE));
	opusEnc->header.channels          = ext_reg_get(&(rCtrl->CHANNELS));
	opusEnc->header.channel_mapping   = (opusEnc->header.channels > 8) ? 255 : (opusEnc->header.channels > 2);

	opusEnc->st = opus_multistream_surround_encoder_create(opusEnc->header.input_sample_rate,
			opusEnc->header.channels,
			opusEnc->header.channel_mapping,
			&opusEnc->header.nb_streams,
			&opusEnc->header.nb_coupled,
			opusEnc->header.stream_map,
			application,
			&ret);
	if (ret != OPUS_OK) {
		log_printf(OPUS_ENC_MODULE, LEVEL_ERRO, "opus encoder create error\n");
		goto OPUS_OPEN_ERR;
	}

	opusEnc->i_frame_size  = frameMS * opusEnc->header.input_sample_rate / 1000;
	opusEnc->i_frame_bytes = (opusEnc->i_frame_size * opusEnc->bits / 8);
	opusEnc->o_frame_bytes = (1275 * 3 + 7) * opusEnc->header.nb_streams;

	if ((opusEnc->i_frame_bytes * opusEnc->header.channels) > OPUS_ENC_I_FRAME_BYTES) {
		log_printf(OPUS_ENC_MODULE, LEVEL_ERRO, "opus encoder opus o_frame_bytes (%d)\n", opusEnc->o_frame_bytes);
		goto OPUS_OPEN_ERR;
	}

	if (opusEnc->o_frame_bytes > OPUS_ENC_O_FRAME_BYTES) {
		log_printf(OPUS_ENC_MODULE, LEVEL_ERRO, "opus encoder opus o_frame_bytes (%d)\n", opusEnc->o_frame_bytes);
		goto OPUS_OPEN_ERR;
	}

	if (opusEnc->bitRate <= 0) {
		unsigned int sampleRate = opusEnc->header.input_sample_rate;
		opusEnc->bitRate = ((64000 * opusEnc->header.nb_streams + 32000 * opusEnc->header.nb_coupled) *
				(IMIN(48, IMAX(8, (( sampleRate < 44100 ? sampleRate : 48000) + 1000) / 1000)) + 16) + 32) >> 6;
	}

	if ((opusEnc->bitRate > (1024000 * opusEnc->header.channels)) || (opusEnc->bitRate < 500)) {
		log_printf(OPUS_ENC_MODULE, LEVEL_ERRO, "opus bit rate(%d) error\n", opusEnc->bitRate);
		goto OPUS_OPEN_ERR;
	}
	opusEnc->bitRate = IMIN(opusEnc->header.channels * 256000, opusEnc->bitRate);

	log_printf(OPUS_ENC_MODULE, LEVEL_INFO, "enc frame time: %d(ms)\n",     frameMS);
	log_printf(OPUS_ENC_MODULE, LEVEL_INFO, "sample rate:    %d(hz)\n",     opusEnc->header.input_sample_rate);
	log_printf(OPUS_ENC_MODULE, LEVEL_INFO, "channels:       %d\n",         opusEnc->header.channels);
	log_printf(OPUS_ENC_MODULE, LEVEL_INFO, "enc bit rate:   %d(byte/s)\n", opusEnc->bitRate);
	log_printf(OPUS_ENC_MODULE, LEVEL_INFO, "enc cbr:        %d\n",         opusEnc->cbr);
	log_printf(OPUS_ENC_MODULE, LEVEL_INFO, "enc cvbr:       %d\n",         opusEnc->cvbr);
	log_printf(OPUS_ENC_MODULE, LEVEL_INFO, "i frame bytes:  %d(byte)\n",   opusEnc->i_frame_size);
	log_printf(OPUS_ENC_MODULE, LEVEL_INFO, "o frame bytes:  %d(byte)\n",   opusEnc->o_frame_bytes);

	opus_multistream_encoder_ctl(opusEnc->st, OPUS_SET_BITRATE(opusEnc->bitRate));
	opus_multistream_encoder_ctl(opusEnc->st, OPUS_SET_VBR(!opusEnc->cbr));
	opus_multistream_encoder_ctl(opusEnc->st, OPUS_SET_VBR_CONSTRAINT(opusEnc->cvbr));
	opus_multistream_encoder_ctl(opusEnc->st, OPUS_SET_COMPLEXITY(opusEnc->complexity));
	opus_multistream_encoder_ctl(opusEnc->st, OPUS_SET_PACKET_LOSS_PERC(opusEnc->expect_loss));
	opus_multistream_encoder_ctl(opusEnc->st, OPUS_SET_LSB_DEPTH(IMAX(8, IMIN(24, opusEnc->bits))));

	opusEnc->header_len = opus_header_to_packet(&opusEnc->header, opusEnc->header_data, sizeof(opusEnc->header_data));
	opusEnc->in  = in;
	opusEnc->out = out;
	opusEnc->r_ctrl = rCtrl;
	opusEnc->w_ctrl = wCtrl;

	return opusEnc;

OPUS_OPEN_ERR:
	if (opusEnc && opusEnc->st)
		opus_multistream_encoder_destroy(opusEnc->st);

	if (opusEnc)
		os_free(opusEnc);

	return NULL;
}

static EncoderStatus opus_enc_encode(EncoderContext *enc)
{
	OpusEncContext *opusEnc = (OpusEncContext *)enc;
	OsBufferState bufferResult = OS_BUFFER_OK;
	unsigned int fillFrameBytes = 0;
	unsigned int needFrameBytes = 0;
	unsigned int encodecBytes = 0;
	unsigned char *i_buffer = NULL;
	unsigned char *o_buffer = NULL;
	int errorCode = 0;

	if (os_buffer_get_channel(opusEnc->in) == 1)
		needFrameBytes = opusEnc->i_frame_bytes * opusEnc->header.channels;
	else
		needFrameBytes = opusEnc->i_frame_bytes;

	bufferResult = os_buffer_check(opusEnc->in, needFrameBytes, NULL, &fillFrameBytes);
	if (OS_BUFFER_DATA_LESS == bufferResult) {
		ext_reg_set(&(opusEnc->w_ctrl->DATA_LESS_NUM), (needFrameBytes - fillFrameBytes));
		log_printf(OPUS_ENC_MODULE, LEVEL_DEBU,
				"%s %d: opus encoder data less (%d %d)\n",
				__FUNCTION__, __LINE__, needFrameBytes, fillFrameBytes);
		return ENCODEC_DATA_LESS;
	} else if (OS_BUFFER_DATA_EOF == bufferResult) {
		log_printf(OPUS_ENC_MODULE, LEVEL_DEBU,
				"%s %d: opus encoder data eof\n", __FUNCTION__, __LINE__);
		return ENCODEC_DATA_EOF;
	} else if (OS_BUFFER_DATA_FULL == bufferResult) {
		log_printf(OPUS_ENC_MODULE, LEVEL_ERRO,
				"%s %d: opus encoder error\n", __FUNCTION__, __LINE__);
		errorCode = -1;
		goto OPUS_ENC_EXIT;
	}

#ifdef CONFIG_DSP32
	i_buffer = (unsigned char *)os_malloc(OPUS_ENC_I_FRAME_BYTES);
	o_buffer = (unsigned char *)OUTBUF_TEMP_ADDR;
#else
	o_buffer = (unsigned char *)os_malloc(OPUS_ENC_O_FRAME_BYTES);
	i_buffer = (unsigned char *)os_malloc(OPUS_ENC_I_FRAME_BYTES);
#endif
	if ((o_buffer == NULL) || (i_buffer == NULL)) {
		log_printf(OPUS_ENC_MODULE, LEVEL_ERRO,
				"%s %d: opus encoder error\n", __FUNCTION__, __LINE__);
		errorCode = -2;
		goto OPUS_ENC_EXIT;
	}

	if (os_buffer_get_channel(opusEnc->in) == 1)
		os_buffer_read(opusEnc->in, 0, 1, i_buffer, needFrameBytes);
	else {
		unsigned int i = 0;
		for (i = 0; i < opusEnc->header.channels; i++) {
			unsigned int update = (i == (opusEnc->header.channels -1)) ? 0 : 1;
			os_buffer_read(opusEnc->in, i, update, i_buffer, needFrameBytes);
		}
		// switch interlace
	}

	encodecBytes = opus_multistream_encode(opusEnc->st,
			(const opus_int16 *)i_buffer, opusEnc->i_frame_size, o_buffer, opusEnc->o_frame_bytes);
	if (encodecBytes < 0) {
		log_printf(OPUS_ENC_MODULE, LEVEL_ERRO,
				"%s %d: opus encoder error\n", __FUNCTION__, __LINE__);
		errorCode = -3;
		goto OPUS_ENC_EXIT;
	}

	if (os_buffer_write(opusEnc->out, 0, 1, o_buffer, encodecBytes) < 0) {
		log_printf(OPUS_ENC_MODULE, LEVEL_ERRO,
				"%s %d: opus encoder error\n", __FUNCTION__, __LINE__);
		errorCode = -4;
		goto OPUS_ENC_EXIT;
	}

OPUS_ENC_EXIT:
	ext_reg_set(&(opusEnc->w_ctrl->ERROR_CODE), (unsigned int)(errorCode));
#ifdef CONFIG_DSP32
	os_free(o_buffer);
#else
	os_free(i_buffer);
	os_free(o_buffer);
#endif

	return ((errorCode == 0) ? ENCODEC_FINISH : ENCODEC_ERROR);
}

static void opus_enc_free(EncoderContext *enc)
{
	OpusEncContext *opusEnc = (OpusEncContext *)enc;

	if (opusEnc && opusEnc->st)
		opus_multistream_encoder_destroy(opusEnc->st);

	if (opusEnc)
		os_free(opusEnc);

	return;
}

EncoderClass opusEncClass = {
	.name      = "opus",
	.long_name = "opus encoder",
	.codec_id  = CODEC_ID_OPUS,
	.init      = opus_enc_init,
	.free      = opus_enc_free,
	.encode    = opus_enc_encode,
};
