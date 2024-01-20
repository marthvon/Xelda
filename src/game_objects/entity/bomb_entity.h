#ifndef BOMB_ENTITY_H
#define BOMB_ENTITY_H

#include "../../components/entity.h"
#include "../../components/map.h"

typedef struct {

} BombEntity;

Entity* ReadyBomb(const float position_x, const float position_y, Map* created_by);
void ExitBomb(Entity* entity);
void ProcessBomb(Entity* entity, const float delta);

#endif