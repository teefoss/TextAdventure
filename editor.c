#include "generic.h"
#include "map.h"
#include "utility.h"

#include <stdbool.h>
#include <textmode.h>

#define CONSOLE_W   53
#define CONSOLE_H   21

typedef enum
{
    PAGE_MAIN,
    PAGE_ITEM_LIST,
    PAGE_NUM_SCREENS
} EditorPage;

char map_path[0x80]; // path to currently open map file
Map map; // the map being currently edited

Glyph cursor; // character to insert
unsigned cx = MAP_SIZE / 2;
unsigned cy = MAP_SIZE / 2;

bool dirty = false;
EditorPage page = PAGE_MAIN;

Generic * objects[1000];
int num_objects = 0;
int selected_object = 0;


// set row and column in character selector
void SetCursorChar(int x, int y)
{
    WRAP(x, 0, 15);
    WRAP(y, 0, 15);
    
    cursor &= ~CY_MASK;
    cursor |= y << 4;
    
    cursor &= ~CX_MASK;
    cursor |= x;
}


void SetCursorGlyph(Glyph glyph)
{
    cursor = glyph;
    for ( int i = 0; i < num_objects; i++ ) {
        if ( objects[i]->map[0] == glyph ) {
            selected_object = i;
        }
    }
}


void OpenMap(char * location_tag)
{
    // make sure the format is 'MAPS_DIR/mapname.map'
    StripExtension(location_tag); // in case user added an extension
    strcpy(map_path, MAPS_DIR);
    strcat(map_path, location_tag);
    strcat(map_path, MAPS_EXT);
    
    FILE * map_file = fopen(map_path, "r+");
    
    if ( map_file ) {
        
        // read map contents are into 'map'
        fread(map, sizeof(u16), MAP_SIZE * MAP_SIZE, map_file);
        printf("Editing %s (%s)\n", location_tag, map_path);
        
    } else { // a map with this tag does not exist
        
        map_file = fopen(map_path, "w"); // try to create it
        
        if ( map_file == NULL ) {
            Error("ERROR: failed to create map file %s!\n", map_path);
        }
    }
    
    fclose(map_file);
}


void SaveMap()
{
    FILE * map_file = fopen(map_path, "w");
    
    if ( map_file == NULL ) {
        fprintf(stderr, "failed to save map!\n");
    }
    
    fwrite(map, sizeof(Glyph), MAP_SIZE * MAP_SIZE, map_file);
    dirty = false;
    fclose(map_file);
}


bool ProcessCtrlKey(SDL_Keycode key)
{
    switch ( key ) {
        case SDLK_s:
            SaveMap();
            break;
        case SDLK_q:
            return false;
        default:
            break;
    }
    
    return true;
}


bool ProcessShiftKey(SDL_Keycode key)
{
    int bg;
    int fg;
    
    switch ( key ) {
        case SDLK_i: // color up
            bg = BG(cursor) - 1;
            WRAP(bg, 0, 15);
            cursor &= ~BG_MASK;
            cursor |= bg << 12;
            break;
        case SDLK_k: // color down
            bg = BG(cursor) + 1;
            WRAP(bg, 0, 15);
            cursor &= ~BG_MASK;
            cursor |= bg << 12;
            break;
        case SDLK_j: // color left
            fg = FG(cursor) - 1;
            WRAP(fg, 0, 15);
            cursor &= ~FG_MASK;
            cursor |= fg << 8;
            break;
        case SDLK_l: // color right
            fg = FG(cursor) + 1;
            WRAP(fg, 0, 15);
            cursor &= ~FG_MASK;
            cursor |= fg << 8;
            break;
        default:
            break;
    }
    
    return true;
}


bool ProcessKey(SDL_Keycode key)
{
    SDL_Keymod mods = SDL_GetModState();
    
    if ( mods & KMOD_CTRL ) {
        return ProcessCtrlKey(key);
    } else if ( mods & KMOD_SHIFT ) {
        return ProcessShiftKey(key);
    }
        
    switch ( key ) {
            
        // move cursor
            
        case SDLK_w: cy--; break;
        case SDLK_a: cx--; break;
        case SDLK_s: cy++; break;
        case SDLK_d: cx++; break;
            
        // move character selector
            
        case SDLK_i: SetCursorChar(CX(cursor),     CY(cursor) - 1); break;
        case SDLK_k: SetCursorChar(CX(cursor),     CY(cursor) + 1); break;
        case SDLK_j: SetCursorChar(CX(cursor) - 1, CY(cursor));     break;
        case SDLK_l: SetCursorChar(CX(cursor) + 1, CY(cursor));     break;
            
        // pick up map glyph at cursor
            
        case SDLK_u:
            cursor = *GetMapGlyph(map, cx, cy);
            break;
            
        // place selected glyph on map
            
        case SDLK_SPACE:
            *GetMapGlyph(map, cx, cy) = cursor;
            dirty = true;
            break;
            
        // erase at cursor location
            
        case SDLK_e:
            *GetMapGlyph(map, cx, cy) = 0;
            dirty = true;
            break;
            
        default:
            break;
    }
        
    cx %= MAP_SIZE;
    cy %= MAP_SIZE;
    
    return true;
}


void ProcessItemListKey(SDL_Keycode key)
{
    switch ( key ) {
        case SDLK_DOWN:
        case SDLK_s:
            selected_object++;
            break;
        case SDLK_UP:
        case SDLK_w:
            selected_object--;
            break;
        case SDLK_RETURN:
            cursor = objects[selected_object]->map[0];
            page = PAGE_MAIN;
            DOS_SwitchPage(page);
            break;
        default:
            break;
    }
    
    WRAP(selected_object, 0, num_objects - 1);
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
        printf("usage: tqed [location tag]\n");
        printf("(the map extension ("MAPS_EXT") is automatically added)\n");
        return EXIT_FAILURE;
    }
    
    OpenMap(argv[1]);
    InitGenericCount(); // editor doesn't need to load generics, just get count
    cursor = GetGlyph("player");
    
    DOS_InitScreen(argv[1], CONSOLE_W, CONSOLE_H, DOS_MODE40, 4);
    DOS_SetScreenScale(3);
    DOS_SetBorderColor(DOS_GRAY);
    
    DOS_SwitchPage(PAGE_ITEM_LIST);
    DOS_SetCursorType(DOS_CURSOR_NONE);
    
    // make a list of all object generics (actors, items, things)
    
    Generic * g = generics;
    for ( int i = 0; i < num_generics && i < 1000; i++, g++ ) {
        
        // those that have a glyph defined in generics.c are objects
        if ( g->map[0]) {
            objects[num_objects++] = g;
        }
    }

    SetCursorGlyph(GetGlyph("player"));
    
    
    bool running = true;
    while ( running ) {
        
        SDL_Event event;
        while ( SDL_PollEvent(&event) ) {
            switch ( event.type ) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    switch ( event.key.keysym.sym ) {
                        case SDLK_1:
                            page = PAGE_MAIN;
                            DOS_SwitchPage(page);
                            break;
                        case SDLK_2:
                            page = PAGE_ITEM_LIST;
                            DOS_SwitchPage(page);
                            break;
                        default:
                            if ( page == PAGE_MAIN ) {
                                ProcessKey(event.key.keysym.sym); // TODO: rename
                            } else if ( page == PAGE_ITEM_LIST ) {
                                ProcessItemListKey(event.key.keysym.sym);
                            }
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
        
        // TODO: only print when necessary!
        
        DOS_SwitchPage(PAGE_MAIN);
        
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
                PrintChar(x == cx && y == cy ? cursor : *GetMapGlyph(map, x, y));
            }
        }

        DOS_PrintString("\n");
                
        // CURRENT CURSOR

        char buf[32] = { 0 };
        snprintf(buf, 32, "Map (%2d,%2d)", cx, cy);
        PrintCharDisplay(*GetMapGlyph(map, cx, cy), buf, map_x, DOS_GetY() + 1);

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

        // ITEMS LIST
        
        DOS_SwitchPage(PAGE_ITEM_LIST);
        
        DOS_ClearScreen();
        DOS_PrintString("OBJECTS (%d) (Press ENTER to select)\n\n", selected_object);

        // TODO: absolutely hacked it
        
        const int menu_list_h = 16;
        int menu_offset = selected_object - menu_list_h + 1;
        if ( menu_offset < 0 ) {
            menu_offset = 0;
        }
        
        Generic * obj = objects[menu_offset];
        for ( int i = 0; i < menu_list_h; i++, obj++ ) {
            if ( i - menu_offset > num_objects - 1 ) {
                break;
            }
            DOS_PrintString("  ");
            PrintChar(obj->map[0]);
            if ( i + menu_offset == selected_object ) {
                DOS_SetForeground(DOS_BRIGHT_WHITE);
            } else {
                DOS_SetForeground(DOS_WHITE);
            }
            DOS_PrintString(" %s\n", obj->name);
        }
        
        DOS_GotoXY(1, selected_object + 2 - menu_offset);
        DOS_SetForeground(DOS_YELLOW);
        DOS_PrintChar(DOS_RIGHTARROW);
        
        
        DOS_SwitchPage(page);
        
        DOS_DrawScreen();
        DOS_LimitFrameRate(25);
    }
        
    return 0;
}
