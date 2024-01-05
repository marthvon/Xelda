#include "start.h"

#include "../../core/visual_server.h"
#include "../../core/global_store.h"
#include "../../components/collision_map.h"
#include "../../components/entity.h"
#include "../entity/player.h"
#include "../entity/entity_table.h"
#include "../textures/texture_table.h"
#include "../textures/spritesheet/grassndirt.h"
#include "../textures/spritesheet/treenrock.h"

#define PIXEL_BLOCK_SIZE_X 16
#define PIXEL_BLOCK_SIZE_Y 16

struct Map* OnReadyStartMap() {
    Map* map = calloc(1, sizeof(Map));
    LoadTextureResource(GRASSnDIRT_SPRITESHEET);
    LoadTextureResource(TREEnROCK_SPRITESHEET);

    map->type = START_MAP;
    map->switch_map = VOID_MAP;

    map->entities = calloc(850, sizeof(Entity*));
    map->entity_count = 850;
    for(int i = 0; i < MAX_COLLISION_LAYER; ++i)
        init_collision_map((map->collision_map)+i, 25, 25, PIXEL_BLOCK_SIZE_X, PIXEL_BLOCK_SIZE_Y);
    map->z_index = calloc(3, sizeof(Z_Index));
    map->z_index_count = 3;
    for(int i = 0; i < 3; ++i)
        map->z_index[i].cache_layer = CreateCacheLayer(400, 400);
    init_vector_ett(&map->z_index[0].entity, 645);
    init_vector_ett(&map->z_index[1].entity, 200);
    init_vector_ett(&map->z_index[2].entity, 5);
    
    map->process_priority = calloc(2, sizeof(Entity*));
    map->process_count = 2;
    map->input_priority = calloc(1, sizeof(Entity*));
    map->input_count = 1;
    map->entity_tail = 0;

    //creating map

    //Water on top
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(3, 0), GRASSnDIRT_SPRITESHEET, GD_G2W_DL, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(4, 1), GRASSnDIRT_SPRITESHEET, GD_G2W_DL, map));
    FILL_HORIZONTAL_LINE(5, 7, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 1), GRASSnDIRT_SPRITESHEET, GD_G2W_D, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(8, 2), GRASSnDIRT_SPRITESHEET, GD_G2W_DL, map));
    FILL_HORIZONTAL_LINE(9, 10, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 2), GRASSnDIRT_SPRITESHEET, GD_G2W_D, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(11, 3), GRASSnDIRT_SPRITESHEET, GD_G2W_DL, map));

    FILL_HORIZONTAL_LINE(4, 20, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 0), GRASSnDIRT_SPRITESHEET, GD_WATER0, map));
    FILL_HORIZONTAL_LINE(8, 16, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 1), GRASSnDIRT_SPRITESHEET, GD_WATER0, map));
    FILL_HORIZONTAL_LINE(11, 13, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 2), GRASSnDIRT_SPRITESHEET, GD_WATER0, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(12, 3), GRASSnDIRT_SPRITESHEET, GD_G2W_D, map));

    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(13, 3), GRASSnDIRT_SPRITESHEET, GD_G2W_DR, map));
    FILL_HORIZONTAL_LINE(14, 15, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 2), GRASSnDIRT_SPRITESHEET, GD_G2W_D, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(16, 2), GRASSnDIRT_SPRITESHEET, GD_G2W_DR, map));
    FILL_HORIZONTAL_LINE(17, 19, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 1), GRASSnDIRT_SPRITESHEET, GD_G2W_D, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(20, 1), GRASSnDIRT_SPRITESHEET, GD_G2W_DR, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(21, 0), GRASSnDIRT_SPRITESHEET, GD_G2W_DR, map));

    //Dirt Path
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(11, 5), GRASSnDIRT_SPRITESHEET, GD_G2D_UL, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(12, 5), GRASSnDIRT_SPRITESHEET, GD_G2D_U, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(13, 5), GRASSnDIRT_SPRITESHEET, GD_G2D_UR, map));

    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(10, 6), GRASSnDIRT_SPRITESHEET, GD_G2D_UL, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(14, 6), GRASSnDIRT_SPRITESHEET, GD_G2D_UR, map));
    FILL_HORIZONTAL_LINE(11, 13, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 6), GRASSnDIRT_SPRITESHEET, GD_DIRT0, map));

    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(9, 7), GRASSnDIRT_SPRITESHEET, GD_G2D_UL, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(9, 8), GRASSnDIRT_SPRITESHEET, GD_G2D_L, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(9, 9), GRASSnDIRT_SPRITESHEET, GD_G2D_DL, map));

    FILL_MAP_2D(10, 15, 7, 9, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_DIRT0, map));

    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(15, 7), GRASSnDIRT_SPRITESHEET, GD_G2D_UR, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(15, 8), GRASSnDIRT_SPRITESHEET, GD_G2D_R, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(15, 9), GRASSnDIRT_SPRITESHEET, GD_G2D_DR, map));

    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(10, 10), GRASSnDIRT_SPRITESHEET, GD_G2D_DL, map));
    FILL_HORIZONTAL_LINE(11, 13, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 10), GRASSnDIRT_SPRITESHEET, GD_DIRT0, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(14, 10), GRASSnDIRT_SPRITESHEET, GD_G2D_DR, map));

    FILL_VERTICAL_LINE(11, 13, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(11, y), GRASSnDIRT_SPRITESHEET, GD_G2D_L, map));
    FILL_VERTICAL_LINE(11, 13, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(12, y), GRASSnDIRT_SPRITESHEET, GD_DIRT0, map));
    FILL_VERTICAL_LINE(11, 13, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(13, y), GRASSnDIRT_SPRITESHEET, GD_G2D_R, map));

    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(10, 14), GRASSnDIRT_SPRITESHEET, GD_G2D_UL, map));
    FILL_VERTICAL_LINE(15,16, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(10, y), GRASSnDIRT_SPRITESHEET, GD_G2D_L, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(10, 17), GRASSnDIRT_SPRITESHEET, GD_G2D_DL, map));

    FILL_MAP_2D(11, 13, 14, 17, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_DIRT0, map));

    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(14, 14), GRASSnDIRT_SPRITESHEET, GD_G2D_UR, map));
    FILL_VERTICAL_LINE(15,16, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(14, y), GRASSnDIRT_SPRITESHEET, GD_G2D_R, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(14, 17), GRASSnDIRT_SPRITESHEET, GD_G2D_DR, map));

    FILL_VERTICAL_LINE(18,24, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(11, y), GRASSnDIRT_SPRITESHEET, GD_G2D_L, map));
    FILL_VERTICAL_LINE(18,24, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(12, y), GRASSnDIRT_SPRITESHEET, GD_DIRT0, map));
    FILL_VERTICAL_LINE(18,24, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(13, y), GRASSnDIRT_SPRITESHEET, GD_G2D_R, map));

    // filling the rest with grass

    FILL_MAP_2D(0, 7, 2, 24, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));
    FILL_MAP_2D(17, 24, 2, 24, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));

    FILL_HORIZONTAL_LINE(0, 2, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 0), GRASSnDIRT_SPRITESHEET, GD_GRASS1, map));
    FILL_HORIZONTAL_LINE(0, 3, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 1), GRASSnDIRT_SPRITESHEET, GD_GRASS3, map));
    FILL_HORIZONTAL_LINE(22, 24, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 0), GRASSnDIRT_SPRITESHEET, GD_GRASS3, map));
    FILL_HORIZONTAL_LINE(21, 24, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 1), GRASSnDIRT_SPRITESHEET, GD_GRASS1, map));

    FILL_MAP_2D(9, 10, 18, 24, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));
    FILL_MAP_2D(14, 15, 18, 24, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS3, map));

    FILL_VERTICAL_LINE(10,17, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(9, y), GRASSnDIRT_SPRITESHEET, GD_GRASS1, map));
    FILL_VERTICAL_LINE(11,13, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(10, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));
    FILL_VERTICAL_LINE(11,13, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(14, y), GRASSnDIRT_SPRITESHEET, GD_GRASS3, map));
    FILL_VERTICAL_LINE(10,17, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(15, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));

    FILL_MAP_2D(9, 10, 3, 5, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));
    FILL_MAP_2D(14, 15, 3, 5, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));

    FILL_HORIZONTAL_LINE(11, 13, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 4), GRASSnDIRT_SPRITESHEET, GD_GRASS1, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(9, 6), GRASSnDIRT_SPRITESHEET, GD_GRASS3, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(15, 6), GRASSnDIRT_SPRITESHEET, GD_GRASS3, map));

    FILL_VERTICAL_LINE(3,24, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(8, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));
    FILL_VERTICAL_LINE(3,24, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(16, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));

    //add trees and rocks    
    FILL_VERTICAL_LINE(2, 24, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(0, y), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    FILL_VERTICAL_LINE(5, 24, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(1, y), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    FILL_VERTICAL_LINE(10, 24, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(2, y), TREEnROCK_SPRITESHEET, TnR_TREE, map));

    FILL_MAP_2D(3, 5, 18, 24, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    FILL_MAP_2D(6, 7, 22, 24, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), TREEnROCK_SPRITESHEET, TnR_TREE, map));

    FILL_MAP_2D(18, 19, 22, 24, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    FILL_MAP_2D(20, 22, 18, 24, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), TREEnROCK_SPRITESHEET, TnR_TREE, map));

    FILL_VERTICAL_LINE(10, 24, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(22, y), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    FILL_VERTICAL_LINE(5, 24, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(23, y), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    FILL_VERTICAL_LINE(2, 24, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(24, y), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(8, 24), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(17, 24), TREEnROCK_SPRITESHEET, TnR_TREE, map));

    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(1, 1), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(2, 0), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(22, 0), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(23, 1), TREEnROCK_SPRITESHEET, TnR_TREE, map));

    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(5, 11), TREEnROCK_SPRITESHEET, TnR_ROCK, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(17, 13), TREEnROCK_SPRITESHEET, TnR_ROCK, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(16, 24), TREEnROCK_SPRITESHEET, TnR_ROCK, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(21, 3), TREEnROCK_SPRITESHEET, TnR_ROCK, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(7, 13), TREEnROCK_SPRITESHEET, TnR_ROCK, map));

    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(4, 5), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(6, 7), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(22, 9), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));

    Entity* statue = ready(STATUE_ENTITY, GET_POSITION_START(12, 7), map);
    INSERT_MAP(2, statue);
    map->process_priority[0] = statue;

    const EntityContainer* const container = FindEntityTag("player");
    Entity* player;
    if(!container) {
        player = ready(PLAYER_ENTITY, GET_POSITION_START(12, 16), map);
        AddEntityTag(player, "player");
    } else {
        player = container->entity;
        player->map = map;
        switch(((Player*)player->instance)->prev_map) {
            case SOUTH_BOROUGH_MAP:
                player->position[0] += 5*PIXEL_BLOCK_SIZE_X;
                player->position[1] = 23.5*PIXEL_BLOCK_SIZE_Y;
                break;
            default:
        }
    }
    insert_entity_only_to_map((map->z_index)+2, player);

    map->process_priority[1] = player;
    map->input_priority[0] = player;
    
    for(int x = 8; x < 17; ++x)
        add_collision_value_on(map, WARP_EVENT_LAYER, ((unsigned short)SOUTH_BOROUGH_MAP)+1, x*PIXEL_BLOCK_SIZE_X, 24*PIXEL_BLOCK_SIZE_Y);
    
    return map;
}

void ExitStartMap(struct Map* map) {
    FreeTextureResource(GRASSnDIRT_SPRITESHEET);
    FreeTextureResource(TREEnROCK_SPRITESHEET);
}