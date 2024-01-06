#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "entity.h"

#include "../core/visual_server.h"
#include "../core/filesystem.h"

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

Map* new_on_ready(MapInstance map_type) {
    OffsetFVCwMap(map_type);
    Map* map = calloc(1, sizeof(Map));
    map->type = map_id;
    
    // read map dimensions
    // read map pixel block size
    for(int i = 0; i < MAX_COLLISION_LAYER; ++i)
        init_collision_map((map->collision_map)+i, 25, 25, 16, 16);
    // read map window offset
    {
        unsigned short texture_count;
        fread(&texture_count, sizeof(texture_count), 1, fvc);
        for(int i = 0; i < texture_count; i++) {
            unsigned short texture_buf;
            fread(&texture_buf, sizeof(texture_buf), 1, fvc);
            LoadTextureResource((TextureEnums)(texture_buf));
        }
    }
    {
        unsigned short capacity_buf;
        // load entity capacity plus extra;
        fread(&capacity_buf, sizeof(capacity_buf), 1, fvc);
        reserve_ett(map->entities, capacity_buf + 10);
        // load process capacity plus extra;
        fread(&capacity_buf, sizeof(capacity_buf), 1, fvc);
        reserve_ett(map->process_priority, capacity_buf + 5);
        // load input capacity plus extra;
        fread(&capacity_buf, sizeof(capacity_buf), 1, fvc);
        reserve_ett(map->input_priority, capacity_buf + 1);
    }
    { // load z_index
        unsigned short zidx_size;
        fread(&zidx_size, sizeof(zidx_size), 1, fvc);
        reserve_zt(map->z_index, zidx_size);
        map->z_index.size = zidx_size;
        FOR_VECTOR(Z_Index, map->z_index,, {
            itr->cache_layer = CreateCacheLayer(400, 400); // modify LoadTextureResource
            unsigned short ett_cap_buf;
            fread(&ett_cap_buf, sizeof(ett_cap_buf), 1, fvc);
            init_vector_ett(&(itr->entity), ett_cap_buf);
        })
    }
    // start initializing local entity
    {
        unsigned short entity_count;
        fread(&entity_count, sizeof(entity_count), 1, fvc);
        for(int i = 0; i < entity_count; ++i) {
            const Int8 is_spritesheet = fgetc(fvc);
            if(is_spritesheet) {
                // use ready_m
            } else {
                // use ready
            }
        }
    }
    return map;
}

// rename to to propagate_exit
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

