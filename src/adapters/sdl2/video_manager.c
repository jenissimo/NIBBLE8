#include "video_manager.h"

SDL_Window *window = NULL;
SDL_Surface *screen = NULL;
SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_Rect viewport;
#ifndef __EMSCRIPTEN__
SDL_Surface *iconSurface;
#endif

const int FPS_DELAY = 1000; // Delay between FPS updates in milliseconds
Uint32 fpsLastTime = 0;
int frameCount = 0;
int fpsCurrent = 0;

int windowWidth = 640;
int windowHeight = 480;

int video_init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0)
    {
        DEBUG_LOG("SDL Video Initialization failed: %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("NIBBLE8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              windowWidth, windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        DEBUG_LOG("SDL Window Creation failed: %s", SDL_GetError());
        return 1;
    }

    #ifndef __EMSCRIPTEN__
    iconSurface = IMG_Load("icon.png");
    SDL_SetWindowIcon(window, iconSurface);
    #endif

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        DEBUG_LOG("SDL Renderer Creation failed: %s", SDL_GetError());
        return 1;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, NIBBLE_WIDTH, NIBBLE_HEIGHT);
    SDL_RenderSetScale(renderer, windowWidth / NIBBLE_WIDTH, windowHeight / NIBBLE_HEIGHT);

    //DEBUG_LOG("SDL Initialized");

    screen = SDL_GetWindowSurface(window);

    return 0;
}

void video_update()
{
    SDL_UpdateTexture(texture, NULL, frame, NIBBLE_WIDTH * sizeof(uint32_t));
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    video_update_aspect_ratio(windowWidth, windowHeight, NIBBLE_WIDTH * NIBBLE_WINDOW_SCALE, NIBBLE_HEIGHT * NIBBLE_WINDOW_SCALE);

#if NIBBLE_DISPLAY_FPS
    nibble_api_draw_fps(fpsCurrent);
#endif

    update_frame();
    SDL_SetRenderDrawColor(renderer, memory.drawState.colorPalette.r[0], memory.drawState.colorPalette.g[0], memory.drawState.colorPalette.b[0], 255);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

#if NIBBLE_DISPLAY_FPS
    frameCount++;
    if (SDL_GetTicks() - fpsLastTime > FPS_DELAY)
    {
        fpsCurrent = frameCount * 1000 / (SDL_GetTicks() - fpsLastTime);
        fpsLastTime = SDL_GetTicks();
        frameCount = 0;
    }
#endif
}

void video_quit()
{
    #ifndef __EMSCRIPTEN__
    SDL_FreeSurface(iconSurface);
    #endif
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void video_toggle_fullscreen(bool fullscreen)
{
    if (fullscreen)
    {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else
    {
        SDL_SetWindowFullscreen(window, 0);
    }
}

void video_update_aspect_ratio(int windowWidth, int windowHeight, int targetWidth, int targetHeight)
{
    float scaleX = (float)windowWidth / targetWidth;
    float scaleY = (float)windowHeight / targetHeight;

    float scale = fminf(scaleX, scaleY);

    int viewportWidth = (int)(targetWidth * scale);
    int viewportHeight = (int)(targetHeight * scale);

    viewport.x = (windowWidth - viewportWidth) / 2;
    viewport.y = (windowHeight - viewportHeight) / 2;
    viewport.w = viewportWidth;
    viewport.h = viewportHeight;

    SDL_RenderSetViewport(renderer, &viewport);
    SDL_RenderSetScale(renderer, 1, 1);
}

void make_screenshot()
{
    write_indexed_png("screenshot.png", memory.screenData, NIBBLE_WIDTH, NIBBLE_HEIGHT, currentPalette(manager));
}