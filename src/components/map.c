#include "map.h"
#include "entity.h"
#include <stddef.h>

#include "../core/visual_server.h"

VECTOR_TEMPLATE_SDEF(Entity*, short,_ett)
VECTOR_TEMPLATE_SDEF(Z_Index, short,_zt)

Map* map_defaults(Map* map) {
    if(map->entities.capacity == 0)
        init_vector_ett(&(map->entities), 25);
    if(map->process_priority.capacity == 0)
        init_vector_ett(&(map->process_priority), 10);
    if(map->input_priority.capacity == 0)
        init_vector_ett(&(map->input_priority), 3);
    if(map->z_index.capacity == 0)
        init_vector_zt(&(map->z_index.val), 2);
        //create cache layer
    return map;
}


Map* propagate_process(Map* map, const float delta) {
    FOR_VECTOR(Entity*, map->process_priority,, {
        if(itr == NULL)
            continue;
        process(*itr, delta);
        if(map->switch_map == VOID_MAP)
            continue;
        ClearCanvas();
        Map* temp = on_ready(map->switch_map);
        propagate_exit(map);
        return temp;
    })
    return map;
}

void propagate_input(Map* map) {
    FOR_VECTOR(Entity*, map->input_priority,,
        if(*itr != NULL)
            input(*itr);
    )
}

void update_texture_cache(Z_Index* z_index) {
    SetCacheLayer(z_index->cache_layer);
    FOR_VECTOR(struct Entity*, z_index->entity,, 
        UpdateCacheLayer(*itr);
    )
}

void propagate_draw(Map* map) {
    BOOL redraw = FALSE;
    FOR_VECTOR(Z_Index, map->z_index,z_,
        FOR_VECTOR_REF(Entity*, z_itr->entity,e_,
            if(!(val->redraw))
                continue;
            update_texture_cache(z_itr);
            redraw = TRUE;
            break;
        )
    )
    if(redraw)
        DrawCanvas(map);
}

void exit_map_def(Map* map) {
    FOR_VECTOR(Entity*, map->entities,,
        if(*itr != NULL)
            terminate(*itr);
    )
    free(map->entities.val);

    for(int i = 0; i < MAX_COLLISION_LAYER; ++i)
        free(map->collision_map[i].grid);    

    FOR_VECTOR(Z_Index, map->z_index,, {
        SDL_DestroyTexture(itr->cache_layer);
        free(itr->entity.val);
    })

    free(map->z_index.val);
    free(map->process_priority.val);
    free(map->input_priority.val);
    
    free(map);
}

void own_entity(Entity* entity) {
    Map* map = entity->map;
    if(map->entity_tail == map->entity_count) {
        fprintf(stderr, "Max size of entitiees for Map has been reached\n");
        return;
    }
    map->entities[map->entity_tail] = entity;
    map->entity_tail++;
}

void display_entity(const unsigned short zindex, struct Entity* entity) {
    push_ett(&(entity->map->z_index[zindex]->entity), entity);
}

void insert_entity(const unsigned short zindex, Entity* entity) {
    own_entity(entity);
    display_entity(zindex, entity);
}

void fill_map_2d(const unsigned short x1, const unsigned short x2, const unsigned short y1, const unsigned short y2, const unsigned short zindex, struct Entity* entity) {
    for(int x = x1; x < (x2+1); ++x) 
        for(int y = y1; y < (y2+1); ++y) 
            insert_entity(zindex, entity);
}

void fill_horizontal_line(const unsigned short x1,const unsigned short x2, const unsigned short zindex, struct Entity* entity) {
    for(int x = x1; x < (x2+1); ++x)
        insert_entity(zindex, entity);
}

void fill_vertical_line(const unsigned short y1, const unsigned short y2, const unsigned short zindex, struct Entity* entity) {
    for(int y = y1; y < (y2+1); ++y) 
        insert_entity(zindex, entity);
}

