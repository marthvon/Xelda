#include "map.h"
#include "entity.h"
#include <stddef.h>

#include "../core/visual_server.h"

VECTOR_TEMPLATE_SDEF(Entity*, short,_ett)
VECTOR_TEMPLATE_SDEF(Z_Index, short,_zt)

Map* propagate_process(Map* map, const float delta) {
    struct Entity** end = map->process_priority + map->process_count;
    for(Entity** itr = map->process_priority; itr != end && *itr != NULL; ++itr)
        if(*itr != NULL){
            process(*itr, delta);
        if(map->switch_map != VOID_MAP) {
            ClearCanvas();
            Map* temp = on_ready(map->switch_map);
            propagate_exit(map);
            return temp;
        }
    }
    return map;
}

void propagate_input(Map* map) {
    struct Entity** end = map->input_priority + map->input_count;
    for(Entity** itr = map->input_priority; itr != end && *itr != NULL; ++itr)
        if(*itr != NULL)
            input(*itr);
}

void update_texture_cache(Z_Index* z_index) {
    SetCacheLayer(z_index->cache_layer);
    FOR_VECTOR(struct Entity*, z_index->entity,, 
        UpdateCacheLayer(*itr);
    )
}

void propagate_draw(Map* map) {
    const Z_Index* const z_end = map->z_index + map->z_index_count;
    for(Z_Index* z_itr = map->z_index; z_itr != z_end; ++z_itr) {
        FOR_VECTOR(Entity*, z_itr->entity,e_, {
            if(!((*e_itr)->redraw))
                continue;
            for(; z_itr != z_end; ++z_itr)
                update_texture_cache(z_itr);
            DrawCanvas(map);
            return;
        })
    }
}

void insert_entity_to_collection(Entity* entity) {
    Map* map = entity->map;
    if(map->entity_tail == map->entity_count) {
        fprintf(stderr, "Max size of entitiees for Map has been reached\n");
        return;
    }
    map->entities[map->entity_tail] = entity;
    map->entity_tail++;
}

void insert_entity_to_map(Z_Index* z_index, Entity* entity) {
    insert_entity_to_collection(entity);
    insert_entity_only_to_map(z_index, entity);
}

void insert_entity_only_to_map(Z_Index* z_index, struct Entity* entity) {
    if(z_index->entity.size == z_index->entity.capacity) {
        fprintf(stderr, "Max size of z-index layer has been reached\n");
        return;
    }
    push_ett(&(z_index->entity), entity);
}