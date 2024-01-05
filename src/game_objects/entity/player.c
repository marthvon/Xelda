#include "player.h"
#include <string.h>

#include "../../core/input_handler.h"
#include "../../core/visual_server.h"
#include "../../components/collision_map.h"
#include "collision_table.h"

#define WALKING_SPEED 1.142857
#define DASH_SPEED 3.2
#define PLAYER_MAX_HP 100
#define PLAYER_MAX_STAMINA 100
#define PLAYER_ATTACK_DAMAGE 10

#define PLAYER_ATTACK_ANIM1_DELAY 0.133333
#define PLAYER_ATTACK_ANIM2_DELAY 0.083333
#define PLAYER_ATTACK_ANIM3_DELAY 0.116666

#define PLAYER_DASH_STARTUP_DELAY 0.03333
#define PLAYER_DASH_ACTIVE_DELAY 0.083333
#define PLAYER_DASH_RECOVERY_DELAY 0.05

#define PLAYER_FRONT_WALK_CASES case PFRONTWALK0: case PFRONTWALK1: case PFRONTWALK2
#define PLAYER_BACK_WALK_CASES case PBACKWALK0: case PBACKWALK1: case PBACKWALK2
#define PLAYER_LEFT_WALK_CASES case PLEFTWALK0: case PLEFTWALK1: case PLEFTWALK2
#define PLAYER_RIGHT_WALK_CASES case PRIGHTWALK0: case PRIGHTWALK1: case PRIGHTWALK2

#define PLAYER_DASH_FRONT_CASES case PFRONTDASHFACINGFRONT0: case PFRONTDASHFACINGFRONT1: case PBACKDASHFACINGFRONT0: case PBACKDASHFACINGFRONT1: \
                                case PRIGHTDASHFACINGFRONT0: case PRIGHTDASHFACINGFRONT1: case PLEFTDASHFACINGFRONT0: case PLEFTDASHFACINGFRONT1
#define PLAYER_DASH_BACK_CASES case PFRONTDASHFACINGBACK0: case PFRONTDASHFACINGBACK1: case PBACKDASHFACINGBACK0: case PBACKDASHFACINGBACK1: \
                                case PRIGHTDASHFACINGBACK0: case PRIGHTDASHFACINGBACK1: case PLEFTDASHFACINGBACK0: case PLEFTDASHFACINGBACK1
#define PLAYER_DASH_RIGHT_CASES case PFRONTDASHFACINGRIGHT0:  case PFRONTDASHFACINGRIGHT1:  case PBACKDASHFACINGRIGHT0:  case PBACKDASHFACINGRIGHT1: \
                                case PRIGHTDASHFACINGRIGHT0:  case PRIGHTDASHFACINGRIGHT1:  case PLEFTDASHFACINGRIGHT0:  case PLEFTDASHFACINGRIGHT1
#define PLAYER_DASH_LEFT_CASES case PFRONTDASHFACINGLEFT0: case PFRONTDASHFACINGLEFT1: case PBACKDASHFACINGLEFT0: case PBACKDASHFACINGLEFT1: \
                                case PRIGHTDASHFACINGLEFT0: case PRIGHTDASHFACINGLEFT1: case PLEFTDASHFACINGLEFT0: case PLEFTDASHFACINGLEFT1


#define PLAYER_ACTIVE_FRONT_CASES case PFRONTIDLE0: case PFRONTIDLE1: case PFRONTIDLE2: PLAYER_FRONT_WALK_CASES
#define PLAYER_ACTIVE_BACK_CASES case PBACKIDLE0: case PBACKIDLE1: case PBACKIDLE2: PLAYER_BACK_WALK_CASES
#define PLAYER_ACTIVE_LEFT_CASES case PLEFTIDLE0: case PLEFTIDLE1: PLAYER_LEFT_WALK_CASES
#define PLAYER_ACTIVE_RIGHT_CASES case PRIGHTIDLE0: case PRIGHTIDLE1: PLAYER_RIGHT_WALK_CASES

#define PLAYER_FRONT_CASES PLAYER_ACTIVE_FRONT_CASES: PLAYER_DASH_FRONT_CASES
#define PLAYER_BACK_CASES PLAYER_ACTIVE_BACK_CASES: PLAYER_DASH_BACK_CASES
#define PLAYER_LEFT_CASES PLAYER_ACTIVE_LEFT_CASES: PLAYER_DASH_LEFT_CASES
#define PLAYER_RIGHT_CASES PLAYER_ACTIVE_RIGHT_CASES: PLAYER_DASH_RIGHT_CASES

#define PLAYER_ALL_ACTIVE_CASES PLAYER_ACTIVE_FRONT_CASES: PLAYER_ACTIVE_BACK_CASES: PLAYER_ACTIVE_LEFT_CASES: PLAYER_ACTIVE_RIGHT_CASES

/* ANIMATION START */
void set_animation_p(Entity* entity, PlayerSpritesheetEnums start_state, void(*anim_proc)(Entity*, const float)) {
    Player* instance = entity->instance;
    float temp = instance->accum_anim;
    while(XF_GREATER_THAN_OR_EQUAL_YF(temp, 0.016666))
        temp -= 0.016666;
    instance->accum_anim = temp;
    memset(instance->anim_metadata, 0, 2*sizeof(Byte));
    instance->state = start_state;
    instance->anim_proc = anim_proc;
    entity->redraw = TRUE;
}

void player_anim_idle(Entity* entity, const float delta) {
    Player* instance = entity->instance;
    if(instance->state < PRIGHTIDLE0) {
        WAIT(instance->accum_anim, 0.333333);
        switch(instance->state) { // looping animation by zig zag
            case PFRONTIDLE0: case PBACKIDLE0:
                instance->anim_metadata[0] = TRUE; break; // go forward
            case PFRONTIDLE2: case PBACKIDLE2:
                instance->anim_metadata[0] = FALSE; // go reverse
            default:
        }
        if(instance->anim_metadata[0])
            ++(instance->state);
        else
            --(instance->state);
        entity->redraw = TRUE;
        return;
    }
    WAIT(instance->accum_anim, 0.333333);
    if(instance->state == PRIGHTIDLE1 || instance->state == PLEFTIDLE1) 
        --(instance->state);
    else
        ++(instance->state);
    entity->redraw = TRUE;
}

void player_anim_attack_front(Entity* entity, const float delta) {
    Player* instance = entity->instance;
    switch(instance->state) {
        case PBLADEUPPERLEFT:
            WAIT(instance->accum_anim, PLAYER_ATTACK_ANIM1_DELAY);
            instance->state = PATTACKFRONT;
            set_extent(entity, 8, 8, 8, 24);
            add_collision_value_on(entity->map, HITBOX_LAYER, PLAYER_ATTACK_DAMAGE, entity->position[0], entity->position[1]+PLAYER_PARTITION_Y);
            break;
        case PATTACKFRONT:
            WAIT(instance->accum_anim, PLAYER_ATTACK_ANIM2_DELAY);
            instance->state = PBLADELOWERRIGHT;
            set_extent(entity, 8, 8, 8, 8);
            null_collision_on(entity->map, HITBOX_LAYER, entity->position[0], entity->position[1]+PLAYER_PARTITION_Y);
            break;
        case PBLADELOWERRIGHT:
            WAIT(instance->accum_anim, PLAYER_ATTACK_ANIM3_DELAY);
            set_animation_p(entity, PFRONTIDLE0, player_anim_idle);
        default:
            return;
    }
    entity->redraw = TRUE;
}

void player_anim_attack_back(Entity* entity, const float delta) {
    Player* instance = entity->instance;
    switch(instance->state) {
        case PBLADEBACKRIGHT:
            WAIT(instance->accum_anim, PLAYER_ATTACK_ANIM1_DELAY);
            instance->state = PATTACKBEHIND;
            set_extent(entity, 8, 24, 8, 8);
            add_collision_value_on(entity->map, HITBOX_LAYER, PLAYER_ATTACK_DAMAGE, entity->position[0], entity->position[1]-PLAYER_PARTITION_Y);
            break;
        case PATTACKBEHIND:
            WAIT(instance->accum_anim, PLAYER_ATTACK_ANIM2_DELAY);
            instance->state = PBLADEBACKLEFT;
            set_extent(entity, 8, 8, 8, 8);
            null_collision_on(entity->map, HITBOX_LAYER, entity->position[0], entity->position[1]-PLAYER_PARTITION_Y);
            break;
        case PBLADEBACKLEFT:
            WAIT(instance->accum_anim, PLAYER_ATTACK_ANIM3_DELAY);
            set_animation_p(entity, PBACKIDLE0, player_anim_idle);
        default:
            return;
    }
    entity->redraw = TRUE;
}

void player_anim_attack_left(Entity* entity, const float delta) {
    Player* instance = entity->instance;
    switch(instance->state) {
        case PBLADEBACKRIGHT:
            WAIT(instance->accum_anim, PLAYER_ATTACK_ANIM1_DELAY);
            instance->state = PATTACKLEFT;
            set_extent(entity, 24, 8, 8, 8);
            add_collision_value_on(entity->map, HITBOX_LAYER, PLAYER_ATTACK_DAMAGE, entity->position[0]-PLAYER_PARTITION_X, entity->position[1]);
            break;
        case PATTACKLEFT:
            WAIT(instance->accum_anim, PLAYER_ATTACK_ANIM2_DELAY);
            instance->state = PBLADELOWERLEFT;
            set_extent(entity, 8, 8, 8, 8);
            null_collision_on(entity->map, HITBOX_LAYER, entity->position[0]-PLAYER_PARTITION_X, entity->position[1]);
            break;
        case PBLADELOWERLEFT:
            WAIT(instance->accum_anim, PLAYER_ATTACK_ANIM3_DELAY);
            set_animation_p(entity, PLEFTIDLE0, player_anim_idle);
        default:
            return;
    }
    entity->redraw = TRUE;
}

void player_anim_attack_right(Entity* entity, const float delta) {
    Player* instance = entity->instance;
    switch(instance->state) {
        case PBLADELOWERLEFT:
            WAIT(instance->accum_anim, PLAYER_ATTACK_ANIM1_DELAY);
            instance->state = PATTACKRIGHT;
            set_extent(entity, 8, 8, 24, 8);
            add_collision_value_on(entity->map, HITBOX_LAYER, PLAYER_ATTACK_DAMAGE, entity->position[0]+PLAYER_PARTITION_X, entity->position[1]);
            break;
        case PATTACKRIGHT:
            WAIT(instance->accum_anim, PLAYER_ATTACK_ANIM2_DELAY);
            instance->state = PBLADEUPPERRIGHT;
            set_extent(entity, 8, 8, 8, 8);
            null_collision_on(entity->map, HITBOX_LAYER, entity->position[0]+PLAYER_PARTITION_X, entity->position[1]);
            break;
        case PBLADEUPPERRIGHT:
            WAIT(instance->accum_anim, PLAYER_ATTACK_ANIM3_DELAY);
            set_animation_p(entity, PRIGHTIDLE0, player_anim_idle);
        default:
            return;
    }
    entity->redraw = TRUE;
}

void player_anim_walk(Entity* entity, const float delta) {
    Player* instance = entity->instance;
    switch(instance->state/PLAYER_MAX_COLUMN) { // looping animation through cycle
        case 0: 
            WAIT(instance->accum_anim, 0.033333);
            instance->state += PLAYER_MAX_COLUMN;
            break;
        case 1: // start here
            WAIT(instance->accum_anim, 0.083333);
            instance->state += PLAYER_MAX_COLUMN;
            break;
        case 2:
            WAIT(instance->accum_anim, 0.1);
            instance->state += PLAYER_MAX_COLUMN;
            break;
        case 3:
            WAIT(instance->accum_anim, 0.05);
            instance->state %= PLAYER_MAX_COLUMN;
        default: return;
    }
    entity->redraw = TRUE;
}

void player_anim_stun(Entity* entity, const float delta) {
    Player* instance = entity->instance;
    WAIT(instance->accum_anim, 0.5);
    switch(instance->state) {
        case PSTUNFRONT:
            set_animation_p(entity, PFRONTIDLE0, player_anim_idle);
            break;
        case PSTUNBACK:
            set_animation_p(entity, PBACKIDLE0, player_anim_idle);
            break;
        case PSTUNLEFT:
            set_animation_p(entity, PLEFTIDLE0, player_anim_idle);
            break;
        case PSTUNRIGHT:
            set_animation_p(entity, PRIGHTIDLE0, player_anim_idle);
            break;
        default: return;
    }
    entity->redraw = TRUE;
}

void player_dash(Entity* entity, const float delta) {
    Player* instance = entity->instance;
    if(!(instance->anim_metadata[0]))
        switch((instance->state/10) == 4) {
            case 4: { // startup of either front or back dash
                WAIT(instance->accum_anim, PLAYER_DASH_STARTUP_DELAY);
                instance->state += 30;
                if((instance->state%10)%2) // Back Dash
                    set_extent(entity, 8, 8, 8, 24);
                else // Front Dash
                    set_extent(entity, 8, 24, 8, 8);
            } return;
            case 7: { // actively dashing on either front or back dash
                WAIT(instance->accum_anim, PLAYER_DASH_ACTIVE_DELAY);
                instance->state -= 30;
                set_extent(entity, 8, 8, 8, 8);
                instance->anim_metadata[0] = TRUE;
            } return;
            case 5: { // startup of either left or right dash
                WAIT(instance->accum_anim, PLAYER_DASH_STARTUP_DELAY);
                if((instance->state%10)%2) { // Right Dash
                    set_extent(entity, 24, 8, 8, 8);
                    instance->state += 40;
                } else { // Left Dash
                    set_extent(entity, 8, 8, 24, 8);
                    instance->state += 10;
                }
            } return;
            case 6: { // actively dashing on left dash
                WAIT(instance->accum_anim, PLAYER_DASH_ACTIVE_DELAY);
                instance->state -= 10;
                set_extent(entity, 8, 8, 8, 8);
                instance->anim_metadata[0] = TRUE;
            } return;
            case 9: { // actively dashing on right dash
                WAIT(instance->accum_anim, PLAYER_DASH_ACTIVE_DELAY);
                instance->state -= 40;
                set_extent(entity, 8, 8, 8, 8);
                instance->anim_metadata[0] = TRUE;
            } return;
    }
    // RECOVERY transition back to idle 
    WAIT(instance->accum_anim, PLAYER_DASH_RECOVERY_DELAY);
    switch((instance->state%10)>>1) {
        case 0:
            set_animation_p(entity, PFRONTIDLE0, player_anim_idle);
            break;
        case 1:
            set_animation_p(entity, PBACKIDLE0, player_anim_idle);
            break;
        case 2:
            set_animation_p(entity, PRIGHTIDLE0, player_anim_idle);
            break;
        case 3:
            set_animation_p(entity, PLEFTIDLE0, player_anim_idle);
            break;
    }
}

void player_fire_arrow(Entity* entity, const float delta) {
    Player* instance = entity->instance;
    WAIT(instance->accum_anim, 0.2);
    switch(instance->state) {
        case PFIREARROWFRONT: 
            set_animation_p(entity, PFRONTIDLE0, player_anim_idle);
        break;
        case PFIREARROWBACK: 
            set_animation_p(entity, PBACKIDLE0, player_anim_idle);
        break;
        case PFIREARROWRIGHT: 
            set_animation_p(entity, PRIGHTIDLE0, player_anim_idle);
        break;
        case PFIREARROWLEFT: 
            set_animation_p(entity, PLEFTIDLE0, player_anim_idle);
        default:
    }
}

void player_plant(Entity* entity, const float delta) {
    Player* instance = entity->instance;
    WAIT(instance->accum_anim, 0.166666);
    switch(instance->state) {
        case PPLANTFRONT: 
            set_animation_p(entity, PFRONTIDLE0, player_anim_idle);
        break;
        case PPLANTBACK: 
            set_animation_p(entity, PBACKIDLE0, player_anim_idle);
        break;
        case PPLANTRIGHT: 
            set_animation_p(entity, PRIGHTIDLE0, player_anim_idle);
        break;
        case PPLANTLEFT: 
            set_animation_p(entity, PLEFTIDLE0, player_anim_idle);
        default:
    }
}

/* ANIMATION END */

/* MOVE AND COLLIDE START */
void move_and_collide(Entity* entity, const float delta) {
    Player* instance = entity->instance;
    remove_collision_on(entity->map, COLLISION_LAYER, entity->position);
    if(!check_collision_with_vec(entity->map, COLLISION_LAYER, entity->position, instance->move_controller)) {
        switch (instance->state) {
            PLAYER_FRONT_WALK_CASES:
                entity->position[1] += WALKING_SPEED;
                entity->redraw = TRUE;
            break;
            PLAYER_BACK_WALK_CASES:
                entity->position[1] -= WALKING_SPEED;
                entity->redraw = TRUE;
            break;
            PLAYER_LEFT_WALK_CASES:
                entity->position[0] -= WALKING_SPEED;
                entity->redraw = TRUE;
            break;
            PLAYER_RIGHT_WALK_CASES:
                entity->position[0] += WALKING_SPEED;
                entity->redraw = TRUE;
            break;
            default:
        }
    }
    add_collision_on(entity->map, COLLISION_LAYER, entity->position);
}
/* MOVE AND COLLIDE END */

/* INPUT START */
BOOL move_input_x(Player* instance, BOOL check_held) {
    const KeyState left = GetStateOfKey(SDL_SCANCODE_A), right = GetStateOfKey(SDL_SCANCODE_D);
    if((check_held? isHeld(left) : (left == PRESSED)) || (instance->move_controller[0] == 1 && right == RELEASED))
        --instance->move_controller[0];
    else if ((check_held? isHeld(right) : (right == PRESSED)) || (instance->move_controller[0] == -1 && left == RELEASED))
        ++instance->move_controller[0];
    else
        return FALSE;
    return TRUE;
}

BOOL move_input_y(Player* instance, BOOL check_held) {
    const KeyState up = GetStateOfKey(SDL_SCANCODE_W), down = GetStateOfKey(SDL_SCANCODE_S);
    if((check_held? isHeld(up) : (up == PRESSED)) || (instance->move_controller[1] == 1 && down == RELEASED))
        --instance->move_controller[1];
    else if ((check_held? isHeld(down) : (down == PRESSED)) || (instance->move_controller[1] == -1 && up == RELEASED))
        ++instance->move_controller[1];
    else
        return FALSE;
    return TRUE;
}

// returns TRUE if input changes player state and FALSE when not
BOOL move_input(Player* instance) {
    if(instance->move_controller[1] == 0) {                                              // if y is active then axis lock in x
        if(move_input_x(instance, FALSE)) {
            if(instance->move_controller[0] == 0)                                        // y axis is locked or x axis is released then,
                move_input_y(instance, TRUE);                                            // release axis lock in y
            return TRUE;
        }
        return instance->move_controller[0] == 0? move_input_y(instance, FALSE) : FALSE; // default{0,0} neutral then x axis lock else nothing happens
    }
    if(move_input_y(instance, FALSE)) {
        if(instance->move_controller[1] == 0)                                            // when y axis is released
            move_input_x(instance, TRUE);                                                // release axis lock in x
        return TRUE;
    }
    return FALSE;
}

void InputPlayer(Entity* entity) {
    Player* instance = (Player*)entity->instance;
    switch(instance->state) {
        PLAYER_ALL_ACTIVE_CASES:
            break;
        default:
            return;
    }
    
    if(instance->hasSword && GetStateOfKey(SDL_SCANCODE_SPACE) == PRESSED) {
        memset(instance->move_controller, 0, sizeof(instance->move_controller));
        switch (instance->state){
            PLAYER_ACTIVE_FRONT_CASES:
                set_animation_p(entity, PBLADEUPPERLEFT, player_anim_attack_front); break;
            PLAYER_ACTIVE_BACK_CASES:
                set_animation_p(entity, PBLADEBACKRIGHT, player_anim_attack_back); break;
            PLAYER_ACTIVE_LEFT_CASES:
                set_animation_p(entity, PBLADEBACKRIGHT, player_anim_attack_left); break;
            PLAYER_ACTIVE_RIGHT_CASES:
                set_animation_p(entity, PBLADELOWERLEFT, player_anim_attack_right); break;
            default:
        }
        return;
    }

    if(instance->last_interaction[0] == -1 && GetStateOfKey(SDL_SCANCODE_F) == PRESSED) {
        Point2f position = {entity->position[0], entity->position[1]};
        switch(instance->state) {
            PLAYER_ACTIVE_FRONT_CASES:
                position[1] += PLAYER_PARTITION_X>>1; break;
            PLAYER_ACTIVE_BACK_CASES:
                position[1] -= EPSILON+(PLAYER_PARTITION_X>>1); break;
            PLAYER_ACTIVE_LEFT_CASES:
                position[0] -= EPSILON+(PLAYER_PARTITION_Y>>1); break;
            PLAYER_ACTIVE_RIGHT_CASES:
                position[0] += PLAYER_PARTITION_Y>>1; break;
            default:
        }
        add_collision_on(entity->map, INTERACTION_LAYER, position);
        memcpy(instance->last_interaction, position, sizeof(Point2f));
    }

    if(!move_input(instance))
        return;
    if(instance->move_controller[0] == 0 && instance->move_controller[1] == 0) {
        set_animation_p(entity, 
            instance->state%PLAYER_MAX_COLUMN,
            player_anim_idle
        );
        return;
    }
    if(isHeld(GetStateOfKey(SDL_SCANCODE_LSHIFT))) { // dash
        unsigned short new_state = (instance->move_controller[0]? 
            (40 + ((instance->move_controller[1] & 0b10)>>1)) : (50 + ((instance->move_controller[0]+2)>>1)));
        switch(instance->state) {
            case PLAYER_ACTIVE_FRONT_CASES:
                break;
            case PLAYER_ACTIVE_BACK_CASES:
                new_state += 2;
            break;
            case PLAYER_ACTIVE_RIGHT_CASES:
                new_state += 4;
            break;
            case PLAYER_ACTIVE_LEFT_CASES:
                new_state += 6;
            default:
        }
        set_animation_p(entity, new_state, player_dash);
        return;
    }
    set_animation_p(entity,
        instance->move_controller[0] != 0? (
            instance->move_controller[0] == -1? PLEFTWALK0 : PRIGHTWALK0
        ) : (instance->move_controller[1] == -1? PBACKWALK0 : PFRONTWALK0),
        player_anim_walk
    );
}
/* INPUT END */

Entity* ReadyPlayer(const float position_x, const float position_y, Map* created_by) {
    Entity* entity = calloc(1, sizeof(Entity));
    entity->type = PLAYER_ENTITY;
    entity->map = created_by;

    LoadTextureResource(PLAYER_SPRITESHEET);
    entity->texture_ref = PLAYER_SPRITESHEET;
    entity->redraw = TRUE;
    
    set_extent(entity, 8, 8, 8, 8);
    set_position(entity, position_x, position_y);
    add_collision_on(entity->map, COLLISION_LAYER, entity->position);

    Player* instance = calloc(1, sizeof(Player));
    entity->instance = instance;
    instance->prev_map = VOID_MAP;
    instance->hp = PLAYER_MAX_HP;
    instance->last_interaction[0] = -1;
    set_animation_p(entity, PFRONTIDLE0, player_anim_idle);

    return entity;
}

void ExitPlayer(Entity* entity) {
    FreeTextureResource(PLAYER_SPRITESHEET);
    remove_collision_on(entity->map, COLLISION_LAYER, entity->position);
}

void ProcessPlayer(Entity* entity, const float delta) {
    Player* instance = ((Player*)entity->instance);

    if(instance->state == PDEATH)
        return;

    int warp = -1+get_collision_on(entity->map, WARP_EVENT_LAYER, entity->position);
    if(warp > VOID_MAP) {
        entity->map->switch_map = (CollisionType)(warp);
        instance->prev_map = entity->map->type;
        return;
    }
    
    if(instance->last_interaction[0] != -1) {
        remove_collision_on(entity->map, INTERACTION_LAYER, instance->last_interaction);
        instance->last_interaction[0] = -1;
    }
    
    unsigned short damage = get_collision_on(entity->map, HITBOX_LAYER, entity->position);
    if(damage) {
        null_collision_on(entity->map, HITBOX_LAYER, entity->position[0], entity->position[1]);
        instance->anim_proc(entity, PLAYER_ATTACK_ANIM1_DELAY + PLAYER_ATTACK_ANIM2_DELAY); // removes hitbox on collision layer if attacking
        if(instance->hp <= damage) {
            instance->state = PDEATH;
            entity->redraw = TRUE;
            printf("You have Died...\n");
            return;
        }
        instance->hp -= damage;

        switch(instance->state) {
            PLAYER_FRONT_CASES:
                set_animation_p(entity, PSTUNFRONT, player_anim_stun); break;
            PLAYER_BACK_CASES:
                set_animation_p(entity, PSTUNBACK, player_anim_stun); break;
            PLAYER_LEFT_CASES:
                set_animation_p(entity, PSTUNLEFT, player_anim_stun); break;
            PLAYER_RIGHT_CASES:
                set_animation_p(entity, PSTUNRIGHT, player_anim_stun); break;
            default:
                if(instance->anim_proc == player_anim_attack_back)
                    set_animation_p(entity, PSTUNBACK, player_anim_stun);
                else if(instance->anim_proc == player_anim_attack_left)
                    set_animation_p(entity, PSTUNLEFT, player_anim_stun);
                if(instance->anim_proc == player_anim_attack_right)
                    set_animation_p(entity, PSTUNRIGHT, player_anim_stun);
                else
                    set_animation_p(entity, PSTUNFRONT, player_anim_stun);
        }
        return;
    }
    
    instance->accum_anim += delta;
    instance->anim_proc(entity, delta);
    move_and_collide(entity, delta);
}

void GetPlayerSrcRectPos(Entity* entity, SDL_Rect* rect) {
    rect->x = (((Player*)entity->instance)->state%PLAYER_MAX_COLUMN)*PLAYER_PARTITION_X;
    rect->y = (((Player*)entity->instance)->state/PLAYER_MAX_COLUMN)*PLAYER_PARTITION_Y;
}