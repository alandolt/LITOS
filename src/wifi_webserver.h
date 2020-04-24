#ifndef WIFI_WEBSERVER_H
#define WIFI_WEBSERVER_H
#include <Arduino.h>
#include <DNSServer.h>

void init_webserver();
void init_wlan();
void init_AP_mode();
void init_connect_mode();
extern DNSServer &ref_DNSServer();
String processor(const String &var);

#endif