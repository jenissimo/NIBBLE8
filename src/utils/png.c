#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <setjmp.h> /* required for error handling */
#include <math.h>
#include <memory.h>

#include "../nibble8.h"
#include "../debug/debug.h"
#include "png.h"

int find_closest_palette_index(const Palette *palette, uint8_t gray_value)
{
   int min_distance = INT32_MAX;
   int closest_index = -1;

   for (int i = 0; i < 4; i++)
   {
      uint8_t palette_gray = (uint8_t)round(0.299 * palette->color[i][0] + 0.587 * palette->color[i][1] + 0.114 * palette->color[i][2]);
      int distance = abs(palette_gray - gray_value);

      if (distance < min_distance)
      {
         min_distance = distance;
         closest_index = i;
      }
   }

   return closest_index;
}

void read_png_from_memory(png_structp png_ptr, png_bytep data, png_size_t length)
{
   unsigned char **png_mem_data = (unsigned char **)png_get_io_ptr(png_ptr);
   memcpy(data, *png_mem_data, length);
   *png_mem_data += length;
}

void read_and_convert_png_from_buffer(uint8_t *dest, uint8_t *png_data, int targetWidth, int targetHeight, const Palette *palette)
{
   int width, height;
   png_structp png_ptr;
   png_infop info_ptr;

   if (png_sig_cmp(png_data, 0, 8))
   {
      fprintf(stderr, "Error: Not a PNG file.\n");
      return;
   }

   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (!png_ptr)
   {
      printf("Failed to create PNG read struct\n");
      return;
   }

   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr)
   {
      printf("Failed to create PNG info struct\n");
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      return;
   }

   if (setjmp(png_jmpbuf(png_ptr)))
   {
      printf("Error reading PNG data\n");
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return;
   }

   png_set_read_fn(png_ptr, &png_data, read_png_from_memory);
   png_read_info(png_ptr, info_ptr);

   width = png_get_image_width(png_ptr, info_ptr);
   height = png_get_image_height(png_ptr, info_ptr);

   if (targetWidth != width || targetHeight != height)
   {
      printf("Error: Image dimensions do not match the target dimensions.\n");
      printf("Target dimensions: %dx%d\n", targetWidth, targetHeight);
      printf("Image dimensions: %dx%d\n", width, height);
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return;
   }

   png_set_expand(png_ptr);
   png_set_gray_to_rgb(png_ptr);
   png_set_strip_16(png_ptr);
   png_set_strip_alpha(png_ptr);
   png_set_packing(png_ptr);
   png_read_update_info(png_ptr, info_ptr);

   // Allocate memory for row pointers and ensure proper freeing
   png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
   if (!row_pointers)
   {
      DEBUG_LOG("Failed to allocate memory for row pointers");
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return;
   }

   for (int i = 0; i < height; i++)
   {
      row_pointers[i] = (png_byte *)malloc(png_get_rowbytes(png_ptr, info_ptr));
      if (!row_pointers[i])
      {
         DEBUG_LOG("Failed to allocate memory for row %d", i);
         // Free previously allocated rows before returning
         for (int j = 0; j < i; j++)
         {
            free(row_pointers[j]);
         }
         free(row_pointers);
         png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
         return;
      }
   }

   png_read_image(png_ptr, row_pointers);

   // Convert the read pixel data into a grayscale image
   uint8_t *grayscale_pixels = (unsigned char *)malloc(width * height);
   for (int y = 0; y < height; y++)
   {
      for (int x = 0; x < width; x++)
      {
         int r = row_pointers[y][x * 3];
         int g = row_pointers[y][x * 3 + 1];
         int b = row_pointers[y][x * 3 + 2];
         grayscale_pixels[y * width + x] = (uint8_t)round(0.299 * r + 0.587 * g + 0.114 * b);
      }
   }

   // Map the grayscale values to the closest palette indices
   int row_bytes = (width + 3) / 4;
   for (int y = 0; y < height; y++)
   {
      for (int x = 0; x < width; x++)
      {
         uint8_t gray_value = grayscale_pixels[y * width + x];
         int index = find_closest_palette_index(palette, gray_value);

         int byte_index = y * row_bytes + x / 4;
         int bit_offset = (3 - (x % 4)) * 2;
         uint8_t byte_value = dest[byte_index];
         byte_value &= ~(0b11 << bit_offset);
         byte_value |= (index << bit_offset);
         dest[byte_index] = byte_value;
      }
   }

   // Free the grayscale_pixels array and row_pointers
   free(grayscale_pixels);

   // Free allocated memory
   for (int i = 0; i < height; i++)
   {
      free(row_pointers[i]);
   }
   free(row_pointers);
   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

void png_write_callback(png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_memory_write_state *state = (png_memory_write_state *)png_get_io_ptr(png_ptr);
   state->data = (uint8_t *)realloc(state->data, state->size + length);
   memcpy(state->data + state->size, data, length);
   state->size += length;
}

png_memory_write_state get_indexed_png(uint8_t *pixels, int width, int height, const Palette *palette)
{
   png_structp png_ptr;
   png_infop info_ptr;
   png_memory_write_state state = {NULL, 0};

   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (!png_ptr)
   {
      printf("Failed to create PNG write struct\n");
      return state;
   }

   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr)
   {
      printf("Failed to create PNG info struct\n");
      png_destroy_write_struct(&png_ptr, NULL);
      return state;
   }

   if (setjmp(png_jmpbuf(png_ptr)))
   {
      printf("Error: Failed to create PNG write and info structs.\n");
      return state;
   }

   // png_init_io(png_ptr, file);

   png_set_IHDR(png_ptr, info_ptr, width, height,
                NIBBLE_BPP, // Bit depth: 2 bits for a 2-bit indexed image
                PNG_COLOR_TYPE_PALETTE,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT);

   png_color png_palette[NIBBLE_PALETTE_SIZE];
   for (int i = 0; i < NIBBLE_PALETTE_SIZE; i++)
   {
      png_palette[i].red = palette->color[i][0];
      png_palette[i].green = palette->color[i][1];
      png_palette[i].blue = palette->color[i][2];
   }

   png_set_PLTE(png_ptr, info_ptr, png_palette, NIBBLE_PALETTE_SIZE);

   png_set_write_fn(png_ptr, &state, png_write_callback, NULL);
   png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

   png_write_info(png_ptr, info_ptr);

   int row_bytes = width / NIBBLE_PALETTE_SIZE;
   png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);

   for (int i = 0; i < height; i++)
   {
      row_pointers[i] = pixels + i * row_bytes;
   }

   png_write_image(png_ptr, row_pointers);
   png_write_end(png_ptr, NULL);

   free(row_pointers);
   png_destroy_write_struct(&png_ptr, &info_ptr);

   return state;
}

void write_indexed_png(const char *filename, uint8_t *pixels, int width, int height, const Palette *palette)
{
   png_memory_write_state state = get_indexed_png(pixels, width, height, palette);

   // Write the PNG data from the memory buffer to the file
   FILE *file = fopen(filename, "wb");
   if (!file)
   {
      DEBUG_LOG("Failed to open file '%s'", filename);
      free(state.data);
      return;
   }

   fwrite(state.data, 1, state.size, file);
   fclose(file);
   free(state.data);
}