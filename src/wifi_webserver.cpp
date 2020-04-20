#include "wifi_webserver.h"

#include <SPIFFS.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "struct.h"
#include "wellplate.h"
#include "display.h"
#include "save_restore_config.h"
static AsyncWebServer server(80);

void init_webserver()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/web/upload.htm", String(), false, processor);
        response->addHeader("Server", "Async Web Server");
        request->send(response);
    });
    /*server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/web/upload.htm", "text/html");
    });*/

    server.on(
        "/upload", HTTP_POST, [](AsyncWebServerRequest *request) { request->send(200); },
        [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
            if (!index)
            {
                Serial.println((String) "UploadStart: " + filename);
                // open the file on first call and store the file handle in the request object
                request->_tempFile = SPIFFS.open("/conf/" + filename, "w");
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
                request->redirect("/");
            }
        });

    server.on("/select_programm", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("select_config", true) && request->hasParam("select_wellplate", true))
        {
            Serial.println("test");
            char config_file[35];
            int select_wellplate_int;
            strcpy(config_file, request->getParam("select_config", true)->value().c_str());
            select_wellplate_int = request->getParam("select_wellplate", true)->value().toInt();
            type_wellplate upper = type_wellplate(select_wellplate_int); // hier noch unterscheiden ob upper/ lower
            config.set_last_config_file(config_file, false);
            config.set_last_wellplate(upper, true);
            plate_A.wellplate_setup();
            draw_home();
            request->redirect("/");
        }
    });

    server.begin();
}

void init_wlan()
{
    const char ssid[] = "pilatus";
    const char password[] = "%Fortress123&";
    WiFi.begin(ssid, password);
    byte connection_count = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        if (connection_count > 5)
        {
            ESP.restart();
        }
        else
        {
            delay(1000);
            Serial.println("Connecting to WiFi..");
            connection_count++;
        }
    }

    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP());
}

String processor(const String &var)
{
    if (var == "LIST_FILES")
    {
        File root = SPIFFS.open("/conf");
        File file = root.openNextFile();
        char string_files[2000];
        char file_name[35];
        char file_buffer[150];
        string_files[0] = '\0';
        while (file)
        {
            strcpy(file_name, file.name());
            sprintf(file_buffer, "<option value=\'%s\'>%s</option>", file_name, &file_name[0] + 6);
            strcat(string_files, file_buffer);
            file = root.openNextFile();
        }
        return (string_files);
    }

    return String();
}
