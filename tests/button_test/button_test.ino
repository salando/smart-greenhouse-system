// button_test.ino
// Button test: checks the Grove button on D2 without the rest of the greenhouse code.
// - The board's built-in LED (the small "L" LED next to pin 13) lights while the button is held,
//   so you can test it without the Serial Monitor.
// - The Serial Monitor (9600 baud) prints the D2 reading 4 times a second.
//
// Button: D2 (same pin as the main sketch)
//
// What you should see:
//   not pressed -> D2 = 0, L LED off
//   pressed     -> D2 = 1, L LED on
// Always 0 even when pressed -> the button signal isn't reaching D2 (cable, port, or broken button)
// Always 1                   -> wired backwards or a different type of button
// Jumping between 0 and 1    -> the pin isn't connected to anything (loose cable)

const int BUTTON_PIN = D2;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000) { }

  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Button test starting. Press and hold the button on D2.");
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);
  digitalWrite(LED_BUILTIN, reading);   // L LED copies the button

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 250) {
    lastPrint = millis();
    Serial.print("D2 = ");
    Serial.print(reading);
    Serial.println(reading == HIGH ? "   (pressed)" : "");
  }
}
