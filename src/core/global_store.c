#include "global_store.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>

static GlobalStore global_store;

void InitStore() {
    EntityContainer* end = global_store.shared_entity + SHARED_ENTITY_SIZE;
    for(EntityContainer* itr = global_store.shared_entity; itr != end; ++itr)
        memset(itr, 0, sizeof(EntityContainer));
    global_store.tail = 0;
    memset(global_store.event_flag, 0, sizeof(global_store.event_flag));
}

void TerminateStore() {
    EntityContainer* end = global_store.shared_entity + SHARED_ENTITY_SIZE;
    for(EntityContainer* itr = global_store.shared_entity; itr != end; ++itr)
        if(itr->tag != NULL || itr->entity != NULL)
            RemoveEntity(itr);
}

void AddEntityTag(struct Entity* entity, char* tag) {
    if(global_store.tail == SHARED_ENTITY_SIZE) {
        EntityContainer* end = global_store.shared_entity + SHARED_ENTITY_SIZE;
        for(EntityContainer* itr = global_store.shared_entity; itr != end; ++itr){
            if(itr->tag != NULL || itr->entity != NULL)
                continue;
            itr->entity = entity;
            itr->tag = tag;
            return;
        }
        fprintf(stderr, "Global Store shared entity is fully allocated\n");
        return;
    }
    global_store.shared_entity[global_store.tail].entity = entity;
    global_store.shared_entity[global_store.tail].tag = malloc(strlen(tag));
    strcpy(global_store.shared_entity[global_store.tail].tag, tag);
    global_store.tail++;
}

void AddEntity(struct Entity* entity) {
    AddEntityTag(entity, NULL);
}

void RemoveEntity(EntityContainer* entity_container) {
    terminate(entity_container->entity);
    if(entity_container->tag != NULL)
        free(entity_container->tag);
    memset(entity_container, 0, sizeof(EntityContainer));
}

void RemoveEntityTagged(char* tag) {
    EntityContainer* entity_container = FindEntityTag(tag);
    if(entity_container) {
        RemoveEntity(entity_container);
        return;
    }
    fprintf(stderr, "Removing shared entity of tag %s in Global Store doesn't exist\n", tag);
}

EntityContainer* FindEntityTag(char* tag) {
    EntityContainer* end = global_store.shared_entity + SHARED_ENTITY_SIZE;
    for(EntityContainer* itr = global_store.shared_entity; itr != end; ++itr) {
        if(itr->tag == NULL || strcmp(itr->tag, tag))
            continue;
        return itr;
    }
    return NULL;
}

BOOL HasEntityTag(char* tag) {
    return FindEntityTag(tag)? TRUE : FALSE;
}

BOOL IsEventTriggered(EventReference event) {
    return global_store.event_flag[event];
}

void ToggleEventTriggered(EventReference event) {
    global_store.event_flag[event] = !(global_store.event_flag[event]);
}