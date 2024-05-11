#include "audio_manager.h"

void audio_callback(void *userdata, Uint8 *buffer, int bytes)
{
    nibble_audio_update(buffer, bytes);
}

void audio_init()
{
    SDL_AudioSpec desired_spec, obtained_spec;

    desired_spec.freq = NIBBLE_SAMPLERATE;
    desired_spec.format = AUDIO_U8;
    desired_spec.channels = NIBBLE_SAMPLE_CHANNELS;
    desired_spec.samples = NIBBLE_SAMPLES;
    desired_spec.callback = audio_callback;
    desired_spec.userdata = NULL;

    if (SDL_OpenAudio(&desired_spec, &obtained_spec) < 0)
    {
        DEBUG_LOG("Failed to open audio device: %s", SDL_GetError());
        fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
        exit(1); // Consider a more graceful exit
    }

    nibble_audio_init(NIBBLE_SAMPLERATE, NULL, 0);
    SDL_PauseAudio(0);
}

void audio_quit()
{
    SDL_PauseAudio(1);
    SDL_Delay(200);                                    // Short delay to let the silence play out

    SDL_CloseAudio();
}