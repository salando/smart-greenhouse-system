const int sensorPin = A0; // Light sensor on A0
const int ledPin = 3;     // Relay/LED on D3
const int threshold = 400;   // Light threshold
int totalSeconds = 0; // stores running time counter

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("System Ready.");
}

void loop() {
  
trackSunLight(totalSeconds);
delay(1000);

}

// function parameters - tSeconds
// returns nothing
void trackSunLight(int& tSeconds){

int lightVal = analogRead(sensorPin);

if (lightVal >= threshold){
  digitalWrite(ledPin, LOW); //turn off LED/Relay
  tSeconds++;
}
else{
  digitalWrite(ledPin, HIGH); //tur on LED/Relay for plants
}

int h = tSeconds / 3600; //getting hours
int m = tSeconds % 3600 / 60;  // getting minutes
int s = tSeconds % 60; //getting seconds

//printing light value and sunlight time
Serial.print("Light: ");
Serial.print(lightVal);
Serial.print(" | Sunlight Time: ");
Serial.print(h);
Serial.print("h ");
Serial.print(m);
Serial.print("m ");
Serial.print(s);
Serial.println("s");
}