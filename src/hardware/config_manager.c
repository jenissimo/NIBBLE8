#include "config_manager.h"

NibbleConfig nibbleConfig;

int nibble_config_load(const char *filename)
{
    if (ini_parse(filename, nibble_config_handler, &nibbleConfig) < 0)
    {
        return 0;
    }

    return 1;
}

int nibble_config_handler(void *user, const char *section, const char *name,
                          const char *value)
{
    NibbleConfig *config = (NibbleConfig *)user;

    if (strcmp(section, "MainPalette") == 0)
    {
        for (int i = 0; i < 4; ++i)
        {
            char colorKey[10];
            sprintf(colorKey, "Color%d", i + 1);
            if (strcmp(name, colorKey) == 0)
            {
                sscanf(value, "%d,%d,%d", &config->mainPalette.color[i][0],
                       &config->mainPalette.color[i][1],
                       &config->mainPalette.color[i][2]);
            }
        }
        nibble_config_calculate_argb(&config->mainPalette);
    }
    else if (strcmp(section, "ExportPalette") == 0)
    {
        for (int i = 0; i < 4; ++i)
        {
            char colorKey[10];
            sprintf(colorKey, "Color%d", i + 1);
            if (strcmp(name, colorKey) == 0)
            {
                sscanf(value, "%d,%d,%d", &config->exportPalette.color[i][0],
                       &config->exportPalette.color[i][1],
                       &config->exportPalette.color[i][2]);
            }
        }
        nibble_config_calculate_argb(&config->exportPalette);
    }
}

void nibble_config_calculate_argb(Palette *palette)
{
    for (int i = 0; i < 4; ++i)
    {
        palette->argb[i] = (255 << 24) | (palette->color[i][0] << 16) |
                           (palette->color[i][1] << 8) | palette->color[i][2];
    }
}