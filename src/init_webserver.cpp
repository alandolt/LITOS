#include "init_webserver.h"

void init_webserver()
{
  // AsyncWebServer server(80);
server= new AsyncWebServer(80);

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    /*
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("anfrage browser");
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/upload.htm", "text/plain");

        //, String(), false, processor)
        response->addHeader("Server", "Async Web Server");
        request->send(response);
    });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/upload.htm", "text/html");
    });

   */
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Hello World!");
    });
    /*
    server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) { request->send(200); },
              [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
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
                      request->redirect("/upload.htm");
                  }
              });

    server.on("/select_programm", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("select_config", true) && request->hasParam("select_wellplate", true))
        {
            char config_file[35];
            int select_wellplate_int;
            strcpy(config_file, request->getParam("select_config", true)->value().c_str());
            select_wellplate_int = request->getParam("select_config", true)->value().toInt();
            type_wellplate upper = type_wellplate(select_wellplate_int);

            upper_plate.wellplate_setup(config_file, upper);
        }
    });
*/
    server->begin();
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

String processor(const String &var)
{
    if (var == "LIST_FILES")
    {
        File root = SPIFFS.open("/");
        File datei = root.openNextFile();
        char files[1500];

        char one_file[150];
        while (datei)
        {
            strcat(one_file, "<option value='");
            strcat(one_file, datei.name());
            strcat(one_file, "'>");
            strcat(one_file, datei.name());
            strcat(one_file, "</option>");

            strcat(files, one_file);
            datei = root.openNextFile();
        }
        return (F(files));
    }

    return String();
}
