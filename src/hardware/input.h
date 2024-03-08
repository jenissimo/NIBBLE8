#ifndef nibble_input_h
#define nibble_input_h

#ifdef DOS
#include <allegro.h>
#else
#include <stdint.h>
#endif
#include <stdbool.h>

typedef enum {
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_A,
    BUTTON_B,
    BUTTON_START,
    BUTTON_SELECT,
    
    // Add more button names as needed
    NUM_BUTTONS
} ControlButton;

typedef struct {
    bool current[NUM_BUTTONS];
    bool previous[NUM_BUTTONS];
} ButtonState;

extern uint8_t nibble_keymap[512];
extern int8_t nibble_keymap_p[512];
extern ButtonState buttonState;

bool nibble_api_key(int key);
bool nibble_api_keyp(int key);
bool nibble_api_btn(int button);
bool nibble_api_btnp(int button);

#endif