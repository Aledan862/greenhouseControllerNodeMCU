#pragma once
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "PageBuilder.h"
#include "WebLED.h"   // Only the LED lighting icon

// Web page structure is described as follows.
// It contains two tokens as {{STYLE}} and {{LEDIO}} also 'led'
//  parameter for GET method.
static const char PROGMEM _PAGE_LED[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8" name="viewport" content="width=device-width, initial-scale=1">
  <title>{{ARCH}} LED Control</title>
  <style type="text/css">
  {{STYLE}}
  </style>
</head>
<body>
  <p>{{ARCH}} LED Control</p>
  <div class="one">
  <p><a class="button" href="/?led=on">ON</a></p>
  <p><a class="button" href="/?led=off">OFF</a></p>
  </div>
  <div class="img">
  <img src="{{LEDIO}}"/>
  </div>
</body>
</html>
)rawliteral";

// A style description can be separated from the page structure.
// It expands from {{STYLE}} caused by declaration of 'Button' PageElement.
static const char PROGMEM _STYLE_BUTTON[] = R"rawliteral(
body {-webkit-appearance:none;}
p {
  font-family:'Arial',sans-serif;
  font-weight:bold;
  text-align:center;
}
.button {
  display:block;
  width:150px;
  margin:10px auto;
  padding:7px 13px;
  text-align:center;
  background:#668ad8;
  font-size:20px;
  color:#ffffff;
  white-space:nowrap;
  box-sizing:border-box;
  -webkit-box-sizing:border-box;
  -moz-box-sizing:border-box;
}
.button:active {
  font-weight:bold;
  vertical-align:top;
  padding:8px 13px 6px;
}
.one a {text-decoration:none;}
.img {text-align:center;}
)rawliteral";

// ONBOARD_LED is WiFi connection indicator.
// BUILTIN_LED is controled by the web page.
#define ONBOARD_LED 2     // Different pin assignment by each module

// Get an architecture of compiled
String getArch(PageArgument& args);

// This function is the logic for BUILTIN_LED on/off.
// It is called from the occurrence of the 'LEDIO' token by PageElement
//  'Button' declaration as following code.
String ledIO(PageArgument& args);

