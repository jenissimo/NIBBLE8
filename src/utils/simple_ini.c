#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "simple_ini.h"

bool simple_ini_open(SimpleIni *ini, const char *filename) {
    ini->filename = filename;
    ini->file = fopen(filename, "r");
    return ini->file != NULL;
}

bool simple_ini_close(SimpleIni *ini) {
    if (ini->file) {
        fclose(ini->file);
        ini->file = NULL;
        return true;
    }
    return false;
}

bool simple_ini_read_next_section(SimpleIni *ini, char *section) {
    if (!ini->file) {
        return false;
    }

    char line[256];

    while (fgets(line, sizeof(line), ini->file)) {
        if (line[0] == '[') {
            sscanf(line, "[%[^]]", section);
            return true;
        }
    }

    return false;
}

bool simple_ini_read_next_key_value(SimpleIni *ini, char *key, char *value) {
    if (!ini->file) {
        return false;
    }

    char line[256];

    while (fgets(line, sizeof(line), ini->file)) {
        if (line[0] == ';') {
            continue;
        } else if (line[0] == '[') {
            fseek(ini->file, -strlen(line), SEEK_CUR);
            return false;
        } else {
            sscanf(line, "%[^=]=%s", key, value);
            return true;
        }
    }

    return false;
}
