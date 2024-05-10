#ifndef nibble_audio_h
#define nibble_audio_h

#include <stdlib.h>
#ifdef DOS
#include <allegro.h>
#else
#include <stdint.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "ram.h"

extern pocketmod_context masterContext;
extern pocketmod_context musicChannel;
extern pocketmod_context sfxChannels[NIBBLE_SFX_CHANNELS];
extern uint8_t *modFileBuffer;
extern int modFileBufferSize;

// Initializing
void nibble_audio_init(int freq, uint8_t *modData, int modSize);
void nibble_audio_init_channel(pocketmod_context *context, pocketmod_context *masterContext);

// Update
void nibble_audio_update(uint8_t *buffer, int bytes);
void nibble_audio_update_sfx(int sfxChannelId, uint8_t *output, int bytes);

// Resetting
void nibble_audio_reset();
void nibble_audio_reset_sfx_channel(int sfxChannelId);
void nibble_audio_reset_context(pocketmod_context *context);

// Single note
void nibble_audio_play_note(pocketmod_context *context, TriggeredNote *note, uint8_t *output, int buffer_size);
void nibble_audio_stop_note();

// Destroy
void nibble_audio_destroy();

// Utils
int nibble_audio_note_to_period(int note);
bool nibble_audio_has_active_sfx();
void nibble_audio_apply_limiter(uint8_t *mix_buffer, int bytes_to_limit);

// API
void nibble_api_sfx(uint8_t n, uint8_t offset, uint8_t length);
void nibble_api_music(int8_t n, uint8_t length);

#endif