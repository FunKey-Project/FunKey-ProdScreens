#include "funkey_prod_screens.h"
#include <signal.h> 

/* Static variable */
static int stop_menu_loop = 0;

/* Handler for SIGUSR1, caused by closing the console */
void handle_sigusr1(int sig) 
{ 
    //printf("Caught signal USR1 %d\n", sig); 
    stop_menu_loop = 1;
}

/// -------------- FUNCTIONS IMPLEMENTATION --------------
static int wait_event_loop(){

    SDL_Event event;
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

                //case SDLK_n:
                case SDLK_ESCAPE:
                    stop_menu_loop = 1;
                    res = 0;
                    break;

                default:
                    break;
            }
        }

        /* To inverstigate but with Buildroot, we need this: */
        SDL_Flip(hw_surface);

        /* Sleep for some time */
        SDL_Delay(SLEEP_PERIOD_MS);
    }

    return res;
}

int launch_prod_screen_magnet(int argc, char *argv[]){
    SDL_Surface *text_surface = NULL;
    SDL_Rect text_pos;

    /* Init Signals */
    signal(SIGUSR1, handle_sigusr1); 

    /* Fill screen white */
    SDL_FillRect(hw_surface, NULL, SDL_MapRGBA(hw_surface->format, bg_color.r, bg_color.g, bg_color.b, 0) );

    /* Write Title */
    text_surface = TTF_RenderText_Shaded(font_title, "FunKey PCBA Tests", text_color, bg_color);
    text_pos.x = SCREEN_HORIZONTAL_SIZE/2 - text_surface->w/2;
    text_pos.y = Y_PADDING;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);

    /* Write "Screen ok ? */
    text_surface = TTF_RenderText_Shaded(font_title, "MAGNET TEST", text_color, bg_color);
    text_pos.x = SCREEN_HORIZONTAL_SIZE/2 - text_surface->w/2;
    text_pos.y = SCREEN_VERTICAL_SIZE/2 - text_surface->h/2;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);

    /* Write:
        "Press 
        L=FAIL
    */
    SDL_Color red_color={220,20,20};
    text_surface = TTF_RenderText_Shaded(font_info, "Press", red_color, bg_color);
    text_pos.x = X_PADDING;
    text_pos.y = SCREEN_VERTICAL_SIZE - Y_PADDING - 2*text_surface->h;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);
    text_surface = TTF_RenderText_Shaded(font_info, "L=FAIL", red_color, bg_color);
    text_pos.x = X_PADDING;
    text_pos.y = SCREEN_VERTICAL_SIZE - Y_PADDING - text_surface->h;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);

    /* Write:
        Press 
        R=OK"
    */
    /*SDL_Color green_color={20,220,20};
    text_surface = TTF_RenderText_Shaded(font_info, "Press", green_color, bg_color);
    text_pos.x = SCREEN_HORIZONTAL_SIZE - text_surface->w - X_PADDING;
    text_pos.y = SCREEN_VERTICAL_SIZE - Y_PADDING - 2*text_surface->h;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);
    text_surface = TTF_RenderText_Shaded(font_info, "R=OK", green_color, bg_color);
    text_pos.x = SCREEN_HORIZONTAL_SIZE - text_surface->w - X_PADDING;
    text_pos.y = SCREEN_VERTICAL_SIZE - Y_PADDING - text_surface->h;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);*/

    /// Render screen
    //SDL_Flip(hw_surface);

    /// 
    int res = wait_event_loop();
    return res;
}