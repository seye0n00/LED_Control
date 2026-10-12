current_location = None


def handle_led_request(location, state):
    global current_location

    commands = []

    if state == "ON":

        if current_location is not None and current_location != location:
            commands.append((current_location, "OFF"))

        commands.append((location, "ON"))
        current_location = location

    elif state == "OFF":

        if current_location == location:
            commands.append((location, "OFF"))
            current_location = None

    return commands

def get_current_location():
    return current_location