#include "timer.h"

#include "SDL2/SDL_timer.h"
#include "SDL2/SDL_stdinc.h"

static Uint64 lastTime;
static float frequency;
static float delta;

void InitTimer() {
    lastTime = SDL_GetPerformanceCounter();
    frequency = (float) SDL_GetPerformanceFrequency();
    delta = 0.0;
}

const float TickTimer() {
    Uint64 currentTime = SDL_GetPerformanceCounter();
    delta += (float)(currentTime - lastTime) / frequency;
    lastTime = currentTime;

    return delta;
}

const float GetDeltaTime() {
    const float temp = delta;
    delta = 0;
    return temp;
}

void ResetTimer() {
    delta = 0;
    lastTime = SDL_GetPerformanceCounter();
}