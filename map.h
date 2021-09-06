#ifndef map_h
#define map_h

#include "types.h"
#include <textmode.h>

#define MAP_SIZE 16

typedef u16 cell_t; // one cell in a map

#define BG_MASK 0xF000 // background color, 0-15
#define FG_MASK 0x0F00 // foreground color, 0-15
#define CY_MASK 0x00F0 // character row
#define CX_MASK 0x000F // character col
#define CH_MASK (CY_MASK | CX_MASK) // ascii character

#define BG(cell) ((cell & BG_MASK) >> 12)
#define FG(cell) ((cell & FG_MASK) >> 8)
#define CY(cell) ((cell & CY_MASK) >> 4)
#define CX(cell) ((cell & CX_MASK) >> 0)
#define CH(cell) ((cell & CH_MASK))

typedef cell_t map_t[MAP_SIZE * MAP_SIZE];

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

void PrintCell(cell_t cell, DOS_Console * con, int x, int y);
cell_t * GetCell(map_t map, int x, int y);
void PrintMap(map_t map, DOS_Console * con);
Point FindCell(map_t map, cell_t cell);

#endif /* map_h */
