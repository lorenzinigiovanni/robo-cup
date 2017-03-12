"""
Motor Class
"""

import RPi.GPIO as GPIO


class Motor:
    def __init__(self, enable, a, b):
        self.pwm = GPIO.PWM(enable, 100)
        self.a = a
        self.b = b
        GPIO.setup(self.a, GPIO.OUT)
        GPIO.setup(self.b, GPIO.OUT)

    def forward(self):
        self.pwm.start(0)
        GPIO.output(self.a, GPIO.HIGH)
        GPIO.output(self.b, GPIO.LOW)

    def reverse(self):
        self.pwm.start(0)
        GPIO.output(self.a, GPIO.LOW)
        GPIO.output(self.b, GPIO.HIGH)

    def speed(self, speed):
        self.pwm.ChangeDutyCycle(speed)

    def stop(self):
        self.pwm.stop()
        GPIO.output(self.a, GPIO.LOW)
        GPIO.output(self.b, GPIO.LOW)
