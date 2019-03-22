#ifndef LOG_H
#define LOG_H

#include <syslog.h>
#include <cstdarg>

void setLogLevel(int logLevel = LOG_DEBUG); //设置日至等级
void log(int logLevel, const char* fileName, int line, const char* format, ...); //打印日志，将日志输出在标准输出

#endif
