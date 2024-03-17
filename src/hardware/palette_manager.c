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

    DEBUG_LOG("Reading palette file: %s\n", ini_file);

    PaletteManager *manager = malloc(sizeof(PaletteManager));
    manager->palettes = malloc(num_palettes * sizeof(Palette));
    manager->num_palettes = num_palettes;
    manager->current_palette = 0;

    DEBUG_LOG("num_palettes: %d\n", num_palettes);

    SimpleIni ini;
    uint8_t r, g, b;
    int palette_index = 0;

    if (simple_ini_open(&ini, ini_file))
    {
        char key[256], value[256];
        Palette *palette = NULL;

        while (simple_ini_read_next_key_value(&ini, key, value))
        {
            if (ini.new_section_encountered)
            {
                if (palette != NULL)
                {
                    // Finalize previous palette
                    // printf("Section %d: %s\n", palette_index - 1, ini.current_section);
                    strncpy(palette->name, ini.current_section, sizeof(ini.current_section) - 1);
                    palette->name[sizeof(palette->name) - 1] = '\0';
                }

                // Start a new palette
                palette = &manager->palettes[palette_index++];
                ini.new_section_encountered = false; // Reset flag after processing new section
                continue;
            }

            // Parse and set color values
            int color_index;
            sscanf(key, "Color%d", &color_index);
            sscanf(value, "%d,%d,%d", &palette->color[color_index - 1][0], &palette->color[color_index - 1][1], &palette->color[color_index - 1][2]);

            // Cache for faster rendering
            r = palette->color[color_index - 1][0];
            g = palette->color[color_index - 1][1];
            b = palette->color[color_index - 1][2];
            palette->argb[color_index - 1] = (255 << 24) + ((int)r << 16) + ((int)g << 8) + (int)b;
        }

        // Process the last section
        if (palette != NULL)
        {
            // printf("Section %d: %s\n", palette_index - 1, ini.current_section);
            strncpy(palette->name, ini.current_section, sizeof(ini.current_section) - 1);
            palette->name[sizeof(palette->name) - 1] = '\0';
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
