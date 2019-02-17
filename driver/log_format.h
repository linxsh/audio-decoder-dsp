#ifndef __LOG_FORMAT_H__
#define __LOG_FORMAT_H__

typedef enum {
	INFO = (1 << 0),
	WARN = (1 << 1),
	ERRO = (1 << 2),
	DEBU = (1 << 3)
} LogLevel;

typedef enum {
	DRIVER = (1 << 0)
} LogModule;

void log_format    (LogModule m, LogLevel l, const char* fmt, ...);
void log_set_module(LogModule m);
void log_set_level (LogLevel  l);

#endif
