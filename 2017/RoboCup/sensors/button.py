"""
Button Class
"""

import RPi.GPIO as GPIO


class Button:
    def __init__(self, buttonPin):
        self.buttonPin = buttonPin
        GPIO.setup(self.buttonPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

    def pressed(self):
        return not GPIO.input(self.buttonPin)
