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

    //DEBUG_LOG("Reading palette file: %s\n", ini_file);

    PaletteManager *manager = malloc(sizeof(PaletteManager));
    manager->palettes = malloc(num_palettes * sizeof(Palette));
    manager->num_palettes = num_palettes;
    manager->current_palette = 0;

    //DEBUG_LOG("num_palettes: %d\n", num_palettes);

    SimpleIni ini;
    uint8_t r, g, b;
    int palette_index = 0;
    bool new_palette_needed = true;
    Palette *palette = NULL;

    if (simple_ini_open(&ini, ini_file))
    {
        char key[256], value[256];
        bool isFirstSection = true; // Flag to handle the first section

        while (simple_ini_read_next_key_value(&ini, key, value))
        {
            // When a new section is detected or we need a new palette
            if (ini.current_section_changed || isFirstSection)
            {
                palette = &manager->palettes[palette_index++];
                // printf("palette_index: %d\n", palette_index);
                //printf("palette: %s\n", ini.current_section);
                strncpy(palette->name, ini.current_section, sizeof(palette->name) - 1);
                palette->name[sizeof(palette->name) - 1] = '\0';
                ini.current_section_changed = false; // Reset flag
                isFirstSection = false;              // Clear the first section flag after handling it
            }

            // Parse and set color values
            int color_index = 0;
            if (sscanf(key, "Color%d", &color_index) == 1 && color_index >= 1 && color_index <= 4)
            {
                sscanf(value, "%hhu,%hhu,%hhu", &r, &g, &b);
                palette->color[color_index - 1][0] = r;
                palette->color[color_index - 1][1] = g;
                palette->color[color_index - 1][2] = b;

                // Cache for faster rendering
                palette->argb[color_index - 1] = (255 << 24) | (r << 16) | (g << 8) | b;
            }
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
    DEBUG_LOG("%s (%d)", manager->palettes[manager->current_palette].name, manager->current_palette);
}

void prevPalette(PaletteManager *manager)
{
    manager->current_palette--;
    if (manager->current_palette < 0)
    {
        manager->current_palette = manager->num_palettes - 1;
    }
    DEBUG_LOG("%s (%d)", manager->palettes[manager->current_palette].name, manager->current_palette);
}

const Palette *currentPalette(PaletteManager *manager)
{
    return &manager->palettes[manager->current_palette];
}
