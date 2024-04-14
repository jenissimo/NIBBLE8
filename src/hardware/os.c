#include "os.h"

GetClipboardTextFunc getClipboardText;
SetClipboardTextFunc setClipboardText;
FreeClipboardTextFunc freeClipboardText;

void nibble_api_reboot()
{
    DEBUG_LOG("Rebooting Nibble8");
    nibble_lua_destroy();
    nibble_lua_init();
    nibble_ram_clear();
    nibble_ram_init();
    nibble_reset_video();
}

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