#include "map.h"
#include "generic.h"


void PrintCell(cell_t cell, DOS_Console * con, int x, int y)
{
    DOS_CSetBackground(con, BG(cell));
    DOS_CSetForeground(con, FG(cell));
    DOS_CGotoXY(con, x, y);
    DOS_CPrintChar(con, CH(cell));
}


cell_t * GetCell(map_t map, int x, int y)
{
    return &map[y * MAP_SIZE + x];
}


void PrintMap(map_t map, DOS_Console * con)
{
    for ( int y = 0; y < MAP_SIZE; y++ ) {
        for ( int x = 0; x < MAP_SIZE; x++ ) {
            cell_t * cell = GetCell(map, x, y);
            
            if ( *cell != CELL_PLAYER ) {
                PrintCell(*cell, con, x, y);
            }
        }
    }
}


Point FindCell(map_t map, cell_t cell)
{
    for ( int y = 0; y < MAP_SIZE; y++ ) {
        for ( int x = 0; x < MAP_SIZE; x++ ) {
            if ( *GetCell(map, x, y) == cell ) {
                return (Point){ x, y };
            }
        }
    }
    
    return (Point){ -1, -1 };
}
