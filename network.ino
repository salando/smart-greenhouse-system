// network.ino
// Joins the hotspot and serves one data line per second on port 8080.

#include <WiFiS3.h>
#include "settings.h"

const char* SSID = "greenhouse";          // case sensitive, no trailing space
const char* PASS = "greenhouse2026";      // case sensitive
const int   NET_PORT = 8080;

WiFiServer netServer(NET_PORT);
WiFiClient netClient;

unsigned long netLastSend = 0;
const unsigned long NET_SEND_INTERVAL = 1000;
const unsigned long NET_JOIN_TIMEOUT  = 15000;   // stop waiting for the hotspot after 15 s
bool netReady = false;                            // true once the board has joined the hotspot


void netSetup() {
  WiFi.config(IPAddress(172, 20, 10, 9));  
  WiFi.begin(SSID, PASS);

  Serial.print("Joining ");
  Serial.println(SSID);

  // Give up after NET_JOIN_TIMEOUT so the greenhouse still runs without WiFi
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start >= NET_JOIN_TIMEOUT) {
      Serial.println();
      Serial.println("No WiFi. Running without the logger.");
      return;
    }
    delay(500);
    Serial.print(".");
  }

  netReady = true;
  Serial.println();
  Serial.print("Board IP: ");
  Serial.println(WiFi.localIP());

  netServer.begin();
  Serial.print("Listening on port ");
  Serial.println(NET_PORT);
}


void netUpdate() {
  if (!netReady) return;   // no WiFi, nothing to send

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

  // D,ms,tempC,light,setTemp,fan,vent,led,alarm
  netClient.print("D,");
  netClient.print(now);
  netClient.print(",");
  netClient.print(sysStatus.tempVal, 1);
  netClient.print(",");
  netClient.print(sysStatus.lightVal);
  netClient.print(",");
  netClient.print(settings.setTemp, 1);
  netClient.print(",");
  netClient.print(sysStatus.fanOn   ? 1 : 0);
  netClient.print(",");
  netClient.print(sysStatus.ventWarning ? 1 : 0);
  netClient.print(",");
  netClient.print(sysStatus.lightOn ? 1 : 0);
  netClient.print(",");
  netClient.println(sysStatus.alarmOn ? 1 : 0);   // println ends the line
}
