#include "../../components/entity.h"
#include "../../components/map.h"
#include <stddef.h>

#include "background.h"
#include "player.h"
#include "statue.h"
#include "enemy_spider.h"
#include "treasure.h"
#include "arrow_entity.h"
#include "bomb_entity.h"
#include "door_entity.h"

Entity* ready_m(EntityInstance type, const float position_x, const float position_y, TextureEnums sprite, unsigned int spritesheet_ref, Map* created_by) {
    switch(type) {
        case BACKGROUND_ENTITY:
            return ReadyBackground(position_x, position_y, sprite, spritesheet_ref, created_by);
        case PLAYER_ENTITY:
            return ReadyPlayer(position_x, position_y, created_by);
        case STATUE_ENTITY:
            return ReadyStatue(position_x, position_y, created_by); 
        case ENEMY_SPIDER_ENTITY:
            return ReadySpider(position_x, position_y, created_by);
        case TREASURE_ENTITY:
            return ReadyTreasure(position_x, position_y, created_by);
        case ARROW_ENTITIY:
            return ReadyArrow(position_x, position_y, created_by);
        case BOMB_ENTITY:
            return ReadyBomb(position_x, position_y, created_by);
        case DOOR_ENTITY:
            return ReadyDoor(position_x, position_y, created_by);
    }
    return NULL;
}

Entity* ready(EntityInstance type, const float position_x, const float position_y, Map* created_by) {
    return ready_m(type, position_x, position_y, 0, 0, created_by);
}

void process(Entity* entity, const float delta) {
    switch (entity->type) {
        case BACKGROUND_ENTITY:
            return;
        case PLAYER_ENTITY:
            ProcessPlayer(entity, delta);
            break;
        case STATUE_ENTITY:
            ProcessStatue(entity, delta);
            break; 
        case ENEMY_SPIDER_ENTITY:
            ProcessSpider(entity, delta);
            break;
        case TREASURE_ENTITY:
            ProcessTreasure(entity);
            break;
        case ARROW_ENTITIY:
            ProcessArrow(entity, delta);
            break;
        case BOMB_ENTITY:
            ProcessBomb(entity, delta);
            break;
        case DOOR_ENTITY:
            ProcessDoor(entity);
            break;
    }
}

void input(Entity* entity) {
    switch(entity->type) {
        case PLAYER_ENTITY:
            InputPlayer(entity);
        default:
            return;
    }
}

void terminate(Entity* entity) {
    switch(entity->type) {
        case BACKGROUND_ENTITY:
            ExitBackground(entity);
            break;
        case PLAYER_ENTITY:
            ExitPlayer(entity);
            break;
        case STATUE_ENTITY:
            ExitStatue(entity);
            break;
        case ENEMY_SPIDER_ENTITY:
            ExitSpider(entity);
            break;
        case TREASURE_ENTITY:
            ExitTreasure(entity);
            break;
        case ARROW_ENTITIY:
            ExitArrow(entity);
            break;
        case BOMB_ENTITY:
            ExitBomb(entity);
            break;
        case DOOR_ENTITY:
            ExitDoor(entity);
            break;
    }
    if(entity->instance != NULL)
        free(entity->instance);
    free(entity);
}

void get_src_rect_pos(Entity* entity, SDL_Rect* rect) {
    switch(entity->type) {
        case BACKGROUND_ENTITY:
            GetBackgroundSrcRectPos(entity, rect);
            break;
        case PLAYER_ENTITY:
            GetPlayerSrcRectPos(entity, rect);
            break;
        case STATUE_ENTITY:
            GetStatueSrcRectPos(entity, rect);
            break; 
        case ENEMY_SPIDER_ENTITY:
            GetSpiderSrcRectPos(entity, rect);
            break;
        default:
    }
}