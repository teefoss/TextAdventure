#include "generic.h"
#include "player.h"
#include "utility.h"

#include <string.h>
#include <stdlib.h>

int num_generics;
bool glyph_is_invisible[NUM_GLYPHS];

Generic generics[] =
{
    // LOCATIONS

    // name format: title capitalization
    
    {
        .tag = "basement",
        .name = "A Basement",
        .links = {
            [DIR_EAST] = "underground_river",
            [DIR_PORTAL] = "underground_river"
        }
    },
    
    {
        .tag = "underground_river",
        .name = "underground cavern",
        .links = {
            [DIR_WEST] = "basement"
        }
    },
    
    {   // in case we make a mistake
        .tag = "missing_link",
        .name = "Error!",
        .description = "You forgot to set a generic's link for this direction!"
    },

    // OBJECTS
    
    // item object name format: all caps
    // other objects: title capitalization
    
    {
        .tag = "empty",
        .name = "Empty",
        .description = "Just empty space",
        .flags = FLAG_INVISIBLE,
        .map = { 0x0000 }
    },
    
    {
        .tag = "player",
        .name = "Player Start",
        .description = "Where the player appears when entering this location",
        .flags = FLAG_INVISIBLE,
        .map = { 0x0902 },
    },
    
    {
        .tag = "rusty_key",
        .name = "RUSTY KEY",
        .description = "It's just a rusty old key",
        .flags = FLAG_COLLECTIBLE,
        .map = { 0x06E5 }
    },
    
    // LOCATION LINKS
    
    // - locations should be flagged with FLAG_LINK
    // - name and description only appear in map editor
    {
        .tag = "north",
        .name = "North Link",
        .description = "changes player location on contact",
        .flags = FLAG_LINK,
        .map = { 0x0F4E }
    },

    {
        .tag = "east",
        .name = "East Link",
        .description = "changes player location on contact",
        .flags = FLAG_LINK,
        .map = { 0x0F45 }
    },

    {
        .tag = "south",
        .name = "South Link",
        .description = "changes player location on contact",
        .flags = FLAG_LINK,
        .map = { 0x0F53 }
    },

    {
        .tag = "west",
        .name = "West Link",
        .description = "changes player location on contact",
        .flags = FLAG_LINK,
        .map = { 0x0F57 }
    },
    
    {
        .tag = "portal",
        .name = "Portal",
        .description = "changes player location on contact",
        .flags = FLAG_LINK,
        .map = { 0x0DEF }
    },
};


FILE * OpenFileWithTag(const char * tag, const char * file_ext)
{
    char file_name[100] = { 0 };
    
    if ( strcmp(file_ext, MAPS_EXT ) == 0 ) {
        strcpy(file_name, MAPS_DIR);
    } else if ( strcmp(file_ext, DESC_EXT) == 0 ) {
        strcpy(file_name, DESC_DIR);
    }
    
    strcat(file_name, tag);
    strcat(file_name, file_ext);
    
    return fopen(file_name, "r");
}


void LoadMap(Generic * gen)
{
    FILE * map_file = OpenFileWithTag(gen->tag, MAPS_EXT);
    bool glyphs_used[0x10000] = { 0 };
    
    if ( map_file ) {
        
        fread(gen->map, sizeof(Glyph), MAP_SIZE * MAP_SIZE, map_file);
        
        // make sure the level has a player start
        MapPoint position = FindMapGlyph(gen->map, GetGlyph("player"));
        if ( position.x == -1 ) {
            Error("Error: %s.map has no player start!\n", gen->tag);
        }
        
    } else { // this generic has no map, it's a single object
        
        // check that its glyph (defined in generics list) isn't a duplicate.
        if ( glyphs_used[gen->map[0]] ) {
            Error("ERROR: Generic \"%s\" has a duplicate glyph!");
        } else {
            glyphs_used[gen->map[0]] = true;
        }
    }
}


// try to load description for Generic 'gen' from text file
void LoadDescription(Generic * gen)
{
    FILE * text_file = OpenFileWithTag(gen->tag, DESC_EXT);
    
    if ( text_file == NULL ) {
        return; // no description text file for this generic, probably
    }
    
    // get the file's size
    fseek(text_file, 0L, SEEK_END);
    long size = ftell(text_file);
    fseek(text_file, 0L, SEEK_SET);
    
    gen->description = calloc(size + 1, sizeof(char));
    fread(gen->description, sizeof(char), size, text_file);
    
    if ( ferror(text_file) != 0 ) {
        Error("Error reading %s.txt!\n", gen->tag);
    }
}


// find how many elements are in the generics list
void InitGenericCount()
{
    num_generics = (int)(sizeof(generics) / sizeof(generics[0]));
}


void InitGenerics()
{
    InitGenericCount();
    
    int num_directions = 0;
    Generic * gen = generics;
    
    for ( int i = 0; i < num_generics; i++, gen++ ) {
        
        LoadMap(gen);
        
        if ( gen->description == NULL ) {
            // no description entered in generic.c, try to load it from file
            LoadDescription(gen);
        }
        
        if ( gen->flags & FLAG_LINK ) {
            gen->id = num_directions++;
        }
        
        if ( gen->flags & FLAG_INVISIBLE ) {
            glyph_is_invisible[gen->map[0]] = true;
        }
    }
    
    // check that there are the same number of links in generics list as in enum
    if ( num_directions != DIR_COUNT ) {
        fprintf(stderr, "WARNING: Direction / Generic Link count mismatch!\n");
    }
}


Generic * GetGenericWithTag(const char * tag)
{
    if ( tag == NULL ) {
        return NULL;
    }
    
    Generic * gen = generics;
    for ( int i = 0; i < num_generics; i++, gen++ ) {
        if ( strcmp(tag, gen->tag) == 0 ) {
            return gen;
        }
    }
    
    Error("ERROR: generic with tag \"%s\" not found!\n", tag);
    return NULL;
}


Generic * GetGenericWithGlyph(Glyph glyph)
{
    Generic * gen = generics;
    for ( int i = 0; i < num_generics; i++, gen++ ) {
        if ( gen->map[0] == glyph ) {
            return gen;
        }
    }
    
    return NULL;
}


Generic * GetMapObject(Map map, int x, int y)
{
    return GetGenericWithGlyph( *GetMapGlyph(map, x, y) );
}


Glyph GetGlyph(const char * tag)
{
    Generic * gen = GetGenericWithTag(tag);
    return gen->map[0];
}
