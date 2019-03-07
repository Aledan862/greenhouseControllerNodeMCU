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


bool DigitalChannel::value() {

  this->val = digitalRead(channel);
  return this->val;

}

uint8 Relay::counter = 0;

Relay::Relay(){
  counter++;
  channel = gpio_Addressing[counter + DO_START - 1];
  pinMode(channel, OUTPUT);
  digitalWrite(channel, LOW);
}


Relay::Relay(uint8 pin){
  channel = gpio_Addressing[pin];
  pinMode(channel, OUTPUT);
  digitalWrite(channel, LOW);
}

bool Relay::value(){
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

};


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

GH::GH(){
  a = new AnalogChannel[1];
  //d = new DigitalChannel[digitalChannels];
  k = new Relay[1];
}

void GH::run(){};

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
};

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
  };
String SysTime::hwclock(){
  String dots = String(":");
  return String(day) + String(" ") + String(hour) + dots + String(min) + dots + String(sec);
}
