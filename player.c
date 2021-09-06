#include "player.h"
#include "generic.h"
#include "map.h"

Player player = {
    .location = "basement",
    .x = 0, // inited on map load
    .y = 0, // "
    .num_items = 0,
    .inventory = { 0 }
};
