#ifndef SDL_ADAPTER_H
#define SDL_ADAPTER_H

#include <SDL2/SDL.h>
#include "../../debug/debug.h"
#include "../../hardware/os.h"
#include "../../api/lua.h"

extern uint32_t next_time;

uint32_t time_left(void);
int nibble_sdl_init();
int nibble_sdl_update();
int nibble_sdl_quit();
void goFullScreen();

#endif // SDL_ADAPTER_H
