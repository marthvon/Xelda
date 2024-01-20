#ifndef ITEMNODE_H
#define ITEMNODE_H

#include "../../components/entity.h"

typedef enum {
    SwordItem,
    ArrowItem,
    BombItem,
    KeyItemA,
    StickItem,
    StoneItem,
    GunPowderItem,
} ItemType;

typedef struct ItemNode {
    ItemType type;
    short count;
} ItemNode;

char* get_item_name(ItemType type);
void on_input(ItemNode* item, Entity* entity);

#endif