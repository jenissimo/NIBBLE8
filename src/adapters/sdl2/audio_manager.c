#include "audio_manager.h"

SDL_AudioDeviceID device;

static void audio_callback(void *userdata, Uint8 *buffer, int bytes)
{
    nibble_audio_update(buffer, bytes);
}

void audio_init()
{
    const Uint32 allowed_changes = 0;
    SDL_AudioSpec format;

    format.freq = NIBBLE_SAMPLERATE;
    format.format = AUDIO_U8;
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
    nibble_audio_init(NIBBLE_SAMPLERATE, NULL, 0);

    // memset(audioBuffer, 0, sizeof(audioBuffer));
    SDL_PauseAudioDevice(device, 0);
}

void audio_quit()
{
    uint8_t silence = 128;

    SDL_PauseAudioDevice(device, 1);                   // Stop playback
    SDL_ClearQueuedAudio(device);                      // Clear all queued audio data
    SDL_QueueAudio(device, &silence, sizeof(silence)); // Queue a single sample of silence
    SDL_PauseAudioDevice(device, 0);                   // Resume playback to flush the buffer
    SDL_Delay(200);                                    // Short delay to let the silence play out

    SDL_CloseAudio();
}