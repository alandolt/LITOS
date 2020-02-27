#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
//#include <vector>
#include <SmartMatrix3.h>

#include "struct.h"
#include "wellplate.h"

int buttonState = 0;	  // variable for reading the pushbutton status
const int buttonPin = 36; // the number of the pushbutton pin
bool matrix_in_use = false;

#include <SmartMatrix3.h>
#define GPIOPINOUT ESP32_FORUM_PINOUT
#define COLOR_DEPTH 24										  // known working: 24, 48 - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24
const uint8_t kMatrixWidth = 64;							  // known working: 32, 64, 96, 128
const uint8_t kMatrixHeight = 32;							  // known working: 16, 32, 48, 64
const uint8_t kRefreshDepth = 24;							  // known working: 24, 36, 48
const uint8_t kDmaBufferRows = 4;							  // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
const uint8_t kPanelType = SMARTMATRIX_HUB75_32ROW_MOD16SCAN; // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels
const uint8_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE);	// see http://docs.pixelmatix.com/SmartMatrix for options
const uint8_t kBackgroundLayerOptions = (SM_BACKGROUND_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_BACKGROUND_LAYER(backgroundLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kBackgroundLayerOptions);

const int defaultBrightness = (10 * 255) / 100; // (10%) brightness

// here defined as can not access to backgroundlayer in class (extern not working, as type of backgroundLayer is runtime defined by SMARTMATRIX_ALLOCATE_BUFFERS)
void wellplate::well_col(int index)
{
	int size = 1;
	int x = well_to_x(led_array[index].row);
	int y = well_to_y(led_array[index].col);
	backgroundLayer.fillRectangle(x, y, x + size, y + size, rgb24{led_array[index].red, led_array[index].blue, led_array[index].green});
}

void wellplate::well_black(int index) // well shutoff light for well
{
	int size = 1;
	int x = well_to_x(led_array[index].row);
	int y = well_to_y(led_array[index].col);
	backgroundLayer.fillRectangle(x, y, x + size, y + size, rgb24{0, 0, 0});
}

// hier eventuell Vektor mit class well

const char *ssid = "pilatus";
const char *password = "%Fortress123&";

AsyncWebServer server(80);
wellplate upper_plate;

void setup()
{
	pinMode(buttonPin, INPUT);
	Serial.begin(115200);
	delay(100);
	Serial.println("Started");

	// Initialize SPIFFS
	if (!SPIFFS.begin(true))
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}
	// Connect to Wi-Fi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.println("Connecting to WiFi..");
	}

	// Print ESP32 Local IP Address
	Serial.println(WiFi.localIP());

	// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/index.html");
	});

	// Route to load style.css file
	server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/style.css", "text/css");
	});

	// Route to set GPIO to HIGH
	server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
		//switch_on();
		request->send(SPIFFS, "/index.html");
	});

	// Route to set GPIO to HIGH
	server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
		//switch_off();
		request->send(SPIFFS, "/index.html");
	});

	// Start server
	server.begin();

	// initialize the digital pin as an output.
	matrix.addLayer(&backgroundLayer);
	matrix.begin();
	matrix.setBrightness(defaultBrightness);

	backgroundLayer.fillRectangle(0, 0, matrix.getScreenWidth(), matrix.getScreenHeight(), {1, 1, 1});
	backgroundLayer.swapBuffers();
	backgroundLayer.fillScreen({0, 0, 0});

	int timepoint = 3;
	float exposure = 10;

	/*
	upper_plate.set_well(1, 2, exposure, 0, 7, 0, 255, 0);
	upper_plate.set_well(3, 2, exposure, 0, 7, 0, 255, 0);
	upper_plate.set_well(2, 4, exposure, 1, 7, 0, 255, 0);
	upper_plate.set_well(2, 5, exposure, 2, 7, 0, 255, 0);
	upper_plate.set_well(2, 6, exposure, 3, 7, 0, 255, 0);

	*/
	for (int i = 3; i <= 8; i++) // row
	{
		for (int j = 2; j <= 12; j++) // col
		{
			upper_plate.set_well(i, j, exposure, timepoint, 7, 0, 255, 0);
		}
		timepoint = timepoint + 1;
	}

	upper_plate.begin(millis());
	upper_plate.debug_print_led_array();

	delay(1000);
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
			Serial.print("button pressed");
			matrix_in_use = true;
			//upper_plate.begin(millis());
		}
	}
	*/
	upper_plate.check(millis());
	backgroundLayer.swapBuffers();
}