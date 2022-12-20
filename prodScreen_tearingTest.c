#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
//#include <SDL/SDL_image.h>
#include "funkey_prod_screens.h"

static int bright = 0;
FILE *fptr;

/// -------------- FUNCTIONS IMPLEMENTATION --------------
static int wait_event_loop(uint32_t fps){

    SDL_Event event;
    int stop_menu_loop = 0;
    int res = EXIT_FAILURE;
    uint32_t prev_ms = SDL_GetTicks();
    uint32_t cur_ms = SDL_GetTicks();
 
    /// -------- Main loop ---------
    while (!stop_menu_loop)
    {
        /// -------- Handle Keyboard Events ---------
        while (SDL_PollEvent(&event))
        switch(event.type)
        {
            case SDL_QUIT:
                stop_menu_loop = 1;
        break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {

                case SDLK_m:
                    stop_menu_loop = 1;
                    res = ERROR_MANUAL_FAIL;
                    break;

                case SDLK_n:
                case SDLK_ESCAPE:
                    stop_menu_loop = 1;
                    res = 0;
                    break;

                default:
                    break;
            }
        }

        /* Fill screen random */
        SDL_Color current_color = {rand() % 256 + 128*bright, 
                                rand() % 256 + 128*bright, 
                                rand() % 256 + 128*bright};
        SDL_FillRect(hw_surface, NULL, SDL_MapRGBA(hw_surface->format, 
            current_color.r, current_color.g, current_color.b, 0) );
        bright = 1-bright;

        /* Flip screen */
        //system("echo 1 > /sys/class/graphics/fb0/switch_backbuf");
        SDL_Flip(hw_surface);

        /* Handle FPS */
        if(fps){
            cur_ms = SDL_GetTicks();
            if(cur_ms-prev_ms < 1000/fps){
                SDL_Delay(1000/fps - (cur_ms-prev_ms));
            }
            prev_ms = SDL_GetTicks();
        }
    }

    return res;
}

int launch_prod_screen_tearingtest(int argc, char *argv[]){
    SDL_Surface *text_surface = NULL;
    SDL_Rect text_pos;

    int fps = 0; // non stop
    if(argc > 0 && argv[0] != NULL){
        fps = atoi(argv[0]);
        if(!fps){
            printf("Cannot convert %s to int\n", argv[0]);
            return EXIT_FAILURE;
        }
    }
    //printf("fps = %d, argv[0] = %s\n", fps, argv[0]);

    /// Main loop
    int res = wait_event_loop(fps);

    return res;
}