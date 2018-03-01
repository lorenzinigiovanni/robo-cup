"""
BNO055 driver
"""


class Gyroscope:
    def __init__(self, serial):
        self.serial = serial

    def getOrientation(self):
        self.serial.print('gyro,ori,')
        s = self.serial.read().split(',')
        if s[0] == 'gyro' and s[1] == 'ori':
            return s[2]
        return -1

    def getPitch(self):
        self.serial.print('gyro,pit,')
        s = self.serial.read().split(',')
        if s[0] == 'gyro' and s[1] == 'pit':
            return s[2]
        return -1
