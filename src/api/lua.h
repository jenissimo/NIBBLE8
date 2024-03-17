#ifndef nibble_lua_h
#define nibble_lua_h

#include "nibble8.h"
#include "debug/debug.h"
#include "hardware/video.h"
#include "hardware/audio.h"
#include "hardware/input.h"
#include "hardware/os.h"
#include "hardware/cart.h"
#include "hardware/utils.h"
#include "utils/error_handling.h"

#include "vendor/lua/lua.h"
#include "vendor/lua/lualib.h"
#include "vendor/lua/lauxlib.h"

extern lua_State* currentVM;
extern lua_State *lua;
extern lua_State *app;

void nibble_lua_init(void);
void nibble_lua_init_api(void);
void nibble_lua_destroy(void);
int nibble_lua_traceback(lua_State *L);
int nibble_lua_execute_code(const char *code);
int nibble_lua_load_file(const char* filename);
void nibble_lua_run_file(const char *filename);
void nibble_lua_run_code(const char *code);
void nibble_lua_close_app(void);
void nibble_lua_register_function(const char* name, lua_CFunction func);

static char* nibble_lua_print_string(lua_State* lua, int index);
static int l_camera(lua_State *L);
static int l_pal(lua_State *L);
static int l_palt(lua_State *L);
static int l_cls(lua_State *L);
static int l_print(lua_State *L);
static int l_split(lua_State *L);
static int l_trace(lua_State *L);
static int l_pset(lua_State *L);
static int l_pget(lua_State *L);
static int l_circ(lua_State *L);
static int l_circfill(lua_State *L);
static int l_rnd(lua_State *L);
static int l_chr(lua_State *L);
static int l_time(lua_State *L);
static int l_flr(lua_State *L);
static int l_ceil(lua_State *L);
static int l_sub(lua_State *L);
static int l_sin(lua_State *L);
static int l_cos(lua_State *L);
static int l_atan2(lua_State *L);
static int l_min(lua_State *L);
static int l_max(lua_State *L);
static int l_mid(lua_State *L);
static int l_line(lua_State *L);
static int l_rect(lua_State *L);
static int l_rectfill(lua_State *L);
static int l_key(lua_State *L);
static int l_keyp(lua_State *L);
static int l_btn(lua_State *L);
static int l_btnp(lua_State *L);
static int l_load_file(lua_State *L);
static int l_reboot(lua_State *L);
static int l_get_code(lua_State *L);
static int l_run_code(lua_State *L);
static int l_import_png(lua_State *L);
static int l_export_png(lua_State *L);
static int l_import_lua(lua_State *L);
static int l_export_lua(lua_State *L);
static int l_ls(lua_State *L);
static int l_get_clipboard_text(lua_State *L);
static int l_set_clipboard_text(lua_State *L);
static int l_read_file(lua_State *L);
static int l_load_cart(lua_State *L);
static int l_save_cart(lua_State *L);
static int l_change_dir(lua_State *L);

// Sprite functions
static int l_spr(lua_State *L);
static int l_sspr(lua_State *L);
static int l_sset(lua_State *L);
static int l_sget(lua_State *L);
static int l_fget(lua_State *L);
static int l_fset(lua_State *L);

// Map functions
static int l_map(lua_State *L);
static int l_mget(lua_State *L);
static int l_mset(lua_State *L);

// audio functions
static int l_note_on(lua_State *L);
static int l_note_off(lua_State *L);
static int l_update_synth(lua_State *L);
static int l_update_filter(lua_State *L);
static int l_set_note(lua_State *L);

void nibble_lua_call(const char* name);
void nibble_lua_call_key(int key_code, bool ctrl_pressed, bool shift_pressed);
void nibble_lua_call_key_up(int key_code, bool ctrl_pressed, bool shift_pressed);
void nibble_lua_call_mouse_press(int x, int y, int button);
void nibble_lua_call_mouse_release(int x, int y, int button);
void nibble_lua_call_mouse_move(int x, int y);
#endif