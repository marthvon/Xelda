#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "SDL2/SDL.h"

#include "../globals.h"

typedef enum {
    FREE = 0,
    HELD = 0b01,    
    RELEASED = 0b10,
    PRESSED = 0b11
} KeyState;

void InitInput();
void UpdateInputState();
void FlushInputEvent(SDL_KeyboardEvent* key_data);

KeyState GetStateOfKey(SDL_Scancode code);
BOOL isHeld(KeyState state);

#endif