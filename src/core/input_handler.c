#include "input_handler.h"

#define INVALID_SCANCODE (unsigned short)(-1)

static KeyState keystate[56];

Byte keyToIndex(SDL_Scancode code) {
    // 4 <= c < 40, 41 < c < 50 < c < 53 < 58, 224 < c < 227 < c < 231
    return ((code < SDL_SCANCODE_NONUSHASH)? 
        ((code < SDL_SCANCODE_RETURN)? 
            ((code < SDL_SCANCODE_A)? INVALID_SCANCODE : (unsigned short)(code) - 4) : 
            ((code == SDL_SCANCODE_ESCAPE)? INVALID_SCANCODE : (unsigned short)(code) - 6)    
        ) :
        ((code < SDL_SCANCODE_LCTRL)? 
            ( (code >= SDL_SCANCODE_F1 || code == SDL_SCANCODE_NONUSHASH || code == SDL_SCANCODE_GRAVE)? INVALID_SCANCODE :
                ((code < SDL_SCANCODE_GRAVE)? (unsigned short)(code) - 7 : (unsigned short)(code) - 8) ) : 
            ((code < SDL_SCANCODE_LGUI)? (unsigned short)(code) - 174 :
                ((code >= SDL_SCANCODE_RGUI)? INVALID_SCANCODE : (unsigned short)(code) - 175))
        )
    );
}

void InitInput() {
    memset(keystate, 0, sizeof(keystate));  
}

void UpdateInputState() {
    for(int i = 0; i < 57; ++i) {
        if(keystate[i] == PRESSED)
            keystate[i] = HELD;
        else if (keystate[i] == RELEASED)
            keystate[i] = FREE;
    }
}

void FlushInputEvent(SDL_KeyboardEvent* keydata) {
    if(keydata->type == SDL_KEYDOWN) {
        if(keystate[keyToIndex(keydata->keysym.scancode)] != HELD)
            keystate[keyToIndex(keydata->keysym.scancode)] = PRESSED;
        return;
    }
    if(keystate[keyToIndex(keydata->keysym.scancode)] != FREE)
        keystate[keyToIndex(keydata->keysym.scancode)] = RELEASED;
}

KeyState GetStateOfKey(SDL_Scancode code) {
    KeyState res = keystate[keyToIndex(code)];
    if(res == PRESSED)
        keystate[keyToIndex(code)] = HELD;
    else if (res == RELEASED)
        keystate[keyToIndex(code)] = FREE;
    return res;
}

BOOL isHeld(KeyState state) {
    return (state & 0b1);
}