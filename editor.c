#include "editor.h"
#include "generic.h"
#include "map.h"

#include <stdbool.h>
#include <textmode.h>

#define CONSOLE_W   53
#define CONSOLE_H   21

char file_name[80];
Map map;
Glyph cursor = 0x0902; // character to insert
unsigned cx, cy; // cursor location
bool dirty;


void SetCursorCharY(int value)
{
    WRAP(value, 0, 15);
    cursor &= ~CY_MASK;
    cursor |= value << 4;
}


void SetCursorCharX(int value)
{
    WRAP(value, 0, 15);
    cursor &= ~CX_MASK;
    cursor |= value;
}


bool ProcessKey(SDL_Keycode key)
{
    SDL_Keymod mods = SDL_GetModState();
        
    switch ( key ) {
        case SDLK_w:
            cy--;
            break;
        case SDLK_a:
            cx--;
            break;
        case SDLK_s:
            if ( mods & KMOD_CTRL ) {
                FILE * map_file = fopen(file_name, "w");
                if ( map_file == NULL ) {
                    fprintf(stderr, "failed to save map!\n");
                }
                
                fwrite(map, sizeof(Glyph), MAP_SIZE * MAP_SIZE, map_file);
                dirty = false;
            } else {
                cy++;
            }
            break;
        case SDLK_d:
            cx++;
            break;
        case SDLK_i:
            if ( mods & KMOD_SHIFT ) {
                int bg = BG(cursor) - 1;
                WRAP(bg, 0, 15);
                cursor &= ~BG_MASK;
                cursor |= bg << 12;
            } else {
                int ch_y = CY(cursor);
                SetCursorCharY(ch_y - 1);
            }
            break;
        case SDLK_k:
            if ( mods & KMOD_SHIFT ) {
                int bg = BG(cursor) + 1;
                WRAP(bg, 0, 15);
                cursor &= ~BG_MASK;
                cursor |= bg << 12;
            } else {
                int ch_y = CY(cursor);
                SetCursorCharY(ch_y + 1);
            }
            break;
        case SDLK_j:
            if ( mods & KMOD_SHIFT ) {
                int fg = FG(cursor) - 1;
                WRAP(fg, 0, 15);
                cursor &= ~FG_MASK;
                cursor |= fg << 8;
            } else {
                int ch_x = CX(cursor);
                SetCursorCharX(ch_x - 1);
            }
            break;
        case SDLK_l:
            if ( mods & KMOD_SHIFT ) {
                int fg = FG(cursor) + 1;
                WRAP(fg, 0, 15);
                cursor &= ~FG_MASK;
                cursor |= fg << 8;
            } else {
                int ch_x = CX(cursor);
                SetCursorCharX(ch_x + 1);
            }
            break;
        case SDLK_u:
            cursor = *GetGlyph(map, cx, cy);
            break;
        case SDLK_q:
            if ( mods & KMOD_CTRL ) {
                return false;
            }
        case SDLK_SPACE: {
            Glyph * glyph = GetGlyph(map, cx, cy);
            *glyph = cursor;
            dirty = true;
            break;
        }
        case SDLK_e:
            *GetGlyph(map, cx, cy) = 0;
            dirty = true;
            break;
        default:
            break;
    }
    
    cx %= MAP_SIZE;
    cy %= MAP_SIZE;
    return true;
}


void PrintChar(u16 value)
{
    DOS_SetBackground( (value & 0xF000) >> 12 );
    DOS_SetForeground( (value & 0x0F00) >> 8 );
    DOS_PrintChar( value & 0x00FF );
}


void PrintCharDisplay(u16 value, const char * title, int x, int y)
{
    int line = y;
    DOS_GotoXY(x, line);
    
    // left column : char display
    
    DOS_SetMargin(x);
    DOS_SetBackground(DOS_BLACK);
    DOS_SetForeground(DOS_WHITE);
    DOS_PrintString("   \n");
    DOS_PrintString("   \n");
    DOS_PrintString("   \n");
    DOS_GotoXY(x + 1, y + 1);
    PrintChar(value);
    
    // right column : char info
    
    DOS_SetMargin(x + 4);
    DOS_GotoXY(x + 4, y);
    DOS_SetBackground(DOS_GRAY);
    DOS_SetForeground(DOS_WHITE);
    DOS_PrintString("%s\n", title);
    DOS_PrintString("%04X\n", value);
    
    DOS_SetForeground(DOS_BRIGHT_GREEN);

    Generic * gen = GetGenericWithGlyph(value);
    if ( gen && gen->map[0] != 0 ) {
        DOS_PrintString("%s", gen->name);
    }
}


int main(int argc, char ** argv)
{
    if ( argc != 2 ) {
        printf("usage: tqed [map name]\n");
        return 1;
    }
    
    strcpy(file_name, argv[1]);
    FILE * map_file = fopen(file_name, "r+");
    
    if ( map_file ) {
        fread(map, sizeof(u16), MAP_SIZE * MAP_SIZE, map_file);
    } else {
        map_file = fopen(argv[1], "w");
        if ( map_file == NULL ) {
            fprintf(stderr, "failed to create map file %s!\n", argv[1]);
        }
    }
    
    InitGenericCount();
    
    fclose(map_file);
    
    DOS_InitScreen(argv[1], CONSOLE_W, CONSOLE_H, DOS_MODE40, 4);
    DOS_SetScreenScale(3);
    DOS_SetBorderColor(DOS_GRAY);
    
    bool running = true;
    while ( running ) {
        SDL_Event event;
        while ( SDL_PollEvent(&event) ) {
            switch ( event.type ) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    ProcessKey(event.key.keysym.sym);
                    break;
                case SDL_TEXTINPUT:
                    
                    break;
                default:
                    break;
            }
        }
        
        DOS_ClearScreen();
        DOS_SetBackground(DOS_GRAY);
        DOS_ClearBackground();
        
        // MAP
        
        if ( dirty ) {
            DOS_SetForeground(DOS_BRIGHT_RED);
            DOS_PrintString("M");
        }
        
        int map_x = 1;
        int map_y = 1;
        
        for ( unsigned y = 0; y < MAP_SIZE; y++ ) {
            DOS_GotoXY(map_x - 1, y + map_y);
            DOS_SetBackground(DOS_GRAY);
            DOS_SetForeground(cy == y ? DOS_YELLOW : DOS_WHITE);
            DOS_PrintString("%X", y);
            for ( unsigned x = 0; x < MAP_SIZE; x++ ) {
                if ( y == 0 ) {
                    DOS_GotoXY(x + map_x, map_y - 1);
                    DOS_SetBackground(DOS_GRAY);
                    DOS_SetForeground(cx == x ? DOS_YELLOW : DOS_WHITE);
                    DOS_PrintString("%X", x);
                }
                DOS_GotoXY(x + map_x, y + map_y);
                PrintChar(x == cx && y == cy ? cursor : *GetGlyph(map, x, y));
            }
        }

        DOS_PrintString("\n");
                
        // CURRENT CURSOR

        char buf[32] = { 0 };
        snprintf(buf, 32, "Map (%2d,%2d)", cx, cy);
        PrintCharDisplay(*GetGlyph(map, cx, cy), buf, map_x, DOS_GetY() + 1);

        DOS_SetBackground(DOS_GRAY);
                
        //  CHARACTER MAP
        
        int cmap_x = 19;
        int cmap_y = 1;
        int ch_y = (cursor & 0xF0) >> 4;
        int ch_x = cursor & 0xF;
        
        for ( int y = 0; y < 16; y++ ) {
            DOS_GotoXY(cmap_x - 1, cmap_y + y);
            DOS_SetBackground(DOS_GRAY);
            DOS_SetForeground(ch_y == y ? DOS_YELLOW : DOS_WHITE);
            DOS_PrintString("%X", y);
            for ( int x = 0; x < 16; x++ ) {
                if ( y == 0 ) {
                    DOS_GotoXY(cmap_x + x, cmap_y - 1);
                    DOS_SetBackground(DOS_GRAY);
                    DOS_SetForeground(ch_x == x ? DOS_YELLOW : DOS_WHITE);
                    DOS_PrintString("%X", x);
                }
                DOS_GotoXY(cmap_x + x, cmap_y + y);
                bool highlight = ch_x == x || ch_y == y;
                DOS_SetBackground(highlight ? DOS_GRAY : DOS_BLACK);
    
                int ch = y * 16 + x;
                if ( ch == CH(cursor) ) {
                    DOS_SetForeground(DOS_YELLOW);
                } else {
                    DOS_SetForeground(highlight ? DOS_BRIGHT_WHITE : DOS_WHITE);
                }
                DOS_PrintChar(ch);
            }
        }
        
        PrintCharDisplay(cursor, "Cursor", cmap_x, cmap_y + 16 + 1);
        
        // COLOR MAP

        int color_map_x = 37;
        int color_map_y = 1;
        int bg = BG(cursor);
        int fg = FG(cursor);

        for ( int y = 0; y < 16; y++ ) {
            DOS_GotoXY(color_map_x - 1, color_map_y + y);
            DOS_SetBackground(DOS_GRAY);
            DOS_SetForeground(bg == y ? DOS_YELLOW : DOS_WHITE);
            DOS_PrintString("%X", y);
            for ( int x = 0; x < 16; x++ ) {
                if ( y == 0 ) {
                    DOS_GotoXY(color_map_x + x, color_map_y - 1);
                    DOS_SetBackground(DOS_GRAY);
                    DOS_SetForeground(fg == x ? DOS_YELLOW : DOS_WHITE);
                    DOS_PrintString("%X", x);
                }
                DOS_GotoXY(color_map_x + x, color_map_y + y);
                DOS_SetBackground(y);
                DOS_SetForeground(x);
                DOS_PrintChar(CH(cursor));
            }
        }
                
        DOS_SetForeground(DOS_BRIGHT_WHITE);
        DOS_GotoXY(cx + map_x, cy + map_y);
        
        DOS_DrawScreen();
        DOS_LimitFrameRate(25);
    }
}
