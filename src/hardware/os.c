#include "os.h"

char *nibble_api_ls(char *path)
{
    DIR *dir;
    int buf_size = 1024;
    char *buf = malloc(buf_size);

    if (path == NULL)
    {
        dir = opendir(".");
    }
    else
    {
        dir = opendir(path);
    }

    buf[0] = '\0';
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        int entry_len = strlen(entry->d_name);
        if (entry_len + 2 > buf_size - strlen(buf))
        {
            buf_size *= 2;
            buf = realloc(buf, buf_size);
        }

        strcat(buf, entry->d_name);
        strcat(buf, "\n");
    }

    closedir(dir);
    printf("LS: %s", buf);
    return buf;
}

int nibble_api_change_dir(char *path)
{
    return chdir(path);
}

char *nibble_api_read_file(char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        return NULL;
    }

    int buf_size = 1024;
    char *buf = malloc(buf_size);
    int buf_len = 0;
    int c;
    while ((c = fgetc(file)) != EOF)
    {
        if (buf_len + 1 > buf_size)
        {
            buf_size *= 2;
            buf = realloc(buf, buf_size);
        }

        buf[buf_len++] = c;
    }

    buf[buf_len] = '\0';
    fclose(file);
    return buf;
}

int nibble_api_write_file(char *path, char *data)
{
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        return 0;
    }

    fputs(data, file);
    fclose(file);
    return 1;
}

const uint8_t *nibble_api_get_clipboard_text()
{
    char *clipboardContent = getClipboardText();
    //printf("Clipboard: %s\n", clipboardContent);
    return clipboardContent;
}

int nibble_api_set_clipboard_text(const uint8_t *text)
{
    //printf("Setting clipboard: %s\n", text);
    return setClipboardText(text);
}

int nibble_api_save_code(char *code)
{
    userLuaCode = code;
    return 1;
}

ErrorCode nibble_api_load_cart(char *path)
{
    uint8_t *png_data;
    const char *luaCode;

    if (access(path, F_OK) != 0)
    {
        return ERROR_CART_NOT_FOUND;
    }

    //printf("Loading cart from %s\n", path);

    // TODO: make error check here also
    load_file_from_zip(path, "app.lua", (void **)&luaCode);
    load_file_from_zip(path, "spritesheet.png", (void **)&png_data);
    userLuaCode = luaCode;
    read_and_convert_png_from_buffer(memory.spriteSheetData, png_data, NIBBLE_SPRITE_SHEET_WIDTH, NIBBLE_SPRITE_SHEET_HEIGHT, &manager->palettes[0]);

    free(png_data);

    return ERROR_SUCCESS;
}

const char *nibble_api_get_code()
{
    return userLuaCode;
}

void nibble_api_run_code(char *code)
{
    userLuaCode = code;
    runLuaAppCode(userLuaCode);
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

int nibble_api_save_cart(char *path, char *luaCode)
{
    remove(path);

    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));

    userLuaCode = luaCode;

    if (!mz_zip_writer_init_file(&zip_archive, path, 0))
    {
        printf("Failed to initialize ZIP writer for %s\n", path);
        return 1;
    }

    // Read app.lua file
    size_t app_lua_size = strlen(userLuaCode);

    printf("app.lua size: %zu\n", app_lua_size);

    if (!userLuaCode)
    {
        printf("Failed to read app.lua\n");
        mz_zip_writer_end(&zip_archive);
        return 1;
    }

    // Add app.lua to the ZIP archive
    if (!mz_zip_writer_add_mem(&zip_archive, "app.lua", userLuaCode, app_lua_size, MZ_DEFAULT_COMPRESSION))
    {
        printf("Failed to add app.lua to cartridge.zip\n");
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
        printf("Failed to add spritesheet.png to cartridge.zip\n");
        // free(spritesheet_data);
        mz_zip_writer_end(&zip_archive);
        return 1;
    }
    // free(spritesheet_data);

    mz_zip_writer_finalize_archive(&zip_archive);
    mz_zip_writer_end(&zip_archive);

    printf("Successfully created %s\n", path);
    return 0;
}

void load_file_from_zip(const char *zip_filename, const char *file_to_load, void **buffer)
{
    mz_bool status;
    mz_zip_archive zip_archive;
    mz_zip_archive_file_stat file_stat;

    memset(&zip_archive, 0, sizeof(zip_archive));

    status = mz_zip_reader_init_file(&zip_archive, zip_filename, 0);
    if (!status)
    {
        printf("Failed to initialize zip reader for %s\n", zip_filename);
        return;
    }

    int file_index = mz_zip_reader_locate_file(&zip_archive, file_to_load, NULL, 0);
    if (file_index < 0)
    {
        printf("Failed to locate %s in %s\n", file_to_load, zip_filename);
        mz_zip_reader_end(&zip_archive);
        return;
    }

    if (!mz_zip_reader_file_stat(&zip_archive, 0, &file_stat))
    {
        printf("Zip file read error %s in %s\n", file_to_load, zip_filename);
        mz_zip_reader_end(&zip_archive);
        return;
    }

    *buffer = mz_zip_reader_extract_file_to_heap(&zip_archive, file_to_load, &file_stat.m_uncomp_size, 0);
    if (*buffer == NULL)
    {
        printf("Failed to load %s into memory\n", file_to_load);
    }
    else
    {
        printf("%s loaded into memory successfully %d bytes\n", file_to_load, file_stat.m_uncomp_size);
    }

    mz_zip_reader_end(&zip_archive);
}