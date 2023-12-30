#include "engine.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "SDL2/SDL.h"

#include "../globals.h"
#include "visual_server.h"
#include "input_handler.h"
#include "timer.h"
#include "global_store.h"
#include "../components/map.h"
#include "../components/entity.h"

#define INITIALIZE(condition, error, cleanup, extension) if(condition) {extension; cleanup;} else error
#define INITIALIZE_NOEXCEPT(statement, cleanup, extension) statement; extension; cleanup
#define FINISH_SETUP(additional) additional; StartLoop()
#define CHECK(condition, extension) if(condition) {extension;}

static SDL_Window* HWind;
static Map* MainMap;

void StartLoop() {
    SDL_Event evt;
    while (TRUE) {
        while(SDL_PollEvent(&evt) != 0) {
            switch (evt.type) {
                case SDL_QUIT:
                    return;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    FlushInputEvent(&evt.key);
                    propagate_input(MainMap);
                    break;
            }
        }

        if(XF_LESS_THAN_YF(TickTimer(), 0.016666))
            continue;

        const Map* const prior_map = MainMap;
        if((MainMap = propagate_process(MainMap, GetDeltaTime())) != prior_map) {
            ResetTimer(); // reset timer on map change/switch
            UpdateInputState();
            continue;
        }
        UpdateInputState();
        propagate_draw(MainMap);
    }
}

void InitProgram() {
    system("clear");

    INITIALIZE(
        SDL_Init(SDL_INIT_VIDEO) == 0,
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError()),
        SDL_Quit(),

    INITIALIZE(
        (HWind = SDL_CreateWindow("Xelda", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0)),
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError()),
        SDL_DestroyWindow(HWind),

    CHECK(FileSystemAssetsScan(),

    INITIALIZE(
        InitVisualServer(HWind),
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError()),
        TerminateVisualServer(),
    
    INITIALIZE_NOEXCEPT(
        InitStore(),
        TerminateStore(),

        InitInput();

    INITIALIZE(
        (MainMap = on_ready_start()),
        fprintf(stderr, "Failed to load Map: START_MAP\n"),
        propagate_exit(MainMap),

    FINISH_SETUP(
        InitTimer();
        time_t generator;
        srand((unsigned) time(&generator))
    )))))));
}