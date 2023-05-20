#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

#define NUM_KEYS 12

extern const char* KEY[];

int key_from_scancode(SDL_Scancode scancode);

void keyboard_update();

int keyboard_get_state(int key);

int keyboard_get_state_changed(int key);

#endif /* KEYBOARD_H */
