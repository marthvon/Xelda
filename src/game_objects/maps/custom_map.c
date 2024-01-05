#include "../../components/map.h"
#include <stddef.h>

#include "start.h"
#include "south_borough.h"
#include "map_table.h"

Map* on_ready(MapInstance map_type) {
    switch(map_type) {
        case START_MAP:
            return map_defaults(OnReadyStartMap());
        case SOUTH_BOROUGH_MAP:
            return map_defaults(OnReadySBoroughMap());
        default:
    };
    return NULL;
}

Map* on_ready_start() {
    return on_ready(START_MAP);
}

void propagate_exit(Map* map) {
    switch(map->type) {
        case START_MAP:
            ExitStartMap(map);
            break;
        case SOUTH_BOROUGH_MAP:
            ExitSBoroughMap(map);
            break;
        default:
    }
    
    exit_map_def(map);
}