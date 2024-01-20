#ifndef PLAYER_H
#define PLAYER_H

#include "../../globals.h"
#include "../../components/entity.h"
#include "../../components/map.h"
#include "item_node.h"
#include "../maps/map_table.h"
#include "../textures/spritesheet/player.h"
#include "../../types/linkedlist.h"

LINKED_LIST_TEMPLATE(ItemNode,Item);

typedef struct {
    PlayerSpritesheetEnums state;
    float hp;
    float stamina;
    BOOL is_update_status_gui;
    BOOL is_menu_open;
    BOOL is_update_menu;
    LinkedListItem inventory;
    Point2f last_interaction;
    Point2S8 move_controller;
    float accum_anim;
    Byte anim_metadata[2];
    void (*anim_proc)(Entity*, const float);
} Player;

Entity* ReadyPlayer(const float position_x, const float position_y, Map* created_by);
void ExitPlayer(Entity* entity);
void ProcessPlayer(Entity* player, const float delta);
void InputPlayer(Entity* entity);

void GetPlayerSrcRectPos(Entity* entity, SDL_Rect* rect);

void SignalSwordAttack(Entity* entity);
void SignalArrowAttack(Entity* entity);
void SignalPlantBomb(Entity* entity);

#endif