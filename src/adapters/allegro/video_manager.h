#ifndef VIDEO_MANAGER_H
#define VIDEO_MANAGER_H

#include <allegro.h>
#include "hardware/audio.h"
#include "hardware/video.h"
#include "hardware/palette_manager.h"
#include "hardware/input.h"
#include "hardware/ram.h"
#include "hardware/os.h"
#include "nibble8.h"
#include "api/lua.h"
#include "utils/png.h"

extern BITMAP *native_buffer;  // Main drawing buffer
extern const int SCREEN_SCALE;
extern const int SCALED_WIDTH;
extern const int SCALED_HEIGHT;

int video_init(void);
void video_setup_palette();
void video_update(void);
void video_update_frame_allgero();
void video_quit(void);
void make_screenshot(void);

#endif // VIDEO_MANAGER_H