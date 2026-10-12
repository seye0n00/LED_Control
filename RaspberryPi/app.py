from flask import Flask, render_template, request, jsonify

from mqtt_pub import MqttPublisher
from mqtt_status import MqttStatusMonitor
from config import MQTT_BROKER, MQTT_PORT, MQTT_PUB_TOPIC
from control import handle_led_request, get_current_location

import json
import time

app = Flask(__name__)

# MQTT publisher instance
mqtt_pub = MqttPublisher()
mqtt_pub.connect()

mqtt_status = MqttStatusMonitor()
mqtt_status.start()

time.sleep(1)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/led/state")
def led_state():
    return jsonify({
        "active_location": get_current_location()
    })

@app.route("/led/<location>/<state>")
def led_control(location, state):

    # ---- validation ----
    if not location or len(location) < 2:
        return jsonify({"result": "invalid location"}), 400

    if state not in ("ON", "OFF"):
        return jsonify({"result": "invalid state"}), 400

    # A0, B1 등의 segment 번호 검사
    try:
        int(location[1:])
    except ValueError:
        return jsonify({"result": "invalid location"}), 400


    # 중앙제어 로직에 요청 전달
    # 현재 A0 ON 상태에서 B1 ON 요청
    # -> [("A0", "OFF"), ("B1", "ON")]
    commands = handle_led_request(location, state)

    success = True

    # 중앙제어 로직이 결정한 명령들을 순서대로 전송
    for target_location, target_state in commands:

        Board_ID = target_location[0]
        seg = int(target_location[1:])

        topic = f"{MQTT_PUB_TOPIC}/{Board_ID}/cmd"
        msg = f"{seg},{target_state}"

        publish_success = mqtt_pub.publish(topic, msg)

        print(
            f"[LED CMD] location={target_location}, "
            f"state={target_state}, "
            f"board={Board_ID}, "
            f"seg={seg}, "
            f"topic={topic}, "
            f"msg={msg}, "
            f"publish={'OK' if publish_success else 'FAIL'}"
        )

        # 하나라도 publish 실패하면 전체 요청 실패
        if not publish_success:
            success = False


    return jsonify({
        "result": "ok" if success else "fail",
        "location": location,
        "state": state
    })

@app.route("/status")
def get_system_status():

    nodes = mqtt_status.get_status()

    # All NodeMCUs are online
    if all(status is True for status in nodes.values()):
        system_status = "normal"

    # At least one NodeMCU is offline
    elif any(status is False for status in nodes.values()):
        system_status = "warning"

    # Status has not been received from all nodes yet
    else:
        system_status = "checking"

    return jsonify({
        "system": system_status,
        "nodes": nodes
    })


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
