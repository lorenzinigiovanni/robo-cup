"""
VL53L0X driver
"""

import smbus
from sensors import VL53L0X


class Distance:
    SensorAddress = 0x29

    def __init__(self, number):
        self.number = number
        self.i2c = smbus.SMBus(1)
        self.i2c.write_byte(0x70, 1 << self.number)
        self.sensor = VL53L0X.VL53L0X(self.SensorAddress)
        self.sensor.start_ranging(VL53L0X.VL53L0X_BETTER_ACCURACY_MODE)

    def distance(self):
        self.i2c.write_byte(0x70, 1 << self.number)
        distance = self.sensor.get_distance()
        if 0 < distance < 1000:
            return distance
        return -1
