# Files
S_FILES=funkey_prod_screens.c \
prodScreen_failScreen.c \
prodScreen_waitBattery.c \
prodScreen_displayTest.c \
prodScreen_brightnessTest.c \
prodScreen_buttonsTest.c \
prodScreen_speakerTest.c \
prodScreen_ledTest.c \
prodScreen_magnetTest.c \
prodScreen_validation.c \
prodScreen_showImage.c

# Output
EXEC=funkey_prod_screens

# Build settings
CC=$(CROSS_COMPILE)gcc

# Other options
CFLAGS += `sdl-config --cflags`
CFLAGS += -O3 -std=c99 -Wall

# SDL options
LIBS += `sdl-config --libs`
LIBS += -lSDL_ttf -lSDL_image 


all:Build

Build:
	$(CC) $(S_FILES) -o $(EXEC) $(LIBS) $(CFLAGS)

clean:
	rm $(EXEC)