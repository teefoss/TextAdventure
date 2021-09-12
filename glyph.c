#include "glyph.h"

void PrintGlyph(Glyph glyph, DOS_Console * con, int x, int y)
{
    DOS_CSetBackground(con, BG(glyph));
    DOS_CSetForeground(con, FG(glyph));
    DOS_CGotoXY(con, x, y);
    DOS_CPrintChar(con, CH(glyph));
}
