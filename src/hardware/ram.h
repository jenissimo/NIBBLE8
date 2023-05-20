#ifndef nibble_ram_h
#define nibble_ram_h

#include <stdint.h>
#include <string.h>
#include "../nibble8.h"

typedef struct DrawState {
    uint8_t drawPaletteMap[4];
	uint8_t screenPaletteMap[4];

    uint8_t clip_xb;
	uint8_t clip_yb;
	uint8_t clip_xe;
	uint8_t clip_ye;

    uint8_t unknown05f24;

    uint8_t color;

    uint8_t text_x;
	uint8_t text_y;

    int16_t camera_x;
	int16_t camera_y;

    uint8_t drawMode;

    uint8_t devkitMode;

    uint8_t persistPalette;

    uint8_t soundPauseState;

    uint8_t suppressPause;

    uint8_t fillPattern[2];

    uint8_t fillPatternTransparencyBit;

    uint8_t colorSettingFlag;

    uint8_t lineInvalid;

    //hardware extension
    uint8_t unknown05f36;
    uint8_t unknown05f37;

    uint8_t tlineMapWidth;
    uint8_t tlineMapHeight;
    uint8_t tlineMapXOffset;
    uint8_t tlineMapYOffset;

    int16_t line_x;
    int16_t line_y;
} DrawState;

typedef struct MemoryLayout
{
    uint8_t spriteSheetData[NIBBLE_SPRITE_SHEET_SIZE];
    uint8_t mapData[NIBBLE_MAP_SIZE];
    uint8_t spriteFlagsData[NIBBLE_SPRITE_FLAG_SIZE];
    uint8_t musicData[NIBBLE_MUSIC_SIZE];
    uint8_t sfxData[NIBBLE_SFX_SIZE];
    DrawState drawState;
    uint8_t hardwareState[NIBBLE_HARDWARE_STATE_SIZE];
    uint8_t screenData[NIBBLE_SCREEN_DATA_SIZE];
} MemoryLayout;

typedef union
{
    struct 
    {
        uint8_t spriteSheetData[NIBBLE_SPRITE_SHEET_SIZE];
        uint8_t mapData[NIBBLE_MAP_SIZE];
        uint8_t spriteFlagsData[NIBBLE_SPRITE_FLAG_SIZE];
        uint8_t musicData[NIBBLE_MUSIC_SIZE];
        uint8_t sfxData[NIBBLE_SFX_SIZE];
        DrawState drawState;
        uint8_t hardwareState[NIBBLE_HARDWARE_STATE_SIZE];
        uint8_t screenData[NIBBLE_SCREEN_DATA_SIZE];
    };
    uint8_t data[sizeof(MemoryLayout)];
} Memory;

Memory memory;
uint8_t *userLuaCode;
uint8_t *clipboard;


void initRAM(void);
void saveMemoryLayout();
void dumpPart(char *name, void *start, int size);
void dumpRAM(void);
void destroyRAM(void);
uint8_t nibble_api_peek(uint16_t addr);
uint16_t nibble_api_peek2(uint16_t addr);
void nibble_api_poke(uint16_t addr, uint8_t value);
void nibble_api_poke2(uint16_t addr, uint16_t value);
void nibble_api_memcpy(uint16_t destaddr, uint16_t sourceaddr, uint16_t len);

#endif