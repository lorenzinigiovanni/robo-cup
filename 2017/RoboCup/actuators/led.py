"""
Led Class
"""

import pigpio


class Led:
    def __init__(self, gpio, ledPin):
        self.gpio = gpio
        self.ledPin = ledPin
        self.gpio.set_mode(ledPin, pigpio.OUTPUT)

    def on(self):
        self.gpio.write(self.ledPin, 1)

    def off(self):
        self.gpio.write(self.ledPin, 0)
