#include "keyboard.h"

static int key_state[NUM_KEYS] = {0};
static int prev_key_state[NUM_KEYS] = {0};

const char* KEY[NUM_KEYS] = {
    "q", "2", "w", "3", "e", "r", "5", "t", "6", "y", "7", "u"
};

int key_from_scancode(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_Q: return 0;
        case SDL_SCANCODE_2: return 1;
        case SDL_SCANCODE_W: return 2;
        case SDL_SCANCODE_3: return 3;
        case SDL_SCANCODE_E: return 4;
        case SDL_SCANCODE_R: return 5;
        case SDL_SCANCODE_5: return 6;
        case SDL_SCANCODE_T: return 7;
        case SDL_SCANCODE_6: return 8;
        case SDL_SCANCODE_Y: return 9;
        case SDL_SCANCODE_7: return 10;
        case SDL_SCANCODE_U: return 11;
        default: return -1;
    }
}

void keyboard_update() {
    memcpy(prev_key_state, key_state, NUM_KEYS * sizeof(int));
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < NUM_KEYS; i++) {
        key_state[i] = state[SDL_GetScancodeFromName(KEY[i])];
    }
}

int keyboard_get_state(int key) {
    if (key < 0 || key >= NUM_KEYS) {
        fprintf(stderr, "Invalid key: %d\n", key);
        return 0;
    }
    return key_state[key];
}

int keyboard_get_state_changed(int key) {
    if (key < 0 || key >= NUM_KEYS) {
        fprintf(stderr, "Invalid key: %d\n", key);
        return 0;
    }
    return (prev_key_state[key] == 0 && key_state[key] == 1);
}