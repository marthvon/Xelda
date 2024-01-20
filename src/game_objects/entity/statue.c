#include "statue.h"

#include "../../core/visual_server.h"
#include "../../core/global_store.h"
#include "../../core/logger.h"
#include "collision_table.h"
#include "player.h"
#include "../maps/events_table.h"
#include "../textures/texture_table.h"

#define STATUE_PARTITION_X 16

Entity* ReadyStatue(const float position_x, const float position_y, Map* created_by) {
    LoadTextureResource(STATUE_SPRITE);

    Entity* entity = calloc(1, sizeof(Entity));
    entity->type = STATUE_ENTITY;
    entity->map = created_by;

    entity->texture_ref = STATUE_SPRITE;
    entity->redraw = TRUE;
    
    set_extent(entity, 8, 8, 8, 8);
    set_position(entity, position_x, position_y);
    add_collision_on(entity->map, COLLISION_LAYER, entity->position);
    
    Statue* instance = calloc(1, sizeof(Statue));
    entity->instance = instance;
    instance->hasSword = !(IsEventTriggered(SWORD_TAKEN_TRIGGERED));

    push_ett(&(created_by->process_priority), entity);
    return entity;
}

void ExitStatue(Entity* entity) {
    FreeTextureResource(STATUE_SPRITE);
    remove_collision_on(entity->map, COLLISION_LAYER, entity->position);
}

void ProcessStatue(Entity* entity, const float delta) {
    if(! (((Statue*)entity->instance)->hasSword) )
        return;
    if(!check_collision_on(entity->map, INTERACTION_LAYER, entity->position))
        return;
    const EntityContainer* const player_container = FindEntityTag("player");
    if(!player_container)
        return;
    Entity* player = player_container->entity;
    ToggleEventTriggered(SWORD_TAKEN_TRIGGERED);

    ItemNode item = {SwordItem, -1};
    push_frontItem(&((Player*)player->instance)->inventory, item);
    ((Statue*)entity->instance)->hasSword = FALSE;

    entity->redraw = TRUE;
    ((Player*)player->instance)->is_update_menu = TRUE;
    WriteOnLog("[Pick Up]: Obtained Sword!!!\n");
}

void GetStatueSrcRectPos(Entity* entity, SDL_Rect* rect) {
    rect->x = ((Statue*)entity->instance)->hasSword*STATUE_PARTITION_X;
    rect->y = 0;
}