#ifndef ENTITY_H
#define ENTITY_H

#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_render.h"

#include "../globals.h"
#include "map.h"
#include "../game_objects/entity/entity_table.h"
#include "../game_objects/textures/texture_table.h"

// forward declaration
struct Map;

typedef union {
    SDL_FPoint offset;
    struct {
        float left;
        float up;
        float right;
        float down;
    } margin;
} Extent;

typedef struct Entity {
    EntityInstance type;
    void* instance;
    TextureEnums texture_ref;
    Point2f position;
    Extent extent;
    float angle;
    SDL_RendererFlip flip;
    struct Map* map;
    BOOL redraw;
} Entity;

Entity* ready(EntityInstance type, const float position_x, const float position_y, struct Map* created_by);
Entity* ready_m(EntityInstance type, const float position_x, const float position_y, TextureEnums sprite, unsigned int spritesheet_ref, struct Map* created_by);
void process(Entity* entity, const float delta);
void input(Entity* entity);
void terminate(Entity* entity);

void set_extent(Entity* entity,
    const unsigned short offset_left, const unsigned short offset_up,
    const unsigned short offset_right, const unsigned short offset_down);

void set_position(Entity* entity, const float position_x, const float position_y);

SDL_Rect get_src_rect(Entity* entity);
void get_src_rect_pos(Entity* entity, SDL_Rect* rect);
SDL_FRect get_dest_rect(Entity* entity);

#endif