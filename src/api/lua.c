#include "lua.h"

lua_State *currentVM;
lua_State *lua;
lua_State *app;

void initLua()
{
    lua = luaL_newstate(); // open Lua
    if (!lua)
    {
        return; // Checks that Lua started up
    }
    currentVM = lua;

    initCoreAPI();
    // loadLuaFile("lib/utils.lua");
    // loadLuaFile("demos/text_editor.lua");
    loadLuaFile("os/os.lua");

    luaL_dostring(lua, "_init()");
}

void initCoreAPI()
{
    luaL_openlibs(currentVM); // load Lua libraries

    luaL_dostring(currentVM, "function _init() end");
    luaL_dostring(currentVM, "function _update() end");
    luaL_dostring(currentVM, "function _draw() end");
    luaL_dostring(currentVM, "function _key(key_code, ctrl_pressed, shift_pressed) end");
    luaL_dostring(currentVM, "function _keyup(key_code, ctrl_pressed, shift_pressed) end");
    luaL_dostring(currentVM, "function _mousep(x, y, button) end");
    luaL_dostring(currentVM, "function _mouser(x, y, button) end");
    luaL_dostring(currentVM, "function _mousem(x, y) end");

    luaL_dostring(currentVM, "function add(t,a) table.insert(t,a) end");
    luaL_dostring(currentVM, "function del(t,a) table.remove(t,a) end");
    luaL_dostring(currentVM, "function str(a) return tostring(a) end");
    luaL_dostring(currentVM, "function len(a) return string.len(a) end");
    luaL_dostring(currentVM, "function sub(str,str_start,str_end) return string.sub(str,str_start,str_end) end");
    registerFunction("print", l_print);
    registerFunction("split", l_split);
    registerFunction("trace", l_trace);

    // Graphics
    registerFunction("camera", l_camera);
    registerFunction("pal", l_pal);
    registerFunction("palt", l_palt);
    registerFunction("pget", l_pget);
    registerFunction("pset", l_pset);
    registerFunction("circ", l_circ);
    registerFunction("circfill", l_circfill);
    registerFunction("line", l_line);
    registerFunction("rect", l_rect);
    registerFunction("rectfill", l_rectfill);
    registerFunction("cls", l_cls);

    // Sprite functions
    registerFunction("spr", l_spr);
    registerFunction("sspr", l_sspr);
    registerFunction("sset", l_sset);
    registerFunction("sget", l_sget);
    registerFunction("fget", l_fget);
    registerFunction("fset", l_fset);

    // Map Functions
    registerFunction("map", l_map);
    registerFunction("mget", l_mget);
    registerFunction("mset", l_mset);

    // Utils
    registerFunction("rnd", l_rnd);
    registerFunction("chr", l_chr);
    registerFunction("time", l_time);
    registerFunction("t", l_time);

    // Math
    registerFunction("flr", l_flr);
    registerFunction("ceil", l_ceil);
    registerFunction("sin", l_sin);
    registerFunction("cos", l_cos);
    registerFunction("atan2", l_atan2);
    registerFunction("mid", l_mid);
    registerFunction("min", l_min);
    registerFunction("max", l_max);

    // OS
    registerFunction("key", l_key);
    registerFunction("keyp", l_keyp);
    registerFunction("btn", l_btn);
    registerFunction("btnp", l_btnp);
    registerFunction("get_clipboard_text", l_get_clipboard_text);
    registerFunction("set_clipboard_text", l_set_clipboard_text);
    registerFunction("reboot", l_reboot);
    registerFunction("get_code", l_get_code);
    registerFunction("run_code", l_run_code);
    registerFunction("load_file", l_load_file);
    registerFunction("read_file", l_read_file);
    registerFunction("ls", l_ls);
    registerFunction("cd", l_change_dir);
    registerFunction("load_cart", l_load_cart);
    registerFunction("save_cart", l_save_cart);
    registerFunction("import_png", l_import_png);
    registerFunction("export_png", l_export_png);
    registerFunction("import_lua", l_import_lua);
    registerFunction("export_lua", l_export_lua);

    // Audio
    registerFunction("note_on", l_note_on);
    registerFunction("note_off", l_note_off);
    registerFunction("update_synth", l_update_synth);
    registerFunction("update_filter", l_update_filter);
    registerFunction("set_note", l_set_note);

    // Load Custom Libs
    luaL_dostring(currentVM, "package.path = package.path .. \";lib/?.lua\" .. \";os/?.lua\"");
    luaL_dostring(currentVM, "KEYCODE = require(\"keys_constants\")");
    luaL_dostring(currentVM, "UTILS = require(\"utils\")");
}

static int l_print(lua_State *L)
{
    // Check argument count
    int nargs = lua_gettop(L);
    if (nargs < 1)
    {
        return luaL_error(L, "Expected at least 1 argument");
    }

    // Assume x = 0, y = 0, fg_color = NIBBLE_DEFAULT_COLOR, bg_color = NIBBLE_DEFAULT_BG_COLOR by default
    int16_t x = 0, y = 0;
    uint8_t fg_color = NIBBLE_DEFAULT_COLOR, bg_color = NIBBLE_DEFAULT_BG_COLOR;

    // Validate and get arguments
    const char *text = luaL_checkstring(L, 1);

    if (nargs >= 3)
    {
        x = (int16_t)lua_tonumber(L, 2);
        y = (int16_t)lua_tonumber(L, 3);
        if (nargs >= 4)
            fg_color = (int8_t)lua_tonumber(L, 4) % NIBBLE_PALETTE_SIZE;
        if (nargs >= 5)
            bg_color = (int8_t)lua_tonumber(L, 5) % NIBBLE_PALETTE_SIZE;
    }

    // Call the API function
    int size = nibble_api_print(text, x, y, fg_color, bg_color);
    lua_pushinteger(L, size);
    return 1;
}

static int l_split(lua_State *L)
{
    const char *s = luaL_checkstring(L, 1);
    const char *sep = luaL_checkstring(L, 2);
    const char *e;
    int i = 1;

    lua_newtable(L); /* result */

    /* repeat for each separator */
    while ((e = strchr(s, *sep)) != NULL)
    {
        lua_pushlstring(L, s, e - s); /* push substring */
        lua_rawseti(L, -2, i++);
        s = e + 1; /* skip separator */
    }

    /* push last substring */
    lua_pushstring(L, s);
    lua_rawseti(L, -2, i);

    return 1; /* return the table */
}

static int l_trace(lua_State *L)
{
    int numArgs = lua_gettop(L);

    if (numArgs >= 1)
    {
        for (int i = 1; i <= numArgs; i++)
        {
            const char *text = lua_tostring(L, i);
            nibble_api_trace(text ? text : "nil");
        }
        return 0;
    }

    return 0;
}

static int l_pset(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        int16_t x = (int16_t)lua_tonumber(L, 1);
        int16_t y = (int16_t)lua_tonumber(L, 2);
        uint8_t color = NIBBLE_DEFAULT_COLOR;

        if (top >= 3)
        {
            color = (uint8_t)lua_tonumber(L, 3) % NIBBLE_PALETTE_SIZE;
        }

        // DEBUG_LOG("pset(%d, %d, %d)\n", x, y, color);

        nibble_api_pset(x, y, color);

        return 0;
    }

    return 0;
}

static int l_pget(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        int16_t x = (int16_t)lua_tonumber(L, 1);
        int16_t y = (int16_t)lua_tonumber(L, 2);

        // DEBUG_LOG("pget(%d, %d)\n", x, y);

        uint8_t color = nibble_api_pget(x, y);

        lua_pushinteger(L, color);

        return 1;
    }

    return 0;
}

static int l_circ(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        int16_t x = (int16_t)lua_tonumber(L, 1);
        int16_t y = (int16_t)lua_tonumber(L, 2);
        int16_t radius = 4;
        uint8_t color = NIBBLE_DEFAULT_COLOR;

        if (top >= 3)
        {
            radius = (int16_t)lua_tonumber(L, 3);
        }

        if (top >= 4)
        {
            color = (uint8_t)lua_tonumber(L, 4) % NIBBLE_PALETTE_SIZE;
        }

        // DEBUG_LOG("circle(%d, %d, %d, %d)\n", x, y, radius, color);

        nibble_api_circ(x, y, radius, color);

        return 0;
    }

    return 0;
}

static int l_circfill(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        int16_t x = (int16_t)lua_tonumber(L, 1);
        int16_t y = (int16_t)lua_tonumber(L, 2);
        int16_t radius = 4;
        uint8_t color = NIBBLE_DEFAULT_COLOR;

        if (top >= 3)
        {
            radius = (int16_t)lua_tonumber(L, 3);
        }

        if (top >= 4)
        {
            color = (uint8_t)lua_tonumber(L, 4) % NIBBLE_PALETTE_SIZE;
        }

        // DEBUG_LOG("circlefill(%d, %d, %d, %d)\n", x, y, radius, color);

        nibble_api_circfill(x, y, radius, color);

        return 0;
    }

    return 0;
}

static int l_rect(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        int16_t x = (int16_t)lua_tonumber(L, 1);
        int16_t y = (int16_t)lua_tonumber(L, 2);
        int16_t width = 4;
        int16_t height = 4;
        uint8_t color = NIBBLE_DEFAULT_COLOR;

        if (top >= 3)
        {
            width = (int16_t)lua_tonumber(L, 3);
        }

        if (top >= 4)
        {
            height = (int16_t)lua_tonumber(L, 4);
        }

        if (top >= 5)
        {
            color = (uint8_t)lua_tonumber(L, 5) % NIBBLE_PALETTE_SIZE;
        }

        // DEBUG_LOG("rect(%d, %d, %d, %d, %d)\n", x, y, width, height, color);

        nibble_api_rect(x, y, width, height, color);

        return 0;
    }

    return 0;
}

static int l_rectfill(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        int16_t x1 = (int16_t)lua_tonumber(L, 1);
        int16_t y1 = (int16_t)lua_tonumber(L, 2);
        int16_t x2 = 4;
        int16_t y2 = 4;
        uint8_t color = NIBBLE_DEFAULT_COLOR;

        if (top >= 3)
        {
            x2 = (int16_t)lua_tonumber(L, 3);
        }

        if (top >= 4)
        {
            y2 = (int16_t)lua_tonumber(L, 4);
        }

        if (top >= 5)
        {
            color = (uint8_t)lua_tonumber(L, 5) % NIBBLE_PALETTE_SIZE;
        }

        // DEBUG_LOG("rectfill(%d, %d, %d, %d, %d)\n", x1, y1, x2, y2, color);

        nibble_api_rectfill(x1, y1, x2, y2, color);

        return 0;
    }

    return 0;
}

static int l_camera(lua_State *L)
{
    int top = lua_gettop(L);

    // Default camera positions if not provided
    int16_t x = 0;
    int16_t y = 0;
    int16_t prev_x, prev_y;

    if (top >= 1)
        x = (int16_t)lua_tointeger(L, 1);
    if (top >= 2)
        y = (int16_t)lua_tointeger(L, 2);

    set_and_get_camera(x, y, &prev_x, &prev_y);

    // Push the previous camera position onto the Lua stack
    lua_pushinteger(L, prev_x);
    lua_pushinteger(L, prev_y);
    return 2; // Number of return values
}

static int l_pal(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        uint8_t color0 = (uint8_t)lua_tonumber(L, 1) % NIBBLE_PALETTE_SIZE;
        uint8_t color1 = (uint8_t)lua_tonumber(L, 2) % NIBBLE_PALETTE_SIZE;
        bool pset = false;

        if (top >= 3)
        {
            pset = lua_toboolean(L, 3);
        }

        // DEBUG_LOG("pal(%d, %d, %d)\n", color0, color1, pset);

        nibble_api_pal(color0, color1, pset);

        return 0;
    }
    else if (top == 0)
    {
        nibble_api_pal_reset();

        return 0;
    }

    return 0;
}

static int l_palt(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        uint8_t color = (uint8_t)lua_tonumber(L, 1) % NIBBLE_PALETTE_SIZE;
        bool transparent = true;

        if (top >= 2)
        {
            transparent = lua_toboolean(L, 2);
        }

        // DEBUG_LOG("palt(%d, %d)\n", color, transparent);

        nibble_api_palt(color, transparent);

        return 0;
    }
    else if (top == 0)
    {
        nibble_api_palt_reset();

        return 0;
    }

    return 0;
}

static int l_cls(lua_State *L)
{
    int top = lua_gettop(L);

    uint8_t color = 0;

    if (top >= 1)
    {
        color = (uint8_t)lua_tonumber(L, 1) % NIBBLE_PALETTE_SIZE;
    }

    // DEBUG_LOG("cls(%d)\n", color);

    nibble_api_cls(color);

    return 0;
}

static int l_rnd(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        double x = lua_tonumber(L, 1);
        double result = nibble_api_rnd(x);

        lua_pushnumber(L, result);

        return 1;
    }

    return 0;
}

static int l_chr(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        int x = lua_tonumber(L, 1);
        char result[2] = "\0";
        result[0] = nibble_api_chr(x);

        // DEBUG_LOG("chr(%d) = %s\n", x, result);

        lua_pushlstring(L, result, 1);

        return 1;
    }

    return 0;
}

static int l_time(lua_State *L)
{
    int top = lua_gettop(L);

    double result = nibble_api_time();
    // DEBUG_LOG("time() = %f\n", result);
    lua_pushnumber(L, result);
    return 1;
}

static int l_flr(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        double x = lua_tonumber(L, 1);
        double result = nibble_api_flr(x);
        // DEBUG_LOG("flr(%f) = %f\n", x, result);
        lua_pushinteger(L, result);
        return 1;
    }

    return 0;
}

static int l_ceil(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        double x = lua_tonumber(L, 1);
        double result = nibble_api_ceil(x);
        // DEBUG_LOG("ceil(%f) = %f\n", x, result);
        lua_pushinteger(L, result);
        return 1;
    }

    return 0;
}

static int l_sub(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        const char *text = lua_tostring(L, 1);
        int start = lua_tonumber(L, 2) - 1; // lua is 1 based
        int end = -1;

        if (top >= 3)
        {
            end = lua_tonumber(L, 3); // lua is 1 based
        }
        char *result = nibble_api_sub(text, start, end);
        // DEBUG_LOG("sub(%s, %d, %d) = %s\n", text, start, end, result);

        lua_pushstring(L, result);

        free(result);

        return 1;
    }

    return 0;
}

static int l_line(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 4)
    {
        int16_t x1 = (int16_t)lua_tonumber(L, 1);
        int16_t y1 = (int16_t)lua_tonumber(L, 2);
        int16_t x2 = (int16_t)lua_tonumber(L, 3);
        int16_t y2 = (int16_t)lua_tonumber(L, 4);
        uint8_t color = NIBBLE_DEFAULT_COLOR;

        if (top >= 5)
        {
            color = (uint8_t)lua_tonumber(L, 5) % NIBBLE_PALETTE_SIZE;
        }

        // DEBUG_LOG("line(%d, %d, %d, %d, %d)\n", x1, y1, x2, y2, color);

        nibble_api_line(x1, y1, x2, y2, color);

        return 0;
    }

    return 0;
}

static int l_sin(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        double x = lua_tonumber(L, 1);
        double result = nibble_api_sin(x);
        // DEBUG_LOG("sin(%f) = %f\n", x, result);
        lua_pushnumber(L, result);
        return 1;
    }

    return 0;
}

static int l_cos(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        double x = lua_tonumber(L, 1);
        double result = nibble_api_cos(x);
        // DEBUG_LOG("cos(%f) = %f\n", x, result);
        lua_pushnumber(L, result);
        return 1;
    }

    return 0;
}

static int l_atan2(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        double x = lua_tonumber(L, 1);
        double y = lua_tonumber(L, 2);
        double result = nibble_api_atan2(x, y);
        // DEBUG_LOG("atan2(%f, %f) = %f\n", x, y, result);
        lua_pushnumber(L, result);
        return 1;
    }

    return 0;
}

static int l_min(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        double x = lua_tonumber(L, 1);
        double y = lua_tonumber(L, 2);
        double result = nibble_api_min(x, y);
        // DEBUG_LOG("min(%f, %f) = %f\n", x, y, result);
        lua_pushnumber(L, result);
        return 1;
    }

    return 0;
}

static int l_max(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        double x = lua_tonumber(L, 1);
        double y = lua_tonumber(L, 2);
        double result = nibble_api_max(x, y);
        // DEBUG_LOG("max(%f, %f) = %f\n", x, y, result);
        lua_pushnumber(L, result);
        return 1;
    }

    return 0;
}

static int l_mid(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 3)
    {
        double x = lua_tonumber(L, 1);
        double y = lua_tonumber(L, 2);
        double z = lua_tonumber(L, 3);
        double result = nibble_api_mid(x, y, z);
        // DEBUG_LOG("mid(%f, %f, %f) = %f\n", x, y, z, result);
        lua_pushnumber(L, result);
        return 1;
    }

    return 0;
}

static int l_btn(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        int i = lua_tonumber(L, 1);
        int result = nibble_api_btn(i);
        // DEBUG_LOG("btn(%d) = %d\n", i, result);
        lua_pushboolean(L, result > 0);
        // lua_pushnumber(L, result);
        return 1;
    }

    return 0;
}

static int l_btnp(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        int i = lua_tonumber(L, 1);
        int result = nibble_api_btnp(i);
        // DEBUG_LOG("btnp(%d) = %d\n", i, result);
        lua_pushboolean(L, result > 0);
        // lua_pushnumber(L, result);
        return 1;
    }

    return 0;
}

static int l_key(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        int x = lua_tonumber(L, 1);
        int result = nibble_api_key(x);
        // DEBUG_LOG("key(%d) = %d\n", x, result);
        // lua_pushnumber(L, result);
        lua_pushboolean(L, result > 0);
        return 1;
    }
    return 0;
}

static int l_keyp(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        int x = lua_tonumber(L, 1);
        int result = nibble_api_keyp(x);
        // DEBUG_LOG("keyp(%d) = %d\n", x, result);
        // lua_pushnumber(L, result);
        lua_pushboolean(L, result > 0);
        return 1;
    }
    return 0;
}

static int l_load_file(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        const char *filename = lua_tostring(L, 1);
        runLuaAppFile(filename);
        // DEBUG_LOG("load(%s) = %d
        // lua_pushnumber(L, result);
        return 1;
    }
    return 0;
}

static int l_reboot(lua_State *L)
{
    rebootRequested = true;
    return 0;
}

static int l_get_code(lua_State *L)
{
    int top = lua_gettop(L);

    lua_pushstring(L, userLuaCode);
    return 1;
}

static int l_run_code(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        const char *code = lua_tostring(L, 1);
        nibble_api_run_code(code);
        // DEBUG_LOG("run(%s) = %d
        // lua_pushnumber(L, result);
        return 0;
    }
    return 0;
}

static int l_import_png(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        const char *filename = lua_tostring(L, 1);
        nibble_api_import_png(filename);
        // DEBUG_LOG("import(%s) = %d
        // lua_pushnumber(L, result);
        return 1;
    }
    return 0;
}

static int l_export_png(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        const char *filename = lua_tostring(L, 1);
        nibble_api_export_png(filename);
        // DEBUG_LOG("export(%s) = %d
        // lua_pushnumber(L, result);
        return 1;
    }
    return 0;
}

static int l_import_lua(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        const char *filename = lua_tostring(L, 1);
        nibble_api_import_lua(filename);
        DEBUG_LOG("import(%s) = %s\n", filename, userLuaCode);
        lua_pushstring(L, userLuaCode);
        return 1;
    }
    return 0;
}

static int l_export_lua(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        const char *filename = lua_tostring(L, 1);
        nibble_api_export_lua(filename);
        // DEBUG_LOG("export(%s) = %d
        // lua_pushnumber(L, result);
        return 0;
    }
    return 0;
}

static int l_ls(lua_State *L)
{
    int top = lua_gettop(L);

    char *result;
    if (top >= 1)
    {
        char *path = lua_tostring(L, 1);
        result = nibble_api_ls(lua_tostring(L, 1));
        // DEBUG_LOG("ls(%s) = %s\n", path, result);
    }
    else
    {
        result = nibble_api_ls(NULL);
        // DEBUG_LOG("ls() = %s\n", result);
    }
    lua_pushstring(L, result);
    free(result);
    return 1;

    return 0;
}

static int l_spr(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 3)
    {
        int16_t n = (int16_t)lua_tonumber(L, 1);
        int16_t x = (int16_t)lua_tonumber(L, 2);
        int16_t y = (int16_t)lua_tonumber(L, 3);
        uint8_t flip_x = 0;
        uint8_t flip_y = 0;

        if (top >= 4)
        {
            flip_x = (uint8_t)lua_tonumber(L, 4);
        }

        if (top >= 5)
        {
            flip_y = (uint8_t)lua_tonumber(L, 5);
        }

        // DEBUG_LOG("spr(%d, %d, %d, %d, %d)\n", n, x, y, flip_x, flip_y);

        nibble_api_spr(n, x, y, flip_x, flip_y);

        return 0;
    }

    return 0;
}

static int l_sspr(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 6)
    {
        int16_t sx = (int16_t)lua_tonumber(L, 1);
        int16_t sy = (int16_t)lua_tonumber(L, 2);
        int16_t sw = (int16_t)lua_tonumber(L, 3);
        int16_t sh = (int16_t)lua_tonumber(L, 4);
        int16_t dx = (int16_t)lua_tonumber(L, 5);
        int16_t dy = (int16_t)lua_tonumber(L, 6);
        int16_t dw = sw;
        int16_t dh = sh;
        uint8_t flip_x = 0;
        uint8_t flip_y = 0;

        if (top >= 7)
        {
            dw = (int16_t)lua_tonumber(L, 7);
        }

        if (top >= 8)
        {
            dh = (int16_t)lua_tonumber(L, 8);
        }

        if (top >= 9)
        {
            flip_x = (uint8_t)lua_toboolean(L, 9);
        }

        if (top >= 10)
        {
            flip_y = (uint8_t)lua_toboolean(L, 10);
        }

        // DEBUG_LOG("sspr(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d)\n", sx, sy, sw, sh, dx, dy, dw, dh, flip_x, flip_y);

        nibble_api_sspr(sx, sy, sw, sh, dx, dy, dw, dh, flip_x, flip_y);

        return 0;
    }

    return 0;
}

static int l_sset(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 3)
    {
        int16_t x = (int16_t)lua_tonumber(L, 1);
        int16_t y = (int16_t)lua_tonumber(L, 2);
        uint8_t color = (uint8_t)lua_tonumber(L, 3) % NIBBLE_PALETTE_SIZE;
        // DEBUG_LOG("sset(%d, %d, %d)\n", x, y, color);
        nibble_api_sset(x, y, color);
        return 0;
    }

    return 0;
}

static int l_sget(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        int16_t x = (int16_t)lua_tonumber(L, 1);
        int16_t y = (int16_t)lua_tonumber(L, 2);
        uint8_t color = nibble_api_sget(x, y);
        // DEBUG_LOG("sget(%d, %d) = %d\n", x, y, color);
        lua_pushinteger(L, color);
        return 1;
    }

    return 0;
}

static int l_fget(lua_State *L)
{
    int nargs = lua_gettop(L);                 // Number of arguments
    int spriteIndex = luaL_checkinteger(L, 1); // Sprite index

    if (spriteIndex < 0 || spriteIndex >= NIBBLE_SPRITE_FLAG_SIZE)
    {
        luaL_error(L, "Sprite index out of bounds");
        return 0;
    }

    if (nargs == 1)
    {
        // If flag index is omitted, return a bit field of all flags
        lua_pushinteger(L, memory.spriteFlagsData[spriteIndex]);
    }
    else if (nargs == 2)
    {
        // If flag index is provided
        int flagIndex = luaL_checkinteger(L, 2);
        if (flagIndex < 0 || flagIndex > 7)
        {
            luaL_error(L, "Flag index must be between 0 and 7");
            return 0;
        }

        // Calculate if the specific flag is set
        bool isSet = (memory.spriteFlagsData[spriteIndex] & (1 << flagIndex)) != 0;
        lua_pushboolean(L, isSet);
    }
    else
    {
        luaL_error(L, "fget takes 1 or 2 arguments: sprite index and optional flag index");
        return 0;
    }

    return 1; // Number of return values
}

static int l_fset(lua_State *L)
{
    int nargs = lua_gettop(L); // Number of arguments

    int spriteIndex = luaL_checkinteger(L, 1); // Use the first argument as sprite index

    // Default initialization
    bool value = false;
    int flagIndex = -1; // Use -1 to indicate "all flags" scenario initially

    // Determine action based on argument count
    if (nargs == 2)
    {
        // If only two arguments, the second argument is the value to set/clear all flags
        value = lua_toboolean(L, 2);
        for (int i = 0; i < 8; ++i)
        {
            if (value)
            {
                memory.spriteFlagsData[spriteIndex] |= (1 << i);
            }
            else
            {
                memory.spriteFlagsData[spriteIndex] &= ~(1 << i);
            }
        }
    }
    else if (nargs == 3)
    {
        // If three arguments, the second is flag index and the third is the value
        flagIndex = luaL_checkinteger(L, 2); // Use the second argument as flag index
        value = lua_toboolean(L, 3);

        // Validate flagIndex range
        if (flagIndex < 0 || flagIndex > 7)
        {
            return luaL_error(L, "Flag index must be between 0 and 7");
        }

        // Set or clear the specific flag
        if (value)
        {
            memory.spriteFlagsData[spriteIndex] |= (1 << flagIndex);
        }
        else
        {
            memory.spriteFlagsData[spriteIndex] &= ~(1 << flagIndex);
        }
    }
    else
    {
        return luaL_error(L, "Incorrect number of arguments to fset");
    }

    return 0; // No return values
}

static int l_map(lua_State *L)
{
    // Check the number of arguments and set defaults
    int numArgs = lua_gettop(L);
    if (numArgs < 5)
    {
        luaL_error(L, "Not enough arguments to map(celx, cely, sx, sy, celw, celh, [layer])");
        return 0;
    }

    int celx = luaL_checkinteger(L, 1);
    int cely = luaL_checkinteger(L, 2);
    int sx = luaL_checkinteger(L, 3);
    int sy = luaL_checkinteger(L, 4);
    int celw = luaL_checkinteger(L, 5);
    int celh = luaL_checkinteger(L, 6);
    uint8_t layer = 0;

    if (numArgs >= 7)
    {
        layer = (uint8_t)luaL_checkinteger(L, 7);
    }

    nibble_api_map(celx, cely, sx, sy, celw, celh, layer);
    return 0;
}

static int l_mget(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        int16_t x = (int16_t)lua_tonumber(L, 1);
        int16_t y = (int16_t)lua_tonumber(L, 2);
        int16_t spriteIndex = nibble_api_mget(x, y);
        // DEBUG_LOG("mget(%d, %d) = %d\n", x, y, spriteIndex);
        lua_pushinteger(L, spriteIndex);
        return 1;
    }

    return 0;
}

static int l_mset(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 3)
    {
        int16_t x = (int16_t)lua_tonumber(L, 1);
        int16_t y = (int16_t)lua_tonumber(L, 2);
        int16_t spriteIndex = (uint8_t)lua_tonumber(L, 3);
        // DEBUG_LOG("mset(%d, %d, %d)\n", x, y, spriteIndex);
        nibble_api_mset(x, y, spriteIndex);
        return 0;
    }

    return 0;
}

static int l_get_clipboard_text(lua_State *L)
{
    int top = lua_gettop(L);

    char *result = nibble_api_get_clipboard_text();
    DEBUG_LOG("get_clipboard_text() = %s\n", result);
    lua_pushstring(L, result);
    freeClipboardText(result);
    return 1;
}

static int l_set_clipboard_text(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        const char *text = lua_tostring(L, 1);
        int result = nibble_api_set_clipboard_text(text);
        // DEBUG_LOG("set_clipboard_text(%s) = %d", text, result);
        lua_pushnumber(L, result);
        return 1;
    }
    return 0;
}

static int l_read_file(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        const char *filename = lua_tostring(L, 1);
        char *result = nibble_api_read_file(filename);
        // DEBUG_LOG("read(%s) = %s
        lua_pushstring(L, result);
        free(result);
        return 1;
    }
    return 0;
}

static int l_write_file(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        const char *filename = lua_tostring(L, 1);
        const char *text = lua_tostring(L, 2);
        int result = nibble_api_write_file(filename, text);
        // DEBUG_LOG("write(%s) = %d
        lua_pushnumber(L, result);
        return 1;
    }
    return 0;
}

static int l_load_cart(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        const char *filename = lua_tostring(L, 1);
        // const char *code = nibble_api_load_cart(filename);
        ErrorCode result = nibble_api_load_cart(filename);
        if (result != ERROR_SUCCESS)
        {
            // DEBUG_LOG("Houston, we have a problem: %s\n", get_error_text(result, filename));
            lua_pushnil(L);
            lua_pushstring(L, get_error_text(result, filename));
        }
        else
        {
            lua_pushstring(L, userLuaCode);
            lua_pushnil(L);
        }
        // DEBUG_LOG("load_cart(%s) = %d", filename, result);
        return 2;
    }
    return 0;
}

static int l_save_cart(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        const char *filename = lua_tostring(L, 1);
        const char *code = lua_tostring(L, 2);

        DEBUG_LOG("%s\n", code);

        int result = nibble_api_save_cart(filename, code);
        // DEBUG_LOG("save(%s) = %d\n", filename, result);
        lua_pushnumber(L, result);
        return 1;
    }
    return 0;
}

static int l_change_dir(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 1)
    {
        const char *filename = lua_tostring(L, 1);
        int result = nibble_api_change_dir(filename);
        // DEBUG_LOG("cd(%s) = %d
        lua_pushnumber(L, result);
        return 1;
    }
    return 0;
}

// audio functions
static int l_note_on(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 2)
    {
        uint8_t note = (int)lua_tonumber(L, 1);
        uint8_t octave = (int)lua_tonumber(L, 2);
        uint8_t instrument = (int)lua_tonumber(L, 3);
        nibble_api_note_on(note, octave, instrument);
        DEBUG_LOG("note_on(%d, %d, %d)\n", note, octave, instrument);
        return 0;
    }

    return 0;
}

static int l_note_off(lua_State *L)
{
    int top = lua_gettop(L);

    nibble_api_note_off();

    return 0;
}

static int l_set_note(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 6)
    {
        uint8_t sfx_index = (int)lua_tonumber(L, 1);
        uint8_t note_index = (int)lua_tonumber(L, 2);
        uint8_t pitch = (int)lua_tonumber(L, 3);
        uint8_t instrument = (int)lua_tonumber(L, 4);
        uint8_t volume = (int)lua_tonumber(L, 5);
        uint8_t effect = (int)lua_tonumber(L, 6);

        nibble_api_set_note(sfx_index, note_index, pitch, instrument, volume, effect);
        // DEBUG_LOG("set_note(%d, %d, %d, %d, %d, %d)\n", sfx_index, note_index, pitch, instrument, volume, effect);

        return 0;
    }
}

static int l_update_synth(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 6)
    {
        uint8_t osc = (int)lua_tonumber(L, 1);
        double attackTime = (double)lua_tonumber(L, 2);
        double decayTime = (double)lua_tonumber(L, 3);
        double sustainAmplitude = (double)lua_tonumber(L, 4);
        double releaseTime = (double)lua_tonumber(L, 5);
        double startAmplitude = (double)lua_tonumber(L, 6);
        nibble_api_update_synth(osc, attackTime, decayTime, sustainAmplitude, releaseTime, startAmplitude);
        return 0;
    }

    return 0;
}

static int l_update_filter(lua_State *L)
{
    int top = lua_gettop(L);

    if (top >= 3)
    {
        float cutoff = (float)lua_tonumber(L, 1);
        float resonance = (float)lua_tonumber(L, 2);
        int16_t mode = (int16_t)lua_tonumber(L, 3);
        DEBUG_LOG("update_filter(%f, %f, %d)\n", cutoff, resonance, mode);
        nibble_api_update_filter(cutoff, resonance, mode);
        return 0;
    }

    return 0;
}

int luaTraceback(lua_State *L)
{
    DEBUG_LOG("Entered luaTraceback");

    // Check if the error is a string
    if (lua_isstring(L, 1))
    {
        DEBUG_LOG("Error message: %s", lua_tostring(L, 1));
    }
    else
    {
        // Try to get more information about the error object
        if (lua_isnoneornil(L, 1))
        {
            DEBUG_LOG("Error is nil or none");
        }
        else
        {
            DEBUG_LOG("Error is of type: %s", luaL_typename(L, 1));
        }
    }

    // Generate a stack trace and append it to the error message
    luaL_traceback(L, L, lua_tostring(L, 1) ? lua_tostring(L, 1) : "unknown error", 1);
    DEBUG_LOG("Traceback: %s", lua_tostring(L, -1));
    return 1;
}

int executeLuaCode(const char *code)
{
    int load_status = luaL_loadstring(currentVM, code); // load Lua script

    if (load_status != 0)
    {
        DEBUG_LOG("Error loading Lua script: %s\n", lua_tostring(currentVM, -1));
        lua_pop(currentVM, 1); // Remove the error message from the stack
        return 2;
    }

    // Push the error handler (luaTraceback) function onto the stack
    lua_pushcfunction(currentVM, luaTraceback);
    // Move the error handler (traceback) function before the loaded Lua script
    lua_insert(currentVM, -2);

    int call_status = lua_pcall(currentVM, 0, LUA_MULTRET, -2);
    if (call_status != 0)
    {
        DEBUG_LOG("Error executing Lua script:\n%s\n", lua_tostring(currentVM, -1));
        lua_pop(currentVM, 1); // Remove the error message (with traceback) from the stack
        return 3;
    }

    return 0;
}

int loadLuaFile(const char *filename)
{
    int load_status = luaL_loadfile(currentVM, filename); // load Lua script

    if (load_status != 0)
    {
        DEBUG_LOG("Error loading Lua script: %s\n", lua_tostring(currentVM, -1));
        lua_pop(currentVM, 1); // Remove the error message from the stack
        // lua_close(currentVM);  // Close the Lua state
        return 2;
    }

    // Push the error handler (luaTraceback) function onto the stack
    lua_pushcfunction(currentVM, luaTraceback);
    // Move the error handler (traceback) function before the loaded Lua script
    lua_insert(currentVM, -2);

    int call_status = lua_pcall(currentVM, 0, LUA_MULTRET, -2);
    if (call_status != 0)
    {
        DEBUG_LOG("Error executing Lua script:\n%s\n", lua_tostring(currentVM, -1));
        lua_pop(currentVM, 1); // Remove the error message (with traceback) from the stack
        // lua_close(currentVM);  // Close the Lua state
        return 3;
    }

    return 0;
}

void runLuaAppFile(const char *filename)
{
    if (app != NULL)
    {
        lua_close(app);
    }

    app = luaL_newstate();
    currentVM = app;

    initCoreAPI();

    loadLuaFile(filename);

    luaL_dostring(currentVM, "_init()");
}

void runLuaAppCode(const char *code)
{
    if (app != NULL)
    {
        lua_close(app);
    }

    app = luaL_newstate();
    currentVM = app;

    initCoreAPI();

    executeLuaCode(code);

    luaL_dostring(currentVM, "_init()");
}

void closeLuaApp()
{
    DEBUG_LOG("Closing Lua App...\n");
    if (app != NULL)
    {
        lua_close(app);
        app = NULL;
        // reset draw state
        nibble_init_video();
    }
    currentVM = lua;
}

void registerFunction(const char *name, lua_CFunction func)
{
    // DEBUG_LOG("%s\n", name);
    lua_pushcfunction(currentVM, func);
    lua_setglobal(currentVM, name);
}

static char *printString(lua_State *L, int index)
{
    lua_getglobal(L, "tostring");
    lua_pushvalue(L, -1);
    lua_pushvalue(L, index);
    lua_call(L, 1, 1);

    const char *text = lua_tostring(L, -1);

    lua_pop(L, 2);

    return text;
}

void callLuaFunction(const char *name)
{
    if (currentVM == NULL)
    {
        return;
    }

    lua_getglobal(currentVM, name);

    if (!lua_isfunction(currentVM, -1))
    {
        DEBUG_LOG("%s not a function\n", name);
        lua_pop(currentVM, 1);
        return;
    }

    if (lua_pcall(currentVM, 0, 0, 0) != 0)
    {
        DEBUG_LOG("error running function `%s': %s\n", name, lua_tostring(currentVM, -1));
        lua_pop(currentVM, 1);
        return;
    }
}

void callLuaKey(int key_code, int ctrl_pressed, int shift_pressed)
{
    if (currentVM == NULL)
    {
        return;
    }

    lua_getglobal(currentVM, "_key");
    lua_pushinteger(currentVM, key_code);
    lua_pushboolean(currentVM, ctrl_pressed);
    lua_pushboolean(currentVM, shift_pressed);
    lua_pcall(currentVM, 3, 0, 0);
    // DEBUG_LOG("key(%d, %d, %d)\n", key_code, ctrl_pressed, shift_pressed);
}

void callLuaKeyUp(int key_code, int ctrl_pressed, int shift_pressed)
{
    if (currentVM == NULL)
    {
        return;
    }

    lua_getglobal(currentVM, "_keyup");
    lua_pushinteger(currentVM, key_code);
    lua_pushboolean(currentVM, ctrl_pressed);
    lua_pushboolean(currentVM, shift_pressed);
    lua_pcall(currentVM, 3, 0, 0);
    // DEBUG_LOG("keyup(%d, %d, %d)\n", key_code, ctrl_pressed, shift_pressed);
}

void callLuaMouseMove(int x, int y)
{
    if (currentVM == NULL)
    {
        return;
    }

    lua_getglobal(currentVM, "_mousem");
    lua_pushinteger(currentVM, x);
    lua_pushinteger(currentVM, y);
    lua_pcall(currentVM, 2, 0, 0);
    // DEBUG_LOG("mouse_move(%d, %d)\n", x, y);
}

void callLuaMousePress(int x, int y, int button)
{
    if (currentVM == NULL)
    {
        return;
    }

    lua_getglobal(currentVM, "_mousep");
    lua_pushinteger(currentVM, x);
    lua_pushinteger(currentVM, y);
    lua_pushinteger(currentVM, button);
    lua_pcall(currentVM, 3, 0, 0);
    // DEBUG_LOG("mouse_press(%d, %d, %d)\n", x, y, button);
}

void callLuaMouseRelease(int x, int y, int button)
{
    if (currentVM == NULL)
    {
        return;
    }

    lua_getglobal(currentVM, "_mouser");
    lua_pushinteger(currentVM, x);
    lua_pushinteger(currentVM, y);
    lua_pushinteger(currentVM, button);
    lua_pcall(currentVM, 3, 0, 0);
    // DEBUG_LOG("mouse_release(%d, %d, %d)\n", x, y, button);
}

void destroyLua()
{
    if (app != NULL)
    {
        lua_close(app);
        app = NULL;
    }

    if (lua != NULL)
    {
        lua_close(lua);
        lua = NULL;
    }

    currentVM = NULL;
}