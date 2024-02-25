#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "../../hardware/input.h"
#include "../../hardware/utils.h"
#include "../../debug/debug.h"
#include "video_manager.h"

int input_init();
int input_update();
void input_check_hotkey(int key);

void updateButtonState();
int nibble_get_custom_key(int key);
void nibble_clear_keys();
void nibble_allegro_save_lua_keys_constants();
void nibble_allegro_print_lua_key(FILE *f, char *name, int key);

static const int allegroToSDLScancode[KEY_MAX] = {
    [KEY_0] = 48,
    [KEY_1] = 49,
    [KEY_2] = 50,
    [KEY_3] = 51,
    [KEY_4] = 52,
    [KEY_5] = 53,
    [KEY_6] = 54,
    [KEY_7] = 55,
    [KEY_8] = 56,
    [KEY_9] = 57,
    [KEY_A] = 97,
    [KEY_B] = 98,
    [KEY_C] = 99,
    [KEY_D] = 100,
    [KEY_E] = 101,
    [KEY_F] = 102,
    [KEY_G] = 103,
    [KEY_H] = 104,
    [KEY_I] = 105,
    [KEY_J] = 106,
    [KEY_K] = 107,
    [KEY_L] = 108,
    [KEY_M] = 109,
    [KEY_N] = 110,
    [KEY_O] = 111,
    [KEY_P] = 112,
    [KEY_Q] = 113,
    [KEY_R] = 114,
    [KEY_S] = 115,
    [KEY_T] = 116,
    [KEY_U] = 117,
    [KEY_V] = 118,
    [KEY_W] = 119,
    [KEY_X] = 120,
    [KEY_Y] = 121,
    [KEY_Z] = 122,
    [KEY_MINUS] = 45,
    [KEY_EQUALS] = 61,
    [KEY_ENTER] = 13,
    [KEY_COLON] = 59,
    [KEY_QUOTE] = 39,
    [KEY_ESC] = 27,
    [KEY_BACKSPACE] = 8,
    [KEY_TAB] = 9,
    [KEY_SPACE] = 32,
    [KEY_TILDE] = 126,
    [KEY_BACKSLASH] = 92,
    [KEY_SLASH] = 47,
    [KEY_COMMA] = 44,
    [KEY_STOP] = 46,
    [KEY_DEL] = 127,
    [KEY_0_PAD] = 226,
    [KEY_1_PAD] = 217,
    [KEY_2_PAD] = 218,
    [KEY_3_PAD] = 219,
    [KEY_4_PAD] = 220,
    [KEY_5_PAD] = 221,
    [KEY_6_PAD] = 222,
    [KEY_7_PAD] = 223,
    [KEY_8_PAD] = 224,
    [KEY_9_PAD] = 225,
    [KEY_UP] = 210,
    [KEY_DOWN] = 209,
    [KEY_LEFT] = 208,
    [KEY_RIGHT] = 207,
    [KEY_HOME] = 202,
    [KEY_END] = 205,
    [KEY_PGUP] = 203,
    [KEY_PGDN] = 206,
    [KEY_F1] = 186,
    [KEY_F2] = 187,
    [KEY_F3] = 188,
    [KEY_F4] = 189,
    [KEY_F5] = 190,
    [KEY_F6] = 191,
    [KEY_F7] = 192,
    [KEY_F8] = 193,
    [KEY_F9] = 194,
    [KEY_F10] = 195,
    [KEY_F11] = 196,
    [KEY_F12] = 197,
    [KEY_RSHIFT] = 357,
    [KEY_LSHIFT] = 353,
    [KEY_RCONTROL] = 356,
    [KEY_LCONTROL] = 352,
    [KEY_ALTGR] = 358,
    [KEY_ALT] = 354
};

#endif // INPUT_MANAGER_H
