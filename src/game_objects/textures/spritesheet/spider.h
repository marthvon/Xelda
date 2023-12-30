#ifndef SPIDER_SPRITESHEET_H
#define SPIDER_SPRITESHEET_H

#define SPIDER_MAX_COLUMN 5
#define SPIDER_MAX_ROW 6

#define SPIDER_PARTITION_X 16
#define SPIDER_PARTITION_Y 16

// X + Y*PLAYER_MAX_COLUMN
typedef enum {
    ESDEATH = 4 + 0*SPIDER_MAX_COLUMN,

    ESFRONTIDLE0 = 0 + 0*SPIDER_MAX_COLUMN,
    ESFRONTIDLE1 = 1 + 0*SPIDER_MAX_COLUMN,
    
    ESBACKIDLE0 = 2 + 0*SPIDER_MAX_COLUMN,
    ESBACKIDLE1 = 3 + 0*SPIDER_MAX_COLUMN,

    ESFRONTATTACKFRONT0 = 2 + 0*SPIDER_MAX_COLUMN,
    ESFRONTATTACKFRONT1 = 2 + 1*SPIDER_MAX_COLUMN,
    ESFRONTATTACKFRONT2 = 2 + 3*SPIDER_MAX_COLUMN,

    ESFRONTATTACKSIDE0 = 0 + 4*SPIDER_MAX_COLUMN,
    ESFRONTATTACKSIDE1 = 1 + 4*SPIDER_MAX_COLUMN,
    ESFRONTATTACKSIDE2 = 3 + 4*SPIDER_MAX_COLUMN,

    ESBACKATTACKFRONT0 = 3 + 0*SPIDER_MAX_COLUMN,
    ESBACKATTACKFRONT1 = 3 + 1*SPIDER_MAX_COLUMN,
    ESBACKATTACKFRONT2 = 3 + 3*SPIDER_MAX_COLUMN,

    ESBACKATTACKSIDE0 = 0 + 5*SPIDER_MAX_COLUMN,
    ESBACKATTACKSIDE1 = 1 + 5*SPIDER_MAX_COLUMN,
    ESBACKATTACKSIDE2 = 3 + 5*SPIDER_MAX_COLUMN,

    ESFRONTSTUN = 1 + 1*SPIDER_MAX_COLUMN,
    ESBACKSTUN = 1 + 3*SPIDER_MAX_COLUMN,

    ESFRONTWALK = 1 + 0*SPIDER_MAX_COLUMN,
    ESBACKWALK = 1 + 2*SPIDER_MAX_COLUMN,

} SpiderSpritesheetEnums;

#endif