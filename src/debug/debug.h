#ifndef DEBUG_H
#define DEBUG_H

void debug_init(const char *logFile);
void debug_log(const char *file, int line, const char *format, ...);
void debug_close(void);

// Macro to automatically fill in file and line number
#define DEBUG_LOG(format, ...) debug_log(__FILE__, __LINE__, format, ##__VA_ARGS__)

#endif // DEBUG_H