#include <stdio.h>
#include <time.h>
#include <string.h>
#include "log.h"

static int level = LOG_DEBUG;
static const int LOG_BUFFER_SIZE = 2048;
static const char* logLevels[] = {
    "EMERGE", "ALERT", "CRITICAL!", "ERROR", "WARN", "NOTICE", "INFO", "DEBUG"
};

void setLogLevel(int logLevel){
    level = logLevel;
}

void log(int logLevel, const char* fileName, int line, const char* format, ...){
    if(logLevel > level){
        return;
    }

    time_t tmp = time(NULL);
    struct tm* curTime = localtime(&tmp);
    if(! curTime){
        return;
    }

    char buff[LOG_BUFFER_SIZE];
    memset(buff, '\0', LOG_BUFFER_SIZE);
    strftime(buff, LOG_BUFFER_SIZE - 1, "%Y-%m-%d %H:%M:%S", curTime);
    printf("%s ", buff);
    printf("%s:%04d ", fileName, line);
    printf("[%s] ", logLevels[logLevel - LOG_EMERG]);
    
    va_list argList;
    va_start(argList, format);
    memset(buff, '\0', LOG_BUFFER_SIZE);
    vsnprintf(buff, LOG_BUFFER_SIZE - 1, format, argList);
    printf("%s\n", buff);
    fflush( stdout );
    va_end(argList);
}
