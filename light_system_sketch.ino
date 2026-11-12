#include "settings.h"

const int lightSensorPin = A0; // Light sensor on A0
const int ledPin = 3;     // Relay/LED on D3
const int threshold = 400;   // Light threshold

unsigned int sunlightSeconds = 0; // stores running time counter

void lightSetup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("System Ready.");
}

// function parameters - tSeconds
// returns nothing
void trackSunlight(int& tSeconds) {
  static int lastRun = millis();
  if (millis() - lastRun < 1000) return;
  lastRun = millis();

  sysStatus.lightVal = analogRead(lightSensorPin);
  if (sysStatus.lightVal < settings.darkThreshold) {
    digitalWrite(ledPin, LOW); // turn on LED/Relay
    tSeconds++;
    }
  else if (sysStatus.lightVal > settings.darkThreshold + 50) {
    digitalWrite(ledPin, LOW); // turn off LED/Relay for plants
  }
  sysStatus.lightOn = digitalRead(ledPin);
}

int tSeconds;
int h = tSeconds / 3600; //getting hours
int m = tSeconds % 3600 / 60;  // getting minutes
int s = tSeconds % 60; //getting seconds

//printing light value and sunlight time
void displayLight() {
  Serial.print("Light: ");
  Serial.print(sysStatus.lightVal);
  Serial.print(" | Sunlight Time: ");
  Serial.print(h);
  Serial.print("h ");
  Serial.print(m);
  Serial.print("m ");
  Serial.print(s);
  Serial.println("s");
}