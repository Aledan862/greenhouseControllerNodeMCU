#include "GHcontrolClass.h"

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

DeviceAddress Therm_Address [THERM_NUM] ={
   { 0x28, 0x76, 0xAB, 0x77, 0x91, 0x11, 0x02, 0x4E  },
   { 0x28, 0x69, 0x34, 0x77, 0x91, 0x0B, 0x02, 0xE2  }
 };

uint8 AnalogChannel::counter = 0;

void AnalogChannel::setSetting(long minValue, long maxValue) {
  this->minValue = minValue;
  this->maxValue = maxValue;
}

void AnalogChannel::init() {
  counter++;
  (counter > 3) ? channel = counter +2 + 16 : channel = counter + 16;
  minValue = 0;
  maxValue = 100;
}

AnalogChannel::AnalogChannel() {
  counter++;
  (counter > 3) ? channel = counter +2+16 : channel = counter+16;
  minValue = 0;
  maxValue = 100;
}


float AnalogChannel::value(){
  int rawReading = analogRead(channel);
  return (maxValue-minValue)*(rawReading / 1023.0) + minValue;
}

uint8 DigitalChannel::counter = 0;

DigitalChannel::DigitalChannel(){
  DigitalChannel(counter);
}

DigitalChannel::DigitalChannel(uint8 diChannel){
  switch (diChannel) {
    case ONE_WIRE_BUS:
      break;
    default:
      pinMode(diChannel, INPUT);
      channel = diChannel;
      break;
  }
}

bool DigitalChannel::value() {
  return  digitalRead(channel);
}

uint8 Relay::counter = 0;

Relay::Relay(){
  counter++;
  channel = counter + DO_START - 1;
  pinMode(channel, OUTPUT);
}

bool Relay::get(){
  return digitalRead(channel);
}

void Relay::set(bool s){
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

void Thermometer::init(uint8 number, DeviceAddress deviceAddress){
  numberOfTherm++;
  this->number = number;
  for (uint8 i=0; i<8 ;i++){
    Therm_Address[i] = deviceAddress[i];
  }
}

float Thermometer::value() {

  float tempC = sensors.getTempC(Therm_Address);
  if (tempC == -127.00) {
    return NAN;
  } else {
    return tempC;
  }
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
    t[i].init(i+1, thermAddresses[i]);
  }
}

void GH::init(uint8 analogChannels,
//              uint8 digitalChannels,
              uint8 relays){
  a = new AnalogChannel[analogChannels];
  //d = new DigitalChannel[digitalChannels];
  k = new Relay[relays];
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

void discretRegul(float pv, float sp, float deadband, int outport ) {
  if ((pv > sp + deadband) and !digitalRead(outport)) {
    digitalWrite(outport, HIGH);
  } else if  ((pv < sp - deadband) and digitalRead(outport)) {
    digitalWrite (outport, LOW);
  }
}

GH GHcontroler;
