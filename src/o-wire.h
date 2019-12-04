#pragma once
#include <Arduino.h>
#include "options.h"

#include <DallasTemperature.h>

bool sensorSetup();
void readTemp();
float readTemp(int i);