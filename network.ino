// network.ino
// Joins the hotspot and serves one data line per second on port 8080.

#include <WiFiS3.h>
#include "settings.h"

const char* SSID = "YOUR_HOTSPOT_NAME";   // case sensitive, no trailing space
const char* PASS = "YOUR_PASSWORD";       // case sensitive
const int   NET_PORT = 8080;

WiFiServer netServer(NET_PORT);
WiFiClient netClient;

unsigned long netLastSend = 0;
const unsigned long NET_SEND_INTERVAL = 1000;


void netSetup() {
  WiFi.config(IPAddress(172, 20, 10, 9));   // fixed address, so the logger always finds it
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

  netServer.begin();
  Serial.print("Listening on port ");
  Serial.println(NET_PORT);
}


void netUpdate() {
  // pick up a logger if one is waiting
  if (!netClient || !netClient.connected()) {
    netClient = netServer.available();
    if (netClient) Serial.println("Logger connected.");
    return;
  }

  // send one line per second, without blocking
  unsigned long now = millis();
  if (now - netLastSend < NET_SEND_INTERVAL) return;
  netLastSend = now;

  netClient.print("D,");
  netClient.print(now);
  netClient.print(",");
  netClient.print(sysStatus.tempVal, 1);
  netClient.print(",");
  netClient.println(sysStatus.lightVal);
}
