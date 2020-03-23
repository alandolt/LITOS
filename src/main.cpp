#include <Arduino.h>

#include <SmartMatrix3.h>

#include <SPI.h>
#include <lcdgfx.h>

#include "init_webserver.h"
#include "struct.h"

//#include "wellplate.h"
//#include "init_matrix.h"

//#include "init_display.h"

#define DEBUG

bool matrix_in_use = false;
//AsyncWebServer server(80);


void setup()
{

	Serial.begin(115200);
	delay(100);

	init_wlan();
	init_webserver();

/*
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "ts World!");
    });

	    server.begin();

/*
	matrix.addLayer(&backgroundLayer);
	matrix.begin();
	matrix.setBrightness(defaultBrightness);
	backgroundLayer.fillScreen({0, 0, 0});
	backgroundLayer.swapBuffers();

	*/
	Serial.println("Started");
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