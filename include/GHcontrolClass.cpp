#include <GHcontrolClass.h>


void AnalogChannel::init(long minV, long maxV) {
  counter++;
  (counter > 3) ? channel = counter +2 : channel = counter;
  minValue = minV;
  maxValue = maxV;
}

void AnalogChannel::init() {
  counter++;
  (counter > 3) ? channel = counter +2 : channel = counter;
  minValue = 0;
  maxValue = 100;
}


float AnalogChannel::value(){
  int rawReading = analogRead(channel);
  return (maxValue-minValue)*(rawReading / 1023.0) + minValue;
}

void DigitalChannel::init(uint8 diChannel){
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

void Relay::init(uint8 _number){
  number=_number;
  channel = _number + DO_START - 1;
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


void Thermometer::init(uint8 _number, DeviceAddress deviceAddress){
  numberOfTherm++;
  number = _number;
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
              uint8 relays){

  for (uint8 i = 0; i< thermometers; i++){
    t[i].init(i+1, thermAddresses[i]);
    }
  for (uint8 i = 0; i< analogChannels; i++){
    a[i].init();
    }
  for (uint8 i = 0; i< digitalChannels; i++){
    d[i].init(i+1);
    }
  for (uint8 i = 0; i< relays; i++){
    k[i].init(i+1);
    }
}



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

void discretRegul(float pv, float sp, float deadband, int outport ) {
  if ((pv > sp + deadband) and !digitalRead(outport)) {
    digitalWrite(outport, HIGH);
  } else if  ((pv < sp - deadband) and digitalRead(outport)) {
    digitalWrite (outport, LOW);
  }
}
