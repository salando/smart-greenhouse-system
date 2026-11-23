// climate.ino
// Climate control: reads the temperature and switches the fan relay.
// Original by Gaurika Sharma, adapted for the merged sketch.

#include "settings.h"
#include <math.h>

const int CLIMATE_TEMP_PIN  = A1;
const int CLIMATE_RELAY_PIN = 7;

const int   CLIMATE_B_VALUE   = 3975;   // thermistor constant for the Grove sensor
const float CLIMATE_DEAD_BAND = 2.0;    // fan switches off this far below the set point
const unsigned long CLIMATE_READ_INTERVAL = 500;

float climateTempC = 0.0;
bool  climateFanOn = false;
unsigned long climateLastRead = 0;


// Returns degrees C, or the last good reading if the sensor looks disconnected.
float readTemperatureC() {
  int raw = analogRead(CLIMATE_TEMP_PIN);

  if (raw <= 0 || raw >= 1023) {
    sysStatus.sensorError = true;
    return climateTempC;
  }

  sysStatus.sensorError = false;

  float resistance = (1023.0 - raw) * 10000.0 / raw;
  float kelvin = 1.0 / (log(resistance / 10000.0) / CLIMATE_B_VALUE + 1.0 / 298.15);

  return kelvin - 273.15;
}


void setFan(bool on) {
  climateFanOn    = on;
  sysStatus.fanOn = on;
  digitalWrite(CLIMATE_RELAY_PIN, on ? HIGH : LOW);
}


void climateSetup() {
  pinMode(CLIMATE_RELAY_PIN, OUTPUT);
  setFan(false);
  analogReadResolution(10);
  climateTempC = readTemperatureC();
}


void climateUpdate() {
  unsigned long now = millis();
  if (now - climateLastRead < CLIMATE_READ_INTERVAL) return;
  climateLastRead = now;

  climateTempC = readTemperatureC();
  sysStatus.tempVal = climateTempC;

  if (sysStatus.sensorError) {
    setFan(false);
    return;
  }

  // Dead band: on at the set point, off two degrees below. Stops the fan chattering.
  if (!climateFanOn && climateTempC >= settings.setTemp) {
    setFan(true);
  }
  else if (climateFanOn && climateTempC <= settings.setTemp - CLIMATE_DEAD_BAND) {
    setFan(false);
  }
}
