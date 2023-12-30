#include "background.h"
#include <stddef.h>

#include "../../components/collision_map.h"
#include "collision_table.h"

#include "../textures/texture_table.h"

Entity* ReadyBackground(const float position_x, const float position_y, TextureEnums sprite, unsigned int spritesheet_ref, Map* created_by) {
    Entity* entity = calloc(1, sizeof(Entity));
    Background* instance = malloc(sizeof(Background));
    entity->instance = instance;
    entity->map = created_by;
    entity->redraw = TRUE;
    set_position(entity, position_x, position_y);
    switch(sprite) {
        case GRASSnDIRT_SPRITESHEET: {
            entity->texture_ref = GRASSnDIRT_SPRITESHEET;
            instance->spritesheet_ref.gnd = spritesheet_ref;
            set_extent(entity, 8, 8, 8, 8);
            if(X2_INBETWEEN_X1_X3(3, instance->spritesheet_ref.gnd%GnD_MAX_COLUMN, 7))
                add_collision_on(entity->map, COLLISION_LAYER, entity->position);
        } break;
        case TREEnROCK_SPRITESHEET: {
            entity->texture_ref = TREEnROCK_SPRITESHEET;
            if((instance->spritesheet_ref.tnr = spritesheet_ref) == TnR_TREE) {
                set_extent(entity, 8, 24, 8, 8);
                add_collision_on_coord(entity->map, COLLISION_LAYER, position_x, position_y-TnR_PARTITION_Y);
            } else
                set_extent(entity, 8, 8, 8, 8);
            add_collision_on(entity->map, COLLISION_LAYER, entity->position);
        } break;
        default:
            free(entity);
            free(entity->instance);
            return NULL;
    }
    return entity;
}

void ExitBackground(Entity* entity) { 
    Background* instance = entity->instance;
    if(entity->texture_ref == GRASSnDIRT_SPRITESHEET && X2_INBETWEEN_X1_X3(3, instance->spritesheet_ref.gnd%GnD_MAX_COLUMN, 7))
        remove_collision_on(entity->map, COLLISION_LAYER, entity->position);
    else if(entity->texture_ref == TREEnROCK_SPRITESHEET) {
        remove_collision_on(entity->map, COLLISION_LAYER, entity->position);
        if(((Background*)entity->instance)->spritesheet_ref.tnr == TnR_TREE)
            remove_collision_on_coord(entity->map, COLLISION_LAYER, entity->position[0], entity->position[1]-TnR_PARTITION_Y);
    }
}

void GetBackgroundSrcRectPos(Entity* entity, SDL_Rect* rect) {
    if(entity->texture_ref == GRASSnDIRT_SPRITESHEET) {
        rect->x = (((Background*)entity->instance)->spritesheet_ref.gnd%GnD_MAX_COLUMN)*GnD_PARTITION_X;
        rect->y = (((Background*)entity->instance)->spritesheet_ref.gnd/GnD_MAX_COLUMN)*GnD_PARTITION_Y;
    } else if(entity->texture_ref == TREEnROCK_SPRITESHEET) {
        rect->x = (((Background*)entity->instance)->spritesheet_ref.tnr%TnR_MAX_COLUMN)*TnR_PARTITION_X;
        rect->y = (((Background*)entity->instance)->spritesheet_ref.tnr/TnR_MAX_COLUMN)*TnR_PARTITION_Y;
    }
}