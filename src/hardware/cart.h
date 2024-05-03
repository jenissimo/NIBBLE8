#ifndef CART_H
#define CART_H

#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include "api/lua.h"
#include "utils/miniz.h"
#include "utils/png.h"
#include "utils/base64.h"
#include "utils/error_handling.h"
#include "hardware/os.h"
#include "hardware/ram.h"
#include "nibble8.h"

#define ZIP_HEADER_SIGNATURE "\x50\x4B\x03\x04"

ErrorCode nibble_api_load_cart(const char *path, char **adjustedPath);

ErrorCode loadCart(const char *path);
ErrorCode loadCartFromBase64(const char *base64);
ErrorCode loadCartFromZipBuffer(const uint8_t *buffer, size_t bufferSize);
void loadLuaCodeFromCart(mz_zip_archive *zip_archive);
void loadMapDataFromCart(mz_zip_archive *zip_archive);
void loadSpriteFlagsFromCart(mz_zip_archive *zip_archive);
void loadSpriteSheetFromCart(mz_zip_archive *zip_archive);

void processMapData(const uint8_t *buffer, size_t bufferSize);

int nibble_api_save_cart(char *path, char *luaCode);
void nibble_api_import_png(char *path);
void nibble_api_export_png(char *path);
void nibble_api_import_lua(char *path);
void nibble_api_export_lua(char *path);

// Zip functions
bool cart_find_zip(const char *path, mz_zip_archive *zip_archive, uint8_t **outBuffer);
bool cart_has_file(mz_zip_archive *zip_archive, const char *file_to_load);
bool extract_file_to_buffer(mz_zip_archive *zip_archive, const char *file_to_load, void **buffer, size_t *buffer_size);
size_t load_file_from_zip(mz_zip_archive *zip_archive, const char *file_to_load, void **buffer);
void load_text_from_zip(mz_zip_archive *zip_archive, const char *file_to_load, char **buffer);

#endif