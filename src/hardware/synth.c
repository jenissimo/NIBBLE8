#include "synth.h"

float lastadvance;
float sample;
float lsample;

float nibble_waveform(Instrument instrument, float advance)
{
    // const from picolove:
    // local function note_to_hz(note)
    //   return 440 * 2 ^ ((note - 33) / 12)
    // end
    // local tscale = note_to_hz(63) / __sample_rate
    const float tscale = 0.11288053831187f;

    float t = fmod(advance, 1.f);
    float ret = 0.f;

    // Multipliers were measured from PICO-8 WAV exports. Waveforms are
    // inferred from those exports by guessing what the original formulas
    // could be.
    switch (instrument)
    {
    case INST_TRIANGLE:
        return 0.5f * (fabs(4.f * t - 2.0f) - 1.0f);
    case INST_TILTED_SAW:
    {
        static float const a = 0.9f;
        ret = t < a ? 2.f * t / a - 1.f
                    : 2.f * (1.f - t) / (1.f - a) - 1.f;
        return ret * 0.5f;
    }
    case INST_SAW:
        return 0.653f * (t < 0.5f ? t : t - 1.f);
    case INST_SQUARE:
        return t < 0.5f ? 0.25f : -0.25f;
    case INST_PULSE:
        return t < 1.f / 3 ? 0.25f : -0.25f;
    case INST_SINE:
        return sinf(2.0f * M_PI * t);
    case INST_NOISE:
    {
        float scale = (advance - lastadvance) / tscale;
        lsample = sample;
        sample = (lsample + scale * (((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f)) / (1.0f + scale);
        lastadvance = advance;

        // Manual implementation of fmin and fmax
        float temp = (lsample + sample) * 4.0f / 3.0f * (1.75f - scale);
        float endval;

        if (temp < -1.0f)
        {
            endval = -1.0f;
        }
        else if (temp > 1.0f)
        {
            endval = 1.0f;
        }
        else
        {
            endval = temp;
        }

        endval *= 0.2f;
        return endval;
    }
    case INST_DARK_SYNTH:
    {
        // Mix different waveforms
        float sine = nibble_waveform(INST_SINE, advance);
        float square = nibble_waveform(INST_SQUARE, advance * 1.005);
        float sawtooth = nibble_waveform(INST_SAW, advance * 0.995);

        // Combine the waveforms and normalize
        float combined = (sine + square + sawtooth) / 3.0;

        return combined;
    }
    }

    return 0.0f;
}
