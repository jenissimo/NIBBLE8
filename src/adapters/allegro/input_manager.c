#include "input_manager.h"

#define INITIAL_DELAY 0.25 // Half a second before repeat starts
#define REPEAT_RATE 0.05   // Repeat event every 0.05 seconds (20 times per second)

// Global keyboard state from the previous frame for comparison
static int old_key[KEY_MAX];
static int old_mouse_b;
static int old_mouse_x = -1;
static int old_mouse_y = -1;

// Track the timestamp of the last press for each key
static double lastKeyPressTime[KEY_MAX];
// Track whether the key is in the repeat phase
static int inRepeatPhase[KEY_MAX];

int input_init()
{
    memset(old_key, 0, sizeof(old_key)); // Initialize old key state array
    memset(lastKeyPressTime, 0, sizeof(lastKeyPressTime));
    memset(inRepeatPhase, 0, sizeof(inRepeatPhase));

    return 0;
}

int input_update()
{
    double currentTime = nibble_api_time();
    bool inputUpdated = false;

    // Refresh the keyboard state
    if (keyboard_needs_poll())
    {
        poll_keyboard();
    }

    if (mouse_needs_poll())
    {
        poll_mouse();
    }

    int ctrl_pressed = key[KEY_LCONTROL] || key[KEY_RCONTROL];
    int shift_pressed = key[KEY_LSHIFT] || key[KEY_RSHIFT];

    nibble_clear_keys();

    for (int i = 0; i < KEY_MAX; i++)
    {
        int custom_key = allegroToSDLScancode[i]; // Map Allegro keycodes to your custom/Sdl keycodes

        // Key press logic
        if (key[i] && !old_key[i])
        {
            // Key was just pressed
            nibble_lua_call_key(custom_key, ctrl_pressed, shift_pressed);
            lastKeyPressTime[i] = currentTime; // Record the time of this key press
            inRepeatPhase[i] = 0;              // Reset the repeat phase
        }
        else if (key[i])
        {
            // Continuous press logic
            if ((!inRepeatPhase[i] && (currentTime - lastKeyPressTime[i] >= INITIAL_DELAY)) ||
                (inRepeatPhase[i] && (currentTime - lastKeyPressTime[i] >= REPEAT_RATE)))
            {
                nibble_lua_call_key(custom_key, ctrl_pressed, shift_pressed);
                lastKeyPressTime[i] = currentTime; // Update the time for this repeat
                inRepeatPhase[i] = 1;              // We are now in repeat phase
            }
        }
        else if (!key[i] && old_key[i])
        {
            // Key was released
            nibble_lua_call_key_up(custom_key, ctrl_pressed, shift_pressed);
            inRepeatPhase[i] = 0; // Reset repeat phase upon key release
            input_check_hotkey(i);
        }

        old_key[i] = key[i];                // Update old key state for next frame
        nibble_keymap[custom_key] = key[i]; // Update custom key state
    }

    if (mouse_b != old_mouse_b)
    {
        if ((mouse_b & 1) && !(old_mouse_b & 1))
        {
            nibble_lua_call_mouse_press(mouse_x / SCREEN_SCALE, mouse_y / SCREEN_SCALE, 1);
        }
        else if (!(mouse_b & 1) && (old_mouse_b & 1))
        {
            nibble_lua_call_mouse_release(mouse_x / SCREEN_SCALE, mouse_y / SCREEN_SCALE, 1);
        }
        else if ((mouse_b & 2) && !(old_mouse_b & 2))
        {
            nibble_lua_call_mouse_press(mouse_x / SCREEN_SCALE, mouse_y / SCREEN_SCALE, 2);
        }
        else if (!(mouse_b & 2) && (old_mouse_b & 2))
        {
            nibble_lua_call_mouse_release(mouse_x / SCREEN_SCALE, mouse_y / SCREEN_SCALE, 2);
        }
        old_mouse_b = mouse_b;
    }

    if (mouse_x != old_mouse_x || mouse_y != old_mouse_y)
    {
        nibble_lua_call_mouse_move(mouse_x / SCREEN_SCALE, mouse_y / SCREEN_SCALE);
        old_mouse_x = mouse_x;
        old_mouse_y = mouse_y;
    }

    updateButtonState();

    /*
    if (key[KEY_F12])
    {
        return 0; // Indicate that the application should quit
    }
    */

    return 1;
}

void input_check_hotkey(int key)
{
    switch (key)
    {
    case KEY_ESC:
        nibble_lua_close_app();
        break;
    case KEY_F7:
        prevPalette(manager);
        break;
    case KEY_F8:
        nextPalette(manager);
        break;
    case KEY_F4:
        //nibble_allegro_save_lua_keys_constants();
        break;
    default:
        break;
    }
}

void updateButtonState()
{
    buttonState.previous[BUTTON_UP] = buttonState.current[BUTTON_UP];
    buttonState.current[BUTTON_UP] = key[KEY_UP];

    buttonState.previous[BUTTON_DOWN] = buttonState.current[BUTTON_DOWN];
    buttonState.current[BUTTON_DOWN] = key[KEY_DOWN];

    buttonState.previous[BUTTON_LEFT] = buttonState.current[BUTTON_LEFT];
    buttonState.current[BUTTON_LEFT] = key[KEY_LEFT];

    buttonState.previous[BUTTON_RIGHT] = buttonState.current[BUTTON_RIGHT];
    buttonState.current[BUTTON_RIGHT] = key[KEY_RIGHT];

    buttonState.previous[BUTTON_START] = buttonState.current[BUTTON_START];
    buttonState.current[BUTTON_START] = key[KEY_ENTER];

    buttonState.previous[BUTTON_SELECT] = buttonState.current[BUTTON_SELECT];
    buttonState.current[BUTTON_SELECT] = key[KEY_LSHIFT];

    buttonState.previous[BUTTON_A] = buttonState.current[BUTTON_A];
    buttonState.current[BUTTON_A] = key[KEY_Z];

    buttonState.previous[BUTTON_B] = buttonState.current[BUTTON_B];
    buttonState.current[BUTTON_B] = key[KEY_X];
}

int nibble_get_custom_key(int key)
{
    if (key > 512)
    {
        return ((key << 16) >> 16) + 128;
    }

    return key;
}

void nibble_clear_keys()
{
    for (int i = 0; i < 512; i++)
    {
        // Skip shift keys
        // if (i == nibble_get_custom_key(KEY_LSHIFT) || i == nibble_get_custom_key(KEY_RSHIFT))
        //    continue;
        nibble_keymap[i] = 0;
        nibble_keymap_p[i] = 0;
    }
}

void nibble_allegro_save_lua_keys_constants()
{
    FILE *f = fopen("lib/keys_constants.lua", "w");
    if (f == NULL)
    {
        DEBUG_LOG("Error opening file!\n");
        exit(1);
    }

    fprintf(f, "local key_constant = {}\n");

    nibble_allegro_print_lua_key(f, "0", allegroToSDLScancode[KEY_0]);
    nibble_allegro_print_lua_key(f, "1", allegroToSDLScancode[KEY_1]);
    nibble_allegro_print_lua_key(f, "2", allegroToSDLScancode[KEY_2]);
    nibble_allegro_print_lua_key(f, "3", allegroToSDLScancode[KEY_3]);
    nibble_allegro_print_lua_key(f, "4", allegroToSDLScancode[KEY_4]);
    nibble_allegro_print_lua_key(f, "5", allegroToSDLScancode[KEY_5]);
    nibble_allegro_print_lua_key(f, "6", allegroToSDLScancode[KEY_6]);
    nibble_allegro_print_lua_key(f, "7", allegroToSDLScancode[KEY_7]);
    nibble_allegro_print_lua_key(f, "8", allegroToSDLScancode[KEY_8]);
    nibble_allegro_print_lua_key(f, "9", allegroToSDLScancode[KEY_9]);
    nibble_allegro_print_lua_key(f, "A", allegroToSDLScancode[KEY_A]);
    nibble_allegro_print_lua_key(f, "B", allegroToSDLScancode[KEY_B]);
    nibble_allegro_print_lua_key(f, "C", allegroToSDLScancode[KEY_C]);
    nibble_allegro_print_lua_key(f, "D", allegroToSDLScancode[KEY_D]);
    nibble_allegro_print_lua_key(f, "E", allegroToSDLScancode[KEY_E]);
    nibble_allegro_print_lua_key(f, "F", allegroToSDLScancode[KEY_F]);
    nibble_allegro_print_lua_key(f, "G", allegroToSDLScancode[KEY_G]);
    nibble_allegro_print_lua_key(f, "H", allegroToSDLScancode[KEY_H]);
    nibble_allegro_print_lua_key(f, "I", allegroToSDLScancode[KEY_I]);
    nibble_allegro_print_lua_key(f, "J", allegroToSDLScancode[KEY_J]);
    nibble_allegro_print_lua_key(f, "K", allegroToSDLScancode[KEY_K]);
    nibble_allegro_print_lua_key(f, "L", allegroToSDLScancode[KEY_L]);
    nibble_allegro_print_lua_key(f, "M", allegroToSDLScancode[KEY_M]);
    nibble_allegro_print_lua_key(f, "N", allegroToSDLScancode[KEY_N]);
    nibble_allegro_print_lua_key(f, "O", allegroToSDLScancode[KEY_O]);
    nibble_allegro_print_lua_key(f, "P", allegroToSDLScancode[KEY_P]);
    nibble_allegro_print_lua_key(f, "Q", allegroToSDLScancode[KEY_Q]);
    nibble_allegro_print_lua_key(f, "R", allegroToSDLScancode[KEY_R]);
    nibble_allegro_print_lua_key(f, "S", allegroToSDLScancode[KEY_S]);
    nibble_allegro_print_lua_key(f, "T", allegroToSDLScancode[KEY_T]);
    nibble_allegro_print_lua_key(f, "U", allegroToSDLScancode[KEY_U]);
    nibble_allegro_print_lua_key(f, "V", allegroToSDLScancode[KEY_V]);
    nibble_allegro_print_lua_key(f, "W", allegroToSDLScancode[KEY_W]);
    nibble_allegro_print_lua_key(f, "X", allegroToSDLScancode[KEY_X]);
    nibble_allegro_print_lua_key(f, "Y", allegroToSDLScancode[KEY_Y]);
    nibble_allegro_print_lua_key(f, "Z", allegroToSDLScancode[KEY_Z]);
    nibble_allegro_print_lua_key(f, "RETURN", allegroToSDLScancode[KEY_ENTER]);
    nibble_allegro_print_lua_key(f, "ESCAPE", allegroToSDLScancode[KEY_ESC]);
    nibble_allegro_print_lua_key(f, "BACKSPACE", allegroToSDLScancode[KEY_BACKSPACE]);
    nibble_allegro_print_lua_key(f, "TAB", allegroToSDLScancode[KEY_TAB]);
    nibble_allegro_print_lua_key(f, "SPACE", allegroToSDLScancode[KEY_SPACE]);
    // nibble_allegro_print_lua_key(f, "EXCLAIM", KEY_EXCLAIM);
    // nibble_allegro_print_lua_key(f, "QUOTEDBL", KEY_QUOTEDBL);
    // nibble_allegro_print_lua_key(f, "HASH", KEY_HASH);
    // nibble_allegro_print_lua_key(f, "PERCENT", KEY_PERCENT);
    // nibble_allegro_print_lua_key(f, "DOLLAR", KEY_DOLLAR);
    // nibble_allegro_print_lua_key(f, "AMPERSAND", KEY_AMPERSAND);
    // nibble_allegro_print_lua_key(f, "QUOTE", KEY_QUOTE);
    // nibble_allegro_print_lua_key(f, "LEFTPAREN", KEY_LEFTPAREN);
    // nibble_allegro_print_lua_key(f, "RIGHTPAREN", KEY_RIGHTPAREN);
    // nibble_allegro_print_lua_key(f, "ASTERISK", KEY_ASTERISK);
    // nibble_allegro_print_lua_key(f, "PLUS", KEY_PLUS);
    nibble_allegro_print_lua_key(f, "COMMA", allegroToSDLScancode[KEY_COMMA]);
    nibble_allegro_print_lua_key(f, "MINUS", allegroToSDLScancode[KEY_MINUS]);
    // nibble_allegro_print_lua_key(f, "PERIOD", KEY_PERIOD);
    // nibble_allegro_print_lua_key(f, "SLASH", KEY_SLASH);
    nibble_allegro_print_lua_key(f, "COLON", allegroToSDLScancode[KEY_COLON]);
    // nibble_allegro_print_lua_key(f, "SEMICOLON", KEY_SEMICOLON);
    // nibble_allegro_print_lua_key(f, "LESS", KEY_LESS);
    nibble_allegro_print_lua_key(f, "EQUALS", allegroToSDLScancode[KEY_EQUALS]);
    // nibble_allegro_print_lua_key(f, "GREATER", KEY_GREATER);
    // nibble_allegro_print_lua_key(f, "QUESTION", KEY_QUESTION);
    // nibble_allegro_print_lua_key(f, "AT", KEY_AT);
    nibble_allegro_print_lua_key(f, "LEFTBRACKET", allegroToSDLScancode[KEY_OPENBRACE]);
    // nibble_allegro_print_lua_key(f, "BACKSLASH", KEY_BACKSLASH);
    nibble_allegro_print_lua_key(f, "RIGHTBRACKET", allegroToSDLScancode[KEY_CLOSEBRACE]);
    // nibble_allegro_print_lua_key(f, "CARET", KEY_CARET);
    // nibble_allegro_print_lua_key(f, "UNDERSCORE", KEY_UNDERSCORE);
    // nibble_allegro_print_lua_key(f, "BACKQUOTE", KEY_BACKQUOTE);
    nibble_allegro_print_lua_key(f, "DELETE", allegroToSDLScancode[KEY_DEL]);
    nibble_allegro_print_lua_key(f, "TILDA", allegroToSDLScancode[KEY_TILDE]);
    nibble_allegro_print_lua_key(f, "KP_0", allegroToSDLScancode[KEY_0_PAD]);
    nibble_allegro_print_lua_key(f, "KP_1", allegroToSDLScancode[KEY_1_PAD]);
    nibble_allegro_print_lua_key(f, "KP_2", allegroToSDLScancode[KEY_2_PAD]);
    nibble_allegro_print_lua_key(f, "KP_3", allegroToSDLScancode[KEY_3_PAD]);
    nibble_allegro_print_lua_key(f, "KP_4", allegroToSDLScancode[KEY_4_PAD]);
    nibble_allegro_print_lua_key(f, "KP_5", allegroToSDLScancode[KEY_5_PAD]);
    nibble_allegro_print_lua_key(f, "KP_6", allegroToSDLScancode[KEY_6_PAD]);
    nibble_allegro_print_lua_key(f, "KP_7", allegroToSDLScancode[KEY_7_PAD]);
    nibble_allegro_print_lua_key(f, "KP_8", allegroToSDLScancode[KEY_8_PAD]);
    nibble_allegro_print_lua_key(f, "KP_9", allegroToSDLScancode[KEY_9_PAD]);
    // nibble_allegro_print_lua_key(f, "KP_PERIOD", KEY_KP_PERIOD);
    // nibble_allegro_print_lua_key(f, "KP_DIVIDE", KEY_KP_DIVIDE);
    // nibble_allegro_print_lua_key(f, "KP_MULTIPLY", KEY_KP_MULTIPLY);
    // nibble_allegro_print_lua_key(f, "KP_MINUS", KEY_KP_MINUS);
    // nibble_allegro_print_lua_key(f, "KP_PLUS", KEY_KP_PLUS);
    // nibble_allegro_print_lua_key(f, "KP_ENTER", KEY_KP_ENTER);
    // nibble_allegro_print_lua_key(f, "KP_EQUALS", KEY_KP_EQUALS);
    nibble_allegro_print_lua_key(f, "UP", allegroToSDLScancode[KEY_UP]);
    nibble_allegro_print_lua_key(f, "DOWN", allegroToSDLScancode[KEY_DOWN]);
    nibble_allegro_print_lua_key(f, "RIGHT", allegroToSDLScancode[KEY_RIGHT]);
    nibble_allegro_print_lua_key(f, "LEFT", allegroToSDLScancode[KEY_LEFT]);
    nibble_allegro_print_lua_key(f, "INSERT", allegroToSDLScancode[KEY_INSERT]);
    nibble_allegro_print_lua_key(f, "HOME", allegroToSDLScancode[KEY_HOME]);
    nibble_allegro_print_lua_key(f, "END", allegroToSDLScancode[KEY_END]);
    nibble_allegro_print_lua_key(f, "PAGEUP", allegroToSDLScancode[KEY_PGUP]);
    nibble_allegro_print_lua_key(f, "PAGEDOWN", allegroToSDLScancode[KEY_PGDN]);
    nibble_allegro_print_lua_key(f, "F1", allegroToSDLScancode[KEY_F1]);
    nibble_allegro_print_lua_key(f, "F2", allegroToSDLScancode[KEY_F2]);
    nibble_allegro_print_lua_key(f, "F3", allegroToSDLScancode[KEY_F3]);
    nibble_allegro_print_lua_key(f, "F4", allegroToSDLScancode[KEY_F4]);
    nibble_allegro_print_lua_key(f, "F5", allegroToSDLScancode[KEY_F5]);
    nibble_allegro_print_lua_key(f, "F6", allegroToSDLScancode[KEY_F6]);
    nibble_allegro_print_lua_key(f, "F7", allegroToSDLScancode[KEY_F7]);
    nibble_allegro_print_lua_key(f, "F8", allegroToSDLScancode[KEY_F8]);
    nibble_allegro_print_lua_key(f, "F9", allegroToSDLScancode[KEY_F9]);
    nibble_allegro_print_lua_key(f, "F10", allegroToSDLScancode[KEY_F10]);
    nibble_allegro_print_lua_key(f, "F11", allegroToSDLScancode[KEY_F11]);
    nibble_allegro_print_lua_key(f, "F12", allegroToSDLScancode[KEY_F12]);
    // nibble_allegro_print_lua_key(f, "F13", KEY_F13);
    // nibble_allegro_print_lua_key(f, "F14", KEY_F14);
    // nibble_allegro_print_lua_key(f, "F15", KEY_F15);
    // nibble_allegro_print_lua_key(f, "NUMLOCK", KEY_NUMLOCKCLEAR);
    // nibble_allegro_print_lua_key(f, "CAPSLOCK", KEY_CAPSLOCK);
    // nibble_allegro_print_lua_key(f, "SCROLLOCK", KEY_SCROLLLOCK);
    nibble_allegro_print_lua_key(f, "RSHIFT", allegroToSDLScancode[KEY_RSHIFT]);
    nibble_allegro_print_lua_key(f, "LSHIFT", allegroToSDLScancode[KEY_LSHIFT]);
    nibble_allegro_print_lua_key(f, "RCTRL", allegroToSDLScancode[KEY_RCONTROL]);
    nibble_allegro_print_lua_key(f, "LCTRL", allegroToSDLScancode[KEY_LCONTROL]);
    nibble_allegro_print_lua_key(f, "RALT", allegroToSDLScancode[KEY_ALTGR]);
    nibble_allegro_print_lua_key(f, "LALT", allegroToSDLScancode[KEY_ALT]);
    // nibble_allegro_print_lua_key(f, "RMETA", KEY_RGUI);
    // nibble_allegro_print_lua_key(f, "LMETA", KEY_LGUI);
    // nibble_allegro_print_lua_key(f, "LSUPER", KEY_LGUI);
    // nibble_allegro_print_lua_key(f, "RSUPER", KEY_RGUI);
    // nibble_allegro_print_lua_key(f, "MODE", KEY_MODE);
    // nibble_allegro_print_lua_key(f, "HELP", KEY_HELP);
    nibble_allegro_print_lua_key(f, "PRINT", allegroToSDLScancode[KEY_PRTSCR]);
    // nibble_allegro_print_lua_key(f, "SYSREQ", KEY_SYSREQ);
    // nibble_allegro_print_lua_key(f, "MENU", KEY_MENU);
    // nibble_allegro_print_lua_key(f, "POWER", KEY_POWER);
    // nibble_allegro_print_lua_key(f, "UNDO", KEY_UNDO);

    fprintf(f, "return key_constant\n");

    fclose(f);

    DEBUG_LOG("Constants printed'\n");
}

void nibble_allegro_print_lua_key(FILE *f, char *name, int key)
{
    fprintf(f, "key_constant.KEY_%s = %d\n", name, nibble_get_custom_key(key));
}