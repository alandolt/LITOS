/**
 * @file main.cpp
 * @author Alex Landolt 
 * @brief Main.cpp file of LITOS, here all the sub librarys and function will be invoked and included. Contains void setup and loop
 * @version 0.3
 * @date 2020-05-25
 */
#include <Arduino.h>
#include <SPIFFS.h>

#include "save_restore_config.h"
#include "wifi_webserver.h"
#include "wellplate.h"
#include "matrix.h"
#include "display.h"
#include "button.h"
#include "buzzer.h"
unsigned long int current_time;
const unsigned long refresh_intervall = 100;
unsigned long last_refresh_time = 0;

void setup()
{
	init_matrix_power();
	delay(100);
	Serial.begin(115200);
	SPIFFS.begin(); /// open file system

	config.load_configuration();
	Serial.println(config.get_con_mode());
	Serial.println(config.get_ssid());
	Serial.println(config.get_wlan_password());
	init_wlan();
	delay(100);
	init_webserver();
	delay(100);
	init_display();

	init_matrix();

	buzzer.init_buzzer();
	plate_A.wellplate_setup();
	if (config.get_last_wellplate('A') > 100)
	{
		plate_B.wellplate_setup();
	}
}
void loop()
{
	current_time = millis();

	switch (screen)
	{
	case home_screen:
		if (button_1.pressed()) /// both well plates A + B will be started
		{
			screen = status_A_B_screen;
			plate_A.begin(current_time);
			plate_B.begin(current_time);
			draw_status_screen();
		}
		if (button_2.pressed()) /// only A will be started
		{
			screen = status_A_screen;
			plate_A.begin(current_time);
			draw_status_screen();
		}
		if (button_3.pressed()) /// only B will be started
		{
			screen = status_B_screen;
			plate_B.begin(current_time);
			draw_status_screen();
		}
		break;
	case home_screen_one:
		if (button_2.pressed()) /// only A will be started
		{
			screen = status_A_screen;
			plate_A.begin(current_time);
			draw_status_screen();
		}
		break;
	case status_A_screen:		/// A is currently running
		if (button_4.pressed()) /// abort and go back to home
		{
			plate_A.abort_program();
			screen = home_screen;
			draw_home();
		}
		ref_backgroundLayer().swapBuffers();

		break;
	case status_B_screen:		/// B is currently running
		if (button_4.pressed()) /// abort and go back to home
		{
			plate_B.abort_program();
			screen = home_screen;
			draw_home();
		}
		ref_backgroundLayer().swapBuffers();
		break;
	case status_A_B_screen:		/// A and B are running simultanously
		if (button_4.pressed()) // abort and go back to home
		{
			plate_A.abort_program();
			plate_B.abort_program();
			screen = home_screen;
			draw_home();
		}
		ref_backgroundLayer().swapBuffers();
		break;
	case setup_screen:
		break;
	case error_screen:
		break;
	}

	if (plate_A.check(current_time) | plate_B.check(current_time)) /// power on / power off Matrix through MOSFET high/low pin
	{
		matrix_on();
	}
	else
	{
		matrix_off();
	}

	if (current_time - last_refresh_time >= refresh_intervall)
	{
		update_status_screen(); /// updates the status screens of the OLED display
		messages.update(current_time);
		buzzer.check_beep(current_time); // check if a beep has been requested by another part of the programm
		last_refresh_time += refresh_intervall;
	}
	//ref_websocket().cleanupClients(); /// can be used in production to clean up dead websocket connections of the ESP32 webserver (however, it still need to be tested if this effective or not)
}
