#ifndef location_h
#define location_h

#include "map.h"

#define CELL_PLAYER 0x0902

typedef enum
{
    FLAG_SOLID          = 0x0001, // level objects
    FLAG_INVISIBLE      = 0x0002, // object is not drawn
    FLAG_COLLECTIBLE    = 0x0004, // items, player can pick it up
    FLAG_LINK           = 0x0008 // ?
} GenericFlags;

typedef enum
{
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    // other directions? down/up, portal?
    DIR_COUNT
} Direction;

// locations, actors, items, links, and anything else
typedef struct generic
{
    // a unique identifier, used internally
    const char * tag;
    
    // for locations: format for sentence: "You are in [name]"
    // for items: format is all caps, e.g. "RUSTY KEY"
    // other things: format is ?
    const char * name;
    
    // for locations: the location's description, if longer than one line,
    // define in tag.txt
    // for items etc, a short description, define in generic list
    char * description;
    
    // tags of locations this generic links to
    const char * links[DIR_COUNT];
    
    // for multiple flags, OR them together, FLAG_SOLID|FLAG_COLLETIBLE
    GenericFlags flags;
    
    // if a location, this is automatically loaded from file (tag.map)
    // it an object/actor, the first element is defined in the generics list
    // this is its appearance
    map_t map;
    
    void (* action)(int input); // TODO: needed?
} Generic;

extern Generic generics[];
extern int num_generics;

void LoadGenerics(void);
Generic * FindGenericWithTag(const char * tag);
Generic * FindGenericWithCell(cell_t cell);

#endif /* location_h */
