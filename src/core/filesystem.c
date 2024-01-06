#include "filesystem.h"

#include "SDL2/SDL.h"

#include "../components/entity.h"
#include "../game_objects/maps/map_table.h"

#include <stdio.h>
#include <stdlib.h>

FILE* fvc;
static unsigned int fset_table[MAX_SCENE_COUNT];

BOOL InitFilesystemVirtualCollection() {
    char absolutePath[256];
    snprintf(absolutePath, sizeof(absolutePath), "%s%s", SDL_GetBasePath(), "./assets/FVC");
    fvc = fopen(absolutePath, "rb");
    if(!fvc)
        return FALSE;
    fread(fset_table, sizeof(unsigned int), MAX_SCENE_COUNT, fvc);
    fseek(fvc, 0, SEEK_SET);
    return TRUE;
}

void TerminateFilesystemVirtualCollection() {
    fclose(fvc);
}

void OffsetFVCwMap(MapInstance map_type) {
    unsigned int fptr_pos = 0;
    for(int i = 0; map_id <= i; ++i)
        fptr_pos += fset_table[i];
    fseek(fvc, fptr_pos, SEEK_SET);
}