#include "treasure.h"

#include "../../globals.h"
#include "../../core/global_store.h"
#include "../../core/logger.h"
#include "player.h"
#include "item_node.h"
#include "entity_table.h"

Entity* ReadyTreasure(const float position_x, const float position_y, Map* created_by) {
    Entity* entity = calloc(1, sizeof(Entity));
    entity->type = TREASURE_ENTITY;
    entity->map = created_by;
    set_extent(entity, 8, 8, 8, 8);
    set_position(entity, position_x, position_y);
    
    Treasure* instance = calloc(1, sizeof(Treasure));
    entity->instance = instance;
    instance->item_on_interact = StickItem + (rand()%3);

    return entity;
}

void ExitTreasure(Entity* entity) {
    FOR_VECTOR(Entity*, entity->map->process_priority,, {
        if(*itr != entity)
            continue;
        *itr = NULL;
    })

    FOR_VECTOR(Entity*, entity->map->entities,, {
        if(*itr != entity)
            continue;
        *itr = NULL;
    })
}

void ProcessTreasure(Entity* entity) {
    if(!check_collision_on(entity->map, INTERACTION_LAYER, entity->position))
        return;
    const EntityContainer* const player_container = FindEntityTag("player");
    if(!player_container)
        return;
    Entity* player_ett = player_container->entity;
    Player* player = player_ett->instance;
    ItemType item_type = ((Treasure*)entity->instance)->item_on_interact;
    terminate(entity);
    player->is_update_menu = TRUE;

    WriteOnLog("[Pick Up]: Obtained ");
    WriteOnLog(get_item_name(item_type));
    WriteOnLog("!!!\n");

    FOR_LIST(Item,, player->inventory, {
        if(itr->val.type != item_type)
            continue;
        itr->val.count++;
        return;
    })
    ItemNode item = {item_type, 1};
    push_backItem(&player->inventory, item);
}
