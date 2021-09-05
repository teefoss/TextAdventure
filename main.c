#include "generic.h"
#include "map.h"

#include <stdio.h>
#include <textmode.h>

#define SCREEN_MARGIN 8

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    const char * location;
    Point position;
} Player;

typedef struct
{
    DOS_Console * console;
    SDL_Point window_location;
    int mode;
    int w;
    int h;
    int scale;
} Console;


Console text_con = {
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

Console map_con = {
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


Player player = {
    .location = "basement",
    .position = { 0, 0 } // inited on map load
};


void RenderConsole(Console * con)
{
    DOS_RenderConsole
    (   con->console,
        con->window_location.x,
        con->window_location.y );
}


SDL_Rect ConsoleSizePixels(Console * con)
{
    SDL_Rect r;
    r.w = con->w * DOS_CHAR_WIDTH * con->scale;
    r.h = con->h * con->mode * con->scale;
    
    return r;
}


void PrintCurrentLocation()
{
    Generic * gen = FindGenericWithTag(player.location);
    
    if ( gen == NULL ) {
        fprintf(stderr, "Player location not found! (%s)\n", player.location);
        exit(EXIT_FAILURE);
    }
    
    DOS_ClearConsole(text_con.console);
    DOS_CPrintString(text_con.console, "You are in %s\n\n", gen->name);
    DOS_CPrintString(text_con.console, "%s", gen->description);

    PrintMap(gen->map, map_con.console);
}


void InitConsole(Console * con, SDL_Renderer * renderer)
{
    con->console = DOS_CreateConsole(renderer, con->w, con->h, con->mode);
    DOS_CSetCursorType(con->console, DOS_CURSOR_NONE);
    DOS_CSetScale(con->console, con->scale);
}


int main()
{
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    SDL_Rect text_con_rect = ConsoleSizePixels(&text_con);
    SDL_Rect map_con_rect = ConsoleSizePixels(&map_con);
    
    SDL_Window * window = SDL_CreateWindow
    (   "TQ",
        0,
        0,
        text_con_rect.w + map_con_rect.w + SCREEN_MARGIN * 3,
        text_con_rect.h + SCREEN_MARGIN * 2,
        0 );
    
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    
    InitConsole(&text_con, renderer);
    InitConsole(&map_con, renderer);
    map_con.window_location.x = text_con_rect.w + SCREEN_MARGIN * 2;
        
    // load maps
    
    int num_generics = NumGenerics();
    for ( int i = 0; i < num_generics; i++ ) {
        char file_name[100] = { 0 };
        strcpy(file_name, generics[i].tag);
        strcat(file_name, ".map");
        
        FILE * map_file = fopen(file_name, "r");
        if ( map_file ) {
            fread(generics[i].map, sizeof(cell_t), MAP_SIZE*MAP_SIZE, map_file);
        }
    }
    
    // init player
    
    player.location = "basement";

    
    bool running = true;
    while ( running ) {
        SDL_Event event;
        while ( SDL_PollEvent(&event) ) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        PrintCurrentLocation();
        
        RenderConsole(&text_con);
        RenderConsole(&map_con);
        
        SDL_RenderPresent(renderer);
        
        DOS_LimitFrameRate(25);
    };
    
    DOS_FreeConsole(text_con.console);
    DOS_FreeConsole(map_con.console);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
	return 0;
}
