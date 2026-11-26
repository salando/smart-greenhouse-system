#include "settings.h"
#include <Adafruit_NeoPixel.h> // LED Strip Library

const int lightSensorPin = A0; // Light sensor on A0
const int ledPin = 6;          // Relay/LED on 
const int LED_COUNT = 8;
const int LED_BRIGHTNESS = 150;
const unsigned long LIGHT_ON_DELAY_MS = 5000; // 5-second wait before turning grow light on

Adafruit_NeoPixel strip(LED_COUNT, ledPin, NEO_GRB + NEO_KHZ800);
const int LIGHT_COLOR = Adafruit_NeoPixel::Color(255, 255, 255); // White light colour

unsigned int sunlightSeconds = 0;


void setLight(bool on) {                // Changes the light only when the status in sysStatus changes
  if (on == sysStatus.lightOn) return;
  if (on) strip.fill(LIGHT_COLOR);
  else strip.clear();
  strip.show();
  sysStatus.lightOn = on;
}

void lightSetup() {
  strip.begin();
  strip.setBrightness(LED_BRIGHTNESS);
  strip.clear();
  strip.show();
  sysStatus.lightOn = false;
  // OLD LED CODE
  // pinMode(ledPin, OUTPUT);
  // digitalWrite(ledPin, LOW);
}

void trackSunlight() {
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
   sysStatus.lighttime++; 
  }

  // 5-second debounce before turning grow light on when light drops below threshold
  if (lightVal < settings.darkThreshold) { 
    if (!isDarkTiming) {
      isDarkTiming = true;
      darkStartMs = now;
    } else if (now - darkStartMs >= LIGHT_ON_DELAY_MS) {
      setLight(true);
      // digitalWrite(ledPin, HIGH); // Turn grow light ON 
    }
  } else {
    // Light recovered before 5 seconds (or is bright enough) -> cancel timer & turn light off
    isDarkTiming = false;
    setLight(false);
    // digitalWrite(ledPin, LOW); OLD CODE
  }
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
