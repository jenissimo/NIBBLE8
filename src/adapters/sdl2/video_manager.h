#ifndef VIDEO_MANAGER_H
#define VIDEO_MANAGER_H

#include <SDL2/SDL.h>
#ifndef __EMSCRIPTEN__
#include <SDL2/SDL_image.h>
#endif
#include "hardware/audio.h"
#include "hardware/video.h"
#include "hardware/config_manager.h"
#include "hardware/input.h"
#include "hardware/ram.h"
#include "hardware/os.h"
#include "nibble8.h"
#include "api/lua.h"
#include "utils/png.h"

extern bool isFullscreen;
extern SDL_Window *window;
extern SDL_Rect viewport;
extern int windowWidth;
extern int windowHeight;

int video_init(void);
void video_update(void);
void video_quit(void);
void video_toggle_fullscreen(bool enable);
void video_update_aspect_ratio(int windowWidth, int windowHeight, int targetWidth, int targetHeight);
void make_screenshot(void);

#endif // VIDEO_MANAGER_H
