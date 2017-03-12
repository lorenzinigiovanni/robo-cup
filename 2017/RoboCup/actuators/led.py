"""
Led Class
"""

import RPi.GPIO as GPIO


class Led:
    def __init__(self, ledPin):
        self.ledPin = ledPin
        GPIO.setup(self.ledPin, GPIO.OUT)

    def on(self):
        GPIO.output(self.ledPin, GPIO.HIGH)

    def off(self):
        GPIO.output(self.ledPin, GPIO.LOW)
