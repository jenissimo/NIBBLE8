#include "allegro_adapter.h"
#include "audio_manager.h"
#include "video_manager.h"
#include "input_manager.h"

bool isFullscreen = false;

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

    lua_getglobal(currentVM, "_update");
    lua_pcall(currentVM, 0, 0, 0);

    lua_getglobal(currentVM, "_draw");
    lua_pcall(currentVM, 0, 0, 0);
    
    //nibble_lua_call("_update");
    //nibble_lua_call("_draw");

    if (frame_dirty)
    {
        video_update();
    }
}

void nibble_allegro_quit()
{
    video_quit();
    audio_quit();
    allegro_exit();
}

END_OF_MAIN(); // Required for some versions of Allegro
