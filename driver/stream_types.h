#ifndef __STREAM_TYPES_H__
#define __STREAM_TYPES_H__

typedef enum {
	CODEC_MPEG12A = 0,
	CODEC_AVSA,
	CODEC_AC3,
	CODEC_EAC3,
	CODEC_RA_AAC,
	CODEC_RA_RA8LBR,
	CODEC_DRA,
	CODEC_MPEG4_AAC,
	CODEC_MPEG4_AAC_LATM,
	CODEC_WMA,
	CODEC_VORBIS,
	CODEC_FLAC,
	CODEC_PCM,
	CODEC_DTS,
	CODEC_OPUS,
} StreamCodec;

typedef enum {
	TASK_DEOCDE = 0,
	TASK_ENCODE,
	TASK_RESAMPLE
} StreamTask;

typedef struct {
	unsigned int size;
	unsigned int full_gate;
	unsigned int empty_gate;
} StreamBuffer;

typedef struct {
	unsigned int endian;
	unsigned int bits;
	unsigned int sample_rate;
	unsigned int channels;
} StreamPCM;

typedef struct {
	StreamCodec i_codec;
	StreamPCM   o_pcm;
} StreamDecode;

typedef struct {
	StreamPCM   i_pcm;
	StreamCodec o_codec;
} StreamEncode;

typedef struct {
	StreamPCM i_pcm;
	StreamPCM o_pcm;
} StreamResample;

typedef struct {
	StreamTask   task;
	StreamBuffer i_buf;
	StreamBuffer o_buf;
	union {
		StreamDecode   decode;
		StreamEncode   encode;
		StreamResample resample;
	};
} StreamConfig;

extern int  stream_init  (void);
extern void stream_uninit(void);
extern int  stream_isr   (void);
extern int  stream_open  (unsigned int id);
extern void stream_close (unsigned int id);
extern int  stream_config(unsigned int id, StreamConfig *config);

#endif
