#include "input_manager.h"

int input_init()
{
    return 0;
}

int input_update()
{
    SDL_Event e;
    bool ctrl_pressed = false;
    bool shift_pressed = false;

    nibble_clear_keys();
    while (SDL_PollEvent(&e))
    {
        ctrl_pressed = ((e.key.keysym.mod & KMOD_GUI) != 0) || ((e.key.keysym.mod & KMOD_CTRL) != 0);
        shift_pressed = (e.key.keysym.mod & KMOD_SHIFT) != 0;

        if (e.type == SDL_QUIT)
        {
            return 0;
        }

        if (e.type == SDL_KEYDOWN)
        {
            nibble_keymap[nibble_get_custom_key(e.key.keysym.sym)] = 1;
            nibble_keymap_p[nibble_get_custom_key(e.key.keysym.sym)] = 1;

            nibble_lua_call_key(nibble_get_custom_key(e.key.keysym.sym), ctrl_pressed, shift_pressed);
            // DEBUG_LOG("Key Down: %d\n", nibble_get_custom_key(e.key.keysym.sym));
            // DEBUG_LOG("Key Scancode: %d\n", e.key.keysym.scancode);
            if (e.key.keysym.sym == SDLK_RETURN && (SDL_GetModState() & KMOD_ALT))
            {
                isFullscreen = !isFullscreen;
                Uint32 flags = isFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
                SDL_SetWindowFullscreen(window, flags);
            }
        }

        if (e.type == SDL_KEYUP)
        {
            nibble_keymap[nibble_get_custom_key(e.key.keysym.sym)] = 0;
            nibble_lua_call_key_up(nibble_get_custom_key(e.key.keysym.sym), ctrl_pressed, shift_pressed);

            // DEBUG_LOG("Key Up: %d\n", nibble_get_custom_key(e.key.keysym.sym));
            // DEBUG_LOG("Key Scancode: %d\n", e.key.keysym.scancode);

            switch (e.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                if (!playerMode)
                {
                    nibble_lua_close_app();
                }
                break;
            case SDLK_F4:
                nibble_sdl_save_lua_keys_constants();
                break;
            case SDLK_F8:
                nibble_ram_dump();
                break;
            // case SDLK_F10:
            //     printVRam();
            //     break;
            case SDLK_F10:
                make_screenshot();
                break;
            case SDLK_F12:
                if (!playerMode)
                {
                    nibble_lua_destroy();
                    nibble_lua_init();
                }
                break;
            default:
                break;
            }
        }

        if (e.type == SDL_MOUSEMOTION)
        {
            SDL_Point mousePos = convertMouseCoordinates(e.motion.x, e.motion.y);
            nibble_lua_call_mouse_move(mousePos.x, mousePos.y);
        }

        if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            SDL_Point mousePos = convertMouseCoordinates(e.motion.x, e.motion.y);
            // DEBUG_LOG("Mouse Down at: %d, %d\n", mousePos.x, mousePos.y);
            nibble_lua_call_mouse_press(mousePos.x, mousePos.y, e.button.button);
        }

        if (e.type == SDL_MOUSEBUTTONUP)
        {
            SDL_Point mousePos = convertMouseCoordinates(e.motion.x, e.motion.y);
            nibble_lua_call_mouse_release(mousePos.x, mousePos.y, e.button.button);
        }
    }

    updateButtonState();

    return 1;
}

SDL_Point convertMouseCoordinates(int mouseX, int mouseY)
{
    int convertedX = (mouseX - viewport.x) / (float)viewport.w * NIBBLE_WIDTH;
    int convertedY = (mouseY - viewport.y) / (float)viewport.h * NIBBLE_HEIGHT;

    SDL_Point convertedPoint = {convertedX, convertedY};
    return convertedPoint;
}

void updateButtonState()
{
    buttonState.previous[BUTTON_UP] = buttonState.current[BUTTON_UP];
    buttonState.current[BUTTON_UP] = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_UP];

    buttonState.previous[BUTTON_DOWN] = buttonState.current[BUTTON_DOWN];
    buttonState.current[BUTTON_DOWN] = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_DOWN];

    buttonState.previous[BUTTON_LEFT] = buttonState.current[BUTTON_LEFT];
    buttonState.current[BUTTON_LEFT] = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LEFT];

    buttonState.previous[BUTTON_RIGHT] = buttonState.current[BUTTON_RIGHT];
    buttonState.current[BUTTON_RIGHT] = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RIGHT];

    buttonState.previous[BUTTON_START] = buttonState.current[BUTTON_START];
    buttonState.current[BUTTON_START] = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RETURN];

    buttonState.previous[BUTTON_SELECT] = buttonState.current[BUTTON_SELECT];
    buttonState.current[BUTTON_SELECT] = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RSHIFT];

    buttonState.previous[BUTTON_A] = buttonState.current[BUTTON_A];
    buttonState.current[BUTTON_A] = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_Z];

    buttonState.previous[BUTTON_B] = buttonState.current[BUTTON_B];
    buttonState.current[BUTTON_B] = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_X];
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
        if (i == nibble_get_custom_key(SDLK_LSHIFT) || i == nibble_get_custom_key(SDLK_RSHIFT))
            continue;
        nibble_keymap[i] = 0;
        nibble_keymap_p[i] = 0;
    }
}

void nibble_sdl_save_lua_keys_constants()
{
    FILE *f = fopen("lib/keys_constants.lua", "w");
    if (f == NULL)
    {
        DEBUG_LOG("Error opening file!\n");
        exit(1);
    }

    fprintf(f, "local key_constant = {}\n");

    nibble_sdl_print_lua_key(f, "0", SDLK_0);
    nibble_sdl_print_lua_key(f, "1", SDLK_1);
    nibble_sdl_print_lua_key(f, "2", SDLK_2);
    nibble_sdl_print_lua_key(f, "3", SDLK_3);
    nibble_sdl_print_lua_key(f, "4", SDLK_4);
    nibble_sdl_print_lua_key(f, "5", SDLK_5);
    nibble_sdl_print_lua_key(f, "6", SDLK_6);
    nibble_sdl_print_lua_key(f, "7", SDLK_7);
    nibble_sdl_print_lua_key(f, "8", SDLK_8);
    nibble_sdl_print_lua_key(f, "9", SDLK_9);
    nibble_sdl_print_lua_key(f, "A", SDLK_a);
    nibble_sdl_print_lua_key(f, "B", SDLK_b);
    nibble_sdl_print_lua_key(f, "C", SDLK_c);
    nibble_sdl_print_lua_key(f, "D", SDLK_d);
    nibble_sdl_print_lua_key(f, "E", SDLK_e);
    nibble_sdl_print_lua_key(f, "F", SDLK_f);
    nibble_sdl_print_lua_key(f, "G", SDLK_g);
    nibble_sdl_print_lua_key(f, "H", SDLK_h);
    nibble_sdl_print_lua_key(f, "I", SDLK_i);
    nibble_sdl_print_lua_key(f, "J", SDLK_j);
    nibble_sdl_print_lua_key(f, "K", SDLK_k);
    nibble_sdl_print_lua_key(f, "L", SDLK_l);
    nibble_sdl_print_lua_key(f, "M", SDLK_m);
    nibble_sdl_print_lua_key(f, "N", SDLK_n);
    nibble_sdl_print_lua_key(f, "O", SDLK_o);
    nibble_sdl_print_lua_key(f, "P", SDLK_p);
    nibble_sdl_print_lua_key(f, "Q", SDLK_q);
    nibble_sdl_print_lua_key(f, "R", SDLK_r);
    nibble_sdl_print_lua_key(f, "S", SDLK_s);
    nibble_sdl_print_lua_key(f, "T", SDLK_t);
    nibble_sdl_print_lua_key(f, "U", SDLK_u);
    nibble_sdl_print_lua_key(f, "V", SDLK_v);
    nibble_sdl_print_lua_key(f, "W", SDLK_w);
    nibble_sdl_print_lua_key(f, "X", SDLK_x);
    nibble_sdl_print_lua_key(f, "Y", SDLK_y);
    nibble_sdl_print_lua_key(f, "Z", SDLK_z);
    nibble_sdl_print_lua_key(f, "RETURN", SDLK_RETURN);
    nibble_sdl_print_lua_key(f, "ESCAPE", SDLK_ESCAPE);
    nibble_sdl_print_lua_key(f, "BACKSPACE", SDLK_BACKSPACE);
    nibble_sdl_print_lua_key(f, "TAB", SDLK_TAB);
    nibble_sdl_print_lua_key(f, "SPACE", SDLK_SPACE);
    nibble_sdl_print_lua_key(f, "EXCLAIM", SDLK_EXCLAIM);
    nibble_sdl_print_lua_key(f, "QUOTEDBL", SDLK_QUOTEDBL);
    nibble_sdl_print_lua_key(f, "HASH", SDLK_HASH);
    nibble_sdl_print_lua_key(f, "PERCENT", SDLK_PERCENT);
    nibble_sdl_print_lua_key(f, "DOLLAR", SDLK_DOLLAR);
    nibble_sdl_print_lua_key(f, "AMPERSAND", SDLK_AMPERSAND);
    nibble_sdl_print_lua_key(f, "QUOTE", SDLK_QUOTE);
    nibble_sdl_print_lua_key(f, "LEFTPAREN", SDLK_LEFTPAREN);
    nibble_sdl_print_lua_key(f, "RIGHTPAREN", SDLK_RIGHTPAREN);
    nibble_sdl_print_lua_key(f, "ASTERISK", SDLK_ASTERISK);
    nibble_sdl_print_lua_key(f, "PLUS", SDLK_PLUS);
    nibble_sdl_print_lua_key(f, "COMMA", SDLK_COMMA);
    nibble_sdl_print_lua_key(f, "MINUS", SDLK_MINUS);
    nibble_sdl_print_lua_key(f, "PERIOD", SDLK_PERIOD);
    nibble_sdl_print_lua_key(f, "SLASH", SDLK_SLASH);
    nibble_sdl_print_lua_key(f, "COLON", SDLK_COLON);
    nibble_sdl_print_lua_key(f, "SEMICOLON", SDLK_SEMICOLON);
    nibble_sdl_print_lua_key(f, "LESS", SDLK_LESS);
    nibble_sdl_print_lua_key(f, "EQUALS", SDLK_EQUALS);
    nibble_sdl_print_lua_key(f, "GREATER", SDLK_GREATER);
    nibble_sdl_print_lua_key(f, "QUESTION", SDLK_QUESTION);
    nibble_sdl_print_lua_key(f, "AT", SDLK_AT);
    nibble_sdl_print_lua_key(f, "LEFTBRACKET", SDLK_LEFTBRACKET);
    nibble_sdl_print_lua_key(f, "BACKSLASH", SDLK_BACKSLASH);
    nibble_sdl_print_lua_key(f, "RIGHTBRACKET", SDLK_RIGHTBRACKET);
    nibble_sdl_print_lua_key(f, "CARET", SDLK_CARET);
    nibble_sdl_print_lua_key(f, "UNDERSCORE", SDLK_UNDERSCORE);
    nibble_sdl_print_lua_key(f, "BACKQUOTE", SDLK_BACKQUOTE);
    nibble_sdl_print_lua_key(f, "DELETE", SDLK_DELETE);
    nibble_sdl_print_lua_key(f, "TILDA", 126);
    nibble_sdl_print_lua_key(f, "KP_0", SDLK_KP_0);
    nibble_sdl_print_lua_key(f, "KP_1", SDLK_KP_1);
    nibble_sdl_print_lua_key(f, "KP_2", SDLK_KP_2);
    nibble_sdl_print_lua_key(f, "KP_3", SDLK_KP_3);
    nibble_sdl_print_lua_key(f, "KP_4", SDLK_KP_4);
    nibble_sdl_print_lua_key(f, "KP_5", SDLK_KP_5);
    nibble_sdl_print_lua_key(f, "KP_6", SDLK_KP_6);
    nibble_sdl_print_lua_key(f, "KP_7", SDLK_KP_7);
    nibble_sdl_print_lua_key(f, "KP_8", SDLK_KP_8);
    nibble_sdl_print_lua_key(f, "KP_9", SDLK_KP_9);
    nibble_sdl_print_lua_key(f, "KP_PERIOD", SDLK_KP_PERIOD);
    nibble_sdl_print_lua_key(f, "KP_DIVIDE", SDLK_KP_DIVIDE);
    nibble_sdl_print_lua_key(f, "KP_MULTIPLY", SDLK_KP_MULTIPLY);
    nibble_sdl_print_lua_key(f, "KP_MINUS", SDLK_KP_MINUS);
    nibble_sdl_print_lua_key(f, "KP_PLUS", SDLK_KP_PLUS);
    nibble_sdl_print_lua_key(f, "KP_ENTER", SDLK_KP_ENTER);
    nibble_sdl_print_lua_key(f, "KP_EQUALS", SDLK_KP_EQUALS);
    nibble_sdl_print_lua_key(f, "UP", SDLK_UP);
    nibble_sdl_print_lua_key(f, "DOWN", SDLK_DOWN);
    nibble_sdl_print_lua_key(f, "RIGHT", SDLK_RIGHT);
    nibble_sdl_print_lua_key(f, "LEFT", SDLK_LEFT);
    nibble_sdl_print_lua_key(f, "INSERT", SDLK_INSERT);
    nibble_sdl_print_lua_key(f, "HOME", SDLK_HOME);
    nibble_sdl_print_lua_key(f, "END", SDLK_END);
    nibble_sdl_print_lua_key(f, "PAGEUP", SDLK_PAGEUP);
    nibble_sdl_print_lua_key(f, "PAGEDOWN", SDLK_PAGEDOWN);
    nibble_sdl_print_lua_key(f, "F1", SDLK_F1);
    nibble_sdl_print_lua_key(f, "F2", SDLK_F2);
    nibble_sdl_print_lua_key(f, "F3", SDLK_F3);
    nibble_sdl_print_lua_key(f, "F4", SDLK_F4);
    nibble_sdl_print_lua_key(f, "F5", SDLK_F5);
    nibble_sdl_print_lua_key(f, "F6", SDLK_F6);
    nibble_sdl_print_lua_key(f, "F7", SDLK_F7);
    nibble_sdl_print_lua_key(f, "F8", SDLK_F8);
    nibble_sdl_print_lua_key(f, "F9", SDLK_F9);
    nibble_sdl_print_lua_key(f, "F10", SDLK_F10);
    nibble_sdl_print_lua_key(f, "F11", SDLK_F11);
    nibble_sdl_print_lua_key(f, "F12", SDLK_F12);
    nibble_sdl_print_lua_key(f, "F13", SDLK_F13);
    nibble_sdl_print_lua_key(f, "F14", SDLK_F14);
    nibble_sdl_print_lua_key(f, "F15", SDLK_F15);
    nibble_sdl_print_lua_key(f, "NUMLOCK", SDLK_NUMLOCKCLEAR);
    nibble_sdl_print_lua_key(f, "CAPSLOCK", SDLK_CAPSLOCK);
    nibble_sdl_print_lua_key(f, "SCROLLOCK", SDLK_SCROLLLOCK);
    nibble_sdl_print_lua_key(f, "RSHIFT", SDLK_RSHIFT);
    nibble_sdl_print_lua_key(f, "LSHIFT", SDLK_LSHIFT);
    nibble_sdl_print_lua_key(f, "RCTRL", SDLK_RCTRL);
    nibble_sdl_print_lua_key(f, "LCTRL", SDLK_LCTRL);
    nibble_sdl_print_lua_key(f, "RALT", SDLK_RALT);
    nibble_sdl_print_lua_key(f, "LALT", SDLK_LALT);
    nibble_sdl_print_lua_key(f, "RMETA", SDLK_RGUI);
    nibble_sdl_print_lua_key(f, "LMETA", SDLK_LGUI);
    nibble_sdl_print_lua_key(f, "LSUPER", SDLK_LGUI);
    nibble_sdl_print_lua_key(f, "RSUPER", SDLK_RGUI);
    nibble_sdl_print_lua_key(f, "MODE", SDLK_MODE);
    nibble_sdl_print_lua_key(f, "HELP", SDLK_HELP);
    nibble_sdl_print_lua_key(f, "PRINT", SDLK_PRINTSCREEN);
    nibble_sdl_print_lua_key(f, "SYSREQ", SDLK_SYSREQ);
    nibble_sdl_print_lua_key(f, "MENU", SDLK_MENU);
    nibble_sdl_print_lua_key(f, "POWER", SDLK_POWER);
    nibble_sdl_print_lua_key(f, "UNDO", SDLK_UNDO);

    fprintf(f, "return key_constant\n");

    fclose(f);

    DEBUG_LOG("Constants printed'\n");
}

void nibble_sdl_print_lua_key(FILE *f, char *name, int key)
{
    fprintf(f, "key_constant.KEY_%s = %d\n", name, nibble_get_custom_key(key));
}