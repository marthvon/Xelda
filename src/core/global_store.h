#ifndef GLOBAL_STORE_H
#define GLOBAL_STORE_H

#include "../globals.h"
#include "../components/entity.h"
#include "../game_objects/maps/events_table.h"

#define SHARED_ENTITY_SIZE 5

struct Entity;

typedef struct EntityContainer {
    char* tag;
    struct Entity* entity;
} EntityContainer;

typedef struct GlobalStore {
    EntityContainer shared_entity[SHARED_ENTITY_SIZE];
    Byte tail;
    EventReference event_flag[MAX_EVENTS_COUNT];
} GlobalStore;

void InitStore();
void TerminateStore();
void AddEntity(struct Entity* entity);
void AddEntityTag(struct Entity* entity, char* tag);
void RemoveEntity(EntityContainer* entity_container);
void RemoveEntityTagged(char* tag);
EntityContainer* FindEntityTag(char* tag);
BOOL HasEntityTag(char* tag);

BOOL IsEventTriggered(EventReference event);
void ToggleEventTriggered(EventReference event);

#endif