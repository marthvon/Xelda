#include "item_node.h"

#include "../../globals.h"
#include "../../core/input_handler.h"
#include "player.h"

char* get_item_name(ItemType type) {
    switch(type) {
        case SwordItem:
            return "Rusty Sword";
        case ArrowItem:
            return "Arrow";
        case BombItem:
            return "Bomb";
        case KeyItemA:
            return "Special Key";
        case StickItem:
            return "Stick";
        case StoneItem:
            return "Stone";
        case GunPowderItem:
            return "Gunpower";
    }
    return NULL;
}

void use_sword(ItemNode* item, Entity* entity) {
    if(GetStateOfKey(SDL_SCANCODE_SPACE) == PRESSED && 
        XF_GREATER_THAN_OR_EQUAL_YF(((Player*)entity->instance)->stamina, 15.0)
    )
        SignalSwordAttack(entity);
}

void use_arrow(ItemNode* item, Entity* entity) {
    if(GetStateOfKey(SDL_SCANCODE_SPACE) == PRESSED && 
        XF_GREATER_THAN_OR_EQUAL_YF(((Player*)entity->instance)->stamina, 6.0)
    )
        SignalArrowAttack(entity);
}

void use_bomb(ItemNode* item, Entity* entity) {
    if(GetStateOfKey(SDL_SCANCODE_SPACE) == PRESSED && 
        XF_GREATER_THAN_OR_EQUAL_YF(((Player*)entity->instance)->stamina, 25.0)
    )
        SignalPlantBomb(entity);
}

void on_input(ItemNode* item, Entity* entity) {
    switch(item->type) {
        case SwordItem:
            use_sword(item, entity);
            break;
        case ArrowItem:
            use_arrow(item, entity);
            break;
        case BombItem:
            use_bomb(item, entity);
        default:
    }
}