#include <ESP8266WiFi.h>
#include "log.h"
#include "config.h"

void setup_wifi() {
  LOG_I("WIFI", "Connecting to WiFi...");
  
  // WiFi.mode(WIFI_STA);

  WiFi.setAutoReconnect(true);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  LOG_I("WIFI", "Connected");
  LOG_I("WIFI", WiFi.localIP().toString());

}