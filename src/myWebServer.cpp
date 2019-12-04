#include "myWebServer.h"

// This function is the logic for ONBOARD_LED on/off.
// It is called from the occurrence of the 'LEDIO' token by PageElement
//  'Button' declaration as following code.
String ledIO(PageArgument& args) {
  String ledImage = "";

  // Blinks ONBOARD_LED according to value of the http request parameter 'led'.
  if (args.hasArg("led")) {
    if (args.arg("led") == "on")
      digitalWrite(RELAYPIN, LOW);
    else if (args.arg("led") == "off")
      digitalWrite(RELAYPIN, HIGH);
  }
  delay(10);

  // Feedback the lighting icon depending on actual port value which
  //  indepent from the http request parameter.
  if (digitalRead(RELAYPIN) == LOW)
    ledImage = FPSTR(_PNG_LED);
  return ledImage;
}

String value(PageArgument& args){
  return String(temp);
}

String setpoint(PageArgument& args){
  int _sp = analogRead(A0);
  return String(map(_sp, 0, 1024, tempSetting[0], tempSetting[1]));
}

String cssVal(PageArgument& args){
  return String(map(temp, tempSetting[0], tempSetting[1], 0, 180));
}

String cssSP(PageArgument& args){
  int _sp = analogRead(A0);
  return String(map(_sp, 0, 1024, 0, 180));
}

// Page construction
PageElement Button(_PAGE_LED, {
  {"SP", cssSP },
  {"VAL", cssVal },
  {"LEDIO", ledIO },
  {"VALUE", value },
  {"SETPOINT", setpoint }
});

PageBuilder LEDPage("/", {Button});
ESP8266WebServer Server;

void wifiServerStart (const char* SSID, const char* PSK){

  WiFi.softAPdisconnect(true);
  delay(1000);
  Serial.print("Starting Wifi. SSID: ");
  Serial.println(SSID);
  Serial.print("PSK: ");
  Serial.println(PSK);
  bool st;
  st = WiFi.softAP(SSID,PSK);
  Serial.println(st);

  delay(1000);

  LEDPage.insert(Server);
  Server.begin();
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Web server http://");
  Serial.println(IP);

};
  