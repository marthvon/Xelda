#ifndef STATUE_H
#define STATUE_H

#include "../../globals.h"
#include "../../components/entity.h"
#include "../../components/map.h"

typedef struct {
    BOOL hasSword;
} Statue;

Entity* ReadyStatue(const float position_x, const float position_y, Map* created_by);
void ExitStatue(Entity* entity);
void ProcessStatue(Entity* entity, const float delta);

void GetStatueSrcRectPos(Entity* entity, SDL_Rect* rect);

#endif