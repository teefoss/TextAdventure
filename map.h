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

#define BG(a) ((a & BG_MASK) >> 12)
#define FG(a) ((a & FG_MASK) >> 8)
#define CY(a) ((a & CY_MASK) >> 4)
#define CX(a) ((a & CX_MASK) >> 0)
#define CH(a) ((a & CH_MASK))

typedef cell_t map_t[MAP_SIZE * MAP_SIZE];

cell_t * GetCell(map_t map, int x, int y);
void PrintMap(map_t map, DOS_Console * con);

#endif /* map_h */
