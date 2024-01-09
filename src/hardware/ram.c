#include "ram.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../nibble8.h"

Memory memory;
uint8_t *userLuaCode;
uint8_t *clipboard;
bool rebootRequested = false;

void initRAM()
{
    //saveMemoryLayout();
}

void clearRAM()
{
    memset(memory.data, 0, sizeof(memory.data));
    userLuaCode = NULL;
}

void saveMemoryLayout()
{
    char *filename = "bin/docs/memory_layout.txt";
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Error opening the file %s", filename);
        return;
    }

    /*
    fprintf(fp, "Sprite Sheet: 0x%04X - 0x%04X\n", 0x0000, NIBBLE_SPRITE_SHEET_SIZE - 1);
    fprintf(fp, "Sprite Flags: 0x%04X - 0x%04X\n", NIBBLE_SPRITE_SHEET_SIZE, NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE - 1);
    fprintf(fp, "Map: 0x%04X - 0x%04X\n", NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE, NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE + NIBBLE_MAP_SIZE - 1);
    fprintf(fp, "Music: 0x%04X - 0x%04X\n", NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE + NIBBLE_MAP_SIZE, NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE + NIBBLE_MAP_SIZE + NIBBLE_MUSIC_SIZE - 1);
    fprintf(fp, "SFX: 0x%04X - 0x%04X\n", NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE + NIBBLE_MAP_SIZE + NIBBLE_MUSIC_SIZE, NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE + NIBBLE_MAP_SIZE + NIBBLE_MUSIC_SIZE + NIBBLE_SFX_SIZE - 1);
    fprintf(fp, "Draw State: 0x%04X - 0x%04X\n", NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE + NIBBLE_MAP_SIZE + NIBBLE_MUSIC_SIZE + NIBBLE_SFX_SIZE, NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE + NIBBLE_MAP_SIZE + NIBBLE_MUSIC_SIZE + NIBBLE_SFX_SIZE + NIBBLE_DRAW_STATE_SIZE - 1);
    fprintf(fp, "Hardware State: 0x%04X - 0x%04X\n", NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE + NIBBLE_MAP_SIZE + NIBBLE_MUSIC_SIZE + NIBBLE_SFX_SIZE + NIBBLE_DRAW_STATE_SIZE, NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE + NIBBLE_MAP_SIZE + NIBBLE_MUSIC_SIZE + NIBBLE_SFX_SIZE + NIBBLE_DRAW_STATE_SIZE + NIBBLE_HARDWARE_STATE_SIZE - 1);
    fprintf(fp, "Screen: 0x%04X - 0x%04X\n", NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE + NIBBLE_MAP_SIZE + NIBBLE_MUSIC_SIZE + NIBBLE_SFX_SIZE + NIBBLE_DRAW_STATE_SIZE + NIBBLE_HARDWARE_STATE_SIZE, NIBBLE_SPRITE_SHEET_SIZE + NIBBLE_SPRITE_FLAG_SIZE + NIBBLE_MAP_SIZE + NIBBLE_MUSIC_SIZE + NIBBLE_SFX_SIZE + NIBBLE_DRAW_STATE_SIZE + NIBBLE_HARDWARE_STATE_SIZE + NIBBLE_SCREEN_DATA_SIZE - 1);
    */

    fclose(fp);
}

void dumpPart(char *name, void *start, int size)
{
    /*
    FILE *f = fopen(name, "wb");
    fwrite(start, 1, size * sizeof(uint8_t), f);
    fclose(f);
    printf("Dumped to %s %lu bytes\n", name, size * sizeof(uint8_t));
    */
}

void dumpRAM()
{
    dumpPart("dump/ram.bin", memory.data, NIBBLE_MEMORY_SIZE);
    dumpPart("dump/sprite_sheet.bin", memory.spriteSheetData, NIBBLE_SPRITE_SHEET_SIZE);
    dumpPart("dump/sprite_flags.bin", memory.spriteFlagsData, NIBBLE_SPRITE_FLAG_SIZE);
    dumpPart("dump/map.bin", memory.mapData, NIBBLE_MAP_SIZE);
    dumpPart("dump/music.bin", memory.musicData, NIBBLE_MUSIC_SIZE);
    dumpPart("dump/sfx.bin", memory.sfxData, NIBBLE_SFX_SIZE);
    //dumpPart("dump/draw_state.bin", (uint8_t *)memory.drawState, sizeof(DrawState));
    dumpPart("dump/hardware_state.bin", memory.hardwareState, NIBBLE_HARDWARE_STATE_SIZE);
    dumpPart("dump/screen.bin", memory.screenData, NIBBLE_SCREEN_DATA_SIZE);
}

void destroyRAM()
{
    // free(state->memory);
    // free(memory);
}

uint8_t nibble_api_peek(uint16_t addr)
{
    return memory.data[addr];
}

uint16_t nibble_api_peek2(uint16_t addr)
{
    return memory.data[addr] << 8 | memory.data[addr + 1];
}

void nibble_api_poke(uint16_t addr, uint8_t value)
{
    memory.data[addr] = value;
}

void nibble_api_poke2(uint16_t addr, uint16_t value)
{
    memory.data[addr] = value >> 8;
    memory.data[addr + 1] = value & 0xFF;
}

void nibble_api_memcpy(uint16_t destaddr, uint16_t sourceaddr, uint16_t len)
{
    memcpy(memory.data + destaddr, memory.data + sourceaddr, len);
}