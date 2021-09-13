#include "generic.h"
#include "map.h"
#include "player.h"
#include "utility.h"
#include "screen.h"

#include <stdio.h>
#include <textmode.h>

// print location name, description, and map
void PrintCurrentLocation()
{
    Generic * gen = GetPlayerLocation();
        
    DOS_ClearConsole(text_area.console);
    
    // location name
    
    DOS_CPrintString(text_area.console, "%s\n\n", gen->name);
    
    // description, with word wrapping!
    
    const size_t size = TEXT_AREA_W * MAP_SIZE;
    char buf[size] = { 0 };
    strncpy(buf, gen->description, size);
    
    char * sep = " \t";
    char * word = strtok(buf, sep);
        
    while ( word ) {
        
        if ( TEXT_AREA_W - DOS_CGetX(text_area.console) < (int)strlen(word) ) {
            DOS_CPrintString(text_area.console, "\n");
        }
        
        DOS_CPrintString(text_area.console, "%s ", word);
        word = strtok(NULL, sep);
    }
        
    DOS_ClearConsole(map_area.console);
    PrintMap(gen->map, map_area.console);
    PrintGlyph(GetGlyph("player"), map_area.console, player.x, player.y);
}


// print a message to message area
void Message(DOS_Color color, const char * format, ...)
{
    DOS_Console * con = message_area.console;
    
    va_list args[2]; // one for the string length, one for format
    va_start(args[0], format);
    va_copy(args[1], args[0]);
    
    // get the length of the format string
    int len = vsnprintf(NULL, 0, format, args[0]);
    char * buffer = calloc(len + 1, sizeof(*buffer));
    vsnprintf(buffer, len + 1, format, args[1]);
    
    va_end(args[0]);
    va_end(args[1]);
    
    DOS_ClearConsole(con);
    DOS_CSetForeground(con, color);
    DOS_CPrintString(con, buffer);
}


// map_glyph: a pointer to the map's glyph for this item
void CollectItem(Generic * item)
{
    if ( player.num_items == MAX_INVENTORY_ITEMS ) {
        Message(DOS_BRIGHT_RED, "You can't carry any more!");
    } else {
        Generic * location = GetPlayerLocation();
        Glyph * map_glyph = GetMapGlyph(location->map, player.x, player.y);

        // add to player inventory and remove from map
        player.inventory[player.num_items++] = item->tag;
        *map_glyph = 0;
        
        Message(FG(item->map[0]), "Picked up %s", item->name);
    }
}


// we have stepped onto a thing, examine its flags and do stuff accordingly
// map_glyph: a pointer to the map's glyph for this thing
void ContactThing(Generic * thing)
{
    if ( thing->flags & FLAG_COLLECTIBLE ) {
        CollectItem(thing);
    } else if ( thing->flags & FLAG_EXIT ) {
        Generic * current_location = GetPlayerLocation();
        EnterLocation(current_location->links[thing->id]);
    }
}


void TryMovePlayerTo(int try_x, int try_y)
{
    // correct weird values by wrapping the new location
    WRAP(try_x, 0, MAP_SIZE - 1);
    WRAP(try_y, 0, MAP_SIZE - 1);

    // get the thing we're stepping into
    Generic * location = GetPlayerLocation();
    Generic * thing = GetMapObject(location->map, try_x, try_y);
    
    // not in generics list, its solid by default
    if ( thing == NULL ) {
        return;
    }
    
    // we can step onto it, move and make contact
    if ( !(thing->flags & FLAG_SOLID) ) {
        player.x = try_x;
        player.y = try_y;
        ContactThing(thing);
    }
}


void ProcessGameKeydown(SDL_Keycode key)
{
    switch ( key ) {
        case SDLK_BACKSLASH:
            ToggleFullscreen();
            break;
        case SDLK_UP:
            TryMovePlayerTo(player.x, player.y - 1);
            break;
        case SDLK_DOWN:
            TryMovePlayerTo(player.x, player.y + 1);
            break;
        case SDLK_LEFT:
            TryMovePlayerTo(player.x - 1, player.y);
            break;
        case SDLK_RIGHT:
            TryMovePlayerTo(player.x + 1, player.y);
            break;
        default:
            break;
    }
}


int main()
{    
    InitGameScreen();
    InitGenerics();
    EnterLocation("basement");
    
    bool running = true;
    
    while ( running ) {
        
        SDL_Event event;
        while ( SDL_PollEvent(&event) ) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    ProcessGameKeydown(event.key.keysym.sym);
                    break;
                default:
                    break;
            }
        }
        
//#define SHOW_BOUNDS
#ifdef SHOW_BOUNDS
        SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
#else
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
#endif

        SDL_RenderClear(renderer);
        
        PrintCurrentLocation();
        
        RenderArea(&text_area);
        RenderArea(&map_area);
        RenderArea(&message_area);
        
        SDL_RenderPresent(renderer);
        
        DOS_LimitFrameRate(25);
    };
        
	return 0;
}
