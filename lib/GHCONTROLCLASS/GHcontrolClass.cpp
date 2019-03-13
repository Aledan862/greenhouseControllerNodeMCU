#include "GHcontrolClass.h"

// Pin addressing            D0,D1,D2,D3,D4,D05,D06,D07,D08
uint8 gpio_Addressing [9] = {16, 5, 4, 0, 2, 14, 12, 13, 15};

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(gpio_Addressing[ONE_WIRE_BUS]);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// DeviceAddress therm_Address [THERM_NUM] ={
//    { 0x28, 0x76, 0xAB, 0x77, 0x91, 0x11, 0x02, 0x4E  },
//    { 0x28, 0x69, 0x34, 0x77, 0x91, 0x0B, 0x02, 0xE2  }
//  };



uint8 AnalogChannel::counter = 0;

void AnalogChannel::setSetting(long minValue, long maxValue) {
  this->minValue = minValue;
  this->maxValue = maxValue;
}

void AnalogChannel::init() {
  counter++;
  channel = counter + 16;
  minValue = 0;
  maxValue = 100;
}

AnalogChannel::AnalogChannel() {
  counter++;
  channel = counter + 16;
  minValue = 0;
  maxValue = 100;
}

float AnalogChannel::value(){
  int rawReading = analogRead(channel);
  this->val = (maxValue-minValue)*(rawReading / 1023.0) + minValue;
  return this->val;
}

uint8 DigitalChannel::counter = 0;

DigitalChannel::DigitalChannel(){
  switch (counter) {
    case ONE_WIRE_BUS:
      counter++; //добавляем 1 чтобы перейти на следующий вход
      channel = gpio_Addressing[counter];
      pinMode(channel, INPUT);
      counter++;
      break;
    default:
      channel = gpio_Addressing[counter];
      pinMode(channel, INPUT);
      counter++;
      break;
  }
}

DigitalChannel::DigitalChannel(uint8 pin){
  channel = gpio_Addressing[pin];
  pinMode(channel, INPUT);
}

bool DigitalChannel::value(){
  this->val = digitalRead(channel);
  return this->val;
}

uint8 Relay::counter = 0;

Relay::Relay() {
  counter++;
  channel = gpio_Addressing[counter + DO_START - 1];
  pinMode(channel, OUTPUT);
  digitalWrite(channel, LOW);
}

Relay::Relay(uint8 pin) {
  channel = gpio_Addressing[pin];
  pinMode(channel, OUTPUT);
  digitalWrite(channel, LOW);
}

bool Relay::value() {
  this->val = digitalRead(channel);
  return this->val;
}

void Relay::value(bool s){
  digitalWrite(channel, s);
}

void Relay::toggle(){
  digitalWrite(channel, !digitalRead(channel));
}

uint8 Thermometer::numberOfTherm = 0;

Thermometer::Thermometer(){
  numberOfTherm++;
  DeviceAddress Common_Address = { 0x28, 0x76, 0xAB, 0x77, 0x91, 0x11, 0x02, 0x4E  };
  this->number = numberOfTherm;
  for (uint8 i=0; i<8 ;i++){
    Therm_Address[i] = Common_Address[i];
  }
}

void Thermometer::init(uint8 number, DeviceAddress deviceAddress, String descr){
  numberOfTherm++;
  this->number = number;
  this->descr = descr;
  for (uint8 i=0; i<8 ;i++){
    Therm_Address[i] = deviceAddress[i];
  }
  sensors.setResolution(Therm_Address, TEMPERATURE_PRECISION);
}

float Thermometer::value() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(Therm_Address);
  if (tempC == -127.00) {
    this->val = NAN;
  } else {
    this->val = tempC;
  }
  return this->val;
}

GH::GH(){
  a = new AnalogChannel[1];
  //d = new DigitalChannel[digitalChannels];
  k = new Relay[1];
}

void GH::init(uint8 thermometers,
                DeviceAddress thermAddresses[],
                uint8 analogChannels,
                uint8 digitalChannels,
                uint8 relays) {
                  a = new AnalogChannel[analogChannels];
                  d = new DigitalChannel[digitalChannels];
                  k = new Relay[relays];
                  t = new Thermometer[thermometers];
                  for (uint8 i = 0; i<thermometers; i++){
                    t[i].init(i+1, thermAddresses[i], String("Термометр #") + String(i+1));
                  }
}

void GH::init(uint8 analogChannels,
//              uint8 digitalChannels,
              uint8 relays){
  a = new AnalogChannel[analogChannels];
  //d = new DigitalChannel[digitalChannels];
  k = new Relay[relays];
}


void GH::run(){}

/*
void GH::readAIs () {
  for (uint8 i =1; i < AI_NUM; i++) {
    a[i].value()
  }
};

void GH::readThemperatures() {
sensors.requestTemperatures();
  for (byte i =0; i < THERM_NUM; i++) {
    therm [i] = getTemperature(Therm_Address[i]);
  }
};

void readDOs() {
  for (byte i =1; i < DO_NUM+1; i++) {
    k [i] = !digitalRead(DO_START+i-1);
  }
}

void writeDOs() {
  for (byte i =1; i < DO_NUM+1; i++) {
    digitalWrite(DO_START+i-1, !k[i]);
  }
}
*/

void DiscretRegul::init(float *pv, float *sp, float deadband, Relay *relay){
  _pv = pv;
  //this->pv = pv;
  _sp = sp;
  this->deadband = deadband;
  outport = relay->number;
}

void discretRegul(float pv, float sp, float deadband, Relay  outport ) {

  if ((pv > sp + deadband) and outport.val) {
    outport.value(0);
  } else if  ((pv < sp - deadband) and !outport.val) {
    outport.value(1);
  }
}

void SysTime::tick() {
    sec = (millis() / 1000)%60;
    if ((last_sec == 59) && (sec == 0)) {
      min = (min+1)%60;
    }
    if ((last_min == 59) && (min == 0)) {
      hour = (hour+1)%24;
    }
    if ((last_hour == 23) && (hour == 0)) {
      day = (day+1)%365;
    }

  if (sec!=last_sec) {newSec = 1;}

  last_sec = sec;
  last_min = min;
  last_hour = hour;
  }

String SysTime::hwclock(){
  String dots = String(":");
  return String(day) + String(" ") + String(hour) + dots + String(min) + dots + String(sec);
}

//Mqtt::Mqtt(){};

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

void Mqtt::setup(char* _ssid, char* _password, char* _mqtt_server){
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(_ssid);

  WiFi.begin(_ssid, _password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setClient(espClient);
  client.setServer(_mqtt_server, 1883);
  //client.setCallback(callback);
}


void Mqtt::reconnect() {
if (!client.connected()){
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
      //Serial.println(" try again in 5 seconds");
      //delay(5000);
    }
  }
}

void Mqtt::sendData(DigitalChannel signal) {
  char topic[64];
  (String(CLIENT_ID)+ String("/") + signal.descr).toCharArray(topic, 32);
  client.publish(topic, signal.val?"1":"0");
}

void Mqtt::sendData(Relay signal) {
  char topic[64];
  (String(CLIENT_ID)+ String("/") + signal.descr).toCharArray(topic, 64);
  client.publish(topic, signal.val?"1":"0");
}

void Mqtt::sendData(AnalogChannel signal){
  char msgBuffer[20];
  char topic[64];
  (String(CLIENT_ID)+ String("/") + signal.descr).toCharArray(topic, 64);
  client.publish(topic, dtostrf(signal.val, 6, 2, msgBuffer));
}

void Mqtt::sendData(Thermometer signal){
  char msgBuffer[20];
  char topic[64];
  (String(CLIENT_ID)+ String("/") + signal.descr).toCharArray(topic, 64);
  client.publish(topic, dtostrf(signal.val, 6, 2, msgBuffer));
}

void Mqtt::sendTime(){
  char msgBuffer[64];
  char topic[64];
  (String(CLIENT_ID)+ String("/") + "System time").toCharArray(topic, 64);
  sysTime.hwclock().toCharArray(msgBuffer,64);
  client.publish(topic, msgBuffer);
}
