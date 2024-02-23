#include "audio.h"
#include "ram.h"
#include "synth.h"

float phi = 0.0f;
float freq;
bool note_on;
Instrument currentInstrument;

void nibble_audio_update(int16_t *stream, int num_samples)
{
    for (int i = 0; i < num_samples; ++i)
    {
        float sample = 0.0f;

        if (note_on)
        {
            sample = nibble_waveform(currentInstrument, phi);
        }

        stream[i] = (int16_t)(nibble_audio_clip(sample, 1.0) * 32767.99f);

        phi += freq / SAMPLE_RATE; // Advance phase for the next sample

        if (phi >= 1.0)
        {
            phi -= 1.0; // Wrap around phase
        }
    }
}

float nibble_audio_clip(float sample, float threshold)
{
    // Standard soft clipping with a hyperbolic tangent function
    if (fabsf(sample) > threshold)
    {
        sample = threshold * tanhf(sample / threshold);
    }
    return sample;
}

void nibble_api_set_note(uint8_t sfx_index, uint8_t note_index, uint8_t pitch, uint8_t instrument, uint8_t volume, uint8_t effect)
{
    memory.sfxData[sfx_index].notes[note_index].pitch = pitch;
    memory.sfxData[sfx_index].notes[note_index].instrument = instrument;
    memory.sfxData[sfx_index].notes[note_index].volume = volume;
    memory.sfxData[sfx_index].notes[note_index].effect = effect;

    for (int i = 0; i < NIBBLE_SFX_MAX_NOTES / 2; i++)
    {
        /*
        printf("pitch %d: %d\n", i, memory.sfxData[sfx_index].notes[i].pitch);
        printf("instrument %d: %d\n", i, memory.sfxData[sfx_index].notes[i].instrument);
        printf("volume %d: %d\n", i, memory.sfxData[sfx_index].notes[i].volume);
        printf("effect %d: %d\n", i, memory.sfxData[sfx_index].notes[i].effect);
        */
    }
}

void nibble_api_note_on(uint8_t note, uint8_t octave, uint8_t instrument)
{
    note_on = true;
    freq = note_to_frequency(note, octave);
    currentInstrument = (Instrument)instrument;

    printf("note_on(%d, %d, %d)\n", note, octave, instrument);
    printf("freq: %f\n", freq);
}

float note_to_frequency(int noteIndex, int octaveIndex)
{
    int A4Index = 9;            // A4 is the 9th note in the octave starting from C
    int A4OctaveIndex = 4;      // A4 is in the 4th octave
    double A4Frequency = 440.0; // Frequency of A4 is 440 Hz

    // Calculate the total number of half steps from A4
    int totalHalfSteps = (octaveIndex - A4OctaveIndex) * 12 + (noteIndex - A4Index);

    // Calculate frequency
    double frequency = A4Frequency * pow(2.0, totalHalfSteps / 12.0);
    return frequency;
}

void nibble_api_note_off()
{
    note_on = false;
}

void nibble_api_update_synth(uint8_t osc, double attackTime, double decayTime, double sustainAmplitude, double releaseTime, double startAmplitude)
{
}

void nibble_api_update_filter(float cutoffPercent, float resonancePercent, int mode)
{
}