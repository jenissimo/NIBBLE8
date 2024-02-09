#ifndef VIDEO_MANAGER_H
#define VIDEO_MANAGER_H

#include <SDL2/SDL.h>
#include "../../hardware/audio.h"
#include "../../hardware/video.h"
#include "../../hardware/palette_manager.h"
#include "../../hardware/input.h"
#include "../../hardware/ram.h"
#include "../../hardware/os.h"
#include "../../nibble8.h"
#include "../../api/lua.h"
#include "../../utils/png.h"

extern bool isFullscreen;
extern SDL_Window *window;
extern SDL_Rect viewport;

int video_init(void);
void video_update(void);
void video_quit(void);
void video_toggle_fullscreen(bool enable);
void updateAspectRatio(SDL_Renderer *renderer, int windowWidth, int windowHeight, int targetWidth, int targetHeight);
void make_screenshot(void);

#endif // VIDEO_MANAGER_H
