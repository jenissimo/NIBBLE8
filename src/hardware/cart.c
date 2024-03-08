#include "cart.h"

// Load cartridge data from a specified file path
ErrorCode nibble_api_load_cart(const char *path)
{
    if (access(path, F_OK) != 0)
    {
        return ERROR_CART_NOT_FOUND;
    }

    loadLuaCodeFromCart(path);
    loadMapDataFromCart(path);
    loadSpriteFlagsFromCart(path);
    loadSpriteSheetFromCart(path);

    return ERROR_SUCCESS;
}

// Load Lua code from the cart
void loadLuaCodeFromCart(const char *path)
{
    const char *luaCode;
    if (cart_has_file(path, "app.lua"))
    {
        load_text_from_zip(path, "app.lua", &luaCode);
        userLuaCode = luaCode;
    }
    else
    {
        userLuaCode = "\0";
    }
}

// Load map data from the cart
void loadMapDataFromCart(const char *path)
{
    if (cart_has_file(path, "map.bin"))
    {
        uint8_t *mapBuffer;
        size_t mapBufferSize = load_file_from_zip(path, "map.bin", (void **)&mapBuffer);
        processMapData(mapBuffer, mapBufferSize);
        free(mapBuffer);
    }
}

// Process raw map data into the memory structure
void processMapData(const uint8_t *buffer, size_t bufferSize)
{
    // Assuming buffer contains uint16_t values in little-endian format
    for (size_t i = 0; i < bufferSize / 2; ++i)
    {
        uint16_t value = buffer[i * 2] | (buffer[i * 2 + 1] << 8);
        memory.mapData[i] = value;
    }
}

// Load sprite flags data from the cart
void loadSpriteFlagsFromCart(const char *path)
{
    if (cart_has_file(path, "spriteFlags.bin"))
    {
        uint8_t *flagsBuffer;
        size_t flagsBufferSize = load_file_from_zip(path, "spriteFlags.bin", (void **)&flagsBuffer);
        // Ensure the buffer size matches NIBBLE_SPRITE_FLAG_SIZE
        if (flagsBufferSize == NIBBLE_SPRITE_FLAG_SIZE)
        {
            memcpy(memory.spriteFlagsData, flagsBuffer, flagsBufferSize);
        }
        else
        {
            DEBUG_LOG("Error: spriteFlags.bin size mismatch.\n");
        }
        free(flagsBuffer);
    }
}

// Load spritesheet image from the cart
void loadSpriteSheetFromCart(const char *path)
{
    uint8_t *png_data;
    if (cart_has_file(path, "spritesheet.png"))
    {
        load_file_from_zip(path, "spritesheet.png", (void **)&png_data);
        read_and_convert_png_from_buffer(memory.spriteSheetData, png_data, NIBBLE_SPRITE_SHEET_WIDTH, NIBBLE_SPRITE_SHEET_HEIGHT, &manager->palettes[0]);
        free(png_data);
    }
}

int nibble_api_save_cart(char *path, char *luaCode)
{
    remove(path);

    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));

    userLuaCode = luaCode;

    if (!mz_zip_writer_init_file(&zip_archive, path, 0))
    {
        DEBUG_LOG("Failed to initialize ZIP writer for %s\n", path);
        return 1;
    }

    // Read app.lua file
    size_t app_lua_size = strlen(userLuaCode);

    DEBUG_LOG("app.lua size: %zu\n", app_lua_size);

    if (!userLuaCode)
    {
        DEBUG_LOG("Failed to read app.lua\n");
        mz_zip_writer_end(&zip_archive);
        return 1;
    }

    // Add app.lua to the ZIP archive
    if (!mz_zip_writer_add_mem(&zip_archive, "app.lua", userLuaCode, app_lua_size, MZ_DEFAULT_COMPRESSION))
    {
        DEBUG_LOG("Failed to add app.lua to cartridge.zip\n");
        // free(app_lua_data);
        mz_zip_writer_end(&zip_archive);
        return 1;
    }
    // free(app_lua_data);

    // Read spritesheet.png file
    png_memory_write_state state = get_indexed_png(memory.spriteSheetData, NIBBLE_SPRITE_SHEET_WIDTH, NIBBLE_SPRITE_SHEET_HEIGHT, &manager->palettes[0]);

    // Add spritesheet.png to the ZIP archive
    if (!state.data || !mz_zip_writer_add_mem(&zip_archive, "spritesheet.png", state.data, state.size, MZ_DEFAULT_COMPRESSION))
    {
        DEBUG_LOG("Failed to add spritesheet.png to cartridge.zip\n");
        // free(spritesheet_data);
        mz_zip_writer_end(&zip_archive);
        return 1;
    }

    // Add spriteFlagsData to the ZIP archive
    if (!mz_zip_writer_add_mem(&zip_archive, "spriteFlags.bin", memory.spriteFlagsData, NIBBLE_SPRITE_FLAG_SIZE, MZ_DEFAULT_COMPRESSION))
    {
        DEBUG_LOG("Failed to add spriteFlags.bin to cartridge.zip\n");
        mz_zip_writer_end(&zip_archive);
        return 1;
    }

    // Add map to the ZIP archive
    if (!mz_zip_writer_add_mem(&zip_archive, "map.bin", memory.mapData, NIBBLE_MAP_SIZE, MZ_DEFAULT_COMPRESSION))
    {
        DEBUG_LOG("Failed to add map.bin to cartridge.zip\n");
        mz_zip_writer_end(&zip_archive);
        return 1;
    }

    mz_zip_writer_finalize_archive(&zip_archive);
    mz_zip_writer_end(&zip_archive);

    DEBUG_LOG("Successfully created %s\n", path);
    return 0;
}

void nibble_api_import_png(char *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        fprintf(stderr, "Error opening file: %s\n", path);
        return;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the buffer
    uint8_t *buffer = (uint8_t *)malloc(file_size);
    if (!buffer)
    {
        fprintf(stderr, "Error allocating memory for file buffer\n");
        fclose(file);
        return;
    }

    // Read the file contents into the buffer
    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    if (bytes_read != file_size)
    {
        fprintf(stderr, "Error reading file: %s\n", path);
        free(buffer);
        return;
    }

    read_and_convert_png_from_buffer(memory.spriteSheetData, buffer, NIBBLE_SPRITE_SHEET_WIDTH, NIBBLE_SPRITE_SHEET_HEIGHT, &manager->palettes[0]);
}

void nibble_api_export_png(char *path)
{
    write_indexed_png(path, memory.spriteSheetData, NIBBLE_SPRITE_SHEET_WIDTH, NIBBLE_SPRITE_SHEET_HEIGHT, &manager->palettes[0]);
}

void nibble_api_import_lua(char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file: %s\n", path);
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    userLuaCode = (uint8_t *)malloc(fileSize + 1);
    if (userLuaCode == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return;
    }

    fread(userLuaCode, 1, fileSize, file);
    userLuaCode[fileSize] = '\0'; // Null-terminate the string

    fclose(file);
}

void nibble_api_export_lua(char *path)
{
    if (userLuaCode == NULL)
    {
        fprintf(stderr, "No Lua code to write\n");
        return;
    }

    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for writing: %s\n", path);
        return;
    }

    fprintf(file, "%s", userLuaCode);
    fclose(file);
}

bool extract_file_to_buffer(const char *zip_filename, const char *file_to_load, void **buffer, size_t *buffer_size)
{
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));
    if (!mz_zip_reader_init_file(&zip_archive, zip_filename, 0))
    {
        DEBUG_LOG("Failed to initialize zip reader for %s\n", zip_filename);
        return false;
    }

    int file_index = mz_zip_reader_locate_file(&zip_archive, file_to_load, NULL, 0);
    if (file_index < 0)
    {
        DEBUG_LOG("Failed to locate %s in %s\n", file_to_load, zip_filename);
        mz_zip_reader_end(&zip_archive);
        return false;
    }

    mz_zip_archive_file_stat file_stat;
    if (!mz_zip_reader_file_stat(&zip_archive, file_index, &file_stat))
    {
        DEBUG_LOG("Zip file read error %s in %s\n", file_to_load, zip_filename);
        mz_zip_reader_end(&zip_archive);
        return false;
    }

    *buffer = mz_zip_reader_extract_file_to_heap(&zip_archive, file_to_load, &file_stat.m_uncomp_size, 0);
    if (buffer_size)
        *buffer_size = file_stat.m_uncomp_size;

    mz_zip_reader_end(&zip_archive);
    return (*buffer != NULL);
}

bool cart_has_file(const char *zip_filename, const char *file_to_load)
{
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));
    if (!mz_zip_reader_init_file(&zip_archive, zip_filename, 0))
    {
        DEBUG_LOG("Failed to initialize zip reader for %s\n", zip_filename);
        return false;
    }

    int file_index = mz_zip_reader_locate_file(&zip_archive, file_to_load, NULL, 0);
    if (file_index < 0)
    {
        DEBUG_LOG("Failed to locate %s in %s\n", file_to_load, zip_filename);
        mz_zip_reader_end(&zip_archive);
        return false;
    }

    mz_zip_reader_end(&zip_archive);
    return true;
}

size_t load_file_from_zip(const char *zip_filename, const char *file_to_load, void **buffer)
{
    size_t buffer_size;
    if (extract_file_to_buffer(zip_filename, file_to_load, buffer, &buffer_size))
    {
        DEBUG_LOG("%s loaded into memory successfully %zu bytes\n", file_to_load, buffer_size);
    }
    else
    {
        DEBUG_LOG("Failed to load %s into memory\n", file_to_load);
    }

    return buffer_size;
}

void load_text_from_zip(const char *zip_filename, const char *file_to_load, char **buffer)
{
    size_t buffer_size;
    if (extract_file_to_buffer(zip_filename, file_to_load, (void **)buffer, &buffer_size))
    {
        char *textBuffer = malloc(buffer_size + 1);
        if (textBuffer)
        {
            memcpy(textBuffer, *buffer, buffer_size);
            textBuffer[buffer_size] = '\0';
            mz_free(*buffer);
            *buffer = textBuffer;
            DEBUG_LOG("%s loaded into memory successfully %zu bytes\n", file_to_load, buffer_size);
        }
        else
        {
            DEBUG_LOG("Failed to allocate memory for %s\n", file_to_load);
            mz_free(*buffer);
        }
    }
    else
    {
        DEBUG_LOG("Failed to load %s into memory\n", file_to_load);
    }
}