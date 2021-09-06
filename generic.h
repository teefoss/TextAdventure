#ifndef location_h
#define location_h

#include "map.h"

#define CELL_PLAYER 0x0902

typedef enum
{
    FLAG_SOLID, // level objects
    FLAG_INVISIBLE,
    FLAG_COLLECTIBLE, // items
    FLAG_LINK,
} GenericFlags;

typedef enum
{
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    DIR_COUNT
} Direction;

// locations, actors, items, links, and anything else
typedef struct generic
{
    const char * tag;
    const char * name;
    char * description;
    const char * links[DIR_COUNT]; // tags that this object links to
    GenericFlags flags;
    map_t map;
    
    void (* action)(int input); // TODO: needed?
} Generic;

extern Generic generics[];
extern int num_generics;

void LoadGenerics(void);
Generic * FindGenericWithTag(const char * tag);
Generic * FindGenericWithCell(cell_t cell);

#endif /* location_h */
