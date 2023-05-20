#include "audio.h"

void nibble_api_note_on(uint8_t note, uint8_t octave)
{
    synth_note_on(&synthState, note, octave);
}

void nibble_api_note_off()
{
    synth_note_off(&synthState);
}

void nibble_api_update_synth(uint8_t osc, double attackTime, double decayTime, double sustainAmplitude, double releaseTime, double startAmplitude)
{
    synth_update_config(&synthState, osc, attackTime, decayTime, sustainAmplitude, releaseTime, startAmplitude);
}

void nibble_api_update_filter(uint8_t filter, double value)
{
    synth_update_filter(&synthState, filter, value);
}