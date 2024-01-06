#include "debug.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static FILE *log_fp = NULL;

static void debug_handle_exit(void)
{
    debug_close();
}

static void debug_handle_signal(int sig)
{
    if (log_fp != NULL) {
        fprintf(log_fp, "Unexpected termination: signal %d\n", sig);
    }
    debug_close();
    exit(sig);
}

void debug_init(const char *logFile)
{
    log_fp = fopen(logFile, "w");
    if (log_fp == NULL)
    {
        fprintf(stderr, "Failed to open log file: %s\n", logFile);
    }
    else
    {
        atexit(debug_handle_exit);
        signal(SIGINT, debug_handle_signal);
        signal(SIGSEGV, debug_handle_signal);
        // Add additional signal handlers as needed
    }
}

void debug_log(const char *file, int line, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    // Print to console
    va_list args_copy;
    va_copy(args_copy, args);
    printf("LOG (%s:%d): ", file, line);
    vprintf(format, args_copy);
    printf("\n");
    va_end(args_copy);

    // Print to file
    if (log_fp != NULL)
    {
        fprintf(log_fp, "LOG (%s:%d): ", file, line);
        vfprintf(log_fp, format, args);
        fprintf(log_fp, "\n");
        fflush(log_fp);
    }

    va_end(args);
}

void debug_close(void)
{
    if (log_fp != NULL)
    {
        fclose(log_fp);
        log_fp = NULL;
    }
}