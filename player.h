#ifndef player_h
#define player_h

#define MAX_INVENTORY_ITEMS 16

typedef struct
{
    const char * location;
    int x, y;
    int num_items; // number of inventory items
    const char * inventory[MAX_INVENTORY_ITEMS]; // tags of items possessed
} Player;

extern Player player;

#endif /* player_h */
