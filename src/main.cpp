#include <Arduino.h>
//#include <UIPEthernet.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include <DallasTemperature.h>
#include "GHcontrolClass.h"
#include <timeClass.cpp>

#define CLIENT_ID "arduino"
#define TOPIC_TEMP  "arduino/temp"
#define TOPIC_OUT "arduino/relay"
#define TOPIC_TEMP_SP "arduino/temp_sp"
#define PUBLISH_PERIOD 30  //период отправки данных в секундах
#define ONE_WIRE_BUS 1 //номер цифрового канала для шины 1-wire
#define SP_TEMP_PIN A0 //номер пина для потенциометра регулирования уставки температуры
#define THERM_NUM 1  //количество термометров
#define AI_NUM 1     // количество аналогов
#define DO_START 3  //с какого пина начинается отсчет
#define DO_NUM 1     // дискретные выходы
#define WIFISETTING 1
//systemtimeobject
SysTime sysTime;

//AnalogChannel analog0;
// Setup a oneWire instance to communicate with any OneWire devices
//OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
//DallasTemperature sensors(&oneWire);

uint8_t Thermometers[2][8] = {
                                  {0x28, 0x85, 0xC7, 0x5B, 0x1E, 0x13, 0x01, 0x79},
                                  {0x28, 0x85, 0xC7, 0x5B, 0x1E, 0x13, 0x01, 0x79}}; //28 85 C7 5B 1E 13 1 79
/*// MAC-адрес нашего устройства
byte mac[] = { 0x00, 0x2A, 0xF6, 0x12, 0x68, 0xFC };
// ip-адрес устройства
byte ip[] = { 192, 168, 0, 52 };
IPAddress mqttServer(185,228,232,60);
EthernetClient ethClient;
PubSubClient mqttClient;
long previousMillis;



void callback(char* topic, byte* payload, unsigned int length) {
  String receivedString;
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));

  for (unsigned int i = 0; i < length; i++) {
    receivedString += (char)payload[i];
    }
   if (strcmp(topic,"inTopic") == 0) {
    (receivedString == "ON") ? digitalWrite(4, HIGH) : digitalWrite(4, LOW);
    (receivedString == "ON") ? Serial.println("on") : Serial.println("off");
    }
}

void sendData() {
  char msgBuffer[20];
  if (mqttClient.connect(CLIENT_ID)) {
    mqttClient.publish(TOPIC_TEMP, dtostrf(t, 6, 2, msgBuffer));
    mqttClient.publish(TOPIC_TEMP_SP, dtostrf(sp, 6, 2, msgBuffer));
    mqttClient.publish(TOPIC_OUT, (digitalRead(RELAY_PIN) == HIGH) ? "Остываем" : "Греем");

    mqttClient.publish("arduino/Moisture", dtostrf(moist, 6, 2, msgBuffer));
    mqttClient.publish("arduino/Moisture_digital", (digitalRead(3) == HIGH) ? "Сухо" : "Влажно");
    //Serial.print(F("Temperature: "));
    //Serial.print(t);
    //Serial.print(F(" Уставка: "));
    //Serial.print(sp);
    //Serial.println((digitalRead(RELAY_PIN) == HIGH) ? " Остываем" : " Греем");

  }
}

void reconnect() {
//  Serial.println(F("start reconnect function..."));
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
//    Serial.println(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (mqttClient.connect(CLIENT_ID)) {
//      Serial.println(F("Connected"));
      // ... and subscribe to topics
      mqttClient.subscribe("inTopic");
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(mqttClient.state());
//      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
*/
//----------------------Setup Ethernet minmaxSetting----------------------------
// Update these with values suitable for your network.
const char* ssid = "Technolink15_2G";
const char* password = "TL12345678";
const char* mqtt_server = "185.228.232.60";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
AnalogChannel *analogObj = new AnalogChannel();
float t,sp;

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

  randomSeed(micros());

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
    client.publish(TOPIC_TEMP, dtostrf(t, 6, 2, msgBuffer));
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

analogObj->setSetting(10, 50);
analogObj->descr = "arduino/temp_sp";

  // setup ethernet communication using DHCP
    setup_wifi();

  // setup mqtt client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  sysTime.tick();
}


void loop() {
sysTime.tick();
Serial.print("значение analogObj");
Serial.println(analogObj->value());

  if (!client.connected()) {
   reconnect();
  }
  client.loop();
  // it's time to send new data?
  if ((sysTime.sec % PUBLISH_PERIOD) == 0) {
    sendData();
  }
delay(300);
}
