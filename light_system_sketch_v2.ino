#include "settings.h"

const int lightSensorPin = A0; // Light sensor on A0
const int ledPin = 3;          // Relay/LED on D3
const unsigned long LIGHT_ON_DELAY_MS = 5000; // 5-second wait before turning grow light on

unsigned int sunlightSeconds = 0;

void lightSetup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void trackSunlight(int& tSeconds) {
  static unsigned long lastSample = 0;
  static unsigned long darkStartMs = 0;
  static bool isDarkTiming = false;

  unsigned long now = millis();
  if (now - lastSample < 1000) return; 
  lastSample = now;

  int lightVal = analogRead(lightSensorPin); 
  sysStatus.lightVal = lightVal; 

  // Count sunlight time when light is at or above threshold
  if (lightVal >= settings.darkThreshold) { 
    tSeconds++; 
  }

  // 5-second debounce before turning grow light on when light drops below threshold
  if (lightVal < settings.darkThreshold) { 
    if (!isDarkTiming) {
      isDarkTiming = true;
      darkStartMs = now;
    } else if (now - darkStartMs >= LIGHT_ON_DELAY_MS) {
      digitalWrite(ledPin, HIGH); // Turn grow light ON 
    }
  } else {
    // Light recovered before 5 seconds (or is bright enough) -> cancel timer & turn light off
    isDarkTiming = false;
    digitalWrite(ledPin, LOW); // Turn grow light OFF 
  }

  sysStatus.lightOn = digitalRead(ledPin); 
}

void displayLight(int totalSec) {
  int h = totalSec / 3600;
  int m = (totalSec % 3600) / 60; 
  int s = totalSec % 60;
   
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