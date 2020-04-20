#ifndef DISPLAY_H
#define DISPLAY_H

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

enum status_screen : byte
{
    home_screen = 0,
    status_A_screen = 1,
    status_B_screen = 2,
    setup_screen = 3
};

extern Adafruit_SSD1351 display;
extern status_screen screen;

void init_display();
void draw_home();
void draw_status_A_screen();
void update_status_A_screen();
void draw_button(const char *button_name, byte button_pos, int correction = 0);
void print_centered_text(const char *text, int y, int correction = 0);
int num_digits(unsigned long int &number);

#endif