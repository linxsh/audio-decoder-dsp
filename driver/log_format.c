#include "kernelcall.h"
#include "log_format.h"
#include <stdarg.h>

static LogModule module = DRIVER;
static LogLevel  level  = 0x0;

void log_format(LogModule m, LogLevel l, const char* fmt, ...)
{
	va_list args;
	const char *mString = NULL;
	const char *lString = NULL;

	switch (m & module) {
	case DRIVER:
		mString = "driver";
		break;
	default:
		return;
	}

	switch (l & level) {
	case INFO:
		lString = "info";
		break;
	case WARN:
		lString = "warn";
		break;
	case ERRO:
		lString = "erro";
		break;
	case DEBU:
		lString = "debu";
		break;
	default:
		return;
	}

	gx_printf("[%s]-[%s]: ", mString, lString);
	va_start(args, fmt);
	gx_vprintf(fmt, args);
	va_end(args);

	return;
}

void log_set_module(LogModule m)
{
	module = m;
}

void log_set_level(LogLevel  l)
{
	level = l;
}
