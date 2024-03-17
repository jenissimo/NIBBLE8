#ifndef PALETTE_MANAGER_H
#define PALETTE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include "debug/debug.h"
#ifdef DOS
#include <allegro.h>
#else
#include <stdint.h>
#endif
#include <string.h>
#include "utils/simple_ini.h"

typedef struct {
    char name[256];
    int color[4][3];
    uint32_t argb[4];
} Palette;

typedef struct {
    Palette *palettes;
    int num_palettes;
    int current_palette;
} PaletteManager;

PaletteManager *palette_manager_create(const char *ini_file);
void palette_manager_destroy(PaletteManager *manager);
void nextPalette(PaletteManager *manager);
void prevPalette(PaletteManager *manager);
const Palette *currentPalette(PaletteManager *manager);

#endif // PALETTE_MANAGER_H
