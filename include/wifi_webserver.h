#ifndef WIFI_WEBSERVER_H
#define WIFI_WEBSERVER_H
#include <Arduino.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>

#include "web/webpage.h"
#include "web/favicon.h"
#include "web/element_icons.h"

void init_webserver();
void init_wlan();
void init_AP_mode();
void init_connect_mode();
//extern DNSServer &ref_DNSServer();
String processor(const String &var);
//extern AsyncWebSocket &ref_websocket();

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void generate_file_list_response(String &result);
#endif