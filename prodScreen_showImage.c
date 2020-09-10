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
  int y_padding = (SCREEN_VERTICAL_SIZE-new_h)/2;
  int x_padding = 0;
  if(w2>SCREEN_HORIZONTAL_SIZE){
    x_padding = (w2-SCREEN_HORIZONTAL_SIZE)/2 + 1;
  }
  int x_padding_ratio = x_padding*w1/w2;

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
    int res = 0;
    int stop_menu_loop = 0;

    /* Load Img */
    char *img_path = argv[0];
    SDL_Surface *image=IMG_Load(img_path);
    if(!image) {
        printf("ERROR IMG_Load: %s\n", IMG_GetError());
        printf("IMG path is: %s\n", img_path);
        exit(1);
    }


    /* Convert img to RGB565 */
    SDL_Surface *image_rgb565 = SDL_CreateRGBSurface(SDL_SWSURFACE, image->w, image->h, 16, 0, 0, 0, 0);
    SDL_BlitSurface(image, NULL, image_rgb565, NULL);
    SDL_FreeSurface(image);
    
    /* Scale to fullscreen */
    flip_NNOptimized_AllowOutOfScreen(image_rgb565, hw_surface, hw_surface->w, hw_surface->h);

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

