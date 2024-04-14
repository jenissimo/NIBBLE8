#include "audio.h"
#include "ram.h"

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
    modContext = malloc(sizeof(pocketmod_context));

    if (!modContext)
    {
        fprintf(stderr, "Failed to allocate memory for modContext\n");
        exit(1);
    }

    /* Initialize the renderer */
    if (!pocketmod_init(modContext, EMPTY_MOD_FILE, EMPTY_MOD_FILE_SIZE, freq))
    {
        DEBUG_LOG("Failed to initialize PocketMod\n");
        exit(1);
    }
    else
    {
        DEBUG_LOG("PocketMod initialized\n");
    }
}

void nibble_audio_destroy()
{
    free(modContext);
}