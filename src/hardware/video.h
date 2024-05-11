#ifndef nibble_video_h
#define nibble_video_h

#include "nibble8.h"
#include "config_manager.h"
#include "debug/debug.h"
#include "ram.h"
#include "os.h"
#ifdef DOS
#include <allegro.h>
#else
#include <stdint.h>
#endif
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

extern uint32_t *frame;
extern uint8_t *nibble_font;
extern bool frame_dirty;
static const uint8_t fullByteColors[4] = {0b00000000, 0b01010101, 0b10101010, 0b11111111};
extern bool transparencyCache[4];

void nibble_init_video(void);
void nibble_reset_video(void);
void nibble_destroy_video(void);
void nibble_load_font(void);
void nibble_load_palettes();
void print_char(int charIndex);
void nibble_api_cls(uint8_t col);
void nibble_api_cpal(uint8_t color, uint8_t r, uint8_t g, uint8_t b);
void nibble_api_pal(uint8_t c0, uint8_t c1, uint8_t p);
void nibble_api_pal_reset();
void nibble_api_palt(uint8_t col, bool t);
void nibble_api_palt_reset();
void nibble_api_circ(int16_t x, int16_t y, int16_t r, uint8_t col);
void nibble_api_circfill(int16_t x, int16_t y, int16_t r, uint8_t col);
void nibble_api_pset(int16_t x, int16_t y, uint8_t col);
uint8_t nibble_api_pget(int16_t x, int16_t y);
void nibble_api_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t col);
void nibble_api_rectfill(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t col);
void nibble_api_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t col);
void nibble_api_hline(int16_t x1, int16_t x2, int16_t y, uint8_t color);
int nibble_api_print(char *text, int16_t x, int16_t y, uint8_t fg_color, uint8_t bg_color);
void nibble_api_cursor(int16_t x, int16_t y);
void nibble_api_color(uint8_t col);
int nibble_print_parse_parameter(uint8_t parameter);
void nibble_api_sspr(int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh, bool flip_x, bool flip_y);
void nibble_api_sset(int16_t x, int16_t y, uint8_t col);
uint8_t nibble_api_sget(int16_t x, int16_t y);
void draw_char(int charIndex, int16_t x, int16_t y, uint8_t fgCol, uint8_t bgCol);
void nibble_api_spr(int16_t sprIndex, int16_t x, int16_t y, uint8_t flipX, uint8_t flipY);

// map functions
void nibble_api_map(int celx, int cely, int sx, int sy, int celw, int celh, uint8_t layer);
void nibble_api_mset(uint16_t x, uint16_t y, int16_t sprite_number);
int16_t nibble_api_mget(uint16_t x, uint16_t y);


uint16_t nibble_get_vram_byte_index(int16_t x, int16_t y, uint16_t width);
uint16_t nibble_get_vram_bitpair_index(int16_t x, int16_t y, uint16_t width);

void move_camera(int16_t dx, int16_t dy);
void set_camera_position(int16_t x, int16_t y);
void set_and_get_camera(int16_t x, int16_t y, int16_t *prev_x, int16_t *prev_y);

void set_pixel_from_sprite(int16_t x, int16_t y, uint8_t col);
bool is_color_transparent(uint8_t color);
void update_frame(void);
void nibble_video_reset_colors();

// debug
void nibble_api_draw_fps(int fps);
void printVRam(void);

#endif