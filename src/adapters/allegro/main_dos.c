#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <allegro.h>
#include "allegro_adapter.h"
#include "audio_manager.h"
#include "nibble8.h"
#include "debug/debug.h"
#include "hardware/os.h"
#include "hardware/ram.h"
#include "hardware/video.h"
#include "hardware/utils.h"
#include "api/lua.h"
#include "utils/base64.h"

int run = 1;
volatile int ticks = 0;
extern int nibble_bench_no_present; // defined in video_manager.c

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
    printf("Welcome to NIBBLE8 v.%d.%d.%d!\n", NIBBLE_MAJOR_VERSION, NIBBLE_MINOR_VERSION, NIBBLE_PATCH_VERSION);

    if (check_param("--debug", argc, argv) > -1)
    {
        debug_init("nibble8.log");
        DEBUG_LOG("Debug mode enabled.\n");
    }

    fflush(stdout);

    srand(time(NULL)); // Initialization, should only be called once.

    if (argc > 0)
    {
        nibble_change_to_sandbox_directory(argv[0]);
    }

    if (nibble_load_rom() > 0)
    {
        DEBUG_LOG("ROM Loading Error");
        return 1;
    }

    nibble_ram_init();
    nibble_init_video();
    nibble_allegro_init();
    nibble_lua_init();
    check_params(argc, argv);

    int benchIdx = check_param("--bench", argc, argv);
    if (benchIdx > -1)
    {
        // Headless benchmark mode: run a fixed number of frames as fast as the
        // (emulated) CPU allows, bypassing the 30 FPS timer cap, and measure the
        // elapsed CPU time with the DJGPP high-resolution uclock(). The result is
        // written to bench_result.txt in the sandbox directory so a host harness
        // can read it back after the program exits. Lower seconds = faster.
        int frames = 600;
        int framesIdx = check_param("--bench-frames", argc, argv);
        if (framesIdx > -1 && framesIdx + 1 < argc)
        {
            frames = atoi(argv[framesIdx + 1]);
            if (frames <= 0)
                frames = 600;
        }

        printf("BENCH: running %d frames (uncapped)...\n", frames);
        fflush(stdout);
        DEBUG_LOG("BENCH: entering loop, frames=%d", frames);

        nibble_bench_no_present = 1; // measure CPU cost only; skip host present

        uclock_t start = uclock();
        for (int f = 0; f < frames && run; f++)
        {
            nibble_allegro_update();
            nibble_frame_count++;
            shutdownRequested = false; // ignore cart/input exit requests during bench
        }
        uclock_t end = uclock();

        double secs = (double)(end - start) / (double)UCLOCKS_PER_SEC;
        double fps = secs > 0.0 ? (double)frames / secs : 0.0;
        DEBUG_LOG("BENCH: loop done, secs=%f fps=%f", secs, fps);

        // Framebuffer fingerprint: the bench scene is deterministic, so this
        // FNV-1a hash of the final packed framebuffer must stay identical across
        // any optimization that is meant to preserve output. Headless regression
        // gate (we can't eyeball the screen under Bochs).
        unsigned long fnv = 2166136261UL;
        for (int b = 0; b < NIBBLE_SCREEN_DATA_SIZE; b++)
            fnv = (fnv ^ memory.screenData[b]) * 16777619UL;
        DEBUG_LOG("BENCH: screen_checksum=%08lx", fnv & 0xffffffffUL);

#ifdef NIBBLE_AUDIO_PROFILE
        // Per-section audio breakdown (emulated ms/frame) when -DNIBBLE_AUDIO_PROFILE.
        extern unsigned long long g_aud_music, g_aud_sfx, g_aud_note, g_aud_mix;
        extern unsigned long g_aud_frames;
        double aps = (double)UCLOCKS_PER_SEC;
        unsigned long af = g_aud_frames ? g_aud_frames : 1;
        DEBUG_LOG("AUDIO: frames=%lu music=%.3f sfx=%.3f note=%.3f mix=%.3f ms/frame (emulated)",
                  g_aud_frames,
                  1000.0 * g_aud_music / aps / af,
                  1000.0 * g_aud_sfx / aps / af,
                  1000.0 * g_aud_note / aps / af,
                  1000.0 * g_aud_mix / aps / af);
#endif

        FILE *bfp = fopen("bench_result.txt", "w");
        DEBUG_LOG("BENCH: fopen bench_result.txt -> %p", (void *)bfp);
        if (bfp)
        {
            fprintf(bfp, "frames=%d\nseconds=%.6f\nfps=%.3f\n", frames, secs, fps);
            fclose(bfp);
        }
        printf("BENCH: frames=%d seconds=%.6f fps=%.3f\n", frames, secs, fps);
        fflush(stdout);
    }
    else
    {
        // Lock the timer handler function
        LOCK_FUNCTION(timer_handler);
        LOCK_VARIABLE(ticks);

        if (install_int_ex(timer_handler, BPS_TO_TIMER(NIBBLE_FPS)) == -1)
        {
            fprintf(stderr, "Failed to install timer interrupt.\n");
            return -1;
        }

        while (run)
        {
            // Wait until 'ticks' has been incremented by the timer handler.
            // Service audio here too: the 30 FPS draw tick is too coarse to keep
            // the stream fed, and when frames run fast we'd otherwise just sleep.
            // Polling audio_update() in the idle wait decouples audio refill from
            // the draw cadence (get_audio_stream_buffer() is a cheap no-op when
            // no buffer is due), so sound stays smooth independent of frame rate.
            while (ticks == 0)
            {
                audio_update();
                rest(1); // Sleep briefly to reduce CPU usage
            }

            // Acknowledge tick
            while (ticks > 0)
            {
                int old_ticks = ticks;
                ticks--;
                if (old_ticks <= ticks)
                {
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
    }

    nibble_lua_destroy();
    nibble_destroy_video();
    nibble_ram_destroy();
    nibble_audio_destroy();
    debug_close();
    base64_cleanup();

    nibble_allegro_quit();
    return 0;
}
END_OF_MAIN(); // Required by some versions of Allegro
