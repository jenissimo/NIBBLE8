#include "sdl_adapter.h"
#include "audio_manager.h"
#include "video_manager.h"
#include "input_manager.h"

bool isFullscreen = false;
uint32_t next_time;

uint32_t time_left(void)
{
    uint32_t now;

    now = SDL_GetTicks();
    if (next_time <= now)
        return 0;
    else
        return next_time - now;
}

int nibble_sdl_init()
{
    if (video_init() != 0)
    {
        return 1;
    }

    audio_init();
    input_init();

    // init clipboard functions
    getClipboardText = SDL_GetClipboardText;
    setClipboardText = SDL_SetClipboardText;
    freeClipboardText = SDL_free;

    SDL_ShowCursor(SDL_DISABLE);

    DEBUG_LOG("SDL Initialized");
    return 0;
}

int nibble_sdl_update()
{
    if (input_update() == 0)
    {
        return 0; // Quit signal received
    }

    nibble_lua_call("_update");
    nibble_lua_call("_draw");

    video_update();

    return 1;
}

int nibble_sdl_quit()
{
    video_quit();
    audio_quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}

void goFullScreen()
{
    isFullscreen = !isFullscreen;
    video_toggle_fullscreen(isFullscreen);
}
