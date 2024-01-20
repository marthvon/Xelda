#ifndef DOOR_ENTITY_H
#define DOOR_ENTITY_H

#include "../../components/entity.h"
#include "../../components/map.h"

typedef struct {

} DoorEntity;

Entity* ReadyDoor(const float position_x, const float position_y, Map* created_by);
void ExitDoor(Entity* entity);
void ProcessDoor(Entity* entity);

#endif