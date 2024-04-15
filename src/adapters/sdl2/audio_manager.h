#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SDL2/SDL.h>
#include <string.h> // For memcpy
#include "hardware/ram.h"
#include "hardware/audio.h"
#include "utils/pocketmod.h"
#include "debug/debug.h"

extern SDL_AudioDeviceID device;

static void audio_callback(void *userdata, Uint8 *buffer, int bytes);
void audio_init();
void audio_quit();

#endif // AUDIO_MANAGER_H
