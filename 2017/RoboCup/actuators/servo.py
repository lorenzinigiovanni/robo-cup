"""
Servo Class
"""

import pigpio


class Servo:
    def __init__(self, gpio, servoPin):
        self.gpio = gpio
        self.servoPin = servoPin
        self.gpio.set_mode(servoPin, pigpio.OUTPUT)
        self.gpio.set_servo_pulsewidth(servoPin, 1500)

    def angle(self, angle):
        angle *= 11
        angle += 500
        self.gpio.set_servo_pulsewidth(self.servoPin, angle)

    def stop(self):
        self.gpio.set_servo_pulsewidth(self.servoPin, 0)
