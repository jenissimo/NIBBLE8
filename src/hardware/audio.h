#ifndef nibble_audio_h
#define nibble_audio_h

#include <stdint.h>
#include "audio/synth.h"

void nibble_api_note_on(uint8_t note, uint8_t octave);
void nibble_api_note_off();
void nibble_api_update_synth(uint8_t osc, double attackTime, double decayTime, double sustainAmplitude, double releaseTime, double startAmplitude);
void nibble_api_update_filter(uint8_t filter, double value);

#endif