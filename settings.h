#pragma once
#include <Arduino.h>

const int BUTTON_PIN = D2;

struct Profile {
  const char* name;
  float temp;
  int light;
};

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


// {name, fan/vent temp in C (top of the plant's ideal range), dark threshold (higher = light comes on sooner)}
const Profile PROFILES[] = {
  {"Basil",      27.0, 500},
  {"Lettuce",    21.0, 400},
  {"Tomato",     27.0, 550},
  {"Pepper",     29.0, 550},
  {"Spinach",    20.0, 400},
  {"Strawberry", 24.0, 500},
  {"Mint",       24.0, 350},
  {"Seedlings",  24.0, 600},
  {"Cactus",     32.0, 600},
  {"Fern",       22.0, 250},
};

const int PROFILE_COUNT = sizeof(PROFILES) / sizeof(PROFILES[0]);

Settings settings = {0, PROFILES[0].temp, PROFILES[0].light};
Status sysStatus;