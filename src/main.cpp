#include <Arduino.h>


#include <SmartMatrix3.h>

#include <SPI.h>
#include <lcdgfx.h>

#include "init_webserver.h"
#include "struct.h"
#include "wellplate.h"
#include "init_matrix.h"
#include "init_display.h"

#define DEBUG

bool matrix_in_use = false;

wellplate upper_plate;

void setup()
{

	Serial.begin(115200);
	delay(100);
	Serial.println("Started");

	// Initialize SPIFFS


	init_wlan();
	init_webserver();

	char file[] = "/demo.csv";
	wellplate main_plate;
	enum type_wellplate main_plate_type = center_96;
	main_plate.wellplate_setup(file, main_plate_type);

	matrix.addLayer(&backgroundLayer);
	matrix.begin();
	matrix.setBrightness(defaultBrightness);
	backgroundLayer.fillScreen({0, 0, 0});
	backgroundLayer.swapBuffers();

	File root = SPIFFS.open("/");

	File datei = root.openNextFile();

	while (datei)
	{

		Serial.print("FILE: ");
		Serial.println(datei.name());

		datei = root.openNextFile();
	}
}

void loop()
{
	/*

	if (millis() > 300)
	{
		if (not matrix_in_use)
		{
			buttonState = digitalRead(buttonPin);
			delay(200);
		}
		if (buttonState == LOW and not matrix_in_use)
		{
		}

		if (matrix_in_use)
		{
			upper_plate.check(millis());
			backgroundLayer.swapBuffers();
		}
	}

	*/
}