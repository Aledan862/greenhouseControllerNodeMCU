#include <Arduino.h>
#include <UIPEthernet.h>
#include "PubSubClient.h"
//#include <OneWire.h>
#include <DallasTemperature.h>
#define CLIENT_ID "arduino"
#define TOPIC_TEMP  "arduino/temp"
#define TOPIC_OUT "arduino/relay"
#define TOPIC_TEMP_SP "arduino/temp_sp"
#define PUBLISH_DELAY   3000 //задержка отправки данных
#define ONE_WIRE_BUS 2 //номер цифрового канала для шины 1-wire
#define SP_TEMP_PIN A0 //номер пина для потенциометра регулирования уставки температуры
#define SP_MAX 40     //максимальная температура уставки
#define SP_MIN 0      //минимальная температура уставки
#define RELAY_PIN 5    //пин на реле регулятора температуры
#define RELAY1_PIN 6   //пины на модуль 4-х канального реле
#define RELAY2_PIN 7
#define RELAY3_PIN 8
#define RELAY4_PIN 9

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

DeviceAddress Thermometer1 = { 0x28, 0x85, 0xC7, 0x5B, 0x1E, 0x13, 0x01, 0x79 }; //28 85 C7 5B 1E 13 1 79
float t;            //значение температуры с термометра
long sp;           //значение уствки температуры
long moist;
// MAC-адрес нашего устройства
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
    /*Serial.print(F("Temperature: "));
    Serial.print(t);
    Serial.print(F(" Уставка: "));
    Serial.print(sp);
    Serial.println((digitalRead(RELAY_PIN) == HIGH) ? " Остываем" : " Греем");*/

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

float getTemperature(DeviceAddress deviceAddress) {
  float tempC = sensors.getTempC(deviceAddress);
   if (tempC == -127.00) {
    return NAN;
   } else {
    return tempC;
   }
 }
void discretRegul(float pv, float sp, float deadband, int outport ) {
  if ((pv > sp + deadband) and !digitalRead(outport)) {
    digitalWrite(outport, HIGH);
  } else if  ((pv < sp - deadband) and digitalRead(outport)) {
    digitalWrite (outport, LOW);
  }
}


void setup() {
  sensors.begin();
  // set the resolution to 10 bit (good enough?)
  sensors.setResolution(Thermometer1, 10);
  // setup output pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);

  // setup serial communication

    Serial.begin(9600);
  // setup ethernet communication using DHCP
    Ethernet.begin(mac);
//    Serial.println(F("Ethernet configured"));
//    Serial.print(F("IP address: "));
    Serial.println(Ethernet.localIP());
    Serial.println();

  // setup mqtt client
  mqttClient.setClient(ethClient);
  mqttClient.setServer(mqttServer, 1883);
  mqttClient.setCallback(callback);

  if (mqttClient.connect(CLIENT_ID)) {
    mqttClient.subscribe("inTopic");
    Serial.println(F("MQTT client configured"));
  } else {
    reconnect();
  }
  Serial.println();
//  Serial.println(F("Ready to send data"));
  previousMillis = millis();
}

void loop() {
  sp = map(analogRead(SP_TEMP_PIN), 0, 1023, SP_MIN, SP_MAX);
  sp = constrain(sp, SP_MIN, SP_MAX);
  sensors.requestTemperatures();
  t = getTemperature(Thermometer1);
  discretRegul(t, sp , 1.0, RELAY_PIN);


  if (!mqttClient.connected()) {
  reconnect();
  }
  // it's time to send new data?
  if (millis() - previousMillis > PUBLISH_DELAY) {
    sendData();
    previousMillis = millis();
  }
  mqttClient.loop();
}
