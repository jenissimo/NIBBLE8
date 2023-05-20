#ifndef nibble_lua_h
#define nibble_lua_h

#include "../nibble8.h"
#include "../hardware/video.h"
#include "../hardware/audio.h"
#include "../hardware/input.h"
#include "../hardware/os.h"
#include "../hardware/utils.h"
#include "../utils/error_handling.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "luajit.h"

lua_State* currentVM;
lua_State *lua;
lua_State *app;

void initLua(void);
void initCoreAPI(void);
void destroyLua(void);
int luaTraceback(lua_State *L);
int executeLuaCode(const char *code);
int loadLuaFile(const char* filename);
void runLuaAppFile(const char *filename);
void runLuaAppCode(const char *code);
void closeLuaApp(void);
void registerFunction(const char* name, lua_CFunction func);

static char* printString(lua_State* lua, int index);
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
static int l_mid(lua_State *L);
static int l_line(lua_State *L);
static int l_rect(lua_State *L);
static int l_rectfill(lua_State *L);
static int l_key(lua_State *L);
static int l_keyp(lua_State *L);
static int l_btn(lua_State *L);
static int l_btnp(lua_State *L);
static int l_load_file(lua_State *L);
static int l_get_code(lua_State *L);
static int l_run_code(lua_State *L);
static int l_import_png(lua_State *L);
static int l_export_png(lua_State *L);
static int l_ls(lua_State *L);
static int l_get_clipboard_text(lua_State *L);
static int l_set_clipboard_text(lua_State *L);
static int l_read_file(lua_State *L);
static int l_load_cart(lua_State *L);
static int l_save_cart(lua_State *L);
static int l_change_dir(lua_State *L);
static int l_spr(lua_State *L);
static int l_sspr(lua_State *L);
static int l_sset(lua_State *L);
static int l_sget(lua_State *L);
static int l_mget(lua_State *L);
static int l_mset(lua_State *L);

// audio functions
static int l_note_on(lua_State *L);
static int l_note_off(lua_State *L);
static int l_update_synth(lua_State *L);
static int l_update_filter(lua_State *L);

void callLuaFunction(const char* name);
void callLuaKey(int key_code, int ctrl_pressed, int shift_pressed);
void callLuaKeyUp(int key_code, int ctrl_pressed, int shift_pressed);
void callLuaMousePress(int x, int y, int button);
void callLuaMouseRelease(int x, int y, int button);
void callLuaMouseMove(int x, int y);
#endif