#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
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
#include "utils/base64.h"
#include "sdl_adapter.h"
#include "input_manager.h"

int run = 1;
uint32_t now_time = 0;
uint32_t frame_time = 0;
uint32_t last_time = 0;
uint32_t targetFrameTimeMs = 1000 / NIBBLE_FPS;

void main_loop()
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
    else if (shutdownRequested)
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

int main(int argc, char *argv[])
{
    printf("Welcome to NIBBLE8!\n");
    fflush(stdout);

    srand(time(NULL)); // Initialization, should only be called once.

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
            isFullscreen = true;
        }
        else if (strcmp(argv[i], "--width") == 0)
        {
            if (i + 1 < argc)
            {
                windowWidth = atoi(argv[i + 1]);
            }
        }
        else if (strcmp(argv[i], "--height") == 0)
        {
            if (i + 1 < argc)
            {
                windowHeight = atoi(argv[i + 1]);
            }
        }
        else if (strcmp(argv[i], "--cart") == 0)
        {
            if (i + 1 < argc)
            {
                cartPath = argv[i + 1];
            }
        }
        else if (strcmp(argv[i], "--cartb64") == 0)
        {
            DEBUG_LOG("Loading cart from base64\n");
            if (i + 1 < argc)
            {
                cartBase64 = argv[i + 1];
            }
        }
        else if (strcmp(argv[i], "--player") == 0)
        {
            if (i + 1 < argc)
            {
                playerMode = (bool)atoi(argv[i + 1]);
            }
        }
    }

    if (argc > 0)
    {
        nibble_change_to_sandbox_directory(argv[0]);
    }

    nibble_ram_init();
    nibble_init_video();
    nibble_sdl_init();
    nibble_lua_init();
    next_time = SDL_GetTicks() + NIBBLE_FPS;

    DEBUG_LOG("NIBBLE8 started\n");
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, -1, 1);
#else
    while (run)
    {
        main_loop();
    }
#endif

    nibble_lua_destroy();
    nibble_destroy_video();
    nibble_ram_destroy();
    nibble_audio_destroy();
    debug_close();
    base64_cleanup();
    return nibble_sdl_quit();
}
