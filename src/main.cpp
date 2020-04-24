#include <Arduino.h>
#include <SPIFFS.h>

#include "init_ota.h"
#include "save_restore_config.h"
#include "wifi_webserver.h"
#include "wellplate.h"
#include "matrix.h"
#include "display.h"
#include "button.h"
#include "buzzer.h"

unsigned long int current_time;

void setup()
{
	delay(100);
	Serial.begin(115200);
	SPIFFS.begin();

	config.load_configuration();

	init_wlan();
	init_webserver();
	init_matrix();
	init_display();

	buzzer.init_buzzer();
	//plate_A.init_wellplate(); deprecated
	plate_A.wellplate_setup();
	//plate_B.init_wellplate();
	plate_B.wellplate_setup();
}
void loop()
{
	//ArduinoOTA.handle();
	ref_DNSServer().processNextRequest();
	current_time = millis();

	switch (screen)
	{
	case home_screen:
		if (button_1.pressed())
		{
			screen = status_A_B_screen;
			plate_A.begin(current_time);
			plate_B.begin(current_time);
			draw_status_screen();
		}
		if (button_2.pressed())
		{
			screen = status_A_screen;
			plate_A.begin(current_time);
			draw_status_screen();
		}
		if (button_3.pressed())
		{
			Serial.println("gumba");

			screen = status_B_screen;
			plate_B.begin(current_time);
			draw_status_screen();
		}
		break;
	case status_A_screen:
		if (button_4.pressed())
		{
			plate_A.abort_program();
			screen = home_screen;
			draw_home();
			// abort and go back to home
		}
		break;
	case status_B_screen:
		if (button_4.pressed()) // abort and go back to home
		{
			plate_B.abort_program();
			screen = home_screen;
			draw_home();
		}
		break;
	case status_A_B_screen:
		if (button_4.pressed()) // abort and go back to home
		{
			plate_A.abort_program();
			plate_B.abort_program();
			screen = home_screen;
			draw_home();
		}
		break;
	case setup_screen:
		break;
	}

	if (plate_A.check(current_time) | plate_B.check(current_time)) // power on / power off Matrix through MOSFET high/low pin
	{
		matrix_on();
	}
	else
	{
		matrix_off();
	}
	ref_backgroundLayer().swapBuffers();
	update_status_screen();
	buzzer.check_beep(current_time); // check if a beep has been requested by another part of the programm
}
