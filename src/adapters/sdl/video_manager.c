#include "video_manager.h"

SDL_Surface *screen = NULL;
const int FPS_DELAY = 1000; // Delay between FPS updates in milliseconds
Uint32 fpsLastTime = 0;
int frameCount = 0;
int fpsCurrent = 0;

int video_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        DEBUG_LOG("SDL Video Initialization failed: %s", SDL_GetError());
        return 1;
    }

    screen = SDL_SetVideoMode(NIBBLE_WIDTH * NIBBLE_WINDOW_SCALE, NIBBLE_HEIGHT * NIBBLE_WINDOW_SCALE, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        DEBUG_LOG("SDL SetVideoMode failed: %s", SDL_GetError());
        return 1;
    }

    DEBUG_LOG("SDL Initialized");
    return 0;
}

void video_update()
{
    // Assuming 'frame' is your source pixel data
    SDL_Surface *frameSurface = SDL_CreateRGBSurfaceFrom(frame, NIBBLE_WIDTH, NIBBLE_HEIGHT, 32, NIBBLE_WIDTH * sizeof(uint32_t), 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    // Use zoomSurface to scale the frameSurface to the screen size
    SDL_Surface *scaledSurface = zoomSurface(frameSurface, NIBBLE_WINDOW_SCALE, NIBBLE_WINDOW_SCALE, SMOOTHING_OFF);

#if NIBBLE_DISPLAY_FPS
    nibble_api_draw_fps(fpsCurrent); // You need to implement this function to draw directly on the surface
#endif

    if (scaledSurface)
    {
        // Blit the scaled surface to the screen at the desired position
        SDL_Rect destRect;
        destRect.x = 0; // Or wherever you want it
        destRect.y = 0;
        SDL_BlitSurface(scaledSurface, NULL, screen, &destRect);

        // Free the scaled surface after blitting
        SDL_FreeSurface(scaledSurface);
    }

    SDL_FreeSurface(frameSurface);

    update_frame(); // This function should update your frame buffer

#if NIBBLE_DISPLAY_FPS
    frameCount++;
    if (SDL_GetTicks() - fpsLastTime > FPS_DELAY)
    {
        fpsCurrent = frameCount * 1000 / (SDL_GetTicks() - fpsLastTime);
        fpsLastTime = SDL_GetTicks();
        frameCount = 0;
    }
#endif

    // Update the display
    SDL_Flip(screen);

    // Additional update logic (e.g., FPS counting) goes here
}

void video_quit()
{
    SDL_Quit();
}

void video_toggle_fullscreen(bool fullscreen)
{
    // SDL 1.2 does not have a direct way to toggle fullscreen like SDL 2.
    // You often have to reset the video mode using SDL_SetVideoMode with or without SDL_FULLSCREEN flag.
    if (fullscreen)
    {
        screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    }
    else
    {
        screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);
    }
}