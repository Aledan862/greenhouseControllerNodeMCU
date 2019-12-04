#pragma once
#include "Arduino.h"
#include "options.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "PageBuilder.h"
#include "WebTemplates.h" 

extern ESP8266WebServer Server;

void wifiServerStart (const char* SSID, const char* PSK);

extern PageBuilder LEDPage;

//определяются в main'е
extern float temp;
extern int tempSetting[2];