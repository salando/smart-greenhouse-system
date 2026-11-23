#include "settings.h"

void setup() {
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
