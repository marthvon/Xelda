#ifndef TREEnROCK_SPRITESHEET_H
#define TREEnROCK_SPRITESHEET_H

#define TnR_MAX_COLUMN 2
#define TnR_MAX_ROW 2

#define TnR_PARTITION_X 16
#define TnR_PARTITION_Y 16
// X + Y*TnR_MAX_COLUMN
typedef enum {
    TnR_TREE = 0 + 0*TnR_MAX_COLUMN,
    TnR_ROCK = 1 + 0*TnR_MAX_COLUMN,
    TnR_TALL_GRASS = 1 + 1*TnR_MAX_COLUMN
} TreenRockSpritesheetEnums;

#endif