#include "screen.h"
#include "map.h"

SDL_Window * window;
SDL_Renderer * renderer;
bool fullscreen = false;

Area text_area = {
    .console = NULL,
    .window_location = {
        .x = SCREEN_MARGIN,
        .y = SCREEN_MARGIN,
    },
    .w = 32,
    .h = 16,
};


Area map_area = {
    .console = NULL,
    .window_location = {
        .x = 0, // initted in main
        .y = SCREEN_MARGIN
    },
    .w = MAP_SIZE,
    .h = MAP_SIZE,
};

Area message_area = {
    .console = NULL,
    .window_location = {
        .x = SCREEN_MARGIN,
        .y = 0, // initted in main
    },
    .w = TEXT_AREA_W,
    .h = 1
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
    r.w = area->w * DOS_CHAR_WIDTH * WINDOW_SCALE;
    r.h = area->h * TQ_MODE * WINDOW_SCALE;
    
    return r;
}


void InitArea(Area * area)
{
    area->console = DOS_CreateConsole(renderer, area->w, area->h, TQ_MODE);
    DOS_CSetCursorType(area->console, DOS_CURSOR_NONE);
    DOS_CSetScale(area->console, WINDOW_SCALE);
}
