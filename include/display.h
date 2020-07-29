/**
 * @file display.h
 * @author Alex Landolt 
 * @brief Header files containing functions used to control the OLED screen of LITOS and class to create countdowns. 
 * @version 0.3
 * @date 2020-05-25
 */
#ifndef DISPLAY_H
#define DISPLAY_H

/// color definitions
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define GREY 0xAD75

#define NAVY 0x000F        /*   0,   0, 128 */
#define DARKGREEN 0x03E0   /*   0, 128,   0 */
#define DARKCYAN 0x03EF    /*   0, 128, 128 */
#define MAROON 0x7800      /* 128,   0,   0 */
#define PURPLE 0x780F      /* 128,   0, 128 */
#define OLIVE 0x7BE0       /* 128, 128,   0 */
#define LIGHTGREY 0xC618   /* 192, 192, 192 */
#define DARKGREY 0x7BEF    /* 128, 128, 128 */
#define ORANGE 0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5 /* 173, 255,  47 */
#define PINK 0xF81F

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include "struct.h"

/// enum to save to which location the screen is pointed
enum status_screen : char
{
    home_screen = 'H',
    home_screen_one = 'O',
    status_A_screen = 'A',
    status_B_screen = 'B',
    status_A_B_screen = 'T',
    setup_screen = 'S',
    test_screen = 'P',
    error_screen = 'E'
};

enum LITOS_error
{
    file_error,
    what_error,
    pattern_error
};

extern Adafruit_SSD1351 display; /// initialize display by loading Adafruit driver
extern status_screen screen;     /// initialize an instance of the enum status screen

void init_display(); /// initialize the display from void setup
void draw_home();
void draw_home_litos_ip();
void draw_error_screen(const char identifier, LITOS_error error);                  /// draw the home screen with IP adress and currently loaded configuration files
void draw_status_screen(bool with_buttons = true);                                 /// invoked when a program is started
void update_status_screen();                                                       /// in order to only refresh the countdown and not the whole screen (no screen flicker), update_status_screen is used in void loop
void draw_button(const char *button_name, uint8_t button_pos, int correction = 0); /// draws the colorful button in the lower part of the screen
void print_centered_text(const char *text, int y, int correction = 0);             /// helper function to display centered text
const char *wellplate_abrev(const type_wellplate &wellplate);                      /// convert wellplate types from struct.h to names used to display on the screen.

/**
 * @brief class created for countdown on status screen
 * 
 */
class show_countdown
{
private:
    unsigned long int last_plate_time_remaining;
    int last_plate_remaining_digits;
    bool displayed_finished;

public:
    show_countdown(); /// constructor
    void reset_countdown();
    void update_countdown(const bool &prog_finished, const unsigned long int &time_remaining, const int x = 75, const int y = 20); /// called in loop to update the time on the countdown
    int num_digits(const unsigned long int &number);                                                                               /// helper function, when number of digits change => screen refresh to avoid artifacts.
    bool display_finished();
};

#endif