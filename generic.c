#include "generic.h"
#include "player.h"

#include <string.h>
#include <stdlib.h>

int num_generics;

//
// GENERICS LIST
//

Generic generics[] =
{
    {
        .tag = "player",
        .name = "Player",
        .description = "The spot the player appears at when entering this location",
        .flags = FLAG_INVISIBLE,
        .map = { 0x0902 },
    },
    {
        .tag = "basement",
        .name = "a basement",
        .flags = 0,
    },
    {
        .tag = "rusty_key",
        .name = "RUSTY KEY",
        .description = "It's just a rusty old key",
        .flags = FLAG_COLLECTIBLE,
        .map = { 0x06E5 }
    }
};


FILE * OpenFileWithTag(const char * tag, const char * file_ext)
{
    char file_name[100] = { 0 };
    strcpy(file_name, tag);
    strcat(file_name, file_ext);
    
    return fopen(file_name, "r");
}


void LoadMap(Generic * gen)
{
    FILE * map_file = OpenFileWithTag(gen->tag, ".map");
    
    if ( map_file ) {
        fread(gen->map, sizeof(Glyph), MAP_SIZE * MAP_SIZE, map_file);
        
        Point position = FindGlyph(gen->map, CELL_PLAYER);
        if ( position.x == -1 ) {
            fprintf(stderr, "Error: %s.map has no player start!\n", gen->tag);
            exit(EXIT_FAILURE);
        }
        
        player.x = position.x;
        player.y = position.y;
    }
}


void LoadText(Generic * gen)
{
    FILE * text_file = OpenFileWithTag(gen->tag, ".txt");
    
    if ( text_file ) {
        if ( gen->description ) {
            fprintf(stderr,
                    "Error: tried to load description text for generic \"%s\" "
                    "which already has a description!\n", gen->tag);
            exit(EXIT_FAILURE);
        }
        
        fseek(text_file, 0L, SEEK_END);
        long size = ftell(text_file);
        fseek(text_file, 0L, SEEK_SET);
        
        gen->description = calloc(size + 1, sizeof(char));
        fread(gen->description, sizeof(char), size, text_file);

        if ( ferror(text_file) != 0 ) {
            fprintf(stderr, "Error reading %s.txt!\n", gen->tag);
            exit(EXIT_FAILURE);
        }
    }
}


void LoadGenerics()
{
    num_generics = (int)(sizeof(generics) / sizeof(generics[0]));
    printf("num_generics: %d\n", num_generics);
    
    Generic * gen = generics;
    for ( int i = 0; i < num_generics; i++, gen++ ) {
        LoadMap(gen);
        LoadText(gen);
    }
}


Generic * FindGenericWithTag(const char * tag)
{
    Generic * gen = generics;
    for ( int i = 0; i < num_generics; i++, gen++ ) {
        if ( strcmp(tag, gen->tag) == 0 ) {
            return gen;
        }
    }
    
    fprintf(stderr, "Error: generic with tag \"%s\" not found!\n", tag);
    exit(EXIT_FAILURE);
}


Generic * FindGenericWithGlyph(Glyph glyph)
{
    Generic * gen = generics;
    for ( int i = 0; i < num_generics; i++, gen++ ) {
        if ( gen->map[0] == glyph ) {
            return gen;
        }
    }
    
    return NULL;
}
