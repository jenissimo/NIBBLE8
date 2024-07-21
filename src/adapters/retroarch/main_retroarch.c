#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include <time.h>
#include <unistd.h> // for usleep

#include "nibble8.h"
#include "hardware/os.h"
#include "hardware/ram.h"
#include "hardware/video.h"
#include "hardware/utils.h"
#include "api/lua.h"
#include "rom.h"
#include <stdio.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#endif
#include "libretro.h"

#define AUDIO_BUFFER_SIZE (NIBBLE_SAMPLES * 2) // Stereo
#define VIDEO_PIXELS NIBBLE_WIDTH *NIBBLE_HEIGHT
#define FRAME_DURATION (0.01 / NIBBLE_FPS)

static struct retro_log_callback logging;
static retro_log_printf_t log_cb;
static bool use_audio_cb;
char retro_base_directory[4096];
char retro_game_path[4096];
static bool can_dupe = false;
static uint32_t now_time = 0;
static uint32_t frame_time = 0;
static uint32_t last_time = 0;
static uint32_t targetFrameTimeMs = 1000 / NIBBLE_FPS;

struct retro_game_info *current_game = NULL;

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
    (void)level;
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
}

static retro_environment_t environ_cb;

void retro_init(void)
{
    DEBUG_LOG("retro_init");

    const char *dir = NULL;
    log_cb(RETRO_LOG_INFO, "NIBBLE8 Init!\n");

    if (nibble_init_rom(ROM, ROM_SIZE) > 0)
    {
        log_cb(RETRO_LOG_ERROR, "ROM Loading Error\n");
        return;
    }

    nibble_ram_init();
    nibble_init_video();
    nibble_audio_init(NIBBLE_SAMPLERATE, NULL, 0);
    nibble_lua_init();

    if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir)
    {
        snprintf(retro_base_directory, sizeof(retro_base_directory), "%s", dir);
    }

    environ_cb(RETRO_ENVIRONMENT_GET_CAN_DUPE, &can_dupe);
}

void retro_deinit(void)
{
    nibble_lua_destroy();
    nibble_destroy_video();
    nibble_ram_destroy();
    nibble_audio_destroy();
}

unsigned retro_api_version(void)
{
    return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
    log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
}

void retro_get_system_info(struct retro_system_info *info)
{
    memset(info, 0, sizeof(*info));
    info->library_name = "NIBBLE8";
    info->library_version = "0.1";
    info->need_fullpath = true;
    info->valid_extensions = "n8";
}

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

void retro_get_system_av_info(struct retro_system_av_info *info)
{
    DEBUG_LOG("=== retro_get_system_av_info ===");
    memset(info, 0, sizeof(*info));
    info->timing.fps = 60.0f;
    info->timing.sample_rate = NIBBLE_SAMPLERATE;

    info->geometry.base_width = NIBBLE_WIDTH;
    info->geometry.base_height = NIBBLE_HEIGHT;
    info->geometry.max_width = NIBBLE_WIDTH;
    info->geometry.max_height = NIBBLE_HEIGHT;
    info->geometry.aspect_ratio = (float)NIBBLE_WIDTH / (float)NIBBLE_HEIGHT;
}

void retro_set_environment(retro_environment_t cb)
{
    environ_cb = cb;

    if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
        log_cb = logging.log;
    else
        log_cb = fallback_log;

    static const struct retro_controller_description controllers[] = {
        {"Nintendo DS", RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0)},
    };

    static const struct retro_controller_info ports[] = {
        {controllers, 1},
        {NULL, 0},
    };

    cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void *)ports);
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
    audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
    audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
    input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
    input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
    video_cb = cb;
}

static unsigned phase;

void retro_reset(void)
{
    nibble_api_reboot();
}

static void update_input(void)
{
    // Poll the input state
    input_poll_cb();

    // Update the button states
    buttonState.previous[BUTTON_UP] = buttonState.current[BUTTON_UP];
    buttonState.current[BUTTON_UP] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);

    buttonState.previous[BUTTON_DOWN] = buttonState.current[BUTTON_DOWN];
    buttonState.current[BUTTON_DOWN] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);

    buttonState.previous[BUTTON_LEFT] = buttonState.current[BUTTON_LEFT];
    buttonState.current[BUTTON_LEFT] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);

    buttonState.previous[BUTTON_RIGHT] = buttonState.current[BUTTON_RIGHT];
    buttonState.current[BUTTON_RIGHT] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);

    buttonState.previous[BUTTON_START] = buttonState.current[BUTTON_START];
    buttonState.current[BUTTON_START] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);

    buttonState.previous[BUTTON_SELECT] = buttonState.current[BUTTON_SELECT];
    buttonState.current[BUTTON_SELECT] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);

    buttonState.previous[BUTTON_A] = buttonState.current[BUTTON_A];
    buttonState.current[BUTTON_A] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);

    buttonState.previous[BUTTON_B] = buttonState.current[BUTTON_B];
    buttonState.current[BUTTON_B] = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
}

static void check_variables(void)
{
}

static void audio_callback(void)
{
    static uint8_t audio_buffer[AUDIO_BUFFER_SIZE];
    nibble_audio_update(audio_buffer, AUDIO_BUFFER_SIZE);

    // Convert 8-bit unsigned samples to 16-bit signed samples
    int16_t samples[NIBBLE_SAMPLES * 2]; // Stereo
    for (int i = 0; i < NIBBLE_SAMPLES * 2; i++)
    {
        samples[i] = (int16_t)((audio_buffer[i] - 128) << 8);
    }

    audio_batch_cb(samples, NIBBLE_SAMPLES);
}

static void audio_set_state(bool enable)
{
    (void)enable;
}

void retro_run(void)
{
    now_time = (uint32_t)clock() * 1000 / CLOCKS_PER_SEC; // Convert clock() to milliseconds
    frame_time = now_time - last_time;
    last_time = now_time;

    if (frame_time < targetFrameTimeMs)
    {
        uint32_t msToSleep = targetFrameTimeMs - frame_time;
        usleep(msToSleep * 1000); // usleep takes microseconds, so multiply by 1000
        last_time += msToSleep;
    }

    update_input();

    nibble_lua_call("_update");
    nibble_lua_call("_draw");

    update_frame();
    video_cb(frame, NIBBLE_WIDTH, NIBBLE_HEIGHT, NIBBLE_WIDTH * sizeof(uint32_t));

    bool updated = false;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
    {
        check_variables();
    }

    nibble_frame_count++;
}

bool retro_load_game(const struct retro_game_info *info)
{
    DEBUG_LOG("retro_load_game");

    struct retro_input_descriptor desc[] = {
        {0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT, "Left"},
        {0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP, "Up"},
        {0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN, "Down"},
        {0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right"},
        {0},
    };

    current_game = info;

    environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
    {
        log_cb(RETRO_LOG_INFO, "XRGB8888 is not supported.\n");
        return false;
    }

    snprintf(retro_game_path, sizeof(retro_game_path), "%s", info->path);
    struct retro_audio_callback audio_cb = {audio_callback, audio_set_state};
    use_audio_cb = environ_cb(RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK, &audio_cb);
    cartPath = retro_game_path;

    //DEBUG_LOG("Load cart at %s", cartPath);

    check_variables();

    (void)info;

    nibble_api_reboot();

    return true;
}

void retro_unload_game(void)
{
}

unsigned retro_get_region(void)
{
    return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
    return false;
}

size_t retro_serialize_size(void)
{
    return 0;
}

bool retro_serialize(void *data_, size_t size)
{
    return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
    return false;
}

void *retro_get_memory_data(unsigned id)
{
    (void)id;
    return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
    (void)id;
    return 0;
}

void retro_cheat_reset(void)
{
}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
    (void)index;
    (void)enabled;
    (void)code;
}