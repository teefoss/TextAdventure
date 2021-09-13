#include "screen.h"
#include "map.h"
#include "utility.h"

SDL_Renderer * renderer;

static SDL_Window * window;
static bool fullscreen = false;

// location title and description printed here
Area text_area = {
    .console = NULL,
    .window_location = {
        .x = SCREEN_MARGIN,
        .y = SCREEN_MARGIN,
    },
    .w = TEXT_AREA_W,
    .h = TEXT_AREA_H,
};

// location's map printed here
Area map_area = {
    .console = NULL,
    .window_location = {
        .x = 0, // initted in main
        .y = SCREEN_MARGIN
    },
    .w = MAP_SIZE,
    .h = MAP_SIZE,
};

// status messages printed herer
Area message_area = {
    .console = NULL,
    .window_location = {
        .x = SCREEN_MARGIN,
        .y = 0, // initted in main
    },
    .w = TEXT_AREA_W,
    .h = 1
};


void ToggleFullscreen()
{
    if ( fullscreen ) {
        SDL_SetWindowFullscreen(window, 0);
    } else {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    
    fullscreen = !fullscreen;
}


void RenderArea(Area * area)
{
    DOS_RenderConsole
    (   area->console,
        area->window_location.x,
        area->window_location.y );
}


static SDL_Rect AreaSizePixels(Area * area)
{
    SDL_Rect r;
    r.w = area->window_location.x;
    r.h = area->window_location.y;
    r.w = area->w * DOS_CHAR_WIDTH * WINDOW_SCALE;
    r.h = area->h * TQ_MODE * WINDOW_SCALE;
    
    return r;
}


// create consoles for each area, and set their options
static void InitAreaConsole(Area * area)
{
    area->console = DOS_CreateConsole(renderer, area->w, area->h, TQ_MODE);
    DOS_CSetCursorType(area->console, DOS_CURSOR_NONE);
    DOS_CSetScale(area->console, WINDOW_SCALE);
}


static void FreeGameScreen()
{
    DOS_FreeConsole(text_area.console);
    DOS_FreeConsole(map_area.console);
    DOS_FreeConsole(message_area.console);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
}


// init SDL_Window, SDL_Renderer, and screen areas
void InitGameScreen()
{
    // we need to know the size of each area in order to calculate
    // the window size.
    SDL_Rect text_area_rect = AreaSizePixels(&text_area);
    SDL_Rect map_area_rect = AreaSizePixels(&map_area);
    SDL_Rect msg_area_rect = AreaSizePixels(&message_area);
    
    // the calculated unscaled window size
    int w = text_area_rect.w + map_area_rect.w + SCREEN_MARGIN * 3;
    int h = text_area_rect.h + msg_area_rect.h + SCREEN_MARGIN * 3;
    
    // init SDL stuff
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        Error("ERROR: SDL_Init() failed: %s", SDL_GetError());
    }
    window = SDL_CreateWindow("TQ", 0, 0, w * WINDOW_SCALE, h * WINDOW_SCALE, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, w, h); // scale up renderer
    
    InitAreaConsole(&text_area);
    InitAreaConsole(&map_area);
    InitAreaConsole(&message_area);
    
    // for area positions that are relative to others, set position
    map_area.window_location.x = text_area_rect.w + SCREEN_MARGIN * 2;
    message_area.window_location.y = text_area_rect.h + SCREEN_MARGIN * 2;
    
    atexit(FreeGameScreen);
}
