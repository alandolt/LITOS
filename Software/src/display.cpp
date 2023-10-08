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
message_list messages;
void init_display()
{
    display.begin();
    draw_home();
}

void draw_home()
{
    if (config.get_last_wellplate('A') > 100)
    {
        screen = home_screen;
    }
    else
    {
        screen = home_screen_one;
    }

    countdown_plate_A.reset_countdown();
    countdown_plate_B.reset_countdown();
    draw_home_litos_ip();

    if (config.get_last_wellplate('A') < 100)
    {
        display.setCursor(5, 50);
        display.setTextColor(RED);
        if (config.get_last_wellplate('A') < 50)
        {
            display.print("CTR: ");
        }
        else
        {
            display.print("COR: ");
        }

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
    }
    draw_button("Align", 4);
}

void draw_home_litos_ip()
{
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
}

void draw_test_screen()
{
    display.fillScreen(BLACK);
    display.setTextColor(RED);
    display.setTextSize(1);
    print_centered_text("Plate placement", 0);
    display.drawRect(0, 10, display.width(), 2, YELLOW);
    display.setTextColor(RED);
    display.setCursor(0, 22);
    display.print(F("Outl: "));
    display.setTextColor(WHITE);
    display.print(F("Marks outlines"));
    display.setCursor(35, 33);
    display.print(F("of plate"));

    display.setTextColor(YELLOW);
    display.setCursor(0, 53);
    display.print(F("Well: "));
    display.setTextColor(WHITE);
    display.print(F("Marks corner"));
    display.setCursor(35, 64);
    display.print(F("wells of plate"));
    draw_button("Outl", 1, -4);
    draw_button("Well", 2, 1);
    draw_button("Quit", 4);
}

void draw_error_screen(const char identifier, LITOS_error error)
{
    screen = error_screen;
    draw_home_litos_ip();
    display.setTextColor(RED);
    display.setCursor(5, 50);
    switch (error)
    {
    case file_error:
        display.print("Error:");
        display.setTextColor(WHITE);
        display.print(F("Loaded pattern"));
        display.setCursor(45, 60);
        display.print(F("does not"));
        display.setCursor(45, 70);
        display.print(F("exist in"));
        display.setCursor(45, 80);
        display.print(F("memory"));
        display.setCursor(0, 95);
        display.setTextColor(ORANGE);
        display.print(F("Please choose another"));
        display.setCursor(0, 105);
        display.print(F("pattern from WEB UI"));
        break;
    case what_error:
        display.print("Error: ");
        display.setTextColor(WHITE);
        display.print(F("Decoding of"));
        display.setCursor(45, 60);
        display.print(F("What field"));
        display.setCursor(45, 70);
        display.print(F("failed"));
        display.setCursor(0, 85);
        display.setTextColor(ORANGE);
        display.print(F("Please review pattern"));
        display.setCursor(0, 95);
        display.print(F("file and reload it"));
        break;
    case pattern_error:
        display.print("Error: ");
        display.setTextColor(WHITE);
        display.print(F("Wrong entry"));
        display.setCursor(45, 60);
        display.print(F("in the loaded"));
        display.setCursor(45, 70);
        display.print(F("pattern file"));
        display.setCursor(0, 85);
        display.setTextColor(ORANGE);
        display.print(F("Please review and"));
        display.setCursor(0, 95);
        display.print(F("reload it"));
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
        display.setTextColor(RED);
        display.setCursor(86, 20);
        display.print(":");
        display.setCursor(101, 20);
        display.print(":");

        display.setCursor(0, 30);
        display.print(F("B fin. in: "));
        display.setCursor(86, 30);
        display.print(":");
        display.setCursor(101, 30);
        display.print(":");

        // here Detail information on A and B (for future versions of LITOS)
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
        display.setTextColor(RED);
        display.setCursor(86, 20);
        display.print(":");
        display.setCursor(101, 20);
        display.print(":");
    }
    if (with_buttons)
    {
        if (QUIT_ON_RED)
        {
            draw_button("Quit", 1);
        }
        else
        {
            draw_button("Quit", 4);
        }
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
    int text_middle = (display.width() - (strlen(text) + 1) * 5) / 2;
    display.setCursor(text_middle + correction - 1, y);
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
    last_timepoint.days = 99999999;
    last_timepoint.hours = 99999999;
    last_timepoint.minutes = 99999999;
    last_timepoint.seconds = 99999999;
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
            char buffer[5];
            show_countdown::timepoint act_time(seconds_to_timepoint(time_remaining));
            display.setTextColor(WHITE, BLACK);
            if (act_time.hours != last_timepoint.hours)
            {
                sprintf(buffer, "%02d", act_time.hours);
                last_timepoint.hours = act_time.hours;
                display.setCursor(x, y);
                display.print(buffer);
            }
            if (act_time.minutes != last_timepoint.minutes)
            {
                sprintf(buffer, "%02d", act_time.minutes);
                last_timepoint.minutes = act_time.minutes;
                display.setCursor(x + 15, y);
                display.print(buffer);
            }
            if (act_time.seconds != last_timepoint.seconds)
            {
                sprintf(buffer, "%02d", act_time.seconds);
                last_timepoint.seconds = act_time.seconds;
                display.setCursor(x + 30, y);
                display.print(buffer);
            }
        }
    }
    else
    {
        if (!displayed_finished)
        {
            buzzer.invoke_beep(SHORT_BEEP);
            display.fillRect(x, y, 128, 7, BLACK);
            // draw_status_screen();
            display.setTextColor(WHITE, BLACK);
            display.setCursor(x, y);
            display.setTextColor(GREEN);
            display.print(F("Finished"));
            displayed_finished = true;
        }
    }
}

show_countdown::timepoint show_countdown::seconds_to_timepoint(unsigned long int seconds_remaining)
{
    show_countdown::timepoint act_timepoint;
    // act_timepoint.days = seconds_remaining / 86400;
    // seconds_remaining -= act_timepoint.days;
    act_timepoint.hours = seconds_remaining / 3600;
    seconds_remaining -= act_timepoint.hours * 3600;
    act_timepoint.minutes = seconds_remaining / 60;
    seconds_remaining -= act_timepoint.minutes * 60;
    act_timepoint.seconds = seconds_remaining;
    return act_timepoint;
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
    case one_6_center:
        return "6, center";
        break;
    case one_6_corner:
        return "6, corner";
        break;
    case two_6_A:
        return "6, upper";
        break;
    case two_6_B:
        return "6, lower";
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

message::message()
{
    status_message = message::not_in_use;
}

void message::init(char *_text_message, char identifier, unsigned int _duration, int _pos_x, int _pos_y)
{
    status_message = message::countdown;
    time_started = millis();
    position_x = _pos_x;
    position_y = _pos_y;
    text_message = _text_message;
    duration = _duration;
    last_timepoint = seconds_to_timepoint(duration);
    initial_draw();
}

void message::initial_draw()
{
    char text_buffer[50];
    strcpy(text_buffer, text_message);
    display.setCursor(position_x, position_y);
    display.setTextColor(RED);

    display.setTextColor(WHITE, BLACK);
    if (strlen(text_message) > 22)
    {
        text_buffer[22] = '\0';
        display.print(text_buffer);
        display.setCursor(position_x, position_y + 15);
        display.print(&text_message[0] + 22);
    }
    else
    {
        display.print(text_message);
    }

    display.setTextColor(RED);
    display.setCursor(display.width() - 30, position_y + 15);
    char buffer[5];
    sprintf(buffer, "%02d", last_timepoint.minutes);
    display.print(buffer);
    display.setTextColor(WHITE);
    display.print(F(":"));
    display.setTextColor(RED);
    sprintf(buffer, "%02d", last_timepoint.seconds);
    display.print(buffer);
}

void message::reset()
{
    display.fillRect(position_x, position_y, display.width(), 30, BLACK);
    last_timepoint.minutes = 99999999;
    last_timepoint.seconds = 99999999;
    status_message = message::not_in_use;
}

void message::update(const unsigned long time_ref)
{
    switch (status_message)
    {
    case not_in_use:
        break;
    case countdown:
    {
        message::timepoint current_timepoint = seconds_to_timepoint((time_started + duration * 1000 - time_ref) / 1000);
        display.setTextColor(RED, BLACK);
        char buffer[5];
        if (current_timepoint.minutes != last_timepoint.minutes)
        {
            display.setCursor(display.width() - 30, position_y + 15);
            sprintf(buffer, "%02d", current_timepoint.minutes);
            display.print(buffer);
            last_timepoint.minutes = current_timepoint.minutes;
        }
        if (current_timepoint.seconds != last_timepoint.seconds)
        {
            display.setCursor(display.width() - 13, position_y + 15);
            sprintf(buffer, "%02d", current_timepoint.seconds);
            display.print(buffer);
            last_timepoint.seconds = current_timepoint.seconds;
        }
        if (current_timepoint.seconds == 0 && current_timepoint.minutes == 0)
        {
            display.setCursor(display.width() - 30, position_y + 15);
            display.setTextColor(GREEN, BLACK);
            display.print(F("00:00"));
            buzzer.invoke_beep(SHORT_BEEP);
            time_finished = time_ref;
            status_message = finished;
        }
        break;
    }
    case finished:
        if (time_ref > time_finished + 10000)
        {
            reset();
        }
        break;
    }
}

message::timepoint message::seconds_to_timepoint(unsigned int seconds_remaining)
{
    message::timepoint act_timepoint;
    act_timepoint.minutes = seconds_remaining / 60;
    seconds_remaining -= act_timepoint.minutes * 60;
    act_timepoint.seconds = seconds_remaining;
    return act_timepoint;
}

bool message::is_free()
{
    if (status_message == message::not_in_use)
    {
        return true;
    }
    else
    {
        return false;
    }
}

message_list::message_list()
{
}

void message_list::update(const unsigned long current_time)
{
    for (int i = 0; i < max_messages; i++)
    {
        array_messages[i].update(current_time);
    }
}

void message_list::add_message(int message_index, char identifier, unsigned int _duration)
{

    for (int i = 0; i < max_messages; i++)
    {
        if (array_messages[i].is_free())
        {
            array_messages[i].init(message_storage[message_index], identifier, _duration, 0, 35 + 30 * i);
            break;
        }
    }
}

void message_list::reset_all()
{
    for (int i = 0; i < max_messages; i++)
    {
        array_messages[i].reset();
    }
    message_store_free = 0;
}

bool message_list::add_message_to_storage(char *_text_message)
{
    strcpy(message_storage[message_store_free], _text_message);
    message_store_free += 1;
    return message_store_free - 1;
}