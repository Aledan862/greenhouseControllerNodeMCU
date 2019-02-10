#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "GHcontrolClass.h"

#define CLIENT_ID "arduino"
#define TOPIC_TEMP  "arduino/temp"
#define TOPIC_OUT "arduino/relay"
#define TOPIC_TEMP_SP "arduino/temp_sp"
#define PUBLISH_PERIOD 30  //период отправки данных в секундах
#define RELAY_NUM 3 //подключено реле

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

void sendData() {
  char msgBuffer[20];
  char topic[32];
  if (sysTime.newSec) {
    client.publish(TOPIC_TEMP, dtostrf(hotWater->value(), 6, 2, msgBuffer));
    analogObj->descr.toCharArray(topic, 32);
    client.publish(topic, dtostrf(analogObj->value(), 6, 2, msgBuffer));

    sysTime.newSec = 0;
  }
}


//------------------------------------------------------------------------------

void setup() {
  // setup serial communication
  Serial.begin(57600);
  Serial.println("Стартуем");
  // setup ethernet communication using DHCP
  setup_wifi();
  // setup mqtt client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //тикаем время
  sysTime.tick();
//-----------сигналы ввода вывода--------------------------------------------
  //настройка потенциометра уставки температуры
  analogObj->setSetting(10, 50);
  analogObj->descr = "arduino/temp_sp";
  //термометр горячей воды
  DeviceAddress hotWaterAddr = { 0x28, 0x0B, 0x5F, 0x77, 0x91, 0x14, 0x02, 0xDA  };
  hotWater->init(1, hotWaterAddr);
}


void loop() {
  sysTime.tick();
  // Serial.print("значение analogObj ");
  // Serial.println(analogObj->value());
  // Serial.print("значение hotWater ");
  // Serial.println(hotWater->value());
  // //Serial.print("значение led ");
  // Serial.println(led[1].value());

  for (uint8 i=0; i<RELAY_NUM; i++){
    discretRegul(hotWater->value(), analogObj->value(), 2.0 , led[i]);
  }

  if (!client.connected()) {
     reconnect();
    }
    client.loop();
    // it's time to send new data?
    if ((sysTime.sec % PUBLISH_PERIOD) == 0) {
      sendData();
    }
}
