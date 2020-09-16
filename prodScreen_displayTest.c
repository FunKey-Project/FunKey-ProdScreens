#include "funkey_prod_screens.h"


/// -------------- FUNCTIONS IMPLEMENTATION --------------
int launch_prod_screen_display(int argc, char *argv[]){
    SDL_Event event;
    SDL_Surface *text_surface = NULL;
    SDL_Rect text_pos;
    int stop_menu_loop = 0;
    int timeout = 31;
    int prev_ms = 0;
    int res = EXIT_FAILURE;

    /*  Main loop */
    while (!stop_menu_loop && timeout)
    {
        if(SDL_GetTicks() - prev_ms > 1000){

            /* Update time*/
            prev_ms = SDL_GetTicks();
            char str_title[50];
            timeout--;

            /* Fill screen white */
            SDL_FillRect(hw_surface, NULL, SDL_MapRGBA(hw_surface->format, bg_color.r, bg_color.g, bg_color.b, 0) );

            /* Write Title */
            text_surface = TTF_RenderText_Shaded(font_title, "FUNKEY PCBA TESTS", text_color, bg_color);
            text_pos.x = SCREEN_HORIZONTAL_SIZE/2 - text_surface->w/2;
            text_pos.y = Y_PADDING;
            SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
            SDL_FreeSurface(text_surface);

            /* Write "Screen ok ? */
            text_surface = TTF_RenderText_Shaded(font_title, "SCREEN OK ?", text_color, bg_color);
            text_pos.x = SCREEN_HORIZONTAL_SIZE/2 - text_surface->w/2;
            text_pos.y = SCREEN_VERTICAL_SIZE/2 - text_surface->h/2 - Y_PADDING;
            SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
            SDL_FreeSurface(text_surface);

            /* Write timeout */
            sprintf(str_title, "%d", timeout);
            text_surface = TTF_RenderText_Shaded(font_title, str_title, text_color, bg_color);
            text_pos.x = SCREEN_HORIZONTAL_SIZE/2 - text_surface->w/2;
            text_pos.y = SCREEN_VERTICAL_SIZE/2 - text_surface->h/2 + Y_PADDING;
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
            SDL_Color green_color={20,220,20};
            text_surface = TTF_RenderText_Shaded(font_info, "Press", green_color, bg_color);
            text_pos.x = SCREEN_HORIZONTAL_SIZE - text_surface->w - X_PADDING;
            text_pos.y = SCREEN_VERTICAL_SIZE - Y_PADDING - 2*text_surface->h;
            SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
            SDL_FreeSurface(text_surface);
            text_surface = TTF_RenderText_Shaded(font_info, "R=OK", green_color, bg_color);
            text_pos.x = SCREEN_HORIZONTAL_SIZE - text_surface->w - X_PADDING;
            text_pos.y = SCREEN_VERTICAL_SIZE - Y_PADDING - text_surface->h;
            SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
            SDL_FreeSurface(text_surface);

            /// Render screen
            SDL_Flip(hw_surface);
        }

        /* Handle events */
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

        /* Sleep for some time */
        SDL_Delay(SLEEP_PERIOD_MS);
    }

    return res;
}