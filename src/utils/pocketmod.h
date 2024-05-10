// based on https://github.com/rombankzero/pocketmod
/* See end of file for license */

#ifndef POCKETMOD_H_INCLUDED
#define POCKETMOD_H_INCLUDED

#include <math.h>
#ifdef DOS
#include <allegro.h>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pocketmod_context pocketmod_context;

int _pocketmod_min(int x, int y);
int _pocketmod_max(int x, int y);
void _pocketmod_next_line(pocketmod_context *c);
void _pocketmod_zero(void *data, int size);
int pocketmod_init(pocketmod_context *c, const void *data, int size, int rate);
int pocketmod_render(pocketmod_context *c, void *buffer, int size);
int pocketmod_render_u8(pocketmod_context *c, void *buffer, int buffer_size);
int pocketmod_loop_count(pocketmod_context *c);
int16_t _pocketmod_clamp_int(int value, int min_val, int max_val);
void _pocketmod_next_tick(pocketmod_context *c);

#ifndef POCKETMOD_MAX_CHANNELS
#define POCKETMOD_MAX_CHANNELS 32
#endif

#ifndef POCKETMOD_MAX_SAMPLES
#define POCKETMOD_MAX_SAMPLES 31
#endif

typedef struct {
    uint8_t name[23];      /* Sample name                             */
    int8_t *data;          /* Sample data buffer                      */
    uint32_t length;       /* Data length (in bytes)                  */
} _pocketmod_sample;

typedef struct {
    uint8_t dirty;        /* Pitch/volume dirty flags                */
    uint8_t sample;       /* Sample number (0..31)                   */
    uint8_t volume;       /* Base volume without tremolo (0..64)     */
    uint8_t balance;      /* Stereo balance (0..255)                 */
    uint16_t period;      /* Note period (113..856)                  */
    uint16_t delayed;     /* Delayed note period (113..856)          */
    uint16_t target;      /* Target period (for tone portamento)     */
    uint8_t finetune;     /* Note finetune (0..15)                   */
    uint8_t loop_count;   /* E6x loop counter                        */
    uint8_t loop_line;    /* E6x target line                         */
    uint8_t lfo_step;     /* Vibrato/tremolo LFO step counter        */
    uint8_t lfo_type[2];  /* LFO type for vibrato/tremolo            */
    uint8_t effect;       /* Current effect (0x0..0xf or 0xe0..0xef) */
    uint8_t param;        /* Raw effect parameter value              */
    uint8_t param3;       /* Parameter memory for 3xx                */
    uint8_t param4;       /* Parameter memory for 4xy                */
    uint8_t param7;       /* Parameter memory for 7xy                */
    uint8_t param9;       /* Parameter memory for 9xx                */
    uint8_t paramE1;      /* Parameter memory for E1x                */
    uint8_t paramE2;      /* Parameter memory for E2x                */
    uint8_t paramEA;      /* Parameter memory for EAx                */
    uint8_t paramEB;      /* Parameter memory for EBx                */
    uint8_t real_volume;  /* Volume (with tremolo adjustment)        */
    float position;             /* Position in sample data buffer          */
    float increment;            /* Position increment per output sample    */
} _pocketmod_chan;

struct pocketmod_context
{
    /* Read-only song data */
    _pocketmod_sample samples[POCKETMOD_MAX_SAMPLES];
    uint32_t source_size; /* Size of MOD data                        */
    uint8_t *source;      /* Pointer to source MOD data              */
    uint8_t *order;       /* Pattern order table                     */
    uint8_t *patterns;    /* Start of pattern data                   */
    uint8_t length;       /* Patterns in the order (1..128)          */
    uint8_t reset;        /* Pattern to loop back to (0..127)        */
    uint8_t num_patterns; /* Patterns in the file (1..128)           */
    uint8_t num_samples;  /* Sample count (15 or 31)                 */
    uint8_t num_channels; /* Channel count (1..32)                   */

    /* Timing variables */
    int32_t samples_per_second;     /* Sample rate (set by user)               */
    int32_t ticks_per_line;         /* A.K.A. song speed (initially 6)         */
    float samples_per_tick;     /* Depends on sample rate and BPM          */

    /* Loop detection state */
    uint8_t visited[16];  /* Bit mask of previously visited patterns */
    int loop_count;             /* How many times the song has looped      */

    /* Render state */
    _pocketmod_chan channels[POCKETMOD_MAX_CHANNELS];
    uint8_t pattern_delay;/* EEx pattern delay counter               */
    uint32_t lfo_rng;       /* RNG used for the random LFO waveform    */

    /* Position in song (from least to most granular) */
    int8_t pattern;        /* Current pattern in order                */
    int8_t line;           /* Current line in pattern                 */
    int16_t tick;                 /* Current tick in line                    */
    float sample;               /* Current sample in tick                  */
};

/* Memorize a parameter unless the new value is zero */
#define POCKETMOD_MEM(dst, src) do { \
        (dst) = (src) ? (src) : (dst); \
    } while (0)

/* Same thing, but memorize each nibble separately */
#define POCKETMOD_MEM2(dst, src) do { \
        (dst) = (((src) & 0x0f) ? ((src) & 0x0f) : ((dst) & 0x0f)) \
              | (((src) & 0xf0) ? ((src) & 0xf0) : ((dst) & 0xf0)); \
    } while (0)

/* Shortcut to sample metadata (sample must be nonzero) */
#define POCKETMOD_SAMPLE(c, sample) ((c)->source + 12 + 30 * (sample))

/* Channel dirty flags */
#define POCKETMOD_PITCH  0x01
#define POCKETMOD_VOLUME 0x02

/* The size of one sample in bytes */
#define POCKETMOD_SAMPLE_SIZE sizeof(float[2])
#define POCKETMOD_SAMPLE_SIZE_U8 sizeof(uint8_t[2])

/* Finetune adjustment table. Three octaves for each finetune setting. */
static const signed char _pocketmod_finetune[16][36] = {
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
    { -6, -6, -5, -5, -4, -3, -3, -3, -3, -3, -3, -3, -3, -3, -2, -3, -2, -2, -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0},
    {-12,-12,-10,-11, -8, -8, -7, -7, -6, -6, -6, -6, -6, -6, -5, -5, -4, -4, -4, -3, -3, -3, -3, -2, -3, -3, -2, -3, -3, -2, -2, -2, -2, -2, -2, -1},
    {-18,-17,-16,-16,-13,-12,-12,-11,-10,-10,-10, -9, -9, -9, -8, -8, -7, -6, -6, -5, -5, -5, -5, -4, -5, -4, -3, -4, -4, -3, -3, -3, -3, -2, -2, -2},
    {-24,-23,-21,-21,-18,-17,-16,-15,-14,-13,-13,-12,-12,-12,-11,-10, -9, -8, -8, -7, -7, -7, -7, -6, -6, -6, -5, -5, -5, -4, -4, -4, -4, -3, -3, -3},
    {-30,-29,-26,-26,-23,-21,-20,-19,-18,-17,-17,-16,-15,-14,-13,-13,-11,-11,-10, -9, -9, -9, -8, -7, -8, -7, -6, -6, -6, -5, -5, -5, -5, -4, -4, -4},
    {-36,-34,-32,-31,-27,-26,-24,-23,-22,-21,-20,-19,-18,-17,-16,-15,-14,-13,-12,-11,-11,-10,-10, -9, -9, -9, -7, -8, -7, -6, -6, -6, -6, -5, -5, -4},
    {-42,-40,-37,-36,-32,-30,-29,-27,-25,-24,-23,-22,-21,-20,-18,-18,-16,-15,-14,-13,-13,-12,-12,-10,-10,-10, -9, -9, -9, -8, -7, -7, -7, -6, -6, -5},
    { 51, 48, 46, 42, 42, 38, 36, 34, 32, 30, 24, 27, 25, 24, 23, 21, 21, 19, 18, 17, 16, 15, 14, 14, 12, 12, 12, 10, 10, 10,  9,  8,  8,  8,  7,  7},
    { 44, 42, 40, 37, 37, 35, 32, 31, 29, 27, 25, 24, 22, 21, 20, 19, 18, 17, 16, 15, 15, 14, 13, 12, 11, 10, 10,  9,  9,  9,  8,  7,  7,  7,  6,  6},
    { 38, 36, 34, 32, 31, 30, 28, 27, 25, 24, 22, 21, 19, 18, 17, 16, 16, 15, 14, 13, 13, 12, 11, 11,  9,  9,  9,  8,  7,  7,  7,  6,  6,  6,  5,  5},
    { 31, 30, 29, 26, 26, 25, 24, 22, 21, 20, 18, 17, 16, 15, 14, 13, 13, 12, 12, 11, 11, 10,  9,  9,  8,  7,  8,  7,  6,  6,  6,  5,  5,  5,  5,  5},
    { 25, 24, 23, 21, 21, 20, 19, 18, 17, 16, 14, 14, 13, 12, 11, 10, 11, 10, 10,  9,  9,  8,  7,  7,  6,  6,  6,  5,  5,  5,  5,  4,  4,  4,  3,  4},
    { 19, 18, 17, 16, 16, 15, 15, 14, 13, 12, 11, 10,  9,  9,  9,  8,  8, 18,  7,  7,  7,  6,  5,  6,  5,  4,  5,  4,  4,  4,  4,  3,  3,  3,  3,  3},
    { 12, 12, 12, 10, 11, 11, 10, 10,  9,  8,  7,  7,  6,  6,  6,  5,  6,  5,  5,  5,  5,  4,  4,  4,  3,  3,  3,  3,  2,  3,  3,  2,  2,  2,  2,  2},
    {  6,  6,  6,  5,  6,  6,  6,  5,  5,  5,  4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  1,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1}
};

int pocketmod_init(pocketmod_context *c, const void *data, int size, int rate);
int pocketmod_render(pocketmod_context *c, void *buffer, int buffer_size);
int pocketmod_loop_count(pocketmod_context *c);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef POCKETMOD_H_INCLUDED */

/*******************************************************************************

MIT License

Copyright (c) 2018 rombankzero

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*******************************************************************************/
