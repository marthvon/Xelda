#ifndef PLAYER_H
#define PLAYER_H

#include "../../globals.h"
#include "../../components/entity.h"
#include "../../components/map.h"
#include "../maps/map_table.h"
#include "../textures/spritesheet/player.h"

typedef struct {
    PlayerSpritesheetEnums state;
    float hp;
    float stamina;
    BOOL hasSword;
    BOOL inventory;
    Point2f last_interaction;
    Point2S8 move_controller;
    float accum_anim;
    Byte anim_metadata[2];
    void (*anim_proc)(Entity*, const float);
    MapInstance prev_map;
} Player;

Entity* ReadyPlayer(const float position_x, const float position_y, Map* created_by);
void ExitPlayer(Entity* entity);
void ProcessPlayer(Entity* player, const float delta);
void InputPlayer(Entity* entity);

void GetPlayerSrcRectPos(Entity* entity, SDL_Rect* rect);

#endif