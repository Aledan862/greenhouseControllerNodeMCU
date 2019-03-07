#include <Arduino.h>
#include <PubSubClient.h>
#include "GHcontrolClass.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "GHcontrolClass.h"

#define TOPIC_TEMP  "arduino/temp"
#define TOPIC_OUT "arduino/relay"
#define TOPIC_TEMP_SP "arduino/temp_sp"
#define PUBLISH_PERIOD 5  //период отправки данных в секундах
#define CLIENT_ID "arduino"

uint8_t Thermometers[2][8] = {
                                  {0x28, 0x85, 0xC7, 0x5B, 0x1E, 0x13, 0x01, 0x79},
                                  {0x28, 0x85, 0xC7, 0x5B, 0x1E, 0x13, 0x01, 0x79}}; //28 85 C7 5B 1E 13 1 79

AnalogChannel tempSetPoint;
Thermometer   water_thermometer;
Relay         led[3];
Relay         heater = Relay(0);
//DiscretRegul  ten1;


SysTime sysTime;    //systemtimeobject
AnalogChannel *analogObj = new AnalogChannel();
Thermometer   *hotWater = new Thermometer();
Relay         led[3];
//DiscretRegul  ten1;

//----------------------Setup Ethernet minmaxSetting----------------------------
// Update these with values suitable for your network.
const char* ssid = "WHITE HOUSE";
const char* password = "donaldtrumP";
const char* mqtt_server = "185.228.232.60";

WiFiClient espClient;
PubSubClient client(espClient);
//long lastMsg = 0;
//char msg[50];

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (uint i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    //digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    //digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
if (sysTime.sec % 5 == 0){
    Serial.print("Attempting MQTT connection...");
    // Set a client ID
    String clientId = CLIENT_ID;
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("Status", "connected");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
  }
}



//------------------------------------------------------------------------------


void setup() {
  // setup serial communication
  Serial.begin(57600);
  Serial.println("Стартуем");
  // setup ethernet communication using DHCP
  setup_wifi();
/*
  // setup mqtt client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  */
  //тикаем время
  sysTime.tick();
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
  //pinMode(16, OUTPUT);

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
}



void loop() {
sysTime.tick();
//обновляем значения
tempSetPoint.value();
water_thermometer.value();
heater.value();
//

discretRegul(water_thermometer.val, tempSetPoint.val, 1.0 , heater);

static int _sp;
if (!(_sp == int(tempSetPoint.val))){
  outToSerial();
  _sp = int(tempSetPoint.val);
}


  if (!client.connected()) {
   reconnect();
  }
  client.loop();
  // it's time to send new data?
  if (((sysTime.sec % PUBLISH_PERIOD) == 0) && sysTime.newSec ) {
    sendData(tempSetPoint);
    sendData(water_thermometer);
    sendData(heater);
    sysTime.newSec = 0;
    outToSerial();
  }
//delay(300);
}
