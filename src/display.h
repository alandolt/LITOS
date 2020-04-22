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
//#define GREY 0x6B6D
#define GREY 0xAD75

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include "struct.h"

enum status_screen : char
{
    home_screen = 'H',
    status_A_screen = 'A',
    status_B_screen = 'B',
    status_A_B_screen = 'T',
    setup_screen = 'S'
};

extern Adafruit_SSD1351 display;
extern status_screen screen;

void init_display();
void draw_home();
void draw_status_screen(bool with_buttons = true);
void update_status_screen();
void draw_button(const char *button_name, uint8_t button_pos, int correction = 0);
void print_centered_text(const char *text, int y, int correction = 0);
const char *wellplate_abrev(const type_wellplate &wellplate);

class show_countdown
{
private:
    unsigned long int last_plate_time_remaining;
    int last_plate_remaining_digits;
    bool displayed_finished;

public:
    show_countdown();
    void reset_countdown();
    void update_countdown(const bool &prog_finished, const unsigned long int &time_remaining, const int x = 75, const int y = 20);
    int num_digits(const unsigned long int &number);
    bool display_finished();
};

#endif