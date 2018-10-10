/*****************************************************************************
 * Author : linxsh
 * Type : Source
 * File Name : opus_dec.c
 * Describle : api for libopus decoder
 * Release History:
 * VERSION Date        AUTHOR    Description
 * 1.2-1   2018.09.24  linxsh    creation
 * ***************************************************************************/

#include "decoder.h"
#include "os_define.h"
#include "os_malloc.h"
#include "os_buffer.h"
#include "log_printf.h"
#include "register.h"
#include "../../libsrc/ogg/include/ogg.h"
#include "../../libsrc/opus/include/opus_types.h"
#include "../../libsrc/opus/include/opus_multistream.h"
#include "opus_header.h"

#define OPUS_SAMPLE_RATE (48000)
#define OPUS_FRAME_SZIE  (0x600)
#define OPUS_INPUT_LEN   (32*1024)
#define OPUS_OUTPUT_LEN  (2*2*OPUS_FRAME_SZIE)

typedef enum {
	OPUS_READ_DATA = 0,
	OPUS_SYNC_PAGEOUT,
	OPUS_SYNC_PACKET
} OpusDecState;

typedef struct {
	OpusMSDecoder    *st;
	OpusDecState     state;
	OsBufferHandle   *in;
	OsBufferHandle   *out;
	DecoderReadCtrl  *r_ctrl;
	DecoderWriteCtrl *w_ctrl;

	ogg_sync_state   oy;
	ogg_page         og;
	ogg_packet       op;
	ogg_stream_state os;
	ogg_int64_t      page_granule;

	OpusHeader       header;
	int              stream_init;
	int              has_opus_stream;
	int              has_tags_packet;
	int              packet_count;
	unsigned char    out_save_buf[OPUS_OUTPUT_LEN];
	unsigned char    in_save_buf [OPUS_INPUT_LEN];
} OpusDecContext;

static unsigned char sSaveBuf[16*1024];

static DecoderContext* opus_dec_init(OsBufferHandle *in,
		OsBufferHandle   *out,
		DecoderReadCtrl  *rCtrl,
		DecoderWriteCtrl *wCtrl)
{
	OpusDecContext *opusDec = os_malloc(sizeof(OpusDecContext));

	if (opusDec == NULL) {
		log_printf(OPUS_DEC_MODULE, LEVEL_ERRO, "malloc error\n");
		return NULL;
	}

	os_memset(opusDec, 0, sizeof(OpusDecContext));
	opusDec->in    = in;
	opusDec->out   = out;
	opusDec->state = OPUS_READ_DATA;

	ogg_sync_init(&opusDec->oy);
#ifdef CONFIG_DSP32
	opusDec->oy.qmem_buf = (unsigned char*)OUTBUF_TEMP_ADDR;
	opusDec->oy.data = (unsigned char*)(((int)opusDec->in_save_buf)|AUDIO_MCU_NOCACHE_ADDR);
#else
	opusDec->oy.data = (unsigned char*)(opusDec->in_save_buf);
#endif

	return (DecoderContext *)opusDec;
}

static void opus_dec_free(DecoderContext *dec)
{
	OpusDecContext *opusDec = (OpusDecContext *)dec;

	if (opusDec->st) {
		opus_multistream_decoder_destroy(opusDec->st);
		opusDec->st = NULL;
	}

	if (opusDec->stream_init)
		ogg_stream_clear(&opusDec->os);

	ogg_sync_clear(&opusDec->oy);

	os_free(opusDec);

	return;
}

static DecoderStatus opus_dec_decode(DecoderContext *dec)
{
	OpusDecContext *opusDec = (OpusDecContext *)dec;

#ifdef CONFIG_DSP32
	int *tmp32Buffer = (int *)OUTBUF_TEMP_ADDR;
#else
	int *tmp32Buffer = (int *)sSaveBuf;
#endif

	if (opusDec->state == OPUS_SYNC_PACKET)
		goto sync_packet;

	while (1) {
		unsigned int bytes;

		bytes = ogg_sync_buffer(&opusDec->oy, 1024, opusDec->in);
		if (OS_BUFFER_NEED == bytes) {
			log_printf(OPUS_DEC_MODULE, LEVEL_DEBU, "%s %d: OPUS data less\n", __FUNCTION__, __LINE__);
			opusDec->state = OPUS_READ_DATA;
			return DECODEC_NEED_DATA;
		} else if (OS_BUFFER_EMPTY == bytes) {
			log_printf(OPUS_DEC_MODULE, LEVEL_DEBU, "%s %d: OPUS data empty\n", __FUNCTION__, __LINE__);
			opusDec->state = OPUS_READ_DATA;
			return DECODEC_OVER;
		}
		ogg_sync_wrote(&opusDec->oy, bytes);

		while (ogg_sync_pageout(&opusDec->oy, &opusDec->og) == 1) {
			if (opusDec->stream_init == 0) {
				ogg_stream_init(&opusDec->os, ogg_page_serialno(&opusDec->og));
				opusDec->stream_init = 1;
			}

			if (ogg_page_serialno(&opusDec->og) != opusDec->os.serialno) {
				ogg_stream_reset_serialno(&opusDec->os, ogg_page_serialno(&opusDec->og));
			}

			ogg_stream_pagein(&opusDec->os, &opusDec->og);
			opusDec->page_granule = ogg_page_granulepos(&opusDec->og);
			while (ogg_stream_packetout(&opusDec->os, &opusDec->op) == 1) {
				if (opusDec->op.b_o_s &&
						opusDec->op.bytes >= 8 &&
						!memcmp(opusDec->op.packet, "OpusHead", 8)) {
					if (opusDec->has_opus_stream && opusDec->has_tags_packet) {
						opusDec->has_opus_stream = 0;
						if (opusDec->st)
							opus_multistream_decoder_destroy(opusDec->st);
						opusDec->st = NULL;
					}
					if (!opusDec->has_opus_stream) {
						opusDec->has_opus_stream = 1;
						opusDec->has_tags_packet = 0;
						opusDec->packet_count = 0;
					}
				}

				if (!opusDec->has_opus_stream) {
					log_printf(OPUS_DEC_MODULE, LEVEL_ERRO, "%s %d: not stream\n", __FUNCTION__, __LINE__);
					break;
				}

				if (opusDec->packet_count == 0) {
					int err;
					OpusHeader *header = &opusDec->header;

					opus_header_parse(opusDec->op.packet, opusDec->op.bytes, header);
					opusDec->st = opus_multistream_decoder_create(OPUS_SAMPLE_RATE,
							header->channels, header->nb_streams, header->nb_coupled, header->stream_map, &err);
					if(err != OPUS_OK){
						log_printf(OPUS_DEC_MODULE, LEVEL_ERRO, "%s %d: create failed\n", __FUNCTION__, __LINE__);
						opusDec->state = OPUS_SYNC_PAGEOUT;
						return -1;
					}
					log_printf(OPUS_DEC_MODULE, LEVEL_INFO, "Opus Head: (%d Hz) (%d ch)\n", OPUS_SAMPLE_RATE, header->channels);
				} else if (opusDec->packet_count == 1) {
					opusDec->has_tags_packet=1;
				} else {
					int frameSize = 0;
					frameSize = opus_multistream_decode(opusDec->st,
							(unsigned char*)opusDec->op.packet, opusDec->op.bytes,
							(opus_int16 *)opusDec->out_save_buf, OPUS_FRAME_SZIE, 0);
					if (frameSize > 0) {
					//trans out_save_buf to tmp32Buffer
						os_buffer_write(opusDec->out,  0, 1, (unsigned char*)tmp32Buffer, (frameSize<<2));

						extra_reg_set(&opusDec->w_ctrl->sample_rate, OPUS_SAMPLE_RATE);
						extra_reg_set(&opusDec->w_ctrl->channels,    0x2);
						reg_set_error_code(0);
					} else {
						reg_set_error_code((unsigned int)(-88));
					}

					opusDec->state = OPUS_SYNC_PACKET;
					return DECODEC_FINISH;
				}

sync_packet:
				opusDec->packet_count++;
			}
		}
	}

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
