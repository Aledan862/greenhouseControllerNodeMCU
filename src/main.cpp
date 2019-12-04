 
#include "options.h"

#include "myWebServer.h"
#include "GyverTimer.h"
#include "o-wire.h"

GTimer_ms tSEC;

const char* SSID = "Greenhouse";  // Modify for your available WiFi AP
const char* PSK  = "12345678";  // Modify for your available WiFi AP


int tempSetting[2] = {-5, 70};

void discretRegul(float pv, float sp, float deadband, int outport ) {
  if ((pv > sp + deadband) and !digitalRead(outport)) {
    digitalWrite(outport, HIGH);
    // Serial.println("Остываем");
  } else if  ((pv < sp - deadband) and digitalRead(outport)) {
    digitalWrite (outport, LOW);
    // Serial.println("Греем");
  }
}


void setup() {
  delay(1000);  // for stable the module.
  Serial.begin(BAUDRATE);

  pinMode(RELAYPIN, OUTPUT);
  // digitalWrite(ONBOARD_LED, HIGH);


  tSEC.setInterval(1000);

  wifiServerStart(SSID,PSK);
  sensorSetup();
}

int8_t lastNumClients = -1;
int ramp = 0;
#ifdef DEVMODE
float dummyTemp;
#else
float temp;
#endif

void loop() {

  int numClients = WiFi.softAPgetStationNum();
  if (numClients != lastNumClients){
    Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
    Serial.println(WiFi.status());
    lastNumClients = numClients;
  };
    
  //if (tSEC.isReady()) Serial.println(WiFi.status());
  #ifdef DEVMODE
  if (tSEC.isReady()&(numClients)) {
    dummyTemp = ramp - 5 + float(random(0,10)/10);
    ramp = ++ramp % 75;
    Serial.printf("dummyTemp = %f\n", dummyTemp);
  };
  #else
    int sp = analogRead(A0);
    sp = map(sp, 0, 1024, tempSetting[0], tempSetting[1]);
    temp = readTemp(0);
    if (tSEC.isReady()) Serial.printf("Temp = %f\n", temp);
  #endif

  discretRegul(temp, sp, 2, RELAYPIN);

  Server.handleClient();
}
