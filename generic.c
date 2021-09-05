#include "generic.h"

#include <string.h>
#include <stdlib.h>

Generic generics[] = {
    {
        .tag = "player",
        .name = "Player",
        .description = "The spot the player appears at when entering this location",
        .flags = 0,
        .map = { 0x0902 },
    },
    {
        .tag = "basement",
        .name = "a basement",
        .description = "It's pretty dark.",
        .flags = 0,
    },
    {   // ID_RUSTY_KEY
        .tag = "rusty_key",
        .name = "RUSTY KEY",
        .description = "It's just a rusty old key",
        .flags = FLAG_COLLECTIBLE,
        .map = { 0x06E5 }
    }
};


int NumGenerics()
{
    return (int)(sizeof(generics) / sizeof(generics[0]));
}


Generic * FindGenericWithTag(const char * tag)
{
    Generic * gen = generics;
    int count = NumGenerics();
    
    for ( int i = 0; i < count; i++, gen++ ) {
        if ( strcmp(tag, gen->tag) == 0 ) {
            return gen;
        }
    }
    
    return NULL;
}
