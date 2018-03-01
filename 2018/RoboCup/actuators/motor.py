"""
Motor Class
"""


class Motor:
    def __init__(self, serial):
        self.serial = serial

    def forward(self):
        self.serial.print('motor,fw,')
        s = self.serial.read().split(',')
        if s[0] == 'motor' and s[1] == 'fw':
            return s[2]
        return -1

    def reverse(self):
        self.serial.print('motor,rw,')
        s = self.serial.read().split(',')
        if s[0] == 'motor' and s[1] == 'rw':
            return s[2]
        return -1

    def right(self):
        self.serial.print('motor,dx,')
        s = self.serial.read().split(',')
        if s[0] == 'motor' and s[1] == 'dx':
            return s[2]
        return -1

    def left(self):
        self.serial.print('motor,sx,')
        s = self.serial.read().split(',')
        if s[0] == 'motor' and s[1] == 'sx':
            return s[2]
        return -1

    def reset(self):
        self.serial.print('motor,reset,')
        s = self.serial.read().split(',')
        if s[0] == 'motor' and s[1] == 'reset':
            return s[2]
        return -1
