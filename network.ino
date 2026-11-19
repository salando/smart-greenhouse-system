#include <WiFiS3.h>

const char* SSID = "YOUR_HOTSPOT_NAME"; // Input your hotspot name (CASE SENSITIVE)
const char* PASS = "YOUR_PASSWORD"; // Input your hotspot password (CASE SENSITIVE)
const int   PORT = 8080;

struct GreenhouseState {
  float tempC;
  int   light;
};

GreenhouseState state;

WiFiServer server(PORT);
WiFiClient client;

unsigned long lastSend = 0;
const unsigned long SEND_INTERVAL = 1000;


void netSetup() {
  WiFi.config(IPAddress(172, 20, 10, 9));
  WiFi.begin(SSID, PASS);

  Serial.print("Joining ");
  Serial.println(SSID);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Board IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.print("Listening on port ");
  Serial.println(PORT);
}


void netUpdate() {
  // pick up a logger if one is waiting
  if (!client || !client.connected()) {
    client = server.available();
    if (client) Serial.println("Logger connected.");
    return;
  }

  // send one line per second, without blocking
  unsigned long now = millis();
  if (now - lastSend < SEND_INTERVAL) return;
  lastSend = now;

  client.print("D,");
  client.print(now);
  client.print(",");
  client.print(state.tempC, 1);
  client.print(",");
  client.println(state.light);
}


void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000) { }

  netSetup();
}

void loop() {
 
  netUpdate();
}
