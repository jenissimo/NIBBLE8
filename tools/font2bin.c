#include <stdlib.h>
#include <stdint.h>
#include "../src/nibble8.h"
#include <png.h>
#include "../src/utils/png.h"

#define FONT_SIZE 128 * 128

uint8_t *font_raw;
uint8_t *font_compressed;

void load_font(char *file_name);
void compress_font();
void write_font(char *file_name);
unsigned int getBitPixel(png_structp png_ptr, png_infop info_ptr, png_bytep row, png_uint_32 x);

int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        font_raw = (uint8_t *)malloc(sizeof(uint8_t) * FONT_SIZE);
        font_compressed = (uint8_t *)malloc(sizeof(uint8_t) * FONT_SIZE_COMPRESSED);
        printf("Font size: %d\n", FONT_SIZE);
        load_font(argv[1]);
        printf("Compressing font...\n");
        compress_font();
        printf("Writing font...\n");
        write_font(argv[2]);
        printf("Done.\n");
    }
    else
    {
        fprintf(stderr, "font2bin: usage: font2bin [font.png] [font.bin]\n");
    }
    return 0;
}

void load_font(char *file_name)
{
    FILE *f = fopen(file_name, "rb");
    volatile png_bytep row = NULL;

    if (f != NULL)
    {
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                     NULL, NULL, NULL);

        if (png_ptr != NULL)
        {
            png_infop info_ptr = png_create_info_struct(png_ptr);

            if (info_ptr != NULL)
            {
                if (setjmp(png_jmpbuf(png_ptr)) == 0)
                {
                    png_uint_32 width, height;
                    int bit_depth, color_type, interlace_method,
                        compression_method, filter_method;
                    png_bytep row_tmp;

                    png_init_io(png_ptr, f);
                    png_read_info(png_ptr, info_ptr);
                    row = png_malloc(png_ptr, png_get_rowbytes(png_ptr,
                                                               info_ptr));
                    row_tmp = row;

                    if (png_get_IHDR(png_ptr, info_ptr, &width, &height,
                                     &bit_depth, &color_type, &interlace_method,
                                     &compression_method, &filter_method))
                    {
                        int passes, pass;

                        switch (interlace_method)
                        {
                        case PNG_INTERLACE_NONE:
                            passes = 1;
                            break;

                        case PNG_INTERLACE_ADAM7:
                            passes = PNG_INTERLACE_ADAM7_PASSES;
                            break;

                        default:
                            png_error(png_ptr, "pngpixel: unknown interlace");
                        }

                        png_start_read_image(png_ptr);

                        int pixelIndex = 0;

                        for (pass = 0; pass < passes; ++pass)
                        {
                            png_uint_32 ystart, xstart, ystep, xstep;
                            png_uint_32 py;

                            if (interlace_method == PNG_INTERLACE_ADAM7)
                            {
                                if (PNG_PASS_COLS(width, pass) == 0)
                                    continue;

                                xstart = PNG_PASS_START_COL(pass);
                                ystart = PNG_PASS_START_ROW(pass);
                                xstep = PNG_PASS_COL_OFFSET(pass);
                                ystep = PNG_PASS_ROW_OFFSET(pass);
                            }

                            else
                            {
                                ystart = xstart = 0;
                                ystep = xstep = 1;
                            }

                            for (py = ystart; py < height; py += ystep)
                            {
                                png_uint_32 px, ppx;

                                png_read_row(png_ptr, row_tmp, NULL);
                                for (px = xstart, ppx = 0; px < width; px += xstep, ++ppx)
                                {
                                    unsigned int pixel = getBitPixel(png_ptr, info_ptr, row_tmp, ppx);
                                    font_raw[pixelIndex++] = pixel > 0 ? 1 : 0;
                                    // printf("%d", font_raw[pixelIndex]);
                                    // pixelIndex++;
                                }
                            } /* y loop */
                        }     /* pass loop */
                        row = NULL;
                        png_free(png_ptr, row_tmp);
                    }
                    else
                    {
                        png_error(png_ptr, "pngpixel: png_get_IHDR failed");
                    }
                }
                else
                {
                    if (row != NULL)
                    {
                        png_bytep row_tmp = row;
                        row = NULL;
                        png_free(png_ptr, row_tmp);
                    }
                }

                png_destroy_info_struct(png_ptr, &info_ptr);
            }
            else
            {
                fprintf(stderr, "pngpixel: out of memory allocating png_info\n");
            }

            png_destroy_read_struct(&png_ptr, NULL, NULL);
        }
        else
        {
            fprintf(stderr, "pngpixel: out of memory allocating png_struct\n");
        }
    }
    else
    {
        fprintf(stderr, "pngpixel: %s: could not open file\n", file_name);
    }
}

uint8_t get_pixel(int x, int y)
{
    int pixelIndex = y * 128 + x;
    return font_raw[pixelIndex];
}

unsigned int getBitPixel(png_structp png_ptr, png_infop info_ptr, png_bytep row, png_uint_32 x)
{
    int color_type = png_get_color_type(png_ptr, info_ptr);
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY)
    {
        if (bit_depth == 8)
        {
            return row[x]; // For 8-bit grayscale images
        }
        else if (bit_depth == 16)
        {
            // For 16-bit grayscale images, assuming little endian
            return row[x * 2] | (row[x * 2 + 1] << 8);
        }
        // Add handling for other bit depths if necessary
    }
    else
    {
        // Add handling for other color types if necessary (e.g., PNG_COLOR_TYPE_RGB)
    }

    // Return a default value if the desired pixel cannot be extracted
    return 0;
}

void compress_font()
{
    int charRowIndex = 0;
    for (int charIndex = 0; charIndex < 256; charIndex++)
    {
        int charX = charIndex % 16 * 8;
        int charY = charIndex / 16 * 8;

        for (int y = charY; y < charY + 8; y++)
        {
            uint8_t charRow = 0;
            for (int x = charX; x < charX + 8; x++)
            {
                uint8_t pixel = get_pixel(x, y);
                if (pixel > 0)
                {
                    charRow |= 1 << (7 - (x % 8));
                }
            }
            font_compressed[charRowIndex++] = charRow;
        }
    }
}

void print_char(int charIndex)
{
    for (int j = charIndex * 8; j < (charIndex * 8 + 8); j++)
    {
        for (int i = 7; i >= 0; i--)
        {
            if ((font_compressed[j] >> i) & 1)
            {
                printf("█");
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void write_font(char *file_name)
{
    FILE *write_ptr;
    for (int i = 128; i < 144; i++)
    {
        print_char(i);
    }
    printf("font size: %d\n", FONT_SIZE_COMPRESSED);
    write_ptr = fopen(file_name, "wb");
    fwrite(font_compressed, sizeof(uint8_t), FONT_SIZE_COMPRESSED, write_ptr);
}