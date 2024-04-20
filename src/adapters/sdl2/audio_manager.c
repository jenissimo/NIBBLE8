#include "audio_manager.h"

SDL_AudioDeviceID device;

static void audio_callback(void *userdata, Uint8 *buffer, int bytes)
{
    memset(buffer, 0, bytes);

    if (!memory.soundState.music_active && !memory.soundState.triggered_note.active)
    {
        return;
    }

    int numSamples = bytes / sizeof(float);
    float mixBuffer[bytes]; // Adjust size based on your typical buffer size needs

    // Clear mixBuffer - essential to remove remnants from previous callbacks
    memset(mixBuffer, 0, numSamples);

    // Render music if active
    if (memory.soundState.music_active)
    {
        DEBUG_LOG("Rendering music");
        /*
        int i = 0;
        while (i < bytes)
        {
            i += pocketmod_render(&memory.soundState.context, mixBuffer + i, bytes - i);
        }
        */
        pocketmod_render(&memory.soundState.context, mixBuffer, bytes);
    }

    // Mix in any active notes into mixBuffer
    if (memory.soundState.triggered_note.active)
    {
        nibble_audio_play_note(&memory.soundState.context, &memory.soundState.triggered_note, buffer, bytes);
    }

    /*
    float *output = (float *)buffer;
    for (int i = 0; i < numSamples; i++)
    {
        output[i] += mixBuffer[i];
    }

    // Apply the limiter to the output buffer
    apply_limiter(output, numSamples, memory.soundState.context.samples_per_second);
    */
}

void apply_limiter(float *buffer, int num_samples, float sampleRate)
{
    float maxSample = 0.0f;

    // Find the maximum absolute sample in the buffer
    for (int i = 0; i < num_samples; i++)
    {
        if (fabs(buffer[i]) > maxSample)
        {
            maxSample = fabs(buffer[i]);
        }
    }

    // If the maximum sample exceeds the threshold, scale the buffer
    if (maxSample > LIMITER_THRESHOLD)
    {
        float scale = LIMITER_THRESHOLD / maxSample;
        for (int i = 0; i < num_samples; i++)
        {
            buffer[i] *= scale;
        }
    }
}

void audio_init()
{
    const Uint32 allowed_changes = SDL_AUDIO_ALLOW_FREQUENCY_CHANGE;
    SDL_AudioSpec format;

    format.freq = NIBBLE_SAMPLERATE;
    format.format = AUDIO_F32;
    format.channels = NIBBLE_SAMPLE_CHANNELS;
    format.samples = NIBBLE_SAMPLES;
    format.callback = audio_callback;

    device = SDL_OpenAudioDevice(NULL, 0, &format, &format, allowed_changes);
    if (!device)
    {
        DEBUG_LOG("Failed to SDL_OpenAudioDevice(): %s", SDL_GetError());
        exit(1); // Consider a more graceful exit
    }
    DEBUG_LOG("Audio format: freq=%d, format=%d, channels=%d, samples=%d", format.freq, format.format, format.channels, format.samples);
    nibble_audio_init(format.freq);

    // memset(audioBuffer, 0, sizeof(audioBuffer));
    SDL_PauseAudioDevice(device, 0);
}

void audio_quit()
{
    static const int silence = 0;
    memory.soundState.music_active = false;
    memory.soundState.triggered_note.active = false;
    memset(memory.soundState.sfx_patterns, -1, NIBBLE_SFX_CHANNELS);

    SDL_PauseAudioDevice(device, 1);                   // Stop playback
    SDL_ClearQueuedAudio(device);                      // Clear all queued audio data
    SDL_QueueAudio(device, &silence, sizeof(silence)); // Queue a single sample of silence
    SDL_PauseAudioDevice(device, 0);                   // Resume playback to flush the buffer
    SDL_Delay(100);                                    // Short delay to let the silence play out

    SDL_CloseAudio();
}