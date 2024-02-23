#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <string.h> // For memcpy
#include "../../hardware/audio.h" // Adjust path as needed
#include "../../debug/debug.h"

void audio_callback(void *userdata, uint8_t *stream, int len);
void audio_init();
void audio_quit();

#endif // AUDIO_MANAGER_H
