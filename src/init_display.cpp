#include "init_display.h"

DisplaySSD1351_128x128x16_SPI display(-1, {-1, 5, 15, 0, -1, -1});
NanoEngine16<DisplaySSD1351_128x128x16_SPI> engine(display);

int display_width = 128;
int display_heigh = 128;

engine.begin();
