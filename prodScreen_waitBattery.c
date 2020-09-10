#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
//#include <SDL/SDL_image.h>
#include "funkey_prod_screens.h"


/// -------------- FUNCTIONS IMPLEMENTATION --------------
static int is_battery_present(){
    char buf[10];
    FILE *fp;
    int res = 0;

    /* Read battery file */
    if ((fp = fopen(BATTERY_PRESENT_FILE, "r")) == NULL) {
        printf("Error! opening file: %s\n", BATTERY_PRESENT_FILE);
        exit(EXIT_FAILURE);
    }
    if(!fscanf(fp, "%s", buf)) exit(EXIT_FAILURE);
    fclose(fp);

    /* Check batery present */
    //printf("%s\n", buf);
    if(atoi(buf) == 1){
        res = 1;
    }

    return res;
}

static int wait_event_loop(){

    SDL_Event event;
    int stop_menu_loop = 0;
    int prev_ms = 0;
    int res = ERROR_MANUAL_FAIL;
 
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

        if(SDL_GetTicks() - prev_ms > CHECK_BATTERY_DELAY_MS){

            /* Update time*/
            prev_ms = SDL_GetTicks();

            /* Check batery present */
            if(is_battery_present() == 1){
                stop_menu_loop = 1;
                res = 0;
            }
        }

        /* Sleep for some time */
        SDL_Delay(SLEEP_PERIOD_MS);
    }

    return res;
}

int launch_prod_screen_waitbattery(int argc, char *argv[]){
    SDL_Surface *text_surface = NULL;
    SDL_Rect text_pos;

    /* Fill screen white */
    SDL_FillRect(hw_surface, NULL, SDL_MapRGB(hw_surface->format, bg_color.r, bg_color.g, bg_color.b));

    /* Write Title */
    text_surface = TTF_RenderText_Shaded(font_title, "FUNKEY PCBA TESTS", text_color, bg_color);
    text_pos.x = SCREEN_HORIZONTAL_SIZE/2 - text_surface->w/2;
    text_pos.y = Y_PADDING;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);

    /* Write "Screen ok ? */
    SDL_Color red={255,0,0};
    text_surface = TTF_RenderText_Shaded(font_title, "INSERT BATTERY", red, bg_color);
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