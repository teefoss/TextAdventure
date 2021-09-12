#ifndef map_h
#define map_h

#include "types.h"
#include <textmode.h>

#define MAP_SIZE 16

typedef u16 Glyph; // one cell in a map

#define BG_MASK 0xF000 // background color, 0-15
#define FG_MASK 0x0F00 // foreground color, 0-15
#define CY_MASK 0x00F0 // character row
#define CX_MASK 0x000F // character col
#define CH_MASK (CY_MASK | CX_MASK) // ascii character

#define BG(glyph) ((glyph & BG_MASK) >> 12)
#define FG(glyph) ((glyph & FG_MASK) >> 8)
#define CY(glyph) ((glyph & CY_MASK) >> 4)
#define CX(glyph) ((glyph & CX_MASK) >> 0)
#define CH(glyph) ((glyph & CH_MASK))

typedef Glyph Map[MAP_SIZE * MAP_SIZE];

#define WRAP(n, min, max)   \
     if ( n < min) {        \
          n = max;          \
     } else if ( n > max) { \
          n = min;          \
     }

typedef struct
{
    int x;
    int y;
} Point;

typedef struct generic Generic;

void PrintGlyph(Glyph glyph, DOS_Console * con, int x, int y);
Glyph * GetMapGlyph(Map map, int x, int y);
Generic * GetMapObject(Map map, int x, int y);
void PrintMap(Map map, DOS_Console * con);
Point FindMapGlyph(Map map, Glyph glyph);

#endif /* map_h */
