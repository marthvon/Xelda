#include "collision_map.h"
#include "map.h"

#define INDEX_2D(x, y) (x) + (y)*colmap->dimensions[0]

void init_collision_map(CollisionMap* map_out, const unsigned short dimension_x, const unsigned short dimension_y, const unsigned short partition_x, const unsigned short partition_y) {
    map_out->dimensions[0] = dimension_x;
    map_out->dimensions[1] = dimension_y;
    map_out->partitions[0] = partition_x;
    map_out->partitions[1] = partition_y;
    map_out->grid = calloc(dimension_x*dimension_y, sizeof(unsigned short));
}

BOOL check_collision_on(Map* map, unsigned int collision_flag, const Point2f position) {
    for(int i = 0; i < MAX_COLLISION_LAYER; ++i, collision_flag >>= 1) {
        if(!(collision_flag & 0b1))
            continue;
        CollisionMap* colmap = &map->collision_map[i];
        Point2U16 grid_pos = {position[0] / colmap->partitions[0], position[1] / colmap->partitions[1]};
        if(
            !X2_INBETWEEN_X1_X3(-1, grid_pos[0], colmap->dimensions[0]) || 
            !X2_INBETWEEN_X1_X3(-1, grid_pos[1], colmap->dimensions[1]) || 
            colmap->grid[INDEX_2D(grid_pos[0], grid_pos[1])]
        )
            return TRUE;
    }
    return FALSE;
}

BOOL check_collision_with_vec(struct Map* map, unsigned int collision_flag, const Point2f position, const Point2S8 vector) {
    for(int i = 0; i < MAX_COLLISION_LAYER; ++i, collision_flag >>= 1) {
        if(!(collision_flag & 0b1))
            continue;
        CollisionMap* colmap = &map->collision_map[i];
        // divide and floor to nearest integer value
        Point2U16 grid_pos = {
            (position[0] / colmap->partitions[0]),
            position[1] / colmap->partitions[1]
        };
        
        // decrement if fract is less than or equal to 0.5 and increment if fract is greater than or equal 0.5 
        if(vector[0] == -1) {
            if(XF_LESS_THAN_OR_EQUAL_YF(position[0]-grid_pos[0]*colmap->partitions[0], colmap->partitions[0]/2))
                --grid_pos[0];
        } else if (vector[0] == 1) {
            if(XF_GREATER_THAN_OR_EQUAL_YF(position[0]-grid_pos[0]*colmap->partitions[0], colmap->partitions[0]/2))
                ++grid_pos[0];
        } else if (vector[1] == -1) {
            if(XF_LESS_THAN_OR_EQUAL_YF(position[1]-grid_pos[1]*colmap->partitions[1], colmap->partitions[1]/2))
                --grid_pos[1];
        } else if(vector[1] == 1) {
            if(XF_GREATER_THAN_OR_EQUAL_YF(position[1]-grid_pos[1]*colmap->partitions[1], colmap->partitions[1]/2))
                ++grid_pos[1];
        } else 
            return TRUE;
        
        // return collision true when out of bounds of map
        if(
            !X2_INBETWEEN_X1_X3(-1, grid_pos[0], colmap->dimensions[0]) || 
            !X2_INBETWEEN_X1_X3(-1, grid_pos[1], colmap->dimensions[1]) || 
            colmap->grid[INDEX_2D(grid_pos[0], grid_pos[1])]
        ) 
            return TRUE;

        // start checking if collision exist in grid and adjacent grids perpendicular of vector 
        if(vector[0] != 0) {
            if(XF_LESS_THAN_YF(position[1]-grid_pos[1]*colmap->partitions[1], colmap->partitions[1]/2)) { 
                if(colmap->grid[INDEX_2D(grid_pos[0], grid_pos[1]-1)])
                    return TRUE;
            } else if(XF_GREATER_THAN_YF(position[1]-grid_pos[1]*colmap->partitions[1], colmap->partitions[1]/2))
                if(colmap->grid[INDEX_2D(grid_pos[0], grid_pos[1]+1)])
                    return TRUE;
        } else {
            if(XF_LESS_THAN_YF(position[0]-grid_pos[0]*colmap->partitions[0], colmap->partitions[0]/2)) {
                if(colmap->grid[INDEX_2D(grid_pos[0]-1, grid_pos[1])])
                    return TRUE;
            } else if(XF_GREATER_THAN_YF(position[0]-grid_pos[0]*colmap->partitions[0], colmap->partitions[0]/2))
                if(colmap->grid[INDEX_2D(grid_pos[0]+1, grid_pos[1])])
                    return TRUE;
        }
    }
    return FALSE;
}

void add_collision_on(Map* map, unsigned int collision_flag, const Point2f position) {
    for(int i = 0; i < MAX_COLLISION_LAYER; ++i, collision_flag >>= 1) {
        if(!(collision_flag & 0b1))
            continue;
        CollisionMap* colmap = &map->collision_map[i];
        Point2U16 grid_pos = {position[0] / colmap->partitions[0], position[1] / colmap->partitions[1]};
        if(!X2_INBETWEEN_X1_X3(-1, grid_pos[0], colmap->dimensions[0]) || !X2_INBETWEEN_X1_X3(-1, grid_pos[1], colmap->dimensions[1]))
            return;
        const unsigned short index = INDEX_2D(grid_pos[0], grid_pos[1]);
        if(colmap->grid[index] != (unsigned short)(-1))
            colmap->grid[index]++;
    }
}

void remove_collision_on(Map* map, unsigned int collision_flag, const Point2f position) {
    for(int i = 0; i < MAX_COLLISION_LAYER; ++i, collision_flag >>= 1) {
        if(!(collision_flag & 0b1))
            continue;
        CollisionMap* colmap = &map->collision_map[i];
        Point2U16 grid_pos = {position[0] / colmap->partitions[0], position[1] / colmap->partitions[1]};
        if(!X2_INBETWEEN_X1_X3(-1, grid_pos[0], colmap->dimensions[0]) || !X2_INBETWEEN_X1_X3(-1, grid_pos[1], colmap->dimensions[1]))
            return;
        const unsigned short index = INDEX_2D(grid_pos[0], grid_pos[1]);
        if(colmap->grid[index])
           colmap->grid[index]--;
    }
}

int get_collision_on(struct Map* map, CollisionType collision_type, const Point2f position) {
    unsigned short i = 0;
    while((collision_type >>= 1))
        ++i;
    CollisionMap* colmap = &map->collision_map[i];
    Point2U16 grid_pos = {position[0] / colmap->partitions[0], position[1] / colmap->partitions[1]};
    if(!X2_INBETWEEN_X1_X3(-1, grid_pos[0], colmap->dimensions[0]) || !X2_INBETWEEN_X1_X3(-1, grid_pos[1], colmap->dimensions[1]))
        return -1;
    return colmap->grid[INDEX_2D(grid_pos[0], grid_pos[1])];
}

void add_collision_value_on(struct Map* map, unsigned int collision_flag, const unsigned short value, const unsigned short x, const unsigned short y) {
    for(int i = 0; i < MAX_COLLISION_LAYER; ++i, collision_flag >>= 1) {
        if(!(collision_flag & 0b1))
            continue;
        CollisionMap* colmap = &map->collision_map[i];
        Point2U16 grid_pos = {x / colmap->partitions[0], y / colmap->partitions[1]};
        if(!X2_INBETWEEN_X1_X3(-1, grid_pos[0], colmap->dimensions[0]) || !X2_INBETWEEN_X1_X3(-1, grid_pos[1], colmap->dimensions[1]))
            return;
        colmap->grid[INDEX_2D(grid_pos[0], grid_pos[1])] += value;
    }
}

void null_collision_on(struct Map* map, unsigned int collision_flag, const unsigned short x, const unsigned short y){
    for(int i = 0; i < MAX_COLLISION_LAYER; ++i, collision_flag >>= 1) {
        if(!(collision_flag & 0b1))
            continue;
        CollisionMap* colmap = &map->collision_map[i];
        Point2U16 grid_pos = {x / colmap->partitions[0], y / colmap->partitions[1]};
        if(!X2_INBETWEEN_X1_X3(-1, grid_pos[0], colmap->dimensions[0]) || !X2_INBETWEEN_X1_X3(-1, grid_pos[1], colmap->dimensions[1]))
            return;
        colmap->grid[INDEX_2D(grid_pos[0], grid_pos[1])] = 0;
    }
}

BOOL check_collision_on_coord(struct Map* map, unsigned int collision_flag, const unsigned short x, const unsigned short y) {
    Point2f temp = {x, y};
    return check_collision_on(map, collision_flag, temp);
}

void add_collision_on_coord(struct Map* map, unsigned int collision_flag, const unsigned short x, const unsigned short y) {
    Point2f temp = {x, y};
    add_collision_on(map, collision_flag, temp);
}

void remove_collision_on_coord(struct Map* map, unsigned int collision_flag, const unsigned short x, const unsigned short y) {
    Point2f temp = {x, y};
    remove_collision_on(map, collision_flag, temp);
}

