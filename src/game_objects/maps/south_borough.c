#include "south_borough.h"

#include "../../core/visual_server.h"
#include "../../core/global_store.h"
#include "../../components/collision_map.h"
#include "../../components/entity.h"
#include "../entity/player.h"
#include "../entity/enemy_spider.h"
#include "../entity/entity_table.h"
#include "../textures/texture_table.h"
#include "../textures/spritesheet/grassndirt.h"
#include "../textures/spritesheet/treenrock.h"

#define PIXEL_BLOCK_SIZE_X 16
#define PIXEL_BLOCK_SIZE_Y 16

struct Map* OnReadySBoroughMap() {
    Map* map = calloc(1, sizeof(Map));
    LoadTextureResource(GRASSnDIRT_SPRITESHEET);
    LoadTextureResource(TREEnROCK_SPRITESHEET);

    map->type = SOUTH_BOROUGH_MAP;
    map->switch_map = VOID_MAP;

    map->entities = calloc(860, sizeof(Entity*));
    map->entity_count = 860;
    for(int i = 0; i < MAX_COLLISION_LAYER; ++i)
        init_collision_map((map->collision_map)+i, 15, 15, PIXEL_BLOCK_SIZE_X, PIXEL_BLOCK_SIZE_Y);
    map->z_index = calloc(3, sizeof(Z_Index));
    map->z_index_count = 3;
    for(int i = 0; i < 3; ++i)
        map->z_index[i].cache_layer = CreateCacheLayer(240, 240);
    init_vector_ett(&map->z_index[0].entity, 225);
    init_vector_ett(&map->z_index[1].entity, 150);
    init_vector_ett(&map->z_index[2].entity, 5);
    
    map->process_priority = calloc(3, sizeof(Entity*));
    map->process_count = 3;
    map->input_priority = calloc(1, sizeof(Entity*));
    map->input_count = 1;
    map->entity_tail = 0;

    // creating map
    // dirt path on top
    FILL_VERTICAL_LINE(0, 1, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(6, y), GRASSnDIRT_SPRITESHEET, GD_G2D_L, map));
    FILL_VERTICAL_LINE(0, 1, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(7, y), GRASSnDIRT_SPRITESHEET, GD_DIRT0, map));
    FILL_VERTICAL_LINE(0, 1, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(8, y), GRASSnDIRT_SPRITESHEET, GD_G2D_R, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(6, 2), GRASSnDIRT_SPRITESHEET, GD_G2D_DL, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(7, 2), GRASSnDIRT_SPRITESHEET, GD_G2D_D, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(8, 2), GRASSnDIRT_SPRITESHEET, GD_G2D_DR, map));
    // patch of dirt in the middle
    FILL_HORIZONTAL_LINE(5, 9, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 4), GRASSnDIRT_SPRITESHEET, GD_G2D_U, map));
    FILL_HORIZONTAL_LINE(5, 9, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 10), GRASSnDIRT_SPRITESHEET, GD_G2D_D, map));
    FILL_VERTICAL_LINE(5, 9, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(4, y), GRASSnDIRT_SPRITESHEET, GD_G2D_L, map));
    FILL_VERTICAL_LINE(5, 9, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(10, y), GRASSnDIRT_SPRITESHEET, GD_G2D_R, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(4, 4), GRASSnDIRT_SPRITESHEET, GD_G2D_UL, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(10, 4), GRASSnDIRT_SPRITESHEET, GD_G2D_UR, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(4, 10), GRASSnDIRT_SPRITESHEET, GD_G2D_DL, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(10, 10), GRASSnDIRT_SPRITESHEET, GD_G2D_DR, map));
    FILL_MAP_2D(5, 9, 5, 9, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_DIRT0, map));
    // dirt path on the left
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(0, 6), GRASSnDIRT_SPRITESHEET, GD_G2D_U, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(0, 7), GRASSnDIRT_SPRITESHEET, GD_DIRT3, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(0, 8), GRASSnDIRT_SPRITESHEET, GD_G2D_D, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(1, 6), GRASSnDIRT_SPRITESHEET, GD_G2D_UR, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(1, 7), GRASSnDIRT_SPRITESHEET, GD_G2D_R, map));
    insert_entity_to_map(map->z_index, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(1, 8), GRASSnDIRT_SPRITESHEET, GD_G2D_DR, map));

    // grass below
    FILL_MAP_2D(0, 14, 11, 14, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS3, map));
    // grass on top of dirt patch
    FILL_HORIZONTAL_LINE(4, 10, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 3), GRASSnDIRT_SPRITESHEET, GD_GRASS1, map));
    FILL_MAP_2D(9, 10, 0, 2, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));
    FILL_MAP_2D(0, 5, 0, 2, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS1, map));
    // grass on the right of dirt patch
    FILL_MAP_2D(11, 14, 0, 10, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));
    // grass on the left of dirt patch
    FILL_MAP_2D(0, 3, 3, 5, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));
    FILL_MAP_2D(2, 3, 6, 8, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS3, map));
    FILL_MAP_2D(0, 3, 9, 10, 0, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), GRASSnDIRT_SPRITESHEET, GD_GRASS2, map));

    // trees and rocks
    FILL_MAP_2D(0, 3, 0, 1, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    FILL_HORIZONTAL_LINE(0, 2, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 2), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    FILL_HORIZONTAL_LINE(0, 1, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 3), TREEnROCK_SPRITESHEET, TnR_TREE, map));

    FILL_MAP_2D(11, 14, 0, 1, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    FILL_HORIZONTAL_LINE(12, 14, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 2), TREEnROCK_SPRITESHEET, TnR_TREE, map));
    FILL_HORIZONTAL_LINE(13, 14, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, 3), TREEnROCK_SPRITESHEET, TnR_TREE, map));

    FILL_MAP_2D(0, 5, 12, 14, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));
    FILL_MAP_2D(6, 10, 13, 14, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), TREEnROCK_SPRITESHEET, TnR_ROCK, map));
    FILL_MAP_2D(11, 14, 12, 14, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(x, y), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));

    FILL_VERTICAL_LINE(4, 5, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(0, y), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));
    FILL_VERTICAL_LINE(9, 11, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(0, y), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(1, 6), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(2, 7), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));
    FILL_VERTICAL_LINE(8, 10, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(2, y), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));

    FILL_VERTICAL_LINE(4, 7, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(14, y), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));
    FILL_VERTICAL_LINE(10, 11, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(14, y), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(13, 7), TREEnROCK_SPRITESHEET, TnR_ROCK, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(12, 8), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(11, 8), TREEnROCK_SPRITESHEET, TnR_ROCK, map));
    INSERT_MAP(1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(11, 9), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));
    FILL_VERTICAL_LINE(9, 11, 1, ready_m(BACKGROUND_ENTITY, GET_POSITION_START(13, y), TREEnROCK_SPRITESHEET, TnR_TALL_GRASS, map));

    Entity* spider = ready(ENEMY_SPIDER_ENTITY, GET_POSITION_START(8, 8), map);
    map->process_priority[1] = spider;
    INSERT_MAP(2, spider);
    
    const EntityContainer* const container = FindEntityTag("player");
    Entity* player;
    if(!container) {
        player = ready(PLAYER_ENTITY, GET_POSITION_START(1, 7), map);
        AddEntityTag(player, "player");
    } else {
        player = container->entity;
        player->map = map;
        switch(((Player*)player->instance)->prev_map) {
            case START_MAP:
                player->position[0] -= 5*PIXEL_BLOCK_SIZE_X;
                player->position[1] = 1.5*PIXEL_BLOCK_SIZE_Y;
                break;
            default:
        }
    }
    insert_entity_only_to_map((map->z_index)+2, player);
    
    map->process_priority[0] = player;
    map->input_priority[0] = player;
    
    for(int x = 4; x < 11; ++x)
        add_collision_value_on(map, WARP_EVENT_LAYER, ((unsigned short)START_MAP)+1, x*PIXEL_BLOCK_SIZE_X, 0);
    
    return map;
}

void ExitSBoroughMap(struct Map* map) {
    FreeTextureResource(GRASSnDIRT_SPRITESHEET);
    FreeTextureResource(TREEnROCK_SPRITESHEET);
}