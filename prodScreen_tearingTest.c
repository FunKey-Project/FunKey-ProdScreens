#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
//#include <SDL/SDL_image.h>
#include "funkey_prod_screens.h"

static int bright = 0;

/// -------------- FUNCTIONS IMPLEMENTATION --------------
static int wait_event_loop(){

    SDL_Event event;
    int stop_menu_loop = 0;
    int prev_ms = 0;
    int res = EXIT_FAILURE;
 
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
        SDL_Color current_color = {rand() % 128 + 128*bright, 
                                rand() % 256 + 128*bright, 
                                rand() % 256 + 128*bright};
        SDL_FillRect(hw_surface, NULL, SDL_MapRGBA(hw_surface->format, 
            current_color.r, current_color.g, current_color.b, 0) );
        bright = 1-bright;

        /* To investigate but with Buildroot, we need this: */
        SDL_Flip(hw_surface);

        /* Sleep for some time */
        //SDL_Delay(SLEEP_PERIOD_MS);
    }

    return res;
}

int launch_prod_screen_tearingtest(int argc, char *argv[]){
    SDL_Surface *text_surface = NULL;
    SDL_Rect text_pos;

    /// Main loop
    int res = wait_event_loop();
    return res;
}