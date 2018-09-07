#include "log.h"

static LogModule module = ~0x0;
static LogLevel  level  = INFO;

void log_printf(LogModule m, LogLevel l, const char* fmt, ...)
{
}

void log_set_module(LogModule m)
{
}

void log_set_level(LogLevel  l)
{
}
