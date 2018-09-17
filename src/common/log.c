#include "log.h"
#include "os_define.h"

static LogModule module = 0x0;
static LogLevel  level  = 0x0;

void log_printf(LogModule m, LogLevel l, const char* fmt, ...)
{
	uart_va_list args;
	const char *mstring = NULL;
	const char *lstring = NULL;

	switch (l & level) {
	case LEVEL_INFO:
		lstring = "info";
		break;
	case LEVEL_WARN:
		lstring = "warn";
		break;
	case LEVEL_ERRO:
		lstring = "erro";
		break;
	case LEVEL_DEBU:
		lstring = "debu";
		break;
	default:
		return;
	}

	switch (m & module) {
	case COMMON_MODULE:
		mstring = "comn mdl";
	case OPUS_DEC_MODULE:
		mstring = "opus dec";
		break;
	case OPUS_ENC_MODULE:
		mstring = "opus enc";
		break;
	case VORBIS_DEC_MODULE:
		mstring = "vorb dec";
		break;
	default:
		return;
	}

	uart_printf("[%s][%s]: ",  mstring, lstring);
	uart_va_start(args, fmt);
	uart_vprintf(fmt, args);
	uart_va_end(args);

	return;
}

void log_set_module(LogModule m)
{
	module = m;
}

void log_set_level(LogLevel l)
{
	level = l;
}
