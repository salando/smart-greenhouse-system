#include "settings.h"

void setup() {
  displaySetup();
  climateSetup();
  lightSetup();
  alarmSetup();
}

void loop() {
  displayUpdate();
  climateUpdate();
  trackSunlight();
  alarmUpdate();
}
