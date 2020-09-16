#include "funkey_prod_screens.h"


/// Defines

/// Static variables


/// -------------- FUNCTIONS IMPLEMENTATION --------------

/// Nearest neighboor optimized with possible out of screen coordinates (for cropping)
SDL_Surface * zoomSurface(SDL_Surface *src_surface, int dst_width, int dst_height){

  /* Declare vars */
  int x_ratio;
  int y_ratio;
  int x2, y2;
  int i, j;
  int rat;

  /* Sanity checks */
  if(src_surface == NULL){
    printf("ERROR in %s, sanity check\n", __func__);
    return NULL;
  }

  /* Compute zoom ratio */
  x_ratio = (int)((src_surface->w << 16) / dst_width);
  y_ratio = (int)((src_surface->h << 16) / dst_height);

  /* Create dst surface */
  SDL_Surface *dst_surface = SDL_CreateRGBSurface(src_surface->flags,
      dst_width, dst_height,
      src_surface->format->BitsPerPixel,
      src_surface->format->Rmask, src_surface->format->Gmask,
      src_surface->format->Bmask, src_surface->format->Amask);
  if(dst_surface == NULL){
    printf("ERROR in %s, cannot create dst_surface: %s\n", __func__, SDL_GetError());
  }

  /* Columns iterations */
  for (i = 0; i < dst_surface->h; i++)
  {

    /* Get current lines in src and dst surfaces */
    uint8_t* t = ( (uint8_t*) dst_surface->pixels + (i*dst_surface->w)*dst_surface->format->BytesPerPixel );
    y2 = ((i*y_ratio)>>16);
    uint8_t* p = ( (uint8_t*) src_surface->pixels + (y2*src_surface->w)*src_surface->format->BytesPerPixel );
    rat =  0;

    /* Lines iterations */
    for (j = 0; j < dst_surface->w; j++)
    {

      /* Get current pixel in src surface */
      x2 = (rat>>16);

      /* Copy src pixel in dst surface */
      memcpy(t, p+x2*src_surface->format->BytesPerPixel, dst_surface->format->BytesPerPixel);
      t += dst_surface->format->BytesPerPixel;

      /* Update x position in source surface */
      rat += x_ratio;
    }
  }

  /* Return new zoomed surface */
  return dst_surface;
}




int launch_prod_screen_showImage(int argc, char *argv[]){
    SDL_Event event;
    SDL_Surface *text_surface = NULL;
    SDL_Rect text_pos;
    int res = EXIT_FAILURE;
    int stop_menu_loop = 0;

    /* Fill screen white */
    SDL_FillRect(hw_surface, NULL, SDL_MapRGBA(hw_surface->format, bg_color.r, bg_color.g, bg_color.b, 0) );

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

    /* Load Img */
    char *img_path = argv[0];
    SDL_Surface *image=IMG_Load(img_path);
    if(!image) {
        printf("ERROR IMG_Load: %s\n", IMG_GetError());
        printf("IMG path is: %s\n", img_path);
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
    int new_img_height = hw_surface->h - height_buttons - height_title;
    int new_img_width = image->w *new_img_height / image->h;
    SDL_Surface *image_RGBA32b_resized = zoomSurface(image_rgb_RGBA32b, new_img_width, new_img_height);
    //SDL_SaveBMP(image_RGBA32b_resized,"./image_RGBA32b_resized.bmp");
    SDL_FreeSurface(image_rgb_RGBA32b);
    
    /* Blit image */
    SDL_Rect pos_img = {(hw_surface->w-image_RGBA32b_resized->w)/2, height_title, image_RGBA32b_resized->w, image_RGBA32b_resized->h};
    SDL_BlitSurface(image_RGBA32b_resized, NULL, hw_surface, &pos_img);
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

