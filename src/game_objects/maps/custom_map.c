#include "../../components/map.h"
#include <stddef.h>

#include "start.h"
#include "south_borough.h"
#include "map_table.h"

Map* on_ready(MapInstance map_type) {
    switch(map_type) {
        case START_MAP:
            return OnReadyStartMap();
        case SOUTH_BOROUGH_MAP:
            return OnReadySBoroughMap();
        default:
    };
    return NULL;
}

Map* on_ready_start() {
    return on_ready(START_MAP);
}

void propagate_exit(Map* map) {
    switch(map->type) {
        case START_MAP:
            ExitStartMap(map);
            break;
        case SOUTH_BOROUGH_MAP:
            ExitSBoroughMap(map);
            break;
        default:
    }
    
    Entity** end = map->entities + map->entity_tail;
    for(Entity** itr = map->entities; itr != end; ++itr)
        if(*itr != NULL)
            terminate(*itr);
    free(map->entities);

    for(int i = 0; i < MAX_COLLISION_LAYER; ++i)
        free(map->collision_map[i].grid);    

    for(int i = 0; i < map->z_index_count; ++i) {
        SDL_DestroyTexture(map->z_index[i].cache_layer);
        free(map->z_index[i].entity.val);
    }
    free(map->z_index);
    free(map->process_priority);
    free(map->input_priority);
    
    free(map);
}