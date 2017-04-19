"""
Servo Class
"""

import RPi.GPIO as GPIO


class Servo:
    def __init__(self, servoPin):
        GPIO.setup(servoPin, GPIO.OUT)
        self.pwm = GPIO.PWM(servoPin, 50)
        self.pwm.start(7)

    def angle(self, angle):
        duty = float(angle) / 18.0 + 2.0
        self.pwm.ChangeDutyCycle(duty)

    def stop(self):
        self.pwm.stop()
