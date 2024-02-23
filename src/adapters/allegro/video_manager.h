#ifndef VIDEO_MANAGER_H
#define VIDEO_MANAGER_H

#include <allegro.h>
#include "../../hardware/audio.h"
#include "../../hardware/video.h"
#include "../../hardware/palette_manager.h"
#include "../../hardware/input.h"
#include "../../hardware/ram.h"
#include "../../hardware/os.h"
#include "../../nibble8.h"
#include "../../api/lua.h"
#include "../../utils/png.h"

extern BITMAP *native_buffer;  // Main drawing buffer

int video_init(void);
void video_update(void);
void video_update_frame_allgero();
void video_quit(void);
void make_screenshot(void);

#endif // VIDEO_MANAGER_H