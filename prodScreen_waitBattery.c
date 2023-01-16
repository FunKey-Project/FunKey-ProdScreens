#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
//#include <SDL/SDL_image.h>
#include "funkey_prod_screens.h"


/// ---------------- DEFINES ----------------
#define BATTERY_PRESENT_FILE    	"/sys/class/power_supply/axp20x-battery/present"
#define BATTERY_VOLTAGE_NOW_FILE    "/sys/class/power_supply/axp20x-battery/voltage_now"
#define USB_PRESENT_FILE    		"/sys/class/power_supply/axp20x-usb/present"
#define CHECK_BATTERY_DELAY_MS  	100
#define NB_BAT_MESUREMENTS			50
#define Y_PADDING_BAT_RES			2
#define MIN_VALID_VOLTAGE			3100000

#define BAT_TESTS \
	X(BAT_CONNECTED, "BAT_CONNECTED") \
	X(USB_NOT_CONNECTED, "USB_NOT_CONNECTED") \
	X(BAT_VALUES_OK, "BAT_VALUES_OK") \
	X(NB_BAT_TESTS, "") 

#undef X
#define X(a, b) a,
typedef enum{BAT_TESTS} ENUM_BAT_TESTS;


/// ----------- STATIC VARS --------------
/*#undef X
#define X(a, b) b,
static const char *bat_tests_strings[] = {BAT_TESTS};*/

static int bat_measurements[NB_BAT_MESUREMENTS] = {0};
static int idx_bat_measurements = 0;
static int nb_bat_measurements = 0;
static bool false_measurement_found = false;
static bool render = true;

/// -------------- STATIC FUNCTIONS IMPLEMENTATION --------------
static int is_battery_present(){
    char buf[10];
    FILE *fp;
    int res = 0;

    /* Read battery file */
    char *file = BATTERY_PRESENT_FILE;
    if ((fp = fopen(file, "r")) == NULL) {
        printf("Error! opening file: %s\n", file);
        exit(EXIT_FAILURE);
    }
    if(!fscanf(fp, "%s", buf)) exit(EXIT_FAILURE);
    fclose(fp);

    /* Check battery present */
    //printf("In %s, res= %s\n", __func__, buf);
    if(atoi(buf) == 1){
        res = 1;
    }

    return res;
}

static int get_battery_voltage(){
    char buf[10];
    FILE *fp;
    //int res = 0;

    /* Read battery voltage file */
    char *file = BATTERY_VOLTAGE_NOW_FILE;
    if ((fp = fopen(file, "r")) == NULL) {
        printf("Error! opening file: %s\n", file);
        exit(EXIT_FAILURE);
    }
    if(!fscanf(fp, "%s", buf)) exit(EXIT_FAILURE);
    fclose(fp);

    /* Check voltage */
    //printf("In %s, res= %s\n", __func__, buf);

    return atoi(buf);
}

static int is_usb_present(){
    char buf[10];
    FILE *fp;
    int res = 0;

    /* Read usb file */
    char *file = USB_PRESENT_FILE;
    if ((fp = fopen(file, "r")) == NULL) {
        printf("Error! opening file: %s\n", file);
        exit(EXIT_FAILURE);
    }
    if(!fscanf(fp, "%s", buf)) exit(EXIT_FAILURE);
    fclose(fp);

    /* Check usb present */
    //printf("In %s, res= %s\n", __func__, buf);
    if(atoi(buf) == 1){
        res = 1;
    }

    return res;
}

static void render_static_text(){
	SDL_Surface *text_surface = NULL;
    SDL_Rect text_pos;

    /* Fill screen white */
    SDL_FillRect(hw_surface, NULL, SDL_MapRGBA(hw_surface->format, bg_color.r, bg_color.g, bg_color.b, 0) );

    /* Write Title */
    text_surface = TTF_RenderText_Shaded(font_title, prog_title, text_color, bg_color);
    text_pos.x = display_width/2 - text_surface->w/2;
    text_pos.y = Y_PADDING_BAT_RES;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);

    /* Write "Test title*/
    /*SDL_Color dark_gray={20,20,20};
    text_surface = TTF_RenderText_Shaded(font_title, "BATTERY TESTS:", dark_gray, bg_color);
    text_pos.x = X_PADDING;
    text_pos.y = display_height/2 - (text_surface->h*(NB_BAT_TESTS+1) + Y_PADDING_BAT_RES*NB_BAT_TESTS)/2;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);*/

    /* Write "Test title*/
    /*SDL_Color red={255,0,0};
    text_surface = TTF_RenderText_Shaded(font_title, "INSERT BATTERY", red, bg_color);
    text_pos.x = display_width/2 - text_surface->w/2;
    text_pos.y = display_height/2 - text_surface->h/2;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);*/

    /* Write:
        "Press 
        L=FAIL
    */
    SDL_Color red_color={220,20,20};
    text_surface = TTF_RenderText_Shaded(font_info, "Press", red_color, bg_color);
    text_pos.x = X_PADDING;
    text_pos.y = display_height - Y_PADDING_BAT_RES - 2*text_surface->h;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);
    text_surface = TTF_RenderText_Shaded(font_info, "L=FAIL", red_color, bg_color);
    text_pos.x = X_PADDING;
    text_pos.y = display_height - Y_PADDING_BAT_RES - text_surface->h;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);

    /* Write:
        Press 
        R=OK"
    */
    /*SDL_Color green_color={20,220,20};
    text_surface = TTF_RenderText_Shaded(font_info, "Press", green_color, bg_color);
    text_pos.x = display_width - text_surface->w - X_PADDING;
    text_pos.y = display_height - Y_PADDING_BAT_RES - 2*text_surface->h;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);
    text_surface = TTF_RenderText_Shaded(font_info, "R=OK", green_color, bg_color);
    text_pos.x = display_width - text_surface->w - X_PADDING;
    text_pos.y = display_height - Y_PADDING_BAT_RES - text_surface->h;
    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
    SDL_FreeSurface(text_surface);*/

    /// Render screen
    render = true;

}

static int wait_event_loop(){

    SDL_Event event;
    int stop_menu_loop = 0;
    int prev_ms = 0;
    int res = EXIT_FAILURE;
 
    /// -------- Main loop ---------
    while (!stop_menu_loop)
    {
    	/* Vars */
		bool all_tests_ok = false;

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


        /* Sleep for some time before next tests */
        if(SDL_GetTicks() - prev_ms > CHECK_BATTERY_DELAY_MS){

        	/* Render BG text */
        	render_static_text();
	        
	        /* Perform tests */
    		SDL_Surface *text_surface = NULL;
    		SDL_Rect text_pos;
    		bool bat_connected = false;
    		bool usb_connected = true;
			int cur_bat_test_idx;
			all_tests_ok = true;
			for(cur_bat_test_idx = 0; cur_bat_test_idx < NB_BAT_TESTS; cur_bat_test_idx++){

				switch(cur_bat_test_idx){

		            /* Check battery present */
		            case BAT_CONNECTED:
		            if(is_battery_present() != 1){

		            	/* bat bool*/
		            	bat_connected = false;

		            	/* Show ERROR message */
					    SDL_Color red={255,0,0};
					    text_surface = TTF_RenderText_Shaded(font_title, "CONNECT BATTERY", red, bg_color);
					    text_pos.x = display_width/2 - text_surface->w/2;
					    text_pos.y = display_height/2 - Y_PADDING - (text_surface->h*(NB_BAT_TESTS) + Y_PADDING_BAT_RES*(NB_BAT_TESTS-1))/2 
					    	+ (text_surface->h+Y_PADDING_BAT_RES)*cur_bat_test_idx;
					    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
					    SDL_FreeSurface(text_surface);
    					
					    /* Force render screen */
    					render = true;

					    /* Set tests KO */
					    all_tests_ok = false;
		            }
		            else{
		            	bat_connected = true;
		            }
		            break;

		            /* Check USB present */
		            case USB_NOT_CONNECTED:
		            if(is_usb_present() == 1){

		            	/* usb bool*/
		            	usb_connected = true;

		            	/* Show ERROR message */
					    SDL_Color red={255,0,0};
					    text_surface = TTF_RenderText_Shaded(font_title, "UNPLUG USB", red, bg_color);
					    text_pos.x = display_width/2 - text_surface->w/2;
					    text_pos.y = display_height/2 - Y_PADDING  - (text_surface->h*(NB_BAT_TESTS) + Y_PADDING_BAT_RES*(NB_BAT_TESTS-1))/2 
					    	+ (text_surface->h+Y_PADDING_BAT_RES)*cur_bat_test_idx;
					    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
					    SDL_FreeSurface(text_surface);
    					
					    /* Force render screen */
    					render = true;

					    /* Set tests KO */
					    all_tests_ok = false;
		            }
		            else{
		            	usb_connected = false;
		            }
		            break;

		            /* Check battery voltage */
		            case BAT_VALUES_OK:
		            if(bat_connected && !usb_connected){

		            	/* Getting new bat measurement */
		            	bat_measurements[idx_bat_measurements] = get_battery_voltage();

		            	/* Check if voltage value not valid */
		            	if(bat_measurements[idx_bat_measurements] < MIN_VALID_VOLTAGE){
		            		false_measurement_found = true;
		            	}
		            	
		            	/* Update idx */
		            	idx_bat_measurements = (idx_bat_measurements+1)%NB_BAT_MESUREMENTS;
		            	nb_bat_measurements++;

		            	/* Check error */
		            	if(false_measurement_found){		            		

			            	/* Show ERROR message */
		            		if(false_measurement_found){
							    SDL_Color red={255,0,0};
							    text_surface = TTF_RenderText_Shaded(font_title, "WRONG VOLTAGE", red, bg_color);
							    text_pos.x = display_width/2 - text_surface->w/2;
							    text_pos.y = display_height/2 - Y_PADDING  - (text_surface->h*(NB_BAT_TESTS) + Y_PADDING_BAT_RES*(NB_BAT_TESTS-1))/2 
							    	+ (text_surface->h+Y_PADDING_BAT_RES)*cur_bat_test_idx;
							    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
							    SDL_FreeSurface(text_surface);

							    /* Set tests KO */
							    all_tests_ok = false;
		    					
							    /* Force render screen */
		    					render = true;
	    					}
		            	}
    					else if(nb_bat_measurements <NB_BAT_MESUREMENTS){

			            	/* Show measurement message */
						    SDL_Color dark_gray={40,40,40};
						    char text_tmp[40];
						    sprintf(text_tmp, "Check voltage (%d/%d)...", idx_bat_measurements, NB_BAT_MESUREMENTS);
						    text_surface = TTF_RenderText_Shaded(font_info, text_tmp, dark_gray, bg_color);
						    text_pos.x = display_width/2 - text_surface->w/2;
						    text_pos.y = display_height/2 - Y_PADDING  - (text_surface->h*(NB_BAT_TESTS) + Y_PADDING_BAT_RES*(NB_BAT_TESTS-1))/2 
						    	+ (text_surface->h+Y_PADDING_BAT_RES)*cur_bat_test_idx;
						    SDL_BlitSurface(text_surface, NULL, hw_surface, &text_pos);
						    SDL_FreeSurface(text_surface);
	    					
						    /* Force render screen */
	    					render = true;	

						    /* Set tests KO */
						    all_tests_ok = false;
    					}
		            }
		            break;

		            default:
		            printf("ERROR in %s, wrong value for cur_bat_test_idx: %d\n", 
		            	__func__, cur_bat_test_idx);
		            break;
				}
			}

            /* Update time*/
            prev_ms = SDL_GetTicks();
        }

        /* Flip screen */
        if(render){
	        SDL_Flip(hw_surface);
	    	render = false;
    	}

		/* Exit success if all tests ok */
		if(all_tests_ok){
			printf("All battery tests ok");
            stop_menu_loop = 1;
            res = 0;
            break;
		}

        /* FPS handling */
        SDL_Delay(SLEEP_PERIOD_MS);
    }

    return res;
}



/// -------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------
int launch_prod_screen_waitbattery(int argc, char *argv[]){


    /// 
    int res = wait_event_loop();
    return res;
}