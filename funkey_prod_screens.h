#ifndef __FUNKEY_PROD_SCREENS__
#define __FUNKEY_PROD_SCREENS__

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include "prodScreen_failScreen.h"
#include "prodScreen_waitBattery.h"
#include "prodScreen_displayTest.h"
#include "prodScreen_brightnessTest.h"
#include "prodScreen_buttonsTest.h"
#include "prodScreen_speakerTest.h"
#include "prodScreen_ledTest.h"
#include "prodScreen_magnetTest.h"
#include "prodScreen_validation.h"
#include "prodScreen_showImage.h"
#include "prodScreen_gamma.h"
#include "prodScreen_tearingTest.h"


/// Defines
#define ERROR_MANUAL_FAIL       2

#define SCREEN_HORIZONTAL_SIZE  240
#define SCREEN_VERTICAL_SIZE    240

#define SLEEP_PERIOD_MS         100

#define COLOR_BG_R      255
#define COLOR_BG_G      255
#define COLOR_BG_B      255
#define COLOR_TEXT_R    130
#define COLOR_TEXT_G    30
#define COLOR_TEXT_B    160

#define Y_PADDING       10
#define X_PADDING       20

#define FOLDER_RESSOURCES	  	"/usr/local/share/ProdResources"	
#define FONT_NAME_TITLE        	FOLDER_RESSOURCES"/FreeSansBold.ttf"
#define FONT_SIZE_TITLE        	20
#define FONT_NAME_INFO         	FONT_NAME_TITLE
#define FONT_SIZE_INFO         	18

typedef struct 
{
    char *cmd_line_argument;
    int (*ptr_function_launch_test)(int argc, char *argv[]);
    int nb_args_needed;
    const char *args_description;
} s_prod_test;


/// Global variables
extern SDL_Surface *hw_surface;
extern TTF_Font *font_title;
extern TTF_Font *font_info;
extern SDL_Color bg_color;
extern SDL_Color text_color;
extern char *prog_title;


#endif //__FUNKEY_PROD_SCREENS__
