#include "pocketmod.h"
#include <stdio.h>
#include <string.h>

/* Min/max helper functions */
int _pocketmod_min(int x, int y) { return x < y ? x : y; }
int _pocketmod_max(int x, int y) { return x > y ? x : y; }

/* Clamp a volume value to the 0..64 range */
static int _pocketmod_clamp_volume(int x)
{
    x = _pocketmod_max(x, 0x00);
    x = _pocketmod_min(x, 0x40);
    return x;
}

/* Clamp an integer value to the specified range */
int16_t _pocketmod_clamp_int(int value, int min_val, int max_val)
{
    if (value < min_val)
    {
        return (int16_t)min_val;
    }
    else if (value > max_val)
    {
        return (int16_t)max_val;
    }
    else
    {
        return (int16_t)value;
    }
}

/* Zero out a block of memory */
void _pocketmod_zero(void *data, int size)
{
    uint8_t *byte = data;
    for (int i = 0; i < size; i++)
    {
        byte[i] = 0;
    }
}

/* Convert a period (at finetune = 0) to a note index in 0..35 */
static int _pocketmod_period_to_note(int period)
{
    switch (period)
    {
    case 856:
        return 0;
    case 808:
        return 1;
    case 762:
        return 2;
    case 720:
        return 3;
    case 678:
        return 4;
    case 640:
        return 5;
    case 604:
        return 6;
    case 570:
        return 7;
    case 538:
        return 8;
    case 508:
        return 9;
    case 480:
        return 10;
    case 453:
        return 11;
    case 428:
        return 12;
    case 404:
        return 13;
    case 381:
        return 14;
    case 360:
        return 15;
    case 339:
        return 16;
    case 320:
        return 17;
    case 302:
        return 18;
    case 285:
        return 19;
    case 269:
        return 20;
    case 254:
        return 21;
    case 240:
        return 22;
    case 226:
        return 23;
    case 214:
        return 24;
    case 202:
        return 25;
    case 190:
        return 26;
    case 180:
        return 27;
    case 170:
        return 28;
    case 160:
        return 29;
    case 151:
        return 30;
    case 143:
        return 31;
    case 135:
        return 32;
    case 127:
        return 33;
    case 120:
        return 34;
    case 113:
        return 35;
    default:
        return 0;
    }
}

/* Table-based sine wave oscillator */
static int _pocketmod_sin(int step)
{
    /* round(sin(x * pi / 32) * 255) for x in 0..15 */
    static const unsigned char sin[16] = {
        0x00, 0x19, 0x32, 0x4a, 0x62, 0x78, 0x8e, 0xa2,
        0xb4, 0xc5, 0xd4, 0xe0, 0xec, 0xf4, 0xfa, 0xfe};
    int x = sin[step & 0x0f];
    x = (step & 0x1f) < 0x10 ? x : 0xff - x;
    return step < 0x20 ? x : -x;
}

/* Oscillators for vibrato/tremolo effects */
static int _pocketmod_lfo(pocketmod_context *c, _pocketmod_chan *ch, int step)
{
    switch (ch->lfo_type[ch->effect == 7] & 3)
    {
    case 0:
        return _pocketmod_sin(step & 0x3f); /* Sine   */
    case 1:
        return 0xff - ((step & 0x3f) << 3); /* Saw    */
    case 2:
        return (step & 0x3f) < 0x20 ? 0xff : -0xff; /* Square */
    case 3:
        return (c->lfo_rng & 0x1ff) - 0xff; /* Random */
    default:
        return 0; /* Hush little compiler */
    }
}

static void _pocketmod_update_pitch(pocketmod_context *c, _pocketmod_chan *ch)
{
    /* Don't do anything if the period is zero */
    ch->increment = 0.0f;
    if (ch->period)
    {
        float period = ch->period;

        /* Apply vibrato (if active) */
        if (ch->effect == 0x4 || ch->effect == 0x6)
        {
            int step = (ch->param4 >> 4) * ch->lfo_step;
            int rate = ch->param4 & 0x0f;
            period += _pocketmod_lfo(c, ch, step) * rate / 128.0f;

            /* Apply arpeggio (if active) */
        }
        else if (ch->effect == 0x0 && ch->param)
        {
            static const float arpeggio[16] = {/* 2^(X/12) for X in 0..15 */
                                               1.000000f, 1.059463f, 1.122462f, 1.189207f,
                                               1.259921f, 1.334840f, 1.414214f, 1.498307f,
                                               1.587401f, 1.681793f, 1.781797f, 1.887749f,
                                               2.000000f, 2.118926f, 2.244924f, 2.378414f};
            int step = (ch->param >> ((2 - c->tick % 3) << 2)) & 0x0f;
            period /= arpeggio[step];
        }

        /* Calculate sample buffer position increment */
        ch->increment = 3546894.6f / (period * c->samples_per_second);
    }

    /* Clear the pitch dirty flag */
    ch->dirty &= ~POCKETMOD_PITCH;
}

static void _pocketmod_update_volume(pocketmod_context *c, _pocketmod_chan *ch)
{
    int volume = ch->volume;
    if (ch->effect == 0x7)
    {
        int step = ch->lfo_step * (ch->param7 >> 4);
        volume += _pocketmod_lfo(c, ch, step) * (ch->param7 & 0x0f) >> 6;
    }
    ch->real_volume = _pocketmod_clamp_volume(volume);
    ch->dirty &= ~POCKETMOD_VOLUME;
}

static void _pocketmod_pitch_slide(_pocketmod_chan *ch, int amount)
{
    int max = 856 + _pocketmod_finetune[ch->finetune][0];
    int min = 113 + _pocketmod_finetune[ch->finetune][35];
    ch->period += amount;
    ch->period = _pocketmod_max(ch->period, min);
    ch->period = _pocketmod_min(ch->period, max);
    ch->dirty |= POCKETMOD_PITCH;
}

static void _pocketmod_volume_slide(_pocketmod_chan *ch, int param)
{
    /* Undocumented quirk: If both x and y are nonzero, then the value of x */
    /* takes precedence. (Yes, there are songs that rely on this behavior.) */
    int change = (param & 0xf0) ? (param >> 4) : -(param & 0x0f);
    ch->volume = _pocketmod_clamp_volume(ch->volume + change);
    ch->dirty |= POCKETMOD_VOLUME;
}

void _pocketmod_next_line(pocketmod_context *c)
{
    unsigned char(*data)[4];
    int i, pos, pattern_break = -1;

    /* When entering a new pattern order index, mark it as "visited" */
    if (c->line == 0)
    {
        c->visited[c->pattern >> 3] |= 1 << (c->pattern & 7);
    }

    /* Move to the next pattern if this was the last line */
    if (++c->line == 64)
    {
        if (++c->pattern == c->length)
        {
            c->pattern = c->reset;
        }
        c->line = 0;
    }

    /* Find the pattern data for the current line */
    pos = (c->order[c->pattern] * 64 + c->line) * c->num_channels * 4;
    data = (unsigned char(*)[4])(c->patterns + pos);
    for (i = 0; i < c->num_channels; i++)
    {

        /* Decode columns */
        int sample = (data[i][0] & 0xf0) | (data[i][2] >> 4);
        int period = ((data[i][0] & 0x0f) << 8) | data[i][1];
        int effect = ((data[i][2] & 0x0f) << 8) | data[i][3];

        /* Memorize effect parameter values */
        _pocketmod_chan *ch = &c->channels[i];
        ch->effect = (effect >> 8) != 0xe ? (effect >> 8) : (effect >> 4);
        ch->param = (effect >> 8) != 0xe ? (effect & 0xff) : (effect & 0x0f);

        /* Set sample */
        if (sample)
        {
            if (sample <= POCKETMOD_MAX_SAMPLES)
            {
                uint8_t *sample_data = POCKETMOD_SAMPLE(c, sample);
                ch->sample = sample;
                ch->finetune = sample_data[2] & 0x0f;
                ch->volume = _pocketmod_min(sample_data[3], 0x40);
                if (ch->effect != 0xED)
                {
                    ch->dirty |= POCKETMOD_VOLUME;
                }
            }
            else
            {
                ch->sample = 0;
            }
        }

        /* Set note */
        if (period)
        {
            int note = _pocketmod_period_to_note(period);
            period += _pocketmod_finetune[ch->finetune][note];
            if (ch->effect != 0x3)
            {
                if (ch->effect != 0xED)
                {
                    ch->period = period;
                    ch->dirty |= POCKETMOD_PITCH;
                    ch->position = 0.0f;
                    ch->lfo_step = 0;
                }
                else
                {
                    ch->delayed = period;
                }
            }
        }

        /* Handle pattern effects */
        switch (ch->effect)
        {

        /* Memorize parameters */
        case 0x3:
            POCKETMOD_MEM(ch->param3, ch->param); /* Fall through */
        case 0x5:
            POCKETMOD_MEM(ch->target, period);
            break;
        case 0x4:
            POCKETMOD_MEM2(ch->param4, ch->param);
            break;
        case 0x7:
            POCKETMOD_MEM2(ch->param7, ch->param);
            break;
        case 0xE1:
            POCKETMOD_MEM(ch->paramE1, ch->param);
            break;
        case 0xE2:
            POCKETMOD_MEM(ch->paramE2, ch->param);
            break;
        case 0xEA:
            POCKETMOD_MEM(ch->paramEA, ch->param);
            break;
        case 0xEB:
            POCKETMOD_MEM(ch->paramEB, ch->param);
            break;

        /* 8xx: Set stereo balance (nonstandard) */
        case 0x8:
        {
            ch->balance = ch->param;
        }
        break;

        /* 9xx: Set sample offset */
        case 0x9:
        {
            if (period != 0 || sample != 0)
            {
                ch->param9 = ch->param ? ch->param : ch->param9;
                ch->position = ch->param9 << 8;
            }
        }
        break;

        /* Bxx: Jump to pattern */
        case 0xB:
        {
            c->pattern = ch->param < c->length ? ch->param : 0;
            c->line = -1;
        }
        break;

        /* Cxx: Set volume */
        case 0xC:
        {
            ch->volume = _pocketmod_clamp_volume(ch->param);
            ch->dirty |= POCKETMOD_VOLUME;
        }
        break;

        /* Dxy: Pattern break */
        case 0xD:
        {
            pattern_break = (ch->param >> 4) * 10 + (ch->param & 15);
        }
        break;

        /* E4x: Set vibrato waveform */
        case 0xE4:
        {
            ch->lfo_type[0] = ch->param;
        }
        break;

        /* E5x: Set sample finetune */
        case 0xE5:
        {
            ch->finetune = ch->param;
            ch->dirty |= POCKETMOD_PITCH;
        }
        break;

        /* E6x: Pattern loop */
        case 0xE6:
        {
            if (ch->param)
            {
                if (!ch->loop_count)
                {
                    ch->loop_count = ch->param;
                    c->line = ch->loop_line;
                }
                else if (--ch->loop_count)
                {
                    c->line = ch->loop_line;
                }
            }
            else
            {
                ch->loop_line = c->line - 1;
            }
        }
        break;

        /* E7x: Set tremolo waveform */
        case 0xE7:
        {
            ch->lfo_type[1] = ch->param;
        }
        break;

        /* E8x: Set stereo balance (nonstandard) */
        case 0xE8:
        {
            ch->balance = ch->param << 4;
        }
        break;

        /* EEx: Pattern delay */
        case 0xEE:
        {
            c->pattern_delay = ch->param;
        }
        break;

        /* Fxx: Set speed */
        case 0xF:
        {
            if (ch->param != 0)
            {
                if (ch->param < 0x20)
                {
                    c->ticks_per_line = ch->param;
                }
                else
                {
                    float rate = c->samples_per_second;
                    c->samples_per_tick = rate / (0.4f * ch->param);
                }
            }
        }
        break;

        default:
            break;
        }
    }

    /* Pattern breaks are handled here, so that only one jump happens even  */
    /* when multiple Dxy commands appear on the same line. (You guessed it: */
    /* There are songs that rely on this behavior!)                         */
    if (pattern_break != -1)
    {
        c->line = (pattern_break < 64 ? pattern_break : 0) - 1;
        if (++c->pattern == c->length)
        {
            c->pattern = c->reset;
        }
    }
}

void _pocketmod_next_tick(pocketmod_context *c)
{
    int i;

    /* Move to the next line if this was the last tick */
    if (++c->tick == c->ticks_per_line)
    {
        if (c->pattern_delay > 0)
        {
            c->pattern_delay--;
        }
        else
        {
            _pocketmod_next_line(c);
        }
        c->tick = 0;
    }

    /* Make per-tick adjustments for all channels */
    for (i = 0; i < c->num_channels; i++)
    {
        _pocketmod_chan *ch = &c->channels[i];
        int param = ch->param;

        /* Advance the LFO random number generator */
        c->lfo_rng = 0x0019660d * c->lfo_rng + 0x3c6ef35f;

        /* Handle effects that may happen on any tick of a line */
        switch (ch->effect)
        {

        /* 0xy: Arpeggio */
        case 0x0:
        {
            ch->dirty |= POCKETMOD_PITCH;
        }
        break;

        /* E9x: Retrigger note every x ticks */
        case 0xE9:
        {
            if (!(param && c->tick % param))
            {
                ch->position = 0.0f;
                ch->lfo_step = 0;
            }
        }
        break;

        /* ECx: Cut note after x ticks */
        case 0xEC:
        {
            if (c->tick == param)
            {
                ch->volume = 0;
                ch->dirty |= POCKETMOD_VOLUME;
            }
        }
        break;

        /* EDx: Delay note for x ticks */
        case 0xED:
        {
            if (c->tick == param && ch->sample)
            {
                ch->dirty |= POCKETMOD_VOLUME | POCKETMOD_PITCH;
                ch->period = ch->delayed;
                ch->position = 0.0f;
                ch->lfo_step = 0;
            }
        }
        break;

        default:
            break;
        }

        /* Handle effects that only happen on the first tick of a line */
        if (c->tick == 0)
        {
            switch (ch->effect)
            {
            case 0xE1:
                _pocketmod_pitch_slide(ch, -ch->paramE1);
                break;
            case 0xE2:
                _pocketmod_pitch_slide(ch, +ch->paramE2);
                break;
            case 0xEA:
                _pocketmod_volume_slide(ch, ch->paramEA << 4);
                break;
            case 0xEB:
                _pocketmod_volume_slide(ch, ch->paramEB & 15);
                break;
            default:
                break;
            }

            /* Handle effects that are not applied on the first tick of a line */
        }
        else
        {
            switch (ch->effect)
            {

            /* 1xx: Portamento up */
            case 0x1:
            {
                _pocketmod_pitch_slide(ch, -param);
            }
            break;

            /* 2xx: Portamento down */
            case 0x2:
            {
                _pocketmod_pitch_slide(ch, +param);
            }
            break;

            /* 5xy: Volume slide + tone portamento */
            case 0x5:
            {
                _pocketmod_volume_slide(ch, param);
            } /* Fall through */

            /* 3xx: Tone portamento */
            case 0x3:
            {
                int rate = ch->param3;
                int order = ch->period < ch->target;
                int closer = ch->period + (order ? rate : -rate);
                int new_order = closer < ch->target;
                ch->period = new_order == order ? closer : ch->target;
                ch->dirty |= POCKETMOD_PITCH;
            }
            break;

            /* 6xy: Volume slide + vibrato */
            case 0x6:
            {
                _pocketmod_volume_slide(ch, param);
            } /* Fall through */

            /* 4xy: Vibrato */
            case 0x4:
            {
                ch->lfo_step++;
                ch->dirty |= POCKETMOD_PITCH;
            }
            break;

            /* 7xy: Tremolo */
            case 0x7:
            {
                ch->lfo_step++;
                ch->dirty |= POCKETMOD_VOLUME;
            }
            break;

            /* Axy: Volume slide */
            case 0xA:
            {
                _pocketmod_volume_slide(ch, param);
            }
            break;

            default:
                break;
            }
        }

        /* Update channel volume/pitch if either is out of date */
        if (ch->dirty & POCKETMOD_VOLUME)
        {
            _pocketmod_update_volume(c, ch);
        }
        if (ch->dirty & POCKETMOD_PITCH)
        {
            _pocketmod_update_pitch(c, ch);
        }
    }
}

static void _pocketmod_render_channel(pocketmod_context *c,
                                      _pocketmod_chan *chan,
                                      float *output,
                                      int samples_to_write)
{
    /* Gather some loop data */
    _pocketmod_sample *sample = &c->samples[chan->sample - 1];
    unsigned char *data = POCKETMOD_SAMPLE(c, chan->sample);
    const int loop_start = ((data[4] << 8) | data[5]) << 1;
    const int loop_length = ((data[6] << 8) | data[7]) << 1;
    const int loop_end = loop_length > 2 ? loop_start + loop_length : 0xffffff;
    const float sample_end = 1 + _pocketmod_min(loop_end, sample->length);

    /* Calculate left/right levels */
    const float volume = chan->real_volume / (float)(128 * 64 * 4);
    const float level_l = volume * (1.0f - chan->balance / 255.0f);
    const float level_r = volume * (0.0f + chan->balance / 255.0f);

    /* Write samples */
    int i, num;
    do
    {

        /* Calculate how many samples we can write in one go */
        num = (sample_end - chan->position) / chan->increment;
        num = _pocketmod_min(num, samples_to_write);

        /* Resample and write 'num' samples */
        for (i = 0; i < num; i++)
        {
            int x0 = chan->position;

#ifdef POCKETMOD_NO_INTERPOLATION
            float s = sample->data[x0];
#else
            int x1 = x0 + 1 - loop_length * (x0 + 1 >= loop_end);

            if (x0 < 0 || x0 >= sample->length || x1 < 0 || x1 >= sample->length)
            {
                // Handle out-of-bound index here, e.g., break or continue with adjusted values
                continue;
            }

            float t = chan->position - x0;
            float s = (1.0f - t) * sample->data[x0] + t * sample->data[x1];
#endif
            chan->position += chan->increment;
            *output++ += level_l * s;
            *output++ += level_r * s;
        }

        /* Rewind the sample when reaching the loop point */
        if (chan->position >= loop_end)
        {
            chan->position -= loop_length;

            /* Cut the sample if the end is reached */
        }
        else if (chan->position >= sample->length)
        {
            chan->position = -1.0f;
            break;
        }

        samples_to_write -= num;
    } while (num > 0);
}

static int _pocketmod_ident(pocketmod_context *c, unsigned char *data, int size)
{
    int i, j;

    /* 31-instrument files are at least 1084 bytes long */
    if (size >= 1084)
    {

        /* The format tag is located at offset 1080 */
        unsigned char *tag = data + 1080;

        /* List of recognized format tags (possibly incomplete) */
        static const struct
        {
            char name[5];
            char channels;
        } tags[] = {
            /* TODO: FLT8 intentionally omitted because I haven't been able */
            /* to find a specimen to test its funky pattern pairing format  */
            {"M.K.", 4},
            {"M!K!", 4},
            {"FLT4", 4},
            {"4CHN", 4},
            {"OKTA", 8},
            {"OCTA", 8},
            {"CD81", 8},
            {"FA08", 8},
            {"1CHN", 1},
            {"2CHN", 2},
            {"3CHN", 3},
            {"4CHN", 4},
            {"5CHN", 5},
            {"6CHN", 6},
            {"7CHN", 7},
            {"8CHN", 8},
            {"9CHN", 9},
            {"10CH", 10},
            {"11CH", 11},
            {"12CH", 12},
            {"13CH", 13},
            {"14CH", 14},
            {"15CH", 15},
            {"16CH", 16},
            {"17CH", 17},
            {"18CH", 18},
            {"19CH", 19},
            {"20CH", 20},
            {"21CH", 21},
            {"22CH", 22},
            {"23CH", 23},
            {"24CH", 24},
            {"25CH", 25},
            {"26CH", 26},
            {"27CH", 27},
            {"28CH", 28},
            {"29CH", 29},
            {"30CH", 30},
            {"31CH", 31},
            {"32CH", 32}};

        /* Check the format tag to determine if this is a 31-sample MOD */
        for (i = 0; i < (int)(sizeof(tags) / sizeof(*tags)); i++)
        {
            if (tags[i].name[0] == tag[0] && tags[i].name[1] == tag[1] && tags[i].name[2] == tag[2] && tags[i].name[3] == tag[3])
            {
                c->num_channels = tags[i].channels;
                c->length = data[950];
                c->reset = data[951];
                c->order = &data[952];
                c->patterns = &data[1084];
                c->num_samples = 31;
                return 1;
            }
        }
    }

    /* A 15-instrument MOD has to be at least 600 bytes long */
    if (size < 600)
    {
        return 0;
    }

    /* Check that the song title only contains ASCII bytes (or null) */
    for (i = 0; i < 20; i++)
    {
        if (data[i] != '\0' && (data[i] < ' ' || data[i] > '~'))
        {
            return 0;
        }
    }

    /* Check that sample names only contain ASCII bytes (or null) */
    for (i = 0; i < 15; i++)
    {
        for (j = 0; j < 22; j++)
        {
            char chr = data[20 + i * 30 + j];
            if (chr != '\0' && (chr < ' ' || chr > '~'))
            {
                return 0;
            }
        }
    }

    /* It looks like we have an older 15-instrument MOD */
    c->length = data[470];
    c->reset = data[471];
    c->order = &data[472];
    c->patterns = &data[600];
    c->num_samples = 15;
    c->num_channels = 4;

    return 1;
}

int pocketmod_init(pocketmod_context *c, const void *data, int size, int rate)
{
    int32_t i, remaining, header_bytes, pattern_bytes;
    uint8_t *byte = (unsigned char *)c;
    int8_t *sample_data;

    /* Check that arguments look more or less sane */
    if (!c || !data || rate <= 0 || size <= 0)
    {
        return 0;
    }

    //printf("Size: %d\n", size);

    /* Zero out the whole context and identify the MOD type */
    _pocketmod_zero(c, sizeof(pocketmod_context));
    c->source = (uint8_t *)data;
    c->source_size = size;
    
    if (!_pocketmod_ident(c, c->source, size))
    {
        return 0;
    }

    /* Check that we are compiled with support for enough channels */
    if (c->num_channels > POCKETMOD_MAX_CHANNELS)
    {
        return 0;
    }

    /* Check that we have enough sample slots for this file */
    if (POCKETMOD_MAX_SAMPLES < 31)
    {
        byte = (uint8_t *)data + 20;
        for (i = 0; i < c->num_samples; i++)
        {
            uint32_t length = 2 * ((byte[22] << 8) | byte[23]);
            if (i >= POCKETMOD_MAX_SAMPLES && length > 2)
            {
                return 0; /* Can't fit this sample */
            }
            byte += 30;
        }
    }

    /* Check that the song length is in valid range (1..128) */
    if (c->length == 0 || c->length > 128)
    {
        return 0;
    }

    /* Make sure that the reset pattern doesn't take us out of bounds */
    if (c->reset >= c->length)
    {
        c->reset = 0;
    }

    /* Count how many patterns there are in the file */
    c->num_patterns = 0;
    for (i = 0; i < 128 && c->order[i] < 128; i++)
    {
        c->num_patterns = _pocketmod_max(c->num_patterns, c->order[i]);
    }

    //printf("%d patterns\n", c->num_patterns);

    pattern_bytes = 256 * c->num_channels * ++c->num_patterns;
    header_bytes = (int32_t)((int8_t *)c->patterns - (int8_t *)data);

    /* Check that each pattern in the order is within file bounds */
    for (i = 0; i < c->length; i++)
    {
        if (header_bytes + 256 * c->num_channels * c->order[i] > size)
        {
            return 0; /* Reading this pattern would be a buffer over-read! */
        }
    }

    /* Check that the pattern data doesn't extend past the end of the file */
    if (header_bytes + pattern_bytes > size)
    {
        return 0;
    }

    /* Extract sample names and sample lengths */
    byte = (uint8_t *)data + 20;
    for (i = 0; i < c->num_samples; i++)
    {
        uint32_t length = 2 * ((byte[22] << 8) | byte[23]);
        memcpy(c->samples[i].name, byte, 22); // Copy 22 bytes of the sample name
        c->samples[i].name[22] = '\0';        // Null-terminate the string

        if (i >= POCKETMOD_MAX_SAMPLES && length > 2)
        {
            return 0; // Can't fit this sample
        }
        byte += 30; // Move to the next sample entry
    }

    /* Load sample payload data, truncating ones that extend outside the file */
    remaining = size - header_bytes - pattern_bytes;
    sample_data = (int8_t *)data + header_bytes + pattern_bytes;
    for (i = 0; i < c->num_samples; i++)
    {
        uint8_t *data = POCKETMOD_SAMPLE(c, i + 1);
        uint32_t length = ((data[0] << 8) | data[1]) << 1;
        _pocketmod_sample *sample = &c->samples[i];
        sample->data = sample_data;
        sample->length = _pocketmod_min(length > 2 ? length : 0, remaining);
        sample_data += sample->length;
        remaining -= sample->length;
    }

    /* Set up ProTracker default panning for all channels */
    for (i = 0; i < c->num_channels; i++)
    {
        c->channels[i].balance = 0x80 + ((((i + 1) >> 1) & 1) ? 0x20 : -0x20);
    }

    /* Prepare to render from the start */
    c->ticks_per_line = 6;
    c->samples_per_second = rate;
    c->samples_per_tick = rate / 50.0f;
    c->lfo_rng = 0xbadc0de;
    c->line = -1;
    c->tick = c->ticks_per_line - 1;

    _pocketmod_next_tick(c);
    return 1;
}

int pocketmod_render(pocketmod_context *c, void *buffer, int buffer_size)
{
    int i, samples_rendered = 0;
    int samples_remaining = buffer_size / POCKETMOD_SAMPLE_SIZE;
    if (c && buffer)
    {
        float(*output)[2] = (float(*)[2])buffer;
        while (samples_remaining > 0)
        {
            /* Calculate the number of samples left in this tick */
            int num = (int)(c->samples_per_tick - c->sample);
            num = _pocketmod_min(num + !num, samples_remaining);

            /* Check if enough buffer space is left before zeroing */
            if (num * POCKETMOD_SAMPLE_SIZE > buffer_size)
            {
                // If not enough space, adjust num to fit the remaining buffer
                num = buffer_size / POCKETMOD_SAMPLE_SIZE;
            }

            _pocketmod_zero(output, num * POCKETMOD_SAMPLE_SIZE);

            for (i = 0; i < c->num_channels; i++)
            {
                _pocketmod_chan *chan = &c->channels[i];
                if (chan->sample != 0 && chan->position >= 0.0f)
                {
                    _pocketmod_render_channel(c, chan, *output, num);
                }
            }

            /* Reduce buffer_size by the amount of data processed */
            buffer_size -= num * POCKETMOD_SAMPLE_SIZE;
            samples_remaining -= num;
            samples_rendered += num;
            output += num;

            /* Advance song position by 'num' samples */
            if ((c->sample += num) >= c->samples_per_tick)
            {
                c->sample -= c->samples_per_tick;
                _pocketmod_next_tick(c);

                /* Stop if a new pattern was reached */
                if (c->line == 0 && c->tick == 0)
                {

                    /* Increment loop counter as needed */
                    if (c->visited[c->pattern >> 3] & (1 << (c->pattern & 7)))
                    {
                        _pocketmod_zero(c->visited, sizeof(c->visited));
                        c->loop_count++;
                    }
                    break;
                }
            }
        }
    }
    return samples_rendered * POCKETMOD_SAMPLE_SIZE;
}

static void _pocketmod_render_channel_u8(pocketmod_context *c,
                                         _pocketmod_chan *chan,
                                         uint8_t *output,
                                         int samples_to_write)
{
    /* Gather some loop data */
    _pocketmod_sample *sample = &c->samples[chan->sample - 1];
    unsigned char *data = POCKETMOD_SAMPLE(c, chan->sample);
    const int loop_start = ((data[4] << 8) | data[5]) << 1;
    const int loop_length = ((data[6] << 8) | data[7]) << 1;
    const int loop_end = loop_length > 2 ? loop_start + loop_length : sample->length;
    const float sample_end = 1 + _pocketmod_min(loop_end, sample->length);

    /* Calculate left/right levels */
    const float volume = chan->real_volume / (float)(128 * 64 * 2);
    const float level_l = volume * (1.0f - chan->balance / 255.0f);
    const float level_r = volume * (chan->balance / 255.0f);

    /* Write samples */
    int i, num;
    do
    {
        /* Calculate how many samples we can write in one go */
        num = (sample_end - chan->position) / chan->increment;
        num = _pocketmod_min(num, samples_to_write);

        /* Resample and write 'num' samples */
        for (i = 0; i < num; i++)
        {
            int x0 = (int)chan->position;

#ifdef POCKETMOD_NO_INTERPOLATION
            float s = sample->data[x0];
#else
            int x1 = x0 + 1;
            if (x0 + 1 >= loop_end)
                x1 = loop_start;

            if (x0 < 0 || x0 >= sample->length || x1 < 0 || x1 >= sample->length)
            {
                // Handle out-of-bound index here, e.g., break or continue with adjusted values
                continue;
            }

            float t = chan->position - x0;
            float s = (1.0f - t) * sample->data[x0] + t * sample->data[x1];
#endif
            chan->position += chan->increment;

            // Convert float sample to 8-bit unsigned integer
            uint8_t sample_left = (int)((level_l * s) * 127.5f);
            uint8_t sample_right = (int)((level_r * s) * 127.5f);

            // Clamp values to the uint8_t range
            sample_left = _pocketmod_clamp_int(sample_left, 0, 255);
            sample_right = _pocketmod_clamp_int(sample_right, 0, 255);

            *output++ += sample_left;
            *output++ += sample_right;
        }

        /* Rewind the sample when reaching the loop point */
        if (chan->position >= loop_end)
        {
            chan->position -= loop_length;
        }
        else if (chan->position >= sample->length)
        {
            // Cut the sample if the end is reached
            chan->position = -1.0f;
            break;
        }

        samples_to_write -= num;
    } while (num > 0);
}

int pocketmod_render_u8(pocketmod_context *c, void *buffer, int buffer_size)
{
    int i, samples_rendered = 0;
    int samples_remaining = buffer_size / POCKETMOD_SAMPLE_SIZE_U8;
    if (c && buffer)
    {
        uint8_t(*output)[2] = (uint8_t(*)[2])buffer;
        while (samples_remaining > 0)
        {
            /* Calculate the number of samples left in this tick */
            int num = (int)(c->samples_per_tick - c->sample);
            num = _pocketmod_min(num + !num, samples_remaining);

            /* Check if enough buffer space is left before zeroing */
            if (num * POCKETMOD_SAMPLE_SIZE_U8 > buffer_size)
            {
                // If not enough space, adjust num to fit the remaining buffer
                num = buffer_size / POCKETMOD_SAMPLE_SIZE_U8;
            }

            // memset(output, 128, num * POCKETMOD_SAMPLE_SIZE); // Fill buffer with the mid-point value of 128 (silence)

            for (i = 0; i < c->num_channels; i++)
            {
                _pocketmod_chan *chan = &c->channels[i];

                if (chan->sample != 0 && chan->position >= 0.0f)
                {
                    _pocketmod_render_channel_u8(c, chan, output, num);
                }
            }

            /* Reduce buffer_size by the amount of data processed */
            buffer_size -= num * POCKETMOD_SAMPLE_SIZE_U8;
            samples_remaining -= num;
            samples_rendered += num;
            output += num;

            /* Advance song position by 'num' samples */
            if ((c->sample += num) >= c->samples_per_tick)
            {
                c->sample -= c->samples_per_tick;
                _pocketmod_next_tick(c);

                /* Stop if a new pattern was reached */
                if (c->line == 0 && c->tick == 0)
                {
                    /* Increment loop counter as needed */
                    if (c->visited[c->pattern >> 3] & (1 << (c->pattern & 7)))
                    {
                        memset(c->visited, 0, sizeof(c->visited));
                        c->loop_count++;
                    }
                    break;
                }
            }
        }
    }
    return samples_rendered * POCKETMOD_SAMPLE_SIZE_U8; // return the number of bytes processed
}

int pocketmod_loop_count(pocketmod_context *c)
{
    return c->loop_count;
}