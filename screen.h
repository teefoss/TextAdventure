#ifndef screen_h
#define screen_h

#include <textmode.h>

#define WINDOW_SCALE 2
#define SCREEN_MARGIN (DOS_CHAR_WIDTH * WINDOW_SCALE)
#define TQ_MODE DOS_MODE40 // all screen area console in mode 40

#define TEXT_AREA_W 32
#define TEXT_AREA_H MAP_SIZE

// represents an area of the game screen with its own console
typedef struct
{
    DOS_Console * console;
    SDL_Point window_location; // pixel location within window
    int w; // size in console cells
    int h;
} Area;

extern SDL_Renderer * renderer;
extern Area text_area;
extern Area map_area;
extern Area message_area;

void InitGameScreen(void);
void RenderArea(Area * area);
void ToggleFullscreen(void);

#endif /* screen_h */
