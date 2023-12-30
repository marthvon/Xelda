#ifndef FILEPATH_TABLES_H
#define FILEPATH_TABLES_H

#include "texture_table.h"

char* GetAssetFilePath(TextureEnums texture) {
    switch(texture) {
        case PLAYER_SPRITESHEET:
            return "./assets/player-spritesheet.png";
        case GRASSnDIRT_SPRITESHEET:
            return "./assets/grass&dirt_biome.png";
        case TREEnROCK_SPRITESHEET:
            return "./assets/tree&rock.png";
        case STATUE_SPRITE:
            return "./assets/statue.png";
        case SPIDER_SPRITESHEET:
            return "./assets/spider-spritesheet.png";
        default:
            return NULL;
    }
}

#endif