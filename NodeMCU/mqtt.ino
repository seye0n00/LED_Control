#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "log.h"
#include "config.h"
#include "device.h"

extern WiFiClient espClient;

const char* mqtt_server = MQTT_SERVER;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;

  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  LOG_I("MQTT", String("RX [") + topic + "] " + msg);

  int comma = msg.indexOf(',');

  if (comma == -1) {
    LOG_E("MQTT", "Invalid message format");
    return;
  }

  String seg_str = msg.substring(0, comma);
  String state_str = msg.substring(comma + 1);

  int seg = seg_str.toInt();

  if (seg < 0 || seg >= NUM_SEGMENT) {
    LOG_E("MQTT", "Invalid segment");
    return;
  }

  if (state_str == "ON") {
    set_segment((uint8_t)seg, true);
  }
  else if (state_str == "OFF") {
    set_segment((uint8_t)seg, false);
  }
  else {
    LOG_E("MQTT", "Invalid state");
  }
}

void mqtt_reconnect() {
  LOG_I("MQTT", "Trying to connect");

  while (!client.connected()) {

    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // node/A/status
    String statusTopic = String(MQTT_SUB_TOPIC) + Board_ID + "/status";

    // MQTT 연결 시 LWT 등록
    bool connected = client.connect(clientId.c_str(), statusTopic.c_str(), 0, true, "offline");

    if (connected) {

      LOG_I("MQTT", "Connected");

      // 정상 연결 상태 publish
      // retain=true 이므로 broker가 현재 상태 기억
      client.publish(statusTopic.c_str(), "online", true);

      LOG_I("MQTT", String("STATUS [") + statusTopic + "] online");

      String subTopic = String(MQTT_SUB_TOPIC) + Board_ID + "/cmd";

      client.subscribe(subTopic.c_str());

      LOG_I("MQTT", String("Subscribed to ") + subTopic);

    } else {
      LOG_E("MQTT", String("Failed, rc=") + client.state());
      delay(500);
    }
  }
}

void mqtt_init() {
  client.setServer(mqtt_server, MQTT_PORT);
  client.setCallback(callback);
}

void mqtt_loop() {

  if (!client.connected()) {
    mqtt_reconnect();
  }
  client.loop();
}
