#include "funkey_prod_screens.h"


/// Defines

/// Static variables


/// -------------- FUNCTIONS IMPLEMENTATION --------------

/* Nearest neighboor optimized with possible out of screen coordinates (for cropping) */
void flip_NNOptimized_AllowOutOfScreen(SDL_Surface *src_surface, SDL_Surface *dst_surface, int new_w, int new_h){
  int w1=src_surface->w;
  //int h1=src_surface->h;
  int w2=new_w;
  int h2=new_h;
  int x_ratio = (int)((src_surface->w<<16)/w2);
  int y_ratio = (int)((src_surface->h<<16)/h2);
  int x2, y2;
  int i, j;

  /* Compute padding for centering when out of bounds */
  int x_padding = 0, y_padding = 0;
  if(h2>SCREEN_HORIZONTAL_SIZE){
     y_padding = (SCREEN_VERTICAL_SIZE-new_h)/2;
  }
  if(w2>SCREEN_HORIZONTAL_SIZE){
    x_padding = (w2-SCREEN_HORIZONTAL_SIZE)/2 + 1;
  }
  int x_padding_ratio = x_padding*w1/w2;

  /* Copy pixels NN */
  for (i=0;i<h2;i++) 
  {
    if(i>=SCREEN_VERTICAL_SIZE){
      continue;
    }

    uint16_t* t = (uint16_t*)(dst_surface->pixels+((i+y_padding)* ((w2>SCREEN_HORIZONTAL_SIZE)?SCREEN_HORIZONTAL_SIZE:w2) )*sizeof(uint16_t));
    y2 = ((i*y_ratio)>>16);
    uint16_t* p = (uint16_t*)(src_surface->pixels + (y2*w1 + x_padding_ratio) *sizeof(uint16_t));
    int rat = 0;
    for (j=0;j<w2;j++) 
    {
      if(j>=SCREEN_HORIZONTAL_SIZE){
        continue;
      }
      x2 = (rat>>16);
      *t++ = p[x2];
      rat += x_ratio;
    } 
  }
}


int launch_prod_screen_showImage(int argc, char *argv[]){
    SDL_Event event;
    SDL_Surface *text_surface = NULL;
    SDL_Rect text_pos;
    int res = EXIT_FAILURE;
    int stop_menu_loop = 0;

    /* Load Img */
    char *img_path = argv[0];
    SDL_Surface *image=IMG_Load(img_path);
    if(!image) {
        printf("ERROR IMG_Load: %s\n", IMG_GetError());
        printf("IMG path is: %s\n", img_path);
        exit(1);
    }

    /* Fill screen white */
    SDL_FillRect(hw_surface, NULL, SDL_MapRGB(hw_surface->format, bg_color.r, bg_color.g, bg_color.b));

    /* Write Title */
    text_surface = TTF_RenderText_Shaded(font_info, "SCAN & PRINT", text_color, bg_color);
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

    /* Convert img to RGB565 */
    SDL_Surface *image_rgb565 = SDL_CreateRGBSurface(SDL_SWSURFACE, image->w, image->h, 16, 0, 0, 0, 0);
    SDL_BlitSurface(image, NULL, image_rgb565, NULL);
    SDL_FreeSurface(image);

    /* Resize img */
    int new_img_height = hw_surface->h - height_buttons - height_title;
    int new_img_width = image->w *new_img_height / image->h;
    SDL_Surface *image_rgb565_resized = SDL_CreateRGBSurface(SDL_SWSURFACE, new_img_width, new_img_height, 16, 0, 0, 0, 0);
    flip_NNOptimized_AllowOutOfScreen(image_rgb565, image_rgb565_resized, image_rgb565_resized->w, image_rgb565_resized->h);
    SDL_FreeSurface(image_rgb565);
    
    /* Blit image */
    SDL_Rect pos_img = {(hw_surface->w-image_rgb565_resized->w)/2, height_title, image_rgb565_resized->w, image_rgb565_resized->h};
    SDL_BlitSurface(image_rgb565_resized, NULL, hw_surface, &pos_img);
    SDL_FreeSurface(image_rgb565_resized);

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

