#include <Arduino.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
#define THERM_NUM 2  //количество термометров
#define AI_NUM 5     //макс количество аналогов
#define DI_NUM 4    //макс количество дискретных входов
#define DO_START 7  //с какого пина начинается отсчет для выходов
#define DO_NUM 3    //макс количество дискретных выходов

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
/*
DeviceAddress Therm_Address [THERM_NUM] ={
   { 0x28, 0x76, 0xAB, 0x77, 0x91, 0x11, 0x02, 0x4E  },
   { 0x28, 0x69, 0x34, 0x77, 0x91, 0x0B, 0x02, 0xE2  }
 };
*/
class AnalogChannel {
public:
  AnalogChannel();
  void init();
  void setSetting(long minValue, long maxValue);
  float value();
//void  set(float value);
private:
  uint8 channel;
  long minValue;
  long maxValue;
  static uint8 counter;
};
uint8 AnalogChannel::counter = 0;

class DigitalChannel {
public:
  DigitalChannel();
  DigitalChannel(uint8 diChannel);
  bool value();
private:
  uint8 channel;
  static uint8 counter;
// void set(bool value);
};
uint8 DigitalChannel::counter = 0;

class Relay {
public:
  Relay();
  void init(uint8 number);
  bool get();
  void set(bool s);
  void toggle();
  uint8 number;
private:
  uint8 channel;
  static uint8 counter;
};
uint8 Relay::counter = 0;

class Thermometer {
public:
  static uint8 numberOfTherm;
  Thermometer();
  //Thermometer(uint8 number, DeviceAddress deviceAddress);
  void init(uint8 number, DeviceAddress deviceAddress);
  float value();
  uint8 number;
private:
  DeviceAddress Therm_Address;
};

uint8 Thermometer::numberOfTherm = 0;

class GH {
public:
static  void init(uint8 thermometers,
            DeviceAddress thermAddresses[],
            uint8 analogChannels,
            uint8 digitalChannel,
            uint8 relay);
  void run();
private:
  void readThemperatures();
  void readAIs();
  void readDOs();
  void writeDOs();
};

class discretRegul {
public:
    float pv;
    float sp;
    float deadband;
    int outport;

    void init();
    void run();
};
/*
Thermometer t[THERM_NUM];
AnalogChannel a[AI_NUM];
DigitalChannel d[DI_NUM];
Relay k[DO_NUM];
*/
