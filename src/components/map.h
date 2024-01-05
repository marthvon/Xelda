#ifndef MAP_H
#define MAP_H

#include "SDL2/SDL_image.h"

#include "../globals.h"
#include "entity.h"
#include "collision_map.h"
#include "../types/vector.h"
#include "../game_objects/maps/map_table.h"
#include "../game_objects/entity/collision_table.h"

#define INSERT_MAP(ZIndex, call) insert_entity(ZIndex, call);
#define FILL_MAP_2D(x1, x2, y1, y2, ZIndex, call) for(int x = x1; x < (x2+1); ++x) for(int y = y1; y < (y2+1); ++y) insert_entity(ZIndex, call);
#define FILL_HORIZONTAL_LINE(x1, x2, ZIndex, call) for(int x = x1; x < (x2+1); ++x) insert_entity(ZIndex, call);
#define FILL_VERTICAL_LINE(y1, y2, ZIndex, call) for(int y = y1; y < (y2+1); ++y) insert_entity(ZIndex, call);
#define GET_POSITION_BLOCK(x,y) (0.5+x)*PIXEL_BLOCK_SIZE_X, (0.5+y)*PIXEL_BLOCK_SIZE_Y

// forward declaration
struct Entity;
struct CollisionMap;
struct GlobalStore;

VECTOR_TEMPLATE_S(struct Entity*, short,_ett)

typedef struct {
        Vector_ett entity;
        SDL_Texture* cache_layer;
} Z_Index;

VECTOR_TEMPLATE_S(Z_Index, short,_zt)

typedef struct Map {
    MapInstance type;

    Vector_ett entities;
    Vector_ett process_priority;
    Vector_ett input_priority;
    Vector_zt z_index;

    struct CollisionMap collision_map[MAX_COLLISION_LAYER];
    SDL_Rect window_offset;
    MapInstance switch_map;
} Map;


Map* on_ready_start();
Map* on_ready(MapInstance map_type);
void propagate_exit(Map* map);

Map* map_defaults(Map* map);
Map* propagate_process(Map* map, const float delta);
void propagate_input(Map* map);
void propagate_draw(Map* map);
void exit_map_def(Map* map);

void own_entity(struct Entity* entity); // owned entities are free'd when map is free'd
void display_entity(const unsigned short zindex, struct Entity* entity);
void insert_entity(const unsigned short zindex, struct Entity* entity);
void fill_map_2d(const unsigned short x1, const unsigned short x2, const unsigned short y1, const unsigned short y2, const unsigned short zindex, struct Entity* entity);
void fill_horizontal_line(const unsigned short x1,const unsigned short x2, const unsigned short zindex, struct Entity* entity);
void fill_vertical_line(const unsigned short y1, const unsigned short y2, const unsigned short zindex, struct Entity* entity);

#endif