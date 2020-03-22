#ifndef INIT_WEBSERVER
#define INIT_WEBSERVER

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

extern const char ssid[];
extern const char password[];
extern AsyncWebServer server;

void init_webserver();
void init_wlan();

#endif