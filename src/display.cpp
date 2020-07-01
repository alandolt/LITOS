/**
 * @file display.cpp
 * @author Alex Landolt 
 * @brief C file for display
 * @version 0.3
 * @date 2020-05-25
 */
#include "display.h"
#include "save_restore_config.h"
#include "wellplate.h"
#include "buzzer.h"

Adafruit_SSD1351 display = Adafruit_SSD1351(128, 128, &SPI, 5, 15); /// screen is connected over SPI (HSPI) with ESP32,
status_screen screen;

static show_countdown countdown_plate_A; /// initialize countdown for plate A and B
static show_countdown countdown_plate_B;

void init_display()
{
    display.begin();
    draw_home();
}

void draw_home()
{
    screen = home_screen;
    countdown_plate_A.reset_countdown();
    countdown_plate_B.reset_countdown();
    display.fillScreen(BLACK);
    display.setCursor(33, 0);
    display.setTextColor(RED);
    display.setTextSize(2);
    display.print("LITOS");
    display.fillRect(0, 20, 128, 4, YELLOW);

    if (config.get_is_AP())
    {
        display.setTextColor(RED);
        display.setCursor(5, 35);
        display.setTextSize(1);
        display.print("AP, SSID: ");
        display.setTextColor(WHITE);
        display.print(config.get_AP_ssid());
    }
    else
    {
        display.setTextColor(RED);
        display.setCursor(5, 35);
        display.setTextSize(1);
        display.print("IP: ");

        display.setTextColor(WHITE);
        display.print(config.get_ip());
    }
    if (config.get_last_wellplate('A') < 100)
    {
        display.setCursor(5, 50);
        display.setTextColor(RED);
        display.print("CTR: ");
        display.setTextColor(WHITE);
        display.setCursor(30, 50);

        display.print(config.get_last_config_filename('A'));
        display.setTextColor(GREY);
        display.setCursor(45, 62);
        display.print(wellplate_abrev(config.get_last_wellplate('A')));
        draw_button("A", 2);
    }
    else
    {
        display.setCursor(5, 50);
        display.setTextColor(RED);
        display.print("A: ");
        display.setTextColor(WHITE);
        display.setCursor(30, 50);

        display.print(config.get_last_config_filename('A'));
        display.setTextColor(GREY);
        display.setCursor(45, 62);
        display.print(wellplate_abrev(config.get_last_wellplate('A')));

        display.setCursor(5, 75);
        display.setTextColor(RED);
        display.print("B: ");
        display.setTextColor(WHITE);
        display.setCursor(30, 75);

        display.print(config.get_last_config_filename('B'));
        display.setTextColor(GREY);
        display.setCursor(45, 87);
        display.print(wellplate_abrev(config.get_last_wellplate('B')));
        draw_button("A+B", 1, -2); /// offset of -2 as it seems to be more centred this way
        draw_button("A", 2);
        draw_button("B", 3);
        //draw_button("Setup", 4);
    }
}

void draw_status_screen(bool with_buttons)
{
    display.fillScreen(BLACK);
    display.setTextColor(RED);
    display.setTextSize(1);
    if (screen == status_A_B_screen)
    {
        print_centered_text("Overview", 0);
        display.drawRect(0, 10, display.width(), 2, YELLOW);
        display.setTextColor(RED);
        display.setCursor(0, 20);
        display.print(F("A fin. in: "));
        display.setCursor(0, 30);
        display.print(F("B fin. in: "));

        //here Detail information on A and B (for future versions of LITOS)
    }
    else
    {
        char buffer[10];
        sprintf(buffer, "Plate %c", screen);
        print_centered_text(buffer, 0);
        display.drawRect(0, 10, display.width(), 2, YELLOW);
        display.setCursor(0, 20);
        display.setTextColor(RED);
        display.print(F("Finished in: "));
    }
    if (with_buttons)
    {
        draw_button("Quit", 4);
    }
}

void update_status_screen()
{
    switch (screen)
    {
    case status_A_screen:
        countdown_plate_A.update_countdown(plate_A.prog_finished(), plate_A.get_time_remaining());
        break;
    case status_B_screen:
        countdown_plate_B.update_countdown(plate_B.prog_finished(), plate_B.get_time_remaining());
        break;
    case status_A_B_screen:
        countdown_plate_A.update_countdown(plate_A.prog_finished(), plate_A.get_time_remaining());
        countdown_plate_B.update_countdown(plate_B.prog_finished(), plate_B.get_time_remaining(), 75, 30);
        break;
    default:
        break;
    }
}

void draw_button(const char *button_name, uint8_t button_pos, int correction)
{
    int button_color;
    switch (button_pos)
    {
    case 1:
        button_color = RED;
        break;
    case 2:
        button_color = YELLOW;
        break;
    case 3:
        button_color = BLUE;
        break;
    case 4:
        button_color = GREEN;
        break;
    default:
        button_color = CYAN;
        break;
    }
    int space_between = display.width() / 5;
    display.fillCircle((button_pos)*space_between, 120, 4, button_color);
    int text_middle = (((strlen(button_name) - 1) * 5) / 2) + 1;
    display.setCursor((button_pos)*space_between - text_middle + correction - 1, 103);
    display.setTextColor(CYAN);
    display.print(button_name);
}

void print_centered_text(const char *text, int y, int correction)
{
    int text_middle = (((strlen(text) - 1)) / 2);
    display.setCursor(text_middle + correction, y);
    display.print(text);
}

show_countdown::show_countdown()
{
    reset_countdown();
}

void show_countdown::reset_countdown()
{
    last_plate_time_remaining = 0;
    last_plate_remaining_digits = 0;
    displayed_finished = false;
}

int show_countdown::num_digits(const unsigned long int &number)
{
    if (number < 10)
    {
        return 1;
    }
    if (number < 1000)
    {
        if (number < 100)
        {
            return 2;
        }
        return 3;
    }
    return 4;
}

void show_countdown::update_countdown(const bool &prog_finished, const unsigned long int &time_remaining, const int x, const int y)
{
    if (!prog_finished)
    {
        if (time_remaining != last_plate_time_remaining)
        {
            int digits = num_digits(time_remaining);
            if (digits != last_plate_remaining_digits)
            {
                display.fillRect(x, y, 128, 7, BLACK);
                last_plate_remaining_digits = digits;
            }
            display.setTextColor(WHITE, BLACK);
            display.setCursor(x, y);
            display.print(time_remaining);
            display.print("s");
            last_plate_time_remaining = time_remaining;
        }
    }
    else
    {
        if (!displayed_finished)
        {
            buzzer.invoke_beep(SHORT_BEEP);
            display.fillRect(x, y, 128, 7, BLACK);
            //draw_status_screen();
            display.setTextColor(WHITE, BLACK);
            display.setCursor(x, y);
            display.print("Finished");
            displayed_finished = true;
        }
    }
}

const char *wellplate_abrev(const type_wellplate &wellplate)
{
    switch (wellplate)
    {
    case one_96_center:
        return "96, center";
        break;
    case one_96_corner:
        return "96, corner";
        break;
    case two_96_A:
        return "96, upper";
        break;
    case two_96_B:
        return "96, lower";
        break;
    case one_48_center:
        return "48, center";
        break;
    case two_48_A:
        return "48, upper";
        break;
    case two_48_B:
        return "48, lower";
        break;
    case one_24_center:
        return "24, center";
        break;
    case two_24_A:
        return "24, upper";
        break;
    case two_24_B:
        return "24, lower";
        break;
    case one_12_center:
        return "12, center";
        break;
    case two_12_A:
        return "12, upper";
        break;
    case two_12_B:
        return "12, lower";
        break;
    default:
        break;
    }

    return "";
}

bool show_countdown::display_finished()
{
    return displayed_finished;
}