#include "display.h"

#include "wifi_webserver.h"
#include "save_restore_config.h"
#include "wellplate.h"

Adafruit_SSD1351 display = Adafruit_SSD1351(128, 128, &SPI, 5, 15);
status_screen screen;

static unsigned long int last_plate_A_time_remaining;
static int last_plate_A_time_remaining_digits;
static bool displayed_A_finished = false;

void init_display()
{
    display.begin();
    draw_home();
}

void draw_home()
{
    screen = home_screen;
    displayed_A_finished = false;
    display.fillScreen(BLACK);
    display.setCursor(29, 10);
    display.setTextColor(RED);
    display.setTextSize(2);
    display.print("LIGHTOS");
    display.fillRect(0, 30, 128, 5, YELLOW);

    if (config.get_acess_point())
    {
        display.setTextColor(WHITE);
        display.setCursor(5, 50);
        display.setTextSize(1);
        display.print("AP mode, SSID:");
    }
    else
    {
        display.setTextColor(RED);
        display.setCursor(5, 45);
        display.setTextSize(1);
        display.print("IP: ");
        display.setTextColor(WHITE);
        display.print(WiFi.localIP());
    }
    display.setCursor(5, 62);
    display.setTextColor(RED);
    display.print("1st plate: ");
    display.setTextColor(WHITE);
    display.print(config.get_last_config_filename());

    display.setCursor(5, 79);
    display.setTextColor(RED);
    display.print("2nd plate: ");
    display.setTextColor(WHITE);
    display.print("NA");

    draw_button("Start", 1, -2);
    draw_button("A", 2);
    draw_button("B", 3);
    draw_button("Setup", 4);
}

void draw_status_A_screen()
{
    screen = status_A_screen;
    display.fillScreen(BLACK);

    display.setTextColor(RED);
    display.setTextSize(1);
    print_centered_text("Plate A", 0);
    display.drawRect(0, 10, display.width(), 2, YELLOW);
    display.setCursor(0, 20);
    display.setTextColor(RED);
    display.print(F("Finished in: "));
}

void update_status_A_screen()
{
    if (!plate_A.prog_finished())
    {
        unsigned long int time_remaining = plate_A.get_time_remaining();
        if (time_remaining != last_plate_A_time_remaining)
        {
            int digits = num_digits(time_remaining);
            if (digits != last_plate_A_time_remaining_digits)
            {
                display.fillScreen(BLACK);
                draw_status_A_screen();
                last_plate_A_time_remaining_digits = digits;
            }
            display.setTextColor(WHITE, BLACK);
            display.setCursor(75, 20);
            display.print(time_remaining);
            display.print("s");
            last_plate_A_time_remaining = time_remaining;
        }
    }
    else
    {
        if (!displayed_A_finished)
        {
            display.fillScreen(BLACK);
            draw_status_A_screen();
            display.setTextColor(WHITE, BLACK);
            display.setCursor(75, 20);
            display.print("Finished");
            displayed_A_finished = true;
        }
    }
}

void draw_button(const char *button_name, uint8_t button_pos, int correction)
{
    int space_between = display.width() / 5;
    display.fillCircle((button_pos)*space_between, 120, 4, CYAN);
    int text_middle = (((strlen(button_name) - 1) * 5) / 2) + 1;
    display.setCursor((button_pos)*space_between - text_middle + correction, 100);
    display.setTextColor(CYAN);
    display.print(button_name);
}

void print_centered_text(const char *text, int y, int correction)
{
    int text_middle = (((strlen(text) - 1)) / 2);
    display.setCursor(text_middle + correction, y);
    display.print(text);
}

int num_digits(unsigned long int &number)
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