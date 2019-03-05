#ifndef LOG_H
#define LOG_H

#include <syslog.h>
#include <cstdarg>

void setLogLevel(int logLevel = LOG_DEBUG);
void log(int logLevel, const char* fileName, int line, const char* format, ...);

#endif
