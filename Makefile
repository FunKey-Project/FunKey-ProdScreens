# Files
S_FILES=funkey_prod_screens.c \
prodScreen_failScreen.c \
prodScreen_waitBattery.c \
prodScreen_displayTest.c \
prodScreen_buttonsTest.c \
prodScreen_speakerTest.c \
prodScreen_ledTest.c \
prodScreen_magnetTest.c \
prodScreen_validation.c

# Output
EXEC=funkey_prod_screens

# Build settings
CC=$(CROSS_COMPILE)gcc
# SDL options
CC_SDL=-lSDL -lSDL_image -lSDL_ttf `sdl-config --cflags --libs`
# Other options
CFLAGS=-O3 -std=c99 -Wall


all:Build

Build:
	$(CC) $(S_FILES) -o $(EXEC) $(CC_SDL) $(CFLAGS)

clean:
	rm $(EXEC)