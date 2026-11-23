// vent.ino
// Vent servo, vent warning and the out-of-range alarm.

#include <Servo.h>
#include "settings.h"

const int VENT_BUZZER_PIN = 4;
const int VENT_SERVO_PIN  = 3;
const int VENT_CLOSED     = 0;    // servo angle when closed
const int VENT_OPEN       = 90;   // servo angle when open

const unsigned long ALARM_DELAY_MS = 60000;   // 60 s out of range before the alarm fires
const unsigned long MUTE_MS        = 300000;  // 5 min mute, then it re-arms

Servo ventServo;

unsigned long ventOutOfRangeStart = 0;
unsigned long ventMuteStart       = 0;
unsigned long ventLastBeep        = 0;


void alarmSetup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(VENT_BUZZER_PIN, OUTPUT);
  ventServo.attach(VENT_SERVO_PIN);
  ventServo.write(VENT_CLOSED);   // start with the vent closed
}


void alarmUpdate() {
  unsigned long now = millis();

  float temp = sysStatus.tempVal;

  // Vent opens two degrees above the set point, closes back at it
  if (!sensorError && temp >= settings.setTemp + 2) ventWarning = true;
  if (!sensorError && temp <= settings.setTemp)     ventWarning = false;

  // Move the servo and share the vent state with the LCD and logger
  ventServo.write(ventWarning ? VENT_OPEN : VENT_CLOSED);
  sysStatus.ventOpen = ventWarning;

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

  sysStatus.alarmOn    = alarmOn;
  sysStatus.alarmMuted = muted;

  if (alarmOn && !muted && now - ventLastBeep >= 500) {
    tone(VENT_BUZZER_PIN, 1000, 250);
    ventLastBeep = now;
  }
}
