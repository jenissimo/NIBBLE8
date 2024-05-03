#include "cart.h"

// Load cartridge data from a specified file path
ErrorCode nibble_api_load_cart(const char *path, char **adjustedPath)
{
    // Check if the original path exists
    if (access(path, F_OK) == 0)
    {
        if (adjustedPath != NULL)
        {                                 // Check if caller is interested in the adjusted path
            *adjustedPath = strdup(path); // Allocate and copy the original path
        }
        return loadCart(path); // Call loadCart with the original path
    }

    size_t newPathLen = strlen(path) + 9; // +8 for ".n8.png" + 1 for null terminator
    char *newPath = (char *)malloc(newPathLen);
    if (!newPath)
    {
        if (adjustedPath != NULL)
            *adjustedPath = NULL; // Indicate failure to adjust path
        DEBUG_LOG("Memory allocation failed.");
        return ERROR_MEMORY_ALLOCATION_FAILED;
    }

    // Check with ".n8" extension
    snprintf(newPath, newPathLen, "%s.n8", path);
    if (access(newPath, F_OK) == 0)
    {
        if (adjustedPath != NULL)
        {
            *adjustedPath = newPath; // Transfer newPath ownership to *adjustedPath
        }
        else
        {
            free(newPath); // If caller doesn't want the adjusted path, free it
        }
        return loadCart(newPath); // Success with ".n8"
    }

    // Check with ".n8.png" extension
    snprintf(newPath, newPathLen, "%s.n8.png", path);
    if (access(newPath, F_OK) == 0)
    {
        if (adjustedPath != NULL)
        {
            *adjustedPath = newPath; // Transfer newPath ownership to *adjustedPath
        }
        else
        {
            free(newPath); // If caller doesn't want the adjusted path, free it
        }
        return loadCart(newPath); // Success with ".n8.png"
    }

    // Cleanup if file not found with any of the extensions
    free(newPath); // Ensure allocated memory is always freed
    if (adjustedPath != NULL)
        *adjustedPath = NULL; // Indicate no valid path was found

    return ERROR_FILE_NOT_FOUND;
}

ErrorCode loadCart(const char *path)
{
    if (!nibble_is_within_sandbox(path))
    {
        return ERROR_CART_NOT_FOUND;
    }

    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));
    uint8_t *zipBuffer = NULL;

    DEBUG_LOG("Loading cart: %s", path);

    if (!cart_find_zip(path, &zip_archive, &zipBuffer))
    {
        return ERROR_INVALID_FORMAT;
    }

    ErrorCode result = loadCartFromZipBuffer(zipBuffer, zip_archive.m_archive_size);

    mz_zip_reader_end(&zip_archive);
    free(zipBuffer); // It's safe to call free on NULL.

    return result;
}

ErrorCode loadCartFromBase64(const char *base64)
{
    if (base64 == NULL)
    {
        return ERROR_INVALID_FORMAT;
    }

    size_t outputLength = 0;
    // Assuming decodeBase64 is implemented as shown in previous examples
    unsigned char *buffer = base64_decode(base64, strlen(base64), &outputLength);

    if (buffer == NULL)
    {
        return ERROR_INVALID_FORMAT;
    }

    ErrorCode loadResult = loadCartFromZipBuffer(buffer, outputLength);
    free(buffer); // Clean up the decoded data buffer
    return loadResult;
}

ErrorCode loadCartFromZipBuffer(const uint8_t *buffer, size_t bufferSize)
{
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));

    if (!mz_zip_reader_init_mem(&zip_archive, buffer, bufferSize, 0))
    {
        return ERROR_INVALID_FORMAT;
    }

    loadLuaCodeFromCart(&zip_archive);
    loadMapDataFromCart(&zip_archive);
    loadSpriteFlagsFromCart(&zip_archive);
    loadSpriteSheetFromCart(&zip_archive);

    mz_zip_reader_end(&zip_archive);

    return ERROR_SUCCESS;
}

// Load Lua code from the cart
void loadLuaCodeFromCart(mz_zip_archive *zip_archive)
{
    char *luaCode = NULL;
    if (cart_has_file(zip_archive, "app.lua"))
    {
        load_text_from_zip(zip_archive, "app.lua", &luaCode);
        userLuaCode = luaCode; // Assign newly loaded code
    }
    else
    {
        userLuaCode = strdup(""); // Allocate a new empty string if not found
    }
}

// Load map data from the cart
void loadMapDataFromCart(mz_zip_archive *zip_archive)
{
    if (cart_has_file(zip_archive, "map.bin"))
    {
        uint8_t *mapBuffer;
        size_t mapBufferSize = load_file_from_zip(zip_archive, "map.bin", (void **)&mapBuffer);
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
void loadSpriteFlagsFromCart(mz_zip_archive *zip_archive)
{
    if (cart_has_file(zip_archive, "spriteFlags.bin"))
    {
        uint8_t *flagsBuffer;
        size_t flagsBufferSize = load_file_from_zip(zip_archive, "spriteFlags.bin", (void **)&flagsBuffer);
        // Ensure the buffer size matches NIBBLE_SPRITE_FLAG_SIZE
        if (flagsBufferSize == NIBBLE_SPRITE_FLAG_SIZE)
        {
            memcpy(memory.spriteFlagsData, flagsBuffer, flagsBufferSize);
        }
        else
        {
            DEBUG_LOG("Error: spriteFlags.bin size mismatch.");
        }
        free(flagsBuffer);
    }
}

// Load spritesheet image from the cart
void loadSpriteSheetFromCart(mz_zip_archive *zip_archive)
{
    uint8_t *png_data;
    if (cart_has_file(zip_archive, "spritesheet.png"))
    {
        load_file_from_zip(zip_archive, "spritesheet.png", (void **)&png_data);
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
        DEBUG_LOG("Failed to initialize ZIP writer for %s", path);
        return 1;
    }

    // Read app.lua file
    size_t app_lua_size = strlen(userLuaCode);

    DEBUG_LOG("app.lua size: %zu", app_lua_size);

    if (!userLuaCode)
    {
        DEBUG_LOG("Failed to read app.lua");
        mz_zip_writer_end(&zip_archive);
        return 1;
    }

    // Add app.lua to the ZIP archive
    if (!mz_zip_writer_add_mem(&zip_archive, "app.lua", userLuaCode, app_lua_size, MZ_DEFAULT_COMPRESSION))
    {
        DEBUG_LOG("Failed to add app.lua to cartridge.zip");
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
        DEBUG_LOG("Failed to add spritesheet.png to cartridge.zip");
        // free(spritesheet_data);
        mz_zip_writer_end(&zip_archive);
        return 1;
    }

    // Add spriteFlagsData to the ZIP archive
    if (!mz_zip_writer_add_mem(&zip_archive, "spriteFlags.bin", memory.spriteFlagsData, NIBBLE_SPRITE_FLAG_SIZE, MZ_DEFAULT_COMPRESSION))
    {
        DEBUG_LOG("Failed to add spriteFlags.bin to cartridge.zip");
        mz_zip_writer_end(&zip_archive);
        return 1;
    }

    // Add map to the ZIP archive
    if (!mz_zip_writer_add_mem(&zip_archive, "map.bin", memory.mapData, NIBBLE_MAP_SIZE, MZ_DEFAULT_COMPRESSION))
    {
        DEBUG_LOG("Failed to add map.bin to cartridge.zip");
        mz_zip_writer_end(&zip_archive);
        return 1;
    }

    mz_zip_writer_finalize_archive(&zip_archive);
    mz_zip_writer_end(&zip_archive);

    DEBUG_LOG("Successfully created %s", path);
    return 0;
}

void nibble_api_import_png(char *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        fprintf(stderr, "Error opening file: %s", path);
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
        fprintf(stderr, "Error allocating memory for file buffer");
        fclose(file);
        return;
    }

    // Read the file contents into the buffer
    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    if (bytes_read != file_size)
    {
        fprintf(stderr, "Error reading file: %s", path);
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
        fprintf(stderr, "Error opening file: %s", path);
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

bool cart_find_zip(const char *path, mz_zip_archive *zip_archive, uint8_t **outBuffer)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        DEBUG_LOG("Error opening file.");
        return false;
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t *buffer = (uint8_t *)malloc(fileSize);
    if (!buffer)
    {
        fclose(file);
        DEBUG_LOG("Memory allocation failed.");
        return false;
    }

    if (fread(buffer, 1, fileSize, file) != fileSize)
    {
        fclose(file);
        free(buffer);
        DEBUG_LOG("Failed to read the entire file.");
        return false;
    }
    fclose(file);

    // Attempt to locate the ZIP header. For PNG files, this might be embedded past the PNG data.
    uint8_t *zipStart = NULL;
    const uint8_t zipHeader[] = {0x50, 0x4B, 0x03, 0x04}; // Standard ZIP header
    for (size_t i = 0; i < fileSize - sizeof(zipHeader); ++i)
    {
        if (memcmp(buffer + i, zipHeader, sizeof(zipHeader)) == 0)
        {
            zipStart = buffer + i;
            break;
        }
    }

    if (!zipStart)
    {
        DEBUG_LOG("ZIP header not found.");
        free(buffer);
        return false;
    }

    size_t zipSize = fileSize - (zipStart - buffer);
    memset(zip_archive, 0, sizeof(mz_zip_archive));
    if (!mz_zip_reader_init_mem(zip_archive, zipStart, zipSize, 0))
    {
        DEBUG_LOG("Failed to initialize ZIP reader.");
        free(buffer);
        return false;
    }

    // Store the buffer in m_pUser for later cleanup.
    *outBuffer = buffer;

    return true; // Success
}

bool extract_file_to_buffer(mz_zip_archive *zip_archive, const char *file_to_load, void **buffer, size_t *buffer_size)
{
    if (!zip_archive)
    {
        DEBUG_LOG("ZIP archive not initialized.");
        return false;
    }

    // Ensure there's a file to load and the buffer pointers are valid.
    if (!file_to_load || !buffer || !buffer_size)
    {
        DEBUG_LOG("Invalid parameters.");
        return false;
    }

    *buffer = mz_zip_reader_extract_file_to_heap(zip_archive, file_to_load, buffer_size, 0);
    return (*buffer != NULL);
}

bool cart_has_file(mz_zip_archive *zip_archive, const char *file_to_load)
{
    if (!zip_archive)
    {
        DEBUG_LOG("ZIP archive not initialized.");
        return false;
    }

    int file_index = mz_zip_reader_locate_file(zip_archive, file_to_load, NULL, 0);
    if (file_index < 0)
    {
        DEBUG_LOG("Failed to locate %s", file_to_load);
        return false;
    }

    return true;
}

size_t load_file_from_zip(mz_zip_archive *zip_archive, const char *file_to_load, void **buffer)
{
    if (!zip_archive)
    {
        DEBUG_LOG("ZIP archive not initialized.");
        return 0;
    }

    size_t buffer_size = 0;
    if (extract_file_to_buffer(zip_archive, file_to_load, buffer, &buffer_size))
    {
        DEBUG_LOG("%s loaded into memory successfully %zu bytes", file_to_load, buffer_size);
    }
    else
    {
        DEBUG_LOG("Failed to load %s into memory", file_to_load);
    }

    return buffer_size;
}

void load_text_from_zip(mz_zip_archive *zip_archive, const char *file_to_load, char **buffer)
{
    if (!zip_archive || !file_to_load || !buffer)
    {
        DEBUG_LOG("Invalid parameters.");
        return;
    }

    size_t buffer_size = 0;

    *buffer = (char *)mz_zip_reader_extract_file_to_heap(zip_archive, file_to_load, &buffer_size, 0);
    if (*buffer)
    {
        size_t actual_size_needed = buffer_size + 1;             // Correctly calculate after buffer_size is known
        char *new_buffer = realloc(*buffer, actual_size_needed); // Attempt to resize the buffer
        if (new_buffer)
        {
            *buffer = new_buffer;          // Update pointer if realloc was successful
            (*buffer)[buffer_size] = '\0'; // Null-terminate the buffer
            DEBUG_LOG("%s loaded into memory successfully %zu bytes", file_to_load, buffer_size);
        }
        else
        {
            free(*buffer);  // Important: free the original buffer to avoid memory leaks if realloc fails
            *buffer = NULL; // Set to NULL to avoid using a freed pointer
            DEBUG_LOG("Failed to reallocate memory for %s", file_to_load);
        }
    }
    else
    {
        DEBUG_LOG("Failed to load %s into memory", file_to_load);
    }
}
