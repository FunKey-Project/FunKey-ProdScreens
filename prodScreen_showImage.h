#ifndef __PROD_SCREEN_SHOW_IMAGE__
#define __PROD_SCREEN_SHOW_IMAGE__

int launch_prod_screen_showImage(int argc, char *argv[]);
SDL_Surface * zoomSurface(SDL_Surface *src_surface, int dst_width, int dst_height);

#endif //__PROD_SCREEN_SHOW_IMAGE__