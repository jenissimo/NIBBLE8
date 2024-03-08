#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <SDL.h>
#include "../../hardware/input.h"
#include "video_manager.h"

typedef struct {
    int x;
    int y;
} Point;

int input_init();
int input_update();

void updateButtonState();
Point convertMouseCoordinates(int mouseX, int mouseY);
int nibble_get_custom_key(int key);
void nibble_clear_keys();
void nibble_sdl_save_lua_keys_constants();
void nibble_sdl_print_lua_key(FILE *f, char *name, int key);

#endif // INPUT_MANAGER_H
