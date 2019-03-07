#ifndef GH_h
#define GH_h

#include <Arduino.h>
#include <DallasTemperature.h>



#define ONE_WIRE_BUS 4
#define TEMPERATURE_PRECISION 11

#define THERM_NUM 1  //количество термометров
//#define AI_NUM 5     //макс количество аналогов
//#define DI_NUM 4    //макс количество дискретных входов
#define DO_START 5  //с какого пина начинается отсчет для выходов
//#define DO_NUM 3    //макс количество дискретных выходов

class SysTime {
public:
  uint8_t sec, min, hour, day;
  bool newSec = 0;
  void tick(); //меряет секунды
  String hwclock();// выводит в строку текущее время
private:
  uint8_t last_sec, last_min, last_hour;
};

class AnalogChannel {
public:
  AnalogChannel();
  void init();
  void setSetting(long minValue, long maxValue);
  float value();
  static uint8 counter;

  float val;

  String descr;
//void  set(float value);
private:
  uint8 channel;
  long minValue;
  long maxValue;
};


class DigitalChannel {
public:
  DigitalChannel();
  DigitalChannel(uint8 pin);
  bool value();

  bool val;
  String descr;
private:
  static uint8 counter;
  uint8 channel;

// void set(bool value);
};


class Relay {
public:
  Relay();

  Relay(uint8 pin);
  void init(uint8 number);
  static void read();


  bool value();
  void value(bool s);
  void toggle();
  uint8 number;

  bool val;
  String descr;
private:
  static uint8 counter;
  uint8 channel;


};


class Thermometer {
public:
  static uint8 numberOfTherm;
  Thermometer();
  //Thermometer(uint8 number, DeviceAddress deviceAddress);

  void init(uint8 number, DeviceAddress deviceAddress, String descr);
  float value();
  uint8 number;
  float val;
  String descr;

private:
  DeviceAddress Therm_Address;
};


class GH {
public:
  AnalogChannel* a;
  DigitalChannel* d;
  Relay* k;
  Thermometer* t;
  GH();
void init(uint8 thermometers,
            DeviceAddress thermAddresses[],
            uint8 analogChannels,
            uint8 digitalChannel,
            uint8 relay);
void init(uint8 analogChannels,
//          uint8 digitalChannels,
          uint8 relays);
void run();
private:
  //void readThemperatures();
  //void readAIs();
  //void readDOs();
  //void writeDOs();
};

class DiscretRegul {
public:
    float *_pv;
    float *_sp;
    float deadband;
    int outport;

    void init(float *pv, float *sp, float deadband, Relay *relay);
    void run();
};

extern void discretRegul(float pv, float sp, float deadband, Relay outport );
#endif

/*
Thermometer t[THERM_NUM];
AnalogChannel a[AI_NUM];
DigitalChannel d[DI_NUM];
Relay k[DO_NUM];
*/
