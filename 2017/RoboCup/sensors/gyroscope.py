"""
BNO055 driver
"""

from Adafruit_BNO055 import BNO055
import time


class Gyroscope:
    ErrorPosition = 45

    def __init__(self, port='/dev/ttyS0', pin=18):
        self.Heading = 0
        self.Roll = 0
        self.Pitch = 0
        self.Rotation = 0
        self.PreviousHeading = 0

        self.sensor = BNO055.BNO055(serial_port=port, rst=pin)
        time.sleep(0.1)
        try:
            self.sensor.begin()
        except:
            print("self.sensor.begin() error")

    def updateEuler(self):
        self.Heading, self.Roll, self.Pitch = self.sensor.read_euler()

    def getHeading(self):
        self.PreviousHeading = self.Heading
        self.updateEuler()

        if self.PreviousHeading - self.Heading > 200:
            self.Rotation += 1
        elif self.PreviousHeading - self.Heading < -200:
            self.Rotation -= 1

        return self.Heading + self.Rotation * 360

    def getRoll(self):
        self.updateEuler()
        return self.Roll

    def getPitch(self):
        self.updateEuler()
        return self.Pitch

    def getOrientation(self):
        self.updateEuler()
        if self.Heading < self.ErrorPosition or self.Heading > 360 - self.ErrorPosition:
            return 0
        elif 90 + self.ErrorPosition > self.Heading > 90 - self.ErrorPosition:
            return 1
        elif 180 + self.ErrorPosition > self.Heading > 180 - self.ErrorPosition:
            return 2
        elif 270 + self.ErrorPosition > self.Heading > 270 - self.ErrorPosition:
            return 3
        else:
            return -1

    def isCalibrated(self):
        calibrationStatus = self.sensor.get_calibration_status()
        calibrated = [3, 3, 3, 3]
        if calibrationStatus == calibrated:
            return True
        else:
            return False
