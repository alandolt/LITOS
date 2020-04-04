#include <Arduino.h>
#include <SPIFFS.h>

#include "init_ota.h"
#include "save_restore_config.h"
#include "init_webserver.h"
#include "wellplate.h"
#include "init_matrix.h"
#include "init_display.h"
#include "button.h"

unsigned long int current_time;

void setup()
{
	delay(100);
	Serial.begin(115200);
	SPIFFS.begin();

	config.load_configuration();
	plate_A.init_wellpalte();

	init_wlan();
	init_webserver();
	init_matrix();
	init_display();

	plate_A.begin(millis());
}
void loop()
{
	//	ArduinoOTA.handle();
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
}
