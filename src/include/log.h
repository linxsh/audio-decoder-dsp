#ifndef __LOG_H__
#define __LOG_H__

typedef enum {
	LEVEL_INFO = (1 << 0),
	LEVEL_WARN = (1 << 1),
	LEVEL_ERRO = (1 << 2),
	LEVEL_DEBU = (1 << 3)
} LogLevel;

typedef enum {
	COMMON_MODULE     = (1 << 0),
	OPUS_DEC_MODULE   = (1 << 1),
	OPUS_ENC_MODULE   = (1 << 2),
	VORBIS_DEC_MODULE = (1 << 3),
	VORBIS_ENC_MODULE = (1 << 4)
} LogModule;

void log_printf(LogModule m, LogLevel l, const char* fmt, ...);
void log_set_module(LogModule m);
void log_set_level (LogLevel  l);

#endif
