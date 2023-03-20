#include "wifi_webserver.h"

#include <SPIFFS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <esp_wpa2.h>
#include <DNSServer.h>
#include <ArduinoJson.h>

#include "struct.h"
#include "wellplate.h"
#include "display.h"
#include "save_restore_config.h"

static AsyncWebServer server(80);
static AsyncWebSocket ws("/litosws");
static DNSServer dnsServer;
// static DNSServer dnsServer;
/*
extern DNSServer &ref_DNSServer()
{
	return dnsServer;
}*/

/*extern AsyncWebSocket &ref_websocket()
{
	return ws;
}*/
bool is_AP_mode = false;

void init_webserver()
{
	server.reset();
	// server.serveStatic("/", SPIFFS, "/w/").setDefaultFile("index.html");

	server.onNotFound([](AsyncWebServerRequest *request)
					  {
		AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", webpage, webpage_size);
		response->addHeader("Content-Encoding", "gzip");
		request->send(response); });

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
		// Send File
		AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", webpage, webpage_size);
		response->addHeader("Content-Encoding", "gzip");
		request->send(response); });
	server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
		AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", favicon, favicon_size);
		response->addHeader("Content-Encoding", "gzip");
		request->send(response); });
	server.on("/fonts/element-icons.woff", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
		AsyncWebServerResponse *response = request->beginResponse_P(200, "/font/woff", element_icons, element_icons_size);
		response->addHeader("Content-Encoding", "gzip");
		request->send(response); });

	// server.serveStatic("/favicon.ico", SPIFFS, "/w/favicon.ico");
	// server.serveStatic("/fonts/", SPIFFS, "/w/fonts/");
	server.serveStatic("/conf/", SPIFFS, "/conf/");

	server.on(
		"/upload", HTTP_POST, [](AsyncWebServerRequest *request)
		{ request->send(200); },
		[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
		{
			Serial.println("upload call received");
			if (!index)
			{
				Serial.println((String) "UploadStart: " + filename);
				// open the file on first call and store the file handle in the request object
				if (filename.length() > 22)
				{
					filename = filename.substring(0, 20);
					filename += ".csv";
				}
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
				if (strstr(config.get_file_list(), filename.c_str()) == NULL)
				{
					snprintf(config.get_file_list(), 35, ",/conf/%s", filename.c_str());
				}
				request->_tempFile.close();
			}
		});

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
			// esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)config.get_EAP_identity(), strlen(config.get_EAP_identity())); // provide identity
			// esp_wifi_sta_wpa2_ent_set_username((uint8_t *)config.get_EAP_identity(), strlen(config.get_EAP_identity())); // provide username --> identity and username is same
			// esp_wifi_sta_wpa2_ent_set_password((uint8_t *)config.get_EAP_password(), strlen(config.get_EAP_password())); // provide password
			// esp_wifi_sta_wpa2_ent_enable();																				 // set config settings to enable function
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

			if (connection_count > 10)
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
	// const byte DNS_PORT = 53;
	const IPAddress apIP(192, 168, 1, 1);
	// const IPAddress apIP(8, 8, 8, 8);
	Serial.println(config.get_AP_ssid());
	WiFi.disconnect();
	WiFi.mode(WIFI_OFF);
	WiFi.mode(WIFI_AP);
	if (config.get_AP_password_protected())
	{
		WiFi.softAP(config.get_AP_ssid(), config.get_AP_password());
	}
	else
	{
		WiFi.softAP(config.get_AP_ssid());
	}
	delay(100);
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	config.set_con_mode(AP_mode);
	dnsServer.start(53, "*", apIP);
	is_AP_mode = true;
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

			// Serial.println("[WEBSOCKET] Message Received: " + message);

			DynamicJsonDocument doc(800);
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
					if (command == "ping")
					{
						ws.text(client->id(), F("{\"response\": \"ping_ack\"}"));
					}
					else if (command == "get_file_list")
					{
						Serial.println("[WEBSOCKET] Got get_file_list Command from Client " + String(client->id()));
						String result = "";
						generate_file_list_response(result);
						ws.text(client->id(), result);
					}
					else if (command == "get_settings")
					{
						String result = "";
						config.get_settings_web(result);
						Serial.println("get_settings_code");
						ws.text(client->id(), result);
					}

					else if (command == "update_settings")
					{
						Serial.println("get new settings");
						config.set_port(doc["webserver"]["port"] | config.get_port());
						config.set_hostname(doc["webserver"]["hostname"] | config.get_hostname());
						config.set_con_mode(doc["webserver"]["mode"] | config.get_con_mode());
						config.set_adv_set(doc["webserver"]["adv_set"] | config.get_adv_set());

						config.set_ssid(doc["WPA_mode"]["ssid"] | config.get_ssid());
						config.set_wlan_password(doc["WPA_mode"]["wpa_password"] | config.get_wlan_password());

						config.set_AP_ssid(doc["AP_mode"]["AP_ssid"] | config.get_AP_ssid());
						config.set_AP_password(doc["AP_mode"]["AP_password"] | config.get_AP_password());
						config.set_AP_password_protected(doc["AP_mode"]["AP_prot"] | config.get_AP_password_protected());

						config.set_EAP_identity(doc["EAP_mode"]["EAP_identity"] | config.get_EAP_identity());
						config.set_EAP_password(doc["EAP_mode"]["EAP_password"] | config.get_EAP_password());

						config.set_global_correction(doc["mat_cor"]["act"] | config.get_global_correction_activated());
						config.set_global_correction_x(doc["mat_cor"]["x"] | config.get_global_correction_x());
						config.set_global_correction_y(doc["mat_cor"]["y"] | config.get_global_correction_y());

						config.set_mark_col(doc["mark_col"] | config.get_mark_col_web());
						config.save_configuration();

						Serial.println(config.get_port());
						Serial.println(config.get_hostname());
						Serial.println(config.get_con_mode());
						Serial.println(config.get_adv_set());
						Serial.println(config.get_ssid());
						Serial.println(config.get_wlan_password());
						Serial.println(config.get_AP_ssid());
						Serial.println(config.get_AP_password());
						Serial.println(config.get_AP_password_protected());
						Serial.println(config.get_EAP_identity());
						Serial.println(config.get_EAP_password());
						Serial.println(config.get_global_correction_activated());
						Serial.println(config.get_global_correction_x());
						Serial.println(config.get_global_correction_y());
						Serial.println(config.get_mark_col_web());

						ESP.restart();
					}

					else if (command == "delete_file")
					{
						Serial.println("get_file_delete command");
						const char *file_to_be_removed = object["file"];
						Serial.println(file_to_be_removed);

						SPIFFS.remove(file_to_be_removed);
						char file_list_buffer[550];
						char *ptr_file_list;

						ptr_file_list = strtok(config.get_file_list(), ",");
						while (ptr_file_list != NULL)
						{

							if (strcmp(ptr_file_list, file_to_be_removed) != 0)
							{
								strcat(file_list_buffer, ptr_file_list);
								strcat(file_list_buffer, ",");
							}
							ptr_file_list = strtok(NULL, ",");
						}
						file_list_buffer[strlen(file_list_buffer) - 1] = '\0';
						config.set_file_list(file_list_buffer);
						Serial.println(config.get_file_list());
					}

					else if (command == "load_config")
					{
						bool two_wellplates_json = object["two_wellplates"];
						JsonObject plate = object["plate_A"];
						bool custom_offset_json = plate["custom_offset"];
						const char *file_json = plate["file"];
						int wellplate_json = plate["wellplate"];
						config.set_last_config_file(file_json, 'A');
						config.set_last_wellplate(wellplate_json, 'A');
						config.set_two_wellplates(false);
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
							config.set_two_wellplates(true);
							config.set_last_config_file(file_json, 'B');
							config.set_last_wellplate(wellplate_json, 'B');
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
						config.save_configuration();
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
	// int file_count = config.get_file_count_spiffs();
	// size_t CAPACITY = JSON_OBJECT_SIZE(4) + JSON_ARRAY_SIZE(file_count) + file_count * JSON_OBJECT_SIZE(3);
	// DynamicJsonDocument doc(CAPACITY + 100);
	DynamicJsonDocument root(1000);
	// StaticJsonDocument<1000> root;
	// JsonObject root = doc.to<JsonObject>();
	root["response"] = "get_file_list";
	JsonArray file_list = root.createNestedArray("files");
	char copy_of_file_list[550];
	strcpy(copy_of_file_list, config.get_file_list());
	char *ptr_file_list;
	ptr_file_list = strtok(copy_of_file_list, ",");
	while (ptr_file_list != NULL)
	{
		file_list.add(ptr_file_list);
		ptr_file_list = strtok(NULL, ",");
	}

	serializeJson(root, result);
	Serial.println(result);
}

void dns_server_next_request()
{
	if (is_AP_mode)
	{
		dnsServer.processNextRequest();
	}
}