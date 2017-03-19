"""
Motor Class
"""

import RPi.GPIO as GPIO


class Motor:
    def __init__(self, enable, a, b):
        GPIO.setup(enable, GPIO.OUT)
        GPIO.setup(a, GPIO.OUT)
        GPIO.setup(b, GPIO.OUT)
        self.pwm = GPIO.PWM(enable, 100)
        self.a = a
        self.b = b

    def forward(self):
        self.pwm.start(0)
        GPIO.output(self.a, GPIO.HIGH)
        GPIO.output(self.b, GPIO.LOW)

    def reverse(self):
        self.pwm.start(0)
        GPIO.output(self.a, GPIO.LOW)
        GPIO.output(self.b, GPIO.HIGH)

    def speed(self, speed):
        speed = abs(speed)
        if speed > 100:
            speed = 100
        elif speed < 0:
            speed = 0
        self.pwm.ChangeDutyCycle(speed)

    def stop(self):
        self.pwm.stop()
        GPIO.output(self.a, GPIO.LOW)
        GPIO.output(self.b, GPIO.LOW)
