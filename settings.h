#pragma once
#include <Arduino.h>

bool ventWarning = false;
bool alarmOn = false;
bool muted = false;
float temperature = 0;
bool sensorError = false;
const int BUTTON_PIN = D2;

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
  float tempVal;
  int lightVal;
  bool fanOn;
  bool ventOpen;
  bool lightOn;
  bool alarmOn;
  bool alarmMuted;
};


const Profile PROFILES[] = {
  {"Basil", 26.0, 400},
  {"Lettuce", 20.0, 450},
  {"Tomato", 27.0, 350},
};
Settings settings = {0, PROFILES[0].temp, PROFILES[0].light};
Status sysStatus;