"""
Button Class
"""

import RPi.GPIO as GPIO


class Button:
    def __init__(self, buttonPin, callbackFunction):
        GPIO.setup(buttonPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.add_event_detect(buttonPin, GPIO.FALLING, callback=callbackFunction, bouncetime=300)
