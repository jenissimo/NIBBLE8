#include "palette_manager.h"

int count_palettes(const char *ini_file)
{
    SimpleIni ini;
    int palette_count = 0;

    if (simple_ini_open(&ini, ini_file))
    {
        char section[256];

        while (simple_ini_read_next_section(&ini, section))
        {
            palette_count++;
        }

        simple_ini_close(&ini);
    }

    return palette_count;
}

PaletteManager *palette_manager_create(const char *ini_file)
{
    int num_palettes = count_palettes(ini_file);
    if (num_palettes == 0)
    {
        return NULL;
    }

    PaletteManager *manager = malloc(sizeof(PaletteManager));
    manager->palettes = malloc(num_palettes * sizeof(Palette));
    manager->num_palettes = num_palettes;
    manager->current_palette = 0;

    int palette_index = 0;
    SimpleIni ini;
    uint8_t r;
    uint8_t g;
    uint8_t b;

    if (simple_ini_open(&ini, ini_file))
    {
        char section[256];
        char key[256];
        char value[256];

        while (simple_ini_read_next_section(&ini, section))
        {
            Palette *palette = &manager->palettes[palette_index];

            strncpy(palette->name, section, sizeof(palette->name) - 1);
            palette->name[sizeof(palette->name) - 1] = '\0';

            while (simple_ini_read_next_key_value(&ini, key, value))
            {
                int color_index;
                sscanf(key, "Color%d", &color_index);
                sscanf(value, "%d,%d,%d", &palette->color[color_index - 1][0], &palette->color[color_index - 1][1], &palette->color[color_index - 1][2]);

                // Cache for faster rendering
                r = palette->color[color_index - 1][0];
                g = palette->color[color_index - 1][1];
                b = palette->color[color_index - 1][2];
                palette->argb[color_index - 1] = (255 << 24) + ((int)r << 16) + ((int)g << 8) + (int)b;
            }

            palette_index++;
        }

        simple_ini_close(&ini);
    }

    return manager;
}

void palette_manager_destroy(PaletteManager *manager)
{
    if (manager)
    {
        if (manager->palettes)
        {
            free(manager->palettes);
        }
        free(manager);
    }
}

void nextPalette(PaletteManager *manager)
{
    manager->current_palette = (manager->current_palette + 1) % manager->num_palettes;
    printf("%s\n", manager->palettes[manager->current_palette].name);
}

void prevPalette(PaletteManager *manager)
{
    manager->current_palette--;
    if (manager->current_palette < 0)
    {
        manager->current_palette = manager->num_palettes - 1;
    }
    printf("%s\n", manager->palettes[manager->current_palette].name);
}

const Palette *currentPalette(PaletteManager *manager)
{
    return &manager->palettes[manager->current_palette];
}
