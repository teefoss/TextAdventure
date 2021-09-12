#ifndef player_h
#define player_h

#define MAX_INVENTORY_ITEMS 16

#include "generic.h"

typedef struct
{
    const char * location;
    int x, y;
    int num_items; // number of inventory items
    const char * inventory[MAX_INVENTORY_ITEMS]; // tags of items possessed
} Player;

extern Player player;

void EnterLocation(const char * tag);
Generic * GetPlayerLocation(void);

#endif /* player_h */
