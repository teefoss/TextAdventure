#ifndef map_h
#define map_h

#include "glyph.h"
#include <textmode.h>

#define MAP_SIZE 16

typedef Glyph Map[MAP_SIZE * MAP_SIZE];

typedef struct
{
    s8 x;
    s8 y;
} MapPoint;

Glyph * GetMapGlyph(Map map, int x, int y);
void PrintMap(Map map, DOS_Console * con);
MapPoint GetGlyphPosition(Map map, Glyph glyph);

#endif /* map_h */
