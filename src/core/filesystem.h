#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include "../globals.h"
#include "../game_objects/maps/map_table.h"

FILE* fvc;

BOOL InitFilesystemVirtualCollection();
void TerminateFilesystemVirtualCollection();
void OffsetFVCwMap(MapInstance map_type);

#endif