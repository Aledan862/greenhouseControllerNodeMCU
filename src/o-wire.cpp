#include "o-wire.h"


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int numberOfDevices; // Number of temperature devices found
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address
// function to print a device address
//DeviceAddress t1 = { 0x28, 0x0B, 0x5F, 0x77, 0x91, 0x14, 0x02, 0xDA  };
void printAddress(DeviceAddress deviceAddress)
{
  Serial.print(" { ");
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(", ");
  }
  Serial.println("  },");
}

long lastSetup;

bool sensorSetup() {
  // Start up the library
  sensors.begin();
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  // locate devices on the bus
  Serial.print("Locating devices...");

  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		Serial.print("Found device ");
		Serial.print(i, DEC);
		Serial.print(" with address: ");
		printAddress(tempDeviceAddress);
		Serial.println();

		Serial.print("Setting resolution to ");
		Serial.println(TEMPERATURE_PRECISION, DEC);
		// set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
		sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
		 Serial.print("Resolution actually set to: ");
		Serial.print(sensors.getResolution(tempDeviceAddress), DEC);
		Serial.println();
	}else{
		Serial.print("Found ghost device at ");
		Serial.print(i, DEC);
		Serial.print(" but could not detect address. Check power and cabling");
	}
  }

lastSetup = millis();
return numberOfDevices;
}

void readTemp()
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  Serial.print("Target reading...");
  Serial.println(sensors.getTempC(tempDeviceAddress));

  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		// Output the device ID
		Serial.print("Temperature for device: ");
		Serial.println(i,DEC);
		// It responds almost immediately. Let's print out the data
		Serial.println(sensors.getTempC(tempDeviceAddress)); // Use a simple function to print out the data
	}
	//else ghost device! Check your power requirements and cabling
  }
}

float readTemp(int i)
{
    sensors.requestTemperatures(); // Send the command to get temperatures
    if(sensors.getAddress(tempDeviceAddress, i)){
        return sensors.getTempC(tempDeviceAddress);
    }else {
        if (millis() - lastSetup > 5000) sensorSetup(); // rescan sensors
        return NAN;
    }

}