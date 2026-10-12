import paho.mqtt.client as mqtt
import threading

from config import MQTT_BROKER, MQTT_PORT, MQTT_PUB_TOPIC


class MqttStatusMonitor:
    def __init__(self):
        self.client = mqtt.Client()

        # Current status of each NodeMCU
        # None  : status not received yet
        # True  : online
        # False : offline
        self.node_status = {
            "A": None,
            "B": None,
            "C": None
        }

        self.lock = threading.Lock()

        self.client.on_connect = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_message = self.on_message


    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print("[MQTT STATUS] Connected to broker")

            topic = f"{MQTT_PUB_TOPIC}/+/status"
            client.subscribe(topic)

            print(f"[MQTT STATUS] Subscribed to {topic}")

        else:
            print(f"[MQTT STATUS] Connection failed (rc={rc})")


    def on_disconnect(self, client, userdata, rc):
        print("[MQTT STATUS] Disconnected from broker")


    def on_message(self, client, userdata, msg):
        topic = msg.topic
        payload = msg.payload.decode().strip()

        # node/A/status
        parts = topic.split("/")

        if len(parts) != 3:
            return

        node_id = parts[1]

        if node_id not in self.node_status:
            return

        if payload not in ("online", "offline"):
            return

        new_status = (payload == "online")

        with self.lock:
            old_status = self.node_status[node_id]

            # Print only when the status changes
            if old_status != new_status:
                self.node_status[node_id] = new_status

                print(f"[NODE {node_id}] {payload}")

                self.print_system_status()


    def print_system_status(self):
        if all(status is True for status in self.node_status.values()):
            print("[SYSTEM] All nodes are online")

        else:
            offline_nodes = [
                node
                for node, status in self.node_status.items()
                if status is False
            ]

            unknown_nodes = [
                node
                for node, status in self.node_status.items()
                if status is None
            ]

            if offline_nodes:
                print(
                    "[SYSTEM] Some nodes are offline: "
                    f"{', '.join(offline_nodes)}"
                )

            if unknown_nodes:
                print(
                    "[SYSTEM] Waiting for status: "
                    f"{', '.join(unknown_nodes)}"
                )


    def get_status(self):
        # This function will later be used by the Flask /status API
        with self.lock:
            return self.node_status.copy()


    def start(self):
        self.client.connect(MQTT_BROKER, MQTT_PORT, 60)
        self.client.loop_start()


    def stop(self):
        self.client.loop_stop()
        self.client.disconnect()


if __name__ == "__main__":
    import time

    monitor = MqttStatusMonitor()
    monitor.start()

    try:
        while True:
            time.sleep(1)

    except KeyboardInterrupt:
        monitor.stop()
