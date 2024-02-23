#include "input_manager.h"

int input_init()
{
    return 0;
}

int input_update()
{
    return 1;
}

void updateButtonState()
{
    // TODO
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
        //if (i == nibble_get_custom_key(SDLK_LSHIFT) || i == nibble_get_custom_key(SDLK_RSHIFT))
        //    continue;
        nibble_keymap[i] = 0;
    }
}

void nibble_allegro_save_lua_keys_constants()
{
    // TODO
}

void nibble_allegro_print_lua_key(FILE *f, char *name, int key)
{
    //fprintf(f, "key_constant.KEY_%s = %d\n", name, nibble_get_custom_key(key));
}