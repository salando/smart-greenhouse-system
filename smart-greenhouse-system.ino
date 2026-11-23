#include "settings.h"

void setup() {
   Serial.begin(9600);
  while (!Serial && millis() < 3000) { }
  displaySetup();
  climateSetup();
  lightSetup();
  alarmSetup();
  netSetup();
}

void loop() {
  displayUpdate();
  climateUpdate();
  trackSunlight();
  alarmUpdate();
  netUpdate();
}
