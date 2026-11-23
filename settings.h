#pragma once
#include <Arduino.h>

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
  float tempVal = 0;
  int lightVal = 0;
  unsigned int lighttime = 0;
  bool fanOn = false;
  bool ventOpen = false;
  bool lightOn = false;
  bool alarmOn = false;    
  bool alarmMuted = false; 
  bool ventWarning = false;
  bool sensorError = false;
};


const Profile PROFILES[] = {
  {"Basil", 26.0, 400},
  {"Lettuce", 20.0, 450},
  {"Tomato", 27.0, 350},
};

Settings settings = {0, PROFILES[0].temp, PROFILES[0].light};
Status sysStatus;