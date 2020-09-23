
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
//#include <SDL/SDL_image.h>
#include "funkey_prod_screens.h"


/* Global variables */
SDL_Surface *hw_surface = NULL;
TTF_Font *font_title = NULL;
TTF_Font *font_info = NULL;
SDL_Color bg_color = {COLOR_BG_R, COLOR_BG_G, COLOR_BG_B};
SDL_Color text_color = {COLOR_TEXT_R, COLOR_TEXT_G, COLOR_TEXT_B};

/* Static Variables */
static s_prod_test prod_tests[] = {
    {"FAIL", launch_prod_screen_fail, 0},
    {"WAIT_BATTERY", launch_prod_screen_waitbattery, 0},
    {"DISPLAY", launch_prod_screen_display, 0},
    {"BRIGHTNESS", launch_prod_screen_brightness, 0},
    {"BUTTONS", launch_prod_screen_buttons, 0},
    {"SPEAKER", launch_prod_screen_speaker, 0},
    {"LED", launch_prod_screen_LED, 0},
    {"MAGNET", launch_prod_screen_magnet, 0},
    {"VALIDATE", launch_prod_screen_validation, 0},
    {"SHOW_IMAGE", launch_prod_screen_showImage, 1}
};
static int idx_current_prod_test = 0;


/// -------------- FUNCTIONS IMPLEMENTATION --------------
void init_libraries(){
    
    /* export SDL_NOMOUSE=1 */
    putenv(strdup("SDL_NOMOUSE=1"));
    
    /// Init SDL Video
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "ERROR init SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /// Init fonts library
    if(TTF_Init())
    {
        fprintf(stderr, "Error TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    /// ----- Loading the fonts -----
    font_title = TTF_OpenFont(FONT_NAME_TITLE, FONT_SIZE_TITLE);
    if(!font_title){
        printf("ERROR in init_menu_SDL: Could not open menu font %s, %s\n", FONT_NAME_TITLE, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    font_info = TTF_OpenFont(FONT_NAME_INFO, FONT_SIZE_INFO);
    if(!font_info){
        printf("ERROR in init_menu_SDL: Could not open menu font %s, %s\n", FONT_NAME_INFO, SDL_GetError());
        exit(EXIT_FAILURE);
    }

    
    /// Open HW screen and set video mode 240x240
    hw_surface = SDL_SetVideoMode(SCREEN_HORIZONTAL_SIZE, SCREEN_VERTICAL_SIZE, 
                        32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (hw_surface == NULL)
    {
        fprintf(stderr, "ERROR SDL_SetVideoMode: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_ShowCursor(0);
    char prog_name[50];
    sprintf(prog_name, "FunKey_Prod_%s", prod_tests[idx_current_prod_test].cmd_line_argument );
    SDL_WM_SetCaption(prog_name, NULL);
}

void deinit_libraries(){
    /// ------ Close font -------
    TTF_CloseFont(font_title);
    TTF_CloseFont(font_info);

    /// deinit libs
    TTF_Quit();
    SDL_Quit();
}

void usage(char *progname){
    int i;
    fprintf(stderr, "Usage: %s [prod_test] [optionnal: arg]\n\n", progname);
    fprintf(stderr, "\"prod_tests\" in:\n");
    for (i = 0; i < sizeof(prod_tests)/sizeof(prod_tests[0]); i++ ){
        if(!prod_tests[i].nb_args_needed){
            fprintf(stderr, "       %s\n", prod_tests[i].cmd_line_argument);
        }
        else{
            fprintf(stderr, "       %s [needs %d additional args]\n", 
                prod_tests[i].cmd_line_argument, prod_tests[i].nb_args_needed);   
        }
    }
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{   
    int i;
    int res = ERROR_MANUAL_FAIL;

    if(argc < 2){
        usage(argv[0]);
    }

    char * prod_test_str = argv[1];
    int test_found = 0;
        
    /* Check argument */
    for (i = 0; i < sizeof(prod_tests)/sizeof(prod_tests[0]); i++ ){
        if(!strcmp(prod_tests[i].cmd_line_argument, prod_test_str)){
            test_found = 1;
            idx_current_prod_test = i;
            break;
        }
    }

    if(test_found && (prod_tests[i].nb_args_needed+2 != argc) ){
        fprintf(stderr, "ERROR: %s needs %d additional args\n", 
            prod_tests[idx_current_prod_test].cmd_line_argument, 
            prod_tests[idx_current_prod_test].nb_args_needed);   
        exit(EXIT_FAILURE);          
    }

    if(!test_found){
        usage(argv[0]);
    } 

    /// Init SDL
    init_libraries();

    /// Launch Program
    res = prod_tests[idx_current_prod_test].ptr_function_launch_test(argc-2, &argv[2]);

    /// Deinit SDL
    deinit_libraries();

    return res;
}
