#include <stdarg.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "PrintLog.h"
#include <string>

#define BUFFER_MAX_LEN  8192

#define RESET           0
#define BRIGHT          1
#define DIM             2
#define UNDERLINE       4
#define BLINK           5
#define REVERSE         7
#define HIDDEN          8

#define BLACK           0
#define RED             1
#define GREEN           2
#define YELLOW          3
#define BLUE            4
#define MAGENTA         5
#define CYAN            6
#define WHITE           7

static const int s_logColor[] = {0, MAGENTA, RED, YELLOW, GREEN, CYAN, WHITE};
static const char* s_logTag[] = {"\0", "fatal", "error", "warn ", "info ", "trace", "debug"};
static LogLevel s_level = logLevelInfo;

int logFilter(int level, const char* module, const char* file, const char* func, int line,
              const char* version, const char* fmt, ...)
{
    static char buffer[BUFFER_MAX_LEN] = {0};

    if (level > s_level)
    {
        return 0;
    }

    struct timeval tv;
    struct tm t;
    gettimeofday(&tv, nullptr);
    localtime_r(&tv.tv_sec, &t);

    int bufferLen = 0;
    std::string filename(file);
    size_t pos = filename.rfind(DIR_SEPARATOR);
    if (pos != filename.npos)
    {
        filename = filename.substr(pos + 1);
    }
    snprintf(buffer, BUFFER_MAX_LEN-1, "\x1b[%d;%dm%02d:%02d:%02d.%06ld|[%s-%s] %s %s:%s():%d, ",
             1, 30+s_logColor[level], t.tm_hour, t.tm_min, t.tm_sec, tv.tv_usec,
             module, version, s_logTag[level], filename.c_str(), func, line);
    bufferLen += strlen(buffer + bufferLen);
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer+bufferLen, sizeof (buffer)-bufferLen, fmt, args);
    va_end(args);
    bufferLen += strlen(buffer + bufferLen);
    buffer[bufferLen] = '\x1b';
    buffer[bufferLen+1] = '[';
    buffer[bufferLen+2] = '0';
    buffer[bufferLen+3] = 'm';
    buffer[bufferLen+4] = '\0';
    printf("%s", buffer);

    return 0;
}

void setPrintLevel(LogLevel level)
{
    s_level = level;
}
