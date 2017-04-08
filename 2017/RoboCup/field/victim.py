"""
Victim Class
"""

from enum import Enum
import time


class Victim:
    VictimType = Enum('VictimType', 'Heated Harmed Stable Unharmed')

    def __init__(self, led, servo):
        self.Led = led
        self.Servo = servo

        self.Type = self.VictimType.Heated

        self.Present = False
        self.Saved = False

    def save(self):
        if self.Present:
            print("Save Victim")
            self.Led.on()
            time.sleep(1)
            self.Led.off()
            time.sleep(1)
            self.Led.on()
            time.sleep(1)
            self.Led.off()
            time.sleep(1)
            self.Led.on()
            time.sleep(1)
            self.Led.off()
            self.Saved = True
            print("Victim Saved")
            return True
        return False
