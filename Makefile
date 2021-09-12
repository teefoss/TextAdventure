CC = clang
CFLAGS = -Wall -Wextra -Wshadow -Werror -g
LINK = -lSDL2 -ltextmode
DIAGOPT = -fno-show-column -fno-caret-diagnostics
O = obj

# objects needed by both editor and game
OBJ = \
	$(O)/generic.o \
	$(O)/map.o \
	$(O)/player.o \
	$(O)/utility.o \
	$(O)/glyph.o

CFLAGS += $(DIAGOPT)

all: game editor

game: $(O)/main.o $(O)/screen.o $(OBJ)
	$(CC) -o $@ $^ $(LINK)

editor: $(O)/editor.o $(OBJ)
	$(CC) -o qed $^ $(LINK)
	
$(O)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	-@rm -rf $(O) qed game
