#ifndef nibble_ram_h
#define nibble_ram_h

#ifdef DOS
#include <allegro.h>
#else
#include <stdint.h>
#endif
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "nibble8.h"
#include "hardware/empty_mod_file.h"
#include "debug/debug.h"
#include "utils/pocketmod.h"

typedef struct ColorPalette
{
    bool flip : 1; // Flip flag
    uint8_t r[4];  // Array of red components for 4 colors
    uint8_t g[4];  // Array of green components for 4 colors
    uint8_t b[4];  // Array of blue components for 4 colors
} ColorPalette;

typedef struct DrawState
{
    ColorPalette colorPalette;
    uint8_t drawPaletteMap[4];
    uint8_t screenPaletteMap[4];

    uint8_t clip_xb;
    uint8_t clip_yb;
    uint8_t clip_xe;
    uint8_t clip_ye;

    uint8_t color;

    uint8_t text_x;
    uint8_t text_y;

    int16_t camera_x;
    int16_t camera_y;

    uint8_t drawMode;

    uint8_t suppressPause;

    uint8_t fillPattern[2];

    uint8_t fillPatternTransparencyBit;

    uint8_t colorSettingFlag;

    uint8_t lineInvalid;

    uint8_t tlineMapWidth;
    uint8_t tlineMapHeight;
    uint8_t tlineMapXOffset;
    uint8_t tlineMapYOffset;

    int16_t line_x;
    int16_t line_y;
} DrawState;

typedef struct {
    uint8_t note_index;
    uint8_t sample_index;
    uint8_t volume;
    bool active : 1;
    bool need_reset : 1;
    float position; // Current position in the sample
    float increment; // Increment per frame, based on the note and sample rate
} TriggeredNote;

typedef struct SoundState
{
    bool music_active : 1;
    uint8_t music_start_pattern;
    uint8_t music_end_pattern;
    TriggeredNote triggered_note;
    int8_t sfx_patterns[NIBBLE_SFX_CHANNELS];
    pocketmod_context context;
} SoundState;

typedef struct MemoryLayout
{
    uint8_t spriteSheetData[NIBBLE_SPRITE_SHEET_SIZE];
    uint16_t mapData[NIBBLE_MAP_COUNT];
    uint8_t spriteFlagsData[NIBBLE_SPRITE_FLAG_SIZE];
    DrawState drawState;
    SoundState soundState;
    uint8_t hardwareState[NIBBLE_HARDWARE_STATE_SIZE];
    uint8_t screenData[NIBBLE_SCREEN_DATA_SIZE];
} MemoryLayout;

typedef union
{
    struct
    {
        uint8_t spriteSheetData[NIBBLE_SPRITE_SHEET_SIZE];
        uint16_t mapData[NIBBLE_MAP_COUNT];
        uint8_t spriteFlagsData[NIBBLE_SPRITE_FLAG_SIZE];
        DrawState drawState;
        SoundState soundState;
        uint8_t hardwareState[NIBBLE_HARDWARE_STATE_SIZE];
        uint8_t screenData[NIBBLE_SCREEN_DATA_SIZE];
    };
    uint8_t data[sizeof(MemoryLayout)];
} Memory;

extern Memory memory;
extern uint8_t *userLuaCode;
extern uint8_t *clipboard;
extern bool rebootRequested;
extern bool shutdownRequested;

void nibble_ram_init(void);
void nibble_ram_clear();
void nibble_save_memory_layout();
void nibble_ram_dump_part(char *name, void *start, int size);
void nibble_ram_dump(void);
void nibble_ram_destroy(void);
void nibble_ram_print_map();
uint8_t nibble_api_peek(uint16_t addr);
uint16_t nibble_api_peek2(uint16_t addr);
void nibble_api_poke(uint16_t addr, uint8_t value);
void nibble_api_poke2(uint16_t addr, uint16_t value);
void nibble_api_memcpy(uint16_t destaddr, uint16_t sourceaddr, uint16_t len);

#endif