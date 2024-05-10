#include "audio_manager.h"

AUDIOSTREAM *audio_stream;

void audio_init()
{
    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0)
    {
        DEBUG_LOG("Failed to initialize sound system.");
        exit(1);
    }

    audio_stream = play_audio_stream(NIBBLE_SAMPLES, 8, 1, NIBBLE_SAMPLERATE, 255, 128);
    if (!audio_stream)
    {
        DEBUG_LOG("Failed to initialize audio stream.");
        exit(1);
    }
    voice_set_priority(audio_stream->voice, 255);
    DEBUG_LOG("Audio initialized: %d Hz, 16-bit stereo", NIBBLE_SAMPLERATE);
    nibble_audio_init(NIBBLE_SAMPLERATE, NULL, 0);
}

void audio_update()
{
    if (!audio_stream)
        return;

    void *buffer = get_audio_stream_buffer(audio_stream);
    if (!buffer)
    {
        return;
    }

    int bytes = NIBBLE_SAMPLES * 2;
    nibble_audio_update(buffer, bytes);

    free_audio_stream_buffer(audio_stream);
}

void audio_quit()
{
    static const int silence = 0;
    memory.soundState.music_active = false;
    memory.soundState.triggered_note.active = false;
    //memset(memory.soundState.sfx_patterns, -1, NIBBLE_SFX_CHANNELS);

    if (audio_stream)
    {
        stop_audio_stream(audio_stream);
        audio_stream = NULL;
    }

    remove_sound();
}