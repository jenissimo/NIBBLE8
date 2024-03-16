#include "audio_manager.h"

static int16_t audioBuffer[NUM_SAMPLES];

void audio_callback(void *userdata, uint8_t *stream, int len)
{
    int16_t *audioStream = (int16_t *)stream;
    int audioLength = len / sizeof(int16_t);

    nibble_audio_update(audioBuffer, audioLength);
    memcpy(audioStream, audioBuffer, len);
}

void audio_init()
{
    SDL_AudioSpec desired_spec, obtained_spec;

    desired_spec.freq = SAMPLE_RATE;
    desired_spec.format = AUDIO_S16SYS;
    desired_spec.channels = NUM_CHANNELS;
    desired_spec.samples = NUM_SAMPLES;
    desired_spec.callback = audio_callback;
    desired_spec.userdata = NULL;

    if (SDL_OpenAudio(&desired_spec, &obtained_spec) < 0)
    {
        DEBUG_LOG("Failed to open audio device: %s", SDL_GetError());
        fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
        exit(1); // Consider a more graceful exit
    }

    memset(audioBuffer, 0, sizeof(audioBuffer));
    SDL_Delay(50);
    SDL_PauseAudio(0);
}

void audio_quit()
{
    SDL_PauseAudio(1);
    SDL_CloseAudio();
}