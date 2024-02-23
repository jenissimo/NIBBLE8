#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "../../hardware/input.h"
#include "video_manager.h"

int input_init();
int input_update();

void updateButtonState();
int nibble_get_custom_key(int key);
void nibble_clear_keys();
void nibble_allegro_save_lua_keys_constants();
void nibble_allegro_print_lua_key(FILE *f, char *name, int key);

#endif // INPUT_MANAGER_H
