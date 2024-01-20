#include "filesystem.h"

#include "SDL2/SDL.h"

#include "../components/entity.h"
#include "../game_objects/maps/map_table.h"

static FILE* fvc;
static unsigned int fset_table[MAX_SCENE_COUNT];

static char streambuffer[128];
static char* stream = NULL;
static char delimeter[2] = " ";

BOOL InitFilesystemVirtualCollection() {
    char absolutePath[256];
    snprintf(absolutePath, sizeof(absolutePath), "%s%s", SDL_GetBasePath(), "./assets/FVC");
    fvc = fopen(absolutePath, "r");
    if(!fvc)
        return FALSE;
    HydrateStreamWithNextLine();
    for(int i = 0; i < MAX_SCENE_COUNT; ++i)
        fset_table[i] = ReadStreamInt();
    return TRUE;
}

void TerminateFilesystemVirtualCollection() {
    fclose(fvc);
}

void OffsetStreamWithMap(MapInstance map_type) {
    unsigned int fptr_pos = 0;
    for(int i = -1; i != map_type;)
        fptr_pos += fset_table[++i];
    fseek(fvc, fptr_pos, SEEK_SET);
}

void HydrateStreamWithNextLine() {
    fscanf(fvc,"%[^\n]", streambuffer);
    stream = streambuffer;
    fgetc(fvc);
}

unsigned short GetFileLineCount() {
    stream = strstr(strstr(strstr(stream, " ")+1, " ")+1, " ")+1;
    return atoi(stream);
}

int ReadStreamInt() {
    int res = atoi(stream);
    stream = strstr(stream, delimeter) + 1;
    return res;
}

int ReadStreamFloat() {
    float res;
    sscanf(stream, "%f", &res);
    stream = strstr(stream, delimeter) + 1;
    return res;
}

char ReadStreamChar() {
    char res = stream[0];
    stream = strstr(stream, delimeter) + 1;
    return res;
}

BOOL HasNext() {
    return stream != (NULL+1);
}

BOOL HasNextInt() {
    return stream != (NULL+1) && '0' <= stream[0] && stream[0] <= '9';
}

void ReadStreamString(char* string) {
    char *temp = strstr(stream, delimeter);
    strncpy(string, stream, temp - stream);
    string[temp-stream] = 0;
    stream = temp+1;
}