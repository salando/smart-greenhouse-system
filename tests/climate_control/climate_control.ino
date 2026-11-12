// Smart Greenhouse - Climate Control Subsystem (Gaurika Sharma)
// Temperature sensor: A1 | Relay (fan): D7 | LCD: I2C (for testing only)
// Turns the fan (relay) ON when it gets too hot and OFF once it cools down 2 C below the set-point.
// The climate functions are kept separate so they can be copied into the final group sketch.
#include <Wire.h>                    // Loads the I2C library so the Arduino can talk to the LCD with 2 wires
#include "rgb_lcd.h"                 // Loads the Grove RGB LCD library (same one used in the labs)
#include <math.h>                    // Loads math functions; we need log() for the temperature formula

// ======================= CLIMATE CONTROL SETTINGS =======================

const int TEMP_PIN  = A1;            // The temperature sensor is plugged into analog port A1
const int RELAY_PIN = 7;             // The relay (our "fan") is plugged into digital port D7

const int   B_VALUE   = 3975;        // A constant for this Grove temperature sensor, from the Lab 3 code
const float DEAD_BAND = 2.0;         // Fan turns OFF only after it cools 2 C below the set-point (stops flicker)

const unsigned long READ_INTERVAL = 500;  // Read the temperature every 500 ms (0.5 s), as in our proposal

// Plant profiles: each one has its own fan set-point in degrees C (from Section 6 of the proposal)
const char *PROFILE_NAMES[3] = {"Seedlings", "Herbs", "Cactus"};  // The names of the 3 plant profiles
const float PROFILE_SETPOINTS[3] = {24.0, 26.0, 30.0};             // Fan set-point for each profile, same order

// ======================= CLIMATE CONTROL VARIABLES =======================

int   currentProfile = 1;            // Which profile is active: 0 = Seedlings, 1 = Herbs, 2 = Cactus (start on Herbs)
float setPoint       = 26.0;         // The temperature (C) where the fan turns ON
float temperatureC   = 0.0;          // The latest temperature reading in degrees C
bool  fanOn          = false;        // Remembers if the fan is ON (true) or OFF (false)
bool  sensorError    = false;        // true if the sensor reading looks broken (unplugged cable)
unsigned long lastReadTime = 0;      // When we last read the sensor, so we know when 500 ms has passed

rgb_lcd lcd;                         // Creates the LCD object (only used here to test this module on its own)

// ======================= CLIMATE CONTROL FUNCTIONS =======================
// These are the parts the group will copy into the final sketch.

// Reads the sensor and turns the raw number (0-1023) into degrees C. Uses the formula from Lab 3.
float readTemperatureC() {
  int raw = analogRead(TEMP_PIN);    // Read the sensor: a number from 0 to 1023
  if (raw <= 0 || raw >= 1023) {     // 0 or 1023 means the cable is loose or unplugged
    sensorError = true;              // Remember that something is wrong
    return temperatureC;             // Keep the last good temperature instead of a nonsense number
  }                                  // End of the error check
  sensorError = false;               // The reading looks fine
  float resistance = (1023.0 - raw) * 10000.0 / raw;   // Work out the sensor's resistance from the reading
  float kelvin = 1.0 / (log(resistance / 10000.0) / B_VALUE + 1.0 / 298.15);  // Thermistor formula gives Kelvin
  return kelvin - 273.15;            // Convert Kelvin to Celsius and send it back
}                                    // End of readTemperatureC()

// Turns the fan on or off by switching the relay, and remembers the new state.
void setFan(bool on) {
  fanOn = on;                        // Remember the new fan state
  digitalWrite(RELAY_PIN, on ? HIGH : LOW);  // HIGH switches the relay ON, LOW switches it OFF
}                                    // End of setFan()

// Changes the plant profile (0, 1 or 2) and loads that profile's set-point.
void setProfile(int profile) {
  if (profile < 0 || profile > 2) return;  // Ignore numbers that aren't a real profile
  currentProfile = profile;          // Remember which profile is now active
  setPoint = PROFILE_SETPOINTS[profile];   // Load that profile's fan set-point
}                                    // End of setProfile()

// Run ONCE in setup(): gets the climate hardware ready.
void climateSetup() {
  pinMode(RELAY_PIN, OUTPUT);        // The relay is an OUTPUT because we switch it
  setFan(false);                     // Make sure the fan starts OFF
  analogReadResolution(10);          // Make analogRead give 0-1023 on the UNO R4 WiFi (same as Lab 3)
  setProfile(currentProfile);        // Load the starting profile's set-point
  temperatureC = readTemperatureC(); // Take a first reading so we don't start at 0 C
}                                    // End of climateSetup()

// Run EVERY loop: reads the temperature every 500 ms and decides if the fan should be on or off.
void climateUpdate() {
  unsigned long now = millis();      // Get the current time in milliseconds
  if (now - lastReadTime < READ_INTERVAL) return;  // Less than 500 ms since the last reading? Do nothing yet
  lastReadTime = now;                // Remember that we are reading now

  temperatureC = readTemperatureC(); // Get the newest temperature

  if (sensorError) {                 // If the sensor is broken...
    setFan(false);                   // ...turn the fan off to be safe
    return;                          // ...and skip the decision below
  }                                  // End of sensor error handling

  if (!fanOn && temperatureC >= setPoint) {               // Fan is OFF and it's at or above the set-point...
    setFan(true);                                         // ...too hot, so turn the fan ON
  }                                                       // End of "turn on" check
  else if (fanOn && temperatureC <= setPoint - DEAD_BAND) {  // Fan is ON and it has cooled 2 C below the set-point...
    setFan(false);                                        // ...cool enough, so turn the fan OFF
  }                                                       // End of "turn off" check
  // Between those two temperatures nothing changes: this gap is the dead band that stops flickering
}                                    // End of climateUpdate()

// Simple "getter" functions so teammates' modules can use my readings without touching my variables
float getTemperatureC() { return temperatureC; }   // Ventilation and Alarm modules need the temperature
float getSetPoint()     { return setPoint; }       // Ventilation and Alarm modules need the set-point
bool  isFanOn()         { return fanOn; }          // Display and logging modules show if the fan is on
bool  isSensorError()   { return sensorError; }    // Display and alarm modules can warn about a broken sensor

// ======================= TESTING ONLY (not needed in the final sketch) =======================

unsigned long lastDisplayTime = 0;   // When we last updated the LCD and Serial Monitor

void setup() {                       // setup() runs ONE time when the Arduino turns on
  Serial.begin(9600);                // Start the Serial Monitor at 9600 baud so we can see readings on the PC
  lcd.begin(16, 2);                  // Start the LCD: 16 characters wide, 2 rows tall
  climateSetup();                    // Get the climate control part ready
  Serial.println("Type 0, 1 or 2 to change profile (Seedlings, Herbs, Cactus)");  // Instructions for testing
}                                    // End of setup()

void loop() {                        // loop() runs over and over forever
  climateUpdate();                   // Do the climate control job (reads temperature, switches the fan)

  if (Serial.available() > 0) {      // If someone typed something in the Serial Monitor...
    char c = Serial.read();          // ...read one character
    if (c >= '0' && c <= '2') {      // If it's 0, 1 or 2...
      setProfile(c - '0');           // ...turn the character into a number and change profile
    }                                // End of profile check
  }                                  // End of Serial input check

  if (millis() - lastDisplayTime >= 500) {  // Every 500 ms, update the screen and Serial Monitor
    lastDisplayTime = millis();      // Remember when we updated

    if (isSensorError()) {           // If the sensor is broken, show an error
      lcd.setRGB(255, 0, 0);         // Backlight RED
      lcd.setCursor(0, 0);           // Top row
      lcd.print("SENSOR ERROR    "); // Error message (spaces clear old letters)
      lcd.setCursor(0, 1);           // Bottom row
      lcd.print("Check A1 cable  "); // Tell the user what to check
      Serial.println("SENSOR ERROR - check A1 cable");  // Also print it to the PC
      return;                        // Skip the normal display this time
    }                                // End of error display

    lcd.setRGB(isFanOn() ? 255 : 0, isFanOn() ? 200 : 255, 0);  // Yellow backlight if the fan is on, green if off
    lcd.setCursor(0, 0);             // Top row
    lcd.print("T:");                 // Label for temperature
    lcd.print(getTemperatureC(), 1); // Temperature with 1 decimal place
    lcd.print("C Set:");             // Label for set-point
    lcd.print(getSetPoint(), 0);     // Set-point with no decimals
    lcd.print("  ");                 // Spaces clear leftover characters
    lcd.setCursor(0, 1);             // Bottom row
    lcd.print(PROFILE_NAMES[currentProfile]);  // Which plant profile is active
    lcd.print(isFanOn() ? " FAN ON    " : " FAN OFF   ");  // Fan state, with spaces to clear old letters

    Serial.print("T=");              // Print to the Serial Monitor in the format our logger will use
    Serial.print(getTemperatureC(), 1);  // Temperature with 1 decimal place
    Serial.print(",SET=");           // Set-point label
    Serial.print(getSetPoint(), 1);  // Set-point
    Serial.print(",FAN=");           // Fan label
    Serial.println(isFanOn() ? 1 : 0);   // 1 = fan on, 0 = fan off, then a new line
  }                                  // End of display update
}                                    // End of loop(); it starts again from the top
