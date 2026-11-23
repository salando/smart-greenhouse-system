// vent.ino
// Vent warning and the out-of-range alarm.

#include "settings.h"

const int VENT_BUZZER_PIN = 4;

const unsigned long ALARM_DELAY_MS = 60000;   // 60 s out of range before the alarm fires
const unsigned long MUTE_MS        = 300000;  // 5 min mute, then it re-arms

unsigned long ventOutOfRangeStart = 0;
unsigned long ventMuteStart       = 0;
unsigned long ventLastBeep        = 0;


void alarmSetup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(VENT_BUZZER_PIN, OUTPUT);
}


void alarmUpdate() {
  unsigned long now = millis();

  float temp = sysStatus.tempVal;

  // Vent opens two degrees above the set point, closes back at it
  if (!sensorError && temp >= settings.setTemp + 2) ventWarning = true;
  if (!sensorError && temp <= settings.setTemp)     ventWarning = false;

  // Alarm condition: broken sensor, or well outside the acceptable band
  bool outOfRange = (sensorError || temp > settings.setTemp + 4 || temp < 15);

  if (!outOfRange) {
    ventOutOfRangeStart = now;
    alarmOn = false;
    muted   = false;
  }
  else if (now - ventOutOfRangeStart >= ALARM_DELAY_MS) {
    alarmOn = true;
  }

  // Button silences the buzzer while the alarm is active
  if (alarmOn && digitalRead(BUTTON_PIN) == HIGH) {
    muted = true;
    ventMuteStart = now;
  }
  if (muted && now - ventMuteStart >= MUTE_MS) muted = false;

  if (alarmOn && !muted && now - ventLastBeep >= 500) {
    tone(VENT_BUZZER_PIN, 1000, 250);
    ventLastBeep = now;
  }
}
