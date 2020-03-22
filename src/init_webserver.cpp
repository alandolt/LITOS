#include "init_webserver.h"

void init_webserver()
{
    AsyncWebServer server(80);

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

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

    server.begin();
}

void init_wlan()
{
    const char ssid[] = "pilatus";
    const char password[] = "%Fortress123&";
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP());
}