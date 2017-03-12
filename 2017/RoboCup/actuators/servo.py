"""
Servo Class
"""

import RPi.GPIO as GPIO


class Servo:
    def __init__(self, servoPin):
        self.pwm = GPIO.PWM(servoPin, 100)
        self.pwm.start(5)

    def angle(self, angle):
        duty = float(angle) / 10.0 + 2.5
        self.pwm.ChangeDutyCycle(duty)
