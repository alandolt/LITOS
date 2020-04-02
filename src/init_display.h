#ifndef INIT_DISPLAY
#define INIT_DISPLAY

#include <lcdgfx.h>

extern DisplaySSD1351_128x128x16_SPI display;
extern NanoEngine16<DisplaySSD1351_128x128x16_SPI> engine;

extern int display_width;
extern int display_heigh;

#endif