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

void nibble_audio_init(int freq);
void nibble_audio_destroy();

#endif