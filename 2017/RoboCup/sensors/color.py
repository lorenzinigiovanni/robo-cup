"""
TCS7472 driver
"""

import time
import smbus


class Color:
    SensorAddress = 0x29

    EnableAddress = 0xa0
    ATimeAddress = 0xa1
    ControlAddress = 0xaf
    IDAddress = 0xb2
    ColorAddress = 0xb4
    StatusAddress = 0xb3
    ClearInterrupts = 0x66

    ENABLE_AIEN = 0x10        # RGBC Interrupt Enable
    ENABLE_WEN = 0x08         # Wait enable - Writing 1 activates the wait timer
    ENABLE_AEN = 0x02         # RGBC Enable - Writing 1 actives the ADC, 0 disables it
    ENABLE_PON = 0x01         # Power on - Writing 1 activates the internal oscillator, 0 disables it

    red_color = 0
    green_color = 0
    blue_color = 0

    IntegrationTime = 0xF6
    Gain = 0x00

    def __init__(self, number):
        self.number = number
        self.i2c = smbus.SMBus(1)
        self.i2c.write_byte(0x70, 1 << self.number)
        self.i2c.write_byte_data(self.SensorAddress, self.ATimeAddress, self.IntegrationTime & 0xFF)
        self.i2c.write_byte_data(self.SensorAddress, self.ControlAddress, self.Gain & 0xFF)
        self.i2c.write_byte_data(self.SensorAddress, self.EnableAddress, 0x03 & 0xFF)
        time.sleep(0.700)

    def isSensorThere(self):
        self.i2c.write_byte(0x70, 1 << self.number)
        device = self.i2c.read_byte_data(self.SensorAddress, self.IDAddress) & 0xFF

        if device == 0x44:
            return 1
        else:
            return 0

    def readStatus(self):
        self.i2c.write_byte(0x70, 1 << self.number)
        status = self.i2c.read_byte_data(self.SensorAddress, self.StatusAddress) & 0xFF

        return status

    def getColors(self):
        self.i2c.write_byte(0x70, 1 << self.number)
        colorList = self.i2c.read_i2c_block_data(self.SensorAddress, self.ColorAddress, 8)

        c = (colorList[1] << 8) + (colorList[0])
        r = (colorList[3] << 8) + (colorList[2])
        g = (colorList[5] << 8) + (colorList[4])
        b = (colorList[7] << 8) + (colorList[6])

        self.red_color = r * 256 / c
        self.green_color = g * 256 / c
        self.blue_color = b * 256 / c

    def setIntegrationTimeAndGain(self, IT, Gain):
        self.i2c.write_byte(0x70, 1 << self.number)
        self.i2c.write_byte_data(self.SensorAddress, self.ATimeAddress, IT & 0xFF)
        self.i2c.write_byte_data(self.SensorAddress, self.ControlAddress, Gain & 0xFF)
        self.i2c.write_byte_data(self.SensorAddress, self.EnableAddress, 0x03 & 0xFF)
        time.sleep(0.700)

        self.IntegrationTime = IT
        self.Gain = Gain

    def disableDevice(self):
        self.i2c.write_byte(0x70, 1 << self.number)
        reg = self.i2c.read_byte_data(self.SensorAddress, self.EnableAddress) & 0xFF

        self.i2c.write_byte_data(self.SensorAddress, self.EnableAddress,
                                 (reg & ~(self.ENABLE_PON | self.ENABLE_AEN)) & 0xFF)

    def setInterrupt(self, state):
        self.i2c.write_byte(0x70, 1 << self.number)
        reg = self.i2c.read_byte_data(self.SensorAddress, self.EnableAddress) & 0xFF
        if state:
            reg |= self.ENABLE_AIEN
        else:
            reg &= ~self.ENABLE_AIEN

        self.i2c.write_byte_data(self.SensorAddress, self.EnableAddress, reg & 0xFF)

    def clearInterrupt(self):
        self.i2c.write_byte(0x70, 1 << self.number)
        self.i2c.write_byte_data(self.SensorAddress, 0x66, 0x00 & 0xFF)

    def setInterruptLimits(self, low, high):
        self.i2c.write_byte(0x70, 1 << self.number)
        self.i2c.write_byte_data(self.SensorAddress, 0x04, low & 0xFF)
        self.i2c.write_byte_data(self.SensorAddress, 0x05, (low >> 8) & 0xFF)
        self.i2c.write_byte_data(self.SensorAddress, 0x06, high & 0xFF)
        self.i2c.write_byte_data(self.SensorAddress, 0x07, (low >> 8) & 0xFF)

    def isWhite(self):
        self.getColors()
        if self.red_color > 150 and self.green_color > 150 and self.blue_color > 150:
            return True
        return False

    def isSilver(self):
        self.getColors()
        if self.red_color > 150 and self.green_color > 150 and self.blue_color > 150:
            return True
        return False

    def isBlack(self):
        self.getColors()
        if self.red_color < 50 and self.green_color < 50 and self.blue_color < 50:
            return True
        return False
