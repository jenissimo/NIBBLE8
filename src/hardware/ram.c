#include "ram.h"

uint8_t *userLuaCode = NULL;
uint8_t *clipboard;
bool rebootRequested = false;
bool shutdownRequested = false;

mz_zip_archive *rom;
uint8_t *romBuffer;

Memory memory;

void nibble_ram_init()
{
    // poke(0x7bfb, 0x00)
    // poke(0x7bfc, 0x01)
    // poke(0x7bfd, 0x40)
    // poke(0x7bfe, 0x01)
    // unsigned char sample;       /* Sample number (0..31)                   */
    // unsigned char volume;       /* Base volume without tremolo (0..64)     */
    // unsigned char balance;      /* Stereo balance (0..255)                 */
    // unsigned short period;      /* Note period (113..856)                  */

    // DEBUG_LOG("Memory offset: %lu", sizeof(memory.spriteSheetData)+sizeof(memory.mapData)+sizeof(memory.spriteFlagsData)+sizeof(memory.drawState));
    // DEBUG_LOG("Size of int8_t*: %zu bytes\n", sizeof(int8_t*));
    //DEBUG_LOG("Alignment of int8_t*: %zu bytes\n", alignof(int8_t*));
}

void nibble_ram_clear()
{
    memset(memory.data, 0, sizeof(memory.data));
    userLuaCode = NULL;
}

void nibble_save_memory_layout()
{
    char *filename = "bin/docs/memory_layout.txt";
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        DEBUG_LOG("Error opening the file %s", filename);
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

void nibble_ram_dump_part(char *name, void *start, int size)
{
    /*
    FILE *f = fopen(name, "wb");
    fwrite(start, 1, size * sizeof(uint8_t), f);
    fclose(f);
    printf("Dumped to %s %lu bytes\n", name, size * sizeof(uint8_t));
    */
}

void nibble_ram_dump()
{
    /*
    nibble_ram_dump_part("dump/ram.bin", memory.data, NIBBLE_MEMORY_SIZE);
    nibble_ram_dump_part("dump/sprite_sheet.bin", memory.spriteSheetData, NIBBLE_SPRITE_SHEET_SIZE);
    nibble_ram_dump_part("dump/sprite_flags.bin", memory.spriteFlagsData, NIBBLE_SPRITE_FLAG_SIZE);
    nibble_ram_dump_part("dump/map.bin", memory.mapData, NIBBLE_MAP_SIZE);
    nibble_ram_dump_part("dump/music.bin", memory.musicData, NIBBLE_MUSIC_SIZE);
    nibble_ram_dump_part("dump/sfx.bin", memory.sfxData, NIBBLE_SFX_SIZE);
    dumpPart("dump/draw_state.bin", (uint8_t *)memory.drawState, sizeof(DrawState));
    nibble_ram_dump_part("dump/hardware_state.bin", memory.hardwareState, NIBBLE_HARDWARE_STATE_SIZE);
    nibble_ram_dump_part("dump/screen.bin", memory.screenData, NIBBLE_SCREEN_DATA_SIZE);
    */
}

void nibble_ram_print_map()
{
    for (int y = 0; y < NIBBLE_MAP_HEIGHT; y++)
    {
        for (int x = 0; x < NIBBLE_MAP_WIDTH; x++)
        {
            printf("%d ", memory.mapData[x + y * NIBBLE_MAP_WIDTH]);
        }
        printf("\n");
    }
}

void nibble_ram_destroy()
{
    if (rom)
    {
        mz_zip_reader_end(rom);
        free(rom);
        free(romBuffer);
        rom = NULL; // Set the pointer to NULL to avoid potential dangling pointers
    }
    // free(state->memory);
    // free(memory);
}

uint8_t nibble_api_peek(uint32_t addr)
{
    return memory.data[addr];
}

uint16_t nibble_api_peek2(uint32_t addr)
{
    return memory.data[addr] << 8 | memory.data[addr + 1];
}

void nibble_api_poke(uint32_t addr, uint8_t value)
{
    memory.data[addr] = value;
}

void nibble_api_poke2(uint32_t addr, uint16_t value)
{
    memory.data[addr] = value >> 8;
    memory.data[addr + 1] = value & 0xFF;
}

void nibble_api_memcpy(uint32_t destaddr, uint32_t sourceaddr, uint16_t len)
{
    memcpy(memory.data + destaddr, memory.data + sourceaddr, len);
}