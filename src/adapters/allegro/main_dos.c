#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <allegro.h>
#include "allegro_adapter.h"
#include "../../nibble8.h"
#include "../../debug/debug.h"
#include "../../hardware/os.h"
#include "../../hardware/ram.h"
#include "../../hardware/video.h"
#include "../../hardware/utils.h"
#include "../../api/lua.h"

int run = 1;

int main(int argc, char *argv[])
{
    uint32_t now_time = 0;
    uint32_t frame_time = 0;
    uint32_t last_time = 0;
    uint32_t targetFrameTimeMs = 1000 / NIBBLE_FPS;

    printf("Welcome to NIBBLE-8 for DOS!\n");
    fflush(stdout);

    srand(time(NULL)); // Initialization, should only be called once.

    allegro_init();
    install_timer();
    initRAM();
    init_video();
    initLua();
    nibble_allegro_init();

    // parse params
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--debug") == 0)
        {
            debug_init("nibble8.log");
            DEBUG_LOG("Debug mode enabled.\n");
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
        now_time = time(NULL); // Or use Allegro's timing functions
        frame_time = now_time - last_time;
        last_time = now_time;

        if (rebootRequested)
        {
            rebootRequested = false;
            nibble_api_reboot();
        }

        nibble_allegro_update();
        // sleep for remainder of time
        if (frame_time < targetFrameTimeMs)
        {
            uint32_t msToSleep = targetFrameTimeMs - frame_time;
            rest(msToSleep); // Allegro's sleep function
            last_time += msToSleep;
        }
        nibble_frame_count++;
    }

    destroyLua();
    destroy_video();
    destroyRAM();
    debug_close();

    nibble_allegro_quit();
    return 0;
}
END_OF_MAIN(); // Required by some versions of Allegro
