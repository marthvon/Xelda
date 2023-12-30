#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "../../components/entity.h"
#include "../../components/map.h"
#include "../textures/spritesheet/grassndirt.h"
#include "../textures/spritesheet/treenrock.h"

typedef struct {
    union {
        GrassnDirtSpritesheetEnums gnd;
        TreenRockSpritesheetEnums tnr;
    } spritesheet_ref;
} Background;

Entity* ReadyBackground(const float position_x, const float position_y, TextureEnums sprite, unsigned int spritesheet_ref, Map* created_by);
void ExitBackground(Entity* entity);

void GetBackgroundSrcRectPos(Entity* entity, SDL_Rect* rect);

#endif