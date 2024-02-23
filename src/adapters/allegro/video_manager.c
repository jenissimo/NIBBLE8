#include "video_manager.h"

// The native resolution buffer
BITMAP *native_buffer;

// The scaled resolution
const int SCREEN_SCALE = 2;
const int SCALED_WIDTH = NIBBLE_WIDTH * SCREEN_SCALE;
const int SCALED_HEIGHT = NIBBLE_HEIGHT * SCREEN_SCALE;

int video_init()
{
    allegro_init();
    set_color_depth(32);

    // Set the graphics mode to the scaled resolution
    if (set_gfx_mode(GFX_SAFE, 320, 240, 0, 0) != 0)
    {
        DEBUG_LOG("Video Initialization failed: %s", allegro_error);
        return -1;
    }

    // Create the off-screen buffer at native resolution
    native_buffer = create_bitmap(NIBBLE_WIDTH, NIBBLE_HEIGHT);
    if (!native_buffer)
    {
        DEBUG_LOG("Failed to create native resolution buffer.");
        return -1;
    }

    /* set the color palette */
    set_palette(desktop_palette);

    /* clear the screen to white */
    clear_to_color(screen, makecol(255, 255, 255));

    return 0;
}

void video_update()
{
    // Instead of SDL's texture and renderer, we draw directly to the buffer
    // Assume update_frame() draws the current frame into the buffer
    // video_update_frame_allgero();

    // Blit the buffer to the screen
    // Stretch the native buffer to the screen buffer
    // stretch_blit(native_buffer, screen, 0, 0, NIBBLE_WIDTH, NIBBLE_HEIGHT, 0, 0, SCALED_WIDTH, SCALED_HEIGHT);

    // Update the display
    vsync();
    // blit(screen, physical_screen, 0, 0, 0, 0, SCALED_WIDTH, SCALED_HEIGHT);

#if NIBBLE_DISPLAY_FPS
    frameCount++;
    int currentTime = time(NULL); // Or use Allegro's timing functions
    if (currentTime - fpsLastTime > FPS_DELAY)
    {
        fpsCurrent = frameCount * 1000 / (currentTime - fpsLastTime);
        fpsLastTime = currentTime;
        frameCount = 0;
        nibble_api_draw_fps(fpsCurrent); // Draw FPS on the screen if enabled
    }
#endif
}

void video_update_frame_allgero()
{
    int pixelIndex = 0;
    const Palette *palette = currentPalette(manager); // Ensure this function exists and is accessible
    uint8_t value;
    uint8_t col;
    int x, y;

    // Lock the bitmap before direct pixel access
    acquire_bitmap(native_buffer);

    for (int i = 0; i < NIBBLE_SCREEN_DATA_SIZE; i++)
    {
        value = memory.screenData[i];
        for (int bit = 7; bit >= 0; bit -= 2)
        {
            col = (((value >> bit) & 0x01) << 1) | ((value >> (bit - 1)) & 0x01);
            x = pixelIndex % NIBBLE_WIDTH;
            y = pixelIndex / NIBBLE_WIDTH;
            //_putpixel32(native_buffer, x, y, palette->argb[col]);
            putpixel(native_buffer, x, y, palette->argb[col]);
            pixelIndex++;
        }
    }

    // Unlock the bitmap after modifications
    release_bitmap(native_buffer);
}

void video_quit()
{
    if (native_buffer)
    {
        destroy_bitmap(native_buffer);
    }
    allegro_exit();
}

void make_screenshot()
{
    // TODO
}
