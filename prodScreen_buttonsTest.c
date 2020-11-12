#include "funkey_prod_screens.h"


/// Defines
#define time_left_FAIL            101 // in seconds
#define NB_KEYS                 13

/// Static variables
static SDL_Surface *img_console_layout;
static SDL_Surface *img_button_LR_green;
static SDL_Surface *img_button_normal_green;
static int keys[NB_KEYS] = {SDLK_m, SDLK_n, SDLK_l, SDLK_u, SDLK_r, SDLK_d, SDLK_b, SDLK_y, SDLK_x, SDLK_a, SDLK_s, SDLK_k, SDLK_q};
static char *keys_str[NB_KEYS] = {"m (L1)", "n (R1)", "l (LEFT)", "u (UP)", "r (RIGHT)", "d (DOWN)", "b (B)", "y (Y)", "x (X)", "a (A)", "s (START)", "k (Select)", "q (POWER ON/OFF)"};
static int keys_pushed[NB_KEYS] = {0};


/// -------------- FUNCTIONS IMPLEMENTATION --------------
int launch_prod_screen_buttons(int argc, char *argv[]){

    /* Declare Vars */
    SDL_Surface *text_surface = NULL;
    SDL_Rect text_pos;
    SDL_Event event;
    int stop_menu_loop = 0;
    int res = EXIT_FAILURE;
    int render = 0;
    int time_left = time_left_FAIL;
    int prev_ms = 0;
    int i;
    char str_title_buttons[50];
    int nb_keys_pushed = 0;

    /* Load images */
    img_console_layout = IMG_Load(IMG_CONSOLE_LAYOUT);
    if(!img_console_layout) {
        printf("Error IMG_Load: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    img_button_LR_green = IMG_Load(IMG_BUTTON_LR_GREEN);
    if(!img_button_LR_green) {
        printf("Error IMG_Load: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    img_button_normal_green = IMG_Load(IMG_BUTTON_NORMAL_GREEN);
    if(!img_button_normal_green) {
        printf("Error IMG_Load: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    /*  Main loop */
    while (!stop_menu_loop && time_left)
    {
        if(SDL_GetTicks() - prev_ms > 1000){

            /* Update time*/
            prev_ms = SDL_GetTicks();
            time_left--;
            
            /* Fill screen white */
            SDL_FillRect(hw_surface, NULL, SDL_MapRGBA(hw_surface->format, bg_color.r, bg_color.g, bg_color.b, 0) );

            /* Background image */
            SDL_BlitSurface(img_console_layout, NULL, hw_surface, NULL);

            /* Write Title */
            text_surface = TTF_RenderText_Shaded(font_title, prog_title, text_color, bg_color);
            text_pos.x = SCREEN_HORIZONTAL_SIZE/2 - text_surface->w/2;
            text_pos.y = Y_PADDING;
            SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
            SDL_FreeSurface(text_surface);

            /* Write "Screen ok ? */
            sprintf(str_title_buttons, "Press all buttons...%ds", time_left);
            text_surface = TTF_RenderText_Shaded(font_info, str_title_buttons, text_color, bg_color);
            text_pos.x = SCREEN_HORIZONTAL_SIZE/2 - text_surface->w/2;
            text_pos.y = 2*Y_PADDING + text_surface->h/2;
            SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
            SDL_FreeSurface(text_surface);


            /* Write key SDLK_m*/
            if(keys_pushed[0]){
                SDL_Rect rect_tmp = {13, 68, img_button_LR_green->w, img_button_LR_green->h};
                SDL_BlitSurface(img_button_LR_green, NULL, hw_surface, &rect_tmp);
            }
            /* Write key SDLK_n*/
            if(keys_pushed[1]){
                SDL_Rect rect_tmp = {207, 68, img_button_LR_green->w, img_button_LR_green->h};
                SDL_BlitSurface(img_button_LR_green, NULL, hw_surface, &rect_tmp);
            }

            /* Write key SDLK_l*/
            if(keys_pushed[2]){
                SDL_Rect rect_tmp = {21, 145, img_button_normal_green->w, img_button_normal_green->h};
                SDL_BlitSurface(img_button_normal_green, NULL, hw_surface, &rect_tmp);
            }
            /* Write key SDLK_u*/
            if(keys_pushed[3]){
                SDL_Rect rect_tmp = {47, 118, img_button_normal_green->w, img_button_normal_green->h};
                SDL_BlitSurface(img_button_normal_green, NULL, hw_surface, &rect_tmp);
            }
            /* Write key SDLK_r*/
            if(keys_pushed[4]){
                SDL_Rect rect_tmp = {74, 143, img_button_normal_green->w, img_button_normal_green->h};
                SDL_BlitSurface(img_button_normal_green, NULL, hw_surface, &rect_tmp);
            }
            /* Write key SDLK_d*/
            if(keys_pushed[5]){
                SDL_Rect rect_tmp = {48, 169, img_button_normal_green->w, img_button_normal_green->h};
                SDL_BlitSurface(img_button_normal_green, NULL, hw_surface, &rect_tmp);
            }

            /* Write key SDLK_b*/
            if(keys_pushed[6]){
                SDL_Rect rect_tmp = {169, 169, img_button_normal_green->w, img_button_normal_green->h};
                SDL_BlitSurface(img_button_normal_green, NULL, hw_surface, &rect_tmp);
            }
            /* Write key SDLK_y*/
            if(keys_pushed[7]){
                SDL_Rect rect_tmp = {143, 144, img_button_normal_green->w, img_button_normal_green->h};
                SDL_BlitSurface(img_button_normal_green, NULL, hw_surface, &rect_tmp);
            }
            /* Write key SDLK_x*/
            if(keys_pushed[8]){
                SDL_Rect rect_tmp = {169, 117, img_button_normal_green->w, img_button_normal_green->h};
                SDL_BlitSurface(img_button_normal_green, NULL, hw_surface, &rect_tmp);
            }
            /* Write key SDLK_a*/
            if(keys_pushed[9]){
                SDL_Rect rect_tmp = {195, 143, img_button_normal_green->w, img_button_normal_green->h};
                SDL_BlitSurface(img_button_normal_green, NULL, hw_surface, &rect_tmp);
            }

            /* Write key SDLK_s*/
            if(keys_pushed[10]){
                SDL_Rect rect_tmp = {125, 203, img_button_normal_green->w, img_button_normal_green->h};
                SDL_BlitSurface(img_button_normal_green, NULL, hw_surface, &rect_tmp);
            }
            /* Write key SDLK_f*/
            if(keys_pushed[11]){
                SDL_Rect rect_tmp = {92, 204, img_button_normal_green->w, img_button_normal_green->h};
                SDL_BlitSurface(img_button_normal_green, NULL, hw_surface, &rect_tmp);
            }

            /* Write key SDLK_q*/
            if(keys_pushed[12]){
                SDL_Rect rect_tmp = {106, 73, img_button_normal_green->w, img_button_normal_green->h};
                SDL_BlitSurface(img_button_normal_green, NULL, hw_surface, &rect_tmp);
            }

            /* Render screen */
            render = 1;
        }

        /* Handle Keyboard Events */
        while (SDL_PollEvent(&event))
        switch(event.type)
        {
            case SDL_QUIT:
                stop_menu_loop = 1;
        break;
        case SDL_KEYDOWN:
            for (i = 0; i < NB_KEYS; i++){
                if(event.key.keysym.sym != keys[i]) continue;

                /* Key pushed for the 1st time */
                if(!keys_pushed[i]){
                    nb_keys_pushed++;
                    keys_pushed[i] = 1;
                }

                /* All keys pushed = exit with success */
                if(nb_keys_pushed == NB_KEYS){
                    stop_menu_loop = 1;
                    res = 0;
                }
                
                /* Render screen */
                render = 1;

                break;
            }
        }


        if(render){ 
            /// Render screen
            SDL_Flip(hw_surface);
            render = 0;
        }

        /* Sleep for some time */
        SDL_Delay(SLEEP_PERIOD_MS);
    }

    /* Print not touched buttons if time_left */
    if(!time_left){
        res = ERROR_MANUAL_FAIL;

        printf("    Missing Keys: ");
        for (i = 0; i < NB_KEYS; i++){
            if(keys_pushed[i]) continue;
            printf("%s,", keys_str[i]);
        }
        printf("\n");
    }

    /*Free surfaces */
    SDL_FreeSurface(img_console_layout);
    SDL_FreeSurface(img_button_LR_green);
    SDL_FreeSurface(img_button_normal_green);

    return res;
}

