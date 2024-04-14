#include "audio_manager.h"

static void audio_callback(void *userdata, Uint8 *buffer, int bytes)
{
    memset(buffer, 0, bytes);
    if (memory.soundState.soundEnabled)
    {
        int i = 0;
        while (i < bytes)
        {
            i += pocketmod_render(modContext, buffer + i, bytes - i);
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
    format.samples = 4096;
    format.callback = audio_callback;
    //format.userdata = modContext;

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
    memory.soundState.soundEnabled = false;
    SDL_PauseAudioDevice(device, 1); // Stop playback
    SDL_ClearQueuedAudio(device); // Clear all queued audio data
    SDL_QueueAudio(device, &silence, sizeof(silence)); // Queue a single sample of silence
    SDL_PauseAudioDevice(device, 0); // Resume playback to flush the buffer
    SDL_Delay(100); // Short delay to let the silence play out

    SDL_CloseAudio();
}