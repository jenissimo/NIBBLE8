#ifndef CART_H
#define CART_H

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

ErrorCode nibble_api_load_cart(const char *path);
void loadLuaCodeFromCart(const char *path);
void loadMapDataFromCart(const char *path);
void processMapData(const uint8_t *buffer, size_t bufferSize);
void loadSpriteSheetFromCart(const char *path);

int nibble_api_save_cart(char *path, char *luaCode);
void nibble_api_import_png(char *path);
void nibble_api_export_png(char *path);
void nibble_api_import_lua(char *path);
void nibble_api_export_lua(char *path);

// Zip functions
bool cart_has_file(const char *zip_filename, const char *file_to_load);
bool extract_file_to_buffer(const char *zip_filename, const char *file_to_load, void **buffer, size_t *buffer_size);
size_t load_file_from_zip(const char *zip_filename, const char *file_to_load, void **buffer);
void load_text_from_zip(const char *zip_filename, const char *file_to_load, char **buffer);

#endif