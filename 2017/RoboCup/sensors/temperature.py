"""
MLX90614 driver.
You might need to enter this command on your Raspberry Pi:
echo "Y" > /sys/module/i2c_bcm2708/parameters/combined
(I've put it in my rc.local so it's executed each bootup)
"""

import smbus


class Temperature:
    MLX90614_RAWIR1 = 0x04
    MLX90614_RAWIR2 = 0x05
    MLX90614_TA = 0x06
    MLX90614_TOBJ1 = 0x07
    MLX90614_TOBJ2 = 0x08

    MLX90614_TOMAX = 0x20
    MLX90614_TOMIN = 0x21
    MLX90614_PWMCTRL = 0x22
    MLX90614_TARANGE = 0x23
    MLX90614_EMISS = 0x24
    MLX90614_CONFIG = 0x25
    MLX90614_ADDR = 0x0E
    MLX90614_ID1 = 0x3C
    MLX90614_ID2 = 0x3D
    MLX90614_ID3 = 0x3E
    MLX90614_ID4 = 0x3F

    def __init__(self, address=0x5a):
        self.address = address
        self.i2c = smbus.SMBus(1)

    def getAmbTemp(self):
        data = self.i2c.read_word_data(self.address, self.MLX90614_TA)
        temp = (data * 0.02) - 273.15
        return temp

    def getObjTemp(self):
        data = self.i2c.read_word_data(self.address, self.MLX90614_TOBJ1)
        temp = (data * 0.02) - 273.15
        return temp
