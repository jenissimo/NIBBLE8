#include "audio.h"

/*
    // print first pattern in hex
    printf("First pattern in hex: \n");
    static const char *NOTE_STRING[12] = {"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"};
    for (int line = 0; line < 64; line++)
    {
        int pos = (context.order[0] * 64 + line) * context.num_channels * 4;
        unsigned char (*data)[4] = (unsigned char(*)[4]) (context.patterns + pos);

        printf("%02x ", line);

        for (int i = 0; i < context.num_channels; i++)
        {
            int sample = (data[i][0] & 0xf0) | (data[i][2] >> 4);
            int period = ((data[i][0] & 0x0f) << 8) | data[i][1];
            int effect = ((data[i][2] & 0x0f) << 8) | data[i][3];
            int note = _pocketmod_period_to_note(period);
            int noteIndex = note % 12;
            int octave = note / 12 + 3;

            //printf("%d", note);

            if (note <= 0)
            {
                printf("---");
            }
            else
            {
                printf("%s%d", NOTE_STRING[noteIndex], octave);
            }

            if (sample > 0)
            {
                printf("%02X", sample);
            }
            else
            {
                printf("--");
            }

            if (effect > 0)
            {
                printf("%03X ", effect);
            }
            else
            {
                printf("--- ");
            }
        }

        printf("\n");
    }
*/

void nibble_audio_init(int freq)
{
    /* Initialize the renderer */
    if (!pocketmod_init(&memory.soundState.context, EMPTY_MOD_FILE, EMPTY_MOD_FILE_SIZE, freq))
    {
        DEBUG_LOG("Failed to initialize PocketMod\n");
        exit(1);
    }
    else
    {
        DEBUG_LOG("PocketMod initialized\n");
    }
    nibble_audio_reset();
}

void nibble_audio_reset()
{
    memory.soundState.music_active = false;
    memory.soundState.music_start_pattern = 0;
    memory.soundState.music_end_pattern = 0;
    memset(memory.soundState.sfx_patterns, -1, NIBBLE_SFX_CHANNELS);

    memory.soundState.triggered_note.active = false;
    memory.soundState.triggered_note.note_index = 0;
    memory.soundState.triggered_note.sample_index = 0;
    memory.soundState.triggered_note.volume = 0;
    memory.soundState.triggered_note.position = 0;
    memory.soundState.triggered_note.increment = 0;
    memory.soundState.triggered_note.need_reset = true;
}

void nibble_audio_play_note(pocketmod_context *context, TriggeredNote *note, float *output, int buffer_size)
{
    if (!note->active || !context || !output)
    {
        // DEBUG_LOG("Note is not active or context/output is NULL\n");
        return;
    }

    if (note->need_reset)
    {
        // DEBUG_LOG("Need to reset note");
        // Calculate the period and the increment for the given note.
        int period = nibble_audio_note_to_period(note->note_index);
        note->increment = 3546894.6f / (period * context->samples_per_second);
        note->position = 0;
        note->need_reset = false;
    }
    // DEBUG_LOG("Note %d, increment %f, position %f", note->note_index, note->increment, note->position);

    int samples_to_write = buffer_size / sizeof(float[2]); // Assuming stereo output

    _pocketmod_sample *sample = &context->samples[note->sample_index - 1];
    unsigned char *data = POCKETMOD_SAMPLE(context, note->sample_index);
    const int loop_start = ((data[4] << 8) | data[5]) << 1;
    const int loop_length = ((data[6] << 8) | data[7]) << 1;
    const int loop_end = loop_length > 2 ? loop_start + loop_length : 0xffffff;
    const float sample_end = _pocketmod_min(loop_end, sample->length);
    const float volume = note->volume / (float)(128 * 64 * 4);

    // DEBUG_LOG("Sample %d, end %f", note->sample_index, sample_end);

    if (sample_end < 2)
    {
        // DEBUG_LOG("Empty sample!");
        nibble_audio_stop_note();
        return;
    }

    /* Write samples */
    int i, num;
    do
    {
        /* Calculate how many samples we can write in one go */
        num = (sample_end - note->position) / note->increment;
        num = _pocketmod_min(num, samples_to_write);

        if (num <= 0)
        {
            // DEBUG_LOG("We reach the end!");
            // nibble_audio_stop_note();
            break;
        }

        /* Resample and write 'num' samples */
        for (i = 0; i < num; i++)
        {
            int x0 = note->position;

#ifdef POCKETMOD_NO_INTERPOLATION
            float s = sample->data[x0];
#else
            int x1 = x0 + 1 - loop_length * (x0 + 1 >= loop_end);

            if (x0 < 0 || x0 >= sample->length || x1 < 0 || x1 >= sample->length)
            {
                //  Handle out-of-bound index here, e.g., break or continue with adjusted values
                continue;
            }

            float t = note->position - x0;
            float s = (1.0f - t) * sample->data[x0] + t * sample->data[x1];
#endif
            note->position += note->increment;
            *output++ += s * volume;
            *output++ += s * volume;
        }

        // DEBUG_LOG("Num %d, position %f of %f", num, note->position, sample_end);

        if (ceil(note->position) >= sample_end)
        {
            nibble_audio_stop_note();
            break;
        }

        // Rewind the sample when reaching the loop point
        if (ceil(note->position) >= loop_end)
        {
            nibble_audio_stop_note();
            break;

            // Cut the sample if the end is reached
        }
        else if (ceil(note->position) >= sample->length)
        {
            nibble_audio_stop_note();
            break;
        }

        samples_to_write -= num;
    } while (num > 0);
}

void nibble_audio_stop_note()
{
    memory.soundState.triggered_note.active = false;
    memory.soundState.triggered_note.need_reset = true;
    memory.soundState.triggered_note.note_index = 0;
    memory.soundState.triggered_note.sample_index = 0;
    memory.soundState.triggered_note.volume = 0;
    memory.soundState.triggered_note.position = 0;
    memory.soundState.triggered_note.increment = 0;
}

int nibble_audio_note_to_period(int note)
{
    switch (note)
    {
    case 0:
        return 856;
    case 1:
        return 808;
    case 2:
        return 762;
    case 3:
        return 720;
    case 4:
        return 678;
    case 5:
        return 640;
    case 6:
        return 604;
    case 7:
        return 570;
    case 8:
        return 538;
    case 9:
        return 508;
    case 10:
        return 480;
    case 11:
        return 453;
    case 12:
        return 428;
    case 13:
        return 404;
    case 14:
        return 381;
    case 15:
        return 360;
    case 16:
        return 339;
    case 17:
        return 320;
    case 18:
        return 302;
    case 19:
        return 285;
    case 20:
        return 269;
    case 21:
        return 254;
    case 22:
        return 240;
    case 23:
        return 226;
    case 24:
        return 214;
    case 25:
        return 202;
    case 26:
        return 190;
    case 27:
        return 180;
    case 28:
        return 170;
    case 29:
        return 160;
    case 30:
        return 151;
    case 31:
        return 143;
    case 32:
        return 135;
    case 33:
        return 127;
    case 34:
        return 120;
    case 35:
        return 113;
    default:
        return 0;
    }
    return 0;
}

void nibble_audio_destroy()
{
}