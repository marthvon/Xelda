#ifndef ENEMY_WOLF_H
#define ENEMY_WOLF_H

#include "../../globals.h"
#include "../../components/entity.h"
#include "../../components/map.h"
#include "../textures/spritesheet/spider.h"

typedef enum {
    VOID_INTENT = -1,
    INTENT_MOVE_FORWARD,
    INTENT_MOVE_BACKWARD,
    INTENT_MOVE_LEFTSIDE,
    INTENT_MOVE_RIGHTSIDE,
} SpiderIntent;

typedef struct {
    SpiderSpritesheetEnums state;
    float hp;
    SpiderIntent intent;
    float accum_anim;
    void (*anim_proc)(Entity*, const float);
    float wait_before_attacking_again;
} Spider;

Entity* ReadySpider(const float position_x, const float position_y, Map* created_by);
void ExitSpider(Entity* entity);
void ProcessSpider(Entity* entity, const float delta);

void GetSpiderSrcRectPos(Entity* entity, SDL_Rect* rect);

#endif