"""
Serial Class
"""

import serial
import io


class ArduinoSerial:
    def __init__(self, port, baud):
        self.port = port
        self.baud = baud
        self.ser = serial.Serial(port, baud)
        self.sio = io.TextIOWrapper(io.BufferedIOBase(self.ser, self.ser))

    def print(self, string):
        self.sio.write(string)

    def read(self):
        return self.sio.readline()
