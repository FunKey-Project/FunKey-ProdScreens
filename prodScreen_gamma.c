#include "funkey_prod_screens.h"


/// Defines

/// Static variables


/// -------------- FUNCTIONS IMPLEMENTATION --------------
int launch_prod_screen_gamma(int argc, char *argv[]){
    SDL_Event event;
    //SDL_Surface *text_surface = NULL;
    //SDL_Rect text_pos;
    int res = EXIT_FAILURE;
    int stop_menu_loop = 0;

    /* Fill screen white */
    SDL_FillRect(hw_surface, NULL, SDL_MapRGBA(hw_surface->format, bg_color.r, bg_color.g, bg_color.b, 0) );

#if 0
    /* Write Title */
    text_surface = TTF_RenderText_Shaded(font_info, "GAMMA", text_color, bg_color);
    int height_title = text_surface->h;
    text_pos.x = hw_surface->w/2 - text_surface->w/2;
    text_pos.y = 0;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);

    /* Write:
        "L=FAIL"
    */
    SDL_Color red_color={220,20,20};
    text_surface = TTF_RenderText_Shaded(font_info, "L=FAIL", red_color, bg_color);
    int height_buttons = text_surface->h;
    text_pos.x = X_PADDING;
    text_pos.y = hw_surface->h  - text_surface->h;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);

    /* Write:
        "R=OK"
    */
    SDL_Color green_color={20,220,20};
    text_surface = TTF_RenderText_Shaded(font_info, "R=DONE", green_color, bg_color);
    text_pos.x = hw_surface->w - text_surface->w - X_PADDING;
    text_pos.y = hw_surface->h  - text_surface->h;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);
#endif

    /* Load Img */
    SDL_Surface *image=IMG_Load(IMG_CONSOLE_COLOR_CHART);
    if(!image) {
        printf("ERROR IMG_Load: %s\n", IMG_GetError());
        printf("IMG path is: %s\n", IMG_CONSOLE_COLOR_CHART);
        exit(1);
    }
    SDL_SetAlpha( image, 0, SDL_ALPHA_OPAQUE );

    /* Convert to RGBA 32bits*/
    SDL_Surface *image_rgb_RGBA32b = SDL_CreateRGBSurface(SDL_SWSURFACE, image->w, image->h, 32, 
      image->format->Rmask, image->format->Gmask,
      image->format->Bmask, image->format->Amask);
    SDL_BlitSurface(image, NULL, image_rgb_RGBA32b, NULL);
    SDL_FreeSurface(image);

    /* Resize image */
    SDL_Surface *image_RGBA32b_resized = zoomSurface(image_rgb_RGBA32b, hw_surface->w, hw_surface->h);
    SDL_FreeSurface(image_rgb_RGBA32b);
    
    /* Blit image */
    SDL_BlitSurface(image_RGBA32b_resized, NULL, hw_surface, NULL);
    SDL_FreeSurface(image_RGBA32b_resized);

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

        /* To investigate but with Buildroot, we need this: */
        SDL_Flip(hw_surface);

        /* Sleep for some time */
        SDL_Delay(SLEEP_PERIOD_MS);
    }

    return res;
}

