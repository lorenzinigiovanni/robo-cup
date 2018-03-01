"""
Servo Class
"""


class Servo:
    def __init__(self, serial):
        self.serial = serial

    def release(self, n):
        self.serial.print('kit,' + str(n) + ',')
