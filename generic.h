#ifndef location_h
#define location_h

#include "map.h"

#define MAX_DIRECTIONS  32

#define GLYPH_PLAYER 0x0902

typedef enum
{
    FLAG_SOLID          = 0x0001, // level objects
    FLAG_INVISIBLE      = 0x0002, // object is not drawn
    FLAG_COLLECTIBLE    = 0x0004, // items, player can pick it up
    FLAG_LINK           = 0x0008  // ?
} GenericFlags;

typedef enum
{
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    DIR_PORTAL,
    // add new direction just before DIR_COUNT
    DIR_COUNT
} Direction;

// locations, actors, items, links, and anything else
typedef struct generic
{
    int id; // all-purpose integer identifier
    
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
    
    // for multiple flags, OR them together, FLAG_SOLID|FLAG_COLLECTIBLE
    GenericFlags flags;
    
    // if a location, this is automatically loaded from file (tag.map)
    // if an object/actor, the first element is defined in the generics list
    // this is its appearance
    Map map;
    
    void (* action)(int input); // TODO: needed?
} Generic;

extern Generic generics[];
extern int num_generics;

void InitGenericCount(void);
void InitGenerics(void);

// returns NULL if tag is NULL
// aborts program if non-NULL tag not found
Generic * GetGenericWithTag(const char * tag);

// returns NULL if not found
Generic * GetGenericWithGlyph(Glyph glyph);

#endif /* location_h */
