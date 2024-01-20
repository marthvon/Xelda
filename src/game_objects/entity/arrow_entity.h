#ifndef ARROW_ENTITY_H
#define ARROW_ENTITY_H

#include "../../components/entity.h"
#include "../../components/map.h"

typedef struct {

} ArrowEntity;

Entity* ReadyArrow(const float position_x, const float position_y, Map* created_by);
void ExitArrow(Entity* entity);
void ProcessArrow(Entity* entity, const float delta);

#endif