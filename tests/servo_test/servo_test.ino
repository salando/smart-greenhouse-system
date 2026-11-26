// servo_test.ino
// Servo calibration: turn the rotary dial to move the vent servo, and read the angle
// in the Serial Monitor (9600 baud). Use it to find the angles where the vent is
// fully closed and fully open, then put those numbers into VENT_CLOSED and VENT_OPEN
// in vent.ino.
//
// Servo: D3 | Rotary Angle Sensor: A2 (same pins as the main sketch)

#include <Servo.h>

const int SERVO_PIN = 3;
const int DIAL_PIN  = A2;

Servo testServo;
int lastAngle = -1;   // -1 so the first angle always gets printed


void setup() {
  Serial.begin(9600);
  testServo.attach(SERVO_PIN);
  Serial.println("Turn the dial to move the servo.");
}


void loop() {
  int dial  = analogRead(DIAL_PIN);        // 0 to 1023
  int angle = map(dial, 0, 1023, 0, 180);  // turn that into 0 to 180 degrees

  // Only move and print when the angle actually changes
  if (angle != lastAngle) {
    testServo.write(angle);
    Serial.print("Angle: ");
    Serial.println(angle);
    lastAngle = angle;
  }

  delay(50);   // fine here: this test does nothing else
}
