from ctypes import *
import smbus

VL53L0X_GOOD_ACCURACY_MODE = 0      # Good Accuracy mode
VL53L0X_BETTER_ACCURACY_MODE = 1    # Better Accuracy mode
VL53L0X_BEST_ACCURACY_MODE = 2      # Best Accuracy mode
VL53L0X_LONG_RANGE_MODE = 3         # Longe Range mode
VL53L0X_HIGH_SPEED_MODE = 4         # High Speed mode

i2c = smbus.SMBus(1)


def i2c_read(address, reg, data_p, length):
    ret_val = 0
    result = []
 
    try:
        result = i2c.read_i2c_block_data(address, reg, length)
    except IOError:
        ret_val = -1

    if ret_val == 0:
        for index in range(length):
            data_p[index] = result[index]

    return ret_val


# i2c bus write callback
def i2c_write(address, reg, data_p, length):
    ret_val = 0
    data = []

    for index in range(length):
        data.append(data_p[index])
    try:
        i2c.write_i2c_block_data(address, reg, data)
    except IOError:
        ret_val = -1

    return ret_val


tof_lib = CDLL("sensors/vl53l0x_python.so")


READFUNC = CFUNCTYPE(c_int, c_ubyte, c_ubyte, POINTER(c_ubyte), c_ubyte)
read_func = READFUNC(i2c_read)


WRITEFUNC = CFUNCTYPE(c_int, c_ubyte, c_ubyte, POINTER(c_ubyte), c_ubyte)
write_func = WRITEFUNC(i2c_write)


tof_lib.VL53L0X_set_i2c(read_func, write_func)


class VL53L0X(object):
    """VL53L0X ToF"""

    object_number = 0

    def __init__(self, address=0x29, TCA9548A_Num=255, TCA9548A_Addr=0, **kwargs):
        """Initialize the VL53L0X ToF Sensor from ST"""
        self.device_address = address
        self.TCA9548A_Device = TCA9548A_Num
        self.TCA9548A_Address = TCA9548A_Addr
        self.my_object_number = VL53L0X.object_number
        VL53L0X.object_number += 1

    def start_ranging(self, mode = VL53L0X_GOOD_ACCURACY_MODE):
        """Start VL53L0X ToF Sensor Ranging"""
        tof_lib.startRanging(self.my_object_number, mode, self.device_address, self.TCA9548A_Device, self.TCA9548A_Address)
        
    def stop_ranging(self):
        """Stop VL53L0X ToF Sensor Ranging"""
        tof_lib.stopRanging(self.my_object_number)

    def get_distance(self):
        """Get distance from VL53L0X ToF Sensor"""
        return tof_lib.getDistance(self.my_object_number)
