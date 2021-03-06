#include "os_printf.h"
#include "log_printf.h"

static LogModule module = 0x0;
static LogLevel  level  = 0x0;

void log_printf(LogModule m, LogLevel l, const char* fmt, ...)
{
	os_va_list args;
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
		break;
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

	os_printf("[%s][%s]: ",  mstring, lstring);
	os_va_start(args, fmt);
	os_vprintf(fmt, args);
	os_va_end(args);

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
