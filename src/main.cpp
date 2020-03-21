#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include <SmartMatrix3.h>

#include <SPI.h>
#include <lcdgfx.h>

#include "struct.h"
#include "wellplate.h"

#define DEBUG

bool matrix_in_use = false;

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

DisplaySSD1351_128x128x16_SPI display(-1, {-1, 5, 15, 0, -1, -1}); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C
NanoEngine16<DisplaySSD1351_128x128x16_SPI> engine(display);

int display_width = 128;
int display_heigh = 128;
unsigned int long last_refreshed = 0;

// here defined as can not access to backgroundlayer in class (extern not working, as type of backgroundLayer is runtime defined by SMARTMATRIX_ALLOCATE_BUFFERS)

void wellplate::well_col(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
	backgroundLayer.fillRectangle(x, y, x + size_of_illumination, y + size_of_illumination, rgb24{r, g, b});
}

const char *ssid = "pilatus";
const char *password = "%Fortress123&";

AsyncWebServer server(80);
wellplate upper_plate;

void setup()
{

	Serial.begin(115200);
	delay(100);
	Serial.println("Started");

	// Initialize SPIFFS
	if (!SPIFFS.begin(true))
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}

	char file[] = "/demo.csv";
	wellplate main_plate;
	enum type_wellplate main_plate_type = center_96;
	main_plate.wellplate_setup(file, main_plate_type);
	char test1[15] = "A1";
	main_plate.what_switch(test1, 255, 0, 0);
	Serial.println("\n");

	char test2[] = "A2";
	main_plate.what_switch(test2, 255, 0, 0);
	Serial.println("\n");

	char test3[] = "A3";
	main_plate.what_switch(test3, 255, 0, 0);
	Serial.println("\n");

	char test4[] = "5";
	main_plate.what_switch(test4, 255, 0, 0);
	Serial.println("\n");

	char test5[] = "Px 20,42";
	main_plate.what_switch(test5, 255, 0, 0);
	Serial.println("\n");

	char test6[] = "Px_20_42";
	main_plate.what_switch(test6, 255, 0, 0);
	Serial.println("\n");

	/*

	
	// Connect to Wi-Fi

	WiFi.begin(ssid, password);

	//WiFi.begin(ssid, password);
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
	backgroundLayer.fillScreen({0, 0, 0});
	backgroundLayer.swapBuffers();

	delay(1000);
	*/
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