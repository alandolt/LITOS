#ifndef WIFI_WEBSERVER_H
#define WIFI_WEBSERVER_H
#include <Arduino.h>
#include <WiFi.h>

extern const char ssid[];
extern const char password[];

void init_webserver();
void init_wlan();
String processor(const String &var);

#endif