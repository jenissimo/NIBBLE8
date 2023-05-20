#ifndef SYNTH_H
#define SYNTH_H

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "filters.h"

#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define NUM_SAMPLES 512
#define PI 3.14159265358979323846

#define NUM_NOTES 12

typedef enum
{
    TRIANGLE,
    TILTED_SAW,
    SAW,
    SQUARE,
    PULSE,
    NOISE,
    SINE
} Oscillator;

typedef struct
{
    Oscillator osc;
    float frequency;
    float time;

    float attackTime;
    float decayTime;
    float sustainAmplitude;
    float releaseTime;
    float startAmplitude;
    float triggerOffTime;
    float triggerOnTime;

    Filter **filters;
    int num_filters;

    bool note_on;
} SynthState;

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

SynthState synthState;

float w(float frequency);
float osc(float frequency, float time, Oscillator type);

void synth_init(SynthState *synth);
void synth_update_config(SynthState *synth, Oscillator osc, double attackTime, double decayTime, double sustainAmplitude, double releaseTime, double startAmplitude);
void synth_update_filter(SynthState *synth, FilterType filter, double value);
void synth_note_on(SynthState *synth, int note, int octave);
void synth_note_off(SynthState *synth);
float synth_get_amplitude(SynthState *synth, float time);
void synth_update(SynthState *synth, int16_t *stream, int num_samples);
float synth_clip(float sample, float max);

#endif
