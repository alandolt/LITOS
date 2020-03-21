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
#include "init_matrix.h"
#include "init_display.h"

#define DEBUG

bool matrix_in_use = false;

const char *ssid = "pilatus";
const char *password = "%Fortress123&";

AsyncWebServer server(80);
wellplate upper_plate;

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
	Serial.println("in handler");

	if (!index)
	{
		Serial.println((String) "UploadStart: " + filename);
		// open the file on first call and store the file handle in the request object
		request->_tempFile = SPIFFS.open("/" + filename, "w");
	}
	if (len)
	{
		// stream the incoming chunk to the opened file
		request->_tempFile.write(data, len);
	}
	if (final)
	{
		Serial.println((String) "UploadEnd: " + filename + ",size: " + index + len);
		// close the file handle as the upload is now done
		request->_tempFile.close();
		//request->redirect("/files");
	}
}

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

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		String html = "<body><div><form method='POST' action='/update' enctype='multipart/form-data' name='update'><input type='file' name='file'><input type='submit' value='Upload'> </form></div></body>";
		AsyncWebServerResponse *response = request->beginResponse(200, "text/html", html);
		response->addHeader("Server", "Async Web Server");
		request->send(response);
	});

	server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
		Serial.println("in server on handle download post");
		request->send(200); },
			  [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
				  Serial.println("in handler");

				  if (!index)
				  {
					  Serial.println((String) "UploadStart: " + filename);
					  // open the file on first call and store the file handle in the request object
					  request->_tempFile = SPIFFS.open("/" + filename, "w");
				  }
				  if (len)
				  {
					  // stream the incoming chunk to the opened file
					  request->_tempFile.write(data, len);
				  }
				  if (final)
				  {
					  Serial.println((String) "UploadEnd: " + filename + ",size: " + index + len);
					  // close the file handle as the upload is now done
					  request->_tempFile.close();
					  //request->redirect("/files");
				  }
			  });

	//server.onFileUpload(handleUpload);
	// Start server
	server.begin();

	matrix.addLayer(&backgroundLayer);
	matrix.begin();
	matrix.setBrightness(defaultBrightness);
	backgroundLayer.fillScreen({0, 0, 0});
	backgroundLayer.swapBuffers();

	Serial.print("test");
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