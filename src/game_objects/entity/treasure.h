#ifndef TREASURE_H
#define TREASURE_H

#include "item_node.h"
#include "../../components/entity.h"
#include "../../components/map.h"

typedef struct {
    ItemType item_on_interact;
} Treasure;

Entity* ReadyTreasure(const float position_x, const float position_y, Map* created_by);
void ExitTreasure(Entity* entity);
void ProcessTreasure(Entity* entity);

#endif