"""
Victim Class
"""

from enum import Enum
import time


class Victim:
    VictimType = Enum('VictimType', 'Heated Harmed Stable Unharmed')
    Type = VictimType.Heated

    Present = False
    Saved = False

    def __init__(self, led, servo):
        self.Led = led
        self.Servo = servo

    def save(self):
        if self.Present:
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
            return True
        return False
