"""
Button Class
"""

import pigpio


class Button:
    def __init__(self, gpio, buttonPin):
        self.gpio = gpio
        self.buttonPin = buttonPin
        self.gpio.set_mode(buttonPin, pigpio.INPUT)
        self.gpio.set_pull_up_down(buttonPin, pigpio.PUD_UP)

    def pressed(self):
        return not bool(self.gpio.read(self.buttonPin))
