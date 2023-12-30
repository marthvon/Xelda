#ifndef MAP_H
#define MAP_H

#include "SDL2/SDL_image.h"

#include "../globals.h"
#include "entity.h"
#include "collision_map.h"
#include "../types/vector.h"
#include "../game_objects/maps/map_table.h"
#include "../game_objects/entity/collision_table.h"

#define INSERT_MAP(ZIndex, call) insert_entity_to_map((map->z_index)+ZIndex, call);
#define FILL_MAP_2D(x1, x2, y1, y2, ZIndex, call) for(int x = x1; x < (x2+1); ++x) for(int y = y1; y < (y2+1); ++y) insert_entity_to_map((map->z_index)+ZIndex, call);
#define FILL_HORIZONTAL_LINE(x1, x2, ZIndex, call) for(int x = x1; x < (x2+1); ++x) insert_entity_to_map((map->z_index)+ZIndex, call);
#define FILL_VERTICAL_LINE(y1, y2, ZIndex, call) for(int y = y1; y < (y2+1); ++y) insert_entity_to_map((map->z_index)+ZIndex, call);
#define GET_POSITION_START(x,y) (0.5+x)*PIXEL_BLOCK_SIZE_X, (0.5+y)*PIXEL_BLOCK_SIZE_Y

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

    struct Entity** entities;
    unsigned short entity_count;
    int entity_tail;

    struct CollisionMap collision_map[MAX_COLLISION_LAYER];

    struct Entity** process_priority;
    unsigned short process_count;
    struct Entity** input_priority;
    unsigned short input_count;

    Z_Index* z_index;
    unsigned short z_index_count;
    //Vector_zt z_index;

    SDL_Rect window_offset;

    MapInstance switch_map;
} Map;


Map* on_ready_start();
Map* on_ready(MapInstance map_type);
Map* propagate_process(Map* map, const float delta);
void propagate_input(Map* map);
void propagate_draw(Map* map);
void propagate_exit(Map* map);

void insert_entity_to_collection(struct Entity* entity);
void insert_entity_to_map(Z_Index* z_index, struct Entity* entity);
void insert_entity_only_to_map(Z_Index* z_index, struct Entity* entity);

/* later
void remove_entity();
void add_entity();
*/
#endif