#ifndef PRINTLOG_H
#define PRINTLOG_H

#include <stdio.h>

#ifndef LOG_LIB_NAME
#define LOG_LIB_NAME    ""
#endif

#ifndef LOG_LIB_VERSION
#define LOG_LIB_VERSION ""
#endif

enum LogLevel
{
    logLevelUnknown = 0,
    logLevelFatal,
    logLevelError,
    logLevelWarn,
    logLevelInfo,
    logLevelTrace,
    logLevelDebug,
};

#if defined _WIN32  || defined _WIN64
#define DIR_SEPARATOR "\\"
#else
#define DIR_SEPARATOR "/"
#endif

void setPrintLevel(LogLevel level);

/*inline*/ int logFilter(int level, const char* module, const char* file, const char* func, int line,
              const char* version, const char* fmt, ...) __attribute__((format(printf, 7, 8)));

#define fatalf(format, ...) \
        logFilter(logLevelFatal, LOG_LIB_NAME, __FILE__, __FUNCTION__, __LINE__, LOG_LIB_VERSION, format, ##__VA_ARGS__)

#define errorf(format, ...) \
        logFilter(logLevelError, LOG_LIB_NAME, __FILE__, __FUNCTION__, __LINE__, LOG_LIB_VERSION, format, ##__VA_ARGS__)

#define warnf(format, ...) \
        logFilter(logLevelWarn, LOG_LIB_NAME, __FILE__, __FUNCTION__, __LINE__, LOG_LIB_VERSION, format, ##__VA_ARGS__)

#define infof(format, ...) \
        logFilter(logLevelInfo, LOG_LIB_NAME, __FILE__, __FUNCTION__, __LINE__, LOG_LIB_VERSION, format, ##__VA_ARGS__)

#define tracef(format, ...) \
        logFilter(logLevelTrace, LOG_LIB_NAME, __FILE__, __FUNCTION__, __LINE__, LOG_LIB_VERSION, format, ##__VA_ARGS__)

#define debugf(format, ...) \
        logFilter(logLevelDebug, LOG_LIB_NAME, __FILE__, __FUNCTION__, __LINE__, LOG_LIB_VERSION, format, ##__VA_ARGS__)


#endif // PRINTLOG_H
