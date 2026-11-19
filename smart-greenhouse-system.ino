#include "settings.h"

void setup() {
  displaySetup();
  tempSetup();
  lightSetup();
  alarmSetup();
}

void loop() {
  displayUpdate();
  readTemp();
  trackSunlight(sysStatus.lighttime);
  alarmUpdate();
}
