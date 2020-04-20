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
	plate_A.init_wellpalte();
}
void loop()
{
	//ArduinoOTA.handle();
	current_time = millis();

	switch (screen)
	{
	case home_screen:
		if (button_2.pressed())
		{
			plate_A.begin(current_time);
			draw_status_A_screen();
		}
		break;
	case status_A_screen:
		update_status_A_screen();
		break;
	}

	if (plate_A.check(current_time) | plate_B.check(current_time))
	{
		matrix_on();
	}
	else
	{
		matrix_off();
	}

	buzzer.check_beep(current_time);
}
