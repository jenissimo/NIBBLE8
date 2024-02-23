#ifndef ALLEGRO_ADAPTER_H
#define ALLEGRO_ADAPTER_H

#include <allegro.h>
#include "../../debug/debug.h"
#include "../../hardware/os.h"
#include "../../api/lua.h"

void nibble_allegro_init();
void nibble_allegro_update();
void nibble_allegro_quit();

#endif // ALLEGRO_ADAPTER_H
