#ifndef COLLISION_MAP_H
#define COLLISION_MAP_H

#include "../globals.h"
#include "../game_objects/entity/collision_table.h"

struct Map;

typedef struct CollisionMap {
    Point2U16 dimensions;
    Point2U16 partitions;
    unsigned short* grid;
} CollisionMap;

void init_collision_map(CollisionMap* map_out, const unsigned short dimension_x, const unsigned short dimension_y, const unsigned short partition_x, const unsigned short partition_y);

BOOL check_collision_on(struct Map* map, unsigned int collision_flag, const Point2f position);
BOOL check_collision_on_coord(struct Map* map, unsigned int collision_flag, const unsigned short x, const unsigned short y);
BOOL check_collision_with_vec(struct Map* map, unsigned int collision_flag, Point2f position, const Point2S8 vector);
BOOL check_collision_with_vec_coord(struct Map* map, unsigned int collision_flag, Point2f position, const short vec_x, const short vec_y);
int get_collision_on(struct Map* map, CollisionType collision_type, const Point2f position);

void add_collision_on(struct Map* map, unsigned int collision_flag, const Point2f position);
void add_collision_on_coord(struct Map* map, unsigned int collision_flag, const unsigned short x, const unsigned short y);
void add_collision_value_on(struct Map* map, unsigned int collision_flag, const unsigned short value, const unsigned short x, const unsigned short y);

void remove_collision_on(struct Map* map, unsigned int collision_flag, const Point2f position);
void remove_collision_on_coord(struct Map* map, unsigned int collision_flag, const unsigned short x, const unsigned short y);
void null_collision_on(struct Map* map, unsigned int collision_flag, const unsigned short x, const unsigned short y);

#endif