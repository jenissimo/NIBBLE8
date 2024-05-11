#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include "debug/debug.h"
#ifdef DOS
#include <allegro.h>
#else
#include <stdint.h>
#endif
#include <string.h>
#include "utils/ini.h"

typedef struct
{
    int color[4][3];
    uint32_t argb[4];
} Palette;

typedef struct
{
    Palette mainPalette;
    Palette exportPalette;
} NibbleConfig;

extern NibbleConfig nibbleConfig;

int nibble_config_load(const char *filename);
int nibble_config_handler(void *user, const char *section, const char *name,
                          const char *value);
void nibble_config_calculate_argb(Palette *palette);

#endif