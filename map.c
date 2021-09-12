#include "map.h"
#include "generic.h"

// get a pointer to glyph on map at x, y
Glyph * GetMapGlyph(Map map, int x, int y)
{
    return &map[y * MAP_SIZE + x];
}


void PrintMap(Map map, DOS_Console * con)
{
    for ( int y = 0; y < MAP_SIZE; y++ ) {
        for ( int x = 0; x < MAP_SIZE; x++ ) {
            Glyph * glyph = GetMapGlyph(map, x, y);
            
            if ( !glyph_is_invisible[*glyph] ) {
                PrintGlyph(*glyph, con, x, y);
            }
        }
    }
}


// Find the location of glyph on map
MapPoint FindMapGlyph(Map map, Glyph glyph)
{
    for ( int y = 0; y < MAP_SIZE; y++ ) {
        for ( int x = 0; x < MAP_SIZE; x++ ) {
            if ( *GetMapGlyph(map, x, y) == glyph ) {
                return (MapPoint){ x, y };
            }
        }
    }
    
    return (MapPoint){ -1, -1 };
}
