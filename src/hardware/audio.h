#ifndef nibble_audio_h
#define nibble_audio_h

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "audio/synth.h"

#define SAMPLE_RATE 22050
#define NUM_CHANNELS 1
#define NUM_SAMPLES 4096

enum
{
    FX_NO_EFFECT = 0,
    FX_SLIDE =     1,
    FX_VIBRATO =   2,
    FX_DROP =      3,
    FX_FADE_IN =   4,
    FX_FADE_OUT =  5,
    FX_ARP_FAST =  6,
    FX_ARP_SLOW =  7,
};

void nibble_audio_update(int16_t *stream, int num_samples);
float nibble_audio_clip(float sample, float threshold);
float note_to_frequency(int noteIndex, int octaveIndex);

void nibble_api_set_note(uint8_t sfx_index, uint8_t note_index, uint8_t pitch, uint8_t instrument, uint8_t volume, uint8_t effect);
void nibble_api_note_on(uint8_t note, uint8_t octave, uint8_t instrument);
void nibble_api_note_off();
void nibble_api_update_synth(uint8_t osc, double attackTime, double decayTime, double sustainAmplitude, double releaseTime, double startAmplitude);
void nibble_api_update_filter(float cutoffPercent, float resonancePercent, int mode);

#endif