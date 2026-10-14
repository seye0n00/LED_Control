#include <ESP8266WiFi.h>
#include "log.h"
#include "device.h"
#include "neopixel_controller.h"
#include "wifi.h"
#include "mqtt.h"

WiFiClient espClient;

void setup() {
  Serial.begin(115200);
  LOG_I("SYSTEM", "Boot start");

  setup_wifi();

  neopixel_init();
  mqtt_init();

  LOG_I("SYSTEM", "Boot complete");
}

void loop() {
  mqtt_loop();
}
