#ifndef nibble_png_h
#define nibble_png_h

#include <png.h>
#include <stdint.h>
#include "miniz.h"
#include "../hardware/palette_manager.h"
#include "../hardware/video.h"

typedef struct {
    uint8_t *data;
    size_t size;
} png_memory_write_state;

int find_closest_palette_index(const Palette *palette, uint8_t gray_value);
uint8_t *read_and_convert_png(const char *filename, int *width, int *height, const Palette *palette);
void read_png_from_memory(png_structp png_ptr, png_bytep data, png_size_t length);
void read_and_convert_png_from_buffer(uint8_t *dest, uint8_t *png_data, int targetWidth, int targetHeight, const Palette *palette);
void png_write_callback(png_structp png_ptr, png_bytep data, png_size_t length);
png_memory_write_state get_indexed_png(uint8_t *pixels, int width, int height, const Palette *palette);
void write_indexed_png(const char *filename, uint8_t *pixels, int width, int height, const Palette *palette);

#endif