#ifndef SIMPLE_INI_H
#define SIMPLE_INI_H

#include <stdio.h>
#include <stdbool.h>

typedef struct
{
    const char *filename;
    FILE *file;
    bool new_section_encountered;
    char current_section[256];
} SimpleIni;

bool simple_ini_open(SimpleIni *ini, const char *filename);
bool simple_ini_close(SimpleIni *ini);
bool simple_ini_read_next_section(SimpleIni *ini, char *section);
bool simple_ini_read_next_key_value(SimpleIni *ini, char *key, char *value);

#endif // SIMPLE_INI_H
