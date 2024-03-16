#include "video_manager.h"

// The native resolution buffer
BITMAP *native_buffer;

// The scaled resolution
const int SCREEN_SCALE = 2;
const int SCALED_WIDTH = NIBBLE_WIDTH * SCREEN_SCALE;
const int SCALED_HEIGHT = NIBBLE_HEIGHT * SCREEN_SCALE;

const int FPS_DELAY = 1000; // Delay between FPS updates in milliseconds
uint32_t fpsLastTime = 0;
int frameCount = 0;
int fpsCurrent = 0;
int currentPaletteIndex = 0;

int video_init()
{
    set_color_depth(8);
    // Set the graphics mode to the scaled resolution
    if (set_gfx_mode(GFX_AUTODETECT, SCALED_WIDTH, SCALED_HEIGHT, 0, 0) != 0)
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

    currentPaletteIndex = manager->current_palette;
    // Setup the palette
    video_setup_palette();

    return 0;
}

void video_setup_palette()
{
    const Palette *palette = currentPalette(manager); // Ensure this function exists and is accessible
    PALETTE allegPalette;

    // Convert and set colors
    for (int i = 0; i < 4; i++)
    {
        allegPalette[i].r = palette->color[i][0] / 4;
        allegPalette[i].g = palette->color[i][1] / 4;
        allegPalette[i].b = palette->color[i][2] / 4;
        DEBUG_LOG("Color %d: %d, %d, %d", i, palette->color[i][0], palette->color[i][1], palette->color[i][2]);
    }

    // Fill the rest of the palette with black or any default color
    for (int i = 4; i < 256; i++)
    {
        allegPalette[i].r = allegPalette[i].g = allegPalette[i].b = 0;
    }

    // Apply the palette
    set_palette(allegPalette);
}

inline void video_update()
{
    // Display FPS on the back buffer if enabled
#if NIBBLE_DISPLAY_FPS
    frameCount++;
    int currentTime = time(NULL); // Or use Allegro's timing functions
    if (currentTime - fpsLastTime > FPS_DELAY)
    {
        fpsCurrent = frameCount * 1000 / (currentTime - fpsLastTime);
        fpsLastTime = currentTime;
        frameCount = 0;
    }
    nibble_api_draw_fps(fpsCurrent);
#endif

    // Perform drawing operations on native_buffer
    video_update_frame_allgero(); // Assuming this function draws the current frame

    if (currentPaletteIndex != manager->current_palette)
    {
        video_setup_palette();
        currentPaletteIndex = manager->current_palette;
    }

    // Update the display (not needed for every version of Allegro, but here for completeness)
    // vsync();

    // Now, blit the entire back buffer (native_buffer) to the screen in one operation
    // Since we are doing double buffering, stretch_blit is used here to scale the drawing
    // from native_buffer to the actual screen.
    acquire_screen(); // Make sure to lock the screen before drawing
    stretch_blit(native_buffer, screen, 0, 0, NIBBLE_WIDTH, NIBBLE_HEIGHT, 0, 0, SCREEN_W, SCREEN_H);
    release_screen(); // Unlock the screen after drawing
    show_video_bitmap(screen);
}

void video_update_frame_allgero()
{
    int pixelIndex = 0;
    // const Palette *palette = currentPalette(manager); // Ensure this function exists and is accessible
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
            // DEBUG_LOG("x: %d, y: %d, col: %d", x, y, col);
            _putpixel(native_buffer, x, y, col);
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
