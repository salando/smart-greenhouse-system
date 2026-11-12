#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;


// Pins
const int BUTTON_PIN = D2;
const int ROTARY_ANGLE_SENSOR = A2;
const unsigned long BUTTON_HOLD_MS = 700;

// Menu Items and Plant Profiles
const char* MENU_ITEMS[] = {"Profile", "Set Temp", "Light Level", "Back"};
const int MENU_COUNT = 4;


// Variable States
unsigned long lastDraw = 0;
int screen = 0;                     // 0 Home, 1 Menu, 2 Pick profile, 3 Set temp, 4 Set light
int menuIndex = 0;
int currentProfile = 0;             // Current Profile Selection
double setTemp = PROFILES[0].temp;
int darkThreshold = PROFILES[0].light;
double editValue = 0;               // Shows the number when editing with the Rotary Angle Sensor

void displaySetup() {
  pinMode(BUTTON_PIN, INPUT);
  lcd.begin(16, 2);
  lcd.setRGB(255, 255, 255); // White
  lcd.print("Welcome");
}


// Updates the display interal status and logic for setting variable values
void displayUpdate() {
  int press = menuButtonEvent();

  if (screen == 0) {
    if (press != 0) {
      screen = 1;
      menuIndex = 0;
    }
  }
  else if (screen == 1) {
    if (press == 1) menuIndex = (menuIndex + 1) % MENU_COUNT;
    if (press == 2) {                         // Long Press
      if (menuIndex == 0) {
        screen = 2;
        menuIndex = 0;
      }
      else if (menuIndex == 1) screen = 3;    // Set Temperature Screen
      else if (menuIndex == 2) screen = 4;    // Set Light Screen
      else screen = 0;                         // Home Screen
    }
  }
  else if (screen == 2) {
    if (press == 1) menuIndex = (menuIndex + 1) % PROFILE_COUNT;
    if (press == 2) {
      currentProfile = menuIndex;
      setTemp = PROFILES[menuIndex].temp;
      darkThreshold = PROFILES[menuIndex].light;
      screen = 1;
      menuIndex = 0;
    }
  }
  else if (screen == 3) {
    editValue = map(analogRead(ROTARY_ANGLE_SENSOR), 0, 1022, 70, 30) / 2.0; // Showing 15-35 degrees with 0.5 degree accuracy
    if (press == 2) {
      setTemp = editValue;
      currentProfile = -1;
      screen = 1;
      menuIndex = 1;
    }
  }
  else if (screen == 4) {
    editValue = map(analogRead(ROTARY_ANGLE_SENSOR), 0, 1022, 100, 0) * 10; // Light Threshold from 0-1000 with 10 increment
    if (press == 2) {
      darkThreshold = editValue;
      currentProfile = -1;
      screen = 1;
      menuIndex = 2;
    }
  }
  if (millis() - lastDraw >= 250) {
    lastDraw = millis();
    displayDraw();
  }
}

void displayDraw() {
  lcd.setCursor(0, 0);
  if (screen == 0) {
   if (currentProfile == -1) lcd.print("Custom");
   else lcd.print(PROFILES[currentProfile].name);
   lcd.print("          ");                            // Removes previous letters
   lcd.setCursor(0, 1);
   lcd.print(sysStatus.tempVal, 1);
   lcd.print("/");
   lcd.print(setTemp, 1);
   lcd.print(" C     ");
  }
  else if (screen == 1) {                              // Menu Screen
   lcd.print("MENU            ");
   lcd.setCursor(0, 1);
   lcd.print("> ");
   lcd.print(MENU_ITEMS[menuIndex]);
   lcd.print("          ");
  }
  else if (screen == 2) {                              // Pick Profile Screen
   lcd.print("Pick plant      ");
   lcd.setCursor(0, 1);
   lcd.print("> ");
   lcd.print(PROFILES[menuIndex].name);
   lcd.print("          ");
  }
  else if (screen == 3) {                              // Set Temperature Screen
   lcd.print("Set temp:       ");
   lcd.setCursor(0, 1);
   lcd.print("> ");
   lcd.print(editValue, 1);                            // 1 decimal place
   lcd.print(" C        ");
  }
  else if (screen == 4) {                              // Set Light Screen
   lcd.print("Set light:        ");
   lcd.setCursor(0, 1);
   lcd.print("> ");
   lcd.print((int)editValue);                          // whole number
   lcd.print("          ");
  }

}

// Returns: 0 = no press, 1 = short press, 2 = long press
int menuButtonEvent() {
  static bool lastButton = LOW;
  static unsigned long lastButtonChange = 0;
  static unsigned long pressStart;
  static bool holdReported = false;
  
  bool now = digitalRead(BUTTON_PIN);
  int result = 0;

  if (now != lastButton && millis() - lastButtonChange > 40) {
    lastButtonChange = millis();
    lastButton = now;
    if (now == HIGH) {
      pressStart = millis();
      holdReported = false;
    } else {
      if (!holdReported) result = 1;
    }
  }
  if (lastButton == HIGH && !holdReported && millis() - pressStart >= BUTTON_HOLD_MS) {
    result = 2;
    holdReported = true;
  }
  return result;
}