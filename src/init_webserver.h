#ifndef INIT_WEBSERVER
#define INIT_WEBSERVER
#include <Arduino.h>
extern const char ssid[];
extern const char password[];

void init_webserver();
void init_wlan();
String processor(const String &var);

#endif