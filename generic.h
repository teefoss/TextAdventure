#ifndef location_h
#define location_h

#include "map.h"

#define MAX_DIRECTIONS  32

#define MAPS_EXT    ".map"
#define MAPS_DIR    "maps/"
#define DESC_EXT    ".txt"
#define DESC_DIR    "descriptions/"

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
    const char * tag; // a unique identifier, used internally
    const char * name;
    char * description;
    const char * links[DIR_COUNT]; // tags of locations this generic links to
    GenericFlags flags;
    
    // if a location, this is automatically loaded from file (tag.map)
    // if an object/actor, the first element is defined in the generics list
    // this is its appearance
    Map map;
    
    void (* action)(int input); // TODO: needed?
} Generic;

extern Generic generics[];
extern int num_generics;
extern bool glyph_is_invisible[NUM_GLYPHS];

void InitGenericCount(void);
void InitGenerics(void);

// returns NULL if tag is NULL
// aborts program if non-NULL tag not found
Generic * GetGenericWithTag(const char * tag);

// returns NULL if not found
Generic * GetGenericWithGlyph(Glyph glyph);

Generic * GetMapObject(Map map, int x, int y);
Glyph GetGlyph(const char * tag);

#endif /* location_h */
