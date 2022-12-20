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
prodScreen_showImage.c \
prodScreen_tearingTest.c \
prodScreen_gamma.c \
prodScreen_tests.c

# Output
EXEC=funkey_prod_screens

# Build settings
CC=$(CROSS_COMPILE)gcc

# Other options
ifeq ($(platform), funkey)
	CFLAGS += $(shell /opt/FunKey-sdk-2.3.0/arm-funkey-linux-musleabihf/sysroot/usr/bin/sdl-config --cflags)
	CFLAGS += -O2 -mfloat-abi=hard -ffast-math -funsafe-math-optimizations -fno-PIC -march=armv7-a+neon-vfpv4 -mtune=cortex-a7 -mfpu=neon-vfpv4
	#CFLAGS += -O2
	LIBS += $(shell /opt/FunKey-sdk-2.3.0/arm-funkey-linux-musleabihf/sysroot/usr/bin/sdl-config --libs)
else
	CFLAGS += `sdl-config --cflags`
	LIBS += `sdl-config --libs`
endif

CFLAGS += -std=c99 -Wall
LIBS += -lSDL_ttf -lSDL_image 


all:Build

Build:
	$(CC) $(S_FILES) -o $(EXEC) $(LIBS) $(CFLAGS)

clean:
	rm $(EXEC)