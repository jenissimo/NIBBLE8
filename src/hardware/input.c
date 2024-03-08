#include "input.h"

uint8_t nibble_keymap[512];
int8_t nibble_keymap_p[512];
ButtonState buttonState;

bool nibble_api_key(int key)
{
    return nibble_keymap[key];
}

bool nibble_api_keyp(int key)
{
    return (nibble_keymap[key] > 0) && (nibble_keymap_p[key] == 0);
}

bool nibble_api_btn(int button)
{
    if (button < 0 || button >= NUM_BUTTONS)
    {
        return 0;
    }
    return buttonState.current[button];
}

bool nibble_api_btnp(int button)
{
    if (button < 0 || button >= NUM_BUTTONS)
    {
        return 0;
    }
    return (buttonState.current[button] > 0) && (buttonState.previous[button] == 0);
}