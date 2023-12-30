#include "enemy_spider.h"

#include "../../core/visual_server.h"
#include "../../components/collision_map.h"
#include "../textures/texture_table.h"

#define WALKING_SPEED 1.5
#define SPIDER_MAX_HP 50
#define SPIDER_ATTACK_DAMAGE 25

#define ATTACK_ANIM1_DELAY 0.166666
#define ATTACK_ANIM2_DELAY 0.066666
#define ATTACK_ANIM3_DELAY 0.183333

void set_animation_es(Entity* entity, SpiderSpritesheetEnums start_state, void(*anim_proc)(Entity*, const float)) {
    Spider* instance = entity->instance;
    float temp = instance->accum_anim;
    while(XF_GREATER_THAN_OR_EQUAL_YF(temp, 0.016666))
        temp -= 0.016666;
    instance->accum_anim = temp;
    instance->state = start_state;
    instance->anim_proc = anim_proc;
    entity->redraw = TRUE;
}

void es_anim_idle(Entity* entity, const float delta) {
    Spider* instance = entity->instance;
    WAIT(instance->accum_anim, 0.25);
    instance->state += ((instance->state/SPIDER_MAX_COLUMN)%2)? -SPIDER_MAX_COLUMN : SPIDER_MAX_COLUMN ;
}

void es_anim_walk(Entity* entity, const float delta) {
    Spider* instance = entity->instance;
    WAIT(instance->accum_anim, 0.05);
    instance->state += instance->state%SPIDER_MAX_COLUMN? -1 : 1;
}

void es_anim_attack_front(Entity* entity, const float delta) {
    Spider* instance = entity->instance;
    switch(instance->state/SPIDER_MAX_COLUMN) {
        case 0:
            WAIT(instance->accum_anim, ATTACK_ANIM1_DELAY);
            instance->state += SPIDER_MAX_COLUMN;
            if(instance->state%SPIDER_MAX_COLUMN) { // back
                set_extent(entity, 8, 24, 8, 8);
                add_collision_value_on(entity->map, HITBOX_LAYER, SPIDER_ATTACK_DAMAGE, entity->position[0], entity->position[1]-(SPIDER_PARTITION_Y/2));
            } else {
                set_extent(entity, 8, 8, 8, 24);
                add_collision_value_on(entity->map, HITBOX_LAYER, SPIDER_ATTACK_DAMAGE, entity->position[0], entity->position[1]+(SPIDER_PARTITION_Y/2));
            }
            break;
        case 1:
            WAIT(instance->accum_anim, ATTACK_ANIM2_DELAY);
            instance->state += SPIDER_MAX_COLUMN*2;
            set_extent(entity, 8, 8, 8, 8);
            if(instance->state%SPIDER_MAX_COLUMN) // back
                null_collision_on(entity->map, HITBOX_LAYER, entity->position[0], entity->position[1]+(SPIDER_PARTITION_Y/2));
            else
                null_collision_on(entity->map, HITBOX_LAYER, entity->position[0], entity->position[1]-(SPIDER_PARTITION_Y/2));
            break;
        case 3:
            WAIT(instance->accum_anim, ATTACK_ANIM3_DELAY);
            set_animation_es(entity, instance->state%SPIDER_MAX_COLUMN? ESBACKIDLE0 : ESFRONTIDLE0, es_anim_idle);
        default:
            return;
    }
    entity->redraw = TRUE;
}

void es_anim_attack_side(Entity* entity, const float delta) {
    Spider* instance = entity->instance;
    switch(instance->state%SPIDER_MAX_COLUMN) {
        case 0:
            WAIT(instance->accum_anim, ATTACK_ANIM1_DELAY);
            instance->state++;
            if(entity->flip) { // left
                set_extent(entity, 24, 8, 8, 8);
                add_collision_value_on(entity->map, HITBOX_LAYER, SPIDER_ATTACK_DAMAGE, entity->position[0]-(SPIDER_PARTITION_X/2), entity->position[1]);
            } else {
                set_extent(entity, 8, 8, 24, 8);
                add_collision_value_on(entity->map, HITBOX_LAYER, SPIDER_ATTACK_DAMAGE, entity->position[0]+(SPIDER_PARTITION_X/2), entity->position[1]);
            }
            break;
        case 1:
            WAIT(instance->accum_anim, ATTACK_ANIM1_DELAY);
            instance->state++;
            set_extent(entity, 8, 8, 8, 8);
            if(entity->flip)  // left
                null_collision_on(entity->map, HITBOX_LAYER, entity->position[0]-(SPIDER_PARTITION_X/2), entity->position[1]);
            else
                null_collision_on(entity->map, HITBOX_LAYER, entity->position[0]+(SPIDER_PARTITION_X/2), entity->position[1]);
            break;
        case 3:
            WAIT(instance->accum_anim, ATTACK_ANIM1_DELAY);
            set_animation_es(entity, (instance->state/SPIDER_MAX_COLUMN)%4? ESBACKIDLE0 : ESFRONTIDLE0, es_anim_idle);
        default:
            return;
    }
    entity->redraw = TRUE;
}

void es_anim_stun(Entity* entity, const float delta) {
    Spider* instance = entity->instance;
    WAIT(instance->accum_anim , 0.116666);
    switch(instance->state) {
        case ESFRONTSTUN:
            set_animation_es(entity, ESFRONTIDLE0, es_anim_idle);
            break;
        case ESBACKSTUN:
            set_animation_es(entity, ESBACKIDLE0, es_anim_idle);
        default: 
            return;
    }
    entity->redraw = TRUE;
}

Entity* ReadySpider(const float position_x, const float position_y, Map* created_by) {
    Entity* entity = calloc(1, sizeof(Entity));
    entity->type = ENEMY_SPIDER_ENTITY;
    entity->map = created_by;
    
    LoadTextureResource(SPIDER_SPRITESHEET);
    entity->texture_ref = SPIDER_SPRITESHEET;
    entity->redraw = TRUE;
    
    set_extent(entity, 8, 8, 8, 8);
    set_position(entity, position_x, position_y);
    add_collision_on(entity->map, COLLISION_LAYER, entity->position);

    Spider* instance = calloc(1, sizeof(Spider));
    entity->instance = instance;
    instance->hp = SPIDER_MAX_HP;
    set_animation_es(entity, ESFRONTIDLE0, es_anim_idle);

    return entity;
}

void ExitSpider(Entity* entity) {
    FreeTextureResource(SPIDER_SPRITESHEET);
    if(((Spider*)entity->instance)->state != ESDEATH)
        remove_collision_on(entity->map, COLLISION_LAYER, entity->position);
}

void ProcessSpider(Entity* entity, const float delta) {
    Spider* instance = entity->instance;

    if(instance->state == ESDEATH)
        return;

    unsigned short damage = get_collision_on(entity->map, HITBOX_LAYER, entity->position);
    if(damage) {
        null_collision_on(entity->map, HITBOX_LAYER, entity->position[0], entity->position[1]);
        instance->anim_proc(entity, ATTACK_ANIM1_DELAY + ATTACK_ANIM2_DELAY); // removes hitbox on hitbox layer if attacking
        if(instance->hp <= damage) {
            instance->state = ESDEATH;
            entity->redraw = TRUE;
            remove_collision_on(entity->map, COLLISION_LAYER, entity->position);
            printf("[LOG]: You have Slain a Spider!!!\n");
            return;
        }
        instance->hp -= damage;

        Point2U8 spritesheet_grid = {instance->state%SPIDER_MAX_COLUMN, instance->state/SPIDER_MAX_COLUMN};
        if(spritesheet_grid[0] == 3 || spritesheet_grid[1] == 5 || 
            (X2_INBETWEEN_X1_X3(spritesheet_grid[0], -1, 2) && X2_INBETWEEN_X1_X3(spritesheet_grid[1], 1, 4)))
            set_animation_es(entity, ESBACKSTUN, es_anim_stun);
        else
            set_animation_es(entity, ESFRONTSTUN, es_anim_stun);
        return;
    }

    instance->accum_anim += delta;
    instance->anim_proc(entity, delta);
}

void GetSpiderSrcRectPos(Entity* entity, SDL_Rect* rect) {
    rect->x = (((Spider*)entity->instance)->state%SPIDER_MAX_COLUMN)*SPIDER_PARTITION_X;
    rect->y = (((Spider*)entity->instance)->state/SPIDER_MAX_COLUMN)*SPIDER_PARTITION_Y;
}
