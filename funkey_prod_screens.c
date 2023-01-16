
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
//#include <SDL/SDL_image.h>
#include "funkey_prod_screens.h"
#include <signal.h>



/* Global variables */
SDL_Surface *hw_surface = NULL;
TTF_Font *font_title = NULL;
TTF_Font *font_info = NULL;
SDL_Color bg_color = {COLOR_BG_R, COLOR_BG_G, COLOR_BG_B};
SDL_Color text_color = {COLOR_TEXT_R, COLOR_TEXT_G, COLOR_TEXT_B};
char *prog_title = "FUNKEY S TESTS";
int display_width = 0;
int display_height = 0;

/* Static Variables */
static s_prod_test prod_tests[] = {
    {"FAIL", launch_prod_screen_fail, 0, NULL},
    {"WAIT_BATTERY", launch_prod_screen_waitbattery, 0, NULL},
    {"DISPLAY", launch_prod_screen_display, 0, NULL},
    {"BRIGHTNESS", launch_prod_screen_brightness, 0, NULL},
    {"BUTTONS", launch_prod_screen_buttons, 0, NULL},
    {"SPEAKER", launch_prod_screen_speaker, 0, NULL},
    {"LED", launch_prod_screen_LED, 0, NULL},
    {"MAGNET", launch_prod_screen_magnet, 0, NULL},
    {"VALIDATE", launch_prod_screen_validation, 0, NULL},
    {"SHOW_IMAGE", launch_prod_screen_showImage, 1, "img_path"},
    {"GAMMA", launch_prod_screen_gamma, 0, NULL},
    {"TEARING", launch_prod_screen_tearingtest, 0, "(FPS)"},
    {"TESTS", launch_prod_screen_tests, 0, NULL}
};
static int idx_current_prod_test = 0;


/// -------------- FUNCTIONS IMPLEMENTATION --------------
void deinit_libraries(){
    /// ------ Close font -------
    TTF_CloseFont(font_title);
    TTF_CloseFont(font_info);

    /// deinit libs
    TTF_Quit();
    SDL_Quit();
}

void intHandler(int dummy) {
    deinit_libraries();
    exit(EXIT_FAILURE);
}

void init_libraries(){

    /* Vars */
    SDL_Rect **modes;

    /* Catch SIGINT and exit */
    signal(SIGINT, intHandler);
    
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

    /* Loading the fonts */
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

    /* Get available fullscreen/hardware modes */
    modes=SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_DOUBLEBUF | SDL_HWSURFACE);

    /* Check is there are any modes available */
    if(modes == (SDL_Rect **)0){
        printf("No modes available!\n");
        exit(-1);
    }

    /* Check if our resolution is restricted */
    if(modes == (SDL_Rect **)-1){
        printf("All resolutions available.\n");
    }
    else{
        /* Print valid modes */
        printf("Available Modes:\n");
        for(int i=0;modes[i];++i){
            printf("  %d x %d\n", modes[i]->w, modes[i]->h);
        }
    }

    /* Get display dimensions (1st returned mode) */
    display_width = modes[0]->w;
    display_height = modes[0]->h;

    /// Open HW screen and set video mode 240x240
    hw_surface = SDL_SetVideoMode(display_width, display_height, 
                        16, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    /*hw_surface = SDL_SetVideoMode(display_width, display_height, 
                        32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);*/
    if (hw_surface == NULL)
    {
        fprintf(stderr, "ERROR SDL_SetVideoMode: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Extra */
    SDL_ShowCursor(0);
    char prog_name[50];
    sprintf(prog_name, "FunKey_Prod_%s", prod_tests[idx_current_prod_test].cmd_line_argument );
    SDL_WM_SetCaption(prog_name, NULL);
}

void usage(char *progname){
    int i;
    fprintf(stderr, "Usage: %s [prod_test] [optionnal: arg]\n\n", progname);
    fprintf(stderr, "\"prod_tests\" in:\n");
    for (i = 0; i < sizeof(prod_tests)/sizeof(prod_tests[0]); i++ ){
        if(prod_tests[i].args_description == NULL){
            fprintf(stderr, "       %s\n", prod_tests[i].cmd_line_argument);
        }
        else{
            fprintf(stderr, "       %s %s\n", 
                prod_tests[i].cmd_line_argument, prod_tests[i].args_description);   
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

    if(test_found && (argc < prod_tests[i].nb_args_needed+2) ){
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
