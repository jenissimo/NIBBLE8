#ifndef nibble_audio_h
#define nibble_audio_h

#include <stdlib.h>
#ifdef DOS
#include <allegro.h>
#else
#include <stdint.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "ram.h"

void nibble_audio_init(int freq);
void nibble_audio_reset();
void nibble_audio_play_note(pocketmod_context *context, TriggeredNote *note, float *output, int buffer_size);
void nibble_audio_stop_note();
int nibble_audio_note_to_period(int note);
void nibble_audio_destroy();

#endif