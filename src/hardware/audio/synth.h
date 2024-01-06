#ifndef nibble_synth_h
#define nibble_synth_h

#include <stdlib.h>
#include <math.h>

typedef enum
{
    INST_TRIANGLE,
    INST_TILTED_SAW,
    INST_SAW,
    INST_SQUARE,
    INST_PULSE,
    INST_NOISE,
    INST_SINE,
    INST_DARK_SYNTH
} Instrument;

typedef enum
{
    EFFECT_NONE,
    EFFECT_SLIDE,
    EFFECT_VIBRATO,
    EFFECT_DROP,
    EFFECT_FADE_IN,
    EFFECT_FADE_OUT,
    EFFECT_ARPEGGIO_FAST,
    EFFECT_ARPEGGIO_SLOW,
} NoteEffect;

float nibble_waveform(Instrument instrument, float advance);

#endif