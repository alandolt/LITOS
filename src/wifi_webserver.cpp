#include "wifi_webserver.h"

#include <SPIFFS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <esp_wpa2.h>
#include <ArduinoJson.h>

#include "struct.h"
#include "wellplate.h"
#include "display.h"
#include "save_restore_config.h"

static AsyncWebServer server(80);
static AsyncWebSocket ws("/litosws");
static DNSServer dnsServer;

extern DNSServer &ref_DNSServer()
{
	return dnsServer;
}

void init_webserver()
{
	//server.serveStatic("/", SPIFFS, "/w/").setDefaultFile("index.html");

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		// Send File
		AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/w/index.htm", "text/plain");
		request->send(response);
	});

	//server.serveStatic("/", SPIFFS, "/w/index.html");
	/*
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/web/upload.htm", String(), false, processor);
		response->addHeader("Server", "Async Web Server");
		request->send(response);
	});
	/*server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/web/upload.htm", "text/html");
    });

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
			// hier noch alles machen für A und B
			char config_file[35];
			int select_wellplate_int;
			strcpy(config_file, request->getParam("select_config", true)->value().c_str());
			select_wellplate_int = request->getParam("select_wellplate", true)->value().toInt();
			type_wellplate upper = type_wellplate(select_wellplate_int); // hier noch unterscheiden ob upper/ lower
			config.set_last_config_file(config_file, 'A');
			config.set_last_wellplate(upper, 'A', true);
			plate_A.wellplate_setup();
			request->redirect("/");
		}
	});*/

	ws.onEvent(onWsEvent);
	server.addHandler(&ws);
	server.begin();
}

void init_wlan()
{
	if (config.get_is_AP())
	{
		init_AP_mode();
	}
	else
	{
		uint8_t connection_count = 0;

		if (config.get_is_EAP())
		{
			esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)config.get_EAP_identity(), strlen(config.get_EAP_identity())); //provide identity
			esp_wifi_sta_wpa2_ent_set_username((uint8_t *)config.get_EAP_identity(), strlen(config.get_EAP_identity())); //provide username --> identity and username is same
			esp_wifi_sta_wpa2_ent_set_password((uint8_t *)config.get_EAP_password(), strlen(config.get_EAP_password())); //provide password
			esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();														 //set config settings to default
			esp_wifi_sta_wpa2_ent_enable(&config);																		 //set config settings to enable function
		}

		while (WiFi.status() != WL_CONNECTED)
		{
			WiFi.disconnect(true);
			WiFi.mode(WIFI_STA);

			if (config.get_is_EAP())
			{
				WiFi.begin("eduroam");
			}
			else
			{
				WiFi.begin(config.get_ssid(), config.get_wlan_password());
			}

			if (connection_count > 7)
			{
				init_AP_mode();
				return;
				// ESP.restart();
			}
			else
			{
				Serial.println("Connecting to WiFi..");
				connection_count++;
				delay(1000);
			}
		}
		config.set_ip(WiFi.localIP().toString().c_str());
		Serial.println(WiFi.localIP());
	}
}

void init_AP_mode()
{
	const byte DNS_PORT = 53;
	const IPAddress apIP(192, 168, 1, 1);
	//const IPAddress apIP(8, 8, 8, 8);

	WiFi.disconnect();
	WiFi.mode(WIFI_OFF);
	WiFi.mode(WIFI_AP);
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	if (config.get_AP_password_protected())
	{
		WiFi.softAP(config.get_AP_ssid(), config.get_AP_password());
	}
	else
	{
		WiFi.softAP(config.get_AP_ssid());
	}
	dnsServer.start(DNS_PORT, "*", apIP);
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

// Handle Websocket Requests
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
	if (type == WS_EVT_CONNECT)
	{
		Serial.println(F("[WEBSOCKET] Client connection received"));
		String result = "";
		generate_file_list_response(result);
		ws.text(client->id(), result);
	}
	else if (type == WS_EVT_DISCONNECT)
	{
		Serial.println(F("[WEBSOCKET] Client connection received"));
	}
	else if (type == WS_EVT_DATA)
	{
		AwsFrameInfo *info = (AwsFrameInfo *)arg;
		if (info->final && info->index == 0 && info->len == len)
		{
			String message = "";
			if (info->opcode == WS_TEXT)
			{
				for (size_t i = 0; i < info->len; i++)
					message += (char)data[i];
			}
			else
			{
				char buff[3];
				for (size_t i = 0; i < info->len; i++)
				{
					sprintf(buff, "%02x ", (uint8_t)data[i]);
					message += buff;
				}
			}

			Serial.println("[WEBSOCKET] Message Received: " + message);

			DynamicJsonDocument doc(1000);
			DeserializationError err = deserializeJson(doc, message);
			if (err)
			{
				Serial.println(F("deserializeJson() failed: "));
				Serial.println(err.c_str());
			}
			else
			{
				JsonObject object = doc.as<JsonObject>();
				String command = object["command"];
				if (command != "")
				{
					if (command == "get_file_list")
					{
						Serial.println("[WEBSOCKET] Got getLayout Command from Client " + String(client->id()));
						String result = "";
						generate_file_list_response(result);
						ws.text(client->id(), result);
					}

					else if (command == "load_config")
					{
						bool two_wellplates_json = object["two_wellplates"];
						JsonObject plate = object["plate_A"];
						bool custom_offset_json = plate["custom_offset"];
						const char *file_json = plate["file"];
						int wellplate_json = plate["wellplate"];
						if (custom_offset_json)
						{
							int min_col = plate["min_col"];
							int max_col = plate["max_col"];
							char min_row = plate["min_row"];
							char max_row = plate["min_row"];
							plate_A.wellplate_setup(file_json, wellplate_json, min_row, min_col, max_row, max_col);
						}
						else
						{
							plate_A.wellplate_setup(file_json, wellplate_json);
						}
						if (two_wellplates_json)
						{
							JsonObject plate = object["plate_B"];
							bool custom_offset_json = plate["custom_offset"];
							const char *file_json = plate["file"];
							int wellplate_json = plate["wellplate"];
							if (custom_offset_json)
							{
								int min_col = plate["min_col"];
								int max_col = plate["max_col"];
								char min_row = plate["min_row"];
								char max_row = plate["min_row"];
								plate_B.wellplate_setup(file_json, wellplate_json, min_row, min_col, max_row, max_col);
							}
							else
							{
								plate_B.wellplate_setup(file_json, wellplate_json);
							}
						}
					}
					else
					{
						Serial.println("[WEBSOCKET] Invalid Command");
					}
				}
			}
		}
	}
}
void generate_file_list_response(String &result)
{
	int file_count = config.get_file_count_spiffs();
	size_t CAPACITY = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(file_count) + file_count * JSON_OBJECT_SIZE(3);
	DynamicJsonDocument doc(CAPACITY + 1000);
	JsonObject root = doc.to<JsonObject>();
	root["response"] = "get_file_list";
	root["version"] = "1";
	root["size"] = CAPACITY + 1000;
	JsonArray file_list = root.createNestedArray("file_list");

	File root_folder = SPIFFS.open("/conf");
	File file = root_folder.openNextFile();
	char file_name[35];
	while (file)
	{
		strcpy(file_name, file.name());
		JsonObject jsonfile = file_list.createNestedObject();
		jsonfile["file"] = file_name;
		jsonfile["filename"] = &file_name[0] + 6;
		file = root_folder.openNextFile();
	}
	serializeJson(doc, result);
}
