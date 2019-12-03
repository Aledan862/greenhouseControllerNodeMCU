#include <Arduino.h>
//#include <ESP8266WiFi.h>
//#include <PubSubClient.h>
#include "webserver.h"
#define TOPIC_TEMP  "arduino/temp"
#define TOPIC_OUT "arduino/relay"
#define TOPIC_TEMP_SP "arduino/temp_sp"
#define PUBLISH_PERIOD 5  //период отправки данных в секундах


#include "GHcontrolClass.h"

extern SysTime sysTime; 

uint8_t Thermometers[2][8] = {
                                  {0x28, 0x85, 0xC7, 0x5B, 0x1E, 0x13, 0x01, 0x79},
                                  {0x28, 0x85, 0xC7, 0x5B, 0x1E, 0x13, 0x01, 0x79}}; //28 85 C7 5B 1E 13 1 79

AnalogChannel tempSetPoint;
Thermometer   water_thermometer;
Relay         led[3];
Relay         heater = Relay(0);
DigitalChannel extPower = DigitalChannel(1);
//DiscretRegul  ten1;
#ifdef MQTT_ENABLE
//----------------------Setup Ethernet minmaxSetting----------------------------
// Update these with values suitable for your network.
char* ssid = "mi";
char* password = "123456781";
char* mqtt_server = "185.228.232.60";

//WiFiClient espClient;
//PubSubClient client(espClient);
//long lastMsg = 0;
//char msg[50];
Mqtt mqtt;

//------------------------------------------------------------------------------
#endif
// Page construction
PageElement Button(_PAGE_LED, {
  {"STYLE", [](PageArgument& arg) { return String(_STYLE_BUTTON); }},
  {"ARCH", getArch },
  {"LEDIO", ledIO }
});
PageBuilder LEDPage("/", {Button});

ESP8266WebServer Server;


const char* SSID = "GreenHouse";  // Modify for your available WiFi AP
const char* PSK  = "12345678";  // Modify for your available WiFi AP


void setup() {
  // setup serial communication
  Serial.begin(57600);
  Serial.println("Стартуем");
  // setup ethernet communication using DHCP
  //setup_wifi();
  //mqtt.setup(ssid, password, mqtt_server);
  //тикаем время
  //sysTime.tick();

  WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(SSID, PSK);
  do {
    delay(500);
    Serial.print("#");
  } while (WiFi.waitForConnectResult() != WL_CONNECTED);
  Serial.println("Connected to " + String(SSID));

  LEDPage.insert(Server);
  Server.begin();

  Serial.print("Web server http://");
  Serial.println(WiFi.localIP());


  //-----------сигналы ввода вывода--------------------------------------------
  //настройка потенциометра уставки температуры
  tempSetPoint.setSetting(10, 50);
  tempSetPoint.descr = String("temp_sp");
  //термометр горячей воды
  DeviceAddress hotWaterAddr = { 0x28, 0x0B, 0x5F, 0x77, 0x91, 0x14, 0x02, 0xDA  };
  water_thermometer.init(1, hotWaterAddr, "Теплоноситель");
  //симулятор реле
  heater = Relay(0);
  heater.descr = String("Heater");
  //сигнал на внешнее питание
  extPower.descr = String("Power");
  //дискретный регулятор
  //ten1.init(hotWater->value(), analogObj->value(), 2, led1);

}

void outToSerial (){
  Serial.print(F("значение уставки "));
  Serial.println(tempSetPoint.val);
  Serial.print(F("значение термометра "));
  Serial.println(water_thermometer.val);
  Serial.print(F("значение heater "));
  Serial.println(heater.val);
  Serial.print(F("значение питания "));
  Serial.println(extPower.val);
  Serial.println(digitalRead(5));

}



void loop() {
sysTime.tick();
//обновляем значения
tempSetPoint.value();
water_thermometer.value();
heater.value();
extPower.value();
//

discretRegul(water_thermometer.val, tempSetPoint.val, 1.0 , heater);

static int _sp;
if (!(_sp == int(tempSetPoint.val))){
  outToSerial();
  _sp = int(tempSetPoint.val);
}

#ifdef MQTT_ENABLE
  if (!mqtt.client.connected()) {
   mqtt.reconnect();
  }
  mqtt.client.loop();
  // it's time to send new data?
  if (((sysTime.sec % PUBLISH_PERIOD) == 0) && sysTime.newSec ) {
    mqtt.sendTime();
    mqtt.sendData(tempSetPoint);
    mqtt.sendData(water_thermometer);
    mqtt.sendData(heater);
    mqtt.sendData(extPower);
    sysTime.newSec = 0;
    outToSerial();
  }
//delay(300);
#endif
Server.handleClient();

}
