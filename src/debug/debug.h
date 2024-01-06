#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

void debug_init(const char *logFile);
void debug_log(const char *format, ...);
void debug_close(void);

#endif // DEBUG_H