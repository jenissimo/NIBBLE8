#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "simple_ini.h"

bool simple_ini_open(SimpleIni *ini, const char *filename)
{
    ini->filename = filename;
    ini->file = fopen(filename, "r");
    ini->new_section_encountered = false;
    ini->current_section_changed = false; // Initialize the flag
    if (ini->file)
    {
        fseek(ini->file, 0, SEEK_SET); // Reset file position
        return true;
    }
    return false;
}

bool simple_ini_close(SimpleIni *ini)
{
    if (ini->file)
    {
        fclose(ini->file);
        ini->file = NULL;
        return true;
    }
    return false;
}

bool simple_ini_read_next_section(SimpleIni *ini, char *section)
{
    if (!ini->file)
    {
        return false;
    }

    if (ini->new_section_encountered)
    {
        // We already have the next section from the previous key-value read
        ini->new_section_encountered = false;
        return true;
    }

    char line[256];
    while (fgets(line, sizeof(line), ini->file))
    {
        line[strcspn(line, "\r\n")] = 0; // Remove newline
        if (line[0] == '[')
        {
            sscanf(line, "[%255[^]]", section);
            strcpy(ini->current_section, section); // Store the section name in current_section
            return true;
        }
    }

    return false;
}

bool simple_ini_read_next_key_value(SimpleIni *ini, char *key, char *value)
{
    if (!ini->file)
    {
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), ini->file))
    {
        line[strcspn(line, "\r\n")] = 0; // Remove newline

        if (line[0] == ';' || line[0] == '\0')
        {
            continue;
        }
        else if (line[0] == '[')
        {
            sscanf(line, "[%255[^]]", ini->current_section);
            ini->new_section_encountered = true;
            ini->current_section_changed = true; // Set flag on new section
            return true;
        }
        else if (sscanf(line, "%255[^=]=%255s", key, value) == 2)
        {
            ini->current_section_changed = false; // Reset the flag if it's a regular key-value line
            return true;
        }
    }

    return false; // No more key-value pairs
}