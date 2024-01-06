#include "debug.h"
#include <stdarg.h>

static FILE *log_fp = NULL;

void debug_init(const char *logFile)
{
    log_fp = fopen(logFile, "w");
    if (log_fp == NULL)
    {
        fprintf(stderr, "Failed to open log file: %s\n", logFile);
    }
}

void debug_log(const char *format, ...)
{
    if (log_fp != NULL)
    {
        va_list args;
        va_start(args, format);
        vfprintf(log_fp, format, args);
        va_end(args);
        fprintf(log_fp, "\n");
        fflush(log_fp);
    }
    printf("%s\n", format);
}

void debug_close(void)
{
    if (log_fp != NULL)
    {
        fclose(log_fp);
    }
}
