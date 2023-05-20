#include "synth.h"
#include <time.h>

double globalTime = 0.0;
float timeStep = 1.0 / SAMPLE_RATE;
SynthState synthState;

// Converts frequency (Hz) to angular velocity
float w(float frequency)
{
    return frequency * 2.0 * PI;
}

float osc(float frequency, float time, Oscillator type)
{
    float phase = fmod(frequency * time, 1.0f);
    float ret = 0.0f;
    const float a = 0.9f;

    switch (type)
    {
    case TRIANGLE:
        return 9.0f * fabsf(2.0f * phase - 1.0f) - 1.0f;
    case TILTED_SAW:
        ret = phase < a ? 2.0f * phase / a - 1.0f
                        : 2.0f * (1.0f - phase) / (1.0f - a) - 1.0f;
        return ret * 3;
    case SAW:
        return (phase < 0.5f ? phase : phase - 1.f) * 3.0f;
    case SQUARE:
        return phase < 0.5f ? 1.0f : -1.0f;
    case PULSE:
        return phase < 1.f / 3 ? 1.0f : -1.0f;
    case NOISE:
        return 2.0 * ((float)rand() / (float)RAND_MAX) - 1.0;
    case SINE:
        return sinf(w(frequency) * time);
        
    }

    return 0.0f;
}

void synth_init(SynthState *synth)
{
    globalTime = 0.0;

    synth->osc = TRIANGLE;
    synth->attackTime = 0.5;
    synth->decayTime = 0.5;
    synth->sustainAmplitude = 0.5;
    synth->releaseTime = 0.5;
    synth->startAmplitude = 1.0;

    synth->note_on = false;
    synth->triggerOffTime = 0.0;
    synth->triggerOnTime = 0.0;

    synth->num_filters = NUM_FILTERS;
    synth->filters = (Filter **)realloc(synth->filters, sizeof(Filter *) * synth->num_filters);

    synth->filters[FILTER_CUTOFF] = filter_create(FILTER_CUTOFF, 0);
    synth->filters[FILTER_LOW_PASS] = filter_create(FILTER_LOW_PASS, 0);
    synth->filters[FILTER_DETUNE] = filter_create(FILTER_DETUNE, 0);
    synth->filters[FILTER_BUZZ] = filter_create(FILTER_BUZZ, 0);
    synth->filters[FILTER_NOISE] = filter_create(FILTER_NOISE, 0);
    synth->filters[FILTER_REVERB] = filter_create(FILTER_REVERB, 0);
    synth->filters[FILTER_DAMPEN] = filter_create(FILTER_DAMPEN, 0);
}

void synth_update_config(SynthState *synth, Oscillator osc, double attackTime, double decayTime, double sustainAmplitude, double releaseTime, double startAmplitude)
{
    synth->osc = osc;
    synth->attackTime = attackTime;
    synth->decayTime = decayTime;
    synth->sustainAmplitude = sustainAmplitude;
    synth->releaseTime = releaseTime;
    synth->startAmplitude = startAmplitude;
}

void synth_update_filter(SynthState *synth, FilterType filter, double value)
{
    filter_update(synth->filters[filter], value);
}

void synth_note_on(SynthState *synth, int note, int octave)
{
    if (note < 0 || note > NUM_NOTES)
        return;

    float a = 440.0; // frequency of A4
    float factor = pow(2.0, ((note + 12 * octave) - 49.0) / 12.0);

    // printf("Note on: %d\n", note);
    synth->frequency = a * factor;
    synth->note_on = true;
    synth->triggerOnTime = globalTime;
}

void synth_note_off(SynthState *synth)
{
    synth->note_on = false;
    synth->triggerOffTime = globalTime;
}

float synth_get_amplitude(SynthState *synth, float time)
{
    float amplitude = 0.0;
    float lifeTime = time - synth->triggerOnTime;

    if (synth->note_on)
    {
        if (lifeTime <= synth->attackTime)
        {
            // In attack Phase - approach max amplitude
            amplitude = (lifeTime / synth->attackTime) * synth->startAmplitude;
        }

        if (lifeTime > synth->attackTime && lifeTime <= (synth->attackTime + synth->decayTime))
        {
            // In decay phase - reduce to sustained amplitude
            amplitude = ((lifeTime - synth->attackTime) / synth->decayTime) * (synth->sustainAmplitude - synth->startAmplitude) + synth->startAmplitude;
        }

        if (lifeTime > (synth->attackTime + synth->decayTime))
        {
            // In sustain phase - dont change until note released
            amplitude = synth->sustainAmplitude;
        }
    }
    else
    {
        // Note has been released, so in release phase
        amplitude = ((time - synth->triggerOffTime) / synth->releaseTime) * (0.0 - synth->sustainAmplitude) + synth->sustainAmplitude;
    }

    // Amplitude should not be negative
    if (amplitude <= 0.0001)
        amplitude = 0.0;

    return amplitude;
}

float synth_clip(float sample, float max)
{
    if (sample >= 0.0)
        return fmin(sample, max);
    else
        return fmax(sample, -max);
}

void synth_update(SynthState *synth, int16_t *stream, int num_samples)
{
    for (int i = 0; i < num_samples; i++)
    {
        float sample = 0.0;

        sample = osc(synth->frequency, globalTime, synth->osc) * synth_get_amplitude(synth, globalTime);

        // Apply filters
        for (int i = 0; i < synth->num_filters; i++)
        {
            sample = filter_apply(synth->filters[i], sample, globalTime);
        }

        sample *= 0.75;

        stream[i] = synth_clip(sample, 1.0) * 32767;
        globalTime += timeStep;
    }
}

//float apply_effect(Instrument *instrument, float time, float frequency, float note_duration)
//{
    /*
    switch (fx)
    {
        case FX_NO_EFFECT:
            break;
        case FX_SLIDE:
        {
            // From the documentation: “Slide to the next note and volume”,
            // but it’s actually _from_ the _prev_ note and volume.
            freq = lerp(key_to_freq(prev_note.getKey()), freq, tmod);
            if (prev_note.getVolume() > 0)
                volume = lerp(prev_note.getVolume() / 7.0f, volume, tmod);
            break;
        }
        case FX_VIBRATO:
        {
            // 7.5f and 0.25f were found empirically by matching
            // frequency graphs of PICO-8 instruments.
            float t = fabs(fmod(7.5f * tmod / offset_per_second, 1.0f) - 0.5f) - 0.25f;
            // Vibrato half a semi-tone, so multiply by pow(2,1/12)
            freq = lerp(freq, freq * 1.059463094359f, t);
            break;
        }
        case FX_DROP:
            freq *= 1.f - fmod(offset, 1.f);
            break;
        case FX_FADE_IN:
            volume *= std::min(1.f, tmod);
            break;
        case FX_FADE_OUT:
            volume *= max(0.0f, 1.f - tmod);
            break;
        case FX_ARP_FAST:
        case FX_ARP_SLOW:
        {
            // From the documentation:
            // “6 arpeggio fast  //  Iterate over groups of 4 notes at speed of 4
            //  7 arpeggio slow  //  Iterate over groups of 4 notes at speed of 8”
            // “If the SFX speed is <= 8, arpeggio speeds are halved to 2, 4”
            int const m = (speed <= 8 ? 32 : 16) / (fx == FX_ARP_FAST ? 4 : 8);
            int const n = (int)(m * 7.5f * offset / offset_per_second);
            int const arp_note = (note_idx & ~3) | (n & 3);
            freq = key_to_freq(sfx.notes[arp_note].getKey());
            break;
        }
    }
    */

    /*
    float new_frequency = frequency;
    float effect_progress = time / note_duration;

    switch (instrument->effect)
    {
    case EFFECT_NONE:
        break;
    case EFFECT_SLIDE:
        new_frequency += instrument->effect_param1 * effect_progress;
        break;
    case EFFECT_VIBRATO:
        new_frequency *= 1.0f + instrument->effect_param1 * sinf(2.0f * M_PI * instrument->effect_param2 * time);
        break;
    case EFFECT_DROP:
        new_frequency *= (1.0f - effect_progress);
        break;
    case EFFECT_FADE_IN:
        break;
    case EFFECT_FADE_OUT:
        break;
    case EFFECT_ARPEGGIO_FAST:
        new_frequency *= powf(2.0f, floorf(effect_progress * 12.0f * instrument->effect_param1) / 12.0f);
        break;
    case EFFECT_ARPEGGIO_SLOW:
        new_frequency *= powf(2.0f, floorf(effect_progress * 4.0f * instrument->effect_param1) / 12.0f);
        break;
    default:
        break;
    }

    return new_frequency;
    */
//}

//float apply_amplitude_effect(Instrument *instrument, float amplitude, float time, float note_duration)
//{
    /*
    float effect_progress = time / note_duration;

    switch (instrument->effect)
    {
    case EFFECT_NONE:
    case EFFECT_SLIDE:
    case EFFECT_VIBRATO:
    case EFFECT_DROP:
        break;
    case EFFECT_FADE_IN:
        amplitude *= effect_progress;
        break;
    case EFFECT_FADE_OUT:
        amplitude *= (1.0f - effect_progress);
        break;
    case EFFECT_ARPEGGIO_FAST:
    case EFFECT_ARPEGGIO_SLOW:
        break;
    default:
        break;
    }

    return amplitude;
    */
//}