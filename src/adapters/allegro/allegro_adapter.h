#ifndef ALLEGRO_ADAPTER_H
#define ALLEGRO_ADAPTER_H

#include <allegro.h>
#include "debug/debug.h"
#include "hardware/os.h"
#include "api/lua.h"

extern char* clipboardText;

void nibble_allegro_init();
void nibble_allegro_update();

char *nibble_allegro_get_clipboard();
int nibble_allegro_set_clipboard(const char *text);
void nibble_allgero_free_clipboard(void *ptr);

void nibble_allegro_quit();

#endif // ALLEGRO_ADAPTER_H
