#include "video.h"
#include "palette_manager.h"
#include "ram.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

uint32_t *frame;
uint8_t *font;
PaletteManager *manager;

void initVideo()
{
    frame = malloc(NIBBLE_WIDTH * NIBBLE_HEIGHT * sizeof(uint32_t));
    loadFont();
    loadPalettes();
    updateFrame();
    nibble_api_pal_reset();

    memory.drawState.camera_x = 0;
    memory.drawState.camera_y = 0;
}

void loadFont()
{
    font = malloc(FONT_SIZE_COMPRESSED * sizeof(uint8_t));
    FILE *f = fopen("font.bin", "rb");
    fread(font, 1, FONT_SIZE_COMPRESSED, f);
    fclose(f);
}

void loadPalettes()
{
    const char *ini_file = "palettes.ini";

    manager = palette_manager_create(ini_file);
    if (!manager)
    {
        printf("Error: Unable to create palette manager.\n");
    }
    manager->current_palette = 1;
}

void print_char(int charIndex)
{
    for (int j = charIndex * 8; j < (charIndex * 8 + 8); j++)
    {
        for (int i = 7; i >= 0; i--)
        {
            if ((font[j] >> i) & 1)
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

void destroyVideo()
{
    free(frame);
}

void nibble_api_cursor(int16_t x, int16_t y, uint8_t col)
{
}

void nibble_api_cls(uint8_t col)
{
    memset(memory.screenData, fullByteColors[col], NIBBLE_SCREEN_DATA_SIZE);
}

void nibble_api_pal(uint8_t c0, uint8_t c1, uint8_t p)
{
    // 0-15 alowed
    c0 &= 0x0f;
    if (p == 0)
    {
        // for draw palette we have to preserve the transparency bit
        memory.drawState.drawPaletteMap[c0] = (memory.drawState.drawPaletteMap[c0] & 0x10) | (c1 & 0xf);
    }
    else if (p == 1)
    {
        // 0-15, or 127-143 allowed
        c1 &= 0x8f;
        memory.drawState.screenPaletteMap[c0] = c1;
    }
}

void nibble_api_pal_reset()
{
    for (uint8_t c = 0; c < 4; c++)
    {
        memory.drawState.drawPaletteMap[c] = c;
        memory.drawState.screenPaletteMap[c] = c;
    }

    nibble_api_palt_reset();
}

void nibble_api_palt(uint8_t col, bool t)
{
    if (t)
    {
        memory.drawState.drawPaletteMap[col] |= 1UL << 4;
    }
    else
    {
        memory.drawState.drawPaletteMap[col] &= ~(1UL << 4);
    }
}

void nibble_api_palt_reset()
{
    memory.drawState.drawPaletteMap[0] |= 1UL << 4;
    for (uint8_t c = 1; c < 4; c++)
    {
        memory.drawState.drawPaletteMap[c] &= ~(1UL << 4);
    }
}

void nibble_api_circ(int16_t xc, int16_t yc, int16_t r, uint8_t col)
{
    int x = r;
    int y = 0;
    int decisionOver2 = 1 - x;

    while (y <= x)
    {
        nibble_api_pset(xc + x, yc + y, col);
        nibble_api_pset(xc + y, yc + x, col);
        nibble_api_pset(xc - x, yc + y, col);
        nibble_api_pset(xc - y, yc + x, col);

        nibble_api_pset(xc - x, yc - y, col);
        nibble_api_pset(xc - y, yc - x, col);
        nibble_api_pset(xc + x, yc - y, col);
        nibble_api_pset(xc + y, yc - x, col);

        y += 1;
        if (decisionOver2 < 0)
        {
            decisionOver2 = decisionOver2 + 2 * y + 1;
        }
        else
        {
            x = x - 1;
            decisionOver2 = decisionOver2 + 2 * (y - x) + 1;
        }
    }
}

void nibble_api_circfill(int16_t xc, int16_t yc, int16_t r, uint8_t col)
{
    if (r == 0)
    {
        nibble_api_pset(xc, yc, col);
    }
    else if (r == 1)
    {
        nibble_api_pset(xc, yc - 1, col);
        nibble_api_hline(xc - 1, xc + 1, yc, col);
        nibble_api_pset(xc, yc + 1, col);
    }
    else if (r > 0)
    {
        int x = -r, y = 0, err = 2 - 2 * r;
        do
        {
            nibble_api_hline(xc - x, xc + x, yc + y, col);
            nibble_api_hline(xc - x, xc + x, yc - y, col);
            r = err;
            if (r > x)
                err += ++x * 2 + 1;
            if (r <= y)
                err += ++y * 2 + 1;
        } while (x < 0);
    }
}

void nibble_api_pset(int16_t x, int16_t y, uint8_t col)
{
    uint16_t index;
    uint8_t bitPairIndex;

    x -= memory.drawState.camera_x;
    y -= memory.drawState.camera_y;

    if ((x < 0) || (y < 0))
        return;
    if ((x >= NIBBLE_WIDTH) || (y >= NIBBLE_HEIGHT))
        return;

    index = nibble_get_vram_byte_index(x, y, NIBBLE_WIDTH);
    bitPairIndex = nibble_get_vram_bitpair_index(x, y, NIBBLE_WIDTH);

    int shift = ((3 - bitPairIndex) * 2);

    memory.screenData[index] &= ~(3 << shift);
    memory.screenData[index] |= (col << shift);
}

uint8_t nibble_api_pget(int16_t x, int16_t y)
{
    uint16_t index;
    uint8_t bitPairIndex;

    if ((x < 0) || (y < 0))
        return 0;

    index = nibble_get_vram_byte_index(x, y, NIBBLE_WIDTH);
    bitPairIndex = nibble_get_vram_bitpair_index(x, y, NIBBLE_WIDTH);

    int shift = ((3 - bitPairIndex) * 2);
    int color = (memory.screenData[index] >> shift) & 0x03;

    return color;
}

void nibble_api_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t col)
{
    if (y1 == y2)
    {
        nibble_api_hline(x1, x2, y1, col);
        return;
    }

    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    for (;;)
    { /* loop */
        nibble_api_pset(x1, y1, col);
        if (x1 == x2 && y1 == y2)
            break;
        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x1 += sx;
        } /* e_xy+e_x > 0 */
        if (e2 <= dx)
        {
            err += dx;
            y1 += sy;
        } /* e_xy+e_y < 0 */
    }
}

// Optimized version of line drawing that uses full bytes for the aligned portion of the line
void nibble_api_hline(int16_t x1, int16_t x2, int16_t y, uint8_t color)
{
    if (x1 > x2)
    {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }

    if (y < 0 || y >= NIBBLE_HEIGHT)
        return;
    if (x1 < 0)
        x1 = 0;
    if (x2 >= NIBBLE_WIDTH)
        x2 = NIBBLE_WIDTH - 1;

    // Stage 1: Draw remaining pixels at the beginning of the line
    while (x1 < x2 && x1 % 4 != 0)
    {
        nibble_api_pset(x1, y, color);
        x1++;
    }

    // Stage 2: Draw full bytes for the aligned portion of the line
    while (x1 < x2 && (x2 - x1) >= 4)
    {
        int idx = ((y * NIBBLE_WIDTH) + x1) / 4;
        memory.screenData[idx] = fullByteColors[color];
        x1 += 4;
    }

    // Stage 3: Draw remaining pixels at the end of the line
    while (x1 <= x2)
    {
        nibble_api_pset(x1, y, color);
        x1++;
    }
}

void nibble_api_rect(int16_t x1, int16_t y1, int16_t width, int16_t height, uint8_t col)
{
    nibble_api_line(x1, y1, x1 + width - 1, y1, col);
    nibble_api_line(x1, y1, x1, y1 + height - 1, col);
    nibble_api_line(x1 + width - 1, y1, x1 + width - 1, y1 + height - 1, col);
    nibble_api_line(x1, y1 + height - 1, x1 + width - 1, y1 + height - 1, col);
}

void nibble_api_rectfill(int16_t x1, int16_t y1, int16_t width, int16_t height, uint8_t col)
{
    for (int i = 0; i < height; i++)
    {
        nibble_api_line(x1, y1 + i, x1 + width - 1, y1 + i, col);
    }
}

int nibble_api_print(char *text, int16_t x, int16_t y, uint8_t fg_color, uint8_t bg_color)
{
    int i = 0;

    // printf("print: %s\n", text);

    while (text[i] != '\0')
    {
        draw_char(text[i], x + (i * NIBBLE_FONT_WIDTH), y, fg_color, bg_color);
        i++;
    }

    return x + (i * 8);
}

void nibble_api_spr(int16_t sprIndex, int16_t x, int16_t y, uint8_t flipX, uint8_t flipY)
{
    uint16_t spriteX = (sprIndex % (NIBBLE_SPRITE_SHEET_WIDTH / NIBBLE_TILE_SIZE)) * NIBBLE_TILE_SIZE;
    uint16_t spriteY = (sprIndex / (NIBBLE_SPRITE_SHEET_WIDTH / NIBBLE_TILE_SIZE)) * NIBBLE_TILE_SIZE;
    uint8_t incX = flipX ? -1 : 1;
    uint8_t incY = flipY ? -1 : 1;
    uint16_t startX = flipX ? spriteX + NIBBLE_TILE_SIZE - 1 : spriteX;
    uint16_t startY = flipY ? spriteY + NIBBLE_TILE_SIZE - 1 : spriteY;
    uint16_t endX = flipX ? spriteX - 1 : spriteX + NIBBLE_TILE_SIZE;
    uint16_t endY = flipY ? spriteY - 1 : spriteY + NIBBLE_TILE_SIZE;

    for (uint16_t sprY = startY; sprY != endY; sprY += incY)
    {
        for (uint16_t sprX = startX; sprX != endX; sprX += incX)
        {
            uint8_t color = nibble_api_sget(sprX, sprY);
            if (!isColorTransparent(color))
            {
                setPixelFromSprite(x + sprX - startX, y + sprY - startY, color);
            }
        }
    }
}

void nibble_api_sspr(int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh, bool flip_x, bool flip_y)
{
    // printf("sspr: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", sx, sy, sw, sh, dx, dy, dw, dh, flip_x, flip_y);
    for (int j = 0; j < dh; j++)
    {
        for (int i = 0; i < dw; i++)
        {
            int si = (i * sw) / dw;
            int sj = (j * sh) / dh;

            if (flip_x)
            {
                si = sw - 1 - si;
            }

            if (flip_y)
            {
                sj = sh - 1 - sj;
            }

            uint8_t color = nibble_api_sget(sx + si, sy + sj);
            if (!isColorTransparent(color))
            {
                setPixelFromSprite(dx + i, dy + j, color);
            }
        }
    }
}

void nibble_api_sset(int16_t x, int16_t y, uint8_t col)
{
    uint16_t index;
    uint8_t bitPairIndex;

    if ((x < 0) || (y < 0))
        return;
    if ((x >= NIBBLE_SPRITE_SHEET_WIDTH) || (y >= NIBBLE_SPRITE_SHEET_HEIGHT))
        return;

    // printf("sset: %d, %d, %d\n", x, y, col);

    index = nibble_get_vram_byte_index(x, y, NIBBLE_SPRITE_SHEET_WIDTH);
    bitPairIndex = nibble_get_vram_bitpair_index(x, y, NIBBLE_SPRITE_SHEET_WIDTH);

    int shift = ((3 - bitPairIndex) * 2);

    memory.spriteSheetData[index] &= ~(3 << shift);
    memory.spriteSheetData[index] |= (col << shift);
}

uint8_t nibble_api_sget(int16_t x, int16_t y)
{
    uint16_t index;
    uint8_t bitPairIndex;

    if ((x < 0) || (y < 0))
        return 0;
    if ((x >= NIBBLE_SPRITE_SHEET_WIDTH) || (y >= NIBBLE_SPRITE_SHEET_WIDTH))
        return 0;

    index = nibble_get_vram_byte_index(x, y, NIBBLE_SPRITE_SHEET_WIDTH);
    bitPairIndex = nibble_get_vram_bitpair_index(x, y, NIBBLE_SPRITE_SHEET_WIDTH);

    return (memory.spriteSheetData[index] >> ((3 - bitPairIndex) * 2)) & 0x03;
}

void nibble_api_map(int celx, int cely, int sx, int sy, int celw, int celh, uint8_t layer)
{
    for (int y = 0; y < celh; y++)
    {
        for (int x = 0; x < celw; x++)
        {
            // Calculate the map cell index
            uint16_t mapIndex = (cely + y) * NIBBLE_MAP_WIDTH + (celx + x);

            // Check bounds
            if (mapIndex < 0 || mapIndex >= NIBBLE_MAP_COUNT)
                continue;

            // Retrieve the sprite index from the map data
            uint16_t spriteIndex = memory.mapData[mapIndex];

            // If a layer is specified, check sprite flags
            if (layer > 0)
            {
                uint8_t spriteFlags = memory.spriteFlagsData[spriteIndex];
                if ((spriteFlags & layer) != layer)
                    continue;
            }

            // Draw the sprite
            int drawX = sx + x * NIBBLE_TILE_SIZE;
            int drawY = sy + y * NIBBLE_TILE_SIZE;
            nibble_api_spr(spriteIndex, drawX, drawY, 0, 0);
        }
    }
}

void nibble_api_mset(uint16_t x, uint16_t y, int16_t sprite_number)
{
    uint32_t index = y * NIBBLE_MAP_WIDTH + x;
    if (index >= 0 && index < NIBBLE_MAP_COUNT)
    {
        memory.mapData[index] = sprite_number;
    }
}

int16_t nibble_api_mget(uint16_t x, uint16_t y)
{
    uint32_t index = y * NIBBLE_MAP_WIDTH + x;
    if (index >= 0 && index < NIBBLE_MAP_COUNT)
    {
        return memory.mapData[index];
    }
    return -1;
}

void draw_char(int charIndex, int16_t x, int16_t y, uint8_t fgCol, uint8_t bgCol)
{
    for (int j = charIndex * 8; j < (charIndex * 8 + NIBBLE_FONT_HEIGHT); j++)
    {
        for (int i = 7; i >= 8 - NIBBLE_FONT_WIDTH; i--)
        {
            if ((font[j] >> i) & 1)
            {
                nibble_api_pset(x + (7 - i), y + (j % 8), fgCol);
            }
            else
            {
                if (!isColorTransparent(bgCol))
                {
                    nibble_api_pset(x + (7 - i), y + (j % 8), bgCol);
                }
            }
        }
    }
}

void nibble_api_draw_fps(int fps)
{
    char fpsStr[10];
    sprintf(fpsStr, "%d", fps);
    uint8_t fpsWidth = 4 * strlen(fpsStr) + 1;
    uint8_t fpsX = 160 - fpsWidth;
    nibble_api_rectfill(fpsX, 0, fpsWidth, 7, 0);
    nibble_api_print(fpsStr, fpsX + 1, 1, 3, 0);
}

uint16_t nibble_get_vram_byte_index(int16_t x, int16_t y, uint16_t width)
{
    return (y * width + x) / NIBBLE_PIXELS_IN_BYTE;
}

uint16_t nibble_get_vram_bitpair_index(int16_t x, int16_t y, uint16_t width)
{
    return (y * width + x) % NIBBLE_PIXELS_IN_BYTE;
}

void moveCamera(int16_t dx, int16_t dy)
{
    memory.drawState.camera_x += dx;
    memory.drawState.camera_y += dy;
}

void setCameraPosition(int16_t x, int16_t y)
{
    memory.drawState.camera_x = x;
    memory.drawState.camera_y = y;
}

void setAndGetCamera(int16_t x, int16_t y, int16_t *prev_x, int16_t *prev_y)
{
    // Store the current camera position
    *prev_x = memory.drawState.camera_x;
    *prev_y = memory.drawState.camera_y;

    // Set the new camera position
    memory.drawState.camera_x = x;
    memory.drawState.camera_y = y;
}

void setPixelFromSprite(int16_t x, int16_t y, uint8_t col)
{
    uint16_t index;
    uint8_t bitPairIndex;

    if ((x < 0) || (y < 0))
        return;
    if ((x >= NIBBLE_WIDTH) || (y >= NIBBLE_HEIGHT))
        return;

    col = memory.drawState.drawPaletteMap[col & 0x0f] & 0x0f;

    index = nibble_get_vram_byte_index(x, y, NIBBLE_WIDTH);
    bitPairIndex = nibble_get_vram_bitpair_index(x, y, NIBBLE_WIDTH);

    int shift = ((3 - bitPairIndex) * 2);

    memory.screenData[index] &= ~(3 << shift);
    memory.screenData[index] |= (col << shift);
}

bool isColorTransparent(uint8_t color)
{
    color = color & 0x0f;
    return (memory.drawState.drawPaletteMap[color] >> 4) > 0; // upper bits indicate transparency
}

void updateFrame()
{
    int pixelIndex = 0;
    const Palette *palette = currentPalette(manager);
    uint8_t value;
    uint8_t col;

    for (int i = 0; i < NIBBLE_SCREEN_DATA_SIZE; i++)
    {
        value = memory.screenData[i];
        for (int bit = 7; bit >= 0; bit -= 2)
        {
            col = ((((value >> bit) & 0x01) << 1) | ((value >> (bit - 1)) & 0x01));
            frame[pixelIndex++] = palette->argb[col];
        }
    }
}

void printVRam()
{
    int v = 0;
    int col = 0;
    for (int i = 0; i < NIBBLE_SCREEN_DATA_SIZE; i++)
    {
        uint8_t value = memory.screenData[i];
        for (int bit = 7; bit >= 0; bit -= 2)
        {
            printf("%d", ((((value >> bit) & 0x01) << 1) | ((value >> (bit - 1)) & 0x01)));
            col++;
        }

        if (col == NIBBLE_WIDTH)
        {
            printf(" :%d\n", v);
            v++;
            col = 0;
        }
    }
}