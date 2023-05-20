#ifndef NIBBLE_SDL_ADAPTER_H
#define NIBBLE_SDL_ADAPTER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

static uint32_t next_time;

uint32_t time_left(void);
static void audio_callback(void *userdata, Uint8 *stream, int len);
int nibble_sdl_init(void);
void nibble_clear_keys();
int nibble_get_custom_key(int key);
void nibble_sdl_print_lua_key(FILE *f, char *name, int key);
void nibble_sdl_save_lua_keys_constants();
int nibble_sdl_update(void);
int nibble_sdl_quit(void);
void make_screenshot();
void updateButtonState();
SDL_Point convertMouseCoordinates(int mouseX, int mouseY);
void goFullScreen();
void updateAspectRatio(SDL_Renderer *renderer, int windowWidth, int windowHeight, int targetWidth, int targetHeight);

#endif