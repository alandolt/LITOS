#include <Arduino.h>
#include <SPIFFS.h>

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
	upper_plate.init_wellpalte();

	//init_wlan();
	//init_webserver();
	//init_matrix();

	//matrix_on();
	display.fill(RGB_COLOR16(255, 0, 0));
}

void loop()
{
	current_time = millis();

	/*
	if (upper_plate.check(current_time) | lower_plate.check(current_time))
	{ 
		matrix_on();
	}
	else
	{
		matrix_off();
	}*/
}