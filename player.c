#include "player.h"
#include "generic.h"
#include "map.h"

Player player = {
    .location = "underground_river",
    .x = 0, // set by calling EnterLocation()
    .y = 0, // "
    .num_items = 0,
    .inventory = { 0 }
};


void EnterLocation(const char * location_tag)
{
    Generic * gen;
    
    // update player's location
    if ( location_tag ) {
        player.location = location_tag;
        gen = GetGenericWithTag(location_tag);
    } else {
        player.location = "missing_link"; // in case we fucked it
        gen = GetGenericWithTag("missing_link");
    }

    // find the player start on this map and move there
    MapPoint position = FindMapGlyph(gen->map, GetGlyph("player"));
    player.x = position.x;
    player.y = position.y;
}


Generic * GetPlayerLocation()
{
    return GetGenericWithTag(player.location);
}
