#include "enemy_spider.h"

#include <math.h>

#include "../../core/visual_server.h"
#include "../../core/logger.h"
#include "../../core/global_store.h"
#include "../../components/collision_map.h"
#include "../textures/texture_table.h"

#define WALKING_SPEED 90
#define SPIDER_MAX_HP 50
#define SPIDER_ATTACK_DAMAGE 25

#define SPIDER_ATTACK_ANIM1_DELAY 0.166666
#define SPIDER_ATTACK_ANIM2_DELAY 0.066666
#define SPIDER_ATTACK_ANIM3_DELAY 0.183333

#define SPIDER_FRONT_CASES case ESFRONTIDLE0: case ESFRONTIDLE1: case ESFRONTWALK: case ESFRONTSTUN: case ESFRONTATTACKFRONT0: case ESFRONTATTACKFRONT1: case ESFRONTATTACKFRONT2: case ESFRONTATTACKSIDE0: case ESFRONTATTACKSIDE1: case ESFRONTATTACKSIDE2
#define SPIDER_BACK_CASE case ESBACKIDLE0: case ESBACKIDLE1: case ESBACKWALK: case ESBACKSTUN: case ESBACKATTACKFRONT0: case ESBACKATTACKFRONT1: case ESBACKATTACKFRONT2: case ESBACKATTACKSIDE0: case ESBACKATTACKSIDE1: case ESBACKATTACKSIDE2
#define SPIDER_ACTIVE_CASE case ESFRONTIDLE0: case ESFRONTIDLE1: case ESFRONTWALK: case ESBACKIDLE0: case ESBACKIDLE1: case ESBACKWALK

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
    WAIT(instance->accum_anim, 0.5);
    instance->state += instance->state%SPIDER_MAX_COLUMN? -1 : 1;
}

void es_anim_attack_front(Entity* entity, const float delta) {
    Spider* instance = entity->instance;
    switch(instance->state/SPIDER_MAX_COLUMN) {
        case 0:
            WAIT(instance->accum_anim, SPIDER_ATTACK_ANIM1_DELAY);
            instance->state += SPIDER_MAX_COLUMN;
            if((instance->state%SPIDER_MAX_COLUMN)%2) { // back facing
                set_extent(entity, 8, 24, 8, 8);
                add_collision_value_on(entity->map, HURTBOX_LAYER, SPIDER_ATTACK_DAMAGE, entity->position[0], entity->position[1]-(SPIDER_PARTITION_Y/2));
            } else { // font facing
                set_extent(entity, 8, 8, 8, 24); 
                add_collision_value_on(entity->map, HURTBOX_LAYER, SPIDER_ATTACK_DAMAGE, entity->position[0], entity->position[1]+(SPIDER_PARTITION_Y/2));
            }
            break;
        case 1:
            WAIT(instance->accum_anim, SPIDER_ATTACK_ANIM2_DELAY);
            instance->state += SPIDER_MAX_COLUMN*2;
            set_extent(entity, 8, 8, 8, 8);
            if((instance->state%SPIDER_MAX_COLUMN)%2) // back facing
                null_collision_on(entity->map, HURTBOX_LAYER, entity->position[0], entity->position[1]+(SPIDER_PARTITION_Y/2));
            else // front facing
                null_collision_on(entity->map, HURTBOX_LAYER, entity->position[0], entity->position[1]-(SPIDER_PARTITION_Y/2));
            break;
        case 3:
            WAIT(instance->accum_anim, SPIDER_ATTACK_ANIM3_DELAY);
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
            WAIT(instance->accum_anim, SPIDER_ATTACK_ANIM1_DELAY);
            instance->state++;
            if(entity->flip) { // left
                set_extent(entity, 24, 8, 8, 8);
                add_collision_value_on(entity->map, HURTBOX_LAYER, SPIDER_ATTACK_DAMAGE, entity->position[0]-(SPIDER_PARTITION_X/2), entity->position[1]);
            } else {
                set_extent(entity, 8, 8, 24, 8);
                add_collision_value_on(entity->map, HURTBOX_LAYER, SPIDER_ATTACK_DAMAGE, entity->position[0]+(SPIDER_PARTITION_X/2), entity->position[1]);
            }
            break;
        case 1:
            WAIT(instance->accum_anim, SPIDER_ATTACK_ANIM1_DELAY);
            instance->state += 2;
            set_extent(entity, 8, 8, 8, 8);
            if(entity->flip)  // left
                null_collision_on(entity->map, HURTBOX_LAYER, entity->position[0]-(SPIDER_PARTITION_X/2), entity->position[1]);
            else
                null_collision_on(entity->map, HURTBOX_LAYER, entity->position[0]+(SPIDER_PARTITION_X/2), entity->position[1]);
            break;
        case 3:
            WAIT(instance->accum_anim, SPIDER_ATTACK_ANIM1_DELAY);
            set_animation_es(entity, (instance->state/SPIDER_MAX_COLUMN)%4? ESBACKIDLE0 : ESFRONTIDLE0, es_anim_idle);
            entity->flip = SDL_FLIP_NONE;
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

void enemy_spider_ai(Entity* entity) {
    const EntityContainer* const player_container = FindEntityTag("player");
    if(!player_container)
        return;
    Spider* spider = entity->instance;
    Entity* player_ett = player_container->entity;
    const Point2f displacement = {player_ett->position[0]-entity->position[0], player_ett->position[1]-entity->position[1]};
    const float distance = sqrtf(displacement[0] * displacement[0] + displacement[1] * displacement[1]);
    if(XF_GREATER_THAN_YF(distance, 88.0)) {
        if(spider->anim_proc != es_anim_idle)
            switch(spider->state) {
                SPIDER_FRONT_CASES:
                    set_animation_es(entity, ESFRONTIDLE0, es_anim_idle);
                break;
                SPIDER_BACK_CASE:
                    set_animation_es(entity, ESBACKIDLE0, es_anim_idle);
                break;
                default:
        }
        spider->intent = VOID_INTENT;
        return;
    }
    const Point2f normalized = {((float)abs(displacement[0]))/distance, ((float)abs(displacement[1]))/distance};
    if(XF_LESS_THAN_YF(distance, 28.0) && XF_LESS_THAN_YF(spider->wait_before_attacking_again, 0.0)) {
        if((rand()%10) > 5){
            spider->intent = VOID_INTENT;
            if((rand()%100) < normalized[1]*100) { // on the y axis
                set_animation_es(entity, displacement[1] > 0? ESFRONTATTACKFRONT0 : ESBACKATTACKFRONT0, es_anim_attack_front);
            } else { // attack on x axis
                entity->flip = displacement[0] < 0? SDL_FLIP_HORIZONTAL: SDL_FLIP_NONE;
                switch(spider->state){
                    SPIDER_FRONT_CASES:
                        set_animation_es(entity, ESFRONTATTACKSIDE0, es_anim_attack_side);
                        break;
                    SPIDER_BACK_CASE:
                        set_animation_es(entity, ESBACKATTACKSIDE0, es_anim_attack_side);
                        break;
                    default:
                }
            }
            spider->wait_before_attacking_again = 0.166666;
            return;
        }
        spider->wait_before_attacking_again = 0.1;
    }
    if(spider->intent != VOID_INTENT && (rand()%100) > 30)
        return;
    
    Point2f position = {entity->position[0], entity->position[1]};
    const Point2U8 wall_toward_player = {
        displacement[0] < 0? 
            check_collision_with_vec_coord(entity->map, COLLISION_LAYER, position, -1, 0) : 
            check_collision_with_vec_coord(entity->map, COLLISION_LAYER, position, 1, 0), 
        displacement[1] < 0? 
            check_collision_with_vec_coord(entity->map, COLLISION_LAYER, position, 0, -1) : 
            check_collision_with_vec_coord(entity->map, COLLISION_LAYER, position, 0, 1)
    };
    const Point2U8 wall_away_player = {
        displacement[0] < 0? 
            check_collision_with_vec_coord(entity->map, COLLISION_LAYER, position, 1, 0) : 
            check_collision_with_vec_coord(entity->map, COLLISION_LAYER, position, -1, 0), 
        displacement[1] < 0? 
            check_collision_with_vec_coord(entity->map, COLLISION_LAYER, position, 0, 1) : 
            check_collision_with_vec_coord(entity->map, COLLISION_LAYER, position, 0, -1)
    };

    if(!(wall_toward_player[0] && wall_toward_player[1]) && (rand()%10) > 3) {
        if(wall_toward_player[0])
            spider->intent = displacement[1] < 0? INTENT_MOVE_BACKWARD : INTENT_MOVE_FORWARD;
        else if(wall_toward_player[1] || (rand()%100000000) < normalized[0]*100000000) {
            spider->intent = displacement[0] < 0? INTENT_MOVE_LEFTSIDE : INTENT_MOVE_RIGHTSIDE;
        } else
            spider->intent = displacement[1] < 0? INTENT_MOVE_BACKWARD : INTENT_MOVE_FORWARD;
    } else {
        if(wall_away_player[0] && wall_away_player[1])
            return;
        if(wall_away_player[0])
            spider->intent = displacement[1] < 0? INTENT_MOVE_FORWARD : INTENT_MOVE_BACKWARD;
        else if(wall_away_player[1] || rand()%2) {
            spider->intent = displacement[0] < 0? INTENT_MOVE_RIGHTSIDE : INTENT_MOVE_LEFTSIDE;
        } else
            spider->intent = displacement[1] < 0? INTENT_MOVE_FORWARD : INTENT_MOVE_BACKWARD;
    }
    set_animation_es(entity, displacement[1] < 0? ESBACKIDLE0 : ESFRONTIDLE0, es_anim_walk);
}

void spider_move_and_collide(Entity* entity, const float delta) {
    Spider* spider = entity->instance;
    Point2f next_pos;
    Point2S8 vector = {0, 0};
    switch(spider->intent) {
        case INTENT_MOVE_FORWARD: {
            next_pos[0] = entity->position[0];
            next_pos[1] = entity->position[1] + (WALKING_SPEED * delta);
            vector[1] = 1;
        } break;
        case INTENT_MOVE_BACKWARD: {
            next_pos[0] = entity->position[0];
            next_pos[1] = entity->position[1] - (WALKING_SPEED * delta);
            vector[1] = -1;
        } break;
        case INTENT_MOVE_LEFTSIDE: {
            next_pos[0] = entity->position[0] - (WALKING_SPEED * delta);
            next_pos[1] = entity->position[1];
            vector[0] = -1;
        } break;
        case INTENT_MOVE_RIGHTSIDE: {
            next_pos[0] = entity->position[0] + (WALKING_SPEED * delta);
            next_pos[1] = entity->position[1];
            vector[0] = 1;
        } break;
        default:
            return;
    }
    check_collision_with_vec(entity->map, COLLISION_LAYER, next_pos, vector);
    entity->position[0] = next_pos[0];
    entity->position[1] = next_pos[1];
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
    instance->intent = VOID_INTENT;
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
        instance->anim_proc(entity, SPIDER_ATTACK_ANIM1_DELAY + SPIDER_ATTACK_ANIM2_DELAY); // removes hitbox on hitbox layer if attacking
        set_extent(entity, 8, 8, 8, 8);

        instance->hp -= damage;
        if(instance->hp <= damage) {
            instance->state = ESDEATH;
            entity->redraw = TRUE;
            remove_collision_on(entity->map, COLLISION_LAYER, entity->position);
            WriteOnLog("[Killed]: Spider Slain!!!\n");
            return;
        }

        Point2U8 spritesheet_grid = {instance->state%SPIDER_MAX_COLUMN, instance->state/SPIDER_MAX_COLUMN};
        if(spritesheet_grid[0] == 3 || spritesheet_grid[1] == 5 || 
            (X2_INBETWEEN_X1_X3(spritesheet_grid[0], -1, 2) && X2_INBETWEEN_X1_X3(spritesheet_grid[1], 1, 4)))
            set_animation_es(entity, ESBACKSTUN, es_anim_stun);
        else
            set_animation_es(entity, ESFRONTSTUN, es_anim_stun);
        return;
    }
    
    remove_collision_on(entity->map, COLLISION_LAYER, entity->position); // ai needs to clear own collision box to work
    switch(instance->state) {
        SPIDER_ACTIVE_CASE:
            instance->wait_before_attacking_again -= delta;
            enemy_spider_ai(entity);
        default:
    }
    if(instance->intent != VOID_INTENT)
        spider_move_and_collide(entity, delta);
    add_collision_on(entity->map, COLLISION_LAYER, entity->position);
    instance->accum_anim += delta;
    instance->anim_proc(entity, delta);
}

void GetSpiderSrcRectPos(Entity* entity, SDL_Rect* rect) {
    rect->x = (((Spider*)entity->instance)->state%SPIDER_MAX_COLUMN)*SPIDER_PARTITION_X;
    rect->y = (((Spider*)entity->instance)->state/SPIDER_MAX_COLUMN)*SPIDER_PARTITION_Y;
}
