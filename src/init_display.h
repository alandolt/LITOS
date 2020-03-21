#ifndef INIT_DISPLAY
#define INIT_DISPLAY

#include <lcdgfx.h>

DisplaySSD1351_128x128x16_SPI display(-1, {-1, 5, 15, 0, -1, -1}); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C
NanoEngine16<DisplaySSD1351_128x128x16_SPI> engine(display);

int display_width = 128;
int display_heigh = 128;
unsigned int long last_refreshed = 0;

#endif