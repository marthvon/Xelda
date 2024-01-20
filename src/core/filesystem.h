#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include <stdio.h>
#include <stdlib.h>

#include "../globals.h"
#include "../game_objects/maps/map_table.h"

BOOL InitFilesystemVirtualCollection();
void TerminateFilesystemVirtualCollection();

/* May change current file offset before calling HydrateStreamWithNextLine() */
void OffsetStreamWithMap(MapInstance map_type);
/* May change current file offset by calling OffsetStreamWithMap() */
void HydrateStreamWithNextLine();

/* Call HydrateStreamWithNextLine() first */
unsigned short GetFileLineCount();
/* Call HydrateStreamWithNextLine() first */
int ReadStreamInt();
/* Call HydrateStreamWithNextLine() first */
int ReadStreamFloat();
/* Call HydrateStreamWithNextLine() first */
char ReadStreamChar();
/* Call HydrateStreamWithNextLine() first */
void ReadStreamString(char* string);

BOOL HasNext();
BOOL HasNextInt();

#endif