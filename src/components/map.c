#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "entity.h"

#include "../core/visual_server.h"
#include "../core/global_store.h"
#include "../core/filesystem.h"

#define GET_POSITION_BLOCK(x,y) (0.5+x)*map->pixel_block_size[0], (0.5+y)*map->pixel_block_size[1]

VECTOR_TEMPLATE_SDEF(Entity*, short,_ett)
VECTOR_TEMPLATE_SDEF(Z_Index, short,_zt)

/* DEPRECATE START */
void own_entity(Entity* entity) {
    push_ett(&(entity->map->entities), entity);
}

void display_entity(const unsigned short zindex, Entity* entity) {
    push_ett(&(entity->map->z_index.val[zindex].entity), entity);
}

void insert_entity_old(const unsigned short zindex, Entity* entity) {
    own_entity(entity);
    display_entity(zindex, entity);
}
/* DEPRECATE END */
#define OWN_ENTITY(entity) push_ett(&(entity->map->entities), entity)
#define DISPLAY_ENTITY(zindex, entity) push_ett(&(entity->map->z_index.val[zindex].entity), entity)
#define PROCESS_ENTITY(entity) push_ett(&(entity->map->process_priority), entity);
#define INPUT_ENTITY(entity) push_ett(&(entity->map->input_priority), entity);

#define FILL_MAP_2D(x1, x2, y1, y2) for(int x = x1; x < (x2+1); ++x) for(int y = y1; y < (y2+1); ++y) insert_entity(entity, properties_flag, z_index);
#define FILL_HORIZONTAL_LINE(x1, x2) for(int x = x1; x < (x2+1); ++x) insert_entity(entity, properties_flag, z_index);
#define FILL_VERTICAL_LINE(y1, y2) for(int y = y1; y < (y2+1); ++y) insert_entity(entity, properties_flag, z_index);

void insert_entity(Entity* entity, BOOL* properties_flag, unsigned short z_index, BOOL local_entity) {
    if(local_entity) // when map is free'd then entity is also freed
        OWN_ENTITY(entity);
    if(properties_flag[1]) // put entity on process
        PROCESS_ENTITY(entity);
    if(properties_flag[2]) // put entity on input
        INPUT_ENTITY(entity);
    if(properties_flag[3]) // put entity on display
        DISPLAY_ENTITY(z_index, entity);
}

unsigned short get_property_index(const char property_char) {
    switch(property_char) {
        case 'm':
            return 0;
        case 'p':
            return 1;
        case 'c': 
            return 2;
        case 'd':
            return 3;
    }
}

Map* on_ready(MapInstance map_type) {
    Map* map = calloc(1, sizeof(Map));
    map->type = map_type;
    map->switch_map = VOID_MAP;

    EntityInstance entity_type;
    Point2f position;
    unsigned short z_index;
    BOOL instantiated_dim_or_pb = FALSE;

    OffsetStreamWithMap(map_type);
    HydrateStreamWithNextLine();
    const unsigned short line_count = GetFileLineCount();

    for(int i = 0; i < line_count; ++i) {
        HydrateStreamWithNextLine();
        const char curr_flag = ReadStreamChar();
        switch(curr_flag) {
            case 'T': { // Load Texture
                while(HasNext())
                    LoadTextureResource((TextureEnums)ReadStreamInt());
            } break;
            case 'D': { // Check Dimensions
                map->dimensions[0] = ReadStreamInt();
                map->dimensions[1] = ReadStreamInt();
                if(instantiated_dim_or_pb)
                    for(int i = 0; i < MAX_COLLISION_LAYER; ++i)
                        init_collision_map((map->collision_map)+i, 
                            map->dimensions[0]/map->pixel_block_size[0], map->dimensions[1]/map->pixel_block_size[1],
                            map->pixel_block_size[0], map->pixel_block_size[1]);
                instantiated_dim_or_pb = TRUE;
            } break;
            case 'B': { // Check Pixel Block Size
                map->pixel_block_size[0] = ReadStreamInt();
                map->pixel_block_size[1] = ReadStreamInt();
                if(instantiated_dim_or_pb)
                    for(int i = 0; i < MAX_COLLISION_LAYER; ++i)
                        init_collision_map((map->collision_map)+i, 
                            map->dimensions[0]/map->pixel_block_size[0], map->dimensions[1]/map->pixel_block_size[1],
                            map->pixel_block_size[0], map->pixel_block_size[1]);
                instantiated_dim_or_pb = TRUE;
            } break;
            case 'O': {
                map->window_offset.x = ReadStreamInt();
                map->window_offset.y = ReadStreamInt();
                map->window_offset.w = ReadStreamInt();
                map->window_offset.h = ReadStreamInt();
            } break;
            case 'E': { // Entity Capacity
                init_vector_ett(&(map->entities), ReadStreamInt() + 10);
            } break;
            case 'Z': { // Z_Index Capacity and Entity Capacity per layer
                const unsigned short z_index_size = ReadStreamInt();
                init_vector_zt(&(map->z_index), z_index_size);
                map->z_index.size = z_index_size;
                FOR_VECTOR(Z_Index, map->z_index,, {
                    itr->cache_layer = CreateCacheLayer(map->dimensions[0], map->dimensions[1]);
                    init_vector_ett(&(itr->entity), ReadStreamInt()+10);
                })
            } break;
            case 'P': { // Process Capacity
                init_vector_ett(&(map->process_priority), ReadStreamInt() + 5);
            } break;
            case 'I': { // Input Capacity
                init_vector_ett(&(map->input_priority), ReadStreamInt() + 1);
            } break;
            case 'g': { // make a global entity otherwise take existing global entity and make appropriate modification to position relative to the map
                BOOL properties_flag[4] = {FALSE, FALSE, FALSE, FALSE}; // true or false {'m','p','c','d'}
                char tag[15];
                while(TRUE) {
                    ReadStreamString(tag);
                    if(strlen(tag) != 1)
                        break;
                    properties_flag[get_property_index(tag[0])] = TRUE;
                }
                if(properties_flag[3])
                    z_index = ReadStreamInt();
                const EntityContainer* const container = FindEntityTag(tag);
                entity_type = ReadStreamInt();
                position[0] = ReadStreamFloat();
                position[1] = ReadStreamFloat();
                Entity* player;
                if(!container) {
                    player = ready(entity_type, GET_POSITION_BLOCK(position[0], position[1]), map);
                    AddEntityTag(player, tag);
                } else {
                    player = container->entity;
                    player->map = map;
                    while(player->prev_map != ReadStreamInt()){
                        ReadStreamChar(); ReadStreamChar(); ReadStreamChar(); ReadStreamChar();
                    }
                    for(int j = 0; j < 2; ++j)
                        switch(ReadStreamChar()) {
                            case '-':
                                player->position[j] -= ReadStreamFloat()*map->pixel_block_size[0];
                                break;
                            case '+':
                                player->position[j] += ReadStreamFloat()*map->pixel_block_size[0];
                                break;
                            case '=':
                                player->position[j] = ReadStreamFloat()*map->pixel_block_size[0];
                                break;
                        }
                }
                insert_entity(player, properties_flag, z_index, FALSE);
            } break;
            case 'w': { // fill warp
                const char insert_type = ReadStreamChar();
                switch(insert_type) {
                    case 'i': { // insert a single warp
                        position[0] = ReadStreamFloat();
                        position[1] = ReadStreamFloat();
                        add_collision_value_on(map, WARP_EVENT_LAYER, (ReadStreamInt())+1, position[0]*map->pixel_block_size[0], position[1]*map->pixel_block_size[1]);
                    } break;
                    case 'h': { // fill horizontally of warp to the same location
                        const int x1 = ReadStreamFloat(), x2 = ReadStreamFloat();
                        position[1] = ReadStreamFloat();
                        const int warp_value = ReadStreamInt();
                        for(float x = x1; XF_LESS_THAN_YF(x, (x2+1)); ++x)
                            add_collision_value_on(map, WARP_EVENT_LAYER, warp_value+1, x*map->pixel_block_size[0], position[1]*map->pixel_block_size[1]);
                    } break;
                    case 'v': { // fill vertically of warp to the same location
                        const int y1 = ReadStreamFloat(), y2 = ReadStreamFloat();
                        position[0] = ReadStreamFloat();
                        const int warp_value = ReadStreamInt();
                        for(float y = y1; XF_LESS_THAN_YF(y, (y2+1)); ++y)
                            add_collision_value_on(map, WARP_EVENT_LAYER, warp_value+1, position[0]*map->pixel_block_size[0], y*map->pixel_block_size[1]);
                    } break;
                    case 'd': { // fill 2d square of warp to the same location
                        const int x1 = ReadStreamFloat(), x2 = ReadStreamFloat(), y1 = ReadStreamFloat(), y2 = ReadStreamFloat();
                        const int warp_value = ReadStreamInt();
                        for(float x = x1; XF_LESS_THAN_YF(x, (x2+1)); ++x) 
                            for(float y = y1; XF_LESS_THAN_YF(y, (y2+1)); ++y)
                                add_collision_value_on(map, WARP_EVENT_LAYER, warp_value+1, x*map->pixel_block_size[0], y*map->pixel_block_size[1]);
                    } break;
                }
            } break;
            default: {
                BOOL properties_flag[4] = {FALSE, FALSE, FALSE, FALSE}; // true or false {'m','p','c','d'}
                TextureEnums tex;
                int spritesheet_ref;
                
                while(!HasNextInt())
                    properties_flag[get_property_index(ReadStreamChar())] = TRUE;

                switch(curr_flag) {
                    case 'i': { // insert a single entity
                        if(properties_flag[3]) // check if entity is displayed in map
                            z_index = ReadStreamInt();
                        entity_type = (EntityInstance)ReadStreamInt();
                        position[0] = ReadStreamFloat();
                        position[1] = ReadStreamFloat();
                        if(properties_flag[0]) {
                            tex = ReadStreamInt();
                            spritesheet_ref = ReadStreamInt();
                            insert_entity(ready_m(entity_type, GET_POSITION_BLOCK(position[0], position[1]), tex, spritesheet_ref, map), 
                                properties_flag, z_index, TRUE);
                        } else
                            insert_entity(ready(entity_type, GET_POSITION_BLOCK(position[0], position[1]), map), properties_flag, z_index, TRUE);
                    } break;
                    case 'h': { // fill horizontally of entity with identical properties
                        const int x1 = ReadStreamFloat(), x2 = ReadStreamFloat();
                        position[1] = ReadStreamFloat();
                        if(properties_flag[3]) // check if entity is displayed in map
                            z_index = ReadStreamInt();
                        entity_type = (EntityInstance)ReadStreamInt();
                        if(properties_flag[0]) {
                            tex = ReadStreamInt();
                            spritesheet_ref = ReadStreamInt();
                        }
                        for(float x = x1; XF_LESS_THAN_YF(x, (x2+1)); ++x)
                            if(properties_flag[0])
                                insert_entity(ready_m(entity_type, GET_POSITION_BLOCK(x, position[1]), tex, spritesheet_ref, map), 
                                properties_flag, z_index, TRUE);
                            else
                                insert_entity(ready(entity_type, GET_POSITION_BLOCK(x, position[1]), map), properties_flag, z_index, TRUE);
                    } break;
                    case 'v': { // fill vertically of entity with identical properties
                        const int y1 = ReadStreamFloat(), y2 = ReadStreamInt();
                        position[0] = ReadStreamFloat();
                        if(properties_flag[3]) // check if entity is displayed in map
                            z_index = ReadStreamInt();
                        entity_type = (EntityInstance)ReadStreamInt();
                        if(properties_flag[0]) {
                            tex = ReadStreamInt();
                            spritesheet_ref = ReadStreamInt();
                        }
                        for(float y = y1; XF_LESS_THAN_YF(y, (y2+1)); ++y)
                            if(properties_flag[0])
                                insert_entity(ready_m(entity_type, GET_POSITION_BLOCK(position[0], y), tex, spritesheet_ref, map), 
                                properties_flag, z_index, TRUE);
                            else
                                insert_entity(ready(entity_type, GET_POSITION_BLOCK(position[0], y), map), properties_flag, z_index, TRUE);
                    } break;
                    case 'd': { // fill 2d square of entity with identical properties
                        const int x1 = ReadStreamFloat(), x2 = ReadStreamFloat(), y1 = ReadStreamFloat(), y2 = ReadStreamFloat();
                        if(properties_flag[3]) // check if entity is displayed in map
                            z_index = ReadStreamInt();
                        entity_type = (EntityInstance)ReadStreamInt();
                        if(properties_flag[0]) {
                            tex = ReadStreamInt();
                            spritesheet_ref = ReadStreamInt();
                        }
                        for(float x = x1; XF_LESS_THAN_YF(x, (x2+1)); ++x) 
                            for(float y = y1; XF_LESS_THAN_YF(y, (y2+1)); ++y)
                                if(properties_flag[0])
                                    insert_entity(ready_m(entity_type, GET_POSITION_BLOCK(x, y), tex, spritesheet_ref, map), 
                                    properties_flag, z_index, TRUE);
                                else
                                    insert_entity(ready(entity_type, GET_POSITION_BLOCK(x, y), map), properties_flag, z_index, TRUE);
                    } break;
                }
            }
        }
    }

    return map;
}

Map* on_ready_start() {
    return on_ready(START_MAP);
}

void propagate_exit(Map* map) {
    OffsetStreamWithMap(map->type);
    do {
        HydrateStreamWithNextLine();
    } while(ReadStreamChar() != 'T');
    while(HasNext())
        FreeTextureResource(ReadStreamInt());

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
        if(*itr == NULL)
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