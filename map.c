#include "map.h"
#include "generic.h"


void PrintGlyph(Glyph glyph, DOS_Console * con, int x, int y)
{
    DOS_CSetBackground(con, BG(glyph));
    DOS_CSetForeground(con, FG(glyph));
    DOS_CGotoXY(con, x, y);
    DOS_CPrintChar(con, CH(glyph));
}


Glyph * GetGlyph(map_t map, int x, int y)
{
    return &map[y * MAP_SIZE + x];
}


void PrintMap(map_t map, DOS_Console * con)
{
    for ( int y = 0; y < MAP_SIZE; y++ ) {
        for ( int x = 0; x < MAP_SIZE; x++ ) {
            Glyph * glyph = GetGlyph(map, x, y);
            
            if ( *glyph != CELL_PLAYER ) {
                PrintGlyph(*glyph, con, x, y);
            }
        }
    }
}


Point FindGlyph(map_t map, Glyph glyph)
{
    for ( int y = 0; y < MAP_SIZE; y++ ) {
        for ( int x = 0; x < MAP_SIZE; x++ ) {
            if ( *GetGlyph(map, x, y) == glyph ) {
                return (Point){ x, y };
            }
        }
    }
    
    return (Point){ -1, -1 };
}
