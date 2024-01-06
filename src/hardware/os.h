#ifndef nibble_os_h
#define nibble_os_h

#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include "../api/lua.h"
#include "../utils/miniz.h"
#include "../utils/png.h"
#include "../utils/error_handling.h"
#include "../hardware/ram.h"
#include "../nibble8.h"

// Platform independent clipboard functions
typedef char* (*GetClipboardTextFunc)();
typedef int (*SetClipboardTextFunc)(const char *);
typedef void (*FreeClipboardTextFunc)(void*);

extern GetClipboardTextFunc getClipboardText;
extern GetClipboardTextFunc setClipboardText;
extern FreeClipboardTextFunc freeClipboardText;

// Other methods
char *nibble_api_ls(char *path);
char *nibble_api_read_file(char *path);
int nibble_api_change_dir(char *path);
int nibble_api_write_file(char *path, char *data);
const uint8_t *nibble_api_get_clipboard_text();
int nibble_api_set_clipboard_text(const uint8_t *text);
ErrorCode nibble_api_load_cart(char *path);
const char *nibble_api_get_code();
void nibble_api_run_code(char *code);
int nibble_api_save_cart(char *path, char *luaCode);
void nibble_api_import_png(char *path);
void nibble_api_export_png(char *path);

// Zip functions
bool extract_file_to_buffer(const char *zip_filename, const char *file_to_load, void **buffer, size_t *buffer_size);
void load_file_from_zip(const char *zip_filename, const char *file_to_load, void **buffer);
void load_text_from_zip(const char *zip_filename, const char *file_to_load, char **buffer);

#endif