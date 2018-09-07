#ifndef __LOG_H__
#define __LOG_H__

typedef enum {
	INFO    = (1 << 0),
	WARNING = (1 << 1),
	ERROR   = (1 << 2),
	DEBUG   = (1 << 3)
} LogLevel;

typedef enum {
	COMMON  = (1 << 0),
	DECODER = (1 << 1),
	ENCODER = (1 << 2),
	DECRYPT = (1 << 3),
	ENCRYPT = (1 << 4),
	FILTER  = (1 << 5)
} LogModule;

void log_printf(LogModule m, LogLevel l, const char* fmt, ...);
void log_set_module(LogModule m);
void log_set_level (LogLevel  l);

#endif
