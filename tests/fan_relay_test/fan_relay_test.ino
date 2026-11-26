// fan_relay_test.ino
// Fan relay test: checks the relay and fan wiring without the rest of the greenhouse code.
// The relay switches ON for 3 seconds, then OFF for 3 seconds, over and over.
// Pressing the button switches to manual mode: each press flips the fan on or off.
// The Serial Monitor (9600 baud) prints every change.
//
// Relay: D7 | Button: D2 (same pins as the main sketch)
//
// What you should see when the relay is ON:
//   - the small LED on the relay board lights up and the relay clicks
//   - the fan spins
// LED and click but no fan?  -> check the fan's + wire goes through the two screw terminals,
//                               and that the fan's power supply is on
// No LED and no click?       -> check the Grove cable is in D7

const int RELAY_PIN  = 7;
const int BUTTON_PIN = D2;
const unsigned long AUTO_INTERVAL_MS = 3000;   // 3 s on, 3 s off

bool fanOn = false;
bool manualMode = false;
bool lastButton = LOW;
unsigned long lastToggle = 0;


void setFan(bool on) {
  fanOn = on;
  digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  Serial.println(on ? "Relay ON  -> fan should spin" : "Relay OFF -> fan should stop");
}


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000) { }

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  setFan(false);

  Serial.println("Fan relay test starting. Auto mode: 3 s on, 3 s off.");
  Serial.println("Press the button to switch to manual mode.");
}


void loop() {
  // Button: each press flips the fan (and switches to manual mode the first time)
  bool button = digitalRead(BUTTON_PIN);

  // Prints every change on D2, so you can see whether the board notices the button at all
  if (button != lastButton) {
    Serial.print("Button D2 = ");
    Serial.println(button == HIGH ? "HIGH (pressed)" : "LOW (released)");
  }

  if (button == HIGH && lastButton == LOW) {
    if (!manualMode) {
      manualMode = true;
      Serial.println("Manual mode: each button press flips the fan.");
    }
    setFan(!fanOn);
    delay(50);   // simple debounce
  }
  lastButton = button;

  // Auto mode: flip every AUTO_INTERVAL_MS
  if (!manualMode && millis() - lastToggle >= AUTO_INTERVAL_MS) {
    lastToggle = millis();
    setFan(!fanOn);
  }
}
