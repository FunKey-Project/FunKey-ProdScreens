#ifndef __PROD_SCREEN_WAITBATTERY__
#define __PROD_SCREEN_WAITBATTERY__

#define BATTERY_PRESENT_FILE    "/sys/class/power_supply/axp20x-battery/present"
#define CHECK_BATTERY_DELAY_MS  500

int launch_prod_screen_waitbattery(int argc, char *argv[]);

#endif //__PROD_SCREEN_WAITBATTERY__