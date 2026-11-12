#include "settings.h"
#include <math.h>

const int TEMP_SENSOR = A1;

void tempSetup() {
  pinMode(TEMP_SENSOR, INPUT);
}

void readTemp() {
  static unsigned long lastRun = millis();
  if (millis() - lastRun < 1000) return;
  lastRun = millis();
  int raw = analogRead(TEMP_SENSOR);
  if (raw <= 0 || raw >= 1023) {
    return;
  }
  float resistance = (1023.0 - raw) * 10000.0 / raw;
  float temperatureC = 1.0 / (log(resistance / 10000.0) / 3975.0 + 1.0 / 298.15) - 273.15;

  sysStatus.tempVal = temperatureC;
}