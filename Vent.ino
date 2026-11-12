
#include <Wire.h>
#include "rgb_lcd.h"
#include <math.h>
#include "settings.h"

const int TEMP_PIN = A1;
const int BUTTON_PIN = 2;
const int BUZZER_PIN = 4;
const int B_VALUE = 3975;
const int SETPOINT = 25;

const unsigned long ALARM_DELAY_MS = 60000;
const unsigned long MUTE_MS = 300000;

rgb_lcd lcd;

bool ventWarning = false;
bool alarmOn = false;
bool muted = false;
unsigned long outOfRangeStart = 0;
unsigned long muteStart = 0;
unsigned long lastBeep = 0;
unsigned long lastUpdate = 0;

void setup() {
    pinMode(BUTTON_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    Serial.begin(9600);
    lcd.begin(16, 2);
}

void loop() {
    unsigned long now = millis();

    int raw = analogRead(TEMP_PIN);
    float resistance = (1023.0 - raw) * 10000.0 / raw;
    float temperature = 1.0 / (log(resistance / 10000.0) / B_VALUE + 1.0 / 298.15) - 273.15;
    bool sensorError = (raw <= 0 || raw >= 1023);

    if (!sensorError && temperature >= SETPOINT + 2) ventWarning = true;
    if (!sensorError && temperature <= SETPOINT) ventWarning = false;

    bool outOfRange = (sensorError || temperature > SETPOINT + 4 || temperature < 15);
    if (!outOfRange) {
        outOfRangeStart = now;
        alarmOn = false;
        muted = false;
    } else if (now - outOfRangeStart >= ALARM_DELAY_MS) {
        alarmOn = true;
    }

    if (alarmOn && digitalRead(BUTTON_PIN) == HIGH) {
        muted = true;
        muteStart = now;
    }
    if (muted && now - muteStart >= MUTE_MS) muted = false;

    if (alarmOn && !muted && now - lastBeep >= 500) {
        tone(BUZZER_PIN, 1000, 250);
        lastBeep = now;
    }

    if (now - lastUpdate >= 250) {
        lastUpdate = now;
        if (alarmOn) lcd.setRGB(255, 0, 0);
        else if (ventWarning) lcd.setRGB(255, 150, 0);
        else lcd.setRGB(0, 255, 0);

        lcd.setCursor(0, 0);
        if (sensorError) {
            lcd.print("SENSOR ERROR    ");
        } else {
            lcd.print("Temp: ");
            lcd.print(temperature, 1);
            lcd.print(" C    ");
        }
        lcd.setCursor(0, 1);
        if (alarmOn) lcd.print(muted ? "ALARM - MUTED   " : "ALARM! Press btn");
        else lcd.print(ventWarning ? "OPEN VENT!      " : "Vent: OK        ");

        Serial.println(temperature, 1);
    }
}
