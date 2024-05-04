#ifndef nibble_os_h
#define nibble_os_h

#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#ifndef DOS
#include <libgen.h>
#endif
#include <unistd.h>
#include "api/lua.h"
#include "utils/png.h"
#include "utils/error_handling.h"
#include "hardware/ram.h"
#include "nibble8.h"

// Platform independent clipboard functions
typedef char* (*GetClipboardTextFunc)();
typedef int (*SetClipboardTextFunc)(const char *);
typedef void (*FreeClipboardTextFunc)(void*);

extern GetClipboardTextFunc getClipboardText;
extern SetClipboardTextFunc setClipboardText;
extern FreeClipboardTextFunc freeClipboardText;
extern char *execPath;

// Other methods
void nibble_api_reboot();
void nibble_change_to_sandbox_directory(const char *exec_path);
int nibble_is_within_sandbox(const char *path);
char *nibble_api_ls(char *path);
char *nibble_api_read_file(char *path);
int nibble_api_change_dir(char *path);
int nibble_api_write_file(char *path, char *data);
const uint8_t *nibble_api_get_clipboard_text();
int nibble_api_set_clipboard_text(const uint8_t *text);
const char *nibble_api_get_code();
void nibble_api_run_code(uint8_t *code);
int nibble_load_rom(void);

#endif