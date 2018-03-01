"""
VL53L0X driver
"""


class Distance:
    def __init__(self, serial, id):
        self.serial = serial
        self.id = id

    def getState(self):
        self.serial.print('dist,' + str(self.id) + ',')
        s = self.serial.read().split(',')
        if s[0] == 'dist' and s[1] == self.id:
            return s[2]
        return -1
