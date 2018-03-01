"""
TCS34725 driver
"""


class Color:
    def __init__(self, serial):
        self.serial = serial

    def getColors(self):
        self.serial.print('color,')
        s = self.serial.read().split(',')
        if s[0] == 'color':
            return s[1]
        return -1

    def isBlack(self):
        if self.getColors() == 0:
            return True
        return False

    def isWhite(self):
        if self.getColors() == 1:
            return True
        return False

    def isSilver(self):
        if self.getColors() == 2:
            return True
        return False
