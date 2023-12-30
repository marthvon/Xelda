#include "entity.h"

void set_extent(Entity* entity,
    const unsigned short offset_left, const unsigned short offset_up,
    const unsigned short offset_right, const unsigned short offset_down
) {
    entity->extent.margin.left = offset_left;
    entity->extent.margin.up = offset_up;
    entity->extent.margin.right = offset_right;
    entity->extent.margin.down = offset_down;

    entity->redraw = TRUE;
}

void set_position(Entity* entity, const float position_x, const float position_y) {
    entity->position[0] = position_x;
    entity->position[1] = position_y;
    entity->redraw = TRUE;
}

SDL_Rect get_src_rect(Entity* entity) {
    SDL_Rect temp = {
        0,0,
        entity->extent.margin.left + entity->extent.margin.right,
        entity->extent.margin.up + entity->extent.margin.down
    };
    get_src_rect_pos(entity, &temp);
    return temp;
}

SDL_FRect get_dest_rect(Entity* entity) {
    SDL_FRect temp = {
        entity->position[0] - entity->extent.offset.x,
        entity->position[1] - entity->extent.offset.y,
        entity->extent.margin.left + entity->extent.margin.right,
        entity->extent.margin.up + entity->extent.margin.down
    };
    return temp;
}