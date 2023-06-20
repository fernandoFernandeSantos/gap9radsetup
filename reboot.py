#!/usr/bin/python3


import gpiozero
import time

RELAY_PIN = 4
SLEEP_DELAY = 0.5

# Triggered by the output pin going high: active_high=True
# Initially off: initial_value=False

relay = gpiozero.OutputDevice(RELAY_PIN, active_high=False, initial_value=False)
value_before = relay.value

relay.on()  # switch on
time.sleep(SLEEP_DELAY)
relay.off()  # switch off

value_after = relay.value

print(f"PIN FROM {value_before} TO {value_after}")
