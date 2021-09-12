#ifndef screen_h
#define screen_h

#include <textmode.h>

#define WINDOW_SCALE 2
#define SCREEN_MARGIN (DOS_CHAR_WIDTH * WINDOW_SCALE)
#define TQ_MODE DOS_MODE40
#define TEXT_AREA_W 32

// represents an area of the game screen with its own console
typedef struct
{
    DOS_Console * console;
    SDL_Point window_location;
    int w;
    int h;
} Area;

extern SDL_Window * window;
extern SDL_Renderer * renderer;
extern bool fullscreen;
extern Area text_area;
extern Area map_area;
extern Area message_area;

void InitArea(Area * area);
SDL_Rect AreaSizePixels(Area * area);
void RenderArea(Area * area);

#endif /* screen_h */
