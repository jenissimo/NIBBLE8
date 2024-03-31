#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "nibble8.h"
#include "debug/debug.h"
#include "hardware/os.h"
#include "hardware/ram.h"
#include "hardware/video.h"
#include "hardware/utils.h"
#include "api/lua.h"
#include "sdl_adapter.h"
#include "utils/base64.h"

int run = 1;

int main(int argc, char *argv[])
{
    uint32_t now_time = 0;
    uint32_t frame_time = 0;
    uint32_t last_time = 0;
    uint32_t targetFrameTimeMs = 1000 / NIBBLE_FPS;

    printf("Welcome to NIBBLE8!\n");
    fflush(stdout);

    srand(time(NULL)); // Initialization, should only be called once.

    nibble_ram_init();
    init_video();
    nibble_lua_init();
    nibble_sdl_init();

    next_time = SDL_GetTicks() + NIBBLE_FPS;

    // parse params
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--debug") == 0)
        {
            debug_init("nibble8.log");
            DEBUG_LOG("Debug mode enabled.\n");
        }
        else if (strcmp(argv[i], "--fullscreen") == 0)
        {
            goFullScreen();
        }
        else if (strcmp(argv[i], "--cart") == 0)
        {
            if (i + 1 < argc)
            {
                lua_getglobal(currentVM, "loadAndPlayCart");
                lua_pushstring(currentVM, argv[i + 1]);
                lua_pcall(currentVM, 1, 0, 0);
            }
        }
    }

    while (run)
    {
        now_time = SDL_GetTicks();
        frame_time = now_time - last_time;
        last_time = now_time;

        if (rebootRequested)
        {
            rebootRequested = false;
            nibble_api_reboot();
        }

        if (nibble_sdl_update() == 0)
        {
            run = 0;
        }
        else
        {
            // sleep for remainder of time
            if (frame_time < targetFrameTimeMs)
            {
                uint32_t msToSleep = targetFrameTimeMs - frame_time;

                SDL_Delay(msToSleep);

                last_time += msToSleep;
            }
            next_time += NIBBLE_FPS;
        }
        nibble_frame_count++;
    }

    nibble_lua_destroy();
    destroy_video();
    destroyRAM();
    debug_close();
    base64_cleanup();
    return nibble_sdl_quit();
}
