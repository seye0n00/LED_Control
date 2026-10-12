import paho.mqtt.client as mqtt
from config import MQTT_BROKER, MQTT_PORT, MQTT_PUB_TOPIC
import time

class MqttPublisher:
    # Create an MQTT client instance
    def __init__(self):
        self.client = mqtt.Client()
        self.connected = False

        self.client.on_connect = self.on_connect
        self.client.on_disconnect = self.on_disconnect

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            self.connected = True
            print("[MQTT] Connected to broker")
        else:
            print(f"[MQTT] Connection failed (rc={rc})")

    def on_disconnect(self, client, userdata, rc):
        self.connected = False
        print("[MQTT] Disconnected")

    def connect(self):
        try:
            # Connect to the MQTT broker
            #self.client.connect(BROKER, PORT, 60)
            self.client.connect(MQTT_BROKER, MQTT_PORT, 60)
            self.client.loop_start()
            time.sleep(0.5)
        except Exception as e:
            print(f"[MQTT] Connect exception: {e}")

    # Publish a message to the configured MQTT topic.
    def publish(self, topic, message):
        if not self.connected:
            print("[MQTT] Publish failed: not connected")
            return False

        result = self.client.publish(topic, message)

        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            print(f"[MQTT] Publish success: {topic}/{message}")
            return True
        else:
            print(f"[MQTT] Publish failed (rc={result.rc})")
            return False