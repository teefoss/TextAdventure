#ifndef glyph_h
#define glyph_h

#include "types.h"
#include <textmode.h>

typedef u16 Glyph;

#define NUM_GLYPHS (U16_MAX + 1)

#define BG_MASK 0xF000 // background color, 0-15
#define FG_MASK 0x0F00 // foreground color, 0-15
#define CY_MASK 0x00F0 // ascii character row
#define CX_MASK 0x000F // ascii character col
#define CH_MASK (CY_MASK | CX_MASK) // ascii character

#define BG(glyph) ((glyph & BG_MASK) >> 12)
#define FG(glyph) ((glyph & FG_MASK) >> 8)
#define CY(glyph) ((glyph & CY_MASK) >> 4)
#define CX(glyph) ((glyph & CX_MASK) >> 0)
#define CH(glyph) ((glyph & CH_MASK))

void PrintGlyph(Glyph glyph, DOS_Console * con, int x, int y);

#endif /* glyph_h */
