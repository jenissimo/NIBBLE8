#include "audio.h"

pocketmod_context masterContext;
pocketmod_context musicContext;
pocketmod_context sfxContexts[NIBBLE_SFX_CHANNELS];

uint8_t *modFileBuffer;
int modFileBufferSize;

void nibble_audio_init(int freq, uint8_t *modData, int modSize)
{
    DEBUG_LOG("Init pocketmod");
    /* Initialize the renderer */

    if (modData == NULL)
    {
        pocketmod_init(&masterContext, EMPTY_MOD_FILE, EMPTY_MOD_FILE_SIZE, freq);
    }
    else if (pocketmod_init(&masterContext, modData, modSize, freq))
    {
        DEBUG_LOG("Initialized PocketMod from cart!");
    }
    else
    {
        DEBUG_LOG("Failed to initialize PocketMod");
        exit(1);
        return;
    }

    free(modFileBuffer);
    modFileBuffer = modData;
    modFileBufferSize = modSize;

    nibble_audio_init_channel(&musicContext, &masterContext);
    for (int i = 0; i < NIBBLE_SFX_CHANNELS; i++)
    {
        nibble_audio_init_channel(&sfxContexts[i], &masterContext);
    }

    nibble_audio_reset();
}

void nibble_audio_update(uint8_t *buffer, int bytes)
{
    static uint8_t music_buffer[NIBBLE_SAMPLES * 2];                     // Buffer for rendering music
    static uint8_t sfx_buffers[NIBBLE_SFX_CHANNELS][NIBBLE_SAMPLES * 2]; // Buffers for rendering sound effects
    static uint8_t mix_buffer[NIBBLE_SAMPLES * 2];                       // Mix buffer for combining music and sound effects

    // Clear the channels to prevent noise
    memset(buffer, 127, bytes);

    // Load new patterns to pocketmod if needed
    if (memory.soundState.need_update_patterns)
    {
        // DEBUG_LOG("Loaded new patterns");
        memory.soundState.need_update_patterns = false;
        memcpy(masterContext.patterns, &memory.soundState.patterns, NIBBLE_PATTERNS_COUNT * NIBBLE_PATTERN_LENGTH * NIBBLE_MUSIC_CHANNELS * NIBBLE_LINE_SIZE);
    }

    // if no active sound - break
    if (!memory.soundState.music_active && !memory.soundState.triggered_note.active && !nibble_audio_has_active_sfx())
    {
        return;
    }

    memset(music_buffer, 127, bytes);
    for (int i = 0; i < NIBBLE_SFX_CHANNELS; i++)
    {
        memset(sfx_buffers[i], 127, bytes);
    }
    memset(mix_buffer, 0, bytes);

    // Render music if active
    if (memory.soundState.music_active)
    {
        int i = 0;
        while (i < bytes)
        {
            i += pocketmod_render_u8(&musicContext, music_buffer + i, bytes - i);
            if (musicContext.pattern >= (memory.soundState.music_start_pattern + memory.soundState.music_length))
            {
                nibble_api_music(-1, 0);
                break;
            }
        }
        memory.soundState.current_pattern = musicContext.pattern;
        memory.soundState.current_line = musicContext.line;
    }

    for (int sfxChannelId = 0; sfxChannelId < NIBBLE_SFX_CHANNELS; sfxChannelId++)
    {
        if (memory.soundState.sfx_channels[sfxChannelId].sfxId > -1)
        {
            nibble_audio_update_sfx(sfxChannelId, &sfx_buffers[sfxChannelId], bytes);
        }
    }

    // Mix in any active notes into mixBuffer
    if (memory.soundState.triggered_note.active)
    {
        // DEBUG_LOG("triggered note: %d", memory.soundState.triggered_note.note_index);
        nibble_audio_play_note(&musicContext, &memory.soundState.triggered_note, music_buffer, bytes);
    }

    // Mix music buffer into the mix buffer
    for (int i = 0; i < bytes; i++)
    {
        mix_buffer[i] += music_buffer[i];
    }

    // Mix sound effect buffers into the mix buffer
    for (int sfx_channel = 0; sfx_channel < NIBBLE_SFX_CHANNELS; sfx_channel++)
    {
        uint8_t *sfx_buffer = sfx_buffers[sfx_channel];
        for (int i = 0; i < bytes; i++)
        {
            mix_buffer[i] += sfx_buffer[i];
        }
    }
    nibble_audio_apply_limiter(mix_buffer, bytes);
    memcpy(buffer, mix_buffer, bytes);
}

void nibble_audio_apply_limiter(uint8_t *mix_buffer, int bytes_to_limit)
{
    for (int i = 0; i < bytes_to_limit; i++)
    {
        if (mix_buffer[i] > NIBBLE_AUDIO_LIMITER_THRESHOLD)
        {
            mix_buffer[i] = NIBBLE_AUDIO_LIMITER_THRESHOLD;
        }
    }
}

void nibble_audio_update_sfx(int sfxChannelId, uint8_t *output, int bytes)
{
    SfxChannel sfxChannel = memory.soundState.sfx_channels[sfxChannelId];
    pocketmod_context *sfxContext = &sfxContexts[sfxChannelId];

    if (!output)
    {
        return;
    }

    // Ensure sfxId is within valid range
    if (sfxChannel.sfxId < 0 || sfxChannel.sfxId >= NIBBLE_PATTERNS_COUNT)
    {
        return;
    }

    // Render the sound effect
    int i = 0;
    while (i < bytes)
    {
        i += pocketmod_render_u8(sfxContext, output + i, bytes - i);
        // Check if we reached the end of the sound effect
        if ((sfxContext->line >= (sfxChannel.offset + sfxChannel.length)) || (sfxContext->pattern != sfxChannel.sfxId))
        {
            nibble_audio_reset_sfx_channel(sfxChannelId);
            nibble_audio_reset_context(sfxContext);
            break;
        }
    }
}

void nibble_audio_reset()
{
    memory.soundState.music_active = false;
    memory.soundState.need_update_patterns = false;
    memory.soundState.music_start_pattern = 0;
    memory.soundState.music_length = 0;

    nibble_audio_reset_context(&musicContext);
    for (int sfxChannelId = 0; sfxChannelId < NIBBLE_SFX_CHANNELS; sfxChannelId++)
    {
        nibble_audio_reset_sfx_channel(sfxChannelId);
        nibble_audio_reset_context(&sfxContexts[sfxChannelId]);
    }

    memory.soundState.triggered_note.active = false;
    memory.soundState.triggered_note.note_index = 0;
    memory.soundState.triggered_note.sample_index = 0;
    memory.soundState.triggered_note.volume = 0;
    memory.soundState.triggered_note.position = 0;
    memory.soundState.triggered_note.increment = 0;
    memory.soundState.triggered_note.need_reset = true;

    // Copy pattern data
    memset(memory.soundState.patterns, 0, NIBBLE_PATTERNS_COUNT * NIBBLE_PATTERN_LENGTH * NIBBLE_MUSIC_CHANNELS * NIBBLE_LINE_SIZE);
    memcpy(memory.soundState.patterns, masterContext.patterns, masterContext.num_patterns * NIBBLE_PATTERN_LENGTH * NIBBLE_MUSIC_CHANNELS * NIBBLE_LINE_SIZE);
    DEBUG_LOG("Memory size: %lu", NIBBLE_PATTERNS_COUNT * NIBBLE_PATTERN_LENGTH * NIBBLE_MUSIC_CHANNELS * NIBBLE_LINE_SIZE);
    DEBUG_LOG("Patterns size: %lu", masterContext.num_patterns * NIBBLE_PATTERN_LENGTH * NIBBLE_MUSIC_CHANNELS * NIBBLE_LINE_SIZE);

    // Copy sample names and data
    for (int i = 0; i < POCKETMOD_MAX_SAMPLES; i++)
    {
        memcpy(&memory.soundState.samples[i].name, &masterContext.samples[i].name, 23);
    }
}

void nibble_audio_reset_sfx_channel(int sfxChannelId)
{
    memory.soundState.sfx_channels[sfxChannelId].sfxId = -1;
    memory.soundState.sfx_channels[sfxChannelId].offset = 0;
    memory.soundState.sfx_channels[sfxChannelId].length = 0;
}

void nibble_audio_init_channel(pocketmod_context *context, pocketmod_context *masterContext)
{
    if (!context || !masterContext)
    {
        DEBUG_LOG("nibble_audio_init_channel Invalid arguments");
        return;
    }

    _pocketmod_zero(context, sizeof(pocketmod_context));
    // Copy necessary pointers from the master context to the new context
    memcpy(context->samples, masterContext->samples, sizeof(masterContext->samples));
    context->source = masterContext->source;
    context->order = masterContext->order;
    context->patterns = masterContext->patterns;
    context->length = masterContext->length;
    context->reset = masterContext->reset;

    context->num_patterns = masterContext->num_patterns;
    context->num_samples = masterContext->num_samples;
    context->num_channels = masterContext->num_channels;

    context->pattern_delay = masterContext->pattern_delay;

    nibble_audio_reset_context(context);
    _pocketmod_next_tick(context);
}

void nibble_audio_reset_context(pocketmod_context *context)
{
    for (int i = 0; i < context->num_channels; i++)
    {
        _pocketmod_zero(&(context->channels[i]), sizeof(_pocketmod_chan));
        context->channels[i].balance = 0x80 + ((((i + 1) >> 1) & 1) ? 0x20 : -0x20);
    }

    /* Prepare to render from the start */
    context->ticks_per_line = 6;
    context->samples_per_second = masterContext.samples_per_second;
    context->samples_per_tick = masterContext.samples_per_tick;
    context->lfo_rng = 0xbadc0de;
    context->line = -1;
    context->tick = context->ticks_per_line - 1;
}

void nibble_audio_play_note(pocketmod_context *context, TriggeredNote *note, uint8_t *output, int buffer_size)
{
    if (!note->active || !context || !output)
    {
        // DEBUG_LOG("Note is not active or context/output is NULL\n");
        return;
    }

    if (note->need_reset)
    {
        // DEBUG_LOG("Need to reset note");
        int period = nibble_audio_note_to_period(note->note_index);
        note->increment = 3546894.6f / (period * context->samples_per_second);
        note->position = 0;
        note->need_reset = false;
    }

    int samples_to_write = buffer_size / 2; // Assuming stereo output, each sample is uint8_t and stereo requires two uint8_t

    _pocketmod_sample *sample = &context->samples[note->sample_index - 1];
    unsigned char *data = POCKETMOD_SAMPLE(context, note->sample_index);
    const int loop_start = ((data[4] << 8) | data[5]) << 1;
    const int loop_length = ((data[6] << 8) | data[7]) << 1;
    const int loop_end = loop_length > 2 ? loop_start + loop_length : 0xffffff;
    const float sample_end = 1 + _pocketmod_min(loop_end, sample->length);

    const float volume = note->volume / (float)(128 * 64);
    const bool has_loop = loop_length > 2;

    int i, num;
    do
    {
        num = (sample_end - note->position) / note->increment;
        num = _pocketmod_min(num, samples_to_write);

        if (num <= 0)
        {
            break;
        }

        for (i = 0; i < num; i++)
        {
            int x0 = (int)note->position;
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

            // Convert float sample to 8-bit integer and clamp the range
            uint8_t s_int = (uint8_t)(_pocketmod_clamp_int((s * volume) * 127.5f, 0, 255));

            *output++ += s_int;
            *output++ += s_int;

            if (has_loop && note->position >= loop_end - 1)
            {
                note->position = loop_start + (note->position - loop_end);
            }
            else if ((!has_loop) && (note->position >= sample->length))
            {
                nibble_audio_stop_note();
                break;
            }
        }

        samples_to_write -= num;
    } while (num > 0 && samples_to_write > 0);
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

void nibble_api_sfx(uint8_t n, uint8_t offset, uint8_t length)
{
    pocketmod_context *sfxContext;

    if (n > NIBBLE_PATTERNS_COUNT)
    {
        n = NIBBLE_PATTERNS_COUNT - 1;
    }

    if (offset >= NIBBLE_PATTERN_LENGTH)
    {
        offset = NIBBLE_PATTERN_LENGTH - 1;
    }

    if (offset + length > NIBBLE_PATTERN_LENGTH)
    {
        length = (offset + length) - NIBBLE_PATTERN_LENGTH;
    }

    int8_t freeChannel = -1;
    // First checking if we already have similiar sfx to avoid sound multiplying
    for (int channelId = 0; channelId < NIBBLE_SFX_CHANNELS; channelId++)
    {
        SfxChannel *sfxChannel = &memory.soundState.sfx_channels[channelId];
        if ((sfxChannel->sfxId == n) && (sfxChannel->offset == offset))
        {
            freeChannel = channelId;
        }
    }

    // If we dont have double - searching for free channel
    if (freeChannel == -1)
    {
        for (int channelId = 0; channelId < NIBBLE_SFX_CHANNELS; channelId++)
        {
            if (memory.soundState.sfx_channels[channelId].sfxId == -1)
            {
                freeChannel = channelId;
                break;
            }
        }
    }

    // If we not found free channel - get first one
    if (freeChannel == -1)
    {
        freeChannel = 0;
    }
    sfxContext = &sfxContexts[freeChannel];

    memory.soundState.sfx_channels[freeChannel].sfxId = n;
    memory.soundState.sfx_channels[freeChannel].offset = offset;
    memory.soundState.sfx_channels[freeChannel].length = length;

    nibble_audio_reset_context(sfxContext);
    sfxContext->pattern = n;
    // Offset to -1 and then next_tick to init
    sfxContext->line = offset - 1;
    _pocketmod_next_tick(sfxContext);
}

void nibble_api_music(int8_t n, uint8_t length)
{
    if (n < 0)
    {
        DEBUG_LOG("Stop Music");
        memory.soundState.music_active = false;
        memory.soundState.music_start_pattern = 0;
        memory.soundState.music_length = 0;
        nibble_audio_reset_context(&musicContext);
        return;
    }

    if (n > NIBBLE_PATTERNS_COUNT)
    {
        n = NIBBLE_PATTERNS_COUNT - 1;
    }

    if (n + length > NIBBLE_PATTERNS_COUNT)
    {
        length = (n + length) - NIBBLE_PATTERNS_COUNT;
    }

    memory.soundState.music_active = true;
    memory.soundState.music_start_pattern = n;
    memory.soundState.music_length = length;

    DEBUG_LOG("Music start %d pattern length %d", n, length);
    nibble_audio_reset_context(&musicContext);
    musicContext.pattern = n;
    // Offset to -1 and then next_tick to init
    musicContext.line = -1;
    _pocketmod_next_tick(&musicContext);
}

bool nibble_audio_has_active_sfx()
{
    for (int channel = 0; channel < NIBBLE_SFX_CHANNELS; channel++)
    {
        if (memory.soundState.sfx_channels[channel].sfxId > -1)
        {
            return true;
        }
    }

    return false;
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
    free(modFileBuffer);

    memory.soundState.music_active = false;
    memory.soundState.music_start_pattern = 0;
    memory.soundState.music_length = 0;
    for (int channel = 0; channel < NIBBLE_SFX_CHANNELS; channel++)
    {
        nibble_audio_reset_sfx_channel(channel);
    }

    memory.soundState.triggered_note.active = false;
    memory.soundState.triggered_note.note_index = 0;
    memory.soundState.triggered_note.sample_index = 0;
    memory.soundState.triggered_note.volume = 0;
    memory.soundState.triggered_note.position = 0;
    memory.soundState.triggered_note.increment = 0;
    memory.soundState.triggered_note.need_reset = true;
}