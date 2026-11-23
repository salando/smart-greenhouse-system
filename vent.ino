#include <Servo.h>
#include "settings.h"

const int VENT_BUZZER_PIN = 4;
const int VENT_SERVO_PIN  = 3;
const int VENT_CLOSED     = 0;    // servo angle when closed
const int VENT_OPEN       = 90;   // servo angle when open

const unsigned long ALARM_DELAY_MS = 900000;   
const unsigned long MUTE_MS        = 600000; 

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
    if (!sysStatus.sensorError && temp >= settings.setTemp + 2) sysStatus.ventWarning = true;
    if (!sysStatus.sensorError && temp <= settings.setTemp)     sysStatus.ventWarning = false;

  // Move the servo and share the vent state with the LCD and logger
    if (sysStatus.ventWarning) {
    ventServo.write(VENT_OPEN);     // 90 degrees
    } 
    else {
    ventServo.write(VENT_CLOSED);   // 0 degrees
    }

    sysStatus.ventOpen = sysStatus.ventWarning;

  // Alarm conditions
    bool outOfRange = (sysStatus.sensorError || temp > settings.setTemp + 4 || temp < 15);

  if (!outOfRange) {
    ventOutOfRangeStart = now;
    sysStatus.alarmOn = false;
    sysStatus.alarmMuted   = false;
  }
  else if (now - ventOutOfRangeStart >= ALARM_DELAY_MS) {
    sysStatus.alarmOn = true;
  }

  // Button silences the buzzer while the alarm is active
  if (sysStatus.alarmOn && digitalRead(BUTTON_PIN) == HIGH) {
    sysStatus.alarmMuted = true;
    ventMuteStart = now;
  }
  if (sysStatus.alarmMuted && now - ventMuteStart >= MUTE_MS) sysStatus.alarmMuted = false;

  if (sysStatus.alarmOn && !sysStatus.alarmMuted && now - ventLastBeep >= 500) {
    tone(VENT_BUZZER_PIN, 1000, 250);
    ventLastBeep = now;
  }
}
