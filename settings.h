#pragma once
#include <Arduino.h>

struct Profile {
  const char* name;
  float temp;
  int light;
};
const int PROFILE_COUNT = 3;

struct Settings {
  int profile;
  float setTemp;
  int darkThreshold;
};

struct Status {
  float temp;
  int light;
  bool fanOn;
  bool ventOpen;
  bool lightOn;
  bool alarmOn;
  bool alarmMuted;
};


const Profile PROFILES[] = {I
  {"Basil",   26.0, 400},
  {"Lettuce", 20.0, 450},
  {"Tomato",  27.0, 350},
};
Settings settings = {0, PROFILES[0].temp, PROFILES[0].light};
Status sysStatus;