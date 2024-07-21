#include "os.h"

GetClipboardTextFunc getClipboardText;
SetClipboardTextFunc setClipboardText;
FreeClipboardTextFunc freeClipboardText;

char nibble_exec_path[1024];
char nibble_sandbox_path[1024];

#if defined(_WIN32)
#include <windows.h>
#include <Shlwapi.h> // Include for PathCombine and other path functions
char *realpath(const char *path, char *resolved_path)
{
    DWORD length = GetFullPathName(path, 0, NULL, NULL);
    if (length == 0)
        return NULL; // Call failed

    char *buffer = resolved_path ? resolved_path : malloc(length);
    if (buffer == NULL)
        return NULL; // Allocation failed

    if (GetFullPathName(path, length, buffer, NULL) == 0)
    {
        if (!resolved_path)
            free(buffer); // Free buffer if it was malloc'ed
        return NULL;
    }

    return buffer;
}

void nibble_change_to_sandbox_directory(const char *path)
{
    if (!GetFullPathName(path, MAX_PATH, nibble_exec_path, NULL))
    {
        perror("Failed to resolve real path of the executable");
        exit(1);
    }

    // Use PathRemoveFileSpec to remove the file specification and get only the directory
    if (!PathRemoveFileSpec(nibble_exec_path))
    {
        perror("Failed to extract directory from path");
        exit(1);
    }

    // Construct the path to the sandbox directory
    if (!PathCombine(nibble_sandbox_path, nibble_exec_path, NIBBLE_SANDBOX_PATH))
    {
        perror("Failed to construct sandbox path");
        exit(1);
    }

    // Change to the sandbox directory
    if (_chdir(nibble_sandbox_path) != 0)
    {
        perror("Failed to change directory to sandbox");
        exit(1);
    }
    else
    {
        DEBUG_LOG("Changed directory to %s", nibble_sandbox_path);
    }
}

#else
void nibble_change_to_sandbox_directory(const char *path)
{
    char real_path[1024];

    if (realpath(path, real_path) == NULL)
    {
        perror("Failed to resolve real path of the executable");
        exit(1);
    }

    // Use dirname to extract the directory from the real path
    strncpy(nibble_exec_path, dirname(real_path), sizeof(nibble_exec_path));
    nibble_exec_path[sizeof(nibble_exec_path) - 1] = '\0'; // Ensure null termination

    // Construct the path to the sandbox directory
    snprintf(nibble_sandbox_path, sizeof(nibble_sandbox_path), "%s/%s", nibble_exec_path, NIBBLE_SANDBOX_PATH);

    // Change to the sandbox directory
    if (chdir(nibble_sandbox_path) != 0)
    {
        perror("Failed to change directory to sandbox");
        exit(1);
    }
    else
    {
        // DEBUG_LOG("Changed directory to %s", nibble_sandbox_path);
    }
}
#endif

int nibble_is_within_sandbox(const char *path)
{
#ifdef __EMSCRIPTEN__
    return 1;
#endif

#ifdef __LIBRETRO__
    return 1;
#endif

    char resolved_path[PATH_MAX];
    char sandbox_abs_path[PATH_MAX];

    // Ensure the sandbox path is resolved
    if (realpath(nibble_sandbox_path, sandbox_abs_path) == NULL)
    {
        perror("Failed to resolve sandbox path");
        return 0; // Failure, can't verify the sandbox path
    }

    // Attempt to resolve the input path to an absolute path
    if (realpath(path, resolved_path) == NULL)
    {
        perror("Failed to resolve input path");
        return 0;
    }

    // Check if the resolved path starts with the resolved sandbox path
    if (strncmp(sandbox_abs_path, resolved_path, strlen(sandbox_abs_path)) == 0)
    {
        return 1; // The path is within the sandbox
    }

    return 0; // The path is not within the sandbox
}

void nibble_api_reboot()
{
    // DEBUG_LOG("Rebooting Nibble8");
    nibble_ram_init();
    nibble_ram_clear();
    nibble_audio_init(NIBBLE_SAMPLERATE, NULL, 0);
    nibble_audio_reset();
    nibble_lua_destroy();
    nibble_reset_video();
    nibble_lua_init();

    nibble_frame_count = 0;
}

char *nibble_api_ls(char *path)
{
    DIR *dir;
    int buf_size = 1024;
    char *buf = malloc(buf_size);
    if (!buf)
    {
        perror("Failed to allocate memory");
        return NULL;
    }

    if (path == NULL)
    {
        dir = opendir(".");
    }
    else if (!nibble_is_within_sandbox(path))
    {
        free(buf);
        return NULL;
    }
    else
    {
        // Check if the path exists
        if (access(path, F_OK) != 0)
        {
            DEBUG_LOG("Error: Path does not exist");
            free(buf);
            return NULL;
        }
        dir = opendir(path);
    }

    if (!dir)
    {
        perror("Failed to open directory");
        free(buf);
        return NULL;
    }

    buf[0] = '\0';
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip hidden files and directories
        if (entry->d_name[0] == '.')
        {
            continue;
        }

        int entry_len = strlen(entry->d_name);
        if (entry_len + 2 > buf_size - strlen(buf))
        {
            buf_size *= 2;
            char *new_buf = realloc(buf, buf_size);
            if (!new_buf)
            {
                perror("Failed to reallocate memory");
                free(buf);
                closedir(dir);
                return NULL;
            }
            buf = new_buf;
        }

        strcat(buf, entry->d_name);
        strcat(buf, "\n");
    }

    closedir(dir);
    // DEBUG_LOG("LS: %s", buf);
    return buf;
}

int nibble_api_change_dir(char *path)
{
    if (!nibble_is_within_sandbox(path))
    {
        return 1;
    }
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
    // printf("Clipboard: %s\n", clipboardContent);
    return clipboardContent;
}

int nibble_api_set_clipboard_text(const uint8_t *text)
{
    if (setClipboardText(text) != NULL)
    {
        return -1; // Indicate failure
    }
    return 0; // Indicate success
}

int nibble_api_save_code(char *code)
{
    userLuaCode = code;
    return 1;
}

const char *nibble_api_get_code()
{
    return userLuaCode;
}

void nibble_api_run_code(uint8_t *code)
{
    if (code != NULL)
    {
        userLuaCode = code;
        nibble_lua_run_code(userLuaCode);
    }
}

int nibble_load_rom()
{
    char romPath[1024];
    snprintf(romPath, sizeof(romPath), "%s/rom.zip", nibble_exec_path);
    // DEBUG_LOG("Loading rom from: %s", romPath);

    // Allocate memory for the ROM archive
    rom = (mz_zip_archive *)malloc(sizeof(mz_zip_archive));
    if (!rom)
    {
        DEBUG_LOG("Failed to allocate memory for ROM archive\n");
        return 1; // Return error code
    }

    // Initialize the ROM archive
    memset(rom, 0, sizeof(mz_zip_archive));

    // Read the ROM archive into memory
    FILE *file = fopen(romPath, "rb");
    if (!file)
    {
        DEBUG_LOG("Failed to open ROM archive: %s", romPath);
        free(rom);
        return 1; // Return error code
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    romBuffer = (uint8_t *)malloc(fileSize);
    if (!romBuffer)
    {
        DEBUG_LOG("Failed to allocate memory for ZIP buffer\n");
        fclose(file);
        free(rom);
        return 1; // Return error code
    }

    if (fread(romBuffer, 1, fileSize, file) != fileSize)
    {
        DEBUG_LOG("Failed to read ROM archive: %s\n", romPath);
        fclose(file);
        free(romBuffer);
        free(rom);
        return 1; // Return error code
    }
    fclose(file);

    // Initialize the ROM archive from the buffer
    if (!mz_zip_reader_init_mem(rom, romBuffer, fileSize, 0))
    {
        DEBUG_LOG("Failed to initialize ROM archive from memory\n");
        free(romBuffer);
        free(rom);
        return 1; // Return error code
    }

    return 0; // Return success code
}

int nibble_init_rom(const unsigned char *rom_data, size_t rom_size)
{
    romBuffer = (uint8_t *)malloc(rom_size);
    if (!romBuffer)
    {
        fprintf(stderr, "Failed to allocate memory for ROM buffer\n");
        free(rom);
        return 1; // Return error code
    }
    memcpy(romBuffer, rom_data, rom_size);

    // Allocate memory for the ROM archive
    rom = (mz_zip_archive *)malloc(sizeof(mz_zip_archive));
    if (!rom)
    {
        fprintf(stderr, "Failed to allocate memory for ROM archive\n");
        return 1; // Return error code
    }

    // Initialize the ROM archive
    memset(rom, 0, sizeof(mz_zip_archive));

    // Initialize the ROM archive from the provided data
    if (!mz_zip_reader_init_mem(rom, rom_data, rom_size, 0))
    {
        fprintf(stderr, "Failed to initialize ROM archive from memory\n");
        free(rom);
        return 1; // Return error code
    }

    DEBUG_LOG("ROM Initialized succesfully");

    return 0; // Return success code
}