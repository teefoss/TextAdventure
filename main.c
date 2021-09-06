#include "generic.h"
#include "map.h"
#include "player.h"

#include <stdio.h>
#include <textmode.h>

#define WINDOW_SCALE 2
#define SCREEN_MARGIN 8

// a rectangle area of the screen with its own console
typedef struct
{
    DOS_Console * console;
    SDL_Point window_location;
    
    int hud_color;
    char hud[100];
    
    // console info
    int mode;
    int w;
    int h;
    int scale;
} Area;


Area text_area = {
    .console = NULL,
    .window_location = {
        .x = SCREEN_MARGIN,
        .y = SCREEN_MARGIN,
    },
    .mode = DOS_MODE80,
    .w = 40,
    .h = 20,
    .scale = 1
};


Area map_area = {
    .console = NULL,
    .window_location = {
        .x = 0, // initted in main
        .y = SCREEN_MARGIN
    },
    .mode = DOS_MODE40,
    .w = MAP_SIZE,
    .h = MAP_SIZE,
    .scale = 2
};


void RenderArea(Area * area)
{
    DOS_RenderConsole
    (   area->console,
        area->window_location.x,
        area->window_location.y );
}


SDL_Rect AreaSizePixels(Area * area)
{
    SDL_Rect r;
    r.w = area->w * DOS_CHAR_WIDTH * area->scale;
    r.h = area->h * area->mode * area->scale;
    
    return r;
}


void PrintCurrentLocation()
{
    Generic * gen = FindGenericWithTag(player.location);
        
    DOS_ClearConsole(text_area.console);
    DOS_CPrintString(text_area.console, "You are in %s.\n\n", gen->name);
    DOS_CPrintString(text_area.console, "%s", gen->description);
    DOS_CSetForeground(text_area.console, text_area.hud_color);
    DOS_CPrintString(text_area.console, "\n\n%s", text_area.hud);

    DOS_ClearConsole(map_area.console);
    PrintMap(gen->map, map_area.console);
    PrintCell(CELL_PLAYER, map_area.console, player.x, player.y);
}


void InitArea(Area * area, SDL_Renderer * renderer)
{
    area->console = DOS_CreateConsole(renderer, area->w, area->h, area->mode);
    DOS_CSetCursorType(area->console, DOS_CURSOR_NONE);
    DOS_CSetScale(area->console, area->scale);
}


void TryMovePlayerTo(int x, int y)
{
    WRAP(player.x, 0, MAP_SIZE - 1); // correct weird values
    WRAP(player.y, 0, MAP_SIZE - 1);

    // get the cell we're stepping into
    Generic * location = FindGenericWithTag(player.location); // TODO: NULL?
    cell_t * cell = GetCell(location->map, x, y);
    
    // there's something there
    if ( *cell ) {
        // check what it is
        Generic * thing = FindGenericWithCell(*cell);
        
        if ( thing == NULL ) { // cell not in generics list, solid by default
            return;
        }
        
        if ( thing->flags & FLAG_SOLID ) {
            return;
        }
        
        if ( thing->flags & FLAG_COLLECTIBLE ) {
            if ( player.num_items == MAX_INVENTORY_ITEMS ) {
                strcpy(text_area.hud, "You can't carry any more!");
                text_area.hud_color = DOS_RED;
            } else {
                player.inventory[player.num_items++] = thing->tag;
                *cell = 0;
                sprintf(text_area.hud, "You got a %s!", thing->name);
                text_area.hud_color = DOS_BRIGHT_GREEN;
            }
        }
    }
    
    player.x = x;
    player.y = y;
}


void ProcessGameKeydown(SDL_Keycode key)
{
    switch ( key ) {
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
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    SDL_Rect text_area_rect = AreaSizePixels(&text_area);
    SDL_Rect map_area_rect = AreaSizePixels(&map_area);
    
    int w = text_area_rect.w + map_area_rect.w + SCREEN_MARGIN * 3;
    int h = text_area_rect.h + SCREEN_MARGIN * 2;
    SDL_Window * window = SDL_CreateWindow("TQ", 0, 0, w * WINDOW_SCALE, h * WINDOW_SCALE, 0);
    
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, w, h);
    
    InitArea(&text_area, renderer);
    InitArea(&map_area, renderer);
    map_area.window_location.x = text_area_rect.w + SCREEN_MARGIN * 2;
        
    LoadGenerics();
    
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
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        PrintCurrentLocation();
        
        RenderArea(&text_area);
        RenderArea(&map_area);
        
        SDL_RenderPresent(renderer);
        
        DOS_LimitFrameRate(25);
    };
    
    DOS_FreeConsole(text_area.console);
    DOS_FreeConsole(map_area.console);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
	return 0;
}
