#include "allegro_adapter.h"
#include "audio_manager.h"
#include "video_manager.h"
#include "input_manager.h"

char* clipboardText = NULL;

void nibble_allegro_init()
{
    if (allegro_init() != 0)
    {
        DEBUG_LOG("Allegro Initialization failed.");
        return 1;
    }

    install_keyboard();
    install_mouse();
    install_timer();

    if (video_init() != 0)
    {
        return;
    }

    audio_init();
    input_init();

    getClipboardText = nibble_allegro_get_clipboard;
    setClipboardText = nibble_allegro_set_clipboard;
    freeClipboardText = nibble_allgero_free_clipboard;

    show_mouse(NULL);
    enable_hardware_cursor();

    DEBUG_LOG("Allegro Initialized");
}

inline void nibble_allegro_update()
{
    if (input_update() == 0)
    {
        shutdownRequested = true;
        return; // Quit signal received
    }

    // nibble_lua_call("_update");
    lua_getglobal(currentVM, "_update");
    lua_pcall(currentVM, 0, 0, 0);

    audio_update();

    // nibble_lua_call("_update");
    lua_getglobal(currentVM, "_draw");
    lua_pcall(currentVM, 0, 0, 0);

    if (frame_dirty)
    {
        video_update();
    }
}

char *nibble_allegro_get_clipboard()
{
    // Return the clipboard text
    return clipboardText;
}

// Default implementation of SetClipboardText
int nibble_allegro_set_clipboard(const char *text)
{
    // Free previously allocated clipboard text, if any
    free(clipboardText);
    // Allocate memory for new clipboard text
    clipboardText = strdup(text);
    if (clipboardText == NULL)
    {
        return -1; // Memory allocation failed
    }
    return NULL; // Success
}

// Default implementation of FreeClipboardText
void nibble_allgero_free_clipboard(void *ptr)
{
    free(ptr);
}

void nibble_allegro_quit()
{
    video_quit();
    audio_quit();
    allegro_exit();
}

END_OF_MAIN(); // Required for some versions of Allegro
