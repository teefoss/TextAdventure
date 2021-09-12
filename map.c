#include "map.h"
#include "generic.h"


void PrintGlyph(Glyph glyph, DOS_Console * con, int x, int y)
{
    DOS_CSetBackground(con, BG(glyph));
    DOS_CSetForeground(con, FG(glyph));
    DOS_CGotoXY(con, x, y);
    DOS_CPrintChar(con, CH(glyph));
}


// get a pointer to glyph on map at x, y
Glyph * GetMapGlyph(Map map, int x, int y)
{
    return &map[y * MAP_SIZE + x];
}


Generic * GetMapObject(Map map, int x, int y)
{
    return GetGenericWithGlyph( *GetMapGlyph(map, x, y) );
}


void PrintMap(Map map, DOS_Console * con)
{
    for ( int y = 0; y < MAP_SIZE; y++ ) {
        for ( int x = 0; x < MAP_SIZE; x++ ) {
            Glyph * glyph = GetMapGlyph(map, x, y);
            
            if ( *glyph != GLYPH_PLAYER ) {
                PrintGlyph(*glyph, con, x, y);
            }
        }
    }
}


// Find the location of glyph on map
Point FindMapGlyph(Map map, Glyph glyph)
{
    for ( int y = 0; y < MAP_SIZE; y++ ) {
        for ( int x = 0; x < MAP_SIZE; x++ ) {
            if ( *GetMapGlyph(map, x, y) == glyph ) {
                return (Point){ x, y };
            }
        }
    }
    
    return (Point){ -1, -1 };
}
