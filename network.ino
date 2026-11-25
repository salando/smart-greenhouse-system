// network.ino
// Joins the hotspot in the background and serves one data line per second on port 8080.
// Nothing here waits for WiFi, so the display and controls keep running while it connects.

#include <WiFiS3.h>
#include "settings.h"

const char* SSID = "greenhouse";          // case sensitive, no trailing space
const char* PASS = "greenhouse2026";      // case sensitive
const int   NET_PORT = 8080;

WiFiServer netServer(NET_PORT);
WiFiClient netClient;

const unsigned long NET_SEND_INTERVAL  = 1000;    // one data line per second
const unsigned long NET_CHECK_INTERVAL = 500;     // how often to ask the WiFi chip for its status
const unsigned long NET_RETRY_INTERVAL = 15000;   // try joining again every 15 s while not connected

bool netReady = false;                            // true while the board is on the hotspot
unsigned long netLastSend    = 0;
unsigned long netLastCheck   = 0;
unsigned long netLastAttempt = 0;


// Starts a join attempt and returns straight away. The WiFi chip connects on its own.
void netStartJoin() {
  Serial.print("Joining ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASS);
  netLastAttempt = millis();
}


void netSetup() {
  WiFi.setTimeout(0);                        // make WiFi.begin() return immediately instead of waiting
  WiFi.config(IPAddress(172, 20, 10, 9));    // fixed address, so the logger always finds it
  netStartJoin();
}


void netUpdate() {
  unsigned long now = millis();
  if (now - netLastCheck < NET_CHECK_INTERVAL) return;
  netLastCheck = now;

  // Not connected: retry every NET_RETRY_INTERVAL
  if (WiFi.status() != WL_CONNECTED) {
    if (netReady) {
      Serial.println("WiFi lost. Retrying in the background.");
      netClient.stop();
      netReady = false;
    }
    if (now - netLastAttempt >= NET_RETRY_INTERVAL) netStartJoin();
    return;
  }

  // Just connected
  if (!netReady) {
    netReady = true;
    Serial.print("WiFi connected. Board IP: ");
    Serial.println(WiFi.localIP());
    netServer.begin();                       // only opens the port the first time
    Serial.print("Listening on port ");
    Serial.println(NET_PORT);
  }

  // Pick up a logger if one is waiting
  if (!netClient || !netClient.connected()) {
    netClient = netServer.available();
    if (netClient) Serial.println("Logger connected.");
    return;
  }

  if (now - netLastSend < NET_SEND_INTERVAL) return;
  netLastSend = now;

  // D,ms,tempC,light,setTemp,fan,vent,led,alarm
  // Built as one line and sent in one go: every print() is a separate trip to the WiFi chip,
  // and each trip can stall for seconds if the connection is bad.
  String line = "D,";
  line += String(now) + ",";
  line += String(sysStatus.tempVal, 1) + ",";
  line += String(sysStatus.lightVal) + ",";
  line += String(settings.setTemp, 1) + ",";
  line += String(sysStatus.fanOn ? 1 : 0) + ",";
  line += String(sysStatus.ventWarning ? 1 : 0) + ",";
  line += String(sysStatus.lightOn ? 1 : 0) + ",";
  line += String(sysStatus.alarmOn ? 1 : 0);
  netClient.println(line);
}
