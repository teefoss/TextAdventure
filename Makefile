CC		= clang
CFLAGS	= -Wall -Wextra -Wshadow -Werror -g
LINK	= -lSDL2 -ltextmode
DIAGOPT = -fno-show-column -fno-caret-diagnostics

# objects needed by both editor and game
OBJ = \
	generic.o \
	map.o \
	player.o \
	utility.o \
	glyph.o

CFLAGS += $(DIAGOPT)

all: game editor

game: main.o screen.o $(OBJ)
	$(CC) -o $@ $^ $(LINK)

editor: editor.o $(OBJ)
	$(CC) -o qed $^ $(LINK)
	
$%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	-@rm -rf *.o qed game
