#include "video_manager.h"

// The native resolution buffer
BITMAP *native_buffer;

// When set (by --bench), skip presenting to the host display (stretch_blit +
// show_video_bitmap). The headless benchmark only cares about the CPU cost of
// drawing + the framebuffer unpack; the host present is both irrelevant there
// and misbehaves under DOSBox-X when frames are pushed without 30 FPS pacing.
int nibble_bench_no_present = 0;

// The scaled resolution
const int SCREEN_SCALE = 2;
const int SCALED_WIDTH = NIBBLE_WIDTH * SCREEN_SCALE;
const int SCALED_HEIGHT = NIBBLE_HEIGHT * SCREEN_SCALE;

const int FPS_DELAY = 1000; // Delay between FPS updates in milliseconds
uint32_t fpsLastTime = 0;
int frameCount = 0;
int fpsCurrent = 0;

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

    // Setup the palette
    video_setup_palette();

    return 0;
}

void video_setup_palette()
{
    static PALETTE lastPalette;
    static int paletteCached = 0;
    PALETTE allegPalette;

    // Convert and set colors
    for (int i = 0; i < 4; i++)
    {
        if (memory.drawState.colorPalette.flip)
        {
            allegPalette[i].r = memory.drawState.colorPalette.r[3 - i] / 4;
            allegPalette[i].g = memory.drawState.colorPalette.g[3 - i] / 4;
            allegPalette[i].b = memory.drawState.colorPalette.b[3 - i] / 4;
        }
        else
        {
            allegPalette[i].r = memory.drawState.colorPalette.r[i] / 4;
            allegPalette[i].g = memory.drawState.colorPalette.g[i] / 4;
            allegPalette[i].b = memory.drawState.colorPalette.b[i] / 4;
        }
        //DEBUG_LOG("Color %d: %d, %d, %d", i, palette->color[i][0], palette->color[i][1], palette->color[i][2]);
    }

    // Fill the rest of the palette with black or any default color
    for (int i = 4; i < 256; i++)
    {
        allegPalette[i].r = allegPalette[i].g = allegPalette[i].b = 0;
    }

    // Pushing all 256 colors to the VGA DAC is expensive on slow hardware (a
    // 386), so only re-program the palette when it actually changed since the
    // last frame instead of every frame.
    if (paletteCached && memcmp(allegPalette, lastPalette, sizeof(PALETTE)) == 0)
    {
        return;
    }

    set_palette(allegPalette);
    memcpy(lastPalette, allegPalette, sizeof(PALETTE));
    paletteCached = 1;
}

inline void video_update()
{
    video_setup_palette();
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

    // In benchmark mode we measure CPU cost only; skip the host present.
    if (nibble_bench_no_present)
    {
        return;
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

// Lookup table: each 2bpp source byte (4 packed pixels) expands to the four
// 8bpp destination bytes, pre-packed as one little-endian uint32 so the unpack
// loop writes a whole dword per source byte instead of four byte stores with
// four shift/mask pairs. Built once on first use.
static uint32_t unpack_lut[256];
static int unpack_lut_ready = 0;

static void build_unpack_lut(void)
{
    for (int v = 0; v < 256; v++)
    {
        // little-endian: low byte -> row[x] (leftmost pixel = bits 7..6)
        unpack_lut[v] = ((uint32_t)((v >> 6) & 0x03)) |
                        ((uint32_t)((v >> 4) & 0x03) << 8) |
                        ((uint32_t)((v >> 2) & 0x03) << 16) |
                        ((uint32_t)((v)      & 0x03) << 24);
    }
    unpack_lut_ready = 1;
}

void video_update_frame_allgero()
{
    // Unpack the 2-bit packed framebuffer into the 8bpp native_buffer.
    // Hot path: runs every frame for all 19200 pixels (it is the fixed per-frame
    // floor under every cart). WBUF: write one 32-bit word (4 pixels) per source
    // byte via unpack_lut instead of four byte stores. Rows are NIBBLE_WIDTH (160,
    // a multiple of 4) wide so each dest row is exactly NIBBLE_WIDTH/4 dwords.
    const uint8_t *src = memory.screenData;
    int i = 0;

    if (!unpack_lut_ready)
        build_unpack_lut();

    acquire_bitmap(native_buffer);

    for (int y = 0; y < NIBBLE_HEIGHT; y++)
    {
        uint32_t *row = (uint32_t *)native_buffer->line[y];
        for (int x = 0; x < NIBBLE_WIDTH / 4; x++)
        {
            row[x] = unpack_lut[src[i++]];
        }
    }

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
