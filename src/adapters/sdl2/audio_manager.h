#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SDL2/SDL.h>
#include <string.h> // For memcpy
#include "hardware/ram.h"
#include "hardware/audio.h"
#include "utils/pocketmod.h"
#include "debug/debug.h"

#define LIMITER_THRESHOLD 0.95f  // Limiter threshold (e.g., 95% of maximum amplitude)
#define LIMITER_RELEASE_TIME 0.01f  // Release time in seconds

extern SDL_AudioDeviceID device;

static void audio_callback(void *userdata, Uint8 *buffer, int bytes);
void apply_limiter(float *buffer, int num_samples, float sampleRate);

void audio_init();
void audio_quit();

#endif // AUDIO_MANAGER_H
