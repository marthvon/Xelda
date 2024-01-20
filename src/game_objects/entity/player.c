#include "player.h"
#include <string.h>

#include "../../core/input_handler.h"
#include "../../core/visual_server.h"
#include "../../core/logger.h"
#include "../../components/collision_map.h"
#include "collision_table.h"

#define WALKING_SPEED 68.571428
#define DASH_SPEED 720
//548.571428

#define PLAYER_MAX_HP 100
#define HP_BLOCK 10
#define PLAYER_MAX_STAMINA 100
#define STAMINA_BLOCK 10
#define STAMINA_RECOVERY_RATE 40
#define PLAYER_ATTACK_DAMAGE 10

#define PLAYER_ATTACK_ANIM1_DELAY 0.133333
#define PLAYER_ATTACK_ANIM2_DELAY 0.083333
#define PLAYER_ATTACK_ANIM3_DELAY 0.116666

#define PLAYER_DASH_STARTUP_DELAY 0.033333
#define PLAYER_DASH_ACTIVE_DELAY 0.133333
#define PLAYER_DASH_RECOVERY_DELAY 0.066666

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

#define PLAYER_FRONTDASH_CASES case PFRONTDASHFACINGBACK1: case PFRONTDASHFACINGFRONT1: case PFRONTDASHFACINGLEFT1: case PFRONTDASHFACINGRIGHT1
#define PLAYER_BACKDASH_CASES case PBACKDASHFACINGBACK1: case PBACKDASHFACINGFRONT1: case PBACKDASHFACINGLEFT1: case PBACKDASHFACINGRIGHT1
#define PLAYER_RIGHTDASH_CASES case PRIGHTDASHFACINGBACK1: case PRIGHTDASHFACINGFRONT1: case PRIGHTDASHFACINGLEFT1: case PRIGHTDASHFACINGRIGHT1
#define PLAYER_LEFTDASH_CASES case PLEFTDASHFACINGBACK1: case PLEFTDASHFACINGFRONT1: case PLEFTDASHFACINGLEFT1: case PLEFTDASHFACINGRIGHT1

#define PLAYER_ACTIVE_FRONT_CASES case PFRONTIDLE0: case PFRONTIDLE1: case PFRONTIDLE2: PLAYER_FRONT_WALK_CASES
#define PLAYER_ACTIVE_BACK_CASES case PBACKIDLE0: case PBACKIDLE1: case PBACKIDLE2: PLAYER_BACK_WALK_CASES
#define PLAYER_ACTIVE_LEFT_CASES case PLEFTIDLE0: case PLEFTIDLE1: PLAYER_LEFT_WALK_CASES
#define PLAYER_ACTIVE_RIGHT_CASES case PRIGHTIDLE0: case PRIGHTIDLE1: PLAYER_RIGHT_WALK_CASES

#define PLAYER_FRONT_CASES PLAYER_ACTIVE_FRONT_CASES: PLAYER_DASH_FRONT_CASES: case PFIREARROWFRONT: case PPLANTFRONT
#define PLAYER_BACK_CASES PLAYER_ACTIVE_BACK_CASES: PLAYER_DASH_BACK_CASES: case PFIREARROWBACK: case PPLANTBACK
#define PLAYER_LEFT_CASES PLAYER_ACTIVE_LEFT_CASES: PLAYER_DASH_LEFT_CASES: case PFIREARROWLEFT: case PPLANTLEFT
#define PLAYER_RIGHT_CASES PLAYER_ACTIVE_RIGHT_CASES: PLAYER_DASH_RIGHT_CASES: case PFIREARROWRIGHT: case PPLANTRIGHT

#define PLAYER_ALL_ACTIVE_CASES PLAYER_ACTIVE_FRONT_CASES: PLAYER_ACTIVE_BACK_CASES: PLAYER_ACTIVE_LEFT_CASES: PLAYER_ACTIVE_RIGHT_CASES


LINKED_LIST_TEMPLATE_DEF(ItemNode,Item);

/* BOUNDARY VALUE(MAX, MIN) SETTER START */
void incr_hp(Player* player, const float health) {
    player->hp += health;
    if(XF_GREATER_THAN_YF(player->hp, (float)PLAYER_MAX_HP))
        player->hp = PLAYER_MAX_HP; 
}
void incr_stamina(Player* player, const float stamina) {
    player->stamina += stamina;
    if(XF_GREATER_THAN_YF(player->stamina, (float)PLAYER_MAX_STAMINA))
        player->stamina = PLAYER_MAX_STAMINA; 
}
void decr_min_0(float* lval, const float rval) {
    *lval -= rval;
    if(XF_LESS_THAN_YF(*lval, 0.0))
        *lval = 0; 
}
/* BOUNDARY VALUE(MAX, MIN) SETTER END */

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
        switch((instance->state/10)) {
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
                    instance->state += 39;
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

PlayerSpritesheetEnums get_player_dash_state(Player* instance) {
    decr_min_0(&(instance->stamina), 50.0);
    instance->is_update_status_gui = TRUE;
    const unsigned short new_state = (instance->move_controller[1]? 
        (40 + ((instance->move_controller[1] & 0b10)>>1)) : (50 + ((instance->move_controller[0]+2)>>1)));
    switch(instance->state) {
        PLAYER_ACTIVE_FRONT_CASES:
            return new_state;
        PLAYER_ACTIVE_BACK_CASES:
            return new_state + 2;
        break;
        PLAYER_ACTIVE_RIGHT_CASES:
            return new_state + 4;
        break;
        PLAYER_ACTIVE_LEFT_CASES:
            return new_state + 6;
        default:
    }
    return -1;
}
/* ANIMATION END */

/* TERMINAL GUI START */ 
void update_status_gui(Player* player) {
    if(!(player->is_update_status_gui))
        return;
    char bar[34];
    bar[33] = 0;
    
    ResetUI(TRUE);
    WriteOnUi(" HP ");
    const int h = EQUAL_TO_F(player->hp, 0.0)? 0 : (player->hp/HP_BLOCK + ((((int)player->hp)%HP_BLOCK)? 0: 1));
    int hidx = 0;
    for(; hidx < h; ++hidx)
        memcpy(bar+(hidx*3), "\u2588", sizeof("\u2588"));
    for(; hidx < ((PLAYER_MAX_HP/HP_BLOCK)+1); ++hidx)
        memcpy(bar+(hidx*3), "\u2591", sizeof("\u2591"));
    WriteOnUi(bar);

    WriteOnUi(" Stamina ");
    const int s = EQUAL_TO_F(player->stamina, 0.0)? 0 : (player->stamina/STAMINA_BLOCK + ((((int)player->stamina)%STAMINA_BLOCK)? 0: 1));
    int sidx = 0;
    for(; sidx < s; ++sidx)
        memcpy(bar+(sidx*3), "\u2588", sizeof("\u2588"));
    for(; sidx < ((PLAYER_MAX_STAMINA/STAMINA_BLOCK)+1); ++sidx)
        memcpy(bar+(sidx*3), "\u2591", sizeof("\u2591"));
    WriteOnUi(bar);

    
    if(!(player->is_menu_open))
        WriteOnUi("\n\n");
    else {
        WriteOnUi("\n");
        SetUiChOnCurr('\n');
    }
    player->is_update_status_gui = FALSE;
}

void write_horizontal_border_on_ui(const char* start, const char* middle, const char* end) {
    WriteOnUi(start);
    WriteOnUi("\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550");
    WriteOnUi(middle);
    WriteOnUi("\u2550\u2550\u2550\u2550\u2550\u2550");
    WriteOnUi(end);
    WriteOnUi("\n");
}

void update_menu_gui(Player* player) {
    if(!player->is_menu_open || !player->is_update_menu)
        return;
    SetUIPosPtr(81); // fixed position after status menu
    WriteOnUi("       ---  INVENTORY  ---      \n");
    write_horizontal_border_on_ui(" \u2554", "\u2566", "\u2557");
    FOR_LIST(Item,, player->inventory, {
        char item_name[23]; 
        TextPadRight(get_item_name(itr->val.type), 22, item_name); 
        WriteOnUi(" \u2551 "); 
        WriteOnUi(item_name);
        WriteOnUi("\u2551");
        if(itr->val.count != -1) { 
            char item_count[7]; 
            char count[6]; 
            sprintf(count, "%d", itr->val.count); 
            TextPadCenter(count, 6, item_count); 
            WriteOnUi(item_count);
        } else 
            WriteOnUi("KeyItm"); 
        WriteOnUi("\u2551\n");
    })
    write_horizontal_border_on_ui(" \u2560", "\u256C", "\u2563");
    write_horizontal_border_on_ui(" \u255A", "\u2569", "\u255D");
    WriteOnUi("\n");
    player->is_update_menu = FALSE;
}

void toggle_menu(Player* player) {
    player->is_menu_open = player->is_menu_open == 0;
    if(player->is_menu_open) {
        player->is_update_menu = TRUE;
    } else {
        SetUIPosPtr(81);
        SetUiChOnCurr('\0');
    }
}
/* TERMINAL GUI END */

/* MOVE AND COLLIDE START */
void move_and_collide(Entity* entity, const float delta) {
    Player* instance = entity->instance;
    remove_collision_on(entity->map, COLLISION_LAYER, entity->position);
    Point2f next_pos;
    Point2S8 vector = {0, 0};
    switch (instance->state) {
        PLAYER_FRONT_WALK_CASES: {
            next_pos[0] = entity->position[0];
            next_pos[1] = entity->position[1] + (WALKING_SPEED * delta);
            vector[0] = instance->move_controller[0];
            vector[1] = instance->move_controller[1];
        } break;
        PLAYER_BACK_WALK_CASES: {
            next_pos[0] = entity->position[0];
            next_pos[1] = entity->position[1] - (WALKING_SPEED * delta);
            vector[0] = instance->move_controller[0];
            vector[1] = instance->move_controller[1];
        } break;
        PLAYER_LEFT_WALK_CASES: {
            next_pos[0] = entity->position[0] - WALKING_SPEED * delta;
            next_pos[1] = entity->position[1];
            vector[0] = instance->move_controller[0];
            vector[1] = instance->move_controller[1];
        } break;
        PLAYER_RIGHT_WALK_CASES: {
            next_pos[0] = entity->position[0] + WALKING_SPEED * delta;
            next_pos[1] = entity->position[1];
            vector[0] = instance->move_controller[0];
            vector[1] = instance->move_controller[1];
        } break;
        PLAYER_FRONTDASH_CASES: {
            next_pos[0] = entity->position[0];
            next_pos[1] = entity->position[1] + (WALKING_SPEED * delta);
            vector[1] = 1;
        } break;
        PLAYER_BACKDASH_CASES: {
            next_pos[0] = entity->position[0];
            next_pos[1] = entity->position[1] - (WALKING_SPEED * delta);
            vector[1] = -1;
        } break;
        PLAYER_RIGHTDASH_CASES: {
            next_pos[0] = entity->position[0] + WALKING_SPEED * delta;
            next_pos[1] = entity->position[1];
            vector[0] = 1;
        } break;
        PLAYER_LEFTDASH_CASES: {
            next_pos[0] = entity->position[0] - WALKING_SPEED * delta;
            next_pos[1] = entity->position[1];
            vector[0] = -1;
        } break;
        default:
            add_collision_on(entity->map, COLLISION_LAYER, entity->position);
        return;
    }
    check_collision_with_vec(entity->map, COLLISION_LAYER, next_pos, vector);
    entity->position[0] = next_pos[0];
    entity->position[1] = next_pos[1];
    add_collision_on(entity->map, COLLISION_LAYER, entity->position);
    entity->redraw = TRUE;
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

void pick_up_item(Entity* entity) {
    Point2f position = {entity->position[0], entity->position[1]};
    switch(((Player*)entity->instance)->state) {
        PLAYER_ACTIVE_FRONT_CASES:
            position[1] += PLAYER_PARTITION_X>>1; break;
        PLAYER_ACTIVE_BACK_CASES:
            position[1] -= BIGGER_EPSILON+(PLAYER_PARTITION_X>>1); break;
        PLAYER_ACTIVE_LEFT_CASES:
            position[0] -= BIGGER_EPSILON+(PLAYER_PARTITION_Y>>1); break;
        PLAYER_ACTIVE_RIGHT_CASES:
            position[0] += PLAYER_PARTITION_Y>>1; break;
        default:
    }
    add_collision_on(entity->map, INTERACTION_LAYER, position);
    memcpy(((Player*)entity->instance)->last_interaction, position, sizeof(Point2f));
}

void InputPlayer(Entity* entity) {
    Player* instance = (Player*)entity->instance;
    
    if(GetStateOfKey(SDL_SCANCODE_SEMICOLON) == PRESSED) // Open/Close Menu
        toggle_menu(instance);

    switch(instance->state) {
        PLAYER_ALL_ACTIVE_CASES:
            break;
        default:
            instance->move_controller[0] = 0;
            instance->move_controller[1] = 0;
            return;
    }

    // check if a key item has a binded input key
    FOR_LIST(Item,, instance->inventory, {
        if(itr->val.count != -1) // if item is no longer key item, then end loop
            break;
        on_input(&(itr->val), entity);
    });

    // if Picking up item
    if(instance->last_interaction[0] == -1 && GetStateOfKey(SDL_SCANCODE_F) == PRESSED)
        pick_up_item(entity);

    const BOOL isDash = isHeld(GetStateOfKey(SDL_SCANCODE_LSHIFT));
    if(!(move_input(instance) || isDash)) // return when move_input is not changed
        return;
    
    if(instance->move_controller[0] == 0 && instance->move_controller[1] == 0) {
        // Reset to idle
        if(!isDash)
            set_animation_p(entity, instance->state%PLAYER_MAX_COLUMN, player_anim_idle);
        return;
    }

    if(isDash) {
        // Dash animation
        if(XF_GREATER_THAN_OR_EQUAL_YF(instance->stamina, 50.0))
            set_animation_p(entity, get_player_dash_state(instance), player_dash);
        return;
    }

    // Walking animation
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
    entity->prev_map = VOID_MAP;
    instance->hp = PLAYER_MAX_HP;
    instance->stamina = PLAYER_MAX_STAMINA;
    instance->last_interaction[0] = -1;
    init_linked_listItem(&(instance->inventory));
    set_animation_p(entity, PFRONTIDLE0, player_anim_idle);
    instance->is_update_status_gui = TRUE;

    return entity;
}

void ExitPlayer(Entity* entity) {
    FreeTextureResource(PLAYER_SPRITESHEET);
    remove_collision_on(entity->map, COLLISION_LAYER, entity->position);
    free_linked_listItem(&((Player*)entity->instance)->inventory);
}

void ProcessPlayer(Entity* entity, const float delta) {
    Player* instance = ((Player*)entity->instance);
    
    if(instance->state == PDEATH)
        return;

    // Check for Warp
    int warp = -1+get_collision_on(entity->map, WARP_EVENT_LAYER, entity->position);
    if(warp > VOID_MAP) {
        entity->map->switch_map = (MapInstance)(warp);
        entity->prev_map = entity->map->type;
        return;
    }
    
    // Remove Interaction after a frame if interaction exist
    if(instance->last_interaction[0] != -1) {
        remove_collision_on(entity->map, INTERACTION_LAYER, instance->last_interaction);
        instance->last_interaction[0] = -1;
    }
    
    // Check if taking damage
    unsigned short damage = get_collision_on(entity->map, HURTBOX_LAYER, entity->position);
    if(damage) {
        null_collision_on(entity->map, HURTBOX_LAYER, entity->position[0], entity->position[1]);
        instance->anim_proc(entity, PLAYER_ATTACK_ANIM1_DELAY + PLAYER_ATTACK_ANIM2_DELAY); // removes hitbox on collision layer if attacking
        decr_min_0(&(instance->hp), damage);
        instance->is_update_status_gui = TRUE;
        
        if(instance->hp <= damage) {
            instance->state = PDEATH;
            entity->redraw = TRUE;
            printf("You have Died...\n");
            update_status_gui(instance);
            return;
        }
        
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
        update_status_gui(instance);
        return;
    }

    // Recover Stamina Bar
    switch(instance->state) {
        PLAYER_ALL_ACTIVE_CASES:
        if(instance->stamina != PLAYER_MAX_STAMINA) {
            incr_stamina(instance, delta * STAMINA_RECOVERY_RATE);
            instance->is_update_status_gui = TRUE;
        } break;
        default:
    }
    
    update_status_gui(instance);
    update_menu_gui(instance);

    instance->accum_anim += delta;
    instance->anim_proc(entity, delta);
    move_and_collide(entity, delta);
}

void GetPlayerSrcRectPos(Entity* entity, SDL_Rect* rect) {
    rect->x = (((Player*)entity->instance)->state%PLAYER_MAX_COLUMN)*PLAYER_PARTITION_X;
    rect->y = (((Player*)entity->instance)->state/PLAYER_MAX_COLUMN)*PLAYER_PARTITION_Y;
}

/* EVENT SIGNALS START */
void SignalSwordAttack(Entity* entity) {
    Player* instance = entity->instance;
    memset(instance->move_controller, 0, sizeof(instance->move_controller));
    decr_min_0(&(instance->stamina), 15);
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
    instance->is_update_status_gui = TRUE;
    return;
}

void SignalArrowAttack(Entity* entity) {

}

void SignalPlantBomb(Entity* entity) {

}
/* EVENT SIGNALS END */