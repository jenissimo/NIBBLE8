#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <allegro.h>
#include "allegro_adapter.h"
#include "nibble8.h"
#include "debug/debug.h"
#include "hardware/os.h"
#include "hardware/ram.h"
#include "hardware/video.h"
#include "hardware/utils.h"
#include "api/lua.h"

int run = 1;
volatile int ticks = 0;

void timer_handler()
{
    ticks++;
}
END_OF_FUNCTION(timer_handler);

int check_param(char *param, int argc, char *argv[])
{
    // parse params
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], param) == 0)
        {
            return i;
        }
    }

    return -1;
}

void check_params(int argc, char *argv[])
{
    // parse params
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--cart") == 0)
        {
            if (i + 1 < argc)
            {
                lua_getglobal(currentVM, "loadAndPlayCart");
                lua_pushstring(currentVM, argv[i + 1]);
                lua_pcall(currentVM, 1, 0, 0);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    printf("Welcome to NIBBLE-8 for DOS!\n");

    if (check_param("--debug", argc, argv) > -1)
    {
        debug_init("nibble8.log");
        DEBUG_LOG("Debug mode enabled.\n");
    }

    fflush(stdout);

    srand(time(NULL)); // Initialization, should only be called once.

    initRAM();
    nibble_init_video();
    initLua();
    nibble_allegro_init();
    check_params(argc, argv);

    // Lock the timer handler function
    LOCK_FUNCTION(timer_handler);
    LOCK_VARIABLE(ticks);

    // Install a timer to increment 'ticks' 30 times per second
    if (install_int_ex(timer_handler, BPS_TO_TIMER(NIBBLE_FPS)) == -1)
    {
        fprintf(stderr, "Failed to install timer interrupt.\n");
        return -1;
    }

    while (run)
    {
        // Wait until 'ticks' has been incremented by the timer handler
        while (ticks == 0)
        {
            rest(1); // Sleep briefly to reduce CPU usage
        }

        // Acknowledge tick
        while (ticks > 0)
        {
            int old_ticks = ticks;
            ticks--;
            if (old_ticks <= ticks)
            { // Ensure ticks is decreasing
                break;
            }
        }

        nibble_allegro_update();
        nibble_frame_count++;

        if (rebootRequested)
        {
            rebootRequested = false;
            nibble_api_reboot();
        }

        if (shutdownRequested)
        {
            shutdownRequested = false;
            run = 0;
        }
    }

    destroyLua();
    nibble_destroy_video();
    destroyRAM();
    debug_close();

    nibble_allegro_quit();
    return 0;
}
END_OF_MAIN(); // Required by some versions of Allegro
