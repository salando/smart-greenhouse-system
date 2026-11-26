// led_strip_test.ino
// WS2812B strip test: checks the strip and its wiring without the rest of the greenhouse code.
// The strip shows red, green, blue and white, then lights each LED one at a time, then turns off.
// The Serial Monitor (9600 baud) prints each step and the light sensor reading, so you can
// compare it with the dark threshold (Basil 400, Lettuce 450, Tomato 350).
//
// Strip DIN: D6 | Light sensor: A0 (same pins as the main sketch)
//
// Nothing lights up?
//   - The wire from D6 must go to the DIN end of the strip (the arrows point away from it)
//   - Strip 5V to 5V, strip GND to the Arduino's GND
//   - Adafruit NeoPixel library must be version 1.12.0 or newer for the UNO R4

#include <Adafruit_NeoPixel.h>

const int STRIP_PIN  = 6;
const int LED_COUNT  = 8;
const int SENSOR_PIN = A0;

Adafruit_NeoPixel strip(LED_COUNT, STRIP_PIN, NEO_GRB + NEO_KHZ800);


// Fills the whole strip with one colour and holds it for a second
void showColor(const char* name, uint8_t r, uint8_t g, uint8_t b) {
  Serial.print(name);
  Serial.print("   | light sensor = ");
  Serial.println(analogRead(SENSOR_PIN));

  strip.fill(strip.Color(r, g, b));
  strip.show();
  delay(1000);
}


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000) { }

  strip.begin();
  strip.setBrightness(50);   // low brightness so the USB port can power it
  strip.clear();
  strip.show();

  Serial.println("LED strip test starting");
}


void loop() {
  // If the colours come out swapped (red shows as green), change NEO_GRB to NEO_RGB above
  showColor("Red  ", 255, 0, 0);
  showColor("Green", 0, 255, 0);
  showColor("Blue ", 0, 0, 255);
  showColor("White", 255, 255, 255);

  // One LED at a time, to find a dead LED or a break in the strip
  Serial.println("Chase: LED 1 to 8");
  for (int i = 0; i < LED_COUNT; i++) {
    strip.clear();
    strip.setPixelColor(i, strip.Color(255, 255, 255));
    strip.show();
    delay(300);
  }

  Serial.println("Off");
  strip.clear();
  strip.show();
  delay(1000);
}
